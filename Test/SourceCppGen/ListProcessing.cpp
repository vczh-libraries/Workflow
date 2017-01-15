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
		::vl::Ptr<::vl::reflection::description::IValueList> xs = [&](){ auto __vwsn_temp__ = ::vl::reflection::description::IValueList::Create(); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 1; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 2; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 3; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 4; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 5; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); return __vwsn_temp__; }();
		::vl::WString s1 = (((([&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::__vwsn::This(xs.Obj())->GetCount(), __vwsn_temp__); return __vwsn_temp__; }() + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize([&](){ ::vl::vint32_t __vwsn_temp__; ::vl::reflection::description::UnboxParameter<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get(0), __vwsn_temp__); return __vwsn_temp__; }(), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize([&](){ ::vl::vint32_t __vwsn_temp__; ::vl::reflection::description::UnboxParameter<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get(4), __vwsn_temp__); return __vwsn_temp__; }(), __vwsn_temp__); return __vwsn_temp__; }());
		::vl::__vwsn::This(xs.Obj())->Set(4, [&](){ ::vl::vint32_t __vwsn_temp__ = 6; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }());
		::vl::__vwsn::This(xs.Obj())->Add(::vl::reflection::description::BoxValue<::vl::vint32_t>(7));
		::vl::WString s2 = (((([&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::__vwsn::This(xs.Obj())->GetCount(), __vwsn_temp__); return __vwsn_temp__; }() + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize([&](){ ::vl::vint32_t __vwsn_temp__; ::vl::reflection::description::UnboxParameter<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get(0), __vwsn_temp__); return __vwsn_temp__; }(), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize([&](){ ::vl::vint32_t __vwsn_temp__; ::vl::reflection::description::UnboxParameter<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get(4), __vwsn_temp__); return __vwsn_temp__; }(), __vwsn_temp__); return __vwsn_temp__; }());
		return ((s1 + ::vl::WString(L", ", false)) + s2);
	}

	ListProcessing& ListProcessing::Instance()
	{
		return Getvl_workflow_global_ListProcessing().instance;
	}
}

