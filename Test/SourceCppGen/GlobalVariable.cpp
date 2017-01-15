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
		::vl::vint32_t z = ::vl_workflow_global::GlobalVariable::Instance().x;
		(::vl_workflow_global::GlobalVariable::Instance().x = (::vl_workflow_global::GlobalVariable::Instance().x + y));
		return z;
	}

	::vl::WString GlobalVariable::main()
	{
		(::vl_workflow_global::GlobalVariable::Instance().x = 10);
		::vl::vint32_t y = ::vl_workflow_global::GlobalVariable::Instance().Add(20);
		return ((static_cast<::vl::vint32_t>(::vl_workflow_global::GlobalVariable::Instance().x) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(y));
	}

	GlobalVariable& GlobalVariable::Instance()
	{
		return Getvl_workflow_global_GlobalVariable().instance;
	}
}

