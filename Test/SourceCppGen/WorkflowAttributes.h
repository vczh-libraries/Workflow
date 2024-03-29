/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/WorkflowAttributes.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_WORKFLOWATTRIBUTES
#define VCZH_WORKFLOW_COMPILER_GENERATED_WORKFLOWATTRIBUTES

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

namespace __vwsn_enums
{
	enum class _calculator_BinaryExpression_BinaryOperator : vl::vuint64_t
	{
		Add = 0UL,
		Sub = 1UL,
		Mul = 2UL,
		Div = 3UL,
	};
	inline _calculator_BinaryExpression_BinaryOperator operator& (_calculator_BinaryExpression_BinaryOperator a, _calculator_BinaryExpression_BinaryOperator b) { return static_cast<_calculator_BinaryExpression_BinaryOperator>(static_cast<::vl::vuint64_t>(a) & static_cast<::vl::vuint64_t>(b)); }
	inline _calculator_BinaryExpression_BinaryOperator operator| (_calculator_BinaryExpression_BinaryOperator a, _calculator_BinaryExpression_BinaryOperator b) { return static_cast<_calculator_BinaryExpression_BinaryOperator>(static_cast<::vl::vuint64_t>(a) | static_cast<::vl::vuint64_t>(b)); }

}
namespace calculator
{
	class Expression;
	class BinaryExpression;
	class NumberExpression;

}
/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class WorkflowAttributes
	{
	public:

		::vl::WString Print(::vl::Ptr<::calculator::Expression> expr);
		::vl::vint Evaluate(::vl::Ptr<::calculator::Expression> expr);
		::vl::WString main();

		static WorkflowAttributes& Instance();
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
