/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/CoRawCoroutine4.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_CORAWCOROUTINE4
#define VCZH_WORKFLOW_COMPILER_GENERATED_CORAWCOROUTINE4

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
	class __vwsnc1_CoRawCoroutine4_main__vl_reflection_description_ICoroutine;
}

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class CoRawCoroutine4
	{
	public:

		::vl::WString s;

		::vl::WString main();

		static CoRawCoroutine4& Instance();
	};

/***********************************************************************
Closures
***********************************************************************/

	class __vwsnc1_CoRawCoroutine4_main__vl_reflection_description_ICoroutine : public ::vl::Object, public virtual ::vl::reflection::description::ICoroutine
	{
	public:
		__vwsnc1_CoRawCoroutine4_main__vl_reflection_description_ICoroutine();

		::vl::vint64_t __vwsn_co0_for_begin_i = 0;
		::vl::vint64_t __vwsn_co1_for_end_i = 0;
		::vl::vint64_t __vwsn_co2_i = 0;
		::vl::Ptr<::vl::reflection::description::IValueList> __vwsn_co3_o;
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
