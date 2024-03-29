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
				
			Value WfMethodProxy::Invoke(Ptr<IValueReadonlyList> arguments)
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				return methodInfo->Invoke(thisObject, UnboxParameter<Array<Value>>(Value::From(arguments)).Ref());
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
			}

/***********************************************************************
WfMethodBase
***********************************************************************/

			Value WfMethodBase::CreateFunctionProxyInternal(const Value& thisObject)
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				return Value::From(Ptr(new WfMethodProxy(thisObject, this)));
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
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

			IMethodInfo::ICpp* WfMethodBase::GetCpp()
			{
				return nullptr;
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
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				auto argumentArray = IValueList::Create(arguments);
				return WfRuntimeLambda::Invoke(Ptr(globalContext), nullptr, functionIndex, argumentArray);
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
			}

			WfStaticMethod::WfStaticMethod()
				:WfMethodBase(true)
			{
			}

/***********************************************************************
WfClassConstructor
***********************************************************************/

			Value WfClassConstructor::InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				auto instance = Ptr(new WfClassInstance(GetOwnerTypeDescriptor()));
				{
					InvokeBaseCtor(Value::From(instance.Obj()), arguments);
				}

				if (returnInfo->GetDecorator() == ITypeInfo::SharedPtr)
				{
					return Value::From(instance);
				}
				else
				{
					return Value::From(instance.Detach());
				}
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
			}

			WfClassConstructor::WfClassConstructor(Ptr<ITypeInfo> type)
				:WfMethodBase(true)
			{
				SetReturn(type);
			}

			void WfClassConstructor::InvokeBaseCtor(const Value& thisObject, collections::Array<Value>& arguments)
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				auto capturedVariables = Ptr(new WfRuntimeVariableContext);
				capturedVariables->variables.Resize(1);
				capturedVariables->variables[0] = Value::From(thisObject.GetRawPtr());
					
				auto argumentArray = IValueList::Create(arguments);
				WfRuntimeLambda::Invoke(Ptr(globalContext), capturedVariables, functionIndex, argumentArray);
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
			}

/***********************************************************************
WfClassMethod
***********************************************************************/

			Value WfClassMethod::InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				auto capturedVariables = Ptr(new WfRuntimeVariableContext);
				capturedVariables->variables.Resize(1);
				capturedVariables->variables[0] = Value::From(thisObject.GetRawPtr());

				auto argumentArray = IValueList::Create(arguments);
				return WfRuntimeLambda::Invoke(Ptr(globalContext), capturedVariables, functionIndex, argumentArray);
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
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
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
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
						if (td != description::GetTypeDescriptor<IDescriptable>())
						{
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
				}

				auto instance = Ptr(new WfInterfaceInstance(GetOwnerTypeDescriptor(), proxy, baseCtors));

				if (returnInfo->GetDecorator() == ITypeInfo::SharedPtr)
				{
					return Value::From(instance);
				}
				else
				{
					return Value::From(instance.Detach());
				}
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
			}

			WfInterfaceConstructor::WfInterfaceConstructor(Ptr<ITypeInfo> type)
				:WfMethodBase(true)
			{
				auto argumentType = TypeInfoRetriver<Ptr<IValueInterfaceProxy>>::CreateTypeInfo();
				auto parameter = Ptr(new ParameterInfoImpl(this, L"proxy", argumentType));
				AddParameter(parameter);
				SetReturn(type);
			}

/***********************************************************************
WfInterfaceMethod
***********************************************************************/

			Value WfInterfaceMethod::InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				auto instance = thisObject.GetRawPtr()->SafeAggregationCast<WfInterfaceInstance>();
				return instance->GetProxy()->Invoke(this, IValueList::Create(From(arguments)));
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
			}

			WfInterfaceMethod::WfInterfaceMethod()
				:WfMethodBase(false)
			{
			}

/***********************************************************************
GetInfoRecord
***********************************************************************/

#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
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
					typedValue = Ptr(new TRecord);
					thisObject->SetInternalProperty(key, typedValue);
				}
				return typedValue;
			}
#endif

/***********************************************************************
WfEvent
***********************************************************************/

			const wchar_t* WfEvent::EventRecordInternalPropertyName = L"WfEvent::EventRecord";

			Ptr<WfEvent::EventRecord> WfEvent::GetEventRecord(DescriptableObject* thisObject, bool createIfNotExist)
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				return GetInfoRecord<EventRecord>(this, thisObject, EventRecordInternalPropertyName, createIfNotExist);
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
			}

			Ptr<IEventHandler> WfEvent::AttachInternal(DescriptableObject* thisObject, Ptr<IValueFunctionProxy> handler)
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				auto record = GetEventRecord(thisObject, true);
				auto result = Ptr(new EventHandlerImpl(handler));
				record->handlers.Add(this, result);
				return result;
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
			}

			bool WfEvent::DetachInternal(DescriptableObject* thisObject, Ptr<IEventHandler> handler)
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				auto impl = handler.Cast<EventHandlerImpl>();
				if (!impl)return false;
				auto record = GetEventRecord(thisObject, true);
				if (record->handlers.Remove(this, impl.Obj()))
				{
					impl->isAttached = false;
					return true;
				}
				return false;
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
			}

			void WfEvent::InvokeInternal(DescriptableObject* thisObject, Ptr<IValueReadonlyList> arguments)
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				auto record = GetEventRecord(thisObject, false);
				if (record)
				{
					vint index = record->handlers.Keys().IndexOf(this);
					if (index != -1)
					{
						auto& values = record->handlers.GetByIndex(index);
						for (auto handler : values)
						{
							handler->proxy->Invoke(arguments);
						}
					}
				}
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
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

			IEventInfo::ICpp* WfEvent::GetCpp()
			{
				return nullptr;
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
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				return GetInfoRecord<FieldRecord>(this, thisObject, FieldRecordInternalPropertyName, createIfNotExist);
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
			}

			Value WfField::GetValueInternal(const Value& thisObject)
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				auto record = GetFieldRecord(thisObject.GetRawPtr(), true);
				return record->values.Get(this);
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
			}

			void WfField::SetValueInternal(Value& thisObject, const Value& newValue)
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				auto record = GetFieldRecord(thisObject.GetRawPtr(), true);
				record->values.Set(this, newValue);
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
			}

			WfField::WfField(ITypeDescriptor* ownerTypeDescriptor, const WString& name)
				:FieldInfoImpl(ownerTypeDescriptor, name, nullptr)
			{
			}

			WfField::~WfField()
			{
			}

			IPropertyInfo::ICpp* WfField::GetCpp()
			{
				return nullptr;
			}

			void WfField::SetReturn(Ptr<ITypeInfo> typeInfo)
			{
				returnInfo = typeInfo;
			}

/***********************************************************************
WfStructField
***********************************************************************/

			Value WfStructField::GetValueInternal(const Value& thisObject)
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				auto structValue = thisObject.GetBoxedValue().Cast<WfStructInstance>();
				if (!structValue)
				{
					throw ArgumentTypeMismtatchException(L"thisObject", GetOwnerTypeDescriptor(), Value::BoxedValue, thisObject);
				}
				vint index = structValue->fieldValues.Keys().IndexOf(this);
				if (index == -1)
				{
					return returnInfo->GetTypeDescriptor()->GetValueType()->CreateDefault();
				}
				else
				{
					return structValue->fieldValues.Values()[index];
				}
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
			}

			void WfStructField::SetValueInternal(Value& thisObject, const Value& newValue)
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				auto structValue = thisObject.GetBoxedValue().Cast<WfStructInstance>();
				if (!structValue)
				{
					throw ArgumentTypeMismtatchException(L"thisObject", GetOwnerTypeDescriptor(), Value::BoxedValue, thisObject);
				}
				structValue->fieldValues.Set(this, newValue);
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
			}

			WfStructField::WfStructField(ITypeDescriptor* ownerTypeDescriptor, const WString& name)
				:FieldInfoImpl(ownerTypeDescriptor, name, nullptr)
			{
			}

			WfStructField::~WfStructField()
			{
			}

			IPropertyInfo::ICpp* WfStructField::GetCpp()
			{
				return nullptr;
			}

			void WfStructField::SetReturn(Ptr<ITypeInfo> typeInfo)
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
WfTypeInfoContent
***********************************************************************/

			WfTypeInfoContent::WfTypeInfoContent(const WString& _workflowTypeName)
				:workflowTypeName(_workflowTypeName)
			{
				typeName = workflowTypeName.Buffer();
				cppFullTypeName = nullptr;
				cppName = TypeInfoContent::CppType;
			}

/***********************************************************************
WfCustomType
***********************************************************************/

			void WfCustomType::SetGlobalContext(runtime::WfRuntimeGlobalContext* _globalContext, IMethodGroupInfo* group)
			{
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

			void WfCustomType::SetGlobalContext(runtime::WfRuntimeGlobalContext* _globalContext)
			{
				globalContext = _globalContext;
				
				if (auto group = GetConstructorGroup())
				{
					SetGlobalContext(globalContext, group);
				}

				vint methodGroupCount = GetMethodGroupCount();
				for (vint i = 0; i < methodGroupCount; i++)
				{
					auto group = GetMethodGroup(i);
					SetGlobalContext(globalContext, group);
				}
			}

			void WfCustomType::LoadInternal()
			{
			}

			WfCustomType::WfCustomType(reflection::description::TypeDescriptorFlags typeDescriptorFlags, const WString& typeName)
				:WfCustomTypeBase<reflection::description::TypeDescriptorImpl>(typeDescriptorFlags, typeName)
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

			void WfCustomType::AddMember(Ptr<WfClassConstructor> value)
			{
				AddConstructor(value);
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
WfStruct
***********************************************************************/

			WfStruct::WfValueType::WfValueType(WfStruct* _owner)
				:owner(_owner)
			{
			}

			Value WfStruct::WfValueType::CreateDefault()
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				return Value::From(Ptr(new WfStructInstance(owner)), owner);
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
			}

			WfStruct::WfStruct(const WString& typeName)
				:WfCustomTypeBase<reflection::description::ValueTypeDescriptorBase>(TypeDescriptorFlags::Struct, typeName)
			{
				this->valueType = Ptr(new WfValueType(this));
			}

			WfStruct::~WfStruct()
			{
			}

			vint WfStruct::GetPropertyCount()
			{
				this->Load();
				return fields.Count();
			}

			IPropertyInfo* WfStruct::GetProperty(vint index)
			{
				this->Load();
				if (index < 0 || index >= fields.Count())
				{
					return nullptr;
				}
				return fields.Values()[index].Obj();
			}

			bool WfStruct::IsPropertyExists(const WString& name, bool inheritable)
			{
				this->Load();
				return fields.Keys().Contains(name);
			}

			IPropertyInfo* WfStruct::GetPropertyByName(const WString& name, bool inheritable)
			{
				this->Load();
				vint index = fields.Keys().IndexOf(name);
				if (index == -1) return nullptr;
				return fields.Values()[index].Obj();
			}

			void WfStruct::AddMember(Ptr<WfStructField> value)
			{
				fields.Add(value->GetName(), value);
			}

/***********************************************************************
WfEnum::WfEnumType
***********************************************************************/

			WfEnum::WfEnumType::WfEnumType(WfEnum* _owner)
				:owner(_owner)
			{
			}

			bool WfEnum::WfEnumType::IsFlagEnum()
			{
				return owner->GetTypeDescriptorFlags() == TypeDescriptorFlags::FlagEnum;
			}

			vint WfEnum::WfEnumType::GetItemCount()
			{
				return owner->enumItems.Count();
			}

			WString WfEnum::WfEnumType::GetItemName(vint index)
			{
				if (index < 0 || index >= owner->enumItems.Count())
				{
					return L"";
				}
				return owner->enumItems.Keys()[index];
			}

			vuint64_t WfEnum::WfEnumType::GetItemValue(vint index)
			{
				if (index < 0 || index >= owner->enumItems.Count())
				{
					return 0;
				}
				return owner->enumItems.Values()[index];
			}

			vint WfEnum::WfEnumType::IndexOfItem(WString name)
			{
				return owner->enumItems.Keys().IndexOf(name);
			}

			Value WfEnum::WfEnumType::ToEnum(vuint64_t value)
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				auto boxedValue = Ptr(new WfEnumInstance(owner));
				boxedValue->value = value;
				return Value::From(boxedValue, owner);
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
			}

			vuint64_t WfEnum::WfEnumType::FromEnum(const Value& value)
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				auto enumValue = value.GetBoxedValue().Cast<WfEnumInstance>();
				if (!enumValue)
				{
					throw ArgumentTypeMismtatchException(L"enumValue", owner, Value::BoxedValue, value);
				}
				return enumValue->value;
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
			}

/***********************************************************************
WfEnum
***********************************************************************/

			WfEnum::WfValueType::WfValueType(WfEnum* _owner)
				:owner(_owner)
			{
			}

			Value WfEnum::WfValueType::CreateDefault()
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				return Value::From(Ptr(new WfEnumInstance(owner)), owner);
#else
				CHECK_FAIL(L"Not Implemented under VCZH_DEBUG_METAONLY_REFLECTION!");
#endif
			}

			WfEnum::WfEnum(bool isFlags, const WString& typeName)
				:WfCustomTypeBase<reflection::description::ValueTypeDescriptorBase>((isFlags ? TypeDescriptorFlags::FlagEnum : TypeDescriptorFlags::NormalEnum), typeName)
			{
				this->valueType = Ptr(new WfValueType(this));
				this->enumType = Ptr(new WfEnumType(this));
			}

			WfEnum::~WfEnum()
			{
			}

			void WfEnum::AddEnumItem(const WString& name, vuint64_t value)
			{
				enumItems.Add(name, value);
			}

/***********************************************************************
WfClassInstance
***********************************************************************/

			WfClassInstance::WfClassInstance(ITypeDescriptor* _typeDescriptor)
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				:Description<WfClassInstance>(_typeDescriptor)
#endif
			{
				classType = dynamic_cast<WfClass*>(_typeDescriptor);
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				InitializeAggregation(classType->GetExpandedBaseTypes().Count());
#endif
			}

			WfClassInstance::~WfClassInstance()
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				if (classType->destructorFunctionIndex != -1)
				{
					auto capturedVariables = Ptr(new WfRuntimeVariableContext);
					capturedVariables->variables.Resize(1);
					capturedVariables->variables[0] = Value::From(this);

					auto argumentArray = IValueList::Create();
					WfRuntimeLambda::Invoke(Ptr(classType->GetGlobalContext()), capturedVariables, classType->destructorFunctionIndex, argumentArray);
				}
#endif
			}

			void WfClassInstance::InstallBaseObject(ITypeDescriptor* td, Value& value)
			{
				Ptr<DescriptableObject> ptr;
				{
					if (!(ptr = value.GetSharedPtr()))
					{
						ptr = Ptr(value.GetRawPtr());
					}
					value = Value();
				}
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				vint index = classType->GetExpandedBaseTypes().IndexOf(td);
				SetAggregationParent(index, ptr);
#endif
			}

/***********************************************************************
WfInterfaceInstance
***********************************************************************/

			WfInterfaceInstance::WfInterfaceInstance(ITypeDescriptor* _typeDescriptor, Ptr<IValueInterfaceProxy> _proxy, collections::List<IMethodInfo*>& baseCtors)
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				:Description<WfInterfaceInstance>(_typeDescriptor)
				, proxy(_proxy)
#endif
			{
				Array<Value> arguments(1);
				arguments[0] = Value::From(_proxy);
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				InitializeAggregation(baseCtors.Count());
				for (auto [ctor, index] : indexed(baseCtors))
				{
					Ptr<DescriptableObject> ptr;
					{
						auto value = ctor->Invoke(Value(), arguments);
						if (!(ptr = value.GetSharedPtr()))
						{
							ptr = Ptr(value.GetRawPtr());
						}
					}

					SetAggregationParent(index, ptr);
				}
#endif
			}

			WfInterfaceInstance::~WfInterfaceInstance()
			{
			}

			Ptr<IValueInterfaceProxy> WfInterfaceInstance::GetProxy()
			{
				return proxy;
			}

/***********************************************************************
WfStructInstance
***********************************************************************/

			WfStructInstance::WfStructInstance(reflection::description::ITypeDescriptor* _typeDescriptor)
				: typeDescriptor(_typeDescriptor)
			{
			}

			PredefinedBoxableType WfStructInstance::GetBoxableType()
			{
				return PredefinedBoxableType::PBT_Unknown;
			}

			Ptr<IBoxedValue> WfStructInstance::Copy()
			{
				auto instance = Ptr(new WfStructInstance(typeDescriptor));
				CopyFrom(instance->fieldValues, fieldValues);
				return instance;
			}

			IBoxedValue::CompareResult WfStructInstance::ComparePrimitive(Ptr<IBoxedValue> boxedValue)
			{
				auto instance = boxedValue.Cast<WfStructInstance>();
				if (!instance) return IBoxedValue::NotComparable;
				if (typeDescriptor != instance->typeDescriptor) return IBoxedValue::NotComparable;
				
				auto&& as = fieldValues;
				auto&& bs = instance->fieldValues;
				if (as.Count() == 0 && bs.Count() == 0) return IBoxedValue::Equal;

				vint ai = 0;
				vint bi = 0;
				while (ai < as.Count() || bi < bs.Count())
				{
					Value af, bf;
					auto ap = ai < as.Count() ? as.Keys()[ai] : nullptr;
					auto bp = bi < bs.Count() ? bs.Keys()[bi] : nullptr;
					auto p =
						ap == nullptr ? bp :
						bp == nullptr ? ap :
						ap < bp ? ap : bp;

					if (p == ap)
					{
						af = as.Values()[ai];
					}
					else if (p->GetReturn()->GetDecorator() == ITypeInfo::TypeDescriptor)
					{
						af = p->GetReturn()->GetTypeDescriptor()->GetValueType()->CreateDefault();
					}

					if (p == bp)
					{
						bf = bs.Values()[bi];
					}
					else if (p->GetReturn()->GetDecorator() == ITypeInfo::TypeDescriptor)
					{
						bf = p->GetReturn()->GetTypeDescriptor()->GetValueType()->CreateDefault();
					}

					auto result = af <=> bf;
					if constexpr (std::is_same_v<decltype(result), std::partial_ordering>)
					{
						if (result == std::partial_ordering::unordered) return IBoxedValue::NotComparable;
					}
					if (result < 0) return IBoxedValue::Smaller;
					if (result > 0) return IBoxedValue::Greater;

					if (p == ap) ai++;
					if (p == bp) bi++;
				}

				return IBoxedValue::Equal;
			}

/***********************************************************************
WfEnumInstance
***********************************************************************/

			WfEnumInstance::WfEnumInstance(reflection::description::ITypeDescriptor* _typeDescriptor)
				: typeDescriptor(_typeDescriptor)
			{
			}

			PredefinedBoxableType WfEnumInstance::GetBoxableType()
			{
				return PredefinedBoxableType::PBT_Unknown;
			}

			Ptr<IBoxedValue> WfEnumInstance::Copy()
			{
				auto instance = Ptr(new WfEnumInstance(typeDescriptor));
				instance->value = value;
				return instance;
			}

			IBoxedValue::CompareResult WfEnumInstance::ComparePrimitive(Ptr<IBoxedValue> boxedValue)
			{
				auto instance = boxedValue.Cast<WfEnumInstance>();
				if (!instance) return IBoxedValue::NotComparable;
				if (typeDescriptor != instance->typeDescriptor) return IBoxedValue::NotComparable;
				
				auto result = value <=> instance->value;
				if (result < 0) return IBoxedValue::Smaller;
				if (result > 0) return IBoxedValue::Greater;
				return IBoxedValue::Equal;
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
				for (auto td : classes)
				{
					td->SetGlobalContext(globalContext);
				}
				for (auto td : interfaces)
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
				for (auto td : classes)
				{
					if (td->GetBaseTypeDescriptorCount() == 0)
					{
						td->AddBaseType(description::GetTypeDescriptor<DescriptableObject>());
					}
					manager->SetTypeDescriptor(td->GetTypeName(), td);
				}
				for (auto td : interfaces)
				{
					if (td->GetBaseTypeDescriptorCount() == 0)
					{
						td->AddBaseType(description::GetTypeDescriptor<IDescriptable>());
					}
					manager->SetTypeDescriptor(td->GetTypeName(), td);
				}
				for (auto td : structs)
				{
					manager->SetTypeDescriptor(td->GetTypeName(), td);
				}
				for (auto td : enums)
				{
					manager->SetTypeDescriptor(td->GetTypeName(), td);
				}
			}

			void WfTypeImpl::Unload(reflection::description::ITypeManager* manager)
			{
				for (auto td : classes)
				{
					manager->SetTypeDescriptor(td->GetTypeName(), nullptr);
				}
				for (auto td : interfaces)
				{
					manager->SetTypeDescriptor(td->GetTypeName(), nullptr);
				}
				for (auto td : structs)
				{
					manager->SetTypeDescriptor(td->GetTypeName(), nullptr);
				}
				for (auto td : enums)
				{
					manager->SetTypeDescriptor(td->GetTypeName(), nullptr);
				}
			}
		}
	}
}