/***********************************************************************
Generated from ../Resources/Codegen/ListProcessing.txt
***********************************************************************/

#include "ListProcessing.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_ListProcessing)
	vl_workflow_global::ListProcessing instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_ListProcessing)

namespace vl_workflow_global
{
	ListProcessing& ListProcessing::Instance()
	{
		return Getvl_workflow_global_ListProcessing().instance;
	}
}

