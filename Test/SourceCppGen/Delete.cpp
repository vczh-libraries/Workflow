/***********************************************************************
Generated from ../Resources/Codegen/Delete.txt
***********************************************************************/

#include "Delete.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_Delete)
	vl_workflow_global::Delete instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_Delete)

namespace vl_workflow_global
{
	bool Delete::Delete(::test::ObservableValue* value)
	{
		throw 0;
	}

	::vl::WString Delete::main()
	{
		throw 0;
	}

	Delete& Delete::Instance()
	{
		return Getvl_workflow_global_Delete().instance;
	}
}

