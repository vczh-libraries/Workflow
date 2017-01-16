/***********************************************************************
Generated from ../Resources/Codegen/RecursiveFunction.txt
***********************************************************************/

#include "RecursiveFunction.h"

#define GLOBAL_SYMBOL ::vl_workflow_global::RecursiveFunction::
#define GLOBAL_NAME ::vl_workflow_global::RecursiveFunction::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::RecursiveFunction::Instance()

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
		return ((n <= 2) ? 1 : (GLOBAL_NAME Fib((n - 1)) + GLOBAL_NAME Fib((n - 2))));
	}

	::vl::WString RecursiveFunction::main()
	{
		return ((((((((::vl::__vwsn::ToString(GLOBAL_NAME Fib(1)) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(GLOBAL_NAME Fib(2))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(GLOBAL_NAME Fib(3))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(GLOBAL_NAME Fib(4))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(GLOBAL_NAME Fib(5)));
	}

	RecursiveFunction& RecursiveFunction::Instance()
	{
		return Getvl_workflow_global_RecursiveFunction().instance;
	}
}


#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ
