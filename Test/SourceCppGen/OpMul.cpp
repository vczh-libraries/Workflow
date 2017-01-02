/***********************************************************************
Generated from ../Resources/Codegen/OpMul.txt
***********************************************************************/

#include "OpMul.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpMul)
	vl_workflow_global::OpMul instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpMul)

namespace vl_workflow_global
{
	OpMul& OpMul::Instance()
	{
		return Getvl_workflow_global_OpMul().instance;
	}
}

