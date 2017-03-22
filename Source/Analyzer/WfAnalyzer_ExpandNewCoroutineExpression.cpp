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
FindCoroutineAwaredStatementVisitor
***********************************************************************/

			class FindCoroutineAwaredStatementVisitor : public empty_visitor::StatementVisitor
			{
			public:
				SortedList<WfStatement*>&				awaredStatements;
				bool									awared = false;

				FindCoroutineAwaredStatementVisitor(SortedList<WfStatement*>& _awaredStatements)
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

/***********************************************************************
FindCoroutineAwaredVariableVisitor
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
			};

			Ptr<FlowChart> GenerateFlowChart(WfLexicalScopeManager* manager, SortedList<WfStatement*>& awaredStatements, List<WfVariableStatement*>& awaredVariables, Ptr<WfStatement> statement)
			{
				throw 0;
			}

/***********************************************************************
GenerateCodeFromFlowChartNode
***********************************************************************/

/***********************************************************************
ExpandNewCoroutineExpression
***********************************************************************/

			void ExpandNewCoroutineExpression(WfLexicalScopeManager* manager, WfNewCoroutineExpression* node)
			{
				SortedList<WfStatement*> awaredStatements;
				List<WfVariableStatement*> awaredVariables;
				{
					FindCoroutineAwaredStatementVisitor visitor(awaredStatements);
					visitor.Call(node->statement);
				}
				{
					FindCoroutineAwaredBlockVisitor visitor(awaredVariables);
					FOREACH(WfStatement*, stat, awaredStatements)
					{
						stat->Accept(&visitor);
					}
				}

				auto newExpr = MakePtr<WfNewInterfaceExpression>();
				node->expandedExpression = newExpr;
				newExpr->type = GetTypeFromTypeInfo(TypeInfoRetriver<Ptr<ICoroutine>>::CreateTypeInfo().Obj());
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