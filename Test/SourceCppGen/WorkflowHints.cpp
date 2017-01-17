/***********************************************************************
Generated from ../Resources/Codegen/WorkflowHints.txt
***********************************************************************/

#include "WorkflowHints.h"

#define GLOBAL_SYMBOL ::vl_workflow_global::WorkflowHints::
#define GLOBAL_NAME ::vl_workflow_global::WorkflowHints::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::WorkflowHints::Instance()

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_WorkflowHints)
	vl_workflow_global::WorkflowHints instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_WorkflowHints)

namespace vl_workflow_global
{
	::vl::WString WorkflowHints::main()
	{
		auto hinters = ::vl::Ptr<::test::Hinters>(::test::CreateHinter());
		{
			auto hinters2 = ::test::CreateHinter(0);
			::vl::__vwsn::This(hinters2)->Dispose(true);
		}
		auto a = [&]()->decltype(auto){ auto __vwsn_temp_x0 = ::vl::__vwsn::Box((::vl::__vwsn::CreateList().Add(1).Add(2).Add(3)).list); ::vl::collections::List<::vl::vint32_t> __vwsn_temp_0; ::vl::reflection::description::UnboxParameter(__vwsn_temp_x0, __vwsn_temp_0); return ::vl::__vwsn::This(hinters.Obj())->GetList(__vwsn_temp_0); }()[0];
		auto b = [&]()->decltype(auto){ auto __vwsn_temp_x0 = ::vl::__vwsn::Box(::vl::__vwsn::Ensure(::vl::Ptr<::vl::reflection::description::IValueReadonlyList>((::vl::__vwsn::CreateList().Add(10).Add(20).Add(30)).list))); ::vl::collections::List<::vl::vint32_t> __vwsn_temp_0; ::vl::reflection::description::UnboxParameter(__vwsn_temp_x0, __vwsn_temp_0); return ::vl::__vwsn::This(hinters.Obj())->GetReadonlyList(__vwsn_temp_0); }()[0];
		auto c = ::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This([&]()->decltype(auto){ auto __vwsn_temp_x0 = ::vl::__vwsn::Box(::vl::__vwsn::Ensure(::vl::Ptr<::vl::reflection::description::IValueReadonlyList>((::vl::__vwsn::CreateList().Add(100).Add(200).Add(300)).list))); ::vl::collections::Array<::vl::vint32_t> __vwsn_temp_0; ::vl::reflection::description::UnboxParameter(__vwsn_temp_x0, __vwsn_temp_0); return [&](){ decltype(auto) __vwsn_temp__ = ::vl::__vwsn::This(hinters.Obj())->GetReadonlyArray(__vwsn_temp_0); return ::vl::__vwsn::Unbox<::vl::Ptr<::vl::reflection::description::IValueReadonlyList>>(::vl::reflection::description::BoxParameter<decltype(__vwsn_temp__)>(__vwsn_temp__)); }(); }().Obj())->Get(0));
		auto d = ::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This([&]()->decltype(auto){ auto __vwsn_temp_x0 = ::vl::__vwsn::Box(::vl::__vwsn::Ensure(::vl::Ptr<::vl::reflection::description::IValueReadonlyList>((::vl::__vwsn::CreateList().Add(1000).Add(2000).Add(3000)).list))); ::vl::collections::SortedList<::vl::vint32_t> __vwsn_temp_0; ::vl::reflection::description::UnboxParameter(__vwsn_temp_x0, __vwsn_temp_0); return [&](){ decltype(auto) __vwsn_temp__ = ::vl::__vwsn::This(hinters.Obj())->GetReadonlySL(__vwsn_temp_0); return ::vl::__vwsn::Unbox<::vl::Ptr<::vl::reflection::description::IValueReadonlyList>>(::vl::reflection::description::BoxParameter<decltype(__vwsn_temp__)>(__vwsn_temp__)); }(); }().Obj())->Get(0));
		auto e = [&]()->decltype(auto){ auto __vwsn_temp_x0 = ::vl::__vwsn::Box((::vl::__vwsn::CreateDictionary().Add(1, 2).Add(3, 4)).dictionary); ::vl::collections::Dictionary<::vl::vint32_t, ::vl::vint32_t> __vwsn_temp_0; ::vl::reflection::description::UnboxParameter(__vwsn_temp_x0, __vwsn_temp_0); return ::test::GetDictionaryFromHinter(::vl::__vwsn::This(hinters.Obj()), __vwsn_temp_0); }()[1];
		auto f = [&]()->decltype(auto){ auto __vwsn_temp_x0 = ::vl::__vwsn::Box(::vl::__vwsn::Ensure(::vl::Ptr<::vl::reflection::description::IValueReadonlyDictionary>((::vl::__vwsn::CreateDictionary().Add(10, 20).Add(30, 40)).dictionary))); ::vl::collections::Dictionary<::vl::vint32_t, ::vl::vint32_t> __vwsn_temp_0; ::vl::reflection::description::UnboxParameter(__vwsn_temp_x0, __vwsn_temp_0); return ::test::GetReadonlyDictionaryFromHinter(::vl::__vwsn::This(hinters.Obj()), __vwsn_temp_0); }()[10];
		[&]()->decltype(auto){ auto __vwsn_temp_x0 = ::vl::__vwsn::Box((::vl::__vwsn::CreateList().Add(1).Add(2).Add(3).Add(4).Add(5)).list); ::vl::collections::List<::vl::vint32_t> __vwsn_temp_0; ::vl::reflection::description::UnboxParameter(__vwsn_temp_x0, __vwsn_temp_0); return ::vl::__vwsn::This(hinters.Obj())->SetStorage(__vwsn_temp_0); }();
		auto g1 = ::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This([&](){ decltype(auto) __vwsn_temp__ = ::vl::__vwsn::This(hinters.Obj())->GetStorage(); return ::vl::__vwsn::Unbox<::vl::Ptr<::vl::reflection::description::IValueList>>(::vl::reflection::description::BoxParameter<decltype(__vwsn_temp__)>(__vwsn_temp__)); }().Obj())->Get(0));
		auto xs = (::vl::__vwsn::CreateList().Add(1).Add(2).Add(3).Add(4).Add(5)).list;
		[&]()->decltype(auto){ auto __vwsn_temp_x0 = ::vl::__vwsn::Box(xs); ::vl::collections::List<::vl::vint32_t> __vwsn_temp_0; ::vl::reflection::description::UnboxParameter(__vwsn_temp_x0, __vwsn_temp_0); return ::vl::__vwsn::This(hinters.Obj())->SetStorage(__vwsn_temp_0); }();
		::vl::__vwsn::This(hinters.Obj())->GetStorage().Set(4, 100);
		auto g2 = ::vl::__vwsn::This(hinters.Obj())->GetStorage()[4];
		return ((((((((((((((((::vl::WString(L"[", false) + ::vl::__vwsn::ToString(a)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(b)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(c)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(d)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(e)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(f)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(g1)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(g2)) + ::vl::WString(L"]", false));
	}

	WorkflowHints& WorkflowHints::Instance()
	{
		return Getvl_workflow_global_WorkflowHints().instance;
	}
}

#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ
