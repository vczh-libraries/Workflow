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
FindCoroutineAwaredStatements
***********************************************************************/

			class FindCoroutineAwaredStatementVisitor : public empty_visitor::StatementVisitor
			{
			public:
				List<WfStatement*>&						awaredStatements;
				bool									awared = false;

				FindCoroutineAwaredStatementVisitor(List<WfStatement*>& _awaredStatements)
					:awaredStatements(_awaredStatements)
				{
				}

				bool Call(Ptr<WfStatement> node)
				{
					if (!node) return false;

					awared = false;
					node->Accept(this);
					if (awared)
					{
						awaredStatements.Add(node.Obj());
					}
					return awared;
				}

				void Dispatch(WfVirtualStatement* node)override
				{
					awared = Call(node->expandedStatement);
				}

				void Dispatch(WfCoroutineStatement* node)override
				{
					awared = true;
				}

				void Visit(WfIfStatement* node)override
				{
					bool a = Call(node->trueBranch);
					bool b = Call(node->falseBranch);
					awared = a || b;
				}

				void Visit(WfWhileStatement* node)override
				{
					awared = Call(node->statement);
				}

				void Visit(WfTryStatement* node)override
				{
					bool a = Call(node->protectedStatement);
					bool b = Call(node->catchStatement);
					bool c = Call(node->finallyStatement);
					awared = a || b || c;
				}

				void Visit(WfBlockStatement* node)override
				{
					bool result = false;
					FOREACH(Ptr<WfStatement>, stat, node->statements)
					{
						bool a = Call(stat);
						result |= a;
					}
					awared = result;
				}
			};

			void FindCoroutineAwaredStatements(Ptr<WfStatement> node, List<WfStatement*>& awaredStatements)
			{
				FindCoroutineAwaredStatementVisitor(awaredStatements).Call(node);
			}

/***********************************************************************
FindCoroutineAwaredVariables
***********************************************************************/

			class FindCoroutineAwaredVariableVisitor : public empty_visitor::StatementVisitor
			{
			public:
				List<WfVariableStatement*>&				awaredVariables;

				FindCoroutineAwaredVariableVisitor(List<WfVariableStatement*>& _awaredVariables)
					:awaredVariables(_awaredVariables)
				{
				}

				void Dispatch(WfVirtualStatement* node)override
				{
					node->expandedStatement->Accept(this);
				}

				void Dispatch(WfCoroutineStatement* node)override
				{
				}

				void Visit(WfVariableStatement* node)override
				{
					awaredVariables.Add(node);
				}
			};

			class FindCoroutineAwaredBlockVisitor : public empty_visitor::StatementVisitor
			{
			public:
				List<WfVariableStatement*>&				awaredVariables;

				FindCoroutineAwaredBlockVisitor(List<WfVariableStatement*>& _awaredVariables)
					:awaredVariables(_awaredVariables)
				{
				}

				void Dispatch(WfVirtualStatement* node)override
				{
					// If an virtual node is coroutine awared
					// than its expandedStatement is also in the list
					// no need to find variables again
				}

				void Dispatch(WfCoroutineStatement* node)override
				{
				}

				void Visit(WfBlockStatement* node)override
				{
					FindCoroutineAwaredVariableVisitor visitor(awaredVariables);
					FOREACH(Ptr<WfStatement>, stat, node->statements)
					{
						stat->Accept(&visitor);
					}
				}
			};

			void FindCoroutineAwaredVariables(WfStatement* node, List<WfVariableStatement*>& awaredVariables)
			{
				FindCoroutineAwaredBlockVisitor visitor(awaredVariables);
				node->Accept(&visitor);
			}

/***********************************************************************
FindCoroutineReferenceRenaming
***********************************************************************/

			void FindCoroutineReferenceRenaming(WfLexicalScopeManager* manager, List<WfStatement*>& awaredStatements, List<WfVariableStatement*>& awaredVariables, Dictionary<WfLexicalSymbol*, WString>& referenceRenaming)
			{
				vint renameCounter = 0;
				auto rename = [&](const WString& name)
				{
					if (name.Length() > 0 && name[0] == L'<')
					{
						vint index = INVLOC.FindFirst(name, L">", Locale::None).key;
						auto category = name.Sub(1, index - 1);
						auto local = name.Sub(index + 1, name.Length() - index - 1);
						return L"<co" + itow(renameCounter++) + L"-" + category + L">" + local;
					}
					else
					{
						return L"<co" + itow(renameCounter++) + L">" + name;
					}
				};

				FOREACH(WfVariableStatement*, stat, awaredVariables)
				{
					auto scope = manager->nodeScopes[stat];
					auto symbol = scope->symbols[stat->variable->name.value][0];
					auto name = rename(stat->variable->name.value);
					referenceRenaming.Add(symbol.Obj(), name);
				}

				FOREACH(WfStatement*, stat, awaredStatements)
				{
					if (auto tryStat = dynamic_cast<WfTryStatement*>(stat))
					{
						if (tryStat->catchStatement)
						{
							auto scope = manager->nodeScopes[tryStat->catchStatement.Obj()]->parentScope.Obj();
							auto symbol = scope->symbols[tryStat->name.value][0];
							auto name = rename(tryStat->name.value);
							referenceRenaming.Add(symbol.Obj(), name);
						}
					}
					else if (auto ifStat = dynamic_cast<WfIfStatement*>(stat))
					{
						if (ifStat->name.value != L"")
						{
							auto scope = manager->nodeScopes[ifStat->trueBranch.Obj()]->parentScope.Obj();
							auto symbol = scope->symbols[ifStat->name.value][0];
							auto name = rename(ifStat->name.value);
							referenceRenaming.Add(symbol.Obj(), name);
						}
					}
				}
			}

/***********************************************************************
GenerateFlowChart
***********************************************************************/

			class FlowChartNode;

			class FlowChartBranch : public Object
			{
			public:
				Ptr<WfExpression>						condition;
				FlowChartNode*							destination = nullptr;
			};

			enum class FlowChartNodeAction
			{
				None,
				SetPause,
			};

			class FlowChartNode : public Object
			{
			public:
				FlowChartNodeAction						action = FlowChartNodeAction::None;
				List<Ptr<WfStatement>>					statements;
				List<Ptr<FlowChartBranch>>				branches;
				FlowChartNode*							destination = nullptr;
				FlowChartNode*							exceptionDestination = nullptr;
				FlowChartNode*							pauseDestination = nullptr;
				WfLexicalSymbol*						exceptionVariable = nullptr;
			};

			class FlowChart : public Object
			{
				typedef Dictionary<WfTryStatement*, Ptr<WfLexicalSymbol>>		TempExVarMap;
			public:
				List<Ptr<FlowChartNode>>				nodes;
				FlowChartNode*							headNode = nullptr;
				FlowChartNode*							lastNode = nullptr;
				TempExVarMap							tempExVars;

				FlowChartNode* CreateNode(FlowChartNode* catchNode)
				{
					auto node = MakePtr<FlowChartNode>();
					node->exceptionDestination = catchNode;
					nodes.Add(node);
					return node.Obj();
				}

				FlowChartNode* AppendNode(FlowChartNode* head, FlowChartNode* catchNode, FlowChartNodeAction action = FlowChartNodeAction::None)
				{
					auto node = CreateNode(catchNode);
					node->action = action;
					if (head)
					{
						CHECK_ERROR(head->destination == nullptr, L"FlowChart::AppendNode(FlowChartNode*, FlowChartNode*, FlowChartNodeAction)#Cannot append a new node to a flow chart node that already has a default destination.");
						head->destination = node;
					}
					return node;
				}

				FlowChartNode* EnsureAppendStatement(FlowChartNode* head, FlowChartNode* catchNode)
				{
					if (head == nullptr)
					{
						return CreateNode(catchNode);
					}
					else if(head->branches.Count() > 0 || head->exceptionDestination != catchNode)
					{
						CHECK_ERROR(head->destination == nullptr, L"FlowChart::EnsureAppendStatement(FlowChartNode*, FlowChartNode*)#Cannot append a statement to a flow chart node that already has a default destination.");
						auto node = CreateNode(catchNode);
						head->destination = node;
						return node;
					}
					else
					{
						return head;
					}
				}
			};

			class GenerateFlowChartModuleVisitor : public copy_visitor::ModuleVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				Dictionary<WfLexicalSymbol*, WString>&	referenceRenaming;

				GenerateFlowChartModuleVisitor(WfLexicalScopeManager* _manager, Dictionary<WfLexicalSymbol*, WString>& _referenceRenaming)
					:manager(_manager)
					, referenceRenaming(_referenceRenaming)
				{
				}

				void Visit(WfReferenceExpression* node)override
				{
					copy_visitor::ExpressionVisitor::Visit(node);
					auto resolvingResult = manager->expressionResolvings[node];
					vint index = referenceRenaming.Keys().IndexOf(resolvingResult.symbol.Obj());
					if (index != -1)
					{
						result.Cast<WfReferenceExpression>()->name.value = referenceRenaming.Values()[index];
					}
				}
			};

			class GenerateFlowChartStatementVisitor
				: public Object
				, public WfStatement::IVisitor
				, public WfCoroutineStatement::IVisitor
			{
			public:
				enum class ScopeType
				{
					Function,
					Loop,
					TryCatch,
				};

				struct ScopeContext
				{
					ScopeContext*						parent = nullptr;
					ScopeType							type = ScopeType::Function;
					FlowChartNode*						enterNode = nullptr;
					FlowChartNode*						leaveNode = nullptr;
					ScopeContext*						exitStatementScope = nullptr;
					Ptr<WfStatement>					exitStatement;
				};

			public:
				WfLexicalScopeManager*					manager;
				SortedList<WfStatement*>&				awaredStatements;
				Dictionary<WfLexicalSymbol*, WString>&	referenceRenaming;
				Ptr<FlowChart>							flowChart;
				FlowChartNode*							headNode;
				FlowChartNode*							catchNode;
				ScopeContext*							scopeContext;
				FlowChartNode*							resultHead = nullptr;
				FlowChartNode*							resultLast = nullptr;

				GenerateFlowChartStatementVisitor(WfLexicalScopeManager* _manager, SortedList<WfStatement*>& _awaredStatements, Dictionary<WfLexicalSymbol*, WString>& _referenceRenaming, Ptr<FlowChart> _flowChart, FlowChartNode* _headNode, FlowChartNode* _catchNode, ScopeContext* _scopeContext)
					:manager(_manager)
					, awaredStatements(_awaredStatements)
					, referenceRenaming(_referenceRenaming)
					, flowChart(_flowChart)
					, headNode(_headNode)
					, catchNode(_catchNode)
					, scopeContext(_scopeContext)
				{
				}

#define COPY_AST(STATEMENT) GenerateFlowChartModuleVisitor(manager, referenceRenaming).CreateField(STATEMENT)

				void AppendAwaredStatement(FlowChartNode* catchNode, ScopeContext* scopeContext, Ptr<WfStatement> statement)
				{
					if (!resultHead)
					{
						resultHead = flowChart->EnsureAppendStatement(headNode, catchNode);
						resultLast = resultHead;
					}
					resultLast = Execute(resultHead, catchNode, scopeContext, statement).value;
				}

				void AppendUnawaredCopiedStatement(FlowChartNode* catchNode, ScopeContext* scopeContext, Ptr<WfStatement> statement)
				{
					if (!resultHead)
					{
						resultHead = flowChart->EnsureAppendStatement(headNode, catchNode);
						resultLast = resultHead;
					}
					resultHead->statements.Add(statement);
				}

				ScopeContext* InlineScopeExitCode(ScopeType untilScopeType, bool exclusive)
				{
					auto current = scopeContext;
					while (current)
					{
						if (exclusive && scopeContext->type == untilScopeType) break;
						if (current->exitStatement)
						{
							AppendAwaredStatement(catchNode, current->exitStatementScope, current->exitStatement);
						}
						if (!exclusive && scopeContext->type == untilScopeType) break;
						current = current->parent;
					}
					return current;
				}

				static Pair<FlowChartNode*, FlowChartNode*> Execute(WfLexicalScopeManager* manager, SortedList<WfStatement*>& awaredStatements, Dictionary<WfLexicalSymbol*, WString>& referenceRenaming, Ptr<FlowChart> flowChart, FlowChartNode* headNode, FlowChartNode* catchNode, ScopeContext* scopeContext, Ptr<WfStatement> statement)
				{
					GenerateFlowChartStatementVisitor visitor(manager, awaredStatements, referenceRenaming, flowChart, headNode, catchNode, scopeContext);
					if (awaredStatements.Contains(statement.Obj()))
					{
						statement->Accept(&visitor);
					}
					else
					{
						visitor.AppendUnawaredCopiedStatement(catchNode, scopeContext, COPY_AST(statement));
					}
					return{ visitor.resultHead,visitor.resultLast };
				}

				Pair<FlowChartNode*, FlowChartNode*> Execute(FlowChartNode* headNode, FlowChartNode* catchNode, ScopeContext* scopeContext, Ptr<WfStatement> statement)
				{
					return Execute(manager, awaredStatements, referenceRenaming, flowChart, headNode, catchNode, scopeContext, statement);
				}

				void Visit(WfBreakStatement* node)override
				{
					auto targetContext = InlineScopeExitCode(ScopeType::Loop, false);
					resultLast->destination = targetContext->leaveNode;
				}

				void Visit(WfContinueStatement* node)override
				{
					auto targetContext = InlineScopeExitCode(ScopeType::Loop, true);
					resultLast->destination = targetContext->enterNode;
				}

				void Visit(WfReturnStatement* node)override
				{
					auto targetContext = InlineScopeExitCode(ScopeType::Function, false);
					AppendUnawaredCopiedStatement(catchNode, scopeContext, COPY_AST(node));
					resultLast->destination = targetContext->leaveNode;
				}

				void Visit(WfDeleteStatement* node)override
				{
					AppendUnawaredCopiedStatement(catchNode, scopeContext, COPY_AST(node));
				}

				void Visit(WfRaiseExceptionStatement* node)override
				{
					AppendUnawaredCopiedStatement(catchNode, scopeContext, COPY_AST(node));
				}

				void GenerateIfWithVar(WfIfStatement* node)
				{
					resultHead = flowChart->EnsureAppendStatement(headNode, catchNode);
					{
						auto branch = MakePtr<FlowChartBranch>();
						resultHead->branches.Add(branch);

						auto scope = manager->nodeScopes[node];
						auto symbol = scope->symbols[node->name.value][0].Obj();
						{
							auto refExpr = MakePtr<WfReferenceExpression>();
							refExpr->name.value = referenceRenaming[symbol];

							auto assignExpr = MakePtr<WfBinaryExpression>();
							assignExpr->op = WfBinaryOperator::Assign;
							assignExpr->first = refExpr;
							assignExpr->second = COPY_AST(node->expression);

							auto stat = MakePtr<WfExpressionStatement>();
							stat->expression = assignExpr;

							SetCodeRange((Ptr<WfStatement>)stat, node->expression->codeRange);
							resultHead->statements.Add(stat);
						}
						{
							auto refExpr = MakePtr<WfReferenceExpression>();
							refExpr->name.value = referenceRenaming[symbol];

							auto testExpr = MakePtr<WfTypeTestingExpression>();
							testExpr->test = WfTypeTesting::IsNotNull;
							testExpr->expression = refExpr;

							SetCodeRange((Ptr<WfExpression>)testExpr, node->expression->codeRange);
							branch->condition = testExpr;
						}
					}
					resultLast = flowChart->CreateNode(catchNode);

					{
						auto pair = Execute(nullptr, catchNode, scopeContext, node->trueBranch);
						pair.value->destination = resultLast;
						resultHead->branches[0]->destination = pair.key;
					}

					if (node->falseBranch)
					{
						auto pair = Execute(nullptr, catchNode, scopeContext, node->falseBranch);
						pair.value->destination = resultLast;
						resultHead->destination = pair.key;
					}
					else
					{
						resultHead->destination = resultLast;
					}
				}

				void GenerateIfWithoutVar(WfIfStatement* node)
				{
					resultHead = flowChart->EnsureAppendStatement(headNode, catchNode);
					resultLast = flowChart->CreateNode(catchNode);

					while (true)
					{
						auto branch = MakePtr<FlowChartBranch>();
						resultHead->branches.Add(branch);
						branch->condition = COPY_AST(node->expression);

						auto pair = Execute(nullptr, catchNode, scopeContext, node->trueBranch);
						pair.value->destination = resultLast;
						branch->destination = pair.key;

						auto next = dynamic_cast<WfIfStatement*>(node->falseBranch.Obj());
						if (next && next->name.value == L"")
						{
							node = next;
						}
						else
						{
							break;
						}
					}

					if (node->falseBranch)
					{
						auto pair = Execute(nullptr, catchNode, scopeContext, node->falseBranch);
						pair.value->destination = resultLast;
						resultHead->destination = pair.key;
					}
					else
					{
						resultHead->destination = resultLast;
					}
				}

				void Visit(WfIfStatement* node)override
				{
					if (node->name.value == L"")
					{
						GenerateIfWithoutVar(node);
					}
					else
					{
						GenerateIfWithVar(node);
					}
				}

				void Visit(WfWhileStatement* node)override
				{
					resultHead = flowChart->EnsureAppendStatement(headNode, catchNode);
					{
						auto branch = MakePtr<FlowChartBranch>();
						resultHead->branches.Add(branch);
						branch->condition = COPY_AST(node->condition);
					}
					auto loopEnd = flowChart->CreateNode(catchNode);
					{
						auto branch = MakePtr<FlowChartBranch>();
						loopEnd->branches.Add(branch);
						branch->condition = COPY_AST(node->condition);
					}
					resultLast = flowChart->CreateNode(catchNode);

					ScopeContext loopContext;
					loopContext.parent = scopeContext;
					loopContext.type = ScopeType::Loop;
					loopContext.enterNode = loopEnd;
					loopContext.leaveNode = resultLast;
					auto pair = Execute(nullptr, catchNode, &loopContext, node->statement);
					pair.value->destination = loopEnd;

					resultHead->branches[0]->destination = pair.key;
					loopEnd->branches[0]->destination = pair.key;
					resultHead->destination = resultLast;
					loopEnd->destination = resultLast;
				}

				WfLexicalSymbol* GetExceptionVariableSymbol(WfTryStatement* node)
				{
					if (node->catchStatement)
					{
						auto scope = manager->nodeScopes[node->catchStatement.Obj()]->parentScope.Obj();
						auto symbol = scope->symbols[node->name.value][0];
						return symbol.Obj();
					}
					else
					{
						vint index = flowChart->tempExVars.Keys().IndexOf(node);
						if (index == -1)
						{
							auto symbol = MakePtr<WfLexicalSymbol>(nullptr);
							symbol->name = L"ex";
							symbol->typeInfo = TypeInfoRetriver<Ptr<IValueException>>::CreateTypeInfo();
							flowChart->tempExVars.Add(node, symbol);

							referenceRenaming.Add(symbol.Obj(), L"<co-tempexvar" + itow(flowChart->tempExVars.Count() - 1) + L">ex");
							return symbol.Obj();
						}
						return flowChart->tempExVars.Values()[index].Obj();
					}
				}

				Pair<FlowChartNode*, FlowChartNode*> GenerateCatch(WfTryStatement* node, FlowChartNode* catchNode)
				{
					auto pair = Execute(nullptr, catchNode, scopeContext, node->catchStatement);
					pair.key->exceptionVariable = GetExceptionVariableSymbol(node);
					return pair;
				}

				Pair<FlowChartNode*, FlowChartNode*> GenerateFinally(WfTryStatement* node, FlowChartNode* catchNode)
				{
					return Execute(nullptr, catchNode, scopeContext, node->finallyStatement);
				}

				Pair<FlowChartNode*, FlowChartNode*> GenerateFinallyAndRaise(WfTryStatement* node, FlowChartNode* catchNode)
				{
					auto pair = Execute(nullptr, catchNode, scopeContext, node->finallyStatement);
					auto raiseNode = flowChart->CreateNode(catchNode);
					{
						pair.key->exceptionVariable = GetExceptionVariableSymbol(node);

						auto refExpr = MakePtr<WfReferenceExpression>();
						refExpr->name.value = referenceRenaming[pair.key->exceptionVariable];

						auto memberExpr = MakePtr<WfMemberExpression>();
						memberExpr->parent = refExpr;
						memberExpr->name.value = L"Message";

						auto raiseStat = MakePtr<WfRaiseExceptionStatement>();
						raiseStat->expression = memberExpr;

						SetCodeRange((Ptr<WfStatement>)raiseStat, node->finallyStatement->codeRange);
						raiseNode->statements.Add(raiseStat);
					}
					pair.value->destination = raiseNode;
					return { pair.key,raiseNode };
				}

				void Visit(WfTryStatement* node)override
				{
					ScopeContext tryContext;
					tryContext.parent = scopeContext;
					tryContext.type = ScopeType::TryCatch;
					if (node->finallyStatement)
					{
						tryContext.exitStatementScope = scopeContext;
						tryContext.exitStatement = node->finallyStatement;
					}

					if (node->catchStatement && !node->finallyStatement)
					{
						auto pairCatch = GenerateCatch(node, catchNode);
						AppendAwaredStatement(pairCatch.key, &tryContext, node->protectedStatement);

						auto endNode = flowChart->CreateNode(catchNode);
						pairCatch.value->destination = endNode;
						resultLast->destination = endNode;
						resultLast = endNode;
					}
					else if (!node->catchStatement && node->finallyStatement)
					{
						auto pairFinallyAndRaise = GenerateFinallyAndRaise(node, catchNode);
						AppendAwaredStatement(pairFinallyAndRaise.key, &tryContext, node->protectedStatement);
						auto pairFinally = GenerateFinally(node, catchNode);

						auto endNode = flowChart->CreateNode(catchNode);
						resultLast->destination = pairFinally.key;
						pairFinally.value->destination = endNode;
						pairFinallyAndRaise.value->destination = endNode;
						resultLast = endNode;
					}
					else
					{
						auto pairFinallyAndRaise = GenerateFinallyAndRaise(node, catchNode);
						auto pairCatch = GenerateCatch(node, pairFinallyAndRaise.key);
						AppendAwaredStatement(pairCatch.key, &tryContext, node->protectedStatement);
						auto pairFinally = GenerateFinally(node, catchNode);

						auto endNode = flowChart->CreateNode(catchNode);
						resultLast->destination = pairFinally.key;
						pairCatch.value->destination = pairFinally.key;
						pairFinally.value->destination = endNode;
						pairFinallyAndRaise.value->destination = endNode;
						resultLast = endNode;
					}
				}

				void Visit(WfBlockStatement* node)override
				{
					resultHead = flowChart->EnsureAppendStatement(headNode, catchNode);
					resultLast = resultHead;

					FOREACH_INDEXER(Ptr<WfStatement>, stat, index, node->statements)
					{
						auto pair = Execute(resultLast, catchNode, scopeContext, stat);
						resultLast = pair.value;
					}
				}

				void Visit(WfVariableStatement* node)override
				{
					resultHead = flowChart->EnsureAppendStatement(headNode, catchNode);
					resultLast = resultHead;

					auto scope = manager->nodeScopes[node];
					auto symbol = scope->symbols[node->variable->name.value][0].Obj();

					auto refExpr = MakePtr<WfReferenceExpression>();
					refExpr->name.value = referenceRenaming[symbol];

					auto assignExpr = MakePtr<WfBinaryExpression>();
					assignExpr->op = WfBinaryOperator::Assign;
					assignExpr->first = refExpr;
					assignExpr->second = COPY_AST(node->variable->expression);

					auto stat = MakePtr<WfExpressionStatement>();
					stat->expression = assignExpr;

					SetCodeRange((Ptr<WfStatement>)stat, node->codeRange);
					AppendUnawaredCopiedStatement(catchNode, scopeContext, stat);
				}

				void Visit(WfExpressionStatement* node)override
				{
					AppendUnawaredCopiedStatement(catchNode, scopeContext, COPY_AST(node));
				}

				void Visit(WfVirtualStatement* node)override
				{
					node->expandedStatement->Accept(this);
				}

				void Visit(WfCoroutineStatement* node)override
				{
					node->Accept(static_cast<WfCoroutineStatement::IVisitor*>(this));
				}

				void Visit(WfCoPauseStatement* node)override
				{
					resultHead = flowChart->AppendNode(headNode, catchNode, FlowChartNodeAction::SetPause);
					resultLast = resultHead;
					if (node->statement)
					{
						AppendUnawaredCopiedStatement(catchNode, scopeContext, COPY_AST(node->statement));
					}

					resultLast = flowChart->AppendNode(resultLast, catchNode);
					{
						auto pauseStat = MakePtr<WfCoPauseStatement>();
						SetCodeRange((Ptr<WfStatement>)pauseStat, node->codeRange);
						resultLast->statements.Add(pauseStat);
					}
					resultLast = flowChart->AppendNode(resultLast, catchNode);
					resultHead->pauseDestination = resultLast;
				}

#undef COPY_STATEMENT
			};

			Ptr<FlowChart> GenerateFlowChart(WfLexicalScopeManager* manager, List<WfStatement*>& awaredStatements, List<WfVariableStatement*>& awaredVariables, Dictionary<WfLexicalSymbol*, WString>& referenceRenaming, Ptr<WfStatement> statement)
			{
				auto flowChart = MakePtr<FlowChart>();
				SortedList<WfStatement*> sortedAwaredStatements;
				CopyFrom(sortedAwaredStatements, awaredStatements, true);
				CopyFrom(sortedAwaredStatements, awaredVariables, true);

				auto endNode = flowChart->CreateNode(nullptr);
				GenerateFlowChartStatementVisitor::ScopeContext context;
				context.leaveNode = endNode;

				auto pair = GenerateFlowChartStatementVisitor::Execute(manager, sortedAwaredStatements, referenceRenaming, flowChart, nullptr, nullptr, &context, statement);
				pair.value->destination = endNode;

				flowChart->headNode = pair.key;
				flowChart->lastNode = endNode;
				return flowChart;
			}

			void RemoveUnnecessaryNodesPass(Ptr<FlowChart> flowChart)
			{
				Dictionary<FlowChartNode*, vint> enterCounts;
				{
					const auto& keys = enterCounts.Keys();
					auto& values = const_cast<Dictionary<FlowChartNode*, vint>::ValueContainer&>(enterCounts.Values());

					FOREACH(Ptr<FlowChartNode>, node, flowChart->nodes)
					{
						enterCounts.Add(node.Obj(), 0);
					}
					enterCounts.Set(flowChart->headNode, 1);

					auto Inc = [&](FlowChartNode* node)
					{
						if (node)
						{
							vint index = keys.IndexOf(node);
							values[index]++;
						}
					};

					FOREACH(Ptr<FlowChartNode>, node, flowChart->nodes)
					{
						Inc(node->destination);
						Inc(node->exceptionDestination);
						Inc(node->pauseDestination);
						FOREACH(Ptr<FlowChartBranch>, branch, node->branches)
						{
							Inc(branch->destination);
						}
					}
				}

				SortedList<FlowChartNode*> mergableNodes;
				List<Ptr<FlowChartNode>> keepingNodes;
				FOREACH(Ptr<FlowChartNode>, node, flowChart->nodes)
				{
					bool mergable = false;

					if (node->branches.Count() == 0 && node->destination)
					{
						if (node->statements.Count() == 0 && node->action == FlowChartNodeAction::None && !node->exceptionVariable)
						{
							mergable = true;
						}
						else if (node->destination->action == FlowChartNodeAction::None && !node->destination->exceptionVariable && enterCounts[node->destination] == 1)
						{
							mergable = true;
						}
					}

					if (mergable)
					{
						mergableNodes.Add(node.Obj());
					}
					else
					{
						keepingNodes.Add(node);
					}
				}

				Dictionary<FlowChartNode*, FlowChartNode*> merging;
				FOREACH(FlowChartNode*, node, mergableNodes)
				{
					auto current = node;
					while (mergableNodes.Contains(current))
					{
						auto target = current->destination;

						if (current->action == FlowChartNodeAction::SetPause)
						{
							target->action = current->action;
							target->pauseDestination = current->pauseDestination;
						}
						if (current->exceptionVariable)
						{
							target->exceptionVariable = current->exceptionVariable;
						}
						if (current->statements.Count() > 0)
						{
							CopyFrom(current->statements, target->statements, true);
							CopyFrom(target->statements, current->statements);
							current->statements.Clear();
						}

						current = target;
					}
					merging.Add(node, current);
				}

#define MERGE_FLOW_CHART_NODE(DESTINATION)\
				{\
					vint index = merging.Keys().IndexOf(DESTINATION);\
					if (index != -1) DESTINATION = merging.Values()[index];\
				}\

				FOREACH(Ptr<FlowChartNode>, node, flowChart->nodes)
				{
					if (!mergableNodes.Contains(node.Obj()))
					{
						MERGE_FLOW_CHART_NODE(node->destination);
						MERGE_FLOW_CHART_NODE(node->exceptionDestination);
						MERGE_FLOW_CHART_NODE(node->pauseDestination);
						FOREACH(Ptr<FlowChartBranch>, branch, node->branches)
						{
							MERGE_FLOW_CHART_NODE(branch->destination);
						}
					}
				}
				MERGE_FLOW_CHART_NODE(flowChart->headNode);
				MERGE_FLOW_CHART_NODE(flowChart->lastNode);

#undef MERGE_FLOW_CHART_NODE

				vint headNodeIndex = keepingNodes.IndexOf(flowChart->headNode);
				if (headNodeIndex != 0)
				{
					auto headNode = keepingNodes[headNodeIndex];
					keepingNodes.RemoveAt(headNodeIndex);
					keepingNodes.Insert(0, headNode);
				}
				CopyFrom(flowChart->nodes, keepingNodes);
			}

			void RemoveUnnecessaryNodes(Ptr<FlowChart> flowChart)
			{
				RemoveUnnecessaryNodesPass(flowChart);
				FOREACH(Ptr<FlowChartNode>, node, flowChart->nodes)
				{
					if (node->pauseDestination && node->statements.Count() > 0 && node->statements[node->statements.Count() - 1].Cast<WfCoPauseStatement>())
					{
						node->destination = nullptr;
					}
				}
				RemoveUnnecessaryNodesPass(flowChart);
			}

/***********************************************************************
ExpandNewCoroutineExpression
***********************************************************************/

			void ExpandNewCoroutineExpression(WfLexicalScopeManager* manager, WfNewCoroutineExpression* node)
			{
				List<WfStatement*> awaredStatements;
				List<WfVariableStatement*> awaredVariables;
				Dictionary<WfLexicalSymbol*, WString> referenceRenaming;

				FindCoroutineAwaredStatements(node->statement, awaredStatements);
				FOREACH(WfStatement*, stat, awaredStatements)
				{
					FindCoroutineAwaredVariables(stat, awaredVariables);
				}
				FindCoroutineReferenceRenaming(manager, awaredStatements, awaredVariables, referenceRenaming);
				auto flowChart = GenerateFlowChart(manager, awaredStatements, awaredVariables, referenceRenaming, node->statement);
				RemoveUnnecessaryNodes(flowChart);

				auto newExpr = MakePtr<WfNewInterfaceExpression>();
				node->expandedExpression = newExpr;
				newExpr->type = GetTypeFromTypeInfo(TypeInfoRetriver<Ptr<ICoroutine>>::CreateTypeInfo().Obj());

				/////////////////////////////////////////////////////////////////////////////
				// Coroutine Awared Variables
				/////////////////////////////////////////////////////////////////////////////

				FOREACH(WfLexicalSymbol*, symbol,
					From(referenceRenaming.Keys())
						.OrderBy([&](WfLexicalSymbol* a, WfLexicalSymbol* b)
						{
							return WString::Compare(referenceRenaming[a], referenceRenaming[b]);
						}))
				{
					auto varDecl = MakePtr<WfVariableDeclaration>();
					newExpr->declarations.Add(varDecl);
					{
						auto member = MakePtr<WfClassMember>();
						member->kind = WfClassMemberKind::Normal;
						varDecl->classMember = member;
					}

					varDecl->name.value = referenceRenaming[symbol];
					varDecl->type = GetTypeFromTypeInfo(symbol->typeInfo.Obj());
					varDecl->expression = CreateDefaultValue(symbol->typeInfo.Obj());
				}

				/////////////////////////////////////////////////////////////////////////////
				// <co-state>
				/////////////////////////////////////////////////////////////////////////////

				{
					auto varDecl = MakePtr<WfVariableDeclaration>();
					newExpr->declarations.Add(varDecl);
					{
						auto member = MakePtr<WfClassMember>();
						member->kind = WfClassMemberKind::Normal;
						varDecl->classMember = member;
					}

					varDecl->name.value = L"<co-state>";
					varDecl->type = GetTypeFromTypeInfo(TypeInfoRetriver<vint>::CreateTypeInfo().Obj());

					auto stateExpr = MakePtr<WfIntegerExpression>();
					stateExpr->value.value = L"0";
					varDecl->expression = stateExpr;
				}

				/////////////////////////////////////////////////////////////////////////////
				// prop Failure : Exception^ {const, not observe}
				/////////////////////////////////////////////////////////////////////////////

				{
					auto propDecl = MakePtr<WfAutoPropertyDeclaration>();
					newExpr->declarations.Add(propDecl);
					{
						auto member = MakePtr<WfClassMember>();
						member->kind = WfClassMemberKind::Override;
						propDecl->classMember = member;
					}

					propDecl->name.value = L"Failure";
					propDecl->type = GetTypeFromTypeInfo(TypeInfoRetriver<Ptr<IValueException>>::CreateTypeInfo().Obj());
					propDecl->configConst = WfAPConst::Readonly;
					propDecl->configObserve = WfAPObserve::NotObservable;

					auto nullExpr = MakePtr<WfLiteralExpression>();
					nullExpr->value = WfLiteralValue::Null;
					propDecl->expression = nullExpr;
				}

				/////////////////////////////////////////////////////////////////////////////
				// prop Status : CoroutineStatus^ {const, not observe}
				/////////////////////////////////////////////////////////////////////////////

				{
					auto propDecl = MakePtr<WfAutoPropertyDeclaration>();
					newExpr->declarations.Add(propDecl);
					{
						auto member = MakePtr<WfClassMember>();
						member->kind = WfClassMemberKind::Override;
						propDecl->classMember = member;
					}

					propDecl->name.value = L"Status";
					propDecl->type = GetTypeFromTypeInfo(TypeInfoRetriver<CoroutineStatus>::CreateTypeInfo().Obj());
					propDecl->configConst = WfAPConst::Readonly;
					propDecl->configObserve = WfAPObserve::NotObservable;

					auto refExpr = MakePtr<WfReferenceExpression>();
					refExpr->name.value = L"Waiting";

					propDecl->expression = refExpr;
				}

				/////////////////////////////////////////////////////////////////////////////
				// GenerateSetStatus
				/////////////////////////////////////////////////////////////////////////////

				auto GenerateSetStatus = [&](const WString& status)
				{
					auto refExpr = MakePtr<WfReferenceExpression>();
					refExpr->name.value = status;

					auto funcExpr = MakePtr<WfReferenceExpression>();
					funcExpr->name.value = L"SetStatus";

					auto callExpr = MakePtr<WfCallExpression>();
					callExpr->function = funcExpr;
					callExpr->arguments.Add(refExpr);

					auto stat = MakePtr<WfExpressionStatement>();
					stat->expression = callExpr;
					
					return stat;
				};

				/////////////////////////////////////////////////////////////////////////////
				// GenerateSetCoState
				/////////////////////////////////////////////////////////////////////////////

				auto GenerateSetCoState = [&](FlowChartNode* node)
				{
					auto refState = MakePtr<WfReferenceExpression>();
					refState->name.value = L"<co-state>";

					auto intState = MakePtr<WfIntegerExpression>();
					intState->value.value = itow(flowChart->nodes.IndexOf(node));

					auto assignExpr = MakePtr<WfBinaryExpression>();
					assignExpr->op = WfBinaryOperator::Assign;
					assignExpr->first = refState;
					assignExpr->second = intState;

					auto stat = MakePtr<WfExpressionStatement>();
					stat->expression = assignExpr;

					return stat;
				};

				/////////////////////////////////////////////////////////////////////////////
				// func Resume(<raise-exception> : bool) : void
				/////////////////////////////////////////////////////////////////////////////

				{
					auto funcDecl = MakePtr<WfFunctionDeclaration>();
					newExpr->declarations.Add(funcDecl);
					{
						auto member = MakePtr<WfClassMember>();
						member->kind = WfClassMemberKind::Override;
						funcDecl->classMember = member;
					}

					funcDecl->name.value = L"Resume";
					funcDecl->returnType = GetTypeFromTypeInfo(TypeInfoRetriver<void>::CreateTypeInfo().Obj());
					{
						auto argument = MakePtr<WfFunctionArgument>();
						funcDecl->arguments.Add(argument);
						argument->name.value = L"<raise-exception>";
						argument->type = GetTypeFromTypeInfo(TypeInfoRetriver<bool>::CreateTypeInfo().Obj());
					}

					auto block = MakePtr<WfBlockStatement>();
					funcDecl->statement = block;

					/////////////////////////////////////////////////////////////////////////////
					// if (Status != Waiting) raise "...";
					/////////////////////////////////////////////////////////////////////////////

					{
						auto ifStat = MakePtr<WfIfStatement>();
						{
							auto refStatus = MakePtr<WfReferenceExpression>();
							refStatus->name.value = L"Status";

							auto waitingStatus = MakePtr<WfReferenceExpression>();
							waitingStatus->name.value = L"Waiting";

							auto inferExpr = MakePtr<WfInferExpression>();
							inferExpr->expression = waitingStatus;
							inferExpr->type = GetTypeFromTypeInfo(TypeInfoRetriver<CoroutineStatus>::CreateTypeInfo().Obj());

							auto compExpr = MakePtr<WfBinaryExpression>();
							compExpr->op = WfBinaryOperator::NE;
							compExpr->first = refStatus;
							compExpr->second = inferExpr;

							ifStat->expression = compExpr;
						}

						auto trueBlock = MakePtr<WfBlockStatement>();
						ifStat->trueBranch = trueBlock;
						{
							auto exExpr = MakePtr<WfStringExpression>();
							exExpr->value.value = L"Resume should be called only when the coroutine is in the waiting status.";

							auto raiseStat = MakePtr<WfRaiseExceptionStatement>();
							raiseStat->expression = exExpr;
							trueBlock->statements.Add(raiseStat);
						}
						block->statements.Add(ifStat);
					}

					/////////////////////////////////////////////////////////////////////////////
					// SetStatus(Executing);
					/////////////////////////////////////////////////////////////////////////////

					block->statements.Add(GenerateSetStatus(L"Executing"));
					{
						/////////////////////////////////////////////////////////////////////////////
						// try { ... }
						/////////////////////////////////////////////////////////////////////////////

						auto tryStat = MakePtr<WfTryStatement>();
						auto tryBlock = MakePtr<WfBlockStatement>();
						tryStat->protectedStatement = tryBlock;
						{
							/////////////////////////////////////////////////////////////////////////////
							// while (true) { ... }
							/////////////////////////////////////////////////////////////////////////////

							auto whileStat = MakePtr<WfWhileStatement>();
							{
								auto trueExpr = MakePtr<WfLiteralExpression>();
								trueExpr->value = WfLiteralValue::True;
								whileStat->condition = trueExpr;
							}

							auto whileBlock = MakePtr<WfBlockStatement>();
							whileStat->statement = whileBlock;

							Ptr<WfStatement> firstIfStat;
							Ptr<WfStatement>* lastStat = &firstIfStat;
							for (vint i = 0; i < flowChart->nodes.Count(); i++)
							{
								auto flowChartNode = flowChart->nodes[i].Obj();

								/////////////////////////////////////////////////////////////////////////////
								// if (<co-state> == THE_CURRENT_STATE) { ... }
								/////////////////////////////////////////////////////////////////////////////

								auto ifStat = MakePtr<WfIfStatement>();
								{
									auto refState = MakePtr<WfReferenceExpression>();
									refState->name.value = L"<co-state>";

									auto intState = MakePtr<WfIntegerExpression>();
									intState->value.value = itow(i);

									auto compExpr = MakePtr<WfBinaryExpression>();
									compExpr->op = WfBinaryOperator::EQ;
									compExpr->first = refState;
									compExpr->second = intState;

									ifStat->expression = compExpr;
								}

								auto stateBlock = MakePtr<WfBlockStatement>();
								ifStat->trueBranch = stateBlock;
								{
									if (flowChartNode->action == FlowChartNodeAction::SetPause)
									{
										/////////////////////////////////////////////////////////////////////////////
										// <co-state> = THE_NEXT_STATE;
										// SetStatus(Waiting);
										/////////////////////////////////////////////////////////////////////////////
										stateBlock->statements.Add(GenerateSetStatus(L"Waiting"));
										stateBlock->statements.Add(GenerateSetCoState(flowChartNode->pauseDestination));
									}

									auto nodeBlock = stateBlock;
									if (flowChartNode->exceptionDestination)
									{
										/////////////////////////////////////////////////////////////////////////////
										// try { ... }
										/////////////////////////////////////////////////////////////////////////////

										auto nodeTryStat = MakePtr<WfTryStatement>();
										nodeBlock = MakePtr<WfBlockStatement>();
										nodeTryStat->protectedStatement = nodeBlock;

										/////////////////////////////////////////////////////////////////////////////
										// catch(<co-ex>)
										// {
										//      THE_EXCEPTION_VARIABLE = <co-ex>;
										//      <co-state> = THE_EXCEPTION_STATE;
										//      continue;
										// }
										/////////////////////////////////////////////////////////////////////////////

										nodeTryStat->name.value = L"<co-ex>";
										auto catchBlock = MakePtr<WfBlockStatement>();
										nodeTryStat->catchStatement = catchBlock;
										{
											auto refTarget = MakePtr<WfReferenceExpression>();
											refTarget->name.value = referenceRenaming[flowChartNode->exceptionDestination->exceptionVariable];

											auto refEx = MakePtr<WfReferenceExpression>();
											refEx->name.value = L"<co-ex>";

											auto assignExpr = MakePtr<WfBinaryExpression>();
											assignExpr->op = WfBinaryOperator::Assign;
											assignExpr->first = refTarget;
											assignExpr->second = refEx;

											auto stat = MakePtr<WfExpressionStatement>();
											stat->expression = assignExpr;
											catchBlock->statements.Add(stat);
										}
										catchBlock->statements.Add(GenerateSetCoState(flowChartNode->exceptionDestination));
										catchBlock->statements.Add(MakePtr<WfContinueStatement>());

										stateBlock->statements.Add(nodeTryStat);
									}

									bool exited = false;
									FOREACH(Ptr<WfStatement>, stat, flowChartNode->statements)
									{
										if (stat.Cast<WfCoPauseStatement>())
										{
											exited = true;
											/////////////////////////////////////////////////////////////////////////////
											// return;
											/////////////////////////////////////////////////////////////////////////////
											nodeBlock->statements.Add(MakePtr<WfReturnStatement>());
										}
										else if (stat.Cast<WfReturnStatement>())
										{
											exited = true;
											/////////////////////////////////////////////////////////////////////////////
											// SetStatus(Stopped);
											// return;
											/////////////////////////////////////////////////////////////////////////////
											stateBlock->statements.Add(GenerateSetStatus(L"Stopped"));
											{
												auto returnStat = MakePtr<WfReturnStatement>();
												stateBlock->statements.Add(returnStat);
											}
										}
										else
										{
											nodeBlock->statements.Add(stat);
										}
									}

									FOREACH(Ptr<FlowChartBranch>, branch, flowChartNode->branches)
									{
										auto ifStat = MakePtr<WfIfStatement>();
										ifStat->expression = branch->condition;

										auto trueBlock = MakePtr<WfBlockStatement>();
										ifStat->trueBranch = trueBlock;

										trueBlock->statements.Add(GenerateSetCoState(branch->destination));
										trueBlock->statements.Add(MakePtr<WfContinueStatement>());

										nodeBlock->statements.Add(ifStat);
									}

									if (!exited)
									{
										if (flowChartNode == flowChart->lastNode)
										{
											/////////////////////////////////////////////////////////////////////////////
											// SetStatus(Stopped);
											// return;
											/////////////////////////////////////////////////////////////////////////////
											stateBlock->statements.Add(GenerateSetStatus(L"Stopped"));
											{
												auto returnStat = MakePtr<WfReturnStatement>();
												stateBlock->statements.Add(returnStat);
											}
										}
										else
										{
											/////////////////////////////////////////////////////////////////////////////
											// <co-state> = THE_NEXT_STATE;
											// continue;
											/////////////////////////////////////////////////////////////////////////////
											stateBlock->statements.Add(GenerateSetCoState(flowChartNode->destination));
											stateBlock->statements.Add(MakePtr<WfContinueStatement>());
										}
									}
								}

								*lastStat = ifStat;
								lastStat = &ifStat->falseBranch;
							}
							whileBlock->statements.Add(firstIfStat);
							tryBlock->statements.Add(whileStat);
						}

						/////////////////////////////////////////////////////////////////////////////
						// catch(<co-ex>)
						// {
						//      SetFailure(<co-ex>);
						//      SetStatus(Stopped);
						//      if (<raise-exception>) raise <co-ex>;
						// }
						/////////////////////////////////////////////////////////////////////////////

						tryStat->name.value = L"<co-ex>";
						auto catchBlock = MakePtr<WfBlockStatement>();
						tryStat->catchStatement = catchBlock;
						{
							auto refExpr = MakePtr<WfReferenceExpression>();
							refExpr->name.value = L"<co-ex>";

							auto funcExpr = MakePtr<WfReferenceExpression>();
							funcExpr->name.value = L"SetFailure";

							auto callExpr = MakePtr<WfCallExpression>();
							callExpr->function = funcExpr;
							callExpr->arguments.Add(refExpr);

							auto stat = MakePtr<WfExpressionStatement>();
							stat->expression = callExpr;
							catchBlock->statements.Add(stat);
						}
						catchBlock->statements.Add(GenerateSetStatus(L"Stopped"));
						{
							auto refExpr = MakePtr<WfReferenceExpression>();
							refExpr->name.value = L"<raise-exception>";

							auto ifStat = MakePtr<WfIfStatement>();
							ifStat->expression = refExpr;

							auto trueBlock = MakePtr<WfBlockStatement>();
							ifStat->trueBranch = trueBlock;
							{
								auto raiseStat = MakePtr<WfRaiseExceptionStatement>();
								trueBlock->statements.Add(raiseStat);
							}
							catchBlock->statements.Add(ifStat);
						}

						block->statements.Add(tryStat);
					}
				}
			}
		}
	}
}