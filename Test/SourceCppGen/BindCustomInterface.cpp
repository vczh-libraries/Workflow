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
	::vl::Ptr<::vl::reflection::description::IValueSubscription> BindCustomInterface::Bind(::vl::Ptr<::ISummer> summer)
	{
		throw 0;
	}

	void BindCustomInterface::Execute(::vl::Ptr<::IAdder> adder)
	{
		throw 0;
	}

	::vl::WString BindCustomInterface::main()
	{
		throw 0;
	}

	BindCustomInterface& BindCustomInterface::Instance()
	{
		return Getvl_workflow_global_BindCustomInterface().instance;
	}

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

		::vl::Ptr<::ISummer> __vwsn_bind_cache_1;
		::vl::Ptr<::vl::reflection::description::IEventHandler> __vwsn_bind_handler_1_0;
		bool __vwsn_bind_opened_ = false;
		bool __vwsn_bind_closed_ = false;
		::vl::Ptr<::vl::reflection::description::IValueDictionary> __vwsn_bind_listeners_;
		void __vwsn_bind_activator_();
		void __vwsn_bind_callback_1_0();
		void __vwsn_bind_initialize_();
		::vl::Ptr<::vl::reflection::description::IValueListener> Subscribe(const ::vl::Func<void(::vl::reflection::description::Value)>& callback) override;
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

	__vwsnf1_BindCustomInterface_Bind_::__vwsnf1_BindCustomInterface_Bind_()
	{
	}

	void __vwsnf1_BindCustomInterface_Bind_::operator()(::vl::reflection::description::Value value) const
	{
		throw 0;
	}

	__vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription::__vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription(::vl::Ptr<::ISummer> __vwsnctor_summer)
		:summer(__vwsnctor_summer)
	{
		this->__vwsn_bind_cache_1 = nullptr;
		this->__vwsn_bind_handler_1_0 = nullptr;
		this->__vwsn_bind_opened_ = false;
		this->__vwsn_bind_closed_ = false;
		this->__vwsn_bind_listeners_ = ::vl::reflection::description::IValueDictionary::Create();
	}

	void __vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription::__vwsn_bind_activator_()
	{
		throw 0;
	}

	void __vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_1_0()
	{
		throw 0;
	}

	void __vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription::__vwsn_bind_initialize_()
	{
		throw 0;
	}

	::vl::Ptr<::vl::reflection::description::IValueListener> __vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription::Subscribe(const ::vl::Func<void(::vl::reflection::description::Value)>& callback)
	{
		throw 0;
	}

	bool __vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription::Update()
	{
		throw 0;
	}

	bool __vwsnc1_BindCustomInterface_Bind__vl_reflection_description_IValueSubscription::Close()
	{
		throw 0;
	}

	__vwsnc2_BindCustomInterface_Bind_Subscribe__vl_reflection_description_IValueListener::__vwsnc2_BindCustomInterface_Bind_Subscribe__vl_reflection_description_IValueListener(::vl::Ptr<::vl::reflection::description::IValueDictionary> __vwsnctor___vwsn_bind_listeners_, ::vl::reflection::description::IValueSubscription* __vwsnctor___vwsn_subscription_, ::vl::reflection::description::IValueSubscription* __vwsnctorthis_0)
		:__vwsn_bind_listeners_(__vwsnctor___vwsn_bind_listeners_)
		, __vwsn_subscription_(__vwsnctor___vwsn_subscription_)
		, __vwsnthis_0(__vwsnctorthis_0)
	{
	}

	::vl::reflection::description::IValueSubscription* __vwsnc2_BindCustomInterface_Bind_Subscribe__vl_reflection_description_IValueListener::GetSubscription()
	{
		throw 0;
	}

	bool __vwsnc2_BindCustomInterface_Bind_Subscribe__vl_reflection_description_IValueListener::GetStopped()
	{
		throw 0;
	}

	bool __vwsnc2_BindCustomInterface_Bind_Subscribe__vl_reflection_description_IValueListener::StopListening()
	{
		throw 0;
	}

	__vwsnc3_BindCustomInterface_main__IAdder::__vwsnc3_BindCustomInterface_main__IAdder()
	{
		this->sum = 0;
	}

	::vl::vint32_t __vwsnc3_BindCustomInterface_main__IAdder::GetSum()
	{
		throw 0;
	}

	void __vwsnc3_BindCustomInterface_main__IAdder::Add(::vl::vint32_t value)
	{
		throw 0;
	}

}

/***********************************************************************
Class (::ISummer)
***********************************************************************/

/***********************************************************************
Class (::IAdder)
***********************************************************************/

