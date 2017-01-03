/***********************************************************************
Generated from ../Resources/Codegen/OpNegative.txt
***********************************************************************/

#include "OpNegative.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpNegative)
	vl_workflow_global::OpNegative instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpNegative)

namespace vl_workflow_global
{
	::vl::WString OpNegative::main()
	{
		throw 0;
	}

	OpNegative& OpNegative::Instance()
	{
		return Getvl_workflow_global_OpNegative().instance;
	}
}

