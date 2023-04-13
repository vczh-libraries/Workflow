/***********************************************************************
This file is generated by: Vczh Parser Generator
From parser definition:Ast
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_WORKFLOW_PARSER_AST_AST_BUILDER
#define VCZH_WORKFLOW_PARSER_AST_AST_BUILDER

#include "WorkflowAst.h"

namespace vl::workflow::builder
{
	class MakeAttachEventExpression : public vl::glr::ParsingAstBuilder<WfAttachEventExpression>
	{
	public:
		MakeAttachEventExpression& event(const vl::Ptr<WfExpression>& value);
		MakeAttachEventExpression& function(const vl::Ptr<WfExpression>& value);
	};

	class MakeAttribute : public vl::glr::ParsingAstBuilder<WfAttribute>
	{
	public:
		MakeAttribute& category(const vl::WString& value);
		MakeAttribute& name(const vl::WString& value);
		MakeAttribute& value(const vl::Ptr<WfExpression>& value);
	};

	class MakeAutoPropertyDeclaration : public vl::glr::ParsingAstBuilder<WfAutoPropertyDeclaration>
	{
	public:
		MakeAutoPropertyDeclaration& configConst(WfAPConst value);
		MakeAutoPropertyDeclaration& configObserve(WfAPObserve value);
		MakeAutoPropertyDeclaration& expression(const vl::Ptr<WfExpression>& value);
		MakeAutoPropertyDeclaration& functionKind(WfFunctionKind value);
		MakeAutoPropertyDeclaration& type(const vl::Ptr<WfType>& value);
		MakeAutoPropertyDeclaration& expandedDeclarations(const vl::Ptr<WfDeclaration>& value);
		MakeAutoPropertyDeclaration& attributes(const vl::Ptr<WfAttribute>& value);
		MakeAutoPropertyDeclaration& name(const vl::WString& value);
	};

	class MakeBaseConstructorCall : public vl::glr::ParsingAstBuilder<WfBaseConstructorCall>
	{
	public:
		MakeBaseConstructorCall& arguments(const vl::Ptr<WfExpression>& value);
		MakeBaseConstructorCall& type(const vl::Ptr<WfType>& value);
	};

	class MakeBinaryExpression : public vl::glr::ParsingAstBuilder<WfBinaryExpression>
	{
	public:
		MakeBinaryExpression& first(const vl::Ptr<WfExpression>& value);
		MakeBinaryExpression& op(WfBinaryOperator value);
		MakeBinaryExpression& second(const vl::Ptr<WfExpression>& value);
	};

	class MakeBindExpression : public vl::glr::ParsingAstBuilder<WfBindExpression>
	{
	public:
		MakeBindExpression& expression(const vl::Ptr<WfExpression>& value);
		MakeBindExpression& expandedExpression(const vl::Ptr<WfExpression>& value);
	};

	class MakeBlockStatement : public vl::glr::ParsingAstBuilder<WfBlockStatement>
	{
	public:
		MakeBlockStatement& endLabel(const vl::WString& value);
		MakeBlockStatement& statements(const vl::Ptr<WfStatement>& value);
	};

	class MakeCallExpression : public vl::glr::ParsingAstBuilder<WfCallExpression>
	{
	public:
		MakeCallExpression& arguments(const vl::Ptr<WfExpression>& value);
		MakeCallExpression& function(const vl::Ptr<WfExpression>& value);
	};

	class MakeCastResultInterfaceDeclaration : public vl::glr::ParsingAstBuilder<WfCastResultInterfaceDeclaration>
	{
	public:
		MakeCastResultInterfaceDeclaration& baseType(const vl::Ptr<WfType>& value);
		MakeCastResultInterfaceDeclaration& elementType(const vl::Ptr<WfType>& value);
		MakeCastResultInterfaceDeclaration& expandedDeclarations(const vl::Ptr<WfDeclaration>& value);
		MakeCastResultInterfaceDeclaration& attributes(const vl::Ptr<WfAttribute>& value);
		MakeCastResultInterfaceDeclaration& name(const vl::WString& value);
	};

	class MakeChildExpression : public vl::glr::ParsingAstBuilder<WfChildExpression>
	{
	public:
		MakeChildExpression& name(const vl::WString& value);
		MakeChildExpression& parent(const vl::Ptr<WfExpression>& value);
	};

	class MakeChildType : public vl::glr::ParsingAstBuilder<WfChildType>
	{
	public:
		MakeChildType& name(const vl::WString& value);
		MakeChildType& parent(const vl::Ptr<WfType>& value);
	};

	class MakeClassDeclaration : public vl::glr::ParsingAstBuilder<WfClassDeclaration>
	{
	public:
		MakeClassDeclaration& baseTypes(const vl::Ptr<WfType>& value);
		MakeClassDeclaration& constructorType(WfConstructorType value);
		MakeClassDeclaration& declarations(const vl::Ptr<WfDeclaration>& value);
		MakeClassDeclaration& kind(WfClassKind value);
		MakeClassDeclaration& attributes(const vl::Ptr<WfAttribute>& value);
		MakeClassDeclaration& name(const vl::WString& value);
	};

	class MakeCoOperatorExpression : public vl::glr::ParsingAstBuilder<WfCoOperatorExpression>
	{
	public:
		MakeCoOperatorExpression& name(const vl::WString& value);
		MakeCoOperatorExpression& expandedExpression(const vl::Ptr<WfExpression>& value);
	};

	class MakeCoOperatorStatement : public vl::glr::ParsingAstBuilder<WfCoOperatorStatement>
	{
	public:
		MakeCoOperatorStatement& arguments(const vl::Ptr<WfExpression>& value);
		MakeCoOperatorStatement& opName(const vl::WString& value);
		MakeCoOperatorStatement& varName(const vl::WString& value);
	};

	class MakeCoPauseStatement : public vl::glr::ParsingAstBuilder<WfCoPauseStatement>
	{
	public:
		MakeCoPauseStatement& statement(const vl::Ptr<WfStatement>& value);
	};

	class MakeCoProviderStatement : public vl::glr::ParsingAstBuilder<WfCoProviderStatement>
	{
	public:
		MakeCoProviderStatement& name(const vl::WString& value);
		MakeCoProviderStatement& statement(const vl::Ptr<WfStatement>& value);
		MakeCoProviderStatement& expandedStatement(const vl::Ptr<WfStatement>& value);
	};

	class MakeConstructorArgument : public vl::glr::ParsingAstBuilder<WfConstructorArgument>
	{
	public:
		MakeConstructorArgument& key(const vl::Ptr<WfExpression>& value);
		MakeConstructorArgument& value(const vl::Ptr<WfExpression>& value);
	};

	class MakeConstructorDeclaration : public vl::glr::ParsingAstBuilder<WfConstructorDeclaration>
	{
	public:
		MakeConstructorDeclaration& arguments(const vl::Ptr<WfFunctionArgument>& value);
		MakeConstructorDeclaration& baseConstructorCalls(const vl::Ptr<WfBaseConstructorCall>& value);
		MakeConstructorDeclaration& constructorType(WfConstructorType value);
		MakeConstructorDeclaration& statement(const vl::Ptr<WfStatement>& value);
		MakeConstructorDeclaration& attributes(const vl::Ptr<WfAttribute>& value);
		MakeConstructorDeclaration& name(const vl::WString& value);
	};

	class MakeConstructorExpression : public vl::glr::ParsingAstBuilder<WfConstructorExpression>
	{
	public:
		MakeConstructorExpression& arguments(const vl::Ptr<WfConstructorArgument>& value);
	};

	class MakeDeclaration : public vl::glr::ParsingAstBuilder<WfDeclaration>
	{
	public:
		MakeDeclaration& attributes(const vl::Ptr<WfAttribute>& value);
		MakeDeclaration& name(const vl::WString& value);
	};

	class MakeDeleteStatement : public vl::glr::ParsingAstBuilder<WfDeleteStatement>
	{
	public:
		MakeDeleteStatement& expression(const vl::Ptr<WfExpression>& value);
	};

	class MakeDestructorDeclaration : public vl::glr::ParsingAstBuilder<WfDestructorDeclaration>
	{
	public:
		MakeDestructorDeclaration& statement(const vl::Ptr<WfStatement>& value);
		MakeDestructorDeclaration& attributes(const vl::Ptr<WfAttribute>& value);
		MakeDestructorDeclaration& name(const vl::WString& value);
	};

	class MakeDetachEventExpression : public vl::glr::ParsingAstBuilder<WfDetachEventExpression>
	{
	public:
		MakeDetachEventExpression& event(const vl::Ptr<WfExpression>& value);
		MakeDetachEventExpression& handler(const vl::Ptr<WfExpression>& value);
	};

	class MakeEnumDeclaration : public vl::glr::ParsingAstBuilder<WfEnumDeclaration>
	{
	public:
		MakeEnumDeclaration& items(const vl::Ptr<WfEnumItem>& value);
		MakeEnumDeclaration& kind(WfEnumKind value);
		MakeEnumDeclaration& attributes(const vl::Ptr<WfAttribute>& value);
		MakeEnumDeclaration& name(const vl::WString& value);
	};

	class MakeEnumItem : public vl::glr::ParsingAstBuilder<WfEnumItem>
	{
	public:
		MakeEnumItem& attributes(const vl::Ptr<WfAttribute>& value);
		MakeEnumItem& intersections(const vl::Ptr<WfEnumItemIntersection>& value);
		MakeEnumItem& kind(WfEnumItemKind value);
		MakeEnumItem& name(const vl::WString& value);
		MakeEnumItem& number(const vl::WString& value);
	};

	class MakeEnumItemIntersection : public vl::glr::ParsingAstBuilder<WfEnumItemIntersection>
	{
	public:
		MakeEnumItemIntersection& name(const vl::WString& value);
	};

	class MakeEnumerableType : public vl::glr::ParsingAstBuilder<WfEnumerableType>
	{
	public:
		MakeEnumerableType& element(const vl::Ptr<WfType>& value);
	};

	class MakeEventDeclaration : public vl::glr::ParsingAstBuilder<WfEventDeclaration>
	{
	public:
		MakeEventDeclaration& arguments(const vl::Ptr<WfType>& value);
		MakeEventDeclaration& attributes(const vl::Ptr<WfAttribute>& value);
		MakeEventDeclaration& name(const vl::WString& value);
	};

	class MakeExpectedTypeCastExpression : public vl::glr::ParsingAstBuilder<WfExpectedTypeCastExpression>
	{
	public:
		MakeExpectedTypeCastExpression& expression(const vl::Ptr<WfExpression>& value);
		MakeExpectedTypeCastExpression& strategy(WfTypeCastingStrategy value);
		MakeExpectedTypeCastExpression& expandedExpression(const vl::Ptr<WfExpression>& value);
	};

	class MakeExpressionStatement : public vl::glr::ParsingAstBuilder<WfExpressionStatement>
	{
	public:
		MakeExpressionStatement& expression(const vl::Ptr<WfExpression>& value);
	};

	class MakeFloatingExpression : public vl::glr::ParsingAstBuilder<WfFloatingExpression>
	{
	public:
		MakeFloatingExpression& value(const vl::WString& value);
	};

	class MakeForEachStatement : public vl::glr::ParsingAstBuilder<WfForEachStatement>
	{
	public:
		MakeForEachStatement& collection(const vl::Ptr<WfExpression>& value);
		MakeForEachStatement& direction(WfForEachDirection value);
		MakeForEachStatement& name(const vl::WString& value);
		MakeForEachStatement& statement(const vl::Ptr<WfStatement>& value);
		MakeForEachStatement& expandedStatement(const vl::Ptr<WfStatement>& value);
	};

	class MakeFormatExpression : public vl::glr::ParsingAstBuilder<WfFormatExpression>
	{
	public:
		MakeFormatExpression& value(const vl::WString& value);
		MakeFormatExpression& expandedExpression(const vl::Ptr<WfExpression>& value);
	};

	class MakeFunctionArgument : public vl::glr::ParsingAstBuilder<WfFunctionArgument>
	{
	public:
		MakeFunctionArgument& attributes(const vl::Ptr<WfAttribute>& value);
		MakeFunctionArgument& name(const vl::WString& value);
		MakeFunctionArgument& type(const vl::Ptr<WfType>& value);
	};

	class MakeFunctionDeclaration : public vl::glr::ParsingAstBuilder<WfFunctionDeclaration>
	{
	public:
		MakeFunctionDeclaration& anonymity(WfFunctionAnonymity value);
		MakeFunctionDeclaration& arguments(const vl::Ptr<WfFunctionArgument>& value);
		MakeFunctionDeclaration& functionKind(WfFunctionKind value);
		MakeFunctionDeclaration& returnType(const vl::Ptr<WfType>& value);
		MakeFunctionDeclaration& statement(const vl::Ptr<WfStatement>& value);
		MakeFunctionDeclaration& attributes(const vl::Ptr<WfAttribute>& value);
		MakeFunctionDeclaration& name(const vl::WString& value);
	};

	class MakeFunctionExpression : public vl::glr::ParsingAstBuilder<WfFunctionExpression>
	{
	public:
		MakeFunctionExpression& function(const vl::Ptr<WfFunctionDeclaration>& value);
	};

	class MakeFunctionType : public vl::glr::ParsingAstBuilder<WfFunctionType>
	{
	public:
		MakeFunctionType& arguments(const vl::Ptr<WfType>& value);
		MakeFunctionType& result(const vl::Ptr<WfType>& value);
	};

	class MakeGotoStatement : public vl::glr::ParsingAstBuilder<WfGotoStatement>
	{
	public:
		MakeGotoStatement& label(const vl::WString& value);
	};

	class MakeIfExpression : public vl::glr::ParsingAstBuilder<WfIfExpression>
	{
	public:
		MakeIfExpression& condition(const vl::Ptr<WfExpression>& value);
		MakeIfExpression& falseBranch(const vl::Ptr<WfExpression>& value);
		MakeIfExpression& trueBranch(const vl::Ptr<WfExpression>& value);
	};

	class MakeIfStatement : public vl::glr::ParsingAstBuilder<WfIfStatement>
	{
	public:
		MakeIfStatement& expression(const vl::Ptr<WfExpression>& value);
		MakeIfStatement& falseBranch(const vl::Ptr<WfStatement>& value);
		MakeIfStatement& name(const vl::WString& value);
		MakeIfStatement& trueBranch(const vl::Ptr<WfStatement>& value);
		MakeIfStatement& type(const vl::Ptr<WfType>& value);
	};

	class MakeInferExpression : public vl::glr::ParsingAstBuilder<WfInferExpression>
	{
	public:
		MakeInferExpression& expression(const vl::Ptr<WfExpression>& value);
		MakeInferExpression& type(const vl::Ptr<WfType>& value);
	};

	class MakeIntegerExpression : public vl::glr::ParsingAstBuilder<WfIntegerExpression>
	{
	public:
		MakeIntegerExpression& value(const vl::WString& value);
	};

	class MakeLetExpression : public vl::glr::ParsingAstBuilder<WfLetExpression>
	{
	public:
		MakeLetExpression& expression(const vl::Ptr<WfExpression>& value);
		MakeLetExpression& variables(const vl::Ptr<WfLetVariable>& value);
	};

	class MakeLetVariable : public vl::glr::ParsingAstBuilder<WfLetVariable>
	{
	public:
		MakeLetVariable& name(const vl::WString& value);
		MakeLetVariable& value(const vl::Ptr<WfExpression>& value);
	};

	class MakeLiteralExpression : public vl::glr::ParsingAstBuilder<WfLiteralExpression>
	{
	public:
		MakeLiteralExpression& value(WfLiteralValue value);
	};

	class MakeMapType : public vl::glr::ParsingAstBuilder<WfMapType>
	{
	public:
		MakeMapType& key(const vl::Ptr<WfType>& value);
		MakeMapType& value(const vl::Ptr<WfType>& value);
		MakeMapType& writability(WfMapWritability value);
	};

	class MakeMemberExpression : public vl::glr::ParsingAstBuilder<WfMemberExpression>
	{
	public:
		MakeMemberExpression& name(const vl::WString& value);
		MakeMemberExpression& parent(const vl::Ptr<WfExpression>& value);
	};

	class MakeMixinCastExpression : public vl::glr::ParsingAstBuilder<WfMixinCastExpression>
	{
	public:
		MakeMixinCastExpression& expression(const vl::Ptr<WfExpression>& value);
		MakeMixinCastExpression& type(const vl::Ptr<WfType>& value);
		MakeMixinCastExpression& expandedExpression(const vl::Ptr<WfExpression>& value);
	};

	class MakeModule : public vl::glr::ParsingAstBuilder<WfModule>
	{
	public:
		MakeModule& declarations(const vl::Ptr<WfDeclaration>& value);
		MakeModule& moduleType(WfModuleType value);
		MakeModule& name(const vl::WString& value);
		MakeModule& paths(const vl::Ptr<WfModuleUsingPath>& value);
	};

	class MakeModuleUsingItem : public vl::glr::ParsingAstBuilder<WfModuleUsingItem>
	{
	public:
		MakeModuleUsingItem& fragments(const vl::Ptr<WfModuleUsingFragment>& value);
	};

	class MakeModuleUsingNameFragment : public vl::glr::ParsingAstBuilder<WfModuleUsingNameFragment>
	{
	public:
		MakeModuleUsingNameFragment& name(const vl::WString& value);
	};

	class MakeModuleUsingPath : public vl::glr::ParsingAstBuilder<WfModuleUsingPath>
	{
	public:
		MakeModuleUsingPath& items(const vl::Ptr<WfModuleUsingItem>& value);
	};

	class MakeNamespaceDeclaration : public vl::glr::ParsingAstBuilder<WfNamespaceDeclaration>
	{
	public:
		MakeNamespaceDeclaration& declarations(const vl::Ptr<WfDeclaration>& value);
		MakeNamespaceDeclaration& attributes(const vl::Ptr<WfAttribute>& value);
		MakeNamespaceDeclaration& name(const vl::WString& value);
	};

	class MakeNewClassExpression : public vl::glr::ParsingAstBuilder<WfNewClassExpression>
	{
	public:
		MakeNewClassExpression& arguments(const vl::Ptr<WfExpression>& value);
		MakeNewClassExpression& type(const vl::Ptr<WfType>& value);
	};

	class MakeNewCoroutineExpression : public vl::glr::ParsingAstBuilder<WfNewCoroutineExpression>
	{
	public:
		MakeNewCoroutineExpression& name(const vl::WString& value);
		MakeNewCoroutineExpression& statement(const vl::Ptr<WfStatement>& value);
		MakeNewCoroutineExpression& expandedExpression(const vl::Ptr<WfExpression>& value);
	};

	class MakeNewInterfaceExpression : public vl::glr::ParsingAstBuilder<WfNewInterfaceExpression>
	{
	public:
		MakeNewInterfaceExpression& declarations(const vl::Ptr<WfDeclaration>& value);
		MakeNewInterfaceExpression& type(const vl::Ptr<WfType>& value);
	};

	class MakeNullableType : public vl::glr::ParsingAstBuilder<WfNullableType>
	{
	public:
		MakeNullableType& element(const vl::Ptr<WfType>& value);
	};

	class MakeObservableListType : public vl::glr::ParsingAstBuilder<WfObservableListType>
	{
	public:
		MakeObservableListType& element(const vl::Ptr<WfType>& value);
	};

	class MakeObserveExpression : public vl::glr::ParsingAstBuilder<WfObserveExpression>
	{
	public:
		MakeObserveExpression& events(const vl::Ptr<WfExpression>& value);
		MakeObserveExpression& expression(const vl::Ptr<WfExpression>& value);
		MakeObserveExpression& name(const vl::WString& value);
		MakeObserveExpression& observeType(WfObserveType value);
		MakeObserveExpression& parent(const vl::Ptr<WfExpression>& value);
	};

	class MakeOrderedLambdaExpression : public vl::glr::ParsingAstBuilder<WfOrderedLambdaExpression>
	{
	public:
		MakeOrderedLambdaExpression& body(const vl::Ptr<WfExpression>& value);
	};

	class MakeOrderedNameExpression : public vl::glr::ParsingAstBuilder<WfOrderedNameExpression>
	{
	public:
		MakeOrderedNameExpression& name(const vl::WString& value);
	};

	class MakePredefinedType : public vl::glr::ParsingAstBuilder<WfPredefinedType>
	{
	public:
		MakePredefinedType& name(WfPredefinedTypeName value);
	};

	class MakePropertyDeclaration : public vl::glr::ParsingAstBuilder<WfPropertyDeclaration>
	{
	public:
		MakePropertyDeclaration& getter(const vl::WString& value);
		MakePropertyDeclaration& setter(const vl::WString& value);
		MakePropertyDeclaration& type(const vl::Ptr<WfType>& value);
		MakePropertyDeclaration& valueChangedEvent(const vl::WString& value);
		MakePropertyDeclaration& attributes(const vl::Ptr<WfAttribute>& value);
		MakePropertyDeclaration& name(const vl::WString& value);
	};

	class MakeRaiseExceptionStatement : public vl::glr::ParsingAstBuilder<WfRaiseExceptionStatement>
	{
	public:
		MakeRaiseExceptionStatement& expression(const vl::Ptr<WfExpression>& value);
	};

	class MakeRangeExpression : public vl::glr::ParsingAstBuilder<WfRangeExpression>
	{
	public:
		MakeRangeExpression& begin(const vl::Ptr<WfExpression>& value);
		MakeRangeExpression& beginBoundary(WfRangeBoundary value);
		MakeRangeExpression& end(const vl::Ptr<WfExpression>& value);
		MakeRangeExpression& endBoundary(WfRangeBoundary value);
	};

	class MakeRawPointerType : public vl::glr::ParsingAstBuilder<WfRawPointerType>
	{
	public:
		MakeRawPointerType& element(const vl::Ptr<WfType>& value);
	};

	class MakeReferenceExpression : public vl::glr::ParsingAstBuilder<WfReferenceExpression>
	{
	public:
		MakeReferenceExpression& name(const vl::WString& value);
	};

	class MakeReferenceType : public vl::glr::ParsingAstBuilder<WfReferenceType>
	{
	public:
		MakeReferenceType& name(const vl::WString& value);
	};

	class MakeReturnStatement : public vl::glr::ParsingAstBuilder<WfReturnStatement>
	{
	public:
		MakeReturnStatement& expression(const vl::Ptr<WfExpression>& value);
	};

	class MakeSetTestingExpression : public vl::glr::ParsingAstBuilder<WfSetTestingExpression>
	{
	public:
		MakeSetTestingExpression& collection(const vl::Ptr<WfExpression>& value);
		MakeSetTestingExpression& element(const vl::Ptr<WfExpression>& value);
		MakeSetTestingExpression& test(WfSetTesting value);
	};

	class MakeSharedPointerType : public vl::glr::ParsingAstBuilder<WfSharedPointerType>
	{
	public:
		MakeSharedPointerType& element(const vl::Ptr<WfType>& value);
	};

	class MakeStateDeclaration : public vl::glr::ParsingAstBuilder<WfStateDeclaration>
	{
	public:
		MakeStateDeclaration& arguments(const vl::Ptr<WfFunctionArgument>& value);
		MakeStateDeclaration& name(const vl::WString& value);
		MakeStateDeclaration& statement(const vl::Ptr<WfStatement>& value);
	};

	class MakeStateInput : public vl::glr::ParsingAstBuilder<WfStateInput>
	{
	public:
		MakeStateInput& arguments(const vl::Ptr<WfFunctionArgument>& value);
		MakeStateInput& name(const vl::WString& value);
	};

	class MakeStateInvokeStatement : public vl::glr::ParsingAstBuilder<WfStateInvokeStatement>
	{
	public:
		MakeStateInvokeStatement& arguments(const vl::Ptr<WfExpression>& value);
		MakeStateInvokeStatement& name(const vl::WString& value);
		MakeStateInvokeStatement& type(WfStateInvokeType value);
	};

	class MakeStateMachineDeclaration : public vl::glr::ParsingAstBuilder<WfStateMachineDeclaration>
	{
	public:
		MakeStateMachineDeclaration& inputs(const vl::Ptr<WfStateInput>& value);
		MakeStateMachineDeclaration& states(const vl::Ptr<WfStateDeclaration>& value);
		MakeStateMachineDeclaration& expandedDeclarations(const vl::Ptr<WfDeclaration>& value);
		MakeStateMachineDeclaration& attributes(const vl::Ptr<WfAttribute>& value);
		MakeStateMachineDeclaration& name(const vl::WString& value);
	};

	class MakeStateSwitchArgument : public vl::glr::ParsingAstBuilder<WfStateSwitchArgument>
	{
	public:
		MakeStateSwitchArgument& name(const vl::WString& value);
	};

	class MakeStateSwitchCase : public vl::glr::ParsingAstBuilder<WfStateSwitchCase>
	{
	public:
		MakeStateSwitchCase& arguments(const vl::Ptr<WfStateSwitchArgument>& value);
		MakeStateSwitchCase& name(const vl::WString& value);
		MakeStateSwitchCase& statement(const vl::Ptr<WfStatement>& value);
	};

	class MakeStateSwitchStatement : public vl::glr::ParsingAstBuilder<WfStateSwitchStatement>
	{
	public:
		MakeStateSwitchStatement& caseBranches(const vl::Ptr<WfStateSwitchCase>& value);
		MakeStateSwitchStatement& type(WfStateSwitchType value);
	};

	class MakeStaticInitDeclaration : public vl::glr::ParsingAstBuilder<WfStaticInitDeclaration>
	{
	public:
		MakeStaticInitDeclaration& statement(const vl::Ptr<WfStatement>& value);
		MakeStaticInitDeclaration& attributes(const vl::Ptr<WfAttribute>& value);
		MakeStaticInitDeclaration& name(const vl::WString& value);
	};

	class MakeStringExpression : public vl::glr::ParsingAstBuilder<WfStringExpression>
	{
	public:
		MakeStringExpression& value(const vl::WString& value);
	};

	class MakeStructDeclaration : public vl::glr::ParsingAstBuilder<WfStructDeclaration>
	{
	public:
		MakeStructDeclaration& members(const vl::Ptr<WfStructMember>& value);
		MakeStructDeclaration& attributes(const vl::Ptr<WfAttribute>& value);
		MakeStructDeclaration& name(const vl::WString& value);
	};

	class MakeStructMember : public vl::glr::ParsingAstBuilder<WfStructMember>
	{
	public:
		MakeStructMember& attributes(const vl::Ptr<WfAttribute>& value);
		MakeStructMember& name(const vl::WString& value);
		MakeStructMember& type(const vl::Ptr<WfType>& value);
	};

	class MakeSwitchCase : public vl::glr::ParsingAstBuilder<WfSwitchCase>
	{
	public:
		MakeSwitchCase& expression(const vl::Ptr<WfExpression>& value);
		MakeSwitchCase& statement(const vl::Ptr<WfStatement>& value);
	};

	class MakeSwitchStatement : public vl::glr::ParsingAstBuilder<WfSwitchStatement>
	{
	public:
		MakeSwitchStatement& caseBranches(const vl::Ptr<WfSwitchCase>& value);
		MakeSwitchStatement& defaultBranch(const vl::Ptr<WfStatement>& value);
		MakeSwitchStatement& expression(const vl::Ptr<WfExpression>& value);
		MakeSwitchStatement& expandedStatement(const vl::Ptr<WfStatement>& value);
	};

	class MakeTopQualifiedExpression : public vl::glr::ParsingAstBuilder<WfTopQualifiedExpression>
	{
	public:
		MakeTopQualifiedExpression& name(const vl::WString& value);
	};

	class MakeTopQualifiedType : public vl::glr::ParsingAstBuilder<WfTopQualifiedType>
	{
	public:
		MakeTopQualifiedType& name(const vl::WString& value);
	};

	class MakeTryStatement : public vl::glr::ParsingAstBuilder<WfTryStatement>
	{
	public:
		MakeTryStatement& catchStatement(const vl::Ptr<WfStatement>& value);
		MakeTryStatement& finallyStatement(const vl::Ptr<WfStatement>& value);
		MakeTryStatement& name(const vl::WString& value);
		MakeTryStatement& protectedStatement(const vl::Ptr<WfStatement>& value);
	};

	class MakeTypeCastingExpression : public vl::glr::ParsingAstBuilder<WfTypeCastingExpression>
	{
	public:
		MakeTypeCastingExpression& expression(const vl::Ptr<WfExpression>& value);
		MakeTypeCastingExpression& strategy(WfTypeCastingStrategy value);
		MakeTypeCastingExpression& type(const vl::Ptr<WfType>& value);
	};

	class MakeTypeOfExpressionExpression : public vl::glr::ParsingAstBuilder<WfTypeOfExpressionExpression>
	{
	public:
		MakeTypeOfExpressionExpression& expression(const vl::Ptr<WfExpression>& value);
	};

	class MakeTypeOfTypeExpression : public vl::glr::ParsingAstBuilder<WfTypeOfTypeExpression>
	{
	public:
		MakeTypeOfTypeExpression& type(const vl::Ptr<WfType>& value);
	};

	class MakeTypeTestingExpression : public vl::glr::ParsingAstBuilder<WfTypeTestingExpression>
	{
	public:
		MakeTypeTestingExpression& expression(const vl::Ptr<WfExpression>& value);
		MakeTypeTestingExpression& test(WfTypeTesting value);
		MakeTypeTestingExpression& type(const vl::Ptr<WfType>& value);
	};

	class MakeUnaryExpression : public vl::glr::ParsingAstBuilder<WfUnaryExpression>
	{
	public:
		MakeUnaryExpression& op(WfUnaryOperator value);
		MakeUnaryExpression& operand(const vl::Ptr<WfExpression>& value);
	};

	class MakeVariableDeclaration : public vl::glr::ParsingAstBuilder<WfVariableDeclaration>
	{
	public:
		MakeVariableDeclaration& expression(const vl::Ptr<WfExpression>& value);
		MakeVariableDeclaration& type(const vl::Ptr<WfType>& value);
		MakeVariableDeclaration& attributes(const vl::Ptr<WfAttribute>& value);
		MakeVariableDeclaration& name(const vl::WString& value);
	};

	class MakeVariableStatement : public vl::glr::ParsingAstBuilder<WfVariableStatement>
	{
	public:
		MakeVariableStatement& variable(const vl::Ptr<WfVariableDeclaration>& value);
	};

	class MakeVirtualCfeDeclaration : public vl::glr::ParsingAstBuilder<WfVirtualCfeDeclaration>
	{
	public:
		MakeVirtualCfeDeclaration& expandedDeclarations(const vl::Ptr<WfDeclaration>& value);
		MakeVirtualCfeDeclaration& attributes(const vl::Ptr<WfAttribute>& value);
		MakeVirtualCfeDeclaration& name(const vl::WString& value);
	};

	class MakeVirtualCfeExpression : public vl::glr::ParsingAstBuilder<WfVirtualCfeExpression>
	{
	public:
		MakeVirtualCfeExpression& expandedExpression(const vl::Ptr<WfExpression>& value);
	};

	class MakeVirtualCseDeclaration : public vl::glr::ParsingAstBuilder<WfVirtualCseDeclaration>
	{
	public:
		MakeVirtualCseDeclaration& expandedDeclarations(const vl::Ptr<WfDeclaration>& value);
		MakeVirtualCseDeclaration& attributes(const vl::Ptr<WfAttribute>& value);
		MakeVirtualCseDeclaration& name(const vl::WString& value);
	};

	class MakeVirtualCseExpression : public vl::glr::ParsingAstBuilder<WfVirtualCseExpression>
	{
	public:
		MakeVirtualCseExpression& expandedExpression(const vl::Ptr<WfExpression>& value);
	};

	class MakeVirtualCseStatement : public vl::glr::ParsingAstBuilder<WfVirtualCseStatement>
	{
	public:
		MakeVirtualCseStatement& expandedStatement(const vl::Ptr<WfStatement>& value);
	};

	class MakeWhileStatement : public vl::glr::ParsingAstBuilder<WfWhileStatement>
	{
	public:
		MakeWhileStatement& condition(const vl::Ptr<WfExpression>& value);
		MakeWhileStatement& statement(const vl::Ptr<WfStatement>& value);
	};

}
#endif