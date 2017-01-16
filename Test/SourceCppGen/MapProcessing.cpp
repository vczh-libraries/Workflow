/***********************************************************************
Generated from ../Resources/Codegen/MapProcessing.txt
***********************************************************************/

#include "MapProcessing.h"

#define GLOBAL_SYMBOL ::vl_workflow_global::MapProcessing::
#define GLOBAL_NAME ::vl_workflow_global::MapProcessing::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::MapProcessing::Instance()

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_MapProcessing)
	vl_workflow_global::MapProcessing instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_MapProcessing)

namespace vl_workflow_global
{
	::vl::WString MapProcessing::main()
	{
		auto xs = (::vl::__vwsn::CreateDictionary().Add(::vl::WString(L"a", false), 1).Add(::vl::WString(L"b", false), 2).Add(::vl::WString(L"c", false), 3).Add(::vl::WString(L"d", false), 4).Add(::vl::WString(L"e", false), 5)).dictionary;
		auto s1 = ((((::vl::__vwsn::ToString(::vl::__vwsn::This(xs.Obj())->GetCount()) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get(::vl::__vwsn::Box(::vl::WString(L"a", false)))))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get(::vl::__vwsn::Box(::vl::WString(L"e", false))))));
		::vl::__vwsn::This(xs.Obj())->Set(::vl::__vwsn::Box(::vl::WString(L"e", false)), ::vl::__vwsn::Box(6));
		::vl::__vwsn::This(xs.Obj())->Set(::vl::__vwsn::Box(::vl::WString(L"f", false)), ::vl::__vwsn::Box(7));
		auto s2 = ((((::vl::__vwsn::ToString(::vl::__vwsn::This(xs.Obj())->GetCount()) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get(::vl::__vwsn::Box(::vl::WString(L"a", false)))))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get(::vl::__vwsn::Box(::vl::WString(L"e", false))))));
		return ((s1 + ::vl::WString(L", ", false)) + s2);
	}

	MapProcessing& MapProcessing::Instance()
	{
		return Getvl_workflow_global_MapProcessing().instance;
	}
}

#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ
