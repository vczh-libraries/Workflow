.PHONY: all clean pre-build
.DEFAULT_GOAL := all

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

./Bin/UnitTest:./Obj/Vlpp.o ./Obj/AnonymousLambda.o ./Obj/BindComplex.o ./Obj/BindCustomInterface.o ./Obj/BindCustomInterface2.o ./Obj/BindCustomInterface2Reflection.o ./Obj/BindCustomInterfaceReflection.o ./Obj/BindFormat.o ./Obj/BindLet.o ./Obj/BindLetReflection.o ./Obj/BindSimple.o ./Obj/BindSimpleReflection.o ./Obj/CallStaticMethod.o ./Obj/CallStaticMethodReflection.o ./Obj/ClassCtor.o ./Obj/ClassCtorReflection.o ./Obj/ClassDtor.o ./Obj/ClassDtorReflection.o ./Obj/ClassField.o ./Obj/ClassMethod.o ./Obj/ClassMethodReflection.o ./Obj/CoAsync.o ./Obj/CoAsyncReflection.o ./Obj/CoEnum.o ./Obj/CoEnum2.o ./Obj/ControlFlow.o ./Obj/CoRawCoroutine.o ./Obj/CoRawCoroutine2.o ./Obj/CoRawCoroutine3.o ./Obj/CoRawCoroutine4.o ./Obj/Delete.o ./Obj/ElementInSet.o ./Obj/EnumCtor.o ./Obj/EnumCtor2.o ./Obj/EnumCtor2Reflection.o ./Obj/Event.o ./Obj/FailedThen.o ./Obj/ForEach.o ./Obj/GlobalVariable.o ./Obj/HelloWorld.o ./Obj/IfNotNull.o ./Obj/LetIn.o ./Obj/ListProcessing.o ./Obj/MapProcessing.o ./Obj/MethodClosure.o ./Obj/NamedLambda.o ./Obj/NestedLambda.o ./Obj/NestedLambdaReflection.o ./Obj/NewCustomInterface.o ./Obj/NewCustomInterface2.o ./Obj/NewCustomInterface2Reflection.o ./Obj/NewCustomInterface3.o ./Obj/NewCustomInterface3Reflection.o ./Obj/NewCustomInterfaceReflection.o ./Obj/NewInterface.o ./Obj/ObservableList.o ./Obj/OpAdd.o ./Obj/OpAnd.o ./Obj/OpCompare.o ./Obj/OpCompareReference.o ./Obj/OpCompareReflection.o ./Obj/OpDiv.o ./Obj/OpMod.o ./Obj/OpMul.o ./Obj/OpNegative.o ./Obj/OpNot.o ./Obj/OpOr.o ./Obj/OpPositive.o ./Obj/OpShl.o ./Obj/OpShr.o ./Obj/OpSub.o ./Obj/OpXor.o ./Obj/OrderedLambda.o ./Obj/Property.o ./Obj/RecursiveFunction.o ./Obj/StaticMethod.o ./Obj/StructCtor.o ./Obj/StructCtor2.o ./Obj/StructCtor2Reflection.o ./Obj/TestCases.o ./Obj/TryCatch.o ./Obj/TryFinally.o ./Obj/WorkflowAttributes.o ./Obj/WorkflowAttributesReflection.o ./Obj/WorkflowAttributes_BinaryExpression.o ./Obj/WorkflowAttributes_Expression.o ./Obj/WorkflowAttributes_NumberExpression.o ./Obj/WorkflowHints.o ./Obj/WorkflowHintsReflection.o ./Obj/CppTypes.o ./Obj/Main.o
	$(CPP_LINK)

./Obj/Vlpp.o: ../../Import/Vlpp.cpp ../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/AnonymousLambda.o: ../SourceCppGen/AnonymousLambda.cpp ../SourceCppGen/AnonymousLambda.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/BindComplex.o: ../SourceCppGen/BindComplex.cpp ../SourceCppGen/BindComplex.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/BindCustomInterface.o: ../SourceCppGen/BindCustomInterface.cpp ../SourceCppGen/BindCustomInterface.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/BindCustomInterface2.o: ../SourceCppGen/BindCustomInterface2.cpp ../SourceCppGen/BindCustomInterface2.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/BindCustomInterface2Reflection.o: ../SourceCppGen/BindCustomInterface2Reflection.cpp ../SourceCppGen/BindCustomInterface2Reflection.h ../SourceCppGen/BindCustomInterface2.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/BindCustomInterfaceReflection.o: ../SourceCppGen/BindCustomInterfaceReflection.cpp ../SourceCppGen/BindCustomInterfaceReflection.h ../SourceCppGen/BindCustomInterface.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/BindFormat.o: ../SourceCppGen/BindFormat.cpp ../SourceCppGen/BindFormat.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/BindLet.o: ../SourceCppGen/BindLet.cpp ../SourceCppGen/BindLet.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/BindLetReflection.o: ../SourceCppGen/BindLetReflection.cpp ../SourceCppGen/BindLetReflection.h ../SourceCppGen/BindLet.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/BindSimple.o: ../SourceCppGen/BindSimple.cpp ../SourceCppGen/BindSimple.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/BindSimpleReflection.o: ../SourceCppGen/BindSimpleReflection.cpp ../SourceCppGen/BindSimpleReflection.h ../SourceCppGen/BindSimple.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/CallStaticMethod.o: ../SourceCppGen/CallStaticMethod.cpp ../SourceCppGen/CallStaticMethod.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/CallStaticMethodReflection.o: ../SourceCppGen/CallStaticMethodReflection.cpp ../SourceCppGen/CallStaticMethodReflection.h ../SourceCppGen/CallStaticMethod.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/ClassCtor.o: ../SourceCppGen/ClassCtor.cpp ../SourceCppGen/ClassCtor.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/ClassCtorReflection.o: ../SourceCppGen/ClassCtorReflection.cpp ../SourceCppGen/ClassCtorReflection.h ../SourceCppGen/ClassCtor.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/ClassDtor.o: ../SourceCppGen/ClassDtor.cpp ../SourceCppGen/ClassDtor.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/ClassDtorReflection.o: ../SourceCppGen/ClassDtorReflection.cpp ../SourceCppGen/ClassDtorReflection.h ../SourceCppGen/ClassDtor.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/ClassField.o: ../SourceCppGen/ClassField.cpp ../SourceCppGen/ClassField.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/ClassMethod.o: ../SourceCppGen/ClassMethod.cpp ../SourceCppGen/ClassMethod.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/ClassMethodReflection.o: ../SourceCppGen/ClassMethodReflection.cpp ../SourceCppGen/ClassMethodReflection.h ../SourceCppGen/ClassMethod.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/CoAsync.o: ../SourceCppGen/CoAsync.cpp ../SourceCppGen/CoAsync.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/CoAsyncReflection.o: ../SourceCppGen/CoAsyncReflection.cpp ../SourceCppGen/CoAsyncReflection.h ../SourceCppGen/CoAsync.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/CoEnum.o: ../SourceCppGen/CoEnum.cpp ../SourceCppGen/CoEnum.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/CoEnum2.o: ../SourceCppGen/CoEnum2.cpp ../SourceCppGen/CoEnum2.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/ControlFlow.o: ../SourceCppGen/ControlFlow.cpp ../SourceCppGen/ControlFlow.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/CoRawCoroutine.o: ../SourceCppGen/CoRawCoroutine.cpp ../SourceCppGen/CoRawCoroutine.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/CoRawCoroutine2.o: ../SourceCppGen/CoRawCoroutine2.cpp ../SourceCppGen/CoRawCoroutine2.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/CoRawCoroutine3.o: ../SourceCppGen/CoRawCoroutine3.cpp ../SourceCppGen/CoRawCoroutine3.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/CoRawCoroutine4.o: ../SourceCppGen/CoRawCoroutine4.cpp ../SourceCppGen/CoRawCoroutine4.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/Delete.o: ../SourceCppGen/Delete.cpp ../SourceCppGen/Delete.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/ElementInSet.o: ../SourceCppGen/ElementInSet.cpp ../SourceCppGen/ElementInSet.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/EnumCtor.o: ../SourceCppGen/EnumCtor.cpp ../SourceCppGen/EnumCtor.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/EnumCtor2.o: ../SourceCppGen/EnumCtor2.cpp ../SourceCppGen/EnumCtor2.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/EnumCtor2Reflection.o: ../SourceCppGen/EnumCtor2Reflection.cpp ../SourceCppGen/EnumCtor2Reflection.h ../SourceCppGen/EnumCtor2.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/Event.o: ../SourceCppGen/Event.cpp ../SourceCppGen/Event.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/FailedThen.o: ../SourceCppGen/FailedThen.cpp ../SourceCppGen/FailedThen.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/ForEach.o: ../SourceCppGen/ForEach.cpp ../SourceCppGen/ForEach.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/GlobalVariable.o: ../SourceCppGen/GlobalVariable.cpp ../SourceCppGen/GlobalVariable.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/HelloWorld.o: ../SourceCppGen/HelloWorld.cpp ../SourceCppGen/HelloWorld.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/IfNotNull.o: ../SourceCppGen/IfNotNull.cpp ../SourceCppGen/IfNotNull.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/LetIn.o: ../SourceCppGen/LetIn.cpp ../SourceCppGen/LetIn.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/ListProcessing.o: ../SourceCppGen/ListProcessing.cpp ../SourceCppGen/ListProcessing.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/MapProcessing.o: ../SourceCppGen/MapProcessing.cpp ../SourceCppGen/MapProcessing.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/MethodClosure.o: ../SourceCppGen/MethodClosure.cpp ../SourceCppGen/MethodClosure.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/NamedLambda.o: ../SourceCppGen/NamedLambda.cpp ../SourceCppGen/NamedLambda.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/NestedLambda.o: ../SourceCppGen/NestedLambda.cpp ../SourceCppGen/NestedLambda.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/NestedLambdaReflection.o: ../SourceCppGen/NestedLambdaReflection.cpp ../SourceCppGen/NestedLambdaReflection.h ../SourceCppGen/NestedLambda.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/NewCustomInterface.o: ../SourceCppGen/NewCustomInterface.cpp ../SourceCppGen/NewCustomInterface.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/NewCustomInterface2.o: ../SourceCppGen/NewCustomInterface2.cpp ../SourceCppGen/NewCustomInterface2.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/NewCustomInterface2Reflection.o: ../SourceCppGen/NewCustomInterface2Reflection.cpp ../SourceCppGen/NewCustomInterface2Reflection.h ../SourceCppGen/NewCustomInterface2.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/NewCustomInterface3.o: ../SourceCppGen/NewCustomInterface3.cpp ../SourceCppGen/NewCustomInterface3.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/NewCustomInterface3Reflection.o: ../SourceCppGen/NewCustomInterface3Reflection.cpp ../SourceCppGen/NewCustomInterface3Reflection.h ../SourceCppGen/NewCustomInterface3.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/NewCustomInterfaceReflection.o: ../SourceCppGen/NewCustomInterfaceReflection.cpp ../SourceCppGen/NewCustomInterfaceReflection.h ../SourceCppGen/NewCustomInterface.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/NewInterface.o: ../SourceCppGen/NewInterface.cpp ../SourceCppGen/NewInterface.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/ObservableList.o: ../SourceCppGen/ObservableList.cpp ../SourceCppGen/ObservableList.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpAdd.o: ../SourceCppGen/OpAdd.cpp ../SourceCppGen/OpAdd.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpAnd.o: ../SourceCppGen/OpAnd.cpp ../SourceCppGen/OpAnd.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpCompare.o: ../SourceCppGen/OpCompare.cpp ../SourceCppGen/OpCompare.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpCompareReference.o: ../SourceCppGen/OpCompareReference.cpp ../SourceCppGen/OpCompareReference.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpCompareReflection.o: ../SourceCppGen/OpCompareReflection.cpp ../SourceCppGen/OpCompareReflection.h ../SourceCppGen/OpCompare.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpDiv.o: ../SourceCppGen/OpDiv.cpp ../SourceCppGen/OpDiv.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpMod.o: ../SourceCppGen/OpMod.cpp ../SourceCppGen/OpMod.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpMul.o: ../SourceCppGen/OpMul.cpp ../SourceCppGen/OpMul.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpNegative.o: ../SourceCppGen/OpNegative.cpp ../SourceCppGen/OpNegative.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpNot.o: ../SourceCppGen/OpNot.cpp ../SourceCppGen/OpNot.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpOr.o: ../SourceCppGen/OpOr.cpp ../SourceCppGen/OpOr.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpPositive.o: ../SourceCppGen/OpPositive.cpp ../SourceCppGen/OpPositive.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpShl.o: ../SourceCppGen/OpShl.cpp ../SourceCppGen/OpShl.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpShr.o: ../SourceCppGen/OpShr.cpp ../SourceCppGen/OpShr.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpSub.o: ../SourceCppGen/OpSub.cpp ../SourceCppGen/OpSub.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpXor.o: ../SourceCppGen/OpXor.cpp ../SourceCppGen/OpXor.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OrderedLambda.o: ../SourceCppGen/OrderedLambda.cpp ../SourceCppGen/OrderedLambda.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/Property.o: ../SourceCppGen/Property.cpp ../SourceCppGen/Property.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/RecursiveFunction.o: ../SourceCppGen/RecursiveFunction.cpp ../SourceCppGen/RecursiveFunction.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/StaticMethod.o: ../SourceCppGen/StaticMethod.cpp ../SourceCppGen/StaticMethod.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/StructCtor.o: ../SourceCppGen/StructCtor.cpp ../SourceCppGen/StructCtor.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/StructCtor2.o: ../SourceCppGen/StructCtor2.cpp ../SourceCppGen/StructCtor2.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/StructCtor2Reflection.o: ../SourceCppGen/StructCtor2Reflection.cpp ../SourceCppGen/StructCtor2Reflection.h ../SourceCppGen/StructCtor2.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/TestCases.o: ../SourceCppGen/TestCases.cpp ../SourceCppGen/HelloWorld.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h ../SourceCppGen/OpNot.h ../SourceCppGen/OpPositive.h ../SourceCppGen/OpNegative.h ../SourceCppGen/OpAdd.h ../SourceCppGen/OpSub.h ../SourceCppGen/OpMul.h ../SourceCppGen/OpDiv.h ../SourceCppGen/OpMod.h ../SourceCppGen/OpShl.h ../SourceCppGen/OpShr.h ../SourceCppGen/OpAnd.h ../SourceCppGen/OpOr.h ../SourceCppGen/OpXor.h ../SourceCppGen/OpCompareReflection.h ../SourceCppGen/OpCompare.h ../SourceCppGen/OpCompareReference.h ../SourceCppGen/ListProcessing.h ../SourceCppGen/MapProcessing.h ../SourceCppGen/ObservableList.h ../SourceCppGen/ElementInSet.h ../SourceCppGen/GlobalVariable.h ../SourceCppGen/RecursiveFunction.h ../SourceCppGen/LetIn.h ../SourceCppGen/Property.h ../SourceCppGen/MethodClosure.h ../SourceCppGen/StaticMethod.h ../SourceCppGen/Event.h ../SourceCppGen/ControlFlow.h ../SourceCppGen/IfNotNull.h ../SourceCppGen/ForEach.h ../SourceCppGen/AnonymousLambda.h ../SourceCppGen/NamedLambda.h ../SourceCppGen/OrderedLambda.h ../SourceCppGen/TryFinally.h ../SourceCppGen/TryCatch.h ../SourceCppGen/Delete.h ../SourceCppGen/FailedThen.h ../SourceCppGen/NewInterface.h ../SourceCppGen/NewCustomInterfaceReflection.h ../SourceCppGen/NewCustomInterface.h ../SourceCppGen/NewCustomInterface2Reflection.h ../SourceCppGen/NewCustomInterface2.h ../SourceCppGen/NewCustomInterface3Reflection.h ../SourceCppGen/NewCustomInterface3.h ../SourceCppGen/BindSimpleReflection.h ../SourceCppGen/BindSimple.h ../SourceCppGen/BindLetReflection.h ../SourceCppGen/BindLet.h ../SourceCppGen/BindComplex.h ../SourceCppGen/BindFormat.h ../SourceCppGen/BindCustomInterfaceReflection.h ../SourceCppGen/BindCustomInterface.h ../SourceCppGen/BindCustomInterface2Reflection.h ../SourceCppGen/BindCustomInterface2.h ../SourceCppGen/ClassField.h ../SourceCppGen/CallStaticMethodReflection.h ../SourceCppGen/CallStaticMethod.h ../SourceCppGen/NestedLambdaReflection.h ../SourceCppGen/NestedLambda.h ../SourceCppGen/ClassMethodReflection.h ../SourceCppGen/ClassMethod.h ../SourceCppGen/ClassCtorReflection.h ../SourceCppGen/ClassCtor.h ../SourceCppGen/ClassDtorReflection.h ../SourceCppGen/ClassDtor.h ../SourceCppGen/StructCtor.h ../SourceCppGen/StructCtor2Reflection.h ../SourceCppGen/StructCtor2.h ../SourceCppGen/EnumCtor.h ../SourceCppGen/EnumCtor2Reflection.h ../SourceCppGen/EnumCtor2.h ../SourceCppGen/WorkflowHintsReflection.h ../SourceCppGen/WorkflowHints.h ../SourceCppGen/WorkflowAttributesReflection.h ../SourceCppGen/WorkflowAttributesIncludes.h ../SourceCppGen/WorkflowAttributes.h ../SourceCppGen/WorkflowAttributes_BinaryExpression.h ../SourceCppGen/WorkflowAttributes_Expression.h ../SourceCppGen/WorkflowAttributes_NumberExpression.h ../SourceCppGen/CoRawCoroutine.h ../SourceCppGen/CoRawCoroutine2.h ../SourceCppGen/CoRawCoroutine3.h ../SourceCppGen/CoRawCoroutine4.h ../SourceCppGen/CoEnum.h ../SourceCppGen/CoEnum2.h ../SourceCppGen/CoAsyncReflection.h ../SourceCppGen/CoAsync.h
	$(CPP_COMPILE)

./Obj/TryCatch.o: ../SourceCppGen/TryCatch.cpp ../SourceCppGen/TryCatch.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/TryFinally.o: ../SourceCppGen/TryFinally.cpp ../SourceCppGen/TryFinally.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/WorkflowAttributes.o: ../SourceCppGen/WorkflowAttributes.cpp ../SourceCppGen/WorkflowAttributesIncludes.h ../SourceCppGen/WorkflowAttributes.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h ../SourceCppGen/WorkflowAttributes_BinaryExpression.h ../SourceCppGen/WorkflowAttributes_Expression.h ../SourceCppGen/WorkflowAttributes_NumberExpression.h
	$(CPP_COMPILE)

./Obj/WorkflowAttributesReflection.o: ../SourceCppGen/WorkflowAttributesReflection.cpp ../SourceCppGen/WorkflowAttributesReflection.h ../SourceCppGen/WorkflowAttributesIncludes.h ../SourceCppGen/WorkflowAttributes.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h ../SourceCppGen/WorkflowAttributes_BinaryExpression.h ../SourceCppGen/WorkflowAttributes_Expression.h ../SourceCppGen/WorkflowAttributes_NumberExpression.h
	$(CPP_COMPILE)

./Obj/WorkflowAttributes_BinaryExpression.o: ../SourceCppGen/WorkflowAttributes_BinaryExpression.cpp ../SourceCppGen/WorkflowAttributesIncludes.h ../SourceCppGen/WorkflowAttributes.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h ../SourceCppGen/WorkflowAttributes_BinaryExpression.h ../SourceCppGen/WorkflowAttributes_Expression.h ../SourceCppGen/WorkflowAttributes_NumberExpression.h
	$(CPP_COMPILE)

./Obj/WorkflowAttributes_Expression.o: ../SourceCppGen/WorkflowAttributes_Expression.cpp ../SourceCppGen/WorkflowAttributesIncludes.h ../SourceCppGen/WorkflowAttributes.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h ../SourceCppGen/WorkflowAttributes_BinaryExpression.h ../SourceCppGen/WorkflowAttributes_Expression.h ../SourceCppGen/WorkflowAttributes_NumberExpression.h
	$(CPP_COMPILE)

./Obj/WorkflowAttributes_NumberExpression.o: ../SourceCppGen/WorkflowAttributes_NumberExpression.cpp ../SourceCppGen/WorkflowAttributesIncludes.h ../SourceCppGen/WorkflowAttributes.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h ../SourceCppGen/WorkflowAttributes_BinaryExpression.h ../SourceCppGen/WorkflowAttributes_Expression.h ../SourceCppGen/WorkflowAttributes_NumberExpression.h
	$(CPP_COMPILE)

./Obj/WorkflowHints.o: ../SourceCppGen/WorkflowHints.cpp ../SourceCppGen/WorkflowHints.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/WorkflowHintsReflection.o: ../SourceCppGen/WorkflowHintsReflection.cpp ../SourceCppGen/WorkflowHintsReflection.h ../SourceCppGen/WorkflowHints.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/CppTypes.o: ../Source/CppTypes.cpp ../Source/CppTypes.h ../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/Main.o: ../UnitTest/UnitTest_CppCodegen/Main.cpp ../UnitTest/UnitTest_CppCodegen/../../../Import/Vlpp.h ../UnitTest/UnitTest_CppCodegen/../../Source/CppTypes.h ../UnitTest/UnitTest_CppCodegen/../../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)
