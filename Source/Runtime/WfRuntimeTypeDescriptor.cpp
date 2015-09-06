#include "WfRuntimeTypeDescriptor.h"

namespace vl
{
	namespace workflow
	{
		namespace typeimpl
		{
			using namespace collections;
			using namespace reflection::description;

/***********************************************************************
WfStaticMethod
***********************************************************************/

			Value WfStaticMethod::InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)
			{
				throw 0;
			}

			Value WfStaticMethod::CreateFunctionProxyInternal(const Value& thisObject)
			{
				throw 0;
			}

			WfStaticMethod::WfStaticMethod()
				:MethodInfoImpl(nullptr, nullptr, true)
				, functionIndex(-1)
			{
			}

			WfStaticMethod::~WfStaticMethod()
			{
			}

			void WfStaticMethod::SetReturn(Ptr<ITypeInfo> type)
			{
				returnInfo = type;
			}

/***********************************************************************
WfClass
***********************************************************************/

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