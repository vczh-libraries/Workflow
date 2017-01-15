/***********************************************************************
Generated from ../Resources/Codegen/BindSimple.txt
***********************************************************************/

#include "BindSimple.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_BindSimple)
	vl_workflow_global::BindSimple instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString(L"", false);
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_BindSimple)

namespace vl_workflow_global
{

	class __vwsnc1_BindSimple_main__vl_reflection_description_IValueSubscription : public ::vl::Object, public virtual ::vl::reflection::description::IValueSubscription
	{
	public:
		::vl::Ptr<::test::ObservableValue> x;
		::vl::Ptr<::test::ObservableValue> y;
		::vl::Ptr<::test::ObservableValue> z;

		__vwsnc1_BindSimple_main__vl_reflection_description_IValueSubscription(::vl::Ptr<::test::ObservableValue> __vwsnctor_x, ::vl::Ptr<::test::ObservableValue> __vwsnctor_y, ::vl::Ptr<::test::ObservableValue> __vwsnctor_z);

		::vl::Ptr<::test::ObservableValue> __vwsn_bind_cache_0;
		::vl::Ptr<::vl::reflection::description::IEventHandler> __vwsn_bind_handler_0_0;
		::vl::Ptr<::test::ObservableValue> __vwsn_bind_cache_1;
		::vl::Ptr<::vl::reflection::description::IEventHandler> __vwsn_bind_handler_1_0;
		::vl::Ptr<::test::ObservableValue> __vwsn_bind_cache_2;
		::vl::Ptr<::vl::reflection::description::IEventHandler> __vwsn_bind_handler_2_0;
		bool __vwsn_bind_opened_ = false;
		bool __vwsn_bind_closed_ = false;
		::vl::Ptr<::vl::reflection::description::IValueDictionary> __vwsn_bind_listeners_;
		void __vwsn_bind_activator_();
		void __vwsn_bind_callback_0_0(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1);
		void __vwsn_bind_callback_2_0(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1);
		void __vwsn_bind_callback_1_0(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1);
		void __vwsn_bind_initialize_();
		::vl::Ptr<::vl::reflection::description::IValueListener> Subscribe(const ::vl::Func<void(::vl::reflection::description::Value)>& callback) override;
		bool Update() override;
		bool Close() override;
	};

	class __vwsnc2_BindSimple_main_Subscribe__vl_reflection_description_IValueListener : public ::vl::Object, public virtual ::vl::reflection::description::IValueListener
	{
	public:
		::vl::Ptr<::vl::reflection::description::IValueDictionary> __vwsn_bind_listeners_;
		::vl::reflection::description::IValueSubscription* __vwsn_subscription_;
		::vl::reflection::description::IValueSubscription* __vwsnthis_0;

		__vwsnc2_BindSimple_main_Subscribe__vl_reflection_description_IValueListener(::vl::Ptr<::vl::reflection::description::IValueDictionary> __vwsnctor___vwsn_bind_listeners_, ::vl::reflection::description::IValueSubscription* __vwsnctor___vwsn_subscription_, ::vl::reflection::description::IValueSubscription* __vwsnctorthis_0);

		::vl::reflection::description::IValueSubscription* GetSubscription() override;
		bool GetStopped() override;
		bool StopListening() override;
	};
	void BindSimple::Callback(::vl::reflection::description::Value value)
	{
		(::vl_workflow_global::BindSimple::Instance().s = ((((::vl::WString(L"", false) + ::vl_workflow_global::BindSimple::Instance().s) + ::vl::WString(L"[", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::reflection::description::UnboxValue<::vl::vint32_t>(value), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L"]", false)));
	}

	::vl::WString BindSimple::main()
	{
		::vl::Ptr<::test::ObservableValue> x = ::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue());
		::vl::Ptr<::test::ObservableValue> y = ::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue());
		::vl::Ptr<::test::ObservableValue> z = ::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue());
		::vl::Ptr<::vl::reflection::description::IValueSubscription> subscription = ::vl::Ptr<::vl::reflection::description::IValueSubscription>(new ::vl_workflow_global::__vwsnc1_BindSimple_main__vl_reflection_description_IValueSubscription(x, y, z));
		::vl::__vwsn::This(subscription)->Subscribe(::vl::Func<void(::vl::reflection::description::Value)>(&::vl_workflow_global::BindSimple::Instance(), &::vl_workflow_global::BindSimple::Callback));
		::vl::__vwsn::This(x)->SetValue(10);
		::vl::__vwsn::This(y)->SetValue(20);
		::vl::__vwsn::This(z)->SetValue(30);
		::vl::__vwsn::This(subscription)->Close();
		return ::vl_workflow_global::BindSimple::Instance().s;
	}

	BindSimple& BindSimple::Instance()
	{
		return Getvl_workflow_global_BindSimple().instance;
	}

	__vwsnc1_BindSimple_main__vl_reflection_description_IValueSubscription::__vwsnc1_BindSimple_main__vl_reflection_description_IValueSubscription(::vl::Ptr<::test::ObservableValue> __vwsnctor_x, ::vl::Ptr<::test::ObservableValue> __vwsnctor_y, ::vl::Ptr<::test::ObservableValue> __vwsnctor_z)
		:x(__vwsnctor_x)
		, y(__vwsnctor_y)
		, z(__vwsnctor_z)
	{
		this->__vwsn_bind_cache_0 = nullptr;
		this->__vwsn_bind_handler_0_0 = nullptr;
		this->__vwsn_bind_cache_1 = nullptr;
		this->__vwsn_bind_handler_1_0 = nullptr;
		this->__vwsn_bind_cache_2 = nullptr;
		this->__vwsn_bind_handler_2_0 = nullptr;
		this->__vwsn_bind_opened_ = false;
		this->__vwsn_bind_closed_ = false;
		this->__vwsn_bind_listeners_ = ::vl::reflection::description::IValueDictionary::Create();
	}

	void __vwsnc1_BindSimple_main__vl_reflection_description_IValueSubscription::__vwsn_bind_activator_()
	{
		::vl::vint32_t __vwsn_bind_activator_result_ = ((::vl::__vwsn::This(__vwsn_bind_cache_0)->GetValue() + ::vl::__vwsn::This(__vwsn_bind_cache_1)->GetValue()) + ::vl::__vwsn::This(__vwsn_bind_cache_2)->GetValue());
		using __vwsnt_0 = ::vl::reflection::description::Value;
		FOREACH(__vwsnt_0, __vwsn_bind_callback_, ::vl::reflection::description::GetLazyList<::vl::reflection::description::Value>(::vl::__vwsn::This(__vwsn_bind_listeners_.Obj())->GetValues()))
		{
			[&](){ ::vl::Func<void(::vl::reflection::description::Value)> __vwsn_temp__; ::vl::reflection::description::UnboxParameter<::vl::Func<void(::vl::reflection::description::Value)>>(__vwsn_bind_callback_, __vwsn_temp__); return __vwsn_temp__; }()(::vl::reflection::description::BoxValue<::vl::vint32_t>(__vwsn_bind_activator_result_));
		}
	}

	void __vwsnc1_BindSimple_main__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_0_0(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1)
	{
		::vl::__vwsn::This(this)->__vwsn_bind_activator_();
	}

	void __vwsnc1_BindSimple_main__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_2_0(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1)
	{
		::vl::__vwsn::This(this)->__vwsn_bind_activator_();
	}

	void __vwsnc1_BindSimple_main__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_1_0(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1)
	{
		::vl::__vwsn::This(this)->__vwsn_bind_activator_();
	}

	void __vwsnc1_BindSimple_main__vl_reflection_description_IValueSubscription::__vwsn_bind_initialize_()
	{
		(__vwsn_bind_cache_1 = ::vl::__vwsn::This(this)->y);
		(__vwsn_bind_cache_2 = ::vl::__vwsn::This(this)->z);
		(__vwsn_bind_cache_0 = ::vl::__vwsn::This(this)->x);
		(__vwsn_bind_handler_0_0 = ::vl::__vwsn::EventAttach(::vl::__vwsn::This(__vwsn_bind_cache_0)->ValueChanged, ::vl::Func<void(::vl::vint32_t, ::vl::vint32_t)>(::vl::__vwsn::This(this), &__vwsnc1_BindSimple_main__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_0_0)));
		(__vwsn_bind_handler_2_0 = ::vl::__vwsn::EventAttach(::vl::__vwsn::This(__vwsn_bind_cache_2)->ValueChanged, ::vl::Func<void(::vl::vint32_t, ::vl::vint32_t)>(::vl::__vwsn::This(this), &__vwsnc1_BindSimple_main__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_2_0)));
		(__vwsn_bind_handler_1_0 = ::vl::__vwsn::EventAttach(::vl::__vwsn::This(__vwsn_bind_cache_1)->ValueChanged, ::vl::Func<void(::vl::vint32_t, ::vl::vint32_t)>(::vl::__vwsn::This(this), &__vwsnc1_BindSimple_main__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_1_0)));
	}

	::vl::Ptr<::vl::reflection::description::IValueListener> __vwsnc1_BindSimple_main__vl_reflection_description_IValueSubscription::Subscribe(const ::vl::Func<void(::vl::reflection::description::Value)>& callback)
	{
		if ((! __vwsn_bind_opened_))
		{
			(__vwsn_bind_opened_ = true);
			::vl::__vwsn::This(this)->__vwsn_bind_initialize_();
		}
		::vl::reflection::description::IValueSubscription* __vwsn_subscription_ = this;
		::vl::Ptr<::vl::reflection::description::IValueListener> __vwsn_listener_shared_ = ::vl::Ptr<::vl::reflection::description::IValueListener>(new ::vl_workflow_global::__vwsnc2_BindSimple_main_Subscribe__vl_reflection_description_IValueListener(__vwsn_bind_listeners_, __vwsn_subscription_, this));
		::vl::__vwsn::This(__vwsn_bind_listeners_)->Set(::vl::reflection::description::BoxValue<::vl::Ptr<::vl::reflection::description::IValueListener>>(__vwsn_listener_shared_), [&](){ ::vl::Func<void(::vl::reflection::description::Value)> __vwsn_temp__ = __vwsn_bind_callback_; return ::vl::reflection::description::BoxParameter<::vl::Func<void(::vl::reflection::description::Value)>>(__vwsn_temp__); }());
		return __vwsn_listener_shared_;
	}

	bool __vwsnc1_BindSimple_main__vl_reflection_description_IValueSubscription::Update()
	{
		if ((! __vwsn_bind_closed_))
		{
			::vl::__vwsn::This(this)->__vwsn_bind_activator_();
			return true;
		}
		return false;
	}

	bool __vwsnc1_BindSimple_main__vl_reflection_description_IValueSubscription::Close()
	{
		if ((! __vwsn_bind_closed_))
		{
			(__vwsn_bind_closed_ = true);
			::vl::__vwsn::EventDetach(::vl::__vwsn::This(__vwsn_bind_cache_0)->ValueChanged, __vwsn_bind_handler_0_0);
			::vl::__vwsn::EventDetach(::vl::__vwsn::This(__vwsn_bind_cache_2)->ValueChanged, __vwsn_bind_handler_2_0);
			::vl::__vwsn::EventDetach(::vl::__vwsn::This(__vwsn_bind_cache_1)->ValueChanged, __vwsn_bind_handler_1_0);
			(__vwsn_bind_cache_0 = nullptr);
			(__vwsn_bind_cache_1 = nullptr);
			(__vwsn_bind_cache_2 = nullptr);
			(__vwsn_bind_handler_0_0 = nullptr);
			(__vwsn_bind_handler_1_0 = nullptr);
			(__vwsn_bind_handler_2_0 = nullptr);
			::vl::__vwsn::This(__vwsn_bind_listeners_)->Clear();
			return true;
		}
		return false;
	}

	__vwsnc2_BindSimple_main_Subscribe__vl_reflection_description_IValueListener::__vwsnc2_BindSimple_main_Subscribe__vl_reflection_description_IValueListener(::vl::Ptr<::vl::reflection::description::IValueDictionary> __vwsnctor___vwsn_bind_listeners_, ::vl::reflection::description::IValueSubscription* __vwsnctor___vwsn_subscription_, ::vl::reflection::description::IValueSubscription* __vwsnctorthis_0)
		:__vwsn_bind_listeners_(__vwsnctor___vwsn_bind_listeners_)
		, __vwsn_subscription_(__vwsnctor___vwsn_subscription_)
		, __vwsnthis_0(__vwsnctorthis_0)
	{
	}

	::vl::reflection::description::IValueSubscription* __vwsnc2_BindSimple_main_Subscribe__vl_reflection_description_IValueListener::GetSubscription()
	{
		return ::vl::__vwsn::This(this)->__vwsn_subscription_;
	}

	bool __vwsnc2_BindSimple_main_Subscribe__vl_reflection_description_IValueListener::GetStopped()
	{
		return (! ::vl::__vwsn::This(::vl::__vwsn::This(::vl::__vwsn::This(this)->__vwsn_bind_listeners_.Obj())->GetKeys())->Contains(::vl::reflection::description::BoxValue<::vl::reflection::description::IValueListener*>(this)));
	}

	bool __vwsnc2_BindSimple_main_Subscribe__vl_reflection_description_IValueListener::StopListening()
	{
		if (::vl::__vwsn::This(::vl::__vwsn::This(::vl::__vwsn::This(this)->__vwsn_bind_listeners_.Obj())->GetKeys())->Contains(::vl::reflection::description::BoxValue<::vl::reflection::description::IValueListener*>(this)))
		{
			::vl::__vwsn::This(::vl::__vwsn::This(this)->__vwsn_bind_listeners_)->Remove(::vl::reflection::description::BoxValue<::vl::reflection::description::IValueListener*>(this));
			return true;
		}
		return false;
	}

}

