/***********************************************************************
Generated from ../Resources/Codegen/OpNot.txt
***********************************************************************/

#include "OpNot.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpNot)
	vl_workflow_global::OpNot instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpNot)

namespace vl_workflow_global
{
	::vl::WString OpNot::main()
	{
		throw 0;
	}

	OpNot& OpNot::Instance()
	{
		return Getvl_workflow_global_OpNot().instance;
	}
}

