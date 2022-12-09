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

			Ptr<WfExpression> GenerateCoroutineInvalidId()
			{
				auto refOne = Ptr(new WfIntegerExpression);
				refOne->value.value = L"1";

				auto refInvalid = Ptr(new WfUnaryExpression);
				refInvalid->op = WfUnaryOperator::Negative;
				refInvalid->operand = refOne;

				return refInvalid;
			}

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

				void Dispatch(WfVirtualCseStatement* node)override
				{
					awared = Call(node->expandedStatement);
				}

				void Dispatch(WfCoroutineStatement* node)override
				{
					awared = true;
				}

				void Dispatch(WfStateMachineStatement* node)override
				{
					CHECK_FAIL(L"FindCoroutineAwaredStatementVisitor::Visit(WfStateMachineStatement*)#Internal error, state machine statement is not allowed in $coroutine expression.");
				}

				void Visit(WfBreakStatement* node)override
				{
					awared = true;
				}

				void Visit(WfContinueStatement* node)override
				{
					awared = true;
				}

				void Visit(WfReturnStatement* node)override
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
					for (auto stat : node->statements)
					{
						bool a = Call(stat);
						result |= a;
					}
					awared = result;
				}

				void Visit(WfGotoStatement* node)override
				{
					awared = true;
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

				void Dispatch(WfVirtualCseStatement* node)override
				{
					node->expandedStatement->Accept(this);
				}

				void Dispatch(WfCoroutineStatement* node)override
				{
				}

				void Dispatch(WfStateMachineStatement* node)override
				{
					CHECK_FAIL(L"FindCoroutineAwaredVariableVisitor::Visit(WfStateMachineStatement*)#Internal error, state machine statement is not allowed in $coroutine expression.");
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

				void Dispatch(WfVirtualCseStatement* node)override
				{
					// If an virtual node is coroutine awared
					// than its expandedStatement is also in the list
					// no need to find variables again
				}

				void Dispatch(WfCoroutineStatement* node)override
				{
				}

				void Dispatch(WfStateMachineStatement* node)override
				{
					CHECK_FAIL(L"FindCoroutineAwaredBlockVisitor::Visit(WfStateMachineStatement*)#Internal error, state machine statement is not allowed in $coroutine expression.");
				}

				void Visit(WfBlockStatement* node)override
				{
					FindCoroutineAwaredVariableVisitor visitor(awaredVariables);
					for (auto stat : node->statements)
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

				for (auto stat : awaredVariables)
				{
					auto scope = manager->nodeScopes[stat];
					auto symbol = scope->symbols[stat->variable->name.value][0];
					auto name = rename(stat->variable->name.value);
					referenceRenaming.Add(symbol.Obj(), name);
				}

				for (auto stat : awaredStatements)
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
FlowChart
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
				bool									embedInBranch = false;
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
					auto node = Ptr(new FlowChartNode);
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

/***********************************************************************
GenerateFlowChart
***********************************************************************/

			class GenerateFlowChartModuleVisitor : public CopyWithExpandVirtualVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				Dictionary<WfLexicalSymbol*, WString>&	referenceRenaming;

				GenerateFlowChartModuleVisitor(WfLexicalScopeManager* _manager, Dictionary<WfLexicalSymbol*, WString>& _referenceRenaming)
					:CopyWithExpandVirtualVisitor(true)
					, manager(_manager)
					, referenceRenaming(_referenceRenaming)
				{
				}

				void Visit(WfReferenceExpression* node)override
				{
					copy_visitor::AstVisitor::Visit(node);
					vint index = manager->expressionResolvings.Keys().IndexOf(node);
					if (index != -1)
					{
						auto resolvingResult = manager->expressionResolvings.Values()[index];
						vint index = referenceRenaming.Keys().IndexOf(resolvingResult.symbol.Obj());
						if (index != -1)
						{
							result.Cast<WfReferenceExpression>()->name.value = referenceRenaming.Values()[index];
						}
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
					Block,
				};

				struct ScopeContext
				{
					ScopeContext*						parent = nullptr;
					ScopeType							type = ScopeType::Function;
					WString								name;
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

#define COPY_AST_RAW(STATEMENT) GenerateFlowChartModuleVisitor(manager, referenceRenaming).CopyNode(STATEMENT)
#define COPY_AST_SHARED(STATEMENT) COPY_AST_RAW((STATEMENT).Obj())

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

				ScopeContext* InlineScopeExitCode(ScopeType untilScopeType, bool exclusive, const WString name = WString::Empty)
				{
					auto current = scopeContext;
					while (current)
					{
						bool found = false;
						if (current->type == untilScopeType)
						{
							if (name == L"" || current->name == name)
							{
								found = true;
							}
						}

						if (exclusive && found) break;
						if (current->exitStatement)
						{
							AppendAwaredStatement(catchNode, current->exitStatementScope, current->exitStatement);
						}
						if (!exclusive && found) break;
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
						visitor.AppendUnawaredCopiedStatement(catchNode, scopeContext, COPY_AST_SHARED(statement));
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
					resultHead = flowChart->EnsureAppendStatement(headNode, catchNode);
					resultLast = resultHead;

					auto branch = Ptr(new FlowChartBranch);
					branch->destination = targetContext->leaveNode;
					resultLast->branches.Add(branch);
				}

				void Visit(WfContinueStatement* node)override
				{
					auto targetContext = InlineScopeExitCode(ScopeType::Loop, true);
					resultHead = flowChart->EnsureAppendStatement(headNode, catchNode);
					resultLast = resultHead;

					auto branch = Ptr(new FlowChartBranch);
					branch->destination = targetContext->enterNode;
					resultLast->branches.Add(branch);
				}

				void Visit(WfReturnStatement* node)override
				{
					auto targetContext = InlineScopeExitCode(ScopeType::Function, false);
					AppendUnawaredCopiedStatement(catchNode, scopeContext, COPY_AST_RAW(node));

					auto branch = Ptr(new FlowChartBranch);
					branch->destination = targetContext->leaveNode;
					resultLast->branches.Add(branch);
				}

				void Visit(WfDeleteStatement* node)override
				{
					AppendUnawaredCopiedStatement(catchNode, scopeContext, COPY_AST_RAW(node));
				}

				void Visit(WfRaiseExceptionStatement* node)override
				{
					AppendUnawaredCopiedStatement(catchNode, scopeContext, COPY_AST_RAW(node));
				}

				void GenerateIfWithVar(WfIfStatement* node)
				{
					resultHead = flowChart->EnsureAppendStatement(headNode, catchNode);
					{
						auto branch = Ptr(new FlowChartBranch);
						resultHead->branches.Add(branch);

						auto scope = manager->nodeScopes[node];
						auto symbol = scope->symbols[node->name.value][0].Obj();
						{
							auto refExpr = Ptr(new WfReferenceExpression);
							refExpr->name.value = referenceRenaming[symbol];

							auto assignExpr = Ptr(new WfBinaryExpression);
							assignExpr->op = WfBinaryOperator::Assign;
							assignExpr->first = refExpr;
							assignExpr->second = COPY_AST_SHARED(node->expression);

							auto stat = Ptr(new WfExpressionStatement);
							stat->expression = assignExpr;

							SetCodeRange((Ptr<WfStatement>)stat, node->expression->codeRange);
							resultHead->statements.Add(stat);
						}
						{
							auto refExpr = Ptr(new WfReferenceExpression);
							refExpr->name.value = referenceRenaming[symbol];

							auto testExpr = Ptr(new WfTypeTestingExpression);
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
						auto branch = Ptr(new FlowChartBranch);
						resultHead->branches.Add(branch);
						branch->condition = COPY_AST_SHARED(node->expression);

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
						auto branch = Ptr(new FlowChartBranch);
						resultHead->branches.Add(branch);
						branch->condition = COPY_AST_SHARED(node->condition);
					}
					auto loopEnd = flowChart->CreateNode(catchNode);
					{
						auto branch = Ptr(new FlowChartBranch);
						loopEnd->branches.Add(branch);
						branch->condition = COPY_AST_SHARED(node->condition);
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
							auto symbol = Ptr(new WfLexicalSymbol(nullptr));
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

						auto refExpr = Ptr(new WfReferenceExpression);
						refExpr->name.value = referenceRenaming[pair.key->exceptionVariable];

						auto memberExpr = Ptr(new WfMemberExpression);
						memberExpr->parent = refExpr;
						memberExpr->name.value = L"Message";

						auto raiseStat = Ptr(new WfRaiseExceptionStatement);
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
					if (node->endLabel.value == L"")
					{
						resultHead = flowChart->EnsureAppendStatement(headNode, catchNode);
						resultLast = resultHead;

						for (auto [stat, index] : indexed(node->statements))
						{
							auto pair = Execute(resultLast, catchNode, scopeContext, stat);
							resultLast = pair.value;
						}
					}
					else
					{
						resultHead = flowChart->EnsureAppendStatement(headNode, catchNode);
						auto blockEnd = flowChart->CreateNode(catchNode);
						resultLast = resultHead;

						ScopeContext blockContext;
						blockContext.parent = scopeContext;
						blockContext.type = ScopeType::Block;
						blockContext.name = node->endLabel.value;
						blockContext.enterNode = resultHead;
						blockContext.leaveNode = blockEnd;

						for (auto [stat, index] : indexed(node->statements))
						{
							auto pair = Execute(resultLast, catchNode, &blockContext, stat);
							resultLast = pair.value;
						}

						resultLast->destination = blockEnd;
						resultLast = blockEnd;
					}
				}

				void Visit(WfGotoStatement* node)override
				{
					auto targetContext = InlineScopeExitCode(ScopeType::Block, false, node->label.value);
					resultHead = flowChart->EnsureAppendStatement(headNode, catchNode);
					resultLast = resultHead;

					auto branch = Ptr(new FlowChartBranch);
					branch->destination = targetContext->leaveNode;
					resultLast->branches.Add(branch);
				}

				void Visit(WfVariableStatement* node)override
				{
					resultHead = flowChart->EnsureAppendStatement(headNode, catchNode);
					resultLast = resultHead;

					auto scope = manager->nodeScopes[node];
					auto symbol = scope->symbols[node->variable->name.value][0].Obj();

					auto refExpr = Ptr(new WfReferenceExpression);
					refExpr->name.value = referenceRenaming[symbol];

					auto assignExpr = Ptr(new WfBinaryExpression);
					assignExpr->op = WfBinaryOperator::Assign;
					assignExpr->first = refExpr;
					assignExpr->second = COPY_AST_SHARED(node->variable->expression);

					auto stat = Ptr(new WfExpressionStatement);
					stat->expression = assignExpr;

					SetCodeRange((Ptr<WfStatement>)stat, node->codeRange);
					AppendUnawaredCopiedStatement(catchNode, scopeContext, stat);
				}

				void Visit(WfExpressionStatement* node)override
				{
					AppendUnawaredCopiedStatement(catchNode, scopeContext, COPY_AST_RAW(node));
				}

				void Visit(WfVirtualCseStatement* node)override
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
						AppendUnawaredCopiedStatement(catchNode, scopeContext, COPY_AST_SHARED(node->statement));
					}

					resultLast = flowChart->AppendNode(resultLast, catchNode);
					{
						auto pauseStat = Ptr(new WfCoPauseStatement);
						SetCodeRange((Ptr<WfStatement>)pauseStat, node->codeRange);
						resultLast->statements.Add(pauseStat);
					}
					resultLast = flowChart->AppendNode(resultLast, catchNode);
					resultHead->pauseDestination = resultLast;
				}

				void Visit(WfCoOperatorStatement* node)override
				{
					CHECK_FAIL(L"GenerateFlowChartStatementVisitor::Visit(WfCoOperatorStatement*)#Internal error, co-operator statement is not allowed in $coroutine expression.");
				}

				void Visit(WfStateMachineStatement* node)override
				{
					CHECK_FAIL(L"GenerateFlowChartStatementVisitor::Visit(WfStateMachineStatement*)#Internal error, state machine statement is not allowed in $coroutine expression.");
				}

#undef COPY_STATEMENT
			};

			Ptr<FlowChart> GenerateFlowChart(WfLexicalScopeManager* manager, List<WfStatement*>& awaredStatements, List<WfVariableStatement*>& awaredVariables, Dictionary<WfLexicalSymbol*, WString>& referenceRenaming, Ptr<WfStatement> statement)
			{
				auto flowChart = Ptr(new FlowChart);
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

/***********************************************************************
RemoveUnnecessaryNodes
***********************************************************************/

			void CalculateEnterCounts(Ptr<FlowChart> flowChart, Dictionary<FlowChartNode*, vint>& enterCounts)
			{
				const auto& keys = enterCounts.Keys();
				auto& values = const_cast<Dictionary<FlowChartNode*, vint>::ValueContainer&>(enterCounts.Values());

				for (auto node : flowChart->nodes)
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

				for (auto node : flowChart->nodes)
				{
					Inc(node->destination);
					Inc(node->exceptionDestination);
					Inc(node->pauseDestination);
					for (auto branch : node->branches)
					{
						Inc(branch->destination);
					}
				}
			}

			void RemoveUnnecessaryNodesPass(Ptr<FlowChart> flowChart)
			{
				Dictionary<FlowChartNode*, vint> enterCounts;
				CalculateEnterCounts(flowChart, enterCounts);

				SortedList<FlowChartNode*> mergableNodes;
				List<Ptr<FlowChartNode>> keepingNodes;
				for (auto node : flowChart->nodes)
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
				for (auto node : mergableNodes)
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

				for (auto node : flowChart->nodes)
				{
					if (!mergableNodes.Contains(node.Obj()))
					{
						MERGE_FLOW_CHART_NODE(node->destination);
						MERGE_FLOW_CHART_NODE(node->exceptionDestination);
						MERGE_FLOW_CHART_NODE(node->pauseDestination);
						for (auto branch : node->branches)
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

				for (auto node : flowChart->nodes)
				{
					if (node->pauseDestination && node->statements.Count() > 0 && node->statements[node->statements.Count() - 1].Cast<WfCoPauseStatement>())
					{
						node->destination = nullptr;
					}
				}

				Dictionary<FlowChartNode*, vint> enterCounts;
				CalculateEnterCounts(flowChart, enterCounts);
				for (auto node : flowChart->nodes)
				{
					for (auto branch : node->branches)
					{
						if (enterCounts[branch->destination] == 1)
						{
							branch->destination->embedInBranch = true;
						}
					}
				}
			}

/***********************************************************************
GenerateSetStatus
***********************************************************************/

			Ptr<WfStatement> GenerateSetStatus(const WString& status)
			{
				auto refExpr = Ptr(new WfReferenceExpression);
				refExpr->name.value = status;

				auto funcExpr = Ptr(new WfReferenceExpression);
				funcExpr->name.value = L"SetStatus";

				auto callExpr = Ptr(new WfCallExpression);
				callExpr->function = funcExpr;
				callExpr->arguments.Add(refExpr);

				auto stat = Ptr(new WfExpressionStatement);
				stat->expression = callExpr;

				return stat;
			};

/***********************************************************************
GenerateSetCoState
***********************************************************************/

			Ptr<WfStatement> GenerateSetCoStateBeforePause()
			{
				auto refBefore = Ptr(new WfReferenceExpression);
				refBefore->name.value = L"<co-state-before-pause>";

				auto refState = Ptr(new WfReferenceExpression);
				refState->name.value = L"<co-state>";

				auto assignExpr = Ptr(new WfBinaryExpression);
				assignExpr->op = WfBinaryOperator::Assign;
				assignExpr->first = refBefore;
				assignExpr->second = refState;

				auto stat = Ptr(new WfExpressionStatement);
				stat->expression = assignExpr;

				return stat;
			}

			Ptr<WfStatement> GenerateSetCoStateBeforePauseToInvalid()
			{
				auto refBefore = Ptr(new WfReferenceExpression);
				refBefore->name.value = L"<co-state-before-pause>";

				auto assignExpr = Ptr(new WfBinaryExpression);
				assignExpr->op = WfBinaryOperator::Assign;
				assignExpr->first = refBefore;
				assignExpr->second = GenerateCoroutineInvalidId();

				auto stat = Ptr(new WfExpressionStatement);
				stat->expression = assignExpr;

				return stat;
			}

			Ptr<WfStatement> GenerateSetCoState(List<FlowChartNode*>& nodeOrders, FlowChartNode* node)
			{
				auto refState = Ptr(new WfReferenceExpression);
				refState->name.value = L"<co-state>";

				auto intState = Ptr(new WfIntegerExpression);
				intState->value.value = itow(nodeOrders.IndexOf(node));

				auto assignExpr = Ptr(new WfBinaryExpression);
				assignExpr->op = WfBinaryOperator::Assign;
				assignExpr->first = refState;
				assignExpr->second = intState;

				auto stat = Ptr(new WfExpressionStatement);
				stat->expression = assignExpr;

				return stat;
			};

/***********************************************************************
ExpandExceptionDestination
***********************************************************************/

			Ptr<WfBlockStatement> ExpandExceptionDestination(FlowChartNode* catchNode, Dictionary<WfLexicalSymbol*, WString>& referenceRenaming, List<FlowChartNode*>& nodeOrders, Ptr<WfBlockStatement> stateBlock)
			{
				/////////////////////////////////////////////////////////////////////////////
				// try { ... }
				/////////////////////////////////////////////////////////////////////////////

				auto nodeTryStat = Ptr(new WfTryStatement);
				auto nodeBlock = Ptr(new WfBlockStatement);
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
				auto catchBlock = Ptr(new WfBlockStatement);
				nodeTryStat->catchStatement = catchBlock;
				{
					auto refTarget = Ptr(new WfReferenceExpression);
					refTarget->name.value = referenceRenaming[catchNode->exceptionVariable];

					auto refEx = Ptr(new WfReferenceExpression);
					refEx->name.value = L"<co-ex>";

					auto assignExpr = Ptr(new WfBinaryExpression);
					assignExpr->op = WfBinaryOperator::Assign;
					assignExpr->first = refTarget;
					assignExpr->second = refEx;

					auto stat = Ptr(new WfExpressionStatement);
					stat->expression = assignExpr;
					catchBlock->statements.Add(stat);
				}
				catchBlock->statements.Add(GenerateSetCoState(nodeOrders, catchNode));
				catchBlock->statements.Add(Ptr(new WfContinueStatement));

				stateBlock->statements.Add(nodeTryStat);
				return nodeBlock;
			}

/***********************************************************************
ExpandFlowChartNode
***********************************************************************/

			void ExpandFlowChartNode(Ptr<FlowChart> flowChart, FlowChartNode* flowChartNode, Dictionary<WfLexicalSymbol*, WString>& referenceRenaming, List<FlowChartNode*>& nodeOrders, FlowChartNode* parentCatchNode, Ptr<WfBlockStatement> stateBlock)
			{
				if (flowChartNode->action == FlowChartNodeAction::SetPause)
				{
					/////////////////////////////////////////////////////////////////////////////
					// <co-state> = THE_NEXT_STATE;
					// SetStatus(Waiting);
					/////////////////////////////////////////////////////////////////////////////
					stateBlock->statements.Add(GenerateSetStatus(L"Waiting"));
					stateBlock->statements.Add(GenerateSetCoStateBeforePause());
					stateBlock->statements.Add(GenerateSetCoState(nodeOrders, flowChartNode->pauseDestination));
				}

				auto nodeBlock = stateBlock;
				if (flowChartNode->exceptionDestination && parentCatchNode != flowChartNode->exceptionDestination)
				{
					nodeBlock = ExpandExceptionDestination(flowChartNode->exceptionDestination, referenceRenaming, nodeOrders, stateBlock);
				}

				bool exited = false;
				for (auto stat : flowChartNode->statements)
				{
					if (stat.Cast<WfCoPauseStatement>())
					{
						exited = true;
						/////////////////////////////////////////////////////////////////////////////
						// return;
						/////////////////////////////////////////////////////////////////////////////
						nodeBlock->statements.Add(Ptr(new WfReturnStatement));
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
							auto returnStat = Ptr(new WfReturnStatement);
							stateBlock->statements.Add(returnStat);
						}
					}
					else
					{
						if (stat.Cast<WfRaiseExceptionStatement>())
						{
							exited = true;
						}
						nodeBlock->statements.Add(stat);
					}
				}

				for (auto branch : flowChartNode->branches)
				{
					Ptr<WfBlockStatement> trueBlock;

					if (branch->condition)
					{
						auto ifStat = Ptr(new WfIfStatement);
						ifStat->expression = branch->condition;

						trueBlock = Ptr(new WfBlockStatement);
						ifStat->trueBranch = trueBlock;

						nodeBlock->statements.Add(ifStat);
					}
					else
					{
						trueBlock = nodeBlock;
					}

					if (branch->destination->embedInBranch)
					{
						ExpandFlowChartNode(flowChart, branch->destination, referenceRenaming, nodeOrders, flowChartNode->exceptionDestination, trueBlock);
					}
					else
					{
						trueBlock->statements.Add(GenerateSetCoState(nodeOrders, branch->destination));
						trueBlock->statements.Add(Ptr(new WfContinueStatement));
					}
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
							auto returnStat = Ptr(new WfReturnStatement);
							stateBlock->statements.Add(returnStat);
						}
					}
					else
					{
						/////////////////////////////////////////////////////////////////////////////
						// <co-state> = THE_NEXT_STATE;
						// continue;
						/////////////////////////////////////////////////////////////////////////////
						stateBlock->statements.Add(GenerateSetCoState(nodeOrders, flowChartNode->destination));
						stateBlock->statements.Add(Ptr(new WfContinueStatement));
					}
				}
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
				for (auto stat : awaredStatements)
				{
					FindCoroutineAwaredVariables(stat, awaredVariables);
				}
				FindCoroutineReferenceRenaming(manager, awaredStatements, awaredVariables, referenceRenaming);
				auto flowChart = GenerateFlowChart(manager, awaredStatements, awaredVariables, referenceRenaming, node->statement);
				RemoveUnnecessaryNodes(flowChart);

				List<FlowChartNode*> nodeOrders;
				CopyFrom(
					nodeOrders,
					From(flowChart->nodes)
						.Select([](Ptr<FlowChartNode> node)
						{
							return node.Obj();
						})
						.Where([](FlowChartNode* node)
						{
							return !node->embedInBranch;
						})
				);

				auto newExpr = Ptr(new WfNewInterfaceExpression);
				node->expandedExpression = newExpr;
				newExpr->type = GetTypeFromTypeInfo(TypeInfoRetriver<Ptr<ICoroutine>>::CreateTypeInfo().Obj());

				/////////////////////////////////////////////////////////////////////////////
				// Coroutine Awared Variables
				/////////////////////////////////////////////////////////////////////////////

				for (auto symbol : From(referenceRenaming.Keys())
					.OrderBy([&](WfLexicalSymbol* a, WfLexicalSymbol* b)
					{
						return WString::Compare(referenceRenaming[a], referenceRenaming[b]);
					}))
				{
					auto varDecl = Ptr(new WfVariableDeclaration);
					newExpr->declarations.Add(varDecl);
					varDecl->name.value = referenceRenaming[symbol];
					varDecl->type = GetTypeFromTypeInfo(symbol->typeInfo.Obj());
					varDecl->expression = CreateDefaultValue(symbol->typeInfo.Obj());
				}

				/////////////////////////////////////////////////////////////////////////////
				// <co-state>
				/////////////////////////////////////////////////////////////////////////////

				{
					auto varDecl = Ptr(new WfVariableDeclaration);
					newExpr->declarations.Add(varDecl);
					varDecl->name.value = L"<co-state>";
					varDecl->type = GetTypeFromTypeInfo(TypeInfoRetriver<vint>::CreateTypeInfo().Obj());

					auto stateExpr = Ptr(new WfIntegerExpression);
					stateExpr->value.value = L"0";
					varDecl->expression = stateExpr;
				}

				/////////////////////////////////////////////////////////////////////////////
				// <co-state-before-pause>
				/////////////////////////////////////////////////////////////////////////////

				{
					auto varDecl = Ptr(new WfVariableDeclaration);
					newExpr->declarations.Add(varDecl);
					varDecl->name.value = L"<co-state-before-pause>";
					varDecl->type = GetTypeFromTypeInfo(TypeInfoRetriver<vint>::CreateTypeInfo().Obj());
					varDecl->expression = GenerateCoroutineInvalidId();
				}

				/////////////////////////////////////////////////////////////////////////////
				// prop Failure : Exception^ {const, not observe}
				/////////////////////////////////////////////////////////////////////////////

				{
					auto propDecl = Ptr(new WfAutoPropertyDeclaration);
					newExpr->declarations.Add(propDecl);
					propDecl->functionKind = WfFunctionKind::Override;
					propDecl->name.value = L"Failure";
					propDecl->type = GetTypeFromTypeInfo(TypeInfoRetriver<Ptr<IValueException>>::CreateTypeInfo().Obj());
					propDecl->configConst = WfAPConst::Readonly;
					propDecl->configObserve = WfAPObserve::NotObservable;

					auto nullExpr = Ptr(new WfLiteralExpression);
					nullExpr->value = WfLiteralValue::Null;
					propDecl->expression = nullExpr;
				}

				/////////////////////////////////////////////////////////////////////////////
				// prop Status : CoroutineStatus^ {const, not observe}
				/////////////////////////////////////////////////////////////////////////////

				{
					auto propDecl = Ptr(new WfAutoPropertyDeclaration);
					newExpr->declarations.Add(propDecl);
					propDecl->functionKind = WfFunctionKind::Override;
					propDecl->name.value = L"Status";
					propDecl->type = GetTypeFromTypeInfo(TypeInfoRetriver<CoroutineStatus>::CreateTypeInfo().Obj());
					propDecl->configConst = WfAPConst::Readonly;
					propDecl->configObserve = WfAPObserve::NotObservable;

					auto refExpr = Ptr(new WfReferenceExpression);
					refExpr->name.value = L"Waiting";

					propDecl->expression = refExpr;
				}

				/////////////////////////////////////////////////////////////////////////////
				// func Resume(<raise-exception> : bool, <coroutine-output> : CoroutineResult^) : void
				/////////////////////////////////////////////////////////////////////////////

				WString coResultName = node->name.value == L"" ? L"<coroutine-output>" : node->name.value;
				{
					auto funcDecl = Ptr(new WfFunctionDeclaration);
					newExpr->declarations.Add(funcDecl);
					funcDecl->functionKind = WfFunctionKind::Override;
					funcDecl->anonymity = WfFunctionAnonymity::Named;
					funcDecl->name.value = L"Resume";
					funcDecl->returnType = GetTypeFromTypeInfo(TypeInfoRetriver<void>::CreateTypeInfo().Obj());
					{
						auto argument = Ptr(new WfFunctionArgument);
						funcDecl->arguments.Add(argument);
						argument->name.value = L"<raise-exception>";
						argument->type = GetTypeFromTypeInfo(TypeInfoRetriver<bool>::CreateTypeInfo().Obj());
					}
					{
						auto argument = Ptr(new WfFunctionArgument);
						funcDecl->arguments.Add(argument);
						argument->name.value = coResultName;
						argument->type = GetTypeFromTypeInfo(TypeInfoRetriver<Ptr<CoroutineResult>>::CreateTypeInfo().Obj());
					}

					auto block = Ptr(new WfBlockStatement);
					funcDecl->statement = block;

					/////////////////////////////////////////////////////////////////////////////
					// if (Status != Waiting) raise "...";
					/////////////////////////////////////////////////////////////////////////////

					{
						auto ifStat = Ptr(new WfIfStatement);
						{
							auto refStatus = Ptr(new WfReferenceExpression);
							refStatus->name.value = L"Status";

							auto waitingStatus = Ptr(new WfChildExpression);
							waitingStatus->parent = GetExpressionFromTypeDescriptor(description::GetTypeDescriptor<CoroutineStatus>());
							waitingStatus->name.value = L"Waiting";

							auto compExpr = Ptr(new WfBinaryExpression);
							compExpr->op = WfBinaryOperator::NE;
							compExpr->first = refStatus;
							compExpr->second = waitingStatus;

							ifStat->expression = compExpr;
						}

						auto trueBlock = Ptr(new WfBlockStatement);
						ifStat->trueBranch = trueBlock;
						{
							auto exExpr = Ptr(new WfStringExpression);
							exExpr->value.value = L"Resume should be called only when the coroutine is in the waiting status.";

							auto raiseStat = Ptr(new WfRaiseExceptionStatement);
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

						auto tryStat = Ptr(new WfTryStatement);
						auto tryBlock = Ptr(new WfBlockStatement);
						tryStat->protectedStatement = tryBlock;
						{
							/////////////////////////////////////////////////////////////////////////////
							// if (<co-state-before-pause != -1) { ... }
							/////////////////////////////////////////////////////////////////////////////

							auto trueBlock = Ptr(new WfBlockStatement);
							{
								auto refBefore = Ptr(new WfReferenceExpression);
								refBefore->name.value = L"<co-state-before-pause>";

								auto compareInput = Ptr(new WfBinaryExpression);
								compareInput->op = WfBinaryOperator::NE;
								compareInput->first = refBefore;
								compareInput->second = GenerateCoroutineInvalidId();

								auto ifStat = Ptr(new WfIfStatement);
								tryBlock->statements.Add(ifStat);
								ifStat->expression = compareInput;
								ifStat->trueBranch = trueBlock;
							}

							/////////////////////////////////////////////////////////////////////////////
							// if (<coroutine-output> is null) { <co-state-before-pause> = -1; }
							// else if (<coroutine-output>.Failure is null) { <co-state-before-pause> = -1; }
							// else { ... }
							/////////////////////////////////////////////////////////////////////////////
							{
								auto nullTrueBlock = Ptr(new WfBlockStatement);
								Ptr<WfStatement>* nullFalseBlock = nullptr;
								auto exTrueBlock = Ptr(new WfBlockStatement);
								auto exFalseBlock = Ptr(new WfBlockStatement);

								{
									auto refOutput = Ptr(new WfReferenceExpression);
									refOutput->name.value = coResultName;

									auto testNullExpr = Ptr(new WfTypeTestingExpression);
									testNullExpr->test = WfTypeTesting::IsNull;
									testNullExpr->expression = refOutput;

									auto ifStat = Ptr(new WfIfStatement);
									trueBlock->statements.Add(ifStat);
									ifStat->expression = testNullExpr;

									ifStat->trueBranch = nullTrueBlock;
									nullTrueBlock->statements.Add(GenerateSetCoStateBeforePauseToInvalid());
									nullFalseBlock = &ifStat->falseBranch;
								}
								{
									auto refOutput = Ptr(new WfReferenceExpression);
									refOutput->name.value = coResultName;

									auto refFailure = Ptr(new WfMemberExpression);
									refFailure->parent = refOutput;
									refFailure->name.value = L"Failure";

									auto testNullExpr = Ptr(new WfTypeTestingExpression);
									testNullExpr->test = WfTypeTesting::IsNull;
									testNullExpr->expression = refFailure;

									auto ifStat = Ptr(new WfIfStatement);
									*nullFalseBlock = ifStat;
									ifStat->expression = testNullExpr;

									ifStat->trueBranch = exTrueBlock;
									exTrueBlock->statements.Add(GenerateSetCoStateBeforePauseToInvalid());
									ifStat->falseBranch = exFalseBlock;
								}
								{
									/////////////////////////////////////////////////////////////////////////////
									// Handle exception from <coroutine-output>
									/////////////////////////////////////////////////////////////////////////////

									Ptr<WfStatement> firstIfStatement;
									Ptr<WfStatement>* currentIfVar = &firstIfStatement;

									using GroupPair = Pair<FlowChartNode*, LazyList<FlowChartNode*>>;
									auto nodeByCatches = From(nodeOrders)
										.GroupBy([](FlowChartNode* node)
										{
											return node->exceptionDestination;
										})
										.OrderBy([&](const GroupPair& p1, const GroupPair& p2)
										{
											return nodeOrders.IndexOf(p1.key) - nodeOrders.IndexOf(p2.key);
										});

									for (auto group : nodeByCatches)
									{
										auto catchNode = group.key;
										if (!catchNode) continue;

										Ptr<WfExpression> condition;
										{
											List<Tuple<vint, vint>> conditionRanges;
											for (auto flowChartNode : group.value)
											{
												vint state = nodeOrders.IndexOf(flowChartNode);
												if (conditionRanges.Count() == 0)
												{
													conditionRanges.Add({ state,state });
												}
												else if (conditionRanges[conditionRanges.Count() - 1].f1 == state - 1)
												{
													conditionRanges[conditionRanges.Count() - 1].f1 = state;
												}
												else
												{
													conditionRanges.Add({ state,state });
												}
											}

											for (vint i = 0; i < conditionRanges.Count(); i++)
											{
												auto range = conditionRanges[i];

												Ptr<WfExpression> singleCondition;
												if (range.f0 == range.f1)
												{
													auto refState = Ptr(new WfReferenceExpression);
													refState->name.value = L"<co-state-before-pause>";

													auto intState = Ptr(new WfIntegerExpression);
													intState->value.value = itow(range.f0);

													auto compExpr = Ptr(new WfBinaryExpression);
													compExpr->op = WfBinaryOperator::EQ;
													compExpr->first = refState;
													compExpr->second = intState;

													singleCondition = compExpr;
												}
												else
												{
													auto refState = Ptr(new WfReferenceExpression);
													refState->name.value = L"<co-state-before-pause>";

													auto intState1 = Ptr(new WfIntegerExpression);
													intState1->value.value = itow(range.f0);

													auto intState2 = Ptr(new WfIntegerExpression);
													intState2->value.value = itow(range.f1);

													auto rangeExpr = Ptr(new WfRangeExpression);
													rangeExpr->begin = intState1;
													rangeExpr->beginBoundary = WfRangeBoundary::Inclusive;
													rangeExpr->end = intState2;
													rangeExpr->endBoundary = WfRangeBoundary::Inclusive;

													auto compExpr = Ptr(new WfSetTestingExpression);
													compExpr->test = WfSetTesting::In;
													compExpr->element = refState;
													compExpr->collection = rangeExpr;

													singleCondition = compExpr;
												}

												if (condition)
												{
													auto orExpr = Ptr(new WfBinaryExpression);
													orExpr->op = WfBinaryOperator::Or;
													orExpr->first = condition;
													orExpr->second = singleCondition;
													condition = orExpr;
												}
												else
												{
													condition = singleCondition;
												}
											}
										}

										auto ifStat = Ptr(new WfIfStatement);
										*currentIfVar = ifStat;
										currentIfVar = &ifStat->falseBranch;
										ifStat->expression = condition;

										auto catchBlock = Ptr(new WfBlockStatement);
										ifStat->trueBranch = catchBlock;

										catchBlock->statements.Add(GenerateSetCoStateBeforePauseToInvalid());
										{
											auto refTarget = Ptr(new WfReferenceExpression);
											refTarget->name.value = referenceRenaming[catchNode->exceptionVariable];

											auto refOutput = Ptr(new WfReferenceExpression);
											refOutput->name.value = coResultName;

											auto refFailure = Ptr(new WfMemberExpression);
											refFailure->parent = refOutput;
											refFailure->name.value = L"Failure";

											auto assignExpr = Ptr(new WfBinaryExpression);
											assignExpr->op = WfBinaryOperator::Assign;
											assignExpr->first = refTarget;
											assignExpr->second = refFailure;

											auto stat = Ptr(new WfExpressionStatement);
											stat->expression = assignExpr;
											catchBlock->statements.Add(stat);
										}
										catchBlock->statements.Add(GenerateSetCoState(nodeOrders, catchNode));
									}

									{
										auto block = Ptr(new WfBlockStatement);
										*currentIfVar = block;

										block->statements.Add(GenerateSetCoStateBeforePauseToInvalid());
										{
											auto refOutput = Ptr(new WfReferenceExpression);
											refOutput->name.value = coResultName;

											auto refFailure = Ptr(new WfMemberExpression);
											refFailure->parent = refOutput;
											refFailure->name.value = L"Failure";

											auto raiseStat = Ptr(new WfRaiseExceptionStatement);
											raiseStat->expression = refFailure;

											block->statements.Add(raiseStat);
										}
									}

									exFalseBlock->statements.Add(firstIfStatement);
								}
							}
						}
						{
							/////////////////////////////////////////////////////////////////////////////
							// while (true) { ... }
							/////////////////////////////////////////////////////////////////////////////

							auto whileStat = Ptr(new WfWhileStatement);
							{
								auto trueExpr = Ptr(new WfLiteralExpression);
								trueExpr->value = WfLiteralValue::True;
								whileStat->condition = trueExpr;
							}

							auto whileBlock = Ptr(new WfBlockStatement);
							whileStat->statement = whileBlock;

							using GroupPair = Pair<FlowChartNode*, LazyList<FlowChartNode*>>;
							auto nodeByCatches = From(nodeOrders)
								.GroupBy([](FlowChartNode* node)
								{
									return node->exceptionDestination;
								})
								.OrderBy([&](const GroupPair& p1, const GroupPair& p2)
								{
									return nodeOrders.IndexOf(p1.key) - nodeOrders.IndexOf(p2.key);
								});

							for (auto group : nodeByCatches)
							{
								auto catchNode = group.key;
								auto groupBlock = whileBlock;
								if (catchNode)
								{
									groupBlock = ExpandExceptionDestination(catchNode, referenceRenaming, nodeOrders, whileBlock);
								}

								for (auto flowChartNode : group.value)
								{
									/////////////////////////////////////////////////////////////////////////////
									// if (<co-state> == THE_CURRENT_STATE) { ... }
									/////////////////////////////////////////////////////////////////////////////

									auto ifStat = Ptr(new WfIfStatement);
									groupBlock->statements.Add(ifStat);
									{
										auto refState = Ptr(new WfReferenceExpression);
										refState->name.value = L"<co-state>";

										auto intState = Ptr(new WfIntegerExpression);
										intState->value.value = itow(nodeOrders.IndexOf(flowChartNode));

										auto compExpr = Ptr(new WfBinaryExpression);
										compExpr->op = WfBinaryOperator::EQ;
										compExpr->first = refState;
										compExpr->second = intState;

										ifStat->expression = compExpr;
									}

									auto stateBlock = Ptr(new WfBlockStatement);
									ifStat->trueBranch = stateBlock;
									{
										ExpandFlowChartNode(flowChart, flowChartNode, referenceRenaming, nodeOrders, catchNode, stateBlock);
									}
								}
							}
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
						auto catchBlock = Ptr(new WfBlockStatement);
						tryStat->catchStatement = catchBlock;
						{
							auto refExpr = Ptr(new WfReferenceExpression);
							refExpr->name.value = L"<co-ex>";

							auto funcExpr = Ptr(new WfReferenceExpression);
							funcExpr->name.value = L"SetFailure";

							auto callExpr = Ptr(new WfCallExpression);
							callExpr->function = funcExpr;
							callExpr->arguments.Add(refExpr);

							auto stat = Ptr(new WfExpressionStatement);
							stat->expression = callExpr;
							catchBlock->statements.Add(stat);
						}
						catchBlock->statements.Add(GenerateSetStatus(L"Stopped"));
						{
							auto refExpr = Ptr(new WfReferenceExpression);
							refExpr->name.value = L"<raise-exception>";

							auto ifStat = Ptr(new WfIfStatement);
							ifStat->expression = refExpr;

							auto trueBlock = Ptr(new WfBlockStatement);
							ifStat->trueBranch = trueBlock;
							{
								auto raiseStat = Ptr(new WfRaiseExceptionStatement);
								trueBlock->statements.Add(raiseStat);
							}
							catchBlock->statements.Add(ifStat);
						}

						block->statements.Add(tryStat);
					}
				}
			}

/***********************************************************************
ExpandNewCoroutineExpression
***********************************************************************/

			void ExpandCoOperatorExpression(WfLexicalScopeManager* manager, WfCoOperatorExpression* node)
			{
				auto scope = manager->nodeScopes[node].Obj();
				auto functionScope = scope->FindFunctionScope();
				auto newCoExpr = functionScope->ownerNode.Cast<WfNewCoroutineExpression>();
				auto providerType = manager->coNewCoroutineResolvings[newCoExpr.Obj()].type;

				auto refProvider = GetExpressionFromTypeDescriptor(providerType->GetTypeDescriptor());

				auto refQueryContext = Ptr(new WfChildExpression);
				refQueryContext->parent = refProvider;
				refQueryContext->name.value = L"QueryContext";

				auto refImpl = Ptr(new WfReferenceExpression);
				refImpl->name.value = L"<co-impl>";

				auto refCall = Ptr(new WfCallExpression);
				refCall->function = refQueryContext;
				refCall->arguments.Add(refImpl);

				auto refOperator = Ptr(new WfMemberExpression);
				refOperator->parent = refCall;
				refOperator->name.value = node->name.value;

				node->expandedExpression = refOperator;
			}
		}
	}
}