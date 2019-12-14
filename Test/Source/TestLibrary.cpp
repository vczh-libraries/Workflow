#include "../../Source/Library/WfLibraryCppHelper.h"
#include "../../Source/Library/WfLibraryPredefined.h"

using namespace vl;
using namespace vl::collections;
using namespace vl::reflection;
using namespace vl::reflection::description;

TEST_FILE
{
	/***********************************************************************
	Coroutine (Enumerable)
	***********************************************************************/

	using EC = EnumerableCoroutine;

	class YieldAndPauseEnum : public Object, public ICoroutine
	{
	public:
		EC::IImpl*				impl;
		CoroutineStatus			status = CoroutineStatus::Waiting;
		vint					current = 0;

		YieldAndPauseEnum(EC::IImpl* _impl)
			:impl(_impl)
		{
		}

		void Resume(bool raiseException, Ptr<CoroutineResult> output)override
		{
			if (current < 5)
			{
				status = CoroutineStatus::Executing;
				EC::YieldAndPause(impl, BoxParameter<vint>(current));
				current++;
				status = CoroutineStatus::Waiting;
			}
			else
			{
				status = CoroutineStatus::Stopped;
			}
		}

		Ptr<IValueException> GetFailure()override
		{
			return nullptr;
		}

		CoroutineStatus GetStatus()override
		{
			return status;
		}
	};

	TEST_CASE(L"Test YieldAndPauseEnum coroutine")
	{
		auto ec = EC::Create([](auto* impl) {return MakePtr<YieldAndPauseEnum>(impl); });
		List<vint> xs;
		CopyFrom(xs, GetLazyList<vint>(ec));
		int ys[] = { 0,1,2,3,4 };
		TEST_ASSERT(CompareEnumerable(xs, From(ys)) == 0);
	});

	class JoinAndPauseEnum : public Object, public ICoroutine
	{
	public:
		EC::IImpl*				impl;
		CoroutineStatus			status = CoroutineStatus::Waiting;
		vint					current = 0;

		JoinAndPauseEnum(EC::IImpl* _impl)
			:impl(_impl)
		{
		}

		void Resume(bool raiseException, Ptr<CoroutineResult> output)override
		{
			if (current < 5)
			{
				status = CoroutineStatus::Executing;
				EC::JoinAndPause(impl, __vwsn::UnboxCollection<IValueEnumerable>(Range<vint>(current, 3)));
				current++;
				status = CoroutineStatus::Waiting;
			}
			else
			{
				status = CoroutineStatus::Stopped;
			}
		}

		Ptr<IValueException> GetFailure()override
		{
			return nullptr;
		}

		CoroutineStatus GetStatus()override
		{
			return status;
		}
	};

	TEST_CASE(L"Test JoinAndPauseEnum coroutine")
	{
		auto ec = EC::Create([](auto* impl) {return MakePtr<JoinAndPauseEnum>(impl); });
		List<vint> xs;
		CopyFrom(xs, GetLazyList<vint>(ec));
		int ys[] = { 0,1,2,1,2,3,2,3,4,3,4,5,4,5,6 };
		TEST_ASSERT(CompareEnumerable(xs, From(ys)) == 0);
	});

	class MixEnum : public Object, public ICoroutine
	{
	public:
		EC::IImpl*				impl;
		CoroutineStatus			status = CoroutineStatus::Waiting;
		vint					current = 0;

		MixEnum(EC::IImpl* _impl)
			:impl(_impl)
		{
		}

		void Resume(bool raiseException, Ptr<CoroutineResult> output)override
		{
			if (current < 5)
			{
				status = CoroutineStatus::Executing;
				if (current % 2 == 0)
				{
					EC::YieldAndPause(impl, BoxParameter<vint>(current));
				}
				else
				{
					EC::JoinAndPause(impl, __vwsn::UnboxCollection<IValueEnumerable>(Range<vint>(current, 3)));
				}
				current++;
				status = CoroutineStatus::Waiting;
			}
			else
			{
				status = CoroutineStatus::Stopped;
			}
		}

		Ptr<IValueException> GetFailure()override
		{
			return nullptr;
		}

		CoroutineStatus GetStatus()override
		{
			return status;
		}
	};

	TEST_CASE(L"Test MixEnum coroutine")
	{
		auto ec = EC::Create([](auto* impl) {return MakePtr<MixEnum>(impl); });
		List<vint> xs;
		CopyFrom(xs, GetLazyList<vint>(ec));
		int ys[] = { 0,1,2,3,2,3,4,5,4 };
		TEST_ASSERT(CompareEnumerable(xs, From(ys)) == 0);
	});

	/***********************************************************************
	Coroutine (Async)
	***********************************************************************/

	using AC = AsyncCoroutine;

	class SyncScheduler : public Object, public IAsyncScheduler, public Description<SyncScheduler>
	{
	public:
		List<Func<void()>>		tasks;

		void Run()
		{
			while (tasks.Count() > 0)
			{
				auto firstTask = tasks[0];
				tasks.RemoveAt(0);
				firstTask();
			}
		}

		void Execute(const Func<void()>& callback)override
		{
			tasks.Add(callback);
		}

		void ExecuteInBackground(const Func<void()>& callback)override
		{
			tasks.Add(callback);
		}

		void DelayExecute(const Func<void()>& callback, vint milliseconds)override
		{
			tasks.Add(callback);
		}
	};

	class EmptyAsync : public Object, public ICoroutine
	{
	public:
		AC::IImpl*				impl;
		CoroutineStatus			status = CoroutineStatus::Waiting;

		EmptyAsync(AC::IImpl* _impl)
			:impl(_impl)
		{
		}

		void Resume(bool raiseException, Ptr<CoroutineResult> output)override
		{
			AC::ReturnAndExit(impl, BoxValue<WString>(L"Empty!"));
			status = CoroutineStatus::Stopped;
		}

		Ptr<IValueException> GetFailure()override
		{
			return nullptr;
		}

		CoroutineStatus GetStatus()override
		{
			return status;
		}

		static Ptr<IAsync> Create()
		{
			return AC::Create([](auto impl) { return MakePtr<EmptyAsync>(impl); });
		}
	};

	class FailAsync : public Object, public ICoroutine
	{
	public:
		AC::IImpl*				impl;
		CoroutineStatus			status = CoroutineStatus::Waiting;

		FailAsync(AC::IImpl* _impl)
			:impl(_impl)
		{
		}

		void Resume(bool raiseException, Ptr<CoroutineResult> output)override
		{
			status = CoroutineStatus::Stopped;
		}

		Ptr<IValueException> GetFailure()override
		{
			return IValueException::Create(L"Fail!");
		}

		CoroutineStatus GetStatus()override
		{
			return status;
		}

		static Ptr<IAsync> Create()
		{
			return AC::Create([](auto impl) { return MakePtr<FailAsync>(impl); });
		}
	};

	TEST_CASE(L"Test EmptyAsync coroutine")
	{
		auto scheduler = MakePtr<SyncScheduler>();
		IAsyncScheduler::RegisterDefaultScheduler(scheduler);
		Ptr<CoroutineResult> cr;
		{
			auto async = EmptyAsync::Create();
			TEST_ASSERT(async->Execute([&](auto output)
			{
				cr = output;
			}) == true);
			TEST_ASSERT(async->Execute({}) == false);
		}
		scheduler->Run();
		TEST_ASSERT(UnboxValue<WString>(cr->GetResult()) == L"Empty!");
		IAsyncScheduler::UnregisterDefaultScheduler();
	});

	TEST_CASE(L"Test FailAsync coroutine")
	{
		auto scheduler = MakePtr<SyncScheduler>();
		IAsyncScheduler::RegisterDefaultScheduler(scheduler);
		Ptr<CoroutineResult> cr;
		{
			auto async = FailAsync::Create();
			TEST_ASSERT(async->Execute([&](auto output)
			{
				cr = output;
			}) == true);
			TEST_ASSERT(async->Execute({}) == false);
		}
		scheduler->Run();
		TEST_ASSERT(cr->GetFailure()->GetMessage() == L"Fail!");
		IAsyncScheduler::UnregisterDefaultScheduler();
	});

	class NestedAsync : public Object, public ICoroutine
	{
	public:
		AC::IImpl*				impl;
		CoroutineStatus			status = CoroutineStatus::Waiting;
		vint					state = 0;
		Ptr<IValueException>	failure;

		NestedAsync(AC::IImpl* _impl)
			:impl(_impl)
		{
		}

		void Resume(bool raiseException, Ptr<CoroutineResult> output)override
		{
			try
			{
				switch (state++)
				{
				case 0:
					AC::AwaitAndRead(impl, EmptyAsync::Create());
					break;
				case 1:
					if (output->GetFailure()) { throw output->GetFailure(); }
					AC::AwaitAndRead(impl, FailAsync::Create());
					break;
				case 2:
					if (output->GetFailure()) { throw output->GetFailure(); }
					AC::ReturnAndExit(impl, BoxValue<WString>(L"Nested!"));
					status = CoroutineStatus::Stopped;
					break;
				}
			}
			catch (Ptr<IValueException> ex)
			{
				failure = ex;
				status = CoroutineStatus::Stopped;
			}
		}

		Ptr<IValueException> GetFailure()override
		{
			return failure;
		}

		CoroutineStatus GetStatus()override
		{
			return status;
		}

		static Ptr<IAsync> Create()
		{
			return AC::Create([](auto impl) { return MakePtr<NestedAsync>(impl); });
		}
	};

	TEST_CASE(L"Test NestedAsync coroutine")
	{
		auto scheduler = MakePtr<SyncScheduler>();
		Ptr<CoroutineResult> cr;
		IAsyncScheduler::RegisterSchedulerForCurrentThread(scheduler);
		{
			auto async = NestedAsync::Create();
			TEST_ASSERT(async->Execute([&](auto output)
			{
				cr = output;
			}) == true);
			TEST_ASSERT(async->Execute({}) == false);
		}
		scheduler->Run();
		TEST_ASSERT(cr->GetFailure()->GetMessage() == L"Fail!");
		IAsyncScheduler::UnregisterSchedulerForCurrentThread();
	});

	class DelayAsync : public Object, public ICoroutine
	{
	public:
		AC::IImpl*				impl;
		CoroutineStatus			status = CoroutineStatus::Waiting;
		vint					state = 0;

		DelayAsync(AC::IImpl* _impl)
			:impl(_impl)
		{
		}

		void Resume(bool raiseException, Ptr<CoroutineResult> output)override
		{
			switch (state++)
			{
			case 0:
				AC::AwaitAndRead(impl, IAsync::Delay(0));
				break;
			case 1:
				AC::ReturnAndExit(impl, BoxValue<WString>(L"Delay!"));
				status = CoroutineStatus::Stopped;
				break;
			}
		}

		Ptr<IValueException> GetFailure()override
		{
			return nullptr;
		}

		CoroutineStatus GetStatus()override
		{
			return status;
		}

		static Ptr<IAsync> Create()
		{
			return AC::Create([](auto impl) { return MakePtr<DelayAsync>(impl); });
		}
	};

	TEST_CASE(L"Test DelayAsync coroutine")
	{
		auto scheduler = MakePtr<SyncScheduler>();
		Ptr<CoroutineResult> cr;
		IAsyncScheduler::RegisterSchedulerForCurrentThread(scheduler);
		{
			auto async = DelayAsync::Create();
			TEST_ASSERT(async->Execute([&](auto output)
			{
				cr = output;
			}) == true);
			TEST_ASSERT(async->Execute({}) == false);
		}
		scheduler->Run();
		TEST_ASSERT(UnboxValue<WString>(cr->GetResult()) == L"Delay!");
		IAsyncScheduler::UnregisterSchedulerForCurrentThread();
	});

	TEST_CASE(L"Test succeeded Future/Promise")
	{
		auto scheduler = MakePtr<SyncScheduler>();
		Ptr<CoroutineResult> cr;
		IAsyncScheduler::RegisterSchedulerForCurrentThread(scheduler);
		{
			auto future = IFuture::Create();
			auto promise = future->GetPromise();
			scheduler->ExecuteInBackground([=]()
			{
				TEST_ASSERT(promise->SendResult(BoxValue<WString>(L"Future!")) == true);
				TEST_ASSERT(promise->SendResult(BoxValue<WString>(L"Future!")) == false);
			});
			scheduler->ExecuteInBackground([=, &cr]()
			{
				TEST_ASSERT(future->Execute([&](auto output)
				{
					cr = output;
				}) == true);
				TEST_ASSERT(future->Execute({}) == false);
			});
		}
		scheduler->Run();
		TEST_ASSERT(UnboxValue<WString>(cr->GetResult()) == L"Future!");
		IAsyncScheduler::UnregisterSchedulerForCurrentThread();
	});

	TEST_CASE(L"Test failed Future/Promise")
	{
		auto scheduler = MakePtr<SyncScheduler>();
		Ptr<CoroutineResult> cr;
		IAsyncScheduler::RegisterSchedulerForCurrentThread(scheduler);
		{
			auto future = IFuture::Create();
			scheduler->ExecuteInBackground([=, &cr]()
			{
				TEST_ASSERT(future->Execute([&](auto output)
				{
					cr = output;
				}) == true);
				TEST_ASSERT(future->Execute({}) == false);
			});
			auto promise = future->GetPromise();
			scheduler->ExecuteInBackground([=]()
			{
				TEST_ASSERT(promise->SendFailure(IValueException::Create(L"Promise!")) == true);
				TEST_ASSERT(promise->SendFailure(IValueException::Create(L"Promise!")) == false);
			});
		}
		scheduler->Run();
		TEST_ASSERT(cr->GetFailure()->GetMessage() == L"Promise!");
		IAsyncScheduler::UnregisterSchedulerForCurrentThread();
	});
}