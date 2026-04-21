#include "RequestServiceReflection.h"
#include "PrimitiveTypesReflection.h"
#include "LocalAndWrapperReflection.h"
#include "ServiceWrapperReflection.h"
#include "DtorReflection.h"
#include "Dtor2Reflection.h"
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
	 LoadDtorTypes();
	 LoadDtor2Types();
}

template<typename TInstance>
void RunRpcTestCase(const WString& expected, void(*registerTypeIds)(const Func<void(const WString&, vint)>&), vint(*decideTypeId)(IDescriptable*))
{
	class LocalRpcMock : public RpcDualLifecycleMock
	{
	public:
		vint(*decideTypeIdCallback)(IDescriptable*) = nullptr;
		using RpcDualLifecycleMock::RpcDualLifecycleMock;
		void RegisterTypeId(const WString& fullName, vint typeId)
		{
			idMap.Set(fullName, typeId);
		}
		vint DecideTypeId(IDescriptable* obj)const override
		{
			auto result = RpcDualLifecycleMock::DecideTypeId(obj);
			if (result != RpcTypeId_NotFound) return result;
			return decideTypeIdCallback(obj);
		}
	};

	auto& instance = TInstance::Instance();

	auto lc1 = Ptr(new LocalRpcMock(1));
	auto lc2 = Ptr(new LocalRpcMock(2));
	auto registerTypeId1 = Func<void(const WString&, vint)>([&](const WString& fullName, vint typeId) { lc1->RegisterTypeId(fullName, typeId); });
	auto registerTypeId2 = Func<void(const WString&, vint)>([&](const WString& fullName, vint typeId) { lc2->RegisterTypeId(fullName, typeId); });
	registerTypeIds(registerTypeId1);
	registerTypeIds(registerTypeId2);
	lc1->decideTypeIdCallback = decideTypeId;
	lc2->decideTypeIdCallback = decideTypeId;
	auto adapter1 = Ptr(new RpcDualLifeCycleAdapter(lc1.Obj()));
	auto adapter2 = Ptr(new RpcDualLifeCycleAdapter(lc2.Obj()));
	lc1->SetPeer(lc2.Obj());
	lc2->SetPeer(lc1.Obj());
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
		[](const Func<void(const WString&, vint)>& registerTypeId)
		{
			auto& instance = ::vl_workflow_global::RequestService::Instance();
			registerTypeId(L"RpcTest::IService", instance.rpctype_RpcTest__IService);
		},
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::RequestService::Instance();
			if (dynamic_cast<::RpcTest::IService*>(obj)) return instance.rpctype_RpcTest__IService;
			return RpcTypeId_NotFound;
		});
});

TEST_CASE(L"Rpc:PrimitiveTypes")
{
	RunRpcTestCase<::vl_workflow_global::PrimitiveTypes>(L"[6][12][1.75][2.875][Hi!][false][Autumn][13,27]",
		[](const Func<void(const WString&, vint)>& registerTypeId)
		{
			auto& instance = ::vl_workflow_global::PrimitiveTypes::Instance();
			registerTypeId(L"RpcPrimitiveTest::IService", instance.rpctype_RpcPrimitiveTest__IService);
		},
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::PrimitiveTypes::Instance();
			if (dynamic_cast<::RpcPrimitiveTest::IService*>(obj)) return instance.rpctype_RpcPrimitiveTest__IService;
			return RpcTypeId_NotFound;
		});
});

TEST_CASE(L"Rpc:LocalAndWrapper")
{
	RunRpcTestCase<::vl_workflow_global::LocalAndWrapper>(L"[false][true][true][false]",
		[](const Func<void(const WString&, vint)>& registerTypeId)
		{
			auto& instance = ::vl_workflow_global::LocalAndWrapper::Instance();
			registerTypeId(L"RpcWrapperTest::IObj1", instance.rpctype_RpcWrapperTest__IObj1);
			registerTypeId(L"RpcWrapperTest::IObj2", instance.rpctype_RpcWrapperTest__IObj2);
			registerTypeId(L"RpcWrapperTest::IService", instance.rpctype_RpcWrapperTest__IService);
		},
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::LocalAndWrapper::Instance();
			if (dynamic_cast<::RpcWrapperTest::IObj1*>(obj)) return instance.rpctype_RpcWrapperTest__IObj1;
			if (dynamic_cast<::RpcWrapperTest::IObj2*>(obj)) return instance.rpctype_RpcWrapperTest__IObj2;
			if (dynamic_cast<::RpcWrapperTest::IService*>(obj)) return instance.rpctype_RpcWrapperTest__IService;
			return RpcTypeId_NotFound;
		});
});

TEST_CASE(L"Rpc:ServiceWrapper")
{
	RunRpcTestCase<::vl_workflow_global::ServiceWrapper>(L"[false][true]",
		[](const Func<void(const WString&, vint)>& registerTypeId)
		{
			auto& instance = ::vl_workflow_global::ServiceWrapper::Instance();
			registerTypeId(L"RpcServiceWrapperTest::IService", instance.rpctype_RpcServiceWrapperTest__IService);
		},
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::ServiceWrapper::Instance();
			if (dynamic_cast<::RpcServiceWrapperTest::IService*>(obj)) return instance.rpctype_RpcServiceWrapperTest__IService;
			return RpcTypeId_NotFound;
		});
});

TEST_CASE(L"Rpc:Dtor")
{
	RunRpcTestCase<::vl_workflow_global::Dtor>(L"[Not Deleted][Deleted]",
		[](const Func<void(const WString&, vint)>& registerTypeId)
		{
			auto& instance = ::vl_workflow_global::Dtor::Instance();
			registerTypeId(L"RpcDtorTest::IService", instance.rpctype_RpcDtorTest__IService);
		},
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Dtor::Instance();
			if (dynamic_cast<::RpcDtorTest::IService*>(obj)) return instance.rpctype_RpcDtorTest__IService;
			return RpcTypeId_NotFound;
		});
});

TEST_CASE(L"Rpc:Dtor2")
{
	RunRpcTestCase<::vl_workflow_global::Dtor2>(L"[Not Deleted][Deleted]",
		[](const Func<void(const WString&, vint)>& registerTypeId)
		{
			auto& instance = ::vl_workflow_global::Dtor2::Instance();
			registerTypeId(L"RpcDtor2Test::IService", instance.rpctype_RpcDtor2Test__IService);
		},
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Dtor2::Instance();
			if (dynamic_cast<::RpcDtor2Test::IService*>(obj)) return instance.rpctype_RpcDtor2Test__IService;
			return RpcTypeId_NotFound;
		});
});
}
