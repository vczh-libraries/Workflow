/***********************************************************************
Generated from ../Resources/Codegen/OpAnd.txt
***********************************************************************/

#include "OpAnd.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpAnd)
	vl_workflow_global::OpAnd instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpAnd)

namespace vl_workflow_global
{
	::vl::WString OpAnd::main()
	{
		throw 0;
	}

	OpAnd& OpAnd::Instance()
	{
		return Getvl_workflow_global_OpAnd().instance;
	}
}

