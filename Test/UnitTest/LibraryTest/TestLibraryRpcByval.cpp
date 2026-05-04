#include "../../Source/RpcDualLifecycleMock.h"
#include "../../../Source/Library/WfLibraryReflection.h"

using namespace vl;
using namespace vl::collections;
using namespace vl::reflection;
using namespace vl::reflection::description;
using namespace vl::rpc_controller;
using namespace vl::rpc_controller_test;

namespace
{
	class StubObjectOps : public Object, public IRpcObjectOps
	{
	private:
		IRpcLifecycle*											lifecycle = nullptr;
	public:
		StubObjectOps(IRpcLifecycle* lc)
			: lifecycle(lc)
		{
		}

		Value InvokeMethod(RpcObjectReference ref, vint methodId, Ptr<IValueArray> arguments)override				{ CHECK_FAIL(L"Not Supported!"); }
		void EndInvokeMethod(vint slot)override																	{ (void)slot; CHECK_FAIL(L"Not Supported!"); }
		void ObjectHold(RpcObjectReference ref, vint remoteClientId, bool hold)override
		{
			if (hold)
			{
				lifecycle->LocalObjectHold(ref, remoteClientId);
			}
			else
			{
				lifecycle->LocalObjectUnhold(ref, remoteClientId);
			}
		}
		void RegisterService(vint typeId, Ptr<IDescriptable> service)override										{ CHECK_FAIL(L"Not Supported!"); }
	};

	class StubObjectEventOps : public Object, public IRpcObjectEventOps
	{
	public:
		void InvokeEvent(RpcObjectReference ref, vint eventId, Ptr<IValueArray> arguments)override { (void)ref; (void)eventId; (void)arguments; CHECK_FAIL(L"Not Supported!"); }
	};

	struct RpcTestContext
	{
		Ptr<RpcDualLifecycleMock>			lifecycle1;
		Ptr<RpcDualLifecycleMock>			lifecycle2;
		Ptr<RpcDualDispatcherMock>			dispatcher;
		Ptr<RpcCalleeListOps>				listOps1;
		Ptr<RpcCalleeListOps>				listOps2;
		Ptr<RpcCalleeListEventBridge>		listEventBridge1;
		Ptr<RpcCalleeListEventBridge>		listEventBridge2;

		RpcTestContext() = default;
		RpcTestContext(const RpcTestContext&) = delete;
		RpcTestContext& operator=(const RpcTestContext&) = delete;

		RpcTestContext(RpcTestContext&& context)
		{
			lifecycle1 = context.lifecycle1;
			lifecycle2 = context.lifecycle2;
			dispatcher = context.dispatcher;
			listOps1 = context.listOps1;
			listOps2 = context.listOps2;
			listEventBridge1 = context.listEventBridge1;
			listEventBridge2 = context.listEventBridge2;
			context.dispatcher = nullptr;
		}

		~RpcTestContext()
		{
			if (dispatcher)
			{
				dispatcher->Finalize();
			}
		}
	};

	RpcTestContext CreateContext()
	{
		RpcTestContext context;
		context.lifecycle1 = Ptr(new RpcDualLifecycleMock(1));
		context.lifecycle2 = Ptr(new RpcDualLifecycleMock(2));
		context.listOps1 = Ptr(new RpcCalleeListOps(context.lifecycle1.Obj(), nullptr));
		context.listOps2 = Ptr(new RpcCalleeListOps(context.lifecycle2.Obj(), nullptr));
		context.listEventBridge1 = Ptr(new RpcCalleeListEventBridge(context.lifecycle1.Obj(), nullptr));
		context.listEventBridge2 = Ptr(new RpcCalleeListEventBridge(context.lifecycle2.Obj(), nullptr));
		context.lifecycle1->GetController()->Register(Ptr(new StubObjectOps(context.lifecycle1.Obj())), Ptr(new StubObjectEventOps), context.listOps1, context.listEventBridge1);
		context.lifecycle2->GetController()->Register(Ptr(new StubObjectOps(context.lifecycle2.Obj())), Ptr(new StubObjectEventOps), context.listOps2, context.listEventBridge2);
		context.dispatcher = Ptr(new RpcDualDispatcherMock(context.lifecycle1.Obj(), context.lifecycle2.Obj()));
		return context;
	}

	RpcObjectReference RegisterRemoteObject(RpcTestContext& context, const Value& reflectedValues)
	{
		auto obj = reflectedValues.GetRawPtr();
		CHECK_ERROR(obj, L"RegisterRemoteObject requires a reflected object.");
		auto desc = dynamic_cast<IDescriptable*>(obj);
		CHECK_ERROR(desc, L"RegisterRemoteObject requires IDescriptable.");
		return context.lifecycle1->PtrToRef(Ptr<IDescriptable>(desc));
	}

	template<typename T>
	Ptr<T> CreateRemoteProxy(RpcTestContext& context, RpcObjectReference ref)
	{
		auto proxyDesc = context.lifecycle2->RefToPtr(ref);
		auto proxy = Ptr(dynamic_cast<T*>(proxyDesc.Obj()));
		CHECK_ERROR(proxy, L"CreateRemoteProxy received an unexpected proxy type.");
		return proxy;
	}
}

TEST_FILE
{
	TEST_CASE(L"RpcByrefArray forwards Set and shrink-only Resize")
	{
		auto context = CreateContext();
		Array<vint> values(4);
		values[0] = 1;
		values[1] = 2;
		values[2] = 3;
		values[3] = 4;

		// Keep reflected wrapper alive
		auto reflectedValues = BoxParameter(values);
		auto ref = RegisterRemoteObject(context, reflectedValues);
		{
			auto proxy = CreateRemoteProxy<IValueArray>(context, ref);

			proxy->Set(1, BoxParameter((vint)42));
			TEST_ASSERT(values[1] == 42);

			proxy->Resize(2);
			TEST_ASSERT(values.Count() == 2);
			TEST_ASSERT(values[0] == 1);
			TEST_ASSERT(values[1] == 42);

			TEST_ERROR(proxy->Resize(3));
		}
		// After proxy destroyed, ObjectHold(false) was sent
	});

	TEST_CASE(L"RpcByrefList forwards Add Insert RemoveAt and Clear")
	{
		auto context = CreateContext();
		List<vint> values;
		values.Add(1);
		values.Add(3);

		auto reflectedValues = BoxParameter(values);
		auto ref = RegisterRemoteObject(context, reflectedValues);
		auto proxy = CreateRemoteProxy<IValueList>(context, ref);

		TEST_ASSERT(proxy->Add(BoxParameter((vint)5)) == 2);
		TEST_ASSERT(values.Count() == 3);
		TEST_ASSERT(values[2] == 5);

		TEST_ASSERT(proxy->Insert(1, BoxParameter((vint)2)) == 1);
		TEST_ASSERT(values.Count() == 4);
		TEST_ASSERT(values[1] == 2);

		TEST_ASSERT(proxy->RemoveAt(0));
		TEST_ASSERT(values.Count() == 3);
		TEST_ASSERT(values[0] == 2);

		proxy->Clear();
		TEST_ASSERT(values.Count() == 0);
	});

	TEST_CASE(L"RpcByrefDictionary forwards Set Get Remove Clear with live views")
	{
		auto context = CreateContext();
		Dictionary<vint, vint> values;
		values.Set(1, 10);
		values.Set(2, 20);

		auto reflectedValues = BoxParameter(values);
		auto ref = RegisterRemoteObject(context, reflectedValues);
		auto proxy = CreateRemoteProxy<IValueDictionary>(context, ref);

		auto keysView = proxy->GetKeys();
		auto valuesView = proxy->GetValues();

		TEST_ASSERT(keysView->GetCount() == 2);
		TEST_ASSERT(UnboxValue<vint>(keysView->Get(0)) == 1);
		TEST_ASSERT(UnboxValue<vint>(keysView->Get(1)) == 2);
		TEST_ASSERT(UnboxValue<vint>(valuesView->Get(0)) == 10);
		TEST_ASSERT(UnboxValue<vint>(valuesView->Get(1)) == 20);

		proxy->Set(BoxParameter((vint)3), BoxParameter((vint)30));
		TEST_ASSERT(values.Count() == 3);
		TEST_ASSERT(values.Get(3) == 30);
		TEST_ASSERT(UnboxValue<vint>(proxy->Get(BoxParameter((vint)3))) == 30);

		TEST_ASSERT(proxy->Remove(BoxParameter((vint)1)));
		TEST_ASSERT(!values.Keys().Contains(1));

		// keysView and valuesView are live views, reflecting current dictionary state
		TEST_ASSERT(keysView->GetCount() == 2);
		TEST_ASSERT(valuesView->GetCount() == 2);
		TEST_ASSERT(UnboxValue<vint>(keysView->Get(0)) == 2);
		TEST_ASSERT(UnboxValue<vint>(keysView->Get(1)) == 3);
		TEST_ASSERT(UnboxValue<vint>(valuesView->Get(0)) == 20);
		TEST_ASSERT(UnboxValue<vint>(valuesView->Get(1)) == 30);

		proxy->Clear();
		TEST_ASSERT(values.Count() == 0);
	});

	TEST_CASE(L"RpcByrefObservableList forwards ItemChanged per operation")
	{
		auto context = CreateContext();
		ObservableList<vint> values;
		values.Add(1);
		values.Add(2);

		auto reflectedValues = BoxParameter(values);
		auto ref = RegisterRemoteObject(context, reflectedValues);

		auto proxy = CreateRemoteProxy<IValueObservableList>(context, ref);
		List<WString> events;
		auto handler = proxy->ItemChanged.Add([&](vint index, vint oldCount, vint newCount)
		{
			events.Add(itow(index) + L"/" + itow(oldCount) + L"/" + itow(newCount));
		});

		proxy->Set(1, BoxParameter((vint)20));
		proxy->Add(BoxParameter((vint)30));
		proxy->Insert(1, BoxParameter((vint)15));
		proxy->RemoveAt(0);
		proxy->Clear();

		TEST_ASSERT(events.Count() == 5);
		TEST_ASSERT(events[0] == L"1/1/1");
		TEST_ASSERT(events[1] == L"2/0/1");
		TEST_ASSERT(events[2] == L"1/0/1");
		TEST_ASSERT(events[3] == L"0/1/0");
		TEST_ASSERT(events[4] == L"0/3/0");
		TEST_ASSERT(values.Count() == 0);

		proxy->ItemChanged.Remove(handler);
	});

	TEST_CASE(L"RpcByrefEnumerator lifecycle releases the remote enumerator")
	{
		auto context = CreateContext();
		List<vint> values;
		values.Add(10);
		values.Add(20);

		auto reflectedValues = BoxParameter(values);
		auto ref = RegisterRemoteObject(context, reflectedValues);
		auto proxy = CreateRemoteProxy<IValueList>(context, ref);
		{
			auto enumerator = proxy->CreateEnumerator();
			TEST_ASSERT(enumerator->Next());
			TEST_ASSERT(UnboxValue<vint>(enumerator->GetCurrent()) == 10);
			TEST_ASSERT(enumerator->Next());
			TEST_ASSERT(UnboxValue<vint>(enumerator->GetCurrent()) == 20);
			TEST_ASSERT(!enumerator->Next());
		}
		// Enumerator destroyed - ObjectHold(false) was sent
	});
}
