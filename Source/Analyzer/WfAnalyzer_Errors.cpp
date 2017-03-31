#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace parsing;
			using namespace reflection;
			using namespace reflection::description;

/***********************************************************************
WfErrors
***********************************************************************/

			Ptr<parsing::ParsingError> WfErrors::WrongFormatStringSyntax(WfExpression* node)
			{
				return new ParsingError(node, L"A0: Wrong format string syntax.");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongSimpleObserveExpression(WfExpression* node)
			{
				return new ParsingError(node, L"A1: Simple observe expression should observe a property under the observed object.");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongSimpleObserveEvent(WfExpression* node)
			{
				return new ParsingError(node, L"A2: Simple observe expression should react to an event under the observed object.");
			}

			Ptr<parsing::ParsingError> WfErrors::EmptyObserveEvent(WfExpression* node)
			{
				return new ParsingError(node, L"A2: Observe expression should react to at least one event.");
			}

			Ptr<parsing::ParsingError> WfErrors::ObserveNotInBind(WfExpression* node)
			{
				return new ParsingError(node, L"A3: Observe expression should appear in a bind expression.");
			}

			Ptr<parsing::ParsingError> WfErrors::ObserveInObserveEvent(WfExpression* node)
			{
				return new ParsingError(node, L"A4: Observe expression should not appear in the event expression in another observe expression.");
			}

			Ptr<parsing::ParsingError> WfErrors::BindInBind(WfExpression* node)
			{
				return new ParsingError(node, L"A4: Bind expression should not appear in another bind expression.");
			}

			Ptr<parsing::ParsingError> WfErrors::AttachInBind(WfExpression* node)
			{
				return new ParsingError(node, L"A4: Attach expression should not appear in another bind expression.");
			}

			Ptr<parsing::ParsingError> WfErrors::DetachInBind(WfExpression* node)
			{
				return new ParsingError(node, L"A4: Detach expression should not appear in another bind expression.");
			}

			Ptr<parsing::ParsingError> WfErrors::ConstructorMixMapAndList(WfExpression* node)
			{
				return new ParsingError(node, L"A5: Key-value pairs are not allowed in list constructor expression.");
			}

			Ptr<parsing::ParsingError> WfErrors::ConstructorMixStructAndList(WfExpression* node)
			{
				return new ParsingError(node, L"A5: Field-value pairs are expected in struct constructor expression.");
			}

			Ptr<parsing::ParsingError> WfErrors::DuplicatedConstructorField(WfReferenceExpression* node)
			{
				return new ParsingError(node, L"A5: The same field cannot be assigned more than once in struct constructor expression.");
			}

			Ptr<parsing::ParsingError> WfErrors::ConstructorMixClassAndInterface(WfNewClassExpression* node)
			{
				return new ParsingError(node, L"A6: Arguments are not allowed in new interface expression.");
			}

			Ptr<parsing::ParsingError> WfErrors::ConstructorMixClassAndInterface(WfNewInterfaceExpression* node)
			{
				return new ParsingError(node, L"A6: Members are not allowed in new class expression.");
			}

			Ptr<parsing::ParsingError> WfErrors::ScopeNameIsNotExpression(WfExpression* node, Ptr<WfLexicalScopeName> scopeName)
			{
				return new ParsingError(node, L"A7: Symbol \"" + scopeName->GetFriendlyName() + L"\" cannot be used as an expression.");
			}

			Ptr<parsing::ParsingError> WfErrors::EventIsNotExpression(WfExpression* node, reflection::description::IEventInfo* eventInfo)
			{
				return new ParsingError(node, L"A7: Event \"" + eventInfo->GetName() + L"\" of type \"" + eventInfo->GetOwnerTypeDescriptor()->GetTypeName() + L"\" cannot be used as an expression.");
			}

			Ptr<parsing::ParsingError> WfErrors::ExpressionIsNotScopeName(WfExpression* node)
			{
				return new ParsingError(node, L"A8: Expression does not reference to a declaration.");
			}

			Ptr<parsing::ParsingError> WfErrors::ExpressionIsNotEvent(WfExpression* node)
			{
				return new ParsingError(node, L"A8: Expression does not reference to an event.");
			}

			Ptr<parsing::ParsingError> WfErrors::ExpressionCannotResolveType(WfExpression* node, Ptr<WfLexicalSymbol> symbol)
			{
				return new ParsingError(node, L"A9: Expression referencing to symbol \"" + symbol->name + L"\" failed to resolve its type.");
			}

			Ptr<parsing::ParsingError> WfErrors::NullCannotResolveType(WfExpression* node)
			{
				return new ParsingError(node, L"A9: Expression \"null\" failed to resolve its type.");
			}

			Ptr<parsing::ParsingError> WfErrors::ConstructorCannotResolveType(WfExpression* node)
			{
				return new ParsingError(node, L"A9: Expression \"{}\" failed to resolve its type.");
			}

			Ptr<parsing::ParsingError> WfErrors::OrderedLambdaCannotResolveType(WfExpression* node)
			{
				return new ParsingError(node, L"A9: Ordered lambda expression failed to resolve its type.");
			}

			Ptr<parsing::ParsingError> WfErrors::NullCannotImplicitlyConvertToType(WfExpression* node, reflection::description::ITypeInfo* toType)
			{
				return new ParsingError(node, L"A10: Expression \"null\" cannot implicitly convert to \"" + toType->GetTypeFriendlyName() + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::ConstructorCannotImplicitlyConvertToType(WfExpression* node, reflection::description::ITypeInfo* toType)
			{
				return new ParsingError(node, L"A10: Expression \"{}\" cannot implicitly convert to \"" + toType->GetTypeFriendlyName() + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::OrderedLambdaCannotImplicitlyConvertToType(WfExpression* node, reflection::description::ITypeInfo* toType)
			{
				return new ParsingError(node, L"A10: Ordered lambda expression cannot implicitly convert to \"" + toType->GetTypeFriendlyName() + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::ExpressionCannotImplicitlyConvertToType(WfExpression* node, reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType)
			{
				return new ParsingError(node, L"A10: Expression of type \"" + fromType->GetTypeFriendlyName() + L"\" cannot implicitly convert to \"" + toType->GetTypeFriendlyName() + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::ExpressionCannotExplicitlyConvertToType(WfExpression* node, reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType)
			{
				return new ParsingError(node, L"A11: Expression of type \"" + fromType->GetTypeFriendlyName() + L"\" cannot explicitly convert to \"" + toType->GetTypeFriendlyName() + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::CannotWeakCastToType(WfExpression* node, reflection::description::ITypeInfo* toType)
			{
				return new ParsingError(node, L"A12: Expression cannot weakly cast to \"" + toType->GetTypeFriendlyName() + L"\" because it cannot be \"null\".");
			}

			Ptr<parsing::ParsingError> WfErrors::IntegerLiteralOutOfRange(WfIntegerExpression* node)
			{
				return new ParsingError(node, L"A13: Integer literal \"" + node->value.value + L"\" out of range.");
			}

			Ptr<parsing::ParsingError> WfErrors::CannotMergeTwoType(WfExpression* node, reflection::description::ITypeInfo* firstType, reflection::description::ITypeInfo* secondType)
			{
				return new ParsingError(node, L"A14: Failed to merge type \"" + firstType->GetTypeFriendlyName() + L"\" with type \"" + secondType->GetTypeFriendlyName() + L"\" together to calculate the result type.");
			}

			Ptr<parsing::ParsingError> WfErrors::RangeShouldBeInteger(WfExpression* node, reflection::description::ITypeInfo* type)
			{
				return new ParsingError(node, L"A15: Elements in a range expression should be integer, not \"" + type->GetTypeFriendlyName() + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::UnaryOperatorOnWrongType(WfUnaryExpression* node, reflection::description::ITypeInfo* type)
			{
				return new ParsingError(node, L"A16: Unary operator cannot apply on expression of type \"" + type->GetTypeFriendlyName() + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::BinaryOperatorOnWrongType(WfBinaryExpression* node, reflection::description::ITypeInfo* type)
			{
				return new ParsingError(node, L"A16: Binary operator cannot apply on expression of type \"" + type->GetTypeFriendlyName() + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::IndexOperatorOnWrongType(WfBinaryExpression* node, reflection::description::ITypeInfo* containerType)
			{
				return new ParsingError(node, L"A17: Container of type \"" + containerType->GetTypeFriendlyName() + L"\" cannot be accessed using index.");
			}

			Ptr<parsing::ParsingError> WfErrors::ExpressionIsNotCollection(WfExpression* node, reflection::description::ITypeInfo* type)
			{
				return new ParsingError(node, L"A18: Expression of type \"" + type->GetTypeFriendlyName() + L"\" is not an enumerable type.");
			}

			Ptr<parsing::ParsingError> WfErrors::ExpressionIsNotFunction(WfExpression* node, reflection::description::ITypeInfo* type)
			{
				return new ParsingError(node, L"A19: Expression of type \"" + type->GetTypeFriendlyName() + L"\" is not an invokable function type.");
			}

			Ptr<parsing::ParsingError> WfErrors::FunctionArgumentCountMismatched(parsing::ParsingTreeCustomBase* node, vint expectedCount, const ResolveExpressionResult& function)
			{
				return new ParsingError(node, L"A20: Function " + function.GetFriendlyName() + L"\" is not allowed to call with " + itow(expectedCount) + L" arguments.");
			}

			Ptr<parsing::ParsingError> WfErrors::FunctionArgumentTypeMismatched(parsing::ParsingTreeCustomBase* node, const ResolveExpressionResult& function, vint index, reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType)
			{
				return new ParsingError(node, L"A21: The " + itow(index) + L"-th argument of function " + function.GetFriendlyName() + L" cannot implicitly convert from \"" + fromType->GetTypeFriendlyName() + L"\" to \"" + toType->GetTypeFriendlyName() + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::CannotPickOverloadedFunctions(parsing::ParsingTreeCustomBase* node, collections::List<ResolveExpressionResult>& results)
			{
				WString description;
				FOREACH_INDEXER(ResolveExpressionResult, result, index, results)
				{
					description += L"\r\n\t";
					description += result.GetFriendlyName();
				}
				return new ParsingError(node, L"A22: Cannot decide which function to call in multiple targets: " + description + L".");
			}

			Ptr<parsing::ParsingError> WfErrors::ClassContainsNoConstructor(WfExpression* node, reflection::description::ITypeInfo* type)
			{
				return new ParsingError(node, L"A23: Class \"" + type->GetTypeFriendlyName() + L"\" does not contain any constructor.");
			}

			Ptr<parsing::ParsingError> WfErrors::InterfaceContainsNoConstructor(WfExpression* node, reflection::description::ITypeInfo* type)
			{
				Ptr<ITypeInfo> proxy = TypeInfoRetriver<Ptr<IValueInterfaceProxy>>::CreateTypeInfo();
				return new ParsingError(node, L"A23: Interface \"" + type->GetTypeFriendlyName() + L"\" does not contain any constructor receiving an \"" + proxy->GetTypeFriendlyName() + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::ConstructorReturnTypeMismatched(WfExpression* node, const ResolveExpressionResult& function, reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType)
			{
				return new ParsingError(node, L"A24: The return type of " + function.GetFriendlyName() + L" cannot implicitly convert from \"" + fromType->GetTypeFriendlyName() + L"\" to \"" + toType->GetTypeFriendlyName() + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::ExpressionIsNotLeftValue(WfExpression* node, const ResolveExpressionResult& result)
			{
				return new ParsingError(node, L"A25: " + result.GetFriendlyName(true) + L" is not assignable.");
			}

			Ptr<parsing::ParsingError> WfErrors::CannotCallMemberOutsideOfClass(WfExpression* node, const ResolveExpressionResult& result)
			{
				return new ParsingError(node, L"A26: " + result.GetFriendlyName(true) + L" cannot be called as a static member.");
			}

			Ptr<parsing::ParsingError> WfErrors::CannotCallMemberInStaticFunction(WfExpression* node, const ResolveExpressionResult& result)
			{
				return new ParsingError(node, L"A27: " + result.GetFriendlyName(true) + L" cannot be called in static functions of its class or interface.");
			}

			Ptr<parsing::ParsingError> WfErrors::FieldCannotInitializeUsingEachOther(WfExpression* node, const ResolveExpressionResult& result)
			{
				return new ParsingError(node, L"A28: " + result.GetFriendlyName(true) + L" cannot be used to initialize other fields in the same type or new interface expression.");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongThisExpression(WfExpression* node)
			{
				return new ParsingError(node, L"A29: The \"this\" expression can only be used in class methods or functions in new interface expressions");
			}

			Ptr<parsing::ParsingError> WfErrors::IncorrectTypeForUnion(WfExpression* node, reflection::description::ITypeInfo* type)
			{
				return new ParsingError(node, L"A30: Expression of type \"" + type->GetTypeFriendlyName() + L"\" cannot be used in union (&) expression because it is not string or flag enum.");
			}

			Ptr<parsing::ParsingError> WfErrors::IncorrectTypeForIntersect(WfExpression* node, reflection::description::ITypeInfo* type)
			{
				return new ParsingError(node, L"A30: Expression of type \"" + type->GetTypeFriendlyName() + L"\" cannot be used in intersect (|) expression because it is not flag enum.");
			}

			Ptr<parsing::ParsingError> WfErrors::ExpressionIsNotConstant(WfExpression* node)
			{
				return new ParsingError(node, L"A31: Expression is not constant. A constant expression consists of:\r\n"
					L"\tconstant primitive values\r\n"
					L"\tenum items\r\n"
					L"\tconstant unary (+, -, not) expressions\r\n"
					L"\tconstant binary (|) expressions\r\n"
					L"\tconstant range expressions\r\n"
					L"\tconstant constructor expressions (to create values of structs, lists or dictionarys)\r\n"
					L"\ttype() or typeof() expressions.");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongVoidType(WfType* node)
			{
				return new ParsingError(node, L"B0: Void is not a type for a value.");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongInterfaceType(WfType* node)
			{
				return new ParsingError(node, L"B1: Interface is not a type for a value.");
			}

			Ptr<parsing::ParsingError> WfErrors::RawPointerToNonReferenceType(WfType* node, reflection::description::ITypeInfo* typeInfo)
			{
				return new ParsingError(node, L"B2: A raw pointer" + (typeInfo ? L" \"" + typeInfo->GetTypeFriendlyName() + L"\"" : L"") + L" should point to a class or interface.");
			}

			Ptr<parsing::ParsingError> WfErrors::SharedPointerToNonReferenceType(WfType* node, reflection::description::ITypeInfo* typeInfo)
			{
				return new ParsingError(node, L"B3: A shared pointer" + (typeInfo ? L" \"" + typeInfo->GetTypeFriendlyName() + L"\"" : L"") + L" should point to a class or interface.");
			}

			Ptr<parsing::ParsingError> WfErrors::NullableToNonReferenceType(WfType* node, reflection::description::ITypeInfo* typeInfo)
			{
				return new ParsingError(node, L"B4: A nullable value" + (typeInfo ? L" \"" + typeInfo->GetTypeFriendlyName() + L"\"" : L"") + L" should point to a struct.");
			}

			Ptr<parsing::ParsingError> WfErrors::ChildOfNonReferenceType(WfType* node)
			{
				return new ParsingError(node, L"B5: Only a reference type have child types.");
			}

			Ptr<parsing::ParsingError> WfErrors::TypeNotExists(WfType* node, Ptr<WfLexicalScopeName> scopeName)
			{
				return new ParsingError(node, L"B6: \"" + scopeName->GetFriendlyName() + L"\" is not a type.");
			}

			Ptr<parsing::ParsingError> WfErrors::TypeNotExists(WfType* node, Ptr<WfLexicalSymbol> symbol)
			{
				return new ParsingError(node, L"B6: \"" + symbol->name + L"\" is not a type.");
			}

			Ptr<parsing::ParsingError> WfErrors::TypeNotForValue(WfType* node, reflection::description::ITypeInfo* typeInfo)
			{
				return new ParsingError(node, L"B7: Type \"" + typeInfo->GetTypeFriendlyName() + L"\" is not a type for a value.");
			}

			Ptr<parsing::ParsingError> WfErrors::BreakNotInLoop(WfStatement* node)
			{
				return new ParsingError(node, L"C0: Break statement should appear in a loop.");
			}

			Ptr<parsing::ParsingError> WfErrors::ContinueNotInLoop(WfStatement* node)
			{
				return new ParsingError(node, L"C1: Continue statement should appear in a loop.");
			}

			Ptr<parsing::ParsingError> WfErrors::RethrowNotInCatch(WfStatement* node)
			{
				return new ParsingError(node, L"C2: Re-raise exception statement should appear in catch.");
			}

			Ptr<parsing::ParsingError> WfErrors::TryMissCatchAndFinally(WfStatement* node)
			{
				return new ParsingError(node, L"C3: Try statement should not appear without both catch and finally.");
			}

			Ptr<parsing::ParsingError> WfErrors::ReturnMissExpression(WfStatement* node, reflection::description::ITypeInfo* type)
			{
				return new ParsingError(node, L"C4: Return statement requires an expression of type \"" + type->GetTypeFriendlyName() + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::DeleteNonRawPointer(WfStatement* node, reflection::description::ITypeInfo* type)
			{
				return new ParsingError(node, L"C5: Delete statement cannot apply on an expression of type \"" + type->GetTypeFriendlyName() + L"\", which is expected to be a raw pointer.");
			}

			Ptr<parsing::ParsingError> WfErrors::CannotReturnExpression(WfStatement* node)
			{
				return new ParsingError(node, L"C6: Return statement cannot have a value inside void functions, constructors, destructors or $coroutine expressions.");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongCoPause(WfStatement* node)
			{
				return new ParsingError(node, L"C7: $pause statement should appear inside a $coroutine expression, and it cannot be nested.");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongCoOperator(WfStatement* node)
			{
				return new ParsingError(node, L"C8: $Operator statement should appear inside a coroutine function (which has a functiona body like ${} or $Provider{}).");
			}

			Ptr<parsing::ParsingError> WfErrors::FunctionShouldHaveName(WfDeclaration* node)
			{
				return new ParsingError(node, L"D0: Function should have a name.");
			}

			Ptr<parsing::ParsingError> WfErrors::FunctionShouldHaveImplementation(WfDeclaration* node)
			{
				return new ParsingError(node, L"D0: Function should be implemented.");
			}

			Ptr<parsing::ParsingError> WfErrors::InterfaceMethodShouldNotHaveImplementation(WfDeclaration* node)
			{
				return new ParsingError(node, L"D0: Interface method should not be implemented.");
			}

			Ptr<parsing::ParsingError> WfErrors::DuplicatedDeclaration(WfDeclaration* node, const WString& firstDeclarationCategory)
			{
				return new ParsingError(node, L"D1: Duplicated declaration \"" + node->name.value + L"\", it has already beed defined as a " + firstDeclarationCategory + L".");
			}

			Ptr<parsing::ParsingError> WfErrors::DuplicatedSymbol(WfDeclaration* node, Ptr<WfLexicalSymbol> symbol)
			{
				return new ParsingError(node, L"D2: Duplicated symbol \"" + symbol->name + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::DuplicatedSymbol(WfFunctionArgument* node, Ptr<WfLexicalSymbol> symbol)
			{
				return new ParsingError(node, L"D2: Duplicated symbol \"" + symbol->name + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::DuplicatedSymbol(WfStatement* node, Ptr<WfLexicalSymbol> symbol)
			{
				return new ParsingError(node, L"D2: Duplicated symbol \"" + symbol->name + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::DuplicatedSymbol(WfExpression* node, Ptr<WfLexicalSymbol> symbol)
			{
				return new ParsingError(node, L"D2: Duplicated symbol \"" + symbol->name + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::InterfaceMethodNotImplemented(WfNewInterfaceExpression* node, reflection::description::IMethodInfo* method)
			{
				auto result = ResolveExpressionResult::Method(method);
				return new ParsingError(node, L"D3: Interface method not implemented: " + result.GetFriendlyName() + L".");
			}

			Ptr<parsing::ParsingError> WfErrors::InterfaceMethodNotFound(WfFunctionDeclaration* node, reflection::description::ITypeInfo* interfaceType, reflection::description::ITypeInfo* methodType)
			{
				return new ParsingError(node, L"D4: Interface \"" + interfaceType->GetTypeFriendlyName() + L"\" does not contain method \"" + node->name.value + L"\" which is in type \"" + methodType->GetTypeFriendlyName() + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::CannotPickOverloadedInterfaceMethods(WfExpression* node, collections::List<ResolveExpressionResult>& results)
			{
				WString description;
				FOREACH_INDEXER(ResolveExpressionResult, result, index, results)
				{
					description += L"\r\n\t";
					description += result.GetFriendlyName();
				}
				return new ParsingError(node, L"D5: Cannot decide which function to implement in multiple targets: " + description + L".");
			}

			Ptr<parsing::ParsingError> WfErrors::CannotPickOverloadedImplementMethods(WfFunctionDeclaration* node, reflection::description::ITypeInfo* type)
			{
				return new ParsingError(node, L"D6: There are some other methods named \"" + node->name.value + L"\" whose types are also \"" + type->GetTypeFriendlyName() + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongDeclaration(WfEventDeclaration* node)
			{
				return new ParsingError(node, L"D7: Event \"" + node->name.value + L"\" cannot be defined outside of classes or interfaces.");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongDeclaration(WfPropertyDeclaration* node)
			{
				return new ParsingError(node, L"D7: Property \"" + node->name.value + L"\" cannot be defined outside of classes or interfaces.");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongDeclaration(WfConstructorDeclaration* node)
			{
				return new ParsingError(node, L"D7: Constructor cannot be defined outside of classes.");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongDeclaration(WfDestructorDeclaration* node)
			{
				return new ParsingError(node, L"D7: Destructor cannot be defined outside of classes.");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongDeclaration(WfAutoPropertyDeclaration* node)
			{
				return new ParsingError(node, L"D7: Auto-property cannot be defined outside of classes.");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongDeclarationInInterfaceConstructor(WfDeclaration* node)
			{
				return new ParsingError(node, L"D8: \"" + node->name.value + L"\" cannot be defined in an new interface expression, only functions, variables and auto-properties are allowed.");
			}

			Ptr<parsing::ParsingError> WfErrors::EnumValuesNotConsecutiveFromZero(WfEnumDeclaration* node)
			{
				return new ParsingError(node, L"D9: Item values in enum \"" + node->name.value + L"\" should be consecutive and starts from zero, like 0, 1, 2, 3, 4, ...");
			}

			Ptr<parsing::ParsingError> WfErrors::FlagValuesNotConsecutiveFromZero(WfEnumDeclaration* node)
			{
				return new ParsingError(node, L"D9: Item values in flag enum \"" + node->name.value + L"\" should be consecutive powers of 2 and starts from zero, like 0, 1, 2, 4, 8, ...");
			}

			Ptr<parsing::ParsingError> WfErrors::FlagValueNotExists(WfEnumItemIntersection* node, WfEnumDeclaration* owner)
			{
				return new ParsingError(node, L"D10: Item \"" + node->name.value + L" does not exists in the current flag enum \"" + owner->name.value + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::DuplicatedEnumValue(WfEnumItem* node, WfEnumDeclaration* owner)
			{
				return new ParsingError(node, L"D11: Item \"" + node->name.value + L" already exists in the current enum \"" + owner->name.value + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::StructContainsNonValueType(WfStructMember* node, WfStructDeclaration* owner)
			{
				return new ParsingError(node, L"D12: Type of member \"" + node->name.value + L"\" of struct \"" + owner->name.value + L"\" is not value type.");
			}

			Ptr<parsing::ParsingError> WfErrors::StructRecursivelyIncludeItself(WfStructDeclaration* node, const WString& path)
			{
				return new ParsingError(node, L"D13: Struct \"" + node->name.value + L"\" recursively include itself via \"" + path + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::DuplicatedStructMember(WfStructMember* node, WfStructDeclaration* owner)
			{
				return new ParsingError(node, L"D14: Member \"" + node->name.value + L"\" already exists in struct \"" + owner->name.value + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::AttributeNotExists(WfAttribute* node)
			{
				return new ParsingError(node, L"D15: Attribute \"" + node->category.value + L":" + node->name.value + L"\" does not exist.");
			}

			Ptr<parsing::ParsingError> WfErrors::AttributeMissValue(WfAttribute* node)
			{
				return new ParsingError(node, L"D15: Value of attribute \"" + node->category.value + L":" + node->name.value + L"\" is missing.");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongUsingPathWildCard(WfModuleUsingPath* node)
			{
				return new ParsingError(node, L"E0: Wild card \"*\" should only appear in the last item of the using path and should appear once.");
			}

			Ptr<parsing::ParsingError> WfErrors::TopQualifiedSymbolNotExists(parsing::ParsingTreeCustomBase* node, const WString& name)
			{
				return new ParsingError(node, L"F0: Top qualified symbol \"" + name + L"\" does not exist.");
			}

			Ptr<parsing::ParsingError> WfErrors::ChildSymbolNotExists(parsing::ParsingTreeCustomBase* node, Ptr<WfLexicalScopeName> scopeName, const WString& name)
			{
				return new ParsingError(node, L"F1: Symbol \"" + name + L"\" does not exist in \"" + scopeName->GetFriendlyName() + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::MemberNotExists(parsing::ParsingTreeCustomBase* node, reflection::description::ITypeDescriptor* typeDescriptor, const WString& name)
			{
				return new ParsingError(node, L"F1: Member \"" + name + L"\" does not exist in \"" + typeDescriptor->GetTypeName() + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::ReferenceNotExists(parsing::ParsingTreeCustomBase* node, const WString& name)
			{
				return new ParsingError(node, L"F2: Symbol \"" + name + L"\" does not exist in the current scope.");
			}

			Ptr<parsing::ParsingError> WfErrors::TooManyTargets(parsing::ParsingTreeCustomBase* node, collections::List<ResolveExpressionResult>& results, const WString& name)
			{
				WString description;
				FOREACH_INDEXER(ResolveExpressionResult, result, index, results)
				{
					description += L"\r\n\t";
					description += result.GetFriendlyName();
				}
				return new ParsingError(node, L"F3: Symbol \"" + name + L"\" references to too many targets: " + description + L".");
			}

			Ptr<parsing::ParsingError> WfErrors::EnumItemNotExists(parsing::ParsingTreeCustomBase* node, reflection::description::ITypeDescriptor* typeDescriptor, const WString& name)
			{
				return new ParsingError(node, L"F2: Enum item \"" + name + L"\" does not exist in enum type \"" + typeDescriptor->GetTypeName() + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongClassMemberConfig(WfDeclaration* node)
			{
				return new ParsingError(node, L"G1: Class member \"" + node->name.value + L"\" cannot be static or override because it is not a function or a auto-property.");
			}

			Ptr<parsing::ParsingError> WfErrors::FunctionInNewTypeExpressionCannotBeStatic(WfDeclaration* node)
			{
				return new ParsingError(node, L"G1: Class member \"" + node->name.value + L"\" cannot be static because it is in a new interface expression.");
			}

			Ptr<parsing::ParsingError> WfErrors::AutoPropertyCannotBeNormalOutsideOfClass(WfDeclaration* node)
			{
				return new ParsingError(node, L"G1: Auto property \"" + node->name.value + L"\" cannot be normal outside of classes or interfaces.");
			}

			Ptr<parsing::ParsingError> WfErrors::AutoPropertyCannotBeStatic(WfDeclaration* node)
			{
				return new ParsingError(node, L"G1: Auto property \"" + node->name.value + L"\" cannot be static  .");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongClassMember(WfNamespaceDeclaration* node)
			{
				return new ParsingError(node, L"G2: Namespace \"" + node->name.value + L"\" cannot be a class member.");
			}

			Ptr<parsing::ParsingError> WfErrors::PropertyGetterNotFound(WfPropertyDeclaration* node, WfClassDeclaration* classDecl)
			{
				return new ParsingError(node, L"G3: Cannot find the getter \"" + node->getter.value + L"\" of property \"" + node->name.value + L"\" in type \"" + classDecl->name.value + L"\". A property getter should be a non-static method in the same type.");
			}

			Ptr<parsing::ParsingError> WfErrors::PropertySetterNotFound(WfPropertyDeclaration* node, WfClassDeclaration* classDecl)
			{
				return new ParsingError(node, L"G3: Cannot find the setter \"" + node->setter.value + L"\" of property \"" + node->name.value + L"\" in type \"" + classDecl->name.value + L"\". A property setter should be a non-static method in the same type.");
			}

			Ptr<parsing::ParsingError> WfErrors::PropertyEventNotFound(WfPropertyDeclaration* node, WfClassDeclaration* classDecl)
			{
				return new ParsingError(node, L"G3: Cannot find the event \"" + node->valueChangedEvent.value + L"\" of property \"" + node->name.value + L"\" in type \"" + classDecl->name.value + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::TooManyPropertyGetter(WfPropertyDeclaration* node, WfClassDeclaration* classDecl)
			{
				return new ParsingError(node, L"G4: Too many symbols found for the getter \"" + node->getter.value + L"\" of property \"" + node->name.value + L"\" in type \"" + classDecl->name.value + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::TooManyPropertySetter(WfPropertyDeclaration* node, WfClassDeclaration* classDecl)
			{
				return new ParsingError(node, L"G4: Too many symbols found for the setter \"" + node->setter.value + L"\" of property \"" + node->name.value + L"\" in type \"" + classDecl->name.value + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::TooManyPropertyEvent(WfPropertyDeclaration* node, WfClassDeclaration* classDecl)
			{
				return new ParsingError(node, L"G4: Too many symbols found for the event \"" + node->valueChangedEvent.value + L"\" of property \"" + node->name.value + L"\" in type \"" + classDecl->name.value + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::PropertyGetterTypeMismatched(WfPropertyDeclaration* node, WfClassDeclaration* classDecl)
			{
				return new ParsingError(node, L"G5: Cannot match the getter \"" + node->getter.value + L"\" of property \"" + node->name.value + L"\" in type \"" + classDecl->name.value + L"\". A property getter should have no argument, and its return type should be identical to the property type.");
			}

			Ptr<parsing::ParsingError> WfErrors::PropertySetterTypeMismatched(WfPropertyDeclaration* node, WfClassDeclaration* classDecl)
			{
				return new ParsingError(node, L"G5: Cannot match the setter \"" + node->getter.value + L"\" of property \"" + node->name.value + L"\" in type \"" + classDecl->name.value + L"\". A property setter should have no return value and have only one argument, and the argument type should be identical to the property type.");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongBaseType(WfClassDeclaration* node, WfType* type)
			{
				return new ParsingError(node, L"G6: A base type of the type \"" + node->name.value + L"\" should be another custom type, it cannot be any predefined type, pointer type, shared pointer type, nullable type, collection type, or function type");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongBaseTypeOfClass(WfClassDeclaration* node, reflection::description::ITypeDescriptor* type)
			{
				return new ParsingError(node, L"G6: Base type \"" + type->GetTypeName() + L"\" of class \"" + node->name.value + L"\" is not a class, or it is a class but it is not aggregatable.");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongBaseTypeOfInterface(WfClassDeclaration* node, reflection::description::ITypeDescriptor* type)
			{
				return new ParsingError(node, L"G6: Base type \"" + type->GetTypeName() + L"\" of interface \"" + node->name.value + L"\" is not an interface.");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongInterfaceBaseType(WfClassDeclaration* node, reflection::description::ITypeDescriptor* type)
			{
				Ptr<ITypeInfo> proxy = TypeInfoRetriver<Ptr<IValueInterfaceProxy>>::CreateTypeInfo();
				return new ParsingError(node, L"G6: Interface \"" + type->GetTypeName() + L"\" should contain a constructor receiving an \"" + proxy->GetTypeFriendlyName() + L"\" to be the base type of \"" + node->name.value + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::ClassWithInterfaceConstructor(WfClassDeclaration* node)
			{
				return new ParsingError(node, L"G7: Cannot use interface constructor type on class \"" + node->name.value + L"\".");
			}

			Ptr<parsing::ParsingError> WfErrors::OverrideShouldImplementInterfaceMethod(WfFunctionDeclaration* node)
			{
				return new ParsingError(node, L"G8: Function \"" + node->name.value + L"\" cannot be override because it doesn't implement any interface methods.");
			}

			Ptr<parsing::ParsingError> WfErrors::OverrideShouldImplementInterfaceMethod(WfAutoPropertyDeclaration* node)
			{
				return new ParsingError(node, L"G8: Auto-property \"" + node->name.value + L"\" cannot be override because it doesn't implement any interface methods.");
			}

			Ptr<parsing::ParsingError> WfErrors::MissingFieldType(WfVariableDeclaration* node)
			{
				return new ParsingError(node, L"G9: Type of field \"" + node->name.value + L"\" is missing.");
			}

			Ptr<parsing::ParsingError> WfErrors::DuplicatedBaseClass(WfClassDeclaration* node, reflection::description::ITypeDescriptor* type)
			{
				return new ParsingError(node, L"G10: Class \"" + node->name.value + L"\" inherits from another class \"" + type->GetTypeName() + L"\" for multiple times.");
			}

			Ptr<parsing::ParsingError> WfErrors::DuplicatedBaseInterface(WfClassDeclaration* node, reflection::description::ITypeDescriptor* type)
			{
				return new ParsingError(node, L"G10: Interface \"" + type->GetTypeName() + L"\" directly or indirectly inherits from itself.");
			}

			Ptr<parsing::ParsingError> WfErrors::WrongBaseConstructorCall(WfBaseConstructorCall* node, reflection::description::ITypeDescriptor* type)
			{
				return new ParsingError(node, L"G11: Type \"" + type->GetTypeName() + L"\" is not a base type of this class.");
			}

			Ptr<parsing::ParsingError> WfErrors::DuplicatedBaseConstructorCall(WfBaseConstructorCall* node, reflection::description::ITypeDescriptor* type)
			{
				return new ParsingError(node, L"G12: Base type \"" + type->GetTypeName() + L"\" has already been initialized.");
			}
			
			Ptr<parsing::ParsingError> WfErrors::TooManyDestructor(WfDestructorDeclaration* node, WfClassDeclaration* classDecl)
			{
				return new ParsingError(node, L"G13: Class \"" + classDecl->name.value + L"\" has too many destructors.");
			}

			Ptr<parsing::ParsingError> WfErrors::AutoPropertyShouldBeInitialized(WfAutoPropertyDeclaration* node)
			{
				return new ParsingError(node, L"G13: Auto property \"" + node->name.value + L"\" should be initialized.");
			}

			Ptr<parsing::ParsingError> WfErrors::AutoPropertyCannotBeInitializedInInterface(WfAutoPropertyDeclaration* node, WfClassDeclaration* classDecl)
			{
				return new ParsingError(node, L"G13: Auto property \"" + node->name.value + L"\" cannot be initialized in interface \"" + classDecl->name.value + L"\".");
			}
		}
	}
}