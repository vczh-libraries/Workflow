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
		auto a = ::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(::vl::__vwsn::This(hinters.Obj())->GetList((::vl::__vwsn::CreateList().Add(1).Add(2).Add(3)).list).Obj())->Get(0));
		auto b = ::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(::vl::__vwsn::This(hinters.Obj())->GetReadonlyList(::vl::__vwsn::Ensure(::vl::Ptr<::vl::reflection::description::IValueReadonlyList>((::vl::__vwsn::CreateList().Add(10).Add(20).Add(30)).list))).Obj())->Get(0));
		auto c = ::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(::vl::__vwsn::This(hinters.Obj())->GetReadonlyArray(::vl::__vwsn::Ensure(::vl::Ptr<::vl::reflection::description::IValueReadonlyList>((::vl::__vwsn::CreateList().Add(100).Add(200).Add(300)).list))).Obj())->Get(0));
		auto d = ::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(::vl::__vwsn::This(hinters.Obj())->GetReadonlySL(::vl::__vwsn::Ensure(::vl::Ptr<::vl::reflection::description::IValueReadonlyList>((::vl::__vwsn::CreateList().Add(1000).Add(2000).Add(3000)).list))).Obj())->Get(0));
		auto e = ::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(::test::GetDictionaryFromHinter(::vl::__vwsn::This(hinters.Obj()), (::vl::__vwsn::CreateDictionary().Add(1, 2).Add(3, 4)).dictionary).Obj())->Get(::vl::__vwsn::Box(1)));
		auto f = ::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(::test::GetReadonlyDictionaryFromHinter(::vl::__vwsn::This(hinters.Obj()), ::vl::__vwsn::Ensure(::vl::Ptr<::vl::reflection::description::IValueReadonlyDictionary>((::vl::__vwsn::CreateDictionary().Add(10, 20).Add(30, 40)).dictionary))).Obj())->Get(::vl::__vwsn::Box(10)));
		::vl::__vwsn::This(hinters.Obj())->SetStorage((::vl::__vwsn::CreateList().Add(1).Add(2).Add(3).Add(4).Add(5)).list);
		auto g = ::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(::vl::__vwsn::This(hinters.Obj())->GetStorage().Obj())->Get(3));
		return ((((((((((((((::vl::WString(L"[", false) + ::vl::__vwsn::ToString(a)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(b)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(c)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(d)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(e)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(f)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(g)) + ::vl::WString(L"]", false));
	}

	WorkflowHints& WorkflowHints::Instance()
	{
		return Getvl_workflow_global_WorkflowHints().instance;
	}
}


#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ
