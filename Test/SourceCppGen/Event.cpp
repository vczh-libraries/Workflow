/***********************************************************************
Generated from ../Resources/Codegen/Event.txt
***********************************************************************/

#include "Event.h"

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
		::vl::__vwsn::This(::vl_workflow_global::Event::Instance().olds.Obj())->Add(::vl::reflection::description::BoxValue<::vl::vint32_t>(oldValue));
		::vl::__vwsn::This(::vl_workflow_global::Event::Instance().news.Obj())->Add(::vl::reflection::description::BoxValue<::vl::vint32_t>(newValue));
	}

	::vl::WString Event::main()
	{
		::vl::Ptr<::test::ObservableValue> o = ::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue(10));
		::vl::Ptr<::vl::reflection::description::IEventHandler> handler = ::vl::__vwsn::EventAttach(::vl::__vwsn::This(o.Obj())->ValueChanged, ::vl::Func<void(::vl::vint32_t, ::vl::vint32_t)>(&::vl_workflow_global::Event::Instance(), &::vl_workflow_global::Event::EventHandler));
		::vl::__vwsn::This(o.Obj())->SetValue(20);
		::vl::__vwsn::This(o.Obj())->SetValue(30);
		bool b1 = ::vl::__vwsn::EventDetach(::vl::__vwsn::This(o.Obj())->ValueChanged, handler);
		::vl::__vwsn::This(o.Obj())->SetValue(40);
		bool b2 = ::vl::__vwsn::EventDetach(::vl::__vwsn::This(o.Obj())->ValueChanged, handler);
		return (((((((((((((([&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::__vwsn::This(::vl_workflow_global::Event::Instance().olds.Obj())->GetCount(), __vwsn_temp__); return __vwsn_temp__; }() + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize([&](){ ::vl::vint32_t __vwsn_temp__; ::vl::reflection::description::UnboxParameter<::vl::vint32_t>(::vl::__vwsn::This(::vl_workflow_global::Event::Instance().olds.Obj())->Get(0), __vwsn_temp__); return __vwsn_temp__; }(), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize([&](){ ::vl::vint32_t __vwsn_temp__; ::vl::reflection::description::UnboxParameter<::vl::vint32_t>(::vl::__vwsn::This(::vl_workflow_global::Event::Instance().olds.Obj())->Get(1), __vwsn_temp__); return __vwsn_temp__; }(), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::__vwsn::This(::vl_workflow_global::Event::Instance().news.Obj())->GetCount(), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize([&](){ ::vl::vint32_t __vwsn_temp__; ::vl::reflection::description::UnboxParameter<::vl::vint32_t>(::vl::__vwsn::This(::vl_workflow_global::Event::Instance().news.Obj())->Get(0), __vwsn_temp__); return __vwsn_temp__; }(), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize([&](){ ::vl::vint32_t __vwsn_temp__; ::vl::reflection::description::UnboxParameter<::vl::vint32_t>(::vl::__vwsn::This(::vl_workflow_global::Event::Instance().news.Obj())->Get(1), __vwsn_temp__); return __vwsn_temp__; }(), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize(b1, __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize(b2, __vwsn_temp__); return __vwsn_temp__; }());
	}

	Event& Event::Instance()
	{
		return Getvl_workflow_global_Event().instance;
	}
}

