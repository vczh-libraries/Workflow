/***********************************************************************
Generated from ../Resources/Codegen/StaticMethod.txt
***********************************************************************/

#include "StaticMethod.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_StaticMethod)
	vl_workflow_global::StaticMethod instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_StaticMethod)

namespace vl_workflow_global
{
	::vl::Ptr<::test::ObservableValue> StaticMethod::Create2(::vl::vint32_t value, const ::vl::WString& name)
	{
		return ::test::ObservableValue::Create(value, name);
	}

	::vl::WString StaticMethod::main()
	{
		::vl::Func<::vl::Ptr<::test::ObservableValue>(::vl::vint32_t, const ::vl::WString&)> create1 = ::vl::Func<::vl::Ptr<::test::ObservableValue>(::vl::vint32_t, const ::vl::WString&)>(&::test::ObservableValue::Create);
		::vl::Func<::vl::Ptr<::test::ObservableValue>(::vl::vint32_t, const ::vl::WString&)> create2 = ::vl::Func<::vl::Ptr<::test::ObservableValue>(::vl::vint32_t, const ::vl::WString&)>(&::vl_workflow_global::StaticMethod::Instance(), &::vl_workflow_global::StaticMethod::Create2);
		::vl::Ptr<::test::ObservableValue> o1 = create1(1, ::vl::WString(L"one", false));
		::vl::Ptr<::test::ObservableValue> o2 = ::vl_workflow_global::StaticMethod::Instance().Create2(2, ::vl::WString(L"two", false));
		return (((((([&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::__vwsn::This(o1)->GetValue(), __vwsn_temp__); return __vwsn_temp__; }() + ::vl::WString(L", ", false)) + ::vl::__vwsn::This(o1)->GetDisplayName()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::__vwsn::This(o2)->GetValue(), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + ::vl::__vwsn::This(o2)->GetDisplayName());
	}

	StaticMethod& StaticMethod::Instance()
	{
		return Getvl_workflow_global_StaticMethod().instance;
	}
}

