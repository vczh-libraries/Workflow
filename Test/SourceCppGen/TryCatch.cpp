/***********************************************************************
Generated from ../Resources/Codegen/TryCatch.txt
***********************************************************************/

#include "TryCatch.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_TryCatch)
	vl_workflow_global::TryCatch instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_TryCatch)

namespace vl_workflow_global
{
	TryCatch& TryCatch::Instance()
	{
		return Getvl_workflow_global_TryCatch().instance;
	}
}

