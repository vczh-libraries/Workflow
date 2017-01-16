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

./Bin/UnitTest:./Obj/Vlpp.o ./Obj/AnonymousLambda.o ./Obj/BindComplex.o ./Obj/BindCustomInterface.o ./Obj/BindCustomInterface2.o ./Obj/BindFormat.o ./Obj/BindSimple.o ./Obj/CallStaticMethod.o ./Obj/ClassCtor.o ./Obj/ClassDtor.o ./Obj/ClassField.o ./Obj/ClassMethod.o ./Obj/ControlFlow.o ./Obj/Delete.o ./Obj/ElementInSet.o ./Obj/EnumCtor.o ./Obj/EnumCtor2.o ./Obj/Event.o ./Obj/FailedThen.o ./Obj/ForEach.o ./Obj/GlobalVariable.o ./Obj/HelloWorld.o ./Obj/IfNotNull.o ./Obj/LetIn.o ./Obj/ListProcessing.o ./Obj/MapProcessing.o ./Obj/MethodClosure.o ./Obj/NamedLambda.o ./Obj/NestedLambda.o ./Obj/NewCustomInterface.o ./Obj/NewCustomInterface2.o ./Obj/NewCustomInterface3.o ./Obj/NewInterface.o ./Obj/OpAdd.o ./Obj/OpAnd.o ./Obj/OpCompare.o ./Obj/OpCompareReference.o ./Obj/OpDiv.o ./Obj/OpMod.o ./Obj/OpMul.o ./Obj/OpNegative.o ./Obj/OpNot.o ./Obj/OpOr.o ./Obj/OpPositive.o ./Obj/OpShl.o ./Obj/OpShr.o ./Obj/OpSub.o ./Obj/OpXor.o ./Obj/OrderedLambda.o ./Obj/Property.o ./Obj/RecursiveFunction.o ./Obj/StaticMethod.o ./Obj/StructCtor.o ./Obj/StructCtor2.o ./Obj/TestCases.o ./Obj/TryCatch.o ./Obj/TryFinally.o ./Obj/CppTypes.o ./Obj/Main.o
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

./Obj/BindFormat.o: ../SourceCppGen/BindFormat.cpp ../SourceCppGen/BindFormat.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/BindSimple.o: ../SourceCppGen/BindSimple.cpp ../SourceCppGen/BindSimple.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/CallStaticMethod.o: ../SourceCppGen/CallStaticMethod.cpp ../SourceCppGen/CallStaticMethod.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/ClassCtor.o: ../SourceCppGen/ClassCtor.cpp ../SourceCppGen/ClassCtor.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/ClassDtor.o: ../SourceCppGen/ClassDtor.cpp ../SourceCppGen/ClassDtor.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/ClassField.o: ../SourceCppGen/ClassField.cpp ../SourceCppGen/ClassField.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/ClassMethod.o: ../SourceCppGen/ClassMethod.cpp ../SourceCppGen/ClassMethod.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/ControlFlow.o: ../SourceCppGen/ControlFlow.cpp ../SourceCppGen/ControlFlow.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/Delete.o: ../SourceCppGen/Delete.cpp ../SourceCppGen/Delete.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/ElementInSet.o: ../SourceCppGen/ElementInSet.cpp ../SourceCppGen/ElementInSet.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/EnumCtor.o: ../SourceCppGen/EnumCtor.cpp ../SourceCppGen/EnumCtor.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/EnumCtor2.o: ../SourceCppGen/EnumCtor2.cpp ../SourceCppGen/EnumCtor2.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
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

./Obj/NewCustomInterface.o: ../SourceCppGen/NewCustomInterface.cpp ../SourceCppGen/NewCustomInterface.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/NewCustomInterface2.o: ../SourceCppGen/NewCustomInterface2.cpp ../SourceCppGen/NewCustomInterface2.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/NewCustomInterface3.o: ../SourceCppGen/NewCustomInterface3.cpp ../SourceCppGen/NewCustomInterface3.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/NewInterface.o: ../SourceCppGen/NewInterface.cpp ../SourceCppGen/NewInterface.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpAdd.o: ../SourceCppGen/OpAdd.cpp ../SourceCppGen/OpAdd.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpAnd.o: ../SourceCppGen/OpAnd.cpp ../SourceCppGen/OpAnd.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpCompare.o: ../SourceCppGen/OpCompare.cpp ../SourceCppGen/OpCompare.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/OpCompareReference.o: ../SourceCppGen/OpCompareReference.cpp ../SourceCppGen/OpCompareReference.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
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

./Obj/TestCases.o: ../SourceCppGen/TestCases.cpp ../SourceCppGen/HelloWorld.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h ../SourceCppGen/OpNot.h ../SourceCppGen/OpPositive.h ../SourceCppGen/OpNegative.h ../SourceCppGen/OpAdd.h ../SourceCppGen/OpSub.h ../SourceCppGen/OpMul.h ../SourceCppGen/OpDiv.h ../SourceCppGen/OpMod.h ../SourceCppGen/OpShl.h ../SourceCppGen/OpShr.h ../SourceCppGen/OpAnd.h ../SourceCppGen/OpOr.h ../SourceCppGen/OpXor.h ../SourceCppGen/OpCompare.h ../SourceCppGen/OpCompareReference.h ../SourceCppGen/ListProcessing.h ../SourceCppGen/MapProcessing.h ../SourceCppGen/ElementInSet.h ../SourceCppGen/GlobalVariable.h ../SourceCppGen/RecursiveFunction.h ../SourceCppGen/LetIn.h ../SourceCppGen/Property.h ../SourceCppGen/MethodClosure.h ../SourceCppGen/StaticMethod.h ../SourceCppGen/Event.h ../SourceCppGen/ControlFlow.h ../SourceCppGen/IfNotNull.h ../SourceCppGen/ForEach.h ../SourceCppGen/AnonymousLambda.h ../SourceCppGen/NamedLambda.h ../SourceCppGen/OrderedLambda.h ../SourceCppGen/TryFinally.h ../SourceCppGen/TryCatch.h ../SourceCppGen/Delete.h ../SourceCppGen/FailedThen.h ../SourceCppGen/NewInterface.h ../SourceCppGen/NewCustomInterface.h ../SourceCppGen/NewCustomInterface2.h ../SourceCppGen/NewCustomInterface3.h ../SourceCppGen/BindSimple.h ../SourceCppGen/BindComplex.h ../SourceCppGen/BindFormat.h ../SourceCppGen/BindCustomInterface.h ../SourceCppGen/BindCustomInterface2.h ../SourceCppGen/ClassField.h ../SourceCppGen/CallStaticMethod.h ../SourceCppGen/NestedLambda.h ../SourceCppGen/ClassMethod.h ../SourceCppGen/ClassCtor.h ../SourceCppGen/ClassDtor.h ../SourceCppGen/StructCtor.h ../SourceCppGen/StructCtor2.h ../SourceCppGen/EnumCtor.h ../SourceCppGen/EnumCtor2.h
	$(CPP_COMPILE)

./Obj/TryCatch.o: ../SourceCppGen/TryCatch.cpp ../SourceCppGen/TryCatch.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/TryFinally.o: ../SourceCppGen/TryFinally.cpp ../SourceCppGen/TryFinally.h ../SourceCppGen/../Source/CppTypes.h ../SourceCppGen/../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/CppTypes.o: ../Source/CppTypes.cpp ../Source/CppTypes.h ../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/Main.o: ../UnitTest/UnitTest_CppCodegen/Main.cpp ../UnitTest/UnitTest_CppCodegen/../../../Import/Vlpp.h ../UnitTest/UnitTest_CppCodegen/../../Source/CppTypes.h ../UnitTest/UnitTest_CppCodegen/../../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)
