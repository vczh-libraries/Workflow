/***********************************************************************
Generated from ../Resources/Codegen/NewCustomInterface.txt
***********************************************************************/

#include "NewCustomInterface.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewCustomInterface)
	vl_workflow_global::NewCustomInterface instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewCustomInterface)

namespace vl_workflow_global
{
	::vl::WString NewCustomInterface::main()
	{
		throw 0;
	}

	NewCustomInterface& NewCustomInterface::Instance()
	{
		return Getvl_workflow_global_NewCustomInterface().instance;
	}
}

/***********************************************************************
Lambda Classes (Declaration)
***********************************************************************/



/***********************************************************************
Lambda Classes (Implementation)
***********************************************************************/



/***********************************************************************
Class (::IMyInterface)
***********************************************************************/

::vl::vint32_t ::IMyInterface::Get50()
{
	throw 0;
}

::vl::vint32_t ::IMyInterface::Get100()
{
	throw 0;
}

