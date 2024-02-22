#include "WfLibraryPredefined.h"

namespace vl
{
	using namespace collections;

	namespace reflection
	{
		namespace description
		{

/***********************************************************************
CoroutineResult
***********************************************************************/

			Value CoroutineResult::GetResult()
			{
				return result;
			}

			void CoroutineResult::SetResult(const Value& value)
			{
				result = value;
			}

			Ptr<IValueException> CoroutineResult::GetFailure()
			{
				return failure;
			}

			void CoroutineResult::SetFailure(Ptr<IValueException> value)
			{
				failure = value;
			}

/***********************************************************************
EnumerableCoroutine
***********************************************************************/

			class CoroutineEnumerator : public Object, public virtual EnumerableCoroutine::IImpl, public Description<CoroutineEnumerator>
			{
			protected:
				EnumerableCoroutine::Creator		creator;
				Ptr<ICoroutine>						coroutine;
				Value								current;
				vint								index = -1;
				Ptr<IValueEnumerator>				joining;

			public:
				CoroutineEnumerator(const EnumerableCoroutine::Creator& _creator)
					:creator(_creator)
				{
				}

				Value GetCurrent()override
				{
					return current;
				}

				vint GetIndex()override
				{
					return index;
				}

				bool Next()override
				{
					if (!coroutine)
					{
						coroutine = creator(this);
					}

					while (coroutine->GetStatus() == CoroutineStatus::Waiting)
					{
						if (joining)
						{
							if (joining->Next())
							{
								current = joining->GetCurrent();
								index++;
								return true;
							}
							else
							{
								joining = nullptr;
							}
						}

						coroutine->Resume(true, nullptr);
						if (coroutine->GetStatus() != CoroutineStatus::Waiting)
						{
							break;
						}

						if (!joining)
						{
							index++;
							return true;
						}
					}
					return false;
				}

				void OnYield(const Value& value)override
				{
					current = value;
					joining = nullptr;
				}

				void OnJoin(Ptr<IValueEnumerable> value)override
				{
					if (!value)
					{
						throw Exception(L"Cannot join a null collection.");
					}
					current = Value();
					joining = value->CreateEnumerator();
				}
			};

			class CoroutineEnumerable : public Object, public virtual IValueEnumerable, public Description<CoroutineEnumerable>
			{
			protected:
				EnumerableCoroutine::Creator		creator;

			public:
				CoroutineEnumerable(const EnumerableCoroutine::Creator& _creator)
					:creator(_creator)
				{
				}

				Ptr<IValueEnumerator> CreateEnumerator()override
				{
					return Ptr(new CoroutineEnumerator(creator));
				}
			};

			void EnumerableCoroutine::YieldAndPause(IImpl* impl, const Value& value)
			{
				impl->OnYield(value);
			}

			void EnumerableCoroutine::JoinAndPause(IImpl* impl, Ptr<IValueEnumerable> value)
			{
				impl->OnJoin(value);
			}

			void EnumerableCoroutine::ReturnAndExit(IImpl* impl)
			{
			}

			Ptr<IValueEnumerable> EnumerableCoroutine::Create(const Creator& creator)
			{
				return Ptr(new CoroutineEnumerable(creator));
			}

/***********************************************************************
AsyncContext
***********************************************************************/

			AsyncContext::AsyncContext(const Value& _context)
				:context(_context)
			{
			}

			AsyncContext::~AsyncContext()
			{
			}

			bool AsyncContext::IsCancelled()
			{
				SPIN_LOCK(lock)
				{
					return cancelled;
				}
				CHECK_FAIL(L"Not reachable");
			}

			bool AsyncContext::Cancel()
			{
				SPIN_LOCK(lock)
				{
					if (cancelled) return false;
					cancelled = true;
					return true;
				}
				CHECK_FAIL(L"Not reachable");
			}

			const description::Value& AsyncContext::GetContext()
			{
				SPIN_LOCK(lock)
				{
					return context;
				}
				CHECK_FAIL(L"Not reachable");
			}

			void AsyncContext::SetContext(const description::Value& value)
			{
				SPIN_LOCK(lock)
				{
					context = value;
				}
			}

/***********************************************************************
DelayAsync
***********************************************************************/

			class DelayAsync : public Object, public virtual IAsync, public Description<DelayAsync>
			{
			protected:
				SpinLock							lock;
				vint								milliseconds;
				AsyncStatus							status = AsyncStatus::Ready;

			public:
				DelayAsync(vint _milliseconds)
					:milliseconds(_milliseconds)
				{
				}

				AsyncStatus GetStatus()override
				{
					return status;
				}

				bool Execute(const Func<void(Ptr<CoroutineResult>)>& _callback, Ptr<AsyncContext> context)override
				{
					SPIN_LOCK(lock)
					{
						if (status != AsyncStatus::Ready) return false;
						status = AsyncStatus::Executing;
						IAsyncScheduler::GetSchedulerForCurrentThread()->DelayExecute([async = Ptr<DelayAsync>(this), callback = _callback]()
						{
							if (callback)
							{
								callback(nullptr);
							}
						}, milliseconds);
					}
					return true;
				}
			};

			Ptr<IAsync> IAsync::Delay(vint milliseconds)
			{
				return Ptr(new DelayAsync(milliseconds));
			}

/***********************************************************************
FutureAndPromiseAsync
***********************************************************************/

			class FutureAndPromiseAsync : public virtual IFuture, public virtual IPromise, public Description<FutureAndPromiseAsync>
			{
			public:
				SpinLock							lock;
				AsyncStatus							status = AsyncStatus::Ready;
				Ptr<CoroutineResult>				cr;
				Func<void(Ptr<CoroutineResult>)>	callback;

				void ExecuteCallbackAndClear()
				{
					status = AsyncStatus::Stopped;
					if (callback)
					{
						callback(cr);
					}
					cr = nullptr;
					callback = {};
				}

				template<typename F>
				bool Send(F f)
				{
					SPIN_LOCK(lock)
					{
						if (status == AsyncStatus::Stopped || cr) return false;
						cr = Ptr(new CoroutineResult);
						f();
						if (status == AsyncStatus::Executing)
						{
							ExecuteCallbackAndClear();
						}
					}
					return true;
				}

				AsyncStatus GetStatus()override
				{
					return status;
				}

				bool Execute(const Func<void(Ptr<CoroutineResult>)>& _callback, Ptr<AsyncContext> context)override
				{
					SPIN_LOCK(lock)
					{
						if (status != AsyncStatus::Ready) return false;
						callback = _callback;
						if (cr)
						{
							ExecuteCallbackAndClear();
						}
						else
						{
							status = AsyncStatus::Executing;
						}
					}
					return true;
				}

				Ptr<IPromise> GetPromise()override
				{
					return Ptr(this);
				}

				bool SendResult(const Value& result)override
				{
					return Send([=]()
					{
						cr->SetResult(result);
					});
				}

				bool SendFailure(Ptr<IValueException> failure)override
				{
					return Send([=]()
					{
						cr->SetFailure(failure);
					});
				}
			};

			Ptr<IFuture> IFuture::Create()
			{
				return Ptr(new FutureAndPromiseAsync);
			}

/***********************************************************************
IAsyncScheduler
***********************************************************************/

			class AsyncSchedulerMap
			{
			public:
				Dictionary<vint, Ptr<IAsyncScheduler>>		schedulers;
				Ptr<IAsyncScheduler>						defaultScheduler;
			};

			AsyncSchedulerMap* asyncSchedulerMap = nullptr;
			SpinLock asyncSchedulerLock;

#define ENSURE_ASYNC_SCHEDULER_MAP\
			if (!asyncSchedulerMap) asyncSchedulerMap = new AsyncSchedulerMap;

#define DISPOSE_ASYNC_SCHEDULER_MAP_IF_NECESSARY\
			if (asyncSchedulerMap->schedulers.Count() == 0 && !asyncSchedulerMap->defaultScheduler)\
			{\
				delete asyncSchedulerMap;\
				asyncSchedulerMap = nullptr;\
			}\

			void IAsyncScheduler::RegisterDefaultScheduler(Ptr<IAsyncScheduler> scheduler)
			{
				SPIN_LOCK(asyncSchedulerLock)
				{
					ENSURE_ASYNC_SCHEDULER_MAP
					CHECK_ERROR(!asyncSchedulerMap->defaultScheduler, L"IAsyncScheduler::RegisterDefaultScheduler()#A default scheduler has already been registered.");
					asyncSchedulerMap->defaultScheduler = scheduler;
				}
			}

			void IAsyncScheduler::RegisterSchedulerForCurrentThread(Ptr<IAsyncScheduler> scheduler)
			{
				SPIN_LOCK(asyncSchedulerLock)
				{
					ENSURE_ASYNC_SCHEDULER_MAP
					CHECK_ERROR(!asyncSchedulerMap->schedulers.Keys().Contains(Thread::GetCurrentThreadId()), L"IAsyncScheduler::RegisterDefaultScheduler()#A scheduler for this thread has already been registered.");
					asyncSchedulerMap->schedulers.Add(Thread::GetCurrentThreadId(), scheduler);
				}
			}

			Ptr<IAsyncScheduler> IAsyncScheduler::UnregisterDefaultScheduler()
			{
				Ptr<IAsyncScheduler> scheduler;
				SPIN_LOCK(asyncSchedulerLock)
				{
					if (asyncSchedulerMap)
					{
						scheduler = asyncSchedulerMap->defaultScheduler;
						asyncSchedulerMap->defaultScheduler = nullptr;
						DISPOSE_ASYNC_SCHEDULER_MAP_IF_NECESSARY
					}
				}
				return scheduler;
			}

			Ptr<IAsyncScheduler> IAsyncScheduler::UnregisterSchedulerForCurrentThread()
			{
				Ptr<IAsyncScheduler> scheduler;
				SPIN_LOCK(asyncSchedulerLock)
				{
					if (asyncSchedulerMap)
					{
						vint index = asyncSchedulerMap->schedulers.Keys().IndexOf(Thread::GetCurrentThreadId());
						if (index != -1)
						{
							scheduler = asyncSchedulerMap->schedulers.Values()[index];
							asyncSchedulerMap->schedulers.Remove(Thread::GetCurrentThreadId());
						}
						DISPOSE_ASYNC_SCHEDULER_MAP_IF_NECESSARY
					}
				}
				return scheduler;
			}

#undef ENSURE_ASYNC_SCHEDULER_MAP
#undef DISPOSE_ASYNC_SCHEDULER_MAP_IF_NECESSARY

			Ptr<IAsyncScheduler> IAsyncScheduler::GetSchedulerForCurrentThread()
			{
				Ptr<IAsyncScheduler> scheduler;
				SPIN_LOCK(asyncSchedulerLock)
				{
					CHECK_ERROR(asyncSchedulerMap != nullptr, L"IAsyncScheduler::GetSchedulerForCurrentThread()#There is no scheduler registered for the current thread.");
					vint index = asyncSchedulerMap->schedulers.Keys().IndexOf(Thread::GetCurrentThreadId());
					if (index != -1)
					{
						scheduler = asyncSchedulerMap->schedulers.Values()[index];
					}
					else if (asyncSchedulerMap->defaultScheduler)
					{
						scheduler = asyncSchedulerMap->defaultScheduler;
					}
					else
					{
						CHECK_FAIL(L"IAsyncScheduler::GetSchedulerForCurrentThread()#There is no scheduler registered for the current thread.");
					}
				}
				return scheduler;
			}

/***********************************************************************
AsyncCoroutine
***********************************************************************/

			class CoroutineAsync : public Object, public virtual AsyncCoroutine::IImpl, public Description<CoroutineAsync>
			{
			protected:
				Ptr<ICoroutine>						coroutine;
				AsyncCoroutine::Creator				creator;
				Ptr<IAsyncScheduler>				scheduler;
				Func<void(Ptr<CoroutineResult>)>	callback;
				Ptr<AsyncContext>					context;
				Value								result;

			public:
				CoroutineAsync(AsyncCoroutine::Creator _creator)
					:creator(_creator)
				{
				}

				AsyncStatus GetStatus()override
				{
					if (!coroutine)
					{
						return AsyncStatus::Ready;
					}
					else if (coroutine->GetStatus() != CoroutineStatus::Stopped)
					{
						return AsyncStatus::Executing;
					}
					else
					{
						return AsyncStatus::Stopped;
					}
				}

				bool Execute(const Func<void(Ptr<CoroutineResult>)>& _callback, Ptr<AsyncContext> _context)override
				{
					if (coroutine) return false;
					scheduler = IAsyncScheduler::GetSchedulerForCurrentThread();
					callback = _callback;
					context = _context;
					coroutine = creator(this);
					OnContinue(nullptr);
					return true;
				}

				Ptr<IAsyncScheduler> GetScheduler()override
				{
					return scheduler;
				}

				Ptr<AsyncContext> GetContext()override
				{
					if (!context)
					{
						context = Ptr(new AsyncContext);
					}
					return context;
				}

				void OnContinue(Ptr<CoroutineResult> output)override
				{
					scheduler->Execute([async = Ptr<CoroutineAsync>(this), output]()
					{
						async->coroutine->Resume(false, output);
						if (async->coroutine->GetStatus() == CoroutineStatus::Stopped && async->callback)
						{
							auto result = Ptr(new CoroutineResult);
							if (async->coroutine->GetFailure())
							{
								result->SetFailure(async->coroutine->GetFailure());
							}
							else
							{
								result->SetResult(async->result);
							}
							async->callback(result);
						}
					});
				}

				void OnReturn(const Value& value)override
				{
					result = value;
				}
			};
			
			void AsyncCoroutine::AwaitAndRead(IImpl* impl, Ptr<IAsync> value)
			{
				value->Execute([async = Ptr<IImpl>(impl)](auto output)
				{
					async->OnContinue(output);
				}, impl->GetContext());
			}

			void AsyncCoroutine::ReturnAndExit(IImpl* impl, const Value& value)
			{
				impl->OnReturn(value);
			}

			Ptr<AsyncContext> AsyncCoroutine::QueryContext(IImpl* impl)
			{
				return impl->GetContext();
			}

			Ptr<IAsync> AsyncCoroutine::Create(const Creator& creator)
			{
				return Ptr(new CoroutineAsync(creator));
			}
			void AsyncCoroutine::CreateAndRun(const Creator& creator)
			{
				Ptr(new CoroutineAsync(creator))->Execute(
					[](Ptr<CoroutineResult> cr)
					{
						if (cr->GetFailure())
						{
#pragma push_macro("GetMessage")
#if defined GetMessage
#undef GetMessage
#endif
							throw Exception(cr->GetFailure()->GetMessage());
#pragma pop_macro("GetMessage")
						}
					}, nullptr);
			}

/***********************************************************************
StateMachine
***********************************************************************/

			void StateMachine::ResumeStateMachine()
			{
				Ptr<CoroutineResult> previousResult;
				while (true)
				{
					if (stateMachineCoroutine == nullptr)
					{
						if (!stateMachineInitialized)
						{
							throw Exception(L"The state machine has not been initialized.");
						}
						if (stateMachineStopped)
						{
							throw Exception(L"The state machine has been stopped.");
						}
						stateMachineStopped = true;

						if (previousResult)
						{
							if (auto failure = previousResult->GetFailure())
							{
								throw Exception(failure->GetMessage());
							}
						}
						break;
					}
					else if (stateMachineCoroutine->GetStatus() != CoroutineStatus::Stopped)
					{
						auto currentCoroutine = stateMachineCoroutine;
						currentCoroutine->Resume(false, previousResult);
						if (stateMachineCoroutine == currentCoroutine)
						{
							// wait for input
							break;
						}
						else if (currentCoroutine->GetStatus() == CoroutineStatus::Stopped)
						{
							// leave a state machine
							previousResult = Ptr(new CoroutineResult);
							if (auto failure = currentCoroutine->GetFailure())
							{
								previousResult->SetFailure(failure);
							}
						}
						else
						{
							// enter a state machine
						}
					}
				}
			}

			StateMachine::StateMachine()
			{
			}

			StateMachine::~StateMachine()
			{
				FinalizeAggregation();
			}

			CoroutineStatus StateMachine::GetStateMachineStatus()
			{
				if (stateMachineStopped)
				{
					return CoroutineStatus::Stopped;
				}
				else if (stateMachineCoroutine)
				{
					if (stateMachineCoroutine->GetStatus() == CoroutineStatus::Waiting)
					{
						return CoroutineStatus::Waiting;
					}
					else
					{
						return CoroutineStatus::Executing;
					}
				}
				else
				{
					return CoroutineStatus::Waiting;
				}
			}

/***********************************************************************
Sys
***********************************************************************/

			namespace system_sys
			{
				class ReverseEnumerable : public Object, public IValueEnumerable
				{
				protected:
					Ptr<IValueReadonlyList>					list;

					class Enumerator : public Object, public IValueEnumerator
					{
					protected:
						Ptr<IValueReadonlyList>				list;
						vint								index;

					public:
						Enumerator(Ptr<IValueReadonlyList> _list)
							:list(_list), index(_list->GetCount())
						{
						}

						Value GetCurrent()
						{
							return list->Get(index);
						}

						vint GetIndex()
						{
							return list->GetCount() - 1 - index;
						}

						bool Next()
						{
							if (index <= 0) return false;
							index--;
							return true;
						}
					};

				public:
					ReverseEnumerable(Ptr<IValueReadonlyList> _list)
						:list(_list)
					{
					}

					Ptr<IValueEnumerator> CreateEnumerator()override
					{
						return Ptr(new Enumerator(list));
					}
				};
			}

			DateTime Sys::GetLocalTime()
			{
				return DateTime::LocalTime();
			}

			DateTime Sys::GetUtcTime()
			{
				return DateTime::UtcTime();
			}

			DateTime Sys::ToLocalTime(DateTime dt)
			{
				return dt.ToLocalTime();
			}

			DateTime Sys::ToUtcTime(DateTime dt)
			{
				return dt.ToUtcTime();
			}

			DateTime Sys::Forward(DateTime dt, vuint64_t milliseconds)
			{
				return dt.Forward(milliseconds);
			}

			DateTime Sys::Backward(DateTime dt, vuint64_t milliseconds)
			{
				return dt.Backward(milliseconds);
			}

			DateTime Sys::CreateDateTime(vint year, vint month, vint day)
			{
				return DateTime::FromDateTime(year, month, day);
			}

			DateTime Sys::CreateDateTime(vint year, vint month, vint day, vint hour, vint minute, vint second, vint milliseconds)
			{
				return DateTime::FromDateTime(year, month, day, hour, minute, second, milliseconds);
			}

			Ptr<IValueEnumerable> Sys::ReverseEnumerable(Ptr<IValueEnumerable> value)
			{
				auto list = value.Cast<IValueReadonlyList>();
				if (!list)
				{
					list = IValueList::Create(GetLazyList<Value>(value));
				}
				return Ptr(new system_sys::ReverseEnumerable(list));
			}

#define DEFINE_COMPARE(TYPE)\
			vint Sys::Compare(TYPE a, TYPE b)\
			{\
				auto result = a <=> b;\
				if (result < 0) return -1;\
				if (result > 0) return 1;\
				return 0;\
			}\

			REFLECTION_PREDEFINED_PRIMITIVE_TYPES(DEFINE_COMPARE)
			DEFINE_COMPARE(DateTime)
#undef DEFINE_COMPARE

/***********************************************************************
Math
***********************************************************************/

#define DEFINE_MINMAX(TYPE)\
			TYPE Math::Min(TYPE a, TYPE b)\
			{\
				return Sys::Compare(a, b) < 0 ? a : b;\
			}\
			TYPE Math::Max(TYPE a, TYPE b)\
			{\
				return Sys::Compare(a, b) > 0 ? a : b;\
			}\

			REFLECTION_PREDEFINED_PRIMITIVE_TYPES(DEFINE_MINMAX)
			DEFINE_MINMAX(DateTime)
#undef DEFINE_MINMAX

/***********************************************************************
Localization
***********************************************************************/

			Locale Localization::Invariant()
			{
				return Locale::Invariant();
			}

			Locale Localization::System()
			{
				return Locale::SystemDefault();
			}

			Locale Localization::User()
			{
				return Locale::UserDefault();
			}

			collections::LazyList<Locale> Localization::Locales()
			{
				auto result = Ptr(new List<Locale>);
				Locale::Enumerate(*result.Obj());
				return result;
			}

			collections::LazyList<WString> Localization::GetShortDateFormats(Locale locale)
			{
				auto result = Ptr(new List<WString>);
				locale.GetShortDateFormats(*result.Obj());
				return result;
			}

			collections::LazyList<WString> Localization::GetLongDateFormats(Locale locale)
			{
				auto result = Ptr(new List<WString>);
				locale.GetLongDateFormats(*result.Obj());
				return result;
			}

			collections::LazyList<WString> Localization::GetYearMonthDateFormats(Locale locale)
			{
				auto result = Ptr(new List<WString>);
				locale.GetYearMonthDateFormats(*result.Obj());
				return result;
			}

			collections::LazyList<WString> Localization::GetLongTimeFormats(Locale locale)
			{
				auto result = Ptr(new List<WString>);
				locale.GetLongTimeFormats(*result.Obj());
				return result;
			}

			collections::LazyList<WString> Localization::GetShortTimeFormats(Locale locale)
			{
				auto result = Ptr(new List<WString>);
				locale.GetShortTimeFormats(*result.Obj());
				return result;
			}

			WString Localization::GetShortDayOfWeekName(Locale locale, vint dayOfWeek)
			{
				return locale.GetShortDayOfWeekName(dayOfWeek);
			}

			WString Localization::GetLongDayOfWeekName(Locale locale, vint dayOfWeek)
			{
				return locale.GetLongDayOfWeekName(dayOfWeek);
			}

			WString Localization::GetShortMonthName(Locale locale, vint month)
			{
				return locale.GetShortMonthName(month);
			}

			WString Localization::GetLongMonthName(Locale locale, vint month)
			{
				return locale.GetLongMonthName(month);
			}

			WString Localization::FormatDate(Locale locale, const WString& format, DateTime date)
			{
				return locale.FormatDate(format, date);
			}

			WString Localization::FormatTime(Locale locale, const WString& format, DateTime date)
			{
				return locale.FormatTime(format, date);
			}

			WString Localization::FormatNumber(Locale locale, const WString& number)
			{
				return locale.FormatNumber(number);
			}

			WString Localization::FormatCurrency(Locale locale, const WString& number)
			{
				return locale.FormatCurrency(number);
			}
		}
	}
}
