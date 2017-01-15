/***********************************************************************
Generated from ../Resources/Codegen/TypeTesting.txt
***********************************************************************/

#include "TypeTesting.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_TypeTesting)
	vl_workflow_global::TypeTesting instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_TypeTesting)

namespace vl_workflow_global
{
	::vl::WString TypeTesting::main()
	{
		::vl::collections::LazyList<::vl::vint32_t> a = ::vl::reflection::description::GetLazyList<::vl::vint32_t>([&](){ auto __vwsn_temp__ = ::vl::reflection::description::IValueList::Create(); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 1; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 2; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 3; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 4; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 5; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); return __vwsn_temp__; }());
		return (((((([&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize(/* NOT SUPPORTS: testing against enumerable type: a is (int[]) */ __vwsn_not_exists__, __vwsn_temp__); return __vwsn_temp__; }() + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize(/* NOT SUPPORTS: testing against enumerable type: a is (int[string]) */ __vwsn_not_exists__, __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize(/* NOT SUPPORTS: testing against enumerable type: a is (List^) */ __vwsn_not_exists__, __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize(/* NOT SUPPORTS: testing against enumerable type: a is (List*) */ __vwsn_not_exists__, __vwsn_temp__); return __vwsn_temp__; }());
	}

	TypeTesting& TypeTesting::Instance()
	{
		return Getvl_workflow_global_TypeTesting().instance;
	}
}

