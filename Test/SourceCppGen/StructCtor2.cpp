/***********************************************************************
Generated from ../Resources/Codegen/StructCtor2.txt
***********************************************************************/

#include "StructCtor2.h"

#define GLOBAL_SYMBOL ::vl_workflow_global::StructCtor2::
#define GLOBAL_NAME ::vl_workflow_global::StructCtor2::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::StructCtor2::Instance()

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_StructCtor2)
	vl_workflow_global::StructCtor2 instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_StructCtor2)

namespace vl_workflow_global
{
	::vl::vint32_t StructCtor2::Do(::Point p)
	{
		return (p.x + p.y);
	}

	::vl::WString StructCtor2::main()
	{
		auto a = [&](){ ::Point __vwsn_temp__; __vwsn_temp__.x = 1; __vwsn_temp__.y = 2; return __vwsn_temp__; }();
		auto b = [&](){ ::Point __vwsn_temp__; __vwsn_temp__.x = 3; __vwsn_temp__.y = 4; return __vwsn_temp__; }();
		return ::vl::__vwsn::ToString(((GLOBAL_NAME Do(a) + GLOBAL_NAME Do(b)) + GLOBAL_NAME Do([&](){ ::Point __vwsn_temp__; __vwsn_temp__.x = 5; __vwsn_temp__.y = 6; return __vwsn_temp__; }())));
	}

	StructCtor2& StructCtor2::Instance()
	{
		return Getvl_workflow_global_StructCtor2().instance;
	}
}


#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ
