/***********************************************************************
Generated from ../Resources/Codegen/BindComplex.txt
***********************************************************************/

#include "BindComplex.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif
#define GLOBAL_SYMBOL ::vl_workflow_global::BindComplex::
#define GLOBAL_NAME ::vl_workflow_global::BindComplex::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::BindComplex::Instance()

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_BindComplex)
	vl_workflow_global::BindComplex instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString(L"", false);
	FINALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString::Empty;
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_BindComplex)

namespace vl_workflow_global
{
	class __vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription : public ::vl::Object, public virtual ::vl::reflection::description::IValueSubscription
	{
	public:
		::vl::Ptr<::test::ObservableValue> x;
		::vl::Ptr<::test::ObservableValue> y;
		::vl::Ptr<::test::ObservableValue> z;

		__vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription(::vl::Ptr<::test::ObservableValue> __vwsnctor_x, ::vl::Ptr<::test::ObservableValue> __vwsnctor_y, ::vl::Ptr<::test::ObservableValue> __vwsnctor_z);

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
		void __vwsn_bind_callback_1_0(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1);
		void __vwsn_bind_callback_2_0(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1);
		void __vwsn_bind_initialize_();
		::vl::Ptr<::vl::reflection::description::IValueListener> Subscribe(const ::vl::Func<void(::vl::reflection::description::Value)>& __vwsn_bind_callback_) override;
		bool Update() override;
		bool Close() override;
	};

	class __vwsnc2_BindComplex_main_Subscribe__vl_reflection_description_IValueListener : public ::vl::Object, public virtual ::vl::reflection::description::IValueListener
	{
	public:
		::vl::Ptr<::vl::reflection::description::IValueDictionary> __vwsn_bind_listeners_;
		::vl::reflection::description::IValueSubscription* __vwsn_subscription_;
		::vl::reflection::description::IValueSubscription* __vwsnthis_0;

		__vwsnc2_BindComplex_main_Subscribe__vl_reflection_description_IValueListener(::vl::Ptr<::vl::reflection::description::IValueDictionary> __vwsnctor___vwsn_bind_listeners_, ::vl::reflection::description::IValueSubscription* __vwsnctor___vwsn_subscription_, ::vl::reflection::description::IValueSubscription* __vwsnctorthis_0);

		::vl::reflection::description::IValueSubscription* GetSubscription() override;
		bool GetStopped() override;
		bool StopListening() override;
	};

	void BindComplex::Callback(::vl::reflection::description::Value value)
	{
		(GLOBAL_NAME s = ((((::vl::WString(L"", false) + GLOBAL_NAME s) + ::vl::WString(L"[", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint32_t>(value))) + ::vl::WString(L"]", false)));
	}

	::vl::WString BindComplex::main()
	{
		auto x = ::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue());
		auto y = ::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue());
		auto z = ::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue());
		auto subscription = ::vl::Ptr<::vl::reflection::description::IValueSubscription>(new ::vl_workflow_global::__vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription(x, y, z));
		::vl::__vwsn::This(subscription.Obj())->Subscribe(::vl::Func<void(::vl::reflection::description::Value)>(GLOBAL_OBJ, &GLOBAL_SYMBOL Callback));
		::vl::__vwsn::This(x.Obj())->SetValue(10);
		::vl::__vwsn::This(y.Obj())->SetValue(20);
		::vl::__vwsn::This(z.Obj())->SetValue(30);
		::vl::__vwsn::This(subscription.Obj())->Close();
		return GLOBAL_NAME s;
	}

	BindComplex& BindComplex::Instance()
	{
		return Getvl_workflow_global_BindComplex().instance;
	}

	__vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::__vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription(::vl::Ptr<::test::ObservableValue> __vwsnctor_x, ::vl::Ptr<::test::ObservableValue> __vwsnctor_y, ::vl::Ptr<::test::ObservableValue> __vwsnctor_z)
		:x(__vwsnctor_x)
		, y(__vwsnctor_y)
		, z(__vwsnctor_z)
	{
		this->__vwsn_bind_cache_0 = ::vl::Ptr<::test::ObservableValue>();
		this->__vwsn_bind_handler_0_0 = ::vl::Ptr<::vl::reflection::description::IEventHandler>();
		this->__vwsn_bind_cache_1 = ::vl::Ptr<::test::ObservableValue>();
		this->__vwsn_bind_handler_1_0 = ::vl::Ptr<::vl::reflection::description::IEventHandler>();
		this->__vwsn_bind_cache_2 = ::vl::Ptr<::test::ObservableValue>();
		this->__vwsn_bind_handler_2_0 = ::vl::Ptr<::vl::reflection::description::IEventHandler>();
		this->__vwsn_bind_opened_ = false;
		this->__vwsn_bind_closed_ = false;
		this->__vwsn_bind_listeners_ = ::vl::reflection::description::IValueDictionary::Create();
	}

	void __vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::__vwsn_bind_activator_()
	{
		auto __vwsn_bind_activator_result_ = [&](auto _x){ return (::vl::__vwsn::This(_x.Obj())->GetValue() + [&](auto _y){ return (::vl::__vwsn::This(_y.Obj())->GetValue() + [&](auto _z){ return ::vl::__vwsn::This(_z.Obj())->GetValue(); }(__vwsn_bind_cache_2)); }(__vwsn_bind_cache_1)); }(__vwsn_bind_cache_0);
		using __vwsnt_0 = ::vl::reflection::description::Value;
		FOREACH(__vwsnt_0, __vwsn_bind_callback_, ::vl::reflection::description::GetLazyList<::vl::reflection::description::Value>(::vl::__vwsn::This(__vwsn_bind_listeners_.Obj())->GetValues()))
		{
			::vl::__vwsn::Unbox<::vl::Func<void(::vl::reflection::description::Value)>>(__vwsn_bind_callback_)(::vl::__vwsn::Box(__vwsn_bind_activator_result_));
		}
	}

	void __vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_0_0(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1)
	{
		::vl::__vwsn::EventDetach(::vl::__vwsn::This(__vwsn_bind_cache_2.Obj())->ValueChanged, __vwsn_bind_handler_2_0);
		::vl::__vwsn::EventDetach(::vl::__vwsn::This(__vwsn_bind_cache_1.Obj())->ValueChanged, __vwsn_bind_handler_1_0);
		(__vwsn_bind_cache_1 = ::vl::__vwsn::This(this)->y);
		(__vwsn_bind_cache_2 = ::vl::__vwsn::This(this)->z);
		(__vwsn_bind_handler_1_0 = ::vl::__vwsn::EventAttach(::vl::__vwsn::This(__vwsn_bind_cache_1.Obj())->ValueChanged, ::vl::Func<void(::vl::vint32_t, ::vl::vint32_t)>(::vl::__vwsn::This(this), &__vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_1_0)));
		(__vwsn_bind_handler_2_0 = ::vl::__vwsn::EventAttach(::vl::__vwsn::This(__vwsn_bind_cache_2.Obj())->ValueChanged, ::vl::Func<void(::vl::vint32_t, ::vl::vint32_t)>(::vl::__vwsn::This(this), &__vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_2_0)));
		::vl::__vwsn::This(this)->__vwsn_bind_activator_();
	}

	void __vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_1_0(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1)
	{
		::vl::__vwsn::EventDetach(::vl::__vwsn::This(__vwsn_bind_cache_2.Obj())->ValueChanged, __vwsn_bind_handler_2_0);
		(__vwsn_bind_cache_2 = ::vl::__vwsn::This(this)->z);
		(__vwsn_bind_handler_2_0 = ::vl::__vwsn::EventAttach(::vl::__vwsn::This(__vwsn_bind_cache_2.Obj())->ValueChanged, ::vl::Func<void(::vl::vint32_t, ::vl::vint32_t)>(::vl::__vwsn::This(this), &__vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_2_0)));
		::vl::__vwsn::This(this)->__vwsn_bind_activator_();
	}

	void __vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_2_0(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1)
	{
		::vl::__vwsn::This(this)->__vwsn_bind_activator_();
	}

	void __vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::__vwsn_bind_initialize_()
	{
		(__vwsn_bind_cache_0 = ::vl::__vwsn::This(this)->x);
		(__vwsn_bind_cache_1 = ::vl::__vwsn::This(this)->y);
		(__vwsn_bind_cache_2 = ::vl::__vwsn::This(this)->z);
		(__vwsn_bind_handler_0_0 = ::vl::__vwsn::EventAttach(::vl::__vwsn::This(__vwsn_bind_cache_0.Obj())->ValueChanged, ::vl::Func<void(::vl::vint32_t, ::vl::vint32_t)>(::vl::__vwsn::This(this), &__vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_0_0)));
		(__vwsn_bind_handler_1_0 = ::vl::__vwsn::EventAttach(::vl::__vwsn::This(__vwsn_bind_cache_1.Obj())->ValueChanged, ::vl::Func<void(::vl::vint32_t, ::vl::vint32_t)>(::vl::__vwsn::This(this), &__vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_1_0)));
		(__vwsn_bind_handler_2_0 = ::vl::__vwsn::EventAttach(::vl::__vwsn::This(__vwsn_bind_cache_2.Obj())->ValueChanged, ::vl::Func<void(::vl::vint32_t, ::vl::vint32_t)>(::vl::__vwsn::This(this), &__vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_2_0)));
	}

	::vl::Ptr<::vl::reflection::description::IValueListener> __vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::Subscribe(const ::vl::Func<void(::vl::reflection::description::Value)>& __vwsn_bind_callback_)
	{
		if ((! __vwsn_bind_opened_))
		{
			(__vwsn_bind_opened_ = true);
			::vl::__vwsn::This(this)->__vwsn_bind_initialize_();
		}
		auto __vwsn_subscription_ = this;
		auto __vwsn_listener_shared_ = ::vl::Ptr<::vl::reflection::description::IValueListener>(new ::vl_workflow_global::__vwsnc2_BindComplex_main_Subscribe__vl_reflection_description_IValueListener(__vwsn_bind_listeners_, __vwsn_subscription_, this));
		::vl::__vwsn::This(__vwsn_bind_listeners_.Obj())->Set(::vl::__vwsn::Box(__vwsn_listener_shared_), ::vl::__vwsn::Box(__vwsn_bind_callback_));
		return __vwsn_listener_shared_;
	}

	bool __vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::Update()
	{
		if ((! __vwsn_bind_closed_))
		{
			::vl::__vwsn::This(this)->__vwsn_bind_activator_();
			return true;
		}
		return false;
	}

	bool __vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::Close()
	{
		if ((! __vwsn_bind_closed_))
		{
			(__vwsn_bind_closed_ = true);
			::vl::__vwsn::EventDetach(::vl::__vwsn::This(__vwsn_bind_cache_0.Obj())->ValueChanged, __vwsn_bind_handler_0_0);
			::vl::__vwsn::EventDetach(::vl::__vwsn::This(__vwsn_bind_cache_1.Obj())->ValueChanged, __vwsn_bind_handler_1_0);
			::vl::__vwsn::EventDetach(::vl::__vwsn::This(__vwsn_bind_cache_2.Obj())->ValueChanged, __vwsn_bind_handler_2_0);
			(__vwsn_bind_cache_0 = ::vl::Ptr<::test::ObservableValue>());
			(__vwsn_bind_cache_1 = ::vl::Ptr<::test::ObservableValue>());
			(__vwsn_bind_cache_2 = ::vl::Ptr<::test::ObservableValue>());
			(__vwsn_bind_handler_0_0 = ::vl::Ptr<::vl::reflection::description::IEventHandler>());
			(__vwsn_bind_handler_1_0 = ::vl::Ptr<::vl::reflection::description::IEventHandler>());
			(__vwsn_bind_handler_2_0 = ::vl::Ptr<::vl::reflection::description::IEventHandler>());
			::vl::__vwsn::This(__vwsn_bind_listeners_.Obj())->Clear();
			return true;
		}
		return false;
	}

	__vwsnc2_BindComplex_main_Subscribe__vl_reflection_description_IValueListener::__vwsnc2_BindComplex_main_Subscribe__vl_reflection_description_IValueListener(::vl::Ptr<::vl::reflection::description::IValueDictionary> __vwsnctor___vwsn_bind_listeners_, ::vl::reflection::description::IValueSubscription* __vwsnctor___vwsn_subscription_, ::vl::reflection::description::IValueSubscription* __vwsnctorthis_0)
		:__vwsn_bind_listeners_(__vwsnctor___vwsn_bind_listeners_)
		, __vwsn_subscription_(__vwsnctor___vwsn_subscription_)
		, __vwsnthis_0(__vwsnctorthis_0)
	{
	}

	::vl::reflection::description::IValueSubscription* __vwsnc2_BindComplex_main_Subscribe__vl_reflection_description_IValueListener::GetSubscription()
	{
		return ::vl::__vwsn::This(this)->__vwsn_subscription_;
	}

	bool __vwsnc2_BindComplex_main_Subscribe__vl_reflection_description_IValueListener::GetStopped()
	{
		return (! ::vl::__vwsn::This(::vl::__vwsn::This(::vl::__vwsn::This(this)->__vwsn_bind_listeners_.Obj())->GetKeys().Obj())->Contains(::vl::__vwsn::Box(this)));
	}

	bool __vwsnc2_BindComplex_main_Subscribe__vl_reflection_description_IValueListener::StopListening()
	{
		if (::vl::__vwsn::This(::vl::__vwsn::This(::vl::__vwsn::This(this)->__vwsn_bind_listeners_.Obj())->GetKeys().Obj())->Contains(::vl::__vwsn::Box(this)))
		{
			::vl::__vwsn::This(::vl::__vwsn::This(this)->__vwsn_bind_listeners_.Obj())->Remove(::vl::__vwsn::Box(this));
			return true;
		}
		return false;
	}

}

#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif
