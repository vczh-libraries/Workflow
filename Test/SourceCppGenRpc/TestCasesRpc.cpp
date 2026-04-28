#include "Collection_DefaultReflection.h"
#include "Collection_InByref_OutByrefReflection.h"
#include "Collection_InByref_OutByvalReflection.h"
#include "Collection_InByval_OutByrefReflection.h"
#include "Collection_InByval_OutByvalReflection.h"
#include "Collection_Interface_DefaultReflection.h"
#include "Collection_Interface_InByref_OutByrefReflection.h"
#include "Collection_Interface_InByref_OutByvalReflection.h"
#include "Collection_Interface_InByval_OutByrefReflection.h"
#include "Collection_Interface_InByval_OutByvalReflection.h"
#include "Collection_Interface_Nested_DefaultReflection.h"
#include "Collection_Interface_Nested_InByref_OutByrefReflection.h"
#include "Collection_Interface_Nested_InByref_OutByvalReflection.h"
#include "Collection_Interface_Nested_InByval_OutByrefReflection.h"
#include "Collection_Interface_Nested_InByval_OutByvalReflection.h"
#include "Collection_Interface_Nested_PropByrefReflection.h"
#include "Collection_Interface_Nested_PropByvalReflection.h"
#include "Collection_Interface_Nested_PropDefaultReflection.h"
#include "Collection_Interface_PropByrefReflection.h"
#include "Collection_Interface_PropByvalReflection.h"
#include "Collection_Interface_PropDefaultReflection.h"
#include "Collection_Nested_DefaultReflection.h"
#include "Collection_Nested_InByref_OutByrefReflection.h"
#include "Collection_Nested_InByref_OutByvalReflection.h"
#include "Collection_Nested_InByval_OutByrefReflection.h"
#include "Collection_Nested_InByval_OutByvalReflection.h"
#include "Collection_Nested_PropByrefReflection.h"
#include "Collection_Nested_PropByvalReflection.h"
#include "Collection_Nested_PropDefaultReflection.h"
#include "Collection_PropByrefReflection.h"
#include "Collection_PropByvalReflection.h"
#include "Collection_PropDefaultReflection.h"
#include "CollectionDict_DefaultReflection.h"
#include "CollectionDict_InByref_OutByrefReflection.h"
#include "CollectionDict_InByref_OutByvalReflection.h"
#include "CollectionDict_InByval_OutByrefReflection.h"
#include "CollectionDict_InByval_OutByvalReflection.h"
#include "CollectionDict_Interface_DefaultReflection.h"
#include "CollectionDict_Interface_InByref_OutByrefReflection.h"
#include "CollectionDict_Interface_InByref_OutByvalReflection.h"
#include "CollectionDict_Interface_InByval_OutByrefReflection.h"
#include "CollectionDict_Interface_InByval_OutByvalReflection.h"
#include "CollectionDict_Interface_Nested_DefaultReflection.h"
#include "CollectionDict_Interface_Nested_InByref_OutByrefReflection.h"
#include "CollectionDict_Interface_Nested_InByref_OutByvalReflection.h"
#include "CollectionDict_Interface_Nested_InByval_OutByrefReflection.h"
#include "CollectionDict_Interface_Nested_InByval_OutByvalReflection.h"
#include "CollectionDict_Interface_Nested_PropByrefReflection.h"
#include "CollectionDict_Interface_Nested_PropByvalReflection.h"
#include "CollectionDict_Interface_Nested_PropDefaultReflection.h"
#include "CollectionDict_Interface_PropByrefReflection.h"
#include "CollectionDict_Interface_PropByvalReflection.h"
#include "CollectionDict_Interface_PropDefaultReflection.h"
#include "CollectionDict_Nested_DefaultReflection.h"
#include "CollectionDict_Nested_InByref_OutByrefReflection.h"
#include "CollectionDict_Nested_InByref_OutByvalReflection.h"
#include "CollectionDict_Nested_InByval_OutByrefReflection.h"
#include "CollectionDict_Nested_InByval_OutByvalReflection.h"
#include "CollectionDict_Nested_PropByrefReflection.h"
#include "CollectionDict_Nested_PropByvalReflection.h"
#include "CollectionDict_Nested_PropDefaultReflection.h"
#include "CollectionDict_PropByrefReflection.h"
#include "CollectionDict_PropByvalReflection.h"
#include "CollectionDict_PropDefaultReflection.h"
#include "CollectionOblist_DefaultReflection.h"
#include "CollectionOblist_InByref_OutByrefReflection.h"
#include "CollectionOblist_InByref_OutByvalReflection.h"
#include "CollectionOblist_InByval_OutByrefReflection.h"
#include "CollectionOblist_InByval_OutByvalReflection.h"
#include "CollectionOblist_Interface_DefaultReflection.h"
#include "CollectionOblist_Interface_InByref_OutByrefReflection.h"
#include "CollectionOblist_Interface_InByref_OutByvalReflection.h"
#include "CollectionOblist_Interface_InByval_OutByrefReflection.h"
#include "CollectionOblist_Interface_InByval_OutByvalReflection.h"
#include "CollectionOblist_Interface_Nested_DefaultReflection.h"
#include "CollectionOblist_Interface_Nested_InByref_OutByrefReflection.h"
#include "CollectionOblist_Interface_Nested_InByref_OutByvalReflection.h"
#include "CollectionOblist_Interface_Nested_InByval_OutByrefReflection.h"
#include "CollectionOblist_Interface_Nested_InByval_OutByvalReflection.h"
#include "CollectionOblist_Interface_Nested_PropByrefReflection.h"
#include "CollectionOblist_Interface_Nested_PropByvalReflection.h"
#include "CollectionOblist_Interface_Nested_PropDefaultReflection.h"
#include "CollectionOblist_Interface_PropByrefReflection.h"
#include "CollectionOblist_Interface_PropByvalReflection.h"
#include "CollectionOblist_Interface_PropDefaultReflection.h"
#include "CollectionOblist_Nested_DefaultReflection.h"
#include "CollectionOblist_Nested_InByref_OutByrefReflection.h"
#include "CollectionOblist_Nested_InByref_OutByvalReflection.h"
#include "CollectionOblist_Nested_InByval_OutByrefReflection.h"
#include "CollectionOblist_Nested_InByval_OutByvalReflection.h"
#include "CollectionOblist_Nested_PropByrefReflection.h"
#include "CollectionOblist_Nested_PropByvalReflection.h"
#include "CollectionOblist_Nested_PropDefaultReflection.h"
#include "CollectionOblist_PropByrefReflection.h"
#include "CollectionOblist_PropByvalReflection.h"
#include "CollectionOblist_PropDefaultReflection.h"
#include "DtorReflection.h"
#include "Dtor2Reflection.h"
#include "Dtor3Reflection.h"
#include "DtorListReflection.h"
#include "DtorList2Reflection.h"
#include "DtorPropCachedReflection.h"
#include "DtorPropCachedListByvalReflection.h"
#include "DtorPropCachedListVByrefReflection.h"
#include "EventReflection.h"
#include "EventOblistReflection.h"
#include "FailDoubleRegistrationReflection.h"
#include "LocalAndWrapperReflection.h"
#include "OverloadingReflection.h"
#include "PrimitiveTypesReflection.h"
#include "PropCachedReflection.h"
#include "PropDefaultReflection.h"
#include "PropDefaultInterfaceReflection.h"
#include "PropDefaultInterfaceListReflection.h"
#include "PropDefaultListReflection.h"
#include "PropDynamicReflection.h"
#include "RequestServiceReflection.h"
#include "ServiceWrapperReflection.h"
#include "../Source/RpcDualLifecycleMock.h"
#include "../Source/TestCasesRpc.h"

using namespace vl;
using namespace vl::collections;
using namespace vl::console;
using namespace vl::reflection;
using namespace vl::reflection::description;
using namespace vl::rpc_controller;
using namespace vl::rpc_controller_test;

void LoadTestCaseRpcTypes()
{
	 LoadRpc_Collection_DefaultTypes();
	 LoadRpc_Collection_InByref_OutByrefTypes();
	 LoadRpc_Collection_InByref_OutByvalTypes();
	 LoadRpc_Collection_InByval_OutByrefTypes();
	 LoadRpc_Collection_InByval_OutByvalTypes();
	 LoadRpc_Collection_Interface_DefaultTypes();
	 LoadRpc_Collection_Interface_InByref_OutByrefTypes();
	 LoadRpc_Collection_Interface_InByref_OutByvalTypes();
	 LoadRpc_Collection_Interface_InByval_OutByrefTypes();
	 LoadRpc_Collection_Interface_InByval_OutByvalTypes();
	 LoadRpc_Collection_Interface_Nested_DefaultTypes();
	 LoadRpc_Collection_Interface_Nested_InByref_OutByrefTypes();
	 LoadRpc_Collection_Interface_Nested_InByref_OutByvalTypes();
	 LoadRpc_Collection_Interface_Nested_InByval_OutByrefTypes();
	 LoadRpc_Collection_Interface_Nested_InByval_OutByvalTypes();
	 LoadRpc_Collection_Interface_Nested_PropByrefTypes();
	 LoadRpc_Collection_Interface_Nested_PropByvalTypes();
	 LoadRpc_Collection_Interface_Nested_PropDefaultTypes();
	 LoadRpc_Collection_Interface_PropByrefTypes();
	 LoadRpc_Collection_Interface_PropByvalTypes();
	 LoadRpc_Collection_Interface_PropDefaultTypes();
	 LoadRpc_Collection_Nested_DefaultTypes();
	 LoadRpc_Collection_Nested_InByref_OutByrefTypes();
	 LoadRpc_Collection_Nested_InByref_OutByvalTypes();
	 LoadRpc_Collection_Nested_InByval_OutByrefTypes();
	 LoadRpc_Collection_Nested_InByval_OutByvalTypes();
	 LoadRpc_Collection_Nested_PropByrefTypes();
	 LoadRpc_Collection_Nested_PropByvalTypes();
	 LoadRpc_Collection_Nested_PropDefaultTypes();
	 LoadRpc_Collection_PropByrefTypes();
	 LoadRpc_Collection_PropByvalTypes();
	 LoadRpc_Collection_PropDefaultTypes();
	 LoadRpc_CollectionDict_DefaultTypes();
	 LoadRpc_CollectionDict_InByref_OutByrefTypes();
	 LoadRpc_CollectionDict_InByref_OutByvalTypes();
	 LoadRpc_CollectionDict_InByval_OutByrefTypes();
	 LoadRpc_CollectionDict_InByval_OutByvalTypes();
	 LoadRpc_CollectionDict_Interface_DefaultTypes();
	 LoadRpc_CollectionDict_Interface_InByref_OutByrefTypes();
	 LoadRpc_CollectionDict_Interface_InByref_OutByvalTypes();
	 LoadRpc_CollectionDict_Interface_InByval_OutByrefTypes();
	 LoadRpc_CollectionDict_Interface_InByval_OutByvalTypes();
	 LoadRpc_CollectionDict_Interface_Nested_DefaultTypes();
	 LoadRpc_CollectionDict_Interface_Nested_InByref_OutByrefTypes();
	 LoadRpc_CollectionDict_Interface_Nested_InByref_OutByvalTypes();
	 LoadRpc_CollectionDict_Interface_Nested_InByval_OutByrefTypes();
	 LoadRpc_CollectionDict_Interface_Nested_InByval_OutByvalTypes();
	 LoadRpc_CollectionDict_Interface_Nested_PropByrefTypes();
	 LoadRpc_CollectionDict_Interface_Nested_PropByvalTypes();
	 LoadRpc_CollectionDict_Interface_Nested_PropDefaultTypes();
	 LoadRpc_CollectionDict_Interface_PropByrefTypes();
	 LoadRpc_CollectionDict_Interface_PropByvalTypes();
	 LoadRpc_CollectionDict_Interface_PropDefaultTypes();
	 LoadRpc_CollectionDict_Nested_DefaultTypes();
	 LoadRpc_CollectionDict_Nested_InByref_OutByrefTypes();
	 LoadRpc_CollectionDict_Nested_InByref_OutByvalTypes();
	 LoadRpc_CollectionDict_Nested_InByval_OutByrefTypes();
	 LoadRpc_CollectionDict_Nested_InByval_OutByvalTypes();
	 LoadRpc_CollectionDict_Nested_PropByrefTypes();
	 LoadRpc_CollectionDict_Nested_PropByvalTypes();
	 LoadRpc_CollectionDict_Nested_PropDefaultTypes();
	 LoadRpc_CollectionDict_PropByrefTypes();
	 LoadRpc_CollectionDict_PropByvalTypes();
	 LoadRpc_CollectionDict_PropDefaultTypes();
	 LoadRpc_CollectionOblist_DefaultTypes();
	 LoadRpc_CollectionOblist_InByref_OutByrefTypes();
	 LoadRpc_CollectionOblist_InByref_OutByvalTypes();
	 LoadRpc_CollectionOblist_InByval_OutByrefTypes();
	 LoadRpc_CollectionOblist_InByval_OutByvalTypes();
	 LoadRpc_CollectionOblist_Interface_DefaultTypes();
	 LoadRpc_CollectionOblist_Interface_InByref_OutByrefTypes();
	 LoadRpc_CollectionOblist_Interface_InByref_OutByvalTypes();
	 LoadRpc_CollectionOblist_Interface_InByval_OutByrefTypes();
	 LoadRpc_CollectionOblist_Interface_InByval_OutByvalTypes();
	 LoadRpc_CollectionOblist_Interface_Nested_DefaultTypes();
	 LoadRpc_CollectionOblist_Interface_Nested_InByref_OutByrefTypes();
	 LoadRpc_CollectionOblist_Interface_Nested_InByref_OutByvalTypes();
	 LoadRpc_CollectionOblist_Interface_Nested_InByval_OutByrefTypes();
	 LoadRpc_CollectionOblist_Interface_Nested_InByval_OutByvalTypes();
	 LoadRpc_CollectionOblist_Interface_Nested_PropByrefTypes();
	 LoadRpc_CollectionOblist_Interface_Nested_PropByvalTypes();
	 LoadRpc_CollectionOblist_Interface_Nested_PropDefaultTypes();
	 LoadRpc_CollectionOblist_Interface_PropByrefTypes();
	 LoadRpc_CollectionOblist_Interface_PropByvalTypes();
	 LoadRpc_CollectionOblist_Interface_PropDefaultTypes();
	 LoadRpc_CollectionOblist_Nested_DefaultTypes();
	 LoadRpc_CollectionOblist_Nested_InByref_OutByrefTypes();
	 LoadRpc_CollectionOblist_Nested_InByref_OutByvalTypes();
	 LoadRpc_CollectionOblist_Nested_InByval_OutByrefTypes();
	 LoadRpc_CollectionOblist_Nested_InByval_OutByvalTypes();
	 LoadRpc_CollectionOblist_Nested_PropByrefTypes();
	 LoadRpc_CollectionOblist_Nested_PropByvalTypes();
	 LoadRpc_CollectionOblist_Nested_PropDefaultTypes();
	 LoadRpc_CollectionOblist_PropByrefTypes();
	 LoadRpc_CollectionOblist_PropByvalTypes();
	 LoadRpc_CollectionOblist_PropDefaultTypes();
	 LoadRpc_DtorTypes();
	 LoadRpc_Dtor2Types();
	 LoadRpc_Dtor3Types();
	 LoadRpc_DtorListTypes();
	 LoadRpc_DtorList2Types();
	 LoadRpc_DtorPropCachedTypes();
	 LoadRpc_DtorPropCachedListByvalTypes();
	 LoadRpc_DtorPropCachedListVByrefTypes();
	 LoadRpc_EventTypes();
	 LoadRpc_EventOblistTypes();
	 LoadRpc_FailDoubleRegistrationTypes();
	 LoadRpc_LocalAndWrapperTypes();
	 LoadRpc_OverloadingTypes();
	 LoadRpc_PrimitiveTypesTypes();
	 LoadRpc_PropCachedTypes();
	 LoadRpc_PropDefaultTypes();
	 LoadRpc_PropDefaultInterfaceTypes();
	 LoadRpc_PropDefaultInterfaceListTypes();
	 LoadRpc_PropDefaultListTypes();
	 LoadRpc_PropDynamicTypes();
	 LoadRpc_RequestServiceTypes();
	 LoadRpc_ServiceWrapperTypes();
}

TEST_FILE
{

TEST_CASE(L"Rpc:Collection_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Default>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Default::Instance();
			if (dynamic_cast<::RpcCollection::Default::IService*>(obj)) return instance.rpctype_RpcCollection__Default__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_InByref_OutByref>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_InByref_OutByref::Instance();
			if (dynamic_cast<::RpcCollection::InByref::OutByref::IService*>(obj)) return instance.rpctype_RpcCollection__InByref__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_InByref_OutByval>(L"[1234][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_InByref_OutByval::Instance();
			if (dynamic_cast<::RpcCollection::InByref::OutByval::IService*>(obj)) return instance.rpctype_RpcCollection__InByref__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_InByval_OutByref>(L"[123][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_InByval_OutByref::Instance();
			if (dynamic_cast<::RpcCollection::InByval::OutByref::IService*>(obj)) return instance.rpctype_RpcCollection__InByval__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_InByval_OutByval>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_InByval_OutByval::Instance();
			if (dynamic_cast<::RpcCollection::InByval::OutByval::IService*>(obj)) return instance.rpctype_RpcCollection__InByval__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Interface_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_Default>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Interface_Default::Instance();
			if (dynamic_cast<::RpcCollection::Interface::Default::IValue*>(obj)) return instance.rpctype_RpcCollection__Interface__Default__IValue;
			if (dynamic_cast<::RpcCollection::Interface::Default::IService*>(obj)) return instance.rpctype_RpcCollection__Interface__Default__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Interface_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_InByref_OutByref>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Interface_InByref_OutByref::Instance();
			if (dynamic_cast<::RpcCollection::Interface::InByref::OutByref::IValue*>(obj)) return instance.rpctype_RpcCollection__Interface__InByref__OutByref__IValue;
			if (dynamic_cast<::RpcCollection::Interface::InByref::OutByref::IService*>(obj)) return instance.rpctype_RpcCollection__Interface__InByref__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Interface_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_InByref_OutByval>(L"[1234][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Interface_InByref_OutByval::Instance();
			if (dynamic_cast<::RpcCollection::Interface::InByref::OutByval::IValue*>(obj)) return instance.rpctype_RpcCollection__Interface__InByref__OutByval__IValue;
			if (dynamic_cast<::RpcCollection::Interface::InByref::OutByval::IService*>(obj)) return instance.rpctype_RpcCollection__Interface__InByref__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Interface_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_InByval_OutByref>(L"[123][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Interface_InByval_OutByref::Instance();
			if (dynamic_cast<::RpcCollection::Interface::InByval::OutByref::IValue*>(obj)) return instance.rpctype_RpcCollection__Interface__InByval__OutByref__IValue;
			if (dynamic_cast<::RpcCollection::Interface::InByval::OutByref::IService*>(obj)) return instance.rpctype_RpcCollection__Interface__InByval__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Interface_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_InByval_OutByval>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Interface_InByval_OutByval::Instance();
			if (dynamic_cast<::RpcCollection::Interface::InByval::OutByval::IValue*>(obj)) return instance.rpctype_RpcCollection__Interface__InByval__OutByval__IValue;
			if (dynamic_cast<::RpcCollection::Interface::InByval::OutByval::IService*>(obj)) return instance.rpctype_RpcCollection__Interface__InByval__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Interface_Nested_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_Default>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Interface_Nested_Default::Instance();
			if (dynamic_cast<::RpcCollection::Interface::Nested::Default::IValue*>(obj)) return instance.rpctype_RpcCollection__Interface__Nested__Default__IValue;
			if (dynamic_cast<::RpcCollection::Interface::Nested::Default::IService*>(obj)) return instance.rpctype_RpcCollection__Interface__Nested__Default__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Interface_Nested_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_InByref_OutByref>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Interface_Nested_InByref_OutByref::Instance();
			if (dynamic_cast<::RpcCollection::Interface::Nested::InByref::OutByref::IValue*>(obj)) return instance.rpctype_RpcCollection__Interface__Nested__InByref__OutByref__IValue;
			if (dynamic_cast<::RpcCollection::Interface::Nested::InByref::OutByref::IService*>(obj)) return instance.rpctype_RpcCollection__Interface__Nested__InByref__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Interface_Nested_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_InByref_OutByval>(L"[1234][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Interface_Nested_InByref_OutByval::Instance();
			if (dynamic_cast<::RpcCollection::Interface::Nested::InByref::OutByval::IValue*>(obj)) return instance.rpctype_RpcCollection__Interface__Nested__InByref__OutByval__IValue;
			if (dynamic_cast<::RpcCollection::Interface::Nested::InByref::OutByval::IService*>(obj)) return instance.rpctype_RpcCollection__Interface__Nested__InByref__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Interface_Nested_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_InByval_OutByref>(L"[123][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Interface_Nested_InByval_OutByref::Instance();
			if (dynamic_cast<::RpcCollection::Interface::Nested::InByval::OutByref::IValue*>(obj)) return instance.rpctype_RpcCollection__Interface__Nested__InByval__OutByref__IValue;
			if (dynamic_cast<::RpcCollection::Interface::Nested::InByval::OutByref::IService*>(obj)) return instance.rpctype_RpcCollection__Interface__Nested__InByval__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Interface_Nested_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_InByval_OutByval>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Interface_Nested_InByval_OutByval::Instance();
			if (dynamic_cast<::RpcCollection::Interface::Nested::InByval::OutByval::IValue*>(obj)) return instance.rpctype_RpcCollection__Interface__Nested__InByval__OutByval__IValue;
			if (dynamic_cast<::RpcCollection::Interface::Nested::InByval::OutByval::IService*>(obj)) return instance.rpctype_RpcCollection__Interface__Nested__InByval__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Interface_Nested_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_PropByref>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Interface_Nested_PropByref::Instance();
			if (dynamic_cast<::RpcCollection::Interface::Nested::InByref::PropByref::IValue*>(obj)) return instance.rpctype_RpcCollection__Interface__Nested__InByref__PropByref__IValue;
			if (dynamic_cast<::RpcCollection::Interface::Nested::InByref::PropByref::IService*>(obj)) return instance.rpctype_RpcCollection__Interface__Nested__InByref__PropByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Interface_Nested_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_PropByval>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Interface_Nested_PropByval::Instance();
			if (dynamic_cast<::RpcCollection::Interface::Nested::InByval::PropByval::IValue*>(obj)) return instance.rpctype_RpcCollection__Interface__Nested__InByval__PropByval__IValue;
			if (dynamic_cast<::RpcCollection::Interface::Nested::InByval::PropByval::IService*>(obj)) return instance.rpctype_RpcCollection__Interface__Nested__InByval__PropByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Interface_Nested_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_PropDefault>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Interface_Nested_PropDefault::Instance();
			if (dynamic_cast<::RpcCollection::Interface::Nested::PropDefault::IValue*>(obj)) return instance.rpctype_RpcCollection__Interface__Nested__PropDefault__IValue;
			if (dynamic_cast<::RpcCollection::Interface::Nested::PropDefault::IService*>(obj)) return instance.rpctype_RpcCollection__Interface__Nested__PropDefault__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Interface_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_PropByref>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Interface_PropByref::Instance();
			if (dynamic_cast<::RpcCollection::Interface::InByref::PropByref::IValue*>(obj)) return instance.rpctype_RpcCollection__Interface__InByref__PropByref__IValue;
			if (dynamic_cast<::RpcCollection::Interface::InByref::PropByref::IService*>(obj)) return instance.rpctype_RpcCollection__Interface__InByref__PropByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Interface_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_PropByval>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Interface_PropByval::Instance();
			if (dynamic_cast<::RpcCollection::Interface::InByval::PropByval::IValue*>(obj)) return instance.rpctype_RpcCollection__Interface__InByval__PropByval__IValue;
			if (dynamic_cast<::RpcCollection::Interface::InByval::PropByval::IService*>(obj)) return instance.rpctype_RpcCollection__Interface__InByval__PropByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Interface_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_PropDefault>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Interface_PropDefault::Instance();
			if (dynamic_cast<::RpcCollection::Interface::PropDefault::IValue*>(obj)) return instance.rpctype_RpcCollection__Interface__PropDefault__IValue;
			if (dynamic_cast<::RpcCollection::Interface::PropDefault::IService*>(obj)) return instance.rpctype_RpcCollection__Interface__PropDefault__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Nested_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Nested_Default>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Nested_Default::Instance();
			if (dynamic_cast<::RpcCollection::Nested::Default::IService*>(obj)) return instance.rpctype_RpcCollection__Nested__Default__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Nested_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Nested_InByref_OutByref>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Nested_InByref_OutByref::Instance();
			if (dynamic_cast<::RpcCollection::Nested::InByref::OutByref::IService*>(obj)) return instance.rpctype_RpcCollection__Nested__InByref__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Nested_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Nested_InByref_OutByval>(L"[1234][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Nested_InByref_OutByval::Instance();
			if (dynamic_cast<::RpcCollection::Nested::InByref::OutByval::IService*>(obj)) return instance.rpctype_RpcCollection__Nested__InByref__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Nested_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Nested_InByval_OutByref>(L"[123][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Nested_InByval_OutByref::Instance();
			if (dynamic_cast<::RpcCollection::Nested::InByval::OutByref::IService*>(obj)) return instance.rpctype_RpcCollection__Nested__InByval__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Nested_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Nested_InByval_OutByval>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Nested_InByval_OutByval::Instance();
			if (dynamic_cast<::RpcCollection::Nested::InByval::OutByval::IService*>(obj)) return instance.rpctype_RpcCollection__Nested__InByval__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Nested_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Nested_PropByref>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Nested_PropByref::Instance();
			if (dynamic_cast<::RpcCollection::Nested::InByref::PropByref::IService*>(obj)) return instance.rpctype_RpcCollection__Nested__InByref__PropByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Nested_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Nested_PropByval>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Nested_PropByval::Instance();
			if (dynamic_cast<::RpcCollection::Nested::InByval::PropByval::IService*>(obj)) return instance.rpctype_RpcCollection__Nested__InByval__PropByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_Nested_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Nested_PropDefault>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_Nested_PropDefault::Instance();
			if (dynamic_cast<::RpcCollection::Nested::PropDefault::IService*>(obj)) return instance.rpctype_RpcCollection__Nested__PropDefault__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_PropByref>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_PropByref::Instance();
			if (dynamic_cast<::RpcCollection::InByref::PropByref::IService*>(obj)) return instance.rpctype_RpcCollection__InByref__PropByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_PropByval>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_PropByval::Instance();
			if (dynamic_cast<::RpcCollection::InByval::PropByval::IService*>(obj)) return instance.rpctype_RpcCollection__InByval__PropByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Collection_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_PropDefault>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Collection_PropDefault::Instance();
			if (dynamic_cast<::RpcCollection::PropDefault::IService*>(obj)) return instance.rpctype_RpcCollection__PropDefault__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Default>(L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Default::Instance();
			if (dynamic_cast<::RpcCollectionDict::Default::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Default__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_InByref_OutByref>(L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_InByref_OutByref::Instance();
			if (dynamic_cast<::RpcCollectionDict::InByref::OutByref::IService*>(obj)) return instance.rpctype_RpcCollectionDict__InByref__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_InByref_OutByval>(L"[1A2B3C4D][1A2B3C4D][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_InByref_OutByval::Instance();
			if (dynamic_cast<::RpcCollectionDict::InByref::OutByval::IService*>(obj)) return instance.rpctype_RpcCollectionDict__InByref__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_InByval_OutByref>(L"[1A2B3C][1A2B3C4D5E][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_InByval_OutByref::Instance();
			if (dynamic_cast<::RpcCollectionDict::InByval::OutByref::IService*>(obj)) return instance.rpctype_RpcCollectionDict__InByval__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_InByval_OutByval>(L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_InByval_OutByval::Instance();
			if (dynamic_cast<::RpcCollectionDict::InByval::OutByval::IService*>(obj)) return instance.rpctype_RpcCollectionDict__InByval__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Interface_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Default>(L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Interface_Default::Instance();
			if (dynamic_cast<::RpcCollectionDict::Interface::Default::IValue*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__Default__IValue;
			if (dynamic_cast<::RpcCollectionDict::Interface::Default::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__Default__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Interface_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_InByref_OutByref>(L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Interface_InByref_OutByref::Instance();
			if (dynamic_cast<::RpcCollectionDict::Interface::InByref::OutByref::IValue*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__InByref__OutByref__IValue;
			if (dynamic_cast<::RpcCollectionDict::Interface::InByref::OutByref::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__InByref__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Interface_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_InByref_OutByval>(L"[1A2B3C4D][1A2B3C4D][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Interface_InByref_OutByval::Instance();
			if (dynamic_cast<::RpcCollectionDict::Interface::InByref::OutByval::IValue*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__InByref__OutByval__IValue;
			if (dynamic_cast<::RpcCollectionDict::Interface::InByref::OutByval::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__InByref__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Interface_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_InByval_OutByref>(L"[1A2B3C][1A2B3C4D5E][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Interface_InByval_OutByref::Instance();
			if (dynamic_cast<::RpcCollectionDict::Interface::InByval::OutByref::IValue*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__InByval__OutByref__IValue;
			if (dynamic_cast<::RpcCollectionDict::Interface::InByval::OutByref::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__InByval__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Interface_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_InByval_OutByval>(L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Interface_InByval_OutByval::Instance();
			if (dynamic_cast<::RpcCollectionDict::Interface::InByval::OutByval::IValue*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__InByval__OutByval__IValue;
			if (dynamic_cast<::RpcCollectionDict::Interface::InByval::OutByval::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__InByval__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Interface_Nested_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_Default>(L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_Default::Instance();
			if (dynamic_cast<::RpcCollectionDict::Interface::Nested::Default::IValue*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__Nested__Default__IValue;
			if (dynamic_cast<::RpcCollectionDict::Interface::Nested::Default::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__Nested__Default__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Interface_Nested_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByref_OutByref>(L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByref_OutByref::Instance();
			if (dynamic_cast<::RpcCollectionDict::Interface::Nested::InByref::OutByref::IValue*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__Nested__InByref__OutByref__IValue;
			if (dynamic_cast<::RpcCollectionDict::Interface::Nested::InByref::OutByref::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__Nested__InByref__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Interface_Nested_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByref_OutByval>(L"[1A2B3C4D][1A2B3C4D][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByref_OutByval::Instance();
			if (dynamic_cast<::RpcCollectionDict::Interface::Nested::InByref::OutByval::IValue*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__Nested__InByref__OutByval__IValue;
			if (dynamic_cast<::RpcCollectionDict::Interface::Nested::InByref::OutByval::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__Nested__InByref__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Interface_Nested_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByval_OutByref>(L"[1A2B3C][1A2B3C4D5E][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByval_OutByref::Instance();
			if (dynamic_cast<::RpcCollectionDict::Interface::Nested::InByval::OutByref::IValue*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__Nested__InByval__OutByref__IValue;
			if (dynamic_cast<::RpcCollectionDict::Interface::Nested::InByval::OutByref::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__Nested__InByval__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Interface_Nested_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByval_OutByval>(L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByval_OutByval::Instance();
			if (dynamic_cast<::RpcCollectionDict::Interface::Nested::InByval::OutByval::IValue*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__Nested__InByval__OutByval__IValue;
			if (dynamic_cast<::RpcCollectionDict::Interface::Nested::InByval::OutByval::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__Nested__InByval__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Interface_Nested_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_PropByref>(L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_PropByref::Instance();
			if (dynamic_cast<::RpcCollectionDict::Interface::Nested::InByref::PropByref::IValue*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__Nested__InByref__PropByref__IValue;
			if (dynamic_cast<::RpcCollectionDict::Interface::Nested::InByref::PropByref::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__Nested__InByref__PropByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Interface_Nested_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_PropByval>(L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_PropByval::Instance();
			if (dynamic_cast<::RpcCollectionDict::Interface::Nested::InByval::PropByval::IValue*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__Nested__InByval__PropByval__IValue;
			if (dynamic_cast<::RpcCollectionDict::Interface::Nested::InByval::PropByval::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__Nested__InByval__PropByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Interface_Nested_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_PropDefault>(L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_PropDefault::Instance();
			if (dynamic_cast<::RpcCollectionDict::Interface::Nested::PropDefault::IValue*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__Nested__PropDefault__IValue;
			if (dynamic_cast<::RpcCollectionDict::Interface::Nested::PropDefault::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__Nested__PropDefault__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Interface_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_PropByref>(L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Interface_PropByref::Instance();
			if (dynamic_cast<::RpcCollectionDict::Interface::InByref::PropByref::IValue*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__InByref__PropByref__IValue;
			if (dynamic_cast<::RpcCollectionDict::Interface::InByref::PropByref::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__InByref__PropByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Interface_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_PropByval>(L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Interface_PropByval::Instance();
			if (dynamic_cast<::RpcCollectionDict::Interface::InByval::PropByval::IValue*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__InByval__PropByval__IValue;
			if (dynamic_cast<::RpcCollectionDict::Interface::InByval::PropByval::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__InByval__PropByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Interface_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_PropDefault>(L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Interface_PropDefault::Instance();
			if (dynamic_cast<::RpcCollectionDict::Interface::PropDefault::IValue*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__PropDefault__IValue;
			if (dynamic_cast<::RpcCollectionDict::Interface::PropDefault::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Interface__PropDefault__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Nested_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_Default>(L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Nested_Default::Instance();
			if (dynamic_cast<::RpcCollectionDict::Nested::Default::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Nested__Default__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Nested_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_InByref_OutByref>(L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Nested_InByref_OutByref::Instance();
			if (dynamic_cast<::RpcCollectionDict::Nested::InByref::OutByref::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Nested__InByref__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Nested_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_InByref_OutByval>(L"[1A2B3C4D][1A2B3C4D][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Nested_InByref_OutByval::Instance();
			if (dynamic_cast<::RpcCollectionDict::Nested::InByref::OutByval::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Nested__InByref__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Nested_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_InByval_OutByref>(L"[1A2B3C][1A2B3C4D5E][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Nested_InByval_OutByref::Instance();
			if (dynamic_cast<::RpcCollectionDict::Nested::InByval::OutByref::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Nested__InByval__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Nested_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_InByval_OutByval>(L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Nested_InByval_OutByval::Instance();
			if (dynamic_cast<::RpcCollectionDict::Nested::InByval::OutByval::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Nested__InByval__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Nested_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_PropByref>(L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Nested_PropByref::Instance();
			if (dynamic_cast<::RpcCollectionDict::Nested::InByref::PropByref::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Nested__InByref__PropByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Nested_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_PropByval>(L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Nested_PropByval::Instance();
			if (dynamic_cast<::RpcCollectionDict::Nested::InByval::PropByval::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Nested__InByval__PropByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_Nested_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_PropDefault>(L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_Nested_PropDefault::Instance();
			if (dynamic_cast<::RpcCollectionDict::Nested::PropDefault::IService*>(obj)) return instance.rpctype_RpcCollectionDict__Nested__PropDefault__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_PropByref>(L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_PropByref::Instance();
			if (dynamic_cast<::RpcCollectionDict::InByref::PropByref::IService*>(obj)) return instance.rpctype_RpcCollectionDict__InByref__PropByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_PropByval>(L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_PropByval::Instance();
			if (dynamic_cast<::RpcCollectionDict::InByval::PropByval::IService*>(obj)) return instance.rpctype_RpcCollectionDict__InByval__PropByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionDict_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_PropDefault>(L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionDict_PropDefault::Instance();
			if (dynamic_cast<::RpcCollectionDict::PropDefault::IService*>(obj)) return instance.rpctype_RpcCollectionDict__PropDefault__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Default>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Default::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Default::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Default__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_InByref_OutByref>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_InByref_OutByref::Instance();
			if (dynamic_cast<::RpcCollectionOblist::InByref::OutByref::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__InByref__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_InByref_OutByval>(L"[1234][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_InByref_OutByval::Instance();
			if (dynamic_cast<::RpcCollectionOblist::InByref::OutByval::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__InByref__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_InByval_OutByref>(L"[123][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_InByval_OutByref::Instance();
			if (dynamic_cast<::RpcCollectionOblist::InByval::OutByref::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__InByval__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_InByval_OutByval>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_InByval_OutByval::Instance();
			if (dynamic_cast<::RpcCollectionOblist::InByval::OutByval::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__InByval__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Default>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Interface_Default::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Interface::Default::IValue*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__Default__IValue;
			if (dynamic_cast<::RpcCollectionOblist::Interface::Default::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__Default__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_InByref_OutByref>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Interface_InByref_OutByref::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Interface::InByref::OutByref::IValue*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__InByref__OutByref__IValue;
			if (dynamic_cast<::RpcCollectionOblist::Interface::InByref::OutByref::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__InByref__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_InByref_OutByval>(L"[1234][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Interface_InByref_OutByval::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Interface::InByref::OutByval::IValue*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__InByref__OutByval__IValue;
			if (dynamic_cast<::RpcCollectionOblist::Interface::InByref::OutByval::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__InByref__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_InByval_OutByref>(L"[123][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Interface_InByval_OutByref::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Interface::InByval::OutByref::IValue*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__InByval__OutByref__IValue;
			if (dynamic_cast<::RpcCollectionOblist::Interface::InByval::OutByref::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__InByval__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_InByval_OutByval>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Interface_InByval_OutByval::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Interface::InByval::OutByval::IValue*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__InByval__OutByval__IValue;
			if (dynamic_cast<::RpcCollectionOblist::Interface::InByval::OutByval::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__InByval__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_Nested_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_Default>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_Default::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Interface::Nested::Default::IValue*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__Nested__Default__IValue;
			if (dynamic_cast<::RpcCollectionOblist::Interface::Nested::Default::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__Nested__Default__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_Nested_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByref_OutByref>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByref_OutByref::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Interface::Nested::InByref::OutByref::IValue*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__Nested__InByref__OutByref__IValue;
			if (dynamic_cast<::RpcCollectionOblist::Interface::Nested::InByref::OutByref::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__Nested__InByref__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_Nested_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByref_OutByval>(L"[1234][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByref_OutByval::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Interface::Nested::InByref::OutByval::IValue*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__Nested__InByref__OutByval__IValue;
			if (dynamic_cast<::RpcCollectionOblist::Interface::Nested::InByref::OutByval::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__Nested__InByref__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_Nested_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByval_OutByref>(L"[123][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByval_OutByref::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Interface::Nested::InByval::OutByref::IValue*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__Nested__InByval__OutByref__IValue;
			if (dynamic_cast<::RpcCollectionOblist::Interface::Nested::InByval::OutByref::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__Nested__InByval__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_Nested_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByval_OutByval>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByval_OutByval::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Interface::Nested::InByval::OutByval::IValue*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__Nested__InByval__OutByval__IValue;
			if (dynamic_cast<::RpcCollectionOblist::Interface::Nested::InByval::OutByval::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__Nested__InByval__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_Nested_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_PropByref>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_PropByref::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Interface::Nested::InByref::PropByref::IValue*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__Nested__InByref__PropByref__IValue;
			if (dynamic_cast<::RpcCollectionOblist::Interface::Nested::InByref::PropByref::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__Nested__InByref__PropByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_Nested_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_PropByval>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_PropByval::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Interface::Nested::InByval::PropByval::IValue*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__Nested__InByval__PropByval__IValue;
			if (dynamic_cast<::RpcCollectionOblist::Interface::Nested::InByval::PropByval::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__Nested__InByval__PropByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_Nested_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_PropDefault>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_PropDefault::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Interface::Nested::PropDefault::IValue*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__Nested__PropDefault__IValue;
			if (dynamic_cast<::RpcCollectionOblist::Interface::Nested::PropDefault::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__Nested__PropDefault__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_PropByref>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Interface_PropByref::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Interface::InByref::PropByref::IValue*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__InByref__PropByref__IValue;
			if (dynamic_cast<::RpcCollectionOblist::Interface::InByref::PropByref::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__InByref__PropByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_PropByval>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Interface_PropByval::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Interface::InByval::PropByval::IValue*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__InByval__PropByval__IValue;
			if (dynamic_cast<::RpcCollectionOblist::Interface::InByval::PropByval::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__InByval__PropByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_PropDefault>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Interface_PropDefault::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Interface::PropDefault::IValue*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__PropDefault__IValue;
			if (dynamic_cast<::RpcCollectionOblist::Interface::PropDefault::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Interface__PropDefault__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Nested_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_Default>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Nested_Default::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Nested::Default::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Nested__Default__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Nested_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_InByref_OutByref>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Nested_InByref_OutByref::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Nested::InByref::OutByref::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Nested__InByref__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Nested_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_InByref_OutByval>(L"[1234][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Nested_InByref_OutByval::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Nested::InByref::OutByval::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Nested__InByref__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Nested_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_InByval_OutByref>(L"[123][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Nested_InByval_OutByref::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Nested::InByval::OutByref::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Nested__InByval__OutByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Nested_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_InByval_OutByval>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Nested_InByval_OutByval::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Nested::InByval::OutByval::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Nested__InByval__OutByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Nested_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_PropByref>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Nested_PropByref::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Nested::InByref::PropByref::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Nested__InByref__PropByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Nested_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_PropByval>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Nested_PropByval::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Nested::InByval::PropByval::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Nested__InByval__PropByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_Nested_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_PropDefault>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_Nested_PropDefault::Instance();
			if (dynamic_cast<::RpcCollectionOblist::Nested::PropDefault::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__Nested__PropDefault__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_PropByref>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_PropByref::Instance();
			if (dynamic_cast<::RpcCollectionOblist::InByref::PropByref::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__InByref__PropByref__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_PropByval>(L"[123][1234][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_PropByval::Instance();
			if (dynamic_cast<::RpcCollectionOblist::InByval::PropByval::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__InByval__PropByval__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:CollectionOblist_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_PropDefault>(L"[12345][12345][12345]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_CollectionOblist_PropDefault::Instance();
			if (dynamic_cast<::RpcCollectionOblist::PropDefault::IService*>(obj)) return instance.rpctype_RpcCollectionOblist__PropDefault__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Dtor")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Dtor>(L"[Not Deleted][Deleted]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Dtor::Instance();
			if (dynamic_cast<::RpcDtorTest::IService*>(obj)) return instance.rpctype_RpcDtorTest__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Dtor2")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Dtor2>(L"[Not Deleted][Deleted]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Dtor2::Instance();
			if (dynamic_cast<::RpcDtor2Test::IService*>(obj)) return instance.rpctype_RpcDtor2Test__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Dtor3")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Dtor3>(L"[1][IContainer][IValue][2][IContainer][3][IValue][4]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Dtor3::Instance();
			if (dynamic_cast<::RpcDtor3Test::IContainer*>(obj)) return instance.rpctype_RpcDtor3Test__IContainer;
			if (dynamic_cast<::RpcDtor3Test::IValue*>(obj)) return instance.rpctype_RpcDtor3Test__IValue;
			if (dynamic_cast<::RpcDtor3Test::IService*>(obj)) return instance.rpctype_RpcDtor3Test__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:DtorList")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_DtorList>(L"[a1][a2][IValue][a3][b1][b2][b3][IValue][c1][c2][c3][IValue][d1][d2][d3][IValue][e1][e2][e3][IValue][f1][f2][IValue][f3]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_DtorList::Instance();
			if (dynamic_cast<::RpcDtorList::IValue*>(obj)) return instance.rpctype_RpcDtorList__IValue;
			if (dynamic_cast<::RpcDtorList::IService*>(obj)) return instance.rpctype_RpcDtorList__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:DtorList2")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_DtorList2>(L"[a1][a2][IValue][a3][b1][b2][b3][IValue][c1][c2][c3][IValue][d1][d2][d3][IValue][e1][e2][e3][IValue]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_DtorList2::Instance();
			if (dynamic_cast<::RpcDtorList2::IValue*>(obj)) return instance.rpctype_RpcDtorList2__IValue;
			if (dynamic_cast<::RpcDtorList2::IService*>(obj)) return instance.rpctype_RpcDtorList2__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:DtorPropCached")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_DtorPropCached>(L"[1][Deleted:A][2][3][4][ValueChanged][5][6][Deleted:B][7][8][ValueChanged][9][Deleted:C]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_DtorPropCached::Instance();
			if (dynamic_cast<::RpcDtorPropCached::IValue*>(obj)) return instance.rpctype_RpcDtorPropCached__IValue;
			if (dynamic_cast<::RpcDtorPropCached::IService*>(obj)) return instance.rpctype_RpcDtorPropCached__IService;
			return RpcTypeId_NotFound;
		},
		[](RpcDualLifecycleMock* mock, RpcObjectReference ref, IDescriptable* obj, List<Func<void()>>& detachments)
		{
			(void)detachments;
			::vl_workflow_global::Rpc_DtorPropCached::Instance().rpclistener_Attach(ref.typeId, mock, ref, obj);
		},
		nullptr);
});

TEST_CASE(L"Rpc:DtorPropCachedListByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_DtorPropCachedListByval>(L"[1][Deleted:A][2][3][4][ValueChanged][5][6][Deleted:B][7][8][ValueChanged][9][Deleted:C]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_DtorPropCachedListByval::Instance();
			if (dynamic_cast<::RpcDtorPropCachedListByval::IValue*>(obj)) return instance.rpctype_RpcDtorPropCachedListByval__IValue;
			if (dynamic_cast<::RpcDtorPropCachedListByval::IService*>(obj)) return instance.rpctype_RpcDtorPropCachedListByval__IService;
			return RpcTypeId_NotFound;
		},
		[](RpcDualLifecycleMock* mock, RpcObjectReference ref, IDescriptable* obj, List<Func<void()>>& detachments)
		{
			(void)detachments;
			::vl_workflow_global::Rpc_DtorPropCachedListByval::Instance().rpclistener_Attach(ref.typeId, mock, ref, obj);
		},
		nullptr);
});

TEST_CASE(L"Rpc:DtorPropCachedListVByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_DtorPropCachedListVByref>(L"[1][Deleted:A][2][3][4][ValueChanged][5][6][Deleted:B][7][8][ValueChanged][9][Deleted:C]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_DtorPropCachedListVByref::Instance();
			if (dynamic_cast<::RpcDtorPropCachedListVByref::IValue*>(obj)) return instance.rpctype_RpcDtorPropCachedListVByref__IValue;
			if (dynamic_cast<::RpcDtorPropCachedListVByref::IService*>(obj)) return instance.rpctype_RpcDtorPropCachedListVByref__IService;
			return RpcTypeId_NotFound;
		},
		[](RpcDualLifecycleMock* mock, RpcObjectReference ref, IDescriptable* obj, List<Func<void()>>& detachments)
		{
			(void)detachments;
			::vl_workflow_global::Rpc_DtorPropCachedListVByref::Instance().rpclistener_Attach(ref.typeId, mock, ref, obj);
		},
		nullptr);
});

TEST_CASE(L"Rpc:Event")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Event>(L"[clientMain:A][serviceMain:B]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Event::Instance();
			if (dynamic_cast<::RpcEvent::IService*>(obj)) return instance.rpctype_RpcEvent__IService;
			return RpcTypeId_NotFound;
		},
		[](RpcDualLifecycleMock* mock, RpcObjectReference ref, IDescriptable* obj, List<Func<void()>>& detachments)
		{
			(void)detachments;
			::vl_workflow_global::Rpc_Event::Instance().rpclistener_Attach(ref.typeId, mock, ref, obj);
		},
		nullptr);
});

TEST_CASE(L"Rpc:EventOblist")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_EventOblist>(L"[clientMain:0,0,1][clientMain:1,0,1][clientMain:0,1,1][clientMain:1,1,0][clientMain:0,1,0][serviceMain:0,0,1][serviceMain:1,0,1][serviceMain:0,1,1][serviceMain:1,1,0][serviceMain:0,1,0]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_EventOblist::Instance();
			if (dynamic_cast<::RpcEventOblist::IService*>(obj)) return instance.rpctype_RpcEventOblist__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:FailDoubleRegistration")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_FailDoubleRegistration>(L"[call][service:Received 1st][client:Received 2nd][call][service:Received 1st][exception][call][exception][call][exception]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_FailDoubleRegistration::Instance();
			if (dynamic_cast<::RpcFailDoubleRegistrationTest::IObject*>(obj)) return instance.rpctype_RpcFailDoubleRegistrationTest__IObject;
			if (dynamic_cast<::RpcFailDoubleRegistrationTest::IService*>(obj)) return instance.rpctype_RpcFailDoubleRegistrationTest__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:LocalAndWrapper")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_LocalAndWrapper>(L"[false][true][true][false]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_LocalAndWrapper::Instance();
			if (dynamic_cast<::RpcWrapperTest::IObj1*>(obj)) return instance.rpctype_RpcWrapperTest__IObj1;
			if (dynamic_cast<::RpcWrapperTest::IObj2*>(obj)) return instance.rpctype_RpcWrapperTest__IObj2;
			if (dynamic_cast<::RpcWrapperTest::IService*>(obj)) return instance.rpctype_RpcWrapperTest__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:Overloading")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Overloading>(L"[123][true][abc][xyz][123,true,abc,xyz]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_Overloading::Instance();
			if (dynamic_cast<::RpcOverloadingTest::IStringRepresentable*>(obj)) return instance.rpctype_RpcOverloadingTest__IStringRepresentable;
			if (dynamic_cast<::RpcOverloadingTest::IService*>(obj)) return instance.rpctype_RpcOverloadingTest__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:PrimitiveTypes")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_PrimitiveTypes>(L"[6][12][1.75][2.875][Hi!][false][Autumn][13,27]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_PrimitiveTypes::Instance();
			if (dynamic_cast<::RpcPrimitiveTest::IService*>(obj)) return instance.rpctype_RpcPrimitiveTest__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:PropCached")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_PropCached>(L"[A][A][ValueChanged][B]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_PropCached::Instance();
			if (dynamic_cast<::RpcPropCached::IService*>(obj)) return instance.rpctype_RpcPropCached__IService;
			return RpcTypeId_NotFound;
		},
		[](RpcDualLifecycleMock* mock, RpcObjectReference ref, IDescriptable* obj, List<Func<void()>>& detachments)
		{
			(void)detachments;
			::vl_workflow_global::Rpc_PropCached::Instance().rpclistener_Attach(ref.typeId, mock, ref, obj);
		},
		nullptr);
});

TEST_CASE(L"Rpc:PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_PropDefault>(L"[A][A][ValueChanged][B]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_PropDefault::Instance();
			if (dynamic_cast<::RpcPropDefault::IService*>(obj)) return instance.rpctype_RpcPropDefault__IService;
			return RpcTypeId_NotFound;
		},
		[](RpcDualLifecycleMock* mock, RpcObjectReference ref, IDescriptable* obj, List<Func<void()>>& detachments)
		{
			(void)detachments;
			::vl_workflow_global::Rpc_PropDefault::Instance().rpclistener_Attach(ref.typeId, mock, ref, obj);
		},
		nullptr);
});

TEST_CASE(L"Rpc:PropDefaultInterface")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_PropDefaultInterface>(L"[A][A][ValueChanged][B]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_PropDefaultInterface::Instance();
			if (dynamic_cast<::RpcPropDefaultInterface::IValue*>(obj)) return instance.rpctype_RpcPropDefaultInterface__IValue;
			if (dynamic_cast<::RpcPropDefaultInterface::IService*>(obj)) return instance.rpctype_RpcPropDefaultInterface__IService;
			return RpcTypeId_NotFound;
		},
		[](RpcDualLifecycleMock* mock, RpcObjectReference ref, IDescriptable* obj, List<Func<void()>>& detachments)
		{
			(void)detachments;
			::vl_workflow_global::Rpc_PropDefaultInterface::Instance().rpclistener_Attach(ref.typeId, mock, ref, obj);
		},
		nullptr);
});

TEST_CASE(L"Rpc:PropDefaultInterfaceList")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_PropDefaultInterfaceList>(L"[A][A][ValueChanged][BCD]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_PropDefaultInterfaceList::Instance();
			if (dynamic_cast<::RpcPropDefaultInterfaceList::IValue*>(obj)) return instance.rpctype_RpcPropDefaultInterfaceList__IValue;
			if (dynamic_cast<::RpcPropDefaultInterfaceList::IService*>(obj)) return instance.rpctype_RpcPropDefaultInterfaceList__IService;
			return RpcTypeId_NotFound;
		},
		[](RpcDualLifecycleMock* mock, RpcObjectReference ref, IDescriptable* obj, List<Func<void()>>& detachments)
		{
			(void)detachments;
			::vl_workflow_global::Rpc_PropDefaultInterfaceList::Instance().rpclistener_Attach(ref.typeId, mock, ref, obj);
		},
		nullptr);
});

TEST_CASE(L"Rpc:PropDefaultList")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_PropDefaultList>(L"[A][A][ValueChanged][BCD]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_PropDefaultList::Instance();
			if (dynamic_cast<::RpcPropDefaultList::IService*>(obj)) return instance.rpctype_RpcPropDefaultList__IService;
			return RpcTypeId_NotFound;
		},
		[](RpcDualLifecycleMock* mock, RpcObjectReference ref, IDescriptable* obj, List<Func<void()>>& detachments)
		{
			(void)detachments;
			::vl_workflow_global::Rpc_PropDefaultList::Instance().rpclistener_Attach(ref.typeId, mock, ref, obj);
		},
		nullptr);
});

TEST_CASE(L"Rpc:PropDynamic")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_PropDynamic>(L"[A][B][ValueChanged][B]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_PropDynamic::Instance();
			if (dynamic_cast<::RpcPropDynamic::IService*>(obj)) return instance.rpctype_RpcPropDynamic__IService;
			return RpcTypeId_NotFound;
		},
		[](RpcDualLifecycleMock* mock, RpcObjectReference ref, IDescriptable* obj, List<Func<void()>>& detachments)
		{
			(void)detachments;
			::vl_workflow_global::Rpc_PropDynamic::Instance().rpclistener_Attach(ref.typeId, mock, ref, obj);
		},
		nullptr);
});

TEST_CASE(L"Rpc:RequestService")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_RequestService>(L"Hello",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_RequestService::Instance();
			if (dynamic_cast<::RpcTest::IService*>(obj)) return instance.rpctype_RpcTest__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});

TEST_CASE(L"Rpc:ServiceWrapper")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_ServiceWrapper>(L"[false][true]",
		[](IDescriptable* obj) -> vint
		{
			auto& instance = ::vl_workflow_global::Rpc_ServiceWrapper::Instance();
			if (dynamic_cast<::RpcServiceWrapperTest::IService*>(obj)) return instance.rpctype_RpcServiceWrapperTest__IService;
			return RpcTypeId_NotFound;
		},
		nullptr,
		nullptr);
});
}
