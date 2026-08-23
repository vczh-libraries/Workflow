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
#include "../Source/TestCasesRpcStdio_Service.h"

using namespace vl;
using namespace vl::rpc_controller_test;

void RunTestCasesRpcStdio_Service(const WString& itemName)
{
	if (itemName == L"Collection_Default")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Default, false>();
		return;
	}
	if (itemName == L"Collection_InByref_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_InByref_OutByref, false>();
		return;
	}
	if (itemName == L"Collection_InByref_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_InByref_OutByval, false>();
		return;
	}
	if (itemName == L"Collection_InByval_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_InByval_OutByref, false>();
		return;
	}
	if (itemName == L"Collection_InByval_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_InByval_OutByval, false>();
		return;
	}
	if (itemName == L"Collection_Interface_Default")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Interface_Default, false>();
		return;
	}
	if (itemName == L"Collection_Interface_InByref_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Interface_InByref_OutByref, false>();
		return;
	}
	if (itemName == L"Collection_Interface_InByref_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Interface_InByref_OutByval, false>();
		return;
	}
	if (itemName == L"Collection_Interface_InByval_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Interface_InByval_OutByref, false>();
		return;
	}
	if (itemName == L"Collection_Interface_InByval_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Interface_InByval_OutByval, false>();
		return;
	}
	if (itemName == L"Collection_Interface_Nested_Default")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Interface_Nested_Default, false>();
		return;
	}
	if (itemName == L"Collection_Interface_Nested_InByref_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Interface_Nested_InByref_OutByref, false>();
		return;
	}
	if (itemName == L"Collection_Interface_Nested_InByref_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Interface_Nested_InByref_OutByval, false>();
		return;
	}
	if (itemName == L"Collection_Interface_Nested_InByval_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Interface_Nested_InByval_OutByref, false>();
		return;
	}
	if (itemName == L"Collection_Interface_Nested_InByval_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Interface_Nested_InByval_OutByval, false>();
		return;
	}
	if (itemName == L"Collection_Interface_Nested_PropByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Interface_Nested_PropByref, false>();
		return;
	}
	if (itemName == L"Collection_Interface_Nested_PropByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Interface_Nested_PropByval, false>();
		return;
	}
	if (itemName == L"Collection_Interface_Nested_PropDefault")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Interface_Nested_PropDefault, false>();
		return;
	}
	if (itemName == L"Collection_Interface_PropByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Interface_PropByref, false>();
		return;
	}
	if (itemName == L"Collection_Interface_PropByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Interface_PropByval, false>();
		return;
	}
	if (itemName == L"Collection_Interface_PropDefault")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Interface_PropDefault, false>();
		return;
	}
	if (itemName == L"Collection_Nested_Default")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Nested_Default, false>();
		return;
	}
	if (itemName == L"Collection_Nested_InByref_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Nested_InByref_OutByref, false>();
		return;
	}
	if (itemName == L"Collection_Nested_InByref_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Nested_InByref_OutByval, false>();
		return;
	}
	if (itemName == L"Collection_Nested_InByval_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Nested_InByval_OutByref, false>();
		return;
	}
	if (itemName == L"Collection_Nested_InByval_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Nested_InByval_OutByval, false>();
		return;
	}
	if (itemName == L"Collection_Nested_PropByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Nested_PropByref, false>();
		return;
	}
	if (itemName == L"Collection_Nested_PropByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Nested_PropByval, false>();
		return;
	}
	if (itemName == L"Collection_Nested_PropDefault")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_Nested_PropDefault, false>();
		return;
	}
	if (itemName == L"Collection_PropByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_PropByref, false>();
		return;
	}
	if (itemName == L"Collection_PropByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_PropByval, false>();
		return;
	}
	if (itemName == L"Collection_PropDefault")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Collection_PropDefault, false>();
		return;
	}
	if (itemName == L"CollectionDict_Default")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Default, false>();
		return;
	}
	if (itemName == L"CollectionDict_InByref_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_InByref_OutByref, false>();
		return;
	}
	if (itemName == L"CollectionDict_InByref_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_InByref_OutByval, false>();
		return;
	}
	if (itemName == L"CollectionDict_InByval_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_InByval_OutByref, false>();
		return;
	}
	if (itemName == L"CollectionDict_InByval_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_InByval_OutByval, false>();
		return;
	}
	if (itemName == L"CollectionDict_Interface_Default")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Interface_Default, false>();
		return;
	}
	if (itemName == L"CollectionDict_Interface_InByref_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Interface_InByref_OutByref, false>();
		return;
	}
	if (itemName == L"CollectionDict_Interface_InByref_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Interface_InByref_OutByval, false>();
		return;
	}
	if (itemName == L"CollectionDict_Interface_InByval_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Interface_InByval_OutByref, false>();
		return;
	}
	if (itemName == L"CollectionDict_Interface_InByval_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Interface_InByval_OutByval, false>();
		return;
	}
	if (itemName == L"CollectionDict_Interface_Nested_Default")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_Default, false>();
		return;
	}
	if (itemName == L"CollectionDict_Interface_Nested_InByref_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByref_OutByref, false>();
		return;
	}
	if (itemName == L"CollectionDict_Interface_Nested_InByref_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByref_OutByval, false>();
		return;
	}
	if (itemName == L"CollectionDict_Interface_Nested_InByval_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByval_OutByref, false>();
		return;
	}
	if (itemName == L"CollectionDict_Interface_Nested_InByval_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByval_OutByval, false>();
		return;
	}
	if (itemName == L"CollectionDict_Interface_Nested_PropByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_PropByref, false>();
		return;
	}
	if (itemName == L"CollectionDict_Interface_Nested_PropByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_PropByval, false>();
		return;
	}
	if (itemName == L"CollectionDict_Interface_Nested_PropDefault")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_PropDefault, false>();
		return;
	}
	if (itemName == L"CollectionDict_Interface_PropByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Interface_PropByref, false>();
		return;
	}
	if (itemName == L"CollectionDict_Interface_PropByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Interface_PropByval, false>();
		return;
	}
	if (itemName == L"CollectionDict_Interface_PropDefault")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Interface_PropDefault, false>();
		return;
	}
	if (itemName == L"CollectionDict_Nested_Default")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Nested_Default, false>();
		return;
	}
	if (itemName == L"CollectionDict_Nested_InByref_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Nested_InByref_OutByref, false>();
		return;
	}
	if (itemName == L"CollectionDict_Nested_InByref_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Nested_InByref_OutByval, false>();
		return;
	}
	if (itemName == L"CollectionDict_Nested_InByval_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Nested_InByval_OutByref, false>();
		return;
	}
	if (itemName == L"CollectionDict_Nested_InByval_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Nested_InByval_OutByval, false>();
		return;
	}
	if (itemName == L"CollectionDict_Nested_PropByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Nested_PropByref, false>();
		return;
	}
	if (itemName == L"CollectionDict_Nested_PropByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Nested_PropByval, false>();
		return;
	}
	if (itemName == L"CollectionDict_Nested_PropDefault")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_Nested_PropDefault, false>();
		return;
	}
	if (itemName == L"CollectionDict_PropByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_PropByref, false>();
		return;
	}
	if (itemName == L"CollectionDict_PropByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_PropByval, false>();
		return;
	}
	if (itemName == L"CollectionDict_PropDefault")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionDict_PropDefault, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Default")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Default, false>();
		return;
	}
	if (itemName == L"CollectionOblist_InByref_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_InByref_OutByref, false>();
		return;
	}
	if (itemName == L"CollectionOblist_InByref_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_InByref_OutByval, false>();
		return;
	}
	if (itemName == L"CollectionOblist_InByval_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_InByval_OutByref, false>();
		return;
	}
	if (itemName == L"CollectionOblist_InByval_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_InByval_OutByval, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Interface_Default")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Interface_Default, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Interface_InByref_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Interface_InByref_OutByref, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Interface_InByref_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Interface_InByref_OutByval, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Interface_InByval_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Interface_InByval_OutByref, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Interface_InByval_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Interface_InByval_OutByval, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Interface_Nested_Default")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_Default, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Interface_Nested_InByref_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByref_OutByref, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Interface_Nested_InByref_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByref_OutByval, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Interface_Nested_InByval_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByval_OutByref, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Interface_Nested_InByval_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByval_OutByval, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Interface_Nested_PropByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_PropByref, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Interface_Nested_PropByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_PropByval, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Interface_Nested_PropDefault")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_PropDefault, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Interface_PropByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Interface_PropByref, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Interface_PropByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Interface_PropByval, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Interface_PropDefault")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Interface_PropDefault, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Nested_Default")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Nested_Default, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Nested_InByref_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Nested_InByref_OutByref, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Nested_InByref_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Nested_InByref_OutByval, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Nested_InByval_OutByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Nested_InByval_OutByref, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Nested_InByval_OutByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Nested_InByval_OutByval, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Nested_PropByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Nested_PropByref, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Nested_PropByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Nested_PropByval, false>();
		return;
	}
	if (itemName == L"CollectionOblist_Nested_PropDefault")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_Nested_PropDefault, false>();
		return;
	}
	if (itemName == L"CollectionOblist_PropByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_PropByref, false>();
		return;
	}
	if (itemName == L"CollectionOblist_PropByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_PropByval, false>();
		return;
	}
	if (itemName == L"CollectionOblist_PropDefault")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_CollectionOblist_PropDefault, false>();
		return;
	}
	if (itemName == L"Dtor")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Dtor, false>();
		return;
	}
	if (itemName == L"Dtor2")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Dtor2, false>();
		return;
	}
	if (itemName == L"Dtor3")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Dtor3, false>();
		return;
	}
	if (itemName == L"DtorList")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_DtorList, false>();
		return;
	}
	if (itemName == L"DtorList2")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_DtorList2, false>();
		return;
	}
	if (itemName == L"DtorPropCached")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_DtorPropCached, true>();
		return;
	}
	if (itemName == L"DtorPropCachedListByval")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_DtorPropCachedListByval, true>();
		return;
	}
	if (itemName == L"DtorPropCachedListVByref")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_DtorPropCachedListVByref, true>();
		return;
	}
	if (itemName == L"Event")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Event, true>();
		return;
	}
	if (itemName == L"EventArgs")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_EventArgs, true>();
		return;
	}
	if (itemName == L"EventOblist")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_EventOblist, false>();
		return;
	}
	if (itemName == L"ListOps_DictionaryException")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_ListOps_DictionaryException, false>();
		return;
	}
	if (itemName == L"ListOps_ListException")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_ListOps_ListException, false>();
		return;
	}
	if (itemName == L"ListOps_OblistEventException")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_ListOps_OblistEventException, false>();
		return;
	}
	if (itemName == L"FailDoubleRegistration")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_FailDoubleRegistration, false>();
		return;
	}
	if (itemName == L"Inheritance")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Inheritance, false>();
		return;
	}
	if (itemName == L"Inheritance_MethodException")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Inheritance_MethodException, false>();
		return;
	}
	if (itemName == L"Inheritance_EventException")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Inheritance_EventException, true>();
		return;
	}
	if (itemName == L"LocalAndWrapper")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_LocalAndWrapper, false>();
		return;
	}
	if (itemName == L"Nullable")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Nullable, false>();
		return;
	}
	if (itemName == L"Overloading")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_Overloading, false>();
		return;
	}
	if (itemName == L"PrimitiveTypes")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_PrimitiveTypes, false>();
		return;
	}
	if (itemName == L"PropCached")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_PropCached, true>();
		return;
	}
	if (itemName == L"PropDefault")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_PropDefault, true>();
		return;
	}
	if (itemName == L"PropDefaultInterface")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_PropDefaultInterface, true>();
		return;
	}
	if (itemName == L"PropDefaultInterfaceList")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_PropDefaultInterfaceList, true>();
		return;
	}
	if (itemName == L"PropDefaultList")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_PropDefaultList, true>();
		return;
	}
	if (itemName == L"PropDynamic")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_PropDynamic, true>();
		return;
	}
	if (itemName == L"RequestService")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_RequestService, false>();
		return;
	}
	if (itemName == L"ServiceWrapper")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_ServiceWrapper, false>();
		return;
	}
	CHECK_FAIL(L"Unknown RPC stdio test case.");
}
