/***********************************************************************
Generated from ../Resources/Codegen/StructCtor.txt
***********************************************************************/

#include "StructCtor.h"

#define GLOBAL_SYMBOL ::vl_workflow_global::StructCtor::
#define GLOBAL_NAME ::vl_workflow_global::StructCtor::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::StructCtor::Instance()

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
		return (p.x + p.y);
	}

	::vl::WString StructCtor::main()
	{
		auto a = [&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.x = 1; __vwsn_temp__.y = 2; return __vwsn_temp__; }();
		auto b = [&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.x = 3; __vwsn_temp__.y = 4; return __vwsn_temp__; }();
		return ::vl::__vwsn::ToString(((GLOBAL_NAME Do(a) + GLOBAL_NAME Do(b)) + GLOBAL_NAME Do([&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.x = 5; __vwsn_temp__.y = 6; return __vwsn_temp__; }())));
	}

	StructCtor& StructCtor::Instance()
	{
		return Getvl_workflow_global_StructCtor().instance;
	}
}


#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ
