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
ExpandNewCoroutineExpression
***********************************************************************/

			void ExpandMixinCastExpression(WfLexicalScopeManager* manager, WfMixinCastExpression* node)
			{
				auto sourceType = manager->expressionResolvings[node->expression.Obj()].type;

				auto newExpr = MakePtr<WfNewInterfaceExpression>();
				node->expandedExpression = newExpr;

				newExpr->type = CopyType(node->type);
				{
					auto varDecl = MakePtr<WfVariableDeclaration>();
					newExpr->declarations.Add(varDecl);
					{
						varDecl->classMember = MakePtr<WfClassMember>();
						varDecl->classMember->kind = WfClassMemberKind::Normal;
					}
					varDecl->name.value = L"<mixin-source>";
					varDecl->expression = CopyExpression(node->expression);

					if (sourceType->GetDecorator() == ITypeInfo::RawPtr)
					{
						auto tdType = MakePtr<TypeDescriptorTypeInfo>(sourceType->GetTypeDescriptor(), TypeInfoHint::Normal);
						auto pointerType = MakePtr<SharedPtrTypeInfo>(tdType);

						auto castExpr = MakePtr<WfTypeCastingExpression>();
						castExpr->strategy = WfTypeCastingStrategy::Strong;
						castExpr->expression = varDecl->expression;
						castExpr->type = GetTypeFromTypeInfo(pointerType.Obj());

						varDecl->expression = castExpr;
					}
				}
				List<ITypeDescriptor*> unprocessed;
				unprocessed.Add(sourceType->GetTypeDescriptor());
				for (vint i = 0; i < unprocessed.Count(); i++)
				{
					auto td = unprocessed[i];
					vint groupCount = td->GetMethodGroupCount();
					for (vint j = 0; j < groupCount; j++)
					{
						auto group = td->GetMethodGroup(j);
						vint methodCount = group->GetMethodCount();
						for (vint k = 0; k < methodCount; k++)
						{
							auto method = group->GetMethod(k);
							if (!method->IsStatic())
							{
								auto funcDecl = MakePtr<WfFunctionDeclaration>();
								newExpr->declarations.Add(funcDecl);
								{
									funcDecl->classMember = MakePtr<WfClassMember>();
									funcDecl->classMember->kind = WfClassMemberKind::Override;
								}
								funcDecl->anonymity = WfFunctionAnonymity::Named;
								funcDecl->name.value = method->GetName();
								funcDecl->returnType = GetTypeFromTypeInfo(method->GetReturn());
								vint parameterCount = method->GetParameterCount();
								for (vint l = 0; l < parameterCount; l++)
								{
									auto parameter = method->GetParameter(l);
									auto argument = MakePtr<WfFunctionArgument>();
									argument->name.value = parameter->GetName();
									argument->type = GetTypeFromTypeInfo(parameter->GetType());
									funcDecl->arguments.Add(argument);
								}

								auto implBlock = MakePtr<WfBlockStatement>();
								funcDecl->statement = implBlock;
								{
									auto refSource = MakePtr<WfReferenceExpression>();
									refSource->name.value = L"<mixin-source>";

									auto memberExpr = MakePtr<WfMemberExpression>();
									if (sourceType->GetTypeDescriptor() == method->GetOwnerTypeDescriptor())
									{
										memberExpr->parent = refSource;
									}
									else if (sourceType->GetTypeDescriptor()->GetMethodGroupByName(method->GetName(), true) == method->GetOwnerMethodGroup())
									{
										memberExpr->parent = refSource;
									}
									else
									{
										auto castExpr = MakePtr<WfTypeCastingExpression>();
										castExpr->strategy = WfTypeCastingStrategy::Strong;
										castExpr->expression = refSource;
										{
											auto tdType = MakePtr<TypeDescriptorTypeInfo>(sourceType->GetTypeDescriptor(), TypeInfoHint::Normal);
											auto pointerType = MakePtr<RawPtrTypeInfo>(tdType);
											castExpr->type = GetTypeFromTypeInfo(pointerType.Obj());
										}

										auto inferExpr = MakePtr<WfInferExpression>();
										inferExpr->expression = castExpr;
										{
											auto tdType = MakePtr<TypeDescriptorTypeInfo>(method->GetOwnerTypeDescriptor(), TypeInfoHint::Normal);
											auto pointerType = MakePtr<RawPtrTypeInfo>(tdType);
											inferExpr->type = GetTypeFromTypeInfo(pointerType.Obj());
										}

										memberExpr->parent = inferExpr;
									}
									memberExpr->name.value = method->GetName();

									auto callExpr = MakePtr<WfCallExpression>();
									callExpr->function = memberExpr;

									for (vint l = 0; l < parameterCount; l++)
									{
										auto parameter = method->GetParameter(l);

										auto argumentExpr = MakePtr<WfReferenceExpression>();
										argumentExpr->name.value = parameter->GetName();
										callExpr->arguments.Add(argumentExpr);
									}

									if (method->GetReturn()->GetTypeDescriptor() == description::GetTypeDescriptor<void>())
									{
										auto stat = MakePtr<WfExpressionStatement>();
										stat->expression = callExpr;
										implBlock->statements.Add(stat);
									}
									else
									{
										auto stat = MakePtr<WfReturnStatement>();
										stat->expression = callExpr;
										implBlock->statements.Add(stat);
									}
								}
							}
						}
					}

					vint count = td->GetBaseTypeDescriptorCount();
					for (vint j = 0; j < count; j++)
					{
						auto baseTd = td->GetBaseTypeDescriptor(j);
						if (!unprocessed.Contains(baseTd))
						{
							unprocessed.Add(baseTd);
						}
					}
				}
			}
		}
	}
}