/***********************************************************************
Generated from ../Resources/Codegen/TryFinally.txt
***********************************************************************/

#include "TryFinally.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_TryFinally)
	vl_workflow_global::TryFinally instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_TryFinally)

namespace vl_workflow_global
{
	TryFinally& TryFinally::Instance()
	{
		return Getvl_workflow_global_TryFinally().instance;
	}
}

