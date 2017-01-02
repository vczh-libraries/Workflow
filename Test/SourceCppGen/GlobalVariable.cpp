/***********************************************************************
Generated from ../Resources/Codegen/GlobalVariable.txt
***********************************************************************/

#include "GlobalVariable.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_GlobalVariable)
	vl_workflow_global::GlobalVariable instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_GlobalVariable)

namespace vl_workflow_global
{
	GlobalVariable& GlobalVariable::Instance()
	{
		return Getvl_workflow_global_GlobalVariable().instance;
	}
}

/***********************************************************************
Lambda Functions (Declaration)
***********************************************************************/

/***********************************************************************
Lambda Classes (Declaration)
***********************************************************************/

/***********************************************************************
Lambda Functions (Implementation)
***********************************************************************/

/***********************************************************************
Lambda Classes (Implementation)
***********************************************************************/

