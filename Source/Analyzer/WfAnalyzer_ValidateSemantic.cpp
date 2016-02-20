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
Helper Functions
***********************************************************************/

			IMethodInfo* FindInterfaceConstructor(ITypeDescriptor* type)
			{
				if (auto ctors = type->GetConstructorGroup())
				{
					auto proxyTd = description::GetTypeDescriptor<IValueInterfaceProxy>();
					for (vint i = 0; i < ctors->GetMethodCount(); i++)
					{
						IMethodInfo* info = ctors->GetMethod(i);
						if (info->GetParameterCount() == 1)
						{
							ITypeInfo* parameterType = info->GetParameter(0)->GetType();
							if (parameterType->GetDecorator() == ITypeInfo::SharedPtr)
							{
								parameterType = parameterType->GetElementType();
								if (parameterType->GetDecorator() == ITypeInfo::TypeDescriptor && parameterType->GetTypeDescriptor() == proxyTd)
								{
									return info;
								}
							}
						}
					}
				}
				return nullptr;
			}

/***********************************************************************
ValidateSemantic(ClassMember)
***********************************************************************/

			class ValidateSemanticClassMemberVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfLexicalScopeManager*				manager;
				Ptr<WfCustomType>					td;
				Ptr<WfClassDeclaration>				classDecl;
				Ptr<WfClassMember>					member;

				ValidateSemanticClassMemberVisitor(Ptr<WfCustomType> _td, Ptr<WfClassDeclaration> _classDecl, Ptr<WfClassMember> _member, WfLexicalScopeManager* _manager)
					:td(_td)
					, classDecl(_classDecl)
					, member(_member)
					, manager(_manager)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					if (node->statement)
					{
						ValidateDeclarationSemantic(manager, node);
					}
				}

				void Visit(WfVariableDeclaration* node)override
				{
					throw 0;
				}

				void Visit(WfEventDeclaration* node)override
				{
				}

				void Visit(WfPropertyDeclaration* node)override
				{
					auto scope = manager->declarationScopes[node];
					if (auto typeInfo = CreateTypeInfoFromType(scope.Obj(), node->type))
					{
						if (node->getter.value != L"")
						{
							auto getter = td->GetMethodGroupByName(node->getter.value, false)->GetMethod(0);
							if (!IsSameType(typeInfo.Obj(), getter->GetReturn()) || getter->GetParameterCount() != 0)
							{
								manager->errors.Add(WfErrors::PropertyGetterTypeMismatched(node, classDecl.Obj()));
							}
						}

						if (node->setter.value != L"")
						{
							auto setter = td->GetMethodGroupByName(node->setter.value, false)->GetMethod(0);
							if (setter->GetReturn()->GetTypeDescriptor() != description::GetTypeDescriptor<void>() || setter->GetParameterCount() != 1 || !IsSameType(typeInfo.Obj(), setter->GetParameter(0)->GetType()))
							{
								manager->errors.Add(WfErrors::PropertySetterTypeMismatched(node, classDecl.Obj()));
							}
						}
					}
				}

				void Visit(WfClassDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);
				}

				static void Execute(Ptr<WfCustomType> td, Ptr<WfClassDeclaration> classDecl, Ptr<WfClassMember> member, WfLexicalScopeManager* manager)
				{
					ValidateSemanticClassMemberVisitor visitor(td, classDecl, member, manager);
					member->declaration->Accept(&visitor);
				}
			};

/***********************************************************************
ValidateSemantic(Declaration)
***********************************************************************/

			class ValidateSemanticDeclarationVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfLexicalScopeManager*				manager;

				ValidateSemanticDeclarationVisitor(WfLexicalScopeManager* _manager)
					:manager(_manager)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					FOREACH(Ptr<WfDeclaration>, declaration, node->declarations)
					{
						declaration->Accept(this);
					}
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					ValidateStatementSemantic(manager, node->statement);
				}

				void Visit(WfVariableDeclaration* node)override
				{
					auto scope = manager->declarationScopes[node];
					auto symbol = scope->symbols[node->name.value][0];
					symbol->typeInfo = GetExpressionType(manager, node->expression, symbol->typeInfo);
					if (symbol->typeInfo && !symbol->type)
					{
						symbol->type = GetTypeFromTypeInfo(symbol->typeInfo.Obj());
					}
				}

				void Visit(WfEventDeclaration* node)override
				{
				}

				void Visit(WfPropertyDeclaration* node)override
				{
				}

				void Visit(WfClassDeclaration* node)override
				{
					auto scope = manager->declarationScopes[node];
					auto td = manager->declarationTypes[node];

					FOREACH(Ptr<WfType>, baseType, node->baseTypes)
					{
						if (auto scopeName = GetScopeNameFromReferenceType(scope.Obj(), baseType))
						{
							if (auto td = scopeName->typeDescriptor)
							{
								bool isClass = (td->GetTypeDescriptorFlags() & TypeDescriptorFlags::InterfaceType) != TypeDescriptorFlags::Undefined;
								bool isInterface = (td->GetTypeDescriptorFlags() & TypeDescriptorFlags::InterfaceType) != TypeDescriptorFlags::Undefined;

								switch (node->kind)
								{
								case WfClassKind::Class:
									{
										if (!isClass && !isInterface)
										{
											manager->errors.Add(WfErrors::WrongBaseTypeOfClass(node, td));
										}
									}
									break;
								case WfClassKind::Interface:
									{
										if (!isInterface)
										{
											manager->errors.Add(WfErrors::WrongBaseTypeOfInterface(node, td));
										}
									}
									break;
								}

								if (isInterface)
								{
									auto ctor = FindInterfaceConstructor(td);
									if (ctor == nullptr)
									{
										manager->errors.Add(WfErrors::WrongInterfaceBaseType(node, td));
									}
								}
							}
						}
					}

					FOREACH(Ptr<WfClassMember>, member, node->members)
					{
						ValidateClassMemberSemantic(manager, td, node, member);
					}
				}

				static void Execute(Ptr<WfDeclaration> declaration, WfLexicalScopeManager* manager)
				{
					ValidateSemanticDeclarationVisitor visitor(manager);
					declaration->Accept(&visitor);
				}
			};

/***********************************************************************
ValidateSemantic(Statement)
***********************************************************************/

			class ValidateSemanticStatementVisitor : public Object, public WfStatement::IVisitor
			{
			public:
				WfLexicalScopeManager*				manager;

				ValidateSemanticStatementVisitor(WfLexicalScopeManager* _manager)
					:manager(_manager)
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
					auto scope = manager->statementScopes[node].Obj();
					auto decl = scope->FindDeclaration().Cast<WfFunctionDeclaration>();
					auto returnType = CreateTypeInfoFromType(scope, decl->returnType);
					if (node->expression)
					{
						GetExpressionType(manager, node->expression, returnType);
					}
					else if (returnType->GetDecorator() != ITypeInfo::TypeDescriptor || returnType->GetTypeDescriptor() != description::GetTypeDescriptor<void>())
					{
						manager->errors.Add(WfErrors::ReturnMissExpression(node, returnType.Obj()));
					}
				}

				void Visit(WfDeleteStatement* node)override
				{
					Ptr<ITypeInfo> type = GetExpressionType(manager, node->expression, 0);
					if (type)
					{
						if (type->GetDecorator() != ITypeInfo::RawPtr)
						{
							manager->errors.Add(WfErrors::DeleteNonRawPointer(node, type.Obj()));
						}
					}
				}

				void Visit(WfRaiseExceptionStatement* node)override
				{
					if (node->expression)
					{
						Ptr<ITypeInfo> stringType = TypeInfoRetriver<WString>::CreateTypeInfo();
						GetExpressionType(manager, node->expression, stringType);
					}
				}

				void Visit(WfIfStatement* node)override
				{
					if (node->type)
					{
						auto scope = manager->statementScopes[node].Obj();
						auto symbol = scope->symbols[node->name.value][0];
						if (!IsNullAcceptableType(symbol->typeInfo.Obj()))
						{
							manager->errors.Add(WfErrors::NullCannotImplicitlyConvertToType(node->expression.Obj(), symbol->typeInfo.Obj()));
						}
						GetExpressionType(manager, node->expression, 0);
					}
					else
					{
						Ptr<ITypeInfo> boolType = TypeInfoRetriver<bool>::CreateTypeInfo();
						GetExpressionType(manager, node->expression, boolType);
					}
					ValidateStatementSemantic(manager, node->trueBranch);
					if (node->falseBranch)
					{
						ValidateStatementSemantic(manager, node->falseBranch);
					}
				}

				void Visit(WfSwitchStatement* node)override
				{
					Ptr<ITypeInfo> type = GetExpressionType(manager, node->expression, 0);
					FOREACH(Ptr<WfSwitchCase>, switchCase, node->caseBranches)
					{
						Ptr<ITypeInfo> caseType;
						if (IsExpressionDependOnExpectedType(manager, switchCase->expression))
						{
							caseType = GetExpressionType(manager, switchCase->expression, type);
						}
						else
						{
							caseType = GetExpressionType(manager, switchCase->expression, 0);
						}

						if (type && caseType)
						{
							if (!GetMergedType(type, caseType))
							{
								manager->errors.Add(WfErrors::CannotMergeTwoType(switchCase->expression.Obj(), type.Obj(), caseType.Obj()));
							}
						}
						ValidateStatementSemantic(manager, switchCase->statement);
					}
					if (node->defaultBranch)
					{
						ValidateStatementSemantic(manager, node->defaultBranch);
					}
				}

				void Visit(WfWhileStatement* node)override
				{
					Ptr<ITypeInfo> boolType = TypeInfoRetriver<bool>::CreateTypeInfo();
					GetExpressionType(manager, node->condition, boolType);
					ValidateStatementSemantic(manager, node->statement);
				}

				void Visit(WfForEachStatement* node)override
				{
					Ptr<ITypeInfo> elementType = GetEnumerableExpressionItemType(manager, node->collection, 0);
					if (elementType)
					{
						auto scope = manager->statementScopes[node].Obj();
						auto symbol = scope->symbols[node->name.value][0];
						symbol->typeInfo = elementType;
						symbol->type = GetTypeFromTypeInfo(elementType.Obj());
					}
					ValidateStatementSemantic(manager, node->statement);
				}

				void Visit(WfTryStatement* node)override
				{
					ValidateStatementSemantic(manager, node->protectedStatement);
					if (node->catchStatement)
					{
						ValidateStatementSemantic(manager, node->catchStatement);
					}
					if (node->finallyStatement)
					{
						ValidateStatementSemantic(manager, node->finallyStatement);
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
					GetExpressionType(manager, node->expression, 0);
				}

				void Visit(WfVariableStatement* node)override
				{
					ValidateDeclarationSemantic(manager, node->variable);
				}

				static void Execute(Ptr<WfStatement> statement, WfLexicalScopeManager* manager)
				{
					ValidateSemanticStatementVisitor visitor(manager);
					statement->Accept(&visitor);
				}
			};

/***********************************************************************
ValidateSemantic(Expression)
***********************************************************************/

			class ValidateSemanticExpressionVisitor : public Object, public WfExpression::IVisitor
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
					auto scope = manager->expressionScopes[node].Obj();
					Ptr<WfFunctionDeclaration> funcDecl;
					while (scope)
					{
						if (scope->ownerExpression.Cast<WfOrderedLambdaExpression>())
						{
							break;
						}
						else if (scope->ownerExpression.Cast<WfFunctionExpression>())
						{
							break;
						}
						else if (auto newType = scope->ownerExpression.Cast<WfNewTypeExpression>())
						{
							if (funcDecl)
							{
								if (auto td = manager->expressionScopes[newType.Obj()]->typeDescriptor)
								{
									auto elementType = MakePtr<TypeInfoImpl>(ITypeInfo::TypeDescriptor);
									elementType->SetTypeDescriptor(td);

									auto pointerType = MakePtr<TypeInfoImpl>(ITypeInfo::RawPtr);
									pointerType->SetElementType(elementType);

									results.Add(ResolveExpressionResult::ReadonlyType(pointerType));
									return;
								}
							}
							else
							{
								break;
							}
						}
						else if (auto classDecl = scope->ownerDeclaration.Cast<WfClassDeclaration>())
						{
							if (funcDecl)
							{
								auto elementType = MakePtr<TypeInfoImpl>(ITypeInfo::TypeDescriptor);
								elementType->SetTypeDescriptor(manager->declarationTypes[classDecl.Obj()].Obj());

								auto pointerType = MakePtr<TypeInfoImpl>(ITypeInfo::RawPtr);
								pointerType->SetElementType(elementType);

								results.Add(ResolveExpressionResult::ReadonlyType(pointerType));
								return;
							}
							else
							{
								break;
							}
						}
						else if (funcDecl = scope->ownerDeclaration.Cast<WfFunctionDeclaration>())
						{
							if (!scope->ownerClassMember || scope->ownerClassMember->kind == WfClassMemberKind::Static)
							{
								break;
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
					auto scope = manager->expressionScopes[node].Obj();
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
							else if (!result.symbol->creatorDeclaration || result.symbol->creatorDeclaration.Cast<WfVariableDeclaration>())
							{
								bool readonlyCaptured = false;
								if (!result.symbol->ownerScope->ownerDeclaration.Cast<WfNamespaceDeclaration>())
								{
									auto currentScope = scope;
									bool testedReadonlyCaptured = false;
									Ptr<WfClassMember> lastFuncDeclClassMember;

									while (currentScope)
									{
										if (auto funcDecl = currentScope->ownerDeclaration.Cast<WfFunctionDeclaration>())
										{
											bool inClassMember = currentScope->ownerClassMember && currentScope->parentScope && currentScope->parentScope->ownerDeclaration;
											if (result.symbol->ownerScope != currentScope && !inClassMember)
											{
												readonlyCaptured = true;
												lastFuncDeclClassMember = currentScope->ownerClassMember;

												if (auto parentScope = currentScope->parentScope.Obj())
												{
													if (parentScope->ownerExpression.Cast<WfNewTypeExpression>())
													{
														if (!testedReadonlyCaptured)
														{
															testedReadonlyCaptured = true;
															if (result.symbol->ownerScope == parentScope)
															{
																readonlyCaptured = false;
															}
														}
													}
												}

												vint index = manager->functionLambdaCaptures.Keys().IndexOf(funcDecl.Obj());
												if (index == -1 || !manager->functionLambdaCaptures.GetByIndex(index).Contains(result.symbol.Obj()))
												{
													manager->functionLambdaCaptures.Add(funcDecl.Obj(), result.symbol);
												}
											}
										}

										if (auto ordered = currentScope->ownerExpression.Cast<WfOrderedLambdaExpression>())
										{
											if (result.symbol->ownerScope != currentScope)
											{
												readonlyCaptured = true;
												vint index = manager->orderedLambdaCaptures.Keys().IndexOf(ordered.Obj());
												if (index == -1 || !manager->orderedLambdaCaptures.GetByIndex(index).Contains(result.symbol.Obj()))
												{
													manager->orderedLambdaCaptures.Add(ordered.Obj(), result.symbol);
												}
											}
										}

										if (auto newType = currentScope->ownerExpression.Cast<WfNewTypeExpression>())
										{
											if (!lastFuncDeclClassMember || !newType->members.Contains(lastFuncDeclClassMember.Obj()))
											{
												if (result.symbol->ownerScope == currentScope)
												{
													manager->errors.Add(WfErrors::FieldCannotInitializeUsingEachOther(node, result));
												}
											}
										}

										if (result.symbol->ownerScope == currentScope)
										{
											break;
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
							results.Add(result);
						}
					}

					if (results.Count() == 0)
					{
						if (nameResults.Count() > 0)
						{
							FOREACH(ResolveExpressionResult, result, nameResults)
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
					ResolveName(node, node->name.value);

					Ptr<WfClassMember> ownerClassMember;
					ITypeDescriptor* ownerClass = nullptr;
					auto scope = manager->expressionScopes[node].Obj();
					while (scope)
					{
						if (scope->ownerExpression.Cast<WfFunctionExpression>())
						{
							break;
						}
						else if (scope->ownerExpression.Cast<WfOrderedLambdaExpression>())
						{
							break;
						}
						else if (scope->ownerExpression.Cast<WfNewTypeExpression>())
						{
							break;
						}
						else if (scope->ownerDeclaration.Cast<WfFunctionDeclaration>())
						{
							if (scope->ownerClassMember && scope->parentScope)
							{
								ownerClassMember = scope->ownerClassMember;
								if (scope->parentScope->ownerDeclaration)
								{
									ownerClass = manager->declarationTypes[scope->parentScope->ownerDeclaration.Obj()].Obj();
								}
								else
								{
									ownerClass = scope->parentScope->typeDescriptor;
								}
								break;
							}
							else
							{
								break;
							}
						}
						scope = scope->parentScope.Obj();
					}

					if (ownerClassMember)
					{
						bool inStaticFunction = ownerClassMember->kind == WfClassMemberKind::Static;
						FOREACH(ResolveExpressionResult, result, results)
						{
							if (result.methodInfo)
							{
								if (!result.methodInfo->IsStatic())
								{
									if (inStaticFunction || !ownerClass->CanConvertTo(result.methodInfo->GetOwnerTypeDescriptor()))
									{
										manager->errors.Add(WfErrors::CannotCallMemberInStaticFunction(node, result));
									}
								}
							}
							else if (result.propertyInfo)
							{
								if (inStaticFunction || !ownerClass->CanConvertTo(result.propertyInfo->GetOwnerTypeDescriptor()))
								{
									manager->errors.Add(WfErrors::CannotCallMemberInStaticFunction(node, result));
								}
							}
							else if (result.eventInfo)
							{
								if (inStaticFunction || !ownerClass->CanConvertTo(result.eventInfo->GetOwnerTypeDescriptor()))
								{
									manager->errors.Add(WfErrors::CannotCallMemberInStaticFunction(node, result));
								}
							}
						}
					}
					else
					{
						FOREACH(ResolveExpressionResult, result, results)
						{
							if ((result.methodInfo && !result.methodInfo->IsStatic()) || result.propertyInfo || result.eventInfo)
							{
								manager->errors.Add(WfErrors::CannotCallMemberOutsideOfClass(node, result));
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
					auto scope = manager->expressionScopes[node].Obj();
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

						Ptr<ITypeInfo> resultType = type->GetGenericArgument(0);
						FOREACH_INDEXER(Ptr<WfLexicalSymbol>, symbol, index, parameterSymbols)
						{
							symbol->typeInfo = type->GetGenericArgument(index + 1);
							symbol->type = GetTypeFromTypeInfo(symbol->typeInfo.Obj());
						}
						GetExpressionType(manager, node->body, resultType);
					}
					else
					{
						auto bodyType = GetExpressionType(manager, node->body, 0);
						if (bodyType)
						{
							Ptr<TypeInfoImpl> funcType = new TypeInfoImpl(ITypeInfo::TypeDescriptor);
							funcType->SetTypeDescriptor(description::GetTypeDescriptor<IValueFunctionProxy>());

							Ptr<TypeInfoImpl> genericType = new TypeInfoImpl(ITypeInfo::Generic);
							genericType->SetElementType(funcType);
							genericType->AddGenericArgument(bodyType);

							Ptr<TypeInfoImpl> pointerType = new TypeInfoImpl(ITypeInfo::SharedPtr);
							pointerType->SetElementType(genericType);
							resultType = pointerType;
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
						manager->ResolveMember(type->GetTypeDescriptor(), node->name.value, false, results);

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
							manager->ResolveMember(scopeName->typeDescriptor, node->name.value, true, results);

							if (results.Count() > 0)
							{
								FOREACH(ResolveExpressionResult, result, results)
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

						manager->errors.Add(WfErrors::ChildSymbolNotExists(node, scopeName, node->name.value));
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
					results.Add(ResolveExpressionResult::ReadonlyType(TypeInfoRetriver<double>::CreateTypeInfo()));
				}

				void Visit(WfIntegerExpression* node)override
				{
					ITypeDescriptor* typeDescriptor = 0;
#ifndef VCZH_64
					typeDescriptor = description::GetTypeDescriptor<vint32_t>();
					if (typeDescriptor->GetValueSerializer()->Validate(node->value.value))
					{
						goto TYPE_FINISHED;
					}

					typeDescriptor = description::GetTypeDescriptor<vuint32_t>();
					if (typeDescriptor->GetValueSerializer()->Validate(node->value.value))
					{
						goto TYPE_FINISHED;
					}
#endif
					typeDescriptor = description::GetTypeDescriptor<vint64_t>();
					if (typeDescriptor->GetValueSerializer()->Validate(node->value.value))
					{
						goto TYPE_FINISHED;
					}

					typeDescriptor = description::GetTypeDescriptor<vuint64_t>();
					if (typeDescriptor->GetValueSerializer()->Validate(node->value.value))
					{
						goto TYPE_FINISHED;
					}

					manager->errors.Add(WfErrors::IntegerLiteralOutOfRange(node));

				TYPE_FINISHED:
					Ptr<TypeInfoImpl> typeInfo = new TypeInfoImpl(ITypeInfo::TypeDescriptor);
					typeInfo->SetTypeDescriptor(typeDescriptor);
					results.Add(ResolveExpressionResult::ReadonlyType(typeInfo));
				}

				void Visit(WfStringExpression* node)override
				{
					results.Add(ResolveExpressionResult::ReadonlyType(TypeInfoRetriver<WString>::CreateTypeInfo()));
				}

				void Visit(WfFormatExpression* node)override
				{
					Ptr<ITypeInfo> typeInfo = TypeInfoRetriver<WString>::CreateTypeInfo();
					results.Add(ResolveExpressionResult::ReadonlyType(typeInfo));
					GetExpressionType(manager, node->expandedExpression, typeInfo);
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
							case TypeFlag::Others:
								manager->errors.Add(WfErrors::UnaryOperatorOnWrongType(node, typeInfo.Obj()));
								break;
							default:;
							}
							break;
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
										indexType = TypeInfoRetriver<vint>::CreateTypeInfo();
										resultType = CopyTypeInfo(genericType->GetGenericArgument(0));
									}
									else if (classType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueList>())
									{
										indexType = TypeInfoRetriver<vint>::CreateTypeInfo();
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
										indexType = TypeInfoRetriver<vint>::CreateTypeInfo();
										resultType = TypeInfoRetriver<Value>::CreateTypeInfo();
									}
									else if (genericType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueList>())
									{
										indexType = TypeInfoRetriver<vint>::CreateTypeInfo();
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
					else if (node->op == WfBinaryOperator::Concat)
					{
						Ptr<ITypeInfo> stringType = TypeInfoRetriver<WString>::CreateTypeInfo();
						GetExpressionType(manager, node->first, stringType);
						GetExpressionType(manager, node->second, stringType);
						results.Add(ResolveExpressionResult::ReadonlyType(stringType));
					}
					else if (node->op == WfBinaryOperator::FailedThen)
					{
						Ptr<ITypeInfo> firstType = GetExpressionType(manager, node->first, 0);
						bool depend = IsExpressionDependOnExpectedType(manager, node->second);
						Ptr<ITypeInfo> secondType = GetExpressionType(manager, node->second, (depend ? firstType : nullptr));

						if (firstType && secondType)
						{
							auto mergedType = GetMergedType(firstType, secondType);
							results.Add(ResolveExpressionResult::ReadonlyType(mergedType ? mergedType : firstType));
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
										/*I1		*/TypeFlag::I,
										/*I2		*/TypeFlag::I,
										/*I4		*/TypeFlag::I,
										/*I8		*/TypeFlag::I8,
										/*U1		*/TypeFlag::U,
										/*U2		*/TypeFlag::U,
										/*U4		*/TypeFlag::U,
										/*U8		*/TypeFlag::U8,
										/*F4		*/TypeFlag::F4,
										/*F8		*/TypeFlag::F8,
										/*String	*/TypeFlag::Unknown,
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
										/*I1		*/TypeFlag::I,
										/*I2		*/TypeFlag::I,
										/*I4		*/TypeFlag::I,
										/*I8		*/TypeFlag::I8,
										/*U1		*/TypeFlag::U,
										/*U2		*/TypeFlag::U,
										/*U4		*/TypeFlag::U,
										/*U8		*/TypeFlag::U8,
										/*F4		*/TypeFlag::Unknown,
										/*F8		*/TypeFlag::Unknown,
										/*String	*/TypeFlag::Unknown,
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
					auto scope = manager->expressionScopes[node].Obj();

					FOREACH(Ptr<WfLetVariable>, variable, node->variables)
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
						bool resolveFirst = !IsExpressionDependOnExpectedType(manager, node->trueBranch);
						bool resolveSecond = !IsExpressionDependOnExpectedType(manager, node->falseBranch);

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

						Ptr<TypeInfoImpl> enumerableType = new TypeInfoImpl(ITypeInfo::TypeDescriptor);
						enumerableType->SetTypeDescriptor(description::GetTypeDescriptor<IValueEnumerable>());

						Ptr<TypeInfoImpl> genericType = new TypeInfoImpl(ITypeInfo::Generic);
						genericType->SetElementType(enumerableType);
						genericType->AddGenericArgument(elementType);

						Ptr<TypeInfoImpl> pointerType = new TypeInfoImpl(ITypeInfo::SharedPtr);
						pointerType->SetElementType(genericType);
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
					if (node->arguments.Count() == 0)
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
						bool map = node->arguments[0]->value;
						Ptr<ITypeInfo> keyType, valueType;
						FOREACH(Ptr<WfConstructorArgument>, argument, node->arguments)
						{
							{
								Ptr<ITypeInfo> newKeyType = GetExpressionType(manager, argument->key, 0);
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
								Ptr<ITypeInfo> newValueType = GetExpressionType(manager, argument->value, 0);
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
								Ptr<TypeInfoImpl> pointerType = new TypeInfoImpl(ITypeInfo::SharedPtr);
								{
									Ptr<TypeInfoImpl> genericType = new TypeInfoImpl(ITypeInfo::Generic);
									pointerType->SetElementType(genericType);
									{
										Ptr<TypeInfoImpl> classType = new TypeInfoImpl(ITypeInfo::TypeDescriptor);
										classType->SetTypeDescriptor(description::GetTypeDescriptor<IValueDictionary>());
										genericType->SetElementType(classType);
									}
									genericType->AddGenericArgument(keyType);
									genericType->AddGenericArgument(valueType);
								}
								results.Add(ResolveExpressionResult::ReadonlyType(pointerType));
							}
						}
						else
						{
							if (keyType)
							{
								Ptr<TypeInfoImpl> pointerType = new TypeInfoImpl(ITypeInfo::SharedPtr);
								{
									Ptr<TypeInfoImpl> genericType = new TypeInfoImpl(ITypeInfo::Generic);
									pointerType->SetElementType(genericType);
									{
										Ptr<TypeInfoImpl> classType = new TypeInfoImpl(ITypeInfo::TypeDescriptor);
										classType->SetTypeDescriptor(description::GetTypeDescriptor<IValueList>());
										genericType->SetElementType(classType);
									}
									genericType->AddGenericArgument(keyType);
								}
								results.Add(ResolveExpressionResult::ReadonlyType(pointerType));
							}
						}
					}
				}

				void Visit(WfInferExpression* node)override
				{
					auto scope = manager->expressionScopes[node].Obj();
					Ptr<ITypeInfo> type = CreateTypeInfoFromType(scope, node->type);
					Ptr<ITypeInfo> expressionType = GetExpressionType(manager, node->expression, type);
					if (expressionType)
					{
						results.Add(ResolveExpressionResult::ReadonlyType(type));
					}
				}

				void Visit(WfTypeCastingExpression* node)override
				{
					auto scope = manager->expressionScopes[node].Obj();
					Ptr<ITypeInfo> type = CreateTypeInfoFromType(scope, node->type);
					Ptr<ITypeInfo> expressionType = GetExpressionType(manager, node->expression, 0);
					if (type)
					{
						if (expressionType)
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
					Ptr<ITypeInfo> pointerType = TypeInfoRetriver<Ptr<IEventHandler>>::CreateTypeInfo();
					GetExpressionType(manager, node->handler, pointerType);
					results.Add(ResolveExpressionResult::ReadonlyType(TypeInfoRetriver<bool>::CreateTypeInfo()));
				}

				void Visit(WfBindExpression* node)override
				{
					vint errorCount = manager->errors.Count();
					GetExpressionType(manager, node->expression, 0);
					results.Add(ResolveExpressionResult::ReadonlyType(TypeInfoRetriver<Ptr<IValueSubscription>>::CreateTypeInfo()));

					if (manager->errors.Count() == errorCount)
					{
						ExpandBindExpression(manager, node);
						auto parentScope = manager->expressionScopes[node];
						BuildScopeForExpression(manager, parentScope, node->expandedExpression);
						if (CheckScopes(manager))
						{
							GetExpressionType(manager, node->expandedExpression, 0);
						}
					}
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
								FOREACH(Ptr<WfExpression>, eventExpr, node->events)
								{
									auto ref = eventExpr.Cast<WfReferenceExpression>();
									IEventInfo* info = td->GetEventByName(ref->name.value, true);
									if (!info)
									{
										manager->errors.Add(WfErrors::MemberNotExists(ref.Obj(), td, ref->name.value));
									}
								}
							}
						}
						else
						{
							auto scope = manager->expressionScopes[node].Obj();
							auto symbol = scope->symbols[node->name.value][0];
							symbol->typeInfo = parentType;
							symbol->type = GetTypeFromTypeInfo(parentType.Obj());

							observeeType = GetExpressionType(manager, node->expression, 0);
							FOREACH(Ptr<WfExpression>, eventExpr, node->events)
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

				ITypeInfo* GetFunctionType(const ResolveExpressionResult& result)
				{
					if (result.eventInfo)
					{
						return result.eventInfo->GetHandlerType();
					}
					else if (result.type)
					{
						return result.type.Obj();
					}
					else
					{
						return nullptr;
					}
				}

				Ptr<ITypeInfo> SelectFunction(WfExpression* node, Ptr<WfExpression> functionExpression, List<ResolveExpressionResult>& functions, List<Ptr<WfExpression>>& arguments)
				{
					List<bool> resolvables;
					List<Ptr<ITypeInfo>> types;
					FOREACH(Ptr<WfExpression>, argument, arguments)
					{
						if (IsExpressionDependOnExpectedType(manager, argument))
						{
							resolvables.Add(false);
							types.Add(0);
						}
						else
						{
							resolvables.Add(true);
							types.Add(GetExpressionType(manager, argument, 0));
						}
					}

					List<Ptr<parsing::ParsingError>> functionErrors, nonFunctionErrors;
					ITypeDescriptor* functionFd = description::GetTypeDescriptor<IValueFunctionProxy>();
					for (vint i = functions.Count() - 1; i >= 0; i--)
					{
						bool failed = false;
						auto result = functions[i];
						ITypeInfo* expressionType = GetFunctionType(result);

						if (expressionType->GetDecorator() == ITypeInfo::SharedPtr)
						{
							ITypeInfo* genericType = expressionType->GetElementType();
							if (genericType->GetDecorator() != ITypeInfo::Generic) goto FUNCTION_TYPE_FAILED;
							ITypeInfo* functionType = genericType->GetElementType();
							if (functionType->GetDecorator() != ITypeInfo::TypeDescriptor || functionType->GetTypeDescriptor() != functionFd) goto FUNCTION_TYPE_FAILED;
								
							if (genericType->GetGenericArgumentCount() != types.Count() + 1)
							{
								functionErrors.Add(WfErrors::FunctionArgumentCountMismatched(node, arguments.Count(), result));
								failed = true;
							}
							else
							{
								for (vint j = 0; j < types.Count(); j++)
								{
									if (resolvables[j] && types[j])
									{
										ITypeInfo* argumentType = genericType->GetGenericArgument(j + 1);
										if (!CanConvertToType(types[j].Obj(), argumentType, false))
										{
											functionErrors.Add(WfErrors::FunctionArgumentTypeMismatched(node, result, i + 1, types[j].Obj(), argumentType));
											failed = true;
										}
									}
								}
							}
						}
						else
						{
							goto FUNCTION_TYPE_FAILED;
						}

						goto FUNCTION_TYPE_FINISHED;
					FUNCTION_TYPE_FAILED:
						nonFunctionErrors.Add(WfErrors::ExpressionIsNotFunction(functionExpression.Obj(), result.type.Obj()));
						failed = true;
					FUNCTION_TYPE_FINISHED:
						if (failed)
						{
							functions.RemoveAt(i);
						}
					}

					if (functions.Count() > 1)
					{
						manager->errors.Add(WfErrors::CannotPickOverloadedFunctions(node, functions));
					}

					if (functions.Count() == 1)
					{
						ITypeInfo* genericType = GetFunctionType(functions[0])->GetElementType();
						for (vint i = 0; i < types.Count(); i++)
						{
							if (!resolvables[i])
							{
								ITypeInfo* argumentType = genericType->GetGenericArgument(i + 1);
								GetExpressionType(manager, arguments[i], CopyTypeInfo(argumentType));
							}
						}

						return CopyTypeInfo(genericType->GetGenericArgument(0));
					}
					else
					{
						CopyFrom(manager->errors, (functionErrors.Count() > 0 ? functionErrors : nonFunctionErrors), true);
					}
					return 0;
				}

				void Visit(WfCallExpression* node)override
				{
					List<ResolveExpressionResult> functions;
					GetExpressionTypes(manager, node->function, nullptr, true, functions);

					Ptr<ITypeInfo> resultType = SelectFunction(node, node->function, functions, node->arguments);
					if (resultType)
					{
						manager->expressionResolvings.Add(node->function, functions[0]);
						results.Add(ResolveExpressionResult::ReadonlyType(resultType));
					}
				}

				void Visit(WfFunctionExpression* node)override
				{
					ValidateDeclarationSemantic(manager, node->function);
					auto scope = manager->declarationScopes[node->function.Obj()].Obj();

					Ptr<TypeInfoImpl> functionType = new TypeInfoImpl(ITypeInfo::SharedPtr);
					{
						Ptr<TypeInfoImpl> genericType = new TypeInfoImpl(ITypeInfo::Generic);
						functionType->SetElementType(genericType);
						{
							Ptr<TypeInfoImpl> elementType = new TypeInfoImpl(ITypeInfo::TypeDescriptor);
							elementType->SetTypeDescriptor(description::GetTypeDescriptor<IValueFunctionProxy>());
							genericType->SetElementType(elementType);
						}

						genericType->AddGenericArgument(CreateTypeInfoFromType(scope, node->function->returnType));
						FOREACH(Ptr<WfFunctionArgument>, argument, node->function->arguments)
						{
							genericType->AddGenericArgument(scope->symbols[argument->name.value][0]->typeInfo);
						}
					}
					results.Add(ResolveExpressionResult::ReadonlyType(functionType));
				}

				Ptr<ITypeInfo> GetFunctionDeclarationType(WfLexicalScope* scope,Ptr<WfFunctionDeclaration> decl)
				{
					Ptr<WfLexicalSymbol> symbol = From(manager->declarationScopes[decl.Obj()]->parentScope->symbols[decl->name.value])
						.Where([decl](Ptr<WfLexicalSymbol> symbol)
						{
							return symbol->creatorDeclaration == decl;
						})
						.First();
					return symbol->typeInfo;
				}

				class NewTypeExpressionVisitor : public Object, public WfDeclaration::IVisitor
				{
				public:
					WfLexicalScopeManager*							manager;
					Ptr<WfClassMember>								currentMember;
					List<Ptr<WfFunctionDeclaration>>				closureFunctions;
					List<Ptr<WfFunctionDeclaration>>				overrideFunctions;
					List<Ptr<WfLexicalSymbol>>						variableSymbols;

					NewTypeExpressionVisitor(WfLexicalScopeManager* _manager)
						:manager(_manager)
					{
					}

					void Visit(WfNamespaceDeclaration* node)override
					{
					}

					void Visit(WfFunctionDeclaration* node)override
					{
						if (currentMember->kind == WfClassMemberKind::Normal)
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
						variableSymbols.Add(
							From(manager->declarationScopes[node]->symbols[node->name.value])
								.Where([=](Ptr<WfLexicalSymbol> symbol)
								{
									return symbol->creatorDeclaration == node;
								})
								.First()
							);
					}

					void Visit(WfEventDeclaration* node)override
					{
					}

					void Visit(WfPropertyDeclaration* node)override
					{
					}

					void Visit(WfClassDeclaration* node)override
					{
					}

					void Execute(WfNewTypeExpression* node)
					{
						FOREACH(Ptr<WfClassMember>, member, node->members)
						{
							ValidateDeclarationSemantic(manager, member->declaration);

							currentMember = member;
							member->declaration->Accept(this);
						}
					}
				};

				void Visit(WfNewTypeExpression* node)override
				{
					auto scope = manager->expressionScopes[node].Obj();
					Ptr<ITypeInfo> type = CreateTypeInfoFromType(scope, node->type);
					if (type)
					{
						ITypeDescriptor* td = type->GetTypeDescriptor();
						IMethodGroupInfo* ctors = td->GetConstructorGroup();
						Ptr<ITypeInfo> selectedType;
						IMethodInfo* selectedConstructor = nullptr;

						scope->typeDescriptor = td;

						if (!ctors || ctors->GetMethodCount() == 0)
						{
							manager->errors.Add(WfErrors::ClassContainsNoConstructor(node, type.Obj()));
						}
						else
						{
							if ((td->GetTypeDescriptorFlags() & TypeDescriptorFlags::ClassType) != TypeDescriptorFlags::Undefined)
							{
								if (node->members.Count() == 0)
								{
									List<ResolveExpressionResult> functions;
									for (vint i = 0; i < ctors->GetMethodCount(); i++)
									{
										IMethodInfo* info = ctors->GetMethod(i);
										functions.Add(ResolveExpressionResult::Method(info));
									}

									selectedType = SelectFunction(node, 0, functions, node->arguments);
									if (selectedType)
									{
										selectedConstructor = functions[0].methodInfo;
									}
								}
								else
								{
									manager->errors.Add(WfErrors::ConstructorMixClassAndInterface(node));
								}
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
									NewTypeExpressionVisitor declVisitor(manager);
									declVisitor.Execute(node);

									List<Ptr<WfLexicalSymbol>> captures;
									CopyFrom(captures, declVisitor.variableSymbols);

									FOREACH(Ptr<WfFunctionDeclaration>, func, declVisitor.overrideFunctions)
									{
										implementMethods.Add(func->name.value, func);
									}

									FOREACH(Ptr<WfFunctionDeclaration>, func, From(declVisitor.overrideFunctions).Concat(declVisitor.closureFunctions))
									{
										vint index = manager->functionLambdaCaptures.Keys().IndexOf(func.Obj());
										if (index != -1)
										{
											auto& values = manager->functionLambdaCaptures.GetByIndex(index);
											FOREACH(Ptr<WfLexicalSymbol>, symbol, values)
											{
												if (!captures.Contains(symbol.Obj()))
												{
													captures.Add(symbol);
												}
											}
										}
									}

									FOREACH(Ptr<WfFunctionDeclaration>, func, From(declVisitor.overrideFunctions).Concat(declVisitor.closureFunctions))
									{
										manager->functionLambdaCaptures.Remove(func.Obj());
										FOREACH(Ptr<WfLexicalSymbol>, symbol, captures)
										{
											manager->functionLambdaCaptures.Add(func.Obj(), symbol);
										}
									}
								}
								{
									List<ITypeDescriptor*> types;
									types.Add(td);
									vint begin = 0;

									while (begin < types.Count())
									{
										ITypeDescriptor* currentType = types[begin++];
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
										FOREACH(IMethodInfo*, method, methods)
										{
											manager->errors.Add(WfErrors::InterfaceMethodNotImplemented(node, method));
										}
									};
								auto discardSecond = [=](const WString& key, const List<Ptr<WfFunctionDeclaration>>& methods)
									{
										FOREACH(Ptr<WfFunctionDeclaration>, decl, methods)
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

										FOREACH(IMethodInfo*, method, interfaces)
										{
											Ptr<ITypeInfo> methodType = CreateTypeInfoFromMethodInfo(method);
											typedInterfaceMethods.Add(methodType->GetTypeFriendlyName(), method);
										}

										FOREACH(Ptr<WfFunctionDeclaration>, decl, implements)
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
													FOREACH(IMethodInfo*, method, interfaces)
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

				static void Execute(Ptr<WfExpression> expression, WfLexicalScopeManager* manager, Ptr<ITypeInfo> expectedType, List<ResolveExpressionResult>& results)
				{
					ValidateSemanticExpressionVisitor visitor(manager, expectedType, results);
					expression->Accept(&visitor);
				}
			};

/***********************************************************************
ValidateSemantic
***********************************************************************/

			void ValidateModuleSemantic(WfLexicalScopeManager* manager, Ptr<WfModule> module)
			{
				FOREACH(Ptr<WfDeclaration>, declaration, module->declarations)
				{
					ValidateDeclarationSemantic(manager, declaration);
				}
			}

			void ValidateClassMemberSemantic(WfLexicalScopeManager* manager, Ptr<typeimpl::WfCustomType> td, Ptr<WfClassDeclaration> classDecl, Ptr<WfClassMember> member)
			{
				return ValidateSemanticClassMemberVisitor::Execute(td, classDecl, member, manager);
			}

			void ValidateDeclarationSemantic(WfLexicalScopeManager* manager, Ptr<WfDeclaration> declaration)
			{
				return ValidateSemanticDeclarationVisitor::Execute(declaration, manager);
			}

			void ValidateStatementSemantic(WfLexicalScopeManager* manager, Ptr<WfStatement> statement)
			{
				return ValidateSemanticStatementVisitor::Execute(statement, manager);
			}

			void ValidateExpressionSemantic(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType, collections::List<ResolveExpressionResult>& results)
			{
				ValidateSemanticExpressionVisitor::Execute(expression, manager, expectedType, results);
				for (vint i = results.Count() - 1; i >= 0; i--)
				{
					auto result = results[i];
					if (result.scopeName && result.scopeName->declarations.Count() > 0)
					{
						List<ResolveExpressionResult> replaces;
						FOREACH(Ptr<WfDeclaration>, decl, result.scopeName->declarations)
						{
							vint index = manager->declarationScopes.Keys().IndexOf(decl);
							if (index == -1) continue;
							auto scope = manager->declarationScopes.Values()[index];
							bool isVariable = decl.Cast<WfVariableDeclaration>();
							if (!isVariable)
							{
								scope = scope->parentScope;
							}

							index = scope->symbols.Keys().IndexOf(decl->name.value);
							if (index == -1) continue;
							FOREACH(Ptr<WfLexicalSymbol>, symbol, scope->symbols.GetByIndex(index))
							{
								if (symbol->creatorDeclaration == decl && symbol->typeInfo)
								{
									replaces.Add(ResolveExpressionResult::Symbol(symbol));
								}
							}
						}

						if (replaces.Count() > 0)
						{
							results.RemoveAt(i);
							FOREACH_INDEXER(ResolveExpressionResult, replaceResult, index, replaces)
							{
								results.Insert(i + index, replaceResult);
							}
						}
					}
				}
			}

/***********************************************************************
GetExpressionScopeName
***********************************************************************/

			Ptr<WfLexicalScopeName> GetExpressionScopeName(WfLexicalScopeManager* manager, Ptr<WfExpression> expression)
			{
				List<ResolveExpressionResult> results;
				ValidateExpressionSemantic(manager, expression, 0, results);
				if (results.Count() == 0) return 0;

				for (vint i = results.Count() - 1; i >= 0; i--)
				{
					auto& result = results[i];
					if (!result.scopeName)
					{
						results.RemoveAt(i);
					}
				}
				if (results.Count() == 0)
				{
					manager->errors.Add(WfErrors::ExpressionIsNotScopeName(expression.Obj()));
					return 0;
				}
				else if (results.Count() > 1)
				{
					manager->errors.Add(WfErrors::TooManyTargets(expression.Obj(), results, GetExpressionName(expression)));
					return 0;
				}

				auto result = results[0];
				manager->expressionResolvings.Add(expression, result);
				return result.scopeName;
			}

/***********************************************************************
GetExpressionEventInfo
***********************************************************************/

			reflection::description::IEventInfo* GetExpressionEventInfo(WfLexicalScopeManager* manager, Ptr<WfExpression> expression)
			{
				List<ResolveExpressionResult> results;
				ValidateExpressionSemantic(manager, expression, 0, results);
				if (results.Count() == 0) return 0;

				for (vint i = results.Count() - 1; i >= 0; i--)
				{
					auto& result = results[i];
					if (!result.eventInfo)
					{
						results.RemoveAt(i);
					}
				}
				if (results.Count() == 0)
				{
					manager->errors.Add(WfErrors::ExpressionIsNotEvent(expression.Obj()));
					return 0;
				}
				else if (results.Count() > 1)
				{
					manager->errors.Add(WfErrors::TooManyTargets(expression.Obj(), results, GetExpressionName(expression)));
					return 0;
				}

				auto result = results[0];
				manager->expressionResolvings.Add(expression, result);
				return result.eventInfo;
			}

/***********************************************************************
GetExpressionTypes/GetExpressionType/GetLeftValueExpressionType
***********************************************************************/

			void GetExpressionTypes(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType, bool allowEvent, collections::List<ResolveExpressionResult>& results)
			{
				ValidateExpressionSemantic(manager, expression, expectedType, results);
				if (results.Count() == 0) return;

				Ptr<WfLexicalScopeName> scopeName;
				IEventInfo* eventInfo = 0;
				for (vint i = results.Count() - 1; i >= 0; i--)
				{
					auto& result = results[i];
					if (result.scopeName && !scopeName)
					{
						scopeName = result.scopeName;
					}
					if (result.eventInfo && !eventInfo)
					{
						eventInfo = result.eventInfo;
					}
					if (!result.type && !(allowEvent && eventInfo))
					{
						results.RemoveAt(i);
					}
				}
				if (results.Count() == 0)
				{
					if (scopeName)
					{
						manager->errors.Add(WfErrors::ScopeNameIsNotExpression(expression.Obj(), scopeName));
					}
					if (eventInfo)
					{
						manager->errors.Add(WfErrors::EventIsNotExpression(expression.Obj(), eventInfo));
					}
				}

				if (expectedType)
				{
					List<Ptr<ITypeInfo>> failedTypes;
					for (vint i = results.Count() - 1; i >= 0; i--)
					{
						auto& result = results[i];
						if (!CanConvertToType(result.type.Obj(), expectedType.Obj(), false))
						{
							failedTypes.Add(result.type);
							results.RemoveAt(i);
						}
					}

					if (results.Count() == 0)
					{
						FOREACH(Ptr<ITypeInfo>, type, failedTypes)
						{
							manager->errors.Add(WfErrors::ExpressionCannotImplicitlyConvertToType(expression.Obj(), type.Obj(), expectedType.Obj()));
						}
					}
				}
			}

			Ptr<reflection::description::ITypeInfo> GetExpressionType(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType)
			{
				List<ResolveExpressionResult> results;
				GetExpressionTypes(manager, expression, expectedType, false, results);
				
				if (results.Count() > 1)
				{
					manager->errors.Add(WfErrors::TooManyTargets(expression.Obj(), results, GetExpressionName(expression)));
					return expectedType;
				}
				else if (results.Count() == 1)
				{
					auto result = results[0];
					result.expectedType = expectedType;
					manager->expressionResolvings.Add(expression, result);
					return expectedType ? expectedType : result.type;
				}
				else
				{
					return expectedType;
				}
			}

			Ptr<reflection::description::ITypeInfo>	GetLeftValueExpressionType(WfLexicalScopeManager* manager, Ptr<WfExpression> expression)
			{
				List<ResolveExpressionResult> results;
				GetExpressionTypes(manager, expression, nullptr, false, results);

				if (results.Count() > 1)
				{
					manager->errors.Add(WfErrors::TooManyTargets(expression.Obj(), results, GetExpressionName(expression)));
				}
				else if (results.Count() == 1)
				{
					if (results[0].writableType)
					{
						auto result = results[0];
						manager->expressionResolvings.Add(expression, result);
						return result.writableType;
					}
					else
					{
						manager->errors.Add(WfErrors::ExpressionIsNotLeftValue(expression.Obj(), results[0]));
					}
				}
				return 0;
			}

/***********************************************************************
GetEnumerableExpressionItemType
***********************************************************************/

			Ptr<reflection::description::ITypeInfo>	GetEnumerableExpressionItemType(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType)
			{
				Ptr<ITypeInfo> collectionType = GetExpressionType(manager, expression, expectedType);
				if (collectionType)
				{
					if (collectionType->GetTypeDescriptor()->CanConvertTo(description::GetTypeDescriptor<IValueEnumerable>()))
					{
						if (collectionType->GetDecorator() == ITypeInfo::SharedPtr)
						{
							ITypeInfo* genericType = collectionType->GetElementType();
							if (genericType->GetDecorator() == ITypeInfo::Generic && genericType->GetGenericArgumentCount() == 1)
							{
								return CopyTypeInfo(genericType->GetGenericArgument(0));
							}
						}
						return TypeInfoRetriver<Value>::CreateTypeInfo();
					}

					manager->errors.Add(WfErrors::ExpressionIsNotCollection(expression.Obj(), collectionType.Obj()));
				}
				return 0;
			}
		}
	}
}
