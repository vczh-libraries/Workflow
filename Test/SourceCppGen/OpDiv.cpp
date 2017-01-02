/***********************************************************************
Generated from ../Resources/Codegen/OpDiv.txt
***********************************************************************/

#include "OpDiv.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpDiv)
	vl_workflow_global::OpDiv instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpDiv)

namespace vl_workflow_global
{
	OpDiv& OpDiv::Instance()
	{
		return Getvl_workflow_global_OpDiv().instance;
	}
}

