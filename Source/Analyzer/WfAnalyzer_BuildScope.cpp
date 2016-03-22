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
			using namespace typeimpl;

/***********************************************************************
BuildScopeForDeclaration
***********************************************************************/

			class BuildScopeForDeclarationVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				ParsingTreeCustomBase*					source;
				Ptr<WfClassMember>						member;
				Ptr<WfLexicalScope>						parentScope;

				Ptr<WfLexicalScope>						resultScope;

				BuildScopeForDeclarationVisitor(WfLexicalScopeManager* _manager, Ptr<WfLexicalScope> _parentScope, ParsingTreeCustomBase* _source, Ptr<WfClassMember> _member)
					:manager(_manager)
					, source(_source)
					, member(_member)
					, parentScope(_parentScope)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					Ptr<WfLexicalSymbol> symbol = new WfLexicalSymbol(parentScope.Obj());
					symbol->name = node->name.value;
					symbol->creatorNode = node;
					parentScope->symbols.Add(symbol->name, symbol);

					resultScope = new WfLexicalScope(parentScope);
					FOREACH(Ptr<WfDeclaration>, declaration, node->declarations)
					{
						BuildScopeForDeclaration(manager, resultScope, declaration, node, nullptr);
					}
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					resultScope = new WfLexicalScope(parentScope);
					auto config = MakePtr<WfLexicalFunctionConfig>();
					resultScope->functionConfig = config;

					if (source)
					{
						if (dynamic_cast<WfFunctionExpression*>(source))
						{
							config->lambda = true;
							config->thisAccessable = false;
							config->parentThisAccessable = true;
						}
						else if (dynamic_cast<WfNewInterfaceExpression*>(source))
						{
							config->lambda = true;
							config->thisAccessable = true;
							config->parentThisAccessable = true;
						}
						else if (dynamic_cast<WfClassDeclaration*>(source))
						{
							config->lambda = false;
							config->thisAccessable = member->kind != WfClassMemberKind::Static;
							config->parentThisAccessable = false;
						}
					}

					if (node->anonymity == WfFunctionAnonymity::Named)
					{
						Ptr<WfLexicalScope> functionNameScope = parentScope;
						if (source && dynamic_cast<WfFunctionExpression*>(source))
						{
							functionNameScope = resultScope;
						}

						Ptr<WfLexicalSymbol> symbol = new WfLexicalSymbol(functionNameScope.Obj());
						symbol->name = node->name.value;
						symbol->creatorNode = node;
						symbol->creatorClassMember = member;
						{
							Ptr<WfFunctionType> type = new WfFunctionType;
							type->result = node->returnType;
							FOREACH(Ptr<WfFunctionArgument>, argument, node->arguments)
							{
								type->arguments.Add(argument->type);
							}
							symbol->type = type;
						}
						functionNameScope->symbols.Add(symbol->name, symbol);
					}

					if (node->statement)
					{
						FOREACH(Ptr<WfFunctionArgument>, argument, node->arguments)
						{
							Ptr<WfLexicalSymbol> argumentSymbol = new WfLexicalSymbol(resultScope.Obj());
							argumentSymbol->name = argument->name.value;
							argumentSymbol->type = argument->type;
							argumentSymbol->creatorNode = argument;
							resultScope->symbols.Add(argumentSymbol->name, argumentSymbol);
						}

						BuildScopeForStatement(manager, resultScope, node->statement);
					}
				}

				void Visit(WfVariableDeclaration* node)override
				{
					Ptr<WfLexicalSymbol> symbol = new WfLexicalSymbol(parentScope.Obj());
					symbol->name = node->name.value;
					symbol->creatorNode = node;
					symbol->creatorClassMember = member;
					symbol->type = node->type;
					parentScope->symbols.Add(symbol->name, symbol);

					BuildScopeForExpression(manager, parentScope, node->expression);
				}

				void Visit(WfEventDeclaration* node)override
				{
					Ptr<WfLexicalSymbol> symbol = new WfLexicalSymbol(parentScope.Obj());
					symbol->name = node->name.value;
					symbol->creatorNode = node;
					symbol->creatorClassMember = member;
					parentScope->symbols.Add(symbol->name, symbol);
				}

				void Visit(WfPropertyDeclaration* node)override
				{
					Ptr<WfLexicalSymbol> symbol = new WfLexicalSymbol(parentScope.Obj());
					symbol->name = node->name.value;
					symbol->creatorNode = node;
					symbol->creatorClassMember = member;
					symbol->type = node->type;
					parentScope->symbols.Add(symbol->name, symbol);
				}

				void Visit(WfConstructorDeclaration* node)override
				{
					resultScope = new WfLexicalScope(parentScope);

					FOREACH(Ptr<WfFunctionArgument>, argument, node->arguments)
					{
						Ptr<WfLexicalSymbol> argumentSymbol = new WfLexicalSymbol(resultScope.Obj());
						argumentSymbol->name = argument->name.value;
						argumentSymbol->type = argument->type;
						argumentSymbol->creatorNode = argument;
						resultScope->symbols.Add(argumentSymbol->name, argumentSymbol);
					}

					FOREACH(Ptr<WfBaseConstructorCall>, call, node->baseConstructorCalls)
					{
						FOREACH(Ptr<WfExpression>, argument, call->arguments)
						{
							BuildScopeForExpression(manager, resultScope, argument);
						}
					}

					auto bodyScope = MakePtr<WfLexicalScope>(resultScope);
					{
						auto config = MakePtr<WfLexicalFunctionConfig>();
						resultScope->functionConfig = config;

						config->lambda = false;
						config->thisAccessable = true;
						config->parentThisAccessable = false;
					}
					BuildScopeForStatement(manager, bodyScope, node->statement);
				}

				void Visit(WfClassDeclaration* node)override
				{
					Ptr<WfLexicalSymbol> symbol = new WfLexicalSymbol(parentScope.Obj());
					symbol->name = node->name.value;
					symbol->creatorNode = node;
					symbol->creatorClassMember = member;
					parentScope->symbols.Add(symbol->name, symbol);
					
					auto td = manager->declarationTypes[node];
					resultScope = new WfLexicalScope(parentScope);
					resultScope->typeOfThisExpr = td.Obj();
					FOREACH(Ptr<WfClassMember>, member, node->members)
					{
						BuildScopeForDeclaration(manager, resultScope, member->declaration, node, member);
					}
				}

				static Ptr<WfLexicalScope> Execute(WfLexicalScopeManager* manager, Ptr<WfLexicalScope> parentScope, ParsingTreeCustomBase* source, Ptr<WfClassMember> member, Ptr<WfDeclaration> declaration)
				{
					BuildScopeForDeclarationVisitor visitor(manager, parentScope, source, member);
					declaration->Accept(&visitor);
					if (visitor.resultScope)
					{
						manager->nodeScopes.Add(declaration.Obj(), visitor.resultScope);
						visitor.resultScope->ownerNode = declaration;
						visitor.resultScope->ownerClassMember = member;
					}
					else
					{
						manager->nodeScopes.Add(declaration.Obj(), parentScope);
					}
					return visitor.resultScope;
				}
			};

/***********************************************************************
BuildScopeForStatement
***********************************************************************/

			class BuildScopeForStatementVisitor : public Object, public WfStatement::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				Ptr<WfLexicalScope>						parentScope;
				Ptr<WfLexicalScope>						resultScope;

				BuildScopeForStatementVisitor(WfLexicalScopeManager* _manager, Ptr<WfLexicalScope> _parentScope)
					:manager(_manager)
					, parentScope(_parentScope)
				{
				}

				void Visit(WfBreakStatement* node)override
				{
				}

				void Visit(WfContinueStatement* node)override
				{
				}

				void Visit(WfReturnStatement* node)override
				{
					if (node->expression)
					{
						BuildScopeForExpression(manager, parentScope, node->expression);
					}
				}

				void Visit(WfDeleteStatement* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->expression);
				}

				void Visit(WfRaiseExceptionStatement* node)override
				{
					if (node->expression)
					{
						BuildScopeForExpression(manager, parentScope, node->expression);
					}
				}

				void Visit(WfIfStatement* node)override
				{
					resultScope = new WfLexicalScope(parentScope);
					if (node->type)
					{
						Ptr<WfLexicalSymbol> symbol = new WfLexicalSymbol(resultScope.Obj());
						symbol->name = node->name.value;
						symbol->type = node->type;
						symbol->creatorNode = node;
						resultScope->symbols.Add(symbol->name, symbol);
					}

					BuildScopeForExpression(manager, resultScope, node->expression);
					BuildScopeForStatement(manager, resultScope, node->trueBranch);
					if (node->falseBranch)
					{
						BuildScopeForStatement(manager, resultScope, node->falseBranch);
					}
				}

				void Visit(WfSwitchStatement* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->expression);
					FOREACH(Ptr<WfSwitchCase>, switchCase, node->caseBranches)
					{
						BuildScopeForExpression(manager, parentScope, switchCase->expression);
						BuildScopeForStatement(manager, parentScope, switchCase->statement);
					}
					if (node->defaultBranch)
					{
						BuildScopeForStatement(manager, parentScope, node->defaultBranch);
					}
				}

				void Visit(WfWhileStatement* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->condition);
					BuildScopeForStatement(manager, parentScope, node->statement);
				}

				void Visit(WfForEachStatement* node)override
				{
					resultScope = new WfLexicalScope(parentScope);

					Ptr<WfLexicalSymbol> symbol = new WfLexicalSymbol(resultScope.Obj());
					symbol->name = node->name.value;
					symbol->creatorNode = node;
					resultScope->symbols.Add(symbol->name, symbol);

					BuildScopeForExpression(manager, parentScope, node->collection);
					BuildScopeForStatement(manager, resultScope, node->statement);
				}

				void Visit(WfTryStatement* node)override
				{
					BuildScopeForStatement(manager, parentScope, node->protectedStatement);
					if (node->catchStatement)
					{
						resultScope = new WfLexicalScope(parentScope);

						Ptr<WfLexicalSymbol> symbol = new WfLexicalSymbol(resultScope.Obj());
						symbol->name = node->name.value;
						symbol->creatorNode = node;
						{
							auto system = MakePtr<WfTopQualifiedType>();
							system->name.value = L"system";

							auto exception = MakePtr<WfChildType>();
							exception->parent = system;
							exception->name.value = L"Exception";

							auto sharedPtr = MakePtr<WfSharedPointerType>();
							sharedPtr->element = exception;

							symbol->type = sharedPtr;
						}
						resultScope->symbols.Add(symbol->name, symbol);

						BuildScopeForStatement(manager, resultScope, node->catchStatement);
					}
					if (node->finallyStatement)
					{
						BuildScopeForStatement(manager, parentScope, node->finallyStatement);
					}
				}

				void Visit(WfBlockStatement* node)override
				{
					resultScope = new WfLexicalScope(parentScope);

					FOREACH(Ptr<WfStatement>, statement, node->statements)
					{
						BuildScopeForStatement(manager, resultScope, statement);
					}
				}

				void Visit(WfExpressionStatement* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->expression);
				}

				void Visit(WfVariableStatement* node)override
				{
					BuildScopeForDeclaration(manager, parentScope, node->variable, node, nullptr);
				}

				static Ptr<WfLexicalScope> Execute(WfLexicalScopeManager* manager, Ptr<WfLexicalScope> parentScope, Ptr<WfStatement> statement)
				{
					BuildScopeForStatementVisitor visitor(manager, parentScope);
					statement->Accept(&visitor);
					if (visitor.resultScope)
					{
						manager->nodeScopes.Add(statement.Obj(), visitor.resultScope);
						visitor.resultScope->ownerNode = statement;
					}
					else
					{
						manager->nodeScopes.Add(statement.Obj(), parentScope);
					}
					return visitor.resultScope;
				}
			};

/***********************************************************************
BuildScopeForExpression
***********************************************************************/

			class BuildScopeForExpressionVisitor : public Object, public WfExpression::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				Ptr<WfLexicalScope>						parentScope;
				Ptr<WfLexicalScope>						resultScope;

				BuildScopeForExpressionVisitor(WfLexicalScopeManager* _manager, Ptr<WfLexicalScope> _parentScope)
					:manager(_manager)
					, parentScope(_parentScope)
				{
				}

				void Visit(WfThisExpression* node)override
				{
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
					manager->CreateLambdaCapture(node);

					SortedList<vint> names;
					SearchOrderedName(parentScope.Obj(), node->body, names);

					resultScope = new WfLexicalScope(parentScope);
					auto config = MakePtr<WfLexicalFunctionConfig>();
					resultScope->functionConfig = config;

					config->lambda = true;
					config->thisAccessable = false;
					config->parentThisAccessable = true;

					FOREACH(vint, name, names)
					{
						Ptr<WfLexicalSymbol> symbol = new WfLexicalSymbol(resultScope.Obj());
						symbol->name = L"$" + itow(name);
						symbol->creatorNode = node;
						resultScope->symbols.Add(symbol->name, symbol);
					}

					BuildScopeForExpression(manager, resultScope, node->body);
				}

				void Visit(WfMemberExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->parent);
				}

				void Visit(WfChildExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->parent);
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
					if (node->expandedExpression)
					{
						BuildScopeForExpression(manager, parentScope, node->expandedExpression);
					}
				}

				void Visit(WfUnaryExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->operand);
				}

				void Visit(WfBinaryExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->first);
					BuildScopeForExpression(manager, parentScope, node->second);
				}

				void Visit(WfLetExpression* node)override
				{
					resultScope = new WfLexicalScope(parentScope);
					FOREACH(Ptr<WfLetVariable>, variable, node->variables)
					{
						Ptr<WfLexicalSymbol> symbol = new WfLexicalSymbol(resultScope.Obj());
						symbol->name = variable->name.value;
						symbol->creatorNode = node;
						resultScope->symbols.Add(symbol->name, symbol);
						BuildScopeForExpression(manager, resultScope, variable->value);
					}

					BuildScopeForExpression(manager, resultScope, node->expression);
				}

				void Visit(WfIfExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->condition);
					BuildScopeForExpression(manager, parentScope, node->trueBranch);
					BuildScopeForExpression(manager, parentScope, node->falseBranch);
				}

				void Visit(WfRangeExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->begin);
					BuildScopeForExpression(manager, parentScope, node->end);
				}

				void Visit(WfSetTestingExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->element);
					BuildScopeForExpression(manager, parentScope, node->collection);
				}

				void Visit(WfConstructorExpression* node)override
				{
					FOREACH(Ptr<WfConstructorArgument>, argument, node->arguments)
					{
						BuildScopeForExpression(manager, parentScope, argument->key);
						if (argument->value)
						{
							BuildScopeForExpression(manager, parentScope, argument->value);
						}
					}
				}

				void Visit(WfInferExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->expression);
				}

				void Visit(WfTypeCastingExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->expression);
				}

				void Visit(WfTypeTestingExpression* node)override
				{
					if (node->expression)
					{
						BuildScopeForExpression(manager, parentScope, node->expression);
					}
				}

				void Visit(WfTypeOfTypeExpression* node)override
				{
				}

				void Visit(WfTypeOfExpressionExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->expression);
				}

				void Visit(WfAttachEventExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->event);
					BuildScopeForExpression(manager, parentScope, node->function);
				}

				void Visit(WfDetachEventExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->handler);
				}

				void Visit(WfBindExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->expression);
				}

				void Visit(WfObserveExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->parent);
					if (node->observeType == WfObserveType::SimpleObserve)
					{
						BuildScopeForExpression(manager, parentScope, node->expression);
						FOREACH(Ptr<WfExpression>, event, node->events)
						{
							BuildScopeForExpression(manager, parentScope, event);
						}
					}
					else
					{
						resultScope = new WfLexicalScope(parentScope);
						{
							Ptr<WfLexicalSymbol> symbol = new WfLexicalSymbol(resultScope.Obj());
							symbol->name = node->name.value;
							symbol->creatorNode = node;
							resultScope->symbols.Add(symbol->name, symbol);
						}

						BuildScopeForExpression(manager, resultScope, node->expression);
						FOREACH(Ptr<WfExpression>, event, node->events)
						{
							BuildScopeForExpression(manager, resultScope, event);
						}
					}
				}

				void Visit(WfCallExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->function);
					FOREACH(Ptr<WfExpression>, argument, node->arguments)
					{
						BuildScopeForExpression(manager, parentScope, argument);
					}
				}

				void Visit(WfFunctionExpression* node)override
				{
					manager->CreateLambdaCapture(node->function.Obj());
					BuildScopeForDeclaration(manager, parentScope, node->function, node, nullptr);
				}

				void Visit(WfNewClassExpression* node)override
				{
					FOREACH(Ptr<WfExpression>, argument, node->arguments)
					{
						BuildScopeForExpression(manager, parentScope, argument);
					}
				}

				void Visit(WfNewInterfaceExpression* node)override
				{
					resultScope = new WfLexicalScope(parentScope);
					auto capture = MakePtr<WfLexicalCapture>();
					FOREACH(Ptr<WfClassMember>, member, node->members)
					{
						if (member->declaration.Cast<WfFunctionDeclaration>())
						{
							manager->CreateLambdaCapture(member->declaration.Obj(), capture);
						}
						BuildScopeForDeclaration(manager, resultScope, member->declaration, node, member);
					}
				}

				static Ptr<WfLexicalScope> Execute(WfLexicalScopeManager* manager, Ptr<WfLexicalScope> parentScope, Ptr<WfExpression> expression)
				{
					BuildScopeForExpressionVisitor visitor(manager, parentScope);
					expression->Accept(&visitor);
					if (visitor.resultScope)
					{
						manager->nodeScopes.Add(expression.Obj(), visitor.resultScope);
						visitor.resultScope->ownerNode = expression;
					}
					else
					{
						manager->nodeScopes.Add(expression.Obj(), parentScope);
					}
					return visitor.resultScope;
				}
			};

/***********************************************************************
BuildScope
***********************************************************************/

			void BuildScopeForModule(WfLexicalScopeManager* manager, Ptr<WfModule> module)
			{
				Ptr<WfLexicalScope> scope = new WfLexicalScope(manager);
				scope->ownerNode = module;
				manager->nodeScopes.Add(module.Obj(), scope);

				FOREACH(Ptr<WfDeclaration>, declaration, module->declarations)
				{
					BuildScopeForDeclaration(manager, scope, declaration, module.Obj(), nullptr);
				}
			}

			void BuildScopeForDeclaration(WfLexicalScopeManager* manager, Ptr<WfLexicalScope> parentScope, Ptr<WfDeclaration> declaration, parsing::ParsingTreeCustomBase* source, Ptr<WfClassMember> member)
			{
				BuildScopeForDeclarationVisitor::Execute(manager, parentScope, source, member, declaration);
			}

			void BuildScopeForStatement(WfLexicalScopeManager* manager, Ptr<WfLexicalScope> parentScope, Ptr<WfStatement> statement)
			{
				BuildScopeForStatementVisitor::Execute(manager, parentScope, statement);
			}

			void BuildScopeForExpression(WfLexicalScopeManager* manager, Ptr<WfLexicalScope> parentScope, Ptr<WfExpression> expression)
			{
				BuildScopeForExpressionVisitor::Execute(manager, parentScope, expression);
			}

/***********************************************************************
CheckScopes
***********************************************************************/

			bool CheckScopes(WfLexicalScopeManager* manager)
			{
				vint errorCount = manager->errors.Count();
				FOREACH(Ptr<WfLexicalScope>, scope, manager->nodeScopes.Values())
				{
					if (!manager->analyzedScopes.Contains(scope.Obj()))
					{
						manager->analyzedScopes.Add(scope);

						for (vint i = 0; i < scope->symbols.Count(); i++)
						{
							const auto& symbols = scope->symbols.GetByIndex(i);
							if (symbols.Count() > 1)
							{
								if (!scope->ownerNode.Cast<WfModule>() && !scope->ownerNode.Cast<WfNamespaceDeclaration>())
								{
									if (symbols.Count() > 1)
									{
										FOREACH(Ptr<WfLexicalSymbol>, symbol, From(symbols))
										{
											if (auto decl = symbol->creatorNode.Cast<WfDeclaration>())
											{
												if (!decl.Cast<WfFunctionDeclaration>())
												{
													manager->errors.Add(WfErrors::DuplicatedSymbol(decl.Obj(), symbol));
												}
											}
											else if (auto arg = symbol->creatorNode.Cast<WfFunctionArgument>())
											{
												manager->errors.Add(WfErrors::DuplicatedSymbol(arg.Obj(), symbol));
											}
											else if (auto stat = symbol->creatorNode.Cast<WfStatement>())
											{
												manager->errors.Add(WfErrors::DuplicatedSymbol(stat.Obj(), symbol));
											}
											else if (auto expr = symbol->creatorNode.Cast<WfExpression>())
											{
												manager->errors.Add(WfErrors::DuplicatedSymbol(expr.Obj(), symbol));
											}
										}
									}
								}
							}
						}

						for (vint i = 0; i < scope->symbols.Count(); i++)
						{
							FOREACH(Ptr<WfLexicalSymbol>, symbol, scope->symbols.GetByIndex(i))
							{
								if (symbol->type)
								{
									symbol->typeInfo = CreateTypeInfoFromType(scope.Obj(), symbol->type);
								}
							}
						}
					}
				}
				return manager->errors.Count() == errorCount;
			}
		}
	}
}