/***********************************************************************
Generated from ../Resources/Codegen/CallStaticMethod.txt
***********************************************************************/

#include "CallStaticMethod.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_CallStaticMethod)
	vl_workflow_global::CallStaticMethod instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_CallStaticMethod)

namespace vl_workflow_global
{
	::vl::WString CallStaticMethod::main()
	{
		return static_cast<::vl::vint32_t>(::Calculator::Internal::Sum(1, 10));
	}

	CallStaticMethod& CallStaticMethod::Instance()
	{
		return Getvl_workflow_global_CallStaticMethod().instance;
	}
}

/***********************************************************************
Class (::Calculator)
***********************************************************************/

::vl::vint32_t Calculator::Sum2(::vl::vint32_t begin, ::vl::vint32_t end, ::vl::vint32_t sum)
{
	if ((begin > end))
	{
		return sum;
	}
	else
	{
		return ::Calculator::Sum2((begin + 1), end, (sum + begin));
	}
}

Calculator::Calculator()
{
}

/***********************************************************************
Class (::Calculator::Internal)
***********************************************************************/

::vl::vint32_t Calculator::Internal::Sum(::vl::vint32_t begin, ::vl::vint32_t end)
{
	return ::Calculator::Sum2(begin, end, 0);
}

