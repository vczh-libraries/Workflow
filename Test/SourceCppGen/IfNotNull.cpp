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
		::vl::reflection::description::Value x = ::vl::__vwsn::Box([&](){ auto __vwsn_temp__ = ::vl::reflection::description::IValueList::Create(); __vwsn_temp__->Add(::vl::__vwsn::Box(1)); return __vwsn_temp__; }());
		::vl::reflection::description::Value y = ::vl::__vwsn::Box([&](){ auto __vwsn_temp__ = ::vl::reflection::description::IValueDictionary::Create(); __vwsn_temp__->Set(::vl::__vwsn::Box(1), ::vl::__vwsn::Box(2)); return __vwsn_temp__; }());
		::vl::WString s = ::vl::WString(L"", false);
		if (::vl::__vwsn::Unbox<bool>(x))
		{
			(s = (s + ::vl::WString(L"{1} is a list", false)));
		}
		else
		{
			(s = (s + ::vl::WString(L"{1} is not a list", false)));
		}
		(s = (s + ::vl::WString(L", ", false)));
		if (::vl::__vwsn::Unbox<bool>(y))
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

