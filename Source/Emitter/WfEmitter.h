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
				typedef collections::Dictionary<Ptr<WfAttribute>, runtime::WfRuntimeValue>			AttributeValueMap;

			protected:
				analyzer::WfLexicalScopeManager*			manager;
				AttributeValueMap							attributeValues;				// cached value for attribute

			public:
				WfAttributeEvaluator(analyzer::WfLexicalScopeManager* _manager);

				Ptr<WfAttribute>							GetAttribute(collections::List<Ptr<WfAttribute>>& atts, const WString& category, const WString& name);
				collections::LazyList<Ptr<WfAttribute>>		GetAttributes(collections::List<Ptr<WfAttribute>>& atts, const WString& category, const WString& name);
				runtime::WfRuntimeValue						GetAttributeValue(Ptr<WfAttribute> att);
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
				typedef collections::List<glr::ParsingTextRange>					RangeMap;
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
				Ptr<WfCodegenScopeContext>			GetCurrentScopeContext(WfCodegenScopeType type, const WString& name = WString::Empty);
				Ptr<WfCodegenScopeContext>			PushScopeContext(WfCodegenScopeType type, const WString& name = WString::Empty);
				void								PopScopeContext();
			};

			class WfCodegenContext : public Object
			{
				typedef collections::Dictionary<analyzer::WfLexicalSymbol*, vint>								SymbolIndexMap;
				typedef collections::Dictionary<WfConstructorDeclaration*, vint>								ConstructorIndexMap;
				typedef collections::Dictionary<WfDestructorDeclaration*, vint>									DestructorIndexMap;
				typedef collections::Dictionary<glr::ParsingAstBase*, glr::ParsingTextRange>					NodePositionMap;
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

				vint								AddInstruction(glr::ParsingAstBase* node, const runtime::WfInstruction& ins);
				void								AddExitInstruction(glr::ParsingAstBase* node, const runtime::WfInstruction& ins);
				void								ApplyExitInstructions(Ptr<WfCodegenScopeContext> scopeContext);
				vint								GetThisStackCount(analyzer::WfLexicalScope* scope);
			};

			extern void										GenerateFunctionDeclarationMetadata(WfCodegenContext& context, WfFunctionDeclaration* node, Ptr<runtime::WfAssemblyFunction> meta);
			extern void										GenerateGlobalDeclarationMetadata(WfCodegenContext& context, WfDeclaration* declaration, const WString& namePrefix = L"");
			extern void										GenerateClosureInstructions(WfCodegenContext& context, Ptr<WfCodegenFunctionContext> functionContext);
			extern void										GenerateInitializeInstructions(WfCodegenContext& context, Ptr<WfDeclaration> declaration);
			extern void										GenerateDeclarationInstructions(WfCodegenContext& context, WfDeclaration* declaration);
			extern void										GenerateStatementInstructions(WfCodegenContext& context, Ptr<WfStatement> statement);
			extern Ptr<reflection::description::ITypeInfo>	GenerateExpressionInstructions(WfCodegenContext& context, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType = 0);
			extern void										GenerateTypeCastInstructions(WfCodegenContext& context, Ptr<reflection::description::ITypeInfo> expectedType, bool strongCast, WfExpression* node);
			extern void										GenerateTypeTestingInstructions(WfCodegenContext& context, Ptr<reflection::description::ITypeInfo> expectedType, WfExpression* node);
			extern runtime::WfInsType						GetInstructionTypeArgument(Ptr<reflection::description::ITypeInfo> expectedType);

			/// <summary>Generate an assembly from semantic analyzed modules. [M:vl.workflow.analyzer.WfLexicalScopeManager.Rebuild] should be called before using this function.</summary>
			/// <returns>The generated assembly.</returns>
			/// <param name="manager">The scope manager that is used to perform semantic analyzing.</param>
			/// <param name="callback">The callback to receive progress information (optional).</param>
			extern Ptr<runtime::WfAssembly>					GenerateAssembly(analyzer::WfLexicalScopeManager* manager, IWfCompilerCallback* callback = nullptr);

			/// <summary>Compile a Workflow program.</summary>
			/// <returns>The generated assembly. Return nullptr if failed to compile.</returns>
			/// <param name="manager">The scope manager. This is reusable, it is recommended to keep this object alive between multiple compiling to improve performance.</param>
			/// <param name="moduleCodes">All workflow module codes.</param>
			/// <param name="errors">Container to get all errors generated during compiling.</param>
			extern Ptr<runtime::WfAssembly>					Compile(analyzer::WfLexicalScopeManager* manager, collections::List<WString>& moduleCodes, collections::List<glr::ParsingError>& errors);
			
			/// <summary>Compile a Workflow program. Use the other one whenever possible, which alloes reusing <see cref="analyzer::WfLexicalScopeManager"/> to improve performance.</summary>
			/// <returns>The generated assembly. Return nullptr if failed to compile.</returns>
			/// <param name="workflowParser">The generated parser class.</param>
			/// <param name="moduleCodes">All workflow module codes.</param>
			/// <param name="errors">Container to get all errors generated during compiling.</param>
			extern Ptr<runtime::WfAssembly>					Compile(workflow::Parser& workflowParser, collections::List<WString>& moduleCodes, collections::List<glr::ParsingError>& errors);
		}
	}
}

#endif