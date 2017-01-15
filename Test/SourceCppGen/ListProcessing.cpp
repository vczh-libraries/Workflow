/***********************************************************************
Generated from ../Resources/Codegen/ListProcessing.txt
***********************************************************************/

#include "ListProcessing.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_ListProcessing)
	vl_workflow_global::ListProcessing instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_ListProcessing)

namespace vl_workflow_global
{
	::vl::WString ListProcessing::main()
	{
		::vl::Ptr<::vl::reflection::description::IValueList> xs = [&](){ auto __vwsn_temp__ = ::vl::reflection::description::IValueList::Create(); __vwsn_temp__->Add(::vl::__vwsn::Box(1)); __vwsn_temp__->Add(::vl::__vwsn::Box(2)); __vwsn_temp__->Add(::vl::__vwsn::Box(3)); __vwsn_temp__->Add(::vl::__vwsn::Box(4)); __vwsn_temp__->Add(::vl::__vwsn::Box(5)); return __vwsn_temp__; }();
		::vl::WString s1 = ((((::vl::__vwsn::ToString(::vl::__vwsn::This(xs.Obj())->GetCount()) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get(0)))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get(4))));
		::vl::__vwsn::This(xs.Obj())->Set(4, ::vl::__vwsn::Box(6));
		::vl::__vwsn::This(xs.Obj())->Add(::vl::__vwsn::Box(7));
		::vl::WString s2 = ((((::vl::__vwsn::ToString(::vl::__vwsn::This(xs.Obj())->GetCount()) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get(0)))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get(4))));
		return ((s1 + ::vl::WString(L", ", false)) + s2);
	}

	ListProcessing& ListProcessing::Instance()
	{
		return Getvl_workflow_global_ListProcessing().instance;
	}
}

