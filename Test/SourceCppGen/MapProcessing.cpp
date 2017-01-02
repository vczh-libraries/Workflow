/***********************************************************************
Generated from ../Resources/Codegen/MapProcessing.txt
***********************************************************************/

#include "MapProcessing.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_MapProcessing)
	vl_workflow_global::MapProcessing instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_MapProcessing)

namespace vl_workflow_global
{
	MapProcessing& MapProcessing::Instance()
	{
		return Getvl_workflow_global_MapProcessing().instance;
	}
}

