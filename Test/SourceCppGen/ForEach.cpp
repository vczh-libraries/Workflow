/***********************************************************************
Generated from ../Resources/Codegen/ForEach.txt
***********************************************************************/

#include "ForEach.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_ForEach)
	vl_workflow_global::ForEach instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_ForEach)

namespace vl_workflow_global
{
	::vl::WString ForEach::Show1(::vl::Ptr<::vl::reflection::description::IValueList> xs)
	{
				::vl::WString s = ::vl::WString(L"", false);
				using __vwsnt_0 = ::vl::vint32_t;
				FOREACH(__vwsnt_0, x,  ::vl::reflection::description::GetLazyList<::vl::vint32_t<(xs));
				{
																		if ((s == ::vl::WString(L"", false)))
																		{
																																																																										(s = (s + static_cast<::vl::vint32_t>(x)));
																		}
																		else
																		{
																																																																										(s = ((s + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(x)));
																		}
				}
				return s;
	}

	::vl::WString ForEach::Show2(::vl::Ptr<::vl::reflection::description::IValueList> xs)
	{
				::vl::WString s = ::vl::WString(L"", false);
				using __vwsnt_0 = ::vl::vint32_t;
				FOREACH(__vwsnt_0, x,  ::vl::reflection::description::GetLazyList<::vl::vint32_t<(xs).Reverse());
				{
																		if ((s == ::vl::WString(L"", false)))
																		{
																																																																										(s = (s + static_cast<::vl::vint32_t>(x)));
																		}
																		else
																		{
																																																																										(s = ((s + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(x)));
																		}
				}
				return s;
	}

	::vl::WString ForEach::main()
	{
				::vl::Ptr<::vl::reflection::description::IValueList> xs = [&](){ auto __vwsn_temp__ = ::vl::reflection::description::IValueList::Create(); __vwsn_temp__->Add(::vl::reflection::description::BoxParameter<::vl::vint32_t>(1)); __vwsn_temp__->Add(::vl::reflection::description::BoxParameter<::vl::vint32_t>(2)); __vwsn_temp__->Add(::vl::reflection::description::BoxParameter<::vl::vint32_t>(3)); __vwsn_temp__->Add(::vl::reflection::description::BoxParameter<::vl::vint32_t>(4)); __vwsn_temp__->Add(::vl::reflection::description::BoxParameter<::vl::vint32_t>(5)); return __vwsn_temp__; }();
				return ((((::vl::WString(L"[", false) + ::vl_workflow_global::ForEach::Instance().Show1(xs)) + ::vl::WString(L"][", false)) + ::vl_workflow_global::ForEach::Instance().Show2(xs)) + ::vl::WString(L"]", false));
	}

	ForEach& ForEach::Instance()
	{
		return Getvl_workflow_global_ForEach().instance;
	}
}

