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
#define FILL_LABEL_TO_INS(LABEL, INS) context.assembly->instructions[LABEL].indexParameter = INS
#define FILL_LABEL_TO_CURRENT(LABEL) FILL_LABEL_TO_INS(LABEL, context.assembly->instructions.Count())

/***********************************************************************
GenerateInstructions(Expression)
***********************************************************************/

			class GenerateExpressionInstructionsVisitor
				: public Object
				, public WfExpression::IVisitor
			{
			public:
				WfCodegenContext&						context;

				GenerateExpressionInstructionsVisitor(WfCodegenContext& _context)
					:context(_context)
				{
				}

				static void GenerateLoadSymbolInstructions(WfCodegenContext& context, WfLexicalSymbol* symbol, glr::ParsingAstBase* node)
				{
					vint index = -1;
					if ((index = context.globalFunctions.Keys().IndexOf(symbol)) != -1)
					{
						vint functionIndex = context.globalFunctions.Values()[index];
						INSTRUCTION(Ins::CreateClosureContext(0));
						INSTRUCTION(Ins::LoadFunction(functionIndex));
						INSTRUCTION(Ins::CreateClosure());
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
					else if ((index = context.closureFunctions.Keys().IndexOf(symbol)) != -1)
					{
						vint functionIndex = context.closureFunctions.Values()[index];
						INSTRUCTION(Ins::LoadClosureContext());
						INSTRUCTION(Ins::LoadFunction(functionIndex));
						INSTRUCTION(Ins::CreateClosure());
					}
					else
					{
						CHECK_FAIL(L"GenerateExpressionInstructionsVisitor::GenerateLoadSymbolInstructions(WfCodegenContext&, WfLexicalSymbol*, ParsingTreeCustomBase*)#Internal error, cannot find any record of this symbol.");
					}
				}

				static vint PushCapturedThisValues(WfCodegenContext& context, WfLexicalScope* lambdaScopeParent, glr::ParsingAstBase* node)
				{
					auto scope = lambdaScopeParent;
					while (scope)
					{
						if (scope->functionConfig)
						{
							vint parentThisCount = context.GetThisStackCount(scope);
							if (scope->functionConfig->lambda)
							{
								auto capture = context.manager->lambdaCaptures[scope->ownerNode.Obj()];
								vint captureCount = capture->symbols.Count();
								for (vint i = 0; i < parentThisCount; i++)
								{
									INSTRUCTION(Ins::LoadCapturedVar(captureCount + i));
								}
							}
							else if (parentThisCount > 0)
							{
								CHECK_ERROR(parentThisCount == 1, L"GenerateExpressionInstructionsVisitor::PushCapturedThisValues(WfCodegenContext&, WfLexicalScope*, ParsingTreeCustomBase*)#Internal error, wrong parentThisCount value.");
								INSTRUCTION(Ins::LoadCapturedVar(0));
							}
							return parentThisCount;
						}
						scope = scope->parentScope.Obj();
					}
					return 0;
				}

				static vint AddClosure(WfCodegenContext& context, WfCodegenLambdaContext lc, const Func<WString(vint)>& getName)
				{
					auto meta = Ptr(new WfAssemblyFunction);
					vint functionIndex = context.assembly->functions.Add(meta);
					meta->name = getName(functionIndex);
					context.assembly->functionByName.Add(meta->name, functionIndex);
					context.functionContext->closuresToCodegen.Add(functionIndex, lc);
					return functionIndex;
				}

				void VisitReferenceExpression(WfExpression* node, const WString& name)
				{
					auto result = context.manager->expressionResolvings[node];
					if (result.symbol)
					{
						GenerateLoadSymbolInstructions(context, result.symbol.Obj(), node);
					}
					else if (result.methodInfo)
					{
						if (result.methodInfo->IsStatic())
						{
							INSTRUCTION(Ins::LoadValue({}));
						}
						else
						{
							VisitThisExpression(node, result.methodInfo->GetOwnerTypeDescriptor());
						}
						INSTRUCTION(Ins::LoadMethodClosure(result.methodInfo));
					}
					else if (result.propertyInfo)
					{
						if (auto getter = result.propertyInfo->GetGetter())
						{
							VisitThisExpression(node, getter->GetOwnerTypeDescriptor());
							INSTRUCTION(Ins::InvokeMethod(getter, 0));
						}
						else
						{
							VisitThisExpression(node, result.propertyInfo->GetOwnerTypeDescriptor());
							INSTRUCTION(Ins::GetProperty(result.propertyInfo));
						}
					}
					else
					{
						if ((result.type->GetTypeDescriptor()->GetTypeDescriptorFlags() & TypeDescriptorFlags::EnumType) != TypeDescriptorFlags::Undefined)
						{
							auto td = result.type->GetTypeDescriptor();
							auto enumType = td->GetEnumType();
							vint index = enumType->IndexOfItem(name);
							if (index != -1)
							{
								auto intValue = enumType->GetItemValue(index);
								INSTRUCTION(Ins::LoadValue({ intValue, td }));
								return;
							}
						}
						CHECK_FAIL(L"GenerateExpressionInstructionsVisitor::VisitReferenceExpression(WfExpression*, const WString&)#Internal error, cannot find any record of this expression.");
					}
				}

				void VisitThisExpression(WfExpression* node, ITypeDescriptor* td)
				{
					auto scope = context.manager->nodeScopes[node].Obj();
					vint count = context.GetThisStackCount(scope);
					vint offset = 0;

					Ptr<WfLexicalCapture> capture;
					Ptr<WfLexicalFunctionConfig> lastConfig;
					while (scope)
					{
						if (scope->typeOfThisExpr)
						{
							if (scope->typeOfThisExpr->CanConvertTo(td))
							{
								if (capture)
								{
									INSTRUCTION(Ins::LoadCapturedVar(capture->symbols.Count() + count - offset - 1));
								}
								else
								{
									INSTRUCTION(Ins::LoadCapturedVar(0));
								}
								return;
							}
							else
							{
								offset++;
							}

							if (lastConfig)
							{
								if (!lastConfig->parentThisAccessable)
								{
									break;
								}
							}
						}

						if (scope->functionConfig)
						{
							lastConfig = scope->functionConfig;
							if (!capture)
							{
								vint index = context.manager->lambdaCaptures.Keys().IndexOf(scope->ownerNode.Obj());
								if (index != -1)
								{
									capture = context.manager->lambdaCaptures.Values()[index];
								}
							}
						}

						scope = scope->parentScope.Obj();
					}
					CHECK_FAIL(L"GenerateExpressionInstructionsVisitor::VisitThisExpression(WfExpression*, ITypeDescriptor*)#Internal error, cannot find any record of the this value.");
				}

				void Visit(WfThisExpression* node)override
				{
					auto scope = context.manager->nodeScopes[node].Obj();
					while (scope)
					{
						if (scope->functionConfig)
						{
							if (scope->functionConfig->thisAccessable)
							{
								if (scope->functionConfig->lambda)
								{
									auto capture = context.manager->lambdaCaptures[scope->ownerNode.Obj()];
									auto count = context.GetThisStackCount(scope);
									INSTRUCTION(Ins::LoadCapturedVar(capture->symbols.Count() + count - 1));
								}
								else
								{
									INSTRUCTION(Ins::LoadCapturedVar(0));
								}
								return;
							}
							CHECK_FAIL(L"GenerateExpressionInstructionsVisitor::Visit(WfThisExpression*)#Internal error, this expression is illegal here.");
						}
						scope = scope->parentScope.Obj();
					}
					CHECK_FAIL(L"GenerateExpressionInstructionsVisitor::Visit(WfThisExpression*)#Internal error, this expression is illegal here.");
				}

				void Visit(WfTopQualifiedExpression* node)override
				{
					VisitReferenceExpression(node, node->name.value);
				}

				void Visit(WfReferenceExpression* node)override
				{
					VisitReferenceExpression(node, node->name.value);
				}

				void Visit(WfOrderedNameExpression* node)override
				{
					VisitReferenceExpression(node, node->name.value);
				}

				void Visit(WfOrderedLambdaExpression* node)override
				{
					auto scope = context.manager->nodeScopes[node].Obj();

					WfCodegenLambdaContext lc;
					lc.orderedLambdaExpression = node;
					auto functionIndex = AddClosure(context, lc, [=](vint index)
					{
						return L"<lambda:(" + itow(index) + L")> in " + context.functionContext->function->name;
					});

					auto capture = context.manager->lambdaCaptures.Get(node);
					for (auto symbol : capture->symbols)
					{
						GenerateLoadSymbolInstructions(context, symbol.Obj(), node);
					}
					vint thisCount = PushCapturedThisValues(context, scope->parentScope.Obj(), node);
					INSTRUCTION(Ins::CreateClosureContext(capture->symbols.Count() + thisCount));
					INSTRUCTION(Ins::LoadFunction(functionIndex));
					INSTRUCTION(Ins::CreateClosure());
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
						GenerateExpressionInstructions(context, node->parent);
						INSTRUCTION(Ins::LoadMethodClosure(result.methodInfo));
					}
				}

				void Visit(WfChildExpression* node)override
				{
					VisitReferenceExpression(node, node->name.value);
				}

				void Visit(WfLiteralExpression* node)override
				{
					switch (node->value)
					{
					case WfLiteralValue::Null:
						INSTRUCTION(Ins::LoadValue({}));
						break;
					case WfLiteralValue::True:
						INSTRUCTION(Ins::LoadValue({ true }));
						break;
					case WfLiteralValue::False:
						INSTRUCTION(Ins::LoadValue({ false }));
						break;
					default:;
					}
				}

#define INSTRUCTION_LOAD_VALUE(TYPE) if (td == description::GetTypeDescriptor<TYPE>()) { INSTRUCTION(Ins::LoadValue({ UnboxValue<TYPE>(output) })); return; }

				void Visit(WfFloatingExpression* node)override
				{
					auto result = context.manager->expressionResolvings[node];
					auto td = result.type->GetTypeDescriptor();
					Value output;
					td->GetSerializableType()->Deserialize(node->value.value, output);
					INSTRUCTION_LOAD_VALUE(float);
					INSTRUCTION_LOAD_VALUE(double);
					CHECK_FAIL(L"Unrecognized value in WfFloatingExpression!");
				}

				void Visit(WfIntegerExpression* node)override
				{
					auto result = context.manager->expressionResolvings[node];
					auto td = result.type->GetTypeDescriptor();
					Value output;
					td->GetSerializableType()->Deserialize(node->value.value, output);
					INSTRUCTION_LOAD_VALUE(vint8_t);
					INSTRUCTION_LOAD_VALUE(vint16_t);
					INSTRUCTION_LOAD_VALUE(vint32_t);
					INSTRUCTION_LOAD_VALUE(vint64_t);
					INSTRUCTION_LOAD_VALUE(vuint8_t);
					INSTRUCTION_LOAD_VALUE(vuint16_t);
					INSTRUCTION_LOAD_VALUE(vuint32_t);
					INSTRUCTION_LOAD_VALUE(vuint64_t);
					INSTRUCTION_LOAD_VALUE(float);
					INSTRUCTION_LOAD_VALUE(double);
					CHECK_FAIL(L"Unrecognized value in WfIntegerExpression!");
				}

#undef INSTRUCTION_LOAD_VALUE

				void Visit(WfStringExpression* node)override
				{
					INSTRUCTION(Ins::LoadValue({ node->value.value }));
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
					default:;
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
							else if ((index = context.functionContext->capturedVariables.Keys().IndexOf(result.symbol.Obj())) != -1)
							{
								vint variableIndex = context.functionContext->capturedVariables.Values()[index];
								INSTRUCTION(Ins::StoreCapturedVar(variableIndex));
							}
							else if (result.propertyInfo)
							{
								if (auto setter = result.propertyInfo->GetSetter())
								{
									VisitThisExpression(node, setter->GetOwnerTypeDescriptor());
									INSTRUCTION(Ins::InvokeMethod(setter, 1));
									INSTRUCTION(Ins::Pop());
								}
								else
								{
									VisitThisExpression(node, result.propertyInfo->GetOwnerTypeDescriptor());
									INSTRUCTION(Ins::SetProperty(result.propertyInfo));
								}
							}
							else
							{
								CHECK_FAIL(L"GenerateExpressionInstructionsVisitor::Visit(WfBinaryExpression*)#Internal error, cannot find any record of this assignable symbol.");
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
					else if (node->op == WfBinaryOperator::FlagAnd)
					{
						auto type = context.manager->expressionResolvings[node].type;
						if (type->GetTypeDescriptor() == description::GetTypeDescriptor<WString>())
						{
							GenerateExpressionInstructions(context, node->first, type);
							GenerateExpressionInstructions(context, node->second, type);
							INSTRUCTION(Ins::OpConcat());
						}
						else
						{
							GenerateExpressionInstructions(context, node->first, type);
							INSTRUCTION(Ins::ConvertToType(Value::BoxedValue, description::GetTypeDescriptor<vuint64_t>()));
							GenerateExpressionInstructions(context, node->second, type);
							INSTRUCTION(Ins::ConvertToType(Value::BoxedValue, description::GetTypeDescriptor<vuint64_t>()));
							INSTRUCTION(Ins::OpAnd(WfInsType::U8));
							INSTRUCTION(Ins::ConvertToType(Value::BoxedValue, type->GetTypeDescriptor()));
						}
					}
					else if (node->op == WfBinaryOperator::FlagOr)
					{
						auto type = context.manager->expressionResolvings[node].type;
						GenerateExpressionInstructions(context, node->first, type);
						INSTRUCTION(Ins::ConvertToType(Value::BoxedValue, description::GetTypeDescriptor<vuint64_t>()));
						GenerateExpressionInstructions(context, node->second, type);
						INSTRUCTION(Ins::ConvertToType(Value::BoxedValue, description::GetTypeDescriptor<vuint64_t>()));
						INSTRUCTION(Ins::OpOr(WfInsType::U8));
						INSTRUCTION(Ins::ConvertToType(Value::BoxedValue, type->GetTypeDescriptor()));
					}
					else if (node->op == WfBinaryOperator::FailedThen)
					{
						auto result = context.manager->expressionResolvings[node];
						vint trapInstruction = INSTRUCTION(Ins::InstallTry(-1));
						GenerateExpressionInstructions(context, node->first, result.type);
						INSTRUCTION(Ins::UninstallTry(1));
						vint finishInstruction = INSTRUCTION(Ins::Jump(-1));

						FILL_LABEL_TO_CURRENT(trapInstruction);
						GenerateExpressionInstructions(context, node->second, result.type);

						FILL_LABEL_TO_CURRENT(finishInstruction);
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
									GenerateExpressionInstructions(context, node->first);
									GenerateExpressionInstructions(context, node->second);
									INSTRUCTION(Ins::CompareValue());
									if (node->op == WfBinaryOperator::NE)
									{
										INSTRUCTION(Ins::OpNot(WfInsType::Bool));
									}
									return;
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
					auto scope = context.manager->nodeScopes[node].Obj();
					Array<vint> variableIndices(node->variables.Count());
					auto function = context.functionContext->function;
					for (auto [var, index] : indexed(node->variables))
					{
						auto symbol = scope->symbols[var->name.value][0];
						vint variableIndex = function->argumentNames.Count() + function->localVariableNames.Add(L"<let>" + var->name.value);
						context.functionContext->localVariables.Add(symbol.Obj(), variableIndex);
						variableIndices[index] = variableIndex;

						GenerateExpressionInstructions(context, var->value);
						INSTRUCTION(Ins::StoreLocalVar(variableIndex));
					}
					GenerateExpressionInstructions(context, node->expression);
					for (auto [var, index] : indexed(node->variables))
					{
						INSTRUCTION(Ins::LoadValue({}));
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
					FILL_LABEL_TO_CURRENT(fillTrueIndex);
					GenerateExpressionInstructions(context, node->trueBranch, result.type);
					FILL_LABEL_TO_CURRENT(fillEndIndex);
				}

				void Visit(WfRangeExpression* node)override
				{
					auto result = context.manager->expressionResolvings[node];
					auto elementType = result.type->GetElementType()->GetGenericArgument(0);
					auto type = GetInstructionTypeArgument(elementType);
					
					GenerateExpressionInstructions(context, node->begin, elementType);
					if (node->beginBoundary == WfRangeBoundary::Exclusive)
					{
						INSTRUCTION(Ins::LoadValue({ (vint)1 }));
						INSTRUCTION(Ins::OpAdd(type));
					}
					
					GenerateExpressionInstructions(context, node->end, elementType);
					if (node->endBoundary == WfRangeBoundary::Exclusive)
					{
						INSTRUCTION(Ins::LoadValue({ (vint)1 }));
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
						if (node->test == WfSetTesting::NotIn)
						{
							INSTRUCTION(Ins::OpNot(WfInsType::Bool));
						}
						INSTRUCTION(Ins::LoadValue({}));
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

						if (node->test == WfSetTesting::NotIn)
						{
							INSTRUCTION(Ins::OpNot(WfInsType::Bool));
						}
					}
				}

				void Visit(WfConstructorExpression* node)override
				{
					auto result = context.manager->expressionResolvings[node];

					if (result.type->GetTypeDescriptor()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Struct)
					{
						auto td = result.type->GetTypeDescriptor();
						INSTRUCTION(Ins::CreateStruct(Value::BoxedValue, td));

						for (auto argument : node->arguments)
						{
							auto prop = td->GetPropertyByName(argument->key.Cast<WfReferenceExpression>()->name.value, true);
							GenerateExpressionInstructions(context, argument->value, CopyTypeInfo(prop->GetReturn()));
							INSTRUCTION(Ins::UpdateProperty(prop));
						}
					}
					else if (result.type->GetTypeDescriptor() == description::GetTypeDescriptor<IValueEnumerable>()
						|| result.type->GetTypeDescriptor() == description::GetTypeDescriptor<IValueReadonlyList>()
						|| result.type->GetTypeDescriptor() == description::GetTypeDescriptor<IValueArray>())
					{
						Ptr<ITypeInfo> keyType = CopyTypeInfo(result.type->GetElementType()->GetGenericArgument(0));
						for (auto argument : From(node->arguments).Reverse())
						{
							GenerateExpressionInstructions(context, argument->key, keyType);
						}
						INSTRUCTION(Ins::NewArray(node->arguments.Count()));
					}
					else if (result.type->GetTypeDescriptor() == description::GetTypeDescriptor<IValueList>())
					{
						Ptr<ITypeInfo> keyType = CopyTypeInfo(result.type->GetElementType()->GetGenericArgument(0));
						for (auto argument : From(node->arguments).Reverse())
						{
							GenerateExpressionInstructions(context, argument->key, keyType);
						}
						INSTRUCTION(Ins::NewList(node->arguments.Count()));
					}
					else if (result.type->GetTypeDescriptor() == description::GetTypeDescriptor<IValueObservableList>())
					{
						Ptr<ITypeInfo> keyType = CopyTypeInfo(result.type->GetElementType()->GetGenericArgument(0));
						for (auto argument : From(node->arguments).Reverse())
						{
							GenerateExpressionInstructions(context, argument->key, keyType);
						}
						INSTRUCTION(Ins::NewObservableList(node->arguments.Count()));
					}
					else
					{
						Ptr<ITypeInfo> keyType = CopyTypeInfo(result.type->GetElementType()->GetGenericArgument(0));
						Ptr<ITypeInfo> valueType = CopyTypeInfo(result.type->GetElementType()->GetGenericArgument(1));
						for (auto argument : From(node->arguments).Reverse())
						{
							GenerateExpressionInstructions(context, argument->key, keyType);
							GenerateExpressionInstructions(context, argument->value, valueType);
						}
						INSTRUCTION(Ins::NewDictionary(node->arguments.Count() * 2));
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
						auto scope = context.manager->nodeScopes[node].Obj();
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
						INSTRUCTION(Ins::LoadValue({}));
						INSTRUCTION(Ins::CompareReference());
						break;
					case WfTypeTesting::IsNotNull:
						GenerateExpressionInstructions(context, node->expression);
						INSTRUCTION(Ins::LoadValue({}));
						INSTRUCTION(Ins::CompareReference());
						INSTRUCTION(Ins::OpNot(WfInsType::Bool));
						break;
					case WfTypeTesting::IsType:
						{
							auto scope = context.manager->nodeScopes[node].Obj();
							auto type = CreateTypeInfoFromType(scope, node->type);
							GenerateExpressionInstructions(context, node->expression);
							GenerateTypeTestingInstructions(context, type, node);
						}
						break;
					case WfTypeTesting::IsNotType:
						{
							auto scope = context.manager->nodeScopes[node].Obj();
							auto type = CreateTypeInfoFromType(scope, node->type);
							GenerateExpressionInstructions(context, node->expression);
							GenerateTypeTestingInstructions(context, type, node);
							INSTRUCTION(Ins::OpNot(WfInsType::Bool));
						}
						break;
					default:;
					}
				}

				void Visit(WfTypeOfTypeExpression* node)override
				{
					auto scope = context.manager->nodeScopes[node].Obj();
					auto type = CreateTypeInfoFromType(scope, node->type, false);
					INSTRUCTION(Ins::LoadValue({ type->GetTypeDescriptor() }));
				}

				void Visit(WfTypeOfExpressionExpression* node)override
				{
					GenerateExpressionInstructions(context, node->expression);
					INSTRUCTION(Ins::GetType());
				}

				void Visit(WfAttachEventExpression* node)override
				{
					auto result = context.manager->expressionResolvings[node->event.Obj()];
					if (auto member = node->event.Cast<WfMemberExpression>())
					{
						GenerateExpressionInstructions(context, member->parent);
					}
					else
					{
						VisitThisExpression(node, result.eventInfo->GetOwnerTypeDescriptor());
					}
					GenerateExpressionInstructions(context, node->function);
					INSTRUCTION(Ins::AttachEvent(result.eventInfo));
				}

				void Visit(WfDetachEventExpression* node)override
				{
					auto result = context.manager->expressionResolvings[node->event.Obj()];
					if (auto member = node->event.Cast<WfMemberExpression>())
					{
						GenerateExpressionInstructions(context, member->parent);
					}
					else
					{
						VisitThisExpression(node, result.eventInfo->GetOwnerTypeDescriptor());
					}
					GenerateExpressionInstructions(context, node->handler);
					INSTRUCTION(Ins::DetachEvent(result.eventInfo));
				}

				void Visit(WfObserveExpression* node)override
				{
				}

				void Visit(WfCallExpression* node)override
				{
					for (auto argument : node->arguments)
					{
						GenerateExpressionInstructions(context, argument);
					}

					auto result = context.manager->expressionResolvings[node->function.Obj()];
					if (result.methodInfo)
					{
						if (result.methodInfo->IsStatic())
						{
							INSTRUCTION(Ins::LoadValue({}));
						}
						else if (auto member = node->function.Cast<WfMemberExpression>())
						{
							GenerateExpressionInstructions(context, member->parent);
						}
						else
						{
							VisitThisExpression(node, result.methodInfo->GetOwnerTypeDescriptor());
						}

						INSTRUCTION(Ins::InvokeMethod(result.methodInfo, node->arguments.Count()));
						return;
					}
					else if (result.eventInfo)
					{
						if(auto member = node->function.Cast<WfMemberExpression>())
						{
							GenerateExpressionInstructions(context, member->parent);
						}
						else
						{
							VisitThisExpression(node, result.eventInfo->GetOwnerTypeDescriptor());
						}
						INSTRUCTION(Ins::InvokeEvent(result.eventInfo, node->arguments.Count()));
						return;
					}
					else if (result.symbol)
					{
						if (result.symbol->creatorNode.Cast<WfFunctionDeclaration>())
						{
							if (result.symbol->ownerScope->ownerNode.Cast<WfNewInterfaceExpression>())
							{
								vint functionIndex = context.closureFunctions[result.symbol.Obj()];
								INSTRUCTION(Ins::InvokeWithContext(functionIndex, node->arguments.Count()));
								return;
							}
							else
							{
								vint index = context.globalFunctions.Keys().IndexOf(result.symbol.Obj());
								if (index != -1)
								{
									vint functionIndex = context.globalFunctions.Values()[index];
									INSTRUCTION(Ins::Invoke(functionIndex, node->arguments.Count()));
									return;
								}
							}
						}
					}

					GenerateExpressionInstructions(context, node->function);
					INSTRUCTION(Ins::InvokeProxy(node->arguments.Count()));
				}

				static void VisitFunction(WfCodegenContext& context, WfFunctionDeclaration* node, WfCodegenLambdaContext lc, const Func<WString(vint)>& getName)
				{
					auto scope = context.manager->nodeScopes[node].Obj();
					bool inNewInterfaceExpr = scope->parentScope && scope->parentScope->ownerNode.Cast<WfNewInterfaceExpression>();
					auto functionIndex = AddClosure(context, lc, getName);

					if (inNewInterfaceExpr)
					{
						INSTRUCTION(Ins::LoadFunction(functionIndex));
					}
					else
					{
						auto capture = context.manager->lambdaCaptures.Get(node);
						for (auto symbol : capture->symbols)
						{
							GenerateLoadSymbolInstructions(context, symbol.Obj(), node);
						}
						vint thisCount = PushCapturedThisValues(context, scope->parentScope.Obj(), node);
						INSTRUCTION(Ins::CreateClosureContext(capture->symbols.Count() + thisCount));
						INSTRUCTION(Ins::LoadFunction(functionIndex));
						INSTRUCTION(Ins::CreateClosure());
					}
				}

				void Visit(WfFunctionExpression* node)override
				{
					WfCodegenLambdaContext lc;
					lc.functionExpression = node;
					VisitFunction(context, node->function.Obj(), lc, [=](vint index)
					{
						return L"<lambda:" + node->function->name.value + L"(" + itow(index) + L")> in " + context.functionContext->function->name;
					});
				}

				class NewInterfaceExpressionVisitor : public empty_visitor::DeclarationVisitor
				{
				public:
					WfCodegenContext&						context;
					vint									variableCount = 0;
					List<Ptr<WfLexicalSymbol>>				variableSymbols;
					List<Ptr<WfFunctionDeclaration>>		closureFunctions;
					List<Ptr<WfFunctionDeclaration>>		overrideFunctions;
					WfFunctionDeclaration*					firstFunction = nullptr;

					NewInterfaceExpressionVisitor(WfCodegenContext& _context)
						:context(_context)
					{
					}

					void Dispatch(WfVirtualCfeDeclaration* node)override
					{
						for (auto decl : node->expandedDeclarations)
						{
							decl->Accept(this);
						}
					}

					void Dispatch(WfVirtualCseDeclaration* node)override
					{
						CHECK_FAIL(L"NewInterfaceExpressionVisitor::Visit(WfVirtualCseDeclaration*)#Internal error, Temporary not supported.");
					}

					void Visit(WfFunctionDeclaration* node)override
					{
						if (!firstFunction)
						{
							firstFunction = node;
						}

						if (node->functionKind == WfFunctionKind::Normal)
						{
							closureFunctions.Add(node);
						}
						else
						{
							overrideFunctions.Add(node);
						}
					}

					void Visit(WfVariableDeclaration* node)override
					{
						variableCount++;
					}

					void Execute(WfNewInterfaceExpression* node)
					{
						for (auto memberDecl : node->declarations)
						{
							memberDecl->Accept(this);
						}

						if (firstFunction != nullptr && variableCount > 0)
						{
							auto capture = context.manager->lambdaCaptures.Get(firstFunction);
							CopyFrom(variableSymbols, From(capture->symbols).Take(variableCount));
						}
					}
				};

				void Visit(WfNewClassExpression* node)override
				{
					auto result = context.manager->expressionResolvings[node];
					for (auto argument : node->arguments)
					{
						GenerateExpressionInstructions(context, argument);
					}
					INSTRUCTION(Ins::LoadValue({}));
					INSTRUCTION(Ins::InvokeMethod(result.constructorInfo, node->arguments.Count()));
				}

				void Visit(WfNewInterfaceExpression* node)override
				{
					auto result = context.manager->expressionResolvings[node];
					NewInterfaceExpressionVisitor declVisitor(context);
					declVisitor.Execute(node);

					if (declVisitor.firstFunction != nullptr)
					{
						for (vint i = 0; i < declVisitor.variableCount; i++)
						{
							auto var = declVisitor.variableSymbols[i]->creatorNode.Cast<WfVariableDeclaration>();
							GenerateExpressionInstructions(context, var->expression);
						}

						auto capture = context.manager->lambdaCaptures.Get(declVisitor.firstFunction);
						for (vint i = declVisitor.variableCount; i < capture->symbols.Count(); i++)
						{
							GenerateLoadSymbolInstructions(context, capture->symbols[i].Obj(), node);
						}
						auto scope = context.manager->nodeScopes[node].Obj();
						vint thisCount = PushCapturedThisValues(context, scope, node);
						INSTRUCTION(Ins::LoadValue({}));
						INSTRUCTION(Ins::CreateClosureContext(capture->symbols.Count() + thisCount + 1));

						for (auto func : declVisitor.closureFunctions)
						{
							WfCodegenLambdaContext lc;
							lc.functionDeclaration = func.Obj();
							auto functionIndex = AddClosure(context, lc, [=](vint index)
							{
								return L"<method:" + func->name.value + L"<" + result.type->GetTypeDescriptor()->GetTypeName() + L">(" + itow(index) + L")> in " + context.functionContext->function->name;
							});

							auto scope = context.manager->nodeScopes[func.Obj()].Obj();
							auto symbol = context.manager->GetDeclarationSymbol(scope, func.Obj());
							context.closureFunctions.Add(symbol.Obj(), functionIndex);
						}

						for (auto func : declVisitor.overrideFunctions)
						{
							auto methodInfo = context.manager->interfaceMethodImpls[func.Obj()];
							INSTRUCTION(Ins::LoadMethodInfo(methodInfo));
							WfCodegenLambdaContext lc;
							lc.functionDeclaration = func.Obj();
							VisitFunction(context, func.Obj(), lc, [=, &declVisitor](vint index)
							{
								return L"<method:" + func->name.value + L"<" + result.type->GetTypeDescriptor()->GetTypeName() + L">(" + itow(index + declVisitor.closureFunctions.Count()) + L")> in " + context.functionContext->function->name;
							});
						}
					}
					else
					{
						INSTRUCTION(Ins::LoadValue({}));
						INSTRUCTION(Ins::CreateClosureContext(1));
					}
					INSTRUCTION(Ins::CreateInterface(result.constructorInfo, declVisitor.overrideFunctions.Count() * 2));
				}

				void Visit(WfVirtualCfeExpression* node)override
				{
					GenerateExpressionInstructions(context, node->expandedExpression);
				}

				void Visit(WfVirtualCseExpression* node)override
				{
					GenerateExpressionInstructions(context, node->expandedExpression);
				}
			};

#undef FILL_LABEL_TO_CURRENT
#undef FILL_LABEL_TO_INS
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
