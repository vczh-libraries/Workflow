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
			public:
				WfStaticMethod();
				~WfStaticMethod();
			};

			class WfClass : public reflection::description::TypeDescriptorImpl
			{
			protected:
				void					LoadInternal()override;

			public:
				WfClass(const WString& typeName);
				~WfClass();

				void					AddMember(Ptr<WfStaticMethod> value);
			};
		}
	}
}

#endif