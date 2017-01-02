/***********************************************************************
Generated from ../Resources/Codegen/NewInterface.txt
***********************************************************************/

#include "NewInterface.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewInterface)
	vl_workflow_global::NewInterface instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewInterface)

namespace vl_workflow_global
{
	NewInterface& NewInterface::Instance()
	{
		return Getvl_workflow_global_NewInterface().instance;
	}
}

/***********************************************************************
Lambda Classes (Declaration)
***********************************************************************/



/***********************************************************************
Lambda Classes (Implementation)
***********************************************************************/



