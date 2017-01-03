/***********************************************************************
Generated from ../Resources/Codegen/OpAdd.txt
***********************************************************************/

#include "OpAdd.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpAdd)
	vl_workflow_global::OpAdd instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpAdd)

namespace vl_workflow_global
{
	::vl::WString OpAdd::main()
	{
		throw 0;
	}

	OpAdd& OpAdd::Instance()
	{
		return Getvl_workflow_global_OpAdd().instance;
	}
}

