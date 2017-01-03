/***********************************************************************
Generated from ../Resources/Codegen/ClassCtor.txt
***********************************************************************/

#include "ClassCtor.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_ClassCtor)
	vl_workflow_global::ClassCtor instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.s = L"";
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_ClassCtor)

namespace vl_workflow_global
{
	::vl::WString ClassCtor::main()
	{
		throw 0;
	}

	ClassCtor& ClassCtor::Instance()
	{
		return Getvl_workflow_global_ClassCtor().instance;
	}
}

/***********************************************************************
Class (::Y)
***********************************************************************/

/***********************************************************************
Class (::A)
***********************************************************************/

