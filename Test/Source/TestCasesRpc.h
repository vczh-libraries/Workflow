#ifndef VCZH_WORKFLOW_TEST_CASES_RPC
#define VCZH_WORKFLOW_TEST_CASES_RPC

#include "CppTypes.h"
#include "RpcDualLifecycleMock.h"

template<typename TInstance>
void RunRpcTestCase(
	const vl::WString& expected,
	vl::vint(*decideTypeId)(vl::reflection::IDescriptable*),
	void(*attachLocalEvents)(vl::rpc_controller_test::RpcDualLifecycleMock*, vl::rpc_controller::RpcObjectReference, vl::reflection::IDescriptable*, vl::collections::List<vl::Func<void()>>&),
	bool(*invokeLocalEvents)(vl::rpc_controller_test::RpcDualLifecycleMock*, vl::rpc_controller::RpcObjectReference, vl::vint, vl::Ptr<vl::reflection::description::IValueArray>))
{
	(void)invokeLocalEvents;

	using namespace vl;
	using namespace vl::collections;
	using namespace vl::console;
	using namespace vl::reflection;
	using namespace vl::reflection::description;
	using namespace vl::rpc_controller;
	using namespace vl::rpc_controller_test;

	class LocalRpcMock : public RpcDualLifecycleMock
	{
	public:
		vint(*decideTypeIdCallback)(IDescriptable*) = nullptr;
		void(*attachLocalEventsCallback)(RpcDualLifecycleMock*, RpcObjectReference, IDescriptable*, List<Func<void()>>&) = nullptr;
		using RpcDualLifecycleMock::RpcDualLifecycleMock;
		void DisconnectTrackedWrappersBeforeDispose()
		{
			DisconnectTrackedWrappers();
		}
		vint DecideTypeId(IDescriptable* obj)const override
		{
			auto result = RpcDualLifecycleMock::DecideTypeId(obj);
			if (result != RpcTypeId_NotFound) return result;
			return decideTypeIdCallback(obj);
		}
		bool AttachLocalObjectEvents(RpcObjectReference ref, IDescriptable* obj, List<Func<void()>>& detachments) override
		{
			if (!attachLocalEventsCallback) return false;
			attachLocalEventsCallback(this, ref, obj, detachments);
			return true;
		}
	};
	auto& instance = TInstance::Instance();

	auto lc1 = Ptr(new LocalRpcMock(1));
	auto lc2 = Ptr(new LocalRpcMock(2));
	auto idMap = UnboxParameter<Dictionary<WString, vint>>(BoxParameter(instance.rpc_GetIds()));
	lc1->SetIdMap(idMap.Ref());
	lc2->SetIdMap(idMap.Ref());
	lc1->decideTypeIdCallback = decideTypeId;
	lc2->decideTypeIdCallback = decideTypeId;
	lc1->attachLocalEventsCallback = attachLocalEvents;
	lc2->attachLocalEventsCallback = attachLocalEvents;

	auto lo1 = Ptr(new RpcCalleeListOps(lc1.Obj()));
	auto leo1 = Ptr(new RpcCalleeListEventBridge(lc1.Obj()));
	auto lo2 = Ptr(new RpcCalleeListOps(lc2.Obj()));
	auto leo2 = Ptr(new RpcCalleeListEventBridge(lc2.Obj()));

	auto oo1 = instance.rpc_IRpcObjectOps(lc1.Obj());
	auto oeo1 = instance.rpc_IRpcObjectEventOps(lc1.Obj());
	auto oo2 = instance.rpc_IRpcObjectOps(lc2.Obj());
	auto oeo2 = instance.rpc_IRpcObjectEventOps(lc2.Obj());
	lc1->RegisterLocalObjectOps(oo1);
	lc2->RegisterLocalObjectOps(oo2);

	lc2->Register(oo1, oeo1, lo1, leo1);
	lc1->Register(oo2, oeo2, lo2, leo2);
	lc1->RegisterWrapperFactory([&](vint typeId, IRpcLifeCycle* lc) { (void)lc; return instance.rpcwrapper_Create(typeId, lc1.Obj()); });
	lc2->RegisterWrapperFactory([&](vint typeId, IRpcLifeCycle* lc) { (void)lc; return instance.rpcwrapper_Create(typeId, lc2.Obj()); });

	instance.serviceMain(lc1.Obj());

	auto actual = instance.clientMain(lc2.Obj());

	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);

	lc2->RegisterLocalObjectOps(nullptr);
	lc1->RegisterLocalObjectOps(nullptr);
	lc2->DisconnectTrackedWrappersBeforeDispose();
	lc1->DisconnectTrackedWrappersBeforeDispose();
}

#endif