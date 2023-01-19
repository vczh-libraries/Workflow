/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/ForEach.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "ForEach.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#endif

#define GLOBAL_SYMBOL ::vl_workflow_global::ForEach::
#define GLOBAL_NAME ::vl_workflow_global::ForEach::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::ForEach::Instance()

/***********************************************************************
Global Variables
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_ForEach)
	vl_workflow_global::ForEach instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_ForEach)

namespace vl_workflow_global
{
/***********************************************************************
Global Functions
***********************************************************************/

	::vl::WString ForEach::Show1(::vl::Ptr<::vl::reflection::description::IValueList> xs)
	{
		auto s = ::vl::WString::Unmanaged(L"");
		{
			auto __vwsn_for_enumerable_x = ::vl::Ptr<::vl::reflection::description::IValueEnumerable>(xs);
			auto __vwsn_for_enumerator_x = ::vl::__vwsn::This(__vwsn_for_enumerable_x.Obj())->CreateEnumerator();
			while (::vl::__vwsn::This(__vwsn_for_enumerator_x.Obj())->Next())
			{
				auto x = ::vl::__vwsn::Unbox<::vl::vint64_t>(::vl::__vwsn::This(__vwsn_for_enumerator_x.Obj())->GetCurrent());
				{
					if ((s == ::vl::WString::Unmanaged(L"")))
					{
						(s = (s + ::vl::__vwsn::ToString(x)));
					}
					else
					{
						(s = ((s + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::ToString(x)));
					}
				}
			}
		}
		return s;
	}

	::vl::WString ForEach::Show2(::vl::Ptr<::vl::reflection::description::IValueList> xs)
	{
		auto s = ::vl::WString::Unmanaged(L"");
		{
			auto __vwsn_for_enumerable_x = ::vl::reflection::description::Sys::ReverseEnumerable(::vl::Ptr<::vl::reflection::description::IValueEnumerable>(xs));
			auto __vwsn_for_enumerator_x = ::vl::__vwsn::This(__vwsn_for_enumerable_x.Obj())->CreateEnumerator();
			while (::vl::__vwsn::This(__vwsn_for_enumerator_x.Obj())->Next())
			{
				auto x = ::vl::__vwsn::Unbox<::vl::vint64_t>(::vl::__vwsn::This(__vwsn_for_enumerator_x.Obj())->GetCurrent());
				{
					if ((s == ::vl::WString::Unmanaged(L"")))
					{
						(s = (s + ::vl::__vwsn::ToString(x)));
					}
					else
					{
						(s = ((s + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::ToString(x)));
					}
				}
			}
		}
		return s;
	}

	::vl::WString ForEach::main()
	{
		auto xs = (::vl::__vwsn::CreateList().Add(static_cast<::vl::vint64_t>(1L)).Add(static_cast<::vl::vint64_t>(2L)).Add(static_cast<::vl::vint64_t>(3L)).Add(static_cast<::vl::vint64_t>(4L)).Add(static_cast<::vl::vint64_t>(5L))).list;
		return ((((::vl::WString::Unmanaged(L"[") + GLOBAL_NAME Show1(xs)) + ::vl::WString::Unmanaged(L"][")) + GLOBAL_NAME Show2(xs)) + ::vl::WString::Unmanaged(L"]"));
	}

	ForEach& ForEach::Instance()
	{
		return Getvl_workflow_global_ForEach().instance;
	}
}

#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
