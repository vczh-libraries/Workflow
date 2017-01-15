/***********************************************************************
Generated from ../Resources/Codegen/OpCompareReference.txt
***********************************************************************/

#include "OpCompareReference.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpCompareReference)
	vl_workflow_global::OpCompareReference instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpCompareReference)

namespace vl_workflow_global
{
	::vl::WString OpCompareReference::main()
	{
		::vl::Ptr<::test::ObservableValue> a = ::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue(1));
		::vl::Ptr<::test::ObservableValue> b = ::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue(2));
		::vl::Ptr<::test::ObservableValue> c = nullptr;
		return (((((((((((((((((((([&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize((! static_cast<bool>(a)), __vwsn_temp__); return __vwsn_temp__; }() + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize(static_cast<bool>(a), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize((a.Obj() == b.Obj()), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize((! static_cast<bool>(b)), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize(static_cast<bool>(b), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize((a.Obj() != b.Obj()), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize((a.Obj() == c.Obj()), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize((b.Obj() == c.Obj()), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize((a.Obj() == a.Obj()), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize((b.Obj() == b.Obj()), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize((c.Obj() == c.Obj()), __vwsn_temp__); return __vwsn_temp__; }());
	}

	OpCompareReference& OpCompareReference::Instance()
	{
		return Getvl_workflow_global_OpCompareReference().instance;
	}
}

