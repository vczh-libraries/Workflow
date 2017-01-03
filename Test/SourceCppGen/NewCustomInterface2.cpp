/***********************************************************************
Generated from ../Resources/Codegen/NewCustomInterface2.txt
***********************************************************************/

#include "NewCustomInterface2.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewCustomInterface2)
	vl_workflow_global::NewCustomInterface2 instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewCustomInterface2)

namespace vl_workflow_global
{
	::vl::WString NewCustomInterface2::main()
	{
		throw 0;
	}

	NewCustomInterface2& NewCustomInterface2::Instance()
	{
		return Getvl_workflow_global_NewCustomInterface2().instance;
	}
}

/***********************************************************************
Lambda Classes (Declaration)
***********************************************************************/



/***********************************************************************
Lambda Classes (Implementation)
***********************************************************************/



/***********************************************************************
Class (::IMyInterface2)
***********************************************************************/

::vl::vint32_t IMyInterface2::Get100()
{
	throw 0;
}

