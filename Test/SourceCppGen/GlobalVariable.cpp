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
		auto z = ::vl_workflow_global::GlobalVariable::Instance().x;
		(::vl_workflow_global::GlobalVariable::Instance().x = (::vl_workflow_global::GlobalVariable::Instance().x + y));
		return z;
	}

	::vl::WString GlobalVariable::main()
	{
		(::vl_workflow_global::GlobalVariable::Instance().x = 10);
		auto y = ::vl_workflow_global::GlobalVariable::Instance().Add(20);
		return ((::vl::__vwsn::ToString(::vl_workflow_global::GlobalVariable::Instance().x) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(y));
	}

	GlobalVariable& GlobalVariable::Instance()
	{
		return Getvl_workflow_global_GlobalVariable().instance;
	}
}

