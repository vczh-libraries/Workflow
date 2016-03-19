#include "WfRuntime.h"

namespace vl
{
	namespace workflow
	{
		namespace typeimpl
		{
			using namespace reflection;
			using namespace reflection::description;
			using namespace collections;
			using namespace runtime;

/***********************************************************************
WfMethodProxy
***********************************************************************/

			WfMethodProxy::WfMethodProxy(const Value& _thisObject, IMethodInfo* _methodInfo)
				:thisObject(_thisObject)
				, methodInfo(_methodInfo)
			{

			}

			WfMethodProxy::~WfMethodProxy()
			{
			}
				
			Value WfMethodProxy::Invoke(Ptr<IValueList> arguments)
			{
				Array<Value> values;
				UnboxParameter(Value::From(arguments), values);
				return methodInfo->Invoke(thisObject, values);
			}

/***********************************************************************
WfMethodBase
***********************************************************************/

			Value WfMethodBase::CreateFunctionProxyInternal(const Value& thisObject)
			{
				return Value::From(MakePtr<WfMethodProxy>(thisObject, this));
			}

			void WfMethodBase::SetGlobalContext(runtime::WfRuntimeGlobalContext* _globalContext)
			{
				globalContext = _globalContext;
			}

			WfMethodBase::WfMethodBase(bool isStatic)
				:MethodInfoImpl(nullptr, nullptr, isStatic)
			{
			}

			WfMethodBase::~WfMethodBase()
			{
			}
			
			runtime::WfRuntimeGlobalContext* WfMethodBase::GetGlobalContext()
			{
				return globalContext;
			}

			void WfMethodBase::SetReturn(Ptr<ITypeInfo> type)
			{
				returnInfo = type;
			}

/***********************************************************************
WfStaticMethod
***********************************************************************/

			Value WfStaticMethod::InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)
			{
				auto lambda = MakePtr<WfRuntimeLambda>(globalContext, nullptr, functionIndex);
				auto argumentArray = IValueList::Create(arguments);
				return lambda->Invoke(argumentArray);
			}

			WfStaticMethod::WfStaticMethod()
				:WfMethodBase(true)
			{
			}

/***********************************************************************
WfClassMethod
***********************************************************************/

			Value WfClassMethod::InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)
			{
				auto context = MakePtr<WfRuntimeVariableContext>();
				context->variables.Resize(1);
				context->variables[0] = thisObject;

				auto lambda = MakePtr<WfRuntimeLambda>(globalContext, nullptr, functionIndex);
				auto argumentArray = IValueList::Create(arguments);
				return lambda->Invoke(argumentArray);
			}

			WfClassMethod::WfClassMethod()
				:WfMethodBase(false)
			{
			}

/***********************************************************************
WfInterfaceConstructor
***********************************************************************/

			Value WfInterfaceConstructor::InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)
			{
				if (arguments.Count() != 1)
				{
					throw ArgumentCountMismtatchException(GetOwnerMethodGroup());
				}
				auto proxy = UnboxValue<Ptr<IValueInterfaceProxy>>(arguments[0]);

				List<IMethodInfo*> baseCtors;
				{
					const auto& baseTypes = dynamic_cast<WfCustomType*>(GetOwnerTypeDescriptor())->GetExpandedBaseTypes();

					for (vint i = 0; i < baseTypes.Count(); i++)
					{
						auto td = baseTypes[i];
						if (auto group = td->GetConstructorGroup())
						{
							vint count = group->GetMethodCount();
							IMethodInfo* selectedCtor = nullptr;

							for (vint j = 0; j < count; j++)
							{
								auto ctor = group->GetMethod(j);
								if (ctor->GetParameterCount() == 1)
								{
									auto type = ctor->GetParameter(0)->GetType();
									if (type->GetDecorator() == ITypeInfo::SharedPtr && type->GetTypeDescriptor() == description::GetTypeDescriptor<IValueInterfaceProxy>())
									{
										selectedCtor = ctor;
										break;
									}
								}
							}

							if (selectedCtor)
							{
								baseCtors.Add(selectedCtor);
							}
							else
							{
								throw ArgumentCountMismtatchException(group);
							}
						}
						else
						{
							throw ConstructorNotExistsException(td);
						}
					}
				}

				Ptr<WfInterfaceInstance> instance = new WfInterfaceInstance(GetOwnerTypeDescriptor(), proxy, baseCtors);

				if (returnInfo->GetDecorator() == ITypeInfo::SharedPtr)
				{
					return Value::From(instance);
				}
				else
				{
					return Value::From(instance.Detach());
				}
			}

			WfInterfaceConstructor::WfInterfaceConstructor(Ptr<ITypeInfo> type)
				:WfMethodBase(true)
			{
				auto argumentType = TypeInfoRetriver<Ptr<IValueInterfaceProxy>>::CreateTypeInfo();
				auto parameter = MakePtr<ParameterInfoImpl>(this, L"proxy", argumentType);
				AddParameter(parameter);
				SetReturn(type);
			}

/***********************************************************************
WfInterfaceMethod
***********************************************************************/

			Value WfInterfaceMethod::InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)
			{
				auto instance = thisObject.GetRawPtr()->SafeAggregationCast<WfInterfaceInstance>();
				return instance->GetProxy()->Invoke(this, IValueList::Create(From(arguments)));
			}

			WfInterfaceMethod::WfInterfaceMethod()
				:WfMethodBase(false)
			{
			}

/***********************************************************************
GetInfoRecord
***********************************************************************/

			template<typename TRecord, typename TInfo>
			Ptr<TRecord> GetInfoRecord(TInfo* target, DescriptableObject* thisObject, const WString& key, bool createIfNotExist)
			{
				if (!thisObject)
				{
					throw ArgumentNullException(L"thisObject", target);
				}
				auto untypedValue = thisObject->GetInternalProperty(key);
				auto typedValue = untypedValue.Cast<TRecord>();
				if (untypedValue)
				{
					if (!typedValue)
					{
						throw ArgumentException(L"Key mismatches with the record type.", L"vl::workflow::typeimpl::GetFieldRecord", L"key");
					}
				}
				else if(createIfNotExist)
				{
					typedValue = new TRecord;
					thisObject->SetInternalProperty(key, typedValue);
				}
				return typedValue;
			}

/***********************************************************************
WfEvent
***********************************************************************/

			const wchar_t* WfEvent::EventRecordInternalPropertyName = L"WfEvent::EventRecord";

			Ptr<WfEvent::EventRecord> WfEvent::GetEventRecord(DescriptableObject* thisObject, bool createIfNotExist)
			{
				return GetInfoRecord<EventRecord>(this, thisObject, EventRecordInternalPropertyName, createIfNotExist);
			}

			void WfEvent::AttachInternal(DescriptableObject* thisObject, IEventHandler* eventHandler)
			{
				auto record = GetEventRecord(thisObject, true);
				record->handlers.Add(this, eventHandler);
			}

			void WfEvent::DetachInternal(DescriptableObject* thisObject, IEventHandler* eventHandler)
			{
				auto record = GetEventRecord(thisObject, true);
				record->handlers.Remove(this, eventHandler);
			}

			void WfEvent::InvokeInternal(DescriptableObject* thisObject, collections::Array<Value>& arguments)
			{
				auto record = GetEventRecord(thisObject, false);
				if (record)
				{
					vint index = record->handlers.Keys().IndexOf(this);
					if (index != -1)
					{
						auto& values = record->handlers.GetByIndex(index);
						FOREACH(IEventHandler*, handler, values)
						{
							handler->Invoke(Value::From(thisObject), arguments);
						}
					}
				}
			}

			Ptr<ITypeInfo> WfEvent::GetHandlerTypeInternal()
			{
				return handlerType;
			}

			WfEvent::WfEvent(ITypeDescriptor* ownerTypeDescriptor, const WString& name)
				:EventInfoImpl(ownerTypeDescriptor, name)
			{
			}

			WfEvent::~WfEvent()
			{
			}

			void WfEvent::SetHandlerType(Ptr<ITypeInfo> typeInfo)
			{
				handlerType = typeInfo;
			}

/***********************************************************************
WfField
***********************************************************************/

			const wchar_t* WfField::FieldRecordInternalPropertyName = L"WfField::FieldRecord";

			Ptr<WfField::FieldRecord> WfField::GetFieldRecord(DescriptableObject* thisObject, bool createIfNotExist)
			{
				return GetInfoRecord<FieldRecord>(this, thisObject, FieldRecordInternalPropertyName, createIfNotExist);
			}

			Value WfField::GetValueInternal(const Value& thisObject)
			{
				auto record = GetFieldRecord(thisObject.GetRawPtr(), true);
				return record->values.Get(this);
			}

			void WfField::SetValueInternal(Value& thisObject, const Value& newValue)
			{
				auto record = GetFieldRecord(thisObject.GetRawPtr(), true);
				record->values.Set(this, newValue);
			}

			WfField::WfField(ITypeDescriptor* ownerTypeDescriptor, const WString& name)
				:FieldInfoImpl(ownerTypeDescriptor, name, nullptr)
			{
			}

			WfField::~WfField()
			{
			}

			void WfField::SetReturn(Ptr<ITypeInfo> typeInfo)
			{
				returnInfo = typeInfo;
			}

/***********************************************************************
WfProperty
***********************************************************************/

			WfProperty::WfProperty(ITypeDescriptor* ownerTypeDescriptor, const WString& name)
				:PropertyInfoImpl(ownerTypeDescriptor, name, nullptr, nullptr, nullptr)
			{
			}

			WfProperty::~WfProperty()
			{
			}

			void WfProperty::SetGetter(MethodInfoImpl* value)
			{
				getter = value;
			}

			void WfProperty::SetSetter(MethodInfoImpl* value)
			{
				setter = value;
			}

			void WfProperty::SetValueChangedEvent(EventInfoImpl* value)
			{
				valueChangedEvent = value;
			}

/***********************************************************************
WfCustomType
***********************************************************************/

			void WfCustomType::SetGlobalContext(runtime::WfRuntimeGlobalContext* _globalContext)
			{
				globalContext = _globalContext;

				vint methodGroupCount = GetMethodGroupCount();
				for (vint i = 0; i < methodGroupCount; i++)
				{
					auto group = GetMethodGroup(i);
					vint methodCount = group->GetMethodCount();
					for (vint j = 0; j < methodCount; j++)
					{
						auto method = group->GetMethod(j);
						if (auto methodInfo = dynamic_cast<WfMethodBase*>(method))
						{
							methodInfo->SetGlobalContext(globalContext);
						}
					}
				}
			}

			void WfCustomType::LoadInternal()
			{
			}

			WfCustomType::WfCustomType(TypeDescriptorFlags typeDescriptorFlags, const WString& typeName)
				:TypeDescriptorImpl(typeDescriptorFlags, typeName, L"")
			{
			}

			WfCustomType::~WfCustomType()
			{
			}
			
			runtime::WfRuntimeGlobalContext* WfCustomType::GetGlobalContext()
			{
				return globalContext;
			}

			const WfCustomType::TypeDescriptorList& WfCustomType::GetExpandedBaseTypes()
			{
				if (!baseTypeExpanded)
				{
					baseTypeExpanded = true;
					TypeDescriptorList customTypes;
					customTypes.Add(this);

					for (vint i = 0; i < customTypes.Count(); i++)
					{
						auto td = customTypes[i];
						vint count = td->GetBaseTypeDescriptorCount();
						for (vint j = 0; j < count; j++)
						{
							auto baseTd = td->GetBaseTypeDescriptor(j);
							if (dynamic_cast<WfCustomType*>(baseTd))
							{
								customTypes.Add(baseTd);
							}
							else
							{
								expandedBaseTypes.Add(baseTd);
							}
						}
					}
				}
				return expandedBaseTypes;
			}

			void WfCustomType::AddBaseType(ITypeDescriptor* type)
			{
				TypeDescriptorImpl::AddBaseType(type);
			}

			void WfCustomType::AddMember(const WString& name, Ptr<WfMethodBase> value)
			{
				AddMethod(name, value);
			}

			void WfCustomType::AddMember(Ptr<WfInterfaceConstructor> value)
			{
				AddConstructor(value);
			}

			void WfCustomType::AddMember(Ptr<WfField> value)
			{
				AddProperty(value);
			}

			void WfCustomType::AddMember(Ptr<WfProperty> value)
			{
				AddProperty(value);
			}

			void WfCustomType::AddMember(Ptr<WfEvent> value)
			{
				AddEvent(value);
			}

/***********************************************************************
WfClass
***********************************************************************/

			WfClass::WfClass(const WString& typeName)
				:WfCustomType(TypeDescriptorFlags::Class, typeName)
			{
			}

			WfClass::~WfClass()
			{
			}

/***********************************************************************
WfInterface
***********************************************************************/

			WfInterface::WfInterface(const WString& typeName)
				:WfCustomType(TypeDescriptorFlags::Interface, typeName)
			{
			}

			WfInterface::~WfInterface()
			{
			}

/***********************************************************************
WfTypeImpl
***********************************************************************/

			WfInterfaceInstance::WfInterfaceInstance(ITypeDescriptor* _typeDescriptor, Ptr<IValueInterfaceProxy> _proxy, collections::List<IMethodInfo*>& baseCtors)
				:Description<WfInterfaceInstance>(_typeDescriptor)
				, proxy(_proxy)
			{
				Array<Value> arguments(1);
				arguments[0] = Value::From(_proxy);

				InitializeAggregation(baseCtors.Count());
				FOREACH_INDEXER(IMethodInfo*, ctor, index, baseCtors)
				{
					Ptr<DescriptableObject> ptr;
					{
						auto value = ctor->Invoke(Value(), arguments);
						if (!(ptr = value.GetSharedPtr()))
						{
							ptr = value.GetRawPtr();
						}
					}

					SetAggregationParent(index, ptr);
				}
			}

			WfInterfaceInstance::~WfInterfaceInstance()
			{
			}

			Ptr<IValueInterfaceProxy> WfInterfaceInstance::GetProxy()
			{
				return proxy;
			}

/***********************************************************************
WfTypeImpl
***********************************************************************/
			
			runtime::WfRuntimeGlobalContext* WfTypeImpl::GetGlobalContext()
			{
				return globalContext;
			}

			void WfTypeImpl::SetGlobalContext(runtime::WfRuntimeGlobalContext* _globalContext)
			{
				if (globalContext)
				{
					CHECK_ERROR(!_globalContext, L"vl::workflow::typeimpl::WfTypeImpl::SetGlobalContext(WfRuntimeGlobalContext*)#Only one global context is allowed to create from an assembly at the same time.");
				}
				else
				{
					CHECK_ERROR(_globalContext, L"vl::workflow::typeimpl::WfTypeImpl::SetGlobalContext(WfRuntimeGlobalContext*)#Only one global context is allowed to create from an assembly at the same time.");
				}

				globalContext = _globalContext;
				FOREACH(Ptr<WfClass>, td, classes)
				{
					td->SetGlobalContext(globalContext);
				}
				FOREACH(Ptr<WfInterface>, td, interfaces)
				{
					td->SetGlobalContext(globalContext);
				}

				if (globalContext)
				{
					GetGlobalTypeManager()->AddTypeLoader(Ptr<WfTypeImpl>(this));
				}
				else
				{
					GetGlobalTypeManager()->RemoveTypeLoader(Ptr<WfTypeImpl>(this));
				}
			}

			void WfTypeImpl::Load(reflection::description::ITypeManager* manager)
			{
				FOREACH(Ptr<WfClass>, td, classes)
				{
					manager->SetTypeDescriptor(td->GetTypeName(), td);
				}
				FOREACH(Ptr<WfInterface>, td, interfaces)
				{
					manager->SetTypeDescriptor(td->GetTypeName(), td);
				}
			}

			void WfTypeImpl::Unload(reflection::description::ITypeManager* manager)
			{
				FOREACH(Ptr<WfClass>, td, classes)
				{
					manager->SetTypeDescriptor(td->GetTypeName(), nullptr);
				}
				FOREACH(Ptr<WfInterface>, td, interfaces)
				{
					manager->SetTypeDescriptor(td->GetTypeName(), nullptr);
				}
			}
		}
	}
}