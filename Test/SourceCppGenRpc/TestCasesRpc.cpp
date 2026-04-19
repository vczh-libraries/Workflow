#include "RequestServiceReflection.h"
#include "PrimitiveTypesReflection.h"
#include "LocalAndWrapperReflection.h"
#include "ServiceWrapperReflection.h"
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
	 LoadLocalAndWrapperTypes();
	 LoadServiceWrapperTypes();
}

template<typename TInstance>
void RunRpcTestCase(const WString& expected, vint(*decideTypeId)(IDescriptable*, const Dictionary<WString, vint>&))
{
	class LocalRpcMock : public RpcDualLifecycleMock
	{
	public:
		vint(*decideTypeIdCallback)(IDescriptable*, const Dictionary<WString, vint>&) = nullptr;
		using RpcDualLifecycleMock::RpcDualLifecycleMock;
		vint DecideTypeId(IDescriptable* obj)const override
		{
			auto result = RpcDualLifecycleMock::DecideTypeId(obj);
			if (result != RpcTypeId_NotFound) return result;
			return decideTypeIdCallback(obj, idMap);
		}
	};

	auto& instance = TInstance::Instance();

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
	lc1->decideTypeIdCallback = decideTypeId;
	lc2->decideTypeIdCallback = decideTypeId;
	auto adapter1 = Ptr(new RpcDualLifeCycleAdapter(lc1.Obj()));
	auto adapter2 = Ptr(new RpcDualLifeCycleAdapter(lc2.Obj()));
	lc1->SetPeer(lc2.Obj());
	lc2->SetPeer(lc1.Obj());
	lc1->SetIdMap(idMap);
	lc2->SetIdMap(idMap);
	lc1->SetAdapter(adapter1.Obj());
	lc2->SetAdapter(adapter2.Obj());

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
	lc1->RegisterWrapperFactory([&](vint typeId, IRpcLifeCycle* lc) { return instance.rpcwrapper_Create(typeId, lc); });
	lc2->RegisterWrapperFactory([&](vint typeId, IRpcLifeCycle* lc) { return instance.rpcwrapper_Create(typeId, lc); });

	instance.serviceMain(adapter1.Obj());

	auto actual = instance.clientMain(adapter2.Obj());

	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_FILE
{

TEST_CASE(L"Rpc:RequestService")
{
	RunRpcTestCase<::vl_workflow_global::RequestService>(L"Hello",
		[](IDescriptable* obj, const Dictionary<WString, vint>& idMap) -> vint
		{
			if (dynamic_cast<::RpcTest::IService*>(obj)) return idMap.Get(L"RpcTest::IService");
			return RpcTypeId_NotFound;
		});
});

TEST_CASE(L"Rpc:PrimitiveTypes")
{
	RunRpcTestCase<::vl_workflow_global::PrimitiveTypes>(L"[6][12][1.75][2.875][Hi!][false][Autumn][13,27]",
		[](IDescriptable* obj, const Dictionary<WString, vint>& idMap) -> vint
		{
			if (dynamic_cast<::RpcPrimitiveTest::IService*>(obj)) return idMap.Get(L"RpcPrimitiveTest::IService");
			return RpcTypeId_NotFound;
		});
});

TEST_CASE(L"Rpc:LocalAndWrapper")
{
	RunRpcTestCase<::vl_workflow_global::LocalAndWrapper>(L"[false][true][true][false]",
		[](IDescriptable* obj, const Dictionary<WString, vint>& idMap) -> vint
		{
			if (dynamic_cast<::RpcWrapperTest::IObj1*>(obj)) return idMap.Get(L"RpcWrapperTest::IObj1");
			if (dynamic_cast<::RpcWrapperTest::IObj2*>(obj)) return idMap.Get(L"RpcWrapperTest::IObj2");
			if (dynamic_cast<::RpcWrapperTest::IService*>(obj)) return idMap.Get(L"RpcWrapperTest::IService");
			return RpcTypeId_NotFound;
		});
});

TEST_CASE(L"Rpc:ServiceWrapper")
{
	RunRpcTestCase<::vl_workflow_global::ServiceWrapper>(L"[false][true]",
		[](IDescriptable* obj, const Dictionary<WString, vint>& idMap) -> vint
		{
			if (dynamic_cast<::RpcServiceWrapperTest::IService*>(obj)) return idMap.Get(L"RpcServiceWrapperTest::IService");
			return RpcTypeId_NotFound;
		});
});
}
