/***********************************************************************
Generated from ../Resources/Codegen/Event.txt
***********************************************************************/

#include "Event.h"

#define GLOBAL_SYMBOL ::vl_workflow_global::Event::
#define GLOBAL_NAME ::vl_workflow_global::Event::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::Event::Instance()

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_Event)
	vl_workflow_global::Event instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.olds = ::vl::reflection::description::IValueList::Create();
		instance.news = ::vl::reflection::description::IValueList::Create();
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_Event)

namespace vl_workflow_global
{
	void Event::EventHandler(::vl::vint32_t oldValue, ::vl::vint32_t newValue)
	{
		::vl::__vwsn::This(GLOBAL_NAME olds.Obj())->Add(::vl::__vwsn::Box(oldValue));
		::vl::__vwsn::This(GLOBAL_NAME news.Obj())->Add(::vl::__vwsn::Box(newValue));
	}

	::vl::WString Event::main()
	{
		auto o = ::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue(10));
		auto handler = ::vl::__vwsn::EventAttach(::vl::__vwsn::This(o.Obj())->ValueChanged, ::vl::Func<void(::vl::vint32_t, ::vl::vint32_t)>(GLOBAL_OBJ, &GLOBAL_SYMBOL EventHandler));
		::vl::__vwsn::This(o.Obj())->SetValue(20);
		::vl::__vwsn::This(o.Obj())->SetValue(30);
		auto b1 = ::vl::__vwsn::EventDetach(::vl::__vwsn::This(o.Obj())->ValueChanged, handler);
		::vl::__vwsn::This(o.Obj())->SetValue(40);
		auto b2 = ::vl::__vwsn::EventDetach(::vl::__vwsn::This(o.Obj())->ValueChanged, handler);
		return ((((((((((((((::vl::__vwsn::ToString(::vl::__vwsn::This(GLOBAL_NAME olds.Obj())->GetCount()) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(GLOBAL_NAME olds.Obj())->Get(0)))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(GLOBAL_NAME olds.Obj())->Get(1)))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::This(GLOBAL_NAME news.Obj())->GetCount())) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(GLOBAL_NAME news.Obj())->Get(0)))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(GLOBAL_NAME news.Obj())->Get(1)))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(b1)) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(b2));
	}

	Event& Event::Instance()
	{
		return Getvl_workflow_global_Event().instance;
	}
}


#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ
