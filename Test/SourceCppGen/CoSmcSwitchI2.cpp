/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/CoSmcSwitchI2.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "CoSmcSwitchI2.h"
/* CodePack:BeginIgnore() */
#ifndef VCZH_DEBUG_NO_REFLECTION
/* CodePack:ConditionOff(VCZH_DEBUG_NO_REFLECTION, CoSmcSwitchI2Reflection.h) */
#include "CoSmcSwitchI2Reflection.h"
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

#define GLOBAL_SYMBOL ::vl_workflow_global::CoSmcSwitchI2::
#define GLOBAL_NAME ::vl_workflow_global::CoSmcSwitchI2::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::CoSmcSwitchI2::Instance()

/***********************************************************************
Global Variables
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_CoSmcSwitchI2)
	vl_workflow_global::CoSmcSwitchI2 instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString::Unmanaged(L"");
	FINALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString::Empty;
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_CoSmcSwitchI2)

namespace vl_workflow_global
{
/***********************************************************************
Global Functions
***********************************************************************/

	::vl::WString CoSmcSwitchI2::main()
	{
		auto sm = ::vl::Ptr<::SMSwitchI2>(new ::SMSwitchI2());
		try
		{
			{
				::vl::__vwsn::This(sm.Obj())->B();
			}
		}
		catch(const ::vl::Exception& __vwsne_0)
		{
			auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_0.Message());
			{
				(GLOBAL_NAME s = ((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"!")));
			}
		}
		return GLOBAL_NAME s;
	}

	CoSmcSwitchI2& CoSmcSwitchI2::Instance()
	{
		return Getvl_workflow_global_CoSmcSwitchI2().instance;
	}

/***********************************************************************
Closures
***********************************************************************/

	//-------------------------------------------------------------------

	__vwsnc1_CoSmcSwitchI2_SMSwitchI2___vwsn_state_CreateCoroutine__vl_reflection_description_ICoroutine::__vwsnc1_CoSmcSwitchI2_SMSwitchI2___vwsn_state_CreateCoroutine__vl_reflection_description_ICoroutine(::vl::Ptr<::vl::reflection::description::ICoroutine> __vwsnctor___vwsn_state_previousCoroutine, ::vl::vint __vwsnctor___vwsn_state_startState, ::SMSwitchI2* __vwsnctor___vwsn_state_stateMachineObject, ::SMSwitchI2* __vwsnctorthis_0)
		:__vwsn_state_previousCoroutine(__vwsnctor___vwsn_state_previousCoroutine)
		, __vwsn_state_startState(__vwsnctor___vwsn_state_startState)
		, __vwsn_state_stateMachineObject(__vwsnctor___vwsn_state_stateMachineObject)
		, __vwsnthis_0(::vl::__vwsn::This(__vwsnctorthis_0))
	{
		this->__vwsn_co_tempexvar0_ex = ::vl::Ptr<::vl::reflection::description::IValueException>();
		this->__vwsn_co0_switch_1 = static_cast<::vl::vint>(0);
		this->__vwsn_co1_state_currentState = static_cast<::vl::vint>(0);
		this->__vwsn_co2_state_state = static_cast<::vl::vint>(0);
		this->__vwsn_co_state_ = static_cast<::vl::vint>(0);
		this->__vwsn_co_state_before_pause_ = (- static_cast<::vl::vint>(1));
		this->__vwsn_prop_Failure = ::vl::Ptr<::vl::reflection::description::IValueException>();
		this->__vwsn_prop_Status = ::vl::reflection::description::CoroutineStatus::Waiting;
	}

	::vl::Ptr<::vl::reflection::description::IValueException> __vwsnc1_CoSmcSwitchI2_SMSwitchI2___vwsn_state_CreateCoroutine__vl_reflection_description_ICoroutine::GetFailure()
	{
		return __vwsn_prop_Failure;
	}
	void __vwsnc1_CoSmcSwitchI2_SMSwitchI2___vwsn_state_CreateCoroutine__vl_reflection_description_ICoroutine::SetFailure(::vl::Ptr<::vl::reflection::description::IValueException> __vwsn_value_)
	{
		(__vwsn_prop_Failure = __vwsn_value_);
	}

	::vl::reflection::description::CoroutineStatus __vwsnc1_CoSmcSwitchI2_SMSwitchI2___vwsn_state_CreateCoroutine__vl_reflection_description_ICoroutine::GetStatus()
	{
		return __vwsn_prop_Status;
	}
	void __vwsnc1_CoSmcSwitchI2_SMSwitchI2___vwsn_state_CreateCoroutine__vl_reflection_description_ICoroutine::SetStatus(::vl::reflection::description::CoroutineStatus __vwsn_value_)
	{
		(__vwsn_prop_Status = __vwsn_value_);
	}

	void __vwsnc1_CoSmcSwitchI2_SMSwitchI2___vwsn_state_CreateCoroutine__vl_reflection_description_ICoroutine::Resume(bool __vwsn_raise_exception_, ::vl::Ptr<::vl::reflection::description::CoroutineResult> __vwsn_coroutine_output_)
	{
		if ((this->GetStatus() != ::vl::reflection::description::CoroutineStatus::Waiting))
		{
			throw ::vl::Exception(::vl::WString::Unmanaged(L"Resume should be called only when the coroutine is in the waiting status."));
		}
		this->SetStatus(::vl::reflection::description::CoroutineStatus::Executing);
		try
		{
			{
				if ((__vwsn_co_state_before_pause_ != (- static_cast<::vl::vint>(1))))
				{
					if ((! static_cast<bool>(__vwsn_coroutine_output_)))
					{
						(__vwsn_co_state_before_pause_ = (- static_cast<::vl::vint>(1)));
					}
					else if ((! static_cast<bool>(::vl::__vwsn::This(__vwsn_coroutine_output_.Obj())->GetFailure())))
					{
						(__vwsn_co_state_before_pause_ = (- static_cast<::vl::vint>(1)));
					}
					else
					{
						if (((__vwsn_co_state_before_pause_ == static_cast<::vl::vint>(0)) || [&](auto __vwsn_1){ return (static_cast<::vl::vint>(3) <=  __vwsn_1 && __vwsn_1 <= static_cast<::vl::vint>(5)); }(__vwsn_co_state_before_pause_)))
						{
							(__vwsn_co_state_before_pause_ = (- static_cast<::vl::vint>(1)));
							(__vwsn_co_tempexvar0_ex = ::vl::__vwsn::This(__vwsn_coroutine_output_.Obj())->GetFailure());
							(__vwsn_co_state_ = static_cast<::vl::vint>(2));
						}
						else
						{
							(__vwsn_co_state_before_pause_ = (- static_cast<::vl::vint>(1)));
							throw ::vl::Exception(::vl::__vwsn::This(::vl::__vwsn::This(__vwsn_coroutine_output_.Obj())->GetFailure().Obj())->GetMessage());
						}
					}
				}
				while (true)
				{
					if ((__vwsn_co_state_ == static_cast<::vl::vint>(1)))
					{
						this->SetStatus(::vl::reflection::description::CoroutineStatus::Stopped);
						return;
					}
					if ((__vwsn_co_state_ == static_cast<::vl::vint>(2)))
					{
						{
							(::vl::__vwsn::This(this->__vwsn_state_stateMachineObject)->stateMachineCoroutine = this->__vwsn_state_previousCoroutine);
						}
						throw ::vl::Exception(::vl::__vwsn::This(__vwsn_co_tempexvar0_ex.Obj())->GetMessage());
					}
					if ((__vwsn_co_state_ == static_cast<::vl::vint>(6)))
					{
						{
							(::vl::__vwsn::This(this->__vwsn_state_stateMachineObject)->stateMachineCoroutine = this->__vwsn_state_previousCoroutine);
						}
						(__vwsn_co_state_ = static_cast<::vl::vint>(1));
						continue;
					}
					try
					{
						{
							if ((__vwsn_co_state_ == static_cast<::vl::vint>(0)))
							{
								(__vwsn_co2_state_state = __vwsn_state_startState);
								if (true)
								{
									(__vwsn_co_state_ = static_cast<::vl::vint>(4));
									continue;
								}
								(__vwsn_co_state_ = static_cast<::vl::vint>(6));
								continue;
							}
							if ((__vwsn_co_state_ == static_cast<::vl::vint>(3)))
							{
								if (true)
								{
									(__vwsn_co_state_ = static_cast<::vl::vint>(4));
									continue;
								}
								(__vwsn_co_state_ = static_cast<::vl::vint>(6));
								continue;
							}
							if ((__vwsn_co_state_ == static_cast<::vl::vint>(4)))
							{
								(__vwsn_co1_state_currentState = __vwsn_co2_state_state);
								(__vwsn_co2_state_state = (- static_cast<::vl::vint>(1)));
								(__vwsn_co0_switch_1 = __vwsn_co1_state_currentState);
								if ((__vwsn_co0_switch_1 == static_cast<::vl::vint>(0)))
								{
									if ((::vl::__vwsn::This(this->__vwsn_state_stateMachineObject)->stateMachineInput == (- static_cast<::vl::vint>(1))))
									{
										this->SetStatus(::vl::reflection::description::CoroutineStatus::Waiting);
										(__vwsn_co_state_before_pause_ = __vwsn_co_state_);
										(__vwsn_co_state_ = static_cast<::vl::vint>(5));
										return;
									}
									(__vwsn_co_state_ = static_cast<::vl::vint>(5));
									continue;
								}
								(__vwsn_co_state_ = static_cast<::vl::vint>(3));
								continue;
							}
							if ((__vwsn_co_state_ == static_cast<::vl::vint>(5)))
							{
								{
									auto __vwsn_switch_0 = ::vl::__vwsn::This(this->__vwsn_state_stateMachineObject)->stateMachineInput;
									if ((__vwsn_switch_0 == static_cast<::vl::vint>(0)))
									{
										(::vl::__vwsn::This(this->__vwsn_state_stateMachineObject)->stateMachineInput = (- static_cast<::vl::vint>(1)));
										{
											(GLOBAL_NAME s = ((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"[A]")));
										}
									}
									else
									{
										(::vl::__vwsn::This(this->__vwsn_state_stateMachineObject)->stateMachineInput = (- static_cast<::vl::vint>(1)));
									}
								}
								(GLOBAL_NAME s = ((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L".")));
								(__vwsn_co_state_ = static_cast<::vl::vint>(6));
								continue;
								(__vwsn_co_state_ = static_cast<::vl::vint>(3));
								continue;
							}
						}
					}
					catch(const ::vl::Exception& __vwsne_1)
					{
						auto __vwsn_co_ex_ = ::vl::reflection::description::IValueException::Create(__vwsne_1.Message());
						{
							(__vwsn_co_tempexvar0_ex = __vwsn_co_ex_);
							(__vwsn_co_state_ = static_cast<::vl::vint>(2));
							continue;
						}
					}
				}
			}
		}
		catch(const ::vl::Exception& __vwsne_0)
		{
			auto __vwsn_co_ex_ = ::vl::reflection::description::IValueException::Create(__vwsne_0.Message());
			{
				this->SetFailure(__vwsn_co_ex_);
				this->SetStatus(::vl::reflection::description::CoroutineStatus::Stopped);
				if (__vwsn_raise_exception_)
				{
					throw;
				}
			}
		}
	}

}

/***********************************************************************
Class (::SMSwitchI2)
***********************************************************************/

void SMSwitchI2::A()
{
	if ((! ::vl::__vwsn::This(this)->stateMachineInitialized))
	{
		(::vl::__vwsn::This(this)->stateMachineInitialized = true);
		::vl::__vwsn::This(this)->__vwsn_state_CreateCoroutine(static_cast<::vl::vint>(0));
		::vl::__vwsn::This(this)->ResumeStateMachine();
	}
	(::vl::__vwsn::This(this)->stateMachineInput = static_cast<::vl::vint>(0));
	::vl::__vwsn::This(this)->ResumeStateMachine();
}
void SMSwitchI2::B()
{
	if ((! ::vl::__vwsn::This(this)->stateMachineInitialized))
	{
		(::vl::__vwsn::This(this)->stateMachineInitialized = true);
		::vl::__vwsn::This(this)->__vwsn_state_CreateCoroutine(static_cast<::vl::vint>(0));
		::vl::__vwsn::This(this)->ResumeStateMachine();
	}
	(::vl::__vwsn::This(this)->stateMachineInput = static_cast<::vl::vint>(1));
	::vl::__vwsn::This(this)->ResumeStateMachine();
}
void SMSwitchI2::__vwsn_state_CreateCoroutine(::vl::vint __vwsn_state_startState)
{
	auto __vwsn_state_stateMachineObject = this;
	auto __vwsn_state_previousCoroutine = ::vl::__vwsn::This(__vwsn_state_stateMachineObject)->stateMachineCoroutine;
	(::vl::__vwsn::This(__vwsn_state_stateMachineObject)->stateMachineCoroutine = ::vl::Ptr<::vl::reflection::description::ICoroutine>(new ::vl_workflow_global::__vwsnc1_CoSmcSwitchI2_SMSwitchI2___vwsn_state_CreateCoroutine__vl_reflection_description_ICoroutine(__vwsn_state_previousCoroutine, __vwsn_state_startState, __vwsn_state_stateMachineObject, this)));
}

SMSwitchI2::SMSwitchI2()
{
}

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
