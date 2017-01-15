/***********************************************************************
Generated from ../Resources/Codegen/StructCtor.txt
***********************************************************************/

#include "StructCtor.h"

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
				return (::vl::__vwsn::This(p). + ::vl::__vwsn::This(p).);
	}

	::vl::WString StructCtor::main()
	{
				::test::Point a = [&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.x = 1; __vwsn_temp__.y = 2; return __vwsn_temp__; }();
				::test::Point b = [&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.x = 3; __vwsn_temp__.y = 4; return __vwsn_temp__; }();
				return static_cast<::vl::vint32_t>(((::vl_workflow_global::StructCtor::Instance().Do(a) + ::vl_workflow_global::StructCtor::Instance().Do(b)) + ::vl_workflow_global::StructCtor::Instance().Do([&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.x = 5; __vwsn_temp__.y = 6; return __vwsn_temp__; }())));
	}

	StructCtor& StructCtor::Instance()
	{
		return Getvl_workflow_global_StructCtor().instance;
	}
}

