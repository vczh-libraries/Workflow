#include "RpcCollection_DefaultReflection.h"
#include "RpcCollection_InByref_OutByrefReflection.h"
#include "RpcCollection_InByref_OutByvalReflection.h"
#include "RpcCollection_InByval_OutByrefReflection.h"
#include "RpcCollection_InByval_OutByvalReflection.h"
#include "RpcCollection_Interface_DefaultReflection.h"
#include "RpcCollection_Interface_InByref_OutByrefReflection.h"
#include "RpcCollection_Interface_InByref_OutByvalReflection.h"
#include "RpcCollection_Interface_InByval_OutByrefReflection.h"
#include "RpcCollection_Interface_InByval_OutByvalReflection.h"
#include "RpcCollection_Interface_Nested_DefaultReflection.h"
#include "RpcCollection_Interface_Nested_InByref_OutByrefReflection.h"
#include "RpcCollection_Interface_Nested_InByref_OutByvalReflection.h"
#include "RpcCollection_Interface_Nested_InByval_OutByrefReflection.h"
#include "RpcCollection_Interface_Nested_InByval_OutByvalReflection.h"
#include "RpcCollection_Interface_Nested_PropByrefReflection.h"
#include "RpcCollection_Interface_Nested_PropByvalReflection.h"
#include "RpcCollection_Interface_Nested_PropDefaultReflection.h"
#include "RpcCollection_Interface_PropByrefReflection.h"
#include "RpcCollection_Interface_PropByvalReflection.h"
#include "RpcCollection_Interface_PropDefaultReflection.h"
#include "RpcCollection_Nested_DefaultReflection.h"
#include "RpcCollection_Nested_InByref_OutByrefReflection.h"
#include "RpcCollection_Nested_InByref_OutByvalReflection.h"
#include "RpcCollection_Nested_InByval_OutByrefReflection.h"
#include "RpcCollection_Nested_InByval_OutByvalReflection.h"
#include "RpcCollection_Nested_PropByrefReflection.h"
#include "RpcCollection_Nested_PropByvalReflection.h"
#include "RpcCollection_Nested_PropDefaultReflection.h"
#include "RpcCollection_PropByrefReflection.h"
#include "RpcCollection_PropByvalReflection.h"
#include "RpcCollection_PropDefaultReflection.h"
#include "RpcCollectionDict_DefaultReflection.h"
#include "RpcCollectionDict_InByref_OutByrefReflection.h"
#include "RpcCollectionDict_InByref_OutByvalReflection.h"
#include "RpcCollectionDict_InByval_OutByrefReflection.h"
#include "RpcCollectionDict_InByval_OutByvalReflection.h"
#include "RpcCollectionDict_Interface_DefaultReflection.h"
#include "RpcCollectionDict_Interface_InByref_OutByrefReflection.h"
#include "RpcCollectionDict_Interface_InByref_OutByvalReflection.h"
#include "RpcCollectionDict_Interface_InByval_OutByrefReflection.h"
#include "RpcCollectionDict_Interface_InByval_OutByvalReflection.h"
#include "RpcCollectionDict_Interface_Nested_DefaultReflection.h"
#include "RpcCollectionDict_Interface_Nested_InByref_OutByrefReflection.h"
#include "RpcCollectionDict_Interface_Nested_InByref_OutByvalReflection.h"
#include "RpcCollectionDict_Interface_Nested_InByval_OutByrefReflection.h"
#include "RpcCollectionDict_Interface_Nested_InByval_OutByvalReflection.h"
#include "RpcCollectionDict_Interface_Nested_PropByrefReflection.h"
#include "RpcCollectionDict_Interface_Nested_PropByvalReflection.h"
#include "RpcCollectionDict_Interface_Nested_PropDefaultReflection.h"
#include "RpcCollectionDict_Interface_PropByrefReflection.h"
#include "RpcCollectionDict_Interface_PropByvalReflection.h"
#include "RpcCollectionDict_Interface_PropDefaultReflection.h"
#include "RpcCollectionDict_Nested_DefaultReflection.h"
#include "RpcCollectionDict_Nested_InByref_OutByrefReflection.h"
#include "RpcCollectionDict_Nested_InByref_OutByvalReflection.h"
#include "RpcCollectionDict_Nested_InByval_OutByrefReflection.h"
#include "RpcCollectionDict_Nested_InByval_OutByvalReflection.h"
#include "RpcCollectionDict_Nested_PropByrefReflection.h"
#include "RpcCollectionDict_Nested_PropByvalReflection.h"
#include "RpcCollectionDict_Nested_PropDefaultReflection.h"
#include "RpcCollectionDict_PropByrefReflection.h"
#include "RpcCollectionDict_PropByvalReflection.h"
#include "RpcCollectionDict_PropDefaultReflection.h"
#include "RpcCollectionOblist_DefaultReflection.h"
#include "RpcCollectionOblist_InByref_OutByrefReflection.h"
#include "RpcCollectionOblist_InByref_OutByvalReflection.h"
#include "RpcCollectionOblist_InByval_OutByrefReflection.h"
#include "RpcCollectionOblist_InByval_OutByvalReflection.h"
#include "RpcCollectionOblist_Interface_DefaultReflection.h"
#include "RpcCollectionOblist_Interface_InByref_OutByrefReflection.h"
#include "RpcCollectionOblist_Interface_InByref_OutByvalReflection.h"
#include "RpcCollectionOblist_Interface_InByval_OutByrefReflection.h"
#include "RpcCollectionOblist_Interface_InByval_OutByvalReflection.h"
#include "RpcCollectionOblist_Interface_Nested_DefaultReflection.h"
#include "RpcCollectionOblist_Interface_Nested_InByref_OutByrefReflection.h"
#include "RpcCollectionOblist_Interface_Nested_InByref_OutByvalReflection.h"
#include "RpcCollectionOblist_Interface_Nested_InByval_OutByrefReflection.h"
#include "RpcCollectionOblist_Interface_Nested_InByval_OutByvalReflection.h"
#include "RpcCollectionOblist_Interface_Nested_PropByrefReflection.h"
#include "RpcCollectionOblist_Interface_Nested_PropByvalReflection.h"
#include "RpcCollectionOblist_Interface_Nested_PropDefaultReflection.h"
#include "RpcCollectionOblist_Interface_PropByrefReflection.h"
#include "RpcCollectionOblist_Interface_PropByvalReflection.h"
#include "RpcCollectionOblist_Interface_PropDefaultReflection.h"
#include "RpcCollectionOblist_Nested_DefaultReflection.h"
#include "RpcCollectionOblist_Nested_InByref_OutByrefReflection.h"
#include "RpcCollectionOblist_Nested_InByref_OutByvalReflection.h"
#include "RpcCollectionOblist_Nested_InByval_OutByrefReflection.h"
#include "RpcCollectionOblist_Nested_InByval_OutByvalReflection.h"
#include "RpcCollectionOblist_Nested_PropByrefReflection.h"
#include "RpcCollectionOblist_Nested_PropByvalReflection.h"
#include "RpcCollectionOblist_Nested_PropDefaultReflection.h"
#include "RpcCollectionOblist_PropByrefReflection.h"
#include "RpcCollectionOblist_PropByvalReflection.h"
#include "RpcCollectionOblist_PropDefaultReflection.h"
#include "RpcDtorReflection.h"
#include "RpcDtor2Reflection.h"
#include "RpcDtor3Reflection.h"
#include "RpcDtorListReflection.h"
#include "RpcDtorList2Reflection.h"
#include "RpcDtorPropCachedReflection.h"
#include "RpcDtorPropCachedListByvalReflection.h"
#include "RpcDtorPropCachedListVByrefReflection.h"
#include "RpcEventReflection.h"
#include "RpcEventArgsReflection.h"
#include "RpcEventOblistReflection.h"
#include "RpcListOps_DictionaryExceptionReflection.h"
#include "RpcListOps_ListExceptionReflection.h"
#include "RpcListOps_OblistEventExceptionReflection.h"
#include "RpcFailDoubleRegistrationReflection.h"
#include "RpcInheritanceReflection.h"
#include "RpcInheritance_MethodExceptionReflection.h"
#include "RpcInheritance_EventExceptionReflection.h"
#include "RpcLocalAndWrapperReflection.h"
#include "RpcNullableReflection.h"
#include "RpcOverloadingReflection.h"
#include "RpcPrimitiveTypesReflection.h"
#include "RpcPropCachedReflection.h"
#include "RpcPropDefaultReflection.h"
#include "RpcPropDefaultInterfaceReflection.h"
#include "RpcPropDefaultInterfaceListReflection.h"
#include "RpcPropDefaultListReflection.h"
#include "RpcPropDynamicReflection.h"
#include "RpcRequestServiceReflection.h"
#include "RpcServiceWrapperReflection.h"
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
	 LoadRpc_EventArgsTypes();
	 LoadRpc_EventOblistTypes();
	 LoadRpc_ListOps_DictionaryExceptionTypes();
	 LoadRpc_ListOps_ListExceptionTypes();
	 LoadRpc_ListOps_OblistEventExceptionTypes();
	 LoadRpc_FailDoubleRegistrationTypes();
	 LoadRpc_InheritanceTypes();
	 LoadRpc_Inheritance_MethodExceptionTypes();
	 LoadRpc_Inheritance_EventExceptionTypes();
	 LoadRpc_LocalAndWrapperTypes();
	 LoadRpc_NullableTypes();
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
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Default, false>(L"Collection_Default", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:Collection_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_InByref_OutByref, false>(L"Collection_InByref_OutByref", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:Collection_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_InByref_OutByval, false>(L"Collection_InByref_OutByval", L"[1234][1234][12345]");
});

TEST_CASE(L"Rpc:Collection_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_InByval_OutByref, false>(L"Collection_InByval_OutByref", L"[123][12345][12345]");
});

TEST_CASE(L"Rpc:Collection_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_InByval_OutByval, false>(L"Collection_InByval_OutByval", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:Collection_Interface_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_Default, false>(L"Collection_Interface_Default", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:Collection_Interface_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_InByref_OutByref, false>(L"Collection_Interface_InByref_OutByref", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:Collection_Interface_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_InByref_OutByval, false>(L"Collection_Interface_InByref_OutByval", L"[1234][1234][12345]");
});

TEST_CASE(L"Rpc:Collection_Interface_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_InByval_OutByref, false>(L"Collection_Interface_InByval_OutByref", L"[123][12345][12345]");
});

TEST_CASE(L"Rpc:Collection_Interface_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_InByval_OutByval, false>(L"Collection_Interface_InByval_OutByval", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:Collection_Interface_Nested_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_Default, false>(L"Collection_Interface_Nested_Default", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:Collection_Interface_Nested_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_InByref_OutByref, false>(L"Collection_Interface_Nested_InByref_OutByref", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:Collection_Interface_Nested_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_InByref_OutByval, false>(L"Collection_Interface_Nested_InByref_OutByval", L"[1234][1234][12345]");
});

TEST_CASE(L"Rpc:Collection_Interface_Nested_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_InByval_OutByref, false>(L"Collection_Interface_Nested_InByval_OutByref", L"[123][12345][12345]");
});

TEST_CASE(L"Rpc:Collection_Interface_Nested_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_InByval_OutByval, false>(L"Collection_Interface_Nested_InByval_OutByval", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:Collection_Interface_Nested_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_PropByref, false>(L"Collection_Interface_Nested_PropByref", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:Collection_Interface_Nested_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_PropByval, false>(L"Collection_Interface_Nested_PropByval", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:Collection_Interface_Nested_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_PropDefault, false>(L"Collection_Interface_Nested_PropDefault", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:Collection_Interface_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_PropByref, false>(L"Collection_Interface_PropByref", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:Collection_Interface_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_PropByval, false>(L"Collection_Interface_PropByval", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:Collection_Interface_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Interface_PropDefault, false>(L"Collection_Interface_PropDefault", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:Collection_Nested_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Nested_Default, false>(L"Collection_Nested_Default", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:Collection_Nested_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Nested_InByref_OutByref, false>(L"Collection_Nested_InByref_OutByref", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:Collection_Nested_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Nested_InByref_OutByval, false>(L"Collection_Nested_InByref_OutByval", L"[1234][1234][12345]");
});

TEST_CASE(L"Rpc:Collection_Nested_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Nested_InByval_OutByref, false>(L"Collection_Nested_InByval_OutByref", L"[123][12345][12345]");
});

TEST_CASE(L"Rpc:Collection_Nested_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Nested_InByval_OutByval, false>(L"Collection_Nested_InByval_OutByval", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:Collection_Nested_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Nested_PropByref, false>(L"Collection_Nested_PropByref", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:Collection_Nested_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Nested_PropByval, false>(L"Collection_Nested_PropByval", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:Collection_Nested_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_Nested_PropDefault, false>(L"Collection_Nested_PropDefault", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:Collection_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_PropByref, false>(L"Collection_PropByref", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:Collection_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_PropByval, false>(L"Collection_PropByval", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:Collection_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Collection_PropDefault, false>(L"Collection_PropDefault", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:CollectionDict_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Default, false>(L"CollectionDict_Default", L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_InByref_OutByref, false>(L"CollectionDict_InByref_OutByref", L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_InByref_OutByval, false>(L"CollectionDict_InByref_OutByval", L"[1A2B3C4D][1A2B3C4D][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_InByval_OutByref, false>(L"CollectionDict_InByval_OutByref", L"[1A2B3C][1A2B3C4D5E][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_InByval_OutByval, false>(L"CollectionDict_InByval_OutByval", L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Interface_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Default, false>(L"CollectionDict_Interface_Default", L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Interface_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_InByref_OutByref, false>(L"CollectionDict_Interface_InByref_OutByref", L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Interface_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_InByref_OutByval, false>(L"CollectionDict_Interface_InByref_OutByval", L"[1A2B3C4D][1A2B3C4D][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Interface_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_InByval_OutByref, false>(L"CollectionDict_Interface_InByval_OutByref", L"[1A2B3C][1A2B3C4D5E][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Interface_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_InByval_OutByval, false>(L"CollectionDict_Interface_InByval_OutByval", L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Interface_Nested_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_Default, false>(L"CollectionDict_Interface_Nested_Default", L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Interface_Nested_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByref_OutByref, false>(L"CollectionDict_Interface_Nested_InByref_OutByref", L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Interface_Nested_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByref_OutByval, false>(L"CollectionDict_Interface_Nested_InByref_OutByval", L"[1A2B3C4D][1A2B3C4D][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Interface_Nested_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByval_OutByref, false>(L"CollectionDict_Interface_Nested_InByval_OutByref", L"[1A2B3C][1A2B3C4D5E][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Interface_Nested_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByval_OutByval, false>(L"CollectionDict_Interface_Nested_InByval_OutByval", L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Interface_Nested_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_PropByref, false>(L"CollectionDict_Interface_Nested_PropByref", L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Interface_Nested_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_PropByval, false>(L"CollectionDict_Interface_Nested_PropByval", L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Interface_Nested_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_PropDefault, false>(L"CollectionDict_Interface_Nested_PropDefault", L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Interface_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_PropByref, false>(L"CollectionDict_Interface_PropByref", L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Interface_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_PropByval, false>(L"CollectionDict_Interface_PropByval", L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Interface_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_PropDefault, false>(L"CollectionDict_Interface_PropDefault", L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Nested_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_Default, false>(L"CollectionDict_Nested_Default", L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Nested_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_InByref_OutByref, false>(L"CollectionDict_Nested_InByref_OutByref", L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Nested_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_InByref_OutByval, false>(L"CollectionDict_Nested_InByref_OutByval", L"[1A2B3C4D][1A2B3C4D][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Nested_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_InByval_OutByref, false>(L"CollectionDict_Nested_InByval_OutByref", L"[1A2B3C][1A2B3C4D5E][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Nested_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_InByval_OutByval, false>(L"CollectionDict_Nested_InByval_OutByval", L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Nested_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_PropByref, false>(L"CollectionDict_Nested_PropByref", L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Nested_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_PropByval, false>(L"CollectionDict_Nested_PropByval", L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_Nested_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_PropDefault, false>(L"CollectionDict_Nested_PropDefault", L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_PropByref, false>(L"CollectionDict_PropByref", L"[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_PropByval, false>(L"CollectionDict_PropByval", L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionDict_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionDict_PropDefault, false>(L"CollectionDict_PropDefault", L"[1A2B3C][1A2B3C4D][1A2B3C4D5E]");
});

TEST_CASE(L"Rpc:CollectionOblist_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Default, false>(L"CollectionOblist_Default", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_InByref_OutByref, false>(L"CollectionOblist_InByref_OutByref", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_InByref_OutByval, false>(L"CollectionOblist_InByref_OutByval", L"[1234][1234][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_InByval_OutByref, false>(L"CollectionOblist_InByval_OutByref", L"[123][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_InByval_OutByval, false>(L"CollectionOblist_InByval_OutByval", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Default, false>(L"CollectionOblist_Interface_Default", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_InByref_OutByref, false>(L"CollectionOblist_Interface_InByref_OutByref", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_InByref_OutByval, false>(L"CollectionOblist_Interface_InByref_OutByval", L"[1234][1234][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_InByval_OutByref, false>(L"CollectionOblist_Interface_InByval_OutByref", L"[123][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_InByval_OutByval, false>(L"CollectionOblist_Interface_InByval_OutByval", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_Nested_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_Default, false>(L"CollectionOblist_Interface_Nested_Default", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_Nested_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByref_OutByref, false>(L"CollectionOblist_Interface_Nested_InByref_OutByref", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_Nested_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByref_OutByval, false>(L"CollectionOblist_Interface_Nested_InByref_OutByval", L"[1234][1234][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_Nested_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByval_OutByref, false>(L"CollectionOblist_Interface_Nested_InByval_OutByref", L"[123][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_Nested_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByval_OutByval, false>(L"CollectionOblist_Interface_Nested_InByval_OutByval", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_Nested_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_PropByref, false>(L"CollectionOblist_Interface_Nested_PropByref", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_Nested_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_PropByval, false>(L"CollectionOblist_Interface_Nested_PropByval", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_Nested_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_PropDefault, false>(L"CollectionOblist_Interface_Nested_PropDefault", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_PropByref, false>(L"CollectionOblist_Interface_PropByref", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_PropByval, false>(L"CollectionOblist_Interface_PropByval", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Interface_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_PropDefault, false>(L"CollectionOblist_Interface_PropDefault", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Nested_Default")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_Default, false>(L"CollectionOblist_Nested_Default", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Nested_InByref_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_InByref_OutByref, false>(L"CollectionOblist_Nested_InByref_OutByref", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Nested_InByref_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_InByref_OutByval, false>(L"CollectionOblist_Nested_InByref_OutByval", L"[1234][1234][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Nested_InByval_OutByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_InByval_OutByref, false>(L"CollectionOblist_Nested_InByval_OutByref", L"[123][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Nested_InByval_OutByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_InByval_OutByval, false>(L"CollectionOblist_Nested_InByval_OutByval", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Nested_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_PropByref, false>(L"CollectionOblist_Nested_PropByref", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Nested_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_PropByval, false>(L"CollectionOblist_Nested_PropByval", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_Nested_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_PropDefault, false>(L"CollectionOblist_Nested_PropDefault", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_PropByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_PropByref, false>(L"CollectionOblist_PropByref", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_PropByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_PropByval, false>(L"CollectionOblist_PropByval", L"[123][1234][12345]");
});

TEST_CASE(L"Rpc:CollectionOblist_PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_CollectionOblist_PropDefault, false>(L"CollectionOblist_PropDefault", L"[12345][12345][12345]");
});

TEST_CASE(L"Rpc:Dtor")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Dtor, false>(L"Dtor", L"[Not Deleted][Deleted]");
});

TEST_CASE(L"Rpc:Dtor2")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Dtor2, false>(L"Dtor2", L"[Not Deleted][Deleted]");
});

TEST_CASE(L"Rpc:Dtor3")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Dtor3, false>(L"Dtor3", L"[1][IContainer][IValue][2][IContainer][3][IValue][4]");
});

TEST_CASE(L"Rpc:DtorList")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_DtorList, false>(L"DtorList", L"[a1][a2][IValue][a3][b1][b2][b3][IValue][c1][c2][c3][IValue][d1][d2][d3][IValue][e1][e2][e3][IValue][f1][f2][IValue][f3]");
});

TEST_CASE(L"Rpc:DtorList2")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_DtorList2, false>(L"DtorList2", L"[a1][a2][IValue][a3][b1][b2][b3][IValue][c1][c2][c3][IValue][d1][d2][d3][IValue][e1][e2][e3][IValue]");
});

TEST_CASE(L"Rpc:DtorPropCached")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_DtorPropCached, true>(L"DtorPropCached", L"[1][Deleted:A][2][3][4][ValueChanged][5][6][Deleted:B][7][8][ValueChanged][9][Deleted:C]");
});

TEST_CASE(L"Rpc:DtorPropCachedListByval")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_DtorPropCachedListByval, true>(L"DtorPropCachedListByval", L"[1][Deleted:A][2][3][4][ValueChanged][5][6][Deleted:B][7][8][ValueChanged][9][Deleted:C]");
});

TEST_CASE(L"Rpc:DtorPropCachedListVByref")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_DtorPropCachedListVByref, true>(L"DtorPropCachedListVByref", L"[1][Deleted:A][2][3][4][ValueChanged][5][6][Deleted:B][7][8][ValueChanged][9][Deleted:C]");
});

TEST_CASE(L"Rpc:Event")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Event, true>(L"Event", L"[clientMain:A][serviceMain:B]");
});

TEST_CASE(L"Rpc:EventArgs")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_EventArgs, true>(L"EventArgs", L"[1][2][3][4][6]");
});

TEST_CASE(L"Rpc:EventOblist")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_EventOblist, false>(L"EventOblist", L"[clientMain:0,0,1][clientMain:1,0,1][clientMain:0,1,1][clientMain:1,1,0][clientMain:0,1,0][serviceMain:0,0,1][serviceMain:1,0,1][serviceMain:0,1,1][serviceMain:1,1,0][serviceMain:0,1,0]");
});

TEST_CASE(L"Rpc:ListOps_DictionaryException")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_ListOps_DictionaryException, false>(L"ListOps_DictionaryException", L"ArrayBase<T, K>::Get(vint)#Argument index not in range.");
});

TEST_CASE(L"Rpc:ListOps_ListException")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_ListOps_ListException, false>(L"ListOps_ListException", L"ArrayBase<T, K>::Get(vint)#Argument index not in range.");
});

TEST_CASE(L"Rpc:ListOps_OblistEventException")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_ListOps_OblistEventException, false>(L"ListOps_OblistEventException", L"0,0,1");
});

TEST_CASE(L"Rpc:FailDoubleRegistration")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_FailDoubleRegistration, false>(L"FailDoubleRegistration", L"[call][service:Received 1st][client:Received 2nd][call][service:Received 1st][exception][call][exception][call][exception]");
});

TEST_CASE(L"Rpc:Inheritance")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Inheritance, false>(L"Inheritance", L"[][One][][Two][][][Derived]");
});

TEST_CASE(L"Rpc:Inheritance_MethodException")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Inheritance_MethodException, false>(L"Inheritance_MethodException", L"[][DoNotSetOneValue][][DoNotSetTwoValue]");
});

TEST_CASE(L"Rpc:Inheritance_EventException")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Inheritance_EventException, true>(L"Inheritance_EventException", L"[][1:CrashedAtServer;][][2:CrashedAtClient;]");
});

TEST_CASE(L"Rpc:LocalAndWrapper")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_LocalAndWrapper, false>(L"LocalAndWrapper", L"[false][true][true][false]");
});

TEST_CASE(L"Rpc:Nullable")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Nullable, false>(L"Nullable", L"[abc][null][null][def]");
});

TEST_CASE(L"Rpc:Overloading")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_Overloading, false>(L"Overloading", L"[123][true][abc][xyz][123,true,abc,xyz]");
});

TEST_CASE(L"Rpc:PrimitiveTypes")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_PrimitiveTypes, false>(L"PrimitiveTypes", L"[6][12][1.75][2.875][Hi!][false][Autumn][13,27]");
});

TEST_CASE(L"Rpc:PropCached")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_PropCached, true>(L"PropCached", L"[A][A][ValueChanged][B]");
});

TEST_CASE(L"Rpc:PropDefault")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_PropDefault, true>(L"PropDefault", L"[A][A][ValueChanged][B]");
});

TEST_CASE(L"Rpc:PropDefaultInterface")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_PropDefaultInterface, true>(L"PropDefaultInterface", L"[A][A][ValueChanged][B]");
});

TEST_CASE(L"Rpc:PropDefaultInterfaceList")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_PropDefaultInterfaceList, true>(L"PropDefaultInterfaceList", L"[A][A][ValueChanged][BCD]");
});

TEST_CASE(L"Rpc:PropDefaultList")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_PropDefaultList, true>(L"PropDefaultList", L"[A][A][ValueChanged][BCD]");
});

TEST_CASE(L"Rpc:PropDynamic")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_PropDynamic, true>(L"PropDynamic", L"[A][B][ValueChanged][B]");
});

TEST_CASE(L"Rpc:RequestService")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_RequestService, false>(L"RequestService", L"Hello");
});

TEST_CASE(L"Rpc:ServiceWrapper")
{
	RunRpcTestCase<::vl_workflow_global::Rpc_ServiceWrapper, false>(L"ServiceWrapper", L"[false][true]");
});
}
