#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;

/***********************************************************************
SearchOrderedName(Declaration)
***********************************************************************/

			class SearchOrderedNameDeclarationVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfLexicalScope*							scope;
				SortedList<vint>&						names;

				SearchOrderedNameDeclarationVisitor(WfLexicalScope* _scope, SortedList<vint>& _names)
					:scope(_scope)
					, names(_names)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					FOREACH(Ptr<WfDeclaration>, declaration, node->declarations)
					{
						SearchOrderedName(scope, declaration, names);
					}
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					SearchOrderedName(scope, node->statement, names);
				}

				void Visit(WfVariableDeclaration* node)override
				{
					SearchOrderedName(scope, node->expression, names);
				}

				void Visit(WfEventDeclaration* node)override
				{
				}

				void Visit(WfPropertyDeclaration* node)override
				{
				}

				void Visit(WfConstructorDeclaration* node)override
				{
					FOREACH(Ptr<WfBaseConstructorCall>, call, node->baseConstructorCalls)
					{
						FOREACH(Ptr<WfExpression>, argument, call->arguments)
						{
							SearchOrderedName(scope, argument, names);
						}
					}
					SearchOrderedName(scope, node->statement, names);
				}

				void Visit(WfDestructorDeclaration* node)override
				{
					SearchOrderedName(scope, node->statement, names);
				}

				void Visit(WfClassDeclaration* node)override
				{
					FOREACH(Ptr<WfClassMember>, member, node->members)
					{
						SearchOrderedName(scope, member->declaration, names);
					}
				}

				void Visit(WfEnumDeclaration* node)override
				{
				}

				void Visit(WfStructDeclaration* node)override
				{
				}

				static void Execute(WfLexicalScope* scope, Ptr<WfDeclaration> declaration, SortedList<vint>& names)
				{
					SearchOrderedNameDeclarationVisitor visitor(scope, names);
					declaration->Accept(&visitor);
				}
			};

/***********************************************************************
SearchOrderedName(Statement)
***********************************************************************/

			class SearchOrderedNameStatementVisitor : public Object, public WfStatement::IVisitor
			{
			public:
				WfLexicalScope*							scope;
				SortedList<vint>&						names;

				SearchOrderedNameStatementVisitor(WfLexicalScope* _scope, SortedList<vint>& _names)
					:scope(_scope)
					, names(_names)
				{
				}

				void Visit(WfBreakStatement* node)override
				{
				}

				void Visit(WfContinueStatement* node)override
				{
				}

				void Visit(WfReturnStatement* node)override
				{
					if (node->expression)
					{
						SearchOrderedName(scope, node->expression, names);
					}
				}

				void Visit(WfDeleteStatement* node)override
				{
					SearchOrderedName(scope, node->expression, names);
				}

				void Visit(WfRaiseExceptionStatement* node)override
				{
					if (node->expression)
					{
						SearchOrderedName(scope, node->expression, names);
					}
				}

				void Visit(WfIfStatement* node)override
				{
					SearchOrderedName(scope, node->expression, names);
					node->trueBranch->Accept(this);
					if (node->falseBranch)
					{
						node->falseBranch->Accept(this);
					}
				}

				void Visit(WfSwitchStatement* node)override
				{
					SearchOrderedName(scope, node->expression, names);
					FOREACH(Ptr<WfSwitchCase>, switchCase, node->caseBranches)
					{
						SearchOrderedName(scope, switchCase->expression, names);
						switchCase->statement->Accept(this);
					}
					if (node->defaultBranch)
					{
						node->defaultBranch->Accept(this);
					}
				}

				void Visit(WfWhileStatement* node)override
				{
					SearchOrderedName(scope, node->condition, names);
					node->statement->Accept(this);
				}

				void Visit(WfForEachStatement* node)override
				{
					SearchOrderedName(scope, node->collection, names);
					node->statement->Accept(this);
				}

				void Visit(WfTryStatement* node)override
				{
					node->protectedStatement->Accept(this);
					if (node->catchStatement)
					{
						node->catchStatement->Accept(this);
					}
					if (node->finallyStatement)
					{
						node->finallyStatement->Accept(this);
					}
				}

				void Visit(WfBlockStatement* node)override
				{
					FOREACH(Ptr<WfStatement>, statement, node->statements)
					{
						statement->Accept(this);
					}
				}

				void Visit(WfExpressionStatement* node)override
				{
					SearchOrderedName(scope, node->expression, names);
				}

				void Visit(WfVariableStatement* node)override
				{
					SearchOrderedName(scope, node->variable.Cast<WfDeclaration>(), names);
				}

				static void Execute(WfLexicalScope* scope, Ptr<WfStatement> statement, SortedList<vint>& names)
				{
					SearchOrderedNameStatementVisitor visitor(scope, names);
					statement->Accept(&visitor);
				}
			};

/***********************************************************************
SearchOrderedName(Expression)
***********************************************************************/

			class SearchOrderedNameExpressionVisitor : public Object, public WfExpression::IVisitor
			{
			public:
				WfLexicalScope*							scope;
				SortedList<vint>&						names;

				SearchOrderedNameExpressionVisitor(WfLexicalScope* _scope, SortedList<vint>& _names)
					:scope(_scope)
					, names(_names)
				{
				}

				void Visit(WfThisExpression* node)override
				{
				}

				void Visit(WfTopQualifiedExpression* node)override
				{
				}

				void Visit(WfReferenceExpression* node)override
				{
				}

				void Visit(WfOrderedNameExpression* node)override
				{
					vint name = wtoi(node->name.value.Sub(1, node->name.value.Length() - 1));
					if (!names.Contains(name))
					{
						WfLexicalScope* currentScope = scope;
						while (currentScope)
						{
							if (currentScope->symbols.Keys().Contains(node->name.value))
							{
								return;
							}
							currentScope = currentScope->parentScope.Obj();
						}
						names.Add(name);
					}
				}

				void Visit(WfOrderedLambdaExpression* node)override
				{
					// names in nested ordered lambda expression is not counted
				}

				void Visit(WfMemberExpression* node)override
				{
					node->parent->Accept(this);
				}

				void Visit(WfChildExpression* node)override
				{
					node->parent->Accept(this);
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

				void Visit(WfFormatExpression* node)override
				{
					if (node->expandedExpression)
					{
						node->expandedExpression->Accept(this);
					}
				}

				void Visit(WfUnaryExpression* node)override
				{
					node->operand->Accept(this);
				}

				void Visit(WfBinaryExpression* node)override
				{
					node->first->Accept(this);
					node->second->Accept(this);
				}

				void Visit(WfLetExpression* node)override
				{
					FOREACH(Ptr<WfLetVariable>, variable, node->variables)
					{
						variable->value->Accept(this);
					}
					
					node->expression->Accept(this);
				}

				void Visit(WfIfExpression* node)override
				{
					node->condition->Accept(this);
					node->trueBranch->Accept(this);
					node->falseBranch->Accept(this);
				}

				void Visit(WfRangeExpression* node)override
				{
					node->begin->Accept(this);
					node->end->Accept(this);
				}

				void Visit(WfSetTestingExpression* node)override
				{
					node->element->Accept(this);
					node->collection->Accept(this);
				}

				void Visit(WfConstructorExpression* node)override
				{
					FOREACH(Ptr<WfConstructorArgument>, argument, node->arguments)
					{
						argument->value->Accept(this);
					}
				}

				void Visit(WfInferExpression* node)override
				{
					node->expression->Accept(this);
				}

				void Visit(WfTypeCastingExpression* node)override
				{
					node->expression->Accept(this);
				}

				void Visit(WfTypeTestingExpression* node)override
				{
					if (node->expression)
					{
						node->expression->Accept(this);
					}
				}

				void Visit(WfTypeOfTypeExpression* node)override
				{
				}

				void Visit(WfTypeOfExpressionExpression* node)override
				{
					node->expression->Accept(this);
				}

				void Visit(WfAttachEventExpression* node)override
				{
					node->event->Accept(this);
					node->function->Accept(this);
				}

				void Visit(WfDetachEventExpression* node)override
				{
					node->handler->Accept(this);
				}

				void Visit(WfBindExpression* node)override
				{
					node->expression->Accept(this);
				}

				void Visit(WfObserveExpression* node)override
				{
					node->parent->Accept(this);
					node->expression->Accept(this);
					FOREACH(Ptr<WfExpression>, event, node->events)
					{
						event->Accept(this);
					}
				}

				void Visit(WfCallExpression* node)override
				{
					node->function->Accept(this);
					FOREACH(Ptr<WfExpression>, argument, node->arguments)
					{
						argument->Accept(this);
					}
				}

				void Visit(WfFunctionExpression* node)override
				{
					SearchOrderedName(scope, node->function.Cast<WfDeclaration>(), names);
				}

				void Visit(WfNewClassExpression* node)override
				{
					FOREACH(Ptr<WfExpression>, argument, node->arguments)
					{
						argument->Accept(this);
					}
				}

				void Visit(WfNewInterfaceExpression* node)override
				{
					FOREACH(Ptr<WfClassMember>, member, node->members)
					{
						SearchOrderedName(scope, member->declaration, names);
					}
				}

				static void Execute(WfLexicalScope* scope, Ptr<WfExpression> expression, SortedList<vint>& names)
				{
					SearchOrderedNameExpressionVisitor visitor(scope, names);
					expression->Accept(&visitor);
				}
			};

/***********************************************************************
SearchOrderedName
***********************************************************************/

			void SearchOrderedName(WfLexicalScope* scope, Ptr<WfDeclaration> declaration, collections::SortedList<vint>& names)
			{
				SearchOrderedNameDeclarationVisitor::Execute(scope, declaration, names);
			}

			void SearchOrderedName(WfLexicalScope* scope, Ptr<WfStatement> statement, collections::SortedList<vint>& names)
			{
				SearchOrderedNameStatementVisitor::Execute(scope, statement, names);
			}

			void SearchOrderedName(WfLexicalScope* scope, Ptr<WfExpression> expression, collections::SortedList<vint>& names)
			{
				SearchOrderedNameExpressionVisitor::Execute(scope, expression, names);
			}
		}
	}
}