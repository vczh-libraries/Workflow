.PHONY: all clean pre-build
.DEFAULT_GOAL := all

CPP_COMPILE_OPTIONS=-I ../../../Import -DVCZH_DEBUG_METAONLY_REFLECTION
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

./Bin/UnitTest:./Obj/Vlpp.o ./Obj/Vlpp.Linux.o ./Obj/VlppGlrParser.o ./Obj/VlppOS.o ./Obj/VlppOS.Linux.o ./Obj/VlppReflection.o ./Obj/VlppRegex.o ./Obj/WfLibraryCppHelper.o ./Obj/WfLibraryPredefined.o ./Obj/WfLibraryReflection.o ./Obj/WfLibraryRpc.o ./Obj/WfLibraryRpcController.o ./Obj/WfLibraryRpcLifecycle.o ./Obj/WfLibraryRpcWrappers.o ./Obj/WfLibraryRpcJson.o ./Obj/WfLibraryRpcJsonDispatcherClient.o ./Obj/WfLibraryRpcJsonDispatcherServer.o ./Obj/AnonymousLambda.o ./Obj/BindComplex.o ./Obj/BindCustomInterface.o ./Obj/BindCustomInterface2.o ./Obj/BindFormat.o ./Obj/BindLet.o ./Obj/BindNull.o ./Obj/BindSimple.o ./Obj/BindSimple2.o ./Obj/CallStaticMethod.o ./Obj/ClassCtor.o ./Obj/ClassDtor.o ./Obj/ClassField.o ./Obj/ClassMethod.o ./Obj/CoAsync.o ./Obj/CoAsync2.o ./Obj/CoEnum.o ./Obj/CoEnum2.o ./Obj/ControlFlow.o ./Obj/CoRawCoroutine.o ./Obj/CoRawCoroutine2.o ./Obj/CoRawCoroutine3.o ./Obj/CoRawCoroutine4.o ./Obj/CoRawCoroutine5.o ./Obj/CoRawCoroutine6.o ./Obj/CoRawCoroutine7.o ./Obj/CoSmcCalculator.o ./Obj/CoSmcGoto.o ./Obj/CoSmcPush.o ./Obj/CoSmcSwitchD1.o ./Obj/CoSmcSwitchD2.o ./Obj/CoSmcSwitchI1.o ./Obj/CoSmcSwitchI2.o ./Obj/CoSmcSwitchIR1.o ./Obj/CoSmcSwitchIR2.o ./Obj/CoSmcSwitchP1.o ./Obj/CoSmcSwitchP2.o ./Obj/CoSmcSwitchPR1.o ./Obj/CoSmcSwitchPR2.o ./Obj/CoSmcTryCatch1.o ./Obj/CoSmcTryCatch2.o ./Obj/Delete.o ./Obj/ElementInSet.o ./Obj/EnumCtor.o ./Obj/EnumCtor2.o ./Obj/Event.o ./Obj/Event2.o ./Obj/FailedThen.o ./Obj/ForEach.o ./Obj/GlobalVariable.o ./Obj/HelloWorld.o ./Obj/IfNotNull.o ./Obj/LetIn.o ./Obj/ListProcessing.o ./Obj/MapProcessing.o ./Obj/MethodClosure.o ./Obj/NamedLambda.o ./Obj/NestedLambda.o ./Obj/NewCustomInterface.o ./Obj/NewCustomInterface2.o ./Obj/NewCustomInterface3.o ./Obj/NewCustomInterfaceDtor.o ./Obj/NewCustomInterfaceDtor2.o ./Obj/NewCustomInterfaceDtor3.o ./Obj/NewEmptyInterface.o ./Obj/NewEmptyInterfaceDtor.o ./Obj/NewInterface.o ./Obj/NewInterfaceDtor.o ./Obj/NullableCast.o ./Obj/ObservableList.o ./Obj/OpAdd.o ./Obj/OpAnd.o ./Obj/OpCompare.o ./Obj/OpCompareIndexOf.o ./Obj/OpCompareReference.o ./Obj/OpCompareStruct1.o ./Obj/OpCompareStruct2.o ./Obj/OpCompareWithString.o ./Obj/OpDiv.o ./Obj/OpMod.o ./Obj/OpMul.o ./Obj/OpNegative.o ./Obj/OpNot.o ./Obj/OpOr.o ./Obj/OpPositive.o ./Obj/OpShl.o ./Obj/OpShr.o ./Obj/OpSub.o ./Obj/OpXor.o ./Obj/OrderedLambda.o ./Obj/Overloading.o ./Obj/Property.o ./Obj/RecursiveFunction.o ./Obj/StaticInit.o ./Obj/StaticMethod.o ./Obj/StructCtor.o ./Obj/StructCtor2.o ./Obj/TryCatch.o ./Obj/TryFinally.o ./Obj/TryFinally2.o ./Obj/WorkflowAttributes.o ./Obj/WorkflowAttributes_Expression.o ./Obj/WorkflowHints.o ./Obj/WorkflowMultiFiles.o ./Obj/WorkflowMultiFiles_Animal.o ./Obj/WorkflowMultiFiles_Bird.o ./Obj/WorkflowMultiFiles_Lion.o ./Obj/Event2Reflection.o ./Obj/OpCompareIndexOfReflection.o ./Obj/OpCompareStruct2Reflection.o ./Obj/WorkflowAttributesReflection.o ./Obj/WorkflowMultiFilesReflection.o ./Obj/BindCustomInterface2Reflection.o ./Obj/BindCustomInterfaceReflection.o ./Obj/BindLetReflection.o ./Obj/BindNullReflection.o ./Obj/BindSimple2Reflection.o ./Obj/BindSimpleReflection.o ./Obj/CallStaticMethodReflection.o ./Obj/ClassCtorReflection.o ./Obj/ClassDtorReflection.o ./Obj/ClassMethodReflection.o ./Obj/CoAsync2Reflection.o ./Obj/CoAsyncReflection.o ./Obj/CoSmcCalculatorReflection.o ./Obj/CoSmcGotoReflection.o ./Obj/CoSmcPushReflection.o ./Obj/CoSmcSwitchD1Reflection.o ./Obj/CoSmcSwitchD2Reflection.o ./Obj/CoSmcSwitchI1Reflection.o ./Obj/CoSmcSwitchI2Reflection.o ./Obj/CoSmcSwitchIR1Reflection.o ./Obj/CoSmcSwitchIR2Reflection.o ./Obj/CoSmcSwitchP1Reflection.o ./Obj/CoSmcSwitchP2Reflection.o ./Obj/CoSmcSwitchPR1Reflection.o ./Obj/CoSmcSwitchPR2Reflection.o ./Obj/CoSmcTryCatch1Reflection.o ./Obj/CoSmcTryCatch2Reflection.o ./Obj/EnumCtor2Reflection.o ./Obj/NestedLambdaReflection.o ./Obj/NewCustomInterface2Reflection.o ./Obj/NewCustomInterface3Reflection.o ./Obj/NewCustomInterfaceDtor2Reflection.o ./Obj/NewCustomInterfaceDtor3Reflection.o ./Obj/NewCustomInterfaceDtorReflection.o ./Obj/NewCustomInterfaceReflection.o ./Obj/NewEmptyInterfaceDtorReflection.o ./Obj/NewEmptyInterfaceReflection.o ./Obj/NewInterfaceDtorReflection.o ./Obj/NullableCastReflection.o ./Obj/OpCompareReflection.o ./Obj/StructCtor2Reflection.o ./Obj/StructCtorReflection.o ./Obj/WorkflowHintsReflection.o ./Obj/RpcCollectionDict_Default.o ./Obj/RpcCollectionDict_InByval_OutByref.o ./Obj/RpcCollectionDict_InByval_OutByval.o ./Obj/RpcCollectionDict_InByref_OutByval.o ./Obj/RpcCollectionDict_InByref_OutByref.o ./Obj/RpcCollectionDict_Interface_Default.o ./Obj/RpcCollectionDict_Interface_InByval_OutByref.o ./Obj/RpcCollectionDict_Interface_InByval_OutByval.o ./Obj/RpcCollectionDict_Interface_InByref_OutByval.o ./Obj/RpcCollectionDict_Interface_InByref_OutByref.o ./Obj/RpcCollectionDict_Interface_Nested_Default.o ./Obj/RpcCollectionDict_Interface_Nested_InByval_OutByref.o ./Obj/RpcCollectionDict_Interface_Nested_InByval_OutByval.o ./Obj/RpcCollectionDict_Interface_Nested_InByref_OutByval.o ./Obj/RpcCollectionDict_Interface_Nested_InByref_OutByref.o ./Obj/RpcCollectionDict_Nested_Default.o ./Obj/RpcCollectionDict_Nested_InByval_OutByref.o ./Obj/RpcCollectionDict_Nested_InByval_OutByval.o ./Obj/RpcCollectionDict_Nested_InByref_OutByval.o ./Obj/RpcCollectionDict_Nested_InByref_OutByref.o ./Obj/RpcCollection_Default.o ./Obj/RpcCollection_InByval_OutByref.o ./Obj/RpcCollection_InByval_OutByval.o ./Obj/RpcCollection_InByref_OutByval.o ./Obj/RpcCollection_InByref_OutByref.o ./Obj/RpcCollection_Interface_Default.o ./Obj/RpcCollection_Interface_InByval_OutByref.o ./Obj/RpcCollection_Interface_InByval_OutByval.o ./Obj/RpcCollection_Interface_InByref_OutByval.o ./Obj/RpcCollection_Interface_InByref_OutByref.o ./Obj/RpcCollection_Interface_Nested_Default.o ./Obj/RpcCollection_Interface_Nested_InByval_OutByref.o ./Obj/RpcCollection_Interface_Nested_InByval_OutByval.o ./Obj/RpcCollection_Interface_Nested_InByref_OutByval.o ./Obj/RpcCollection_Interface_Nested_InByref_OutByref.o ./Obj/RpcCollection_Nested_Default.o ./Obj/RpcCollection_Nested_InByval_OutByref.o ./Obj/RpcCollection_Nested_InByval_OutByval.o ./Obj/RpcCollection_Nested_InByref_OutByval.o ./Obj/RpcCollection_Nested_InByref_OutByref.o ./Obj/RpcCollectionDict_PropDefault.o ./Obj/RpcCollectionDict_PropByval.o ./Obj/RpcCollectionDict_PropByref.o ./Obj/RpcCollectionDict_Interface_PropDefault.o ./Obj/RpcCollectionDict_Interface_PropByval.o ./Obj/RpcCollectionDict_Interface_PropByref.o ./Obj/RpcCollectionDict_Interface_Nested_PropDefault.o ./Obj/RpcCollectionDict_Interface_Nested_PropByval.o ./Obj/RpcCollectionDict_Interface_Nested_PropByref.o ./Obj/RpcCollectionDict_Nested_PropDefault.o ./Obj/RpcCollectionDict_Nested_PropByval.o ./Obj/RpcCollectionDict_Nested_PropByref.o ./Obj/RpcCollection_PropDefault.o ./Obj/RpcCollection_PropByval.o ./Obj/RpcCollection_PropByref.o ./Obj/RpcCollection_Interface_PropDefault.o ./Obj/RpcCollection_Interface_PropByval.o ./Obj/RpcCollection_Interface_PropByref.o ./Obj/RpcCollection_Interface_Nested_PropDefault.o ./Obj/RpcCollection_Interface_Nested_PropByval.o ./Obj/RpcCollection_Interface_Nested_PropByref.o ./Obj/RpcCollection_Nested_PropDefault.o ./Obj/RpcCollection_Nested_PropByval.o ./Obj/RpcCollection_Nested_PropByref.o ./Obj/RpcCollectionOblist_Default.o ./Obj/RpcCollectionOblist_InByval_OutByref.o ./Obj/RpcCollectionOblist_InByval_OutByval.o ./Obj/RpcCollectionOblist_InByref_OutByval.o ./Obj/RpcCollectionOblist_InByref_OutByref.o ./Obj/RpcCollectionOblist_Interface_Default.o ./Obj/RpcCollectionOblist_Interface_InByval_OutByref.o ./Obj/RpcCollectionOblist_Interface_InByval_OutByval.o ./Obj/RpcCollectionOblist_Interface_InByref_OutByval.o ./Obj/RpcCollectionOblist_Interface_InByref_OutByref.o ./Obj/RpcCollectionOblist_Interface_Nested_Default.o ./Obj/RpcCollectionOblist_Interface_Nested_InByval_OutByref.o ./Obj/RpcCollectionOblist_Interface_Nested_InByval_OutByval.o ./Obj/RpcCollectionOblist_Interface_Nested_InByref_OutByval.o ./Obj/RpcCollectionOblist_Interface_Nested_InByref_OutByref.o ./Obj/RpcCollectionOblist_Nested_Default.o ./Obj/RpcCollectionOblist_Nested_InByval_OutByref.o ./Obj/RpcCollectionOblist_Nested_InByval_OutByval.o ./Obj/RpcCollectionOblist_Nested_InByref_OutByval.o ./Obj/RpcCollectionOblist_Nested_InByref_OutByref.o ./Obj/RpcCollectionOblist_PropDefault.o ./Obj/RpcCollectionOblist_PropByval.o ./Obj/RpcCollectionOblist_PropByref.o ./Obj/RpcCollectionOblist_Interface_PropDefault.o ./Obj/RpcCollectionOblist_Interface_PropByval.o ./Obj/RpcCollectionOblist_Interface_PropByref.o ./Obj/RpcCollectionOblist_Interface_Nested_PropDefault.o ./Obj/RpcCollectionOblist_Interface_Nested_PropByval.o ./Obj/RpcCollectionOblist_Interface_Nested_PropByref.o ./Obj/RpcCollectionOblist_Nested_PropDefault.o ./Obj/RpcCollectionOblist_Nested_PropByval.o ./Obj/RpcCollectionOblist_Nested_PropByref.o ./Obj/RpcDtor.o ./Obj/RpcDtor2.o ./Obj/RpcDtor3.o ./Obj/RpcDtorList.o ./Obj/RpcDtorList2.o ./Obj/RpcDtorPropCached.o ./Obj/RpcDtorPropCachedListByval.o ./Obj/RpcDtorPropCachedListVByref.o ./Obj/RpcFailDoubleRegistration.o ./Obj/RpcInheritance.o ./Obj/RpcInheritance_MethodException.o ./Obj/RpcInheritance_EventException.o ./Obj/RpcLocalAndWrapper.o ./Obj/RpcNullable.o ./Obj/RpcOverloading.o ./Obj/RpcEvent.o ./Obj/RpcEventArgs.o ./Obj/RpcEventOblist.o ./Obj/RpcListOps_DictionaryException.o ./Obj/RpcListOps_ListException.o ./Obj/RpcListOps_OblistEventException.o ./Obj/RpcPrimitiveTypes.o ./Obj/RpcPropCached.o ./Obj/RpcPropDefault.o ./Obj/RpcPropDefaultInterface.o ./Obj/RpcPropDefaultInterfaceList.o ./Obj/RpcPropDefaultList.o ./Obj/RpcPropDynamic.o ./Obj/RpcRequestService.o ./Obj/RpcServiceWrapper.o ./Obj/RpcCollectionDict_DefaultReflection.o ./Obj/RpcCollectionDict_InByval_OutByrefReflection.o ./Obj/RpcCollectionDict_InByval_OutByvalReflection.o ./Obj/RpcCollectionDict_InByref_OutByvalReflection.o ./Obj/RpcCollectionDict_InByref_OutByrefReflection.o ./Obj/RpcCollectionDict_Interface_DefaultReflection.o ./Obj/RpcCollectionDict_Interface_InByval_OutByrefReflection.o ./Obj/RpcCollectionDict_Interface_InByval_OutByvalReflection.o ./Obj/RpcCollectionDict_Interface_InByref_OutByvalReflection.o ./Obj/RpcCollectionDict_Interface_InByref_OutByrefReflection.o ./Obj/RpcCollectionDict_Interface_Nested_DefaultReflection.o ./Obj/RpcCollectionDict_Interface_Nested_InByval_OutByrefReflection.o ./Obj/RpcCollectionDict_Interface_Nested_InByval_OutByvalReflection.o ./Obj/RpcCollectionDict_Interface_Nested_InByref_OutByvalReflection.o ./Obj/RpcCollectionDict_Interface_Nested_InByref_OutByrefReflection.o ./Obj/RpcCollectionDict_Nested_DefaultReflection.o ./Obj/RpcCollectionDict_Nested_InByval_OutByrefReflection.o ./Obj/RpcCollectionDict_Nested_InByval_OutByvalReflection.o ./Obj/RpcCollectionDict_Nested_InByref_OutByvalReflection.o ./Obj/RpcCollectionDict_Nested_InByref_OutByrefReflection.o ./Obj/RpcCollection_DefaultReflection.o ./Obj/RpcCollection_InByval_OutByrefReflection.o ./Obj/RpcCollection_InByval_OutByvalReflection.o ./Obj/RpcCollection_InByref_OutByvalReflection.o ./Obj/RpcCollection_InByref_OutByrefReflection.o ./Obj/RpcCollection_Interface_DefaultReflection.o ./Obj/RpcCollection_Interface_InByval_OutByrefReflection.o ./Obj/RpcCollection_Interface_InByval_OutByvalReflection.o ./Obj/RpcCollection_Interface_InByref_OutByvalReflection.o ./Obj/RpcCollection_Interface_InByref_OutByrefReflection.o ./Obj/RpcCollection_Interface_Nested_DefaultReflection.o ./Obj/RpcCollection_Interface_Nested_InByval_OutByrefReflection.o ./Obj/RpcCollection_Interface_Nested_InByval_OutByvalReflection.o ./Obj/RpcCollection_Interface_Nested_InByref_OutByvalReflection.o ./Obj/RpcCollection_Interface_Nested_InByref_OutByrefReflection.o ./Obj/RpcCollection_Nested_DefaultReflection.o ./Obj/RpcCollection_Nested_InByval_OutByrefReflection.o ./Obj/RpcCollection_Nested_InByval_OutByvalReflection.o ./Obj/RpcCollection_Nested_InByref_OutByvalReflection.o ./Obj/RpcCollection_Nested_InByref_OutByrefReflection.o ./Obj/RpcCollectionDict_PropDefaultReflection.o ./Obj/RpcCollectionDict_PropByvalReflection.o ./Obj/RpcCollectionDict_PropByrefReflection.o ./Obj/RpcCollectionDict_Interface_PropDefaultReflection.o ./Obj/RpcCollectionDict_Interface_PropByvalReflection.o ./Obj/RpcCollectionDict_Interface_PropByrefReflection.o ./Obj/RpcCollectionDict_Interface_Nested_PropDefaultReflection.o ./Obj/RpcCollectionDict_Interface_Nested_PropByvalReflection.o ./Obj/RpcCollectionDict_Interface_Nested_PropByrefReflection.o ./Obj/RpcCollectionDict_Nested_PropDefaultReflection.o ./Obj/RpcCollectionDict_Nested_PropByvalReflection.o ./Obj/RpcCollectionDict_Nested_PropByrefReflection.o ./Obj/RpcCollection_PropDefaultReflection.o ./Obj/RpcCollection_PropByvalReflection.o ./Obj/RpcCollection_PropByrefReflection.o ./Obj/RpcCollection_Interface_PropDefaultReflection.o ./Obj/RpcCollection_Interface_PropByvalReflection.o ./Obj/RpcCollection_Interface_PropByrefReflection.o ./Obj/RpcCollection_Interface_Nested_PropDefaultReflection.o ./Obj/RpcCollection_Interface_Nested_PropByvalReflection.o ./Obj/RpcCollection_Interface_Nested_PropByrefReflection.o ./Obj/RpcCollection_Nested_PropDefaultReflection.o ./Obj/RpcCollection_Nested_PropByvalReflection.o ./Obj/RpcCollection_Nested_PropByrefReflection.o ./Obj/RpcCollectionOblist_DefaultReflection.o ./Obj/RpcCollectionOblist_InByval_OutByrefReflection.o ./Obj/RpcCollectionOblist_InByval_OutByvalReflection.o ./Obj/RpcCollectionOblist_InByref_OutByvalReflection.o ./Obj/RpcCollectionOblist_InByref_OutByrefReflection.o ./Obj/RpcCollectionOblist_Interface_DefaultReflection.o ./Obj/RpcCollectionOblist_Interface_InByval_OutByrefReflection.o ./Obj/RpcCollectionOblist_Interface_InByval_OutByvalReflection.o ./Obj/RpcCollectionOblist_Interface_InByref_OutByvalReflection.o ./Obj/RpcCollectionOblist_Interface_InByref_OutByrefReflection.o ./Obj/RpcCollectionOblist_Interface_Nested_DefaultReflection.o ./Obj/RpcCollectionOblist_Interface_Nested_InByval_OutByrefReflection.o ./Obj/RpcCollectionOblist_Interface_Nested_InByval_OutByvalReflection.o ./Obj/RpcCollectionOblist_Interface_Nested_InByref_OutByvalReflection.o ./Obj/RpcCollectionOblist_Interface_Nested_InByref_OutByrefReflection.o ./Obj/RpcCollectionOblist_Nested_DefaultReflection.o ./Obj/RpcCollectionOblist_Nested_InByval_OutByrefReflection.o ./Obj/RpcCollectionOblist_Nested_InByval_OutByvalReflection.o ./Obj/RpcCollectionOblist_Nested_InByref_OutByvalReflection.o ./Obj/RpcCollectionOblist_Nested_InByref_OutByrefReflection.o ./Obj/RpcCollectionOblist_PropDefaultReflection.o ./Obj/RpcCollectionOblist_PropByvalReflection.o ./Obj/RpcCollectionOblist_PropByrefReflection.o ./Obj/RpcCollectionOblist_Interface_PropDefaultReflection.o ./Obj/RpcCollectionOblist_Interface_PropByvalReflection.o ./Obj/RpcCollectionOblist_Interface_PropByrefReflection.o ./Obj/RpcCollectionOblist_Interface_Nested_PropDefaultReflection.o ./Obj/RpcCollectionOblist_Interface_Nested_PropByvalReflection.o ./Obj/RpcCollectionOblist_Interface_Nested_PropByrefReflection.o ./Obj/RpcCollectionOblist_Nested_PropDefaultReflection.o ./Obj/RpcCollectionOblist_Nested_PropByvalReflection.o ./Obj/RpcCollectionOblist_Nested_PropByrefReflection.o ./Obj/RpcDtorReflection.o ./Obj/RpcDtor2Reflection.o ./Obj/RpcDtor3Reflection.o ./Obj/RpcDtorListReflection.o ./Obj/RpcDtorList2Reflection.o ./Obj/RpcDtorPropCachedReflection.o ./Obj/RpcDtorPropCachedListByvalReflection.o ./Obj/RpcDtorPropCachedListVByrefReflection.o ./Obj/RpcFailDoubleRegistrationReflection.o ./Obj/RpcInheritanceReflection.o ./Obj/RpcInheritance_MethodExceptionReflection.o ./Obj/RpcInheritance_EventExceptionReflection.o ./Obj/RpcLocalAndWrapperReflection.o ./Obj/RpcNullableReflection.o ./Obj/RpcOverloadingReflection.o ./Obj/RpcEventReflection.o ./Obj/RpcEventArgsReflection.o ./Obj/RpcEventOblistReflection.o ./Obj/RpcListOps_DictionaryExceptionReflection.o ./Obj/RpcListOps_ListExceptionReflection.o ./Obj/RpcListOps_OblistEventExceptionReflection.o ./Obj/RpcPrimitiveTypesReflection.o ./Obj/RpcPropCachedReflection.o ./Obj/RpcPropDefaultReflection.o ./Obj/RpcPropDefaultInterfaceReflection.o ./Obj/RpcPropDefaultInterfaceListReflection.o ./Obj/RpcPropDefaultListReflection.o ./Obj/RpcPropDynamicReflection.o ./Obj/RpcRequestServiceReflection.o ./Obj/RpcServiceWrapperReflection.o ./Obj/CppTypes.o ./Obj/TestCases.o ./Obj/TestCasesRpc.o ./Obj/GetExePath.o ./Obj/RpcDualJsonDispatcherMock.o ./Obj/RpcDualLifecycleMock.o ./Obj/Main.o
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

./Obj/AnonymousLambda.o: ../../SourceCppGen/AnonymousLambda.cpp
	$(CPP_COMPILE)

./Obj/BindComplex.o: ../../SourceCppGen/BindComplex.cpp
	$(CPP_COMPILE)

./Obj/BindCustomInterface.o: ../../SourceCppGen/BindCustomInterface.cpp
	$(CPP_COMPILE)

./Obj/BindCustomInterface2.o: ../../SourceCppGen/BindCustomInterface2.cpp
	$(CPP_COMPILE)

./Obj/BindFormat.o: ../../SourceCppGen/BindFormat.cpp
	$(CPP_COMPILE)

./Obj/BindLet.o: ../../SourceCppGen/BindLet.cpp
	$(CPP_COMPILE)

./Obj/BindNull.o: ../../SourceCppGen/BindNull.cpp
	$(CPP_COMPILE)

./Obj/BindSimple.o: ../../SourceCppGen/BindSimple.cpp
	$(CPP_COMPILE)

./Obj/BindSimple2.o: ../../SourceCppGen/BindSimple2.cpp
	$(CPP_COMPILE)

./Obj/CallStaticMethod.o: ../../SourceCppGen/CallStaticMethod.cpp
	$(CPP_COMPILE)

./Obj/ClassCtor.o: ../../SourceCppGen/ClassCtor.cpp
	$(CPP_COMPILE)

./Obj/ClassDtor.o: ../../SourceCppGen/ClassDtor.cpp
	$(CPP_COMPILE)

./Obj/ClassField.o: ../../SourceCppGen/ClassField.cpp
	$(CPP_COMPILE)

./Obj/ClassMethod.o: ../../SourceCppGen/ClassMethod.cpp
	$(CPP_COMPILE)

./Obj/CoAsync.o: ../../SourceCppGen/CoAsync.cpp
	$(CPP_COMPILE)

./Obj/CoAsync2.o: ../../SourceCppGen/CoAsync2.cpp
	$(CPP_COMPILE)

./Obj/CoEnum.o: ../../SourceCppGen/CoEnum.cpp
	$(CPP_COMPILE)

./Obj/CoEnum2.o: ../../SourceCppGen/CoEnum2.cpp
	$(CPP_COMPILE)

./Obj/ControlFlow.o: ../../SourceCppGen/ControlFlow.cpp
	$(CPP_COMPILE)

./Obj/CoRawCoroutine.o: ../../SourceCppGen/CoRawCoroutine.cpp
	$(CPP_COMPILE)

./Obj/CoRawCoroutine2.o: ../../SourceCppGen/CoRawCoroutine2.cpp
	$(CPP_COMPILE)

./Obj/CoRawCoroutine3.o: ../../SourceCppGen/CoRawCoroutine3.cpp
	$(CPP_COMPILE)

./Obj/CoRawCoroutine4.o: ../../SourceCppGen/CoRawCoroutine4.cpp
	$(CPP_COMPILE)

./Obj/CoRawCoroutine5.o: ../../SourceCppGen/CoRawCoroutine5.cpp
	$(CPP_COMPILE)

./Obj/CoRawCoroutine6.o: ../../SourceCppGen/CoRawCoroutine6.cpp
	$(CPP_COMPILE)

./Obj/CoRawCoroutine7.o: ../../SourceCppGen/CoRawCoroutine7.cpp
	$(CPP_COMPILE)

./Obj/CoSmcCalculator.o: ../../SourceCppGen/CoSmcCalculator.cpp
	$(CPP_COMPILE)

./Obj/CoSmcGoto.o: ../../SourceCppGen/CoSmcGoto.cpp
	$(CPP_COMPILE)

./Obj/CoSmcPush.o: ../../SourceCppGen/CoSmcPush.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchD1.o: ../../SourceCppGen/CoSmcSwitchD1.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchD2.o: ../../SourceCppGen/CoSmcSwitchD2.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchI1.o: ../../SourceCppGen/CoSmcSwitchI1.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchI2.o: ../../SourceCppGen/CoSmcSwitchI2.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchIR1.o: ../../SourceCppGen/CoSmcSwitchIR1.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchIR2.o: ../../SourceCppGen/CoSmcSwitchIR2.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchP1.o: ../../SourceCppGen/CoSmcSwitchP1.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchP2.o: ../../SourceCppGen/CoSmcSwitchP2.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchPR1.o: ../../SourceCppGen/CoSmcSwitchPR1.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchPR2.o: ../../SourceCppGen/CoSmcSwitchPR2.cpp
	$(CPP_COMPILE)

./Obj/CoSmcTryCatch1.o: ../../SourceCppGen/CoSmcTryCatch1.cpp
	$(CPP_COMPILE)

./Obj/CoSmcTryCatch2.o: ../../SourceCppGen/CoSmcTryCatch2.cpp
	$(CPP_COMPILE)

./Obj/Delete.o: ../../SourceCppGen/Delete.cpp
	$(CPP_COMPILE)

./Obj/ElementInSet.o: ../../SourceCppGen/ElementInSet.cpp
	$(CPP_COMPILE)

./Obj/EnumCtor.o: ../../SourceCppGen/EnumCtor.cpp
	$(CPP_COMPILE)

./Obj/EnumCtor2.o: ../../SourceCppGen/EnumCtor2.cpp
	$(CPP_COMPILE)

./Obj/Event.o: ../../SourceCppGen/Event.cpp
	$(CPP_COMPILE)

./Obj/Event2.o: ../../SourceCppGen/Event2.cpp
	$(CPP_COMPILE)

./Obj/FailedThen.o: ../../SourceCppGen/FailedThen.cpp
	$(CPP_COMPILE)

./Obj/ForEach.o: ../../SourceCppGen/ForEach.cpp
	$(CPP_COMPILE)

./Obj/GlobalVariable.o: ../../SourceCppGen/GlobalVariable.cpp
	$(CPP_COMPILE)

./Obj/HelloWorld.o: ../../SourceCppGen/HelloWorld.cpp
	$(CPP_COMPILE)

./Obj/IfNotNull.o: ../../SourceCppGen/IfNotNull.cpp
	$(CPP_COMPILE)

./Obj/LetIn.o: ../../SourceCppGen/LetIn.cpp
	$(CPP_COMPILE)

./Obj/ListProcessing.o: ../../SourceCppGen/ListProcessing.cpp
	$(CPP_COMPILE)

./Obj/MapProcessing.o: ../../SourceCppGen/MapProcessing.cpp
	$(CPP_COMPILE)

./Obj/MethodClosure.o: ../../SourceCppGen/MethodClosure.cpp
	$(CPP_COMPILE)

./Obj/NamedLambda.o: ../../SourceCppGen/NamedLambda.cpp
	$(CPP_COMPILE)

./Obj/NestedLambda.o: ../../SourceCppGen/NestedLambda.cpp
	$(CPP_COMPILE)

./Obj/NewCustomInterface.o: ../../SourceCppGen/NewCustomInterface.cpp
	$(CPP_COMPILE)

./Obj/NewCustomInterface2.o: ../../SourceCppGen/NewCustomInterface2.cpp
	$(CPP_COMPILE)

./Obj/NewCustomInterface3.o: ../../SourceCppGen/NewCustomInterface3.cpp
	$(CPP_COMPILE)

./Obj/NewCustomInterfaceDtor.o: ../../SourceCppGen/NewCustomInterfaceDtor.cpp
	$(CPP_COMPILE)

./Obj/NewCustomInterfaceDtor2.o: ../../SourceCppGen/NewCustomInterfaceDtor2.cpp
	$(CPP_COMPILE)

./Obj/NewCustomInterfaceDtor3.o: ../../SourceCppGen/NewCustomInterfaceDtor3.cpp
	$(CPP_COMPILE)

./Obj/NewEmptyInterface.o: ../../SourceCppGen/NewEmptyInterface.cpp
	$(CPP_COMPILE)

./Obj/NewEmptyInterfaceDtor.o: ../../SourceCppGen/NewEmptyInterfaceDtor.cpp
	$(CPP_COMPILE)

./Obj/NewInterface.o: ../../SourceCppGen/NewInterface.cpp
	$(CPP_COMPILE)

./Obj/NewInterfaceDtor.o: ../../SourceCppGen/NewInterfaceDtor.cpp
	$(CPP_COMPILE)

./Obj/NullableCast.o: ../../SourceCppGen/NullableCast.cpp
	$(CPP_COMPILE)

./Obj/ObservableList.o: ../../SourceCppGen/ObservableList.cpp
	$(CPP_COMPILE)

./Obj/OpAdd.o: ../../SourceCppGen/OpAdd.cpp
	$(CPP_COMPILE)

./Obj/OpAnd.o: ../../SourceCppGen/OpAnd.cpp
	$(CPP_COMPILE)

./Obj/OpCompare.o: ../../SourceCppGen/OpCompare.cpp
	$(CPP_COMPILE)

./Obj/OpCompareIndexOf.o: ../../SourceCppGen/OpCompareIndexOf.cpp
	$(CPP_COMPILE)

./Obj/OpCompareReference.o: ../../SourceCppGen/OpCompareReference.cpp
	$(CPP_COMPILE)

./Obj/OpCompareStruct1.o: ../../SourceCppGen/OpCompareStruct1.cpp
	$(CPP_COMPILE)

./Obj/OpCompareStruct2.o: ../../SourceCppGen/OpCompareStruct2.cpp
	$(CPP_COMPILE)

./Obj/OpCompareWithString.o: ../../SourceCppGen/OpCompareWithString.cpp
	$(CPP_COMPILE)

./Obj/OpDiv.o: ../../SourceCppGen/OpDiv.cpp
	$(CPP_COMPILE)

./Obj/OpMod.o: ../../SourceCppGen/OpMod.cpp
	$(CPP_COMPILE)

./Obj/OpMul.o: ../../SourceCppGen/OpMul.cpp
	$(CPP_COMPILE)

./Obj/OpNegative.o: ../../SourceCppGen/OpNegative.cpp
	$(CPP_COMPILE)

./Obj/OpNot.o: ../../SourceCppGen/OpNot.cpp
	$(CPP_COMPILE)

./Obj/OpOr.o: ../../SourceCppGen/OpOr.cpp
	$(CPP_COMPILE)

./Obj/OpPositive.o: ../../SourceCppGen/OpPositive.cpp
	$(CPP_COMPILE)

./Obj/OpShl.o: ../../SourceCppGen/OpShl.cpp
	$(CPP_COMPILE)

./Obj/OpShr.o: ../../SourceCppGen/OpShr.cpp
	$(CPP_COMPILE)

./Obj/OpSub.o: ../../SourceCppGen/OpSub.cpp
	$(CPP_COMPILE)

./Obj/OpXor.o: ../../SourceCppGen/OpXor.cpp
	$(CPP_COMPILE)

./Obj/OrderedLambda.o: ../../SourceCppGen/OrderedLambda.cpp
	$(CPP_COMPILE)

./Obj/Overloading.o: ../../SourceCppGen/Overloading.cpp
	$(CPP_COMPILE)

./Obj/Property.o: ../../SourceCppGen/Property.cpp
	$(CPP_COMPILE)

./Obj/RecursiveFunction.o: ../../SourceCppGen/RecursiveFunction.cpp
	$(CPP_COMPILE)

./Obj/StaticInit.o: ../../SourceCppGen/StaticInit.cpp
	$(CPP_COMPILE)

./Obj/StaticMethod.o: ../../SourceCppGen/StaticMethod.cpp
	$(CPP_COMPILE)

./Obj/StructCtor.o: ../../SourceCppGen/StructCtor.cpp
	$(CPP_COMPILE)

./Obj/StructCtor2.o: ../../SourceCppGen/StructCtor2.cpp
	$(CPP_COMPILE)

./Obj/TryCatch.o: ../../SourceCppGen/TryCatch.cpp
	$(CPP_COMPILE)

./Obj/TryFinally.o: ../../SourceCppGen/TryFinally.cpp
	$(CPP_COMPILE)

./Obj/TryFinally2.o: ../../SourceCppGen/TryFinally2.cpp
	$(CPP_COMPILE)

./Obj/WorkflowAttributes.o: ../../SourceCppGen/WorkflowAttributes.cpp
	$(CPP_COMPILE)

./Obj/WorkflowAttributes_Expression.o: ../../SourceCppGen/WorkflowAttributes_Expression.cpp
	$(CPP_COMPILE)

./Obj/WorkflowHints.o: ../../SourceCppGen/WorkflowHints.cpp
	$(CPP_COMPILE)

./Obj/WorkflowMultiFiles.o: ../../SourceCppGen/WorkflowMultiFiles.cpp
	$(CPP_COMPILE)

./Obj/WorkflowMultiFiles_Animal.o: ../../SourceCppGen/WorkflowMultiFiles_Animal.cpp
	$(CPP_COMPILE)

./Obj/WorkflowMultiFiles_Bird.o: ../../SourceCppGen/WorkflowMultiFiles_Bird.cpp
	$(CPP_COMPILE)

./Obj/WorkflowMultiFiles_Lion.o: ../../SourceCppGen/WorkflowMultiFiles_Lion.cpp
	$(CPP_COMPILE)

./Obj/Event2Reflection.o: ../../SourceCppGen/Event2Reflection.cpp
	$(CPP_COMPILE)

./Obj/OpCompareIndexOfReflection.o: ../../SourceCppGen/OpCompareIndexOfReflection.cpp
	$(CPP_COMPILE)

./Obj/OpCompareStruct2Reflection.o: ../../SourceCppGen/OpCompareStruct2Reflection.cpp
	$(CPP_COMPILE)

./Obj/WorkflowAttributesReflection.o: ../../SourceCppGen/WorkflowAttributesReflection.cpp
	$(CPP_COMPILE)

./Obj/WorkflowMultiFilesReflection.o: ../../SourceCppGen/WorkflowMultiFilesReflection.cpp
	$(CPP_COMPILE)

./Obj/BindCustomInterface2Reflection.o: ../../SourceCppGen/BindCustomInterface2Reflection.cpp
	$(CPP_COMPILE)

./Obj/BindCustomInterfaceReflection.o: ../../SourceCppGen/BindCustomInterfaceReflection.cpp
	$(CPP_COMPILE)

./Obj/BindLetReflection.o: ../../SourceCppGen/BindLetReflection.cpp
	$(CPP_COMPILE)

./Obj/BindNullReflection.o: ../../SourceCppGen/BindNullReflection.cpp
	$(CPP_COMPILE)

./Obj/BindSimple2Reflection.o: ../../SourceCppGen/BindSimple2Reflection.cpp
	$(CPP_COMPILE)

./Obj/BindSimpleReflection.o: ../../SourceCppGen/BindSimpleReflection.cpp
	$(CPP_COMPILE)

./Obj/CallStaticMethodReflection.o: ../../SourceCppGen/CallStaticMethodReflection.cpp
	$(CPP_COMPILE)

./Obj/ClassCtorReflection.o: ../../SourceCppGen/ClassCtorReflection.cpp
	$(CPP_COMPILE)

./Obj/ClassDtorReflection.o: ../../SourceCppGen/ClassDtorReflection.cpp
	$(CPP_COMPILE)

./Obj/ClassMethodReflection.o: ../../SourceCppGen/ClassMethodReflection.cpp
	$(CPP_COMPILE)

./Obj/CoAsync2Reflection.o: ../../SourceCppGen/CoAsync2Reflection.cpp
	$(CPP_COMPILE)

./Obj/CoAsyncReflection.o: ../../SourceCppGen/CoAsyncReflection.cpp
	$(CPP_COMPILE)

./Obj/CoSmcCalculatorReflection.o: ../../SourceCppGen/CoSmcCalculatorReflection.cpp
	$(CPP_COMPILE)

./Obj/CoSmcGotoReflection.o: ../../SourceCppGen/CoSmcGotoReflection.cpp
	$(CPP_COMPILE)

./Obj/CoSmcPushReflection.o: ../../SourceCppGen/CoSmcPushReflection.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchD1Reflection.o: ../../SourceCppGen/CoSmcSwitchD1Reflection.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchD2Reflection.o: ../../SourceCppGen/CoSmcSwitchD2Reflection.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchI1Reflection.o: ../../SourceCppGen/CoSmcSwitchI1Reflection.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchI2Reflection.o: ../../SourceCppGen/CoSmcSwitchI2Reflection.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchIR1Reflection.o: ../../SourceCppGen/CoSmcSwitchIR1Reflection.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchIR2Reflection.o: ../../SourceCppGen/CoSmcSwitchIR2Reflection.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchP1Reflection.o: ../../SourceCppGen/CoSmcSwitchP1Reflection.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchP2Reflection.o: ../../SourceCppGen/CoSmcSwitchP2Reflection.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchPR1Reflection.o: ../../SourceCppGen/CoSmcSwitchPR1Reflection.cpp
	$(CPP_COMPILE)

./Obj/CoSmcSwitchPR2Reflection.o: ../../SourceCppGen/CoSmcSwitchPR2Reflection.cpp
	$(CPP_COMPILE)

./Obj/CoSmcTryCatch1Reflection.o: ../../SourceCppGen/CoSmcTryCatch1Reflection.cpp
	$(CPP_COMPILE)

./Obj/CoSmcTryCatch2Reflection.o: ../../SourceCppGen/CoSmcTryCatch2Reflection.cpp
	$(CPP_COMPILE)

./Obj/EnumCtor2Reflection.o: ../../SourceCppGen/EnumCtor2Reflection.cpp
	$(CPP_COMPILE)

./Obj/NestedLambdaReflection.o: ../../SourceCppGen/NestedLambdaReflection.cpp
	$(CPP_COMPILE)

./Obj/NewCustomInterface2Reflection.o: ../../SourceCppGen/NewCustomInterface2Reflection.cpp
	$(CPP_COMPILE)

./Obj/NewCustomInterface3Reflection.o: ../../SourceCppGen/NewCustomInterface3Reflection.cpp
	$(CPP_COMPILE)

./Obj/NewCustomInterfaceDtor2Reflection.o: ../../SourceCppGen/NewCustomInterfaceDtor2Reflection.cpp
	$(CPP_COMPILE)

./Obj/NewCustomInterfaceDtor3Reflection.o: ../../SourceCppGen/NewCustomInterfaceDtor3Reflection.cpp
	$(CPP_COMPILE)

./Obj/NewCustomInterfaceDtorReflection.o: ../../SourceCppGen/NewCustomInterfaceDtorReflection.cpp
	$(CPP_COMPILE)

./Obj/NewCustomInterfaceReflection.o: ../../SourceCppGen/NewCustomInterfaceReflection.cpp
	$(CPP_COMPILE)

./Obj/NewEmptyInterfaceDtorReflection.o: ../../SourceCppGen/NewEmptyInterfaceDtorReflection.cpp
	$(CPP_COMPILE)

./Obj/NewEmptyInterfaceReflection.o: ../../SourceCppGen/NewEmptyInterfaceReflection.cpp
	$(CPP_COMPILE)

./Obj/NewInterfaceDtorReflection.o: ../../SourceCppGen/NewInterfaceDtorReflection.cpp
	$(CPP_COMPILE)

./Obj/NullableCastReflection.o: ../../SourceCppGen/NullableCastReflection.cpp
	$(CPP_COMPILE)

./Obj/OpCompareReflection.o: ../../SourceCppGen/OpCompareReflection.cpp
	$(CPP_COMPILE)

./Obj/StructCtor2Reflection.o: ../../SourceCppGen/StructCtor2Reflection.cpp
	$(CPP_COMPILE)

./Obj/StructCtorReflection.o: ../../SourceCppGen/StructCtorReflection.cpp
	$(CPP_COMPILE)

./Obj/WorkflowHintsReflection.o: ../../SourceCppGen/WorkflowHintsReflection.cpp
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

./Obj/TestCases.o: ../../SourceCppGen/TestCases.cpp
	$(CPP_COMPILE)

./Obj/TestCasesRpc.o: ../../SourceCppGenRpc/TestCasesRpc.cpp
	$(CPP_COMPILE)

./Obj/GetExePath.o: ../../Source/GetExePath.cpp
	$(CPP_COMPILE)

./Obj/RpcDualJsonDispatcherMock.o: ../../Source/RpcDualJsonDispatcherMock.cpp
	$(CPP_COMPILE)

./Obj/RpcDualLifecycleMock.o: ../../Source/RpcDualLifecycleMock.cpp
	$(CPP_COMPILE)

./Obj/Main.o: ../../UnitTest/CppTest_Metaonly/Main.cpp
	$(CPP_COMPILE)
