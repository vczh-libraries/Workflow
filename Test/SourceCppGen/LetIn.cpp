/***********************************************************************
Generated from ../Resources/Codegen/LetIn.txt
***********************************************************************/

#include "LetIn.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_LetIn)
	vl_workflow_global::LetIn instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_LetIn)

namespace vl_workflow_global
{
	::vl::WString LetIn::main()
	{
		throw 0;
	}

	LetIn& LetIn::Instance()
	{
		return Getvl_workflow_global_LetIn().instance;
	}
}

