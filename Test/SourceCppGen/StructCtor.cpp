/***********************************************************************
Generated from ../Resources/Codegen/StructCtor.txt
***********************************************************************/

#include "StructCtor.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_StructCtor)
	vl_workflow_global::StructCtor instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_StructCtor)

namespace vl_workflow_global
{
	::vl::vint32_t StructCtor::Do(::test::Point p)
	{
		throw 0;
	}

	::vl::WString StructCtor::main()
	{
		throw 0;
	}

	StructCtor& StructCtor::Instance()
	{
		return Getvl_workflow_global_StructCtor().instance;
	}
}

