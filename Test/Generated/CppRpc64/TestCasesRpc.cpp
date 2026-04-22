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
void RunRpcTestCase(const WString& expected, vint(*decideTypeId)(IDescriptable*))
{
	class LocalRpcMock : public RpcDualLifecycleMock
	{
	public:
		vint(*decideTypeIdCallback)(IDescriptable*) = nullptr;
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
	};

	auto& instance = TInstance::Instance();

	auto lc1 = Ptr(new LocalRpcMock(1));
	auto lc2 = Ptr(new LocalRpcMock(2));
	auto idMap = UnboxParameter<Dictionary<WString, vint>>(BoxParameter(instance.rpc_GetIds()));
	lc1->SetIdMap(idMap.Ref());
	lc2->SetIdMap(idMap.Ref());
	lc1->decideTypeIdCallback = decideTypeId;
	lc2->decideTypeIdCallback = decideTypeId;

	auto lo1 = Ptr(new RpcCalleeListOps(lc1.Obj()));
	auto leo1 = Ptr(new RpcCalleeListEventBridge(lc1.Obj()));
	auto lo2 = Ptr(new RpcCalleeListOps(lc2.Obj()));
	auto leo2 = Ptr(new RpcCalleeListEventBridge(lc2.Obj()));

	auto oo1 = instance.rpc_IRpcObjectOps(lc1.Obj());
	auto oeo1 = instance.rpc_IRpcObjectEventOps(lc1.Obj());
	auto oo2 = instance.rpc_IRpcObjectOps(lc2.Obj());
	auto oeo2 = instance.rpc_IRpcObjectEventOps(lc2.Obj());
	auto doo1 = Ptr(new RpcDualObjectOps(lc1.Obj(), oo1));
	auto doo2 = Ptr(new RpcDualObjectOps(lc2.Obj(), oo2));

	lc2->Register(doo1, oeo1, lo1, leo1);
	lc1->Register(doo2, oeo2, lo2, leo2);
	lc1->RegisterWrapperFactory([&](vint typeId, IRpcLifeCycle* lc) { (void)lc; return instance.rpcwrapper_Create(typeId, lc1.Obj()); });
	lc2->RegisterWrapperFactory([&](vint typeId, IRpcLifeCycle* lc) { (void)lc; return instance.rpcwrapper_Create(typeId, lc2.Obj()); });

	instance.serviceMain(lc1.Obj());

	auto actual = instance.clientMain(lc2.Obj());

	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);

	lc2->DisconnectTrackedWrappersBeforeDispose();
	lc1->DisconnectTrackedWrappersBeforeDispose();
}

TEST_FILE
{

TEST_CASE(L"Rpc:RequestService")
{
	RunRpcTestCase<::vl_workflow_global::RequestService>(L"Hello",
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
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Dtor2::Instance();
			if (dynamic_cast<::RpcDtor2Test::IService*>(obj)) return instance.rpctype_RpcDtor2Test__IService;
			return RpcTypeId_NotFound;
		});
});
}
