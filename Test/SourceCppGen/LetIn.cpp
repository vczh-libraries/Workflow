/***********************************************************************
Generated from ../Resources/Codegen/LetIn.txt
***********************************************************************/

#include "LetIn.h"

#define GLOBAL_SYMBOL ::vl_workflow_global::LetIn::
#define GLOBAL_NAME ::vl_workflow_global::LetIn::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::LetIn::Instance()

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
		return [&](auto x, auto y){ return ::vl::__vwsn::ToString((x + y)); }(10, 20);
	}

	LetIn& LetIn::Instance()
	{
		return Getvl_workflow_global_LetIn().instance;
	}
}


#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ
