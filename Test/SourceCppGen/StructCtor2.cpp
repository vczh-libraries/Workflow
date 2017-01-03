/***********************************************************************
Generated from ../Resources/Codegen/StructCtor2.txt
***********************************************************************/

#include "StructCtor2.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_StructCtor2)
	vl_workflow_global::StructCtor2 instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_StructCtor2)

namespace vl_workflow_global
{
	::vl::vint32_t StructCtor2::Do(::Point p)
	{
		throw 0;
	}

	::vl::WString StructCtor2::main()
	{
		throw 0;
	}

	StructCtor2& StructCtor2::Instance()
	{
		return Getvl_workflow_global_StructCtor2().instance;
	}
}

