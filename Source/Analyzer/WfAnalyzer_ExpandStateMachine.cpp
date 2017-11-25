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

				FOREACH(Ptr<WfStateDeclaration>, state, node->states)
				{
					smInfo->stateIds.Add(state->name.value, smInfo->stateIds.Count());

					FOREACH(Ptr<WfFunctionArgument>, argument, state->arguments)
					{
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

					auto funcDecl = MakePtr<WfFunctionDeclaration>();
					funcDecl->name.value = methodInfo->GetName();
					funcDecl->returnType = GetTypeFromTypeInfo(methodInfo->GetReturn());
					FOREACH_INDEXER(Ptr<WfFunctionArgument>, argument, index, input->arguments)
					{
						auto funcArgument = MakePtr<WfFunctionArgument>();
						funcArgument->name.value = argument->name.value;
						funcArgument->type = GetTypeFromTypeInfo(methodInfo->GetParameter(index)->GetType());
					}

					auto classMember = MakePtr<WfClassMember>();
					classMember->kind = WfClassMemberKind::Normal;
					funcDecl->classMember = classMember;

					node->expandedDeclarations.Add(funcDecl);
					manager->declarationMemberInfos.Add(funcDecl, methodInfo);

					auto block = MakePtr<WfBlockStatement>();
					funcDecl->statement = block;
				}

				{
					auto funcDecl = MakePtr<WfFunctionDeclaration>();
					funcDecl->name.value = smInfo->createCoroutineMethod->GetName();
					funcDecl->returnType = GetTypeFromTypeInfo(TypeInfoRetriver<void>::CreateTypeInfo().Obj());
					{
						auto parameterInfo = smInfo->createCoroutineMethod->GetParameter(0);

						auto funcArgument = MakePtr<WfFunctionArgument>();
						funcArgument->name.value = parameterInfo->GetName();
						funcArgument->type = GetTypeFromTypeInfo(parameterInfo->GetType());
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