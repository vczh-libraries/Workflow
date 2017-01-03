/***********************************************************************
Generated from ../Resources/Codegen/AnonymousLambda.txt
***********************************************************************/

#include "AnonymousLambda.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_AnonymousLambda)
	vl_workflow_global::AnonymousLambda instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_AnonymousLambda)

namespace vl_workflow_global
{
	::vl::Func<::vl::Func<::vl::vint32_t()>(::vl::vint32_t)> AnonymousLambda::Adder(::vl::vint32_t x)
	{
		throw 0;
	}

	::vl::WString AnonymousLambda::main()
	{
		throw 0;
	}

	AnonymousLambda& AnonymousLambda::Instance()
	{
		return Getvl_workflow_global_AnonymousLambda().instance;
	}




}

