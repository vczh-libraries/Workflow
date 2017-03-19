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

						auto compare = MakePtr<WfBinaryExpression>();
						compare->first = refExpr;
						compare->second = CopyExpression(switchCase->expression);
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
						if (range->beginBoundary == WfRangeBoundary::Exclusive)
						{
							auto one = MakePtr<WfIntegerExpression>();
							one->value.value = L"1";

							auto addExpr = MakePtr<WfBinaryExpression>();
							addExpr->first = decl->expression;
							addExpr->second = one;
							addExpr->op = WfBinaryOperator::Sub;

							decl->expression = addExpr;
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

							whileBlock->statements.Add(CopyStatement(node->statement));
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
								whileBlock->statements.Add(stat);
							}
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
						decl->expression = CopyExpression(node->collection);
						if (node->direction == WfForEachDirection::Reversed)
						{
							throw 0;
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

								auto decl = MakePtr<WfVariableDeclaration>();
								decl->name.value = node->name.value;
								decl->expression = callExpr;

								auto stat = MakePtr<WfVariableStatement>();
								stat->variable = decl;
								block->statements.Add(stat);
							}
							whileBlock->statements.Add(CopyStatement(node->statement));
						}
						block->statements.Add(whileStat);
					}
				}
			}
		}
	}
}