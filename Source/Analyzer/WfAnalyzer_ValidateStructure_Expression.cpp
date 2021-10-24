#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace parsing;
			using namespace reflection::description;

/***********************************************************************
ValidateStructure(Expression)
***********************************************************************/

			class ValidateStructureExpressionVisitor
				: public Object
				, public WfExpression::IVisitor
				, public WfVirtualCseExpression::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				ValidateStructureContext*				context;
				Ptr<WfExpression>						result;

				ValidateStructureExpressionVisitor(WfLexicalScopeManager* _manager, ValidateStructureContext* _context)
					:manager(_manager)
					, context(_context)
				{
				}

				void Visit(WfThisExpression* node)override
				{
					if (context->currentBindExpression)
					{
						manager->errors.Add(WfErrors::WrongThisExpression(node));
					}
				}

				void Visit(WfTopQualifiedExpression* node)override
				{
				}

				void Visit(WfReferenceExpression* node)override
				{
				}

				void Visit(WfOrderedNameExpression* node)override
				{
				}

				void Visit(WfOrderedLambdaExpression* node)override
				{
					ValidateStructureContext context;
					ValidateExpressionStructure(manager, &context, node->body);
				}

				void Visit(WfMemberExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->parent);
				}

				void Visit(WfChildExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->parent);
				}

				void Visit(WfLiteralExpression* node)override
				{
				}

				void Visit(WfFloatingExpression* node)override
				{
				}

				void Visit(WfIntegerExpression* node)override
				{
				}

				void Visit(WfStringExpression* node)override
				{
				}

				void Visit(WfUnaryExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->operand);
				}

				void Visit(WfBinaryExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->first);
					ValidateExpressionStructure(manager, context, node->second);
				}

				void Visit(WfLetExpression* node)override
				{
					for (auto variable : node->variables)
					{
						ValidateExpressionStructure(manager, context, variable->value);
					}
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfIfExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->condition);
					ValidateExpressionStructure(manager, context, node->trueBranch);
					ValidateExpressionStructure(manager, context, node->falseBranch);
				}

				void Visit(WfRangeExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->begin);
					ValidateExpressionStructure(manager, context, node->end);
				}

				void Visit(WfSetTestingExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->element);
					ValidateExpressionStructure(manager, context, node->collection);
				}

				void Visit(WfConstructorExpression* node)override
				{
					vint listElementCount = 0;
					vint mapElementCount = 0;
					for (auto argument : node->arguments)
					{
						ValidateExpressionStructure(manager, context, argument->key);
						if (argument->value)
						{
							ValidateExpressionStructure(manager, context, argument->value);
							mapElementCount++;
						}
						else
						{
							listElementCount++;
						}
					}

					if (listElementCount*mapElementCount != 0)
					{
						manager->errors.Add(WfErrors::ConstructorMixMapAndList(node));
					}
				}

				void Visit(WfInferExpression* node)override
				{
					ValidateTypeStructure(manager, node->type);
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfTypeCastingExpression* node)override
				{
					ValidateTypeStructure(manager, node->type);
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfTypeTestingExpression* node)override
				{
					if (node->type)
					{
						ValidateTypeStructure(manager, node->type);
					}
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfTypeOfTypeExpression* node)override
				{
					ValidateTypeStructure(manager, node->type);
				}

				void Visit(WfTypeOfExpressionExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfAttachEventExpression* node)override
				{
					if (context->currentBindExpression)
					{
						manager->errors.Add(WfErrors::AttachInBind(node));
					}
					ValidateExpressionStructure(manager, context, node->event);
					ValidateExpressionStructure(manager, context, node->function);
				}

				void Visit(WfDetachEventExpression* node)override
				{
					if (context->currentBindExpression)
					{
						manager->errors.Add(WfErrors::DetachInBind(node));
					}
					ValidateExpressionStructure(manager, context, node->event);
					ValidateExpressionStructure(manager, context, node->handler);
				}

				void Visit(WfObserveExpression* node)override
				{
					if (!context->currentBindExpression)
					{
						manager->errors.Add(WfErrors::ObserveNotInBind(node));
					}
					if (context->currentObserveExpression)
					{
						manager->errors.Add(WfErrors::ObserveInObserveEvent(node));
					}

					if (node->observeType == WfObserveType::SimpleObserve)
					{
						if (!node->expression.Cast<WfReferenceExpression>())
						{
							manager->errors.Add(WfErrors::WrongSimpleObserveExpression(node->expression.Obj()));
						}
						for (auto event : node->events)
						{
							if (!event.Cast<WfReferenceExpression>())
							{
								manager->errors.Add(WfErrors::WrongSimpleObserveEvent(event.Obj()));
							}
						}
					}
					else if (node->events.Count() == 0)
					{
						manager->errors.Add(WfErrors::EmptyObserveEvent(node));
					}

					ValidateExpressionStructure(manager, context, node->parent);
					ValidateExpressionStructure(manager, context, node->expression);
					context->currentObserveExpression = node;
					for (vint i = 0; i < node->events.Count(); i++)
					{
						ValidateExpressionStructure(manager, context, node->events[i]);
					}
					context->currentObserveExpression = 0;
				}

				void Visit(WfCallExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->function);
					for (vint i = 0; i < node->arguments.Count(); i++)
					{
						ValidateExpressionStructure(manager, context, node->arguments[i]);
					}
				}

				void Visit(WfFunctionExpression* node)override
				{
					ValidateDeclarationStructure(manager, node->function, nullptr, node);
				}

				void Visit(WfNewClassExpression* node)override
				{
					ValidateTypeStructure(manager, node->type);
					for (vint i = 0; i < node->arguments.Count(); i++)
					{
						ValidateExpressionStructure(manager, context, node->arguments[i]);
					}
				}

				void Visit(WfNewInterfaceExpression* node)override
				{
					ValidateTypeStructure(manager, node->type);
					for (auto memberDecl : node->declarations)
					{
						ValidateDeclarationStructure(manager, memberDecl, nullptr, node);
					}
				}

				void Visit(WfVirtualCfeExpression* node)override
				{
					if (node->expandedExpression)
					{
						ValidateExpressionStructure(manager, context, node->expandedExpression);
					}
				}

				void Visit(WfVirtualCseExpression* node)override
				{
					node->Accept((WfVirtualCseExpression::IVisitor*)this);
				}

				void Visit(WfBindExpression* node)override
				{
					if (context->currentBindExpression)
					{
						manager->errors.Add(WfErrors::BindInBind(node));
					}

					auto bind = context->currentBindExpression;
					context->currentBindExpression = node;
					ValidateExpressionStructure(manager, context, node->expression);
					context->currentBindExpression = bind;
				}

				void Visit(WfNewCoroutineExpression* node)override
				{
					ValidateStructureContext context;
					context.currentNewCoroutineExpression = node;
					ValidateStatementStructure(manager, &context, node->statement);
				}

				void Visit(WfMixinCastExpression* node)override
				{
					ValidateTypeStructure(manager, node->type);
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfExpectedTypeCastExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfCoOperatorExpression* node)override
				{
					if (!context->currentCoProviderStatement)
					{
						manager->errors.Add(WfErrors::WrongCoOperator(node));
					}
				}

				static void Execute(Ptr<WfExpression>& expression, WfLexicalScopeManager* manager, ValidateStructureContext* context)
				{
					ValidateStructureExpressionVisitor visitor(manager, context);
					expression->Accept(&visitor);
					if (visitor.result)
					{
						expression = visitor.result;
					}
				}
			};

/***********************************************************************
ValidateStructure
***********************************************************************/

			void ValidateExpressionStructure(WfLexicalScopeManager* manager, ValidateStructureContext* context, Ptr<WfExpression>& expression)
			{
				ValidateStructureExpressionVisitor::Execute(expression, manager, context);
			}
		}
	}
}
