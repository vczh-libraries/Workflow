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

			class ExpandStateMachineStatementVisitor
				: public copy_visitor::StatementVisitor
				, public copy_visitor::VirtualCseStatementVisitor
				, public WfStateMachineStatement::IVisitor
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

				vl::Ptr<WfExpression> CreateField(vl::Ptr<WfExpression> from)override
				{
					return CopyExpression(from, true);
				}

				vl::Ptr<WfType> CreateField(vl::Ptr<WfType> from)override
				{
					return CopyType(from);
				}

				vl::Ptr<WfStatement> CreateField(vl::Ptr<WfStatement> from)override
				{
					if (!from) return nullptr;
					from->Accept(this);
					return result.Cast<WfStatement>();
				}

				vl::Ptr<vl::parsing::ParsingTreeCustomBase> Dispatch(WfVirtualCseStatement* node)override
				{
					node->Accept((WfVirtualCseStatement::IVisitor*)this);
					return result;
				}

				vl::Ptr<vl::parsing::ParsingTreeCustomBase> Dispatch(WfCoroutineStatement* node)override
				{
					CHECK_FAIL(L"ExpandStateMachineStatementVisitor::Dispatch(WfCoroutineStatement*)#ValidateStatementStructure should check coroutine statement's location.");
				}

				vl::Ptr<vl::parsing::ParsingTreeCustomBase> Dispatch(WfStateMachineStatement* node)override
				{
					node->Accept((WfStateMachineStatement::IVisitor*)this);
					return result;
				}

				Ptr<WfStatement> GenerateIngoreInputStatement()
				{
					auto refThis = MakePtr<WfReferenceExpression>();
					refThis->name.value = L"<state>stateMachineObject";

					auto refInput = MakePtr<WfMemberExpression>();
					refInput->parent = refThis;
					refInput->name.value = L"stateMachineInput";

					auto refOne = MakePtr<WfIntegerExpression>();
					refOne->value.value = L"1";

					auto refInvalid = MakePtr<WfUnaryExpression>();
					refInvalid->op = WfUnaryOperator::Negative;
					refInvalid->operand = refOne;

					auto assignExpr = MakePtr<WfBinaryExpression>();
					assignExpr->op = WfBinaryOperator::Assign;
					assignExpr->first = refInput;
					assignExpr->second = refInvalid;

					auto exprStat = MakePtr<WfExpressionStatement>();
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

					auto switchCase = MakePtr<WfSwitchCase>();
					switchStat->caseBranches.Add(switchCase);
					{
						auto refInputId = MakePtr<WfIntegerExpression>();
						refInputId->value.value = itow(smInfo->inputIds[inputName]);
						switchCase->expression = refInputId;
					}

					caseBlock = MakePtr<WfBlockStatement>();
					switchCase->statement = caseBlock;
					caseBlock->statements.Add(GenerateIngoreInputStatement());
				}

				void Visit(WfStateSwitchStatement* node)override
				{
					auto smcScope = manager->nodeScopes[node]->FindFunctionScope()->parentScope.Obj();

					auto block = MakePtr<WfBlockStatement>();
					block->statements.Add(MakePtr<WfCoPauseStatement>());

					auto switchStat = MakePtr<WfSwitchStatement>();
					{
						auto refThis = MakePtr<WfReferenceExpression>();
						refThis->name.value = L"<state>stateMachineObject";

						auto refInput = MakePtr<WfMemberExpression>();
						refInput->parent = refThis;
						refInput->name.value = L"stateMachineInput";

						switchStat->expression = refInput;
					}
					block->statements.Add(switchStat);

					FOREACH(Ptr<WfStateSwitchCase>, stateSwitchCase, node->caseBranches)
					{
						Ptr<WfStateInput> input;
						Ptr<WfBlockStatement> caseBlock;
						GenerateStateSwitchCase(stateSwitchCase->name.value, smcScope, switchStat, input, caseBlock);

						FOREACH_INDEXER(Ptr<WfStateSwitchArgument>, argument, index, stateSwitchCase->arguments)
						{
							auto refThis = MakePtr<WfReferenceExpression>();
							refThis->name.value = L"<state>stateMachineObject";

							auto refArgument = MakePtr<WfMemberExpression>();
							refArgument->parent = refThis;
							refArgument->name.value = manager->stateInputArguments[input->arguments[index].Obj()]->GetName();

							auto varDecl = MakePtr<WfVariableDeclaration>();
							varDecl->name.value = argument->name.value;
							varDecl->expression = refArgument;

							auto varStat = MakePtr<WfVariableStatement>();
							varStat->variable = varDecl;
							caseBlock->statements.Add(varStat);
						}
						caseBlock->statements.Add(CreateField(stateSwitchCase->statement));
					}

					if (node->type == WfStateSwitchType::Default)
					{
						auto invalidInputs=
							From(smInfo->inputIds.Keys())
							.Except(
								From(node->caseBranches)
								.Select([](Ptr<WfStateSwitchCase> switchCase) {return switchCase->name.value; })
								);
						FOREACH(WString, inputName, invalidInputs)
						{
							Ptr<WfStateInput> input;
							Ptr<WfBlockStatement> caseBlock;
							GenerateStateSwitchCase(inputName, smcScope, switchStat, input, caseBlock);

							caseBlock->statements.Add(GenerateIngoreInputStatement());
							{
								auto refException = MakePtr<WfStringExpression>();
								refException->value.value = L"Method \"" + inputName + L"\" of class \"" + manager->stateInputMethods[input.Obj()]->GetOwnerTypeDescriptor()->GetTypeName() + L"\" cannot be called at this moment.";

								auto raiseStat = MakePtr<WfRaiseExceptionStatement>();
								raiseStat->expression = refException;

								caseBlock->statements.Add(raiseStat);
							}
						}
					}

					auto defaultBlock = MakePtr<WfBlockStatement>();

					switch (node->type)
					{
					case WfStateSwitchType::Default:
					case WfStateSwitchType::Pass:
						break;
					case WfStateSwitchType::PassAndReturn:
						{
							auto gotoStat = MakePtr<WfGotoStatement>();
							gotoStat->label.value = L"<state-label>OUT_OF_CURRENT_STATE";
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

							auto gotoStat = MakePtr<WfGotoStatement>();
							gotoStat->label.value = L"<state-label>OUT_OF_CURRENT_STATE";
							defaultBlock->statements.Add(gotoStat);
						}
						break;
					}

					if (defaultBlock->statements.Count() > 0)
					{
						switchStat->defaultBranch = defaultBlock;
					}
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

					auto block = MakePtr<WfBlockStatement>();

					FOREACH_INDEXER(Ptr<WfFunctionArgument>, argument, index, stateDecl->arguments)
					{
						auto refThis = MakePtr<WfReferenceExpression>();
						refThis->name.value = L"<state>stateMachineObject";
						
						auto refArgument = MakePtr<WfMemberExpression>();
						refArgument->parent = refThis;
						refArgument->name.value = manager->stateDeclArguments[argument.Obj()]->GetName();

						auto assignExpr = MakePtr<WfBinaryExpression>();
						assignExpr->op = WfBinaryOperator::Assign;
						assignExpr->first = refArgument;
						assignExpr->second = CreateField(node->arguments[index]);

						auto exprStat = MakePtr<WfExpressionStatement>();
						exprStat->expression = assignExpr;
						block->statements.Add(exprStat);
					}

					switch (node->type)
					{
					case WfStateInvokeType::Goto:
						{
							{
								auto refState = MakePtr<WfReferenceExpression>();
								refState->name.value = L"<state>state";

								auto refStateId = MakePtr<WfIntegerExpression>();
								refStateId->value.value = itow(smInfo->stateIds[node->name.value]);

								auto assignExpr = MakePtr<WfBinaryExpression>();
								assignExpr->op = WfBinaryOperator::Assign;
								assignExpr->first = refState;
								assignExpr->second = refStateId;

								auto exprStat = MakePtr<WfExpressionStatement>();
								exprStat->expression = assignExpr;
								block->statements.Add(exprStat);
							}
							{
								auto gotoStat = MakePtr<WfGotoStatement>();
								gotoStat->label.value = L"<state-label>OUT_OF_CURRENT_STATE";
								block->statements.Add(gotoStat);
							}
						}
						break;
					case WfStateInvokeType::Push:
						{
							{
								auto refThis = MakePtr<WfReferenceExpression>();
								refThis->name.value = L"<state>stateMachineObject";

								auto refCoroutine = MakePtr<WfMemberExpression>();
								refCoroutine->parent = refThis;
								refCoroutine->name.value = L"<state>CreateCoroutine";

								auto refStateId = MakePtr<WfIntegerExpression>();
								refStateId->value.value = itow(smInfo->stateIds[node->name.value]);

								auto callExpr = MakePtr<WfCallExpression>();
								callExpr->function = refCoroutine;
								callExpr->arguments.Add(refStateId);

								auto exprStat = MakePtr<WfExpressionStatement>();
								exprStat->expression = callExpr;
								block->statements.Add(exprStat);
							}
							{
								block->statements.Add(MakePtr<WfCoPauseStatement>());
							}
						}
						break;
					}
					result = block;
				}
			};

/***********************************************************************
ExpandStateMachine
***********************************************************************/

			void ExpandStateMachine(WfLexicalScopeManager* manager, WfStateMachineDeclaration* node)
			{
				auto& smInfo = manager->stateMachineInfos[node];

				FOREACH(Ptr<WfStateInput>, input, node->inputs)
				{
					smInfo->inputIds.Add(input->name.value, smInfo->inputIds.Count());

					FOREACH(Ptr<WfFunctionArgument>, argument, input->arguments)
					{
						// var <stateip-INPUT>NAME = <DEFAULT-VALUE>;
						auto fieldInfo = manager->stateInputArguments[argument.Obj()];

						auto varDecl = MakePtr<WfVariableDeclaration>();
						varDecl->name.value = fieldInfo->GetName();
						varDecl->type = GetTypeFromTypeInfo(fieldInfo->GetReturn());
						varDecl->expression = CreateDefaultValue(fieldInfo->GetReturn());

						auto classMember = MakePtr<WfClassMember>();
						classMember->kind = WfClassMemberKind::Normal;
						varDecl->classMember = classMember;

						auto att = MakePtr<WfAttribute>();
						att->category.value = L"cpp";
						att->name.value = L"Private";
						varDecl->attributes.Add(att);

						node->expandedDeclarations.Add(varDecl);
						manager->declarationMemberInfos.Add(varDecl, fieldInfo);
					}
				}

				smInfo->stateIds.Add(L"", 0);
				FOREACH(Ptr<WfStateDeclaration>, state, node->states)
				{
					if (state->name.value != L"")
					{
						smInfo->stateIds.Add(state->name.value, smInfo->stateIds.Count());
					}

					FOREACH(Ptr<WfFunctionArgument>, argument, state->arguments)
					{
						// var <statesp-INPUT>NAME = <DEFAULT-VALUE>;
						auto fieldInfo = manager->stateDeclArguments[argument.Obj()];

						auto varDecl = MakePtr<WfVariableDeclaration>();
						varDecl->name.value = fieldInfo->GetName();
						varDecl->type = GetTypeFromTypeInfo(fieldInfo->GetReturn());
						varDecl->expression = CreateDefaultValue(fieldInfo->GetReturn());

						auto classMember = MakePtr<WfClassMember>();
						classMember->kind = WfClassMemberKind::Normal;
						varDecl->classMember = classMember;

						auto att = MakePtr<WfAttribute>();
						att->category.value = L"cpp";
						att->name.value = L"Private";
						varDecl->attributes.Add(att);

						node->expandedDeclarations.Add(varDecl);
						manager->declarationMemberInfos.Add(varDecl, fieldInfo);
					}
				}

				FOREACH(Ptr<WfStateInput>, input, node->inputs)
				{
					auto methodInfo = manager->stateInputMethods[input.Obj()];

					// func INPUT(ARGUMENTS ...): void
					auto funcDecl = MakePtr<WfFunctionDeclaration>();
					funcDecl->name.value = methodInfo->GetName();
					funcDecl->returnType = GetTypeFromTypeInfo(methodInfo->GetReturn());
					FOREACH_INDEXER(Ptr<WfFunctionArgument>, argument, index, input->arguments)
					{
						auto funcArgument = MakePtr<WfFunctionArgument>();
						funcArgument->name.value = argument->name.value;
						funcArgument->type = GetTypeFromTypeInfo(methodInfo->GetParameter(index)->GetType());
						funcDecl->arguments.Add(funcArgument);
					}

					auto classMember = MakePtr<WfClassMember>();
					classMember->kind = WfClassMemberKind::Normal;
					funcDecl->classMember = classMember;

					node->expandedDeclarations.Add(funcDecl);
					manager->declarationMemberInfos.Add(funcDecl, methodInfo);

					auto block = MakePtr<WfBlockStatement>();
					funcDecl->statement = block;
					{
						// if (not this.stateMachineInitialized)
						auto trueBlock = MakePtr<WfBlockStatement>();
						{
							auto refInit = MakePtr<WfMemberExpression>();
							refInit->parent = MakePtr<WfThisExpression>();
							refInit->name.value = L"stateMachineInitialized";

							auto notExpr = MakePtr<WfUnaryExpression>();
							notExpr->op = WfUnaryOperator::Not;
							notExpr->operand = refInit;

							auto ifStat = MakePtr<WfIfStatement>();
							ifStat->expression = notExpr;
							ifStat->trueBranch = trueBlock;

							block->statements.Add(ifStat);
						}
						{
							// this.stateMachineInitialized = true;
							auto refInit = MakePtr<WfMemberExpression>();
							refInit->parent = MakePtr<WfThisExpression>();
							refInit->name.value = L"stateMachineInitialized";

							auto refTrue = MakePtr<WfLiteralExpression>();
							refTrue->value = WfLiteralValue::True;

							auto assignExpr = MakePtr<WfBinaryExpression>();
							assignExpr->op = WfBinaryOperator::Assign;
							assignExpr->first = refInit;
							assignExpr->second = refTrue;

							auto exprStat = MakePtr<WfExpressionStatement>();
							exprStat->expression = assignExpr;
							trueBlock->statements.Add(exprStat);
						}
						{
							// this.<state>CreateCoroutine(0);
							auto refCC = MakePtr<WfMemberExpression>();
							refCC->parent = MakePtr<WfThisExpression>();
							refCC->name.value = smInfo->createCoroutineMethod->GetName();

							auto refZero = MakePtr<WfIntegerExpression>();
							refZero->value.value = L"0";

							auto callExpr = MakePtr<WfCallExpression>();
							callExpr->function = refCC;
							callExpr->arguments.Add(refZero);

							auto exprStat = MakePtr<WfExpressionStatement>();
							exprStat->expression = callExpr;
							trueBlock->statements.Add(exprStat);
						}
						{
							// this.ResumeStateMachine();
							auto refResume = MakePtr<WfMemberExpression>();
							refResume->parent = MakePtr<WfThisExpression>();
							refResume->name.value = L"ResumeStateMachine";

							auto callExpr = MakePtr<WfCallExpression>();
							callExpr->function = refResume;

							auto exprStat = MakePtr<WfExpressionStatement>();
							exprStat->expression = callExpr;
							trueBlock->statements.Add(exprStat);
						}
					}
					{
						// this.stateMachineInput = <INPUT>
						auto refInput = MakePtr<WfMemberExpression>();
						refInput->parent = MakePtr<WfThisExpression>();
						refInput->name.value = L"stateMachineInput";

						auto refInputId = MakePtr<WfIntegerExpression>();
						refInputId->value.value = itow(smInfo->inputIds[input->name.value]);

						auto assignExpr = MakePtr<WfBinaryExpression>();
						assignExpr->op = WfBinaryOperator::Assign;
						assignExpr->first = refInput;
						assignExpr->second = refInputId;

						auto exprStat = MakePtr<WfExpressionStatement>();
						exprStat->expression = assignExpr;
						block->statements.Add(exprStat);
					}
					FOREACH_INDEXER(Ptr<WfFunctionArgument>, argument, index, input->arguments)
					{
						// this.<stateip-INPUT>NAME = NAME;
						auto refField = MakePtr<WfMemberExpression>();
						refField->parent = MakePtr<WfThisExpression>();
						refField->name.value = manager->stateInputArguments[argument.Obj()]->GetName();

						auto refArgument = MakePtr<WfReferenceExpression>();
						refArgument->name.value = methodInfo->GetParameter(index)->GetName();

						auto assignExpr = MakePtr<WfBinaryExpression>();
						assignExpr->op = WfBinaryOperator::Assign;
						assignExpr->first = refField;
						assignExpr->second = refArgument;

						auto exprStat = MakePtr<WfExpressionStatement>();
						exprStat->expression = assignExpr;
						block->statements.Add(exprStat);
					}
					{
						// this.ResumeStateMachine();
						auto refResume = MakePtr<WfMemberExpression>();
						refResume->parent = MakePtr<WfThisExpression>();
						refResume->name.value = L"ResumeStateMachine";

						auto callExpr = MakePtr<WfCallExpression>();
						callExpr->function = refResume;

						auto exprStat = MakePtr<WfExpressionStatement>();
						exprStat->expression = callExpr;
						block->statements.Add(exprStat);
					}
				}

				{
					// func <state>CreateCoroutine(<state>startState: int): void
					auto funcDecl = MakePtr<WfFunctionDeclaration>();
					funcDecl->name.value = smInfo->createCoroutineMethod->GetName();
					funcDecl->returnType = GetTypeFromTypeInfo(TypeInfoRetriver<void>::CreateTypeInfo().Obj());
					{
						auto parameterInfo = smInfo->createCoroutineMethod->GetParameter(0);

						auto funcArgument = MakePtr<WfFunctionArgument>();
						funcArgument->name.value = parameterInfo->GetName();
						funcArgument->type = GetTypeFromTypeInfo(parameterInfo->GetType());
						funcDecl->arguments.Add(funcArgument);
					}

					auto classMember = MakePtr<WfClassMember>();
					classMember->kind = WfClassMemberKind::Normal;
					funcDecl->classMember = classMember;

					auto att = MakePtr<WfAttribute>();
					att->category.value = L"cpp";
					att->name.value = L"Private";
					funcDecl->attributes.Add(att);

					node->expandedDeclarations.Add(funcDecl);
					manager->declarationMemberInfos.Add(funcDecl, smInfo->createCoroutineMethod);

					auto block = MakePtr<WfBlockStatement>();
					funcDecl->statement = block;

					{
						// <state>stateMachineObject = this;
						auto varDecl = MakePtr<WfVariableDeclaration>();
						varDecl->name.value = L"<state>stateMachineObject";
						varDecl->expression = MakePtr<WfThisExpression>();

						auto stat = MakePtr<WfVariableStatement>();
						stat->variable = varDecl;

						block->statements.Add(stat);
					}
					{
						// <state>previousCoroutine = stateMachineCoroutine;
						auto refThis = MakePtr<WfReferenceExpression>();
						refThis->name.value = L"<state>stateMachineObject";

						auto refCoroutine = MakePtr<WfMemberExpression>();
						refCoroutine->parent = refThis;
						refCoroutine->name.value = L"stateMachineCoroutine";

						auto varDecl = MakePtr<WfVariableDeclaration>();
						varDecl->name.value = L"<state>previousCoroutine";
						varDecl->expression = refCoroutine;

						auto stat = MakePtr<WfVariableStatement>();
						stat->variable = varDecl;

						block->statements.Add(stat);
					}
					{
						// stateMachineCoroutine = $coroutine{ ... }
						auto refThis = MakePtr<WfReferenceExpression>();
						refThis->name.value = L"<state>stateMachineObject";

						auto refCoroutine = MakePtr<WfMemberExpression>();
						refCoroutine->parent = refThis;
						refCoroutine->name.value = L"stateMachineCoroutine";

						auto coroutine = MakePtr<WfNewCoroutineExpression>();
						auto coroutineBlock = MakePtr<WfBlockStatement>();
						coroutine->statement = coroutineBlock;

						auto assignExpr = MakePtr<WfBinaryExpression>();
						assignExpr->op = WfBinaryOperator::Assign;
						assignExpr->first = refCoroutine;
						assignExpr->second = coroutine;

						auto exprStat = MakePtr<WfExpressionStatement>();
						exprStat->expression = assignExpr;
						block->statements.Add(exprStat);

						{
							auto tryStat = MakePtr<WfTryStatement>();
							coroutineBlock->statements.Add(tryStat);

							auto tryBlock = MakePtr<WfBlockStatement>();
							tryBlock->endLabel.value = L"<state-label>OUT_OF_STATE_MACHINE";
							tryStat->protectedStatement = tryBlock;

							auto finallyBlock = MakePtr<WfBlockStatement>();
							tryStat->finallyStatement = finallyBlock;

							// try {... <state-label>OUT_OF_STATE_MACHINE:;}
							{
								// var <state>State = <state>startState;
								auto refStartState = MakePtr<WfReferenceExpression>();
								refStartState->name.value = L"<state>startState";

								auto varDecl = MakePtr<WfVariableDeclaration>();
								varDecl->name.value = L"<state>state";
								varDecl->expression = refStartState;

								auto stat = MakePtr<WfVariableStatement>();
								stat->variable = varDecl;

								tryBlock->statements.Add(stat);
							}
							{
								// while(true){... <state-label>OUT_OF_CURRENT_STATE:;}
								auto whileBlock = MakePtr<WfBlockStatement>();
								whileBlock->endLabel.value = L"<state-label>OUT_OF_CURRENT_STATE";
								{
									auto refTrue = MakePtr<WfLiteralExpression>();
									refTrue->value = WfLiteralValue::True;

									auto whileStat = MakePtr<WfWhileStatement>();
									whileStat->condition = refTrue;
									whileStat->statement = whileBlock;

									tryBlock->statements.Add(whileStat);
								}
								{
									// var <state>currentState = <state>state;
									auto refStartState = MakePtr<WfReferenceExpression>();
									refStartState->name.value = L"<state>state";

									auto varDecl = MakePtr<WfVariableDeclaration>();
									varDecl->name.value = L"<state>currentState";
									varDecl->expression = refStartState;

									auto stat = MakePtr<WfVariableStatement>();
									stat->variable = varDecl;

									whileBlock->statements.Add(stat);
								}
								{
									// <state>state = -1;
									auto refState = MakePtr<WfReferenceExpression>();
									refState->name.value = L"<state>state";

									auto refOne = MakePtr<WfIntegerExpression>();
									refOne->value.value = L"1";

									auto refInvalid = MakePtr<WfUnaryExpression>();
									refInvalid->op = WfUnaryOperator::Negative;
									refInvalid->operand = refOne;

									auto assignExpr = MakePtr<WfBinaryExpression>();
									assignExpr->op = WfBinaryOperator::Assign;
									assignExpr->first = refState;
									assignExpr->second = refInvalid;

									auto exprStat = MakePtr<WfExpressionStatement>();
									exprStat->expression = assignExpr;
									whileBlock->statements.Add(exprStat);
								}
								{
									// switch(<state>currentState) { case STATE:{... goto <state-label>OUT_OF_STATE_MACHINE;} ... }
									auto refCurrentState = MakePtr<WfReferenceExpression>();
									refCurrentState->name.value = L"<state>currentState";

									auto switchStat = MakePtr<WfSwitchStatement>();
									switchStat->expression = refCurrentState;
									whileBlock->statements.Add(switchStat);

									FOREACH(Ptr<WfStateDeclaration>, state, node->states)
									{
										auto switchCase = MakePtr<WfSwitchCase>();
										switchStat->caseBranches.Add(switchCase);

										auto refStateId = MakePtr<WfIntegerExpression>();
										refStateId->value.value = itow(smInfo->stateIds[state->name.value]);
										switchCase->expression = refStateId;

										auto caseBlock = MakePtr<WfBlockStatement>();
										switchCase->statement = caseBlock;

										FOREACH(Ptr<WfFunctionArgument>, argument, state->arguments)
										{
											auto refThis = MakePtr<WfReferenceExpression>();
											refThis->name.value = L"<state>stateMachineObject";

											auto refArgument = MakePtr<WfMemberExpression>();
											refArgument->parent = refThis;
											refArgument->name.value = manager->stateDeclArguments[argument.Obj()]->GetName();

											auto varDecl = MakePtr<WfVariableDeclaration>();
											varDecl->name.value = argument->name.value;
											varDecl->expression = refArgument;

											auto varStat = MakePtr<WfVariableStatement>();
											varStat->variable = varDecl;
											caseBlock->statements.Add(varStat);
										}
										caseBlock->statements.Add(ExpandStateMachineStatementVisitor(manager, smInfo.Obj()).CreateField(state->statement));
										{
											auto gotoStat = MakePtr<WfGotoStatement>();
											gotoStat->label.value = L"<state-label>OUT_OF_STATE_MACHINE";
											caseBlock->statements.Add(gotoStat);
										}
									}
								}
							}
							// finally {stateMachineCoroutine = <state>previousCoroutine;}
							{
								auto refThis = MakePtr<WfReferenceExpression>();
								refThis->name.value = L"<state>stateMachineObject";

								auto refCoroutine = MakePtr<WfMemberExpression>();
								refCoroutine->parent = refThis;
								refCoroutine->name.value = L"stateMachineCoroutine";

								auto refPrevious = MakePtr<WfReferenceExpression>();
								refPrevious->name.value = L"<state>previousCoroutine";

								auto assignExpr = MakePtr<WfBinaryExpression>();
								assignExpr->op = WfBinaryOperator::Assign;
								assignExpr->first = refCoroutine;
								assignExpr->second = refPrevious;

								auto exprStat = MakePtr<WfExpressionStatement>();
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