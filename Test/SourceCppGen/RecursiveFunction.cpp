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
		throw 0;
	}

	::vl::WString RecursiveFunction::main()
	{
		throw 0;
	}

	RecursiveFunction& RecursiveFunction::Instance()
	{
		return Getvl_workflow_global_RecursiveFunction().instance;
	}
}

