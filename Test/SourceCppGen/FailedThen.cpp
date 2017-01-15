/***********************************************************************
Generated from ../Resources/Codegen/FailedThen.txt
***********************************************************************/

#include "FailedThen.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_FailedThen)
	vl_workflow_global::FailedThen instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_FailedThen)

namespace vl_workflow_global
{
	::vl::vint32_t FailedThen::GetValue(::vl::Ptr<::test::ObservableValue> value, ::vl::vint32_t increase, ::vl::vint32_t defaultValue)
	{
		return [&](){ try{ return (::vl::__vwsn::This(value)->GetValue() + increase); } catch(...){ return defaultValue; } }();
	}

	::vl::WString FailedThen::main()
	{
		return (((::vl::WString(L"", false) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl_workflow_global::FailedThen::Instance().GetValue(nullptr, 10, (- 1)), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl_workflow_global::FailedThen::Instance().GetValue(::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue(100)), 10, (- 1)), __vwsn_temp__); return __vwsn_temp__; }());
	}

	FailedThen& FailedThen::Instance()
	{
		return Getvl_workflow_global_FailedThen().instance;
	}
}

