/***********************************************************************
Generated from ../Resources/Codegen/BindSimple.txt
***********************************************************************/

#include "BindSimple.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_BindSimple)
	vl_workflow_global::BindSimple instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.s = L"";
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_BindSimple)

namespace vl_workflow_global
{
	void BindSimple::Callback(::vl::reflection::description::Value value)
	{
		throw 0;
	}

	::vl::WString BindSimple::main()
	{
		throw 0;
	}

	BindSimple& BindSimple::Instance()
	{
		return Getvl_workflow_global_BindSimple().instance;
	}
}

/***********************************************************************
Lambda Classes (Declaration)
***********************************************************************/



/***********************************************************************
Lambda Classes (Implementation)
***********************************************************************/



