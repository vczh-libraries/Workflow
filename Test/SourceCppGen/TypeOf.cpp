/***********************************************************************
Generated from ../Resources/Codegen/TypeOf.txt
***********************************************************************/

#include "TypeOf.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_TypeOf)
	vl_workflow_global::TypeOf instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_TypeOf)

namespace vl_workflow_global
{
	::vl::WString TypeOf::main()
	{
		throw 0;
	}

	TypeOf& TypeOf::Instance()
	{
		return Getvl_workflow_global_TypeOf().instance;
	}
}

