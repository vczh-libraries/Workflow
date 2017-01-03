/***********************************************************************
Generated from ../Resources/Codegen/EnumCtor.txt
***********************************************************************/

#include "EnumCtor.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_EnumCtor)
	vl_workflow_global::EnumCtor instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_EnumCtor)

namespace vl_workflow_global
{
	::test::Seasons EnumCtor::Id(::test::Seasons s)
	{
		throw 0;
	}

	::vl::WString EnumCtor::main()
	{
		throw 0;
	}

	EnumCtor& EnumCtor::Instance()
	{
		return Getvl_workflow_global_EnumCtor().instance;
	}
}

