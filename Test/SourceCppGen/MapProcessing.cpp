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
		::vl::Ptr<::vl::reflection::description::IValueDictionary> xs = [&](){ auto __vwsn_temp__ = ::vl::reflection::description::IValueDictionary::Create(); __vwsn_temp__->Set([&](){ ::vl::WString __vwsn_temp__ = ::vl::WString(L"a", false); return ::vl::reflection::description::BoxParameter<::vl::WString>(__vwsn_temp__); }(), [&](){ ::vl::vint32_t __vwsn_temp__ = 1; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Set([&](){ ::vl::WString __vwsn_temp__ = ::vl::WString(L"b", false); return ::vl::reflection::description::BoxParameter<::vl::WString>(__vwsn_temp__); }(), [&](){ ::vl::vint32_t __vwsn_temp__ = 2; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Set([&](){ ::vl::WString __vwsn_temp__ = ::vl::WString(L"c", false); return ::vl::reflection::description::BoxParameter<::vl::WString>(__vwsn_temp__); }(), [&](){ ::vl::vint32_t __vwsn_temp__ = 3; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Set([&](){ ::vl::WString __vwsn_temp__ = ::vl::WString(L"d", false); return ::vl::reflection::description::BoxParameter<::vl::WString>(__vwsn_temp__); }(), [&](){ ::vl::vint32_t __vwsn_temp__ = 4; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Set([&](){ ::vl::WString __vwsn_temp__ = ::vl::WString(L"e", false); return ::vl::reflection::description::BoxParameter<::vl::WString>(__vwsn_temp__); }(), [&](){ ::vl::vint32_t __vwsn_temp__ = 5; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); return __vwsn_temp__; }();
		::vl::WString s1 = (((([&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::__vwsn::This(xs.Obj())->GetCount(), __vwsn_temp__); return __vwsn_temp__; }() + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize([&](){ ::vl::vint32_t __vwsn_temp__; ::vl::reflection::description::UnboxParameter<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get([&](){ ::vl::WString __vwsn_temp__ = ::vl::WString(L"a", false); return ::vl::reflection::description::BoxParameter<::vl::WString>(__vwsn_temp__); }()), __vwsn_temp__); return __vwsn_temp__; }(), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize([&](){ ::vl::vint32_t __vwsn_temp__; ::vl::reflection::description::UnboxParameter<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get([&](){ ::vl::WString __vwsn_temp__ = ::vl::WString(L"e", false); return ::vl::reflection::description::BoxParameter<::vl::WString>(__vwsn_temp__); }()), __vwsn_temp__); return __vwsn_temp__; }(), __vwsn_temp__); return __vwsn_temp__; }());
		::vl::__vwsn::This(xs.Obj())->Set([&](){ ::vl::WString __vwsn_temp__ = ::vl::WString(L"e", false); return ::vl::reflection::description::BoxParameter<::vl::WString>(__vwsn_temp__); }(), [&](){ ::vl::vint32_t __vwsn_temp__ = 6; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }());
		::vl::__vwsn::This(xs.Obj())->Set(::vl::reflection::description::BoxValue<::vl::WString>(::vl::WString(L"f", false)), ::vl::reflection::description::BoxValue<::vl::vint32_t>(7));
		::vl::WString s2 = (((([&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::__vwsn::This(xs.Obj())->GetCount(), __vwsn_temp__); return __vwsn_temp__; }() + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize([&](){ ::vl::vint32_t __vwsn_temp__; ::vl::reflection::description::UnboxParameter<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get([&](){ ::vl::WString __vwsn_temp__ = ::vl::WString(L"a", false); return ::vl::reflection::description::BoxParameter<::vl::WString>(__vwsn_temp__); }()), __vwsn_temp__); return __vwsn_temp__; }(), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize([&](){ ::vl::vint32_t __vwsn_temp__; ::vl::reflection::description::UnboxParameter<::vl::vint32_t>(::vl::__vwsn::This(xs.Obj())->Get([&](){ ::vl::WString __vwsn_temp__ = ::vl::WString(L"e", false); return ::vl::reflection::description::BoxParameter<::vl::WString>(__vwsn_temp__); }()), __vwsn_temp__); return __vwsn_temp__; }(), __vwsn_temp__); return __vwsn_temp__; }());
		return ((s1 + ::vl::WString(L", ", false)) + s2);
	}

	MapProcessing& MapProcessing::Instance()
	{
		return Getvl_workflow_global_MapProcessing().instance;
	}
}

