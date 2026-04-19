#include "RequestServiceReflection.h"
#include "PrimitiveTypesReflection.h"
#include "../Source/RpcDualLifecycleMock.h"

using namespace vl;
using namespace vl::collections;
using namespace vl::console;
using namespace vl::reflection;
using namespace vl::reflection::description;
using namespace vl::rpc_controller;
using namespace vl::rpc_controller_test;

void LoadTestCaseRpcTypes()
{
	 LoadRequestServiceTypes();
	 LoadPrimitiveTypesTypes();
}

TEST_FILE
{

TEST_CASE(L"Rpc:RequestService")
{
	class LocalRpcMock : public RpcDualLifecycleMock
	{
	public:
		using RpcDualLifecycleMock::RpcDualLifecycleMock;
		vint DecideTypeId(IDescriptable* obj)const override
		{
			auto result = RpcDualLifecycleMock::DecideTypeId(obj);
			if (result != RpcTypeId_NotFound) return result;
			if (dynamic_cast<::RpcTest::IService*>(obj)) return idMap.Get(L"RpcTest::IService");
			return RpcTypeId_NotFound;
		}
	};

	WString expected = L"Hello";

	auto& instance = ::vl_workflow_global::RequestService::Instance();

	auto idDictObj = instance.rpc_GetIds();
	Dictionary<WString, vint> idMap;
	{
		auto keys = idDictObj->GetKeys();
		auto values = idDictObj->GetValues();
		for (vint i = 0; i < idDictObj->GetCount(); i++)
		{
			auto key = UnboxValue<WString>(keys->Get(i));
			auto val = UnboxValue<vint>(values->Get(i));
			idMap.Set(key, val);
		}
	}

	auto lc1 = Ptr(new LocalRpcMock(1));
	auto lc2 = Ptr(new LocalRpcMock(2));
	auto adapter1 = Ptr(new RpcDualLifeCycleAdapter(lc1.Obj()));
	auto adapter2 = Ptr(new RpcDualLifeCycleAdapter(lc2.Obj()));
	lc1->SetPeer(lc2.Obj());
	lc2->SetPeer(lc1.Obj());
	lc1->SetIdMap(idMap);
	lc2->SetIdMap(idMap);
	lc1->SetAdapter(adapter1.Obj());
	lc2->SetAdapter(adapter2.Obj());

	{
		auto typeId = idMap.Get(L"RpcTest::IService");
		lc1->RegisterWrapperFactory(typeId, [&instance](IRpcLifeCycle* lc) -> Ptr<IDescriptable> { return instance.rpcwrapper_RpcTest__IService(lc); });
		lc2->RegisterWrapperFactory(typeId, [&instance](IRpcLifeCycle* lc) -> Ptr<IDescriptable> { return instance.rpcwrapper_RpcTest__IService(lc); });
	}

	auto lo1 = Ptr(new RpcCalleeListOps(adapter1.Obj()));
	auto leo1 = Ptr(new RpcCalleeListEventBridge(adapter1.Obj()));
	auto lo2 = Ptr(new RpcCalleeListOps(adapter2.Obj()));
	auto leo2 = Ptr(new RpcCalleeListEventBridge(adapter2.Obj()));

	auto oo1 = instance.rpc_IRpcObjectOps(adapter1.Obj());
	auto oeo1 = instance.rpc_IRpcObjectEventOps(adapter1.Obj());
	auto oo2 = instance.rpc_IRpcObjectOps(adapter2.Obj());
	auto oeo2 = instance.rpc_IRpcObjectEventOps(adapter2.Obj());

	lc2->Register(oo1, oeo1, lo1, leo1);
	lc1->Register(oo2, oeo2, lo2, leo2);

	instance.serviceMain(adapter1.Obj());

	auto actual = instance.clientMain(adapter2.Obj());

	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"Rpc:PrimitiveTypes")
{
	class LocalRpcMock : public RpcDualLifecycleMock
	{
	public:
		using RpcDualLifecycleMock::RpcDualLifecycleMock;
		vint DecideTypeId(IDescriptable* obj)const override
		{
			auto result = RpcDualLifecycleMock::DecideTypeId(obj);
			if (result != RpcTypeId_NotFound) return result;
			if (dynamic_cast<::RpcPrimitiveTest::IService*>(obj)) return idMap.Get(L"RpcPrimitiveTest::IService");
			return RpcTypeId_NotFound;
		}
	};

	WString expected = L"[6][12][1.75][2.875][Hi!][false][Autumn][13,27]";

	auto& instance = ::vl_workflow_global::PrimitiveTypes::Instance();

	auto idDictObj = instance.rpc_GetIds();
	Dictionary<WString, vint> idMap;
	{
		auto keys = idDictObj->GetKeys();
		auto values = idDictObj->GetValues();
		for (vint i = 0; i < idDictObj->GetCount(); i++)
		{
			auto key = UnboxValue<WString>(keys->Get(i));
			auto val = UnboxValue<vint>(values->Get(i));
			idMap.Set(key, val);
		}
	}

	auto lc1 = Ptr(new LocalRpcMock(1));
	auto lc2 = Ptr(new LocalRpcMock(2));
	auto adapter1 = Ptr(new RpcDualLifeCycleAdapter(lc1.Obj()));
	auto adapter2 = Ptr(new RpcDualLifeCycleAdapter(lc2.Obj()));
	lc1->SetPeer(lc2.Obj());
	lc2->SetPeer(lc1.Obj());
	lc1->SetIdMap(idMap);
	lc2->SetIdMap(idMap);
	lc1->SetAdapter(adapter1.Obj());
	lc2->SetAdapter(adapter2.Obj());

	{
		auto typeId = idMap.Get(L"RpcPrimitiveTest::IService");
		lc1->RegisterWrapperFactory(typeId, [&instance](IRpcLifeCycle* lc) -> Ptr<IDescriptable> { return instance.rpcwrapper_RpcPrimitiveTest__IService(lc); });
		lc2->RegisterWrapperFactory(typeId, [&instance](IRpcLifeCycle* lc) -> Ptr<IDescriptable> { return instance.rpcwrapper_RpcPrimitiveTest__IService(lc); });
	}

	auto lo1 = Ptr(new RpcCalleeListOps(adapter1.Obj()));
	auto leo1 = Ptr(new RpcCalleeListEventBridge(adapter1.Obj()));
	auto lo2 = Ptr(new RpcCalleeListOps(adapter2.Obj()));
	auto leo2 = Ptr(new RpcCalleeListEventBridge(adapter2.Obj()));

	auto oo1 = instance.rpc_IRpcObjectOps(adapter1.Obj());
	auto oeo1 = instance.rpc_IRpcObjectEventOps(adapter1.Obj());
	auto oo2 = instance.rpc_IRpcObjectOps(adapter2.Obj());
	auto oeo2 = instance.rpc_IRpcObjectEventOps(adapter2.Obj());

	lc2->Register(oo1, oeo1, lo1, leo1);
	lc1->Register(oo2, oeo2, lo2, leo2);

	instance.serviceMain(adapter1.Obj());

	auto actual = instance.clientMain(adapter2.Obj());

	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});
}
