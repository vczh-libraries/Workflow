/***********************************************************************
Generated from ../Resources/Codegen/BindFormat.txt
***********************************************************************/

#include "BindFormat.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_BindFormat)
	vl_workflow_global::BindFormat instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.s = L"";
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_BindFormat)

namespace vl_workflow_global
{
	void BindFormat::Callback(::vl::reflection::description::Value value)
	{
		throw 0;
	}

	::vl::WString BindFormat::main()
	{
		throw 0;
	}

	BindFormat& BindFormat::Instance()
	{
		return Getvl_workflow_global_BindFormat().instance;
	}
}

/***********************************************************************
Lambda Classes (Declaration)
***********************************************************************/



/***********************************************************************
Lambda Classes (Implementation)
***********************************************************************/



