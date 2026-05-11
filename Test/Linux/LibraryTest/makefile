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

all:pre-build ./Bin/UnitTest

./Bin/UnitTest:./Obj/Vlpp.o ./Obj/Vlpp.Linux.o ./Obj/VlppGlrParser.o ./Obj/VlppOS.o ./Obj/VlppOS.Linux.o ./Obj/VlppReflection.o ./Obj/VlppRegex.o ./Obj/WfLibraryCppHelper.o ./Obj/WfLibraryPredefined.o ./Obj/WfLibraryReflection.o ./Obj/WfLibraryRpc.o ./Obj/WfLibraryRpcController.o ./Obj/WfLibraryRpcJson.o ./Obj/WfLibraryRpcLifecycle.o ./Obj/WfLibraryRpcWrappers.o ./Obj/RpcDualLifecycleMock.o ./Obj/Main.o ./Obj/TestLibrary.o ./Obj/TestLibraryRpcByval.o
	$(CPP_LINK)

./Obj/Vlpp.o: ../../../Import/Vlpp.cpp ../../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/Vlpp.Linux.o: ../../../Import/Vlpp.Linux.cpp ../../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/VlppGlrParser.o: ../../../Import/VlppGlrParser.cpp ../../../Import/VlppGlrParser.h ../../../Import/VlppReflection.h ../../../Import/VlppOS.h ../../../Import/Vlpp.h ../../../Import/VlppRegex.h
	$(CPP_COMPILE)

./Obj/VlppOS.o: ../../../Import/VlppOS.cpp ../../../Import/VlppOS.h ../../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/VlppOS.Linux.o: ../../../Import/VlppOS.Linux.cpp ../../../Import/VlppOS.h ../../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/VlppReflection.o: ../../../Import/VlppReflection.cpp ../../../Import/VlppReflection.h ../../../Import/VlppOS.h ../../../Import/Vlpp.h ../../../Import/VlppRegex.h
	$(CPP_COMPILE)

./Obj/VlppRegex.o: ../../../Import/VlppRegex.cpp ../../../Import/VlppRegex.h ../../../Import/Vlpp.h ../../../Import/VlppOS.h
	$(CPP_COMPILE)

./Obj/WfLibraryCppHelper.o: ../../../Source/Library/WfLibraryCppHelper.cpp ../../../Source/Library/WfLibraryCppHelper.h ../../../Source/Library/WfLibraryPredefined.h ../../../Import/VlppReflection.h ../../../Import/VlppOS.h ../../../Import/Vlpp.h ../../../Import/VlppRegex.h
	$(CPP_COMPILE)

./Obj/WfLibraryPredefined.o: ../../../Source/Library/WfLibraryPredefined.cpp ../../../Source/Library/WfLibraryPredefined.h ../../../Import/VlppReflection.h ../../../Import/VlppOS.h ../../../Import/Vlpp.h ../../../Import/VlppRegex.h
	$(CPP_COMPILE)

./Obj/WfLibraryReflection.o: ../../../Source/Library/WfLibraryReflection.cpp ../../../Source/Library/WfLibraryReflection.h ../../../Source/Library/WfLibraryPredefined.h ../../../Import/VlppReflection.h ../../../Import/VlppOS.h ../../../Import/Vlpp.h ../../../Import/VlppRegex.h ../../../Source/Library/Rpc/WfLibraryRpc.h ../../../Source/Library/Rpc/../WfLibraryPredefined.h ../../../Source/Library/Rpc/WfLibraryRpcJson.h ../../../Import/VlppGlrParser.h ../../../Source/Library/Rpc/WfLibraryRpcWrappers.h
	$(CPP_COMPILE)

./Obj/WfLibraryRpc.o: ../../../Source/Library/Rpc/WfLibraryRpc.cpp ../../../Source/Library/Rpc/WfLibraryRpc.h ../../../Source/Library/Rpc/../WfLibraryPredefined.h ../../../Import/VlppReflection.h ../../../Import/VlppOS.h ../../../Import/Vlpp.h ../../../Import/VlppRegex.h
	$(CPP_COMPILE)

./Obj/WfLibraryRpcController.o: ../../../Source/Library/Rpc/WfLibraryRpcController.cpp ../../../Source/Library/Rpc/WfLibraryRpcController.h ../../../Source/Library/Rpc/WfLibraryRpc.h ../../../Source/Library/Rpc/../WfLibraryPredefined.h ../../../Import/VlppReflection.h ../../../Import/VlppOS.h ../../../Import/Vlpp.h ../../../Import/VlppRegex.h
	$(CPP_COMPILE)

./Obj/WfLibraryRpcJson.o: ../../../Source/Library/Rpc/WfLibraryRpcJson.cpp ../../../Source/Library/Rpc/WfLibraryRpcJson.h ../../../Source/Library/Rpc/WfLibraryRpc.h ../../../Source/Library/Rpc/../WfLibraryPredefined.h ../../../Import/VlppReflection.h ../../../Import/VlppOS.h ../../../Import/Vlpp.h ../../../Import/VlppRegex.h ../../../Import/VlppGlrParser.h ../../../Source/Library/Rpc/../WfLibraryCppHelper.h
	$(CPP_COMPILE)

./Obj/WfLibraryRpcLifecycle.o: ../../../Source/Library/Rpc/WfLibraryRpcLifecycle.cpp ../../../Source/Library/Rpc/WfLibraryRpcLifecycle.h ../../../Source/Library/Rpc/WfLibraryRpcController.h ../../../Source/Library/Rpc/WfLibraryRpc.h ../../../Source/Library/Rpc/../WfLibraryPredefined.h ../../../Import/VlppReflection.h ../../../Import/VlppOS.h ../../../Import/Vlpp.h ../../../Import/VlppRegex.h ../../../Source/Library/Rpc/WfLibraryRpcWrappers.h
	$(CPP_COMPILE)

./Obj/WfLibraryRpcWrappers.o: ../../../Source/Library/Rpc/WfLibraryRpcWrappers.cpp ../../../Source/Library/Rpc/WfLibraryRpcWrappers.h ../../../Source/Library/Rpc/WfLibraryRpc.h ../../../Source/Library/Rpc/../WfLibraryPredefined.h ../../../Import/VlppReflection.h ../../../Import/VlppOS.h ../../../Import/Vlpp.h ../../../Import/VlppRegex.h ../../../Source/Library/Rpc/WfLibraryRpcLifecycle.h ../../../Source/Library/Rpc/WfLibraryRpcController.h ../../../Source/Library/Rpc/WfLibraryRpcJson.h ../../../Import/VlppGlrParser.h ../../../Source/Library/Rpc/../WfLibraryReflection.h ../../../Source/Library/Rpc/../Rpc/WfLibraryRpc.h ../../../Source/Library/Rpc/../Rpc/WfLibraryRpcJson.h ../../../Source/Library/Rpc/../Rpc/WfLibraryRpcWrappers.h
	$(CPP_COMPILE)

./Obj/RpcDualLifecycleMock.o: ../../Source/RpcDualLifecycleMock.cpp ../../Source/RpcDualLifecycleMock.h ../../Source/../../Source/Library/Rpc/WfLibraryRpcLifecycle.h ../../Source/../../Source/Library/Rpc/WfLibraryRpcController.h ../../Source/../../Source/Library/Rpc/WfLibraryRpc.h ../../Source/../../Source/Library/Rpc/../WfLibraryPredefined.h ../../../Import/VlppReflection.h ../../../Import/VlppOS.h ../../../Import/Vlpp.h ../../../Import/VlppRegex.h
	$(CPP_COMPILE)

./Obj/Main.o: ../../UnitTest/LibraryTest/Main.cpp ../../UnitTest/LibraryTest/../../Source/CppTypes.h ../../../Import/Vlpp.h ../../UnitTest/LibraryTest/../../Source/../../Source/Library/WfLibraryCppHelper.h ../../UnitTest/LibraryTest/../../Source/../../Source/Library/WfLibraryPredefined.h ../../../Import/VlppReflection.h ../../../Import/VlppOS.h ../../../Import/VlppRegex.h ../../UnitTest/LibraryTest/../../Source/../../Source/Library/WfLibraryReflection.h ../../UnitTest/LibraryTest/../../Source/../../Source/Library/Rpc/WfLibraryRpc.h ../../UnitTest/LibraryTest/../../Source/../../Source/Library/Rpc/../WfLibraryPredefined.h ../../UnitTest/LibraryTest/../../Source/../../Source/Library/Rpc/WfLibraryRpcJson.h ../../../Import/VlppGlrParser.h ../../UnitTest/LibraryTest/../../Source/../../Source/Library/Rpc/WfLibraryRpcWrappers.h
	$(CPP_COMPILE)

./Obj/TestLibrary.o: ../../UnitTest/LibraryTest/TestLibrary.cpp ../../UnitTest/LibraryTest/../../../Source/Library/WfLibraryCppHelper.h ../../UnitTest/LibraryTest/../../../Source/Library/WfLibraryPredefined.h ../../../Import/VlppReflection.h ../../../Import/VlppOS.h ../../../Import/Vlpp.h ../../../Import/VlppRegex.h
	$(CPP_COMPILE)

./Obj/TestLibraryRpcByval.o: ../../UnitTest/LibraryTest/TestLibraryRpcByval.cpp ../../UnitTest/LibraryTest/../../Source/RpcDualLifecycleMock.h ../../UnitTest/LibraryTest/../../Source/../../Source/Library/Rpc/WfLibraryRpcLifecycle.h ../../UnitTest/LibraryTest/../../Source/../../Source/Library/Rpc/WfLibraryRpcController.h ../../UnitTest/LibraryTest/../../Source/../../Source/Library/Rpc/WfLibraryRpc.h ../../UnitTest/LibraryTest/../../Source/../../Source/Library/Rpc/../WfLibraryPredefined.h ../../../Import/VlppReflection.h ../../../Import/VlppOS.h ../../../Import/Vlpp.h ../../../Import/VlppRegex.h ../../UnitTest/LibraryTest/../../../Source/Library/WfLibraryReflection.h ../../UnitTest/LibraryTest/../../../Source/Library/WfLibraryPredefined.h ../../UnitTest/LibraryTest/../../../Source/Library/Rpc/WfLibraryRpc.h ../../UnitTest/LibraryTest/../../../Source/Library/Rpc/WfLibraryRpcJson.h ../../../Import/VlppGlrParser.h ../../UnitTest/LibraryTest/../../../Source/Library/Rpc/WfLibraryRpcWrappers.h
	$(CPP_COMPILE)
