/***********************************************************************
Generated from ../Resources/Codegen/OpCompare.txt
***********************************************************************/

#include "OpCompare.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpCompare)
	vl_workflow_global::OpCompare instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpCompare)

namespace vl_workflow_global
{
	OpCompare& OpCompare::Instance()
	{
		return Getvl_workflow_global_OpCompare().instance;
	}
}

