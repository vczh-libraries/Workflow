/***********************************************************************
Generated from ../Resources/Codegen/BindComplex.txt
***********************************************************************/

#include "BindComplex.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_BindComplex)
	vl_workflow_global::BindComplex instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.s = L"";
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_BindComplex)

namespace vl_workflow_global
{
	void BindComplex::Callback(::vl::reflection::description::Value value)
	{
		throw 0;
	}

	::vl::WString BindComplex::main()
	{
		throw 0;
	}

	BindComplex& BindComplex::Instance()
	{
		return Getvl_workflow_global_BindComplex().instance;
	}

	class __vwsnc2_BindComplex_main_Subscribe__vl_reflection_description_IValueListener : public ::vl::Object, public virtual ::vl::reflection::description::IValueListener
	{
	public:
		::vl::reflection::description::IValueSubscription* GetSubscription() override;
		bool GetStopped() override;
		bool StopListening() override;
	};

	class __vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription : public ::vl::Object, public virtual ::vl::reflection::description::IValueSubscription
	{
	public:
		::vl::Ptr<::test::ObservableValue> __vwsn_bind_cache_1;
		::vl::Ptr<::vl::reflection::description::IEventHandler> __vwsn_bind_handler_1_0;
		::vl::Ptr<::test::ObservableValue> __vwsn_bind_cache_2;
		::vl::Ptr<::vl::reflection::description::IEventHandler> __vwsn_bind_handler_2_0;
		::vl::Ptr<::test::ObservableValue> __vwsn_bind_cache_3;
		::vl::Ptr<::vl::reflection::description::IEventHandler> __vwsn_bind_handler_3_0;
		bool __vwsn_bind_opened_ = false;
		bool __vwsn_bind_closed_ = false;
		::vl::Ptr<::vl::reflection::description::IValueDictionary> __vwsn_bind_listeners_;
		void __vwsn_bind_activator_();
		void __vwsn_bind_callback_1_0(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1);
		void __vwsn_bind_callback_2_0(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1);
		void __vwsn_bind_callback_3_0(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1);
		void __vwsn_bind_initialize_();
		::vl::Ptr<::vl::reflection::description::IValueListener> Subscribe(const ::vl::Func<void(::vl::reflection::description::Value)>& callback) override;
		bool Update() override;
		bool Close() override;
	};


	::vl::reflection::description::IValueSubscription* __vwsnc2_BindComplex_main_Subscribe__vl_reflection_description_IValueListener::GetSubscription()
	{
		throw 0;
	}

	bool __vwsnc2_BindComplex_main_Subscribe__vl_reflection_description_IValueListener::GetStopped()
	{
		throw 0;
	}

	bool __vwsnc2_BindComplex_main_Subscribe__vl_reflection_description_IValueListener::StopListening()
	{
		throw 0;
	}











	void __vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::__vwsn_bind_activator_()
	{
		throw 0;
	}

	void __vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_1_0(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1)
	{
		throw 0;
	}

	void __vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_2_0(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1)
	{
		throw 0;
	}

	void __vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_3_0(::vl::vint32_t __vwsn_bind_callback_argument_0, ::vl::vint32_t __vwsn_bind_callback_argument_1)
	{
		throw 0;
	}

	void __vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::__vwsn_bind_initialize_()
	{
		throw 0;
	}

	::vl::Ptr<::vl::reflection::description::IValueListener> __vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::Subscribe(const ::vl::Func<void(::vl::reflection::description::Value)>& callback)
	{
		throw 0;
	}

	bool __vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::Update()
	{
		throw 0;
	}

	bool __vwsnc1_BindComplex_main__vl_reflection_description_IValueSubscription::Close()
	{
		throw 0;
	}
}

