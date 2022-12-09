#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace reflection;
			using namespace reflection::description;

/***********************************************************************
ExpandStateMachineStatementVisitor
***********************************************************************/

			Ptr<WfExpression> GenerateStateMachineInvalidId()
			{
				auto refOne = Ptr(new WfIntegerExpression);
				refOne->value.value = L"1";

				auto refInvalid = Ptr(new WfUnaryExpression);
				refInvalid->op = WfUnaryOperator::Negative;
				refInvalid->operand = refOne;

				return refInvalid;
			}

			class ExpandStateMachineStatementVisitor
				: public copy_visitor::AstVisitor
			{
			protected:
				WfLexicalScopeManager*							manager;
				WfStateMachineInfo*								smInfo;

			public:
				ExpandStateMachineStatementVisitor(WfLexicalScopeManager* _manager, WfStateMachineInfo* _smInfo)
					:manager(_manager)
					, smInfo(_smInfo)
				{
				}

				using copy_visitor::AstVisitor::CopyNode;
				vl::Ptr<WfExpression> CopyNode(WfExpression* node)override
				{
					return CopyExpression(node, true);
				}

				void Visit(WfCoroutineStatement* node)override
				{
					CHECK_FAIL(L"ExpandStateMachineStatementVisitor::Dispatch(WfCoroutineStatement*)#ValidateStatementStructure should check coroutine statement's location.");
				}

				Ptr<WfStatement> GenerateIngoreInputStatement()
				{
					auto refThis = Ptr(new WfReferenceExpression);
					refThis->name.value = L"<state>stateMachineObject";

					auto refInput = Ptr(new WfMemberExpression);
					refInput->parent = refThis;
					refInput->name.value = L"stateMachineInput";

					auto assignExpr = Ptr(new WfBinaryExpression);
					assignExpr->op = WfBinaryOperator::Assign;
					assignExpr->first = refInput;
					assignExpr->second = GenerateStateMachineInvalidId();

					auto exprStat = Ptr(new WfExpressionStatement);
					exprStat->expression = assignExpr;

					return exprStat;
				}

				void GenerateStateSwitchCase(const WString& inputName, WfLexicalScope* smcScope, Ptr<WfSwitchStatement> switchStat, Ptr<WfStateInput>& input, Ptr<WfBlockStatement>& caseBlock)
				{
					input = From(smcScope->symbols[inputName])
						.Select([=](Ptr<WfLexicalSymbol> symbol)
						{
							return symbol->creatorNode.Cast<WfStateInput>();
						})
						.Where([=](Ptr<WfStateInput> decl)
						{
							return decl != nullptr;
						})
						.First();

					auto switchCase = Ptr(new WfSwitchCase);
					switchStat->caseBranches.Add(switchCase);
					{
						auto refInputId = Ptr(new WfIntegerExpression);
						refInputId->value.value = itow(smInfo->inputIds[inputName]);
						switchCase->expression = refInputId;
					}

					caseBlock = Ptr(new WfBlockStatement);
					switchCase->statement = caseBlock;
					caseBlock->statements.Add(GenerateIngoreInputStatement());
				}

				void Visit(WfStateSwitchStatement* node)override
				{
					auto smcScope = manager->nodeScopes[node]->FindFunctionScope()->parentScope.Obj();

					auto block = Ptr(new WfBlockStatement);
					{
						auto refThis = Ptr(new WfReferenceExpression);
						refThis->name.value = L"<state>stateMachineObject";

						auto refInput = Ptr(new WfMemberExpression);
						refInput->parent = refThis;
						refInput->name.value = L"stateMachineInput";

						auto compareInput = Ptr(new WfBinaryExpression);
						compareInput->op = WfBinaryOperator::EQ;
						compareInput->first = refInput;
						compareInput->second = GenerateStateMachineInvalidId();

						auto ifStat = Ptr(new WfIfStatement);
						block->statements.Add(ifStat);
						ifStat->expression = compareInput;

						auto trueBlock = Ptr(new WfBlockStatement);
						ifStat->trueBranch = trueBlock;

						trueBlock->statements.Add(Ptr(new WfCoPauseStatement));
					}

					auto switchStat = Ptr(new WfSwitchStatement);
					{
						auto refThis = Ptr(new WfReferenceExpression);
						refThis->name.value = L"<state>stateMachineObject";

						auto refInput = Ptr(new WfMemberExpression);
						refInput->parent = refThis;
						refInput->name.value = L"stateMachineInput";

						switchStat->expression = refInput;
					}
					block->statements.Add(switchStat);

					for (auto stateSwitchCase : node->caseBranches)
					{
						Ptr<WfStateInput> input;
						Ptr<WfBlockStatement> caseBlock;
						GenerateStateSwitchCase(stateSwitchCase->name.value, smcScope, switchStat, input, caseBlock);

						for (auto [argument, index] : indexed(stateSwitchCase->arguments))
						{
							auto refThis = Ptr(new WfReferenceExpression);
							refThis->name.value = L"<state>stateMachineObject";

							auto refArgument = Ptr(new WfMemberExpression);
							refArgument->parent = refThis;
							refArgument->name.value = manager->stateInputArguments[input->arguments[index].Obj()]->GetName();

							auto varDecl = Ptr(new WfVariableDeclaration);
							varDecl->name.value = argument->name.value;
							varDecl->expression = refArgument;

							auto varStat = Ptr(new WfVariableStatement);
							varStat->variable = varDecl;
							caseBlock->statements.Add(varStat);
						}
						caseBlock->statements.Add(CopyNode(stateSwitchCase->statement.Obj()));
						SetCodeRange(Ptr<WfStatement>(caseBlock), stateSwitchCase->codeRange);
					}

					if (node->type == WfStateSwitchType::Default)
					{
						auto invalidInputs=
							From(smInfo->inputIds.Keys())
							.Except(
								From(node->caseBranches)
								.Select([](Ptr<WfStateSwitchCase> switchCase) {return switchCase->name.value; })
								);
						for (auto inputName : invalidInputs)
						{
							Ptr<WfStateInput> input;
							Ptr<WfBlockStatement> caseBlock;
							GenerateStateSwitchCase(inputName, smcScope, switchStat, input, caseBlock);

							{
								auto refException = Ptr(new WfStringExpression);
								refException->value.value = L"Method \"" + inputName + L"\" of class \"" + manager->stateInputMethods[input.Obj()]->GetOwnerTypeDescriptor()->GetTypeName() + L"\" cannot be called at this moment.";

								auto raiseStat = Ptr(new WfRaiseExceptionStatement);
								raiseStat->expression = refException;

								caseBlock->statements.Add(raiseStat);
							}
						}
					}

					auto defaultBlock = Ptr(new WfBlockStatement);

					switch (node->type)
					{
					case WfStateSwitchType::Default:
					case WfStateSwitchType::Pass:
						break;
					case WfStateSwitchType::PassAndReturn:
						{
							auto gotoStat = Ptr(new WfGotoStatement);
							gotoStat->label.value = L"<state-label>OUT_OF_STATE_MACHINE";
							defaultBlock->statements.Add(gotoStat);
						}
						break;
					case WfStateSwitchType::Ignore:
						{
							defaultBlock->statements.Add(GenerateIngoreInputStatement());
						}
						break;
					case WfStateSwitchType::IgnoreAndReturn:
						{
							defaultBlock->statements.Add(GenerateIngoreInputStatement());

							auto gotoStat = Ptr(new WfGotoStatement);
							gotoStat->label.value = L"<state-label>OUT_OF_STATE_MACHINE";
							defaultBlock->statements.Add(gotoStat);
						}
						break;
					default:;
					}

					if (defaultBlock->statements.Count() > 0)
					{
						switchStat->defaultBranch = defaultBlock;
					}
					SetCodeRange(Ptr<WfStatement>(block), node->codeRange);
					result = block;
				}

				void Visit(WfStateInvokeStatement* node)override
				{
					auto smcScope = manager->nodeScopes[node]->FindFunctionScope()->parentScope.Obj();
					auto stateDecl = From(smcScope->symbols[node->name.value])
						.Select([=](Ptr<WfLexicalSymbol> symbol)
						{
							return symbol->creatorNode.Cast<WfStateDeclaration>();
						})
						.Where([=](Ptr<WfStateDeclaration> decl)
						{
							return decl != nullptr;
						})
						.First();

					auto block = Ptr(new WfBlockStatement);

					for (auto [argument, index] : indexed(stateDecl->arguments))
					{
						auto refThis = Ptr(new WfReferenceExpression);
						refThis->name.value = L"<state>stateMachineObject";
						
						auto refArgument = Ptr(new WfMemberExpression);
						refArgument->parent = refThis;
						refArgument->name.value = manager->stateDeclArguments[argument.Obj()]->GetName();

						auto assignExpr = Ptr(new WfBinaryExpression);
						assignExpr->op = WfBinaryOperator::Assign;
						assignExpr->first = refArgument;
						assignExpr->second = CopyNode(node->arguments[index].Obj());

						auto exprStat = Ptr(new WfExpressionStatement);
						exprStat->expression = assignExpr;
						block->statements.Add(exprStat);
					}

					switch (node->type)
					{
					case WfStateInvokeType::Goto:
						{
							{
								auto refState = Ptr(new WfReferenceExpression);
								refState->name.value = L"<state>state";

								auto refStateId = Ptr(new WfIntegerExpression);
								refStateId->value.value = itow(smInfo->stateIds[node->name.value]);

								auto assignExpr = Ptr(new WfBinaryExpression);
								assignExpr->op = WfBinaryOperator::Assign;
								assignExpr->first = refState;
								assignExpr->second = refStateId;

								auto exprStat = Ptr(new WfExpressionStatement);
								exprStat->expression = assignExpr;
								block->statements.Add(exprStat);
							}
							{
								auto gotoStat = Ptr(new WfGotoStatement);
								gotoStat->label.value = L"<state-label>OUT_OF_CURRENT_STATE";
								block->statements.Add(gotoStat);
							}
						}
						break;
					case WfStateInvokeType::Push:
						{
							{
								auto refThis = Ptr(new WfReferenceExpression);
								refThis->name.value = L"<state>stateMachineObject";

								auto refCoroutine = Ptr(new WfMemberExpression);
								refCoroutine->parent = refThis;
								refCoroutine->name.value = L"<state>CreateCoroutine";

								auto refStateId = Ptr(new WfIntegerExpression);
								refStateId->value.value = itow(smInfo->stateIds[node->name.value]);

								auto callExpr = Ptr(new WfCallExpression);
								callExpr->function = refCoroutine;
								callExpr->arguments.Add(refStateId);

								auto exprStat = Ptr(new WfExpressionStatement);
								exprStat->expression = callExpr;
								block->statements.Add(exprStat);
							}
							{
								block->statements.Add(Ptr(new WfCoPauseStatement));
							}
						}
						break;
					default:;
					}
					SetCodeRange(Ptr<WfStatement>(block), node->codeRange);
					result = block;
				}
			};

/***********************************************************************
ExpandStateMachine
***********************************************************************/

			void ExpandStateMachine(WfLexicalScopeManager* manager, WfStateMachineDeclaration* node)
			{
				auto& smInfo = manager->stateMachineInfos[node];

				for (auto input : node->inputs)
				{
					smInfo->inputIds.Add(input->name.value, smInfo->inputIds.Count());

					for (auto argument : input->arguments)
					{
						// var <stateip-INPUT>NAME = <DEFAULT-VALUE>;
						auto fieldInfo = manager->stateInputArguments[argument.Obj()];

						auto varDecl = Ptr(new WfVariableDeclaration);
						varDecl->name.value = fieldInfo->GetName();
						varDecl->type = GetTypeFromTypeInfo(fieldInfo->GetReturn());
						varDecl->expression = CreateDefaultValue(fieldInfo->GetReturn());

						auto att = Ptr(new WfAttribute);
						att->category.value = L"cpp";
						att->name.value = L"Private";
						varDecl->attributes.Add(att);

						node->expandedDeclarations.Add(varDecl);
						manager->declarationMemberInfos.Add(varDecl, fieldInfo);
					}
				}

				smInfo->stateIds.Add(L"", 0);
				for (auto state : node->states)
				{
					if (state->name.value != L"")
					{
						smInfo->stateIds.Add(state->name.value, smInfo->stateIds.Count());
					}

					for (auto argument : state->arguments)
					{
						// var <statesp-INPUT>NAME = <DEFAULT-VALUE>;
						auto fieldInfo = manager->stateDeclArguments[argument.Obj()];

						auto varDecl = Ptr(new WfVariableDeclaration);
						varDecl->name.value = fieldInfo->GetName();
						varDecl->type = GetTypeFromTypeInfo(fieldInfo->GetReturn());
						varDecl->expression = CreateDefaultValue(fieldInfo->GetReturn());

						auto att = Ptr(new WfAttribute);
						att->category.value = L"cpp";
						att->name.value = L"Private";
						varDecl->attributes.Add(att);

						node->expandedDeclarations.Add(varDecl);
						manager->declarationMemberInfos.Add(varDecl, fieldInfo);
					}
				}

				for (auto input : node->inputs)
				{
					auto methodInfo = manager->stateInputMethods[input.Obj()];

					// func INPUT(ARGUMENTS ...): void
					auto funcDecl = Ptr(new WfFunctionDeclaration);
					funcDecl->functionKind = WfFunctionKind::Normal;
					funcDecl->anonymity = WfFunctionAnonymity::Named;
					funcDecl->name.value = methodInfo->GetName();
					funcDecl->returnType = GetTypeFromTypeInfo(methodInfo->GetReturn());
					for (auto [argument, index] : indexed(input->arguments))
					{
						auto funcArgument = Ptr(new WfFunctionArgument);
						funcArgument->name.value = argument->name.value;
						funcArgument->type = GetTypeFromTypeInfo(methodInfo->GetParameter(index)->GetType());
						funcDecl->arguments.Add(funcArgument);
					}

					node->expandedDeclarations.Add(funcDecl);
					manager->declarationMemberInfos.Add(funcDecl, methodInfo);

					auto block = Ptr(new WfBlockStatement);
					funcDecl->statement = block;
					{
						// if (not this.stateMachineInitialized)
						auto trueBlock = Ptr(new WfBlockStatement);
						{
							auto refInit = Ptr(new WfMemberExpression);
							refInit->parent = Ptr(new WfThisExpression);
							refInit->name.value = L"stateMachineInitialized";

							auto notExpr = Ptr(new WfUnaryExpression);
							notExpr->op = WfUnaryOperator::Not;
							notExpr->operand = refInit;

							auto ifStat = Ptr(new WfIfStatement);
							ifStat->expression = notExpr;
							ifStat->trueBranch = trueBlock;

							block->statements.Add(ifStat);
						}
						{
							// this.stateMachineInitialized = true;
							auto refInit = Ptr(new WfMemberExpression);
							refInit->parent = Ptr(new WfThisExpression);
							refInit->name.value = L"stateMachineInitialized";

							auto refTrue = Ptr(new WfLiteralExpression);
							refTrue->value = WfLiteralValue::True;

							auto assignExpr = Ptr(new WfBinaryExpression);
							assignExpr->op = WfBinaryOperator::Assign;
							assignExpr->first = refInit;
							assignExpr->second = refTrue;

							auto exprStat = Ptr(new WfExpressionStatement);
							exprStat->expression = assignExpr;
							trueBlock->statements.Add(exprStat);
						}
						{
							// this.<state>CreateCoroutine(0);
							auto refCC = Ptr(new WfMemberExpression);
							refCC->parent = Ptr(new WfThisExpression);
							refCC->name.value = smInfo->createCoroutineMethod->GetName();

							auto refZero = Ptr(new WfIntegerExpression);
							refZero->value.value = L"0";

							auto callExpr = Ptr(new WfCallExpression);
							callExpr->function = refCC;
							callExpr->arguments.Add(refZero);

							auto exprStat = Ptr(new WfExpressionStatement);
							exprStat->expression = callExpr;
							trueBlock->statements.Add(exprStat);
						}
						{
							// this.ResumeStateMachine();
							auto refResume = Ptr(new WfMemberExpression);
							refResume->parent = Ptr(new WfThisExpression);
							refResume->name.value = L"ResumeStateMachine";

							auto callExpr = Ptr(new WfCallExpression);
							callExpr->function = refResume;

							auto exprStat = Ptr(new WfExpressionStatement);
							exprStat->expression = callExpr;
							trueBlock->statements.Add(exprStat);
						}
					}
					{
						// this.stateMachineInput = <INPUT>
						auto refInput = Ptr(new WfMemberExpression);
						refInput->parent = Ptr(new WfThisExpression);
						refInput->name.value = L"stateMachineInput";

						auto refInputId = Ptr(new WfIntegerExpression);
						refInputId->value.value = itow(smInfo->inputIds[input->name.value]);

						auto assignExpr = Ptr(new WfBinaryExpression);
						assignExpr->op = WfBinaryOperator::Assign;
						assignExpr->first = refInput;
						assignExpr->second = refInputId;

						auto exprStat = Ptr(new WfExpressionStatement);
						exprStat->expression = assignExpr;
						block->statements.Add(exprStat);
					}
					for (auto [argument, index] : indexed(input->arguments))
					{
						// this.<stateip-INPUT>NAME = NAME;
						auto refField = Ptr(new WfMemberExpression);
						refField->parent = Ptr(new WfThisExpression);
						refField->name.value = manager->stateInputArguments[argument.Obj()]->GetName();

						auto refArgument = Ptr(new WfReferenceExpression);
						refArgument->name.value = methodInfo->GetParameter(index)->GetName();

						auto assignExpr = Ptr(new WfBinaryExpression);
						assignExpr->op = WfBinaryOperator::Assign;
						assignExpr->first = refField;
						assignExpr->second = refArgument;

						auto exprStat = Ptr(new WfExpressionStatement);
						exprStat->expression = assignExpr;
						block->statements.Add(exprStat);
					}
					{
						// this.ResumeStateMachine();
						auto refResume = Ptr(new WfMemberExpression);
						refResume->parent = Ptr(new WfThisExpression);
						refResume->name.value = L"ResumeStateMachine";

						auto callExpr = Ptr(new WfCallExpression);
						callExpr->function = refResume;

						auto exprStat = Ptr(new WfExpressionStatement);
						exprStat->expression = callExpr;
						block->statements.Add(exprStat);
					}
				}

				{
					// func <state>CreateCoroutine(<state>startState: int): void
					auto funcDecl = Ptr(new WfFunctionDeclaration);
					funcDecl->functionKind = WfFunctionKind::Normal;
					funcDecl->anonymity = WfFunctionAnonymity::Named;
					funcDecl->name.value = smInfo->createCoroutineMethod->GetName();
					funcDecl->returnType = GetTypeFromTypeInfo(TypeInfoRetriver<void>::CreateTypeInfo().Obj());
					{
						auto parameterInfo = smInfo->createCoroutineMethod->GetParameter(0);

						auto funcArgument = Ptr(new WfFunctionArgument);
						funcArgument->name.value = parameterInfo->GetName();
						funcArgument->type = GetTypeFromTypeInfo(parameterInfo->GetType());
						funcDecl->arguments.Add(funcArgument);
					}

					auto att = Ptr(new WfAttribute);
					att->category.value = L"cpp";
					att->name.value = L"Private";
					funcDecl->attributes.Add(att);

					node->expandedDeclarations.Add(funcDecl);
					manager->declarationMemberInfos.Add(funcDecl, smInfo->createCoroutineMethod);

					auto block = Ptr(new WfBlockStatement);
					funcDecl->statement = block;

					{
						// <state>stateMachineObject = this;
						auto varDecl = Ptr(new WfVariableDeclaration);
						varDecl->name.value = L"<state>stateMachineObject";
						varDecl->expression = Ptr(new WfThisExpression);

						auto stat = Ptr(new WfVariableStatement);
						stat->variable = varDecl;

						block->statements.Add(stat);
					}
					{
						// <state>previousCoroutine = stateMachineCoroutine;
						auto refThis = Ptr(new WfReferenceExpression);
						refThis->name.value = L"<state>stateMachineObject";

						auto refCoroutine = Ptr(new WfMemberExpression);
						refCoroutine->parent = refThis;
						refCoroutine->name.value = L"stateMachineCoroutine";

						auto varDecl = Ptr(new WfVariableDeclaration);
						varDecl->name.value = L"<state>previousCoroutine";
						varDecl->expression = refCoroutine;

						auto stat = Ptr(new WfVariableStatement);
						stat->variable = varDecl;

						block->statements.Add(stat);
					}
					{
						// stateMachineCoroutine = $coroutine{ ... }
						auto refThis = Ptr(new WfReferenceExpression);
						refThis->name.value = L"<state>stateMachineObject";

						auto refCoroutine = Ptr(new WfMemberExpression);
						refCoroutine->parent = refThis;
						refCoroutine->name.value = L"stateMachineCoroutine";

						auto coroutine = Ptr(new WfNewCoroutineExpression);
						auto coroutineBlock = Ptr(new WfBlockStatement);
						coroutine->statement = coroutineBlock;

						auto assignExpr = Ptr(new WfBinaryExpression);
						assignExpr->op = WfBinaryOperator::Assign;
						assignExpr->first = refCoroutine;
						assignExpr->second = coroutine;

						auto exprStat = Ptr(new WfExpressionStatement);
						exprStat->expression = assignExpr;
						block->statements.Add(exprStat);

						{
							auto tryStat = Ptr(new WfTryStatement);
							coroutineBlock->statements.Add(tryStat);

							auto tryBlock = Ptr(new WfBlockStatement);
							tryBlock->endLabel.value = L"<state-label>OUT_OF_STATE_MACHINE";
							tryStat->protectedStatement = tryBlock;

							auto finallyBlock = Ptr(new WfBlockStatement);
							tryStat->finallyStatement = finallyBlock;

							// try {... <state-label>OUT_OF_STATE_MACHINE:;}
							{
								// var <state>State = <state>startState;
								auto refStartState = Ptr(new WfReferenceExpression);
								refStartState->name.value = L"<state>startState";

								auto varDecl = Ptr(new WfVariableDeclaration);
								varDecl->name.value = L"<state>state";
								varDecl->expression = refStartState;

								auto stat = Ptr(new WfVariableStatement);
								stat->variable = varDecl;

								tryBlock->statements.Add(stat);
							}
							{
								// while(true){... <state-label>OUT_OF_CURRENT_STATE:;}
								auto whileBlock = Ptr(new WfBlockStatement);
								whileBlock->endLabel.value = L"<state-label>OUT_OF_CURRENT_STATE";
								{
									auto refTrue = Ptr(new WfLiteralExpression);
									refTrue->value = WfLiteralValue::True;

									auto whileStat = Ptr(new WfWhileStatement);
									whileStat->condition = refTrue;
									whileStat->statement = whileBlock;

									tryBlock->statements.Add(whileStat);
								}
								{
									// var <state>currentState = <state>state;
									auto refStartState = Ptr(new WfReferenceExpression);
									refStartState->name.value = L"<state>state";

									auto varDecl = Ptr(new WfVariableDeclaration);
									varDecl->name.value = L"<state>currentState";
									varDecl->expression = refStartState;

									auto stat = Ptr(new WfVariableStatement);
									stat->variable = varDecl;

									whileBlock->statements.Add(stat);
								}
								{
									// <state>state = -1;
									auto refState = Ptr(new WfReferenceExpression);
									refState->name.value = L"<state>state";

									auto assignExpr = Ptr(new WfBinaryExpression);
									assignExpr->op = WfBinaryOperator::Assign;
									assignExpr->first = refState;
									assignExpr->second = GenerateStateMachineInvalidId();

									auto exprStat = Ptr(new WfExpressionStatement);
									exprStat->expression = assignExpr;
									whileBlock->statements.Add(exprStat);
								}
								{
									// switch(<state>currentState) { case STATE:{... goto <state-label>OUT_OF_STATE_MACHINE;} ... }
									auto refCurrentState = Ptr(new WfReferenceExpression);
									refCurrentState->name.value = L"<state>currentState";

									auto switchStat = Ptr(new WfSwitchStatement);
									switchStat->expression = refCurrentState;
									whileBlock->statements.Add(switchStat);

									for (auto state : node->states)
									{
										auto switchCase = Ptr(new WfSwitchCase);
										switchStat->caseBranches.Add(switchCase);

										auto refStateId = Ptr(new WfIntegerExpression);
										refStateId->value.value = itow(smInfo->stateIds[state->name.value]);
										switchCase->expression = refStateId;

										auto caseBlock = Ptr(new WfBlockStatement);
										switchCase->statement = caseBlock;

										for (auto argument : state->arguments)
										{
											auto refThis = Ptr(new WfReferenceExpression);
											refThis->name.value = L"<state>stateMachineObject";

											auto refArgument = Ptr(new WfMemberExpression);
											refArgument->parent = refThis;
											refArgument->name.value = manager->stateDeclArguments[argument.Obj()]->GetName();

											auto varDecl = Ptr(new WfVariableDeclaration);
											varDecl->name.value = argument->name.value;
											varDecl->expression = refArgument;

											auto varStat = Ptr(new WfVariableStatement);
											varStat->variable = varDecl;
											caseBlock->statements.Add(varStat);
										}
										caseBlock->statements.Add(ExpandStateMachineStatementVisitor(manager, smInfo.Obj()).CopyNode(state->statement.Obj()));
										{
											auto gotoStat = Ptr(new WfGotoStatement);
											gotoStat->label.value = L"<state-label>OUT_OF_STATE_MACHINE";
											caseBlock->statements.Add(gotoStat);
										}
										SetCodeRange(Ptr<WfStatement>(caseBlock), state->codeRange);
									}
								}
							}
							// finally {stateMachineCoroutine = <state>previousCoroutine;}
							{
								auto refThis = Ptr(new WfReferenceExpression);
								refThis->name.value = L"<state>stateMachineObject";

								auto refCoroutine = Ptr(new WfMemberExpression);
								refCoroutine->parent = refThis;
								refCoroutine->name.value = L"stateMachineCoroutine";

								auto refPrevious = Ptr(new WfReferenceExpression);
								refPrevious->name.value = L"<state>previousCoroutine";

								auto assignExpr = Ptr(new WfBinaryExpression);
								assignExpr->op = WfBinaryOperator::Assign;
								assignExpr->first = refCoroutine;
								assignExpr->second = refPrevious;

								auto exprStat = Ptr(new WfExpressionStatement);
								exprStat->expression = assignExpr;
								finallyBlock->statements.Add(exprStat);
							}
						}
					}
				}
			}
		}
	}
}