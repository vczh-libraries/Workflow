/***********************************************************************
Generated from ../Resources/Codegen/MapProcessing.txt
***********************************************************************/

#include "MapProcessing.h"

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
		::vl::Ptr<::vl::reflection::description::IValueDictionary> xs = [&](){ auto __vwsn_temp__ = ::vl::reflection::description::IValueDictionary::Create(); __vwsn_temp__->Set(::vl::__vwsn::Box(::vl::WString(L"a", false)), ::vl::__vwsn::Box(1)); __vwsn_temp__->Set(::vl::__vwsn::Box(::vl::WString(L"b", false)), ::vl::__vwsn::Box(2)); __vwsn_temp__->Set(::vl::__vwsn::Box(::vl::WString(L"c", false)), ::vl::__vwsn::Box(3)); __vwsn_temp__->Set(::vl::__vwsn::Box(::vl::WString(L"d", false)), ::vl::__vwsn::Box(4)); __vwsn_temp__->Set(::vl::__vwsn::Box(::vl::WString(L"e", false)), ::vl::__vwsn::Box(5)); return __vwsn_temp__; }();
		::vl::WString s1 = ((((::vl::__vwsn::ToString(::vl::__vwsn::This(xs.Obj())->GetCount()) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get(::vl::__vwsn::Box(::vl::WString(L"a", false)))))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get(::vl::__vwsn::Box(::vl::WString(L"e", false))))));
		::vl::__vwsn::This(xs.Obj())->Set(::vl::__vwsn::Box(::vl::WString(L"e", false)), ::vl::__vwsn::Box(6));
		::vl::__vwsn::This(xs.Obj())->Set(::vl::__vwsn::Box(::vl::WString(L"f", false)), ::vl::__vwsn::Box(7));
		::vl::WString s2 = ((((::vl::__vwsn::ToString(::vl::__vwsn::This(xs.Obj())->GetCount()) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get(::vl::__vwsn::Box(::vl::WString(L"a", false)))))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get(::vl::__vwsn::Box(::vl::WString(L"e", false))))));
		return ((s1 + ::vl::WString(L", ", false)) + s2);
	}

	MapProcessing& MapProcessing::Instance()
	{
		return Getvl_workflow_global_MapProcessing().instance;
	}
}

