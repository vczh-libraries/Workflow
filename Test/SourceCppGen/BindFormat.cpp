/***********************************************************************
Generated from ../Resources/Codegen/BindFormat.txt
***********************************************************************/

#include "BindFormat.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_BindFormat)
	vl_workflow_global::BindFormat instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.s = L"";
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_BindFormat)

namespace vl_workflow_global
{
	void BindFormat::Callback(::vl::reflection::description::Value value)
	{
		throw 0;
	}

	::vl::WString BindFormat::main()
	{
		throw 0;
	}

	BindFormat& BindFormat::Instance()
	{
		return Getvl_workflow_global_BindFormat().instance;
	}

	class __vwsnc1_main__vl_reflection_description_IValueSubscription : public ::vl::Object, public virtual ::vl::reflection::description::IValueSubscription
	{
	public:
		::vl::Ptr<::test::ObservableValue> __vwsn_bind_cache_1;
		::vl::Ptr<::vl::reflection::description::IEventHandler> __vwsn_bind_handler_1_0;
		bool __vwsn_bind_opened_ = false;
		bool __vwsn_bind_closed_ = false;
		::vl::Ptr<::vl::reflection::description::IValueDictionary> __vwsn_bind_listeners_;
		void __vwsn_bind_activator_();
		void __vwsn_bind_callback_1_0(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1);
		void __vwsn_bind_initialize_();
		::vl::Ptr<::vl::reflection::description::IValueListener> Subscribe(::vl::Func<void(::vl::reflection::description::Value)> callback) override;
		bool Update() override;
		bool Close() override;
	};

	class __vwsnc2_main_Subscribe__vl_reflection_description_IValueListener : public ::vl::Object, public virtual ::vl::reflection::description::IValueListener
	{
	public:
		::vl::reflection::description::IValueSubscription* GetSubscription() override;
		bool GetStopped() override;
		bool StopListening() override;
	};







	void __vwsnc1_main__vl_reflection_description_IValueSubscription::__vwsnc1_main__vl_reflection_description_IValueSubscription()
	{
		throw 0;
	}

	void __vwsnc1_main__vl_reflection_description_IValueSubscription::__vwsnc1_main__vl_reflection_description_IValueSubscription(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1)
	{
		throw 0;
	}

	void __vwsnc1_main__vl_reflection_description_IValueSubscription::__vwsnc1_main__vl_reflection_description_IValueSubscription()
	{
		throw 0;
	}

	::vl::Ptr<::vl::reflection::description::IValueListener> __vwsnc1_main__vl_reflection_description_IValueSubscription::__vwsnc1_main__vl_reflection_description_IValueSubscription(::vl::Func<void(::vl::reflection::description::Value)> callback)
	{
		throw 0;
	}

	bool __vwsnc1_main__vl_reflection_description_IValueSubscription::__vwsnc1_main__vl_reflection_description_IValueSubscription()
	{
		throw 0;
	}

	bool __vwsnc1_main__vl_reflection_description_IValueSubscription::__vwsnc1_main__vl_reflection_description_IValueSubscription()
	{
		throw 0;
	}


	::vl::reflection::description::IValueSubscription* __vwsnc2_main_Subscribe__vl_reflection_description_IValueListener::__vwsnc2_main_Subscribe__vl_reflection_description_IValueListener()
	{
		throw 0;
	}

	bool __vwsnc2_main_Subscribe__vl_reflection_description_IValueListener::__vwsnc2_main_Subscribe__vl_reflection_description_IValueListener()
	{
		throw 0;
	}

	bool __vwsnc2_main_Subscribe__vl_reflection_description_IValueListener::__vwsnc2_main_Subscribe__vl_reflection_description_IValueListener()
	{
		throw 0;
	}
}

