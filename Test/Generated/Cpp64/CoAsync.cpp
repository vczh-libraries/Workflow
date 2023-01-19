/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/CoAsync.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "CoAsync.h"
/* CodePack:BeginIgnore() */
#ifndef VCZH_DEBUG_NO_REFLECTION
/* CodePack:ConditionOff(VCZH_DEBUG_NO_REFLECTION, CoAsyncReflection.h) */
#include "CoAsyncReflection.h"
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

#define GLOBAL_SYMBOL ::vl_workflow_global::CoAsync::
#define GLOBAL_NAME ::vl_workflow_global::CoAsync::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::CoAsync::Instance()

/***********************************************************************
Global Variables
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_CoAsync)
	vl_workflow_global::CoAsync instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString::Unmanaged(L"");
	FINALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString::Empty;
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_CoAsync)

namespace vl_workflow_global
{
/***********************************************************************
Global Functions
***********************************************************************/

	::vl::Ptr<::CoAsync_types::IStringAsync> CoAsync::GetStringAsync(::vl::vint64_t x)
	{
		auto __vwsn_co_mixin_source_variable_ = ::vl::reflection::description::AsyncCoroutine::Create(vl::Func(::vl_workflow_global::__vwsnf1_CoAsync_GetStringAsync_(x)));
		return ::vl::Ptr<::CoAsync_types::IStringAsync>(new ::vl_workflow_global::__vwsnc2_CoAsync_GetStringAsync__CoAsync_types_IStringAsync(__vwsn_co_mixin_source_variable_));
	}

	::vl::Ptr<::CoAsync_types::IStringAsync> CoAsync::GetStringAsync2(::vl::vint64_t x)
	{
		auto future = ::vl::reflection::description::IFuture::Create();
		::vl::__vwsn::This(::vl::reflection::description::IAsyncScheduler::GetSchedulerForCurrentThread().Obj())->ExecuteInBackground(vl::Func(::vl_workflow_global::__vwsnf2_CoAsync_GetStringAsync2_(future, x)));
		return ::vl::Ptr<::CoAsync_types::IStringAsync>(new ::vl_workflow_global::__vwsnc3_CoAsync_GetStringAsync2__CoAsync_types_IStringAsync(future));
	}

	void CoAsync::GetMultipleString()
	{
		::vl::reflection::description::AsyncCoroutine::CreateAndRun(vl::Func(::vl_workflow_global::__vwsnf3_CoAsync_GetMultipleString_()));
	}

	::vl::WString CoAsync::main()
	{
		::test::SyncScheduler::Run(vl::Func(::vl_workflow_global::__vwsno4_CoAsync_main_()));
		return GLOBAL_NAME s;
	}

	CoAsync& CoAsync::Instance()
	{
		return Getvl_workflow_global_CoAsync().instance;
	}

/***********************************************************************
Closures
***********************************************************************/

	//-------------------------------------------------------------------

	__vwsnf1_CoAsync_GetStringAsync_::__vwsnf1_CoAsync_GetStringAsync_(::vl::vint64_t __vwsnctor_x)
		:x(__vwsnctor_x)
	{
	}

	::vl::Ptr<::vl::reflection::description::ICoroutine> __vwsnf1_CoAsync_GetStringAsync_::operator()(::vl::reflection::description::AsyncCoroutine::IImpl* __vwsn_co_impl_) const
	{
		return ::vl::Ptr<::vl::reflection::description::ICoroutine>(new ::vl_workflow_global::__vwsnc1_CoAsync_GetStringAsync___vl_reflection_description_ICoroutine(__vwsn_co_impl_, x));
	}

	//-------------------------------------------------------------------

	__vwsnf2_CoAsync_GetStringAsync2_::__vwsnf2_CoAsync_GetStringAsync2_(::vl::Ptr<::vl::reflection::description::IFuture> __vwsnctor_future, ::vl::vint64_t __vwsnctor_x)
		:future(__vwsnctor_future)
		, x(__vwsnctor_x)
	{
	}

	void __vwsnf2_CoAsync_GetStringAsync2_::operator()() const
	{
		(GLOBAL_NAME s = ((((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"[")) + ::vl::__vwsn::ToString(x)) + ::vl::WString::Unmanaged(L"]")));
		::vl::__vwsn::This(::vl::__vwsn::This(this->future.Obj())->GetPromise().Obj())->SendResult(::vl::__vwsn::Box(::vl::__vwsn::ToString(x)));
	}

	//-------------------------------------------------------------------

	__vwsnf3_CoAsync_GetMultipleString_::__vwsnf3_CoAsync_GetMultipleString_()
	{
	}

	::vl::Ptr<::vl::reflection::description::ICoroutine> __vwsnf3_CoAsync_GetMultipleString_::operator()(::vl::reflection::description::AsyncCoroutine::IImpl* __vwsn_co_impl_) const
	{
		return ::vl::Ptr<::vl::reflection::description::ICoroutine>(new ::vl_workflow_global::__vwsnc4_CoAsync_GetMultipleString___vl_reflection_description_ICoroutine(__vwsn_co_impl_));
	}

	//-------------------------------------------------------------------

	__vwsno4_CoAsync_main_::__vwsno4_CoAsync_main_()
	{
	}

	void __vwsno4_CoAsync_main_::operator()() const
	{
		return GLOBAL_NAME GetMultipleString();
	}

	//-------------------------------------------------------------------

	__vwsnc1_CoAsync_GetStringAsync___vl_reflection_description_ICoroutine::__vwsnc1_CoAsync_GetStringAsync___vl_reflection_description_ICoroutine(::vl::reflection::description::AsyncCoroutine::IImpl* __vwsnctor___vwsn_co_impl_, ::vl::vint64_t __vwsnctor_x)
		:__vwsn_co_impl_(__vwsnctor___vwsn_co_impl_)
		, x(__vwsnctor_x)
	{
		this->__vwsn_co_state_ = static_cast<::vl::vint64_t>(0L);
		this->__vwsn_co_state_before_pause_ = (- static_cast<::vl::vint64_t>(1L));
		this->__vwsn_prop_Failure = ::vl::Ptr<::vl::reflection::description::IValueException>();
		this->__vwsn_prop_Status = ::vl::reflection::description::CoroutineStatus::Waiting;
	}

	::vl::Ptr<::vl::reflection::description::IValueException> __vwsnc1_CoAsync_GetStringAsync___vl_reflection_description_ICoroutine::GetFailure()
	{
		return __vwsn_prop_Failure;
	}
	void __vwsnc1_CoAsync_GetStringAsync___vl_reflection_description_ICoroutine::SetFailure(::vl::Ptr<::vl::reflection::description::IValueException> __vwsn_value_)
	{
		(__vwsn_prop_Failure = __vwsn_value_);
	}

	::vl::reflection::description::CoroutineStatus __vwsnc1_CoAsync_GetStringAsync___vl_reflection_description_ICoroutine::GetStatus()
	{
		return __vwsn_prop_Status;
	}
	void __vwsnc1_CoAsync_GetStringAsync___vl_reflection_description_ICoroutine::SetStatus(::vl::reflection::description::CoroutineStatus __vwsn_value_)
	{
		(__vwsn_prop_Status = __vwsn_value_);
	}

	void __vwsnc1_CoAsync_GetStringAsync___vl_reflection_description_ICoroutine::Resume(bool __vwsn_raise_exception_, ::vl::Ptr<::vl::reflection::description::CoroutineResult> __vwsn_co_result_)
	{
		if ((this->GetStatus() != ::vl::reflection::description::CoroutineStatus::Waiting))
		{
			throw ::vl::Exception(::vl::WString::Unmanaged(L"Resume should be called only when the coroutine is in the waiting status."));
		}
		this->SetStatus(::vl::reflection::description::CoroutineStatus::Executing);
		try
		{
			{
				if ((__vwsn_co_state_before_pause_ != (- static_cast<::vl::vint64_t>(1L))))
				{
					if ((! static_cast<bool>(__vwsn_co_result_)))
					{
						(__vwsn_co_state_before_pause_ = (- static_cast<::vl::vint64_t>(1L)));
					}
					else if ((! static_cast<bool>(::vl::__vwsn::This(__vwsn_co_result_.Obj())->GetFailure())))
					{
						(__vwsn_co_state_before_pause_ = (- static_cast<::vl::vint64_t>(1L)));
					}
					else
					{
						{
							(__vwsn_co_state_before_pause_ = (- static_cast<::vl::vint64_t>(1L)));
							throw ::vl::Exception(::vl::__vwsn::This(::vl::__vwsn::This(__vwsn_co_result_.Obj())->GetFailure().Obj())->GetMessage());
						}
					}
				}
				while (true)
				{
					if ((__vwsn_co_state_ == static_cast<::vl::vint64_t>(0L)))
					{
						(GLOBAL_NAME s = ((((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"[")) + ::vl::__vwsn::ToString(x)) + ::vl::WString::Unmanaged(L"]")));
						::vl::reflection::description::AsyncCoroutine::ReturnAndExit(__vwsn_co_impl_, ::CoAsync_types::IStringAsync::StoreResult(::vl::__vwsn::ToString(x)));
						this->SetStatus(::vl::reflection::description::CoroutineStatus::Stopped);
						return;
						(__vwsn_co_state_ = static_cast<::vl::vint64_t>(1L));
						continue;
					}
					if ((__vwsn_co_state_ == static_cast<::vl::vint64_t>(1L)))
					{
						this->SetStatus(::vl::reflection::description::CoroutineStatus::Stopped);
						return;
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

	//-------------------------------------------------------------------

	__vwsnc2_CoAsync_GetStringAsync__CoAsync_types_IStringAsync::__vwsnc2_CoAsync_GetStringAsync__CoAsync_types_IStringAsync(::vl::Ptr<::vl::reflection::description::IAsync> __vwsnctor___vwsn_co_mixin_source_variable_)
	{
		this->__vwsn_mixin_source_ = __vwsnctor___vwsn_co_mixin_source_variable_;
	}

	bool __vwsnc2_CoAsync_GetStringAsync__CoAsync_types_IStringAsync::Execute(const ::vl::Func<void(::vl::Ptr<::vl::reflection::description::CoroutineResult>)>& __vwsn_mixin_parameter_callback, ::vl::Ptr<::vl::reflection::description::AsyncContext> __vwsn_mixin_parameter_context)
	{
		return ::vl::__vwsn::This(__vwsn_mixin_source_.Obj())->Execute(__vwsn_mixin_parameter_callback, __vwsn_mixin_parameter_context);
	}

	::vl::reflection::description::AsyncStatus __vwsnc2_CoAsync_GetStringAsync__CoAsync_types_IStringAsync::GetStatus()
	{
		return ::vl::__vwsn::This(__vwsn_mixin_source_.Obj())->GetStatus();
	}

	//-------------------------------------------------------------------

	__vwsnc3_CoAsync_GetStringAsync2__CoAsync_types_IStringAsync::__vwsnc3_CoAsync_GetStringAsync2__CoAsync_types_IStringAsync(::vl::Ptr<::vl::reflection::description::IFuture> __vwsnctor_future)
	{
		this->__vwsn_mixin_source_ = ::vl::Ptr<::vl::reflection::description::IAsync>(__vwsnctor_future);
	}

	bool __vwsnc3_CoAsync_GetStringAsync2__CoAsync_types_IStringAsync::Execute(const ::vl::Func<void(::vl::Ptr<::vl::reflection::description::CoroutineResult>)>& __vwsn_mixin_parameter_callback, ::vl::Ptr<::vl::reflection::description::AsyncContext> __vwsn_mixin_parameter_context)
	{
		return ::vl::__vwsn::This(__vwsn_mixin_source_.Obj())->Execute(__vwsn_mixin_parameter_callback, __vwsn_mixin_parameter_context);
	}

	::vl::reflection::description::AsyncStatus __vwsnc3_CoAsync_GetStringAsync2__CoAsync_types_IStringAsync::GetStatus()
	{
		return ::vl::__vwsn::This(__vwsn_mixin_source_.Obj())->GetStatus();
	}

	//-------------------------------------------------------------------

	__vwsnc4_CoAsync_GetMultipleString___vl_reflection_description_ICoroutine::__vwsnc4_CoAsync_GetMultipleString___vl_reflection_description_ICoroutine(::vl::reflection::description::AsyncCoroutine::IImpl* __vwsnctor___vwsn_co_impl_)
		:__vwsn_co_impl_(__vwsnctor___vwsn_co_impl_)
	{
		this->__vwsn_co0_x = ::vl::WString::Unmanaged(L"");
		this->__vwsn_co1_y = ::vl::WString::Unmanaged(L"");
		this->__vwsn_co2_for_begin_i = static_cast<::vl::vint64_t>(0L);
		this->__vwsn_co3_for_end_i = static_cast<::vl::vint64_t>(0L);
		this->__vwsn_co4_i = static_cast<::vl::vint64_t>(0L);
		this->__vwsn_co_state_ = static_cast<::vl::vint64_t>(0L);
		this->__vwsn_co_state_before_pause_ = (- static_cast<::vl::vint64_t>(1L));
		this->__vwsn_prop_Failure = ::vl::Ptr<::vl::reflection::description::IValueException>();
		this->__vwsn_prop_Status = ::vl::reflection::description::CoroutineStatus::Waiting;
	}

	::vl::Ptr<::vl::reflection::description::IValueException> __vwsnc4_CoAsync_GetMultipleString___vl_reflection_description_ICoroutine::GetFailure()
	{
		return __vwsn_prop_Failure;
	}
	void __vwsnc4_CoAsync_GetMultipleString___vl_reflection_description_ICoroutine::SetFailure(::vl::Ptr<::vl::reflection::description::IValueException> __vwsn_value_)
	{
		(__vwsn_prop_Failure = __vwsn_value_);
	}

	::vl::reflection::description::CoroutineStatus __vwsnc4_CoAsync_GetMultipleString___vl_reflection_description_ICoroutine::GetStatus()
	{
		return __vwsn_prop_Status;
	}
	void __vwsnc4_CoAsync_GetMultipleString___vl_reflection_description_ICoroutine::SetStatus(::vl::reflection::description::CoroutineStatus __vwsn_value_)
	{
		(__vwsn_prop_Status = __vwsn_value_);
	}

	void __vwsnc4_CoAsync_GetMultipleString___vl_reflection_description_ICoroutine::Resume(bool __vwsn_raise_exception_, ::vl::Ptr<::vl::reflection::description::CoroutineResult> __vwsn_co_result_)
	{
		if ((this->GetStatus() != ::vl::reflection::description::CoroutineStatus::Waiting))
		{
			throw ::vl::Exception(::vl::WString::Unmanaged(L"Resume should be called only when the coroutine is in the waiting status."));
		}
		this->SetStatus(::vl::reflection::description::CoroutineStatus::Executing);
		try
		{
			{
				if ((__vwsn_co_state_before_pause_ != (- static_cast<::vl::vint64_t>(1L))))
				{
					if ((! static_cast<bool>(__vwsn_co_result_)))
					{
						(__vwsn_co_state_before_pause_ = (- static_cast<::vl::vint64_t>(1L)));
					}
					else if ((! static_cast<bool>(::vl::__vwsn::This(__vwsn_co_result_.Obj())->GetFailure())))
					{
						(__vwsn_co_state_before_pause_ = (- static_cast<::vl::vint64_t>(1L)));
					}
					else
					{
						{
							(__vwsn_co_state_before_pause_ = (- static_cast<::vl::vint64_t>(1L)));
							throw ::vl::Exception(::vl::__vwsn::This(::vl::__vwsn::This(__vwsn_co_result_.Obj())->GetFailure().Obj())->GetMessage());
						}
					}
				}
				while (true)
				{
					if ((__vwsn_co_state_ == static_cast<::vl::vint64_t>(0L)))
					{
						(__vwsn_co2_for_begin_i = static_cast<::vl::vint64_t>(0L));
						(__vwsn_co3_for_end_i = static_cast<::vl::vint64_t>(1L));
						(__vwsn_co4_i = __vwsn_co2_for_begin_i);
						if ((__vwsn_co4_i <= __vwsn_co3_for_end_i))
						{
							(__vwsn_co_state_ = static_cast<::vl::vint64_t>(3L));
							continue;
						}
						(__vwsn_co_state_ = static_cast<::vl::vint64_t>(1L));
						continue;
					}
					if ((__vwsn_co_state_ == static_cast<::vl::vint64_t>(1L)))
					{
						this->SetStatus(::vl::reflection::description::CoroutineStatus::Stopped);
						return;
					}
					if ((__vwsn_co_state_ == static_cast<::vl::vint64_t>(2L)))
					{
						if (static_cast<bool>(__vwsn_co_result_))
						{
							if (static_cast<bool>(::vl::__vwsn::This(__vwsn_co_result_.Obj())->GetFailure()))
							{
								throw ::vl::Exception(::vl::__vwsn::This(::vl::__vwsn::This(__vwsn_co_result_.Obj())->GetFailure().Obj())->GetMessage());
							}
						}
						(GLOBAL_NAME s = ((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"!")));
						(__vwsn_co4_i = (__vwsn_co4_i + static_cast<::vl::vint64_t>(1L)));
						if ((__vwsn_co4_i <= __vwsn_co3_for_end_i))
						{
							(__vwsn_co_state_ = static_cast<::vl::vint64_t>(3L));
							continue;
						}
						(__vwsn_co_state_ = static_cast<::vl::vint64_t>(1L));
						continue;
					}
					if ((__vwsn_co_state_ == static_cast<::vl::vint64_t>(3L)))
					{
						this->SetStatus(::vl::reflection::description::CoroutineStatus::Waiting);
						(__vwsn_co_state_before_pause_ = __vwsn_co_state_);
						(__vwsn_co_state_ = static_cast<::vl::vint64_t>(4L));
						{
							::vl::reflection::description::AsyncCoroutine::AwaitAndRead(__vwsn_co_impl_, ::vl::Ptr<::vl::reflection::description::IAsync>(GLOBAL_NAME GetStringAsync(__vwsn_co4_i)));
						}
						return;
					}
					if ((__vwsn_co_state_ == static_cast<::vl::vint64_t>(4L)))
					{
						if (static_cast<bool>(::vl::__vwsn::This(__vwsn_co_result_.Obj())->GetFailure()))
						{
							throw ::vl::Exception(::vl::__vwsn::This(::vl::__vwsn::This(__vwsn_co_result_.Obj())->GetFailure().Obj())->GetMessage());
						}
						(__vwsn_co0_x = ::CoAsync_types::IStringAsync::CastResult(::vl::__vwsn::This(__vwsn_co_result_.Obj())->GetResult()));
						(GLOBAL_NAME s = ((((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"[+")) + __vwsn_co0_x) + ::vl::WString::Unmanaged(L"]")));
						(__vwsn_co_state_ = static_cast<::vl::vint64_t>(5L));
						continue;
					}
					if ((__vwsn_co_state_ == static_cast<::vl::vint64_t>(5L)))
					{
						this->SetStatus(::vl::reflection::description::CoroutineStatus::Waiting);
						(__vwsn_co_state_before_pause_ = __vwsn_co_state_);
						(__vwsn_co_state_ = static_cast<::vl::vint64_t>(6L));
						{
							::vl::reflection::description::AsyncCoroutine::AwaitAndRead(__vwsn_co_impl_, ::vl::Ptr<::vl::reflection::description::IAsync>(GLOBAL_NAME GetStringAsync2(__vwsn_co4_i)));
						}
						return;
					}
					if ((__vwsn_co_state_ == static_cast<::vl::vint64_t>(6L)))
					{
						if (static_cast<bool>(::vl::__vwsn::This(__vwsn_co_result_.Obj())->GetFailure()))
						{
							throw ::vl::Exception(::vl::__vwsn::This(::vl::__vwsn::This(__vwsn_co_result_.Obj())->GetFailure().Obj())->GetMessage());
						}
						(__vwsn_co1_y = ::CoAsync_types::IStringAsync::CastResult(::vl::__vwsn::This(__vwsn_co_result_.Obj())->GetResult()));
						(GLOBAL_NAME s = ((((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"[-")) + __vwsn_co1_y) + ::vl::WString::Unmanaged(L"]")));
						(__vwsn_co_state_ = static_cast<::vl::vint64_t>(7L));
						continue;
					}
					if ((__vwsn_co_state_ == static_cast<::vl::vint64_t>(7L)))
					{
						this->SetStatus(::vl::reflection::description::CoroutineStatus::Waiting);
						(__vwsn_co_state_before_pause_ = __vwsn_co_state_);
						(__vwsn_co_state_ = static_cast<::vl::vint64_t>(2L));
						{
							::vl::reflection::description::AsyncCoroutine::AwaitAndRead(__vwsn_co_impl_, ::vl::reflection::description::IAsync::Delay(static_cast<::vl::vint64_t>(0L)));
						}
						return;
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
Class (::CoAsync_types::IStringAsync)
***********************************************************************/

namespace CoAsync_types
{
	::vl::WString IStringAsync::CastResult(const ::vl::reflection::description::Value& value)
	{
		return ::vl::__vwsn::Unbox<::vl::WString>(value);
	}

	::vl::reflection::description::Value IStringAsync::StoreResult(const ::vl::WString& value)
	{
		return ::vl::__vwsn::Box(value);
	}

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
