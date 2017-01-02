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
	RecursiveFunction& RecursiveFunction::Instance()
	{
		return Getvl_workflow_global_RecursiveFunction().instance;
	}
}

