#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace regex;
			using namespace parsing;
			using namespace reflection;
			using namespace reflection::description;
			using namespace runtime;

			typedef WfInstruction Ins;

#define INSTRUCTION(X) context.AddInstruction(node, X)

/***********************************************************************
GenerateInstructions(Expression)
***********************************************************************/

			class GenerateExpressionInstructionsVisitor : public Object, public WfExpression::IVisitor
			{
			public:
				WfCodegenContext&						context;

				GenerateExpressionInstructionsVisitor(WfCodegenContext& _context)
					:context(_context)
				{
				}

				void GenerateLoadSymbolInstructions(WfLexicalSymbol* symbol, parsing::ParsingTreeCustomBase* node)
				{
					vint index = -1;
					if ((index = context.globalFunctions.Keys().IndexOf(symbol)) != -1)
					{
						vint functionIndex = context.globalFunctions.Values()[index];
						INSTRUCTION(Ins::LoadClosure(functionIndex, 0));
					}
					else if ((index = context.globalVariables.Keys().IndexOf(symbol)) != -1)
					{
						vint variableIndex = context.globalVariables.Values()[index];
						INSTRUCTION(Ins::LoadGlobalVar(variableIndex));
					}
					else if ((index = context.functionContext->capturedVariables.Keys().IndexOf(symbol)) != -1)
					{
						vint variableIndex = context.functionContext->capturedVariables.Values()[index];
						INSTRUCTION(Ins::LoadCapturedVar(variableIndex));
					}
					else if ((index = context.functionContext->localVariables.Keys().IndexOf(symbol)) != -1)
					{
						vint variableIndex = context.functionContext->localVariables.Values()[index];
						INSTRUCTION(Ins::LoadLocalVar(variableIndex));
					}
					else if ((index = context.functionContext->arguments.Keys().IndexOf(symbol)) != -1)
					{
						vint variableIndex = context.functionContext->arguments.Values()[index];
						INSTRUCTION(Ins::LoadLocalVar(variableIndex));
					}
				}

				void VisitReferenceExpression(WfExpression* node)
				{
					auto result = context.manager->expressionResolvings[node];
					if (result.symbol)
					{
						GenerateLoadSymbolInstructions(result.symbol.Obj(), node);
					}
					else
					{
						if (result.methodInfo && result.methodInfo->IsStatic())
						{
							auto meta = MakePtr<WfAssemblyFunction>();
							meta->name = result.methodInfo->GetName() + L"<" + result.methodInfo->GetOwnerTypeDescriptor()->GetTypeName() + L">";
							vint functionIndex = context.assembly->functions.Add(meta);
							context.assembly->functionByName.Add(meta->name, functionIndex);
						
							WfCodegenLambdaContext lc;
							lc.staticMethodReferenceExpression = node;
							context.functionContext->closuresToCodegen.Add(functionIndex, lc);

							INSTRUCTION(Ins::LoadClosure(functionIndex, 0));
						}
					}
				}

				void Visit(WfTopQualifiedExpression* node)override
				{
					VisitReferenceExpression(node);
				}

				void Visit(WfReferenceExpression* node)override
				{
					VisitReferenceExpression(node);
				}

				void Visit(WfOrderedNameExpression* node)override
				{
					VisitReferenceExpression(node);
				}

				void Visit(WfOrderedLambdaExpression* node)override
				{
					auto meta = MakePtr<WfAssemblyFunction>();
					vint functionIndex = context.assembly->functions.Add(meta);
					meta->name = L"<lambda:(" + itow(functionIndex) + L")> in " + context.functionContext->function->name;
					context.assembly->functionByName.Add(meta->name, functionIndex);

					WfCodegenLambdaContext lc;
					lc.orderedLambdaExpression = node;
					context.functionContext->closuresToCodegen.Add(functionIndex, lc);

					vint index = context.manager->orderedLambdaCaptures.Keys().IndexOf(node);
					if (index != -1)
					{
						const auto& symbols = context.manager->orderedLambdaCaptures.GetByIndex(index);
						FOREACH(Ptr<WfLexicalSymbol>, symbol, symbols)
						{
							GenerateLoadSymbolInstructions(symbol.Obj(), node);
						}
						INSTRUCTION(Ins::LoadClosure(functionIndex, symbols.Count()));
					}
					else
					{
						INSTRUCTION(Ins::LoadClosure(functionIndex, 0));
					}
				}

				void Visit(WfMemberExpression* node)override
				{
					auto result = context.manager->expressionResolvings[node];
					if (result.propertyInfo)
					{
						GenerateExpressionInstructions(context, node->parent);
						if (result.propertyInfo->GetGetter())
						{
							INSTRUCTION(Ins::InvokeMethod(result.propertyInfo->GetGetter(), 0));
						}
						else
						{
							INSTRUCTION(Ins::GetProperty(result.propertyInfo));
						}
					}
					else
					{
						auto meta = MakePtr<WfAssemblyFunction>();
						meta->name = result.methodInfo->GetName() + L"<" + result.methodInfo->GetOwnerTypeDescriptor()->GetTypeName() + L">";
						vint functionIndex = context.assembly->functions.Add(meta);
						context.assembly->functionByName.Add(meta->name, functionIndex);
						
						WfCodegenLambdaContext lc;
						lc.methodReferenceExpression = node;
						context.functionContext->closuresToCodegen.Add(functionIndex, lc);

						GenerateExpressionInstructions(context, node->parent);
						INSTRUCTION(Ins::LoadClosure(functionIndex, 1));
					}
				}

				void Visit(WfChildExpression* node)override
				{
					VisitReferenceExpression(node);
				}

				void Visit(WfLiteralExpression* node)override
				{
					switch (node->value)
					{
					case WfLiteralValue::Null:
						INSTRUCTION(Ins::LoadValue(Value()));
						break;
					case WfLiteralValue::True:
						INSTRUCTION(Ins::LoadValue(BoxValue(true)));
						break;
					case WfLiteralValue::False:
						INSTRUCTION(Ins::LoadValue(BoxValue(false)));
						break;
					}
				}

				void Visit(WfFloatingExpression* node)override
				{
					INSTRUCTION(Ins::LoadValue(BoxValue(wtof(node->value.value))));
				}

				void Visit(WfIntegerExpression* node)override
				{
					auto result = context.manager->expressionResolvings[node];
					auto td = result.type->GetTypeDescriptor();
					if (td == description::GetTypeDescriptor<vint32_t>())
					{
						INSTRUCTION(Ins::LoadValue(BoxValue((vint32_t)wtoi(node->value.value))));
					}
					else if (td == description::GetTypeDescriptor<vint64_t>())
					{
						INSTRUCTION(Ins::LoadValue(BoxValue((vint64_t)wtoi64(node->value.value))));
					}
					else if (td == description::GetTypeDescriptor<vuint64_t>())
					{
						INSTRUCTION(Ins::LoadValue(BoxValue((vuint64_t)wtou64(node->value.value))));
					}
				}

				void Visit(WfStringExpression* node)override
				{
					INSTRUCTION(Ins::LoadValue(BoxValue(node->value.value)));
				}

				void Visit(WfFormatExpression* node)override
				{
					GenerateExpressionInstructions(context, node->expandedExpression);
				}

				void Visit(WfUnaryExpression* node)override
				{
					auto type = GenerateExpressionInstructions(context, node->operand);
					switch (node->op)
					{
					case WfUnaryOperator::Not:
						INSTRUCTION(Ins::OpNot(GetInstructionTypeArgument(type)));
						break;
					case WfUnaryOperator::Positive:
						INSTRUCTION(Ins::OpPositive(GetInstructionTypeArgument(type)));
						break;
					case WfUnaryOperator::Negative:
						INSTRUCTION(Ins::OpNegative(GetInstructionTypeArgument(type)));
						break;
					}
				}

				void Visit(WfBinaryExpression* node)override
				{
					if (node->op == WfBinaryOperator::Assign)
					{
						if (auto binary = node->first.Cast<WfBinaryExpression>())
						{
							auto result = context.manager->expressionResolvings[binary->first.Obj()];
							auto containerType = result.expectedType ? result.expectedType : result.type;
							auto methodInfo = containerType->GetTypeDescriptor()->GetMethodGroupByName(L"Set", true)->GetMethod(0);
							GenerateExpressionInstructions(context, binary->second);
							INSTRUCTION(Ins::Duplicate(0));
							GenerateExpressionInstructions(context, node->second);
							GenerateExpressionInstructions(context, binary->first);
							INSTRUCTION(Ins::InvokeMethod(methodInfo, 2));
							INSTRUCTION(Ins::Pop());
						}
						else if (auto member = node->first.Cast<WfMemberExpression>())
						{
							auto result = context.manager->expressionResolvings[member.Obj()];
							if (auto methodInfo = result.propertyInfo->GetSetter())
							{
								GenerateExpressionInstructions(context, node->second);
								INSTRUCTION(Ins::Duplicate(0));
								GenerateExpressionInstructions(context, member->parent);
								INSTRUCTION(Ins::InvokeMethod(methodInfo, 1));
								INSTRUCTION(Ins::Pop());
							}
							else
							{
								GenerateExpressionInstructions(context, node->second);
								INSTRUCTION(Ins::Duplicate(0));
								GenerateExpressionInstructions(context, member->parent);
								INSTRUCTION(Ins::SetProperty(result.propertyInfo));
							}
						}
						else
						{
							GenerateExpressionInstructions(context, node->second);
							INSTRUCTION(Ins::Duplicate(0));
							auto result = context.manager->expressionResolvings[node->first.Obj()];
							vint index = -1;
							if ((index = context.globalVariables.Keys().IndexOf(result.symbol.Obj())) != -1)
							{
								vint variableIndex = context.globalVariables.Values()[index];
								INSTRUCTION(Ins::StoreGlobalVar(variableIndex));
							}
							else if ((index = context.functionContext->localVariables.Keys().IndexOf(result.symbol.Obj())) != -1)
							{
								vint variableIndex = context.functionContext->localVariables.Values()[index];
								INSTRUCTION(Ins::StoreLocalVar(variableIndex));
							}
						}
					}
					else if (node->op == WfBinaryOperator::Index)
					{
						auto result = context.manager->expressionResolvings[node->first.Obj()];
						auto containerType = result.expectedType ? result.expectedType : result.type;
						auto methodInfo = containerType->GetTypeDescriptor()->GetMethodGroupByName(L"Get", true)->GetMethod(0);
						GenerateExpressionInstructions(context, node->second);
						GenerateExpressionInstructions(context, node->first);
						INSTRUCTION(Ins::InvokeMethod(methodInfo, 1));
					}
					else if (node->op == WfBinaryOperator::Concat)
					{
						auto type = TypeInfoRetriver<WString>::CreateTypeInfo();
						GenerateExpressionInstructions(context, node->first, type);
						GenerateExpressionInstructions(context, node->second, type);
						INSTRUCTION(Ins::OpConcat());
					}
					else if (node->op == WfBinaryOperator::FailedThen)
					{
						auto result = context.manager->expressionResolvings[node];
						vint trapInstruction = INSTRUCTION(Ins::InstallTry(-1));
						GenerateExpressionInstructions(context, node->first, result.type);
						INSTRUCTION(Ins::UninstallTry(1));
						vint finishInstruction = INSTRUCTION(Ins::Jump(-1));

						context.assembly->instructions[trapInstruction].indexParameter = context.assembly->instructions.Count();
						GenerateExpressionInstructions(context, node->second, result.type);

						context.assembly->instructions[finishInstruction].indexParameter = context.assembly->instructions.Count();
					}
					else
					{
						Ptr<ITypeInfo> mergedType;
						switch (node->op)
						{
						case WfBinaryOperator::Exp:
						case WfBinaryOperator::Add:
						case WfBinaryOperator::Sub:
						case WfBinaryOperator::Mul:
						case WfBinaryOperator::Div:
						case WfBinaryOperator::Mod:
						case WfBinaryOperator::Shl:
						case WfBinaryOperator::Shr:
							{
								auto result = context.manager->expressionResolvings[node];
								mergedType = result.type;
							}
							break;
						default:
							{
								auto firstResult = context.manager->expressionResolvings[node->first.Obj()];
								auto secondResult = context.manager->expressionResolvings[node->second.Obj()];
								auto firstType = firstResult.expectedType ? firstResult.expectedType : firstResult.type;
								auto secondType = secondResult.expectedType ? secondResult.expectedType : secondResult.type;
								if (node->op == WfBinaryOperator::EQ || node->op == WfBinaryOperator::NE)
								{
									if (firstType->GetDecorator() == ITypeInfo::RawPtr || firstType->GetDecorator() == ITypeInfo::SharedPtr)
									{
										GenerateExpressionInstructions(context, node->first);
										GenerateExpressionInstructions(context, node->second);
										INSTRUCTION(Ins::CompareReference());
										if (node->op == WfBinaryOperator::NE)
										{
											INSTRUCTION(Ins::OpNot(WfInsType::Bool));
										}
										return;
									}
								}

								mergedType = GetMergedType(firstType, secondType);
								if (node->op == WfBinaryOperator::EQ || node->op == WfBinaryOperator::NE)
								{
									if (mergedType->GetTypeDescriptor() == description::GetTypeDescriptor<Value>())
									{
										GenerateExpressionInstructions(context, node->first);
										GenerateExpressionInstructions(context, node->second);
										INSTRUCTION(Ins::CompareValue());
										if (node->op == WfBinaryOperator::NE)
										{
											INSTRUCTION(Ins::OpNot(WfInsType::Bool));
										}
										return;
									}
									else if (mergedType->GetTypeDescriptor()->GetValueSerializer())
									{
										auto structType = mergedType->GetDecorator() == ITypeInfo::Nullable ? CopyTypeInfo(mergedType->GetElementType()) : mergedType;
										auto insType = GetInstructionTypeArgument(structType);
										if (insType == WfInsType::Unknown)
										{
											GenerateExpressionInstructions(context, node->first);
											GenerateExpressionInstructions(context, node->second);
											INSTRUCTION(Ins::CompareStruct());
											if (node->op == WfBinaryOperator::NE)
											{
												INSTRUCTION(Ins::OpNot(WfInsType::Bool));
											}
											return;
										}
									}
								}
							}
						}

						GenerateExpressionInstructions(context, node->first, mergedType);
						GenerateExpressionInstructions(context, node->second, mergedType);

						switch (node->op)
						{
						case WfBinaryOperator::Exp:
							INSTRUCTION(Ins::OpExp(GetInstructionTypeArgument(mergedType)));
							break;
						case WfBinaryOperator::Add:
							INSTRUCTION(Ins::OpAdd(GetInstructionTypeArgument(mergedType)));
							break;
						case WfBinaryOperator::Sub:
							INSTRUCTION(Ins::OpSub(GetInstructionTypeArgument(mergedType)));
							break;
						case WfBinaryOperator::Mul:
							INSTRUCTION(Ins::OpMul(GetInstructionTypeArgument(mergedType)));
							break;
						case WfBinaryOperator::Div:
							INSTRUCTION(Ins::OpDiv(GetInstructionTypeArgument(mergedType)));
							break;
						case WfBinaryOperator::Mod:
							INSTRUCTION(Ins::OpMod(GetInstructionTypeArgument(mergedType)));
							break;
						case WfBinaryOperator::Shl:
							INSTRUCTION(Ins::OpShl(GetInstructionTypeArgument(mergedType)));
							break;
						case WfBinaryOperator::Shr:
							INSTRUCTION(Ins::OpShr(GetInstructionTypeArgument(mergedType)));
							break;
						case WfBinaryOperator::LT:
							INSTRUCTION(Ins::CompareLiteral(GetInstructionTypeArgument(mergedType)));
							INSTRUCTION(Ins::OpLT());
							break;
						case WfBinaryOperator::GT:
							INSTRUCTION(Ins::CompareLiteral(GetInstructionTypeArgument(mergedType)));
							INSTRUCTION(Ins::OpGT());
							break;
						case WfBinaryOperator::LE:
							INSTRUCTION(Ins::CompareLiteral(GetInstructionTypeArgument(mergedType)));
							INSTRUCTION(Ins::OpLE());
							break;
						case WfBinaryOperator::GE:
							INSTRUCTION(Ins::CompareLiteral(GetInstructionTypeArgument(mergedType)));
							INSTRUCTION(Ins::OpGE());
							break;
						case WfBinaryOperator::EQ:
							INSTRUCTION(Ins::CompareLiteral(GetInstructionTypeArgument(mergedType)));
							INSTRUCTION(Ins::OpEQ());
							break;
						case WfBinaryOperator::NE:
							INSTRUCTION(Ins::CompareLiteral(GetInstructionTypeArgument(mergedType)));
							INSTRUCTION(Ins::OpNE());
							break;
						case WfBinaryOperator::Xor:
							INSTRUCTION(Ins::OpXor(GetInstructionTypeArgument(mergedType)));
							break;
						case WfBinaryOperator::And:
							INSTRUCTION(Ins::OpAnd(GetInstructionTypeArgument(mergedType)));
							break;
						case WfBinaryOperator::Or:
							INSTRUCTION(Ins::OpOr(GetInstructionTypeArgument(mergedType)));
							break;
						default:;
						}
					}
				}

				void Visit(WfLetExpression* node)override
				{
					auto scope = context.manager->expressionScopes[node].Obj();
					Array<vint> variableIndices(node->variables.Count());
					auto function = context.functionContext->function;
					FOREACH_INDEXER(Ptr<WfLetVariable>, var, index, node->variables)
					{
						auto symbol = scope->symbols[var->name.value][0];
						vint variableIndex = function->argumentNames.Count() + function->localVariableNames.Add(L"<let>" + var->name.value);
						context.functionContext->localVariables.Add(symbol.Obj(), variableIndex);
						variableIndices[index] = variableIndex;

						GenerateExpressionInstructions(context, var->value);
						INSTRUCTION(Ins::StoreLocalVar(variableIndex));
					}
					GenerateExpressionInstructions(context, node->expression);
					FOREACH_INDEXER(Ptr<WfLetVariable>, var, index, node->variables)
					{
						INSTRUCTION(Ins::LoadValue(Value()));
						INSTRUCTION(Ins::StoreLocalVar(variableIndices[index]));
					}
				}

				void Visit(WfIfExpression* node)override
				{
					auto result = context.manager->expressionResolvings[node];
					GenerateExpressionInstructions(context, node->condition);
					vint fillTrueIndex = INSTRUCTION(Ins::JumpIf(-1));
					GenerateExpressionInstructions(context, node->falseBranch, result.type);
					vint fillEndIndex = INSTRUCTION(Ins::Jump(-1));
					context.assembly->instructions[fillTrueIndex].indexParameter = context.assembly->instructions.Count();
					GenerateExpressionInstructions(context, node->trueBranch, result.type);
					context.assembly->instructions[fillEndIndex].indexParameter = context.assembly->instructions.Count();
				}

				void Visit(WfRangeExpression* node)override
				{
					auto result = context.manager->expressionResolvings[node];
					auto elementType = result.type->GetElementType()->GetGenericArgument(0);
					auto type = GetInstructionTypeArgument(elementType);
					
					GenerateExpressionInstructions(context, node->begin, elementType);
					if (node->beginBoundary == WfRangeBoundary::Exclusive)
					{
						INSTRUCTION(Ins::LoadValue(BoxValue<vint>(1)));
						INSTRUCTION(Ins::OpAdd(type));
					}
					
					GenerateExpressionInstructions(context, node->end, elementType);
					if (node->endBoundary == WfRangeBoundary::Exclusive)
					{
						INSTRUCTION(Ins::LoadValue(BoxValue<vint>(1)));
						INSTRUCTION(Ins::OpSub(type));
					}

					INSTRUCTION(Ins::CreateRange(type));
				}

				void Visit(WfSetTestingExpression* node)override
				{
					if (auto range = node->collection.Cast<WfRangeExpression>())
					{
						auto resultElement = context.manager->expressionResolvings[node->element.Obj()];
						auto resultBegin = context.manager->expressionResolvings[range->begin.Obj()];
						auto resultEnd = context.manager->expressionResolvings[range->end.Obj()];
						auto typeElement = resultElement.expectedType ? resultElement.expectedType : resultElement.type;
						auto typeBegin = resultBegin.expectedType ? resultBegin.expectedType : resultBegin.type;
						auto typeEnd = resultEnd.expectedType ? resultEnd.expectedType : resultEnd.type;
						auto typeLeft = GetMergedType(typeElement, typeBegin);
						auto typeRight = GetMergedType(typeElement, typeEnd);

						auto function = context.functionContext->function;
						vint index = function->argumentNames.Count() + function->localVariableNames.Add(L"<anonymous-range-test>");
						GenerateExpressionInstructions(context, node->element);
						INSTRUCTION(Ins::StoreLocalVar(index));
						
						INSTRUCTION(Ins::LoadLocalVar(index));
						if (!IsSameType(typeElement.Obj(), typeLeft.Obj()))
						{
							GenerateTypeCastInstructions(context, typeLeft, true, node);
						}
						GenerateExpressionInstructions(context, range->begin, typeLeft);
						INSTRUCTION(Ins::CompareLiteral(GetInstructionTypeArgument(typeLeft)));
						if (range->beginBoundary == WfRangeBoundary::Exclusive)
						{
							INSTRUCTION(Ins::OpGT());
						}
						else
						{
							INSTRUCTION(Ins::OpGE());
						}

						INSTRUCTION(Ins::LoadLocalVar(index));
						if (!IsSameType(typeElement.Obj(), typeRight.Obj()))
						{
							GenerateTypeCastInstructions(context, typeRight, true, node);
						}
						GenerateExpressionInstructions(context, range->end, typeRight);
						INSTRUCTION(Ins::CompareLiteral(GetInstructionTypeArgument(typeRight)));
						if (range->endBoundary == WfRangeBoundary::Exclusive)
						{
							INSTRUCTION(Ins::OpLT());
						}
						else
						{
							INSTRUCTION(Ins::OpLE());
						}

						INSTRUCTION(Ins::OpAnd(WfInsType::Bool));
						INSTRUCTION(Ins::LoadValue(Value()));
						INSTRUCTION(Ins::StoreLocalVar(index));
					}
					else
					{
						auto result = context.manager->expressionResolvings[node->collection.Obj()];
						auto type = result.expectedType ? result.expectedType : result.type;

						GenerateExpressionInstructions(context, node->element);
						GenerateExpressionInstructions(context, node->collection);

						auto tdList = description::GetTypeDescriptor<IValueReadonlyList>();
						if (result.type->GetTypeDescriptor()->CanConvertTo(tdList))
						{
							auto method = tdList->GetMethodGroupByName(L"Contains", true)->GetMethod(0);
							INSTRUCTION(Ins::InvokeMethod(method, 1));
						}
						else
						{
							INSTRUCTION(Ins::TestElementInSet());
						}
					}
				}

				void Visit(WfConstructorExpression* node)override
				{
					auto result = context.manager->expressionResolvings[node];

					if (result.type->GetTypeDescriptor() == description::GetTypeDescriptor<IValueEnumerable>()
						|| result.type->GetTypeDescriptor() == description::GetTypeDescriptor<IValueReadonlyList>()
						|| result.type->GetTypeDescriptor() == description::GetTypeDescriptor<IValueList>())
					{
						Ptr<ITypeInfo> keyType = CopyTypeInfo(result.type->GetElementType()->GetGenericArgument(0));
						FOREACH(Ptr<WfConstructorArgument>, argument, From(node->arguments).Reverse())
						{
							GenerateExpressionInstructions(context, argument->key, keyType);
						}
						INSTRUCTION(Ins::CreateArray(node->arguments.Count()));
					}
					else
					{
						Ptr<ITypeInfo> keyType = CopyTypeInfo(result.type->GetElementType()->GetGenericArgument(0));
						Ptr<ITypeInfo> valueType = CopyTypeInfo(result.type->GetElementType()->GetGenericArgument(1));
						FOREACH(Ptr<WfConstructorArgument>, argument, From(node->arguments).Reverse())
						{
							GenerateExpressionInstructions(context, argument->key, keyType);
							GenerateExpressionInstructions(context, argument->value, valueType);
						}
						INSTRUCTION(Ins::CreateMap(node->arguments.Count() * 2));
					}
				}

				void Visit(WfInferExpression* node)override
				{
					GenerateExpressionInstructions(context, node->expression);
				}

				void Visit(WfTypeCastingExpression* node)override
				{
					if (node->strategy == WfTypeCastingStrategy::Strong)
					{
						auto result = context.manager->expressionResolvings[node];
						GenerateExpressionInstructions(context, node->expression, result.type);
					}
					else
					{
						auto scope = context.manager->expressionScopes[node].Obj();
						auto type = CreateTypeInfoFromType(scope, node->type);
						GenerateExpressionInstructions(context, node->expression);
						GenerateTypeCastInstructions(context, type, false, node);
					}
				}

				void Visit(WfTypeTestingExpression* node)override
				{
					switch (node->test)
					{
					case WfTypeTesting::IsNull:
						GenerateExpressionInstructions(context, node->expression);
						INSTRUCTION(Ins::LoadValue(Value()));
						INSTRUCTION(Ins::CompareReference());
						break;
					case WfTypeTesting::IsNotNull:
						GenerateExpressionInstructions(context, node->expression);
						INSTRUCTION(Ins::LoadValue(Value()));
						INSTRUCTION(Ins::CompareReference());
						INSTRUCTION(Ins::OpNot(WfInsType::Bool));
						break;
					case WfTypeTesting::IsType:
						{
							auto scope = context.manager->expressionScopes[node].Obj();
							auto type = CreateTypeInfoFromType(scope, node->type);
							GenerateExpressionInstructions(context, node->expression);
							GenerateTypeTestingInstructions(context, type, node);
						}
						break;
					case WfTypeTesting::IsNotType:
						{
							auto scope = context.manager->expressionScopes[node].Obj();
							auto type = CreateTypeInfoFromType(scope, node->type);
							GenerateExpressionInstructions(context, node->expression);
							GenerateTypeTestingInstructions(context, type, node);
							INSTRUCTION(Ins::OpNot(WfInsType::Bool));
						}
						break;
					}
				}

				void Visit(WfTypeOfTypeExpression* node)override
				{
					auto scope = context.manager->expressionScopes[node].Obj();
					auto type = CreateTypeInfoFromType(scope, node->type);
					auto value = Value::From(type->GetTypeDescriptor());
					INSTRUCTION(Ins::LoadValue(value));
				}

				void Visit(WfTypeOfExpressionExpression* node)override
				{
					GenerateExpressionInstructions(context, node->expression);
					INSTRUCTION(Ins::GetType());
				}

				void Visit(WfAttachEventExpression* node)override
				{
					auto result = context.manager->expressionResolvings[node->event.Obj()];
					auto parent = node->event.Cast<WfMemberExpression>()->parent;
					GenerateExpressionInstructions(context, parent);
					GenerateExpressionInstructions(context, node->function);
					INSTRUCTION(Ins::AttachEvent(result.eventInfo));
				}

				void Visit(WfDetachEventExpression* node)override
				{
					GenerateExpressionInstructions(context, node->handler);
					INSTRUCTION(Ins::DetachEvent());
				}

				void Visit(WfBindExpression* node)override
				{
					GenerateExpressionInstructions(context, node->expandedExpression);
				}

				void Visit(WfObserveExpression* node)override
				{
				}

				void Visit(WfCallExpression* node)override
				{
					FOREACH(Ptr<WfExpression>, argument, node->arguments)
					{
						GenerateExpressionInstructions(context, argument);
					}

					auto result = context.manager->expressionResolvings[node->function.Obj()];
					if (result.methodInfo)
					{
						if (result.methodInfo->IsStatic())
						{
							INSTRUCTION(Ins::LoadValue(Value()));
						}
						else
						{
							auto member = node->function.Cast<WfMemberExpression>();
							GenerateExpressionInstructions(context, member->parent);
						}

						INSTRUCTION(Ins::InvokeMethod(result.methodInfo, node->arguments.Count()));
						return;
					}
					else if (result.symbol)
					{
						vint index = context.globalFunctions.Keys().IndexOf(result.symbol.Obj());
						if (index != -1)
						{
							vint functionIndex = context.globalFunctions.Values()[index];
							INSTRUCTION(Ins::Invoke(functionIndex, node->arguments.Count()));
							return;
						}
					}

					GenerateExpressionInstructions(context, node->function);
					INSTRUCTION(Ins::InvokeProxy(node->arguments.Count()));
				}

				void VisitFunction(WfFunctionDeclaration* node, WfCodegenLambdaContext lc, const Func<WString(vint)>& getName)
				{
					auto meta = MakePtr<WfAssemblyFunction>();
					vint functionIndex = context.assembly->functions.Add(meta);
					meta->name = getName(functionIndex);
					context.assembly->functionByName.Add(meta->name, functionIndex);

					context.functionContext->closuresToCodegen.Add(functionIndex, lc);

					vint index = context.manager->functionLambdaCaptures.Keys().IndexOf(node);
					if (index != -1)
					{
						const auto& symbols = context.manager->functionLambdaCaptures.GetByIndex(index);
						FOREACH(Ptr<WfLexicalSymbol>, symbol, symbols)
						{
							GenerateLoadSymbolInstructions(symbol.Obj(), node);
						}
						INSTRUCTION(Ins::LoadClosure(functionIndex, symbols.Count()));
					}
					else
					{
						INSTRUCTION(Ins::LoadClosure(functionIndex, 0));
					}
				}

				void Visit(WfFunctionExpression* node)override
				{
					WfCodegenLambdaContext lc;
					lc.functionExpression = node;
					VisitFunction(node->function.Obj(), lc, [=](vint index)
					{
						return L"<lambda:" + node->function->name.value + L"(" + itow(index) + L")> in " + context.functionContext->function->name;
					});
				}

				void Visit(WfNewTypeExpression* node)override
				{
					auto result = context.manager->expressionResolvings[node];
					auto td = result.methodInfo->GetOwnerTypeDescriptor();
					if ((td->GetTypeDescriptorFlags() & TypeDescriptorFlags::ClassType) != TypeDescriptorFlags::Undefined)
					{
						FOREACH(Ptr<WfExpression>, argument, node->arguments)
						{
							GenerateExpressionInstructions(context, argument);
						}
						INSTRUCTION(Ins::LoadValue(Value()));
						INSTRUCTION(Ins::InvokeMethod(result.methodInfo, node->arguments.Count()));
					}
					else
					{
						FOREACH(Ptr<WfFunctionDeclaration>, decl, node->functions)
						{
							auto methodInfo = context.manager->interfaceMethodImpls[decl.Obj()];
							INSTRUCTION(Ins::LoadMethodInfo(methodInfo));
							WfCodegenLambdaContext lc;
							lc.functionDeclaration = decl.Obj();
							VisitFunction(decl.Obj(), lc, [=](vint index)
							{
								return L"<method:" + decl->name.value + L"<" + result.type->GetTypeDescriptor()->GetTypeName() + L">(" + itow(index) + L")> in " + context.functionContext->function->name;
							});
						}
						INSTRUCTION(Ins::CreateInterface(node->functions.Count() * 2));
						INSTRUCTION(Ins::LoadValue(Value()));
						INSTRUCTION(Ins::InvokeMethod(result.methodInfo, 1));
					}
				}
			};

#undef INSTRUCTION

			Ptr<reflection::description::ITypeInfo> GenerateExpressionInstructions(WfCodegenContext& context, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType)
			{
				GenerateExpressionInstructionsVisitor visitor(context);
				expression->Accept(&visitor);

				auto result = context.manager->expressionResolvings[expression.Obj()];
				auto type = result.type;

				if (result.expectedType && !IsSameType(type.Obj(), result.expectedType.Obj()))
				{
					type = result.expectedType;
					GenerateTypeCastInstructions(context, type, true, expression.Obj());
				}

				if (expectedType && !IsSameType(type.Obj(), expectedType.Obj()))
				{
					type = expectedType;
					GenerateTypeCastInstructions(context, type, true, expression.Obj());
				}

				return type;
			}
		}
	}
}