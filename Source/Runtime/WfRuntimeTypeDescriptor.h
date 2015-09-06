/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::Runtime

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_RUNTIME_WFRUNTIMETYPEDESCRIPTOR
#define VCZH_WORKFLOW_RUNTIME_WFRUNTIMETYPEDESCRIPTOR

#include "../WorkflowVlppReferences.h"

namespace vl
{
	namespace workflow
	{
		namespace typeimpl
		{
			class WfStaticMethod : public reflection::description::MethodInfoImpl
			{
				typedef reflection::description::ITypeInfo					ITypeInfo;
				typedef reflection::description::Value						Value;
			protected:

				Value									InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)override;
				Value									CreateFunctionProxyInternal(const Value& thisObject)override;

			public:
				vint									functionIndex;

				WfStaticMethod();
				~WfStaticMethod();

				void									SetReturn(Ptr<ITypeInfo> type);
			};

			class WfClass : public reflection::description::TypeDescriptorImpl
			{
				typedef reflection::description::ITypeDescriptor			ITypeDescriptor;
				typedef reflection::description::ITypeInfo					ITypeInfo;
			protected:

				void									LoadInternal()override;

			public:
				WfClass(const WString& typeName);
				~WfClass();

				void									AddBaseType(ITypeDescriptor* type);
				void									AddMember(const WString& name, Ptr<WfStaticMethod> value);
			};

			class WfTypeImpl : public Object
			{
			public:
				collections::List<Ptr<WfClass>>			classes;
			};
		}
	}
}

#endif