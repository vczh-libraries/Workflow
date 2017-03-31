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

			void ExpandCoProviderStatement(WfLexicalScopeManager* manager, WfCoProviderStatement* node)
			{
				throw 0;
			}
		}
	}
}