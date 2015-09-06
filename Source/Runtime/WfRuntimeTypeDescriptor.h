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
		namespace runtime
		{
			class WfRuntimeGlobalContext;
		}

		namespace typeimpl
		{
			class WfClass;
			class WfTypeImpl;

			class WfStaticMethod : public reflection::description::MethodInfoImpl
			{
				friend class WfClass;
				typedef reflection::description::ITypeInfo					ITypeInfo;
				typedef reflection::description::Value						Value;
			protected:
				runtime::WfRuntimeGlobalContext*		globalContext = nullptr;
				
				void									SetGlobalContext(runtime::WfRuntimeGlobalContext* _globalContext);
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
				friend class WfTypeImpl;
				typedef reflection::description::ITypeDescriptor			ITypeDescriptor;
				typedef reflection::description::ITypeInfo					ITypeInfo;
			protected:
				runtime::WfRuntimeGlobalContext*		globalContext = nullptr;
				
				void									SetGlobalContext(runtime::WfRuntimeGlobalContext* _globalContext);
				void									LoadInternal()override;
			public:
				WfClass(const WString& typeName);
				~WfClass();

				void									AddBaseType(ITypeDescriptor* type);
				void									AddMember(const WString& name, Ptr<WfStaticMethod> value);
			};

			class WfTypeImpl : public Object, public reflection::description::ITypeLoader, public reflection::Description<WfTypeImpl>
			{
			protected:
				runtime::WfRuntimeGlobalContext*		globalContext = nullptr;

			public:
				collections::List<Ptr<WfClass>>			classes;

				void									SetGlobalContext(runtime::WfRuntimeGlobalContext* _globalContext);
				void									Load(reflection::description::ITypeManager* manager)override;
				void									Unload(reflection::description::ITypeManager* manager)override;
			};
		}
	}
}

#endif