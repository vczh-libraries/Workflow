.PHONY: all clean pre-build
.DEFAULT_GOAL := all

CPP_COMPILE_OPTIONS=-I ../../../Import -DVCZH_DEBUG_NO_REFLECTION
include $(VCPROOT)/vl/makefile-cpp

pre-build:
	if ! [ -d ./Bin ]; then mkdir ./Bin; fi
	if ! [ -d ./Obj ]; then mkdir ./Obj; fi
	if ! [ -d ./Coverage ]; then mkdir ./Coverage; fi

clean:
	if [ -d ./Bin ]; then rm -r ./Bin; fi
	if [ -d ./Obj ]; then rm -r ./Obj; fi
	if [ -d ./Coverage ]; then rm -r ./Coverage; fi

all:pre-build ./Bin/RpcStdioTest_Driver

./Bin/RpcStdioTest_Driver:./Obj/Vlpp.o ./Obj/Vlpp.Linux.o ./Obj/VlppGlrParser.o ./Obj/VlppOS.o ./Obj/VlppOS.Linux.o ./Obj/VlppReflection.o ./Obj/VlppRegex.o ./Obj/WfLibraryCppHelper.o ./Obj/WfLibraryPredefined.o ./Obj/WfLibraryReflection.o ./Obj/WfLibraryRpc.o ./Obj/WfLibraryRpcController.o ./Obj/WfLibraryRpcLifecycle.o ./Obj/WfLibraryRpcWrappers.o ./Obj/WfLibraryRpcJson.o ./Obj/WfLibraryRpcJsonDispatcherClient.o ./Obj/WfLibraryRpcJsonDispatcherServer.o ./Obj/RpcCollectionDict_Default.o ./Obj/RpcCollectionDict_InByval_OutByref.o ./Obj/RpcCollectionDict_InByval_OutByval.o ./Obj/RpcCollectionDict_InByref_OutByval.o ./Obj/RpcCollectionDict_InByref_OutByref.o ./Obj/RpcCollectionDict_Interface_Default.o ./Obj/RpcCollectionDict_Interface_InByval_OutByref.o ./Obj/RpcCollectionDict_Interface_InByval_OutByval.o ./Obj/RpcCollectionDict_Interface_InByref_OutByval.o ./Obj/RpcCollectionDict_Interface_InByref_OutByref.o ./Obj/RpcCollectionDict_Interface_Nested_Default.o ./Obj/RpcCollectionDict_Interface_Nested_InByval_OutByref.o ./Obj/RpcCollectionDict_Interface_Nested_InByval_OutByval.o ./Obj/RpcCollectionDict_Interface_Nested_InByref_OutByval.o ./Obj/RpcCollectionDict_Interface_Nested_InByref_OutByref.o ./Obj/RpcCollectionDict_Nested_Default.o ./Obj/RpcCollectionDict_Nested_InByval_OutByref.o ./Obj/RpcCollectionDict_Nested_InByval_OutByval.o ./Obj/RpcCollectionDict_Nested_InByref_OutByval.o ./Obj/RpcCollectionDict_Nested_InByref_OutByref.o ./Obj/RpcCollection_Default.o ./Obj/RpcCollection_InByval_OutByref.o ./Obj/RpcCollection_InByval_OutByval.o ./Obj/RpcCollection_InByref_OutByval.o ./Obj/RpcCollection_InByref_OutByref.o ./Obj/RpcCollection_Interface_Default.o ./Obj/RpcCollection_Interface_InByval_OutByref.o ./Obj/RpcCollection_Interface_InByval_OutByval.o ./Obj/RpcCollection_Interface_InByref_OutByval.o ./Obj/RpcCollection_Interface_InByref_OutByref.o ./Obj/RpcCollection_Interface_Nested_Default.o ./Obj/RpcCollection_Interface_Nested_InByval_OutByref.o ./Obj/RpcCollection_Interface_Nested_InByval_OutByval.o ./Obj/RpcCollection_Interface_Nested_InByref_OutByval.o ./Obj/RpcCollection_Interface_Nested_InByref_OutByref.o ./Obj/RpcCollection_Nested_Default.o ./Obj/RpcCollection_Nested_InByval_OutByref.o ./Obj/RpcCollection_Nested_InByval_OutByval.o ./Obj/RpcCollection_Nested_InByref_OutByval.o ./Obj/RpcCollection_Nested_InByref_OutByref.o ./Obj/RpcCollectionDict_PropDefault.o ./Obj/RpcCollectionDict_PropByval.o ./Obj/RpcCollectionDict_PropByref.o ./Obj/RpcCollectionDict_Interface_PropDefault.o ./Obj/RpcCollectionDict_Interface_PropByval.o ./Obj/RpcCollectionDict_Interface_PropByref.o ./Obj/RpcCollectionDict_Interface_Nested_PropDefault.o ./Obj/RpcCollectionDict_Interface_Nested_PropByval.o ./Obj/RpcCollectionDict_Interface_Nested_PropByref.o ./Obj/RpcCollectionDict_Nested_PropDefault.o ./Obj/RpcCollectionDict_Nested_PropByval.o ./Obj/RpcCollectionDict_Nested_PropByref.o ./Obj/RpcCollection_PropDefault.o ./Obj/RpcCollection_PropByval.o ./Obj/RpcCollection_PropByref.o ./Obj/RpcCollection_Interface_PropDefault.o ./Obj/RpcCollection_Interface_PropByval.o ./Obj/RpcCollection_Interface_PropByref.o ./Obj/RpcCollection_Interface_Nested_PropDefault.o ./Obj/RpcCollection_Interface_Nested_PropByval.o ./Obj/RpcCollection_Interface_Nested_PropByref.o ./Obj/RpcCollection_Nested_PropDefault.o ./Obj/RpcCollection_Nested_PropByval.o ./Obj/RpcCollection_Nested_PropByref.o ./Obj/RpcCollectionOblist_Default.o ./Obj/RpcCollectionOblist_InByval_OutByref.o ./Obj/RpcCollectionOblist_InByval_OutByval.o ./Obj/RpcCollectionOblist_InByref_OutByval.o ./Obj/RpcCollectionOblist_InByref_OutByref.o ./Obj/RpcCollectionOblist_Interface_Default.o ./Obj/RpcCollectionOblist_Interface_InByval_OutByref.o ./Obj/RpcCollectionOblist_Interface_InByval_OutByval.o ./Obj/RpcCollectionOblist_Interface_InByref_OutByval.o ./Obj/RpcCollectionOblist_Interface_InByref_OutByref.o ./Obj/RpcCollectionOblist_Interface_Nested_Default.o ./Obj/RpcCollectionOblist_Interface_Nested_InByval_OutByref.o ./Obj/RpcCollectionOblist_Interface_Nested_InByval_OutByval.o ./Obj/RpcCollectionOblist_Interface_Nested_InByref_OutByval.o ./Obj/RpcCollectionOblist_Interface_Nested_InByref_OutByref.o ./Obj/RpcCollectionOblist_Nested_Default.o ./Obj/RpcCollectionOblist_Nested_InByval_OutByref.o ./Obj/RpcCollectionOblist_Nested_InByval_OutByval.o ./Obj/RpcCollectionOblist_Nested_InByref_OutByval.o ./Obj/RpcCollectionOblist_Nested_InByref_OutByref.o ./Obj/RpcCollectionOblist_PropDefault.o ./Obj/RpcCollectionOblist_PropByval.o ./Obj/RpcCollectionOblist_PropByref.o ./Obj/RpcCollectionOblist_Interface_PropDefault.o ./Obj/RpcCollectionOblist_Interface_PropByval.o ./Obj/RpcCollectionOblist_Interface_PropByref.o ./Obj/RpcCollectionOblist_Interface_Nested_PropDefault.o ./Obj/RpcCollectionOblist_Interface_Nested_PropByval.o ./Obj/RpcCollectionOblist_Interface_Nested_PropByref.o ./Obj/RpcCollectionOblist_Nested_PropDefault.o ./Obj/RpcCollectionOblist_Nested_PropByval.o ./Obj/RpcCollectionOblist_Nested_PropByref.o ./Obj/RpcDtor.o ./Obj/RpcDtor2.o ./Obj/RpcDtor3.o ./Obj/RpcDtorList.o ./Obj/RpcDtorList2.o ./Obj/RpcDtorPropCached.o ./Obj/RpcDtorPropCachedListByval.o ./Obj/RpcDtorPropCachedListVByref.o ./Obj/RpcFailDoubleRegistration.o ./Obj/RpcInheritance.o ./Obj/RpcInheritance_MethodException.o ./Obj/RpcInheritance_EventException.o ./Obj/RpcLocalAndWrapper.o ./Obj/RpcNullable.o ./Obj/RpcOverloading.o ./Obj/RpcEvent.o ./Obj/RpcEventArgs.o ./Obj/RpcEventOblist.o ./Obj/RpcListOps_DictionaryException.o ./Obj/RpcListOps_ListException.o ./Obj/RpcListOps_OblistEventException.o ./Obj/RpcPrimitiveTypes.o ./Obj/RpcPropCached.o ./Obj/RpcPropDefault.o ./Obj/RpcPropDefaultInterface.o ./Obj/RpcPropDefaultInterfaceList.o ./Obj/RpcPropDefaultList.o ./Obj/RpcPropDynamic.o ./Obj/RpcRequestService.o ./Obj/RpcServiceWrapper.o ./Obj/RpcCollectionDict_DefaultReflection.o ./Obj/RpcCollectionDict_InByval_OutByrefReflection.o ./Obj/RpcCollectionDict_InByval_OutByvalReflection.o ./Obj/RpcCollectionDict_InByref_OutByvalReflection.o ./Obj/RpcCollectionDict_InByref_OutByrefReflection.o ./Obj/RpcCollectionDict_Interface_DefaultReflection.o ./Obj/RpcCollectionDict_Interface_InByval_OutByrefReflection.o ./Obj/RpcCollectionDict_Interface_InByval_OutByvalReflection.o ./Obj/RpcCollectionDict_Interface_InByref_OutByvalReflection.o ./Obj/RpcCollectionDict_Interface_InByref_OutByrefReflection.o ./Obj/RpcCollectionDict_Interface_Nested_DefaultReflection.o ./Obj/RpcCollectionDict_Interface_Nested_InByval_OutByrefReflection.o ./Obj/RpcCollectionDict_Interface_Nested_InByval_OutByvalReflection.o ./Obj/RpcCollectionDict_Interface_Nested_InByref_OutByvalReflection.o ./Obj/RpcCollectionDict_Interface_Nested_InByref_OutByrefReflection.o ./Obj/RpcCollectionDict_Nested_DefaultReflection.o ./Obj/RpcCollectionDict_Nested_InByval_OutByrefReflection.o ./Obj/RpcCollectionDict_Nested_InByval_OutByvalReflection.o ./Obj/RpcCollectionDict_Nested_InByref_OutByvalReflection.o ./Obj/RpcCollectionDict_Nested_InByref_OutByrefReflection.o ./Obj/RpcCollection_DefaultReflection.o ./Obj/RpcCollection_InByval_OutByrefReflection.o ./Obj/RpcCollection_InByval_OutByvalReflection.o ./Obj/RpcCollection_InByref_OutByvalReflection.o ./Obj/RpcCollection_InByref_OutByrefReflection.o ./Obj/RpcCollection_Interface_DefaultReflection.o ./Obj/RpcCollection_Interface_InByval_OutByrefReflection.o ./Obj/RpcCollection_Interface_InByval_OutByvalReflection.o ./Obj/RpcCollection_Interface_InByref_OutByvalReflection.o ./Obj/RpcCollection_Interface_InByref_OutByrefReflection.o ./Obj/RpcCollection_Interface_Nested_DefaultReflection.o ./Obj/RpcCollection_Interface_Nested_InByval_OutByrefReflection.o ./Obj/RpcCollection_Interface_Nested_InByval_OutByvalReflection.o ./Obj/RpcCollection_Interface_Nested_InByref_OutByvalReflection.o ./Obj/RpcCollection_Interface_Nested_InByref_OutByrefReflection.o ./Obj/RpcCollection_Nested_DefaultReflection.o ./Obj/RpcCollection_Nested_InByval_OutByrefReflection.o ./Obj/RpcCollection_Nested_InByval_OutByvalReflection.o ./Obj/RpcCollection_Nested_InByref_OutByvalReflection.o ./Obj/RpcCollection_Nested_InByref_OutByrefReflection.o ./Obj/RpcCollectionDict_PropDefaultReflection.o ./Obj/RpcCollectionDict_PropByvalReflection.o ./Obj/RpcCollectionDict_PropByrefReflection.o ./Obj/RpcCollectionDict_Interface_PropDefaultReflection.o ./Obj/RpcCollectionDict_Interface_PropByvalReflection.o ./Obj/RpcCollectionDict_Interface_PropByrefReflection.o ./Obj/RpcCollectionDict_Interface_Nested_PropDefaultReflection.o ./Obj/RpcCollectionDict_Interface_Nested_PropByvalReflection.o ./Obj/RpcCollectionDict_Interface_Nested_PropByrefReflection.o ./Obj/RpcCollectionDict_Nested_PropDefaultReflection.o ./Obj/RpcCollectionDict_Nested_PropByvalReflection.o ./Obj/RpcCollectionDict_Nested_PropByrefReflection.o ./Obj/RpcCollection_PropDefaultReflection.o ./Obj/RpcCollection_PropByvalReflection.o ./Obj/RpcCollection_PropByrefReflection.o ./Obj/RpcCollection_Interface_PropDefaultReflection.o ./Obj/RpcCollection_Interface_PropByvalReflection.o ./Obj/RpcCollection_Interface_PropByrefReflection.o ./Obj/RpcCollection_Interface_Nested_PropDefaultReflection.o ./Obj/RpcCollection_Interface_Nested_PropByvalReflection.o ./Obj/RpcCollection_Interface_Nested_PropByrefReflection.o ./Obj/RpcCollection_Nested_PropDefaultReflection.o ./Obj/RpcCollection_Nested_PropByvalReflection.o ./Obj/RpcCollection_Nested_PropByrefReflection.o ./Obj/RpcCollectionOblist_DefaultReflection.o ./Obj/RpcCollectionOblist_InByval_OutByrefReflection.o ./Obj/RpcCollectionOblist_InByval_OutByvalReflection.o ./Obj/RpcCollectionOblist_InByref_OutByvalReflection.o ./Obj/RpcCollectionOblist_InByref_OutByrefReflection.o ./Obj/RpcCollectionOblist_Interface_DefaultReflection.o ./Obj/RpcCollectionOblist_Interface_InByval_OutByrefReflection.o ./Obj/RpcCollectionOblist_Interface_InByval_OutByvalReflection.o ./Obj/RpcCollectionOblist_Interface_InByref_OutByvalReflection.o ./Obj/RpcCollectionOblist_Interface_InByref_OutByrefReflection.o ./Obj/RpcCollectionOblist_Interface_Nested_DefaultReflection.o ./Obj/RpcCollectionOblist_Interface_Nested_InByval_OutByrefReflection.o ./Obj/RpcCollectionOblist_Interface_Nested_InByval_OutByvalReflection.o ./Obj/RpcCollectionOblist_Interface_Nested_InByref_OutByvalReflection.o ./Obj/RpcCollectionOblist_Interface_Nested_InByref_OutByrefReflection.o ./Obj/RpcCollectionOblist_Nested_DefaultReflection.o ./Obj/RpcCollectionOblist_Nested_InByval_OutByrefReflection.o ./Obj/RpcCollectionOblist_Nested_InByval_OutByvalReflection.o ./Obj/RpcCollectionOblist_Nested_InByref_OutByvalReflection.o ./Obj/RpcCollectionOblist_Nested_InByref_OutByrefReflection.o ./Obj/RpcCollectionOblist_PropDefaultReflection.o ./Obj/RpcCollectionOblist_PropByvalReflection.o ./Obj/RpcCollectionOblist_PropByrefReflection.o ./Obj/RpcCollectionOblist_Interface_PropDefaultReflection.o ./Obj/RpcCollectionOblist_Interface_PropByvalReflection.o ./Obj/RpcCollectionOblist_Interface_PropByrefReflection.o ./Obj/RpcCollectionOblist_Interface_Nested_PropDefaultReflection.o ./Obj/RpcCollectionOblist_Interface_Nested_PropByvalReflection.o ./Obj/RpcCollectionOblist_Interface_Nested_PropByrefReflection.o ./Obj/RpcCollectionOblist_Nested_PropDefaultReflection.o ./Obj/RpcCollectionOblist_Nested_PropByvalReflection.o ./Obj/RpcCollectionOblist_Nested_PropByrefReflection.o ./Obj/RpcDtorReflection.o ./Obj/RpcDtor2Reflection.o ./Obj/RpcDtor3Reflection.o ./Obj/RpcDtorListReflection.o ./Obj/RpcDtorList2Reflection.o ./Obj/RpcDtorPropCachedReflection.o ./Obj/RpcDtorPropCachedListByvalReflection.o ./Obj/RpcDtorPropCachedListVByrefReflection.o ./Obj/RpcFailDoubleRegistrationReflection.o ./Obj/RpcInheritanceReflection.o ./Obj/RpcInheritance_MethodExceptionReflection.o ./Obj/RpcInheritance_EventExceptionReflection.o ./Obj/RpcLocalAndWrapperReflection.o ./Obj/RpcNullableReflection.o ./Obj/RpcOverloadingReflection.o ./Obj/RpcEventReflection.o ./Obj/RpcEventArgsReflection.o ./Obj/RpcEventOblistReflection.o ./Obj/RpcListOps_DictionaryExceptionReflection.o ./Obj/RpcListOps_ListExceptionReflection.o ./Obj/RpcListOps_OblistEventExceptionReflection.o ./Obj/RpcPrimitiveTypesReflection.o ./Obj/RpcPropCachedReflection.o ./Obj/RpcPropDefaultReflection.o ./Obj/RpcPropDefaultInterfaceReflection.o ./Obj/RpcPropDefaultInterfaceListReflection.o ./Obj/RpcPropDefaultListReflection.o ./Obj/RpcPropDynamicReflection.o ./Obj/RpcRequestServiceReflection.o ./Obj/RpcServiceWrapperReflection.o ./Obj/CppTypes.o ./Obj/TestCasesRpcStdio_Driver.o ./Obj/Main.o
	$(CPP_LINK)

./Obj/Vlpp.o: ../../../Import/Vlpp.cpp
	$(CPP_COMPILE)

./Obj/Vlpp.Linux.o: ../../../Import/Vlpp.Linux.cpp
	$(CPP_COMPILE)

./Obj/VlppGlrParser.o: ../../../Import/VlppGlrParser.cpp
	$(CPP_COMPILE)

./Obj/VlppOS.o: ../../../Import/VlppOS.cpp
	$(CPP_COMPILE)

./Obj/VlppOS.Linux.o: ../../../Import/VlppOS.Linux.cpp
	$(CPP_COMPILE)

./Obj/VlppReflection.o: ../../../Import/VlppReflection.cpp
	$(CPP_COMPILE)

./Obj/VlppRegex.o: ../../../Import/VlppRegex.cpp
	$(CPP_COMPILE)

./Obj/WfLibraryCppHelper.o: ../../../Source/Library/WfLibraryCppHelper.cpp
	$(CPP_COMPILE)

./Obj/WfLibraryPredefined.o: ../../../Source/Library/WfLibraryPredefined.cpp
	$(CPP_COMPILE)

./Obj/WfLibraryReflection.o: ../../../Source/Library/WfLibraryReflection.cpp
	$(CPP_COMPILE)

./Obj/WfLibraryRpc.o: ../../../Source/Library/Rpc/WfLibraryRpc.cpp
	$(CPP_COMPILE)

./Obj/WfLibraryRpcController.o: ../../../Source/Library/Rpc/WfLibraryRpcController.cpp
	$(CPP_COMPILE)

./Obj/WfLibraryRpcLifecycle.o: ../../../Source/Library/Rpc/WfLibraryRpcLifecycle.cpp
	$(CPP_COMPILE)

./Obj/WfLibraryRpcWrappers.o: ../../../Source/Library/Rpc/WfLibraryRpcWrappers.cpp
	$(CPP_COMPILE)

./Obj/WfLibraryRpcJson.o: ../../../Source/Library/RpcJson/WfLibraryRpcJson.cpp
	$(CPP_COMPILE)

./Obj/WfLibraryRpcJsonDispatcherClient.o: ../../../Source/Library/RpcJson/WfLibraryRpcJsonDispatcherClient.cpp
	$(CPP_COMPILE)

./Obj/WfLibraryRpcJsonDispatcherServer.o: ../../../Source/Library/RpcJson/WfLibraryRpcJsonDispatcherServer.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Default.o: ../../SourceCppGenRpc/RpcCollectionDict_Default.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_InByval_OutByref.o: ../../SourceCppGenRpc/RpcCollectionDict_InByval_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_InByval_OutByval.o: ../../SourceCppGenRpc/RpcCollectionDict_InByval_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_InByref_OutByval.o: ../../SourceCppGenRpc/RpcCollectionDict_InByref_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_InByref_OutByref.o: ../../SourceCppGenRpc/RpcCollectionDict_InByref_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_Default.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_Default.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_InByval_OutByref.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_InByval_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_InByval_OutByval.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_InByval_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_InByref_OutByval.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_InByref_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_InByref_OutByref.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_InByref_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_Nested_Default.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_Nested_Default.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_Nested_InByval_OutByref.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_Nested_InByval_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_Nested_InByval_OutByval.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_Nested_InByval_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_Nested_InByref_OutByval.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_Nested_InByref_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_Nested_InByref_OutByref.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_Nested_InByref_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Nested_Default.o: ../../SourceCppGenRpc/RpcCollectionDict_Nested_Default.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Nested_InByval_OutByref.o: ../../SourceCppGenRpc/RpcCollectionDict_Nested_InByval_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Nested_InByval_OutByval.o: ../../SourceCppGenRpc/RpcCollectionDict_Nested_InByval_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Nested_InByref_OutByval.o: ../../SourceCppGenRpc/RpcCollectionDict_Nested_InByref_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Nested_InByref_OutByref.o: ../../SourceCppGenRpc/RpcCollectionDict_Nested_InByref_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Default.o: ../../SourceCppGenRpc/RpcCollection_Default.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_InByval_OutByref.o: ../../SourceCppGenRpc/RpcCollection_InByval_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_InByval_OutByval.o: ../../SourceCppGenRpc/RpcCollection_InByval_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_InByref_OutByval.o: ../../SourceCppGenRpc/RpcCollection_InByref_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_InByref_OutByref.o: ../../SourceCppGenRpc/RpcCollection_InByref_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_Default.o: ../../SourceCppGenRpc/RpcCollection_Interface_Default.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_InByval_OutByref.o: ../../SourceCppGenRpc/RpcCollection_Interface_InByval_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_InByval_OutByval.o: ../../SourceCppGenRpc/RpcCollection_Interface_InByval_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_InByref_OutByval.o: ../../SourceCppGenRpc/RpcCollection_Interface_InByref_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_InByref_OutByref.o: ../../SourceCppGenRpc/RpcCollection_Interface_InByref_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_Nested_Default.o: ../../SourceCppGenRpc/RpcCollection_Interface_Nested_Default.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_Nested_InByval_OutByref.o: ../../SourceCppGenRpc/RpcCollection_Interface_Nested_InByval_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_Nested_InByval_OutByval.o: ../../SourceCppGenRpc/RpcCollection_Interface_Nested_InByval_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_Nested_InByref_OutByval.o: ../../SourceCppGenRpc/RpcCollection_Interface_Nested_InByref_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_Nested_InByref_OutByref.o: ../../SourceCppGenRpc/RpcCollection_Interface_Nested_InByref_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Nested_Default.o: ../../SourceCppGenRpc/RpcCollection_Nested_Default.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Nested_InByval_OutByref.o: ../../SourceCppGenRpc/RpcCollection_Nested_InByval_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Nested_InByval_OutByval.o: ../../SourceCppGenRpc/RpcCollection_Nested_InByval_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Nested_InByref_OutByval.o: ../../SourceCppGenRpc/RpcCollection_Nested_InByref_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Nested_InByref_OutByref.o: ../../SourceCppGenRpc/RpcCollection_Nested_InByref_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_PropDefault.o: ../../SourceCppGenRpc/RpcCollectionDict_PropDefault.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_PropByval.o: ../../SourceCppGenRpc/RpcCollectionDict_PropByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_PropByref.o: ../../SourceCppGenRpc/RpcCollectionDict_PropByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_PropDefault.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_PropDefault.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_PropByval.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_PropByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_PropByref.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_PropByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_Nested_PropDefault.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_Nested_PropDefault.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_Nested_PropByval.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_Nested_PropByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_Nested_PropByref.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_Nested_PropByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Nested_PropDefault.o: ../../SourceCppGenRpc/RpcCollectionDict_Nested_PropDefault.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Nested_PropByval.o: ../../SourceCppGenRpc/RpcCollectionDict_Nested_PropByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Nested_PropByref.o: ../../SourceCppGenRpc/RpcCollectionDict_Nested_PropByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_PropDefault.o: ../../SourceCppGenRpc/RpcCollection_PropDefault.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_PropByval.o: ../../SourceCppGenRpc/RpcCollection_PropByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_PropByref.o: ../../SourceCppGenRpc/RpcCollection_PropByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_PropDefault.o: ../../SourceCppGenRpc/RpcCollection_Interface_PropDefault.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_PropByval.o: ../../SourceCppGenRpc/RpcCollection_Interface_PropByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_PropByref.o: ../../SourceCppGenRpc/RpcCollection_Interface_PropByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_Nested_PropDefault.o: ../../SourceCppGenRpc/RpcCollection_Interface_Nested_PropDefault.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_Nested_PropByval.o: ../../SourceCppGenRpc/RpcCollection_Interface_Nested_PropByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_Nested_PropByref.o: ../../SourceCppGenRpc/RpcCollection_Interface_Nested_PropByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Nested_PropDefault.o: ../../SourceCppGenRpc/RpcCollection_Nested_PropDefault.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Nested_PropByval.o: ../../SourceCppGenRpc/RpcCollection_Nested_PropByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Nested_PropByref.o: ../../SourceCppGenRpc/RpcCollection_Nested_PropByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Default.o: ../../SourceCppGenRpc/RpcCollectionOblist_Default.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_InByval_OutByref.o: ../../SourceCppGenRpc/RpcCollectionOblist_InByval_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_InByval_OutByval.o: ../../SourceCppGenRpc/RpcCollectionOblist_InByval_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_InByref_OutByval.o: ../../SourceCppGenRpc/RpcCollectionOblist_InByref_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_InByref_OutByref.o: ../../SourceCppGenRpc/RpcCollectionOblist_InByref_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_Default.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_Default.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_InByval_OutByref.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_InByval_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_InByval_OutByval.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_InByval_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_InByref_OutByval.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_InByref_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_InByref_OutByref.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_InByref_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_Nested_Default.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_Nested_Default.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_Nested_InByval_OutByref.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_Nested_InByval_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_Nested_InByval_OutByval.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_Nested_InByval_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_Nested_InByref_OutByval.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_Nested_InByref_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_Nested_InByref_OutByref.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_Nested_InByref_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Nested_Default.o: ../../SourceCppGenRpc/RpcCollectionOblist_Nested_Default.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Nested_InByval_OutByref.o: ../../SourceCppGenRpc/RpcCollectionOblist_Nested_InByval_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Nested_InByval_OutByval.o: ../../SourceCppGenRpc/RpcCollectionOblist_Nested_InByval_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Nested_InByref_OutByval.o: ../../SourceCppGenRpc/RpcCollectionOblist_Nested_InByref_OutByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Nested_InByref_OutByref.o: ../../SourceCppGenRpc/RpcCollectionOblist_Nested_InByref_OutByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_PropDefault.o: ../../SourceCppGenRpc/RpcCollectionOblist_PropDefault.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_PropByval.o: ../../SourceCppGenRpc/RpcCollectionOblist_PropByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_PropByref.o: ../../SourceCppGenRpc/RpcCollectionOblist_PropByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_PropDefault.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_PropDefault.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_PropByval.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_PropByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_PropByref.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_PropByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_Nested_PropDefault.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_Nested_PropDefault.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_Nested_PropByval.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_Nested_PropByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_Nested_PropByref.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_Nested_PropByref.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Nested_PropDefault.o: ../../SourceCppGenRpc/RpcCollectionOblist_Nested_PropDefault.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Nested_PropByval.o: ../../SourceCppGenRpc/RpcCollectionOblist_Nested_PropByval.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Nested_PropByref.o: ../../SourceCppGenRpc/RpcCollectionOblist_Nested_PropByref.cpp
	$(CPP_COMPILE)

./Obj/RpcDtor.o: ../../SourceCppGenRpc/RpcDtor.cpp
	$(CPP_COMPILE)

./Obj/RpcDtor2.o: ../../SourceCppGenRpc/RpcDtor2.cpp
	$(CPP_COMPILE)

./Obj/RpcDtor3.o: ../../SourceCppGenRpc/RpcDtor3.cpp
	$(CPP_COMPILE)

./Obj/RpcDtorList.o: ../../SourceCppGenRpc/RpcDtorList.cpp
	$(CPP_COMPILE)

./Obj/RpcDtorList2.o: ../../SourceCppGenRpc/RpcDtorList2.cpp
	$(CPP_COMPILE)

./Obj/RpcDtorPropCached.o: ../../SourceCppGenRpc/RpcDtorPropCached.cpp
	$(CPP_COMPILE)

./Obj/RpcDtorPropCachedListByval.o: ../../SourceCppGenRpc/RpcDtorPropCachedListByval.cpp
	$(CPP_COMPILE)

./Obj/RpcDtorPropCachedListVByref.o: ../../SourceCppGenRpc/RpcDtorPropCachedListVByref.cpp
	$(CPP_COMPILE)

./Obj/RpcFailDoubleRegistration.o: ../../SourceCppGenRpc/RpcFailDoubleRegistration.cpp
	$(CPP_COMPILE)

./Obj/RpcInheritance.o: ../../SourceCppGenRpc/RpcInheritance.cpp
	$(CPP_COMPILE)

./Obj/RpcInheritance_MethodException.o: ../../SourceCppGenRpc/RpcInheritance_MethodException.cpp
	$(CPP_COMPILE)

./Obj/RpcInheritance_EventException.o: ../../SourceCppGenRpc/RpcInheritance_EventException.cpp
	$(CPP_COMPILE)

./Obj/RpcLocalAndWrapper.o: ../../SourceCppGenRpc/RpcLocalAndWrapper.cpp
	$(CPP_COMPILE)

./Obj/RpcNullable.o: ../../SourceCppGenRpc/RpcNullable.cpp
	$(CPP_COMPILE)

./Obj/RpcOverloading.o: ../../SourceCppGenRpc/RpcOverloading.cpp
	$(CPP_COMPILE)

./Obj/RpcEvent.o: ../../SourceCppGenRpc/RpcEvent.cpp
	$(CPP_COMPILE)

./Obj/RpcEventArgs.o: ../../SourceCppGenRpc/RpcEventArgs.cpp
	$(CPP_COMPILE)

./Obj/RpcEventOblist.o: ../../SourceCppGenRpc/RpcEventOblist.cpp
	$(CPP_COMPILE)

./Obj/RpcListOps_DictionaryException.o: ../../SourceCppGenRpc/RpcListOps_DictionaryException.cpp
	$(CPP_COMPILE)

./Obj/RpcListOps_ListException.o: ../../SourceCppGenRpc/RpcListOps_ListException.cpp
	$(CPP_COMPILE)

./Obj/RpcListOps_OblistEventException.o: ../../SourceCppGenRpc/RpcListOps_OblistEventException.cpp
	$(CPP_COMPILE)

./Obj/RpcPrimitiveTypes.o: ../../SourceCppGenRpc/RpcPrimitiveTypes.cpp
	$(CPP_COMPILE)

./Obj/RpcPropCached.o: ../../SourceCppGenRpc/RpcPropCached.cpp
	$(CPP_COMPILE)

./Obj/RpcPropDefault.o: ../../SourceCppGenRpc/RpcPropDefault.cpp
	$(CPP_COMPILE)

./Obj/RpcPropDefaultInterface.o: ../../SourceCppGenRpc/RpcPropDefaultInterface.cpp
	$(CPP_COMPILE)

./Obj/RpcPropDefaultInterfaceList.o: ../../SourceCppGenRpc/RpcPropDefaultInterfaceList.cpp
	$(CPP_COMPILE)

./Obj/RpcPropDefaultList.o: ../../SourceCppGenRpc/RpcPropDefaultList.cpp
	$(CPP_COMPILE)

./Obj/RpcPropDynamic.o: ../../SourceCppGenRpc/RpcPropDynamic.cpp
	$(CPP_COMPILE)

./Obj/RpcRequestService.o: ../../SourceCppGenRpc/RpcRequestService.cpp
	$(CPP_COMPILE)

./Obj/RpcServiceWrapper.o: ../../SourceCppGenRpc/RpcServiceWrapper.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_DefaultReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_DefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_InByval_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_InByval_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_InByval_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_InByval_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_InByref_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_InByref_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_InByref_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_InByref_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_DefaultReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_DefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_InByval_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_InByval_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_InByval_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_InByval_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_InByref_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_InByref_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_InByref_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_InByref_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_Nested_DefaultReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_Nested_DefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_Nested_InByval_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_Nested_InByval_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_Nested_InByval_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_Nested_InByval_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_Nested_InByref_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_Nested_InByref_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_Nested_InByref_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_Nested_InByref_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Nested_DefaultReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Nested_DefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Nested_InByval_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Nested_InByval_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Nested_InByval_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Nested_InByval_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Nested_InByref_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Nested_InByref_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Nested_InByref_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Nested_InByref_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_DefaultReflection.o: ../../SourceCppGenRpc/RpcCollection_DefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_InByval_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollection_InByval_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_InByval_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollection_InByval_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_InByref_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollection_InByref_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_InByref_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollection_InByref_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_DefaultReflection.o: ../../SourceCppGenRpc/RpcCollection_Interface_DefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_InByval_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollection_Interface_InByval_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_InByval_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollection_Interface_InByval_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_InByref_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollection_Interface_InByref_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_InByref_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollection_Interface_InByref_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_Nested_DefaultReflection.o: ../../SourceCppGenRpc/RpcCollection_Interface_Nested_DefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_Nested_InByval_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollection_Interface_Nested_InByval_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_Nested_InByval_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollection_Interface_Nested_InByval_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_Nested_InByref_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollection_Interface_Nested_InByref_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_Nested_InByref_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollection_Interface_Nested_InByref_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Nested_DefaultReflection.o: ../../SourceCppGenRpc/RpcCollection_Nested_DefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Nested_InByval_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollection_Nested_InByval_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Nested_InByval_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollection_Nested_InByval_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Nested_InByref_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollection_Nested_InByref_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Nested_InByref_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollection_Nested_InByref_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_PropDefaultReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_PropDefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_PropByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_PropByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_PropByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_PropByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_PropDefaultReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_PropDefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_PropByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_PropByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_PropByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_PropByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_Nested_PropDefaultReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_Nested_PropDefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_Nested_PropByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_Nested_PropByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Interface_Nested_PropByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Interface_Nested_PropByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Nested_PropDefaultReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Nested_PropDefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Nested_PropByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Nested_PropByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionDict_Nested_PropByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionDict_Nested_PropByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_PropDefaultReflection.o: ../../SourceCppGenRpc/RpcCollection_PropDefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_PropByvalReflection.o: ../../SourceCppGenRpc/RpcCollection_PropByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_PropByrefReflection.o: ../../SourceCppGenRpc/RpcCollection_PropByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_PropDefaultReflection.o: ../../SourceCppGenRpc/RpcCollection_Interface_PropDefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_PropByvalReflection.o: ../../SourceCppGenRpc/RpcCollection_Interface_PropByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_PropByrefReflection.o: ../../SourceCppGenRpc/RpcCollection_Interface_PropByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_Nested_PropDefaultReflection.o: ../../SourceCppGenRpc/RpcCollection_Interface_Nested_PropDefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_Nested_PropByvalReflection.o: ../../SourceCppGenRpc/RpcCollection_Interface_Nested_PropByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Interface_Nested_PropByrefReflection.o: ../../SourceCppGenRpc/RpcCollection_Interface_Nested_PropByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Nested_PropDefaultReflection.o: ../../SourceCppGenRpc/RpcCollection_Nested_PropDefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Nested_PropByvalReflection.o: ../../SourceCppGenRpc/RpcCollection_Nested_PropByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollection_Nested_PropByrefReflection.o: ../../SourceCppGenRpc/RpcCollection_Nested_PropByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_DefaultReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_DefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_InByval_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_InByval_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_InByval_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_InByval_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_InByref_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_InByref_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_InByref_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_InByref_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_DefaultReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_DefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_InByval_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_InByval_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_InByval_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_InByval_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_InByref_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_InByref_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_InByref_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_InByref_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_Nested_DefaultReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_Nested_DefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_Nested_InByval_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_Nested_InByval_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_Nested_InByval_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_Nested_InByval_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_Nested_InByref_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_Nested_InByref_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_Nested_InByref_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_Nested_InByref_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Nested_DefaultReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Nested_DefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Nested_InByval_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Nested_InByval_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Nested_InByval_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Nested_InByval_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Nested_InByref_OutByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Nested_InByref_OutByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Nested_InByref_OutByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Nested_InByref_OutByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_PropDefaultReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_PropDefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_PropByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_PropByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_PropByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_PropByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_PropDefaultReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_PropDefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_PropByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_PropByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_PropByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_PropByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_Nested_PropDefaultReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_Nested_PropDefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_Nested_PropByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_Nested_PropByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Interface_Nested_PropByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Interface_Nested_PropByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Nested_PropDefaultReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Nested_PropDefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Nested_PropByvalReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Nested_PropByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcCollectionOblist_Nested_PropByrefReflection.o: ../../SourceCppGenRpc/RpcCollectionOblist_Nested_PropByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcDtorReflection.o: ../../SourceCppGenRpc/RpcDtorReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcDtor2Reflection.o: ../../SourceCppGenRpc/RpcDtor2Reflection.cpp
	$(CPP_COMPILE)

./Obj/RpcDtor3Reflection.o: ../../SourceCppGenRpc/RpcDtor3Reflection.cpp
	$(CPP_COMPILE)

./Obj/RpcDtorListReflection.o: ../../SourceCppGenRpc/RpcDtorListReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcDtorList2Reflection.o: ../../SourceCppGenRpc/RpcDtorList2Reflection.cpp
	$(CPP_COMPILE)

./Obj/RpcDtorPropCachedReflection.o: ../../SourceCppGenRpc/RpcDtorPropCachedReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcDtorPropCachedListByvalReflection.o: ../../SourceCppGenRpc/RpcDtorPropCachedListByvalReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcDtorPropCachedListVByrefReflection.o: ../../SourceCppGenRpc/RpcDtorPropCachedListVByrefReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcFailDoubleRegistrationReflection.o: ../../SourceCppGenRpc/RpcFailDoubleRegistrationReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcInheritanceReflection.o: ../../SourceCppGenRpc/RpcInheritanceReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcInheritance_MethodExceptionReflection.o: ../../SourceCppGenRpc/RpcInheritance_MethodExceptionReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcInheritance_EventExceptionReflection.o: ../../SourceCppGenRpc/RpcInheritance_EventExceptionReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcLocalAndWrapperReflection.o: ../../SourceCppGenRpc/RpcLocalAndWrapperReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcNullableReflection.o: ../../SourceCppGenRpc/RpcNullableReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcOverloadingReflection.o: ../../SourceCppGenRpc/RpcOverloadingReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcEventReflection.o: ../../SourceCppGenRpc/RpcEventReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcEventArgsReflection.o: ../../SourceCppGenRpc/RpcEventArgsReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcEventOblistReflection.o: ../../SourceCppGenRpc/RpcEventOblistReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcListOps_DictionaryExceptionReflection.o: ../../SourceCppGenRpc/RpcListOps_DictionaryExceptionReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcListOps_ListExceptionReflection.o: ../../SourceCppGenRpc/RpcListOps_ListExceptionReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcListOps_OblistEventExceptionReflection.o: ../../SourceCppGenRpc/RpcListOps_OblistEventExceptionReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcPrimitiveTypesReflection.o: ../../SourceCppGenRpc/RpcPrimitiveTypesReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcPropCachedReflection.o: ../../SourceCppGenRpc/RpcPropCachedReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcPropDefaultReflection.o: ../../SourceCppGenRpc/RpcPropDefaultReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcPropDefaultInterfaceReflection.o: ../../SourceCppGenRpc/RpcPropDefaultInterfaceReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcPropDefaultInterfaceListReflection.o: ../../SourceCppGenRpc/RpcPropDefaultInterfaceListReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcPropDefaultListReflection.o: ../../SourceCppGenRpc/RpcPropDefaultListReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcPropDynamicReflection.o: ../../SourceCppGenRpc/RpcPropDynamicReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcRequestServiceReflection.o: ../../SourceCppGenRpc/RpcRequestServiceReflection.cpp
	$(CPP_COMPILE)

./Obj/RpcServiceWrapperReflection.o: ../../SourceCppGenRpc/RpcServiceWrapperReflection.cpp
	$(CPP_COMPILE)

./Obj/CppTypes.o: ../../Source/CppTypes.cpp
	$(CPP_COMPILE)

./Obj/TestCasesRpcStdio_Driver.o: ../../SourceCppGenRpc/TestCasesRpcStdio_Driver.cpp
	$(CPP_COMPILE)

./Obj/Main.o: ../../UnitTest/RpcStdioTest_Driver/Main.cpp
	$(CPP_COMPILE)
