#ifndef VCZH_WORKFLOW_TEST_CASES_RPC
#define VCZH_WORKFLOW_TEST_CASES_RPC

#include "CppTypes.h"
#include "RpcDualLifecycleMock.h"
#include "RpcDualJsonDispatcherMock.h"

namespace vl
{
	namespace rpc_controller_test
	{
		using namespace vl::collections;
		using namespace vl::console;
		using namespace vl::reflection;
		using namespace vl::reflection::description;
		using namespace vl::rpc_controller;

		class RpcCppLifecycleMock : public RpcDualLifecycleMock
		{
		public:
			vint(*decideTypeIdCallback)(IDescriptable*) = nullptr;
			Func<void(RpcDualLifecycleMock*, RpcObjectReference, IDescriptable*)> attachLocalEventsCallback;
			using RpcDualLifecycleMock::RpcDualLifecycleMock;

			vint DecideTypeId(IDescriptable* obj)const override
			{
				auto result = RpcDualLifecycleMock::DecideTypeId(obj);
				if (result != RpcTypeId_NotFound) return result;
				return decideTypeIdCallback(obj);
			}

			void AttachLocalObjectEvents(RpcObjectReference ref, IDescriptable* obj)override
			{
				if (!attachLocalEventsCallback) return;
				if (ref.typeId < 0) return;
				attachLocalEventsCallback(this, ref, obj);
			}

		};

		template<typename TInstance, bool HasEvent>
		void RunRpcTestCase_JsonValue(
			const WString& itemName,
			const WString& expected,
			vint(*decideTypeId)(IDescriptable*))
		{
			auto& instance = TInstance::Instance();

			auto lc1 = Ptr(new RpcCppLifecycleMock(1));
			auto lc2 = Ptr(new RpcCppLifecycleMock(2));
			auto idMap = UnboxParameter<Dictionary<WString, vint>>(BoxParameter(instance.rpc_GetIds()));
			lc1->SetIdMap(idMap.Ref());
			lc2->SetIdMap(idMap.Ref());
			lc1->decideTypeIdCallback = decideTypeId;
			lc2->decideTypeIdCallback = decideTypeId;

			lc1->SetSerializer(instance.rpcops_IRpcSerializer());
			lc2->SetSerializer(instance.rpcops_IRpcSerializer());

			auto oo1 = instance.rpcops_IRpcObjectOpsJson(lc1.Obj());
			auto oeo1 = instance.rpcops_IRpcObjectEventOpsJson(lc1.Obj());
			auto oo2 = instance.rpcops_IRpcObjectOpsJson(lc2.Obj());
			auto oeo2 = instance.rpcops_IRpcObjectEventOpsJson(lc2.Obj());
			auto ops1 = instance.rpcops_IOps_CreateJson(lc1.Obj());
			auto ops2 = instance.rpcops_IOps_CreateJson(lc2.Obj());

			auto lo1 = Ptr(new RpcCalleeListOps(lc1.Obj(), lc1->GetSerializer()));
			auto leo1 = Ptr(new RpcCalleeListEventOps(lc1.Obj(), lc1->GetSerializer()));
			auto lo2 = Ptr(new RpcCalleeListOps(lc2.Obj(), lc2->GetSerializer()));
			auto leo2 = Ptr(new RpcCalleeListEventOps(lc2.Obj(), lc2->GetSerializer()));
			auto ooForList1 = Ptr(new RpcCalleeObjectOpsForList(lo1, oo1, lc1->GetSerializer()));
			auto oeoForList1 = Ptr(new RpcCalleeObjectEventOpsForList(leo1, oeo1, lc1->GetSerializer()));
			auto ooForList2 = Ptr(new RpcCalleeObjectOpsForList(lo2, oo2, lc2->GetSerializer()));
			auto oeoForList2 = Ptr(new RpcCalleeObjectEventOpsForList(leo2, oeo2, lc2->GetSerializer()));

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

			lc1->GetController()->Register(ooForList1, oeoForList1);
			lc2->GetController()->Register(ooForList2, oeoForList2);

			RpcDualJsonDispatcherMock dispatcher(lc1.Obj(), lc2.Obj());
			lc1->RegisterWrapperFactory([&](RpcObjectReference ref, IRpcLifecycle* lc) { return instance.rpcwrapper_Create(ref, lc, ops1); });
			lc2->RegisterWrapperFactory([&](RpcObjectReference ref, IRpcLifecycle* lc) { return instance.rpcwrapper_Create(ref, lc, ops2); });

			instance.serviceMain(lc1.Obj());
			lc1->Initialize();
			lc2->Initialize();

			auto actual = instance.clientMain(lc2.Obj());

			Console::WriteLine(L"    expected : " + expected);
			Console::WriteLine(L"    actual   : " + actual);
			TEST_ASSERT(actual == expected);

			dispatcher.Finalize();
			dispatcher.DumpJsonValues(itemName);
		}

		template<typename TInstance, bool HasEvent>
		void RunRpcTestCase_JsonRequest(
			const WString& itemName,
			const WString& expected,
			vint(*decideTypeId)(IDescriptable*))
		{
			auto& instance = TInstance::Instance();

			auto lc1 = Ptr(new RpcCppLifecycleMock(1));
			auto lc2 = Ptr(new RpcCppLifecycleMock(2));
			auto idMap = UnboxParameter<Dictionary<WString, vint>>(BoxParameter(instance.rpc_GetIds()));
			lc1->SetIdMap(idMap.Ref());
			lc2->SetIdMap(idMap.Ref());
			lc1->decideTypeIdCallback = decideTypeId;
			lc2->decideTypeIdCallback = decideTypeId;

			lc1->SetSerializer(instance.rpcops_IRpcSerializer());
			lc2->SetSerializer(instance.rpcops_IRpcSerializer());

			auto oo1 = instance.rpcops_IRpcObjectOpsJson(lc1.Obj());
			auto oeo1 = instance.rpcops_IRpcObjectEventOpsJson(lc1.Obj());
			auto oo2 = instance.rpcops_IRpcObjectOpsJson(lc2.Obj());
			auto oeo2 = instance.rpcops_IRpcObjectEventOpsJson(lc2.Obj());
			auto ops1 = instance.rpcops_IOps_CreateJson(lc1.Obj());
			auto ops2 = instance.rpcops_IOps_CreateJson(lc2.Obj());

			auto lo1 = Ptr(new RpcCalleeListOps(lc1.Obj(), lc1->GetSerializer()));
			auto leo1 = Ptr(new RpcCalleeListEventOps(lc1.Obj(), lc1->GetSerializer()));
			auto lo2 = Ptr(new RpcCalleeListOps(lc2.Obj(), lc2->GetSerializer()));
			auto leo2 = Ptr(new RpcCalleeListEventOps(lc2.Obj(), lc2->GetSerializer()));
			auto ooForList1 = Ptr(new RpcCalleeObjectOpsForList(lo1, oo1, lc1->GetSerializer()));
			auto oeoForList1 = Ptr(new RpcCalleeObjectEventOpsForList(leo1, oeo1, lc1->GetSerializer()));
			auto ooForList2 = Ptr(new RpcCalleeObjectOpsForList(lo2, oo2, lc2->GetSerializer()));
			auto oeoForList2 = Ptr(new RpcCalleeObjectEventOpsForList(leo2, oeo2, lc2->GetSerializer()));

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

			lc1->GetController()->Register(ooForList1, oeoForList1);
			lc2->GetController()->Register(ooForList2, oeoForList2);

			RpcDualJsonRequestDispatcherMock dispatcher(lc1.Obj(), lc2.Obj());
			lc1->RegisterWrapperFactory([&](RpcObjectReference ref, IRpcLifecycle* lc) { return instance.rpcwrapper_Create(ref, lc, ops1); });
			lc2->RegisterWrapperFactory([&](RpcObjectReference ref, IRpcLifecycle* lc) { return instance.rpcwrapper_Create(ref, lc, ops2); });

			instance.serviceMain(lc1.Obj());
			lc1->Initialize();
			lc2->Initialize();

			auto actual = instance.clientMain(lc2.Obj());

			Console::WriteLine(L"    expected : " + expected);
			Console::WriteLine(L"    actual   : " + actual);
			TEST_ASSERT(actual == expected);

			dispatcher.Finalize();
			dispatcher.DumpJsonRequests(itemName);
		}

		template<typename TInstance, bool HasEvent>
		void RunRpcTestCase_Flat(
			const WString& itemName,
			const WString& expected,
			vint(*decideTypeId)(IDescriptable*))
		{
			auto& instance = TInstance::Instance();

			auto lc1 = Ptr(new RpcCppLifecycleMock(1));
			auto lc2 = Ptr(new RpcCppLifecycleMock(2));
			auto idMap = UnboxParameter<Dictionary<WString, vint>>(BoxParameter(instance.rpc_GetIds()));
			lc1->SetIdMap(idMap.Ref());
			lc2->SetIdMap(idMap.Ref());
			lc1->decideTypeIdCallback = decideTypeId;
			lc2->decideTypeIdCallback = decideTypeId;

			auto oo1 = instance.rpcops_IRpcObjectOps(lc1.Obj());
			auto oeo1 = instance.rpcops_IRpcObjectEventOps(lc1.Obj());
			auto oo2 = instance.rpcops_IRpcObjectOps(lc2.Obj());
			auto oeo2 = instance.rpcops_IRpcObjectEventOps(lc2.Obj());
			auto ops1 = instance.rpcops_IOps_Create(lc1.Obj());
			auto ops2 = instance.rpcops_IOps_Create(lc2.Obj());

			auto lo1 = Ptr(new RpcCalleeListOps(lc1.Obj(), lc1->GetSerializer()));
			auto leo1 = Ptr(new RpcCalleeListEventOps(lc1.Obj(), lc1->GetSerializer()));
			auto lo2 = Ptr(new RpcCalleeListOps(lc2.Obj(), lc2->GetSerializer()));
			auto leo2 = Ptr(new RpcCalleeListEventOps(lc2.Obj(), lc2->GetSerializer()));
			auto ooForList1 = Ptr(new RpcCalleeObjectOpsForList(lo1, oo1, lc1->GetSerializer()));
			auto oeoForList1 = Ptr(new RpcCalleeObjectEventOpsForList(leo1, oeo1, lc1->GetSerializer()));
			auto ooForList2 = Ptr(new RpcCalleeObjectOpsForList(lo2, oo2, lc2->GetSerializer()));
			auto oeoForList2 = Ptr(new RpcCalleeObjectEventOpsForList(leo2, oeo2, lc2->GetSerializer()));

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

			lc1->GetController()->Register(ooForList1, oeoForList1);
			lc2->GetController()->Register(ooForList2, oeoForList2);

			RpcDualDispatcherMock dispatcher(lc1.Obj(), lc2.Obj());
			lc1->RegisterWrapperFactory([&](RpcObjectReference ref, IRpcLifecycle* lc) { return instance.rpcwrapper_Create(ref, lc, ops1); });
			lc2->RegisterWrapperFactory([&](RpcObjectReference ref, IRpcLifecycle* lc) { return instance.rpcwrapper_Create(ref, lc, ops2); });

			instance.serviceMain(lc1.Obj());
			lc1->Initialize();
			lc2->Initialize();

			auto actual = instance.clientMain(lc2.Obj());

			Console::WriteLine(L"    expected : " + expected);
			Console::WriteLine(L"    actual   : " + actual);
			TEST_ASSERT(actual == expected);

			dispatcher.Finalize();
		}
	}
}

#ifdef VCZH_DEBUG_NO_REFLECTION
#define RunRpcTestCase ::vl::rpc_controller_test::RunRpcTestCase_JsonValue
#elif defined VCZH_DEBUG_METAONLY_REFLECTION
#define RunRpcTestCase ::vl::rpc_controller_test::RunRpcTestCase_JsonRequest
#else
#define RunRpcTestCase ::vl::rpc_controller_test::RunRpcTestCase_Flat
#endif

#endif
