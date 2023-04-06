/***********************************************************************
This file is generated by: Vczh Parser Generator
From parser definition:Workflow
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_WORKFLOW_PARSER_AST_ASSEMBLER
#define VCZH_WORKFLOW_PARSER_AST_ASSEMBLER

#include "WorkflowAst.h"

namespace vl::workflow
{
	enum class WorkflowClasses : vl::vint32_t
	{
		AttachEventExpression = 0,
		Attribute = 1,
		AutoPropertyDeclaration = 2,
		BaseConstructorCall = 3,
		BinaryExpression = 4,
		BindExpression = 5,
		BlockStatement = 6,
		BreakStatement = 7,
		CallExpression = 8,
		CastResultInterfaceDeclaration = 9,
		ChildExpression = 10,
		ChildType = 11,
		ClassDeclaration = 12,
		CoOperatorExpression = 13,
		CoOperatorStatement = 14,
		CoPauseStatement = 15,
		CoProviderStatement = 16,
		ConstructorArgument = 17,
		ConstructorDeclaration = 18,
		ConstructorExpression = 19,
		ContinueStatement = 20,
		CoroutineStatement = 21,
		Declaration = 22,
		DeleteStatement = 23,
		DestructorDeclaration = 24,
		DetachEventExpression = 25,
		EnumDeclaration = 26,
		EnumItem = 27,
		EnumItemIntersection = 28,
		EnumerableType = 29,
		EventDeclaration = 30,
		ExpectedTypeCastExpression = 31,
		Expression = 32,
		ExpressionStatement = 33,
		FloatingExpression = 34,
		ForEachStatement = 35,
		FormatExpression = 36,
		FunctionArgument = 37,
		FunctionDeclaration = 38,
		FunctionExpression = 39,
		FunctionType = 40,
		GotoStatement = 41,
		IfExpression = 42,
		IfStatement = 43,
		InferExpression = 44,
		IntegerExpression = 45,
		LetExpression = 46,
		LetVariable = 47,
		LiteralExpression = 48,
		MapType = 49,
		MemberExpression = 50,
		MixinCastExpression = 51,
		Module = 52,
		ModuleUsingFragment = 53,
		ModuleUsingItem = 54,
		ModuleUsingNameFragment = 55,
		ModuleUsingPath = 56,
		ModuleUsingWildCardFragment = 57,
		NamespaceDeclaration = 58,
		NewClassExpression = 59,
		NewCoroutineExpression = 60,
		NewInterfaceExpression = 61,
		NullableType = 62,
		ObservableListType = 63,
		ObserveExpression = 64,
		OrderedLambdaExpression = 65,
		OrderedNameExpression = 66,
		PredefinedType = 67,
		PropertyDeclaration = 68,
		RaiseExceptionStatement = 69,
		RangeExpression = 70,
		RawPointerType = 71,
		ReferenceExpression = 72,
		ReferenceType = 73,
		ReturnStatement = 74,
		SetTestingExpression = 75,
		SharedPointerType = 76,
		StateDeclaration = 77,
		StateInput = 78,
		StateInvokeStatement = 79,
		StateMachineDeclaration = 80,
		StateMachineStatement = 81,
		StateSwitchArgument = 82,
		StateSwitchCase = 83,
		StateSwitchStatement = 84,
		Statement = 85,
		StringExpression = 86,
		StructDeclaration = 87,
		StructMember = 88,
		SwitchCase = 89,
		SwitchStatement = 90,
		ThisExpression = 91,
		TopQualifiedExpression = 92,
		TopQualifiedType = 93,
		TryStatement = 94,
		Type = 95,
		TypeCastingExpression = 96,
		TypeOfExpressionExpression = 97,
		TypeOfTypeExpression = 98,
		TypeTestingExpression = 99,
		UnaryExpression = 100,
		VariableDeclaration = 101,
		VariableStatement = 102,
		VirtualCfeDeclaration = 103,
		VirtualCfeExpression = 104,
		VirtualCseDeclaration = 105,
		VirtualCseExpression = 106,
		VirtualCseStatement = 107,
		WhileStatement = 108,
	};

	enum class WorkflowFields : vl::vint32_t
	{
		AttachEventExpression_event = 0,
		AttachEventExpression_function = 1,
		Attribute_category = 2,
		Attribute_name = 3,
		Attribute_value = 4,
		AutoPropertyDeclaration_configConst = 5,
		AutoPropertyDeclaration_configObserve = 6,
		AutoPropertyDeclaration_expression = 7,
		AutoPropertyDeclaration_functionKind = 8,
		AutoPropertyDeclaration_type = 9,
		BaseConstructorCall_arguments = 10,
		BaseConstructorCall_type = 11,
		BinaryExpression_first = 12,
		BinaryExpression_op = 13,
		BinaryExpression_second = 14,
		BindExpression_expression = 15,
		BlockStatement_endLabel = 16,
		BlockStatement_statements = 17,
		CallExpression_arguments = 18,
		CallExpression_function = 19,
		CastResultInterfaceDeclaration_baseType = 20,
		CastResultInterfaceDeclaration_elementType = 21,
		ChildExpression_name = 22,
		ChildExpression_parent = 23,
		ChildType_name = 24,
		ChildType_parent = 25,
		ClassDeclaration_baseTypes = 26,
		ClassDeclaration_constructorType = 27,
		ClassDeclaration_declarations = 28,
		ClassDeclaration_kind = 29,
		CoOperatorExpression_name = 30,
		CoOperatorStatement_arguments = 31,
		CoOperatorStatement_opName = 32,
		CoOperatorStatement_varName = 33,
		CoPauseStatement_statement = 34,
		CoProviderStatement_name = 35,
		CoProviderStatement_statement = 36,
		ConstructorArgument_key = 37,
		ConstructorArgument_value = 38,
		ConstructorDeclaration_arguments = 39,
		ConstructorDeclaration_baseConstructorCalls = 40,
		ConstructorDeclaration_constructorType = 41,
		ConstructorDeclaration_statement = 42,
		ConstructorExpression_arguments = 43,
		Declaration_attributes = 44,
		Declaration_name = 45,
		DeleteStatement_expression = 46,
		DestructorDeclaration_statement = 47,
		DetachEventExpression_event = 48,
		DetachEventExpression_handler = 49,
		EnumDeclaration_items = 50,
		EnumDeclaration_kind = 51,
		EnumItem_attributes = 52,
		EnumItem_intersections = 53,
		EnumItem_kind = 54,
		EnumItem_name = 55,
		EnumItem_number = 56,
		EnumItemIntersection_name = 57,
		EnumerableType_element = 58,
		EventDeclaration_arguments = 59,
		ExpectedTypeCastExpression_expression = 60,
		ExpectedTypeCastExpression_strategy = 61,
		ExpressionStatement_expression = 62,
		FloatingExpression_value = 63,
		ForEachStatement_collection = 64,
		ForEachStatement_direction = 65,
		ForEachStatement_name = 66,
		ForEachStatement_statement = 67,
		FormatExpression_value = 68,
		FunctionArgument_attributes = 69,
		FunctionArgument_name = 70,
		FunctionArgument_type = 71,
		FunctionDeclaration_anonymity = 72,
		FunctionDeclaration_arguments = 73,
		FunctionDeclaration_functionKind = 74,
		FunctionDeclaration_returnType = 75,
		FunctionDeclaration_statement = 76,
		FunctionExpression_function = 77,
		FunctionType_arguments = 78,
		FunctionType_result = 79,
		GotoStatement_label = 80,
		IfExpression_condition = 81,
		IfExpression_falseBranch = 82,
		IfExpression_trueBranch = 83,
		IfStatement_expression = 84,
		IfStatement_falseBranch = 85,
		IfStatement_name = 86,
		IfStatement_trueBranch = 87,
		IfStatement_type = 88,
		InferExpression_expression = 89,
		InferExpression_type = 90,
		IntegerExpression_value = 91,
		LetExpression_expression = 92,
		LetExpression_variables = 93,
		LetVariable_name = 94,
		LetVariable_value = 95,
		LiteralExpression_value = 96,
		MapType_key = 97,
		MapType_value = 98,
		MapType_writability = 99,
		MemberExpression_name = 100,
		MemberExpression_parent = 101,
		MixinCastExpression_expression = 102,
		MixinCastExpression_type = 103,
		Module_declarations = 104,
		Module_moduleType = 105,
		Module_name = 106,
		Module_paths = 107,
		ModuleUsingItem_fragments = 108,
		ModuleUsingNameFragment_name = 109,
		ModuleUsingPath_items = 110,
		NamespaceDeclaration_declarations = 111,
		NewClassExpression_arguments = 112,
		NewClassExpression_type = 113,
		NewCoroutineExpression_name = 114,
		NewCoroutineExpression_statement = 115,
		NewInterfaceExpression_declarations = 116,
		NewInterfaceExpression_type = 117,
		NullableType_element = 118,
		ObservableListType_element = 119,
		ObserveExpression_events = 120,
		ObserveExpression_expression = 121,
		ObserveExpression_name = 122,
		ObserveExpression_observeType = 123,
		ObserveExpression_parent = 124,
		OrderedLambdaExpression_body = 125,
		OrderedNameExpression_name = 126,
		PredefinedType_name = 127,
		PropertyDeclaration_getter = 128,
		PropertyDeclaration_setter = 129,
		PropertyDeclaration_type = 130,
		PropertyDeclaration_valueChangedEvent = 131,
		RaiseExceptionStatement_expression = 132,
		RangeExpression_begin = 133,
		RangeExpression_beginBoundary = 134,
		RangeExpression_end = 135,
		RangeExpression_endBoundary = 136,
		RawPointerType_element = 137,
		ReferenceExpression_name = 138,
		ReferenceType_name = 139,
		ReturnStatement_expression = 140,
		SetTestingExpression_collection = 141,
		SetTestingExpression_element = 142,
		SetTestingExpression_test = 143,
		SharedPointerType_element = 144,
		StateDeclaration_arguments = 145,
		StateDeclaration_name = 146,
		StateDeclaration_statement = 147,
		StateInput_arguments = 148,
		StateInput_name = 149,
		StateInvokeStatement_arguments = 150,
		StateInvokeStatement_name = 151,
		StateInvokeStatement_type = 152,
		StateMachineDeclaration_inputs = 153,
		StateMachineDeclaration_states = 154,
		StateSwitchArgument_name = 155,
		StateSwitchCase_arguments = 156,
		StateSwitchCase_name = 157,
		StateSwitchCase_statement = 158,
		StateSwitchStatement_caseBranches = 159,
		StateSwitchStatement_type = 160,
		StringExpression_value = 161,
		StructDeclaration_members = 162,
		StructMember_attributes = 163,
		StructMember_name = 164,
		StructMember_type = 165,
		SwitchCase_expression = 166,
		SwitchCase_statement = 167,
		SwitchStatement_caseBranches = 168,
		SwitchStatement_defaultBranch = 169,
		SwitchStatement_expression = 170,
		TopQualifiedExpression_name = 171,
		TopQualifiedType_name = 172,
		TryStatement_catchStatement = 173,
		TryStatement_finallyStatement = 174,
		TryStatement_name = 175,
		TryStatement_protectedStatement = 176,
		TypeCastingExpression_expression = 177,
		TypeCastingExpression_strategy = 178,
		TypeCastingExpression_type = 179,
		TypeOfExpressionExpression_expression = 180,
		TypeOfTypeExpression_type = 181,
		TypeTestingExpression_expression = 182,
		TypeTestingExpression_test = 183,
		TypeTestingExpression_type = 184,
		UnaryExpression_op = 185,
		UnaryExpression_operand = 186,
		VariableDeclaration_expression = 187,
		VariableDeclaration_type = 188,
		VariableStatement_variable = 189,
		VirtualCfeDeclaration_expandedDeclarations = 190,
		VirtualCfeExpression_expandedExpression = 191,
		VirtualCseDeclaration_expandedDeclarations = 192,
		VirtualCseExpression_expandedExpression = 193,
		VirtualCseStatement_expandedStatement = 194,
		WhileStatement_condition = 195,
		WhileStatement_statement = 196,
	};

	extern const wchar_t* WorkflowTypeName(WorkflowClasses type);
	extern const wchar_t* WorkflowCppTypeName(WorkflowClasses type);
	extern const wchar_t* WorkflowFieldName(WorkflowFields field);
	extern const wchar_t* WorkflowCppFieldName(WorkflowFields field);

	class WorkflowAstInsReceiver : public vl::glr::AstInsReceiverBase
	{
	protected:
		vl::Ptr<vl::glr::ParsingAstBase> CreateAstNode(vl::vint32_t type) override;
		void SetField(vl::glr::ParsingAstBase* object, vl::vint32_t field, vl::Ptr<vl::glr::ParsingAstBase> value) override;
		void SetField(vl::glr::ParsingAstBase* object, vl::vint32_t field, const vl::regex::RegexToken& token, vl::vint32_t tokenIndex) override;
		void SetField(vl::glr::ParsingAstBase* object, vl::vint32_t field, vl::vint32_t enumItem, bool weakAssignment) override;
		vl::Ptr<vl::glr::ParsingAstBase> ResolveAmbiguity(vl::vint32_t type, vl::collections::Array<vl::Ptr<vl::glr::ParsingAstBase>>& candidates) override;
	};
}
#endif