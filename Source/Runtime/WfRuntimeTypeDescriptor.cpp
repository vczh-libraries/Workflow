#include "WfRuntime.h"

namespace vl
{
	namespace workflow
	{
		namespace typeimpl
		{
			using namespace collections;
			using namespace reflection::description;
			using namespace runtime;

/***********************************************************************
WfMethodBase
***********************************************************************/

			void WfMethodBase::SetGlobalContext(runtime::WfRuntimeGlobalContext* _globalContext)
			{
				globalContext = _globalContext;
			}


			WfMethodBase::WfMethodBase()
				:MethodInfoImpl(nullptr, nullptr, true)
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

			Value WfStaticMethod::CreateFunctionProxyInternal(const Value& thisObject)
			{
				auto lambda = MakePtr<WfRuntimeLambda>(globalContext, nullptr, functionIndex);
				return Value::From(lambda);
			}

/***********************************************************************
WfInterfaceMethod
***********************************************************************/

			Value WfInterfaceMethod::InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)
			{
				throw 0;
			}

			Value WfInterfaceMethod::CreateFunctionProxyInternal(const Value& thisObject)
			{
				throw 0;
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
				throw 0;
			}

			WfEvent::WfEvent(ITypeDescriptor* ownerTypeDescriptor, const WString& name)
				:EventInfoImpl(ownerTypeDescriptor, name)
			{
			}

			WfEvent::~WfEvent()
			{
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

			WfCustomType::WfCustomType(const WString& typeName)
				:TypeDescriptorImpl(typeName, L"")
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

			void WfCustomType::AddMember(const WString& name, Ptr<WfProperty> value)
			{
				AddProperty(value);
			}

			void WfCustomType::AddMember(const WString& name, Ptr<WfEvent> value)
			{
				AddEvent(value);
			}

/***********************************************************************
WfClass
***********************************************************************/

			WfClass::WfClass(const WString& typeName)
				:WfCustomType(typeName)
			{
			}

			WfClass::~WfClass()
			{
			}

/***********************************************************************
WfInterface
***********************************************************************/

			WfInterface::WfInterface(const WString& typeName)
				:WfCustomType(typeName)
			{
			}

			WfInterface::~WfInterface()
			{
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
			}

			void WfTypeImpl::Unload(reflection::description::ITypeManager* manager)
			{
				FOREACH(Ptr<WfClass>, td, classes)
				{
					manager->SetTypeDescriptor(td->GetTypeName(), nullptr);
				}
			}
		}
	}
}