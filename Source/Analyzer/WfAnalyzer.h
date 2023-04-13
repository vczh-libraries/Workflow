/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::Analyzer

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_ANALYZER_WFANALYZER
#define VCZH_WORKFLOW_ANALYZER_WFANALYZER

#include "../Library/WfLibraryReflection.h"
#include "../Parser/WfExpression.h"
#include "../Runtime/WfRuntime.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			class WfLexicalSymbol;
			class WfLexicalScope;
			class WfLexicalScopeManager;
		}

/***********************************************************************
Callback
***********************************************************************/

		/// <summary>A callback for tracking compiling progress. This is used in <see cref="analyzer::WfLexicalScopeManager::Rebuild"/> and <see cref="emitter::GenerateAssembly"/>.</summary>
		class IWfCompilerCallback : public Interface
		{
		public:
			/// <summary>1st step: Called before loading global type information.</summary>
			virtual void								OnLoadEnvironment() = 0;

			/// <summary>2nd step: Called before compiling.</summary>
			/// <param name="manager">The scope manager object for storing all information generated from Workflow modules during compiling.</param>
			virtual void								OnInitialize(analyzer::WfLexicalScopeManager* manager) = 0;

			/// <summary>3rd step: Called before validating a Workflow module. This could be called multiple times for each module.</summary>
			/// <param name="module">The module for validating.</param>
			virtual void								OnValidateModule(Ptr<WfModule> module) = 0;

			/// <summary>4th step: Called before generating metadata for functions and types created by given Workflow modules.</summary>
			virtual void								OnGenerateMetadata() = 0;

			/// <summary>5th step: Called before generating instructions for a Workflow module. This could be called multiple times for each module.</summary>
			/// <param name="module">The module for generating code.</param>
			virtual void								OnGenerateCode(Ptr<WfModule> module) = 0;

			/// <summary>6th step: Called before generating debug information.</summary>
			virtual void								OnGenerateDebugInfo() = 0;
		};

		namespace analyzer
		{
/***********************************************************************
Scope
***********************************************************************/

			class WfLexicalSymbol : public Object
			{
			public:
				WString										name;				// name of this symbol
				Ptr<WfType>									type;				// type of this symbol
				Ptr<reflection::description::ITypeInfo>		typeInfo;			// reflection type info of this symbol, nullable
				Ptr<glr::ParsingAstBase>					creatorNode;		// nullable
				WfLexicalScope*								ownerScope;			// scope that contains this symbol

				WfLexicalSymbol(WfLexicalScope* _ownerScope);
				~WfLexicalSymbol();
				
				WString										GetFriendlyName();
			};

			class WfLexicalFunctionConfig : public Object
			{
			public:
				bool										lambda = false;
				bool										thisAccessable = false;
				bool										parentThisAccessable = false;
			};

			class WfLexicalScope : public Object
			{
				typedef collections::Group<WString, Ptr<WfLexicalSymbol>>		TypeGroup;
			public:
				WfLexicalScopeManager*						ownerManager;				// nullable
				Ptr<glr::ParsingAstBase>					ownerNode;					// nullable
				glr::ParsingAstBase*						ownerNodeSource = nullptr;	// nullable

				Ptr<WfLexicalFunctionConfig>				functionConfig;
				reflection::description::ITypeDescriptor*	typeOfThisExpr = nullptr;	// visible members to this scope

				Ptr<WfLexicalScope>							parentScope;				// null means that this is the root scope
				TypeGroup									symbols;					// all symbols in this scope

				WfLexicalScope(WfLexicalScopeManager* _ownerManager);
				WfLexicalScope(Ptr<WfLexicalScope> _parentScope);
				~WfLexicalScope();

				WfLexicalScopeManager*						FindManager();
				Ptr<WfModule>								FindModule();
				WfLexicalScope*								FindFunctionScope();
				WString										GetFriendlyName();
			};

/***********************************************************************
Scope Manager
***********************************************************************/

			class WfLexicalScopeName : public Object
			{
				typedef collections::Dictionary<WString, Ptr<WfLexicalScopeName>>		NameMap;
				typedef collections::List<Ptr<WfDeclaration>>							DeclarationList;
			public:
				WfLexicalScopeName*							parent = nullptr;
				bool										imported = true;
				NameMap										children;
				WString										name;
				reflection::description::ITypeDescriptor*	typeDescriptor = nullptr;	// type that form this name
				DeclarationList								declarations;				// declarations that form this name

				WfLexicalScopeName(bool _imported);
				~WfLexicalScopeName();

				Ptr<WfLexicalScopeName>						AccessChild(const WString& name, bool imported);
				void										RemoveNonTypeDescriptorNames(WfLexicalScopeManager* manager);
				WString										GetFriendlyName();
			};

			struct ResolveExpressionResult
			{
				Ptr<WfLexicalScopeName>						scopeName;
				Ptr<WfLexicalSymbol>						symbol;
				reflection::description::IPropertyInfo*		propertyInfo = nullptr;
				reflection::description::IMethodInfo*		methodInfo = nullptr;
				reflection::description::IMethodInfo*		constructorInfo = nullptr;
				reflection::description::IEventInfo*		eventInfo = nullptr;
				Ptr<reflection::description::ITypeInfo>		type;
				Ptr<reflection::description::ITypeInfo>		writableType;
				Ptr<reflection::description::ITypeInfo>		expectedType;

				WString										GetFriendlyName(bool upperCase = false)const;

				static ResolveExpressionResult				ScopeName(Ptr<WfLexicalScopeName> _scopeName);
				static ResolveExpressionResult				ReadonlySymbol(Ptr<WfLexicalSymbol> _symbol);
				static ResolveExpressionResult				Symbol(Ptr<WfLexicalSymbol> _symbol);
				static ResolveExpressionResult				ReadonlyType(Ptr<reflection::description::ITypeInfo> _type);
				static ResolveExpressionResult				WritableType(Ptr<reflection::description::ITypeInfo> _type);
				static ResolveExpressionResult				Property(reflection::description::IPropertyInfo* _propertyInfo);
				static ResolveExpressionResult				Method(reflection::description::IMethodInfo* _methodInfo);
				static ResolveExpressionResult				Constructor(reflection::description::IMethodInfo* _constructorInfo);
				static ResolveExpressionResult				Event(reflection::description::IEventInfo* _eventInfo);

				bool										operator==(const ResolveExpressionResult& result) const;
				bool										operator!=(const ResolveExpressionResult& result) const;
			};

			struct WfLexicalCapture
			{
				collections::List<Ptr<WfLexicalSymbol>>		symbols;
				collections::List<Ptr<WfLexicalSymbol>>		ctorArgumentSymbols;
			};

			struct WfStateMachineInfo
			{
				Ptr<typeimpl::WfClassMethod>				createCoroutineMethod;
				collections::Dictionary<WString, vint>		inputIds;
				collections::Dictionary<WString, vint>		stateIds;
			};

			/// <summary>
			/// CPU architecture
			/// </summary>
			enum class WfCpuArchitecture
			{
				x86,
				x64,
				AsExecutable,
			};

			/// <summary>Scope manager for storing all information generated from Workflow modules during compiling.</summary>
			class WfLexicalScopeManager : public Object
			{
				typedef reflection::description::ITypeDescriptor											ITypeDescriptor;
				typedef reflection::description::IMemberInfo												IMemberInfo;
				typedef reflection::description::IMethodInfo												IMethodInfo;
				typedef reflection::description::ITypeInfo													ITypeInfo;
				typedef reflection::description::Value														Value;

				typedef collections::List<Ptr<WfModule>>													ModuleList;
				typedef collections::List<WString>															ModuleCodeList;
				typedef collections::Pair<WString, WString>													AttributeKey;
				typedef collections::Dictionary<AttributeKey, Ptr<ITypeInfo>>								AttributeTypeMap;

				typedef collections::Dictionary<ITypeDescriptor*, Ptr<WfLexicalScopeName>>					TypeNameMap;

				typedef collections::List<glr::ParsingError>												ParsingErrorList;
				typedef collections::Dictionary<Ptr<WfNamespaceDeclaration>, Ptr<WfLexicalScopeName>>		NamespaceNameMap;
				typedef collections::Dictionary<glr::ParsingAstBase*, Ptr<WfLexicalScope>>					NodeScopeMap;
				typedef collections::SortedList<WfLexicalScope*>											AnalyzedScopeList;

				typedef collections::Dictionary<Ptr<WfExpression>, ResolveExpressionResult>					ExpressionResolvingMap;
				typedef collections::Dictionary<Ptr<WfStatement>, ResolveExpressionResult>					CoOperatorResolvingMap;
				typedef collections::Pair<WfConstructorDeclaration*, ITypeDescriptor*>						BaseConstructorCallKey;
				typedef collections::Pair<WfBaseConstructorCall*, IMethodInfo*>								BaseConstructorCallValue;
				typedef collections::Dictionary<BaseConstructorCallKey, BaseConstructorCallValue>			BaseConstructorCallResolvingMap;

				typedef collections::Dictionary<glr::ParsingAstBase*, Ptr<WfLexicalCapture>>				LambdaCaptureMap;
				typedef collections::Dictionary<WfFunctionDeclaration*, IMethodInfo*>						InterfaceMethodImplementationMap;
				typedef collections::Dictionary<Ptr<WfDeclaration>, glr::ParsingAstBase*>					DeclarationSourceMap;
				typedef collections::Dictionary<Ptr<WfDeclaration>, Ptr<ITypeDescriptor>>					DeclarationTypeMap;
				typedef collections::Dictionary<Ptr<WfDeclaration>, Ptr<IMemberInfo>>						DeclarationMemberInfoMap;

				typedef collections::Dictionary<Ptr<WfStateInput>, Ptr<typeimpl::WfClassMethod>>			StateInputMethodMap;
				typedef collections::Dictionary<Ptr<WfFunctionArgument>, Ptr<typeimpl::WfField>>			StateArgumentFieldMap;
				typedef collections::Dictionary<Ptr<WfStateMachineDeclaration>, Ptr<WfStateMachineInfo>>	StateMachineInfoMap;

			protected:
				ModuleList									modules;
				ModuleCodeList								moduleCodes;
				vint										usedCodeIndex = 0;

			public:
				WfCpuArchitecture							cpuArchitecture = WfCpuArchitecture::AsExecutable;
				ITypeDescriptor*							cputdSInt = nullptr;
				ITypeDescriptor*							cputdUInt = nullptr;
				Ptr<ITypeInfo>								cputiSInt;
				Ptr<ITypeInfo>								cputiUInt;

				workflow::Parser&							workflowParser;
				Ptr<EventHandler>							workflowParserHandler;
				AttributeTypeMap							attributes;

				Ptr<WfLexicalScopeName>						globalName;							// root scope
				TypeNameMap									typeNames;							// ITypeDescriptor* to scope name map

				vint										usedTempVars = 0;
				ParsingErrorList							errors;								// compile errors
				NamespaceNameMap							namespaceNames;						// namespace to scope name map
				NodeScopeMap								nodeScopes;							// the nearest scope for a AST
				AnalyzedScopeList							checkedScopes_DuplicatedSymbol;		// scopes that validated by CheckScopes_DuplicatedSymbol
				AnalyzedScopeList							checkedScopes_SymbolType;			// scopes that validated by CheckScopes_SymbolType

				ExpressionResolvingMap						expressionResolvings;				// the resolving result for the expression
				ExpressionResolvingMap						coNewCoroutineResolvings;			// the coroutine type for the WfNewCoroutineExpression (e.g. AsyncCoroutine)
				CoOperatorResolvingMap						coOperatorResolvings;				// the method for the co-operator statement (e.g. AwaitAndRead)
				CoOperatorResolvingMap						coProviderResolvings;				// the constructor for the co-provider statement (e.g. AsyncCoroutine::Create)
				CoOperatorResolvingMap						coCastResultResolvings;				// the method for the co-operator's type casting (e.g. IStringAsync::CastResult)
				BaseConstructorCallResolvingMap				baseConstructorCallResolvings;		// all base constructor call resolvings

				LambdaCaptureMap							lambdaCaptures;						// all captured symbols in a lambda AST
				InterfaceMethodImplementationMap			interfaceMethodImpls;				// the IMethodInfo* that implemented by a function
				DeclarationSourceMap						declaractionScopeSources;			// the source used to build the scope for a declaration
				DeclarationTypeMap							declarationTypes;					// ITypeDescriptor* for type declaration
				DeclarationMemberInfoMap					declarationMemberInfos;				// IMemberInfo* for type description
				StateInputMethodMap							stateInputMethods;					// IMethodInfo* for state input
				StateArgumentFieldMap						stateInputArguments;				// IPropertyInfo* for state input argument temporary cache
				StateArgumentFieldMap						stateDeclArguments;					// IPropertyInfo* for state argument temporary cache
				StateMachineInfoMap							stateMachineInfos;					// members of state machine

				/// <summary>Create a Workflow compiler.</summary>
				/// <param name="_parsingTable">The workflow parser table. It can be retrived from [M:vl.workflow.WfLoadTable].</param>
				/// <param name="_cpuArchitecture">The target CPU architecture.</param>
				WfLexicalScopeManager(workflow::Parser& _workflowParser, WfCpuArchitecture _cpuArchitecture);
				~WfLexicalScopeManager();
				
				/// <summary>Add a Workflow module. Syntax errors can be found at <see cref="errors"/>.</summary>
				/// <param name="moduleCode">The source code of a workflow module.</param>
				/// <returns>Returns the code index, which is a number representing a module in data structured used in Workflow compiler, runtime and debugger.</returns>
				vint										AddModule(const WString& moduleCode);
				/// <summary>Add a parsed Workflow module.</summary>
				/// <param name="module">The syntax tree of a workflow module.</param>
				/// <returns>Returns the code index, which is a number representing a module in data structured used in Workflow compiler, runtime and debugger.</returns>
				vint										AddModule(Ptr<WfModule> module);
				/// <summary>Get all added modules in adding order.</summary>
				/// <returns>All added modules in adding order.</returns>
				ModuleList&									GetModules();
				/// <summary>Get all module codes in adding order. For any added parsed module, the code is an empty string.</summary>
				/// <returns>All module codes in adding order.</returns>
				ModuleCodeList&								GetModuleCodes();

				/// <summary>Clean all generated information from the last called to <see cref="Rebuild"/>.</summary>
				/// <param name="keepTypeDescriptorNames">
				/// Set to true so that only scopes for types in Workflow modules are deleted.
				/// Set to false to also delete scopes for cached global types.
				/// If there is no new reflectable native type that is registered after the last call to <see cref="Rebuild"/>,
				/// using true for this argument saves times by reusing scopes created from global types.
				/// </param>
				/// <param name="deleteModules">Set to true to delete all added modules.</param>
				void										Clear(bool keepTypeDescriptorNames, bool deleteModules);
				/// <summary>Perform semantic analyzing for added modules.</summary>
				/// <param name="keepTypeDescriptorNames">The same to the argument in <see cref="Clear"/>.</param>
				/// <param name="callback">The callback to receive progress information (optional).</param>
				void										Rebuild(bool keepTypeDescriptorNames, IWfCompilerCallback* callback = nullptr);

				bool										ResolveMember(ITypeDescriptor* typeDescriptor, const WString& name, bool preferStatic, collections::SortedList<ITypeDescriptor*>& searchedTypes, collections::List<ResolveExpressionResult>& results);
				bool										ResolveName(WfLexicalScope* scope, const WString& name, collections::List<ResolveExpressionResult>& results);
				Ptr<WfLexicalSymbol>						GetDeclarationSymbol(WfLexicalScope* scope, WfDeclaration* node);
				void										CreateLambdaCapture(glr::ParsingAstBase* node, Ptr<WfLexicalCapture> capture = nullptr);
			};

/***********************************************************************
Type Analyzing
***********************************************************************/

			enum class TypeFlag
			{
				Bool,
				I1,
				I2,
				I4,
				I8,
				U1,
				U2,
				U4,
				U8,
				F4,
				F8,
				String,
				Enum,
				Struct,
				Others,
				Count,
				Unknown = -1,
			};

			extern TypeFlag									GetTypeFlag(reflection::description::ITypeDescriptor* typeDescriptor);
			extern TypeFlag									GetTypeFlag(reflection::description::ITypeInfo* typeInfo);
			extern Ptr<reflection::description::ITypeInfo>	CreateTypeInfoFromTypeFlag(TypeFlag flag);

			extern void										GetTypeFragments(reflection::description::ITypeDescriptor* typeDescriptor, collections::List<WString>& fragments);
			extern Ptr<WfExpression>						GetExpressionFromTypeDescriptor(reflection::description::ITypeDescriptor* typeDescriptor);
			extern Ptr<WfType>								GetTypeFromTypeInfo(reflection::description::ITypeInfo* typeInfo);
			extern Ptr<WfLexicalScopeName>					GetScopeNameFromReferenceType(WfLexicalScope* scope, Ptr<WfType> type);
			extern Ptr<reflection::description::ITypeInfo>	CreateTypeInfoFromType(WfLexicalScope* scope, Ptr<WfType> type, bool checkTypeForValue = true);

			extern Ptr<reflection::description::ITypeInfo>	CopyTypeInfo(reflection::description::ITypeInfo* typeInfo);
			extern bool										CanConvertToType(reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType, bool explicitly);
			extern bool										IsSameType(reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType);
			extern Ptr<reflection::description::ITypeInfo>	GetMergedType(Ptr<reflection::description::ITypeInfo> firstType, Ptr<reflection::description::ITypeInfo> secondType);
			extern bool										IsNullAcceptableType(reflection::description::ITypeInfo* type);

			extern Ptr<reflection::description::ITypeInfo>	CreateTypeInfoFromMethodInfo(reflection::description::IMethodInfo* info);

			extern bool										IsExpressionDependOnExpectedType(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, bool hasExpectedType);
			extern WString									GetExpressionName(WfExpression* expression);
			extern WString									GetExpressionName(Ptr<WfExpression> expression);
			extern void										SearchOrderedName(WfLexicalScope* scope, WfExpression* expression, collections::SortedList<vint>& names);
			extern void										SearchOrderedName(WfLexicalScope* scope, Ptr<WfExpression> expression, collections::SortedList<vint>& names);

/***********************************************************************
Structure Analyzing
***********************************************************************/
			
			struct ValidateStructureContext
			{
				WfBindExpression*							currentBindExpression = nullptr;
				WfObserveExpression*						currentObserveExpression = nullptr;
				WfStatement*								currentLoopStatement = nullptr;
				WfStatement*								currentCatchStatement = nullptr;
				WfCoProviderStatement*						currentCoProviderStatement = nullptr;
				WfNewCoroutineExpression*					currentNewCoroutineExpression = nullptr;
				WfCoPauseStatement*							currentCoPauseStatement = nullptr;
				WfStateDeclaration*							currentStateDeclaration = nullptr;

				ValidateStructureContext();
			};

			enum class ValidateTypeStragety
			{
				Value,
				ReturnType,
				BaseType,
			};

			extern void										SetCodeRange(Ptr<WfType> node, glr::ParsingTextRange codeRange, bool asOffset = false);
			extern void										SetCodeRange(Ptr<WfExpression> node, glr::ParsingTextRange codeRange, bool asOffset = false);
			extern void										SetCodeRange(Ptr<WfStatement> node, glr::ParsingTextRange codeRange, bool asOffset = false);
			extern void										SetCodeRange(Ptr<WfDeclaration> node, glr::ParsingTextRange codeRange, bool asOffset = false);
			extern void										SetCodeRange(Ptr<WfModule> node, glr::ParsingTextRange codeRange, bool asOffset = false);

			extern void										ContextFreeModuleDesugar(WfLexicalScopeManager* manager, Ptr<WfModule> module);
			extern void										ContextFreeDeclarationDesugar(WfLexicalScopeManager* manager, Ptr<WfDeclaration> declaration);
			extern void										ContextFreeStatementDesugar(WfLexicalScopeManager* manager, Ptr<WfStatement> statement);
			extern void										ContextFreeExpressionDesugar(WfLexicalScopeManager* manager, Ptr<WfExpression> expression);

			extern void										ValidateTypeStructure(WfLexicalScopeManager* manager, Ptr<WfType> type, ValidateTypeStragety strategy = ValidateTypeStragety::Value, WfClassDeclaration* classDecl = nullptr);
			extern void										ValidateModuleStructure(WfLexicalScopeManager* manager, Ptr<WfModule> module);
			extern void										ValidateDeclarationStructure(WfLexicalScopeManager* manager, Ptr<WfDeclaration> declaration, WfClassDeclaration* classDecl = nullptr , WfExpression* surroundingLambda = nullptr);
			extern void										ValidateStatementStructure(WfLexicalScopeManager* manager, ValidateStructureContext* context, Ptr<WfStatement>& statement);
			extern void										ValidateExpressionStructure(WfLexicalScopeManager* manager, ValidateStructureContext* context, Ptr<WfExpression>& expression);

/***********************************************************************
Global Name
***********************************************************************/

			extern void										BuildGlobalNameFromTypeDescriptors(WfLexicalScopeManager* manager);
			extern void										BuildNameForDeclaration(WfLexicalScopeManager* manager, Ptr<WfLexicalScopeName> name, Ptr<WfDeclaration> decl);
			extern void										BuildGlobalNameFromModules(WfLexicalScopeManager* manager);
			extern void										ValidateScopeName(WfLexicalScopeManager* manager, Ptr<WfLexicalScopeName> name);

/***********************************************************************
Scope Analyzing
***********************************************************************/
			
			extern void										CompleteScopeForClassMember(WfLexicalScopeManager* manager, Ptr<typeimpl::WfCustomType> td, Ptr<WfDeclaration> memberDecl);
			extern void										CompleteScopeForDeclaration(WfLexicalScopeManager* manager, WfDeclaration* declaration);
			extern void										CompleteScopeForModule(WfLexicalScopeManager* manager, Ptr<WfModule> module);

			extern void										BuildScopeForModule(WfLexicalScopeManager* manager, Ptr<WfModule> module);
			extern void										BuildScopeForDeclaration(WfLexicalScopeManager* manager, Ptr<WfLexicalScope> parentScope, Ptr<WfDeclaration> declaration, glr::ParsingAstBase* source);
			extern void										BuildScopeForStatement(WfLexicalScopeManager* manager, Ptr<WfLexicalScope> parentScope, Ptr<WfStatement> statement);
			extern void										BuildScopeForExpression(WfLexicalScopeManager* manager, Ptr<WfLexicalScope> parentScope, Ptr<WfExpression> expression);
			extern bool										CheckScopes_DuplicatedSymbol(WfLexicalScopeManager* manager);
			extern bool										CheckScopes_SymbolType(WfLexicalScopeManager* manager);
			extern bool										CheckScopes_CycleDependency(WfLexicalScopeManager* manager);

/***********************************************************************
Semantic Analyzing
***********************************************************************/

			extern reflection::description::IMethodInfo*	FindInterfaceConstructor(reflection::description::ITypeDescriptor* type);
			extern Ptr<reflection::description::ITypeInfo>	SelectFunction(WfLexicalScopeManager* manager, glr::ParsingAstBase* node, Ptr<WfExpression> functionExpression, collections::List<ResolveExpressionResult>& functions, collections::List<Ptr<WfExpression>>& arguments, vint& selectedFunctionIndex);

			extern void										ValidateModuleSemantic(WfLexicalScopeManager* manager, Ptr<WfModule> module);
			extern void										ValidateClassMemberSemantic(WfLexicalScopeManager* manager, Ptr<typeimpl::WfCustomType> td, WfClassDeclaration* classDecl, Ptr<WfDeclaration> memberDecl);
			extern void										ValidateDeclarationSemantic(WfLexicalScopeManager* manager, WfDeclaration* declaration);
			extern void										ValidateDeclarationSemantic(WfLexicalScopeManager* manager, Ptr<WfDeclaration> declaration);
			extern void										ValidateStatementSemantic(WfLexicalScopeManager* manager, WfStatement* statement);
			extern void										ValidateStatementSemantic(WfLexicalScopeManager* manager, Ptr<WfStatement> statement);
			extern void										ValidateExpressionSemantic(WfLexicalScopeManager* manager, WfExpression* expression, Ptr<reflection::description::ITypeInfo> expectedType, collections::List<ResolveExpressionResult>& results);
			extern void										ValidateExpressionSemantic(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType, collections::List<ResolveExpressionResult>& results);
			extern void										ValidateConstantExpression(WfLexicalScopeManager* manager, WfExpression* expression, Ptr<reflection::description::ITypeInfo> expectedType);
			extern void										ValidateConstantExpression(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType);

			extern Ptr<WfLexicalScopeName>					GetExpressionScopeName(WfLexicalScopeManager* manager, WfExpression* expression);
			extern Ptr<WfLexicalScopeName>					GetExpressionScopeName(WfLexicalScopeManager* manager, Ptr<WfExpression> expression);
			extern reflection::description::IEventInfo*		GetExpressionEventInfo(WfLexicalScopeManager* manager, WfExpression* expression);
			extern reflection::description::IEventInfo*		GetExpressionEventInfo(WfLexicalScopeManager* manager, Ptr<WfExpression> expression);
			extern void										GetExpressionTypes(WfLexicalScopeManager* manager, WfExpression* expression, Ptr<reflection::description::ITypeInfo> expectedType, bool allowEvent, collections::List<ResolveExpressionResult>& results);
			extern void										GetExpressionTypes(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType, bool allowEvent, collections::List<ResolveExpressionResult>& results);
			extern Ptr<reflection::description::ITypeInfo>	GetExpressionType(WfLexicalScopeManager* manager, WfExpression* expression, Ptr<reflection::description::ITypeInfo> expectedType);
			extern Ptr<reflection::description::ITypeInfo>	GetExpressionType(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType);
			extern Ptr<reflection::description::ITypeInfo>	GetLeftValueExpressionType(WfLexicalScopeManager* manager, WfExpression* expression);
			extern Ptr<reflection::description::ITypeInfo>	GetLeftValueExpressionType(WfLexicalScopeManager* manager, Ptr<WfExpression> expression);
			extern Ptr<reflection::description::ITypeInfo>	GetEnumerableExpressionItemType(WfLexicalScopeManager* manager, WfExpression* expression, Ptr<reflection::description::ITypeInfo> expectedType);
			extern Ptr<reflection::description::ITypeInfo>	GetEnumerableExpressionItemType(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType);

/***********************************************************************
Expanding Virtual Nodes
***********************************************************************/

			class CopyWithExpandVirtualVisitor : public copy_visitor::AstVisitor
			{
			private:
				bool										expandVirtualAst;

				void										Expand(collections::List<Ptr<WfDeclaration>>& decls);

				void										Visit(WfVirtualCfeExpression* node)override;
				void										Visit(WfVirtualCseExpression* node)override;
				void										Visit(WfVirtualCseStatement* node)override;

				void										Visit(WfNamespaceDeclaration* node)override;
				void										Visit(WfClassDeclaration* node)override;
				void										Visit(WfNewInterfaceExpression* node)override;

			public:
				CopyWithExpandVirtualVisitor(bool _expandVirtualAst);
			};

			extern Ptr<WfStatement>							SearchUntilNonVirtualStatement(Ptr<WfStatement> statement);
			extern Ptr<WfExpression>						CreateDefaultValue(reflection::description::ITypeInfo* elementType);

			extern Ptr<WfType>								CopyType(Ptr<WfType> type);
			extern Ptr<WfExpression>						CopyExpression(Ptr<WfExpression> expression, bool expandVirtualExprStat);
			extern Ptr<WfStatement>							CopyStatement(Ptr<WfStatement> statement, bool expandVirtualExprStat);
			extern Ptr<WfDeclaration>						CopyDeclaration(Ptr<WfDeclaration> declaration, bool expandVirtualExprStat);
			extern Ptr<WfModule>							CopyModule(Ptr<WfModule> module, bool expandVirtualExprStat);

			extern Ptr<WfType>								CopyType(WfType* type);
			extern Ptr<WfExpression>						CopyExpression(WfExpression* expression, bool expandVirtualExprStat);
			extern Ptr<WfStatement>							CopyStatement(WfStatement* statement, bool expandVirtualExprStat);
			extern Ptr<WfDeclaration>						CopyDeclaration(WfDeclaration* declaration, bool expandVirtualExprStat);
			extern Ptr<WfModule>							CopyModule(WfModule* module, bool expandVirtualExprStat);

			extern void										ExpandBindExpression(WfLexicalScopeManager* manager, WfBindExpression* node);
			extern void										ExpandNewCoroutineExpression(WfLexicalScopeManager* manager, WfNewCoroutineExpression* node);
			extern void										ExpandCoOperatorExpression(WfLexicalScopeManager* manager, WfCoOperatorExpression* node);
			extern void										ExpandMixinCastExpression(WfLexicalScopeManager* manager, WfMixinCastExpression* node);
			extern void										ExpandSwitchStatement(WfLexicalScopeManager* manager, WfSwitchStatement* node);
			extern void										ExpandForEachStatement(WfLexicalScopeManager* manager, WfForEachStatement* node);
			extern void										ExpandCoProviderStatement(WfLexicalScopeManager* manager, WfCoProviderStatement* node);
			extern void										ExpandStateMachine(WfLexicalScopeManager* manager, WfStateMachineDeclaration* node);

/***********************************************************************
Error Messages
***********************************************************************/

			struct WfErrors
			{
				// A: Expression error
				static glr::ParsingError					WrongFormatStringSyntax(WfExpression* node);
				static glr::ParsingError					WrongSimpleObserveExpression(WfExpression* node);
				static glr::ParsingError					WrongSimpleObserveEvent(WfExpression* node);
				static glr::ParsingError					EmptyObserveEvent(WfExpression* node);
				static glr::ParsingError					ObserveNotInBind(WfExpression* node);
				static glr::ParsingError					ObserveInObserveEvent(WfExpression* node);
				static glr::ParsingError					BindInBind(WfExpression* node);
				static glr::ParsingError					AttachInBind(WfExpression* node);
				static glr::ParsingError					DetachInBind(WfExpression* node);
				static glr::ParsingError					ConstructorMixMapAndList(WfExpression* node);
				static glr::ParsingError					ConstructorMixStructAndList(WfExpression* node);
				static glr::ParsingError					DuplicatedConstructorField(WfReferenceExpression* node);
				static glr::ParsingError					ConstructorMixClassAndInterface(WfNewClassExpression* node);
				static glr::ParsingError					ConstructorMixClassAndInterface(WfNewInterfaceExpression* node);
				static glr::ParsingError					ScopeNameIsNotExpression(WfExpression* node, Ptr<WfLexicalScopeName> scopeName);
				static glr::ParsingError					EventIsNotExpression(WfExpression* node, reflection::description::IEventInfo* eventInfo);
				static glr::ParsingError					ExpressionIsNotScopeName(WfExpression* node);
				static glr::ParsingError					ExpressionIsNotEvent(WfExpression* node);
				static glr::ParsingError					ExpressionCannotResolveType(WfExpression* node, Ptr<WfLexicalSymbol> symbol);
				static glr::ParsingError					NullCannotResolveType(WfExpression* node);
				static glr::ParsingError					ConstructorCannotResolveType(WfExpression* node);
				static glr::ParsingError					OrderedLambdaCannotResolveType(WfExpression* node);
				static glr::ParsingError					NullCannotImplicitlyConvertToType(WfExpression* node, reflection::description::ITypeInfo* toType);
				static glr::ParsingError					ConstructorCannotImplicitlyConvertToType(WfExpression* node, reflection::description::ITypeInfo* toType);
				static glr::ParsingError					OrderedLambdaCannotImplicitlyConvertToType(WfExpression* node, reflection::description::ITypeInfo* toType);
				static glr::ParsingError					ExpressionCannotImplicitlyConvertToType(WfExpression* node, reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType);
				static glr::ParsingError					ExpressionCannotExplicitlyConvertToType(WfExpression* node, reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType);
				static glr::ParsingError					CannotWeakCastToType(WfExpression* node, reflection::description::ITypeInfo* toType);
				static glr::ParsingError					IntegerLiteralOutOfRange(WfIntegerExpression* node);
				static glr::ParsingError					FloatingLiteralOutOfRange(WfFloatingExpression* node);
				static glr::ParsingError					CannotMergeTwoType(WfExpression* node, reflection::description::ITypeInfo* firstType, reflection::description::ITypeInfo* secondType);
				static glr::ParsingError					RangeShouldBeInteger(WfExpression* node, reflection::description::ITypeInfo* type);
				static glr::ParsingError					UnaryOperatorOnWrongType(WfUnaryExpression* node, reflection::description::ITypeInfo* type);
				static glr::ParsingError					BinaryOperatorOnWrongType(WfBinaryExpression* node, reflection::description::ITypeInfo* type);
				static glr::ParsingError					IndexOperatorOnWrongType(WfBinaryExpression* node, reflection::description::ITypeInfo* containerType);
				static glr::ParsingError					ExpressionIsNotCollection(WfExpression* node, reflection::description::ITypeInfo* type);
				static glr::ParsingError					ExpressionIsNotFunction(WfExpression* node, reflection::description::ITypeInfo* type);
				static glr::ParsingError					FunctionArgumentCountMismatched(glr::ParsingAstBase* node, vint expectedCount, const ResolveExpressionResult& function);
				static glr::ParsingError					FunctionArgumentTypeMismatched(glr::ParsingAstBase* node, const ResolveExpressionResult& function, vint index, reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType);
				static glr::ParsingError					CannotPickOverloadedFunctions(glr::ParsingAstBase* node, collections::List<ResolveExpressionResult>& results);
				static glr::ParsingError					ClassContainsNoConstructor(WfExpression* node, reflection::description::ITypeInfo* type);
				static glr::ParsingError					InterfaceContainsNoConstructor(WfExpression* node, reflection::description::ITypeInfo* type);
				static glr::ParsingError					ConstructorReturnTypeMismatched(WfExpression* node, const ResolveExpressionResult& function, reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType);
				static glr::ParsingError					ExpressionIsNotLeftValue(WfExpression* node, const ResolveExpressionResult& result);
				static glr::ParsingError					CannotCallMemberOutsideOfClass(WfExpression* node, const ResolveExpressionResult& result);
				static glr::ParsingError					CannotCallMemberInStaticFunction(WfExpression* node, const ResolveExpressionResult& result);
				static glr::ParsingError					FieldCannotInitializeUsingEachOther(WfExpression* node, const ResolveExpressionResult& result);
				static glr::ParsingError					WrongThisExpression(WfExpression* node);
				static glr::ParsingError					IncorrectTypeForUnion(WfExpression* node, reflection::description::ITypeInfo* type);
				static glr::ParsingError					IncorrectTypeForIntersect(WfExpression* node, reflection::description::ITypeInfo* type);
				static glr::ParsingError					ExpressionIsNotConstant(WfExpression* node);
				static glr::ParsingError					WrongMixinTargetType(WfExpression* node, reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType);
				static glr::ParsingError					ExpectedTypeCastCannotResolveType(WfExpression* node);

				// B: Type error
				static glr::ParsingError					WrongVoidType(WfType* node);
				static glr::ParsingError					WrongInterfaceType(WfType* node);
				static glr::ParsingError					RawPointerToNonReferenceType(WfType* node, reflection::description::ITypeInfo* typeInfo = 0);
				static glr::ParsingError					SharedPointerToNonReferenceType(WfType* node, reflection::description::ITypeInfo* typeInfo = 0);
				static glr::ParsingError					NullableToNonReferenceType(WfType* node, reflection::description::ITypeInfo* typeInfo = 0);
				static glr::ParsingError					ChildOfNonReferenceType(WfType* node);
				static glr::ParsingError					TypeNotExists(WfType* node, Ptr<WfLexicalScopeName> scopeName);
				static glr::ParsingError					TypeNotExists(WfType* node, const ResolveExpressionResult& result);
				static glr::ParsingError					TypeNotForValue(WfType* node, reflection::description::ITypeInfo* typeInfo);
				
				// C: Statement error
				static glr::ParsingError					BreakNotInLoop(WfStatement* node);
				static glr::ParsingError					ContinueNotInLoop(WfStatement* node);
				static glr::ParsingError					RethrowNotInCatch(WfStatement* node);
				static glr::ParsingError					TryMissCatchAndFinally(WfStatement* node);
				static glr::ParsingError					ReturnMissExpression(WfStatement* node, reflection::description::ITypeInfo* type);
				static glr::ParsingError					DeleteNonRawPointer(WfStatement* node, reflection::description::ITypeInfo* type);
				static glr::ParsingError					CannotReturnExpression(WfStatement* node);
				static glr::ParsingError					WrongCoPause(WfStatement* node);
				static glr::ParsingError					WrongCoOperator(WfStatement* node);
				static glr::ParsingError					WrongCoOperator(WfExpression* node);
				static glr::ParsingError					CoProviderNotExists(WfCoProviderStatement* node, collections::List<WString>& candidates);
				static glr::ParsingError					CoOperatorNotExists(WfReturnStatement* node, reflection::description::ITypeInfo* type);
				static glr::ParsingError					CoOperatorNotExists(WfCoOperatorStatement* node, reflection::description::ITypeInfo* type);
				static glr::ParsingError					CoOperatorNotExists(WfCoOperatorExpression* node, reflection::description::ITypeInfo* type);
				static glr::ParsingError					CoOperatorCannotResolveResultType(WfCoOperatorStatement* node, collections::List<reflection::description::ITypeInfo*>& types);
				static glr::ParsingError					CoProviderCreateNotExists(WfCoProviderStatement* node, reflection::description::ITypeInfo* type);
				static glr::ParsingError					CoProviderCreateAndRunNotExists(WfCoProviderStatement* node, reflection::description::ITypeInfo* type);
				static glr::ParsingError					GotoLabelNotExists(WfGotoStatement* node);
				static glr::ParsingError					TooManyGotoLabel(WfGotoStatement* node);
				static glr::ParsingError					WrongStateSwitchStatement(WfStateSwitchStatement* node);
				static glr::ParsingError					WrongStateInvokeStatement(WfStateInvokeStatement* node);
				static glr::ParsingError					StateInputNotExists(WfStateSwitchCase* node);
				static glr::ParsingError					StateSwitchArgumentCountNotMatch(WfStateSwitchCase* node);
				static glr::ParsingError					StateNotExists(WfStateInvokeStatement* node);
				static glr::ParsingError					StateArgumentCountNotMatch(WfStateInvokeStatement* node);

				// D: Declaration error
				static glr::ParsingError					FunctionShouldHaveName(WfDeclaration* node);
				static glr::ParsingError					FunctionShouldHaveImplementation(WfDeclaration* node);
				static glr::ParsingError					InterfaceMethodShouldNotHaveImplementation(WfDeclaration* node);
				static glr::ParsingError					DuplicatedDeclaration(WfDeclaration* node, const WString& firstDeclarationCategory);
				static glr::ParsingError					DuplicatedDeclaration(WfStateMachineDeclaration* node);
				static glr::ParsingError					DuplicatedSymbol(WfDeclaration* node, Ptr<WfLexicalSymbol> symbol);
				static glr::ParsingError					DuplicatedSymbol(WfFunctionArgument* node, Ptr<WfLexicalSymbol> symbol);
				static glr::ParsingError					DuplicatedSymbol(WfStatement* node, Ptr<WfLexicalSymbol> symbol);
				static glr::ParsingError					DuplicatedSymbol(WfExpression* node, Ptr<WfLexicalSymbol> symbol);
				static glr::ParsingError					DuplicatedSymbol(WfStateInput* node, Ptr<WfLexicalSymbol> symbol);
				static glr::ParsingError					DuplicatedSymbol(WfStateDeclaration* node, Ptr<WfLexicalSymbol> symbol);
				static glr::ParsingError					DuplicatedSymbol(WfStateSwitchArgument* node, Ptr<WfLexicalSymbol> symbol);
				static glr::ParsingError					InterfaceMethodNotImplemented(WfNewInterfaceExpression* node, reflection::description::IMethodInfo* method);
				static glr::ParsingError					InterfaceMethodNotFound(WfFunctionDeclaration* node, reflection::description::ITypeInfo* interfaceType, reflection::description::ITypeInfo* methodType);
				static glr::ParsingError					CannotPickOverloadedInterfaceMethods(WfExpression* node, collections::List<ResolveExpressionResult>& results);
				static glr::ParsingError					CannotPickOverloadedImplementMethods(WfFunctionDeclaration* node, reflection::description::ITypeInfo* type);
				static glr::ParsingError					WrongDeclaration(WfEventDeclaration* node);
				static glr::ParsingError					WrongDeclaration(WfPropertyDeclaration* node);
				static glr::ParsingError					WrongDeclaration(WfConstructorDeclaration* node);
				static glr::ParsingError					WrongDeclaration(WfDestructorDeclaration* node);
				static glr::ParsingError					WrongDeclaration(WfAutoPropertyDeclaration* node);
				static glr::ParsingError					WrongDeclaration(WfStateMachineDeclaration* node);
				static glr::ParsingError					WrongDeclaration(WfStaticInitDeclaration* node);
				static glr::ParsingError					WrongDeclarationInInterfaceConstructor(WfDeclaration* node);
				static glr::ParsingError					EnumValuesNotConsecutiveFromZero(WfEnumDeclaration* node);
				static glr::ParsingError					FlagValuesNotConsecutiveFromZero(WfEnumDeclaration* node);
				static glr::ParsingError					FlagValueNotExists(WfEnumItemIntersection* node, WfEnumDeclaration* owner);
				static glr::ParsingError					DuplicatedEnumValue(WfEnumItem* node, WfEnumDeclaration* owner);
				static glr::ParsingError					StructContainsNonValueType(WfStructMember* node, WfStructDeclaration* owner);
				static glr::ParsingError					StructRecursivelyIncludeItself(WfStructDeclaration* node, collections::List<reflection::description::ITypeDescriptor*>& tds);
				static glr::ParsingError					DuplicatedStructMember(WfStructMember* node, WfStructDeclaration* owner);
				static glr::ParsingError					AttributeNotExists(WfAttribute* node);
				static glr::ParsingError					AttributeMissValue(WfAttribute* node);
				static glr::ParsingError					StateMachineClassNotInheritFromStateMachine(WfClassDeclaration* node);
				static glr::ParsingError					MissingDefaultState(WfStateMachineDeclaration* node);

				// E: Module error
				static glr::ParsingError					WrongUsingPathWildCard(WfModuleUsingPath* node);

				// F: Symbol Error
				static glr::ParsingError					TopQualifiedSymbolNotExists(glr::ParsingAstBase* node, const WString& name);
				static glr::ParsingError					ChildSymbolNotExists(glr::ParsingAstBase* node, Ptr<WfLexicalScopeName> scopeName, const WString& name);
				static glr::ParsingError					MemberNotExists(glr::ParsingAstBase* node, reflection::description::ITypeDescriptor* typeDescriptor, const WString& name);
				static glr::ParsingError					ReferenceNotExists(glr::ParsingAstBase* node, const WString& name);
				static glr::ParsingError					TooManyTargets(glr::ParsingAstBase* node, collections::List<ResolveExpressionResult>& results, const WString& name);
				static glr::ParsingError					EnumItemNotExists(glr::ParsingAstBase* node, reflection::description::ITypeDescriptor* typeDescriptor, const WString& name);

				// G: Class error
				static glr::ParsingError					WrongClassMemberConfig(WfDeclaration* node);
				static glr::ParsingError					FunctionInNewTypeExpressionCannotBeStatic(WfDeclaration* node);
				static glr::ParsingError					AutoPropertyCannotBeNormalOutsideOfClass(WfDeclaration* node);
				static glr::ParsingError					AutoPropertyCannotBeStatic(WfDeclaration* node);
				static glr::ParsingError					WrongClassMember(WfNamespaceDeclaration* node);
				static glr::ParsingError					PropertyGetterNotFound(WfPropertyDeclaration* node, WfClassDeclaration* classDecl);
				static glr::ParsingError					PropertySetterNotFound(WfPropertyDeclaration* node, WfClassDeclaration* classDecl);
				static glr::ParsingError					PropertyEventNotFound(WfPropertyDeclaration* node, WfClassDeclaration* classDecl);
				static glr::ParsingError					TooManyPropertyGetter(WfPropertyDeclaration* node, WfClassDeclaration* classDecl);
				static glr::ParsingError					TooManyPropertySetter(WfPropertyDeclaration* node, WfClassDeclaration* classDecl);
				static glr::ParsingError					TooManyPropertyEvent(WfPropertyDeclaration* node, WfClassDeclaration* classDecl);
				static glr::ParsingError					PropertyGetterTypeMismatched(WfPropertyDeclaration* node, WfClassDeclaration* classDecl);
				static glr::ParsingError					PropertySetterTypeMismatched(WfPropertyDeclaration* node, WfClassDeclaration* classDecl);
				static glr::ParsingError					WrongBaseType(WfClassDeclaration* node, WfType* type);
				static glr::ParsingError					WrongBaseTypeOfClass(WfClassDeclaration* node, reflection::description::ITypeDescriptor* type);
				static glr::ParsingError					WrongBaseTypeOfInterface(WfClassDeclaration* node, reflection::description::ITypeDescriptor* type);
				static glr::ParsingError					WrongInterfaceBaseType(WfClassDeclaration* node, reflection::description::ITypeDescriptor* type);
				static glr::ParsingError					ClassWithInterfaceConstructor(WfClassDeclaration* node);
				static glr::ParsingError					OverrideShouldImplementInterfaceMethod(WfFunctionDeclaration* node);
				static glr::ParsingError					OverrideShouldImplementInterfaceMethod(WfAutoPropertyDeclaration* node);
				static glr::ParsingError					MissingFieldType(WfVariableDeclaration* node);
				static glr::ParsingError					DuplicatedBaseClass(WfClassDeclaration* node, reflection::description::ITypeDescriptor* type);
				static glr::ParsingError					ClassRecursiveInheritance(WfClassDeclaration* node, collections::List<reflection::description::ITypeDescriptor*>& tds);
				static glr::ParsingError					InterfaceRecursiveInheritance(WfClassDeclaration* node, collections::List<reflection::description::ITypeDescriptor*>& tds);
				static glr::ParsingError					WrongBaseConstructorCall(WfBaseConstructorCall* node, reflection::description::ITypeDescriptor* type);
				static glr::ParsingError					DuplicatedBaseConstructorCall(WfBaseConstructorCall* node, reflection::description::ITypeDescriptor* type);
				static glr::ParsingError					TooManyDestructor(WfDestructorDeclaration* node, WfClassDeclaration* classDecl);
				static glr::ParsingError					AutoPropertyShouldBeInitialized(WfAutoPropertyDeclaration* node);
				static glr::ParsingError					AutoPropertyCannotBeInitializedInInterface(WfAutoPropertyDeclaration* node, WfClassDeclaration* classDecl);

				// CPP: C++ code generation error
				static glr::ParsingError					CppUnableToDecideClassOrder(WfClassDeclaration* node, collections::List<reflection::description::ITypeDescriptor*>& tds);
				static glr::ParsingError					CppUnableToSeparateCustomFile(WfClassDeclaration* node, collections::List<reflection::description::ITypeDescriptor*>& tds);
			};
		}
	}
}

#endif