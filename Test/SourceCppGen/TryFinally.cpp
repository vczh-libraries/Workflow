/***********************************************************************
Generated from ../Resources/Codegen/TryFinally.txt
***********************************************************************/

#include "TryFinally.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_TryFinally)
	vl_workflow_global::TryFinally instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.sum = 0;
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_TryFinally)

namespace vl_workflow_global
{
	void TryFinally::test1()
	{
		throw 0;
	}

	void TryFinally::test2()
	{
		throw 0;
	}

	void TryFinally::test3()
	{
		throw 0;
	}

	void TryFinally::test4()
	{
		throw 0;
	}

	::vl::WString TryFinally::main()
	{
		throw 0;
	}

	TryFinally& TryFinally::Instance()
	{
		return Getvl_workflow_global_TryFinally().instance;
	}
}

