#ifndef VCZH_WORKFLOW_TEST_CASES_RPC
#define VCZH_WORKFLOW_TEST_CASES_RPC

#include "CppTypes.h"
#include "RpcDualLifecycleMock.h"
#ifdef VCZH_DEBUG_NO_REFLECTION
#include "RpcDualJsonDispatcherMock.h"
#endif

namespace vl
{
	namespace rpc_controller_test
	{
		class RpcCppLifecycleMock : public RpcDualLifecycleMock
		{
		public:
			vint(*decideTypeIdCallback)(reflection::IDescriptable*) = nullptr;
			Func<void(RpcDualLifecycleMock*, rpc_controller::RpcObjectReference, reflection::IDescriptable*)> attachLocalEventsCallback;
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

template<typename TInstance, bool HasEvent>
void RunRpcTestCase(
	const vl::WString& itemName,
	const vl::WString& expected,
	vl::vint(*decideTypeId)(vl::reflection::IDescriptable*))
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

#ifdef VCZH_DEBUG_NO_REFLECTION
	lc1->SetSerializer(instance.rpcops_IRpcSerializer());
	lc2->SetSerializer(instance.rpcops_IRpcSerializer());
#endif

#ifdef VCZH_DEBUG_NO_REFLECTION
	auto oo1 = instance.rpcops_IRpcObjectOpsJson(lc1.Obj());
	auto oeo1 = instance.rpcops_IRpcObjectEventOpsJson(lc1.Obj());
	auto oo2 = instance.rpcops_IRpcObjectOpsJson(lc2.Obj());
	auto oeo2 = instance.rpcops_IRpcObjectEventOpsJson(lc2.Obj());
	auto ops1 = instance.rpcops_IOps_CreateJson(lc1.Obj());
	auto ops2 = instance.rpcops_IOps_CreateJson(lc2.Obj());
#else
	auto oo1 = instance.rpcops_IRpcObjectOps(lc1.Obj());
	auto oeo1 = instance.rpcops_IRpcObjectEventOps(lc1.Obj());
	auto oo2 = instance.rpcops_IRpcObjectOps(lc2.Obj());
	auto oeo2 = instance.rpcops_IRpcObjectEventOps(lc2.Obj());
	auto ops1 = instance.rpcops_IOps_Create(lc1.Obj());
	auto ops2 = instance.rpcops_IOps_Create(lc2.Obj());
#endif

	auto lo1 = Ptr(new RpcCalleeListOps(lc1.Obj(), lc1->GetSerializer()));
	auto leo1 = Ptr(new RpcCalleeListEventBridge(lc1.Obj(), lc1->GetSerializer()));
	auto lo2 = Ptr(new RpcCalleeListOps(lc2.Obj(), lc2->GetSerializer()));
	auto leo2 = Ptr(new RpcCalleeListEventBridge(lc2.Obj(), lc2->GetSerializer()));

	if constexpr (HasEvent)
	{
		lc1->attachLocalEventsCallback = Func<void(RpcDualLifecycleMock*, RpcObjectReference, IDescriptable*)>([&](RpcDualLifecycleMock* mock, RpcObjectReference ref, IDescriptable* obj)
		{
			instance.rpclistener_Attach(ref.typeId, mock, ref, obj, ops1);
		});
		lc2->attachLocalEventsCallback = Func<void(RpcDualLifecycleMock*, RpcObjectReference, IDescriptable*)>([&](RpcDualLifecycleMock* mock, RpcObjectReference ref, IDescriptable* obj)
		{
			instance.rpclistener_Attach(ref.typeId, mock, ref, obj, ops2);
		});
	}

	lc1->GetController()->Register(oo1, oeo1, lo1, leo1);
	lc2->GetController()->Register(oo2, oeo2, lo2, leo2);
#ifdef VCZH_DEBUG_NO_REFLECTION
	RpcDualJsonDispatcherMock dispatcher(lc1.Obj(), lc2.Obj());
#else
	RpcDualDispatcherMock dispatcher(lc1.Obj(), lc2.Obj());
#endif
	lc1->RegisterWrapperFactory([&](RpcObjectReference ref, IRpcLifecycle* lc) { return instance.rpcwrapper_Create(ref, lc, ops1); });
	lc2->RegisterWrapperFactory([&](RpcObjectReference ref, IRpcLifecycle* lc) { return instance.rpcwrapper_Create(ref, lc, ops2); });

	instance.serviceMain(lc1.Obj());

	auto actual = instance.clientMain(lc2.Obj());

	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);

	dispatcher.Finalize();
#ifdef VCZH_DEBUG_NO_REFLECTION
	dispatcher.DumpJsonValues(itemName);
#endif
}

#endif
