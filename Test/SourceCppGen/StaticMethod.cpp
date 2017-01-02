/***********************************************************************
Generated from ../Resources/Codegen/StaticMethod.txt
***********************************************************************/

#include "StaticMethod.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_StaticMethod)
	vl_workflow_global::StaticMethod instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_StaticMethod)

namespace vl_workflow_global
{
	StaticMethod& StaticMethod::Instance()
	{
		return Getvl_workflow_global_StaticMethod().instance;
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

