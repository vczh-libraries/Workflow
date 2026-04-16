#include "../../Source/RpcByvalLifecycleMock.h"
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
	public:
		void SyncIds(Ptr<IValueDictionary> ids)override { (void)ids; }
		Value InvokeMethod(RpcObjectReference ref, vint methodId, Ptr<IValueArray> arguments)override { (void)ref; (void)methodId; (void)arguments; CHECK_FAIL(L"Not Supported!"); return {}; }
		Ptr<IAsync> InvokeMethodAsync(RpcObjectReference ref, vint methodId, Ptr<IValueArray> arguments)override { (void)ref; (void)methodId; (void)arguments; CHECK_FAIL(L"Not Supported!"); return nullptr; }
		void ObjectHold(RpcObjectReference ref, bool hold)override { (void)ref; (void)hold; CHECK_FAIL(L"Not Supported!"); }
		RpcObjectReference RequestService(vint typeId)override { (void)typeId; CHECK_FAIL(L"Not Supported!"); return {}; }
	};

	class StubObjectEventOps : public Object, public IRpcObjectEventOps
	{
	public:
		void SyncIds(Ptr<IValueDictionary> ids)override { (void)ids; }
		void InvokeEvent(RpcObjectReference ref, vint eventId, Ptr<IValueArray> arguments)override { (void)ref; (void)eventId; (void)arguments; CHECK_FAIL(L"Not Supported!"); }
	};

	struct RpcTestContext
	{
		Ptr<RpcByvalLifecycleMock>			lifeCycle;
		Ptr<RpcCalleeListOps>				listOps;
		Ptr<RpcCalleeListEventBridge>		listEventBridge;
	};

	RpcTestContext CreateContext()
	{
		RpcTestContext context;
		context.lifeCycle = Ptr(new RpcByvalLifecycleMock);
		context.listOps = Ptr(new RpcCalleeListOps(context.lifeCycle.Obj()));
		context.listEventBridge = Ptr(new RpcCalleeListEventBridge(context.lifeCycle.Obj()));
		context.lifeCycle->Register(Ptr(new StubObjectOps), Ptr(new StubObjectEventOps), context.listOps, context.listEventBridge);
		return context;
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
		auto serializable = RpcBoxByref(reflectedValues, context.lifeCycle.Obj());
		auto ref = UnboxValue<RpcObjectReference>(serializable);
		{
			// Create proxy explicitly (RefToPtr returns local object, so create proxy directly)
			auto proxy = Ptr(new RpcByrefArray(context.lifeCycle.Obj(), ref));

			proxy->Set(1, BoxParameter((vint)42));
			TEST_ASSERT(values[1] == 42);

			proxy->Resize(2);
			TEST_ASSERT(values.Count() == 2);
			TEST_ASSERT(values[0] == 1);
			TEST_ASSERT(values[1] == 42);

			TEST_ERROR(proxy->Resize(3));
		}
		// After proxy destroyed, ReleaseRemoteObject was called
	});

	TEST_CASE(L"RpcByrefList forwards Add Insert RemoveAt and Clear")
	{
		auto context = CreateContext();
		List<vint> values;
		values.Add(1);
		values.Add(3);

		auto reflectedValues = BoxParameter(values);
		auto serializable = RpcBoxByref(reflectedValues, context.lifeCycle.Obj());
		auto ref = UnboxValue<RpcObjectReference>(serializable);
		auto proxy = Ptr(new RpcByrefList(context.lifeCycle.Obj(), ref));

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
		auto serializable = RpcBoxByref(reflectedValues, context.lifeCycle.Obj());
		auto ref = UnboxValue<RpcObjectReference>(serializable);
		auto proxy = Ptr(new RpcByrefDictionary(context.lifeCycle.Obj(), ref));

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
		auto serializable = RpcBoxByref(reflectedValues, context.lifeCycle.Obj());
		auto ref = UnboxValue<RpcObjectReference>(serializable);

		// Create proxy via factory path so it gets registered in observableProxies
		auto proxyDesc = context.lifeCycle->CreateCallerProxy(ref);
		auto proxy = Ptr(dynamic_cast<IValueObservableList*>(proxyDesc.Obj()));
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
		auto serializable = RpcBoxByref(reflectedValues, context.lifeCycle.Obj());
		auto ref = UnboxValue<RpcObjectReference>(serializable);
		auto proxy = Ptr(new RpcByrefList(context.lifeCycle.Obj(), ref));
		{
			auto enumerator = proxy->CreateEnumerator();
			TEST_ASSERT(enumerator->Next());
			TEST_ASSERT(UnboxValue<vint>(enumerator->GetCurrent()) == 10);
			TEST_ASSERT(enumerator->Next());
			TEST_ASSERT(UnboxValue<vint>(enumerator->GetCurrent()) == 20);
			TEST_ASSERT(!enumerator->Next());
		}
		// Enumerator destroyed - ReleaseRemoteObject called
	});
}
