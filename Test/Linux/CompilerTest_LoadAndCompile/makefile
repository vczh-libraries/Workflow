.PHONY: all clean pre-build
.DEFAULT_GOAL := all

CPP_COMPILE_OPTIONS=-I ../../../Import -DVCZH_DEBUG_METAONLY_REFLECTION
include $(VCPROOT)/vl/makefile-cpp

pre-build:
	if ! [ -d ./Bin ]; then mkdir ./Bin; fi
	if ! [ -d ./Obj ]; then mkdir ./Obj; fi
	if ! [ -d ./Coverage ]; then mkdir ./Coverage; fi
	if ! [ -d ../../Output ]; then mkdir ../../Output; fi
	if ! [ -d ../../Output/Workflow64 ]; then mkdir ../../Output/Workflow64; fi
	if ! [ -d ../../Output/Cpp64 ]; then mkdir ../../Output/Cpp64; fi

clean:
	if [ -d ./Bin ]; then rm -r ./Bin; fi
	if [ -d ./Obj ]; then rm -r ./Obj; fi
	if [ -d ./Coverage ]; then rm -r ./Coverage; fi
	if [ -d ./../../Output ]; then rm -r ../../Output; fi
	if [ -d ./../../Output/Workflow64 ]; then rm -r ../../Output/Workflow64; fi
	if [ -d ./../../Output/Cpp64 ]; then rm -r ../../Output/Cpp64; fi

all:pre-build ./Bin/UnitTest

./Bin/UnitTest:./Obj/Vlpp.o ./Obj/Vlpp.Linux.o ./Obj/VlppGlrParser.o ./Obj/VlppOS.o ./Obj/VlppOS.Linux.o ./Obj/VlppReflection.o ./Obj/VlppRegex.o ./Obj/WfLibraryCppHelper.o ./Obj/WfLibraryPredefined.o ./Obj/WfLibraryReflection.o ./Obj/WfLibraryRpc.o ./Obj/WfLibraryRpcController.o ./Obj/WfLibraryRpcLifecycle.o ./Obj/WfLibraryRpcWrappers.o ./Obj/WfLibraryRpcJson.o ./Obj/WfLibraryRpcJsonDispatcherClient.o ./Obj/WfLibraryRpcJsonDispatcherServer.o ./Obj/WfRuntime.o ./Obj/WfRuntimeAssembly.o ./Obj/WfRuntimeConstructions.o ./Obj/WfRuntimeDebugger.o ./Obj/WfRuntimeExecution.o ./Obj/WfRuntimeInstruction.o ./Obj/WfRuntimeTypeDescriptor.o ./Obj/WfAnalyzer_CheckScopes_CycleDependency.o ./Obj/WfAnalyzer.o ./Obj/WfAnalyzer_BuildGlobalNameFromModules.o ./Obj/WfAnalyzer_BuildGlobalNameFromTypeDescriptors.o ./Obj/WfAnalyzer_BuildScope.o ./Obj/WfAnalyzer_CheckScope_DuplicatedSymbol.o ./Obj/WfAnalyzer_CheckScope_SymbolType.o ./Obj/WfAnalyzer_CompleteScope.o ./Obj/WfAnalyzer_ContextFreeDesugar.o ./Obj/WfAnalyzer_Errors.o ./Obj/WfAnalyzer_ExpandBindExpression.o ./Obj/WfAnalyzer_ExpandMixinCastExpression.o ./Obj/WfAnalyzer_ExpandNewCoroutineExpression.o ./Obj/WfAnalyzer_ExpandStateMachine.o ./Obj/WfAnalyzer_ExpandStatement.o ./Obj/WfAnalyzer_RpcAstHelpers.o ./Obj/WfAnalyzer_GenerateRpc_JsonSerialization.o ./Obj/WfAnalyzer_GenerateRpc_JsonDts.o ./Obj/WfAnalyzer_GenerateRpc.o ./Obj/WfAnalyzer_Misc.o ./Obj/WfAnalyzer_SearchOrderedName.o ./Obj/WfAnalyzer_TypeInfo.o ./Obj/WfAnalyzer_ValidateRPC.o ./Obj/WfAnalyzer_ValidateScopeName.o ./Obj/WfAnalyzer_ValidateSemantic.o ./Obj/WfAnalyzer_ValidateSemantic_Declaration.o ./Obj/WfAnalyzer_ValidateSemantic_Expression.o ./Obj/WfAnalyzer_ValidateSemantic_Statement.o ./Obj/WfAnalyzer_ValidateStructure.o ./Obj/WfAnalyzer_ValidateStructure_Declaration.o ./Obj/WfAnalyzer_ValidateStructure_Expression.o ./Obj/WfAnalyzer_ValidateStructure_Statement.o ./Obj/WfAnalyzer_ValidateStructure_Type.o ./Obj/WfCpp.o ./Obj/WfCpp_AssignClassDeclsToFiles.o ./Obj/WfCpp_Class.o ./Obj/WfCpp_Collect.o ./Obj/WfCpp_Expression.o ./Obj/WfCpp_File.o ./Obj/WfCpp_GenerateCppFiles.o ./Obj/WfCpp_Statement.o ./Obj/WfCpp_WriteClass.o ./Obj/WfCpp_WriteEnum.o ./Obj/WfCpp_WriteFunctionHeader.o ./Obj/WfCpp_WriteGlobal.o ./Obj/WfCpp_WriteLambda.o ./Obj/WfCpp_WriteReflection.o ./Obj/WfCpp_WriteStruct.o ./Obj/WfMergeCpp.o ./Obj/WfEmitter.o ./Obj/WfEmitter_Assembly.o ./Obj/WfEmitter_Declaration.o ./Obj/WfEmitter_Expression.o ./Obj/WfEmitter_Metadata.o ./Obj/WfEmitter_Statement.o ./Obj/WorkflowAst.o ./Obj/WorkflowAst_Builder.o ./Obj/WorkflowAst_Copy.o ./Obj/WorkflowAst_Empty.o ./Obj/WorkflowAst_Json.o ./Obj/WorkflowAst_Traverse.o ./Obj/WorkflowParser.o ./Obj/Workflow_Assembler.o ./Obj/Workflow_Lexer.o ./Obj/WfExpression.o ./Obj/CppTypes.o ./Obj/GetExePath.o ./Obj/Helper.o ./Obj/Main.o ./Obj/TestAnalyzer.o ./Obj/TestAppCompile.o ./Obj/TestCodegen.o ./Obj/TestRpcCompile.o ./Obj/TestSamples.o
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

./Obj/WfRuntime.o: ../../../Source/Runtime/WfRuntime.cpp
	$(CPP_COMPILE)

./Obj/WfRuntimeAssembly.o: ../../../Source/Runtime/WfRuntimeAssembly.cpp
	$(CPP_COMPILE)

./Obj/WfRuntimeConstructions.o: ../../../Source/Runtime/WfRuntimeConstructions.cpp
	$(CPP_COMPILE)

./Obj/WfRuntimeDebugger.o: ../../../Source/Runtime/WfRuntimeDebugger.cpp
	$(CPP_COMPILE)

./Obj/WfRuntimeExecution.o: ../../../Source/Runtime/WfRuntimeExecution.cpp
	$(CPP_COMPILE)

./Obj/WfRuntimeInstruction.o: ../../../Source/Runtime/WfRuntimeInstruction.cpp
	$(CPP_COMPILE)

./Obj/WfRuntimeTypeDescriptor.o: ../../../Source/Runtime/WfRuntimeTypeDescriptor.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_CheckScopes_CycleDependency.o: ../../../Source/Analyzer/WfAnalyzer_CheckScopes_CycleDependency.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer.o: ../../../Source/Analyzer/WfAnalyzer.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_BuildGlobalNameFromModules.o: ../../../Source/Analyzer/WfAnalyzer_BuildGlobalNameFromModules.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_BuildGlobalNameFromTypeDescriptors.o: ../../../Source/Analyzer/WfAnalyzer_BuildGlobalNameFromTypeDescriptors.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_BuildScope.o: ../../../Source/Analyzer/WfAnalyzer_BuildScope.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_CheckScope_DuplicatedSymbol.o: ../../../Source/Analyzer/WfAnalyzer_CheckScope_DuplicatedSymbol.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_CheckScope_SymbolType.o: ../../../Source/Analyzer/WfAnalyzer_CheckScope_SymbolType.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_CompleteScope.o: ../../../Source/Analyzer/WfAnalyzer_CompleteScope.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ContextFreeDesugar.o: ../../../Source/Analyzer/WfAnalyzer_ContextFreeDesugar.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_Errors.o: ../../../Source/Analyzer/WfAnalyzer_Errors.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ExpandBindExpression.o: ../../../Source/Analyzer/WfAnalyzer_ExpandBindExpression.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ExpandMixinCastExpression.o: ../../../Source/Analyzer/WfAnalyzer_ExpandMixinCastExpression.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ExpandNewCoroutineExpression.o: ../../../Source/Analyzer/WfAnalyzer_ExpandNewCoroutineExpression.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ExpandStateMachine.o: ../../../Source/Analyzer/WfAnalyzer_ExpandStateMachine.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ExpandStatement.o: ../../../Source/Analyzer/WfAnalyzer_ExpandStatement.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_RpcAstHelpers.o: ../../../Source/Analyzer/Rpc/WfAnalyzer_RpcAstHelpers.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_GenerateRpc_JsonSerialization.o: ../../../Source/Analyzer/Rpc/WfAnalyzer_GenerateRpc_JsonSerialization.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_GenerateRpc_JsonDts.o: ../../../Source/Analyzer/Rpc/WfAnalyzer_GenerateRpc_JsonDts.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_GenerateRpc.o: ../../../Source/Analyzer/Rpc/WfAnalyzer_GenerateRpc.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_Misc.o: ../../../Source/Analyzer/WfAnalyzer_Misc.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_SearchOrderedName.o: ../../../Source/Analyzer/WfAnalyzer_SearchOrderedName.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_TypeInfo.o: ../../../Source/Analyzer/WfAnalyzer_TypeInfo.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ValidateRPC.o: ../../../Source/Analyzer/Rpc/WfAnalyzer_ValidateRPC.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ValidateScopeName.o: ../../../Source/Analyzer/WfAnalyzer_ValidateScopeName.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ValidateSemantic.o: ../../../Source/Analyzer/WfAnalyzer_ValidateSemantic.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ValidateSemantic_Declaration.o: ../../../Source/Analyzer/WfAnalyzer_ValidateSemantic_Declaration.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ValidateSemantic_Expression.o: ../../../Source/Analyzer/WfAnalyzer_ValidateSemantic_Expression.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ValidateSemantic_Statement.o: ../../../Source/Analyzer/WfAnalyzer_ValidateSemantic_Statement.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ValidateStructure.o: ../../../Source/Analyzer/WfAnalyzer_ValidateStructure.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ValidateStructure_Declaration.o: ../../../Source/Analyzer/WfAnalyzer_ValidateStructure_Declaration.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ValidateStructure_Expression.o: ../../../Source/Analyzer/WfAnalyzer_ValidateStructure_Expression.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ValidateStructure_Statement.o: ../../../Source/Analyzer/WfAnalyzer_ValidateStructure_Statement.cpp
	$(CPP_COMPILE)

./Obj/WfAnalyzer_ValidateStructure_Type.o: ../../../Source/Analyzer/WfAnalyzer_ValidateStructure_Type.cpp
	$(CPP_COMPILE)

./Obj/WfCpp.o: ../../../Source/Cpp/WfCpp.cpp
	$(CPP_COMPILE)

./Obj/WfCpp_AssignClassDeclsToFiles.o: ../../../Source/Cpp/WfCpp_AssignClassDeclsToFiles.cpp
	$(CPP_COMPILE)

./Obj/WfCpp_Class.o: ../../../Source/Cpp/WfCpp_Class.cpp
	$(CPP_COMPILE)

./Obj/WfCpp_Collect.o: ../../../Source/Cpp/WfCpp_Collect.cpp
	$(CPP_COMPILE)

./Obj/WfCpp_Expression.o: ../../../Source/Cpp/WfCpp_Expression.cpp
	$(CPP_COMPILE)

./Obj/WfCpp_File.o: ../../../Source/Cpp/WfCpp_File.cpp
	$(CPP_COMPILE)

./Obj/WfCpp_GenerateCppFiles.o: ../../../Source/Cpp/WfCpp_GenerateCppFiles.cpp
	$(CPP_COMPILE)

./Obj/WfCpp_Statement.o: ../../../Source/Cpp/WfCpp_Statement.cpp
	$(CPP_COMPILE)

./Obj/WfCpp_WriteClass.o: ../../../Source/Cpp/WfCpp_WriteClass.cpp
	$(CPP_COMPILE)

./Obj/WfCpp_WriteEnum.o: ../../../Source/Cpp/WfCpp_WriteEnum.cpp
	$(CPP_COMPILE)

./Obj/WfCpp_WriteFunctionHeader.o: ../../../Source/Cpp/WfCpp_WriteFunctionHeader.cpp
	$(CPP_COMPILE)

./Obj/WfCpp_WriteGlobal.o: ../../../Source/Cpp/WfCpp_WriteGlobal.cpp
	$(CPP_COMPILE)

./Obj/WfCpp_WriteLambda.o: ../../../Source/Cpp/WfCpp_WriteLambda.cpp
	$(CPP_COMPILE)

./Obj/WfCpp_WriteReflection.o: ../../../Source/Cpp/WfCpp_WriteReflection.cpp
	$(CPP_COMPILE)

./Obj/WfCpp_WriteStruct.o: ../../../Source/Cpp/WfCpp_WriteStruct.cpp
	$(CPP_COMPILE)

./Obj/WfMergeCpp.o: ../../../Source/Cpp/WfMergeCpp.cpp
	$(CPP_COMPILE)

./Obj/WfEmitter.o: ../../../Source/Emitter/WfEmitter.cpp
	$(CPP_COMPILE)

./Obj/WfEmitter_Assembly.o: ../../../Source/Emitter/WfEmitter_Assembly.cpp
	$(CPP_COMPILE)

./Obj/WfEmitter_Declaration.o: ../../../Source/Emitter/WfEmitter_Declaration.cpp
	$(CPP_COMPILE)

./Obj/WfEmitter_Expression.o: ../../../Source/Emitter/WfEmitter_Expression.cpp
	$(CPP_COMPILE)

./Obj/WfEmitter_Metadata.o: ../../../Source/Emitter/WfEmitter_Metadata.cpp
	$(CPP_COMPILE)

./Obj/WfEmitter_Statement.o: ../../../Source/Emitter/WfEmitter_Statement.cpp
	$(CPP_COMPILE)

./Obj/WorkflowAst.o: ../../../Source/Parser/Generated/WorkflowAst.cpp
	$(CPP_COMPILE)

./Obj/WorkflowAst_Builder.o: ../../../Source/Parser/Generated/WorkflowAst_Builder.cpp
	$(CPP_COMPILE)

./Obj/WorkflowAst_Copy.o: ../../../Source/Parser/Generated/WorkflowAst_Copy.cpp
	$(CPP_COMPILE)

./Obj/WorkflowAst_Empty.o: ../../../Source/Parser/Generated/WorkflowAst_Empty.cpp
	$(CPP_COMPILE)

./Obj/WorkflowAst_Json.o: ../../../Source/Parser/Generated/WorkflowAst_Json.cpp
	$(CPP_COMPILE)

./Obj/WorkflowAst_Traverse.o: ../../../Source/Parser/Generated/WorkflowAst_Traverse.cpp
	$(CPP_COMPILE)

./Obj/WorkflowParser.o: ../../../Source/Parser/Generated/WorkflowParser.cpp
	$(CPP_COMPILE)

./Obj/Workflow_Assembler.o: ../../../Source/Parser/Generated/Workflow_Assembler.cpp
	$(CPP_COMPILE)

./Obj/Workflow_Lexer.o: ../../../Source/Parser/Generated/Workflow_Lexer.cpp
	$(CPP_COMPILE)

./Obj/WfExpression.o: ../../../Source/Parser/WfExpression.cpp
	$(CPP_COMPILE)

./Obj/CppTypes.o: ../../Source/CppTypes.cpp
	$(CPP_COMPILE)

./Obj/GetExePath.o: ../../Source/GetExePath.cpp
	$(CPP_COMPILE)

./Obj/Helper.o: ../../Source/Helper.cpp
	$(CPP_COMPILE)

./Obj/Main.o: ../../UnitTest/CompilerTest_LoadAndCompile/Main.cpp
	$(CPP_COMPILE)

./Obj/TestAnalyzer.o: ../../UnitTest/CompilerTest_LoadAndCompile/TestAnalyzer.cpp
	$(CPP_COMPILE)

./Obj/TestAppCompile.o: ../../UnitTest/CompilerTest_LoadAndCompile/TestAppCompile.cpp
	$(CPP_COMPILE)

./Obj/TestCodegen.o: ../../UnitTest/CompilerTest_LoadAndCompile/TestCodegen.cpp
	$(CPP_COMPILE)

./Obj/TestRpcCompile.o: ../../UnitTest/CompilerTest_LoadAndCompile/TestRpcCompile.cpp
	$(CPP_COMPILE)

./Obj/TestSamples.o: ../../UnitTest/CompilerTest_LoadAndCompile/TestSamples.cpp
	$(CPP_COMPILE)
