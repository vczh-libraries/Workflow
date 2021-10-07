/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::C++ Code Generator

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_WFCPP
#define VCZH_WORKFLOW_CPP_WFCPP

#include <VlppOS.h>

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{

/***********************************************************************
MergeCpp
***********************************************************************/

			extern WString				MergeCppMultiPlatform(const WString& code32, const WString& code64);
			extern WString				MergeCppFileContent(const WString& dst, const WString& src);
		}
	}
}

#endif