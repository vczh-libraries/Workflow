/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::Analyzer

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_ANALYZER_WFEMITTER
#define VCZH_WORKFLOW_ANALYZER_WFEMITTER

#include "../Analyzer/WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace emitter
		{

/***********************************************************************
Attribute Evaluator
***********************************************************************/

			class WfAttributeEvaluator : public Object
			{
				typedef reflection::description::Value														Value;
				typedef collections::Dictionary<Ptr<WfAttribute>, Value>									AttributeValueMap;

			protected:
				analyzer::WfLexicalScopeManager*			manager;
				AttributeValueMap							attributeValues;				// cached value for attribute
				Ptr<runtime::WfAssembly>					attributeAssembly;				// shared assembly for evaluating attribute value
				Ptr<runtime::WfRuntimeGlobalContext>		attributeGlobalContext;			// shared shared context for evaluating attribute value

			public:
				WfAttributeEvaluator(analyzer::WfLexicalScopeManager* _manager);

				Ptr<WfAttribute>							GetAttribute(collections::List<Ptr<WfAttribute>>& atts, const WString& category, const WString& name);
				collections::LazyList<Ptr<WfAttribute>>		GetAttributes(collections::List<Ptr<WfAttribute>>& atts, const WString& category, const WString& name);
				Value										GetAttributeValue(Ptr<WfAttribute> att);
			};

/***********************************************************************
Code Generation
***********************************************************************/

			struct WfCodegenLambdaContext
			{
				WfFunctionDeclaration*				functionDeclaration = 0;
				WfFunctionExpression*				functionExpression = 0;
				WfOrderedLambdaExpression*			orderedLambdaExpression = 0;
			};

			enum class WfCodegenScopeType
			{
				Function,	// contains the whole function
				Loop,		// contains all loops
				TryCatch,	// contains try and catch, not finally
				Block,		// contains labeled blocks
			};

			class WfCodegenScopeContext : public Object
			{
				typedef collections::List<vint>										InstructionIndexList;
				typedef collections::List<runtime::WfInstruction>					InstructionList;
				typedef collections::List<parsing::ParsingTextRange>				RangeMap;
			public:
				WfCodegenScopeType					type = WfCodegenScopeType::Function;
				InstructionIndexList				continueInstructions;
				InstructionIndexList				breakInstructions;
				WString								name;
				
				InstructionList						exitInstructions;
				RangeMap							instructionCodeMappingBeforeCodegen;
				RangeMap							instructionCodeMappingAfterCodegen;
				Ptr<WfStatement>					exitStatement;
			};

			class WfCodegenFunctionContext : public Object
			{
				typedef collections::Dictionary<analyzer::WfLexicalSymbol*, vint>	VariableIndexMap;
				typedef collections::Dictionary<vint, WfCodegenLambdaContext>		ClosureIndexMap;
				typedef collections::List<Ptr<WfCodegenScopeContext>>				ScopeContextList;
			public:
				Ptr<runtime::WfAssemblyFunction>	function;
				VariableIndexMap					capturedVariables;
				VariableIndexMap					arguments;
				VariableIndexMap					localVariables;
				ClosureIndexMap						closuresToCodegen;
				ScopeContextList					scopeContextStack;

				WfCodegenFunctionContext();
				
				Ptr<WfCodegenScopeContext>			GetCurrentScopeContext();
				Ptr<WfCodegenScopeContext>			GetCurrentScopeContext(WfCodegenScopeType type, const WString& name = L"");
				Ptr<WfCodegenScopeContext>			PushScopeContext(WfCodegenScopeType type, const WString& name = L"");
				void								PopScopeContext();
			};

			class WfCodegenContext : public Object
			{
				typedef collections::Dictionary<analyzer::WfLexicalSymbol*, vint>								SymbolIndexMap;
				typedef collections::Dictionary<WfConstructorDeclaration*, vint>								ConstructorIndexMap;
				typedef collections::Dictionary<WfDestructorDeclaration*, vint>									DestructorIndexMap;
				typedef collections::Dictionary<parsing::ParsingTreeCustomBase*, parsing::ParsingTextRange>		NodePositionMap;
				typedef collections::Dictionary<Ptr<analyzer::WfLexicalFunctionConfig>, vint>					ThisStackCountMap;
			public:
				Ptr<runtime::WfAssembly>			assembly;
				analyzer::WfLexicalScopeManager*	manager;
				SymbolIndexMap						globalVariables;
				SymbolIndexMap						globalFunctions;
				ConstructorIndexMap					constructors;
				DestructorIndexMap					destructors;
				SymbolIndexMap						closureFunctions;
				Ptr<WfCodegenFunctionContext>		functionContext;
				NodePositionMap						nodePositionsBeforeCodegen;
				NodePositionMap						nodePositionsAfterCodegen;
				ThisStackCountMap					thisStackCounts;

				WfCodegenContext(Ptr<runtime::WfAssembly> _assembly, analyzer::WfLexicalScopeManager* _manager);

				vint								AddInstruction(parsing::ParsingTreeCustomBase* node, const runtime::WfInstruction& ins);
				void								AddExitInstruction(parsing::ParsingTreeCustomBase* node, const runtime::WfInstruction& ins);
				void								ApplyExitInstructions(Ptr<WfCodegenScopeContext> scopeContext);
				vint								GetThisStackCount(analyzer::WfLexicalScope* scope);
			};

			extern void										GenerateFunctionDeclarationMetadata(WfCodegenContext& context, WfFunctionDeclaration* node, Ptr<runtime::WfAssemblyFunction> meta);
			extern void										GenerateGlobalDeclarationMetadata(WfCodegenContext& context, Ptr<WfDeclaration> declaration, const WString& namePrefix = L"");
			extern void										GenerateClosureInstructions(WfCodegenContext& context, Ptr<WfCodegenFunctionContext> functionContext);
			extern void										GenerateInitializeInstructions(WfCodegenContext& context, Ptr<WfDeclaration> declaration);
			extern void										GenerateDeclarationInstructions(WfCodegenContext& context, Ptr<WfDeclaration> declaration);
			extern void										GenerateStatementInstructions(WfCodegenContext& context, Ptr<WfStatement> statement);
			extern Ptr<reflection::description::ITypeInfo>	GenerateExpressionInstructions(WfCodegenContext& context, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType = 0);
			extern void										GenerateTypeCastInstructions(WfCodegenContext& context, Ptr<reflection::description::ITypeInfo> expectedType, bool strongCast, WfExpression* node);
			extern void										GenerateTypeTestingInstructions(WfCodegenContext& context, Ptr<reflection::description::ITypeInfo> expectedType, WfExpression* node);
			extern runtime::WfInsType						GetInstructionTypeArgument(Ptr<reflection::description::ITypeInfo> expectedType);

			/// <summary>Generate an assembly from a compiler. [M:vl.workflow.analyzer.WfLexicalScopeManager.Rebuild] should be called before using this function.</summary>
			/// <returns>The generated assembly.</returns>
			/// <param name="manager">The Workflow compiler.</param>
			extern Ptr<runtime::WfAssembly>					GenerateAssembly(analyzer::WfLexicalScopeManager* manager, IWfCompilerCallback* callback = nullptr);

			/// <summary>Compile a Workflow program.</summary>
			/// <returns>The generated assembly.</returns>
			/// <param name="table">The workflow parser table. It can be retrived from [M:vl.workflow.WfLoadTable].</param>
			/// <param name="manager">The workflow compiler to reuse the cache of C++ reflectable types.</param>
			/// <param name="moduleCodes">All workflow module codes.</param>
			/// <param name="errors">Container to get all compileing errors.</param>
			extern Ptr<runtime::WfAssembly>					Compile(Ptr<parsing::tabling::ParsingTable> table, analyzer::WfLexicalScopeManager* manager, collections::List<WString>& moduleCodes, collections::List<Ptr<parsing::ParsingError>>& errors);
			
			/// <summary>Compile a Workflow program.</summary>
			/// <returns>The generated assembly.</returns>
			/// <param name="table">The workflow parser table. It can be retrived from [M:vl.workflow.WfLoadTable].</param>
			/// <param name="moduleCodes">All workflow module codes.</param>
			/// <param name="errors">Container to get all compileing errors.</param>
			extern Ptr<runtime::WfAssembly>					Compile(Ptr<parsing::tabling::ParsingTable> table, collections::List<WString>& moduleCodes, collections::List<Ptr<parsing::ParsingError>>& errors);
		}
	}
}

#endif