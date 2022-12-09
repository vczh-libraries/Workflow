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
ExpandSwitchStatement
***********************************************************************/

			void ExpandSwitchStatement(WfLexicalScopeManager* manager, WfSwitchStatement* node)
			{
				auto block = Ptr(new WfBlockStatement);
				node->expandedStatement = block;
				auto varName = L"<switch>" + itow(manager->usedTempVars++);

				{
					auto result = manager->expressionResolvings[node->expression.Obj()];
					auto decl = Ptr(new WfVariableDeclaration);
					decl->name.value = varName;
					decl->type = GetTypeFromTypeInfo(result.type.Obj());
					decl->expression = CopyExpression(node->expression, true);

					auto stat = Ptr(new WfVariableStatement);
					stat->variable = decl;
					block->statements.Add(stat);
				}

				Ptr<WfStatement> rootIfStat;
				auto tailIfStat = &rootIfStat;

				for (auto switchCase : node->caseBranches)
				{
					auto ifStat = Ptr(new WfIfStatement);
					*tailIfStat = ifStat;
					tailIfStat = &ifStat->falseBranch;

					{
						auto refExpr = Ptr(new WfReferenceExpression);
						refExpr->name.value = varName;

						auto inferExpr = Ptr(new WfInferExpression);
						inferExpr->expression= CopyExpression(switchCase->expression, true);
						{
							auto result = manager->expressionResolvings[switchCase->expression.Obj()];
							inferExpr->type = GetTypeFromTypeInfo(result.type.Obj());
						}

						auto compare = Ptr(new WfBinaryExpression);
						compare->first = refExpr;
						compare->second = inferExpr;
						compare->op = WfBinaryOperator::EQ;

						ifStat->expression = compare;
					}
					ifStat->trueBranch = CopyStatement(switchCase->statement, true);
				}

				if (node->defaultBranch)
				{
					*tailIfStat = CopyStatement(node->defaultBranch, true);
				}

				if (rootIfStat)
				{
					block->statements.Add(rootIfStat);
				}
			}

/***********************************************************************
ExpandForEachStatement
***********************************************************************/

			Ptr<WfStatement> GenerateForEachStepStatement(WfForEachStatement* node)
			{
				auto refVar1 = Ptr(new WfReferenceExpression);
				refVar1->name.value = node->name.value;

				auto refVar2 = Ptr(new WfReferenceExpression);
				refVar2->name.value = node->name.value;

				auto one = Ptr(new WfIntegerExpression);
				one->value.value = L"1";

				auto stepExpr = Ptr(new WfBinaryExpression);
				stepExpr->first = refVar2;
				stepExpr->second = one;
				stepExpr->op = node->direction == WfForEachDirection::Normal ? WfBinaryOperator::Add : WfBinaryOperator::Sub;

				auto assignExpr = Ptr(new WfBinaryExpression);
				assignExpr->first = refVar1;
				assignExpr->second = stepExpr;
				assignExpr->op = WfBinaryOperator::Assign;

				auto stat = Ptr(new WfExpressionStatement);
				stat->expression = assignExpr;
				return stat;
			}

			class CopyForEachRangeBodyVisitor : public CopyWithExpandVirtualVisitor
			{
			public:
				WfLexicalScopeManager*						manager;
				WfForEachStatement*							forEach;

				CopyForEachRangeBodyVisitor(WfLexicalScopeManager* _manager, WfForEachStatement* _forEach)
					:CopyWithExpandVirtualVisitor(true)
					, manager(_manager)
					, forEach(_forEach)
				{
				}

				void Visit(WfContinueStatement* node)
				{
					auto scope = manager->nodeScopes[node];
					while (scope)
					{
						if (scope->ownerNode.Cast<WfWhileStatement>())
						{
							break;
						}
						else if (scope->ownerNode.Cast<WfForEachStatement>())
						{
							if (scope->ownerNode != forEach)
							{
								break;
							}
							else
							{
								auto block = Ptr(new WfBlockStatement);
								block->statements.Add(GenerateForEachStepStatement(forEach));
								block->statements.Add(Ptr(new WfContinueStatement));
								SetCodeRange((Ptr<WfStatement>)block, node->codeRange);
								result = block;
								return;
							}
						}
						scope = scope->parentScope;
					}
					copy_visitor::AstVisitor::Visit(node);
				}
			};

			void ExpandForEachStatement(WfLexicalScopeManager* manager, WfForEachStatement* node)
			{
				auto block = Ptr(new WfBlockStatement);
				node->expandedStatement = block;

				if (auto range = node->collection.Cast<WfRangeExpression>())
				{
					auto varBegin = L"<for-begin>" + node->name.value;
					auto varEnd = L"<for-end>" + node->name.value;
					{
						auto result = manager->expressionResolvings[range->begin.Obj()];
						auto decl = Ptr(new WfVariableDeclaration);
						decl->name.value = varBegin;
						decl->type = GetTypeFromTypeInfo(result.type.Obj());
						decl->expression = CopyExpression(range->begin, true);
						if (range->beginBoundary == WfRangeBoundary::Exclusive)
						{
							auto one = Ptr(new WfIntegerExpression);
							one->value.value = L"1";

							auto addExpr = Ptr(new WfBinaryExpression);
							addExpr->first = decl->expression;
							addExpr->second = one;
							addExpr->op = WfBinaryOperator::Add;
							
							decl->expression = addExpr;
						}

						auto stat = Ptr(new WfVariableStatement);
						stat->variable = decl;
						block->statements.Add(stat);
					}
					{
						auto result = manager->expressionResolvings[range->end.Obj()];
						auto decl = Ptr(new WfVariableDeclaration);
						decl->name.value = varEnd;
						decl->type = GetTypeFromTypeInfo(result.type.Obj());
						decl->expression = CopyExpression(range->end, true);
						if (range->endBoundary == WfRangeBoundary::Exclusive)
						{
							auto one = Ptr(new WfIntegerExpression);
							one->value.value = L"1";

							auto subExpr = Ptr(new WfBinaryExpression);
							subExpr->first = decl->expression;
							subExpr->second = one;
							subExpr->op = WfBinaryOperator::Sub;

							decl->expression = subExpr;
						}

						auto stat = Ptr(new WfVariableStatement);
						stat->variable = decl;
						block->statements.Add(stat);
					}
					{
						auto refBegin = Ptr(new WfReferenceExpression);
						refBegin->name.value = node->direction == WfForEachDirection::Normal ? varBegin : varEnd;

						auto decl = Ptr(new WfVariableDeclaration);
						decl->name.value = node->name.value;
						decl->expression = refBegin;

						auto stat = Ptr(new WfVariableStatement);
						stat->variable = decl;
						block->statements.Add(stat);
					}
					{
						auto whileStat = Ptr(new WfWhileStatement);
						{
							auto refVar = Ptr(new WfReferenceExpression);
							refVar->name.value = node->name.value;

							auto refBegin = Ptr(new WfReferenceExpression);
							refBegin->name.value = node->direction == WfForEachDirection::Normal ? varEnd : varBegin;

							auto compare = Ptr(new WfBinaryExpression);
							compare->first = refVar;
							compare->second = refBegin;
							compare->op = node->direction == WfForEachDirection::Normal ? WfBinaryOperator::LE : WfBinaryOperator::GE;

							whileStat->condition = compare;
						}
						{
							auto whileBlock = Ptr(new WfBlockStatement);
							whileStat->statement = whileBlock;

							{
								CopyForEachRangeBodyVisitor visitor(manager, node);
								whileBlock->statements.Add(visitor.CopyNode(node->statement.Obj()));
							}
							whileBlock->statements.Add(GenerateForEachStepStatement(node));
						}
						block->statements.Add(whileStat);
					}
				}
				else
				{
					auto varEnum = L"<for-enumerable>" + node->name.value;
					auto varIter = L"<for-enumerator>" + node->name.value;
					{
						auto decl = Ptr(new WfVariableDeclaration);
						decl->name.value = varEnum;
						if (node->direction == WfForEachDirection::Normal)
						{
							auto inferExpr = Ptr(new WfInferExpression);
							inferExpr->expression = CopyExpression(node->collection, true);
							inferExpr->type = GetTypeFromTypeInfo(TypeInfoRetriver<Ptr<IValueEnumerable>>::CreateTypeInfo().Obj());

							decl->expression = inferExpr;
						}
						else
						{
							auto refMethod = Ptr(new WfChildExpression);
							refMethod->parent = GetExpressionFromTypeDescriptor(description::GetTypeDescriptor<Sys>());
							refMethod->name.value = L"ReverseEnumerable";

							auto refCall = Ptr(new WfCallExpression);
							refCall->function = refMethod;
							refCall->arguments.Add(CopyExpression(node->collection, true));

							decl->expression = refCall;
						}

						auto stat = Ptr(new WfVariableStatement);
						stat->variable = decl;
						block->statements.Add(stat);
					}
					{
						auto refEnum = Ptr(new WfReferenceExpression);
						refEnum->name.value = varEnum;

						auto refMethod = Ptr(new WfMemberExpression);
						refMethod->parent = refEnum;
						refMethod->name.value = L"CreateEnumerator";

						auto callExpr = Ptr(new WfCallExpression);
						callExpr->function = refMethod;

						auto decl = Ptr(new WfVariableDeclaration);
						decl->name.value = varIter;
						decl->expression = callExpr;

						auto stat = Ptr(new WfVariableStatement);
						stat->variable = decl;
						block->statements.Add(stat);
					}
					{
						auto whileStat = Ptr(new WfWhileStatement);
						{
							auto refIter = Ptr(new WfReferenceExpression);
							refIter->name.value = varIter;

							auto refMethod = Ptr(new WfMemberExpression);
							refMethod->parent = refIter;
							refMethod->name.value = L"Next";

							auto callExpr = Ptr(new WfCallExpression);
							callExpr->function = refMethod;

							whileStat->condition = callExpr;
						}
						{
							auto whileBlock = Ptr(new WfBlockStatement);
							whileStat->statement = whileBlock;

							{
								auto refIter = Ptr(new WfReferenceExpression);
								refIter->name.value = varIter;

								auto refMethod = Ptr(new WfMemberExpression);
								refMethod->parent = refIter;
								refMethod->name.value = L"GetCurrent";

								auto callExpr = Ptr(new WfCallExpression);
								callExpr->function = refMethod;

								auto castExpr = Ptr(new WfTypeCastingExpression);
								castExpr->expression = callExpr;
								castExpr->strategy = WfTypeCastingStrategy::Strong;
								{
									auto parentScope = manager->nodeScopes[node];
									auto symbol = parentScope->symbols[node->name.value][0];
									castExpr->type = GetTypeFromTypeInfo(symbol->typeInfo.Obj());
								}

								auto decl = Ptr(new WfVariableDeclaration);
								decl->name.value = node->name.value;
								decl->expression = castExpr;

								auto stat = Ptr(new WfVariableStatement);
								stat->variable = decl;
								whileBlock->statements.Add(stat);
							}
							whileBlock->statements.Add(CopyStatement(node->statement, true));
						}
						block->statements.Add(whileStat);
					}
				}
			}

/***********************************************************************
ExpandCoProviderStatement
***********************************************************************/

			class ExpandCoProviderStatementVisitor : public CopyWithExpandVirtualVisitor
			{
			public:
				WfLexicalScopeManager*						manager;

				ExpandCoProviderStatementVisitor(WfLexicalScopeManager* _manager)
					:CopyWithExpandVirtualVisitor(true)
					, manager(_manager)
				{
				}

				void Visit(WfReturnStatement* node)override
				{
					auto opInfo = manager->coOperatorResolvings[node].methodInfo;
					auto block = Ptr(new WfBlockStatement);

					{
						auto refImpl = Ptr(new WfReferenceExpression);
						refImpl->name.value = L"<co-impl>";

						auto funcExpr = Ptr(new WfChildExpression);
						funcExpr->parent = GetExpressionFromTypeDescriptor(opInfo->GetOwnerTypeDescriptor());
						funcExpr->name.value = opInfo->GetName();

						auto callExpr = Ptr(new WfCallExpression);
						callExpr->function = funcExpr;
						callExpr->arguments.Add(refImpl);
						if (node->expression)
						{
							auto returnValue = CopyNode(node->expression.Obj());
							{
								auto scope = manager->nodeScopes[node].Obj();
								auto functionScope = scope->FindFunctionScope();
								if (auto funcDecl = functionScope->ownerNode.Cast<WfFunctionDeclaration>())
								{
									auto returnType = CreateTypeInfoFromType(scope, funcDecl->returnType);
									if (auto group = returnType->GetTypeDescriptor()->GetMethodGroupByName(L"StoreResult", true))
									{
										vint count = group->GetMethodCount();
										for (vint i = 0; i < count; i++)
										{
											auto method = group->GetMethod(i);
											if (method->IsStatic() && method->GetParameterCount() == 1)
											{
												auto refType = GetExpressionFromTypeDescriptor(returnType->GetTypeDescriptor());

												auto refStoreResult = Ptr(new WfChildExpression);
												refStoreResult->parent = refType;
												refStoreResult->name.value = L"StoreResult";

												auto callExpr = Ptr(new WfCallExpression);
												callExpr->function = refStoreResult;
												callExpr->arguments.Add(returnValue);

												returnValue = callExpr;
												break;
											}
										}
									}
								}
							}
							callExpr->arguments.Add(returnValue);
						}

						auto stat = Ptr(new WfExpressionStatement);
						stat->expression = callExpr;
						block->statements.Add(stat);
					}
					block->statements.Add(Ptr(new WfReturnStatement));

					SetCodeRange(Ptr<WfStatement>(block), node->codeRange);
					result = block;
				}

				void Visit(WfCoOperatorStatement* node)override
				{
					auto opInfo = manager->coOperatorResolvings[node].methodInfo;
					auto block = Ptr(new WfBlockStatement);

					{
						auto refImpl = Ptr(new WfReferenceExpression);
						refImpl->name.value = L"<co-impl>";

						auto funcExpr = Ptr(new WfChildExpression);
						funcExpr->parent = GetExpressionFromTypeDescriptor(opInfo->GetOwnerTypeDescriptor());
						funcExpr->name.value = opInfo->GetName();

						auto callExpr = Ptr(new WfCallExpression);
						callExpr->function = funcExpr;
						callExpr->arguments.Add(refImpl);
						for (auto argument : node->arguments)
						{
							callExpr->arguments.Add(CopyNode(argument.Obj()));
						}

						auto stat = Ptr(new WfExpressionStatement);
						stat->expression = callExpr;

						auto pauseBlock = Ptr(new WfBlockStatement);
						pauseBlock->statements.Add(stat);

						auto pauseStat = Ptr(new WfCoPauseStatement);
						pauseStat->statement = pauseBlock;

						block->statements.Add(pauseStat);
					}
					{
						Ptr<WfIfStatement> ifHasResultStat;

						if (node->varName.value == L"")
						{
							ifHasResultStat = Ptr(new WfIfStatement);
							{
								auto refCoResult = Ptr(new WfReferenceExpression);
								refCoResult->name.value = L"<co-result>";

								auto testExpr = Ptr(new WfTypeTestingExpression);
								testExpr->expression = refCoResult;
								testExpr->test = WfTypeTesting::IsNotNull;

								ifHasResultStat->expression = testExpr;
							}
						}

						auto ifStat = Ptr(new WfIfStatement);
						{
							auto refCoResult = Ptr(new WfReferenceExpression);
							refCoResult->name.value = L"<co-result>";

							auto refFailure = Ptr(new WfMemberExpression);
							refFailure->parent = refCoResult;
							refFailure->name.value = L"Failure";

							auto testExpr = Ptr(new WfTypeTestingExpression);
							testExpr->expression = refFailure;
							testExpr->test = WfTypeTesting::IsNotNull;

							ifStat->expression = testExpr;
						}
						{
							auto refCoResult = Ptr(new WfReferenceExpression);
							refCoResult->name.value = L"<co-result>";

							auto refFailure = Ptr(new WfMemberExpression);
							refFailure->parent = refCoResult;
							refFailure->name.value = L"Failure";

							auto raiseStat = Ptr(new WfRaiseExceptionStatement);
							raiseStat->expression = refFailure;

							auto ifBlock = Ptr(new WfBlockStatement);
							ifBlock->statements.Add(raiseStat);
							ifStat->trueBranch = ifBlock;
						}

						if (ifHasResultStat)
						{
							auto ifBlock = Ptr(new WfBlockStatement);
							ifHasResultStat->trueBranch = ifBlock;
							ifBlock->statements.Add(ifStat);
							block->statements.Add(ifHasResultStat);
						}
						else
						{
							block->statements.Add(ifStat);
						}
					}
					if (node->varName.value != L"")
					{
						auto refCoResult = Ptr(new WfReferenceExpression);
						refCoResult->name.value = L"<co-result>";

						auto refResult = Ptr(new WfMemberExpression);
						refResult->parent = refCoResult;
						refResult->name.value = L"Result";

						auto castResultInfo = manager->coCastResultResolvings[node].methodInfo;
						auto refCastResult = Ptr(new WfChildExpression);
						refCastResult->parent = GetExpressionFromTypeDescriptor(castResultInfo->GetOwnerTypeDescriptor());
						refCastResult->name.value = L"CastResult";

						auto callExpr = Ptr(new WfCallExpression);
						callExpr->function = refCastResult;
						callExpr->arguments.Add(refResult);

						auto varDecl = Ptr(new WfVariableDeclaration);
						varDecl->name.value = node->varName.value;
						varDecl->expression = callExpr;

						auto stat = Ptr(new WfVariableStatement);
						stat->variable = varDecl;
						block->statements.Add(stat);
					}

					SetCodeRange(Ptr<WfStatement>(block), node->codeRange);
					result = block;
				}

				void Visit(WfBlockStatement* node)override
				{
					auto block = Ptr(new WfBlockStatement);

					for (auto statement : node->statements)
					{
						statement = SearchUntilNonVirtualStatement(statement);

						if (auto coOperatorStat = statement.Cast<WfCoOperatorStatement>())
						{
							coOperatorStat->Accept(this);
							CopyFrom(block->statements, result.Cast<WfBlockStatement>()->statements, true);
						}
						else
						{
							block->statements.Add(CopyNode(statement.Obj()));
						}
					}

					SetCodeRange(Ptr<WfStatement>(block), node->codeRange);
					result = block;
				}
			};

			void ExpandCoProviderStatement(WfLexicalScopeManager* manager, WfCoProviderStatement* node)
			{
				auto scope = manager->nodeScopes[node].Obj();
				auto functionScope = scope->FindFunctionScope();
				auto funcDecl = functionScope->ownerNode.Cast<WfFunctionDeclaration>();
				auto providerScope = manager->nodeScopes[funcDecl->statement.Obj()];
				auto providerType = providerScope->symbols[L"$PROVIDER"][0]->typeInfo;
				auto implType = providerScope->symbols[L"$IMPL"][0]->typeInfo;

				auto coroutineExpr = Ptr(new WfNewCoroutineExpression);
				{
					coroutineExpr->name.value = L"<co-result>";
					coroutineExpr->statement = ExpandCoProviderStatementVisitor(manager).CopyNode(node->statement.Obj());
				}
				manager->coNewCoroutineResolvings.Add(coroutineExpr, ResolveExpressionResult::ReadonlyType(providerType));

				auto creatorExpr = Ptr(new WfFunctionExpression);
				{
					auto creatorDecl = Ptr(new WfFunctionDeclaration);
					creatorExpr->function = creatorDecl;
					creatorDecl->functionKind = WfFunctionKind::Normal;
					creatorDecl->anonymity = WfFunctionAnonymity::Anonymous;
					creatorDecl->returnType = GetTypeFromTypeInfo(TypeInfoRetriver<Ptr<ICoroutine>>::CreateTypeInfo().Obj());
					{
						auto argument = Ptr(new WfFunctionArgument);
						creatorDecl->arguments.Add(argument);
						argument->name.value = L"<co-impl>";
						argument->type = GetTypeFromTypeInfo(implType.Obj());
					}

					auto block = Ptr(new WfBlockStatement);
					creatorDecl->statement = block;

					auto returnStat = Ptr(new WfReturnStatement);
					returnStat->expression = coroutineExpr;
					block->statements.Add(returnStat);
				}

				auto providerBlock = Ptr(new WfBlockStatement);
				{
					auto funcReturnType = CreateTypeInfoFromType(functionScope, funcDecl->returnType);
					auto creatorInfo = manager->coProviderResolvings[node].methodInfo;

					auto funcExpr = Ptr(new WfChildExpression);
					funcExpr->parent = GetExpressionFromTypeDescriptor(creatorInfo->GetOwnerTypeDescriptor());
					funcExpr->name.value = creatorInfo->GetName();

					auto callExpr = Ptr(new WfCallExpression);
					callExpr->function = funcExpr;
					callExpr->arguments.Add(creatorExpr);

					if (funcReturnType->GetTypeDescriptor() == description::GetTypeDescriptor<void>())
					{
						auto stat = Ptr(new WfExpressionStatement);
						stat->expression = callExpr;
						providerBlock->statements.Add(stat);
					}
					else
					{
						if (IsSameType(funcReturnType.Obj(), creatorInfo->GetReturn()))
						{
							auto stat = Ptr(new WfReturnStatement);
							stat->expression = callExpr;
							providerBlock->statements.Add(stat);
						}
						else if (funcReturnType->GetTypeDescriptor() == creatorInfo->GetReturn()->GetTypeDescriptor())
						{
							auto castExpr = Ptr(new WfTypeCastingExpression);
							castExpr->strategy = WfTypeCastingStrategy::Strong;
							castExpr->type = GetTypeFromTypeInfo(funcReturnType.Obj());
							castExpr->expression = callExpr;

							auto stat = Ptr(new WfReturnStatement);
							stat->expression = castExpr;
							providerBlock->statements.Add(stat);
						}
						else
						{
							{
								auto varDecl = Ptr(new WfVariableDeclaration);
								varDecl->name.value = L"<co-mixin-source-variable>";
								varDecl->expression = callExpr;

								auto stat = Ptr(new WfVariableStatement);
								stat->variable = varDecl;
								providerBlock->statements.Add(stat);
							}
							{
								auto refExpr = Ptr(new WfReferenceExpression);
								refExpr->name.value = L"<co-mixin-source-variable>";

								auto castExpr = Ptr(new WfMixinCastExpression);
								castExpr->type = GetTypeFromTypeInfo(funcReturnType.Obj());
								castExpr->expression = refExpr;

								auto stat = Ptr(new WfReturnStatement);
								stat->expression = castExpr;
								providerBlock->statements.Add(stat);
							}
						}
					}
				}
				
				node->expandedStatement = providerBlock;
			}
		}
	}
}