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

			class FlowChartNode : public Object
			{
			public:
				List<Ptr<WfStatement>>					statements;
				List<Ptr<FlowChartBranch>>				branches;
				FlowChartNode*							destination = nullptr;
				FlowChartNode*							exceptionDestination = nullptr;
			};

			class FlowChart : public Object
			{
			public:
				List<Ptr<FlowChartNode>>				nodes;
				FlowChartNode*							headNode = nullptr;
				FlowChartNode*							lastNode = nullptr;

				FlowChartNode* CreateNode()
				{
					auto node = MakePtr<FlowChartNode>();
					nodes.Add(node);
					return node.Obj();
				}

				FlowChartNode* EnsureAppendStatement(FlowChartNode* head, FlowChartNode* catchNode)
				{
					if (head == nullptr || head->branches.Count() > 0 || head->exceptionDestination != catchNode)
					{
						CHECK_ERROR(head->destination == nullptr, L"FlowChart::EnsureAppendStatement(FlowChartNode*, FlowChartNode*)#Cannot append statement to a flow chart node that already has a default destination.");
						auto node = CreateNode();
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

			class GenerateFlowChartStatementVisitor : public Object, public WfStatement::IVisitor
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

				static Pair<FlowChartNode*, FlowChartNode*> Execute(WfLexicalScopeManager* manager, SortedList<WfStatement*>& awaredStatements, Dictionary<WfLexicalSymbol*, WString>& referenceRenaming, Ptr<FlowChart> flowChart, FlowChartNode* headNode, FlowChartNode* catchNode, ScopeContext* scopeContext, Ptr<WfStatement> statement)
				{
					GenerateFlowChartStatementVisitor visitor(manager, awaredStatements, referenceRenaming, flowChart, headNode, catchNode, scopeContext);
					statement->Accept(&visitor);
					return{ visitor.resultHead,visitor.resultLast };
				}

				Pair<FlowChartNode*, FlowChartNode*> Execute(FlowChartNode* headNode, FlowChartNode* catchNode, ScopeContext* scopeContext, Ptr<WfStatement> statement)
				{
					return Execute(manager, awaredStatements, referenceRenaming, flowChart, headNode, catchNode, scopeContext, statement);
				}

				void Visit(WfBreakStatement* node)override
				{
				}

				void Visit(WfContinueStatement* node)override
				{
				}

				void Visit(WfReturnStatement* node)override
				{
				}

				void Visit(WfDeleteStatement* node)override
				{
				}

				void Visit(WfRaiseExceptionStatement* node)override
				{
				}

				void Visit(WfIfStatement* node)override
				{
				}

				void Visit(WfWhileStatement* node)override
				{
				}

				void Visit(WfTryStatement* node)override
				{
				}

				void Visit(WfBlockStatement* node)override
				{
				}

				void Visit(WfVariableStatement* node)override
				{
				}

				void Visit(WfExpressionStatement* node)override
				{
				}

				void Visit(WfVirtualStatement* node)override
				{
					node->expandedStatement->Accept(this);
				}

				void Visit(WfCoroutineStatement* node)override
				{
				}
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
						argument->name.value = L"raiseException";
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