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
#include "../Source/TestCasesRpcStdio_Driver.h"

using namespace vl;
using namespace vl::collections;
using namespace vl::rpc_controller_test;

void RunTestCasesRpcStdio_Driver(const WString& serviceCommand, const SortedList<WString>& skippedTestCases)
{
	if (skippedTestCases.Contains(L"Collection_Default"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Default");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Default::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Default, false>(L"Collection_Default", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_InByref_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_InByref_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::InByref::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_InByref_OutByref, false>(L"Collection_InByref_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_InByref_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_InByref_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::InByref::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_InByref_OutByval, false>(L"Collection_InByref_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_InByval_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_InByval_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::InByval::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_InByval_OutByref, false>(L"Collection_InByval_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_InByval_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_InByval_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::InByval::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_InByval_OutByval, false>(L"Collection_InByval_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Interface_Default"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Interface_Default");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Interface::Default::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Interface_Default, false>(L"Collection_Interface_Default", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Interface_InByref_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Interface_InByref_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Interface::InByref::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Interface_InByref_OutByref, false>(L"Collection_Interface_InByref_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Interface_InByref_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Interface_InByref_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Interface::InByref::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Interface_InByref_OutByval, false>(L"Collection_Interface_InByref_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Interface_InByval_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Interface_InByval_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Interface::InByval::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Interface_InByval_OutByref, false>(L"Collection_Interface_InByval_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Interface_InByval_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Interface_InByval_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Interface::InByval::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Interface_InByval_OutByval, false>(L"Collection_Interface_InByval_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Interface_Nested_Default"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Interface_Nested_Default");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Interface::Nested::Default::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_Default, false>(L"Collection_Interface_Nested_Default", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Interface_Nested_InByref_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Interface_Nested_InByref_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Interface::Nested::InByref::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_InByref_OutByref, false>(L"Collection_Interface_Nested_InByref_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Interface_Nested_InByref_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Interface_Nested_InByref_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Interface::Nested::InByref::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_InByref_OutByval, false>(L"Collection_Interface_Nested_InByref_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Interface_Nested_InByval_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Interface_Nested_InByval_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Interface::Nested::InByval::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_InByval_OutByref, false>(L"Collection_Interface_Nested_InByval_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Interface_Nested_InByval_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Interface_Nested_InByval_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Interface::Nested::InByval::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_InByval_OutByval, false>(L"Collection_Interface_Nested_InByval_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Interface_Nested_PropByref"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Interface_Nested_PropByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Interface::Nested::InByref::PropByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_PropByref, false>(L"Collection_Interface_Nested_PropByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Interface_Nested_PropByval"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Interface_Nested_PropByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Interface::Nested::InByval::PropByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_PropByval, false>(L"Collection_Interface_Nested_PropByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Interface_Nested_PropDefault"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Interface_Nested_PropDefault");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Interface::Nested::PropDefault::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Interface_Nested_PropDefault, false>(L"Collection_Interface_Nested_PropDefault", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Interface_PropByref"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Interface_PropByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Interface::InByref::PropByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Interface_PropByref, false>(L"Collection_Interface_PropByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Interface_PropByval"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Interface_PropByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Interface::InByval::PropByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Interface_PropByval, false>(L"Collection_Interface_PropByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Interface_PropDefault"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Interface_PropDefault");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Interface::PropDefault::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Interface_PropDefault, false>(L"Collection_Interface_PropDefault", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Nested_Default"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Nested_Default");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Nested::Default::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Nested_Default, false>(L"Collection_Nested_Default", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Nested_InByref_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Nested_InByref_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Nested::InByref::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Nested_InByref_OutByref, false>(L"Collection_Nested_InByref_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Nested_InByref_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Nested_InByref_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Nested::InByref::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Nested_InByref_OutByval, false>(L"Collection_Nested_InByref_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Nested_InByval_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Nested_InByval_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Nested::InByval::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Nested_InByval_OutByref, false>(L"Collection_Nested_InByval_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Nested_InByval_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Nested_InByval_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Nested::InByval::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Nested_InByval_OutByval, false>(L"Collection_Nested_InByval_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Nested_PropByref"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Nested_PropByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Nested::InByref::PropByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Nested_PropByref, false>(L"Collection_Nested_PropByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Nested_PropByval"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Nested_PropByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Nested::InByval::PropByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Nested_PropByval, false>(L"Collection_Nested_PropByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_Nested_PropDefault"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_Nested_PropDefault");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::Nested::PropDefault::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_Nested_PropDefault, false>(L"Collection_Nested_PropDefault", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_PropByref"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_PropByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::InByref::PropByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_PropByref, false>(L"Collection_PropByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_PropByval"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_PropByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::InByval::PropByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_PropByval, false>(L"Collection_PropByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Collection_PropDefault"))
	{
		PrintRpcStdioSkippedTestCase(L"Collection_PropDefault");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollection::PropDefault::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Collection_PropDefault, false>(L"Collection_PropDefault", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Default"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Default");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Default::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Default, false>(L"CollectionDict_Default", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_InByref_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_InByref_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::InByref::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_InByref_OutByref, false>(L"CollectionDict_InByref_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_InByref_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_InByref_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::InByref::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_InByref_OutByval, false>(L"CollectionDict_InByref_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_InByval_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_InByval_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::InByval::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_InByval_OutByref, false>(L"CollectionDict_InByval_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_InByval_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_InByval_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::InByval::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_InByval_OutByval, false>(L"CollectionDict_InByval_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Interface_Default"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Interface_Default");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Interface::Default::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Default, false>(L"CollectionDict_Interface_Default", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Interface_InByref_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Interface_InByref_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Interface::InByref::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_InByref_OutByref, false>(L"CollectionDict_Interface_InByref_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Interface_InByref_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Interface_InByref_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Interface::InByref::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_InByref_OutByval, false>(L"CollectionDict_Interface_InByref_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Interface_InByval_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Interface_InByval_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Interface::InByval::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_InByval_OutByref, false>(L"CollectionDict_Interface_InByval_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Interface_InByval_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Interface_InByval_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Interface::InByval::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_InByval_OutByval, false>(L"CollectionDict_Interface_InByval_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Interface_Nested_Default"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Interface_Nested_Default");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Interface::Nested::Default::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_Default, false>(L"CollectionDict_Interface_Nested_Default", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Interface_Nested_InByref_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Interface_Nested_InByref_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Interface::Nested::InByref::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByref_OutByref, false>(L"CollectionDict_Interface_Nested_InByref_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Interface_Nested_InByref_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Interface_Nested_InByref_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Interface::Nested::InByref::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByref_OutByval, false>(L"CollectionDict_Interface_Nested_InByref_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Interface_Nested_InByval_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Interface_Nested_InByval_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Interface::Nested::InByval::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByval_OutByref, false>(L"CollectionDict_Interface_Nested_InByval_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Interface_Nested_InByval_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Interface_Nested_InByval_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Interface::Nested::InByval::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_InByval_OutByval, false>(L"CollectionDict_Interface_Nested_InByval_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Interface_Nested_PropByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Interface_Nested_PropByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Interface::Nested::InByref::PropByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_PropByref, false>(L"CollectionDict_Interface_Nested_PropByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Interface_Nested_PropByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Interface_Nested_PropByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Interface::Nested::InByval::PropByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_PropByval, false>(L"CollectionDict_Interface_Nested_PropByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Interface_Nested_PropDefault"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Interface_Nested_PropDefault");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Interface::Nested::PropDefault::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_Nested_PropDefault, false>(L"CollectionDict_Interface_Nested_PropDefault", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Interface_PropByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Interface_PropByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Interface::InByref::PropByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_PropByref, false>(L"CollectionDict_Interface_PropByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Interface_PropByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Interface_PropByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Interface::InByval::PropByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_PropByval, false>(L"CollectionDict_Interface_PropByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Interface_PropDefault"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Interface_PropDefault");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Interface::PropDefault::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Interface_PropDefault, false>(L"CollectionDict_Interface_PropDefault", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Nested_Default"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Nested_Default");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Nested::Default::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_Default, false>(L"CollectionDict_Nested_Default", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Nested_InByref_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Nested_InByref_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Nested::InByref::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_InByref_OutByref, false>(L"CollectionDict_Nested_InByref_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Nested_InByref_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Nested_InByref_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Nested::InByref::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_InByref_OutByval, false>(L"CollectionDict_Nested_InByref_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Nested_InByval_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Nested_InByval_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Nested::InByval::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_InByval_OutByref, false>(L"CollectionDict_Nested_InByval_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Nested_InByval_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Nested_InByval_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Nested::InByval::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_InByval_OutByval, false>(L"CollectionDict_Nested_InByval_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Nested_PropByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Nested_PropByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Nested::InByref::PropByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_PropByref, false>(L"CollectionDict_Nested_PropByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Nested_PropByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Nested_PropByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Nested::InByval::PropByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_PropByval, false>(L"CollectionDict_Nested_PropByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_Nested_PropDefault"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_Nested_PropDefault");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::Nested::PropDefault::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_Nested_PropDefault, false>(L"CollectionDict_Nested_PropDefault", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_PropByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_PropByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::InByref::PropByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_PropByref, false>(L"CollectionDict_PropByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_PropByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_PropByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::InByval::PropByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_PropByval, false>(L"CollectionDict_PropByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionDict_PropDefault"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionDict_PropDefault");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionDict::PropDefault::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionDict_PropDefault, false>(L"CollectionDict_PropDefault", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Default"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Default");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Default::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Default, false>(L"CollectionOblist_Default", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_InByref_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_InByref_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::InByref::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_InByref_OutByref, false>(L"CollectionOblist_InByref_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_InByref_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_InByref_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::InByref::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_InByref_OutByval, false>(L"CollectionOblist_InByref_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_InByval_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_InByval_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::InByval::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_InByval_OutByref, false>(L"CollectionOblist_InByval_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_InByval_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_InByval_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::InByval::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_InByval_OutByval, false>(L"CollectionOblist_InByval_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Interface_Default"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Interface_Default");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Interface::Default::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Default, false>(L"CollectionOblist_Interface_Default", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Interface_InByref_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Interface_InByref_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Interface::InByref::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_InByref_OutByref, false>(L"CollectionOblist_Interface_InByref_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Interface_InByref_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Interface_InByref_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Interface::InByref::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_InByref_OutByval, false>(L"CollectionOblist_Interface_InByref_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Interface_InByval_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Interface_InByval_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Interface::InByval::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_InByval_OutByref, false>(L"CollectionOblist_Interface_InByval_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Interface_InByval_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Interface_InByval_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Interface::InByval::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_InByval_OutByval, false>(L"CollectionOblist_Interface_InByval_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Interface_Nested_Default"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Interface_Nested_Default");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Interface::Nested::Default::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_Default, false>(L"CollectionOblist_Interface_Nested_Default", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Interface_Nested_InByref_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Interface_Nested_InByref_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Interface::Nested::InByref::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByref_OutByref, false>(L"CollectionOblist_Interface_Nested_InByref_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Interface_Nested_InByref_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Interface_Nested_InByref_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Interface::Nested::InByref::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByref_OutByval, false>(L"CollectionOblist_Interface_Nested_InByref_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Interface_Nested_InByval_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Interface_Nested_InByval_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Interface::Nested::InByval::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByval_OutByref, false>(L"CollectionOblist_Interface_Nested_InByval_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Interface_Nested_InByval_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Interface_Nested_InByval_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Interface::Nested::InByval::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_InByval_OutByval, false>(L"CollectionOblist_Interface_Nested_InByval_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Interface_Nested_PropByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Interface_Nested_PropByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Interface::Nested::InByref::PropByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_PropByref, false>(L"CollectionOblist_Interface_Nested_PropByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Interface_Nested_PropByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Interface_Nested_PropByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Interface::Nested::InByval::PropByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_PropByval, false>(L"CollectionOblist_Interface_Nested_PropByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Interface_Nested_PropDefault"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Interface_Nested_PropDefault");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Interface::Nested::PropDefault::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_Nested_PropDefault, false>(L"CollectionOblist_Interface_Nested_PropDefault", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Interface_PropByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Interface_PropByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Interface::InByref::PropByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_PropByref, false>(L"CollectionOblist_Interface_PropByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Interface_PropByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Interface_PropByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Interface::InByval::PropByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_PropByval, false>(L"CollectionOblist_Interface_PropByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Interface_PropDefault"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Interface_PropDefault");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Interface::PropDefault::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Interface_PropDefault, false>(L"CollectionOblist_Interface_PropDefault", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Nested_Default"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Nested_Default");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Nested::Default::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_Default, false>(L"CollectionOblist_Nested_Default", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Nested_InByref_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Nested_InByref_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Nested::InByref::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_InByref_OutByref, false>(L"CollectionOblist_Nested_InByref_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Nested_InByref_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Nested_InByref_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Nested::InByref::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_InByref_OutByval, false>(L"CollectionOblist_Nested_InByref_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Nested_InByval_OutByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Nested_InByval_OutByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Nested::InByval::OutByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_InByval_OutByref, false>(L"CollectionOblist_Nested_InByval_OutByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Nested_InByval_OutByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Nested_InByval_OutByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Nested::InByval::OutByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_InByval_OutByval, false>(L"CollectionOblist_Nested_InByval_OutByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Nested_PropByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Nested_PropByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Nested::InByref::PropByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_PropByref, false>(L"CollectionOblist_Nested_PropByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Nested_PropByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Nested_PropByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Nested::InByval::PropByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_PropByval, false>(L"CollectionOblist_Nested_PropByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_Nested_PropDefault"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_Nested_PropDefault");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::Nested::PropDefault::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_Nested_PropDefault, false>(L"CollectionOblist_Nested_PropDefault", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_PropByref"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_PropByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::InByref::PropByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_PropByref, false>(L"CollectionOblist_PropByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_PropByval"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_PropByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::InByval::PropByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_PropByval, false>(L"CollectionOblist_PropByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"CollectionOblist_PropDefault"))
	{
		PrintRpcStdioSkippedTestCase(L"CollectionOblist_PropDefault");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcCollectionOblist::PropDefault::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_CollectionOblist_PropDefault, false>(L"CollectionOblist_PropDefault", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Dtor"))
	{
		PrintRpcStdioSkippedTestCase(L"Dtor");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcDtorTest::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Dtor, false>(L"Dtor", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Dtor2"))
	{
		PrintRpcStdioSkippedTestCase(L"Dtor2");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcDtor2Test::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Dtor2, false>(L"Dtor2", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Dtor3"))
	{
		PrintRpcStdioSkippedTestCase(L"Dtor3");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcDtor3Test::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Dtor3, false>(L"Dtor3", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"DtorList"))
	{
		PrintRpcStdioSkippedTestCase(L"DtorList");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcDtorList::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_DtorList, false>(L"DtorList", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"DtorList2"))
	{
		PrintRpcStdioSkippedTestCase(L"DtorList2");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcDtorList2::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_DtorList2, false>(L"DtorList2", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"DtorPropCached"))
	{
		PrintRpcStdioSkippedTestCase(L"DtorPropCached");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcDtorPropCached::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_DtorPropCached, true>(L"DtorPropCached", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"DtorPropCachedListByval"))
	{
		PrintRpcStdioSkippedTestCase(L"DtorPropCachedListByval");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcDtorPropCachedListByval::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_DtorPropCachedListByval, true>(L"DtorPropCachedListByval", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"DtorPropCachedListVByref"))
	{
		PrintRpcStdioSkippedTestCase(L"DtorPropCachedListVByref");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcDtorPropCachedListVByref::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_DtorPropCachedListVByref, true>(L"DtorPropCachedListVByref", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Event"))
	{
		PrintRpcStdioSkippedTestCase(L"Event");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcEvent::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Event, true>(L"Event", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"EventArgs"))
	{
		PrintRpcStdioSkippedTestCase(L"EventArgs");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcEventArgs::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_EventArgs, true>(L"EventArgs", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"EventOblist"))
	{
		PrintRpcStdioSkippedTestCase(L"EventOblist");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcEventOblist::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_EventOblist, false>(L"EventOblist", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"ListOps_DictionaryException"))
	{
		PrintRpcStdioSkippedTestCase(L"ListOps_DictionaryException");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcListOpsDictionaryException::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_ListOps_DictionaryException, false>(L"ListOps_DictionaryException", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"ListOps_ListException"))
	{
		PrintRpcStdioSkippedTestCase(L"ListOps_ListException");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcListOpsListException::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_ListOps_ListException, false>(L"ListOps_ListException", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"ListOps_OblistEventException"))
	{
		PrintRpcStdioSkippedTestCase(L"ListOps_OblistEventException");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcListOpsOblistEventException::IServer");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_ListOps_OblistEventException, false>(L"ListOps_OblistEventException", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"FailDoubleRegistration"))
	{
		PrintRpcStdioSkippedTestCase(L"FailDoubleRegistration");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcFailDoubleRegistrationTest::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_FailDoubleRegistration, false>(L"FailDoubleRegistration", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Inheritance"))
	{
		PrintRpcStdioSkippedTestCase(L"Inheritance");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcInheritance::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Inheritance, false>(L"Inheritance", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Inheritance_MethodException"))
	{
		PrintRpcStdioSkippedTestCase(L"Inheritance_MethodException");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcInheritanceMethodException::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Inheritance_MethodException, false>(L"Inheritance_MethodException", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Inheritance_EventException"))
	{
		PrintRpcStdioSkippedTestCase(L"Inheritance_EventException");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcInheritanceEventException::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Inheritance_EventException, true>(L"Inheritance_EventException", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"LocalAndWrapper"))
	{
		PrintRpcStdioSkippedTestCase(L"LocalAndWrapper");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcWrapperTest::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_LocalAndWrapper, false>(L"LocalAndWrapper", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Nullable"))
	{
		PrintRpcStdioSkippedTestCase(L"Nullable");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcNullable::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Nullable, false>(L"Nullable", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"Overloading"))
	{
		PrintRpcStdioSkippedTestCase(L"Overloading");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcOverloadingTest::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_Overloading, false>(L"Overloading", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"PrimitiveTypes"))
	{
		PrintRpcStdioSkippedTestCase(L"PrimitiveTypes");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcPrimitiveTest::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_PrimitiveTypes, false>(L"PrimitiveTypes", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"PropCached"))
	{
		PrintRpcStdioSkippedTestCase(L"PropCached");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcPropCached::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_PropCached, true>(L"PropCached", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"PropDefault"))
	{
		PrintRpcStdioSkippedTestCase(L"PropDefault");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcPropDefault::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_PropDefault, true>(L"PropDefault", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"PropDefaultInterface"))
	{
		PrintRpcStdioSkippedTestCase(L"PropDefaultInterface");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcPropDefaultInterface::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_PropDefaultInterface, true>(L"PropDefaultInterface", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"PropDefaultInterfaceList"))
	{
		PrintRpcStdioSkippedTestCase(L"PropDefaultInterfaceList");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcPropDefaultInterfaceList::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_PropDefaultInterfaceList, true>(L"PropDefaultInterfaceList", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"PropDefaultList"))
	{
		PrintRpcStdioSkippedTestCase(L"PropDefaultList");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcPropDefaultList::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_PropDefaultList, true>(L"PropDefaultList", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"PropDynamic"))
	{
		PrintRpcStdioSkippedTestCase(L"PropDynamic");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcPropDynamic::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_PropDynamic, true>(L"PropDynamic", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"RequestService"))
	{
		PrintRpcStdioSkippedTestCase(L"RequestService");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcTest::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_RequestService, false>(L"RequestService", serviceCommand, waitingForServices);
	}
	if (skippedTestCases.Contains(L"ServiceWrapper"))
	{
		PrintRpcStdioSkippedTestCase(L"ServiceWrapper");
	}
	else
	{
		List<WString> waitingForServices;
		waitingForServices.Add(L"RpcServiceWrapperTest::IService");
		RunRpcStdioTestCase<::vl_workflow_global::Rpc_ServiceWrapper, false>(L"ServiceWrapper", serviceCommand, waitingForServices);
	}
}
