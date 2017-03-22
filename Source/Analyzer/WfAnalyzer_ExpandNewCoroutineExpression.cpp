#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;

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
					node->expandedStatement->Accept(this);
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
				throw 0;
			}
		}
	}
}