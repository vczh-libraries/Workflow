/***********************************************************************
Generated from ../Resources/Codegen/FailedThen.txt
***********************************************************************/

#include "FailedThen.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_FailedThen)
	vl_workflow_global::FailedThen instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_FailedThen)

namespace vl_workflow_global
{
	::vl::vint32_t FailedThen::GetValue(::vl::Ptr<::test::ObservableValue> value, ::vl::vint32_t increase, ::vl::vint32_t defaultValue)
	{
		throw 0;
	}

	::vl::WString FailedThen::main()
	{
		throw 0;
	}

	FailedThen& FailedThen::Instance()
	{
		return Getvl_workflow_global_FailedThen().instance;
	}
}

