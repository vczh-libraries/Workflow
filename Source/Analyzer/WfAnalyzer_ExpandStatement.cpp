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
ExpandSwitchStatement
***********************************************************************/

			void ExpandSwitchStatement(WfLexicalScopeManager* manager, WfSwitchStatement* node)
			{
				auto block = MakePtr<WfBlockStatement>();
				node->expandedStatement = block;
				auto varName = L"<switch>" + itow(manager->usedTempVars++);

				{
					auto result = manager->expressionResolvings[node->expression.Obj()];
					auto decl = MakePtr<WfVariableDeclaration>();
					decl->name.value = varName;
					decl->type = GetTypeFromTypeInfo(result.type.Obj());
					decl->expression = CopyExpression(node->expression);

					auto stat = MakePtr<WfVariableStatement>();
					stat->variable = decl;
					block->statements.Add(stat);
				}

				Ptr<WfStatement> rootIfStat;
				auto tailIfStat = &rootIfStat;

				FOREACH(Ptr<WfSwitchCase>, switchCase, node->caseBranches)
				{
					auto ifStat = MakePtr<WfIfStatement>();
					*tailIfStat = ifStat;
					tailIfStat = &ifStat->falseBranch;

					{
						auto refExpr = MakePtr<WfReferenceExpression>();
						refExpr->name.value = varName;

						auto inferExpr = MakePtr<WfInferExpression>();
						inferExpr->expression= CopyExpression(switchCase->expression);
						{
							auto result = manager->expressionResolvings[switchCase->expression.Obj()];
							inferExpr->type = GetTypeFromTypeInfo(result.type.Obj());
						}

						auto compare = MakePtr<WfBinaryExpression>();
						compare->first = refExpr;
						compare->second = inferExpr;
						compare->op = WfBinaryOperator::EQ;

						ifStat->expression = compare;
					}
					ifStat->trueBranch = CopyStatement(switchCase->statement);
				}

				if (node->defaultBranch)
				{
					*tailIfStat = CopyStatement(node->defaultBranch);
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
				auto refVar1 = MakePtr<WfReferenceExpression>();
				refVar1->name.value = node->name.value;

				auto refVar2 = MakePtr<WfReferenceExpression>();
				refVar2->name.value = node->name.value;

				auto one = MakePtr<WfIntegerExpression>();
				one->value.value = L"1";

				auto stepExpr = MakePtr<WfBinaryExpression>();
				stepExpr->first = refVar2;
				stepExpr->second = one;
				stepExpr->op = node->direction == WfForEachDirection::Normal ? WfBinaryOperator::Add : WfBinaryOperator::Sub;

				auto assignExpr = MakePtr<WfBinaryExpression>();
				assignExpr->first = refVar1;
				assignExpr->second = stepExpr;
				assignExpr->op = WfBinaryOperator::Assign;

				auto stat = MakePtr<WfExpressionStatement>();
				stat->expression = assignExpr;
				return stat;
			}

			class CopyForEachRangeBodyVisitor
				: public copy_visitor::ModuleVisitor
			{
			public:
				WfLexicalScopeManager*						manager;
				WfForEachStatement*							forEach;

				CopyForEachRangeBodyVisitor(WfLexicalScopeManager* _manager, WfForEachStatement* _forEach)
					:manager(_manager)
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
								auto block = MakePtr<WfBlockStatement>();
								block->statements.Add(GenerateForEachStepStatement(forEach));
								block->statements.Add(MakePtr<WfContinueStatement>());
								SetCodeRange((Ptr<WfStatement>)block, node->codeRange);
								result = block;
								return;
							}
						}
						scope = scope->parentScope;
					}
					copy_visitor::StatementVisitor::Visit(node);
				}
			};

			void ExpandForEachStatement(WfLexicalScopeManager* manager, WfForEachStatement* node)
			{
				auto block = MakePtr<WfBlockStatement>();
				node->expandedStatement = block;

				if (auto range = node->collection.Cast<WfRangeExpression>())
				{
					auto varBegin = L"<for-begin>" + node->name.value;
					auto varEnd = L"<for-end>" + node->name.value;
					{
						auto result = manager->expressionResolvings[range->begin.Obj()];
						auto decl = MakePtr<WfVariableDeclaration>();
						decl->name.value = varBegin;
						decl->type = GetTypeFromTypeInfo(result.type.Obj());
						decl->expression = CopyExpression(range->begin);
						if (range->beginBoundary == WfRangeBoundary::Exclusive)
						{
							auto one = MakePtr<WfIntegerExpression>();
							one->value.value = L"1";

							auto addExpr = MakePtr<WfBinaryExpression>();
							addExpr->first = decl->expression;
							addExpr->second = one;
							addExpr->op = WfBinaryOperator::Add;
							
							decl->expression = addExpr;
						}

						auto stat = MakePtr<WfVariableStatement>();
						stat->variable = decl;
						block->statements.Add(stat);
					}
					{
						auto result = manager->expressionResolvings[range->end.Obj()];
						auto decl = MakePtr<WfVariableDeclaration>();
						decl->name.value = varEnd;
						decl->type = GetTypeFromTypeInfo(result.type.Obj());
						decl->expression = CopyExpression(range->end);
						if (range->endBoundary == WfRangeBoundary::Exclusive)
						{
							auto one = MakePtr<WfIntegerExpression>();
							one->value.value = L"1";

							auto subExpr = MakePtr<WfBinaryExpression>();
							subExpr->first = decl->expression;
							subExpr->second = one;
							subExpr->op = WfBinaryOperator::Sub;

							decl->expression = subExpr;
						}

						auto stat = MakePtr<WfVariableStatement>();
						stat->variable = decl;
						block->statements.Add(stat);
					}
					{
						auto refBegin = MakePtr<WfReferenceExpression>();
						refBegin->name.value = node->direction == WfForEachDirection::Normal ? varBegin : varEnd;

						auto decl = MakePtr<WfVariableDeclaration>();
						decl->name.value = node->name.value;
						decl->expression = refBegin;

						auto stat = MakePtr<WfVariableStatement>();
						stat->variable = decl;
						block->statements.Add(stat);
					}
					{
						auto whileStat = MakePtr<WfWhileStatement>();
						{
							auto refVar = MakePtr<WfReferenceExpression>();
							refVar->name.value = node->name.value;

							auto refBegin = MakePtr<WfReferenceExpression>();
							refBegin->name.value = node->direction == WfForEachDirection::Normal ? varEnd : varBegin;

							auto compare = MakePtr<WfBinaryExpression>();
							compare->first = refVar;
							compare->second = refBegin;
							compare->op = node->direction == WfForEachDirection::Normal ? WfBinaryOperator::LE : WfBinaryOperator::GE;

							whileStat->condition = compare;
						}
						{
							auto whileBlock = MakePtr<WfBlockStatement>();
							whileStat->statement = whileBlock;

							{
								CopyForEachRangeBodyVisitor visitor(manager, node);
								node->statement->Accept(&visitor);
								whileBlock->statements.Add(visitor.result.Cast<WfStatement>());
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
						auto decl = MakePtr<WfVariableDeclaration>();
						decl->name.value = varEnum;
						if (node->direction == WfForEachDirection::Normal)
						{
							auto inferExpr = MakePtr<WfInferExpression>();
							inferExpr->expression = CopyExpression(node->collection);
							inferExpr->type = GetTypeFromTypeInfo(TypeInfoRetriver<Ptr<IValueEnumerable>>::CreateTypeInfo().Obj());

							decl->expression = inferExpr;
						}
						else
						{
							auto refSystem = MakePtr<WfTopQualifiedExpression>();
							refSystem->name.value = L"system";

							auto refSys = MakePtr<WfChildExpression>();
							refSys->parent = refSystem;
							refSys->name.value = L"Sys";

							auto refMethod = MakePtr<WfChildExpression>();
							refMethod->parent = refSys;
							refMethod->name.value = L"ReverseEnumerable";

							auto refCall = MakePtr<WfCallExpression>();
							refCall->function = refMethod;
							refCall->arguments.Add(CopyExpression(node->collection));

							decl->expression = refCall;
						}

						auto stat = MakePtr<WfVariableStatement>();
						stat->variable = decl;
						block->statements.Add(stat);
					}
					{
						auto refEnum = MakePtr<WfReferenceExpression>();
						refEnum->name.value = varEnum;

						auto refMethod = MakePtr<WfMemberExpression>();
						refMethod->parent = refEnum;
						refMethod->name.value = L"CreateEnumerator";

						auto callExpr = MakePtr<WfCallExpression>();
						callExpr->function = refMethod;

						auto decl = MakePtr<WfVariableDeclaration>();
						decl->name.value = varIter;
						decl->expression = callExpr;

						auto stat = MakePtr<WfVariableStatement>();
						stat->variable = decl;
						block->statements.Add(stat);
					}
					{
						auto whileStat = MakePtr<WfWhileStatement>();
						{
							auto refIter = MakePtr<WfReferenceExpression>();
							refIter->name.value = varIter;

							auto refMethod = MakePtr<WfMemberExpression>();
							refMethod->parent = refIter;
							refMethod->name.value = L"Next";

							auto callExpr = MakePtr<WfCallExpression>();
							callExpr->function = refMethod;

							whileStat->condition = callExpr;
						}
						{
							auto whileBlock = MakePtr<WfBlockStatement>();
							whileStat->statement = whileBlock;

							{
								auto refIter = MakePtr<WfReferenceExpression>();
								refIter->name.value = varIter;

								auto refMethod = MakePtr<WfMemberExpression>();
								refMethod->parent = refIter;
								refMethod->name.value = L"GetCurrent";

								auto callExpr = MakePtr<WfCallExpression>();
								callExpr->function = refMethod;

								auto castExpr = MakePtr<WfTypeCastingExpression>();
								castExpr->expression = callExpr;
								castExpr->strategy = WfTypeCastingStrategy::Strong;
								{
									auto parentScope = manager->nodeScopes[node];
									auto symbol = parentScope->symbols[node->name.value][0];
									castExpr->type = GetTypeFromTypeInfo(symbol->typeInfo.Obj());
								}

								auto decl = MakePtr<WfVariableDeclaration>();
								decl->name.value = node->name.value;
								decl->expression = castExpr;

								auto stat = MakePtr<WfVariableStatement>();
								stat->variable = decl;
								whileBlock->statements.Add(stat);
							}
							whileBlock->statements.Add(CopyStatement(node->statement));
						}
						block->statements.Add(whileStat);
					}
				}
			}

/***********************************************************************
ExpandCoProviderStatement
***********************************************************************/

			class ExpandCoProviderStatementVisitor
				: public copy_visitor::StatementVisitor
				, public copy_visitor::CoroutineStatementVisitor
			{
			public:
				WfLexicalScopeManager*						manager;

				ExpandCoProviderStatementVisitor(WfLexicalScopeManager* _manager)
					:manager(_manager)
				{
				}

				Ptr<WfExpression> CreateField(Ptr<WfExpression> from)override
				{
					return CopyExpression(from);
				}

				Ptr<WfType> CreateField(Ptr<WfType> from)override
				{
					return CopyType(from);
				}

				Ptr<WfStatement> CreateField(Ptr<WfStatement> from)override
				{
					from->Accept(this);
					return result.Cast<WfStatement>();
				}

				void Visit(WfReturnStatement* node)override
				{
					auto opInfo = manager->coOperatorResolvings[node].methodInfo;
					auto block = MakePtr<WfBlockStatement>();

					{
						auto refImpl = MakePtr<WfReferenceExpression>();
						refImpl->name.value = L"<co-impl>";

						auto funcExpr = MakePtr<WfChildExpression>();
						funcExpr->parent = GetExpressionFromTypeDescriptor(opInfo->GetOwnerTypeDescriptor());
						funcExpr->name.value = opInfo->GetName();

						auto callExpr = MakePtr<WfCallExpression>();
						callExpr->function = funcExpr;
						callExpr->arguments.Add(refImpl);
						if (node->expression)
						{
							callExpr->arguments.Add(CreateField(node->expression));
						}

						auto stat = MakePtr<WfExpressionStatement>();
						stat->expression = callExpr;
						block->statements.Add(stat);
					}
					block->statements.Add(MakePtr<WfReturnStatement>());

					SetCodeRange(Ptr<WfStatement>(block), node->codeRange);
					result = block;
				}

				void Visit(WfCoOperatorStatement* node)override
				{
					auto opInfo = manager->coOperatorResolvings[node].methodInfo;
					auto block = MakePtr<WfBlockStatement>();

					{
						auto refImpl = MakePtr<WfReferenceExpression>();
						refImpl->name.value = L"<co-impl>";

						auto funcExpr = MakePtr<WfChildExpression>();
						funcExpr->parent = GetExpressionFromTypeDescriptor(opInfo->GetOwnerTypeDescriptor());
						funcExpr->name.value = opInfo->GetName();

						auto callExpr = MakePtr<WfCallExpression>();
						callExpr->function = funcExpr;
						callExpr->arguments.Add(refImpl);
						FOREACH(Ptr<WfExpression>, argument, node->arguments)
						{
							callExpr->arguments.Add(CreateField(argument));
						}

						auto stat = MakePtr<WfExpressionStatement>();
						stat->expression = callExpr;
						block->statements.Add(stat);

						auto pauseBlock = MakePtr<WfBlockStatement>();
						pauseBlock->statements.Add(stat);

						auto pauseStat = MakePtr<WfCoPauseStatement>();
						pauseStat->statement = pauseBlock;

						block->statements.Add(pauseStat);
					}
					{
						auto ifStat = MakePtr<WfIfStatement>();
						block->statements.Add(ifStat);
						{
							auto refCoResult = MakePtr<WfReferenceExpression>();
							refCoResult->name.value = L"<co-result>";

							auto refFailure = MakePtr<WfMemberExpression>();
							refFailure->parent = refCoResult;
							refFailure->name.value = L"Failure";

							auto testExpr = MakePtr<WfTypeTestingExpression>();
							testExpr->expression = refFailure;
							testExpr->test = WfTypeTesting::IsNotNull;

							ifStat->expression = testExpr;
						}
						{
							auto refCoResult = MakePtr<WfReferenceExpression>();
							refCoResult->name.value = L"<co-result>";

							auto refFailure = MakePtr<WfMemberExpression>();
							refFailure->parent = refCoResult;
							refFailure->name.value = L"Failure";

							auto raiseStat = MakePtr<WfRaiseExceptionStatement>();
							raiseStat->expression = refFailure;

							auto ifBlock = MakePtr<WfBlockStatement>();
							ifBlock->statements.Add(raiseStat);
							ifStat->trueBranch = ifBlock;
						}
					}
					if (node->varName.value != L"")
					{
						auto refCoResult = MakePtr<WfReferenceExpression>();
						refCoResult->name.value = L"<co-result>";

						auto refResult = MakePtr<WfMemberExpression>();
						refResult->parent = refCoResult;
						refResult->name.value = L"Result";

						auto castResultInfo = manager->coCastResultResolvings[node].methodInfo;
						auto refCastResult = MakePtr<WfChildExpression>();
						refCastResult->parent = GetExpressionFromTypeDescriptor(castResultInfo->GetOwnerTypeDescriptor());
						refCastResult->name.value = refCastResult->name.value;

						auto callExpr = MakePtr<WfCallExpression>();
						callExpr->function = refCastResult;
						callExpr->arguments.Add(refResult);

						auto varDecl = MakePtr<WfVariableDeclaration>();
						varDecl->name.value = node->varName.value;
						varDecl->expression = callExpr;

						auto stat = MakePtr<WfVariableStatement>();
						stat->variable = varDecl;
						block->statements.Add(stat);
					}

					SetCodeRange(Ptr<WfStatement>(block), node->codeRange);
					result = block;
				}

				void Visit(WfBlockStatement* node)override
				{
					auto block = MakePtr<WfBlockStatement>();

					FOREACH(Ptr<WfStatement>, statement, node->statements)
					{
						while (auto virtualStat = statement.Cast<WfVirtualStatement>())
						{
							statement = virtualStat->expandedStatement;
						}
						if (auto coOperatorStat = statement.Cast<WfCoOperatorStatement>())
						{
							coOperatorStat->Accept(this);
							CopyFrom(block->statements, result.Cast<WfBlockStatement>()->statements, true);
						}
						else
						{
							block->statements.Add(CreateField(statement));
						}
					}

					SetCodeRange(Ptr<WfStatement>(block), node->codeRange);
					result = block;
				}

				Ptr<ParsingTreeCustomBase> Dispatch(WfVirtualStatement* node)override
				{
					node->expandedStatement->Accept(this);
					return result;
				}

				Ptr<ParsingTreeCustomBase> Dispatch(WfCoroutineStatement* node)override
				{
					node->Accept((WfCoroutineStatement::IVisitor*)this);
					return result;
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

				auto coroutineExpr = MakePtr<WfNewCoroutineExpression>();
				{
					coroutineExpr->name.value = L"<co-result>";
					coroutineExpr->statement = ExpandCoProviderStatementVisitor(manager).CreateField(node->statement);
				}

				auto creatorExpr = MakePtr<WfFunctionExpression>();
				{
					auto creatorDecl = MakePtr<WfFunctionDeclaration>();
					creatorExpr->function = creatorDecl;
					creatorDecl->anonymity = WfFunctionAnonymity::Anonymous;
					creatorDecl->returnType = GetTypeFromTypeInfo(TypeInfoRetriver<Ptr<ICoroutine>>::CreateTypeInfo().Obj());
					{
						auto argument = MakePtr<WfFunctionArgument>();
						creatorDecl->arguments.Add(argument);
						argument->name.value = L"<co-impl>";
						argument->type = GetTypeFromTypeInfo(implType.Obj());
					}

					auto block = MakePtr<WfBlockStatement>();
					creatorDecl->statement = block;

					auto returnStat = MakePtr<WfReturnStatement>();
					returnStat->expression = coroutineExpr;
					block->statements.Add(returnStat);
				}

				auto providerBlock = MakePtr<WfBlockStatement>();
				{
					auto funcSymbol = manager->GetDeclarationSymbol(manager->nodeScopes[funcDecl.Obj()].Obj(), funcDecl.Obj());
					auto funcReturnType = funcSymbol->typeInfo->GetElementType()->GetGenericArgument(0);
					auto creatorInfo = manager->coProviderResolvings[node].methodInfo;

					auto funcExpr = MakePtr<WfChildExpression>();
					funcExpr->parent = GetExpressionFromTypeDescriptor(creatorInfo->GetOwnerTypeDescriptor());
					funcExpr->name.value = creatorInfo->GetName();

					auto callExpr = MakePtr<WfCallExpression>();
					callExpr->function = funcExpr;
					callExpr->arguments.Add(creatorExpr);

					if (funcReturnType->GetTypeDescriptor() == description::GetTypeDescriptor<void>())
					{
						auto stat = MakePtr<WfExpressionStatement>();
						stat->expression = callExpr;
						providerBlock->statements.Add(stat);
					}
					else
					{
						if (IsSameType(funcReturnType, creatorInfo->GetReturn()))
						{
							auto stat = MakePtr<WfReturnStatement>();
							stat->expression = callExpr;
							providerBlock->statements.Add(stat);
						}
						else if (funcReturnType->GetTypeDescriptor() == creatorInfo->GetReturn()->GetTypeDescriptor())
						{
							auto castExpr = MakePtr<WfTypeCastingExpression>();
							castExpr->strategy = WfTypeCastingStrategy::Strong;
							castExpr->type = GetTypeFromTypeInfo(funcReturnType);
							castExpr->expression = callExpr;

							auto stat = MakePtr<WfReturnStatement>();
							stat->expression = castExpr;
							providerBlock->statements.Add(stat);
						}
						else
						{
							throw 0;
						}
					}
				}
				
				node->expandedStatement = providerBlock;
			}
		}
	}
}