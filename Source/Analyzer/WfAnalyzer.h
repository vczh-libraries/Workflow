/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::Analyzer

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_ANALYZER_WFANALYZER
#define VCZH_WORKFLOW_ANALYZER_WFANALYZER

#include "../Expression/WfExpression.h"
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

/***********************************************************************
Scope
***********************************************************************/

			class WfLexicalSymbol : public Object
			{
			public:
				WString										name;				// name of this symbol
				Ptr<WfType>									type;				// type of this symbol
				Ptr<reflection::description::ITypeInfo>		typeInfo;			// reflection type info of this symbol, nullable
				Ptr<parsing::ParsingTreeCustomBase>			creatorNode;		// nullable
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
				Ptr<parsing::ParsingTreeCustomBase>			ownerNode;					// nullable
				parsing::ParsingTreeCustomBase*				ownerNodeSource = nullptr;	// nullable

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
				Ptr<WfClassMember>							GetOwnerClassMember();
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
			};

			/// <summary>Workflow compiler.</summary>
			class WfLexicalScopeManager : public Object
			{
				typedef reflection::description::ITypeDescriptor											ITypeDescriptor;
				typedef reflection::description::IMemberInfo												IMemberInfo;
				typedef reflection::description::IMethodInfo												IMethodInfo;
				typedef reflection::description::ITypeInfo													ITypeInfo;
				typedef reflection::description::Value														Value;

				typedef collections::List<Ptr<WfModule>>													ModuleList;
				typedef collections::List<WString>															ModuleCodeList;
				typedef collections::List<Ptr<parsing::ParsingError>>										ParsingErrorList;
				typedef collections::Dictionary<Ptr<WfNamespaceDeclaration>, Ptr<WfLexicalScopeName>>		NamespaceNameMap;
				typedef collections::Dictionary<ITypeDescriptor*, Ptr<WfLexicalScopeName>>					TypeNameMap;
				typedef collections::Dictionary<parsing::ParsingTreeCustomBase*, Ptr<WfLexicalScope>>		NodeScopeMap;
				typedef collections::Dictionary<Ptr<WfExpression>, ResolveExpressionResult>					ExpressionResolvingMap;
				typedef collections::Dictionary<Ptr<WfStatement>, ResolveExpressionResult>					CoOperatorResolvingMap;
				typedef collections::Dictionary<parsing::ParsingTreeCustomBase*, Ptr<WfLexicalCapture>>		LambdaCaptureMap;
				typedef collections::Dictionary<WfFunctionDeclaration*, IMethodInfo*>						InterfaceMethodImplementationMap;
				typedef collections::Dictionary<Ptr<WfDeclaration>, Ptr<ITypeDescriptor>>					DeclarationTypeMap;
				typedef collections::Dictionary<Ptr<WfDeclaration>, Ptr<IMemberInfo>>						DeclarationMemberInfoMap;

				typedef collections::Pair<WfConstructorDeclaration*, ITypeDescriptor*>						BaseConstructorCallKey;
				typedef collections::Pair<WfBaseConstructorCall*, IMethodInfo*>								BaseConstructorCallValue;
				typedef collections::Dictionary<BaseConstructorCallKey, BaseConstructorCallValue>			BaseConstructorCallResolvingMap;

				typedef collections::Pair<WString, WString>													AttributeKey;
				typedef collections::Dictionary<AttributeKey, Ptr<ITypeInfo>>								AttributeTypeMap;
			protected:
				ModuleList									modules;
				ModuleCodeList								moduleCodes;
				vint										usedCodeIndex = 0;

			public:
				Ptr<parsing::tabling::ParsingTable>			parsingTable;
				AttributeTypeMap							attributes;
				vint										usedTempVars = 0;

				ParsingErrorList							errors;							// compile errors

				Ptr<WfLexicalScopeName>						globalName;						// root scope
				NamespaceNameMap							namespaceNames;					// namespace to scope name map
				TypeNameMap									typeNames;						// ITypeDescriptor* to scope name map

				NodeScopeMap								nodeScopes;						// the nearest scope for a AST
				ExpressionResolvingMap						expressionResolvings;			// the resolving result for the expression
				CoOperatorResolvingMap						coOperatorResolvings;			// the resolving result for the co-operator statement
				LambdaCaptureMap							lambdaCaptures;					// all captured symbols in a lambda AST
				InterfaceMethodImplementationMap			interfaceMethodImpls;			// the IMethodInfo* that implemented by a function
				DeclarationTypeMap							declarationTypes;				// ITypeDescriptor* for type declaration
				DeclarationMemberInfoMap					declarationMemberInfos;			// IMemberInfo* for type description
				BaseConstructorCallResolvingMap				baseConstructorCallResolvings;	// all base constructor call resolvings

				/// <summary>Create a Workflow compiler.</summary>
				/// <param name="_parsingTable">The workflow parser table. It can be retrived from [M:vl.workflow.WfLoadTable].</param>
				WfLexicalScopeManager(Ptr<parsing::tabling::ParsingTable> _parsingTable);
				~WfLexicalScopeManager();
				
				/// <summary>Add a Workflow module. Syntax errors can be found at <see cref="errors"/>.</summary>
				/// <param name="moduleCode">The source code of a workflow module.</param>
				/// <returns>Returns the code index, which is a number representing a module in data structured used in Workflow compiler, runtime and debugger.</returns>
				vint										AddModule(const WString& moduleCode);
				/// <summary>Add a Workflow module.</summary>
				/// <param name="module">The syntax tree of a workflow module.</param>
				/// <returns>Returns the code index, which is a number representing a module in data structured used in Workflow compiler, runtime and debugger.</returns>
				vint										AddModule(Ptr<WfModule> module);
				/// <summary>Get all added modules.</summary>
				/// <returns>All added modules.</returns>
				ModuleList&									GetModules();
				/// <summary>Get all module codes. If a module is added from a syntax tree, then the source code is empty.</summary>
				/// <returns>All module codes.</returns>
				ModuleCodeList&								GetModuleCodes();

				/// <summary>Clean compiling results.</summary>
				/// <param name="keepTypeDescriptorNames">Set to false to delete all cache of reflectable C++ types.</param>
				/// <param name="deleteModules">Set to true to delete all added modules.</param>
				void										Clear(bool keepTypeDescriptorNames, bool deleteModules);
				/// <summary>Compile.</summary>
				/// <param name="keepTypeDescriptorNames">Set to false to delete all cache of reflectable C++ types before compiling.</param>
				void										Rebuild(bool keepTypeDescriptorNames);

				bool										ResolveMember(ITypeDescriptor* typeDescriptor, const WString& name, bool preferStatic, collections::SortedList<ITypeDescriptor*>& searchedTypes, collections::List<ResolveExpressionResult>& results);
				bool										ResolveName(WfLexicalScope* scope, const WString& name, collections::List<ResolveExpressionResult>& results);
				Ptr<WfLexicalSymbol>						GetDeclarationSymbol(WfLexicalScope* scope, WfDeclaration* node);
				void										CreateLambdaCapture(parsing::ParsingTreeCustomBase* node, Ptr<WfLexicalCapture> capture = nullptr);
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
#ifdef VCZH_64
				I = I8,
				U = U8,
#else
				I = I4,
				U = U4,
#endif
			};

			extern TypeFlag									GetTypeFlag(reflection::description::ITypeDescriptor* typeDescriptor);
			extern TypeFlag									GetTypeFlag(reflection::description::ITypeInfo* typeInfo);
			extern Ptr<reflection::description::ITypeInfo>	CreateTypeInfoFromTypeFlag(TypeFlag flag);

			extern Ptr<WfType>								GetTypeFromTypeInfo(reflection::description::ITypeInfo* typeInfo);
			extern Ptr<WfLexicalScopeName>					GetScopeNameFromReferenceType(WfLexicalScope* scope, Ptr<WfType> type);
			extern Ptr<reflection::description::ITypeInfo>	CreateTypeInfoFromType(WfLexicalScope* scope, Ptr<WfType> type);

			extern Ptr<reflection::description::ITypeInfo>	CopyTypeInfo(reflection::description::ITypeInfo* typeInfo);
			extern bool										CanConvertToType(reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType, bool explicitly);
			extern bool										IsSameType(reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType);
			extern Ptr<reflection::description::ITypeInfo>	GetMergedType(Ptr<reflection::description::ITypeInfo> firstType, Ptr<reflection::description::ITypeInfo> secondType);
			extern bool										IsNullAcceptableType(reflection::description::ITypeInfo* type);

			extern Ptr<reflection::description::ITypeInfo>	CreateTypeInfoFromMethodInfo(reflection::description::IMethodInfo* info);

			extern bool										IsExpressionDependOnExpectedType(WfLexicalScopeManager* manager, Ptr<WfExpression> expression);
			extern WString									GetExpressionName(Ptr<WfExpression> expression);
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

				ValidateStructureContext();
			};

			enum class ValidateTypeStragety
			{
				Value,
				ReturnType,
				BaseType,
			};

			extern void										SetCodeRange(Ptr<WfType> node, parsing::ParsingTextRange codeRange);
			extern void										SetCodeRange(Ptr<WfExpression> node, parsing::ParsingTextRange codeRange);
			extern void										SetCodeRange(Ptr<WfStatement> node, parsing::ParsingTextRange codeRange);
			extern void										SetCodeRange(Ptr<WfDeclaration> node, parsing::ParsingTextRange codeRange);
			extern void										SetCodeRange(Ptr<WfModule> node, parsing::ParsingTextRange codeRange);

			extern void										ContextFreeModuleDesugar(WfLexicalScopeManager* manager, Ptr<WfModule> module);
			extern void										ContextFreeDeclarationDesugar(WfLexicalScopeManager* manager, Ptr<WfDeclaration> declaration);
			extern void										ContextFreeStatementDesugar(WfLexicalScopeManager* manager, Ptr<WfStatement> statement);
			extern void										ContextFreeExpressionDesugar(WfLexicalScopeManager* manager, Ptr<WfExpression> expression);

			extern void										ValidateTypeStructure(WfLexicalScopeManager* manager, Ptr<WfType> type, ValidateTypeStragety strategy = ValidateTypeStragety::Value, WfClassDeclaration* classDecl = nullptr);
			extern void										ValidateModuleStructure(WfLexicalScopeManager* manager, Ptr<WfModule> module);
			extern void										ValidateDeclarationStructure(WfLexicalScopeManager* manager, Ptr<WfDeclaration> declaration, WfClassDeclaration* classDecl = 0 , WfExpression* surroundingLambda = 0);
			extern void										ValidateStatementStructure(WfLexicalScopeManager* manager, ValidateStructureContext* context, Ptr<WfStatement>& statement);
			extern void										ValidateExpressionStructure(WfLexicalScopeManager* manager, ValidateStructureContext* context, Ptr<WfExpression>& expression);

/***********************************************************************
Global Name
***********************************************************************/

			extern void										BuildGlobalNameFromTypeDescriptors(WfLexicalScopeManager* manager);
			extern void										BuildNameForDeclaration(WfLexicalScopeManager* manager, Ptr<WfLexicalScopeName> name, WfDeclaration* decl);
			extern void										BuildGlobalNameFromModules(WfLexicalScopeManager* manager);
			extern void										ValidateScopeName(WfLexicalScopeManager* manager, Ptr<WfLexicalScopeName> name);

/***********************************************************************
Scope Analyzing
***********************************************************************/
			
			extern void										CompleteScopeForClassMember(WfLexicalScopeManager* manager, Ptr<typeimpl::WfCustomType> td, Ptr<WfClassDeclaration> classDecl, Ptr<WfDeclaration> memberDecl);
			extern void										CompleteScopeForDeclaration(WfLexicalScopeManager* manager, Ptr<WfDeclaration> declaration);
			extern void										CompleteScopeForModule(WfLexicalScopeManager* manager, Ptr<WfModule> module);

			extern void										BuildScopeForModule(WfLexicalScopeManager* manager, Ptr<WfModule> module);
			extern void										BuildScopeForDeclaration(WfLexicalScopeManager* manager, Ptr<WfLexicalScope> parentScope, Ptr<WfDeclaration> declaration, parsing::ParsingTreeCustomBase* source);
			extern void										BuildScopeForStatement(WfLexicalScopeManager* manager, Ptr<WfLexicalScope> parentScope, Ptr<WfStatement> statement);
			extern void										BuildScopeForExpression(WfLexicalScopeManager* manager, Ptr<WfLexicalScope> parentScope, Ptr<WfExpression> expression);
			extern bool										CheckScopes_DuplicatedSymbol(WfLexicalScopeManager* manager);
			extern bool										CheckScopes_SymbolType(WfLexicalScopeManager* manager);
			extern bool										CheckScopes_BaseType(WfLexicalScopeManager* manager);

/***********************************************************************
Semantic Analyzing
***********************************************************************/

			class WfObservingDependency : public Object
			{
				typedef collections::Group<WfExpression*, WfExpression*>			DependencyGroup;
				typedef collections::List<WfExpression*>							ObserveList;
			public:
				ObserveList							inputObserves;
				ObserveList							outputObserves;
				DependencyGroup&					dependencies;
				
				WfObservingDependency(WfObservingDependency& dependency);
				WfObservingDependency(DependencyGroup& _dependencies);
				WfObservingDependency(DependencyGroup& _dependencies, ObserveList& _inputObserves);
				
				void								Prepare(WfExpression* observe);
				void								AddInternal(WfExpression* observe, WfExpression* dependedObserve);
				void								Add(WfExpression* observe);
				void								Add(WfExpression* observe, WfObservingDependency& dependency);
				void								TurnToInput();
				void								Cleanup();
			};

			extern void										ValidateModuleSemantic(WfLexicalScopeManager* manager, Ptr<WfModule> module);
			extern void										ValidateClassMemberSemantic(WfLexicalScopeManager* manager, Ptr<typeimpl::WfCustomType> td, Ptr<WfClassDeclaration> classDecl, Ptr<WfDeclaration> memberDecl);
			extern void										ValidateDeclarationSemantic(WfLexicalScopeManager* manager, Ptr<WfDeclaration> declaration);
			extern void										ValidateStatementSemantic(WfLexicalScopeManager* manager, Ptr<WfStatement> statement);
			extern void										ValidateExpressionSemantic(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType, collections::List<ResolveExpressionResult>& results);
			extern void										ValidateConstantExpression(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType);
			extern void										GetObservingDependency(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, WfObservingDependency& dependency);

			extern Ptr<WfLexicalScopeName>					GetExpressionScopeName(WfLexicalScopeManager* manager, Ptr<WfExpression> expression);
			extern reflection::description::IEventInfo*		GetExpressionEventInfo(WfLexicalScopeManager* manager, Ptr<WfExpression> expression);
			extern void										GetExpressionTypes(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType, bool allowEvent, collections::List<ResolveExpressionResult>& results);
			extern Ptr<reflection::description::ITypeInfo>	GetExpressionType(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType);
			extern Ptr<reflection::description::ITypeInfo>	GetLeftValueExpressionType(WfLexicalScopeManager* manager, Ptr<WfExpression> expression);
			extern Ptr<reflection::description::ITypeInfo>	GetEnumerableExpressionItemType(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType);

/***********************************************************************
Expanding Virtual Nodes
***********************************************************************/

			extern Ptr<WfExpression>						ExpandObserveExpression(WfExpression* expression, collections::Dictionary<WfExpression*, WString>& cacheNames, collections::Dictionary<WString, WString>& referenceReplacement, bool useCache = true);
			extern Ptr<WfType>								CopyType(Ptr<WfType> type);
			extern Ptr<WfExpression>						CopyExpression(Ptr<WfExpression> expression);
			extern Ptr<WfStatement>							CopyStatement(Ptr<WfStatement> statement);
			extern Ptr<WfDeclaration>						CopyDeclaration(Ptr<WfDeclaration> declaration);
			extern Ptr<WfExpression>						CreateDefaultValue(reflection::description::ITypeInfo* elementType);

			extern void										ExpandBindExpression(WfLexicalScopeManager* manager, WfBindExpression* node);
			extern void										ExpandNewCoroutineExpression(WfLexicalScopeManager* manager, WfNewCoroutineExpression* node);
			extern void										ExpandSwitchStatement(WfLexicalScopeManager* manager, WfSwitchStatement* node);
			extern void										ExpandForEachStatement(WfLexicalScopeManager* manager, WfForEachStatement* node);

/***********************************************************************
Error Messages
***********************************************************************/

			struct WfErrors
			{
				// A: Expression error
				static Ptr<parsing::ParsingError>			WrongFormatStringSyntax(WfExpression* node);
				static Ptr<parsing::ParsingError>			WrongSimpleObserveExpression(WfExpression* node);
				static Ptr<parsing::ParsingError>			WrongSimpleObserveEvent(WfExpression* node);
				static Ptr<parsing::ParsingError>			EmptyObserveEvent(WfExpression* node);
				static Ptr<parsing::ParsingError>			ObserveNotInBind(WfExpression* node);
				static Ptr<parsing::ParsingError>			ObserveInObserveEvent(WfExpression* node);
				static Ptr<parsing::ParsingError>			BindInBind(WfExpression* node);
				static Ptr<parsing::ParsingError>			AttachInBind(WfExpression* node);
				static Ptr<parsing::ParsingError>			DetachInBind(WfExpression* node);
				static Ptr<parsing::ParsingError>			ConstructorMixMapAndList(WfExpression* node);
				static Ptr<parsing::ParsingError>			ConstructorMixStructAndList(WfExpression* node);
				static Ptr<parsing::ParsingError>			DuplicatedConstructorField(WfReferenceExpression* node);
				static Ptr<parsing::ParsingError>			ConstructorMixClassAndInterface(WfNewClassExpression* node);
				static Ptr<parsing::ParsingError>			ConstructorMixClassAndInterface(WfNewInterfaceExpression* node);
				static Ptr<parsing::ParsingError>			ScopeNameIsNotExpression(WfExpression* node, Ptr<WfLexicalScopeName> scopeName);
				static Ptr<parsing::ParsingError>			EventIsNotExpression(WfExpression* node, reflection::description::IEventInfo* eventInfo);
				static Ptr<parsing::ParsingError>			ExpressionIsNotScopeName(WfExpression* node);
				static Ptr<parsing::ParsingError>			ExpressionIsNotEvent(WfExpression* node);
				static Ptr<parsing::ParsingError>			ExpressionCannotResolveType(WfExpression* node, Ptr<WfLexicalSymbol> symbol);
				static Ptr<parsing::ParsingError>			NullCannotResolveType(WfExpression* node);
				static Ptr<parsing::ParsingError>			ConstructorCannotResolveType(WfExpression* node);
				static Ptr<parsing::ParsingError>			OrderedLambdaCannotResolveType(WfExpression* node);
				static Ptr<parsing::ParsingError>			NullCannotImplicitlyConvertToType(WfExpression* node, reflection::description::ITypeInfo* toType);
				static Ptr<parsing::ParsingError>			ConstructorCannotImplicitlyConvertToType(WfExpression* node, reflection::description::ITypeInfo* toType);
				static Ptr<parsing::ParsingError>			OrderedLambdaCannotImplicitlyConvertToType(WfExpression* node, reflection::description::ITypeInfo* toType);
				static Ptr<parsing::ParsingError>			ExpressionCannotImplicitlyConvertToType(WfExpression* node, reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType);
				static Ptr<parsing::ParsingError>			ExpressionCannotExplicitlyConvertToType(WfExpression* node, reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType);
				static Ptr<parsing::ParsingError>			CannotWeakCastToType(WfExpression* node, reflection::description::ITypeInfo* toType);
				static Ptr<parsing::ParsingError>			IntegerLiteralOutOfRange(WfIntegerExpression* node);
				static Ptr<parsing::ParsingError>			CannotMergeTwoType(WfExpression* node, reflection::description::ITypeInfo* firstType, reflection::description::ITypeInfo* secondType);
				static Ptr<parsing::ParsingError>			RangeShouldBeInteger(WfExpression* node, reflection::description::ITypeInfo* type);
				static Ptr<parsing::ParsingError>			UnaryOperatorOnWrongType(WfUnaryExpression* node, reflection::description::ITypeInfo* type);
				static Ptr<parsing::ParsingError>			BinaryOperatorOnWrongType(WfBinaryExpression* node, reflection::description::ITypeInfo* type);
				static Ptr<parsing::ParsingError>			IndexOperatorOnWrongType(WfBinaryExpression* node, reflection::description::ITypeInfo* containerType);
				static Ptr<parsing::ParsingError>			ExpressionIsNotCollection(WfExpression* node, reflection::description::ITypeInfo* type);
				static Ptr<parsing::ParsingError>			ExpressionIsNotFunction(WfExpression* node, reflection::description::ITypeInfo* type);
				static Ptr<parsing::ParsingError>			FunctionArgumentCountMismatched(parsing::ParsingTreeCustomBase* node, vint expectedCount, const ResolveExpressionResult& function);
				static Ptr<parsing::ParsingError>			FunctionArgumentTypeMismatched(parsing::ParsingTreeCustomBase* node, const ResolveExpressionResult& function, vint index, reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType);
				static Ptr<parsing::ParsingError>			CannotPickOverloadedFunctions(parsing::ParsingTreeCustomBase* node, collections::List<ResolveExpressionResult>& results);
				static Ptr<parsing::ParsingError>			ClassContainsNoConstructor(WfExpression* node, reflection::description::ITypeInfo* type);
				static Ptr<parsing::ParsingError>			InterfaceContainsNoConstructor(WfExpression* node, reflection::description::ITypeInfo* type);
				static Ptr<parsing::ParsingError>			ConstructorReturnTypeMismatched(WfExpression* node, const ResolveExpressionResult& function, reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType);
				static Ptr<parsing::ParsingError>			ExpressionIsNotLeftValue(WfExpression* node, const ResolveExpressionResult& result);
				static Ptr<parsing::ParsingError>			CannotCallMemberOutsideOfClass(WfExpression* node, const ResolveExpressionResult& result);
				static Ptr<parsing::ParsingError>			CannotCallMemberInStaticFunction(WfExpression* node, const ResolveExpressionResult& result);
				static Ptr<parsing::ParsingError>			FieldCannotInitializeUsingEachOther(WfExpression* node, const ResolveExpressionResult& result);
				static Ptr<parsing::ParsingError>			WrongThisExpression(WfExpression* node);
				static Ptr<parsing::ParsingError>			IncorrectTypeForUnion(WfExpression* node, reflection::description::ITypeInfo* type);
				static Ptr<parsing::ParsingError>			IncorrectTypeForIntersect(WfExpression* node, reflection::description::ITypeInfo* type);
				static Ptr<parsing::ParsingError>			ExpressionIsNotConstant(WfExpression* node);

				// B: Type error
				static Ptr<parsing::ParsingError>			WrongVoidType(WfType* node);
				static Ptr<parsing::ParsingError>			WrongInterfaceType(WfType* node);
				static Ptr<parsing::ParsingError>			RawPointerToNonReferenceType(WfType* node, reflection::description::ITypeInfo* typeInfo = 0);
				static Ptr<parsing::ParsingError>			SharedPointerToNonReferenceType(WfType* node, reflection::description::ITypeInfo* typeInfo = 0);
				static Ptr<parsing::ParsingError>			NullableToNonReferenceType(WfType* node, reflection::description::ITypeInfo* typeInfo = 0);
				static Ptr<parsing::ParsingError>			ChildOfNonReferenceType(WfType* node);
				static Ptr<parsing::ParsingError>			TypeNotExists(WfType* node, Ptr<WfLexicalScopeName> scopeName);
				static Ptr<parsing::ParsingError>			TypeNotExists(WfType* node, Ptr<WfLexicalSymbol> symbol);
				static Ptr<parsing::ParsingError>			TypeNotForValue(WfType* node, reflection::description::ITypeInfo* typeInfo);
				
				// C: Statement error
				static Ptr<parsing::ParsingError>			BreakNotInLoop(WfStatement* node);
				static Ptr<parsing::ParsingError>			ContinueNotInLoop(WfStatement* node);
				static Ptr<parsing::ParsingError>			RethrowNotInCatch(WfStatement* node);
				static Ptr<parsing::ParsingError>			TryMissCatchAndFinally(WfStatement* node);
				static Ptr<parsing::ParsingError>			ReturnMissExpression(WfStatement* node, reflection::description::ITypeInfo* type);
				static Ptr<parsing::ParsingError>			DeleteNonRawPointer(WfStatement* node, reflection::description::ITypeInfo* type);
				static Ptr<parsing::ParsingError>			CannotReturnExpression(WfStatement* node);
				static Ptr<parsing::ParsingError>			WrongCoPause(WfStatement* node);
				static Ptr<parsing::ParsingError>			WrongCoOperator(WfStatement* node);
				static Ptr<parsing::ParsingError>			CoProviderNotExists(WfCoProviderStatement* node, collections::List<WString>& candidates);
				static Ptr<parsing::ParsingError>			CoOperatorNotExists(WfReturnStatement* node, reflection::description::ITypeInfo* type);
				static Ptr<parsing::ParsingError>			CoOperatorNotExists(WfCoOperatorStatement* node, reflection::description::ITypeInfo* type);

				// D: Declaration error
				static Ptr<parsing::ParsingError>			FunctionShouldHaveName(WfDeclaration* node);
				static Ptr<parsing::ParsingError>			FunctionShouldHaveImplementation(WfDeclaration* node);
				static Ptr<parsing::ParsingError>			InterfaceMethodShouldNotHaveImplementation(WfDeclaration* node);
				static Ptr<parsing::ParsingError>			DuplicatedDeclaration(WfDeclaration* node, const WString& firstDeclarationCategory);
				static Ptr<parsing::ParsingError>			DuplicatedSymbol(WfDeclaration* node, Ptr<WfLexicalSymbol> symbol);
				static Ptr<parsing::ParsingError>			DuplicatedSymbol(WfFunctionArgument* node, Ptr<WfLexicalSymbol> symbol);
				static Ptr<parsing::ParsingError>			DuplicatedSymbol(WfStatement* node, Ptr<WfLexicalSymbol> symbol);
				static Ptr<parsing::ParsingError>			DuplicatedSymbol(WfExpression* node, Ptr<WfLexicalSymbol> symbol);
				static Ptr<parsing::ParsingError>			InterfaceMethodNotImplemented(WfNewInterfaceExpression* node, reflection::description::IMethodInfo* method);
				static Ptr<parsing::ParsingError>			InterfaceMethodNotFound(WfFunctionDeclaration* node, reflection::description::ITypeInfo* interfaceType, reflection::description::ITypeInfo* methodType);
				static Ptr<parsing::ParsingError>			CannotPickOverloadedInterfaceMethods(WfExpression* node, collections::List<ResolveExpressionResult>& results);
				static Ptr<parsing::ParsingError>			CannotPickOverloadedImplementMethods(WfFunctionDeclaration* node, reflection::description::ITypeInfo* type);
				static Ptr<parsing::ParsingError>			WrongDeclaration(WfEventDeclaration* node);
				static Ptr<parsing::ParsingError>			WrongDeclaration(WfPropertyDeclaration* node);
				static Ptr<parsing::ParsingError>			WrongDeclaration(WfConstructorDeclaration* node);
				static Ptr<parsing::ParsingError>			WrongDeclaration(WfDestructorDeclaration* node);
				static Ptr<parsing::ParsingError>			WrongDeclaration(WfAutoPropertyDeclaration* node);
				static Ptr<parsing::ParsingError>			WrongDeclarationInInterfaceConstructor(WfDeclaration* node);
				static Ptr<parsing::ParsingError>			EnumValuesNotConsecutiveFromZero(WfEnumDeclaration* node);
				static Ptr<parsing::ParsingError>			FlagValuesNotConsecutiveFromZero(WfEnumDeclaration* node);
				static Ptr<parsing::ParsingError>			FlagValueNotExists(WfEnumItemIntersection* node, WfEnumDeclaration* owner);
				static Ptr<parsing::ParsingError>			DuplicatedEnumValue(WfEnumItem* node, WfEnumDeclaration* owner);
				static Ptr<parsing::ParsingError>			StructContainsNonValueType(WfStructMember* node, WfStructDeclaration* owner);
				static Ptr<parsing::ParsingError>			StructRecursivelyIncludeItself(WfStructDeclaration* node, const WString& path);
				static Ptr<parsing::ParsingError>			DuplicatedStructMember(WfStructMember* node, WfStructDeclaration* owner);
				static Ptr<parsing::ParsingError>			AttributeNotExists(WfAttribute* node);
				static Ptr<parsing::ParsingError>			AttributeMissValue(WfAttribute* node);

				// E: Module error
				static Ptr<parsing::ParsingError>			WrongUsingPathWildCard(WfModuleUsingPath* node);

				// F: Symbol Error
				static Ptr<parsing::ParsingError>			TopQualifiedSymbolNotExists(parsing::ParsingTreeCustomBase* node, const WString& name);
				static Ptr<parsing::ParsingError>			ChildSymbolNotExists(parsing::ParsingTreeCustomBase* node, Ptr<WfLexicalScopeName> scopeName, const WString& name);
				static Ptr<parsing::ParsingError>			MemberNotExists(parsing::ParsingTreeCustomBase* node, reflection::description::ITypeDescriptor* typeDescriptor, const WString& name);
				static Ptr<parsing::ParsingError>			ReferenceNotExists(parsing::ParsingTreeCustomBase* node, const WString& name);
				static Ptr<parsing::ParsingError>			TooManyTargets(parsing::ParsingTreeCustomBase* node, collections::List<ResolveExpressionResult>& results, const WString& name);
				static Ptr<parsing::ParsingError>			EnumItemNotExists(parsing::ParsingTreeCustomBase* node, reflection::description::ITypeDescriptor* typeDescriptor, const WString& name);

				// G: Class error
				static Ptr<parsing::ParsingError>			WrongClassMemberConfig(WfDeclaration* node);
				static Ptr<parsing::ParsingError>			FunctionInNewTypeExpressionCannotBeStatic(WfDeclaration* node);
				static Ptr<parsing::ParsingError>			AutoPropertyCannotBeNormalOutsideOfClass(WfDeclaration* node);
				static Ptr<parsing::ParsingError>			AutoPropertyCannotBeStatic(WfDeclaration* node);
				static Ptr<parsing::ParsingError>			WrongClassMember(WfNamespaceDeclaration* node);
				static Ptr<parsing::ParsingError>			PropertyGetterNotFound(WfPropertyDeclaration* node, WfClassDeclaration* classDecl);
				static Ptr<parsing::ParsingError>			PropertySetterNotFound(WfPropertyDeclaration* node, WfClassDeclaration* classDecl);
				static Ptr<parsing::ParsingError>			PropertyEventNotFound(WfPropertyDeclaration* node, WfClassDeclaration* classDecl);
				static Ptr<parsing::ParsingError>			TooManyPropertyGetter(WfPropertyDeclaration* node, WfClassDeclaration* classDecl);
				static Ptr<parsing::ParsingError>			TooManyPropertySetter(WfPropertyDeclaration* node, WfClassDeclaration* classDecl);
				static Ptr<parsing::ParsingError>			TooManyPropertyEvent(WfPropertyDeclaration* node, WfClassDeclaration* classDecl);
				static Ptr<parsing::ParsingError>			PropertyGetterTypeMismatched(WfPropertyDeclaration* node, WfClassDeclaration* classDecl);
				static Ptr<parsing::ParsingError>			PropertySetterTypeMismatched(WfPropertyDeclaration* node, WfClassDeclaration* classDecl);
				static Ptr<parsing::ParsingError>			WrongBaseType(WfClassDeclaration* node, WfType* type);
				static Ptr<parsing::ParsingError>			WrongBaseTypeOfClass(WfClassDeclaration* node, reflection::description::ITypeDescriptor* type);
				static Ptr<parsing::ParsingError>			WrongBaseTypeOfInterface(WfClassDeclaration* node, reflection::description::ITypeDescriptor* type);
				static Ptr<parsing::ParsingError>			WrongInterfaceBaseType(WfClassDeclaration* node, reflection::description::ITypeDescriptor* type);
				static Ptr<parsing::ParsingError>			ClassWithInterfaceConstructor(WfClassDeclaration* node);
				static Ptr<parsing::ParsingError>			OverrideShouldImplementInterfaceMethod(WfFunctionDeclaration* node);
				static Ptr<parsing::ParsingError>			OverrideShouldImplementInterfaceMethod(WfAutoPropertyDeclaration* node);
				static Ptr<parsing::ParsingError>			MissingFieldType(WfVariableDeclaration* node);
				static Ptr<parsing::ParsingError>			DuplicatedBaseClass(WfClassDeclaration* node, reflection::description::ITypeDescriptor* type);
				static Ptr<parsing::ParsingError>			DuplicatedBaseInterface(WfClassDeclaration* node, reflection::description::ITypeDescriptor* type);
				static Ptr<parsing::ParsingError>			WrongBaseConstructorCall(WfBaseConstructorCall* node, reflection::description::ITypeDescriptor* type);
				static Ptr<parsing::ParsingError>			DuplicatedBaseConstructorCall(WfBaseConstructorCall* node, reflection::description::ITypeDescriptor* type);
				static Ptr<parsing::ParsingError>			TooManyDestructor(WfDestructorDeclaration* node, WfClassDeclaration* classDecl);
				static Ptr<parsing::ParsingError>			AutoPropertyShouldBeInitialized(WfAutoPropertyDeclaration* node);
				static Ptr<parsing::ParsingError>			AutoPropertyCannotBeInitializedInInterface(WfAutoPropertyDeclaration* node, WfClassDeclaration* classDecl);
			};
		}
	}
}

#endif