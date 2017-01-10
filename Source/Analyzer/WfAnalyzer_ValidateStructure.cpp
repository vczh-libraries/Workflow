#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace parsing;
			using namespace reflection::description;

/***********************************************************************
ValidateStructureContext
***********************************************************************/

			ValidateStructureContext::ValidateStructureContext()
				:currentBindExpression(0)
				, currentObserveExpression(0)
				, currentLoopStatement(0)
				, currentCatchStatement(0)
			{
			}

/***********************************************************************
ValidateStructure(Type)
***********************************************************************/

			class ValidateReferenceTypeVisitor : public Object, public WfType::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				bool									result;

				ValidateReferenceTypeVisitor(WfLexicalScopeManager* _manager)
					:manager(_manager)
					, result(true)
				{
				}
				
				void Visit(WfPredefinedType* node)override
				{
				}

				void Visit(WfTopQualifiedType* node)override
				{
				}

				void Visit(WfReferenceType* node)override
				{
				}

				void Visit(WfRawPointerType* node)override
				{
					result = false;
				}

				void Visit(WfSharedPointerType* node)override
				{
					result = false;
				}

				void Visit(WfNullableType* node)override
				{
					result = false;
				}

				void Visit(WfEnumerableType* node)override
				{
					result = false;
				}

				void Visit(WfMapType* node)override
				{
					result = false;
				}

				void Visit(WfFunctionType* node)override
				{
					result = false;
				}

				void Visit(WfChildType* node)override
				{
					node->parent->Accept(this);
				}

				static bool Execute(Ptr<WfType> type, WfLexicalScopeManager* manager)
				{
					ValidateReferenceTypeVisitor visitor(manager);
					type->Accept(&visitor);
					return visitor.result;
				}
			};

			class ValidateStructureTypeVisitor : public Object, public WfType::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				ValidateTypeStragety					strategy;
				WfClassDeclaration*						classDecl;

				ValidateStructureTypeVisitor(WfLexicalScopeManager* _manager, ValidateTypeStragety _strategy, WfClassDeclaration* _classDecl)
					:manager(_manager)
					, strategy(_strategy)
					, classDecl(_classDecl)
				{
				}
				
				void Visit(WfPredefinedType* node)override
				{
					switch (node->name)
					{
					case WfPredefinedTypeName::Void:
						switch (strategy)
						{
						case ValidateTypeStragety::Value:
							manager->errors.Add(WfErrors::WrongVoidType(node));
							break;
						case ValidateTypeStragety::BaseType:
							manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
						default:;
						}
						break;
					case WfPredefinedTypeName::Interface:
						switch (strategy)
						{
						case ValidateTypeStragety::ReturnType:
						case ValidateTypeStragety::Value:
							manager->errors.Add(WfErrors::WrongInterfaceType(node));
							break;
						default:;
						}
					default:;
					}
				}

				void Visit(WfTopQualifiedType* node)override
				{
				}

				void Visit(WfReferenceType* node)override
				{
				}

				void Visit(WfRawPointerType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					if (!ValidateReferenceTypeVisitor::Execute(node->element, manager))
					{
						manager->errors.Add(WfErrors::RawPointerToNonReferenceType(node));
					}
					else if (auto predefinedType = node->element.Cast<WfPredefinedType>())
					{
						if (predefinedType->name != WfPredefinedTypeName::Interface)
						{
							manager->errors.Add(WfErrors::RawPointerToNonReferenceType(node));
						}
					}
				}

				void Visit(WfSharedPointerType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					if (!ValidateReferenceTypeVisitor::Execute(node->element, manager))
					{
						manager->errors.Add(WfErrors::SharedPointerToNonReferenceType(node));
					}
					else if (auto predefinedType = node->element.Cast<WfPredefinedType>())
					{
						if (predefinedType->name != WfPredefinedTypeName::Interface)
						{
							manager->errors.Add(WfErrors::SharedPointerToNonReferenceType(node));
						}
					}
				}

				void Visit(WfNullableType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					if (!ValidateReferenceTypeVisitor::Execute(node->element, manager))
					{
						manager->errors.Add(WfErrors::NullableToNonReferenceType(node));
					}
					else if (auto predefinedType = node->element.Cast<WfPredefinedType>())
					{
						switch (predefinedType->name)
						{
						case WfPredefinedTypeName::Void:
						case WfPredefinedTypeName::Object:
						case WfPredefinedTypeName::Interface:
							manager->errors.Add(WfErrors::NullableToNonReferenceType(node));
							break;
						default:;
						}
					}
				}

				void Visit(WfEnumerableType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					ValidateTypeStructure(manager, node->element);
				}

				void Visit(WfMapType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					if (node->key)
					{
						ValidateTypeStructure(manager, node->key);
					}
					ValidateTypeStructure(manager, node->value);
				}

				void Visit(WfFunctionType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					ValidateTypeStructure(manager, node->result, ValidateTypeStragety::ReturnType);
					FOREACH(Ptr<WfType>, argument, node->arguments)
					{
						ValidateTypeStructure(manager, argument);
					}
				}

				void Visit(WfChildType* node)override
				{
					if (!ValidateReferenceTypeVisitor::Execute(node->parent, manager))
					{
						manager->errors.Add(WfErrors::ChildOfNonReferenceType(node));
					}
				}

				static void Execute(Ptr<WfType> type, WfLexicalScopeManager* manager, ValidateTypeStragety strategy, WfClassDeclaration* classDecl)
				{
					ValidateStructureTypeVisitor visitor(manager, strategy, classDecl);
					type->Accept(&visitor);
				}
			};

/***********************************************************************
ValidateStructure(Declaration)
***********************************************************************/

			class ValidateStructureDeclarationVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				WfClassDeclaration*						classDecl;
				ParsingTreeCustomBase*					source;

				ValidateStructureDeclarationVisitor(WfLexicalScopeManager* _manager, WfClassDeclaration* _classDecl, ParsingTreeCustomBase* _source)
					:manager(_manager)
					, classDecl(_classDecl)
					, source(_source)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					if (auto classMember = dynamic_cast<WfClassMember*>(source))
					{
						manager->errors.Add(WfErrors::WrongClassMember(node));
					}
					else if (dynamic_cast<WfNewInterfaceExpression*>(source))
					{
						manager->errors.Add(WfErrors::WrongDeclarationInInterfaceConstructor(node));
					}

					for (vint i = 0; i < node->declarations.Count(); i++)
					{
						ValidateDeclarationStructure(manager, node->declarations[i]);
					}
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					if (auto classMember = dynamic_cast<WfClassMember*>(source))
					{
						switch (classDecl->kind)
						{
						case WfClassKind::Class:
							{
								switch (classMember->kind)
								{
								case WfClassMemberKind::Normal:
								case WfClassMemberKind::Static:
									if (!node->statement)
									{
										manager->errors.Add(WfErrors::FunctionShouldHaveImplementation(node));
									}
									break;
								case WfClassMemberKind::Override:
									manager->errors.Add(WfErrors::OverrideShouldImplementInterfaceMethod(node));
									break;
								}
							}
							break;
						case WfClassKind::Interface:
							{
								switch (classMember->kind)
								{
								case WfClassMemberKind::Normal:
									if (node->statement)
									{
										manager->errors.Add(WfErrors::InterfaceMethodShouldNotHaveImplementation(node));
									}
									break;
								case WfClassMemberKind::Static:
									if (!node->statement)
									{
										manager->errors.Add(WfErrors::FunctionShouldHaveImplementation(node));
									}
									break;
								case WfClassMemberKind::Override:
									manager->errors.Add(WfErrors::OverrideShouldImplementInterfaceMethod(node));
									break;
								}
							}
							break;
						}
					}
					else
					{
						if (!node->statement)
						{
							manager->errors.Add(WfErrors::FunctionShouldHaveImplementation(node));
						}
					}

					if (node->anonymity == WfFunctionAnonymity::Anonymous)
					{
						if (!source || !dynamic_cast<WfFunctionExpression*>(source))
						{
							manager->errors.Add(WfErrors::FunctionShouldHaveName(node));
						}
					}


					ValidateTypeStructure(manager, node->returnType, ValidateTypeStragety::ReturnType);
					FOREACH(Ptr<WfFunctionArgument>, argument, node->arguments)
					{
						ValidateTypeStructure(manager, argument->type);
					}

					if (node->statement)
					{
						ValidateStructureContext context;
						ValidateStatementStructure(manager, &context, node->statement);
					}
				}

				void Visit(WfVariableDeclaration* node)override
				{
					if (auto classMember = dynamic_cast<WfClassMember*>(source))
					{
						switch (classMember->kind)
						{
						case WfClassMemberKind::Normal:
							break;
						case WfClassMemberKind::Static:
						case WfClassMemberKind::Override:
							manager->errors.Add(WfErrors::NonFunctionClassMemberCannotBeStaticOrOverride(classMember));
							break;
						}
					}

					if (node->type)
					{
						ValidateTypeStructure(manager, node->type);
					}
					else if (classDecl)
					{
						manager->errors.Add(WfErrors::MissingFieldType(node));
					}
					ValidateStructureContext context;
					ValidateExpressionStructure(manager, &context, node->expression);
				}

				void Visit(WfEventDeclaration* node)override
				{
					if (auto classMember = dynamic_cast<WfClassMember*>(source))
					{
						switch (classMember->kind)
						{
						case WfClassMemberKind::Normal:
							break;
						case WfClassMemberKind::Static:
						case WfClassMemberKind::Override:
							manager->errors.Add(WfErrors::NonFunctionClassMemberCannotBeStaticOrOverride(classMember));
							break;
						}

						FOREACH(Ptr<WfType>, argument, node->arguments)
						{
							ValidateTypeStructure(manager, argument);
						}
					}
					else if (dynamic_cast<WfNewInterfaceExpression*>(source))
					{
						manager->errors.Add(WfErrors::WrongDeclarationInInterfaceConstructor(node));
					}
					else
					{
						manager->errors.Add(WfErrors::WrongDeclaration(node));
					}
				}

				void Visit(WfPropertyDeclaration* node)override
				{
					if (auto classMember = dynamic_cast<WfClassMember*>(source))
					{
						switch (classMember->kind)
						{
						case WfClassMemberKind::Normal:
							break;
						case WfClassMemberKind::Static:
						case WfClassMemberKind::Override:
							manager->errors.Add(WfErrors::NonFunctionClassMemberCannotBeStaticOrOverride(classMember));
							break;
						}

						ValidateTypeStructure(manager, node->type);

						Ptr<WfClassMember> getter, setter, valueChangedEvent;
						bool duplicateGetter = false;
						bool duplicateSetter = false;
						bool duplicateEvent = false;
						FOREACH(Ptr<WfClassMember>, member, classDecl->members)
						{
							if (!duplicateGetter && member->declaration->name.value == node->getter.value)
							{
								if (getter)
								{
									duplicateGetter = true;
									manager->errors.Add(WfErrors::TooManyPropertyGetter(node, classDecl));
								}
								else
								{
									getter = member;
								}
							}

							if (!duplicateSetter && node->setter.value != L"" && member->declaration->name.value == node->setter.value)
							{
								if (setter)
								{
									duplicateSetter = true;
									manager->errors.Add(WfErrors::TooManyPropertySetter(node, classDecl));
								}
								else
								{
									setter = member;
								}
							}

							if (!duplicateEvent && node->valueChangedEvent.value != L"" && member->declaration->name.value == node->valueChangedEvent.value)
							{
								if (valueChangedEvent)
								{
									duplicateEvent = true;
									manager->errors.Add(WfErrors::TooManyPropertyEvent(node, classDecl));
								}
								else
								{
									valueChangedEvent = member;
								}
							}
						}

						if (!getter || getter->kind == WfClassMemberKind::Static || !getter->declaration.Cast<WfFunctionDeclaration>())
						{
							manager->errors.Add(WfErrors::PropertyGetterNotFound(node, classDecl));
						}

						if (node->setter.value != L"" && (!setter || setter->kind == WfClassMemberKind::Static || !setter->declaration.Cast<WfFunctionDeclaration>()))
						{
							manager->errors.Add(WfErrors::PropertySetterNotFound(node, classDecl));
						}

						if (node->valueChangedEvent.value != L"" && (!valueChangedEvent || valueChangedEvent->kind == WfClassMemberKind::Static || !valueChangedEvent->declaration.Cast<WfEventDeclaration>()))
						{
							manager->errors.Add(WfErrors::PropertyEventNotFound(node, classDecl));
						}
					}
					else if (dynamic_cast<WfNewInterfaceExpression*>(source))
					{
						manager->errors.Add(WfErrors::WrongDeclarationInInterfaceConstructor(node));
					}
					else
					{
						manager->errors.Add(WfErrors::WrongDeclaration(node));
					}
				}

				void Visit(WfConstructorDeclaration* node)override
				{
					if (auto classMember = dynamic_cast<WfClassMember*>(source))
					{
						switch (classMember->kind)
						{
						case WfClassMemberKind::Normal:
							break;
						case WfClassMemberKind::Static:
						case WfClassMemberKind::Override:
							manager->errors.Add(WfErrors::NonFunctionClassMemberCannotBeStaticOrOverride(classMember));
							break;
						}

						if (classDecl->kind != WfClassKind::Class)
						{
							manager->errors.Add(WfErrors::WrongDeclaration(node));
						}

						FOREACH(Ptr<WfBaseConstructorCall>, call, node->baseConstructorCalls)
						{
							ValidateTypeStructure(manager, call->type, ValidateTypeStragety::BaseType, classDecl);
							FOREACH(Ptr<WfExpression>, argument, call->arguments)
							{
								ValidateStructureContext context;
								ValidateExpressionStructure(manager, &context, argument);
							}
						}
						{
							ValidateStructureContext context;
							ValidateStatementStructure(manager, &context, node->statement);
						}
					}
					else
					{
						manager->errors.Add(WfErrors::WrongDeclaration(node));
					}
				}

				void Visit(WfDestructorDeclaration* node)override
				{
					if (auto classMember = dynamic_cast<WfClassMember*>(source))
					{
						switch (classMember->kind)
						{
						case WfClassMemberKind::Normal:
							break;
						case WfClassMemberKind::Static:
						case WfClassMemberKind::Override:
							manager->errors.Add(WfErrors::NonFunctionClassMemberCannotBeStaticOrOverride(classMember));
							break;
						}

						if (classDecl->kind != WfClassKind::Class)
						{
							manager->errors.Add(WfErrors::WrongDeclaration(node));
						}

						ValidateStructureContext context;
						ValidateStatementStructure(manager, &context, node->statement);
					}
					else
					{
						manager->errors.Add(WfErrors::WrongDeclaration(node));
					}
				}

				void Visit(WfClassDeclaration* node)override
				{
					if (auto classMember = dynamic_cast<WfClassMember*>(source))
					{
						switch (classMember->kind)
						{
						case WfClassMemberKind::Normal:
							break;
						case WfClassMemberKind::Static:
						case WfClassMemberKind::Override:
							manager->errors.Add(WfErrors::NonFunctionClassMemberCannotBeStaticOrOverride(classMember));
							break;
						}
					}
					else if (dynamic_cast<WfNewInterfaceExpression*>(source))
					{
						manager->errors.Add(WfErrors::WrongDeclarationInInterfaceConstructor(node));
					}

					switch (node->kind)
					{
					case WfClassKind::Class:
						{
							if (node->constructorType != WfConstructorType::Undefined)
							{
								manager->errors.Add(WfErrors::ClassWithInterfaceConstructor(node));
							}
							if (!From(node->members)
								.Select([](Ptr<WfClassMember> member)
								{
									return member->declaration;
								})
								.FindType<WfConstructorDeclaration>()
								.First(nullptr))
							{
								auto member = MakePtr<WfClassMember>();
								member->kind = WfClassMemberKind::Normal;
								node->members.Add(member);

								auto ctor = MakePtr<WfConstructorDeclaration>();
								member->declaration = ctor;
								ctor->codeRange = node->codeRange;
								
								auto stat = MakePtr<WfBlockStatement>();
								ctor->statement = stat;
								stat->codeRange = node->codeRange;
							}
						}
						break;
					case WfClassKind::Interface:
						break;
					}

					FOREACH(Ptr<WfType>, type, node->baseTypes)
					{
						ValidateTypeStructure(manager, type, ValidateTypeStragety::BaseType, node);
					}

					Ptr<WfDestructorDeclaration> dtor;
					FOREACH(Ptr<WfClassMember>, member, node->members)
					{
						ValidateDeclarationStructure(manager, member->declaration, node, member.Obj());
						auto decl = member->declaration.Cast<WfDestructorDeclaration>();
						if (decl)
						{
							if (dtor)
							{
								manager->errors.Add(WfErrors::TooManyDestructor(decl.Obj(), node));
							}
							else
							{
								dtor = decl;
							}
						}
					}
				}

				void Visit(WfEnumDeclaration* node)override
				{
					vuint64_t current = 0;
					bool reportedNotConsecutive = false;
					SortedList<WString> discoveredItems;
					FOREACH(Ptr<WfEnumItem>, item, node->items)
					{
						switch (item->kind)
						{
						case WfEnumItemKind::Constant:
							{
								vuint64_t value = -1;
								TypedValueSerializerProvider<vuint64_t>::Deserialize(item->number.value, value);
								if (!reportedNotConsecutive && value != current)
								{
									reportedNotConsecutive = true;
									switch (node->kind)
									{
									case WfEnumKind::Normal:
										manager->errors.Add(WfErrors::EnumValuesNotConsecutiveFromZero(node));
										break;
									case WfEnumKind::Flag:
										manager->errors.Add(WfErrors::FlagValuesNotConsecutiveFromZero(node));
										break;
									}
								}
								switch (node->kind)
								{
								case WfEnumKind::Normal:
									current++;
									break;
								case WfEnumKind::Flag:
									current = current == 0 ? 1 : current * 2;
									break;
								}
							}
							break;
						case WfEnumItemKind::Intersection:
							FOREACH(Ptr<WfEnumItemIntersection>, enumInt, item->intersections)
							{
								if (!discoveredItems.Contains(enumInt->name.value))
								{
									manager->errors.Add(WfErrors::FlagValueNotExists(enumInt.Obj(), node));
								}
							}
							break;
						}

						if (discoveredItems.Contains(item->name.value))
						{
							manager->errors.Add(WfErrors::DuplicatedEnumValue(item.Obj(), node));
						}
						else
						{
							discoveredItems.Add(item->name.value);
						}
					}
				}

				void Visit(WfStructDeclaration* node)override
				{
					SortedList<WString> discoveredItems;
					FOREACH(Ptr<WfStructMember>, member, node->members)
					{
						if (discoveredItems.Contains(member->name.value))
						{
							manager->errors.Add(WfErrors::DuplicatedStructMember(member.Obj(), node));
						}
						else
						{
							discoveredItems.Add(member->name.value);
						}
					}
				}

				static void Execute(Ptr<WfDeclaration> declaration, WfLexicalScopeManager* manager, WfClassDeclaration* classDecl, ParsingTreeCustomBase* source)
				{
					ValidateStructureDeclarationVisitor visitor(manager, classDecl, source);
					declaration->Accept(&visitor);
				}
			};

/***********************************************************************
ValidateStructure(Statement)
***********************************************************************/

			class ValidateStructureStatementVisitor : public Object, public WfStatement::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				ValidateStructureContext*				context;
				Ptr<WfStatement>						result;

				ValidateStructureStatementVisitor(WfLexicalScopeManager* _manager, ValidateStructureContext* _context)
					:manager(_manager)
					, context(_context)
				{
				}

				void Visit(WfBreakStatement* node)override
				{
					if (!context->currentLoopStatement)
					{
						manager->errors.Add(WfErrors::BreakNotInLoop(node));
					}
				}

				void Visit(WfContinueStatement* node)override
				{
					if (!context->currentLoopStatement)
					{
						manager->errors.Add(WfErrors::ContinueNotInLoop(node));
					}
				}

				void Visit(WfReturnStatement* node)override
				{
					if (node->expression)
					{
						ValidateExpressionStructure(manager, context, node->expression);
					}
				}

				void Visit(WfDeleteStatement* node)override
				{
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfRaiseExceptionStatement* node)override
				{
					if (node->expression)
					{
						ValidateExpressionStructure(manager, context, node->expression);
					}
					else if (!context->currentCatchStatement)
					{
						manager->errors.Add(WfErrors::RethrowNotInCatch(node));
					}
				}

				void Visit(WfIfStatement* node)override
				{
					if (node->type)
					{
						ValidateTypeStructure(manager, node->type);
					}
					ValidateExpressionStructure(manager, context, node->expression);
					ValidateStatementStructure(manager, context, node->trueBranch);
					if (node->falseBranch)
					{
						ValidateStatementStructure(manager, context, node->falseBranch);
					}
				}

				void Visit(WfSwitchStatement* node)override
				{
					ValidateExpressionStructure(manager, context, node->expression);
					FOREACH(Ptr<WfSwitchCase>, switchCase, node->caseBranches)
					{
						ValidateExpressionStructure(manager, context, switchCase->expression);
						ValidateStatementStructure(manager, context, switchCase->statement);
					}
					if (node->defaultBranch)
					{
						ValidateStatementStructure(manager, context, node->defaultBranch);
					}
				}

				void Visit(WfWhileStatement* node)override
				{
					auto oldLoop = context->currentLoopStatement;
					context->currentLoopStatement = node;
					ValidateExpressionStructure(manager, context, node->condition);
					ValidateStatementStructure(manager, context, node->statement);
					context->currentLoopStatement = oldLoop;
				}

				void Visit(WfForEachStatement* node)override
				{
					auto oldLoop = context->currentLoopStatement;
					context->currentLoopStatement = node;
					ValidateExpressionStructure(manager, context, node->collection);
					ValidateStatementStructure(manager, context, node->statement);
					context->currentLoopStatement = oldLoop;
				}

				void Visit(WfTryStatement* node)override
				{
					ValidateStatementStructure(manager, context, node->protectedStatement);
					if (node->catchStatement)
					{
						auto oldCatch = context->currentCatchStatement;
						context->currentCatchStatement = node->catchStatement.Obj();
						ValidateStatementStructure(manager, context, node->catchStatement);
						context->currentCatchStatement = oldCatch;
					}
					if (node->finallyStatement)
					{
						ValidateStatementStructure(manager, context, node->finallyStatement);
					}
					if (!node->catchStatement && !node->finallyStatement)
					{
						manager->errors.Add(WfErrors::TryMissCatchAndFinally(node));
					}
				}

				void Visit(WfBlockStatement* node)override
				{
					for (vint i = 0; i < node->statements.Count(); i++)
					{
						ValidateStatementStructure(manager, context, node->statements[i]);
					}
				}

				void Visit(WfExpressionStatement* node)override
				{
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfVariableStatement* node)override
				{
					ValidateDeclarationStructure(manager, node->variable, nullptr, node);
				}

				static void Execute(Ptr<WfStatement>& statement, WfLexicalScopeManager* manager, ValidateStructureContext* context)
				{
					ValidateStructureStatementVisitor visitor(manager, context);
					statement->Accept(&visitor);
					if (visitor.result)
					{
						statement = visitor.result;
					}
				}
			};

/***********************************************************************
ValidateStructure(Expression)
***********************************************************************/

			class ValidateStructureExpressionVisitor : public Object, public WfExpression::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				ValidateStructureContext*				context;
				Ptr<WfExpression>						result;

				ValidateStructureExpressionVisitor(WfLexicalScopeManager* _manager, ValidateStructureContext* _context)
					:manager(_manager)
					, context(_context)
				{
				}

				void Visit(WfThisExpression* node)override
				{
					if (context->currentBindExpression)
					{
						manager->errors.Add(WfErrors::WrongThisExpression(node));
					}
				}

				void Visit(WfTopQualifiedExpression* node)override
				{
				}

				void Visit(WfReferenceExpression* node)override
				{
				}

				void Visit(WfOrderedNameExpression* node)override
				{
				}

				void Visit(WfOrderedLambdaExpression* node)override
				{
					auto oldBind = context->currentBindExpression;
					context->currentBindExpression = 0;
					ValidateExpressionStructure(manager, context, node->body);
					context->currentBindExpression = oldBind;
				}

				void Visit(WfMemberExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->parent);
				}

				void Visit(WfChildExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->parent);
				}

				void Visit(WfLiteralExpression* node)override
				{
				}

				void Visit(WfFloatingExpression* node)override
				{
				}

				void Visit(WfIntegerExpression* node)override
				{
				}

				void Visit(WfStringExpression* node)override
				{
				}

				void Visit(WfFormatExpression* node)override
				{
					if (!node->expandedExpression)
					{
						List<Ptr<WfExpression>> expressions;
						const wchar_t* reading = node->value.value.Buffer();

						while (*reading)
						{
							const wchar_t* begin = wcsstr(reading, L"$(");
							if (begin)
							{
								Ptr<WfStringExpression> expression = new WfStringExpression;
								expression->codeRange = node->codeRange;
								expression->value.value = WString(reading, vint(begin - reading));
								expressions.Add(expression);
							}
							else
							{
								break;
							}

							const wchar_t* end = begin + 2;
							vint counter = 1;
							while (wchar_t c = *end++)
							{
								switch (c)
								{
								case L'(':
									counter++;
									break;
								case L')':
									counter--;
									break;
								}
								if (counter == 0)
								{
									break;
								}
							}

							if (counter != 0)
							{
								auto error = WfErrors::WrongFormatStringSyntax(node);
								error->errorMessage += L" (Does not find matched close bracket.)";
								manager->errors.Add(error);
								return;
							}
							else
							{
								WString input(begin + 2, vint(end - begin - 3));
								List<Ptr<ParsingError>> errors;
								if (auto expression = WfParseExpression(input, manager->parsingTable, errors))
								{
									expressions.Add(expression);
								}
								FOREACH(Ptr<ParsingError>, originalError, errors)
								{
									auto error = WfErrors::WrongFormatStringSyntax(node);
									error->errorMessage += L" (" + originalError->errorMessage + L")";
									manager->errors.Add(error);
								}
								reading = end;
							}
						}
						if (*reading || expressions.Count() == 0)
						{
							Ptr<WfStringExpression> expression = new WfStringExpression;
							expression->codeRange = node->codeRange;
							expression->value.value = reading;
							expressions.Add(expression);
						}

						if (expressions.Count() > 0)
						{
							Ptr<WfExpression> current = expressions[0];
							FOREACH(Ptr<WfExpression>, expression, From(expressions).Skip(1))
							{
								Ptr<WfBinaryExpression> binary = new WfBinaryExpression;
								binary->codeRange = node->codeRange;
								binary->first = current;
								binary->second = expression;
								binary->op = WfBinaryOperator::Union;
								current = binary;
							}

							node->expandedExpression = current;
						}
					}

					if (node->expandedExpression)
					{
						ValidateExpressionStructure(manager, context, node->expandedExpression);
					}
				}

				void Visit(WfUnaryExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->operand);
				}

				void Visit(WfBinaryExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->first);
					ValidateExpressionStructure(manager, context, node->second);
				}

				void Visit(WfLetExpression* node)override
				{
					FOREACH(Ptr<WfLetVariable>, variable, node->variables)
					{
						ValidateExpressionStructure(manager, context, variable->value);
					}
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfIfExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->condition);
					ValidateExpressionStructure(manager, context, node->trueBranch);
					ValidateExpressionStructure(manager, context, node->falseBranch);
				}

				void Visit(WfRangeExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->begin);
					ValidateExpressionStructure(manager, context, node->end);
				}

				void Visit(WfSetTestingExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->element);
					ValidateExpressionStructure(manager, context, node->collection);
				}

				void Visit(WfConstructorExpression* node)override
				{
					vint listElementCount = 0;
					vint mapElementCount = 0;
					FOREACH(Ptr<WfConstructorArgument>, argument, node->arguments)
					{
						ValidateExpressionStructure(manager, context, argument->key);
						if (argument->value)
						{
							ValidateExpressionStructure(manager, context, argument->value);
							mapElementCount++;
						}
						else
						{
							listElementCount++;
						}
					}

					if (listElementCount*mapElementCount != 0)
					{
						manager->errors.Add(WfErrors::ConstructorMixMapAndList(node));
					}
				}

				void Visit(WfInferExpression* node)override
				{
					ValidateTypeStructure(manager, node->type);
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfTypeCastingExpression* node)override
				{
					ValidateTypeStructure(manager, node->type);
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfTypeTestingExpression* node)override
				{
					if (node->type)
					{
						ValidateTypeStructure(manager, node->type);
					}
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfTypeOfTypeExpression* node)override
				{
					ValidateTypeStructure(manager, node->type);
				}

				void Visit(WfTypeOfExpressionExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfAttachEventExpression* node)override
				{
					if (context->currentBindExpression)
					{
						manager->errors.Add(WfErrors::AttachInBind(node));
					}
					ValidateExpressionStructure(manager, context, node->event);
					ValidateExpressionStructure(manager, context, node->function);
				}

				void Visit(WfDetachEventExpression* node)override
				{
					if (context->currentBindExpression)
					{
						manager->errors.Add(WfErrors::DetachInBind(node));
					}
					ValidateExpressionStructure(manager, context, node->event);
					ValidateExpressionStructure(manager, context, node->handler);
				}

				void Visit(WfBindExpression* node)override
				{
					if (context->currentBindExpression)
					{
						manager->errors.Add(WfErrors::BindInBind(node));
					}

					auto bind = context->currentBindExpression;
					context->currentBindExpression = node;
					ValidateExpressionStructure(manager, context, node->expression);
					context->currentBindExpression = bind;
				}

				void Visit(WfObserveExpression* node)override
				{
					if (!context->currentBindExpression)
					{
						manager->errors.Add(WfErrors::ObserveNotInBind(node));
					}
					if (context->currentObserveExpression)
					{
						manager->errors.Add(WfErrors::ObserveInObserveEvent(node));
					}

					if (node->observeType == WfObserveType::SimpleObserve)
					{
						if (!node->expression.Cast<WfReferenceExpression>())
						{
							manager->errors.Add(WfErrors::WrongSimpleObserveExpression(node->expression.Obj()));
						}
						FOREACH(Ptr<WfExpression>, event, node->events)
						{
							if (!event.Cast<WfReferenceExpression>())
							{
								manager->errors.Add(WfErrors::WrongSimpleObserveEvent(event.Obj()));
							}
						}
					}
					if (node->events.Count() == 0)
					{
						manager->errors.Add(WfErrors::EmptyObserveEvent(node));
					}

					ValidateExpressionStructure(manager, context, node->parent);
					ValidateExpressionStructure(manager, context, node->expression);
					context->currentObserveExpression = node;
					for (vint i = 0; i < node->events.Count(); i++)
					{
						ValidateExpressionStructure(manager, context, node->events[i]);
					}
					context->currentObserveExpression = 0;
				}

				void Visit(WfCallExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->function);
					for (vint i = 0; i < node->arguments.Count(); i++)
					{
						ValidateExpressionStructure(manager, context, node->arguments[i]);
					}
				}

				void Visit(WfFunctionExpression* node)override
				{
					ValidateDeclarationStructure(manager, node->function, nullptr, node);
				}

				void Visit(WfNewClassExpression* node)override
				{
					ValidateTypeStructure(manager, node->type);
					for (vint i = 0; i < node->arguments.Count(); i++)
					{
						ValidateExpressionStructure(manager, context, node->arguments[i]);
					}
				}

				void Visit(WfNewInterfaceExpression* node)override
				{
					ValidateTypeStructure(manager, node->type);
					FOREACH(Ptr<WfClassMember>, member, node->members)
					{
						if (member->declaration.Cast<WfFunctionDeclaration>())
						{
							switch (member->kind)
							{
							case WfClassMemberKind::Normal:
							case WfClassMemberKind::Override:
								break;
							case WfClassMemberKind::Static:
								manager->errors.Add(WfErrors::FunctionInNewTypeExpressionCannotBeStatic(member.Obj()));
								break;
							}
						}
						else
						{
							switch (member->kind)
							{
							case WfClassMemberKind::Normal:
								break;
							case WfClassMemberKind::Static:
							case WfClassMemberKind::Override:
								manager->errors.Add(WfErrors::NonFunctionClassMemberCannotBeStaticOrOverride(member.Obj()));
								break;
							}
						}
						ValidateDeclarationStructure(manager, member->declaration, nullptr, node);
					}
				}

				static void Execute(Ptr<WfExpression>& expression, WfLexicalScopeManager* manager, ValidateStructureContext* context)
				{
					ValidateStructureExpressionVisitor visitor(manager, context);
					expression->Accept(&visitor);
					if (visitor.result)
					{
						expression = visitor.result;
					}
				}
			};

/***********************************************************************
ValidateStructure
***********************************************************************/

			void ValidateTypeStructure(WfLexicalScopeManager* manager, Ptr<WfType> type, ValidateTypeStragety strategy, WfClassDeclaration* classDecl)
			{
				ValidateStructureTypeVisitor::Execute(type, manager, strategy, classDecl);
			}

			void ValidateModuleStructure(WfLexicalScopeManager* manager, Ptr<WfModule> module)
			{
				FOREACH(Ptr<WfModuleUsingPath>, path, module->paths)
				{
					FOREACH_INDEXER(Ptr<WfModuleUsingItem>, item, index, path->items)
					{
						vint counter = 0;
						FOREACH(Ptr<WfModuleUsingFragment>, fragment, item->fragments)
						{
							if (fragment.Cast<WfModuleUsingWildCardFragment>())
							{
								if (index == path->items.Count() - 1)
								{
									counter++;
								}
								else
								{
									goto USING_PATH_INCORRECT;
								}
							}
						}

						if (index == path->items.Count() - 1 && counter != 1)
						{
							goto USING_PATH_INCORRECT;
						}
					}
					continue;
				USING_PATH_INCORRECT:
					manager->errors.Add(WfErrors::WrongUsingPathWildCard(path.Obj()));
				}
				for (vint i = 0; i < module->declarations.Count(); i++)
				{
					ValidateDeclarationStructure(manager, module->declarations[i]);
				}
			}

			void ValidateDeclarationStructure(WfLexicalScopeManager* manager, Ptr<WfDeclaration> declaration, WfClassDeclaration* classDecl, parsing::ParsingTreeCustomBase* source)
			{
				ValidateStructureDeclarationVisitor::Execute(declaration, manager, classDecl, source);
			}

			void ValidateStatementStructure(WfLexicalScopeManager* manager, ValidateStructureContext* context, Ptr<WfStatement>& statement)
			{
				ValidateStructureStatementVisitor::Execute(statement, manager, context);
			}

			void ValidateExpressionStructure(WfLexicalScopeManager* manager, ValidateStructureContext* context, Ptr<WfExpression>& expression)
			{
				ValidateStructureExpressionVisitor::Execute(expression, manager, context);
			}
		}
	}
}
