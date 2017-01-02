/***********************************************************************
Generated from ../Resources/Codegen/Event.txt
***********************************************************************/

#include "Event.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_Event)
	vl_workflow_global::Event instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_Event)

namespace vl_workflow_global
{
	Event& Event::Instance()
	{
		return Getvl_workflow_global_Event().instance;
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

