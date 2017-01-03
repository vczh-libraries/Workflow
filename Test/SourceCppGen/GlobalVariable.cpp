/***********************************************************************
Generated from ../Resources/Codegen/GlobalVariable.txt
***********************************************************************/

#include "GlobalVariable.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_GlobalVariable)
	vl_workflow_global::GlobalVariable instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.x = 0;
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_GlobalVariable)

namespace vl_workflow_global
{
	::vl::vint32_t GlobalVariable::Add(::vl::vint32_t y)
	{
		throw 0;
	}

	::vl::WString GlobalVariable::main()
	{
		throw 0;
	}

	GlobalVariable& GlobalVariable::Instance()
	{
		return Getvl_workflow_global_GlobalVariable().instance;
	}
}

