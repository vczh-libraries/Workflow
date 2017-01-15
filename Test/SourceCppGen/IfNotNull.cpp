/***********************************************************************
Generated from ../Resources/Codegen/IfNotNull.txt
***********************************************************************/

#include "IfNotNull.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_IfNotNull)
	vl_workflow_global::IfNotNull instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_IfNotNull)

namespace vl_workflow_global
{
	::vl::WString IfNotNull::main()
	{
		::vl::reflection::description::Value x = ::vl::reflection::description::BoxValue<::vl::Ptr<::vl::reflection::description::IValueList>>([&](){ auto __vwsn_temp__ = ::vl::reflection::description::IValueList::Create(); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 1; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); return __vwsn_temp__; }());
		::vl::reflection::description::Value y = ::vl::reflection::description::BoxValue<::vl::Ptr<::vl::reflection::description::IValueDictionary>>([&](){ auto __vwsn_temp__ = ::vl::reflection::description::IValueDictionary::Create(); __vwsn_temp__->Set([&](){ ::vl::vint32_t __vwsn_temp__ = 1; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }(), [&](){ ::vl::vint32_t __vwsn_temp__ = 2; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); return __vwsn_temp__; }());
		::vl::WString s = ::vl::WString(L"", false);
		if (::vl::reflection::description::UnboxValue<bool>(x))
		{
			(s = (s + ::vl::WString(L"{1} is a list", false)));
		}
		else
		{
			(s = (s + ::vl::WString(L"{1} is not a list", false)));
		}
		(s = (s + ::vl::WString(L", ", false)));
		if (::vl::reflection::description::UnboxValue<bool>(y))
		{
			(s = (s + ::vl::WString(L"{1:2} is a list", false)));
		}
		else
		{
			(s = (s + ::vl::WString(L"{1:2} is not a list", false)));
		}
		return s;
	}

	IfNotNull& IfNotNull::Instance()
	{
		return Getvl_workflow_global_IfNotNull().instance;
	}
}

