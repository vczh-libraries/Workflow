/***********************************************************************
Generated from ../Resources/Codegen/Property.txt
***********************************************************************/

#include "Property.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_Property)
	vl_workflow_global::Property instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_Property)

namespace vl_workflow_global
{
	Property& Property::Instance()
	{
		return Getvl_workflow_global_Property().instance;
	}
}

