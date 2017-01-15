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
		FOREACH(__vwsnt_0, x,  ::vl::reflection::description::GetLazyList<::vl::vint32_t<(xs))
		{
			if ((s == ::vl::WString(L"", false)))
			{
				(s = (s + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(x, __vwsn_temp__); return __vwsn_temp__; }()));
			}
			else
			{
				(s = ((s + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(x, __vwsn_temp__); return __vwsn_temp__; }()));
			}
		}
		return s;
	}

	::vl::WString ForEach::Show2(::vl::Ptr<::vl::reflection::description::IValueList> xs)
	{
		::vl::WString s = ::vl::WString(L"", false);
		using __vwsnt_0 = ::vl::vint32_t;
		FOREACH(__vwsnt_0, x,  ::vl::reflection::description::GetLazyList<::vl::vint32_t<(xs).Reverse())
		{
			if ((s == ::vl::WString(L"", false)))
			{
				(s = (s + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(x, __vwsn_temp__); return __vwsn_temp__; }()));
			}
			else
			{
				(s = ((s + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(x, __vwsn_temp__); return __vwsn_temp__; }()));
			}
		}
		return s;
	}

	::vl::WString ForEach::main()
	{
		::vl::Ptr<::vl::reflection::description::IValueList> xs = [&](){ auto __vwsn_temp__ = ::vl::reflection::description::IValueList::Create(); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 1; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 2; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 3; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 4; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 5; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); return __vwsn_temp__; }();
		return ((((::vl::WString(L"[", false) + ::vl_workflow_global::ForEach::Instance().Show1(xs)) + ::vl::WString(L"][", false)) + ::vl_workflow_global::ForEach::Instance().Show2(xs)) + ::vl::WString(L"]", false));
	}

	ForEach& ForEach::Instance()
	{
		return Getvl_workflow_global_ForEach().instance;
	}
}

