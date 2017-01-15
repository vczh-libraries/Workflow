/***********************************************************************
Generated from ../Resources/Codegen/BindCustomInterface.txt
***********************************************************************/

#include "BindCustomInterface.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_BindCustomInterface)
	vl_workflow_global::BindCustomInterface instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString(L"", false);
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_BindCustomInterface)

namespace vl_workflow_global
{
	struct __vwsnf1_BindCustomInterface_Bind_
	{

		__vwsnf1_BindCustomInterface_Bind_();

		void operator()(::vl::reflection::description::Value value) const;
	};

	class __vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription : public ::vl::Object, public virtual ::vl::reflection::description::IValueSubscription
	{
	public:
		::vl::Ptr<::ISummer> summer;

		__vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription(::vl::Ptr<::ISummer> __vwsnctor_summer);

		::vl::Ptr<::ISummer> __vwsn_bind_cache_0;
		::vl::Ptr<::vl::reflection::description::IEventHandler> __vwsn_bind_handler_0_0;
		bool __vwsn_bind_opened_ = false;
		bool __vwsn_bind_closed_ = false;
		::vl::Ptr<::vl::reflection::description::IValueDictionary> __vwsn_bind_listeners_;
		void __vwsn_bind_activator_();
		void __vwsn_bind_callback_0_0();
		void __vwsn_bind_initialize_();
		::vl::Ptr<::vl::reflection::description::IValueListener> Subscribe(const ::vl::Func<void(::vl::reflection::description::Value)>& __vwsn_bind_callback_) override;
		bool Update() override;
		bool Close() override;
	};

	class __vwsnc2_BindCustomInterface_Bind_Subscribe__vl_reflection_description_IValueListener : public ::vl::Object, public virtual ::vl::reflection::description::IValueListener
	{
	public:
		::vl::Ptr<::vl::reflection::description::IValueDictionary> __vwsn_bind_listeners_;
		::vl::reflection::description::IValueSubscription* __vwsn_subscription_;
		::vl::reflection::description::IValueSubscription* __vwsnthis_0;

		__vwsnc2_BindCustomInterface_Bind_Subscribe__vl_reflection_description_IValueListener(::vl::Ptr<::vl::reflection::description::IValueDictionary> __vwsnctor___vwsn_bind_listeners_, ::vl::reflection::description::IValueSubscription* __vwsnctor___vwsn_subscription_, ::vl::reflection::description::IValueSubscription* __vwsnctorthis_0);

		::vl::reflection::description::IValueSubscription* GetSubscription() override;
		bool GetStopped() override;
		bool StopListening() override;
	};

	class __vwsnc3_BindCustomInterface_main__IAdder : public ::vl::Object, public virtual ::IAdder
	{
	public:

		__vwsnc3_BindCustomInterface_main__IAdder();

		::vl::vint32_t sum = 0;
		::vl::vint32_t GetSum() override;
		void Add(::vl::vint32_t value) override;
	};

	::vl::Ptr<::vl::reflection::description::IValueSubscription> BindCustomInterface::Bind(::vl::Ptr<::ISummer> summer)
	{
		auto subscription = ::vl::Ptr<::vl::reflection::description::IValueSubscription>(new ::vl_workflow_global::__vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription(summer));
		::vl::__vwsn::This(subscription.Obj())->Subscribe(LAMBDA(::vl_workflow_global::__vwsnf1_BindCustomInterface_Bind_()));
		return subscription;
	}

	void BindCustomInterface::Execute(::vl::Ptr<::IAdder> adder)
	{
		auto subscription = ::vl_workflow_global::BindCustomInterface::Instance().Bind(adder.Cast<::ISummer>());
		using __vwsnt_0 = ::vl::vint32_t;
		FOREACH(__vwsnt_0, x, ::vl::__vwsn::Range(1, 5 + 1))
		{
			::vl::__vwsn::This(adder.Obj())->Add(x);
		}
		::vl::__vwsn::This(subscription.Obj())->Close();
	}

	::vl::WString BindCustomInterface::main()
	{
		::vl_workflow_global::BindCustomInterface::Instance().Execute(::vl::Ptr<::IAdder>(new ::vl_workflow_global::__vwsnc3_BindCustomInterface_main__IAdder()));
		return ::vl_workflow_global::BindCustomInterface::Instance().s;
	}

	BindCustomInterface& BindCustomInterface::Instance()
	{
		return Getvl_workflow_global_BindCustomInterface().instance;
	}

	__vwsnf1_BindCustomInterface_Bind_::__vwsnf1_BindCustomInterface_Bind_()
	{
	}

	void __vwsnf1_BindCustomInterface_Bind_::operator()(::vl::reflection::description::Value value) const
	{
		(::vl_workflow_global::BindCustomInterface::Instance().s = ((((::vl::WString(L"", false) + ::vl_workflow_global::BindCustomInterface::Instance().s) + ::vl::WString(L"[", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint32_t>(value))) + ::vl::WString(L"]", false)));
	}

	__vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription::__vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription(::vl::Ptr<::ISummer> __vwsnctor_summer)
		:summer(__vwsnctor_summer)
	{
		this->__vwsn_bind_cache_0 = ::vl::Ptr<::ISummer>();
		this->__vwsn_bind_handler_0_0 = ::vl::Ptr<::vl::reflection::description::IEventHandler>();
		this->__vwsn_bind_opened_ = false;
		this->__vwsn_bind_closed_ = false;
		this->__vwsn_bind_listeners_ = ::vl::reflection::description::IValueDictionary::Create();
	}

	void __vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription::__vwsn_bind_activator_()
	{
		auto __vwsn_bind_activator_result_ = ::vl::__vwsn::This(__vwsn_bind_cache_0.Obj())->GetSum();
		using __vwsnt_0 = ::vl::reflection::description::Value;
		FOREACH(__vwsnt_0, __vwsn_bind_callback_, ::vl::reflection::description::GetLazyList<::vl::reflection::description::Value>(::vl::__vwsn::This(__vwsn_bind_listeners_.Obj())->GetValues()))
		{
			::vl::__vwsn::Unbox<::vl::Func<void(::vl::reflection::description::Value)>>(__vwsn_bind_callback_)(::vl::__vwsn::Box(__vwsn_bind_activator_result_));
		}
	}

	void __vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_0_0()
	{
		::vl::__vwsn::This(this)->__vwsn_bind_activator_();
	}

	void __vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription::__vwsn_bind_initialize_()
	{
		(__vwsn_bind_cache_0 = summer);
		(__vwsn_bind_handler_0_0 = ::vl::__vwsn::EventAttach(::vl::__vwsn::This(__vwsn_bind_cache_0.Obj())->SumChanged, ::vl::Func<void()>(::vl::__vwsn::This(this), &__vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_0_0)));
	}

	::vl::Ptr<::vl::reflection::description::IValueListener> __vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription::Subscribe(const ::vl::Func<void(::vl::reflection::description::Value)>& __vwsn_bind_callback_)
	{
		if ((! __vwsn_bind_opened_))
		{
			(__vwsn_bind_opened_ = true);
			::vl::__vwsn::This(this)->__vwsn_bind_initialize_();
		}
		auto __vwsn_subscription_ = this;
		auto __vwsn_listener_shared_ = ::vl::Ptr<::vl::reflection::description::IValueListener>(new ::vl_workflow_global::__vwsnc2_BindCustomInterface_Bind_Subscribe__vl_reflection_description_IValueListener(__vwsn_bind_listeners_, __vwsn_subscription_, this));
		::vl::__vwsn::This(__vwsn_bind_listeners_.Obj())->Set(::vl::__vwsn::Box(__vwsn_listener_shared_), ::vl::__vwsn::Box(__vwsn_bind_callback_));
		return __vwsn_listener_shared_;
	}

	bool __vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription::Update()
	{
		if ((! __vwsn_bind_closed_))
		{
			::vl::__vwsn::This(this)->__vwsn_bind_activator_();
			return true;
		}
		return false;
	}

	bool __vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription::Close()
	{
		if ((! __vwsn_bind_closed_))
		{
			(__vwsn_bind_closed_ = true);
			::vl::__vwsn::EventDetach(::vl::__vwsn::This(__vwsn_bind_cache_0.Obj())->SumChanged, __vwsn_bind_handler_0_0);
			(__vwsn_bind_cache_0 = ::vl::Ptr<::ISummer>());
			(__vwsn_bind_handler_0_0 = ::vl::Ptr<::vl::reflection::description::IEventHandler>());
			::vl::__vwsn::This(__vwsn_bind_listeners_.Obj())->Clear();
			return true;
		}
		return false;
	}

	__vwsnc2_BindCustomInterface_Bind_Subscribe__vl_reflection_description_IValueListener::__vwsnc2_BindCustomInterface_Bind_Subscribe__vl_reflection_description_IValueListener(::vl::Ptr<::vl::reflection::description::IValueDictionary> __vwsnctor___vwsn_bind_listeners_, ::vl::reflection::description::IValueSubscription* __vwsnctor___vwsn_subscription_, ::vl::reflection::description::IValueSubscription* __vwsnctorthis_0)
		:__vwsn_bind_listeners_(__vwsnctor___vwsn_bind_listeners_)
		, __vwsn_subscription_(__vwsnctor___vwsn_subscription_)
		, __vwsnthis_0(__vwsnctorthis_0)
	{
	}

	::vl::reflection::description::IValueSubscription* __vwsnc2_BindCustomInterface_Bind_Subscribe__vl_reflection_description_IValueListener::GetSubscription()
	{
		return ::vl::__vwsn::This(this)->__vwsn_subscription_;
	}

	bool __vwsnc2_BindCustomInterface_Bind_Subscribe__vl_reflection_description_IValueListener::GetStopped()
	{
		return (! ::vl::__vwsn::This(::vl::__vwsn::This(::vl::__vwsn::This(this)->__vwsn_bind_listeners_.Obj())->GetKeys().Obj())->Contains(::vl::__vwsn::Box(this)));
	}

	bool __vwsnc2_BindCustomInterface_Bind_Subscribe__vl_reflection_description_IValueListener::StopListening()
	{
		if (::vl::__vwsn::This(::vl::__vwsn::This(::vl::__vwsn::This(this)->__vwsn_bind_listeners_.Obj())->GetKeys().Obj())->Contains(::vl::__vwsn::Box(this)))
		{
			::vl::__vwsn::This(::vl::__vwsn::This(this)->__vwsn_bind_listeners_.Obj())->Remove(::vl::__vwsn::Box(this));
			return true;
		}
		return false;
	}

	__vwsnc3_BindCustomInterface_main__IAdder::__vwsnc3_BindCustomInterface_main__IAdder()
	{
		this->sum = 0;
	}

	::vl::vint32_t __vwsnc3_BindCustomInterface_main__IAdder::GetSum()
	{
		return sum;
	}

	void __vwsnc3_BindCustomInterface_main__IAdder::Add(::vl::vint32_t value)
	{
		(sum = (sum + value));
		::vl::__vwsn::EventInvoke(::vl::__vwsn::This(this)->SumChanged)();
	}

}

/***********************************************************************
Class (::ISummer)
***********************************************************************/

/***********************************************************************
Class (::IAdder)
***********************************************************************/

