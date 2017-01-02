/***********************************************************************
Generated from ../Resources/Codegen/MethodClosure.txt
***********************************************************************/

#include "MethodClosure.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_MethodClosure)
	vl_workflow_global::MethodClosure instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_MethodClosure)

namespace vl_workflow_global
{
	MethodClosure& MethodClosure::Instance()
	{
		return Getvl_workflow_global_MethodClosure().instance;
	}
}

