/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/OrderedLambda.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_ORDEREDLAMBDA
#define VCZH_WORKFLOW_COMPILER_GENERATED_ORDEREDLAMBDA

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

namespace vl_workflow_global
{
	struct __vwsno1_OrderedLambda_Adder_;
	struct __vwsno2_OrderedLambda_Adder_;
}

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class OrderedLambda
	{
	public:

		::vl::vint32_t Apply(const ::vl::Func<::vl::vint32_t(::vl::vint32_t)>& f, ::vl::vint32_t arg);
		::vl::Func<::vl::vint32_t(::vl::vint32_t)> Adder(::vl::vint32_t x);
		::vl::WString main();

		static OrderedLambda& Instance();
	};

/***********************************************************************
Closures
***********************************************************************/

	struct __vwsno1_OrderedLambda_Adder_
	{
		::vl::vint32_t x;

		__vwsno1_OrderedLambda_Adder_(::vl::vint32_t __vwsnctor_x);

		::vl::vint32_t operator()(::vl::vint32_t __vwsno_1) const;
	};

	struct __vwsno2_OrderedLambda_Adder_
	{
		::vl::vint32_t __vwsno_1;

		__vwsno2_OrderedLambda_Adder_(::vl::vint32_t __vwsnctor___vwsno_1);

		::vl::vint32_t operator()(::vl::vint32_t __vwsno_2) const;
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
