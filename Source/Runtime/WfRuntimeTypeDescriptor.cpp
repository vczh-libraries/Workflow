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
					List<ITypeDescriptor*> baseTypes;
					baseTypes.Add(GetOwnerTypeDescriptor());

					for (vint i = 0; i < baseTypes.Count(); i++)
					{
						auto td = baseTypes[i];
						if (dynamic_cast<WfCustomType*>(td))
						{
							vint count = td->GetBaseTypeDescriptorCount();
							for (vint j = 0; j < count; j++)
							{
								baseTypes.Add(td->GetBaseTypeDescriptor(j));
							}
						}
						else if (auto group = td->GetConstructorGroup())
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

				auto instance = new WfInterfaceInstance(GetOwnerTypeDescriptor(), proxy, baseCtors);

				if (returnInfo->GetDecorator() == ITypeInfo::SharedPtr)
				{
					return Value::From(Ptr<WfInterfaceInstance>(instance));
				}
				else
				{
					return Value::From(instance);
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
WfEvent
***********************************************************************/

			void WfEvent::AttachInternal(DescriptableObject* thisObject, IEventHandler* eventHandler)
			{
				throw 0;
			}

			void WfEvent::DetachInternal(DescriptableObject* thisObject, IEventHandler* eventHandler)
			{
				throw 0;
			}

			void WfEvent::InvokeInternal(DescriptableObject* thisObject, collections::Array<Value>& arguments)
			{
				throw 0;
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