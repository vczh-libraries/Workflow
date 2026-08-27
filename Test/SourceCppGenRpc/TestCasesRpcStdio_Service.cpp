#include "RpcCollection_Default.h"
#include "RpcCollection_InByref_OutByref.h"
#include "RpcCollection_InByref_OutByval.h"
#include "RpcCollection_InByval_OutByref.h"
#include "RpcCollection_InByval_OutByval.h"
#include "RpcCollection_Interface_Default.h"
#include "RpcCollection_Interface_InByref_OutByref.h"
#include "RpcCollection_Interface_InByref_OutByval.h"
#include "RpcCollection_Interface_InByval_OutByref.h"
#include "RpcCollection_Interface_InByval_OutByval.h"
#include "RpcCollection_Interface_Nested_Default.h"
#include "RpcCollection_Interface_Nested_InByref_OutByref.h"
#include "RpcCollection_Interface_Nested_InByref_OutByval.h"
#include "RpcCollection_Interface_Nested_InByval_OutByref.h"
#include "RpcCollection_Interface_Nested_InByval_OutByval.h"
#include "RpcCollection_Interface_Nested_PropByref.h"
#include "RpcCollection_Interface_Nested_PropByval.h"
#include "RpcCollection_Interface_Nested_PropDefault.h"
#include "RpcCollection_Interface_PropByref.h"
#include "RpcCollection_Interface_PropByval.h"
#include "RpcCollection_Interface_PropDefault.h"
#include "RpcCollection_Nested_Default.h"
#include "RpcCollection_Nested_InByref_OutByref.h"
#include "RpcCollection_Nested_InByref_OutByval.h"
#include "RpcCollection_Nested_InByval_OutByref.h"
#include "RpcCollection_Nested_InByval_OutByval.h"
#include "RpcCollection_Nested_PropByref.h"
#include "RpcCollection_Nested_PropByval.h"
#include "RpcCollection_Nested_PropDefault.h"
#include "RpcCollection_PropByref.h"
#include "RpcCollection_PropByval.h"
#include "RpcCollection_PropDefault.h"
#include "RpcCollectionDict_Default.h"
#include "RpcCollectionDict_InByref_OutByref.h"
#include "RpcCollectionDict_InByref_OutByval.h"
#include "RpcCollectionDict_InByval_OutByref.h"
#include "RpcCollectionDict_InByval_OutByval.h"
#include "RpcCollectionDict_Interface_Default.h"
#include "RpcCollectionDict_Interface_InByref_OutByref.h"
#include "RpcCollectionDict_Interface_InByref_OutByval.h"
#include "RpcCollectionDict_Interface_InByval_OutByref.h"
#include "RpcCollectionDict_Interface_InByval_OutByval.h"
#include "RpcCollectionDict_Interface_Nested_Default.h"
#include "RpcCollectionDict_Interface_Nested_InByref_OutByref.h"
#include "RpcCollectionDict_Interface_Nested_InByref_OutByval.h"
#include "RpcCollectionDict_Interface_Nested_InByval_OutByref.h"
#include "RpcCollectionDict_Interface_Nested_InByval_OutByval.h"
#include "RpcCollectionDict_Interface_Nested_PropByref.h"
#include "RpcCollectionDict_Interface_Nested_PropByval.h"
#include "RpcCollectionDict_Interface_Nested_PropDefault.h"
#include "RpcCollectionDict_Interface_PropByref.h"
#include "RpcCollectionDict_Interface_PropByval.h"
#include "RpcCollectionDict_Interface_PropDefault.h"
#include "RpcCollectionDict_Nested_Default.h"
#include "RpcCollectionDict_Nested_InByref_OutByref.h"
#include "RpcCollectionDict_Nested_InByref_OutByval.h"
#include "RpcCollectionDict_Nested_InByval_OutByref.h"
#include "RpcCollectionDict_Nested_InByval_OutByval.h"
#include "RpcCollectionDict_Nested_PropByref.h"
#include "RpcCollectionDict_Nested_PropByval.h"
#include "RpcCollectionDict_Nested_PropDefault.h"
#include "RpcCollectionDict_PropByref.h"
#include "RpcCollectionDict_PropByval.h"
#include "RpcCollectionDict_PropDefault.h"
#include "RpcCollectionOblist_Default.h"
#include "RpcCollectionOblist_InByref_OutByref.h"
#include "RpcCollectionOblist_InByref_OutByval.h"
#include "RpcCollectionOblist_InByval_OutByref.h"
#include "RpcCollectionOblist_InByval_OutByval.h"
#include "RpcCollectionOblist_Interface_Default.h"
#include "RpcCollectionOblist_Interface_InByref_OutByref.h"
#include "RpcCollectionOblist_Interface_InByref_OutByval.h"
#include "RpcCollectionOblist_Interface_InByval_OutByref.h"
#include "RpcCollectionOblist_Interface_InByval_OutByval.h"
#include "RpcCollectionOblist_Interface_Nested_Default.h"
#include "RpcCollectionOblist_Interface_Nested_InByref_OutByref.h"
#include "RpcCollectionOblist_Interface_Nested_InByref_OutByval.h"
#include "RpcCollectionOblist_Interface_Nested_InByval_OutByref.h"
#include "RpcCollectionOblist_Interface_Nested_InByval_OutByval.h"
#include "RpcCollectionOblist_Interface_Nested_PropByref.h"
#include "RpcCollectionOblist_Interface_Nested_PropByval.h"
#include "RpcCollectionOblist_Interface_Nested_PropDefault.h"
#include "RpcCollectionOblist_Interface_PropByref.h"
#include "RpcCollectionOblist_Interface_PropByval.h"
#include "RpcCollectionOblist_Interface_PropDefault.h"
#include "RpcCollectionOblist_Nested_Default.h"
#include "RpcCollectionOblist_Nested_InByref_OutByref.h"
#include "RpcCollectionOblist_Nested_InByref_OutByval.h"
#include "RpcCollectionOblist_Nested_InByval_OutByref.h"
#include "RpcCollectionOblist_Nested_InByval_OutByval.h"
#include "RpcCollectionOblist_Nested_PropByref.h"
#include "RpcCollectionOblist_Nested_PropByval.h"
#include "RpcCollectionOblist_Nested_PropDefault.h"
#include "RpcCollectionOblist_PropByref.h"
#include "RpcCollectionOblist_PropByval.h"
#include "RpcCollectionOblist_PropDefault.h"
#include "RpcDtor.h"
#include "RpcDtor2.h"
#include "RpcDtor3.h"
#include "RpcDtorList.h"
#include "RpcDtorList2.h"
#include "RpcDtorPropCached.h"
#include "RpcDtorPropCachedListByval.h"
#include "RpcDtorPropCachedListVByref.h"
#include "RpcEvent.h"
#include "RpcEventArgs.h"
#include "RpcEventOblist.h"
#include "RpcListOps_DictionaryException.h"
#include "RpcListOps_ListException.h"
#include "RpcListOps_OblistEventException.h"
#include "RpcFailDoubleRegistration.h"
#include "RpcFailDoubleRegistration_SharedMemsp.h"
#include "RpcInheritance.h"
#include "RpcInheritance_MethodException.h"
#include "RpcInheritance_EventException.h"
#include "RpcLocalAndWrapper.h"
#include "RpcLocalAndWrapper_SharedMemsp.h"
#include "RpcNullable.h"
#include "RpcOverloading.h"
#include "RpcPrimitiveTypes.h"
#include "RpcPropCached.h"
#include "RpcPropDefault.h"
#include "RpcPropDefaultInterface.h"
#include "RpcPropDefaultInterfaceList.h"
#include "RpcPropDefaultList.h"
#include "RpcPropDynamic.h"
#include "RpcRequestService.h"
#include "RpcServiceWrapper.h"
#include "RpcServiceWrapper_SharedMemsp.h"
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
	if (itemName == L"FailDoubleRegistration_SharedMemsp")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_FailDoubleRegistration_SharedMemsp, false>();
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
	if (itemName == L"LocalAndWrapper_SharedMemsp")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_LocalAndWrapper_SharedMemsp, false>();
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
	if (itemName == L"ServiceWrapper_SharedMemsp")
	{
		RunRpcStdioService<::vl_workflow_global::Rpc_ServiceWrapper_SharedMemsp, false>();
		return;
	}
	CHECK_FAIL(L"Unknown RPC stdio test case.");
}
