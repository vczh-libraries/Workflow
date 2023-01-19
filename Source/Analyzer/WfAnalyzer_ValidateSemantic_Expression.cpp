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
			using namespace typeimpl;

/***********************************************************************
ValidateSemantic(Expression)
***********************************************************************/

			class ExpandVirtualExpressionVisitor : public Object, public WfVirtualCseExpression::IVisitor
			{
			public:
				WfLexicalScopeManager*				manager;
				Ptr<ITypeInfo>						expectedType;

				ExpandVirtualExpressionVisitor(WfLexicalScopeManager* _manager, Ptr<ITypeInfo> _expectedType)
					:manager(_manager)
					, expectedType(_expectedType)
				{
				}

				void Visit(WfBindExpression* node)override
				{
					ExpandBindExpression(manager, node);
				}

				void Visit(WfNewCoroutineExpression* node)override
				{
					ExpandNewCoroutineExpression(manager, node);
				}

				void Visit(WfMixinCastExpression* node)override
				{
					ExpandMixinCastExpression(manager, node);
				}

				void Visit(WfExpectedTypeCastExpression* node)override
				{
					auto castExpr = Ptr(new WfTypeCastingExpression);
					castExpr->strategy = node->strategy;
					castExpr->expression = CopyExpression(node->expression, true);
					castExpr->type = GetTypeFromTypeInfo(expectedType.Obj());
					node->expandedExpression = castExpr;
				}

				void Visit(WfCoOperatorExpression* node)override
				{
					auto scope = manager->nodeScopes[node].Obj();
					auto functionScope = scope->FindFunctionScope();
					if (functionScope->ownerNode.Cast<WfNewCoroutineExpression>())
					{
						// <co-impl> variable is created after $coroutine{} is generated
						// so the expanding is delayed until $coroutine{} is ready
						ExpandCoOperatorExpression(manager, node);
					}
				}
			};

			class ValidateSemanticExpressionVisitor
				: public Object
				, public WfExpression::IVisitor
				, public WfVirtualCseExpression::IVisitor
			{
			public:
				WfLexicalScopeManager*				manager;
				Ptr<ITypeInfo>						expectedType;
				List<ResolveExpressionResult>&		results;

				ValidateSemanticExpressionVisitor(WfLexicalScopeManager* _manager, Ptr<ITypeInfo> _expectedType, List<ResolveExpressionResult>& _results)
					:manager(_manager)
					, expectedType(_expectedType)
					, results(_results)
				{
				}

				void Visit(WfThisExpression* node)override
				{
					auto scope = manager->nodeScopes[node].Obj();
					Ptr<WfLexicalFunctionConfig> lastConfig;
					while (scope)
					{
						if (auto config = scope->functionConfig)
						{
							lastConfig = config;
							if (!lastConfig->thisAccessable)
							{
								break;
							}
						}

						if (scope->typeOfThisExpr)
						{
							if (!lastConfig)
							{
								break;
							}

							if (lastConfig->thisAccessable)
							{
								auto elementType = Ptr(new TypeDescriptorTypeInfo(scope->typeOfThisExpr, TypeInfoHint::Normal));
								auto pointerType = Ptr(new RawPtrTypeInfo(elementType));

								results.Add(ResolveExpressionResult::ReadonlyType(pointerType));
								return;
							}
						}
						scope = scope->parentScope.Obj();
					}
					manager->errors.Add(WfErrors::WrongThisExpression(node));
				}

				void Visit(WfTopQualifiedExpression* node)override
				{
					if (manager->globalName)
					{
						vint index = manager->globalName->children.Keys().IndexOf(node->name.value);
						if (index != -1)
						{
							results.Add(ResolveExpressionResult::ScopeName(manager->globalName->children.Values()[index]));
							return;
						}
					}
					manager->errors.Add(WfErrors::TopQualifiedSymbolNotExists(node, node->name.value));
				}

				void ResolveName(WfExpression* node, const WString& name)
				{
					auto scope = manager->nodeScopes[node].Obj();
					List<ResolveExpressionResult> nameResults;
					manager->ResolveName(scope, name, nameResults);
					
					for (vint i = 0; i < nameResults.Count(); i++)
					{
						auto& result = nameResults[i];
						if (result.symbol)
						{
							if (!result.type)
							{
								manager->errors.Add(WfErrors::ExpressionCannotResolveType(node, result.symbol));
							}
							else if (!result.symbol->creatorNode.Cast<WfDeclaration>() || result.symbol->creatorNode.Cast<WfVariableDeclaration>())
							{
								bool readonlyCaptured = false;
								if (!result.symbol->ownerScope->ownerNode.Cast<WfModule>() && !result.symbol->ownerScope->ownerNode.Cast<WfNamespaceDeclaration>())
								{
									auto currentScope = scope;
									WfLexicalScope* firstConfigScope = nullptr;
									WfLexicalScope* lastConfigScope = nullptr;
									while (currentScope)
									{
										if (currentScope->functionConfig)
										{
											if (!firstConfigScope)
											{
												firstConfigScope = currentScope;
											}
											lastConfigScope = currentScope;
										}

										if (result.symbol->ownerScope == currentScope)
										{
											if (firstConfigScope && firstConfigScope->functionConfig->lambda)
											{
												readonlyCaptured = true;
											}

											if (currentScope->ownerNode.Cast<WfNewInterfaceExpression>())
											{
												if (firstConfigScope)
												{
													readonlyCaptured = firstConfigScope != lastConfigScope;

													if (
														lastConfigScope->parentScope != currentScope ||
														!lastConfigScope->ownerNode.Cast<WfFunctionDeclaration>() ||
														(lastConfigScope->ownerNodeSource && lastConfigScope->ownerNodeSource != currentScope->ownerNode)
														)
													{
														manager->errors.Add(WfErrors::FieldCannotInitializeUsingEachOther(node, result));
													}
												}
												else
												{
													manager->errors.Add(WfErrors::FieldCannotInitializeUsingEachOther(node, result));
												}
											}
											break;
										}

										if (currentScope->ownerNode.Cast<WfNewInterfaceExpression>())
										{
											auto capture = manager->lambdaCaptures[currentScope->ownerNode.Obj()];
											if (!capture->ctorArgumentSymbols.Contains(result.symbol.Obj()))
											{
												if (lastConfigScope == nullptr)
												{
													capture->ctorArgumentSymbols.Add(result.symbol);
												}
												else
												{
													auto functionCapture = manager->lambdaCaptures[lastConfigScope->ownerNode.Obj()];
													if (capture != functionCapture)
													{
														capture->ctorArgumentSymbols.Add(result.symbol);
													}
												}
											}
										}

										if (currentScope->functionConfig)
										{
											if (currentScope->functionConfig->lambda)
											{
												auto capture = manager->lambdaCaptures[currentScope->ownerNode.Obj()];
												if (!capture->symbols.Contains(result.symbol.Obj()))
												{
													capture->symbols.Add(result.symbol);
												}
											}
										}
										currentScope = currentScope->parentScope.Obj();
									}
								}

								if (readonlyCaptured)
								{
									results.Add(ResolveExpressionResult::ReadonlySymbol(result.symbol));
								}
								else
								{
									results.Add(ResolveExpressionResult::Symbol(result.symbol));
								}
							}
							else
							{
								results.Add(result);
							}
						}
						else
						{
							if (result.propertyInfo && dynamic_cast<WfReferenceExpression*>(node))
							{
								auto currentScope = scope;
								WfLexicalScope* firstConfigScope = nullptr;
								WfLexicalScope* lastConfigScope = nullptr;
								while (currentScope)
								{
									if (currentScope->functionConfig)
									{
										if (!firstConfigScope)
										{
											firstConfigScope = currentScope;
										}
										lastConfigScope = currentScope;
									}

									if (currentScope->ownerNode.Cast<WfClassDeclaration>() && currentScope->typeOfThisExpr == result.propertyInfo->GetOwnerTypeDescriptor())
									{
										if (firstConfigScope)
										{
											bool inMethodBody = 
												lastConfigScope->parentScope == currentScope &&
												lastConfigScope->ownerNodeSource == currentScope->ownerNode &&
												lastConfigScope->ownerNode.Cast<WfFunctionDeclaration>();
											bool inDtorBody = lastConfigScope->ownerNode.Cast<WfDestructorDeclaration>();
											bool inCtorBody = lastConfigScope->parentScope->ownerNode.Cast<WfConstructorDeclaration>();
											bool inStateBody = lastConfigScope->ownerNode.Cast<WfStateDeclaration>();

											if (!inMethodBody && !inDtorBody && !inCtorBody && !inStateBody)
											{
												manager->errors.Add(WfErrors::FieldCannotInitializeUsingEachOther(node, result));
											}
										}
										else if (!scope->ownerNode.Cast<WfConstructorDeclaration>())
										{
											manager->errors.Add(WfErrors::FieldCannotInitializeUsingEachOther(node, result));
										}
									}
									currentScope = currentScope->parentScope.Obj();
								}
							}
							results.Add(result);
						}
					}

					if (results.Count() == 0)
					{
						if (nameResults.Count() > 0)
						{
							for (auto result : nameResults)
							{
								manager->errors.Add(WfErrors::ExpressionCannotResolveType(node, result.symbol));
							}
						}
						else
						{
							manager->errors.Add(WfErrors::ReferenceNotExists(node, name));
						}
					}
				}

				void Visit(WfReferenceExpression* node)override
				{
					if (expectedType && (expectedType->GetTypeDescriptor()->GetTypeDescriptorFlags() & TypeDescriptorFlags::EnumType) != TypeDescriptorFlags::Undefined)
					{
						auto scope = manager->nodeScopes[node].Obj();
						List<ResolveExpressionResult> testResults;
						manager->ResolveName(scope, node->name.value, testResults);

						if (testResults.Count() == 0)
						{
							auto enumType = expectedType->GetTypeDescriptor()->GetEnumType();
							if (enumType->IndexOfItem(node->name.value) == -1)
							{
								manager->errors.Add(WfErrors::EnumItemNotExists(node, expectedType->GetTypeDescriptor(), node->name.value));
							}
							else
							{
								results.Add(ResolveExpressionResult::ReadonlyType(expectedType));
							}
							return;
						}
					}

					ResolveName(node, node->name.value);
					for (auto result : results)
					{
						ITypeDescriptor* td = nullptr;
						if (result.methodInfo)
						{
							td = result.methodInfo->GetOwnerTypeDescriptor();
						}
						else if (result.propertyInfo)
						{
							td = result.propertyInfo->GetOwnerTypeDescriptor();
						}
						else if (result.eventInfo)
						{
							td = result.eventInfo->GetOwnerTypeDescriptor();
						}

						if (td)
						{
							auto scope = manager->nodeScopes[node].Obj();
							bool visibleToNonStatic = false;
							while (scope)
							{
								if (scope->functionConfig)
								{
									visibleToNonStatic = scope->functionConfig->thisAccessable || scope->functionConfig->parentThisAccessable;
								}

								if (scope->typeOfThisExpr && scope->typeOfThisExpr->CanConvertTo(td))
								{
									if (!visibleToNonStatic)
									{
										if (result.methodInfo)
										{
											if (!result.methodInfo->IsStatic())
											{
												manager->errors.Add(WfErrors::CannotCallMemberInStaticFunction(node, result));
											}
										}
										else if (result.propertyInfo)
										{
											manager->errors.Add(WfErrors::CannotCallMemberInStaticFunction(node, result));
										}
										else if (result.eventInfo)
										{
											manager->errors.Add(WfErrors::CannotCallMemberInStaticFunction(node, result));
										}
										break;
									}
								}
								scope = scope->parentScope.Obj();
							}
						}
					}
				}

				void Visit(WfOrderedNameExpression* node)override
				{
					ResolveName(node, node->name.value);
				}

				void Visit(WfOrderedLambdaExpression* node)override
				{
					auto scope = manager->nodeScopes[node].Obj();
					List<Ptr<WfLexicalSymbol>> parameterSymbols;
					CopyFrom(
						parameterSymbols,
						Range<vint>(0, scope->symbols.Count())
							.Select([scope](vint index)->Ptr<WfLexicalSymbol>{return scope->symbols.GetByIndex(index)[0];})
							.OrderBy([](Ptr<WfLexicalSymbol> a, Ptr<WfLexicalSymbol> b)
							{
								vint aId = wtoi(a->name.Sub(1, a->name.Length() - 1));
								vint bId = wtoi(b->name.Sub(1, a->name.Length() - 1));
								return aId - bId;
							})
						);
					Ptr<ITypeInfo> resultType = expectedType;

					if (!expectedType && parameterSymbols.Count() > 0)
					{
						manager->errors.Add(WfErrors::OrderedLambdaCannotResolveType(node));
						return;
					}
					else if (expectedType)
					{
						ITypeInfo* type = expectedType.Obj();
						if (type->GetDecorator() != ITypeInfo::SharedPtr)
						{
							goto ORDERED_FAILED;
						}
						type = type->GetElementType();
						if (type->GetDecorator() != ITypeInfo::Generic)
						{
							goto ORDERED_FAILED;
						}
						{
							ITypeInfo* functionType = type->GetElementType();
							if (functionType->GetDecorator() != ITypeInfo::TypeDescriptor)
							{
								goto ORDERED_FAILED;
							}
							if (functionType->GetTypeDescriptor() != description::GetTypeDescriptor<IValueFunctionProxy>())
							{
								goto ORDERED_FAILED;
							}
						}

						if (type->GetGenericArgumentCount() != parameterSymbols.Count() + 1)
						{
							goto ORDERED_FAILED;
						}

						auto resultType = Ptr(type->GetGenericArgument(0));
						for (auto [symbol, index] : indexed(parameterSymbols))
						{
							symbol->typeInfo = Ptr(type->GetGenericArgument(index + 1));
							symbol->type = GetTypeFromTypeInfo(symbol->typeInfo.Obj());
						}
						GetExpressionType(manager, node->body, resultType);
					}
					else
					{
						auto bodyType = GetExpressionType(manager, node->body, 0);
						if (bodyType)
						{
							auto funcType = Ptr(new TypeDescriptorTypeInfo(description::GetTypeDescriptor<IValueFunctionProxy>(), TypeInfoHint::Normal));
							auto genericType = Ptr(new GenericTypeInfo(funcType));
							genericType->AddGenericArgument(bodyType);

							resultType = Ptr(new SharedPtrTypeInfo(genericType));
						}
					}

					goto ORDERED_FINISHED;
				ORDERED_FAILED:
					manager->errors.Add(WfErrors::OrderedLambdaCannotImplicitlyConvertToType(node, expectedType.Obj()));
				ORDERED_FINISHED:
					if (resultType)
					{
						results.Add(ResolveExpressionResult::ReadonlyType(resultType));
					}
				}

				void Visit(WfMemberExpression* node)override
				{
					Ptr<ITypeInfo> type = GetExpressionType(manager, node->parent, 0);
					if (type)
					{
						SortedList<ITypeDescriptor*> searchedTypes;
						manager->ResolveMember(type->GetTypeDescriptor(), node->name.value, false, searchedTypes, results);

						if (results.Count() == 0)
						{
							manager->errors.Add(WfErrors::MemberNotExists(node, type->GetTypeDescriptor(), node->name.value));
						}
					}
				}

				void Visit(WfChildExpression* node)override
				{
					if (Ptr<WfLexicalScopeName> scopeName = GetExpressionScopeName(manager, node->parent))
					{
						vint index = scopeName->children.Keys().IndexOf(node->name.value);
						if (index != -1)
						{
							results.Add(ResolveExpressionResult::ScopeName(scopeName->children.Values()[index]));
							return;
						}
						
						if (scopeName->typeDescriptor)
						{
							SortedList<ITypeDescriptor*> searchedTypes;
							manager->ResolveMember(scopeName->typeDescriptor, node->name.value, true, searchedTypes, results);

							if (results.Count() > 0)
							{
								for (auto result : results)
								{
									if (result.methodInfo)
									{
										if (!result.methodInfo->IsStatic())
										{
											manager->errors.Add(WfErrors::CannotCallMemberOutsideOfClass(node, result));
										}
									}
									else if (result.propertyInfo)
									{
										manager->errors.Add(WfErrors::CannotCallMemberOutsideOfClass(node, result));
									}
									else if (result.eventInfo)
									{
										manager->errors.Add(WfErrors::CannotCallMemberOutsideOfClass(node, result));
									}
								}
								return;
							}
						}

						if (scopeName->typeDescriptor != nullptr && (scopeName->typeDescriptor->GetTypeDescriptorFlags() & TypeDescriptorFlags::EnumType) != TypeDescriptorFlags::Undefined)
						{
							manager->errors.Add(WfErrors::EnumItemNotExists(node, scopeName->typeDescriptor, node->name.value));
						}
						else
						{
							manager->errors.Add(WfErrors::ChildSymbolNotExists(node, scopeName, node->name.value));
						}
					}
				}

				void Visit(WfLiteralExpression* node)override
				{
					if (node->value == WfLiteralValue::Null)
					{
						if (!expectedType)
						{
							manager->errors.Add(WfErrors::NullCannotResolveType(node));
						}
						else if (!IsNullAcceptableType(expectedType.Obj()))
						{
							manager->errors.Add(WfErrors::NullCannotImplicitlyConvertToType(node, expectedType.Obj()));
						}

						results.Add(ResolveExpressionResult::ReadonlyType(expectedType));
					}
					else
					{
						results.Add(ResolveExpressionResult::ReadonlyType(TypeInfoRetriver<bool>::CreateTypeInfo()));
					}
				}

				void Visit(WfFloatingExpression* node)override
				{
					auto typeDescriptor = expectedType ? expectedType->GetTypeDescriptor() : nullptr;
					if (!typeDescriptor || typeDescriptor->GetTypeDescriptorFlags() == TypeDescriptorFlags::Object || typeDescriptor == description::GetTypeDescriptor<WString>())
					{
						typeDescriptor = description::GetTypeDescriptor<double>();
					}

					if (auto serializableType = typeDescriptor->GetSerializableType())
					{
						Value output;
						if (serializableType->Deserialize(node->value.value, output))
						{
							results.Add(ResolveExpressionResult::ReadonlyType(Ptr(new TypeDescriptorTypeInfo(typeDescriptor, TypeInfoHint::Normal))));
							return;
						}
					}
					manager->errors.Add(WfErrors::FloatingLiteralOutOfRange(node));
				}

				void Visit(WfIntegerExpression* node)override
				{
					auto typeDescriptor = expectedType ? expectedType->GetTypeDescriptor() : nullptr;
					if (!typeDescriptor || typeDescriptor->GetTypeDescriptorFlags() == TypeDescriptorFlags::Object || typeDescriptor==description::GetTypeDescriptor<WString>())
					{
						typeDescriptor = manager->cputdSInt;
					}

					if (auto serializableType = typeDescriptor->GetSerializableType())
					{
						Value output;
						if (serializableType->Deserialize(node->value.value, output))
						{
							results.Add(ResolveExpressionResult::ReadonlyType(Ptr(new TypeDescriptorTypeInfo(typeDescriptor, TypeInfoHint::Normal))));
							return;
						}
					}
					manager->errors.Add(WfErrors::IntegerLiteralOutOfRange(node));
				}

				void Visit(WfStringExpression* node)override
				{
					results.Add(ResolveExpressionResult::ReadonlyType(TypeInfoRetriver<WString>::CreateTypeInfo()));
				}

				void Visit(WfUnaryExpression* node)override
				{
					Ptr<ITypeInfo> typeInfo = GetExpressionType(manager, node->operand, 0);
					if (typeInfo)
					{
						TypeFlag flag = GetTypeFlag(typeInfo.Obj());
						switch (node->op)
						{
						case WfUnaryOperator::Not:
							switch (flag)
							{
							case TypeFlag::F4:
							case TypeFlag::F8:
							case TypeFlag::String:
							case TypeFlag::Enum:
							case TypeFlag::Struct:
							case TypeFlag::Others:
								manager->errors.Add(WfErrors::UnaryOperatorOnWrongType(node, typeInfo.Obj()));
								break;
							default:;
							}
							break;
						case WfUnaryOperator::Positive:
							switch (flag)
							{
							case TypeFlag::Bool:
							case TypeFlag::String:
							case TypeFlag::Enum:
							case TypeFlag::Struct:
							case TypeFlag::Others:
								manager->errors.Add(WfErrors::UnaryOperatorOnWrongType(node, typeInfo.Obj()));
								break;
							default:;
							}
							break;
						case WfUnaryOperator::Negative:
							switch (flag)
							{
							case TypeFlag::Bool:
							case TypeFlag::U1:
							case TypeFlag::U2:
							case TypeFlag::U4:
							case TypeFlag::U8:
							case TypeFlag::String:
							case TypeFlag::Enum:
							case TypeFlag::Struct:
							case TypeFlag::Others:
								manager->errors.Add(WfErrors::UnaryOperatorOnWrongType(node, typeInfo.Obj()));
								break;
							default:;
							}
							break;
						default:;
						}

						results.Add(ResolveExpressionResult::ReadonlyType(typeInfo));
					}
				}

				void Visit(WfBinaryExpression* node)override
				{
					if (node->op == WfBinaryOperator::Assign)
					{
						Ptr<ITypeInfo> variableType = GetLeftValueExpressionType(manager, node->first);
						GetExpressionType(manager, node->second, variableType);
						if (variableType)
						{
							results.Add(ResolveExpressionResult::ReadonlyType(variableType));
						}
					}
					else if (node->op == WfBinaryOperator::Index)
					{
						Ptr<ITypeInfo> containerType = GetExpressionType(manager, node->first, 0);
						if (containerType)
						{
							if (containerType->GetDecorator() == ITypeInfo::SharedPtr)
							{
								ITypeInfo* genericType = containerType->GetElementType();
								Ptr<ITypeInfo> indexType;
								Ptr<ITypeInfo> resultType;
								bool leftValue = false;

								if (genericType->GetDecorator() == ITypeInfo::Generic)
								{
									ITypeInfo* classType = genericType->GetElementType();
									if (classType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueReadonlyList>())
									{
										indexType = manager->cputiSInt;
										resultType = CopyTypeInfo(genericType->GetGenericArgument(0));
									}
									else if (classType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueArray>())
									{
										indexType = manager->cputiSInt;
										resultType = CopyTypeInfo(genericType->GetGenericArgument(0));
										leftValue = true;
									}
									else if (classType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueList>())
									{
										indexType = manager->cputiSInt;
										resultType = CopyTypeInfo(genericType->GetGenericArgument(0));
										leftValue = true;
									}
									else if (classType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueObservableList>())
									{
										indexType = manager->cputiSInt;
										resultType = CopyTypeInfo(genericType->GetGenericArgument(0));
										leftValue = true;
									}
									else if (classType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueReadonlyDictionary>())
									{
										indexType = CopyTypeInfo(genericType->GetGenericArgument(0));
										resultType = CopyTypeInfo(genericType->GetGenericArgument(1));
									}
									else if (classType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueDictionary>())
									{
										indexType = CopyTypeInfo(genericType->GetGenericArgument(0));
										resultType = CopyTypeInfo(genericType->GetGenericArgument(1));
										leftValue = true;
									}
									else
									{
										manager->errors.Add(WfErrors::IndexOperatorOnWrongType(node, containerType.Obj()));
									}
								}
								else
								{
									if (genericType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueReadonlyList>())
									{
										indexType = manager->cputiSInt;
										resultType = TypeInfoRetriver<Value>::CreateTypeInfo();
									}
									else if (genericType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueArray>())
									{
										indexType = manager->cputiSInt;
										resultType = TypeInfoRetriver<Value>::CreateTypeInfo();
										leftValue = true;
									}
									else if (genericType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueList>())
									{
										indexType = manager->cputiSInt;
										resultType = TypeInfoRetriver<Value>::CreateTypeInfo();
										leftValue = true;
									}
									else if (genericType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueObservableList>())
									{
										indexType = manager->cputiSInt;
										resultType = TypeInfoRetriver<Value>::CreateTypeInfo();
										leftValue = true;
									}
									else if (genericType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueReadonlyDictionary>())
									{
										indexType = TypeInfoRetriver<Value>::CreateTypeInfo();
										resultType = TypeInfoRetriver<Value>::CreateTypeInfo();
									}
									else if (genericType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueDictionary>())
									{
										indexType = TypeInfoRetriver<Value>::CreateTypeInfo();
										resultType = TypeInfoRetriver<Value>::CreateTypeInfo();
										leftValue = true;
									}
									else
									{
										manager->errors.Add(WfErrors::IndexOperatorOnWrongType(node, containerType.Obj()));
									}
								}

								GetExpressionType(manager, node->second, indexType);
								if (resultType)
								{
									if (leftValue)
									{
										results.Add(ResolveExpressionResult::WritableType(resultType));
									}
									else
									{
										results.Add(ResolveExpressionResult::ReadonlyType(resultType));
									}
								}
							}
							else
							{
								manager->errors.Add(WfErrors::IndexOperatorOnWrongType(node, containerType.Obj()));
							}
						}
					}
					else if (node->op == WfBinaryOperator::FlagAnd)
					{
						auto typeA = GetExpressionType(manager, node->first, expectedType);
						auto typeB = GetExpressionType(manager, node->second, expectedType);

						if (typeA && typeB)
						{
							if (typeA->GetDecorator() == ITypeInfo::TypeDescriptor && typeB->GetDecorator() == ITypeInfo::TypeDescriptor)
							{
								auto stringType = TypeInfoRetriver<WString>::CreateTypeInfo();
								if (CanConvertToType(typeA.Obj(), stringType.Obj(), false) && CanConvertToType(typeB.Obj(), stringType.Obj(), false))
								{
									results.Add(ResolveExpressionResult::ReadonlyType(stringType));
									return;
								}
								else if (auto type = GetMergedType(typeA, typeB))
								{
									if (type->GetTypeDescriptor()->GetTypeDescriptorFlags() != TypeDescriptorFlags::FlagEnum)
									{
										manager->errors.Add(WfErrors::IncorrectTypeForUnion(node->first.Obj(), type.Obj()));
									}
									results.Add(ResolveExpressionResult::ReadonlyType(type));
									return;
								}
							}
							manager->errors.Add(WfErrors::CannotMergeTwoType(node, typeA.Obj(), typeB.Obj()));
						}
					}
					else if (node->op == WfBinaryOperator::FlagOr)
					{
						auto typeA = GetExpressionType(manager, node->first, expectedType);
						auto typeB = GetExpressionType(manager, node->second, expectedType);

						if (typeA && typeB)
						{
							if (typeA->GetDecorator() == ITypeInfo::TypeDescriptor && typeB->GetDecorator() == ITypeInfo::TypeDescriptor)
							{
								if (auto type = GetMergedType(typeA, typeB))
								{
									if (type->GetTypeDescriptor()->GetTypeDescriptorFlags() != TypeDescriptorFlags::FlagEnum)
									{
										manager->errors.Add(WfErrors::IncorrectTypeForIntersect(node->first.Obj(), type.Obj()));
									}
									results.Add(ResolveExpressionResult::ReadonlyType(type));
									return;
								}
							}
							manager->errors.Add(WfErrors::CannotMergeTwoType(node, typeA.Obj(), typeB.Obj()));
						}
					}
					else if (node->op == WfBinaryOperator::FailedThen)
					{
						Ptr<ITypeInfo> firstType = GetExpressionType(manager, node->first, 0);
						bool depend = IsExpressionDependOnExpectedType(manager, node->second, firstType);
						Ptr<ITypeInfo> secondType = GetExpressionType(manager, node->second, (depend ? firstType : nullptr));

						if (firstType && secondType)
						{
							if (auto mergedType = GetMergedType(firstType, secondType))
							{
								results.Add(ResolveExpressionResult::ReadonlyType(mergedType));
							}
							else
							{
								manager->errors.Add(WfErrors::CannotMergeTwoType(node, firstType.Obj(), secondType.Obj()));
							}
						}
					}
					else
					{
						Ptr<ITypeInfo> firstType = GetExpressionType(manager, node->first, 0);
						Ptr<ITypeInfo> secondType = GetExpressionType(manager, node->second, 0);
						Ptr<ITypeInfo> elementType;
						if (firstType && secondType)
						{
							if (!(elementType = GetMergedType(firstType, secondType)))
							{
								manager->errors.Add(WfErrors::CannotMergeTwoType(node, firstType.Obj(), secondType.Obj()));
							}
						}

						if (elementType)
						{
							TypeFlag flag = GetTypeFlag(elementType.Obj());
							TypeFlag* selectedTable = 0;
							switch (node->op)
							{
							case WfBinaryOperator::Exp:
								{
									static TypeFlag conversionTable[(vint)TypeFlag::Count] = {
										/*Bool		*/TypeFlag::Unknown,
										/*I1		*/TypeFlag::F4,
										/*I2		*/TypeFlag::F4,
										/*I4		*/TypeFlag::F8,
										/*I8		*/TypeFlag::F8,
										/*U1		*/TypeFlag::F4,
										/*U2		*/TypeFlag::F4,
										/*U4		*/TypeFlag::F8,
										/*U8		*/TypeFlag::F8,
										/*F4		*/TypeFlag::F4,
										/*F8		*/TypeFlag::F8,
										/*String	*/TypeFlag::Unknown,
										/*Enum		*/TypeFlag::Unknown,
										/*Struct	*/TypeFlag::Unknown,
										/*Others	*/TypeFlag::Unknown,
									};
									selectedTable = conversionTable;
								}
								break;
							case WfBinaryOperator::Add:
							case WfBinaryOperator::Sub:
							case WfBinaryOperator::Mul:
							case WfBinaryOperator::Div:
								{
									static TypeFlag conversionTable[(vint)TypeFlag::Count] = {
										/*Bool		*/TypeFlag::Unknown,
										/*I1		*/TypeFlag::I4,
										/*I2		*/TypeFlag::I4,
										/*I4		*/TypeFlag::I4,
										/*I8		*/TypeFlag::I8,
										/*U1		*/TypeFlag::U4,
										/*U2		*/TypeFlag::U4,
										/*U4		*/TypeFlag::U4,
										/*U8		*/TypeFlag::U8,
										/*F4		*/TypeFlag::F4,
										/*F8		*/TypeFlag::F8,
										/*String	*/TypeFlag::Unknown,
										/*Enum		*/TypeFlag::Unknown,
										/*Struct	*/TypeFlag::Unknown,
										/*Others	*/TypeFlag::Unknown,
									};
									selectedTable = conversionTable;
								}
								break;
							case WfBinaryOperator::Mod:
							case WfBinaryOperator::Shl:
							case WfBinaryOperator::Shr:
								{
									static TypeFlag conversionTable[(vint)TypeFlag::Count] = {
										/*Bool		*/TypeFlag::Unknown,
										/*I1		*/TypeFlag::I4,
										/*I2		*/TypeFlag::I4,
										/*I4		*/TypeFlag::I4,
										/*I8		*/TypeFlag::I8,
										/*U1		*/TypeFlag::U4,
										/*U2		*/TypeFlag::U4,
										/*U4		*/TypeFlag::U4,
										/*U8		*/TypeFlag::U8,
										/*F4		*/TypeFlag::Unknown,
										/*F8		*/TypeFlag::Unknown,
										/*String	*/TypeFlag::Unknown,
										/*Enum		*/TypeFlag::Unknown,
										/*Struct	*/TypeFlag::Unknown,
										/*Others	*/TypeFlag::Unknown,
									};
									selectedTable = conversionTable;
								}
								break;
							case WfBinaryOperator::LT:
							case WfBinaryOperator::GT:
							case WfBinaryOperator::LE:
							case WfBinaryOperator::GE:
								{
									static TypeFlag conversionTable[(vint)TypeFlag::Count] = {
										/*Bool		*/TypeFlag::Unknown,
										/*I1		*/TypeFlag::Bool,
										/*I2		*/TypeFlag::Bool,
										/*I4		*/TypeFlag::Bool,
										/*I8		*/TypeFlag::Bool,
										/*U1		*/TypeFlag::Bool,
										/*U2		*/TypeFlag::Bool,
										/*U4		*/TypeFlag::Bool,
										/*U8		*/TypeFlag::Bool,
										/*F4		*/TypeFlag::Bool,
										/*F8		*/TypeFlag::Bool,
										/*String	*/TypeFlag::Bool,
										/*Enum		*/TypeFlag::Unknown,
										/*Struct	*/TypeFlag::Unknown,
										/*Others	*/TypeFlag::Unknown,
									};
									selectedTable = conversionTable;
								}
								break;
							case WfBinaryOperator::EQ:
							case WfBinaryOperator::NE:
								{
									results.Add(ResolveExpressionResult::ReadonlyType(TypeInfoRetriver<bool>::CreateTypeInfo()));
									return;
								}
								break;
							case WfBinaryOperator::Xor:
							case WfBinaryOperator::And:
							case WfBinaryOperator::Or:
								{
									static TypeFlag conversionTable[(vint)TypeFlag::Count] = {
										/*Bool		*/TypeFlag::Bool,
										/*I1		*/TypeFlag::I1,
										/*I2		*/TypeFlag::I2,
										/*I4		*/TypeFlag::I4,
										/*I8		*/TypeFlag::I8,
										/*U1		*/TypeFlag::U1,
										/*U2		*/TypeFlag::U2,
										/*U4		*/TypeFlag::U4,
										/*U8		*/TypeFlag::U8,
										/*F4		*/TypeFlag::Unknown,
										/*F8		*/TypeFlag::Unknown,
										/*String	*/TypeFlag::Unknown,
										/*Enum		*/TypeFlag::Unknown,
										/*Struct	*/TypeFlag::Unknown,
										/*Others	*/TypeFlag::Unknown,
									};
									selectedTable = conversionTable;
								}
								break;
							default:;
							}

							TypeFlag resultFlag = selectedTable[(vint)flag];
							if (resultFlag == TypeFlag::Unknown)
							{
								manager->errors.Add(WfErrors::BinaryOperatorOnWrongType(node, elementType.Obj()));
							}
							else
							{
								results.Add(ResolveExpressionResult::ReadonlyType(CreateTypeInfoFromTypeFlag(resultFlag)));
							}
						}
					}
				}

				void Visit(WfLetExpression* node)override
				{
					auto scope = manager->nodeScopes[node].Obj();

					for (auto variable : node->variables)
					{
						auto symbol = scope->symbols[variable->name.value][0];
						symbol->typeInfo = GetExpressionType(manager, variable->value, 0);
						if (symbol->typeInfo)
						{
							symbol->type = GetTypeFromTypeInfo(symbol->typeInfo.Obj());
						}
					}
					Ptr<ITypeInfo> type = GetExpressionType(manager, node->expression, expectedType);
					if (type)
					{
						results.Add(ResolveExpressionResult::ReadonlyType(type));
					}
				}

				void Visit(WfIfExpression* node)override
				{
					Ptr<ITypeInfo> boolType = TypeInfoRetriver<bool>::CreateTypeInfo();
					GetExpressionType(manager, node->condition, boolType);

					Ptr<ITypeInfo> firstType, secondType;

					if (expectedType)
					{
						firstType = GetExpressionType(manager, node->trueBranch, expectedType);
						secondType = GetExpressionType(manager, node->falseBranch, expectedType);
					}
					else
					{
						bool resolveFirst = !IsExpressionDependOnExpectedType(manager, node->trueBranch, false);
						bool resolveSecond = !IsExpressionDependOnExpectedType(manager, node->falseBranch, false);

						if (resolveFirst == resolveSecond)
						{
							firstType = GetExpressionType(manager, node->trueBranch, 0);
							secondType = GetExpressionType(manager, node->falseBranch, 0);
						}
						else if (resolveFirst)
						{
							firstType = GetExpressionType(manager, node->trueBranch, 0);
							secondType = GetExpressionType(manager, node->falseBranch, firstType);
						}
						else if (resolveSecond)
						{
							secondType = GetExpressionType(manager, node->falseBranch, 0);
							firstType = GetExpressionType(manager, node->trueBranch, secondType);
						}
					}
					
					if (firstType && !secondType)
					{
						results.Add(ResolveExpressionResult::ReadonlyType(firstType));
					}
					else if (!firstType && secondType)
					{
						results.Add(ResolveExpressionResult::ReadonlyType(secondType));
					}
					else if (firstType && secondType)
					{
						if (auto mergedType = GetMergedType(firstType, secondType))
						{
							results.Add(ResolveExpressionResult::ReadonlyType(mergedType));
						}
						else
						{
							manager->errors.Add(WfErrors::CannotMergeTwoType(node, firstType.Obj(), secondType.Obj()));
						}
					}
				}

				void Visit(WfRangeExpression* node)override
				{
					Ptr<ITypeInfo> firstType = GetExpressionType(manager, node->begin, 0);
					Ptr<ITypeInfo> secondType = GetExpressionType(manager, node->end, 0);
					Ptr<ITypeInfo> elementType;

					if (firstType && !secondType)
					{
						elementType = firstType;
					}
					else if (!firstType && secondType)
					{
						elementType = secondType;
					}
					else if (firstType && secondType)
					{
						if (!(elementType = GetMergedType(firstType, secondType)))
						{
							manager->errors.Add(WfErrors::CannotMergeTwoType(node, firstType.Obj(), secondType.Obj()));
						}
					}

					if (elementType)
					{
						TypeFlag flag = GetTypeFlag(elementType.Obj());
						switch (flag)
						{
						case TypeFlag::I1:
						case TypeFlag::I2:
						case TypeFlag::I4:
						case TypeFlag::I8:
						case TypeFlag::U1:
						case TypeFlag::U2:
						case TypeFlag::U4:
						case TypeFlag::U8:
							break;
						default:
							manager->errors.Add(WfErrors::RangeShouldBeInteger(node, elementType.Obj()));
						}

						auto enumerableType = Ptr(new TypeDescriptorTypeInfo(description::GetTypeDescriptor<IValueEnumerable>(), TypeInfoHint::Normal));
						auto genericType = Ptr(new GenericTypeInfo(enumerableType));
						genericType->AddGenericArgument(elementType);

						auto pointerType = Ptr(new SharedPtrTypeInfo(genericType));
						results.Add(ResolveExpressionResult::ReadonlyType(pointerType));
					}
				}

				void Visit(WfSetTestingExpression* node)override
				{
					Ptr<ITypeInfo> elementType = GetExpressionType(manager, node->element, 0);

					if (auto range = node->collection.Cast<WfRangeExpression>())
					{
						Ptr<ITypeInfo> beginType = GetExpressionType(manager, range->begin, 0);
						Ptr<ITypeInfo> endType = GetExpressionType(manager, range->end, 0);

						if (elementType && beginType)
						{
							if (!GetMergedType(elementType, beginType))
							{
								manager->errors.Add(WfErrors::CannotMergeTwoType(node, elementType.Obj(), beginType.Obj()));
							}
						}
						if (elementType && endType)
						{
							if (!GetMergedType(elementType, endType))
							{
								manager->errors.Add(WfErrors::CannotMergeTwoType(node, elementType.Obj(), endType.Obj()));
							}
						}
					}
					else
					{
						Ptr<ITypeInfo> itemType = GetEnumerableExpressionItemType(manager, node->collection, 0);
						if (elementType && itemType)
						{
							if (!GetMergedType(elementType, itemType))
							{
								manager->errors.Add(WfErrors::CannotMergeTwoType(node, elementType.Obj(), itemType.Obj()));
							}
						}
					}

					results.Add(ResolveExpressionResult::ReadonlyType(TypeInfoRetriver<bool>::CreateTypeInfo()));
				}

				void Visit(WfConstructorExpression* node)override
				{
					if (expectedType && expectedType->GetTypeDescriptor()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Struct)
					{
						SortedList<WString> fields;
						for (auto argument : node->arguments)
						{
							if (!argument->value)
							{
								manager->errors.Add(WfErrors::ConstructorMixStructAndList(node));
								return;
							}
							else if (auto field = argument->key.Cast<WfReferenceExpression>())
							{
								if (auto prop = expectedType->GetTypeDescriptor()->GetPropertyByName(field->name.value, true))
								{
									if (fields.Contains(field->name.value))
									{
										manager->errors.Add(WfErrors::DuplicatedConstructorField(field.Obj()));
									}
									else
									{
										fields.Add(field->name.value);
									}
									GetExpressionType(manager, argument->value, CopyTypeInfo(prop->GetReturn()));
								}
								else
								{
									manager->errors.Add(WfErrors::MemberNotExists(field.Obj(), expectedType->GetTypeDescriptor(), field->name.value));
								}
							}
							else
							{
								manager->errors.Add(WfErrors::ConstructorMixStructAndList(argument->value.Obj()));
							}
						}
						results.Add(ResolveExpressionResult::ReadonlyType(expectedType));
					}
					else if (node->arguments.Count() == 0)
					{
						if (expectedType)
						{
							ITypeDescriptor* td = expectedType->GetTypeDescriptor();
							if (!td->CanConvertTo(description::GetTypeDescriptor<IValueEnumerable>()) && !td->CanConvertTo(description::GetTypeDescriptor<IValueReadonlyDictionary>()))
							{
								manager->errors.Add(WfErrors::ConstructorCannotImplicitlyConvertToType(node, expectedType.Obj()));
							}
							results.Add(ResolveExpressionResult::ReadonlyType(expectedType));
						}
						else
						{
							manager->errors.Add(WfErrors::ConstructorCannotResolveType(node));
						}
					}
					else
					{
						Ptr<ITypeInfo> expectedKeyType;
						Ptr<ITypeInfo> expectedValueType;
						if (expectedType)
						{
							if (expectedType->GetDecorator() == ITypeInfo::SharedPtr)
							{
								auto genericType = expectedType->GetElementType();
								if (genericType->GetDecorator() == ITypeInfo::Generic)
								{
									if (genericType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueDictionary>()
										|| genericType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueReadonlyDictionary>())
									{
										if (genericType->GetGenericArgumentCount() == 2)
										{
											expectedKeyType = Ptr(genericType->GetGenericArgument(0));
											expectedValueType = Ptr(genericType->GetGenericArgument(1));
										}
									}
									else if (genericType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueObservableList>()
										|| genericType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueArray>()
										|| genericType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueList>()
										|| genericType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueReadonlyList>()
										|| genericType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueEnumerable>())
									{
										if (genericType->GetGenericArgumentCount() == 1)
										{
											expectedKeyType = Ptr(genericType->GetGenericArgument(0));
										}
									}
								}
							}
						}

						bool map = node->arguments[0]->value;
						Ptr<ITypeInfo> keyType, valueType;
						for (auto argument : node->arguments)
						{
							{
								Ptr<ITypeInfo> newKeyType = GetExpressionType(manager, argument->key, expectedKeyType);
								if (!keyType)
								{
									keyType = newKeyType;
								}
								else if (auto mergedType = GetMergedType(keyType, newKeyType))
								{
									keyType = mergedType;
								}
								else
								{
									manager->errors.Add(WfErrors::CannotMergeTwoType(node, keyType.Obj(), newKeyType.Obj()));
								}
							}
							if (map)
							{
								Ptr<ITypeInfo> newValueType = GetExpressionType(manager, argument->value, expectedValueType);
								if (!valueType)
								{
									valueType = newValueType;
								}
								else if (auto mergedType = GetMergedType(valueType, newValueType))
								{
									valueType = mergedType;
								}
								else
								{
									manager->errors.Add(WfErrors::CannotMergeTwoType(node, valueType.Obj(), newValueType.Obj()));
								}
							}
						}

						if (map)
						{
							if (keyType && valueType)
							{
								auto classType = Ptr(new TypeDescriptorTypeInfo(description::GetTypeDescriptor<IValueDictionary>(), TypeInfoHint::Normal));
								auto genericType = Ptr(new GenericTypeInfo(classType));
								genericType->AddGenericArgument(keyType);
								genericType->AddGenericArgument(valueType);

								auto pointerType = Ptr(new SharedPtrTypeInfo(genericType));
								results.Add(ResolveExpressionResult::ReadonlyType(pointerType));
							}
						}
						else
						{
							if (keyType)
							{
								Ptr<ITypeInfo> classType;
								if (expectedType)
								{
									switch (expectedType->GetHint())
									{
									case TypeInfoHint::ObservableList:
										classType = Ptr(new TypeDescriptorTypeInfo(description::GetTypeDescriptor<IValueObservableList>(), expectedType->GetHint()));
										break;
									case TypeInfoHint::Array:
										classType = Ptr(new TypeDescriptorTypeInfo(description::GetTypeDescriptor<IValueArray>(), expectedType->GetHint()));
										break;
									case TypeInfoHint::Normal:
										if (expectedType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueObservableList>())
										{
											classType = Ptr(new TypeDescriptorTypeInfo(description::GetTypeDescriptor<IValueObservableList>(), expectedType->GetHint()));
										}
										else
										{
											classType = Ptr(new TypeDescriptorTypeInfo(description::GetTypeDescriptor<IValueList>(), expectedType->GetHint()));
										}
										break;
									default:
										classType = Ptr(new TypeDescriptorTypeInfo(description::GetTypeDescriptor<IValueList>(), expectedType->GetHint()));
									}
								}
								else
								{
									classType = Ptr(new TypeDescriptorTypeInfo(description::GetTypeDescriptor<IValueList>(), TypeInfoHint::Normal));
								}
								auto genericType = Ptr(new GenericTypeInfo(classType));
								genericType->AddGenericArgument(keyType);

								auto pointerType = Ptr(new SharedPtrTypeInfo(genericType));
								results.Add(ResolveExpressionResult::ReadonlyType(pointerType));
							}
						}
					}
				}

				void Visit(WfInferExpression* node)override
				{
					auto scope = manager->nodeScopes[node].Obj();
					Ptr<ITypeInfo> type = CreateTypeInfoFromType(scope, node->type);
					Ptr<ITypeInfo> expressionType = GetExpressionType(manager, node->expression, type);
					if (expressionType)
					{
						results.Add(ResolveExpressionResult::ReadonlyType(type));
					}
				}

				void Visit(WfTypeCastingExpression* node)override
				{
					auto scope = manager->nodeScopes[node].Obj();
					if (auto type = CreateTypeInfoFromType(scope, node->type))
					{
						if (auto expressionType = GetExpressionType(manager, node->expression, 0))
						{
							if (!CanConvertToType(expressionType.Obj(), type.Obj(), true))
							{
								manager->errors.Add(WfErrors::ExpressionCannotExplicitlyConvertToType(node->expression.Obj(), expressionType.Obj(), type.Obj()));
							}
						}
						if (node->strategy == WfTypeCastingStrategy::Weak)
						{
							switch (type->GetDecorator())
							{
							case ITypeInfo::RawPtr:
							case ITypeInfo::SharedPtr:
							case ITypeInfo::Nullable:
								break;
							default:
								manager->errors.Add(WfErrors::CannotWeakCastToType(node->expression.Obj(), type.Obj()));
							}
						}
						results.Add(ResolveExpressionResult::ReadonlyType(type));
					}
				}

				void Visit(WfTypeTestingExpression* node)override
				{
					Ptr<ITypeInfo> type = GetExpressionType(manager, node->expression, 0);
					if (type)
					{
						switch (node->test)
						{
						case WfTypeTesting::IsNull:
						case WfTypeTesting::IsNotNull:
							if (!IsNullAcceptableType(type.Obj()))
							{
								manager->errors.Add(WfErrors::NullCannotImplicitlyConvertToType(node->expression.Obj(), type.Obj()));
							}
							break;
						default:;
						}
					}
					results.Add(ResolveExpressionResult::ReadonlyType(TypeInfoRetriver<bool>::CreateTypeInfo()));
				}

				void Visit(WfTypeOfTypeExpression* node)override
				{
					results.Add(ResolveExpressionResult::ReadonlyType(TypeInfoRetriver<ITypeDescriptor*>::CreateTypeInfo()));
				}

				void Visit(WfTypeOfExpressionExpression* node)override
				{
					GetExpressionType(manager, node->expression, 0);
					results.Add(ResolveExpressionResult::ReadonlyType(TypeInfoRetriver<ITypeDescriptor*>::CreateTypeInfo()));
				}

				void Visit(WfAttachEventExpression* node)override
				{
					IEventInfo* eventInfo = GetExpressionEventInfo(manager, node->event);
					Ptr<ITypeInfo> functionType;
					if (eventInfo)
					{
						functionType = CopyTypeInfo(eventInfo->GetHandlerType());
					}
					GetExpressionType(manager, node->function, functionType);
					results.Add(ResolveExpressionResult::ReadonlyType(TypeInfoRetriver<Ptr<IEventHandler>>::CreateTypeInfo()));
				}

				void Visit(WfDetachEventExpression* node)override
				{
					GetExpressionEventInfo(manager, node->event);
					Ptr<ITypeInfo> pointerType = TypeInfoRetriver<Ptr<IEventHandler>>::CreateTypeInfo();
					GetExpressionType(manager, node->handler, pointerType);
					results.Add(ResolveExpressionResult::ReadonlyType(TypeInfoRetriver<bool>::CreateTypeInfo()));
				}

				void Visit(WfObserveExpression* node)override
				{
					Ptr<ITypeInfo> parentType = GetExpressionType(manager, node->parent, 0);
					Ptr<ITypeInfo> observeeType;
					if (parentType)
					{
						if (node->observeType == WfObserveType::SimpleObserve)
						{
							ITypeDescriptor* td = parentType->GetTypeDescriptor();
							IPropertyInfo* propertyInfo = 0;
							{
								auto ref = node->expression.Cast<WfReferenceExpression>();
								propertyInfo = td->GetPropertyByName(ref->name.value, true);
								if (propertyInfo)
								{
									observeeType = CopyTypeInfo(propertyInfo->GetReturn());
									manager->expressionResolvings.Add(node->expression, ResolveExpressionResult::Property(propertyInfo));
								}
								else
								{
									manager->errors.Add(WfErrors::MemberNotExists(ref.Obj(), td, ref->name.value));
								}
							}

							if (node->events.Count() == 0)
							{
								if (propertyInfo)
								{
									IEventInfo* eventInfo = propertyInfo->GetValueChangedEvent();
									if (!eventInfo)
									{
										eventInfo = td->GetEventByName(propertyInfo->GetName() + L"Changed", true);
									}
									if (!eventInfo)
									{
										manager->errors.Add(WfErrors::MemberNotExists(node->expression.Obj(), td, propertyInfo->GetName() + L"Changed"));
									}
								}
							}
							else
							{
								for (auto eventExpr : node->events)
								{
									auto ref = eventExpr.Cast<WfReferenceExpression>();
									IEventInfo* info = td->GetEventByName(ref->name.value, true);
									if (info)
									{
										manager->expressionResolvings.Add(eventExpr, ResolveExpressionResult::Event(info));
									}
									else if (!info)
									{
										manager->errors.Add(WfErrors::MemberNotExists(ref.Obj(), td, ref->name.value));
									}
								}
							}
						}
						else
						{
							auto scope = manager->nodeScopes[node].Obj();
							auto symbol = scope->symbols[node->name.value][0];
							symbol->typeInfo = parentType;
							symbol->type = GetTypeFromTypeInfo(parentType.Obj());

							observeeType = GetExpressionType(manager, node->expression, 0);
							for (auto eventExpr : node->events)
							{
								GetExpressionEventInfo(manager, eventExpr);
							}
						}
					}

					if (observeeType)
					{
						results.Add(ResolveExpressionResult::ReadonlyType(observeeType));
					}
				}

				void Visit(WfCallExpression* node)override
				{
					List<ResolveExpressionResult> functions;
					GetExpressionTypes(manager, node->function, nullptr, true, functions);

					vint selectedFunctionIndex = -1;
					Ptr<ITypeInfo> resultType = SelectFunction(manager, node, node->function, functions, node->arguments, selectedFunctionIndex);
					if (resultType)
					{
						manager->expressionResolvings.Add(node->function, functions[selectedFunctionIndex]);
						results.Add(ResolveExpressionResult::ReadonlyType(resultType));
					}
				}

				void Visit(WfFunctionExpression* node)override
				{
					ValidateDeclarationSemantic(manager, node->function);
					auto scope = manager->nodeScopes[node->function.Obj()].Obj();

					auto classType = Ptr(new TypeDescriptorTypeInfo(description::GetTypeDescriptor<IValueFunctionProxy>(), TypeInfoHint::Normal));
					auto genericType = Ptr(new GenericTypeInfo(classType));
					genericType->AddGenericArgument(CreateTypeInfoFromType(scope, node->function->returnType));
					for (auto argument : node->function->arguments)
					{
						genericType->AddGenericArgument(scope->symbols[argument->name.value][0]->typeInfo);
					}

					auto pointerType = Ptr(new SharedPtrTypeInfo(genericType));
					results.Add(ResolveExpressionResult::ReadonlyType(pointerType));
				}

				Ptr<ITypeInfo> GetFunctionDeclarationType(WfLexicalScope* scope, Ptr<WfFunctionDeclaration> decl)
				{
					Ptr<WfLexicalSymbol> symbol = From(manager->nodeScopes[decl.Obj()]->parentScope->symbols[decl->name.value])
						.Where([decl](Ptr<WfLexicalSymbol> symbol)
						{
							return symbol->creatorNode == decl;
						})
						.First();
					return symbol->typeInfo;
				}

				class NewInterfaceExpressionVisitor
					: public empty_visitor::DeclarationVisitor
				{
				public:
					WfLexicalScopeManager*							manager;
					List<Ptr<WfFunctionDeclaration>>				overrideFunctions;
					List<Ptr<WfLexicalSymbol>>						variableSymbols;
					WfFunctionDeclaration*							lastFunction = nullptr;

					NewInterfaceExpressionVisitor(WfLexicalScopeManager* _manager)
						:manager(_manager)
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
						lastFunction = node;
						if (node->functionKind == WfFunctionKind::Override)
						{
							overrideFunctions.Add(Ptr(node));
						}
					}

					void Visit(WfVariableDeclaration* node)override
					{
						variableSymbols.Add(
							From(manager->nodeScopes[node]->symbols[node->name.value])
								.Where([=](Ptr<WfLexicalSymbol> symbol)
								{
									return symbol->creatorNode == node;
								})
								.First()
							);
					}

					void Execute(WfNewInterfaceExpression* node)
					{
						for (auto memberDecl : node->declarations)
						{
							memberDecl->Accept(this);
							ValidateDeclarationSemantic(manager, memberDecl);
						}
					}
				};

				void Visit(WfNewClassExpression* node)override
				{
					auto scope = manager->nodeScopes[node].Obj();
					Ptr<ITypeInfo> type = CreateTypeInfoFromType(scope, node->type);
					if (type)
					{
						ITypeDescriptor* td = type->GetTypeDescriptor();
						IMethodGroupInfo* ctors = td->GetConstructorGroup();
						Ptr<ITypeInfo> selectedType;
						IMethodInfo* selectedConstructor = nullptr;

						if (!ctors || ctors->GetMethodCount() == 0)
						{
							manager->errors.Add(WfErrors::ClassContainsNoConstructor(node, type.Obj()));
						}
						else
						{
							if ((td->GetTypeDescriptorFlags() & TypeDescriptorFlags::ClassType) != TypeDescriptorFlags::Undefined)
							{
								List<ResolveExpressionResult> functions;
								for (vint i = 0; i < ctors->GetMethodCount(); i++)
								{
									IMethodInfo* info = ctors->GetMethod(i);
									functions.Add(ResolveExpressionResult::Method(info));
								}

								vint selectedFunctionIndex = -1;
								selectedType = SelectFunction(manager, node, nullptr, functions, node->arguments, selectedFunctionIndex);
								if (selectedType)
								{
									selectedConstructor = functions[selectedFunctionIndex].methodInfo;
								}
							}
							else if ((td->GetTypeDescriptorFlags() & TypeDescriptorFlags::InterfaceType) != TypeDescriptorFlags::Undefined)
							{
								manager->errors.Add(WfErrors::ConstructorMixClassAndInterface(node));
							}
							else
							{
								manager->errors.Add(WfErrors::ClassContainsNoConstructor(node, type.Obj()));
							}
						}

						if (selectedType)
						{
							auto result = ResolveExpressionResult::Constructor(selectedConstructor);
							if (!IsSameType(selectedType.Obj(), type.Obj()))
							{
								manager->errors.Add(WfErrors::ConstructorReturnTypeMismatched(node, result, selectedType.Obj(), type.Obj()));
							}
							results.Add(result);
						}
					}
				}

				void Visit(WfNewInterfaceExpression* node)override
				{
					auto scope = manager->nodeScopes[node].Obj();
					Ptr<ITypeInfo> type = CreateTypeInfoFromType(scope, node->type);
					if (type)
					{
						ITypeDescriptor* td = type->GetTypeDescriptor();
						IMethodGroupInfo* ctors = td->GetConstructorGroup();
						Ptr<ITypeInfo> selectedType;
						IMethodInfo* selectedConstructor = nullptr;

						scope->typeOfThisExpr = td;

						if (!ctors || ctors->GetMethodCount() == 0)
						{
							manager->errors.Add(WfErrors::ClassContainsNoConstructor(node, type.Obj()));
						}
						else
						{
							if ((td->GetTypeDescriptorFlags() & TypeDescriptorFlags::ClassType) != TypeDescriptorFlags::Undefined)
							{
								manager->errors.Add(WfErrors::ConstructorMixClassAndInterface(node));
							}
							else if ((td->GetTypeDescriptorFlags() & TypeDescriptorFlags::InterfaceType) != TypeDescriptorFlags::Undefined)
							{
								if (auto info = FindInterfaceConstructor(td))
								{
									selectedType = CopyTypeInfo(info->GetReturn());
									selectedConstructor = info;
								}
								else
								{
									manager->errors.Add(WfErrors::InterfaceContainsNoConstructor(node, type.Obj()));
								}

								Group<WString, IMethodInfo*> interfaceMethods;
								Group<WString, Ptr<WfFunctionDeclaration>> implementMethods;
								
								{
									NewInterfaceExpressionVisitor declVisitor(manager);
									declVisitor.Execute(node);

									if (declVisitor.lastFunction)
									{
										for (auto func : declVisitor.overrideFunctions)
										{
											implementMethods.Add(func->name.value, func);
										}

										auto capture = manager->lambdaCaptures[declVisitor.lastFunction];
										List<Ptr<WfLexicalSymbol>> readonlySymbols;
										CopyFrom(readonlySymbols, From(capture->symbols).Except(declVisitor.variableSymbols));
										CopyFrom(capture->symbols, declVisitor.variableSymbols);
										CopyFrom(capture->symbols, readonlySymbols, true);
									}
								}
								{
									SortedList<ITypeDescriptor*> searchedTypes;
									List<ITypeDescriptor*> types;
									types.Add(td);
									vint begin = 0;

									while (begin < types.Count())
									{
										ITypeDescriptor* currentType = types[begin++];
										if (searchedTypes.Contains(currentType))
										{
											continue;
										}
										searchedTypes.Add(currentType);

										vint count = currentType->GetBaseTypeDescriptorCount();
										for (vint i = 0; i < count; i++)
										{
											types.Add(currentType->GetBaseTypeDescriptor(i));
										}

										count = currentType->GetMethodGroupCount();
										for (vint i = 0; i < count; i++)
										{
											IMethodGroupInfo* group = currentType->GetMethodGroup(i);
											vint methodCount = group->GetMethodCount();
											for (vint j = 0; j < methodCount; j++)
											{
												auto method = group->GetMethod(j);
												if (!method->IsStatic())
												{
													interfaceMethods.Add(group->GetName(), method);
												}
											}
										}
									}
								}

								auto discardFirst = [=](const WString& key, const List<IMethodInfo*>& methods)
									{
										for (auto method : methods)
										{
											manager->errors.Add(WfErrors::InterfaceMethodNotImplemented(node, method));
										}
									};
								auto discardSecond = [=](const WString& key, const List<Ptr<WfFunctionDeclaration>>& methods)
									{
										for (auto decl : methods)
										{
											Ptr<ITypeInfo> declType = GetFunctionDeclarationType(scope, decl);
											manager->errors.Add(WfErrors::InterfaceMethodNotFound(decl.Obj(), type.Obj(), declType.Obj()));
										}
									};

								GroupInnerJoin(
									interfaceMethods,
									implementMethods,
									discardFirst,
									discardSecond,
									[=](const WString& key, const List<IMethodInfo*>& interfaces, const List<Ptr<WfFunctionDeclaration>>& implements)
									{
										Group<WString, IMethodInfo*> typedInterfaceMethods;
										Group<WString, Ptr<WfFunctionDeclaration>> typedImplementMethods;

										for (auto method : interfaces)
										{
											Ptr<ITypeInfo> methodType = CreateTypeInfoFromMethodInfo(method);
											typedInterfaceMethods.Add(methodType->GetTypeFriendlyName(), method);
										}

										for (auto decl : implements)
										{
											Ptr<ITypeInfo> methodType = GetFunctionDeclarationType(scope, decl);
											typedImplementMethods.Add(methodType->GetTypeFriendlyName(), decl);
										}

										GroupInnerJoin(
											typedInterfaceMethods,
											typedImplementMethods,
											discardFirst,
											discardSecond,
											[=](const WString& key, const List<IMethodInfo*>& interfaces, const List<Ptr<WfFunctionDeclaration>>& implements)
											{
												if (interfaces.Count() > 1)
												{
													List<ResolveExpressionResult> functions;
													for (auto method : interfaces)
													{
														functions.Add(ResolveExpressionResult::Constructor(method));
														manager->errors.Add(WfErrors::CannotPickOverloadedInterfaceMethods(node, functions));
													}
												}
												if (implements.Count() > 1)
												{
													auto decl = implements[0];
													Ptr<ITypeInfo> methodType = GetFunctionDeclarationType(scope, decl);
													manager->errors.Add(WfErrors::CannotPickOverloadedImplementMethods(decl.Obj(), methodType.Obj()));
												}

												if (interfaces.Count() == 1 && implements.Count() == 1)
												{
													manager->interfaceMethodImpls.Add(implements[0].Obj(), interfaces[0]);
												}
											});
									});
							}
							else
							{
								manager->errors.Add(WfErrors::ClassContainsNoConstructor(node, type.Obj()));
							}
						}

						if (selectedType)
						{
							auto result = ResolveExpressionResult::Constructor(selectedConstructor);
							if (!IsSameType(selectedType.Obj(), type.Obj()))
							{
								manager->errors.Add(WfErrors::ConstructorReturnTypeMismatched(node, result, selectedType.Obj(), type.Obj()));
							}
							results.Add(result);
						}
					}
				}

				void Visit(WfVirtualCfeExpression* node)override
				{
					if (GetExpressionType(manager, node->expandedExpression, expectedType))
					{
						results.Add(manager->expressionResolvings[node->expandedExpression.Obj()]);
					}
				}

				void Visit(WfVirtualCseExpression* node)override
				{
					bool expanded = node->expandedExpression;
					vint errorCount = manager->errors.Count();
					node->Accept((WfVirtualCseExpression::IVisitor*)this);

					if (!expanded && manager->errors.Count() == errorCount)
					{
						ExpandVirtualExpressionVisitor visitor(manager, expectedType);
						node->Accept(&visitor);
						if (!node->expandedExpression)
						{
							return;
						}

						SetCodeRange(node->expandedExpression, node->codeRange);

						auto parentScope = manager->nodeScopes[node];
						if (parentScope->ownerNode == node)
						{
							parentScope = parentScope->parentScope;
						}

						ContextFreeExpressionDesugar(manager, node->expandedExpression);
						BuildScopeForExpression(manager, parentScope, node->expandedExpression);

						manager->checkedScopes_DuplicatedSymbol.Remove(parentScope.Obj());
						manager->checkedScopes_SymbolType.Remove(parentScope.Obj());
						if (!CheckScopes_DuplicatedSymbol(manager) || !CheckScopes_SymbolType(manager))
						{
							return;
						}
					}

					if (node->expandedExpression)
					{
						if (results.Count() == 1)
						{
							GetExpressionType(manager, node->expandedExpression, results[0].type);
						}
						else
						{
							GetExpressionType(manager, node->expandedExpression, nullptr);
						}
					}
				}

				void Visit(WfBindExpression* node)override
				{
					GetExpressionType(manager, node->expression, 0);
					results.Add(ResolveExpressionResult::ReadonlyType(TypeInfoRetriver<Ptr<IValueSubscription>>::CreateTypeInfo()));
				}

				void Visit(WfNewCoroutineExpression* node)override
				{
					Ptr<ITypeInfo> typeInfo = TypeInfoRetriver<Ptr<ICoroutine>>::CreateTypeInfo();
					results.Add(ResolveExpressionResult::ReadonlyType(typeInfo));
					ValidateStatementSemantic(manager, node->statement);
				}

				void Visit(WfMixinCastExpression* node)override
				{
					auto scope = manager->nodeScopes[node].Obj();
					auto type = CreateTypeInfoFromType(scope, node->type);
					auto expressionType = GetExpressionType(manager, node->expression, 0);
					if (type && expressionType)
					{
						results.Add(ResolveExpressionResult::ReadonlyType(type));
						if (expressionType)
						{
							auto fromTd = expressionType->GetTypeDescriptor();
							auto toTd = type->GetTypeDescriptor();
							if ((fromTd->GetTypeDescriptorFlags() & TypeDescriptorFlags::InterfaceType) != TypeDescriptorFlags::Undefined)
							{
								if ((toTd->GetTypeDescriptorFlags() & TypeDescriptorFlags::InterfaceType) != TypeDescriptorFlags::Undefined)
								{
									if (fromTd != toTd && toTd->CanConvertTo(fromTd))
									{
										goto PASS_VALIDATION;
									}
								}
							}
							manager->errors.Add(WfErrors::WrongMixinTargetType(node, type.Obj(), expressionType.Obj()));
						PASS_VALIDATION:;
						}
					}
				}

				void Visit(WfExpectedTypeCastExpression* node)override
				{
					GetExpressionType(manager, node->expression, nullptr);
					if (expectedType)
					{
						results.Add(ResolveExpressionResult::ReadonlyType(expectedType));
					}
					else
					{
						manager->errors.Add(WfErrors::ExpectedTypeCastCannotResolveType(node));
					}
				}

				void Visit(WfCoOperatorExpression* node)override
				{
					auto scope = manager->nodeScopes[node].Obj();
					auto functionScope = scope->FindFunctionScope();
					if (auto funcDecl = functionScope->ownerNode.Cast<WfFunctionDeclaration>())
					{
						if (funcDecl->statement.Cast<WfCoProviderStatement>())
						{
							auto providerScope = manager->nodeScopes[funcDecl->statement.Obj()];
							auto providerSymbol = providerScope->symbols[L"$PROVIDER"][0];
							auto implSymbol = providerScope->symbols[L"$IMPL"][0];
							if (providerSymbol->typeInfo && implSymbol->typeInfo)
							{
								List<ResolveExpressionResult> methods;
								if (auto group = providerSymbol->typeInfo->GetTypeDescriptor()->GetMethodGroupByName(L"QueryContext", true))
								{
									vint count = group->GetMethodCount();
									for (vint i = 0; i < count; i++)
									{
										auto method = group->GetMethod(i);
										if (method->IsStatic())
										{
											if (method->GetParameterCount() == 1 && IsSameType(implSymbol->typeInfo.Obj(), method->GetParameter(0)->GetType()))
											{
												methods.Add(ResolveExpressionResult::Method(method));
											}
										}
									}
								}

								if (methods.Count() > 1)
								{
									manager->errors.Add(WfErrors::CannotPickOverloadedFunctions(node, methods));
								}
								else if (methods.Count() == 1)
								{
									auto contextType = methods[0].methodInfo->GetReturn();
									SortedList<ITypeDescriptor*> searchedTypes;
									manager->ResolveMember(contextType->GetTypeDescriptor(), node->name.value, false, searchedTypes, results);

									if (results.Count() == 0)
									{
										manager->errors.Add(WfErrors::MemberNotExists(node, contextType->GetTypeDescriptor(), node->name.value));
									}
								}
								else
								{
									manager->errors.Add(WfErrors::CoOperatorNotExists(node, providerSymbol->typeInfo.Obj()));
								}
							}
						}
					}
				}

				static void Execute(WfExpression* expression, WfLexicalScopeManager* manager, Ptr<ITypeInfo> expectedType, List<ResolveExpressionResult>& results)
				{
					ValidateSemanticExpressionVisitor visitor(manager, expectedType, results);
					expression->Accept(&visitor);
				}
			};

/***********************************************************************
IsConstantExpression
***********************************************************************/

			class ValidateConstantExpressionVisitor
				: public empty_visitor::ExpressionVisitor
				, public empty_visitor::VirtualCseExpressionVisitor
			{
			public:
				WfLexicalScopeManager*				manager;
				bool								isConstant = false;

				ValidateConstantExpressionVisitor(WfLexicalScopeManager* _manager)
					:manager(_manager)
				{
				}

				bool Call(Ptr<WfExpression> node)
				{
					bool cache = isConstant;
					isConstant = false;
					node->Accept(this);
					bool result = isConstant;
					isConstant = cache;
					return result;
				}

				void VisitReferenceExpression(WfExpression* node, const WString& name)
				{
					auto result = manager->expressionResolvings[node];
					if (auto enumType = result.type->GetTypeDescriptor()->GetEnumType())
					{
						vint index = enumType->IndexOfItem(name);
						if (index != -1)
						{
							isConstant = true;
						}
					}
				}

				void Dispatch(WfVirtualCfeExpression* node)override
				{
					node->expandedExpression->Accept(this);
				}

				void Dispatch(WfVirtualCseExpression* node)override
				{
					node->Accept((WfVirtualCseExpression::IVisitor*)this);
				}

				void Visit(WfReferenceExpression* node)override
				{
					VisitReferenceExpression(node, node->name.value);
				}

				void Visit(WfChildExpression* node)override
				{
					VisitReferenceExpression(node, node->name.value);
				}

				void Visit(WfLiteralExpression* node)override
				{
					isConstant = true;
				}

				void Visit(WfFloatingExpression* node)override
				{
					isConstant = true;
				}

				void Visit(WfIntegerExpression* node)override
				{
					isConstant = true;
				}

				void Visit(WfStringExpression* node)override
				{
					isConstant = true;
				}

				/*

				void Visit(WfUnaryExpression* node)override
				{
					isConstant = Call(node->operand);
				}

				void Visit(WfBinaryExpression* node)override
				{
					if (node->op == WfBinaryOperator::FlagAnd || node->op == WfBinaryOperator::FlagOr)
					{
						isConstant = Call(node->first) && Call(node->second);
					}
				}

				void Visit(WfRangeExpression* node)override
				{
					isConstant = Call(node->begin) && Call(node->end);
				}

				void Visit(WfConstructorExpression* node)override
				{
					auto result = manager->expressionResolvings[node];
					bool isStruct = (result.type->GetTypeDescriptor()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Struct);

					for (auto argument : node->arguments)
					{
						if (argument->key && !isStruct)
						{
							if (!Call(argument->key)) return;
						}
						if (argument->value)
						{
							if (!Call(argument->value)) return;
						}
					}
					isConstant = true;
				}

				*/

				void Visit(WfTypeOfTypeExpression* node)override
				{
					isConstant = true;
				}

				void Visit(WfTypeOfExpressionExpression* node)override
				{
					isConstant = true;
				}

				static void Execute(WfExpression* expression, WfLexicalScopeManager* manager, Ptr<ITypeInfo> expectedType)
				{
					if (GetExpressionType(manager, expression, expectedType))
					{
						ValidateConstantExpressionVisitor visitor(manager);
						expression->Accept(&visitor);
						if (!visitor.isConstant)
						{
							manager->errors.Add(WfErrors::ExpressionIsNotConstant(expression));
						}
					}
				}
			};

/***********************************************************************
ValidateSemantic
***********************************************************************/

			void ValidateExpressionSemantic(WfLexicalScopeManager* manager, WfExpression* expression, Ptr<reflection::description::ITypeInfo> expectedType, collections::List<ResolveExpressionResult>& results)
			{
				ValidateSemanticExpressionVisitor::Execute(expression, manager, expectedType, results);
				for (vint i = results.Count() - 1; i >= 0; i--)
				{
					auto result = results[i];
					if (result.scopeName && result.scopeName->declarations.Count() > 0)
					{
						List<ResolveExpressionResult> replaces;
						for (auto decl : result.scopeName->declarations)
						{
							vint index = manager->nodeScopes.Keys().IndexOf(decl.Obj());
							if (index == -1) continue;
							auto scope = manager->nodeScopes.Values()[index];
							if (scope->ownerNode == decl)
							{
								scope = scope->parentScope;
							}

							index = scope->symbols.Keys().IndexOf(decl->name.value);
							if (index == -1) continue;
							for (auto symbol : scope->symbols.GetByIndex(index))
							{
								if (symbol->creatorNode == decl && symbol->typeInfo)
								{
									replaces.Add(ResolveExpressionResult::Symbol(symbol));
								}
							}
						}

						if (replaces.Count() > 0)
						{
							results.RemoveAt(i);
							for (auto [replaceResult, index] : indexed(replaces))
							{
								results.Insert(i + index, replaceResult);
							}
						}
					}
				}
			}

			void ValidateExpressionSemantic(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType, collections::List<ResolveExpressionResult>& results)
			{
				ValidateExpressionSemantic(manager, expression.Obj(), expectedType, results);
			}

			void ValidateConstantExpression(WfLexicalScopeManager* manager, WfExpression* expression, Ptr<reflection::description::ITypeInfo> expectedType)
			{
				ValidateConstantExpressionVisitor::Execute(expression, manager, expectedType);
			}

			void ValidateConstantExpression(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType)
			{
				ValidateConstantExpressionVisitor::Execute(expression.Obj(), manager, expectedType);
			}
		}
	}
}
