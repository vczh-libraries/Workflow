/***********************************************************************
Generated from ../Resources/Codegen/StructCtor2.txt
***********************************************************************/

#include "StructCtor2.h"

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
		return (::vl::__vwsn::This(p). + ::vl::__vwsn::This(p).);
	}

	::vl::WString StructCtor2::main()
	{
		::Point a = [&](){ ::Point __vwsn_temp__; __vwsn_temp__.x = 1; __vwsn_temp__.y = 2; return __vwsn_temp__; }();
		::Point b = [&](){ ::Point __vwsn_temp__; __vwsn_temp__.x = 3; __vwsn_temp__.y = 4; return __vwsn_temp__; }();
		return static_cast<::vl::vint32_t>(((::vl_workflow_global::StructCtor2::Instance().Do(a) + ::vl_workflow_global::StructCtor2::Instance().Do(b)) + ::vl_workflow_global::StructCtor2::Instance().Do([&](){ ::Point __vwsn_temp__; __vwsn_temp__.x = 5; __vwsn_temp__.y = 6; return __vwsn_temp__; }())));
	}

	StructCtor2& StructCtor2::Instance()
	{
		return Getvl_workflow_global_StructCtor2().instance;
	}
}

