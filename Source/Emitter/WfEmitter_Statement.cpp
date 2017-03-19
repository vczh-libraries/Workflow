#include "WfEmitter.h"

namespace vl
{
	namespace workflow
	{
		namespace emitter
		{
			using namespace collections;
			using namespace reflection;
			using namespace reflection::description;
			using namespace analyzer;
			using namespace runtime;

			typedef WfInstruction Ins;

#define INSTRUCTION(X) context.AddInstruction(node, X)
#define EXIT_CODE(X) context.AddExitInstruction(node, X)

/***********************************************************************
GenerateInstructions(Statement)
***********************************************************************/

			class GenerateStatementInstructionsVisitor : public Object, public WfStatement::IVisitor
			{
			public:
				WfCodegenContext&						context;

				GenerateStatementInstructionsVisitor(WfCodegenContext& _context)
					:context(_context)
				{
				}

				void ApplyExitCode(Ptr<WfCodegenScopeContext> scopeContext)
				{
					context.ApplyExitInstructions(scopeContext);
					if (scopeContext->exitStatement)
					{
						GenerateStatementInstructions(context, scopeContext->exitStatement);
					}
				}

				void ApplyCurrentScopeExitCode()
				{
					auto scopeContext = context.functionContext->GetCurrentScopeContext();
					ApplyExitCode(scopeContext);
				}

				void InlineScopeExitCode(WfCodegenScopeType untilScopeType, bool exclusive)
				{
					vint index = context.functionContext->scopeContextStack.Count() - 1;
					while (index >= 0)
					{
						auto scopeContext = context.functionContext->scopeContextStack[index];
						if (exclusive && scopeContext->type == untilScopeType) break;
						ApplyExitCode(scopeContext);
						if (!exclusive && scopeContext->type == untilScopeType) break;
						index--;
					}
				}

				void Visit(WfBreakStatement* node)override
				{
					InlineScopeExitCode(WfCodegenScopeType::Loop, false);
					context.functionContext->GetCurrentScopeContext(WfCodegenScopeType::Loop)->breakInstructions.Add(INSTRUCTION(Ins::Jump(-1)));
				}

				void Visit(WfContinueStatement* node)override
				{
					InlineScopeExitCode(WfCodegenScopeType::Loop, true);
					context.functionContext->GetCurrentScopeContext(WfCodegenScopeType::Loop)->continueInstructions.Add(INSTRUCTION(Ins::Jump(-1)));
				}

				void Visit(WfReturnStatement* node)override
				{
					InlineScopeExitCode(WfCodegenScopeType::Function, false);
					if (node->expression)
					{
						GenerateExpressionInstructions(context, node->expression);
					}
					else
					{
						INSTRUCTION(Ins::LoadValue(Value()));
					}
					INSTRUCTION(Ins::Return());
				}

				void Visit(WfDeleteStatement* node)override
				{
					GenerateExpressionInstructions(context, node->expression);
					INSTRUCTION(Ins::DeleteRawPtr());
				}

				void Visit(WfRaiseExceptionStatement* node)override
				{
					if (node->expression)
					{
						GenerateExpressionInstructions(context, node->expression);
					}
					else
					{
						auto scope = context.manager->nodeScopes[node];
						while (scope)
						{
							if (auto tryCatch = scope->ownerNode.Cast<WfTryStatement>())
							{
								if (tryCatch->name.value != L"")
								{
									auto symbol = scope->symbols[tryCatch->name.value][0].Obj();
									vint index = context.functionContext->localVariables[symbol];
									INSTRUCTION(Ins::LoadLocalVar(index));
									break;
								}
							}
							scope = scope->parentScope;
						}
					}
					InlineScopeExitCode(WfCodegenScopeType::TryCatch, true);
					INSTRUCTION(Ins::RaiseException());
				}

				void Visit(WfIfStatement* node)override
				{
					vint variableIndex = -1;

					GenerateExpressionInstructions(context, node->expression);
					if (node->name.value != L"")
					{
						auto scope = context.manager->nodeScopes[node];
						auto symbol = scope->symbols[node->name.value][0];
						auto function = context.functionContext->function;
						variableIndex = function->argumentNames.Count() + function->localVariableNames.Add(L"<if>" + node->name.value);
						context.functionContext->localVariables.Add(symbol.Obj(), variableIndex);

						GenerateTypeCastInstructions(context, symbol->typeInfo, false, node->expression.Obj());
						INSTRUCTION(Ins::StoreLocalVar(variableIndex));
						INSTRUCTION(Ins::LoadLocalVar(variableIndex));
						INSTRUCTION(Ins::LoadValue(Value()));
						INSTRUCTION(Ins::CompareReference());
					}
					else
					{
						INSTRUCTION(Ins::OpNot(WfInsType::Bool));
					}
					vint fillElseIndex = INSTRUCTION(Ins::JumpIf(-1));

					GenerateStatementInstructions(context, node->trueBranch);
					if (variableIndex != -1)
					{
						INSTRUCTION(Ins::LoadValue(Value()));
						INSTRUCTION(Ins::StoreLocalVar(variableIndex));
					}
					vint fillEndIndex = INSTRUCTION(Ins::Jump(-1));
					context.assembly->instructions[fillElseIndex].indexParameter = context.assembly->instructions.Count();

					if (node->falseBranch)
					{
						GenerateStatementInstructions(context, node->falseBranch);
					}
					context.assembly->instructions[fillEndIndex].indexParameter = context.assembly->instructions.Count();
				}

				void Visit(WfWhileStatement* node)override
				{
					vint continueLabelIndex = -1;
					vint breakLabelIndex = -1;
					vint loopLabelIndex = -1;
					auto loopContext = context.functionContext->PushScopeContext(WfCodegenScopeType::Loop);

					loopLabelIndex = context.assembly->instructions.Count();
					continueLabelIndex = context.assembly->instructions.Count();
					GenerateExpressionInstructions(context, node->condition);
					INSTRUCTION(Ins::OpNot(WfInsType::Bool));
					loopContext->breakInstructions.Add(INSTRUCTION(Ins::JumpIf(-1)));
					GenerateStatementInstructions(context, node->statement);
					INSTRUCTION(Ins::Jump(loopLabelIndex));
					breakLabelIndex = context.assembly->instructions.Count();

					FOREACH(vint, index, loopContext->continueInstructions)
					{
						context.assembly->instructions[index].indexParameter = continueLabelIndex;
					}
					FOREACH(vint, index, loopContext->breakInstructions)
					{
						context.assembly->instructions[index].indexParameter = breakLabelIndex;
					}
					context.functionContext->PopScopeContext();
				}

				void VisitTryCatch(WfTryStatement* node)
				{
					if (!node->catchStatement)
					{
						GenerateStatementInstructions(context, node->protectedStatement);
					}
					else
					{
						auto catchContext = context.functionContext->PushScopeContext(WfCodegenScopeType::TryCatch);
						EXIT_CODE(Ins::UninstallTry(0));

						vint trapInstruction = INSTRUCTION(Ins::InstallTry(-1));
						GenerateStatementInstructions(context, node->protectedStatement);
						INSTRUCTION(Ins::UninstallTry(0));
						vint finishInstruction = INSTRUCTION(Ins::Jump(-1));
						context.functionContext->PopScopeContext();
						
						context.assembly->instructions[trapInstruction].indexParameter = context.assembly->instructions.Count();
						auto scope = context.manager->nodeScopes[node].Obj();
						auto symbol = scope->symbols[node->name.value][0].Obj();
						auto function = context.functionContext->function;
						vint variableIndex = function->argumentNames.Count() + function->localVariableNames.Add(L"<catch>" + node->name.value);
						context.functionContext->localVariables.Add(symbol, variableIndex);
						INSTRUCTION(Ins::LoadException());
						INSTRUCTION(Ins::StoreLocalVar(variableIndex));
						GenerateStatementInstructions(context, node->catchStatement);
						
						context.assembly->instructions[finishInstruction].indexParameter = context.assembly->instructions.Count();
					}
				}

				void VisitTryFinally(WfTryStatement* node)
				{
					if (!node->finallyStatement)
					{
						VisitTryCatch(node);
					}
					else
					{
						auto catchContext = context.functionContext->PushScopeContext(WfCodegenScopeType::TryCatch);
						EXIT_CODE(Ins::UninstallTry(0));
						catchContext->exitStatement = node->finallyStatement;
						
						auto function = context.functionContext->function;
						vint variableIndex = function->argumentNames.Count() + function->localVariableNames.Add(L"<try-finally-exception>");
						INSTRUCTION(Ins::LoadValue(Value()));
						INSTRUCTION(Ins::StoreLocalVar(variableIndex));
						vint trapInstruction = INSTRUCTION(Ins::InstallTry(-1));
						VisitTryCatch(node);
						INSTRUCTION(Ins::UninstallTry(0));
						vint untrapInstruction = INSTRUCTION(Ins::Jump(-1));
						context.functionContext->PopScopeContext();

						context.assembly->instructions[trapInstruction].indexParameter = context.assembly->instructions.Count();
						INSTRUCTION(Ins::LoadException());
						INSTRUCTION(Ins::StoreLocalVar(variableIndex));

						context.assembly->instructions[untrapInstruction].indexParameter = context.assembly->instructions.Count();
						GenerateStatementInstructions(context, node->finallyStatement);

						INSTRUCTION(Ins::LoadLocalVar(variableIndex));
						INSTRUCTION(Ins::LoadValue(Value()));
						INSTRUCTION(Ins::CompareReference());
						vint finishInstruction = INSTRUCTION(Ins::JumpIf(-1));
						INSTRUCTION(Ins::LoadLocalVar(variableIndex));
						INSTRUCTION(Ins::RaiseException());
						context.assembly->instructions[finishInstruction].indexParameter = context.assembly->instructions.Count();
					}
				}

				void Visit(WfTryStatement* node)override
				{
					VisitTryFinally(node);
				}

				void Visit(WfBlockStatement* node)override
				{
					FOREACH(Ptr<WfStatement>, statement, node->statements)
					{
						GenerateStatementInstructions(context, statement);
					}
				}

				void Visit(WfExpressionStatement* node)override
				{
					GenerateExpressionInstructions(context, node->expression);
					INSTRUCTION(Ins::Pop());
				}

				void Visit(WfVariableStatement* node)override
				{
					auto manager = context.manager;
					auto scope = manager->nodeScopes[node->variable.Obj()];
					auto symbol = scope->symbols[node->variable->name.value][0].Obj();
					auto function = context.functionContext->function;
					vint index = function->argumentNames.Count() + function->localVariableNames.Add(node->variable->name.value);
					context.functionContext->localVariables.Add(symbol, index);

					GenerateExpressionInstructions(context, node->variable->expression);
					INSTRUCTION(Ins::StoreLocalVar(index));
				}

				void Visit(WfVirtualStatement* node)override
				{
					GenerateStatementInstructions(context, node->expandedStatement);
				}

				void Visit(WfCoroutineStatement* node)override
				{
					CHECK_FAIL(L"GenerateStatementInstructionsVisitor::Visit(WfCoroutineStatement*)#Internal error, All coroutine statements do not emit instructions.");
				}
			};

#undef EXIT_CODE
#undef INSTRUCTION

			void GenerateStatementInstructions(WfCodegenContext& context, Ptr<WfStatement> statement)
			{
				GenerateStatementInstructionsVisitor visitor(context);
				statement->Accept(&visitor);
			}
		}
	}
}