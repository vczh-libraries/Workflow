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
				::vl::Ptr<::vl::reflection::description::IValueList> xs = [&](){ auto __vwsn_temp__ = ::vl::reflection::description::IValueList::Create(); __vwsn_temp__->Add(::vl::reflection::description::BoxParameter<::vl::vint32_t>(1)); __vwsn_temp__->Add(::vl::reflection::description::BoxParameter<::vl::vint32_t>(2)); __vwsn_temp__->Add(::vl::reflection::description::BoxParameter<::vl::vint32_t>(3)); __vwsn_temp__->Add(::vl::reflection::description::BoxParameter<::vl::vint32_t>(4)); __vwsn_temp__->Add(::vl::reflection::description::BoxParameter<::vl::vint32_t>(5)); return __vwsn_temp__; }();
				::vl::WString s1 = ((((::vl::__vwsn::This(xs)->GetCount() + ::vl::WString(L", ", false)) + [&](){ ::vl::vint32_t __vwsn_temp__; ::vl::reflection::description::UnboxParameter<::vl::vint32_t>(::vl::__vwsn::This(xs)->Get(0), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::vint32_t __vwsn_temp__; ::vl::reflection::description::UnboxParameter<::vl::vint32_t>(::vl::__vwsn::This(xs)->Get(4), __vwsn_temp__); return __vwsn_temp__; }());
				::vl::__vwsn::This(xs)->Set(4, ::vl::reflection::description::BoxParameter<::vl::vint32_t>(6));
				::vl::__vwsn::This(xs)->Add(7);
				::vl::WString s2 = ((((::vl::__vwsn::This(xs)->GetCount() + ::vl::WString(L", ", false)) + [&](){ ::vl::vint32_t __vwsn_temp__; ::vl::reflection::description::UnboxParameter<::vl::vint32_t>(::vl::__vwsn::This(xs)->Get(0), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::vint32_t __vwsn_temp__; ::vl::reflection::description::UnboxParameter<::vl::vint32_t>(::vl::__vwsn::This(xs)->Get(4), __vwsn_temp__); return __vwsn_temp__; }());
				return ((s1 + ::vl::WString(L", ", false)) + s2);
	}

	ListProcessing& ListProcessing::Instance()
	{
		return Getvl_workflow_global_ListProcessing().instance;
	}
}

