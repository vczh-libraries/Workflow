/***********************************************************************
Generated from ../Resources/Codegen/ForEach.txt
***********************************************************************/

#include "ForEach.h"

#define GLOBAL_SYMBOL ::vl_workflow_global::ForEach::
#define GLOBAL_NAME ::vl_workflow_global::ForEach::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::ForEach::Instance()

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
		auto s = ::vl::WString(L"", false);
		using __vwsnt_0 = ::vl::vint32_t;
		FOREACH(__vwsnt_0, x, ::vl::reflection::description::GetLazyList<::vl::vint32_t>(xs))
		{
			if ((s == ::vl::WString(L"", false)))
			{
				(s = (s + ::vl::__vwsn::ToString(x)));
			}
			else
			{
				(s = ((s + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(x)));
			}
		}
		return s;
	}

	::vl::WString ForEach::Show2(::vl::Ptr<::vl::reflection::description::IValueList> xs)
	{
		auto s = ::vl::WString(L"", false);
		using __vwsnt_0 = ::vl::vint32_t;
		FOREACH(__vwsnt_0, x, ::vl::reflection::description::GetLazyList<::vl::vint32_t>(xs).Reverse())
		{
			if ((s == ::vl::WString(L"", false)))
			{
				(s = (s + ::vl::__vwsn::ToString(x)));
			}
			else
			{
				(s = ((s + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(x)));
			}
		}
		return s;
	}

	::vl::WString ForEach::main()
	{
		auto xs = (::vl::__vwsn::CreateList().Add(1).Add(2).Add(3).Add(4).Add(5)).list;
		return ((((::vl::WString(L"[", false) + GLOBAL_NAME Show1(xs)) + ::vl::WString(L"][", false)) + GLOBAL_NAME Show2(xs)) + ::vl::WString(L"]", false));
	}

	ForEach& ForEach::Instance()
	{
		return Getvl_workflow_global_ForEach().instance;
	}
}


#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ
