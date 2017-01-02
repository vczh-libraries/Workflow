/***********************************************************************
Generated from ../Resources/Codegen/ClassDtor.txt
***********************************************************************/

#include "ClassDtor.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_ClassDtor)
	vl_workflow_global::ClassDtor instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_ClassDtor)

namespace vl_workflow_global
{
	ClassDtor& ClassDtor::Instance()
	{
		return Getvl_workflow_global_ClassDtor().instance;
	}
}

/***********************************************************************
Class (::A)
***********************************************************************/

