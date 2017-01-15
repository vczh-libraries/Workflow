/***********************************************************************
Generated from ../Resources/Codegen/RecursiveFunction.txt
***********************************************************************/

#include "RecursiveFunction.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_RecursiveFunction)
	vl_workflow_global::RecursiveFunction instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_RecursiveFunction)

namespace vl_workflow_global
{
	::vl::vint32_t RecursiveFunction::Fib(::vl::vint32_t n)
	{
				return [&](){ if ((n <= 2)) return 1; else return (::vl_workflow_global::RecursiveFunction::Instance().Fib((n - 1)) + ::vl_workflow_global::RecursiveFunction::Instance().Fib((n - 2))); }();
	}

	::vl::WString RecursiveFunction::main()
	{
				return ((((((((static_cast<::vl::vint32_t>(::vl_workflow_global::RecursiveFunction::Instance().Fib(1)) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(::vl_workflow_global::RecursiveFunction::Instance().Fib(2))) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(::vl_workflow_global::RecursiveFunction::Instance().Fib(3))) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(::vl_workflow_global::RecursiveFunction::Instance().Fib(4))) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(::vl_workflow_global::RecursiveFunction::Instance().Fib(5)));
	}

	RecursiveFunction& RecursiveFunction::Instance()
	{
		return Getvl_workflow_global_RecursiveFunction().instance;
	}
}

