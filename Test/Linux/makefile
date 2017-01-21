.PHONY: all clean pre-build
.DEFAULT_GOAL := all

include $(VCPROOT)/vl/makefile-cpp

pre-build:
	if ! [ -d ./Bin ]; then mkdir ./Bin; fi
	if ! [ -d ./Obj ]; then mkdir ./Obj; fi
	if ! [ -d ./Coverage ]; then mkdir ./Coverage; fi
	if ! [ -d ../Output ]; then mkdir ../Output; fi

clean:
	if [ -d ./Bin ]; then rm -r ./Bin; fi
	if [ -d ./Obj ]; then rm -r ./Obj; fi
	if [ -d ./Coverage ]; then rm -r ./Coverage; fi
	if [ -d ./../Output ]; then rm -r ../Output; fi

all:pre-build ./Bin/UnitTest

./Bin/UnitTest:./Obj/Vlpp.o ./Obj/WfAnalyzer.o ./Obj/WfAnalyzer_BuildGlobalName.o ./Obj/WfAnalyzer_BuildScope.o ./Obj/WfAnalyzer_CompleteScope.o ./Obj/WfAnalyzer_Errors.o ./Obj/WfAnalyzer_GenerateAssembly.o ./Obj/WfAnalyzer_GenerateBind.o ./Obj/WfAnalyzer_GenerateDeclaration.o ./Obj/WfAnalyzer_GenerateExpression.o ./Obj/WfAnalyzer_GenerateMetadata.o ./Obj/WfAnalyzer_GenerateStatement.o ./Obj/WfAnalyzer_Misc.o ./Obj/WfAnalyzer_SearchOrderedName.o ./Obj/WfAnalyzer_TypeInfo.o ./Obj/WfAnalyzer_ValidateSemantic.o ./Obj/WfAnalyzer_ValidateStructure.o ./Obj/WfCpp.o ./Obj/WfCpp_Class.o ./Obj/WfCpp_Collect.o ./Obj/WfCpp_Expression.o ./Obj/WfCpp_Statement.o ./Obj/WfCpp_WriteClass.o ./Obj/WfCpp_WriteEnum.o ./Obj/WfCpp_WriteFunctionHeader.o ./Obj/WfCpp_WriteGlobal.o ./Obj/WfCpp_WriteLambda.o ./Obj/WfCpp_WriteReflection.o ./Obj/WfCpp_WriteStruct.o ./Obj/WfExpression.o ./Obj/WfExpressionParser.o ./Obj/WfRuntime.o ./Obj/WfRuntimeConstructions.o ./Obj/WfRuntimeInstruction.o ./Obj/WfRuntimeAssembly.o ./Obj/WfRuntimeDebugger.o ./Obj/WfRuntimeExecution.o ./Obj/WfRuntimeTypeDescriptor.o ./Obj/CppTypes.o ./Obj/Helper.o ./Obj/TestAnalyzer.o ./Obj/TestCodegen.o ./Obj/TestDebugger.o ./Obj/TestSamples.o ./Obj/Main.o
	$(CPP_LINK)

./Obj/Vlpp.o: ../../Import/Vlpp.cpp ../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/WfAnalyzer.o: ../../Source/Analyzer/WfAnalyzer.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfAnalyzer_BuildGlobalName.o: ../../Source/Analyzer/WfAnalyzer_BuildGlobalName.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfAnalyzer_BuildScope.o: ../../Source/Analyzer/WfAnalyzer_BuildScope.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfAnalyzer_CompleteScope.o: ../../Source/Analyzer/WfAnalyzer_CompleteScope.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfAnalyzer_Errors.o: ../../Source/Analyzer/WfAnalyzer_Errors.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfAnalyzer_GenerateAssembly.o: ../../Source/Analyzer/WfAnalyzer_GenerateAssembly.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfAnalyzer_GenerateBind.o: ../../Source/Analyzer/WfAnalyzer_GenerateBind.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfAnalyzer_GenerateDeclaration.o: ../../Source/Analyzer/WfAnalyzer_GenerateDeclaration.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfAnalyzer_GenerateExpression.o: ../../Source/Analyzer/WfAnalyzer_GenerateExpression.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfAnalyzer_GenerateMetadata.o: ../../Source/Analyzer/WfAnalyzer_GenerateMetadata.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfAnalyzer_GenerateStatement.o: ../../Source/Analyzer/WfAnalyzer_GenerateStatement.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfAnalyzer_Misc.o: ../../Source/Analyzer/WfAnalyzer_Misc.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfAnalyzer_SearchOrderedName.o: ../../Source/Analyzer/WfAnalyzer_SearchOrderedName.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfAnalyzer_TypeInfo.o: ../../Source/Analyzer/WfAnalyzer_TypeInfo.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ValidateSemantic.o: ../../Source/Analyzer/WfAnalyzer_ValidateSemantic.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ValidateStructure.o: ../../Source/Analyzer/WfAnalyzer_ValidateStructure.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfCpp.o: ../../Source/Cpp/WfCpp.cpp ../../Source/Cpp/WfCpp.h ../../Source/Cpp/../Analyzer/WfAnalyzer.h ../../Source/Cpp/../Analyzer/../Expression/WfExpression.h ../../Source/Cpp/../Analyzer/../Expression/WfExpressionParser.h ../../Source/Cpp/../Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntime.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Cpp/../Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfCpp_Class.o: ../../Source/Cpp/WfCpp_Class.cpp ../../Source/Cpp/WfCpp.h ../../Source/Cpp/../Analyzer/WfAnalyzer.h ../../Source/Cpp/../Analyzer/../Expression/WfExpression.h ../../Source/Cpp/../Analyzer/../Expression/WfExpressionParser.h ../../Source/Cpp/../Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntime.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Cpp/../Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfCpp_Collect.o: ../../Source/Cpp/WfCpp_Collect.cpp ../../Source/Cpp/WfCpp.h ../../Source/Cpp/../Analyzer/WfAnalyzer.h ../../Source/Cpp/../Analyzer/../Expression/WfExpression.h ../../Source/Cpp/../Analyzer/../Expression/WfExpressionParser.h ../../Source/Cpp/../Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntime.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Cpp/../Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfCpp_Expression.o: ../../Source/Cpp/WfCpp_Expression.cpp ../../Source/Cpp/WfCpp.h ../../Source/Cpp/../Analyzer/WfAnalyzer.h ../../Source/Cpp/../Analyzer/../Expression/WfExpression.h ../../Source/Cpp/../Analyzer/../Expression/WfExpressionParser.h ../../Source/Cpp/../Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntime.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Cpp/../Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfCpp_Statement.o: ../../Source/Cpp/WfCpp_Statement.cpp ../../Source/Cpp/WfCpp.h ../../Source/Cpp/../Analyzer/WfAnalyzer.h ../../Source/Cpp/../Analyzer/../Expression/WfExpression.h ../../Source/Cpp/../Analyzer/../Expression/WfExpressionParser.h ../../Source/Cpp/../Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntime.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Cpp/../Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfCpp_WriteClass.o: ../../Source/Cpp/WfCpp_WriteClass.cpp ../../Source/Cpp/WfCpp.h ../../Source/Cpp/../Analyzer/WfAnalyzer.h ../../Source/Cpp/../Analyzer/../Expression/WfExpression.h ../../Source/Cpp/../Analyzer/../Expression/WfExpressionParser.h ../../Source/Cpp/../Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntime.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Cpp/../Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfCpp_WriteEnum.o: ../../Source/Cpp/WfCpp_WriteEnum.cpp ../../Source/Cpp/WfCpp.h ../../Source/Cpp/../Analyzer/WfAnalyzer.h ../../Source/Cpp/../Analyzer/../Expression/WfExpression.h ../../Source/Cpp/../Analyzer/../Expression/WfExpressionParser.h ../../Source/Cpp/../Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntime.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Cpp/../Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfCpp_WriteFunctionHeader.o: ../../Source/Cpp/WfCpp_WriteFunctionHeader.cpp ../../Source/Cpp/WfCpp.h ../../Source/Cpp/../Analyzer/WfAnalyzer.h ../../Source/Cpp/../Analyzer/../Expression/WfExpression.h ../../Source/Cpp/../Analyzer/../Expression/WfExpressionParser.h ../../Source/Cpp/../Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntime.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Cpp/../Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfCpp_WriteGlobal.o: ../../Source/Cpp/WfCpp_WriteGlobal.cpp ../../Source/Cpp/WfCpp.h ../../Source/Cpp/../Analyzer/WfAnalyzer.h ../../Source/Cpp/../Analyzer/../Expression/WfExpression.h ../../Source/Cpp/../Analyzer/../Expression/WfExpressionParser.h ../../Source/Cpp/../Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntime.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Cpp/../Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfCpp_WriteLambda.o: ../../Source/Cpp/WfCpp_WriteLambda.cpp ../../Source/Cpp/WfCpp.h ../../Source/Cpp/../Analyzer/WfAnalyzer.h ../../Source/Cpp/../Analyzer/../Expression/WfExpression.h ../../Source/Cpp/../Analyzer/../Expression/WfExpressionParser.h ../../Source/Cpp/../Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntime.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Cpp/../Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfCpp_WriteReflection.o: ../../Source/Cpp/WfCpp_WriteReflection.cpp ../../Source/Cpp/WfCpp.h ../../Source/Cpp/../Analyzer/WfAnalyzer.h ../../Source/Cpp/../Analyzer/../Expression/WfExpression.h ../../Source/Cpp/../Analyzer/../Expression/WfExpressionParser.h ../../Source/Cpp/../Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntime.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Cpp/../Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfCpp_WriteStruct.o: ../../Source/Cpp/WfCpp_WriteStruct.cpp ../../Source/Cpp/WfCpp.h ../../Source/Cpp/../Analyzer/WfAnalyzer.h ../../Source/Cpp/../Analyzer/../Expression/WfExpression.h ../../Source/Cpp/../Analyzer/../Expression/WfExpressionParser.h ../../Source/Cpp/../Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntime.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Cpp/../Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Cpp/../Analyzer/../Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfExpression.o: ../../Source/Expression/WfExpression.cpp ../../Source/Expression/WfExpression.h ../../Source/Expression/WfExpressionParser.h ../../Source/Expression/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/WfExpressionParser.o: ../../Source/Expression/WfExpressionParser.cpp ../../Source/Expression/WfExpressionParser.h ../../Source/Expression/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/WfRuntime.o: ../../Source/Runtime/WfRuntime.cpp ../../Source/Runtime/WfRuntime.h ../../Source/Runtime/WfRuntimeAssembly.h ../../Source/Runtime/WfRuntimeInstruction.h ../../Source/Runtime/../../Import/Vlpp.h ../../Source/Runtime/WfRuntimeTypeDescriptor.h ../../Source/Runtime/WfRuntimeConstructions.h ../../Source/Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfRuntimeConstructions.o: ../../Source/Runtime/WfRuntimeConstructions.cpp ../../Source/Runtime/WfRuntime.h ../../Source/Runtime/WfRuntimeAssembly.h ../../Source/Runtime/WfRuntimeInstruction.h ../../Source/Runtime/../../Import/Vlpp.h ../../Source/Runtime/WfRuntimeTypeDescriptor.h ../../Source/Runtime/WfRuntimeConstructions.h ../../Source/Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfRuntimeInstruction.o: ../../Source/Runtime/WfRuntimeInstruction.cpp ../../Source/Runtime/WfRuntimeInstruction.h ../../Source/Runtime/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/WfRuntimeAssembly.o: ../../Source/Runtime/WfRuntimeAssembly.cpp ../../Source/Runtime/WfRuntimeAssembly.h ../../Source/Runtime/WfRuntimeInstruction.h ../../Source/Runtime/../../Import/Vlpp.h ../../Source/Runtime/WfRuntimeTypeDescriptor.h
	$(CPP_COMPILE)

./Obj/WfRuntimeDebugger.o: ../../Source/Runtime/WfRuntimeDebugger.cpp ../../Source/Runtime/WfRuntime.h ../../Source/Runtime/WfRuntimeAssembly.h ../../Source/Runtime/WfRuntimeInstruction.h ../../Source/Runtime/../../Import/Vlpp.h ../../Source/Runtime/WfRuntimeTypeDescriptor.h ../../Source/Runtime/WfRuntimeConstructions.h ../../Source/Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfRuntimeExecution.o: ../../Source/Runtime/WfRuntimeExecution.cpp ../../Source/Runtime/WfRuntime.h ../../Source/Runtime/WfRuntimeAssembly.h ../../Source/Runtime/WfRuntimeInstruction.h ../../Source/Runtime/../../Import/Vlpp.h ../../Source/Runtime/WfRuntimeTypeDescriptor.h ../../Source/Runtime/WfRuntimeConstructions.h ../../Source/Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/WfRuntimeTypeDescriptor.o: ../../Source/Runtime/WfRuntimeTypeDescriptor.cpp ../../Source/Runtime/WfRuntime.h ../../Source/Runtime/WfRuntimeAssembly.h ../../Source/Runtime/WfRuntimeInstruction.h ../../Source/Runtime/../../Import/Vlpp.h ../../Source/Runtime/WfRuntimeTypeDescriptor.h ../../Source/Runtime/WfRuntimeConstructions.h ../../Source/Runtime/WfRuntimeDebugger.h
	$(CPP_COMPILE)

./Obj/CppTypes.o: ../Source/CppTypes.cpp ../Source/CppTypes.h ../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/Helper.o: ../Source/Helper.cpp ../Source/Helper.h ../Source/../../Source/Analyzer/WfAnalyzer.h ../Source/../../Source/Analyzer/../Expression/WfExpression.h ../Source/../../Source/Analyzer/../Expression/WfExpressionParser.h ../Source/../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../Source/../../Source/Analyzer/../Runtime/WfRuntime.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../Source/../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h ../Source/../../Source/Cpp/WfCpp.h
	$(CPP_COMPILE)

./Obj/TestAnalyzer.o: ../Source/TestAnalyzer.cpp ../Source/Helper.h ../Source/../../Source/Analyzer/WfAnalyzer.h ../Source/../../Source/Analyzer/../Expression/WfExpression.h ../Source/../../Source/Analyzer/../Expression/WfExpressionParser.h ../Source/../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../Source/../../Source/Analyzer/../Runtime/WfRuntime.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../Source/../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h ../Source/../../Source/Cpp/WfCpp.h ../Source/CppTypes.h ../Source/../../Import/Vlpp.h
	$(CPP_COMPILE)

./Obj/TestCodegen.o: ../Source/TestCodegen.cpp ../Source/Helper.h ../Source/../../Source/Analyzer/WfAnalyzer.h ../Source/../../Source/Analyzer/../Expression/WfExpression.h ../Source/../../Source/Analyzer/../Expression/WfExpressionParser.h ../Source/../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../Source/../../Source/Analyzer/../Runtime/WfRuntime.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../Source/../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h ../Source/../../Source/Cpp/WfCpp.h
	$(CPP_COMPILE)

./Obj/TestDebugger.o: ../Source/TestDebugger.cpp ../Source/Helper.h ../Source/../../Source/Analyzer/WfAnalyzer.h ../Source/../../Source/Analyzer/../Expression/WfExpression.h ../Source/../../Source/Analyzer/../Expression/WfExpressionParser.h ../Source/../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../Source/../../Source/Analyzer/../Runtime/WfRuntime.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../Source/../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h ../Source/../../Source/Cpp/WfCpp.h
	$(CPP_COMPILE)

./Obj/TestSamples.o: ../Source/TestSamples.cpp ../Source/Helper.h ../Source/../../Source/Analyzer/WfAnalyzer.h ../Source/../../Source/Analyzer/../Expression/WfExpression.h ../Source/../../Source/Analyzer/../Expression/WfExpressionParser.h ../Source/../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../Source/../../Source/Analyzer/../Runtime/WfRuntime.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../Source/../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h ../Source/../../Source/Cpp/WfCpp.h
	$(CPP_COMPILE)

./Obj/Main.o: ../UnitTest/UnitTest/Main.cpp ../UnitTest/UnitTest/../../Source/CppTypes.h ../UnitTest/UnitTest/../../Source/../../Import/Vlpp.h ../UnitTest/UnitTest/../../Source/Helper.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/WfAnalyzer.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Expression/WfExpression.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Expression/WfExpressionParser.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Runtime/WfRuntime.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h ../UnitTest/UnitTest/../../Source/../../Source/Cpp/WfCpp.h
	$(CPP_COMPILE)
