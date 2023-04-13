#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace reflection::description;

/***********************************************************************
BuildScopeForDeclaration
***********************************************************************/

			class BuildScopeForDeclarationVisitor
				: public Object
				, public WfDeclaration::IVisitor
				, public WfVirtualCseDeclaration::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				glr::ParsingAstBase*					source;
				Ptr<WfLexicalScope>						parentScope;

				Ptr<WfLexicalScope>						resultScope;

				BuildScopeForDeclarationVisitor(WfLexicalScopeManager* _manager, Ptr<WfLexicalScope> _parentScope, glr::ParsingAstBase* _source)
					:manager(_manager)
					, source(_source)
					, parentScope(_parentScope)
				{
				}

				void Visit(Ptr<WfLexicalScope> scope, List<Ptr<WfAttribute>>& attributes)
				{
					for (auto attribute : attributes)
					{
						if (attribute->value)
						{
							BuildScopeForExpression(manager, scope, attribute->value);
						}
					}
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					auto symbol = Ptr(new WfLexicalSymbol(parentScope.Obj()));
					symbol->name = node->name.value;
					symbol->creatorNode = Ptr(node);
					parentScope->symbols.Add(symbol->name, symbol);

					resultScope = Ptr(new WfLexicalScope(parentScope));
					for (auto declaration : node->declarations)
					{
						BuildScopeForDeclaration(manager, resultScope, declaration, node);
					}
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					resultScope = Ptr(new WfLexicalScope(parentScope));
					auto config = Ptr(new WfLexicalFunctionConfig);
					resultScope->functionConfig = config;
					resultScope->ownerNodeSource = source;

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
							config->thisAccessable = node->functionKind != WfFunctionKind::Static;
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

						auto symbol = Ptr(new WfLexicalSymbol(functionNameScope.Obj()));
						symbol->name = node->name.value;
						symbol->creatorNode = Ptr(node);
						{
							auto type = Ptr(new WfFunctionType);
							type->result = node->returnType;
							for (auto argument : node->arguments)
							{
								type->arguments.Add(argument->type);
							}
							symbol->type = type;
						}
						functionNameScope->symbols.Add(symbol->name, symbol);
					}

					if (node->statement)
					{
						for (auto argument : node->arguments)
						{
							auto argumentSymbol = Ptr(new WfLexicalSymbol(resultScope.Obj()));
							argumentSymbol->name = argument->name.value;
							argumentSymbol->type = argument->type;
							argumentSymbol->creatorNode = argument;
							resultScope->symbols.Add(argumentSymbol->name, argumentSymbol);

							Visit(parentScope, argument->attributes);
						}

						BuildScopeForStatement(manager, resultScope, node->statement);
					}
				}

				void Visit(WfVariableDeclaration* node)override
				{
					auto symbol = Ptr(new WfLexicalSymbol(parentScope.Obj()));
					symbol->name = node->name.value;
					symbol->creatorNode = Ptr(node);
					symbol->type = node->type;
					parentScope->symbols.Add(symbol->name, symbol);

					BuildScopeForExpression(manager, parentScope, node->expression);
				}

				void Visit(WfEventDeclaration* node)override
				{
					auto symbol = Ptr(new WfLexicalSymbol(parentScope.Obj()));
					symbol->name = node->name.value;
					symbol->creatorNode = Ptr(node);
					parentScope->symbols.Add(symbol->name, symbol);
				}

				void Visit(WfPropertyDeclaration* node)override
				{
					auto symbol = Ptr(new WfLexicalSymbol(parentScope.Obj()));
					symbol->name = node->name.value;
					symbol->creatorNode = Ptr(node);
					symbol->type = node->type;
					parentScope->symbols.Add(symbol->name, symbol);
				}

				void Visit(WfStaticInitDeclaration* node)override
				{
					resultScope = Ptr(new WfLexicalScope(parentScope));
					auto config = Ptr(new WfLexicalFunctionConfig);
					resultScope->functionConfig = config;
					BuildScopeForStatement(manager, resultScope, node->statement);
				}

				void Visit(WfConstructorDeclaration* node)override
				{
					resultScope = Ptr(new WfLexicalScope(parentScope));

					for (auto argument : node->arguments)
					{
						auto argumentSymbol = Ptr(new WfLexicalSymbol(resultScope.Obj()));
						argumentSymbol->name = argument->name.value;
						argumentSymbol->type = argument->type;
						argumentSymbol->creatorNode = argument;
						resultScope->symbols.Add(argumentSymbol->name, argumentSymbol);
					}

					for (auto call : node->baseConstructorCalls)
					{
						for (auto argument : call->arguments)
						{
							BuildScopeForExpression(manager, resultScope, argument);
						}
					}

					auto bodyScope = Ptr(new WfLexicalScope(resultScope));
					{
						auto config = Ptr(new WfLexicalFunctionConfig);
						bodyScope->functionConfig = config;

						config->lambda = false;
						config->thisAccessable = true;
						config->parentThisAccessable = false;
					}
					BuildScopeForStatement(manager, bodyScope, node->statement);
				}

				void Visit(WfDestructorDeclaration* node)override
				{
					resultScope = Ptr(new WfLexicalScope(parentScope));
					{
						auto config = Ptr(new WfLexicalFunctionConfig);
						resultScope->functionConfig = config;

						config->lambda = false;
						config->thisAccessable = true;
						config->parentThisAccessable = false;
					}
					BuildScopeForStatement(manager, resultScope, node->statement);
				}

				void Visit(WfClassDeclaration* node)override
				{
					auto symbol = Ptr(new WfLexicalSymbol(parentScope.Obj()));
					symbol->name = node->name.value;
					symbol->creatorNode = Ptr(node);
					parentScope->symbols.Add(symbol->name, symbol);
					
					auto td = manager->declarationTypes[node];
					resultScope = Ptr(new WfLexicalScope(parentScope));
					resultScope->typeOfThisExpr = td.Obj();
					for (auto memberDecl : node->declarations)
					{
						BuildScopeForDeclaration(manager, resultScope, memberDecl, node);
					}
				}

				void Visit(WfEnumDeclaration* node)override
				{
					auto symbol = Ptr(new WfLexicalSymbol(parentScope.Obj()));
					symbol->name = node->name.value;
					symbol->creatorNode = Ptr(node);
					parentScope->symbols.Add(symbol->name, symbol);

					for (auto item : node->items)
					{
						Visit(parentScope, item->attributes);
					}
				}

				void Visit(WfStructDeclaration* node)override
				{
					auto symbol = Ptr(new WfLexicalSymbol(parentScope.Obj()));
					symbol->name = node->name.value;
					symbol->creatorNode = Ptr(node);
					parentScope->symbols.Add(symbol->name, symbol);

					for (auto member : node->members)
					{
						Visit(parentScope, member->attributes);
					}
				}

				void Visit(WfVirtualCfeDeclaration* node)override
				{
					for (auto decl : node->expandedDeclarations)
					{
						Execute(manager, parentScope, source, decl);
					}
				}

				void Visit(WfVirtualCseDeclaration* node)override
				{
					node->Accept((WfVirtualCseDeclaration::IVisitor*)this);
				}

				void Visit(WfStateMachineDeclaration* node)override
				{
					for (auto input : node->inputs)
					{
						auto stateSymbol = Ptr(new WfLexicalSymbol(parentScope.Obj()));
						stateSymbol->name = input->name.value;
						stateSymbol->creatorNode = input;
						parentScope->symbols.Add(stateSymbol->name, stateSymbol);
					}

					for (auto state : node->states)
					{
						auto stateSymbol = Ptr(new WfLexicalSymbol(parentScope.Obj()));
						stateSymbol->name = state->name.value;
						stateSymbol->creatorNode = state;
						parentScope->symbols.Add(stateSymbol->name, stateSymbol);

						auto stateScope = Ptr(new WfLexicalScope(parentScope));
						{
							auto config = Ptr(new WfLexicalFunctionConfig);
							stateScope->functionConfig = config;

							config->lambda = false;
							config->thisAccessable = true;
							config->parentThisAccessable = true;
						}
						stateScope->ownerNode = state;
						manager->nodeScopes.Add(state.Obj(), stateScope);

						for (auto argument : state->arguments)
						{
							auto argumentSymbol = Ptr(new WfLexicalSymbol(stateScope.Obj()));
							argumentSymbol->name = argument->name.value;
							argumentSymbol->type = argument->type;
							argumentSymbol->creatorNode = argument;
							stateScope->symbols.Add(argumentSymbol->name, argumentSymbol);
						}

						BuildScopeForStatement(manager, stateScope, state->statement);
					}
				}

				static Ptr<WfLexicalScope> Execute(WfLexicalScopeManager* manager, Ptr<WfLexicalScope> parentScope, glr::ParsingAstBase* source, Ptr<WfDeclaration> declaration)
				{
					BuildScopeForDeclarationVisitor visitor(manager, parentScope, source);
					declaration->Accept(&visitor);
					visitor.Visit(parentScope, declaration->attributes);

					manager->declaractionScopeSources.Add(declaration, source);
					if (visitor.resultScope)
					{
						manager->nodeScopes.Add(declaration.Obj(), visitor.resultScope);
						visitor.resultScope->ownerNode = declaration;
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

			class BuildScopeForStatementVisitor
				: public Object
				, public WfStatement::IVisitor
				, public WfVirtualCseStatement::IVisitor
				, public WfCoroutineStatement::IVisitor
				, public WfStateMachineStatement::IVisitor
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
					resultScope = Ptr(new WfLexicalScope(parentScope));
					if (node->type)
					{
						auto symbol = Ptr(new WfLexicalSymbol(resultScope.Obj()));
						symbol->name = node->name.value;
						symbol->type = node->type;
						symbol->creatorNode = Ptr(node);
						resultScope->symbols.Add(symbol->name, symbol);
					}

					BuildScopeForExpression(manager, resultScope, node->expression);
					BuildScopeForStatement(manager, resultScope, node->trueBranch);
					if (node->falseBranch)
					{
						BuildScopeForStatement(manager, resultScope, node->falseBranch);
					}
				}

				void Visit(WfWhileStatement* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->condition);
					BuildScopeForStatement(manager, parentScope, node->statement);
				}

				void Visit(WfTryStatement* node)override
				{
					BuildScopeForStatement(manager, parentScope, node->protectedStatement);
					if (node->catchStatement)
					{
						resultScope = Ptr(new WfLexicalScope(parentScope));

						auto symbol = Ptr(new WfLexicalSymbol(resultScope.Obj()));
						symbol->name = node->name.value;
						symbol->creatorNode = Ptr(node);
						{
							auto system = Ptr(new WfTopQualifiedType);
							system->name.value = L"system";

							auto exception = Ptr(new WfChildType);
							exception->parent = system;
							exception->name.value = L"Exception";

							auto sharedPtr = Ptr(new WfSharedPointerType);
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
					resultScope = Ptr(new WfLexicalScope(parentScope));

					for (auto statement : node->statements)
					{
						BuildScopeForStatement(manager, resultScope, statement);
					}
				}

				void Visit(WfGotoStatement* node)override
				{
				}

				void Visit(WfExpressionStatement* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->expression);
				}

				void Visit(WfVariableStatement* node)override
				{
					BuildScopeForDeclaration(manager, parentScope, node->variable, node);
				}

				void Visit(WfVirtualCseStatement* node)override
				{
					node->Accept((WfVirtualCseStatement::IVisitor*)this);
				}

				void Visit(WfSwitchStatement* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->expression);
					for (auto switchCase : node->caseBranches)
					{
						BuildScopeForExpression(manager, parentScope, switchCase->expression);
						BuildScopeForStatement(manager, parentScope, switchCase->statement);
					}
					if (node->defaultBranch)
					{
						BuildScopeForStatement(manager, parentScope, node->defaultBranch);
					}
				}

				void Visit(WfForEachStatement* node)override
				{
					resultScope = Ptr(new WfLexicalScope(parentScope));

					auto symbol = Ptr(new WfLexicalSymbol(resultScope.Obj()));
					symbol->name = node->name.value;
					symbol->creatorNode = Ptr(node);
					resultScope->symbols.Add(symbol->name, symbol);

					BuildScopeForExpression(manager, parentScope, node->collection);
					BuildScopeForStatement(manager, resultScope, node->statement);
				}

				void Visit(WfCoProviderStatement* node)override
				{
					resultScope = Ptr(new WfLexicalScope(parentScope));
					{
						auto symbol = Ptr(new WfLexicalSymbol(resultScope.Obj()));
						symbol->name = L"$PROVIDER";
						symbol->creatorNode = Ptr(node);
						resultScope->symbols.Add(symbol->name, symbol);
					}
					{
						auto symbol = Ptr(new WfLexicalSymbol(resultScope.Obj()));
						symbol->name = L"$IMPL";
						symbol->creatorNode = Ptr(node);
						resultScope->symbols.Add(symbol->name, symbol);
					}
					BuildScopeForStatement(manager, resultScope, node->statement);
				}

				void Visit(WfCoroutineStatement* node)override
				{
					node->Accept((WfCoroutineStatement::IVisitor*)this);
				}

				void Visit(WfCoPauseStatement* node)override
				{
					if (node->statement)
					{
						BuildScopeForStatement(manager, parentScope, node->statement);
					}
				}

				void Visit(WfCoOperatorStatement* node)override
				{
					if (node->varName.value != L"")
					{
						auto symbol = Ptr(new WfLexicalSymbol(parentScope.Obj()));
						symbol->name = node->varName.value;
						symbol->creatorNode = Ptr(node);
						parentScope->symbols.Add(symbol->name, symbol);
					}

					for (auto argument : node->arguments)
					{
						BuildScopeForExpression(manager, parentScope, argument);
					}
				}

				void Visit(WfStateMachineStatement* node)override
				{
					node->Accept((WfStateMachineStatement::IVisitor*)this);
				}

				void Visit(WfStateSwitchStatement* node)override
				{
					resultScope = Ptr(new WfLexicalScope(parentScope));

					for (auto switchCase : node->caseBranches)
					{
						auto caseScope = Ptr(new WfLexicalScope(resultScope));
						caseScope->ownerNode = switchCase;
						manager->nodeScopes.Add(switchCase.Obj(), caseScope);

						for (auto argument : switchCase->arguments)
						{
							auto symbol = Ptr(new WfLexicalSymbol(caseScope.Obj()));
							symbol->name = argument->name.value;
							symbol->creatorNode = argument;
							caseScope->symbols.Add(symbol->name, symbol);
						}

						BuildScopeForStatement(manager, caseScope, switchCase->statement);
					}
				}

				void Visit(WfStateInvokeStatement* node)override
				{
					for (auto argument : node->arguments)
					{
						BuildScopeForExpression(manager, parentScope, argument);
					}
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

			class BuildScopeForExpressionVisitor
				: public Object
				, public WfExpression::IVisitor
				, public WfVirtualCseExpression::IVisitor
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

					resultScope = Ptr(new WfLexicalScope(parentScope));
					auto config = Ptr(new WfLexicalFunctionConfig);
					resultScope->functionConfig = config;

					config->lambda = true;
					config->thisAccessable = false;
					config->parentThisAccessable = true;

					for (auto name : names)
					{
						auto symbol = Ptr(new WfLexicalSymbol(resultScope.Obj()));
						symbol->name = L"$" + itow(name);
						symbol->creatorNode = Ptr(node);
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
					resultScope = Ptr(new WfLexicalScope(parentScope));
					for (auto variable : node->variables)
					{
						auto symbol = Ptr(new WfLexicalSymbol(resultScope.Obj()));
						symbol->name = variable->name.value;
						symbol->creatorNode = Ptr(node);
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
					for (auto argument : node->arguments)
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
					BuildScopeForExpression(manager, parentScope, node->event);
					BuildScopeForExpression(manager, parentScope, node->handler);
				}

				void Visit(WfObserveExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->parent);
					if (node->observeType == WfObserveType::SimpleObserve)
					{
						BuildScopeForExpression(manager, parentScope, node->expression);
						for (auto event : node->events)
						{
							BuildScopeForExpression(manager, parentScope, event);
						}
					}
					else
					{
						resultScope = Ptr(new WfLexicalScope(parentScope));
						{
							auto symbol = Ptr(new WfLexicalSymbol(resultScope.Obj()));
							symbol->name = node->name.value;
							symbol->creatorNode = Ptr(node);
							resultScope->symbols.Add(symbol->name, symbol);
						}

						BuildScopeForExpression(manager, resultScope, node->expression);
						for (auto event : node->events)
						{
							BuildScopeForExpression(manager, resultScope, event);
						}
					}
				}

				void Visit(WfCallExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->function);
					for (auto argument : node->arguments)
					{
						BuildScopeForExpression(manager, parentScope, argument);
					}
				}

				void Visit(WfFunctionExpression* node)override
				{
					manager->CreateLambdaCapture(node->function.Obj());
					BuildScopeForDeclaration(manager, parentScope, node->function, node);
				}

				void Visit(WfNewClassExpression* node)override
				{
					for (auto argument : node->arguments)
					{
						BuildScopeForExpression(manager, parentScope, argument);
					}
				}

				class CreateLambdaCaptureVisitor
					: public empty_visitor::DeclarationVisitor
				{
				public:
					WfLexicalScopeManager*				manager;
					Ptr<WfLexicalCapture>				capture;

					CreateLambdaCaptureVisitor(WfLexicalScopeManager* _manager, Ptr<WfLexicalCapture> _capture)
						:manager(_manager)
						, capture(_capture)
					{
					}

					void Dispatch(WfVirtualCfeDeclaration* node)override
					{
						for (auto decl : node->expandedDeclarations)
						{
							decl->Accept(this);
						}
					}

					void Dispatch(WfVirtualCseDeclaration* node)override
					{
					}

					void Visit(WfFunctionDeclaration* node)override
					{
						manager->CreateLambdaCapture(node, capture);
					}
				};

				void Visit(WfNewInterfaceExpression* node)override
				{
					resultScope = Ptr(new WfLexicalScope(parentScope));
					auto capture = Ptr(new WfLexicalCapture);
					manager->CreateLambdaCapture(node, capture);

					CreateLambdaCaptureVisitor visitor(manager, capture);
					for (auto memberDecl : node->declarations)
					{
						memberDecl->Accept(&visitor);
						BuildScopeForDeclaration(manager, resultScope, memberDecl, node);
					}
				}

				void Visit(WfVirtualCfeExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->expandedExpression);
				}

				void Visit(WfVirtualCseExpression* node)override
				{
					node->Accept((WfVirtualCseExpression::IVisitor*)this);
				}

				void Visit(WfBindExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->expression);
				}

				void Visit(WfNewCoroutineExpression* node)override
				{
					manager->CreateLambdaCapture(node);

					resultScope = Ptr(new WfLexicalScope(parentScope));
					auto config = Ptr(new WfLexicalFunctionConfig);
					resultScope->functionConfig = config;

					if (node->name.value != L"")
					{
						auto symbol = Ptr(new WfLexicalSymbol(resultScope.Obj()));
						symbol->name = node->name.value;
						symbol->creatorNode = Ptr(node);
						symbol->typeInfo = TypeInfoRetriver<Ptr<CoroutineResult>>::CreateTypeInfo();
						symbol->type = GetTypeFromTypeInfo(symbol->typeInfo.Obj());
						resultScope->symbols.Add(symbol->name, symbol);
					}

					config->lambda = true;
					config->thisAccessable = false;
					config->parentThisAccessable = true;

					BuildScopeForStatement(manager, resultScope, node->statement);
				}

				void Visit(WfMixinCastExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->expression);
				}

				void Visit(WfExpectedTypeCastExpression* node)override
				{
					BuildScopeForExpression(manager, parentScope, node->expression);
				}

				void Visit(WfCoOperatorExpression* node)override
				{
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
				auto scope = Ptr(new WfLexicalScope(manager));
				scope->ownerNode = module;
				manager->nodeScopes.Add(module.Obj(), scope);

				for (auto declaration : module->declarations)
				{
					BuildScopeForDeclaration(manager, scope, declaration, module.Obj());
				}
			}

			void BuildScopeForDeclaration(WfLexicalScopeManager* manager, Ptr<WfLexicalScope> parentScope, Ptr<WfDeclaration> declaration, glr::ParsingAstBase* source)
			{
				BuildScopeForDeclarationVisitor::Execute(manager, parentScope, source, declaration);
			}

			void BuildScopeForStatement(WfLexicalScopeManager* manager, Ptr<WfLexicalScope> parentScope, Ptr<WfStatement> statement)
			{
				BuildScopeForStatementVisitor::Execute(manager, parentScope, statement);
			}

			void BuildScopeForExpression(WfLexicalScopeManager* manager, Ptr<WfLexicalScope> parentScope, Ptr<WfExpression> expression)
			{
				BuildScopeForExpressionVisitor::Execute(manager, parentScope, expression);
			}
		}
	}
}
