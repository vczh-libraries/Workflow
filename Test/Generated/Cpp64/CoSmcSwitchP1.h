/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/CoSmcSwitchP1.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_COSMCSWITCHP1
#define VCZH_WORKFLOW_COMPILER_GENERATED_COSMCSWITCHP1

#include "../Source/CppTypes.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#endif

namespace vl_workflow_global
{
	class __vwsnc1_CoSmcSwitchP1_SMSwitchP1___vwsn_state_CreateCoroutine__vl_reflection_description_ICoroutine;
}

class SMSwitchP1;

class SMSwitchP1 : public ::vl::reflection::description::StateMachine, public ::vl::reflection::Description<SMSwitchP1>
{
	friend class ::vl_workflow_global::__vwsnc1_CoSmcSwitchP1_SMSwitchP1___vwsn_state_CreateCoroutine__vl_reflection_description_ICoroutine;
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
	friend struct ::vl::reflection::description::CustomTypeDescriptorSelector<SMSwitchP1>;
#endif
public:
	void A();
	void B();
	void __vwsn_state_CreateCoroutine(::vl::vint64_t __vwsn_state_startState);
	SMSwitchP1();
};

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class CoSmcSwitchP1
	{
	public:

		::vl::WString s;

		::vl::WString main();

		static CoSmcSwitchP1& Instance();
	};

/***********************************************************************
Closures
***********************************************************************/

	class __vwsnc1_CoSmcSwitchP1_SMSwitchP1___vwsn_state_CreateCoroutine__vl_reflection_description_ICoroutine : public ::vl::Object, public virtual ::vl::reflection::description::ICoroutine
	{
	public:
		::vl::Ptr<::vl::reflection::description::ICoroutine> __vwsn_state_previousCoroutine;
		::vl::vint64_t __vwsn_state_startState;
		::SMSwitchP1* __vwsn_state_stateMachineObject;
		::SMSwitchP1* __vwsnthis_0;

		__vwsnc1_CoSmcSwitchP1_SMSwitchP1___vwsn_state_CreateCoroutine__vl_reflection_description_ICoroutine(::vl::Ptr<::vl::reflection::description::ICoroutine> __vwsnctor___vwsn_state_previousCoroutine, ::vl::vint64_t __vwsnctor___vwsn_state_startState, ::SMSwitchP1* __vwsnctor___vwsn_state_stateMachineObject, ::SMSwitchP1* __vwsnctorthis_0);

		::vl::Ptr<::vl::reflection::description::IValueException> __vwsn_co_tempexvar0_ex;
		::vl::vint64_t __vwsn_co0_switch_2 = 0;
		::vl::vint64_t __vwsn_co1_state_currentState = 0;
		::vl::vint64_t __vwsn_co2_state_state = 0;
		::vl::vint64_t __vwsn_co_state_ = 0;
		::vl::vint64_t __vwsn_co_state_before_pause_ = 0;
		::vl::Ptr<::vl::reflection::description::IValueException> __vwsn_prop_Failure;
		::vl::Ptr<::vl::reflection::description::IValueException> GetFailure() override;
		void SetFailure(::vl::Ptr<::vl::reflection::description::IValueException> __vwsn_value_);
		::vl::reflection::description::CoroutineStatus __vwsn_prop_Status = static_cast<::vl::reflection::description::CoroutineStatus>(0);
		::vl::reflection::description::CoroutineStatus GetStatus() override;
		void SetStatus(::vl::reflection::description::CoroutineStatus __vwsn_value_);
		void Resume(bool __vwsn_raise_exception_, ::vl::Ptr<::vl::reflection::description::CoroutineResult> __vwsn_coroutine_output_) override;
	};
}

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif
