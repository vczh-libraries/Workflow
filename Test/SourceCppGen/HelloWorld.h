/***********************************************************************
Generated from ../Resources/Codegen/HelloWorld.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_HELLOWORLD
#define VCZH_WORKFLOW_CPP_GENERATED_HELLOWORLD

#include "../Source/CppTypes.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif


/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class HelloWorld
	{
	public:

		::vl::WString main();

		static HelloWorld& Instance();
	};
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
