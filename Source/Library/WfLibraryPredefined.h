/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Reflection

Interfaces:
***********************************************************************/

#ifndef VCZH_WORKFLOW_LIBRARY_PREDEFINED
#define VCZH_WORKFLOW_LIBRARY_PREDEFINED

#include <math.h>
#include "../../Import/Vlpp.h"

namespace vl
{
	namespace reflection
	{
		namespace description
		{

/***********************************************************************
Coroutine
***********************************************************************/

			enum class CoroutineStatus
			{
				Waiting,
				Executing,
				Stopped,
			};

			class CoroutineResult : public virtual IDescriptable, public Description<CoroutineResult>
			{
			protected:
				Value									result;
				Ptr<IValueException>					failure;

			public:
				Value									GetResult();
				void									SetResult(const Value& value);
				Ptr<IValueException>					GetFailure();
				void									SetFailure(Ptr<IValueException> value);
			};

			class ICoroutine : public virtual IDescriptable, public Description<ICoroutine>
			{
			public:
				virtual void							Resume(bool raiseException, Ptr<CoroutineResult> output) = 0;
				virtual Ptr<IValueException>			GetFailure() = 0;
				virtual CoroutineStatus					GetStatus() = 0;
			};

/***********************************************************************
Coroutine (Enumerable)
***********************************************************************/

			class EnumerableCoroutine : public Object, public Description<EnumerableCoroutine>
			{
			public:
				class IImpl : public virtual IValueEnumerator, public Description<IImpl>
				{
				public:
					virtual void						OnYield(const Value& value) = 0;
					virtual void						OnJoin(Ptr<IValueEnumerable> value) = 0;
				};

				typedef Func<Ptr<ICoroutine>(IImpl*)>	Creator;

				static void								YieldAndPause(IImpl* impl, const Value& value);
				static void								JoinAndPause(IImpl* impl, Ptr<IValueEnumerable> value);
				static void								ReturnAndExit(IImpl* impl);
				static Ptr<IValueEnumerable>			Create(const Creator& creator);
			};

/***********************************************************************
Coroutine (Async)
***********************************************************************/

			enum class AsyncStatus
			{
				Ready,
				Executing,
				Stopped,
			};

			class AsyncContext : public virtual IDescriptable, public Description<AsyncContext>
			{
			protected:
				SpinLock								lock;
				bool									cancelled = false;
				Value									context;

			public:
				AsyncContext(const Value& _context = {});
				~AsyncContext();

				bool									IsCancelled();
				bool									Cancel();

				const description::Value&				GetContext();
				void									SetContext(const description::Value& value);
			};

			class IAsync : public virtual IDescriptable, public Description<IAsync>
			{
			public:
				virtual AsyncStatus						GetStatus() = 0;
				virtual bool							Execute(const Func<void(Ptr<CoroutineResult>)>& callback, Ptr<AsyncContext> context = nullptr) = 0;

				static Ptr<IAsync>						Delay(vint milliseconds);
			};

			class IPromise : public virtual IDescriptable, public Description<IPromise>
			{
			public:
				virtual bool							SendResult(const Value& result) = 0;
				virtual bool							SendFailure(Ptr<IValueException> failure) = 0;
			};

			class IFuture : public virtual IAsync, public Description<IFuture>
			{
			public:
				virtual Ptr<IPromise>					GetPromise() = 0;

				static Ptr<IFuture>						Create();
			};

			class IAsyncScheduler : public virtual IDescriptable, public Description<IAsyncScheduler>
			{
			public:
				virtual void							Execute(const Func<void()>& callback) = 0;
				virtual void							ExecuteInBackground(const Func<void()>& callback) = 0;
				virtual void							DelayExecute(const Func<void()>& callback, vint milliseconds) = 0;

				static void								RegisterDefaultScheduler(Ptr<IAsyncScheduler> scheduler);
				static void								RegisterSchedulerForCurrentThread(Ptr<IAsyncScheduler> scheduler);
				static Ptr<IAsyncScheduler>				UnregisterDefaultScheduler();
				static Ptr<IAsyncScheduler>				UnregisterSchedulerForCurrentThread();
				static Ptr<IAsyncScheduler>				GetSchedulerForCurrentThread();
			};

			class AsyncCoroutine : public Object, public Description<AsyncCoroutine>
			{
			public:
				class IImpl : public virtual IAsync, public Description<IImpl>
				{
				public:
					virtual Ptr<IAsyncScheduler>		GetScheduler() = 0;
					virtual Ptr<AsyncContext>			GetContext() = 0;
					virtual void						OnContinue(Ptr<CoroutineResult> output) = 0;
					virtual void						OnReturn(const Value& value) = 0;
				};

				typedef Func<Ptr<ICoroutine>(IImpl*)>	Creator;

				static void								AwaitAndRead(IImpl* impl, Ptr<IAsync> value);
				static void								ReturnAndExit(IImpl* impl, const Value& value);
				static Ptr<AsyncContext>				QueryContext(IImpl* impl);
				static Ptr<IAsync>						Create(const Creator& creator);
				static void								CreateAndRun(const Creator& creator);
			};

/***********************************************************************
Coroutine (State Machine)
***********************************************************************/

			class StateMachine : public Object, public AggregatableDescription<StateMachine>
			{
#ifndef VCZH_DEBUG_NO_REFLECTION
				friend struct CustomTypeDescriptorSelector<StateMachine>;
#endif
			protected:
				bool									stateMachineInitialized = false;
				bool									stateMachineStopped = false;
				vint									stateMachineInput = -1;
				Ptr<ICoroutine>							stateMachineCoroutine;

				void									ResumeStateMachine();
			public:
				StateMachine();
				~StateMachine();

				CoroutineStatus							GetStateMachineStatus();
			};

/***********************************************************************
Libraries
***********************************************************************/

			class Sys : public Description<Sys>
			{
			public:
				static vint			Int32ToInt(vint32_t value)							{ return (vint)value; }
				static vint			Int64ToInt(vint64_t value)							{ return (vint)value; }
				static vint32_t		IntToInt32(vint value)								{ return (vint32_t)value; }
				static vint64_t		IntToInt64(vint value)								{ return (vint64_t)value; }
				
				static vuint		UInt32ToUInt(vuint32_t value)						{ return (vuint)value; }
				static vuint		UInt64ToUInt(vuint64_t value)						{ return (vuint)value; }
				static vuint32_t	UIntToUInt32(vuint value)							{ return (vuint32_t)value; }
				static vuint64_t	UIntToUInt64(vuint value)							{ return (vuint64_t)value; }

				static vint			Len(const WString& value)							{ return value.Length(); }
				static WString		Left(const WString& value, vint length)				{ return value.Left(length); }
				static WString		Right(const WString& value, vint length)			{ return value.Right(length); }
				static WString		Mid(const WString& value, vint start, vint length)	{ return value.Sub(start, length); }
				static vint			Find(const WString& value, const WString& substr)	{ return INVLOC.FindFirst(value, substr, Locale::Normalization::None).key; }
				static WString		UCase(const WString& value)							{ return wupper(value); }
				static WString		LCase(const WString& value)							{ return wlower(value); }

				static WString		LoremIpsumTitle(vint bestLength)					{ return vl::LoremIpsumTitle(bestLength); }
				static WString		LoremIpsumSentence(vint bestLength)					{ return vl::LoremIpsumSentence(bestLength); }
				static WString		LoremIpsumParagraph(vint bestLength)				{ return vl::LoremIpsumParagraph(bestLength); }

#define DEFINE_COMPARE(TYPE) static vint Compare(TYPE a, TYPE b);
				REFLECTION_PREDEFINED_PRIMITIVE_TYPES(DEFINE_COMPARE)
				DEFINE_COMPARE(DateTime)
#undef DEFINE_COMPARE

				static DateTime		GetLocalTime();
				static DateTime		GetUtcTime();
				static DateTime		ToLocalTime(DateTime dt);
				static DateTime		ToUtcTime(DateTime dt);
				static DateTime		Forward(DateTime dt, vuint64_t milliseconds);
				static DateTime		Backward(DateTime dt, vuint64_t milliseconds);
				static DateTime		CreateDateTime(vint year, vint month, vint day);
				static DateTime		CreateDateTime(vint year, vint month, vint day, vint hour, vint minute, vint second, vint milliseconds);
				static DateTime		CreateDateTime(vuint64_t filetime);

				static Ptr<IValueEnumerable>		ReverseEnumerable(Ptr<IValueEnumerable> value);
			};

			class Math : public Description<Math>
			{
			public:
				static double		Pi()							{ return ASin(1) * 2; }

				static vint8_t		Abs(vint8_t value)				{ return value > 0 ? value : -value; }
				static vint16_t		Abs(vint16_t value)				{ return value > 0 ? value : -value; }
				static vint32_t		Abs(vint32_t value)				{ return value > 0 ? value : -value; }
				static vint64_t		Abs(vint64_t value)				{ return value > 0 ? value : -value; }
				static float		Abs(float value)				{ return value > 0 ? value : -value; }
				static double		Abs(double value)				{ return value > 0 ? value : -value; }
				
#define DEFINE_MINMAX(TYPE)\
				static TYPE Min(TYPE a, TYPE b);\
				static TYPE Max(TYPE a, TYPE b);\

				REFLECTION_PREDEFINED_PRIMITIVE_TYPES(DEFINE_MINMAX)
				DEFINE_MINMAX(DateTime)
#undef DEFINE_MINMAX

				static double		Sin(double value)				{ return sin(value); }
				static double		Cos(double value)				{ return cos(value); }
				static double		Tan(double value)				{ return tan(value); }
				static double		ASin(double value)				{ return asin(value); }
				static double		ACos(double value)				{ return acos(value); }
				static double		ATan(double value)				{ return atan(value); }
				static double		ATan2(double x, double y)		{ return atan2(y, x); }

				static double		Exp(double value)				{ return exp(value);  }
				static double		LogN(double value)				{ return log(value); }
				static double		Log10(double value)				{ return log10(value); }
				static double		Log(double value, double base)	{ return log(value) / log(base); }
				static double		Pow(double value, double power)	{ return pow(value, power); }
				static double		Ceil(double value)				{ return ceil(value); }
				static double		Floor(double value)				{ return floor(value); }
				static double		Round(double value)				{ return round(value); }
				static double		Trunc(double value)				{ return trunc(value); }
			};

			class Localization : public Description<Localization>
			{
			public:
				static Locale							Invariant();
				static Locale							System();
				static Locale							User();
				static collections::LazyList<Locale>	Locales();

				static collections::LazyList<WString>	GetShortDateFormats(Locale locale);
				static collections::LazyList<WString>	GetLongDateFormats(Locale locale);
				static collections::LazyList<WString>	GetYearMonthDateFormats(Locale locale);
				static collections::LazyList<WString>	GetLongTimeFormats(Locale locale);
				static collections::LazyList<WString>	GetShortTimeFormats(Locale locale);

				static WString							GetShortDayOfWeekName(Locale locale, vint dayOfWeek);
				static WString							GetLongDayOfWeekName(Locale locale, vint dayOfWeek);
				static WString							GetShortMonthName(Locale locale, vint month);
				static WString							GetLongMonthName(Locale locale, vint month);

				static WString							FormatDate(Locale locale, const WString& format, DateTime date);
				static WString							FormatTime(Locale locale, const WString& format, DateTime date);
				static WString							FormatNumber(Locale locale, const WString& number);
				static WString							FormatCurrency(Locale locale, const WString& number);
			};
		}
	}
}

#endif
