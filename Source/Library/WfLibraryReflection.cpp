#include <limits.h>
#include <float.h>
#include "WfLibraryReflection.h"

namespace vl
{
	using namespace collections;
	using namespace regex;

	namespace reflection
	{
		namespace description
		{

/***********************************************************************
TypeName
***********************************************************************/

#ifndef VCZH_DEBUG_NO_REFLECTION

			IMPL_TYPE_INFO_RENAME(vl::reflection::description::Sys, system::Sys)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::Math, system::Math)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::Localization, system::Localization)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::CoroutineStatus, system::CoroutineStatus)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::CoroutineResult, system::CoroutineResult)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::ICoroutine, system::Coroutine)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::EnumerableCoroutine::IImpl, system::EnumerableCoroutine::IImpl)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::EnumerableCoroutine, system::EnumerableCoroutine)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::AsyncStatus, system::AsyncStatus)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::AsyncContext, system::AsyncContext)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IAsync, system::Async)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IPromise, system::Promise)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IFuture, system::Future)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IAsyncScheduler, system::AsyncScheduler)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::AsyncCoroutine::IImpl, system::AsyncCoroutine::IImpl)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::AsyncCoroutine, system::AsyncCoroutine)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::StateMachine, system::StateMachine)

#endif

/***********************************************************************
WfLoadLibraryTypes
***********************************************************************/

#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA

#define _ ,	

			template<>
			struct CustomTypeDescriptorSelector<DescriptableObject>
			{
			public:
				class CustomTypeDescriptorImpl : public TypeDescriptorImpl
				{
				public:
					CustomTypeDescriptorImpl()
						:TypeDescriptorImpl(TypeDescriptorFlags::Class, &TypeInfo<DescriptableObject>::content)
					{
						Description<DescriptableObject>::SetAssociatedTypeDescroptor(this);
					}
					~CustomTypeDescriptorImpl()
					{
						Description<DescriptableObject>::SetAssociatedTypeDescroptor(0);
					}
				protected:
					void LoadInternal()override
					{
					}
				};
			};

			BEGIN_CLASS_MEMBER(Sys)
				CLASS_MEMBER_STATIC_METHOD(Int32ToInt, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(Int64ToInt, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(IntToInt32, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(IntToInt64, { L"value" })

				CLASS_MEMBER_STATIC_METHOD(UInt32ToUInt, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(UInt64ToUInt, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(UIntToUInt32, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(UIntToUInt64, { L"value" })

				CLASS_MEMBER_STATIC_METHOD(Len, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(Left, { L"value" _ L"length" })
				CLASS_MEMBER_STATIC_METHOD(Right, { L"value" _ L"length" })
				CLASS_MEMBER_STATIC_METHOD(Mid, { L"value" _ L"start" _ L"length" })
				CLASS_MEMBER_STATIC_METHOD(Find, { L"value" _ L"substr" })
				CLASS_MEMBER_STATIC_METHOD(UCase, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(LCase, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(LoremIpsumTitle, { L"bestLength" })
				CLASS_MEMBER_STATIC_METHOD(LoremIpsumSentence, { L"bestLength" })
				CLASS_MEMBER_STATIC_METHOD(LoremIpsumParagraph, { L"bestLength" })
				CLASS_MEMBER_STATIC_METHOD(ReverseEnumerable, { L"value" })
#pragma push_macro("CompareString")
#if defined CompareString
#undef CompareString
#endif
#define DEFINE_COMPARE(TYPE) CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Compare, PROTECT_PARAMETERS({L"a" _ L"b"}), vint(*)(TYPE, TYPE))
				REFLECTION_PREDEFINED_PRIMITIVE_TYPES(DEFINE_COMPARE)
				DEFINE_COMPARE(DateTime)
#undef DEFINE_COMPARE
#pragma pop_macro("CompareString")

				CLASS_MEMBER_STATIC_METHOD(GetLocalTime, NO_PARAMETER)
				CLASS_MEMBER_STATIC_METHOD(GetUtcTime, NO_PARAMETER)
				CLASS_MEMBER_STATIC_METHOD(ToLocalTime, { L"dt" })
				CLASS_MEMBER_STATIC_METHOD(ToUtcTime, { L"dt" })
				CLASS_MEMBER_STATIC_METHOD(Forward, { L"dt" _ L"milliseconds" })
				CLASS_MEMBER_STATIC_METHOD(Backward, { L"dt" _ L"milliseconds" })

				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(CreateDateTime, {L"year" _ L"month" _ L"day" }, DateTime(*)(vint, vint, vint))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(CreateDateTime, { L"year" _ L"month" _ L"day" _ L"hour" _ L"minute" _ L"second" _ L"milliseconds" }, DateTime(*)(vint, vint, vint, vint, vint, vint, vint))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(CreateDateTime, { L"filetime" }, DateTime(*)(vuint64_t))
			END_CLASS_MEMBER(Sys)

			BEGIN_CLASS_MEMBER(Math)
				CLASS_MEMBER_STATIC_METHOD(Pi, NO_PARAMETER)

				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Abs, { L"value" }, vint8_t(*)(vint8_t))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Abs, { L"value" }, vint16_t(*)(vint16_t))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Abs, { L"value" }, vint32_t(*)(vint32_t))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Abs, { L"value" }, vint64_t(*)(vint64_t))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Abs, { L"value" }, float(*)(float))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Abs, { L"value" }, double(*)(double))

#define DEFINE_MINMAX(TYPE)\
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Min, PROTECT_PARAMETERS({L"a" _ L"b"}), TYPE(*)(TYPE, TYPE))\
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Max, PROTECT_PARAMETERS({L"a" _ L"b"}), TYPE(*)(TYPE, TYPE))\

				REFLECTION_PREDEFINED_PRIMITIVE_TYPES(DEFINE_MINMAX)
				DEFINE_MINMAX(DateTime)
#undef DEFINE_MINMAX

				CLASS_MEMBER_STATIC_METHOD(Sin, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(Cos, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(Tan, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(ASin, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(ACos, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(ATan, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(ATan2, { L"x" _ L"y" })

				CLASS_MEMBER_STATIC_METHOD(Exp, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(LogN, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(Log10, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(Log, { L"value" _ L"base" })
				CLASS_MEMBER_STATIC_METHOD(Pow, { L"value" _ L"power" })
				CLASS_MEMBER_STATIC_METHOD(Ceil, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(Floor, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(Round, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(Trunc, { L"value" })
			END_CLASS_MEMBER(Math)

			BEGIN_CLASS_MEMBER(Localization)
				CLASS_MEMBER_STATIC_METHOD(Invariant, NO_PARAMETER)
				CLASS_MEMBER_STATIC_METHOD(System, NO_PARAMETER)
				CLASS_MEMBER_STATIC_METHOD(User, NO_PARAMETER)
				CLASS_MEMBER_STATIC_METHOD(Locales, NO_PARAMETER)

				CLASS_MEMBER_STATIC_METHOD(GetShortDateFormats, { L"locale" })
				CLASS_MEMBER_STATIC_METHOD(GetLongDateFormats, { L"locale" })
				CLASS_MEMBER_STATIC_METHOD(GetYearMonthDateFormats, { L"locale" })
				CLASS_MEMBER_STATIC_METHOD(GetLongTimeFormats, { L"locale" })
				CLASS_MEMBER_STATIC_METHOD(GetShortTimeFormats, { L"locale" })

				CLASS_MEMBER_STATIC_METHOD(GetShortDayOfWeekName, { L"locale" _ L"dayOfWeek" })
				CLASS_MEMBER_STATIC_METHOD(GetLongDayOfWeekName, { L"locale" _ L"dayOfWeek" })
				CLASS_MEMBER_STATIC_METHOD(GetShortMonthName, { L"locale" _ L"month" })
				CLASS_MEMBER_STATIC_METHOD(GetLongMonthName, { L"locale" _ L"month" })

				CLASS_MEMBER_STATIC_METHOD(FormatDate, { L"locale" _ L"format" _ L"date" })
				CLASS_MEMBER_STATIC_METHOD(FormatTime, { L"locale" _ L"format" _ L"date" })
				CLASS_MEMBER_STATIC_METHOD(FormatNumber, { L"locale" _ L"number" })
				CLASS_MEMBER_STATIC_METHOD(FormatCurrency, { L"locale" _ L"number" })
			END_CLASS_MEMBER(Localization)

			BEGIN_ENUM_ITEM(CoroutineStatus)
				ENUM_CLASS_ITEM(Waiting)
				ENUM_CLASS_ITEM(Executing)
				ENUM_CLASS_ITEM(Stopped)
			END_ENUM_ITEM(CoroutineStatus)

			BEGIN_INTERFACE_MEMBER(ICoroutine)
				CLASS_MEMBER_METHOD(Resume, { L"raiseException" _ L"output" })
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Failure)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Status)
			END_INTERFACE_MEMBER(ICoroutine)

			BEGIN_CLASS_MEMBER(CoroutineResult)
				CLASS_MEMBER_CONSTRUCTOR(Ptr<CoroutineResult>(), NO_PARAMETER)
				CLASS_MEMBER_PROPERTY_FAST(Result)
				CLASS_MEMBER_PROPERTY_FAST(Failure)
			END_CLASS_MEMBER(CoroutineResult)

			BEGIN_INTERFACE_MEMBER_NOPROXY(EnumerableCoroutine::IImpl)
				CLASS_MEMBER_BASE(IValueEnumerator)
			END_INTERFACE_MEMBER(EnumerableCoroutine::IImpl)

			BEGIN_CLASS_MEMBER(EnumerableCoroutine)
				CLASS_MEMBER_STATIC_METHOD(YieldAndPause, { L"impl" _ L"value" })
				CLASS_MEMBER_STATIC_METHOD(JoinAndPause, { L"impl" _ L"value" })
				CLASS_MEMBER_STATIC_METHOD(ReturnAndExit, { L"impl" })
				CLASS_MEMBER_STATIC_METHOD(Create, { L"creator" })
			END_CLASS_MEMBER(EnumerableCoroutine)

			BEGIN_ENUM_ITEM(AsyncStatus)
				ENUM_CLASS_ITEM(Ready)
				ENUM_CLASS_ITEM(Executing)
				ENUM_CLASS_ITEM(Stopped)
			END_ENUM_ITEM(AsyncStatus)

			BEGIN_CLASS_MEMBER(AsyncContext)
				CLASS_MEMBER_CONSTRUCTOR(Ptr<AsyncContext>(const Value&), {L"context"})
				CLASS_MEMBER_METHOD(IsCancelled, NO_PARAMETER)
				CLASS_MEMBER_METHOD(Cancel, NO_PARAMETER)
				CLASS_MEMBER_PROPERTY_FAST(Context)
			END_CLASS_MEMBER(AsyncContext)

			BEGIN_INTERFACE_MEMBER(IAsync)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Status)
				CLASS_MEMBER_METHOD(Execute, { L"callback" _ L"context" })
				CLASS_MEMBER_STATIC_METHOD(Delay, { L"milliseconds" })
			END_INTERFACE_MEMBER(IAsync)

			BEGIN_INTERFACE_MEMBER_NOPROXY(IPromise)
				CLASS_MEMBER_METHOD(SendResult, { L"result" })
				CLASS_MEMBER_METHOD(SendFailure, { L"failure" })
			END_INTERFACE_MEMBER(IPromise)

			BEGIN_INTERFACE_MEMBER_NOPROXY(IFuture)
				CLASS_MEMBER_BASE(IAsync)
				CLASS_MEMBER_BASE(IPromise)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Promise)
				CLASS_MEMBER_STATIC_METHOD(Create, NO_PARAMETER)
			END_INTERFACE_MEMBER(IFuture)

			BEGIN_INTERFACE_MEMBER_NOPROXY(IAsyncScheduler)
				CLASS_MEMBER_METHOD(Execute, { L"callback" })
				CLASS_MEMBER_METHOD(ExecuteInBackground, { L"callback" })
				CLASS_MEMBER_METHOD(DelayExecute, { L"callback" _ L"milliseconds" })
				CLASS_MEMBER_STATIC_METHOD(GetSchedulerForCurrentThread, NO_PARAMETER)
			END_INTERFACE_MEMBER(IAsyncScheduler)

			BEGIN_INTERFACE_MEMBER_NOPROXY(AsyncCoroutine::IImpl)
				CLASS_MEMBER_BASE(IAsync)
			END_INTERFACE_MEMBER(AsyncCoroutine::IImpl)

			BEGIN_CLASS_MEMBER(AsyncCoroutine)
				CLASS_MEMBER_STATIC_METHOD(AwaitAndRead, { L"impl" _ L"value" })
				CLASS_MEMBER_STATIC_METHOD(ReturnAndExit, { L"impl" _ L"value"})
				CLASS_MEMBER_STATIC_METHOD(QueryContext, { L"impl" })
				CLASS_MEMBER_STATIC_METHOD(Create, { L"creator" })
				CLASS_MEMBER_STATIC_METHOD(CreateAndRun, { L"creator" })
			END_CLASS_MEMBER(AsyncCoroutine)

			BEGIN_CLASS_MEMBER(StateMachine)
				CLASS_MEMBER_FIELD(stateMachineInitialized)
				CLASS_MEMBER_FIELD(stateMachineStopped)
				CLASS_MEMBER_FIELD(stateMachineInput)
				CLASS_MEMBER_FIELD(stateMachineCoroutine)

				CLASS_MEMBER_CONSTRUCTOR(Ptr<StateMachine>(), NO_PARAMETER)
				CLASS_MEMBER_METHOD(ResumeStateMachine, NO_PARAMETER)

				CLASS_MEMBER_PROPERTY_READONLY_FAST(StateMachineStatus)
			END_CLASS_MEMBER(StateMachine)
#undef _

			class WfLibraryTypeLoader : public Object, public ITypeLoader
			{
			public:
				void Load(ITypeManager* manager)override
				{
					WORKFLOW_LIBRARY_TYPES(ADD_TYPE_INFO)
				}

				void Unload(ITypeManager* manager)override
				{
				}
			};

#endif

			bool WfLoadLibraryTypes()
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				ITypeManager* manager = GetGlobalTypeManager();
				if (manager)
				{
					Ptr<ITypeLoader> loader = new WfLibraryTypeLoader;
					return manager->AddTypeLoader(loader);
				}
#endif
				return false;
			}
		}
	}
}
