#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace reflection::description;

/***********************************************************************
ValidateStructure(Statement)
***********************************************************************/

			class ValidateStructureStatementVisitor
				: public Object
				, public WfStatement::IVisitor
				, public WfVirtualCseStatement::IVisitor
				, public WfCoroutineStatement::IVisitor
				, public WfStateMachineStatement::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				ValidateStructureContext*				context;
				Ptr<WfStatement>						result;

				ValidateStructureStatementVisitor(WfLexicalScopeManager* _manager, ValidateStructureContext* _context)
					:manager(_manager)
					, context(_context)
				{
				}

				void Visit(WfBreakStatement* node)override
				{
					if (!context->currentLoopStatement)
					{
						manager->errors.Add(WfErrors::BreakNotInLoop(node));
					}
				}

				void Visit(WfContinueStatement* node)override
				{
					if (!context->currentLoopStatement)
					{
						manager->errors.Add(WfErrors::ContinueNotInLoop(node));
					}
				}

				void Visit(WfReturnStatement* node)override
				{
					if (node->expression)
					{
						ValidateExpressionStructure(manager, context, node->expression);
					}
				}

				void Visit(WfDeleteStatement* node)override
				{
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfRaiseExceptionStatement* node)override
				{
					if (node->expression)
					{
						ValidateExpressionStructure(manager, context, node->expression);
					}
					else if (!context->currentCatchStatement)
					{
						manager->errors.Add(WfErrors::RethrowNotInCatch(node));
					}
				}

				void Visit(WfIfStatement* node)override
				{
					if (node->type)
					{
						ValidateTypeStructure(manager, node->type);
					}
					ValidateExpressionStructure(manager, context, node->expression);
					ValidateStatementStructure(manager, context, node->trueBranch);
					if (node->falseBranch)
					{
						ValidateStatementStructure(manager, context, node->falseBranch);
					}
				}

				void Visit(WfWhileStatement* node)override
				{
					auto oldLoop = context->currentLoopStatement;
					context->currentLoopStatement = node;
					ValidateExpressionStructure(manager, context, node->condition);
					ValidateStatementStructure(manager, context, node->statement);
					context->currentLoopStatement = oldLoop;
				}

				void Visit(WfTryStatement* node)override
				{
					ValidateStatementStructure(manager, context, node->protectedStatement);
					if (node->catchStatement)
					{
						auto oldCatch = context->currentCatchStatement;
						context->currentCatchStatement = node->catchStatement.Obj();
						ValidateStatementStructure(manager, context, node->catchStatement);
						context->currentCatchStatement = oldCatch;
					}
					if (node->finallyStatement)
					{
						ValidateStatementStructure(manager, context, node->finallyStatement);
					}
					if (!node->catchStatement && !node->finallyStatement)
					{
						manager->errors.Add(WfErrors::TryMissCatchAndFinally(node));
					}
				}

				void Visit(WfBlockStatement* node)override
				{
					for (vint i = 0; i < node->statements.Count(); i++)
					{
						ValidateStatementStructure(manager, context, node->statements[i]);
					}
				}

				void Visit(WfGotoStatement* node)override
				{
				}

				void Visit(WfExpressionStatement* node)override
				{
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfVariableStatement* node)override
				{
					ValidateDeclarationStructure(manager, node->variable);
				}

				void Visit(WfVirtualCseStatement* node)override
				{
					node->Accept((WfVirtualCseStatement::IVisitor*)this);
				}

				void Visit(WfSwitchStatement* node)override
				{
					ValidateExpressionStructure(manager, context, node->expression);
					for (auto switchCase : node->caseBranches)
					{
						ValidateExpressionStructure(manager, context, switchCase->expression);
						ValidateStatementStructure(manager, context, switchCase->statement);
					}
					if (node->defaultBranch)
					{
						ValidateStatementStructure(manager, context, node->defaultBranch);
					}
				}

				void Visit(WfForEachStatement* node)override
				{
					auto oldLoop = context->currentLoopStatement;
					context->currentLoopStatement = node;
					ValidateExpressionStructure(manager, context, node->collection);
					ValidateStatementStructure(manager, context, node->statement);
					context->currentLoopStatement = oldLoop;
				}

				void Visit(WfCoProviderStatement* node)override
				{
					auto oldProvider = context->currentCoProviderStatement;
					context->currentCoProviderStatement = node;
					ValidateStatementStructure(manager, context, node->statement);
					context->currentCoProviderStatement = oldProvider;
				}

				void Visit(WfCoroutineStatement* node)override
				{
					node->Accept((WfCoroutineStatement::IVisitor*)this);
				}

				void Visit(WfCoPauseStatement* node)override
				{
					if (!context->currentNewCoroutineExpression || context->currentCoPauseStatement)
					{
						manager->errors.Add(WfErrors::WrongCoPause(node));
					}
					if (node->statement)
					{
						auto oldCpPause = context->currentCoPauseStatement;
						context->currentCoPauseStatement = node;
						ValidateStatementStructure(manager, context, node->statement);
						context->currentCoPauseStatement = oldCpPause;
					}
				}

				void Visit(WfCoOperatorStatement* node)override
				{
					if (!context->currentCoProviderStatement)
					{
						manager->errors.Add(WfErrors::WrongCoOperator(node));
					}
				}

				void Visit(WfStateMachineStatement* node)override
				{
					node->Accept((WfStateMachineStatement::IVisitor*)this);
				}

				void Visit(WfStateSwitchStatement* node)override
				{
					if (!context->currentStateDeclaration)
					{
						manager->errors.Add(WfErrors::WrongStateSwitchStatement(node));
					}

					for (auto switchCase : node->caseBranches)
					{
						ValidateStatementStructure(manager, context, switchCase->statement);
					}
				}

				void Visit(WfStateInvokeStatement* node)override
				{
					if (!context->currentStateDeclaration)
					{
						manager->errors.Add(WfErrors::WrongStateInvokeStatement(node));
					}

					for (auto argument : node->arguments)
					{
						ValidateExpressionStructure(manager, context, argument);
					}
				}

				static void Execute(Ptr<WfStatement>& statement, WfLexicalScopeManager* manager, ValidateStructureContext* context)
				{
					ValidateStructureStatementVisitor visitor(manager, context);
					statement->Accept(&visitor);
					if (visitor.result)
					{
						statement = visitor.result;
					}
				}
			};

/***********************************************************************
ValidateStructure
***********************************************************************/

			void ValidateStatementStructure(WfLexicalScopeManager* manager, ValidateStructureContext* context, Ptr<WfStatement>& statement)
			{
				ValidateStructureStatementVisitor::Execute(statement, manager, context);
			}
		}
	}
}
