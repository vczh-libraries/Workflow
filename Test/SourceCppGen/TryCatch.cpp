/***********************************************************************
Generated from ../Resources/Codegen/TryCatch.txt
***********************************************************************/

#include "TryCatch.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_TryCatch)
	vl_workflow_global::TryCatch instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.log = ::vl::WString(L"", false);
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_TryCatch)

namespace vl_workflow_global
{
	void TryCatch::Test1()
	{
		throw 0;
	}

	void TryCatch::Test2()
	{
		throw 0;
	}

	void TryCatch::Throw(const ::vl::WString& message)
	{
		throw 0;
	}

	void TryCatch::Test3()
	{
		throw 0;
	}

	void TryCatch::Test4()
	{
		throw 0;
	}

	::vl::WString TryCatch::main()
	{
		throw 0;
	}

	TryCatch& TryCatch::Instance()
	{
		return Getvl_workflow_global_TryCatch().instance;
	}
}

