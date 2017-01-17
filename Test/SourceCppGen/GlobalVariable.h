/***********************************************************************
Generated from ../Resources/Codegen/GlobalVariable.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_GLOBALVARIABLE
#define VCZH_WORKFLOW_CPP_GENERATED_GLOBALVARIABLE

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
	class GlobalVariable
	{
	public:

		::vl::vint32_t x = 0;

		::vl::vint32_t Add(::vl::vint32_t y);
		::vl::WString main();

		static GlobalVariable& Instance();
	};
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
