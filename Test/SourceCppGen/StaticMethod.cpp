/***********************************************************************
Generated from ../Resources/Codegen/StaticMethod.txt
***********************************************************************/

#include "StaticMethod.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_StaticMethod)
	vl_workflow_global::StaticMethod instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_StaticMethod)

namespace vl_workflow_global
{
	::vl::Ptr<::test::ObservableValue> StaticMethod::Create2(::vl::vint32_t value, ::vl::WString name)
	{
		throw 0;
	}

	::vl::WString StaticMethod::main()
	{
		throw 0;
	}

	StaticMethod& StaticMethod::Instance()
	{
		return Getvl_workflow_global_StaticMethod().instance;
	}
}

