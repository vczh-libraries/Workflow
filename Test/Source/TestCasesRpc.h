#ifndef VCZH_WORKFLOW_TEST_CASES_RPC
#define VCZH_WORKFLOW_TEST_CASES_RPC

#include "CppTypes.h"
#include "RpcDualLifecycleMock.h"

namespace vl
{
	namespace rpc_controller_test
	{
		class RpcCppLifecycleMock : public RpcDualLifecycleMock
		{
		public:
			vint(*decideTypeIdCallback)(reflection::IDescriptable*) = nullptr;
			void(*attachLocalEventsCallback)(RpcDualLifecycleMock*, rpc_controller::RpcObjectReference, reflection::IDescriptable*) = nullptr;
			using RpcDualLifecycleMock::RpcDualLifecycleMock;

			vint DecideTypeId(reflection::IDescriptable* obj)const override
			{
				auto result = RpcDualLifecycleMock::DecideTypeId(obj);
				if (result != rpc_controller::RpcTypeId_NotFound) return result;
				return decideTypeIdCallback(obj);
			}

			void AttachLocalObjectEvents(rpc_controller::RpcObjectReference ref, reflection::IDescriptable* obj)override
			{
				if (!attachLocalEventsCallback) return;
				if (ref.typeId < 0) return;
				attachLocalEventsCallback(this, ref, obj);
			}
		};
	}
}

template<typename TInstance>
void RunRpcTestCase(
	const vl::WString& expected,
	vl::vint(*decideTypeId)(vl::reflection::IDescriptable*),
	void(*attachLocalEvents)(vl::rpc_controller_test::RpcDualLifecycleMock*, vl::rpc_controller::RpcObjectReference, vl::reflection::IDescriptable*))
{
	using namespace vl;
	using namespace vl::collections;
	using namespace vl::console;
	using namespace vl::reflection;
	using namespace vl::reflection::description;
	using namespace vl::rpc_controller;
	using namespace vl::rpc_controller_test;

	auto& instance = TInstance::Instance();

	auto lc1 = Ptr(new RpcCppLifecycleMock(1));
	auto lc2 = Ptr(new RpcCppLifecycleMock(2));
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

	lc1->GetController()->Register(oo1, oeo1, lo1, leo1);
	lc2->GetController()->Register(oo2, oeo2, lo2, leo2);
	RpcDualDispatcherMock dispatcher(lc1.Obj(), lc2.Obj());
	lc1->RegisterWrapperFactory([&](RpcObjectReference ref, IRpcLifecycle* lc) { return instance.rpcwrapper_Create(ref, lc); });
	lc2->RegisterWrapperFactory([&](RpcObjectReference ref, IRpcLifecycle* lc) { return instance.rpcwrapper_Create(ref, lc); });

	instance.serviceMain(lc1.Obj());

	auto actual = instance.clientMain(lc2.Obj());

	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);

	dispatcher.Finalize();
}

#endif
