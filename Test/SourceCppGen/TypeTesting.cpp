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
		::vl::collections::LazyList<::vl::vint32_t> a = [&](){ auto __vwsn_temp__ = ::vl::reflection::description::IValueList::Create(); __vwsn_temp__->Add(::vl::reflection::description::BoxParameter<::vl::vint32_t>(1)); __vwsn_temp__->Add(::vl::reflection::description::BoxParameter<::vl::vint32_t>(2)); __vwsn_temp__->Add(::vl::reflection::description::BoxParameter<::vl::vint32_t>(3)); __vwsn_temp__->Add(::vl::reflection::description::BoxParameter<::vl::vint32_t>(4)); __vwsn_temp__->Add(::vl::reflection::description::BoxParameter<::vl::vint32_t>(5)); return __vwsn_temp__; }().Cast<::vl::reflection::description::IValueList>();
		return ((((((static_cast<bool>(static_cast<bool>(a.Cast<::vl::reflection::description::IValueList>())) + ::vl::WString(L", ", false)) + static_cast<bool>(static_cast<bool>(a.Cast<::vl::reflection::description::IValueDictionary>()))) + ::vl::WString(L", ", false)) + static_cast<bool>(static_cast<bool>(a.Cast<::vl::reflection::description::IValueList>()))) + ::vl::WString(L", ", false)) + static_cast<bool>(static_cast<bool>(a.Cast<::vl::reflection::description::IValueList>())));
	}

	TypeTesting& TypeTesting::Instance()
	{
		return Getvl_workflow_global_TypeTesting().instance;
	}
}

