/***********************************************************************
Generated from ../Resources/Codegen/AnonymousLambda.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_ANONYMOUSLAMBDA
#define VCZH_WORKFLOW_CPP_GENERATED_ANONYMOUSLAMBDA

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
	class AnonymousLambda
	{
	public:

		::vl::Func<::vl::Func<::vl::vint32_t()>(::vl::vint32_t)> Adder(::vl::vint32_t x);
		::vl::WString main();

		static AnonymousLambda& Instance();
	};
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
