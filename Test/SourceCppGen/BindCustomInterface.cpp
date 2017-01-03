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

		instance.s = L"";
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


	class __vwsnc1_Bind__vl_reflection_description_IValueSubscription : public ::vl::Object, public virtual ::vl::reflection::description::IValueSubscription
	{
	public:
		::vl::Ptr<::ISummer> __vwsn_bind_cache_1;
		::vl::Ptr<::vl::reflection::description::IEventHandler> __vwsn_bind_handler_1_0;
		bool __vwsn_bind_opened_ = false;
		bool __vwsn_bind_closed_ = false;
		::vl::Ptr<::vl::reflection::description::IValueDictionary> __vwsn_bind_listeners_;
		void __vwsn_bind_activator_();
		void __vwsn_bind_callback_1_0();
		void __vwsn_bind_initialize_();
		::vl::Ptr<::vl::reflection::description::IValueListener> Subscribe(::vl::Func<void(::vl::reflection::description::Value)> callback) override;
		bool Update() override;
		bool Close() override;
	};

	class __vwsnc2_Bind_Subscribe__vl_reflection_description_IValueListener : public ::vl::Object, public virtual ::vl::reflection::description::IValueListener
	{
	public:
		::vl::reflection::description::IValueSubscription* GetSubscription() override;
		bool GetStopped() override;
		bool StopListening() override;
	};

	class __vwsnc3_main__IAdder : public ::vl::Object, public virtual ::IAdder
	{
	public:
		::vl::vint32_t sum = 0;
		::vl::vint32_t GetSum() override;
		void Add(::vl::vint32_t value) override;
	};








	void __vwsnc1_Bind__vl_reflection_description_IValueSubscription::__vwsnc1_Bind__vl_reflection_description_IValueSubscription()
	{
		throw 0;
	}

	void __vwsnc1_Bind__vl_reflection_description_IValueSubscription::__vwsnc1_Bind__vl_reflection_description_IValueSubscription()
	{
		throw 0;
	}

	void __vwsnc1_Bind__vl_reflection_description_IValueSubscription::__vwsnc1_Bind__vl_reflection_description_IValueSubscription()
	{
		throw 0;
	}

	::vl::Ptr<::vl::reflection::description::IValueListener> __vwsnc1_Bind__vl_reflection_description_IValueSubscription::__vwsnc1_Bind__vl_reflection_description_IValueSubscription(::vl::Func<void(::vl::reflection::description::Value)> callback)
	{
		throw 0;
	}

	bool __vwsnc1_Bind__vl_reflection_description_IValueSubscription::__vwsnc1_Bind__vl_reflection_description_IValueSubscription()
	{
		throw 0;
	}

	bool __vwsnc1_Bind__vl_reflection_description_IValueSubscription::__vwsnc1_Bind__vl_reflection_description_IValueSubscription()
	{
		throw 0;
	}


	::vl::reflection::description::IValueSubscription* __vwsnc2_Bind_Subscribe__vl_reflection_description_IValueListener::__vwsnc2_Bind_Subscribe__vl_reflection_description_IValueListener()
	{
		throw 0;
	}

	bool __vwsnc2_Bind_Subscribe__vl_reflection_description_IValueListener::__vwsnc2_Bind_Subscribe__vl_reflection_description_IValueListener()
	{
		throw 0;
	}

	bool __vwsnc2_Bind_Subscribe__vl_reflection_description_IValueListener::__vwsnc2_Bind_Subscribe__vl_reflection_description_IValueListener()
	{
		throw 0;
	}



	::vl::vint32_t __vwsnc3_main__IAdder::__vwsnc3_main__IAdder()
	{
		throw 0;
	}

	void __vwsnc3_main__IAdder::__vwsnc3_main__IAdder(::vl::vint32_t value)
	{
		throw 0;
	}
}

/***********************************************************************
Class (::ISummer)
***********************************************************************/

::vl::vint32_t ISummer::ISummer()
{
	throw 0;
}

/***********************************************************************
Class (::IAdder)
***********************************************************************/

void IAdder::IAdder(::vl::vint32_t value)
{
	throw 0;
}

