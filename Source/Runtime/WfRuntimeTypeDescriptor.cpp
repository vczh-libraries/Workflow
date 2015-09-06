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
WfStaticMethod
***********************************************************************/

			void WfStaticMethod::SetGlobalContext(runtime::WfRuntimeGlobalContext* _globalContext)
			{
				globalContext = _globalContext;
			}

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

			WfStaticMethod::WfStaticMethod()
				:MethodInfoImpl(nullptr, nullptr, true)
				, functionIndex(-1)
			{
			}

			WfStaticMethod::~WfStaticMethod()
			{
			}
			
			runtime::WfRuntimeGlobalContext* WfStaticMethod::GetGlobalContext()
			{
				return globalContext;
			}

			void WfStaticMethod::SetReturn(Ptr<ITypeInfo> type)
			{
				returnInfo = type;
			}

/***********************************************************************
WfClass
***********************************************************************/

			void WfClass::SetGlobalContext(runtime::WfRuntimeGlobalContext* _globalContext)
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
						if (auto staticMethod = dynamic_cast<WfStaticMethod*>(method))
						{
							staticMethod->SetGlobalContext(globalContext);
						}
					}
				}
			}

			void WfClass::LoadInternal()
			{
			}

			WfClass::WfClass(const WString& typeName)
				:TypeDescriptorImpl(typeName, L"")
			{
			}

			WfClass::~WfClass()
			{
			}
			
			runtime::WfRuntimeGlobalContext* WfClass::GetGlobalContext()
			{
				return globalContext;
			}

			void WfClass::AddBaseType(ITypeDescriptor* type)
			{
				TypeDescriptorImpl::AddBaseType(type);
			}

			void WfClass::AddMember(const WString& name, Ptr<WfStaticMethod> value)
			{
				AddMethod(name, value);
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