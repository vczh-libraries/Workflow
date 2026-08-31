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

./Bin/RpcStdioTest_Driver:./Obj/Vlpp.o ./Obj/Vlpp.Linux.o ./Obj/VlppGlrParser.o ./Obj/VlppOS.o ./Obj/VlppOS.Linux.o ./Obj/VlppReflection.o ./Obj/VlppRegex.o ./Obj/WfLibraryCppHelper.o ./Obj/WfLibraryPredefined.o ./Obj/WfLibraryReflection.o ./Obj/WfLibraryRpc.o ./Obj/WfLibraryRpcController.o ./Obj/WfLibraryRpcLifecycle.o ./Obj/WfLibraryRpcWrappers.o ./Obj/WfLibraryRpcJson.o ./Obj/WfLibraryRpcJsonDispatcherClient.o ./Obj/WfLibraryRpcJsonDispatcherServer.o ./Obj/RpcCollectionDict_Default.o ./Obj/RpcCollectionDict_InByval_OutByref.o ./Obj/RpcCollectionDict_InByval_OutByval.o ./Obj/RpcCollectionDict_InByref_OutByval.o ./Obj/RpcCollectionDict_InByref_OutByref.o ./Obj/RpcCollectionDict_Interface_Default.o ./Obj/RpcCollectionDict_Interface_InByval_OutByref.o ./Obj/RpcCollectionDict_Interface_InByval_OutByval.o ./Obj/RpcCollectionDict_Interface_InByref_OutByval.o ./Obj/RpcCollectionDict_Interface_InByref_OutByref.o ./Obj/RpcCollectionDict_Interface_Nested_Default.o ./Obj/RpcCollectionDict_Interface_Nested_InByval_OutByref.o ./Obj/RpcCollectionDict_Interface_Nested_InByval_OutByval.o ./Obj/RpcCollectionDict_Interface_Nested_InByref_OutByval.o ./Obj/RpcCollectionDict_Interface_Nested_InByref_OutByref.o ./Obj/RpcCollectionDict_Nested_Default.o ./Obj/RpcCollectionDict_Nested_InByval_OutByref.o ./Obj/RpcCollectionDict_Nested_InByval_OutByval.o ./Obj/RpcCollectionDict_Nested_InByref_OutByval.o ./Obj/RpcCollectionDict_Nested_InByref_OutByref.o ./Obj/RpcCollection_Default.o ./Obj/RpcCollection_InByval_OutByref.o ./Obj/RpcCollection_InByval_OutByval.o ./Obj/RpcCollection_InByref_OutByval.o ./Obj/RpcCollection_InByref_OutByref.o ./Obj/RpcCollection_Interface_Default.o ./Obj/RpcCollection_Interface_InByval_OutByref.o ./Obj/RpcCollection_Interface_InByval_OutByval.o ./Obj/RpcCollection_Interface_InByref_OutByval.o ./Obj/RpcCollection_Interface_InByref_OutByref.o ./Obj/RpcCollection_Interface_Nested_Default.o ./Obj/RpcCollection_Interface_Nested_InByval_OutByref.o ./Obj/RpcCollection_Interface_Nested_InByval_OutByval.o ./Obj/RpcCollection_Interface_Nested_InByref_OutByval.o ./Obj/RpcCollection_Interface_Nested_InByref_OutByref.o ./Obj/RpcCollection_Nested_Default.o ./Obj/RpcCollection_Nested_InByval_OutByref.o ./Obj/RpcCollection_Nested_InByval_OutByval.o ./Obj/RpcCollection_Nested_InByref_OutByval.o ./Obj/RpcCollection_Nested_InByref_OutByref.o ./Obj/RpcCollectionDict_PropDefault.o ./Obj/RpcCollectionDict_PropByval.o ./Obj/RpcCollectionDict_PropByref.o ./Obj/RpcCollectionDict_Interface_PropDefault.o ./Obj/RpcCollectionDict_Interface_PropByval.o ./Obj/RpcCollectionDict_Interface_PropByref.o ./Obj/RpcCollectionDict_Interface_Nested_PropDefault.o ./Obj/RpcCollectionDict_Interface_Nested_PropByval.o ./Obj/RpcCollectionDict_Interface_Nested_PropByref.o ./Obj/RpcCollectionDict_Nested_PropDefault.o ./Obj/RpcCollectionDict_Nested_PropByval.o ./Obj/RpcCollectionDict_Nested_PropByref.o ./Obj/RpcCollection_PropDefault.o ./Obj/RpcCollection_PropByval.o ./Obj/RpcCollection_PropByref.o ./Obj/RpcCollection_Interface_PropDefault.o ./Obj/RpcCollection_Interface_PropByval.o ./Obj/RpcCollection_Interface_PropByref.o ./Obj/RpcCollection_Interface_Nested_PropDefault.o ./Obj/RpcCollection_Interface_Nested_PropByval.o ./Obj/RpcCollection_Interface_Nested_PropByref.o ./Obj/RpcCollection_Nested_PropDefault.o ./Obj/RpcCollection_Nested_PropByval.o ./Obj/RpcCollection_Nested_PropByref.o ./Obj/RpcCollectionOblist_Default.o ./Obj/RpcCollectionOblist_InByval_OutByref.o ./Obj/RpcCollectionOblist_InByval_OutByval.o ./Obj/RpcCollectionOblist_InByref_OutByval.o ./Obj/RpcCollectionOblist_InByref_OutByref.o ./Obj/RpcCollectionOblist_Interface_Default.o ./Obj/RpcCollectionOblist_Interface_InByval_OutByref.o ./Obj/RpcCollectionOblist_Interface_InByval_OutByval.o ./Obj/RpcCollectionOblist_Interface_InByref_OutByval.o ./Obj/RpcCollectionOblist_Interface_InByref_OutByref.o ./Obj/RpcCollectionOblist_Interface_Nested_Default.o ./Obj/RpcCollectionOblist_Interface_Nested_InByval_OutByref.o ./Obj/RpcCollectionOblist_Interface_Nested_InByval_OutByval.o ./Obj/RpcCollectionOblist_Interface_Nested_InByref_OutByval.o ./Obj/RpcCollectionOblist_Interface_Nested_InByref_OutByref.o ./Obj/RpcCollectionOblist_Nested_Default.o ./Obj/RpcCollectionOblist_Nested_InByval_OutByref.o ./Obj/RpcCollectionOblist_Nested_InByval_OutByval.o ./Obj/RpcCollectionOblist_Nested_InByref_OutByval.o ./Obj/RpcCollectionOblist_Nested_InByref_OutByref.o ./Obj/RpcCollectionOblist_PropDefault.o ./Obj/RpcCollectionOblist_PropByval.o ./Obj/RpcCollectionOblist_PropByref.o ./Obj/RpcCollectionOblist_Interface_PropDefault.o ./Obj/RpcCollectionOblist_Interface_PropByval.o ./Obj/RpcCollectionOblist_Interface_PropByref.o ./Obj/RpcCollectionOblist_Interface_Nested_PropDefault.o ./Obj/RpcCollectionOblist_Interface_Nested_PropByval.o ./Obj/RpcCollectionOblist_Interface_Nested_PropByref.o ./Obj/RpcCollectionOblist_Nested_PropDefault.o ./Obj/RpcCollectionOblist_Nested_PropByval.o ./Obj/RpcCollectionOblist_Nested_PropByref.o ./Obj/RpcDtor.o ./Obj/RpcDtor2.o ./Obj/RpcDtor3.o ./Obj/RpcDtorList.o ./Obj/RpcDtorList2.o ./Obj/RpcDtorPropCached.o ./Obj/RpcDtorPropCachedListByval.o ./Obj/RpcDtorPropCachedListVByref.o ./Obj/RpcFailDoubleRegistration.o ./Obj/RpcFailDoubleRegistration_SharedMemsp.o ./Obj/RpcInheritance.o ./Obj/RpcInheritance_MethodException.o ./Obj/RpcInheritance_EventException.o ./Obj/RpcLocalAndWrapper.o ./Obj/RpcLocalAndWrapper_SharedMemsp.o ./Obj/RpcNullable.o ./Obj/RpcOverloading.o ./Obj/RpcEvent.o ./Obj/RpcEventArgs.o ./Obj/RpcEventOblist.o ./Obj/RpcListOps_DictionaryException.o ./Obj/RpcListOps_ListException.o ./Obj/RpcListOps_OblistEventException.o ./Obj/RpcPrimitiveTypes.o ./Obj/RpcPropCached.o ./Obj/RpcPropDefault.o ./Obj/RpcPropDefaultInterface.o ./Obj/RpcPropDefaultInterfaceList.o ./Obj/RpcPropDefaultList.o ./Obj/RpcPropDynamic.o ./Obj/RpcRequestService.o ./Obj/RpcServiceWrapper.o ./Obj/RpcServiceWrapper_SharedMemsp.o ./Obj/CppTypes.o ./Obj/TestCasesRpcStdio_Driver.o ./Obj/Main.o
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

./Obj/RpcFailDoubleRegistration_SharedMemsp.o: ../../SourceCppGenRpc/RpcFailDoubleRegistration_SharedMemsp.cpp
	$(CPP_COMPILE)

./Obj/RpcInheritance.o: ../../SourceCppGenRpc/RpcInheritance.cpp
	$(CPP_COMPILE)

./Obj/RpcInheritance_MethodException.o: ../../SourceCppGenRpc/RpcInheritance_MethodException.cpp
	$(CPP_COMPILE)

./Obj/RpcInheritance_EventException.o: ../../SourceCppGenRpc/RpcInheritance_EventException.cpp
	$(CPP_COMPILE)

./Obj/RpcLocalAndWrapper.o: ../../SourceCppGenRpc/RpcLocalAndWrapper.cpp
	$(CPP_COMPILE)

./Obj/RpcLocalAndWrapper_SharedMemsp.o: ../../SourceCppGenRpc/RpcLocalAndWrapper_SharedMemsp.cpp
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

./Obj/RpcServiceWrapper_SharedMemsp.o: ../../SourceCppGenRpc/RpcServiceWrapper_SharedMemsp.cpp
	$(CPP_COMPILE)

./Obj/CppTypes.o: ../../Source/CppTypes.cpp
	$(CPP_COMPILE)

./Obj/TestCasesRpcStdio_Driver.o: ../../SourceCppGenRpc/TestCasesRpcStdio_Driver.cpp
	$(CPP_COMPILE)

./Obj/Main.o: ../../UnitTest/RpcStdioTest_Driver/Main.cpp
	$(CPP_COMPILE)
