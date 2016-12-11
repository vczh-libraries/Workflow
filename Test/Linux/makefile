
.PHONY: all clean pre-build
.DEFAULT_GOAL := all
COVERAGE?=NO

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

./Bin/UnitTest:./Obj/Vlpp.o ./Obj/WfAnalyzer.o ./Obj/WfAnalyzer_BuildGlobalName.o ./Obj/WfAnalyzer_BuildScope.o ./Obj/WfAnalyzer_CompleteScope.o ./Obj/WfAnalyzer_Errors.o ./Obj/WfAnalyzer_GenerateAssembly.o ./Obj/WfAnalyzer_GenerateBind.o ./Obj/WfAnalyzer_GenerateDeclaration.o ./Obj/WfAnalyzer_GenerateExpression.o ./Obj/WfAnalyzer_GenerateMetadata.o ./Obj/WfAnalyzer_GenerateStatement.o ./Obj/WfAnalyzer_Misc.o ./Obj/WfAnalyzer_SearchOrderedName.o ./Obj/WfAnalyzer_TypeInfo.o ./Obj/WfAnalyzer_ValidateSemantic.o ./Obj/WfAnalyzer_ValidateStructure.o ./Obj/WfExpression.o ./Obj/WfExpressionParser.o ./Obj/WfRuntime.o ./Obj/WfRuntimeConstructions.o ./Obj/WfRuntimeInstruction.o ./Obj/WfRuntimeAssembly.o ./Obj/WfRuntimeDebugger.o ./Obj/WfRuntimeExecution.o ./Obj/WfRuntimeTypeDescriptor.o ./Obj/TestAnalyzer.o ./Obj/TestCodegen.o ./Obj/TestDebugger.o ./Obj/TestSamples.o ./Obj/Main.o
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -pthread -g -o $@ $^
else
	g++ -std=c++11 -g -fprofile-arcs -pthread -o $@ $^
endif


./Obj/Vlpp.o: ../../Import/Vlpp.cpp ../../Import/Vlpp.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfAnalyzer.o: ../../Source/Analyzer/WfAnalyzer.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfAnalyzer_BuildGlobalName.o: ../../Source/Analyzer/WfAnalyzer_BuildGlobalName.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfAnalyzer_BuildScope.o: ../../Source/Analyzer/WfAnalyzer_BuildScope.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfAnalyzer_CompleteScope.o: ../../Source/Analyzer/WfAnalyzer_CompleteScope.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfAnalyzer_Errors.o: ../../Source/Analyzer/WfAnalyzer_Errors.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfAnalyzer_GenerateAssembly.o: ../../Source/Analyzer/WfAnalyzer_GenerateAssembly.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfAnalyzer_GenerateBind.o: ../../Source/Analyzer/WfAnalyzer_GenerateBind.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfAnalyzer_GenerateDeclaration.o: ../../Source/Analyzer/WfAnalyzer_GenerateDeclaration.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfAnalyzer_GenerateExpression.o: ../../Source/Analyzer/WfAnalyzer_GenerateExpression.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfAnalyzer_GenerateMetadata.o: ../../Source/Analyzer/WfAnalyzer_GenerateMetadata.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfAnalyzer_GenerateStatement.o: ../../Source/Analyzer/WfAnalyzer_GenerateStatement.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfAnalyzer_Misc.o: ../../Source/Analyzer/WfAnalyzer_Misc.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfAnalyzer_SearchOrderedName.o: ../../Source/Analyzer/WfAnalyzer_SearchOrderedName.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfAnalyzer_TypeInfo.o: ../../Source/Analyzer/WfAnalyzer_TypeInfo.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfAnalyzer_ValidateSemantic.o: ../../Source/Analyzer/WfAnalyzer_ValidateSemantic.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfAnalyzer_ValidateStructure.o: ../../Source/Analyzer/WfAnalyzer_ValidateStructure.cpp ../../Source/Analyzer/WfAnalyzer.h ../../Source/Analyzer/../Expression/WfExpression.h ../../Source/Analyzer/../Expression/WfExpressionParser.h ../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntime.h ../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfExpression.o: ../../Source/Expression/WfExpression.cpp ../../Source/Expression/WfExpression.h ../../Source/Expression/WfExpressionParser.h ../../Source/Expression/../../Import/Vlpp.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfExpressionParser.o: ../../Source/Expression/WfExpressionParser.cpp ../../Source/Expression/WfExpressionParser.h ../../Source/Expression/../../Import/Vlpp.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfRuntime.o: ../../Source/Runtime/WfRuntime.cpp ../../Source/Runtime/WfRuntime.h ../../Source/Runtime/WfRuntimeAssembly.h ../../Source/Runtime/WfRuntimeInstruction.h ../../Source/Runtime/../../Import/Vlpp.h ../../Source/Runtime/WfRuntimeTypeDescriptor.h ../../Source/Runtime/WfRuntimeConstructions.h ../../Source/Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfRuntimeConstructions.o: ../../Source/Runtime/WfRuntimeConstructions.cpp ../../Source/Runtime/WfRuntime.h ../../Source/Runtime/WfRuntimeAssembly.h ../../Source/Runtime/WfRuntimeInstruction.h ../../Source/Runtime/../../Import/Vlpp.h ../../Source/Runtime/WfRuntimeTypeDescriptor.h ../../Source/Runtime/WfRuntimeConstructions.h ../../Source/Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfRuntimeInstruction.o: ../../Source/Runtime/WfRuntimeInstruction.cpp ../../Source/Runtime/WfRuntimeInstruction.h ../../Source/Runtime/../../Import/Vlpp.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfRuntimeAssembly.o: ../../Source/Runtime/WfRuntimeAssembly.cpp ../../Source/Runtime/WfRuntimeAssembly.h ../../Source/Runtime/WfRuntimeInstruction.h ../../Source/Runtime/../../Import/Vlpp.h ../../Source/Runtime/WfRuntimeTypeDescriptor.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfRuntimeDebugger.o: ../../Source/Runtime/WfRuntimeDebugger.cpp ../../Source/Runtime/WfRuntime.h ../../Source/Runtime/WfRuntimeAssembly.h ../../Source/Runtime/WfRuntimeInstruction.h ../../Source/Runtime/../../Import/Vlpp.h ../../Source/Runtime/WfRuntimeTypeDescriptor.h ../../Source/Runtime/WfRuntimeConstructions.h ../../Source/Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfRuntimeExecution.o: ../../Source/Runtime/WfRuntimeExecution.cpp ../../Source/Runtime/WfRuntime.h ../../Source/Runtime/WfRuntimeAssembly.h ../../Source/Runtime/WfRuntimeInstruction.h ../../Source/Runtime/../../Import/Vlpp.h ../../Source/Runtime/WfRuntimeTypeDescriptor.h ../../Source/Runtime/WfRuntimeConstructions.h ../../Source/Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/WfRuntimeTypeDescriptor.o: ../../Source/Runtime/WfRuntimeTypeDescriptor.cpp ../../Source/Runtime/WfRuntime.h ../../Source/Runtime/WfRuntimeAssembly.h ../../Source/Runtime/WfRuntimeInstruction.h ../../Source/Runtime/../../Import/Vlpp.h ../../Source/Runtime/WfRuntimeTypeDescriptor.h ../../Source/Runtime/WfRuntimeConstructions.h ../../Source/Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/TestAnalyzer.o: ../Source/TestAnalyzer.cpp ../Source/TestFunctions.h ../Source/../../Source/Analyzer/WfAnalyzer.h ../Source/../../Source/Analyzer/../Expression/WfExpression.h ../Source/../../Source/Analyzer/../Expression/WfExpressionParser.h ../Source/../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../Source/../../Source/Analyzer/../Runtime/WfRuntime.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../Source/../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/TestCodegen.o: ../Source/TestCodegen.cpp ../Source/TestFunctions.h ../Source/../../Source/Analyzer/WfAnalyzer.h ../Source/../../Source/Analyzer/../Expression/WfExpression.h ../Source/../../Source/Analyzer/../Expression/WfExpressionParser.h ../Source/../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../Source/../../Source/Analyzer/../Runtime/WfRuntime.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../Source/../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/TestDebugger.o: ../Source/TestDebugger.cpp ../Source/TestFunctions.h ../Source/../../Source/Analyzer/WfAnalyzer.h ../Source/../../Source/Analyzer/../Expression/WfExpression.h ../Source/../../Source/Analyzer/../Expression/WfExpressionParser.h ../Source/../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../Source/../../Source/Analyzer/../Runtime/WfRuntime.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../Source/../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/TestSamples.o: ../Source/TestSamples.cpp ../Source/TestFunctions.h ../Source/../../Source/Analyzer/WfAnalyzer.h ../Source/../../Source/Analyzer/../Expression/WfExpression.h ../Source/../../Source/Analyzer/../Expression/WfExpressionParser.h ../Source/../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../Source/../../Source/Analyzer/../Runtime/WfRuntime.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../Source/../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../Source/../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif

./Obj/Main.o: ../UnitTest/UnitTest/Main.cpp ../UnitTest/UnitTest/../../Source/TestFunctions.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/WfAnalyzer.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Expression/WfExpression.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Expression/WfExpressionParser.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Expression/../../Import/Vlpp.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Runtime/WfRuntime.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Runtime/WfRuntimeAssembly.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Runtime/WfRuntimeInstruction.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Runtime/../../Import/Vlpp.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Runtime/WfRuntimeTypeDescriptor.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Runtime/WfRuntimeConstructions.h ../UnitTest/UnitTest/../../Source/../../Source/Analyzer/../Runtime/WfRuntimeDebugger.h
ifeq ($(COVERAGE),NO)
	clang++ -std=c++14 -g -o $@ -c $<
else
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
endif
