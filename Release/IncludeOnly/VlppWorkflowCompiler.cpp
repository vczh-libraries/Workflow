﻿/***********************************************************************
THIS FILE IS AUTOMATICALLY GENERATED. DO NOT MODIFY
DEVELOPER: Zihan Chen(vczh)
***********************************************************************/
#include "VlppWorkflowCompiler.h"

#include "..\..\Source\Analyzer\CheckScopes_CycleDependency.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_BuildGlobalNameFromModules.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_BuildGlobalNameFromTypeDescriptors.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_BuildScope.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_CheckScope_DuplicatedSymbol.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_CheckScope_SymbolType.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_CompleteScope.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_ContextFreeDesugar.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_Errors.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_ExpandBindExpression.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_ExpandMixinCastExpression.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_ExpandNewCoroutineExpression.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_ExpandStateMachine.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_ExpandStatement.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_Misc.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_SearchOrderedName.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_TypeInfo.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_ValidateScopeName.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_ValidateSemantic.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_ValidateSemantic_Declaration.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_ValidateSemantic_Expression.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_ValidateSemantic_Statement.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_ValidateStructure.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_ValidateStructure_Declaration.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_ValidateStructure_Expression.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_ValidateStructure_Statement.cpp"
#include "..\..\Source\Analyzer\WfAnalyzer_ValidateStructure_Type.cpp"
#include "..\..\Source\Cpp\WfCpp.cpp"
#include "..\..\Source\Cpp\WfCpp_AssignClassDeclsToFiles.cpp"
#include "..\..\Source\Cpp\WfCpp_Class.cpp"
#include "..\..\Source\Cpp\WfCpp_Collect.cpp"
#include "..\..\Source\Cpp\WfCpp_Expression.cpp"
#include "..\..\Source\Cpp\WfCpp_File.cpp"
#include "..\..\Source\Cpp\WfCpp_GenerateCppFiles.cpp"
#include "..\..\Source\Cpp\WfCpp_Statement.cpp"
#include "..\..\Source\Cpp\WfCpp_WriteClass.cpp"
#include "..\..\Source\Cpp\WfCpp_WriteEnum.cpp"
#include "..\..\Source\Cpp\WfCpp_WriteFunctionHeader.cpp"
#include "..\..\Source\Cpp\WfCpp_WriteGlobal.cpp"
#include "..\..\Source\Cpp\WfCpp_WriteLambda.cpp"
#include "..\..\Source\Cpp\WfCpp_WriteReflection.cpp"
#include "..\..\Source\Cpp\WfCpp_WriteStruct.cpp"
#include "..\..\Source\Cpp\WfMergeCpp.cpp"
#include "..\..\Source\Emitter\WfEmitter.cpp"
#include "..\..\Source\Emitter\WfEmitter_Assembly.cpp"
#include "..\..\Source\Emitter\WfEmitter_Declaration.cpp"
#include "..\..\Source\Emitter\WfEmitter_Expression.cpp"
#include "..\..\Source\Emitter\WfEmitter_Metadata.cpp"
#include "..\..\Source\Emitter\WfEmitter_Statement.cpp"
#include "..\..\Source\Parser\WfExpression.cpp"
#include "..\..\Source\Parser\Generated\WorkflowAst.cpp"
#include "..\..\Source\Parser\Generated\WorkflowAst_Builder.cpp"
#include "..\..\Source\Parser\Generated\WorkflowAst_Copy.cpp"
#include "..\..\Source\Parser\Generated\WorkflowAst_Empty.cpp"
#include "..\..\Source\Parser\Generated\WorkflowAst_Json.cpp"
#include "..\..\Source\Parser\Generated\WorkflowAst_Traverse.cpp"
#include "..\..\Source\Parser\Generated\WorkflowParser.cpp"
#include "..\..\Source\Parser\Generated\Workflow_Assembler.cpp"
#include "..\..\Source\Parser\Generated\Workflow_Lexer.cpp"
