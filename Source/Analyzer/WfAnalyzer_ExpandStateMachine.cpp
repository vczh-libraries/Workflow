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
ExpandStateMachine
***********************************************************************/

			void ExpandStateMachine(WfLexicalScopeManager* manager, WfStateMachineDeclaration* node)
			{
				auto& smInfo = manager->stateMachineInfos[node];

				FOREACH(Ptr<WfStateInput>, input, node->inputs)
				{
					smInfo->inputIds.Add(input->name.value, smInfo->inputIds.Count());

					FOREACH(Ptr<WfFunctionArgument>, argument, input->arguments)
					{
						// var <stateip-INPUT>NAME = <DEFAULT-VALUE>;
						auto fieldInfo = manager->stateInputArguments[argument.Obj()];

						auto varDecl = MakePtr<WfVariableDeclaration>();
						varDecl->name.value = fieldInfo->GetName();
						varDecl->type = GetTypeFromTypeInfo(fieldInfo->GetReturn());
						varDecl->expression = CreateDefaultValue(fieldInfo->GetReturn());

						auto classMember = MakePtr<WfClassMember>();
						classMember->kind = WfClassMemberKind::Normal;
						varDecl->classMember = classMember;

						auto att = MakePtr<WfAttribute>();
						att->category.value = L"cpp";
						att->name.value = L"Private";
						varDecl->attributes.Add(att);

						node->expandedDeclarations.Add(varDecl);
						manager->declarationMemberInfos.Add(varDecl, fieldInfo);
					}
				}

				smInfo->stateIds.Add(L"", 0);
				FOREACH(Ptr<WfStateDeclaration>, state, node->states)
				{
					if (state->name.value != L"")
					{
						smInfo->stateIds.Add(state->name.value, smInfo->stateIds.Count());
					}

					FOREACH(Ptr<WfFunctionArgument>, argument, state->arguments)
					{
						// var <statesp-INPUT>NAME = <DEFAULT-VALUE>;
						auto fieldInfo = manager->stateDeclArguments[argument.Obj()];

						auto varDecl = MakePtr<WfVariableDeclaration>();
						varDecl->name.value = fieldInfo->GetName();
						varDecl->type = GetTypeFromTypeInfo(fieldInfo->GetReturn());
						varDecl->expression = CreateDefaultValue(fieldInfo->GetReturn());

						auto classMember = MakePtr<WfClassMember>();
						classMember->kind = WfClassMemberKind::Normal;
						varDecl->classMember = classMember;

						auto att = MakePtr<WfAttribute>();
						att->category.value = L"cpp";
						att->name.value = L"Private";
						varDecl->attributes.Add(att);

						node->expandedDeclarations.Add(varDecl);
						manager->declarationMemberInfos.Add(varDecl, fieldInfo);
					}
				}

				FOREACH(Ptr<WfStateInput>, input, node->inputs)
				{
					auto methodInfo = manager->stateInputMethods[input.Obj()];

					// func INPUT(ARGUMENTS ...): void
					auto funcDecl = MakePtr<WfFunctionDeclaration>();
					funcDecl->name.value = methodInfo->GetName();
					funcDecl->returnType = GetTypeFromTypeInfo(methodInfo->GetReturn());
					FOREACH_INDEXER(Ptr<WfFunctionArgument>, argument, index, input->arguments)
					{
						auto funcArgument = MakePtr<WfFunctionArgument>();
						funcArgument->name.value = argument->name.value;
						funcArgument->type = GetTypeFromTypeInfo(methodInfo->GetParameter(index)->GetType());
						funcDecl->arguments.Add(funcArgument);
					}

					auto classMember = MakePtr<WfClassMember>();
					classMember->kind = WfClassMemberKind::Normal;
					funcDecl->classMember = classMember;

					node->expandedDeclarations.Add(funcDecl);
					manager->declarationMemberInfos.Add(funcDecl, methodInfo);

					auto block = MakePtr<WfBlockStatement>();
					funcDecl->statement = block;
					{
						// if (not this.stateMachineInitialized)
						auto trueBlock = MakePtr<WfBlockStatement>();
						{
							auto refInit = MakePtr<WfMemberExpression>();
							refInit->parent = MakePtr<WfThisExpression>();
							refInit->name.value = L"stateMachineInitialized";

							auto notExpr = MakePtr<WfUnaryExpression>();
							notExpr->op = WfUnaryOperator::Not;
							notExpr->operand = refInit;

							auto ifStat = MakePtr<WfIfStatement>();
							ifStat->expression = notExpr;
							ifStat->trueBranch = trueBlock;

							block->statements.Add(ifStat);
						}
						{
							// this.stateMachineInitialized = true;
							auto refInit = MakePtr<WfMemberExpression>();
							refInit->parent = MakePtr<WfThisExpression>();
							refInit->name.value = L"stateMachineInitialized";

							auto refTrue = MakePtr<WfLiteralExpression>();
							refTrue->value = WfLiteralValue::True;

							auto assignExpr = MakePtr<WfBinaryExpression>();
							assignExpr->op = WfBinaryOperator::Assign;
							assignExpr->first = refInit;
							assignExpr->second = refTrue;

							auto exprStat = MakePtr<WfExpressionStatement>();
							exprStat->expression = assignExpr;
							trueBlock->statements.Add(exprStat);
						}
						{
							// this.<state>CreateCoroutine(0);
							auto refCC = MakePtr<WfMemberExpression>();
							refCC->parent = MakePtr<WfThisExpression>();
							refCC->name.value = smInfo->createCoroutineMethod->GetName();

							auto refZero = MakePtr<WfIntegerExpression>();
							refZero->value.value = L"0";

							auto callExpr = MakePtr<WfCallExpression>();
							callExpr->function = refCC;
							callExpr->arguments.Add(refZero);

							auto exprStat = MakePtr<WfExpressionStatement>();
							exprStat->expression = callExpr;
							trueBlock->statements.Add(exprStat);
						}
						{
							// this.ResumeStateMachine();
							auto refResume = MakePtr<WfMemberExpression>();
							refResume->parent = MakePtr<WfThisExpression>();
							refResume->name.value = L"ResumeStateMachine";

							auto callExpr = MakePtr<WfCallExpression>();
							callExpr->function = refResume;

							auto exprStat = MakePtr<WfExpressionStatement>();
							exprStat->expression = callExpr;
							trueBlock->statements.Add(exprStat);
						}
					}
					{
						// this.stateMachineInput = <INPUT>
						auto refInput = MakePtr<WfMemberExpression>();
						refInput->parent = MakePtr<WfThisExpression>();
						refInput->name.value = L"stateMachineInput";

						auto refInputId = MakePtr<WfIntegerExpression>();
						refInputId->value.value = itow(smInfo->inputIds[input->name.value]);

						auto assignExpr = MakePtr<WfBinaryExpression>();
						assignExpr->op = WfBinaryOperator::Assign;
						assignExpr->first = refInput;
						assignExpr->second = refInputId;

						auto exprStat = MakePtr<WfExpressionStatement>();
						exprStat->expression = assignExpr;
						block->statements.Add(exprStat);
					}
					FOREACH_INDEXER(Ptr<WfFunctionArgument>, argument, index, input->arguments)
					{
						// this.<stateip-INPUT>NAME = NAME;
						auto refField = MakePtr<WfMemberExpression>();
						refField->parent = MakePtr<WfThisExpression>();
						refField->name.value = manager->stateInputArguments[argument.Obj()]->GetName();

						auto refArgument = MakePtr<WfReferenceExpression>();
						refArgument->name.value = methodInfo->GetParameter(index)->GetName();

						auto assignExpr = MakePtr<WfBinaryExpression>();
						assignExpr->op = WfBinaryOperator::Assign;
						assignExpr->first = refField;
						assignExpr->second = refArgument;

						auto exprStat = MakePtr<WfExpressionStatement>();
						exprStat->expression = assignExpr;
						block->statements.Add(exprStat);
					}
					{
						// this.ResumeStateMachine();
						auto refResume = MakePtr<WfMemberExpression>();
						refResume->parent = MakePtr<WfThisExpression>();
						refResume->name.value = L"ResumeStateMachine";

						auto callExpr = MakePtr<WfCallExpression>();
						callExpr->function = refResume;

						auto exprStat = MakePtr<WfExpressionStatement>();
						exprStat->expression = callExpr;
						block->statements.Add(exprStat);
					}
				}

				{
					// func <state>CreateCoroutine(<state>startState: int): void
					auto funcDecl = MakePtr<WfFunctionDeclaration>();
					funcDecl->name.value = smInfo->createCoroutineMethod->GetName();
					funcDecl->returnType = GetTypeFromTypeInfo(TypeInfoRetriver<void>::CreateTypeInfo().Obj());
					{
						auto parameterInfo = smInfo->createCoroutineMethod->GetParameter(0);

						auto funcArgument = MakePtr<WfFunctionArgument>();
						funcArgument->name.value = parameterInfo->GetName();
						funcArgument->type = GetTypeFromTypeInfo(parameterInfo->GetType());
						funcDecl->arguments.Add(funcArgument);
					}

					auto classMember = MakePtr<WfClassMember>();
					classMember->kind = WfClassMemberKind::Normal;
					funcDecl->classMember = classMember;

					auto att = MakePtr<WfAttribute>();
					att->category.value = L"cpp";
					att->name.value = L"Private";
					funcDecl->attributes.Add(att);

					node->expandedDeclarations.Add(funcDecl);
					manager->declarationMemberInfos.Add(funcDecl, smInfo->createCoroutineMethod);

					auto block = MakePtr<WfBlockStatement>();
					funcDecl->statement = block;
				}
			}
		}
	}
}