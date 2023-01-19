/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/ControlFlow.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_CONTROLFLOW
#define VCZH_WORKFLOW_COMPILER_GENERATED_CONTROLFLOW

#include "../Source/CppTypes.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#endif

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class ControlFlow
	{
	public:

		::vl::vint32_t Sum1(::vl::vint32_t start, ::vl::vint32_t end);
		::vl::vint32_t Sum2(::vl::vint32_t start, ::vl::vint32_t end);
		::vl::vint32_t Sum3(::vl::vint32_t start, ::vl::vint32_t end);
		::vl::vint32_t Sum4(::vl::vint32_t start, ::vl::vint32_t end);
		::vl::vint32_t Sum5(::vl::vint32_t start, ::vl::vint32_t end);
		::vl::WString main();

		static ControlFlow& Instance();
	};
}

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif
