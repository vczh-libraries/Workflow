/***********************************************************************
Generated from ../Resources/Codegen/OpPositive.txt
***********************************************************************/

#include "OpPositive.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpPositive)
	vl_workflow_global::OpPositive instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpPositive)

namespace vl_workflow_global
{
	OpPositive& OpPositive::Instance()
	{
		return Getvl_workflow_global_OpPositive().instance;
	}
}

