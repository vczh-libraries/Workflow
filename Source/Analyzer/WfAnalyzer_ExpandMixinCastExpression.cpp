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

				auto newExpr = Ptr(new WfNewInterfaceExpression);
				node->expandedExpression = newExpr;

				newExpr->type = CopyType(node->type);
				{
					auto varDecl = Ptr(new WfVariableDeclaration);
					newExpr->declarations.Add(varDecl);
					varDecl->name.value = L"<mixin-source>";
					varDecl->expression = CopyExpression(node->expression, true);

					if (sourceType->GetDecorator() == ITypeInfo::RawPtr)
					{
						auto castExpr = Ptr(new WfTypeCastingExpression);
						castExpr->strategy = WfTypeCastingStrategy::Strong;
						castExpr->expression = varDecl->expression;
						auto castType = Ptr(new WfSharedPointerType);
						castType->element = GetTypeFromTypeDescriptor(sourceType->GetTypeDescriptor());
						castExpr->type = castType;

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
								auto funcDecl = Ptr(new WfFunctionDeclaration);
								newExpr->declarations.Add(funcDecl);
								funcDecl->functionKind = WfFunctionKind::Override;
								funcDecl->anonymity = WfFunctionAnonymity::Named;
								funcDecl->name.value = method->GetName();
								funcDecl->returnType = GetTypeFromTypeInfo(method->GetReturn());
								vint parameterCount = method->GetParameterCount();
								for (vint l = 0; l < parameterCount; l++)
								{
									auto parameter = method->GetParameter(l);
									auto argument = Ptr(new WfFunctionArgument);
									argument->name.value = L"<mixin-parameter>" + parameter->GetName();
									argument->type = GetTypeFromTypeInfo(parameter->GetType());
									funcDecl->arguments.Add(argument);
								}

								auto implBlock = Ptr(new WfBlockStatement);
								funcDecl->statement = implBlock;
								{
									auto refSource = Ptr(new WfReferenceExpression);
									refSource->name.value = L"<mixin-source>";

									auto memberExpr = Ptr(new WfMemberExpression);
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
										auto castExpr = Ptr(new WfTypeCastingExpression);
										castExpr->strategy = WfTypeCastingStrategy::Strong;
										castExpr->expression = refSource;
										{
											auto castType = Ptr(new WfRawPointerType);
											castType->element = GetTypeFromTypeDescriptor(sourceType->GetTypeDescriptor());
											castExpr->type = castType;
										}

										auto inferExpr = Ptr(new WfInferExpression);
										inferExpr->expression = castExpr;
										{
											auto inferType = Ptr(new WfRawPointerType);
											inferType->element = GetTypeFromTypeDescriptor(method->GetOwnerTypeDescriptor());
											inferExpr->type = inferType;
										}

										memberExpr->parent = inferExpr;
									}
									memberExpr->name.value = method->GetName();

									auto callExpr = Ptr(new WfCallExpression);
									callExpr->function = memberExpr;

									for (vint l = 0; l < parameterCount; l++)
									{
										auto parameter = method->GetParameter(l);

										auto argumentExpr = Ptr(new WfReferenceExpression);
										argumentExpr->name.value = L"<mixin-parameter>" + parameter->GetName();
										callExpr->arguments.Add(argumentExpr);
									}

									if (method->GetReturn()->GetTypeDescriptor() == description::GetTypeDescriptor<void>())
									{
										auto stat = Ptr(new WfExpressionStatement);
										stat->expression = callExpr;
										implBlock->statements.Add(stat);
									}
									else
									{
										auto stat = Ptr(new WfReturnStatement);
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
