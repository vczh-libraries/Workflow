/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/BindCustomInterface2.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "BindCustomInterface2.h"
/* CodePack:BeginIgnore() */
#ifndef VCZH_DEBUG_NO_REFLECTION
/* CodePack:ConditionOff(VCZH_DEBUG_NO_REFLECTION, BindCustomInterface2Reflection.h) */
#include "BindCustomInterface2Reflection.h"
#endif
/* CodePack:EndIgnore() */

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#endif

#define GLOBAL_SYMBOL ::vl_workflow_global::BindCustomInterface2::
#define GLOBAL_NAME ::vl_workflow_global::BindCustomInterface2::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::BindCustomInterface2::Instance()

/***********************************************************************
Global Variables
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_BindCustomInterface2)
	vl_workflow_global::BindCustomInterface2 instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString::Unmanaged(L"");
	FINALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString::Empty;
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_BindCustomInterface2)

namespace vl_workflow_global
{
/***********************************************************************
Global Functions
***********************************************************************/

	::vl::Ptr<::vl::reflection::description::IValueSubscription> BindCustomInterface2::Bind(::vl::Ptr<::ISummer2> summer)
	{
		auto subscription = ::vl::Ptr<::vl::reflection::description::IValueSubscription>(new ::vl_workflow_global::__vwsnc1_BindCustomInterface2_Bind__vl_reflection_description_IValueSubscription(summer));
		::vl::__vwsn::This(subscription.Obj())->Open();
		::vl::__vwsn::EventAttach(::vl::__vwsn::This(subscription.Obj())->ValueChanged, vl::Func(::vl_workflow_global::__vwsnf1_BindCustomInterface2_Bind_()));
		return subscription;
	}

	void BindCustomInterface2::Execute(::vl::Ptr<::IAdder2> adder)
	{
		auto subscription = GLOBAL_NAME Bind(::vl::Ptr<::ISummer2>(adder));
		{
			auto __vwsn_for_begin_x = static_cast<::vl::vint>(1);
			auto __vwsn_for_end_x = static_cast<::vl::vint>(5);
			auto x = __vwsn_for_begin_x;
			while ((x <= __vwsn_for_end_x))
			{
				{
					::vl::__vwsn::This(adder.Obj())->Add(x);
				}
				(x = (x + static_cast<::vl::vint>(1)));
			}
		}
		::vl::__vwsn::This(subscription.Obj())->Close();
	}

	::vl::WString BindCustomInterface2::main()
	{
		GLOBAL_NAME Execute(::vl::Ptr<::IAdder2>(new ::vl_workflow_global::__vwsnc2_BindCustomInterface2_main__IAdder2()));
		return GLOBAL_NAME s;
	}

	BindCustomInterface2& BindCustomInterface2::Instance()
	{
		return Getvl_workflow_global_BindCustomInterface2().instance;
	}

/***********************************************************************
Closures
***********************************************************************/

	//-------------------------------------------------------------------

	__vwsnf1_BindCustomInterface2_Bind_::__vwsnf1_BindCustomInterface2_Bind_()
	{
	}

	void __vwsnf1_BindCustomInterface2_Bind_::operator()(const ::vl::reflection::description::Value& value) const
	{
		(GLOBAL_NAME s = ((((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"[")) + ::vl::__vwsn::ToString(::vl::__vwsn::Unbox<::vl::vint>(value))) + ::vl::WString::Unmanaged(L"]")));
	}

	//-------------------------------------------------------------------

	__vwsnc1_BindCustomInterface2_Bind__vl_reflection_description_IValueSubscription::__vwsnc1_BindCustomInterface2_Bind__vl_reflection_description_IValueSubscription(::vl::Ptr<::ISummer2> __vwsnctor_summer)
		:summer(__vwsnctor_summer)
	{
		this->__vwsn_bind_cache_0 = ::vl::Ptr<::ISummer2>();
		this->__vwsn_bind_handler_0_0 = ::vl::Ptr<::vl::reflection::description::IEventHandler>();
		this->__vwsn_bind_opened_ = false;
		this->__vwsn_bind_closed_ = false;
	}

	void __vwsnc1_BindCustomInterface2_Bind__vl_reflection_description_IValueSubscription::__vwsn_bind_activator_()
	{
		auto __vwsn_bind_activator_result_ = ::vl::__vwsn::This(__vwsn_bind_cache_0.Obj())->GetSum();
		::vl::__vwsn::EventInvoke(this->ValueChanged)(::vl::__vwsn::Box(__vwsn_bind_activator_result_));
	}

	void __vwsnc1_BindCustomInterface2_Bind__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_0_0()
	{
		this->__vwsn_bind_activator_();
	}

	bool __vwsnc1_BindCustomInterface2_Bind__vl_reflection_description_IValueSubscription::Open()
	{
		if ((! __vwsn_bind_opened_))
		{
			(__vwsn_bind_opened_ = true);
			(__vwsn_bind_cache_0 = [&](){ try{ return summer; } catch(...){ return ::vl::Ptr<::ISummer2>(); } }());
			(__vwsn_bind_handler_0_0 = [&](){ try{ return ::vl::__vwsn::EventAttach(::vl::__vwsn::This(__vwsn_bind_cache_0.Obj())->SumChanged, ::vl::Func<void()>(this, &__vwsnc1_BindCustomInterface2_Bind__vl_reflection_description_IValueSubscription::__vwsn_bind_callback_0_0)); } catch(...){ return ::vl::Ptr<::vl::reflection::description::IEventHandler>(); } }());
			return true;
		}
		return false;
	}

	bool __vwsnc1_BindCustomInterface2_Bind__vl_reflection_description_IValueSubscription::Update()
	{
		if ((__vwsn_bind_opened_ && (! __vwsn_bind_closed_)))
		{
			this->__vwsn_bind_activator_();
			return true;
		}
		return false;
	}

	bool __vwsnc1_BindCustomInterface2_Bind__vl_reflection_description_IValueSubscription::Close()
	{
		if ((! __vwsn_bind_closed_))
		{
			(__vwsn_bind_closed_ = true);
			if (static_cast<bool>(__vwsn_bind_handler_0_0))
			{
				::vl::__vwsn::EventDetach(::vl::__vwsn::This(__vwsn_bind_cache_0.Obj())->SumChanged, __vwsn_bind_handler_0_0);
				(__vwsn_bind_handler_0_0 = ::vl::Ptr<::vl::reflection::description::IEventHandler>());
			}
			(__vwsn_bind_cache_0 = ::vl::Ptr<::ISummer2>());
			(__vwsn_bind_handler_0_0 = ::vl::Ptr<::vl::reflection::description::IEventHandler>());
			return true;
		}
		return false;
	}

	//-------------------------------------------------------------------

	__vwsnc2_BindCustomInterface2_main__IAdder2::__vwsnc2_BindCustomInterface2_main__IAdder2()
	{
		this->__vwsn_prop_Sum = static_cast<::vl::vint>(0);
	}

	::vl::vint __vwsnc2_BindCustomInterface2_main__IAdder2::GetSum()
	{
		return __vwsn_prop_Sum;
	}
	void __vwsnc2_BindCustomInterface2_main__IAdder2::SetSum(::vl::vint __vwsn_value_)
	{
		if ((__vwsn_prop_Sum != __vwsn_value_))
		{
			(__vwsn_prop_Sum = __vwsn_value_);
			::vl::__vwsn::EventInvoke(this->SumChanged)();
		}
	}

	void __vwsnc2_BindCustomInterface2_main__IAdder2::Add(::vl::vint value)
	{
		this->SetSum((this->GetSum() + value));
	}

}

/***********************************************************************
Class (::ISummer2)
***********************************************************************/

/***********************************************************************
Class (::IAdder2)
***********************************************************************/

#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
