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
IsExpressionDependOnExpectedType(Expression)
***********************************************************************/

			class IsExpressionDependOnExpectedTypeVisitor
				: public empty_visitor::ExpressionVisitor
				, public empty_visitor::VirtualCseExpressionVisitor
			{
			public:
				WfLexicalScopeManager*				manager;
				bool								hasExpectedType;
				bool								result = false;

				IsExpressionDependOnExpectedTypeVisitor(WfLexicalScopeManager* _manager, bool _hasExpectedType)
					:manager(_manager)
					, hasExpectedType(_hasExpectedType)
				{
				}

				void Dispatch(WfVirtualCfeExpression* node)override
				{
					node->expandedExpression->Accept(this);
				}

				void Dispatch(WfVirtualCseExpression* node)override
				{
					node->Accept((WfVirtualCseExpression::IVisitor*)this);
				}

				bool Execute(Ptr<WfExpression> expression)
				{
					result = false;
					expression->Accept(this);
					return result;
				}

				void Visit(WfReferenceExpression* node)override
				{
					auto scope = manager->nodeScopes[node].Obj();
					List<ResolveExpressionResult> testResults;
					manager->ResolveName(scope, node->name.value, testResults);
					if (testResults.Count() == 0)
					{
						result = true;
					}
				}

				void Visit(WfOrderedLambdaExpression* node)override
				{
					auto scope = manager->nodeScopes[node].Obj();
					result = scope->symbols.Count() > 0;
				}

				void Visit(WfLiteralExpression* node)override
				{
					if (node->value == WfLiteralValue::Null)
					{
						result = true;
					}
				}

				void Visit(WfBinaryExpression* node)override
				{
					if (node->op == WfBinaryOperator::FlagOr)
					{
						result = Execute(node->first) && Execute(node->second);
					}
				}

				void Visit(WfIfExpression* node)override
				{
					result = Execute(node->trueBranch) && Execute(node->falseBranch);
				}

				void Visit(WfConstructorExpression* node)override
				{
					if (hasExpectedType || node->arguments.Count() == 0)
					{
						result = true;
					}
					else
					{
						vint possibleFieldCount = 0;
						bool unresolvableField = false;
						auto scope = manager->nodeScopes[node].Obj();

						for (auto argument : node->arguments)
						{
							if (argument->value)
							{
								if (auto refExpr = argument->key.Cast<WfReferenceExpression>())
								{
									possibleFieldCount++;
									List<ResolveExpressionResult> testResults;
									manager->ResolveName(scope, refExpr->name.value, testResults);
									if (testResults.Count() == 0)
									{
										unresolvableField = true;
									}
								}
							}
						}

						result = unresolvableField&&possibleFieldCount == node->arguments.Count();
					}
				}

				void Visit(WfExpectedTypeCastExpression* node)override
				{
					result = true;
				}
			};

			bool IsExpressionDependOnExpectedType(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, bool hasExpectedType)
			{
				IsExpressionDependOnExpectedTypeVisitor visitor(manager, hasExpectedType);
				expression->Accept(&visitor);
				return visitor.result;
			}

/***********************************************************************
GetExpressionName(Expression)
***********************************************************************/

			class GetExpressionNameVisitor
				: public empty_visitor::ExpressionVisitor
				, public empty_visitor::VirtualCseExpressionVisitor
			{
			public:
				WString								result;

				void Dispatch(WfVirtualCfeExpression* node)override
				{
					node->expandedExpression->Accept(this);
				}

				void Dispatch(WfVirtualCseExpression* node)override
				{
					node->Accept((WfVirtualCseExpression::IVisitor*)this);
				}

				void Visit(WfTopQualifiedExpression* node)override
				{
					result = node->name.value;
				}

				void Visit(WfReferenceExpression* node)override
				{
					result = node->name.value;
				}

				void Visit(WfOrderedNameExpression* node)override
				{
					result = node->name.value;
				}

				void Visit(WfMemberExpression* node)override
				{
					result = node->name.value;
				}

				void Visit(WfChildExpression* node)override
				{
					result = node->name.value;
				}
			};

			WString GetExpressionName(WfExpression* expression)
			{
				GetExpressionNameVisitor visitor;
				expression->Accept(&visitor);
				return visitor.result;
			}

			WString GetExpressionName(Ptr<WfExpression> expression)
			{
				return GetExpressionName(expression.Obj());
			}
		}
	}
}