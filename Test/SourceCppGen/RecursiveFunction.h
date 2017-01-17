/***********************************************************************
Generated from ../Resources/Codegen/RecursiveFunction.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_RECURSIVEFUNCTION
#define VCZH_WORKFLOW_CPP_GENERATED_RECURSIVEFUNCTION

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
	class RecursiveFunction
	{
	public:

		::vl::vint32_t Fib(::vl::vint32_t n);
		::vl::WString main();

		static RecursiveFunction& Instance();
	};
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
