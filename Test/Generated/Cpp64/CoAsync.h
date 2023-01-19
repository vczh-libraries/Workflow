/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/CoAsync.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_COASYNC
#define VCZH_WORKFLOW_COMPILER_GENERATED_COASYNC

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
	struct __vwsnf1_CoAsync_GetStringAsync_;
	struct __vwsnf2_CoAsync_GetStringAsync2_;
	struct __vwsnf3_CoAsync_GetMultipleString_;
	struct __vwsno4_CoAsync_main_;
	class __vwsnc1_CoAsync_GetStringAsync___vl_reflection_description_ICoroutine;
	class __vwsnc2_CoAsync_GetStringAsync__CoAsync_types_IStringAsync;
	class __vwsnc3_CoAsync_GetStringAsync2__CoAsync_types_IStringAsync;
	class __vwsnc4_CoAsync_GetMultipleString___vl_reflection_description_ICoroutine;
}

namespace CoAsync_types
{
	class IStringAsync;

	class IStringAsync : public virtual ::vl::reflection::description::IAsync, public ::vl::reflection::Description<IStringAsync>
	{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
		friend struct ::vl::reflection::description::CustomTypeDescriptorSelector<IStringAsync>;
#endif
	public:
		static ::vl::WString CastResult(const ::vl::reflection::description::Value& value);
		static ::vl::reflection::description::Value StoreResult(const ::vl::WString& value);
	};

}
/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class CoAsync
	{
	public:

		::vl::WString s;

		::vl::Ptr<::CoAsync_types::IStringAsync> GetStringAsync(::vl::vint64_t x);
		::vl::Ptr<::CoAsync_types::IStringAsync> GetStringAsync2(::vl::vint64_t x);
		void GetMultipleString();
		::vl::WString main();

		static CoAsync& Instance();
	};

/***********************************************************************
Closures
***********************************************************************/

	struct __vwsnf1_CoAsync_GetStringAsync_
	{
		::vl::vint64_t x;

		__vwsnf1_CoAsync_GetStringAsync_(::vl::vint64_t __vwsnctor_x);

		::vl::Ptr<::vl::reflection::description::ICoroutine> operator()(::vl::reflection::description::AsyncCoroutine::IImpl* __vwsn_co_impl_) const;
	};

	struct __vwsnf2_CoAsync_GetStringAsync2_
	{
		::vl::Ptr<::vl::reflection::description::IFuture> future;
		::vl::vint64_t x;

		__vwsnf2_CoAsync_GetStringAsync2_(::vl::Ptr<::vl::reflection::description::IFuture> __vwsnctor_future, ::vl::vint64_t __vwsnctor_x);

		void operator()() const;
	};

	struct __vwsnf3_CoAsync_GetMultipleString_
	{
		__vwsnf3_CoAsync_GetMultipleString_();

		::vl::Ptr<::vl::reflection::description::ICoroutine> operator()(::vl::reflection::description::AsyncCoroutine::IImpl* __vwsn_co_impl_) const;
	};

	struct __vwsno4_CoAsync_main_
	{
		__vwsno4_CoAsync_main_();

		void operator()() const;
	};

	class __vwsnc1_CoAsync_GetStringAsync___vl_reflection_description_ICoroutine : public ::vl::Object, public virtual ::vl::reflection::description::ICoroutine
	{
	public:
		::vl::reflection::description::AsyncCoroutine::IImpl* __vwsn_co_impl_;
		::vl::vint64_t x;

		__vwsnc1_CoAsync_GetStringAsync___vl_reflection_description_ICoroutine(::vl::reflection::description::AsyncCoroutine::IImpl* __vwsnctor___vwsn_co_impl_, ::vl::vint64_t __vwsnctor_x);

		::vl::vint64_t __vwsn_co_state_ = 0;
		::vl::vint64_t __vwsn_co_state_before_pause_ = 0;
		::vl::Ptr<::vl::reflection::description::IValueException> __vwsn_prop_Failure;
		::vl::Ptr<::vl::reflection::description::IValueException> GetFailure() override;
		void SetFailure(::vl::Ptr<::vl::reflection::description::IValueException> __vwsn_value_);
		::vl::reflection::description::CoroutineStatus __vwsn_prop_Status = static_cast<::vl::reflection::description::CoroutineStatus>(0);
		::vl::reflection::description::CoroutineStatus GetStatus() override;
		void SetStatus(::vl::reflection::description::CoroutineStatus __vwsn_value_);
		void Resume(bool __vwsn_raise_exception_, ::vl::Ptr<::vl::reflection::description::CoroutineResult> __vwsn_co_result_) override;
	};

	class __vwsnc2_CoAsync_GetStringAsync__CoAsync_types_IStringAsync : public ::vl::Object, public virtual ::CoAsync_types::IStringAsync
	{
	public:
		__vwsnc2_CoAsync_GetStringAsync__CoAsync_types_IStringAsync(::vl::Ptr<::vl::reflection::description::IAsync> __vwsnctor___vwsn_co_mixin_source_variable_);

		::vl::Ptr<::vl::reflection::description::IAsync> __vwsn_mixin_source_;
		bool Execute(const ::vl::Func<void(::vl::Ptr<::vl::reflection::description::CoroutineResult>)>& __vwsn_mixin_parameter_callback, ::vl::Ptr<::vl::reflection::description::AsyncContext> __vwsn_mixin_parameter_context) override;
		::vl::reflection::description::AsyncStatus GetStatus() override;
	};

	class __vwsnc3_CoAsync_GetStringAsync2__CoAsync_types_IStringAsync : public ::vl::Object, public virtual ::CoAsync_types::IStringAsync
	{
	public:
		__vwsnc3_CoAsync_GetStringAsync2__CoAsync_types_IStringAsync(::vl::Ptr<::vl::reflection::description::IFuture> __vwsnctor_future);

		::vl::Ptr<::vl::reflection::description::IAsync> __vwsn_mixin_source_;
		bool Execute(const ::vl::Func<void(::vl::Ptr<::vl::reflection::description::CoroutineResult>)>& __vwsn_mixin_parameter_callback, ::vl::Ptr<::vl::reflection::description::AsyncContext> __vwsn_mixin_parameter_context) override;
		::vl::reflection::description::AsyncStatus GetStatus() override;
	};

	class __vwsnc4_CoAsync_GetMultipleString___vl_reflection_description_ICoroutine : public ::vl::Object, public virtual ::vl::reflection::description::ICoroutine
	{
	public:
		::vl::reflection::description::AsyncCoroutine::IImpl* __vwsn_co_impl_;

		__vwsnc4_CoAsync_GetMultipleString___vl_reflection_description_ICoroutine(::vl::reflection::description::AsyncCoroutine::IImpl* __vwsnctor___vwsn_co_impl_);

		::vl::WString __vwsn_co0_x;
		::vl::WString __vwsn_co1_y;
		::vl::vint64_t __vwsn_co2_for_begin_i = 0;
		::vl::vint64_t __vwsn_co3_for_end_i = 0;
		::vl::vint64_t __vwsn_co4_i = 0;
		::vl::vint64_t __vwsn_co_state_ = 0;
		::vl::vint64_t __vwsn_co_state_before_pause_ = 0;
		::vl::Ptr<::vl::reflection::description::IValueException> __vwsn_prop_Failure;
		::vl::Ptr<::vl::reflection::description::IValueException> GetFailure() override;
		void SetFailure(::vl::Ptr<::vl::reflection::description::IValueException> __vwsn_value_);
		::vl::reflection::description::CoroutineStatus __vwsn_prop_Status = static_cast<::vl::reflection::description::CoroutineStatus>(0);
		::vl::reflection::description::CoroutineStatus GetStatus() override;
		void SetStatus(::vl::reflection::description::CoroutineStatus __vwsn_value_);
		void Resume(bool __vwsn_raise_exception_, ::vl::Ptr<::vl::reflection::description::CoroutineResult> __vwsn_co_result_) override;
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
