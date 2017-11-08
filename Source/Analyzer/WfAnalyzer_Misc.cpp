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
				, public empty_visitor::VirtualExpressionVisitor
			{
			public:
				WfLexicalScopeManager*				manager;
				bool								result = false;

				IsExpressionDependOnExpectedTypeVisitor(WfLexicalScopeManager* _manager)
					:manager(_manager)
				{
				}

				void Dispatch(WfVirtualExpression* node)override
				{
					node->Accept(static_cast<empty_visitor::VirtualExpressionVisitor*>(this));
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

				void Visit(WfIfExpression* node)override
				{
					result = Execute(node->trueBranch) && Execute(node->falseBranch);
				}

				void Visit(WfConstructorExpression* node)override
				{
					if (node->arguments.Count() == 0)
					{
						result = true;
					}
					else
					{
						vint possibleFieldCount = 0;
						bool unresolvableField = false;
						auto scope = manager->nodeScopes[node].Obj();

						FOREACH(Ptr<WfConstructorArgument>, argument, node->arguments)
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

			bool IsExpressionDependOnExpectedType(WfLexicalScopeManager* manager, Ptr<WfExpression> expression)
			{
				IsExpressionDependOnExpectedTypeVisitor visitor(manager);
				expression->Accept(&visitor);
				return visitor.result;
			}

/***********************************************************************
GetExpressionName(Expression)
***********************************************************************/

			class GetExpressionNameVisitor
				: public empty_visitor::ExpressionVisitor
				, public empty_visitor::VirtualExpressionVisitor
			{
			public:
				WString								result;

				void Dispatch(WfVirtualExpression* node)override
				{
					node->Accept(static_cast<empty_visitor::VirtualExpressionVisitor*>(this));
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

			WString GetExpressionName(Ptr<WfExpression> expression)
			{
				GetExpressionNameVisitor visitor;
				expression->Accept(&visitor);
				return visitor.result;
			}
		}
	}
}