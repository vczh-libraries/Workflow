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
				Pause,
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
			};

			class FlowChart : public Object
			{
			public:
				List<Ptr<FlowChartNode>>				nodes;
				FlowChartNode*							headNode = nullptr;
				FlowChartNode*							lastNode = nullptr;

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
					else if(head->action == FlowChartNodeAction::Pause || head->branches.Count() > 0 || head->exceptionDestination != catchNode)
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

				void Visit(WfIfStatement* node)override
				{
					resultHead = flowChart->EnsureAppendStatement(headNode, catchNode);
					{
						auto branch = MakePtr<FlowChartBranch>();
						resultHead->branches.Add(branch);
						if (node->name.value == L"")
						{
							branch->condition = COPY_AST(node->expression);
						}
						else
						{
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

								resultHead->statements.Add(stat);
							}
							{
								auto refExpr = MakePtr<WfReferenceExpression>();
								refExpr->name.value = referenceRenaming[symbol];

								auto testExpr = MakePtr<WfTypeTestingExpression>();
								testExpr->test = WfTypeTesting::IsNotNull;
								testExpr->expression = refExpr;

								branch->condition = testExpr;
							}
						}
					}
					resultLast = flowChart->CreateNode(catchNode);

					{
						auto pair = Execute(nullptr, catchNode, scopeContext, node->trueBranch);
						pair.value->destination = resultLast;
						resultHead->branches[0]->destination = pair.key;
					}
					
					if(node->falseBranch)
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

				void Visit(WfWhileStatement* node)override
				{
					throw 0;
				}

				void Visit(WfTryStatement* node)override
				{
					throw 0;
				}

				void Visit(WfBlockStatement* node)override
				{
					FOREACH(Ptr<WfStatement>, stat, node->statements)
					{
						AppendAwaredStatement(catchNode, scopeContext, stat);
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

					resultLast = flowChart->AppendNode(resultLast, catchNode, FlowChartNodeAction::Pause);
					resultHead->pauseDestination = resultLast;
				}

#undef COPY_STATEMENT
			};

			Ptr<FlowChart> GenerateFlowChart(WfLexicalScopeManager* manager, List<WfStatement*>& awaredStatements, List<WfVariableStatement*>& awaredVariables, Dictionary<WfLexicalSymbol*, WString>& referenceRenaming, Ptr<WfStatement> statement)
			{
				auto flowChart = MakePtr<FlowChart>();
				SortedList<WfStatement*> sortedAwaredStatements;
				CopyFrom(sortedAwaredStatements, awaredStatements);

				GenerateFlowChartStatementVisitor::ScopeContext context;
				auto pair = GenerateFlowChartStatementVisitor::Execute(manager, sortedAwaredStatements, referenceRenaming, flowChart, nullptr, nullptr, &context, statement);

				flowChart->headNode = pair.key;
				flowChart->lastNode = pair.value;
				return flowChart;
			}

/***********************************************************************
GenerateCodeFromFlowChartNode
***********************************************************************/

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

				auto newExpr = MakePtr<WfNewInterfaceExpression>();
				node->expandedExpression = newExpr;
				newExpr->type = GetTypeFromTypeInfo(TypeInfoRetriver<Ptr<ICoroutine>>::CreateTypeInfo().Obj());

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

					{
						auto stat = MakePtr<WfRaiseExceptionStatement>();
						block->statements.Add(stat);

						auto strExpr = MakePtr<WfStringExpression>();
						strExpr->value.value = L"Not Implemented!";
						stat->expression = strExpr;
					}
				}
			}
		}
	}
}