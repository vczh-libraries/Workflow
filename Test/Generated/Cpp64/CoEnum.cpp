/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/CoEnum.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "CoEnum.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#endif

#define GLOBAL_SYMBOL ::vl_workflow_global::CoEnum::
#define GLOBAL_NAME ::vl_workflow_global::CoEnum::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::CoEnum::Instance()

/***********************************************************************
Global Variables
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_CoEnum)
	vl_workflow_global::CoEnum instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString::Unmanaged(L"");
	FINALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString::Empty;
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_CoEnum)

namespace vl_workflow_global
{
/***********************************************************************
Global Functions
***********************************************************************/

	::vl::collections::LazyList<::vl::vint64_t> CoEnum::GetNumbers()
	{
		return ::vl::reflection::description::GetLazyList<::vl::vint64_t>(::vl::reflection::description::EnumerableCoroutine::Create(vl::Func(::vl_workflow_global::__vwsnf1_CoEnum_GetNumbers_())));
	}

	::vl::WString CoEnum::main()
	{
		{
			auto __vwsn_for_enumerable_i = ::vl::__vwsn::UnboxCollection<::vl::reflection::description::IValueEnumerable>(GLOBAL_NAME GetNumbers());
			auto __vwsn_for_enumerator_i = ::vl::__vwsn::This(__vwsn_for_enumerable_i.Obj())->CreateEnumerator();
			while (::vl::__vwsn::This(__vwsn_for_enumerator_i.Obj())->Next())
			{
				auto i = ::vl::__vwsn::Unbox<::vl::vint64_t>(::vl::__vwsn::This(__vwsn_for_enumerator_i.Obj())->GetCurrent());
				{
					(GLOBAL_NAME s = ((((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"[")) + ::vl::__vwsn::ToString(i)) + ::vl::WString::Unmanaged(L"]")));
				}
			}
		}
		return GLOBAL_NAME s;
	}

	CoEnum& CoEnum::Instance()
	{
		return Getvl_workflow_global_CoEnum().instance;
	}

/***********************************************************************
Closures
***********************************************************************/

	//-------------------------------------------------------------------

	__vwsnf1_CoEnum_GetNumbers_::__vwsnf1_CoEnum_GetNumbers_()
	{
	}

	::vl::Ptr<::vl::reflection::description::ICoroutine> __vwsnf1_CoEnum_GetNumbers_::operator()(::vl::reflection::description::EnumerableCoroutine::IImpl* __vwsn_co_impl_) const
	{
		return ::vl::Ptr<::vl::reflection::description::ICoroutine>(new ::vl_workflow_global::__vwsnc1_CoEnum_GetNumbers___vl_reflection_description_ICoroutine(__vwsn_co_impl_));
	}

	//-------------------------------------------------------------------

	__vwsnc1_CoEnum_GetNumbers___vl_reflection_description_ICoroutine::__vwsnc1_CoEnum_GetNumbers___vl_reflection_description_ICoroutine(::vl::reflection::description::EnumerableCoroutine::IImpl* __vwsnctor___vwsn_co_impl_)
		:__vwsn_co_impl_(__vwsnctor___vwsn_co_impl_)
	{
		this->__vwsn_co0_for_begin_i = static_cast<::vl::vint64_t>(0L);
		this->__vwsn_co1_for_end_i = static_cast<::vl::vint64_t>(0L);
		this->__vwsn_co2_i = static_cast<::vl::vint64_t>(0L);
		this->__vwsn_co_state_ = static_cast<::vl::vint64_t>(0L);
		this->__vwsn_co_state_before_pause_ = (- static_cast<::vl::vint64_t>(1L));
		this->__vwsn_prop_Failure = ::vl::Ptr<::vl::reflection::description::IValueException>();
		this->__vwsn_prop_Status = ::vl::reflection::description::CoroutineStatus::Waiting;
	}

	::vl::Ptr<::vl::reflection::description::IValueException> __vwsnc1_CoEnum_GetNumbers___vl_reflection_description_ICoroutine::GetFailure()
	{
		return __vwsn_prop_Failure;
	}
	void __vwsnc1_CoEnum_GetNumbers___vl_reflection_description_ICoroutine::SetFailure(::vl::Ptr<::vl::reflection::description::IValueException> __vwsn_value_)
	{
		(__vwsn_prop_Failure = __vwsn_value_);
	}

	::vl::reflection::description::CoroutineStatus __vwsnc1_CoEnum_GetNumbers___vl_reflection_description_ICoroutine::GetStatus()
	{
		return __vwsn_prop_Status;
	}
	void __vwsnc1_CoEnum_GetNumbers___vl_reflection_description_ICoroutine::SetStatus(::vl::reflection::description::CoroutineStatus __vwsn_value_)
	{
		(__vwsn_prop_Status = __vwsn_value_);
	}

	void __vwsnc1_CoEnum_GetNumbers___vl_reflection_description_ICoroutine::Resume(bool __vwsn_raise_exception_, ::vl::Ptr<::vl::reflection::description::CoroutineResult> __vwsn_co_result_)
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
						(__vwsn_co0_for_begin_i = static_cast<::vl::vint64_t>(0L));
						(__vwsn_co1_for_end_i = static_cast<::vl::vint64_t>(4L));
						(__vwsn_co2_i = __vwsn_co0_for_begin_i);
						if ((__vwsn_co2_i <= __vwsn_co1_for_end_i))
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
						(__vwsn_co2_i = (__vwsn_co2_i + static_cast<::vl::vint64_t>(1L)));
						if ((__vwsn_co2_i <= __vwsn_co1_for_end_i))
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
						(__vwsn_co_state_ = static_cast<::vl::vint64_t>(2L));
						{
							::vl::reflection::description::EnumerableCoroutine::YieldAndPause(__vwsn_co_impl_, ::vl::__vwsn::Box(__vwsn_co2_i));
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
