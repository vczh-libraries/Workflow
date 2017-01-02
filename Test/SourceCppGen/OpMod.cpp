/***********************************************************************
Generated from ../Resources/Codegen/OpMod.txt
***********************************************************************/

#include "OpMod.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpMod)
	vl_workflow_global::OpMod instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpMod)

namespace vl_workflow_global
{
	OpMod& OpMod::Instance()
	{
		return Getvl_workflow_global_OpMod().instance;
	}
}

