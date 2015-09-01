#include "WfRuntimeTypeDescriptor.h"

namespace vl
{
	namespace workflow
	{
		namespace typeimpl
		{
			using namespace reflection::description;

/***********************************************************************
WfStaticMethod
***********************************************************************/

			WfStaticMethod::WfStaticMethod()
				:MethodInfoImpl(nullptr, nullptr, true)
			{
			}

			WfStaticMethod::~WfStaticMethod()
			{
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
		}
	}
}