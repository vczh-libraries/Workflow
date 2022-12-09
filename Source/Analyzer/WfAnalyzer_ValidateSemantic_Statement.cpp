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
			using namespace typeimpl;

/***********************************************************************
ValidateSemantic(Statement)
***********************************************************************/

			class ExpandVirtualStatementVisitor : public Object, public WfVirtualCseStatement::IVisitor
			{
			public:
				WfLexicalScopeManager*				manager;

				ExpandVirtualStatementVisitor(WfLexicalScopeManager* _manager)
					:manager(_manager)
				{
				}

				void Visit(WfSwitchStatement* node)override
				{
					ExpandSwitchStatement(manager, node);
				}

				void Visit(WfForEachStatement* node)override
				{
					ExpandForEachStatement(manager, node);
				}

				void Visit(WfCoProviderStatement* node)override
				{
					ExpandCoProviderStatement(manager, node);
				}
			};

			class ValidateSemanticStatementVisitor
				: public Object
				, public WfStatement::IVisitor
				, public WfVirtualCseStatement::IVisitor
				, public WfCoroutineStatement::IVisitor
				, public WfStateMachineStatement::IVisitor
			{
			public:
				WfLexicalScopeManager*				manager;

				ValidateSemanticStatementVisitor(WfLexicalScopeManager* _manager)
					:manager(_manager)
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
					auto scope = manager->nodeScopes[node].Obj();
					auto functionScope = scope->FindFunctionScope();
					if (auto funcDecl = functionScope->ownerNode.Cast<WfFunctionDeclaration>())
					{
						auto providerStat = funcDecl->statement.Cast<WfCoProviderStatement>();
						if (providerStat && !providerStat->expandedStatement)
						{
							auto providerScope = manager->nodeScopes[funcDecl->statement.Obj()];
							auto providerSymbol = providerScope->symbols[L"$PROVIDER"][0];
							auto implSymbol = providerScope->symbols[L"$IMPL"][0];
							if (providerSymbol->typeInfo && implSymbol->typeInfo)
							{
								if (auto group = providerSymbol->typeInfo->GetTypeDescriptor()->GetMethodGroupByName(L"ReturnAndExit", true))
								{
									List<ResolveExpressionResult> functions;
									vint count = group->GetMethodCount();
									for (vint i = 0; i < count; i++)
									{
										auto method = group->GetMethod(i);
										if (method->IsStatic())
										{
											if (method->GetParameterCount() > 0 && IsSameType(implSymbol->typeInfo.Obj(), method->GetParameter(0)->GetType()))
											{
												functions.Add(ResolveExpressionResult::Method(method));
											}
										}
									}

									if (functions.Count() != 0)
									{
										vint selectedFunctionIndex = -1;
										List<Ptr<WfExpression>> arguments;
										arguments.Add(nullptr);
										if (node->expression)
										{
											arguments.Add(node->expression);
										}
										SelectFunction(manager, node, nullptr, functions, arguments, selectedFunctionIndex);
										if (selectedFunctionIndex != -1)
										{
											manager->coOperatorResolvings.Add(node, functions[selectedFunctionIndex]);
										}
									}
									else
									{
										manager->errors.Add(WfErrors::CoOperatorNotExists(node, providerSymbol->typeInfo.Obj()));
									}
								}
								else
								{
									manager->errors.Add(WfErrors::CoOperatorNotExists(node, providerSymbol->typeInfo.Obj()));
								}
							}
						}
						else
						{
							auto returnType = CreateTypeInfoFromType(scope, funcDecl->returnType);
							if (node->expression)
							{
								if (returnType->GetTypeDescriptor() == description::GetTypeDescriptor<void>())
								{
									manager->errors.Add(WfErrors::CannotReturnExpression(node));
								}
								else
								{
									GetExpressionType(manager, node->expression, returnType);
								}
							}
							else if (returnType->GetDecorator() != ITypeInfo::TypeDescriptor || returnType->GetTypeDescriptor() != description::GetTypeDescriptor<void>())
							{
								manager->errors.Add(WfErrors::ReturnMissExpression(node, returnType.Obj()));
							}
						}
					}
					else
					{
						if (node->expression)
						{
							manager->errors.Add(WfErrors::CannotReturnExpression(node));
						}
					}
				}

				void Visit(WfDeleteStatement* node)override
				{
					Ptr<ITypeInfo> type = GetExpressionType(manager, node->expression, 0);
					if (type)
					{
						if (type->GetDecorator() != ITypeInfo::RawPtr)
						{
							manager->errors.Add(WfErrors::DeleteNonRawPointer(node, type.Obj()));
						}
					}
				}

				void Visit(WfRaiseExceptionStatement* node)override
				{
					if (node->expression)
					{
						if (auto type = GetExpressionType(manager, node->expression, nullptr))
						{
							auto stringType = TypeInfoRetriver<WString>::CreateTypeInfo();
							auto exceptionType = TypeInfoRetriver<Ptr<IValueException>>::CreateTypeInfo();
							if (!CanConvertToType(type.Obj(), stringType.Obj(), false) && !CanConvertToType(type.Obj(), exceptionType.Obj(), false))
							{
								manager->errors.Add(WfErrors::ExpressionCannotImplicitlyConvertToType(node->expression.Obj(), type.Obj(), stringType.Obj()));
							}
						}
					}
				}

				void Visit(WfIfStatement* node)override
				{
					if (node->type)
					{
						auto scope = manager->nodeScopes[node].Obj();
						auto symbol = scope->symbols[node->name.value][0];
						if (!IsNullAcceptableType(symbol->typeInfo.Obj()))
						{
							manager->errors.Add(WfErrors::NullCannotImplicitlyConvertToType(node->expression.Obj(), symbol->typeInfo.Obj()));
						}
						GetExpressionType(manager, node->expression, nullptr);
					}
					else
					{
						Ptr<ITypeInfo> boolType = TypeInfoRetriver<bool>::CreateTypeInfo();
						GetExpressionType(manager, node->expression, boolType);
					}
					ValidateStatementSemantic(manager, node->trueBranch);
					if (node->falseBranch)
					{
						ValidateStatementSemantic(manager, node->falseBranch);
					}
				}

				void Visit(WfWhileStatement* node)override
				{
					Ptr<ITypeInfo> boolType = TypeInfoRetriver<bool>::CreateTypeInfo();
					GetExpressionType(manager, node->condition, boolType);
					ValidateStatementSemantic(manager, node->statement);
				}

				void Visit(WfTryStatement* node)override
				{
					ValidateStatementSemantic(manager, node->protectedStatement);
					if (node->catchStatement)
					{
						ValidateStatementSemantic(manager, node->catchStatement);
					}
					if (node->finallyStatement)
					{
						ValidateStatementSemantic(manager, node->finallyStatement);
					}
				}

				void Visit(WfBlockStatement* node)override
				{
					for (auto statement : node->statements)
					{
						ValidateStatementSemantic(manager, statement);
					}
				}

				void Visit(WfGotoStatement* node)override
				{
					auto scope = manager->nodeScopes[node].Obj();
					vint counter = 0;
					while (scope && !scope->functionConfig)
					{
						if (auto block = scope->ownerNode.Cast<WfBlockStatement>())
						{
							if (block->endLabel.value == node->label.value)
							{
								counter++;
							}
						}
						scope = scope->parentScope.Obj();
					}

					if (counter == 0)
					{
						manager->errors.Add(WfErrors::GotoLabelNotExists(node));
					}
					else if (counter > 1)
					{
						manager->errors.Add(WfErrors::TooManyGotoLabel(node));
					}
				}

				void Visit(WfExpressionStatement* node)override
				{
					GetExpressionType(manager, node->expression, 0);
				}

				void Visit(WfVariableStatement* node)override
				{
					ValidateDeclarationSemantic(manager, node->variable);
				}

				void Visit(WfVirtualCseStatement* node)override
				{
					bool expanded = node->expandedStatement;
					vint errorCount = manager->errors.Count();
					node->Accept((WfVirtualCseStatement::IVisitor*)this);

					if (!expanded && manager->errors.Count() == errorCount)
					{
						ExpandVirtualStatementVisitor visitor(manager);
						node->Accept(&visitor);
						SetCodeRange(node->expandedStatement, node->codeRange);

						auto parentScope = manager->nodeScopes[node];
						if (parentScope->ownerNode == node)
						{
							parentScope = parentScope->parentScope;
						}

						ContextFreeStatementDesugar(manager, node->expandedStatement);
						BuildScopeForStatement(manager, parentScope, node->expandedStatement);

						manager->checkedScopes_DuplicatedSymbol.Remove(parentScope.Obj());
						manager->checkedScopes_SymbolType.Remove(parentScope.Obj());
						if (!CheckScopes_DuplicatedSymbol(manager) || !CheckScopes_SymbolType(manager))
						{
							return;
						}
					}

					if (node->expandedStatement)
					{
						ValidateStatementSemantic(manager, node->expandedStatement);
					}
				}

				void Visit(WfSwitchStatement* node)override
				{
					Ptr<ITypeInfo> type = GetExpressionType(manager, node->expression, 0);
					for (auto switchCase : node->caseBranches)
					{
						Ptr<ITypeInfo> caseType;
						if (IsExpressionDependOnExpectedType(manager, switchCase->expression, type))
						{
							caseType = GetExpressionType(manager, switchCase->expression, type);
						}
						else
						{
							caseType = GetExpressionType(manager, switchCase->expression, 0);
						}

						if (type && caseType)
						{
							if (!GetMergedType(type, caseType))
							{
								manager->errors.Add(WfErrors::CannotMergeTwoType(switchCase->expression.Obj(), type.Obj(), caseType.Obj()));
							}
						}
						ValidateStatementSemantic(manager, switchCase->statement);
					}
					if (node->defaultBranch)
					{
						ValidateStatementSemantic(manager, node->defaultBranch);
					}
				}

				void Visit(WfForEachStatement* node)override
				{
					Ptr<ITypeInfo> elementType = GetEnumerableExpressionItemType(manager, node->collection, 0);
					if (elementType)
					{
						auto scope = manager->nodeScopes[node].Obj();
						auto symbol = scope->symbols[node->name.value][0];
						symbol->typeInfo = elementType;
						symbol->type = GetTypeFromTypeInfo(elementType.Obj());
					}
					ValidateStatementSemantic(manager, node->statement);
				}

				void Visit(WfCoProviderStatement* node)override
				{
					auto scope = manager->nodeScopes[node].Obj();
					auto providerSymbol = scope->symbols[L"$PROVIDER"][0];
					auto implSymbol = scope->symbols[L"$IMPL"][0];
					Ptr<ITypeInfo> funcReturnType;
					{
						auto decl = scope->parentScope->ownerNode.Cast<WfFunctionDeclaration>();
						funcReturnType = CreateTypeInfoFromType(scope->parentScope.Obj(), decl->returnType);
					}
					ITypeDescriptor* selectedProviderTd = nullptr;
					List<WString> candidates;

					if (node->name.value == L"")
					{
						if (funcReturnType)
						{
							List<ITypeDescriptor*> unprocessed;
							unprocessed.Add(funcReturnType->GetTypeDescriptor());

							for (vint i = 0; i < unprocessed.Count(); i++)
							{
								auto td = unprocessed[i];
								auto candidate = td->GetTypeName() + L"Coroutine";
								if ((selectedProviderTd = description::GetTypeDescriptor(candidate)))
								{
									break;
								}
								else
								{
									candidates.Add(candidate);
								}

								vint count = td->GetBaseTypeDescriptorCount();
								for (vint i = 0; i < count; i++)
								{
									auto baseTd = td->GetBaseTypeDescriptor(i);
									if (!unprocessed.Contains(baseTd))
									{
										unprocessed.Add(baseTd);
									}
								}
							}
						}
					}
					else
					{
						List<ResolveExpressionResult> results, resolveResults;
						auto providerName = node->name.value.Right(node->name.value.Length() - 1);

						if (manager->ResolveName(scope, providerName, resolveResults))
						{
							CopyFrom(results, resolveResults);

							for (vint i = results.Count() - 1; i >= 0; i--)
							{
								auto& result = results[i];
								ITypeDescriptor* providerTd = nullptr;

								if (result.scopeName && result.scopeName->typeDescriptor)
								{
									auto candidate = result.scopeName->typeDescriptor->GetTypeName() + L"Coroutine";
									providerTd = description::GetTypeDescriptor(candidate);
									if (providerTd)
									{
										selectedProviderTd = providerTd;
									}
									else
									{
										candidates.Add(candidate);
									}
								}

								if (!providerTd)
								{
									results.RemoveAt(i);
								}
							}

							if (results.Count() == 1)
							{
								goto FINISH_SEARCHING;
							}
							else if (results.Count() > 1)
							{
								manager->errors.Add(WfErrors::TooManyTargets(node, resolveResults, providerName));
								goto SKIP_SEARCHING;
							}
						}

						resolveResults.Clear();
						if (manager->ResolveName(scope, providerName + L"Coroutine", resolveResults))
						{
							CopyFrom(results, resolveResults);

							for (vint i = results.Count() - 1; i >= 0; i--)
							{
								auto& result = results[i];

								if (result.scopeName && result.scopeName->typeDescriptor)
								{
									selectedProviderTd = result.scopeName->typeDescriptor;
								}
								else
								{
									results.RemoveAt(i);
								}
							}

							if (results.Count() == 1)
							{
								goto FINISH_SEARCHING;
							}
							else if (results.Count() > 1)
							{
								manager->errors.Add(WfErrors::TooManyTargets(node, resolveResults, providerName));
								goto SKIP_SEARCHING;
							}
						}

						candidates.Add(providerName);
						candidates.Add(providerName + L"Coroutine");
					}

				FINISH_SEARCHING:
					if (selectedProviderTd)
					{
						providerSymbol->typeInfo = MakePtr<TypeDescriptorTypeInfo>(selectedProviderTd, TypeInfoHint::Normal);

						if (funcReturnType)
						{
							WString creatorName;
							if (funcReturnType->GetTypeDescriptor() == description::GetTypeDescriptor<void>())
							{
								creatorName = L"CreateAndRun";
							}
							else
							{
								creatorName = L"Create";
							}

							if (auto group = selectedProviderTd->GetMethodGroupByName(creatorName, true))
							{
								List<ResolveExpressionResult> results;
								ITypeInfo* selectedImplType = nullptr;
								IMethodInfo* selectedCreator = nullptr;
								vint count = group->GetMethodCount();

								for (vint i = 0; i < count; i++)
								{
									auto method = group->GetMethod(i);
									if (method->IsStatic())
									{
										if (method->GetParameterCount() == 1)
										{
											auto creatorType = method->GetParameter(0)->GetType();
											if (creatorType->GetDecorator() == ITypeInfo::SharedPtr)
											{
												auto functionType = creatorType->GetElementType();
												if (functionType->GetDecorator() == ITypeInfo::Generic &&
													functionType->GetGenericArgumentCount() == 2 &&
													functionType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueFunctionProxy>()
													)
												{
													auto returnType = functionType->GetGenericArgument(0);
													if (returnType->GetDecorator() == ITypeInfo::SharedPtr &&returnType->GetTypeDescriptor() == description::GetTypeDescriptor<ICoroutine>())
													{
														selectedImplType = functionType->GetGenericArgument(1);
														selectedCreator = method;
														results.Add(ResolveExpressionResult::Method(method));
													}
												}
											}
										}
									}
								}

								if (results.Count() == 1)
								{
									implSymbol->typeInfo = CopyTypeInfo(selectedImplType);
									manager->coProviderResolvings.Add(node, ResolveExpressionResult::Method(selectedCreator));
								}
								else if (results.Count() > 1)
								{
									manager->errors.Add(WfErrors::TooManyTargets(node, results, creatorName));
								}
							}

							if (!implSymbol->typeInfo)
							{
								if (funcReturnType->GetTypeDescriptor() == description::GetTypeDescriptor<void>())
								{
									manager->errors.Add(WfErrors::CoProviderCreateAndRunNotExists(node, providerSymbol->typeInfo.Obj()));
								}
								else
								{
									manager->errors.Add(WfErrors::CoProviderCreateNotExists(node, providerSymbol->typeInfo.Obj()));
								}
							}
						}
					}
					else
					{
						manager->errors.Add(WfErrors::CoProviderNotExists(node, candidates));
					}
				SKIP_SEARCHING:
					ValidateStatementSemantic(manager, node->statement);
				}

				void Visit(WfCoroutineStatement* node)override
				{
					node->Accept((WfCoroutineStatement::IVisitor*)this);
				}

				void Visit(WfCoPauseStatement* node)override
				{
					if (node->statement)
					{
						ValidateStatementSemantic(manager, node->statement);
					}
				}

				void Visit(WfCoOperatorStatement* node)override
				{
					auto scope = manager->nodeScopes[node].Obj();
					auto functionScope = scope->FindFunctionScope();
					if (auto funcDecl = functionScope->ownerNode.Cast<WfFunctionDeclaration>())
					{
						if (funcDecl->statement.Cast<WfCoProviderStatement>())
						{
							auto providerScope = manager->nodeScopes[funcDecl->statement.Obj()];
							auto providerSymbol = providerScope->symbols[L"$PROVIDER"][0];
							auto implSymbol = providerScope->symbols[L"$IMPL"][0];
							if (providerSymbol->typeInfo && implSymbol->typeInfo)
							{
								List<IMethodGroupInfo*> groups;
								auto operatorName = node->opName.value.Right(node->opName.value.Length() - 1);
								if (auto group = providerSymbol->typeInfo->GetTypeDescriptor()->GetMethodGroupByName(operatorName + L"AndRead", true))
								{
									groups.Add(group);
								}
								if (node->varName.value == L"")
								{
									if (auto group = providerSymbol->typeInfo->GetTypeDescriptor()->GetMethodGroupByName(operatorName + L"AndPause", true))
									{
										groups.Add(group);
									}
								}

								List<ResolveExpressionResult> functions;
								for (auto group : groups)
								{
									vint count = group->GetMethodCount();
									for (vint i = 0; i < count; i++)
									{
										auto method = group->GetMethod(i);
										if (method->IsStatic())
										{
											if (method->GetParameterCount() > 0 && IsSameType(implSymbol->typeInfo.Obj(), method->GetParameter(0)->GetType()))
											{
												functions.Add(ResolveExpressionResult::Method(method));
											}
										}
									}
								}

								if (functions.Count() == 0)
								{
									manager->errors.Add(WfErrors::CoOperatorNotExists(node, providerSymbol->typeInfo.Obj()));
								}
								else
								{
									vint selectedFunctionIndex = -1;
									vint oldErrorCount = manager->errors.Count();
									List<Ptr<WfExpression>> arguments;
									arguments.Add(nullptr);
									CopyFrom(arguments, node->arguments, true);
									SelectFunction(manager, node, nullptr, functions, arguments, selectedFunctionIndex);
									if (selectedFunctionIndex != -1)
									{
										manager->coOperatorResolvings.Add(node, functions[selectedFunctionIndex]);
										if (node->varName.value != L"" && manager->errors.Count() == oldErrorCount)
										{
											auto symbol = scope->symbols[node->varName.value][0];
											List<ITypeInfo*> types;

											for (auto argument : node->arguments)
											{
												vint index = manager->expressionResolvings.Keys().IndexOf(argument.Obj());
												if (index != -1)
												{
													auto type = manager->expressionResolvings.Values()[index].type;
													if (!types.Contains(type.Obj()))
													{
														types.Add(type.Obj());
														if (auto group = type->GetTypeDescriptor()->GetMethodGroupByName(L"CastResult", true))
														{
															vint count = group->GetMethodCount();
															for (vint i = 0; i < count; i++)
															{
																auto method = group->GetMethod(i);
																if (method->IsStatic())
																{
																	if (method->GetParameterCount() == 1 &&
																		method->GetParameter(0)->GetType()->GetTypeDescriptor() == description::GetTypeDescriptor<Value>() &&
																		method->GetReturn()->GetTypeDescriptor() != description::GetTypeDescriptor<void>()
																		)
																	{
																		manager->coCastResultResolvings.Add(node, ResolveExpressionResult::Method(method));
																		symbol->typeInfo = CopyTypeInfo(method->GetReturn());
																		break;
																	}
																}
															}
														}
													}
												}
											}

											if (!symbol->typeInfo)
											{
												manager->errors.Add(WfErrors::CoOperatorCannotResolveResultType(node, types));
											}
										}
									}
								}
							}
						}
					}
				}

				void Visit(WfStateMachineStatement* node)override
				{
					node->Accept((WfStateMachineStatement::IVisitor*)this);
				}

				void Visit(WfStateSwitchStatement* node)override
				{
					auto smcScope = manager->nodeScopes[node]->FindFunctionScope()->parentScope.Obj();
					CHECK_ERROR(smcScope->ownerNode.Cast<WfClassDeclaration>(), L"ValidateSemanticStatementVisitor::Visit(WfStateSwitchStatement*)#ValidateStatementStructure should check state machine statements' location.");

					for (auto switchCase : node->caseBranches)
					{
						auto caseScope = manager->nodeScopes[switchCase.Obj()].Obj();
						Ptr<WfLexicalSymbol> inputSymbol;
						{
							vint index = smcScope->symbols.Keys().IndexOf(switchCase->name.value);
							if (index != -1)
							{
								inputSymbol = smcScope->symbols.GetByIndex(index)[0];
							}
						}
						if (!inputSymbol || !inputSymbol->creatorNode.Cast<WfStateInput>())
						{
							manager->errors.Add(WfErrors::StateInputNotExists(switchCase.Obj()));
						}
						else
						{
							auto td = manager->declarationTypes[smcScope->ownerNode.Cast<WfClassDeclaration>().Obj()].Obj();
							auto inputMethod = td->GetMethodGroupByName(switchCase->name.value, false)->GetMethod(0);
							if (switchCase->arguments.Count() != inputMethod->GetParameterCount())
							{
								manager->errors.Add(WfErrors::StateSwitchArgumentCountNotMatch(switchCase.Obj()));
							}
							else
							{
								for (auto [argument, index] : indexed(switchCase->arguments))
								{
									auto argumentSymbol = caseScope->symbols[argument->name.value][0];
									argumentSymbol->typeInfo = CopyTypeInfo(inputMethod->GetParameter(index)->GetType());
									argumentSymbol->type = GetTypeFromTypeInfo(argumentSymbol->typeInfo.Obj());
								}
							}
						}

						ValidateStatementSemantic(manager, switchCase->statement);
					}
				}

				void Visit(WfStateInvokeStatement* node)override
				{
					auto smcScope = manager->nodeScopes[node]->FindFunctionScope()->parentScope.Obj();
					CHECK_ERROR(smcScope->ownerNode.Cast<WfClassDeclaration>(), L"ValidateSemanticStatementVisitor::Visit(WfStateSwitchStatement*)#ValidateStatementStructure should check state machine statements' location.");

					Ptr<WfLexicalSymbol> stateSymbol;
					{
						vint index = smcScope->symbols.Keys().IndexOf(node->name.value);
						if (index != -1)
						{
							stateSymbol = smcScope->symbols.GetByIndex(index)[0];
						}
					}
					if (!stateSymbol || !stateSymbol->creatorNode.Cast<WfStateDeclaration>())
					{
						manager->errors.Add(WfErrors::StateNotExists(node));
					}
					else
					{
						auto stateDecl = stateSymbol->creatorNode.Cast<WfStateDeclaration>();
						if (stateDecl->arguments.Count() != node->arguments.Count())
						{
							manager->errors.Add(WfErrors::StateArgumentCountNotMatch(node));
						}
						else
						{
							auto stateScope = manager->nodeScopes[stateDecl.Obj()];
							for (auto [argument, index] : indexed(node->arguments))
							{
								auto typeInfo = stateScope->symbols[stateDecl->arguments[index]->name.value][0]->typeInfo;
								GetExpressionType(manager, argument, typeInfo);
							}
						}
					}
				}

				static void Execute(WfStatement* statement, WfLexicalScopeManager* manager)
				{
					ValidateSemanticStatementVisitor visitor(manager);
					statement->Accept(&visitor);
				}
			};

/***********************************************************************
ValidateSemantic
***********************************************************************/

			void ValidateStatementSemantic(WfLexicalScopeManager* manager, WfStatement* statement)
			{
				return ValidateSemanticStatementVisitor::Execute(statement, manager);
			}

			void ValidateStatementSemantic(WfLexicalScopeManager* manager, Ptr<WfStatement> statement)
			{
				return ValidateSemanticStatementVisitor::Execute(statement.Obj(), manager);
			}
		}
	}
}
