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

			Ptr<ITypeInfo> SelectFunction(WfLexicalScopeManager* manager, parsing::ParsingTreeCustomBase* node, Ptr<WfExpression> functionExpression, List<ResolveExpressionResult>& functions, List<Ptr<WfExpression>>& arguments, vint& selectedFunctionIndex)
			{
				selectedFunctionIndex = -1;

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
				List<vint> selectedFunctionIndices;
				ITypeDescriptor* functionFd = description::GetTypeDescriptor<IValueFunctionProxy>();
				for (vint i = 0; i < functions.Count(); i++)
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
					if (!failed)
					{
						selectedFunctionIndices.Add(i);
					}
				}

				if (selectedFunctionIndices.Count() == 1)
				{
					selectedFunctionIndex = selectedFunctionIndices[0];
					ITypeInfo* genericType = GetFunctionType(functions[selectedFunctionIndex])->GetElementType();
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
					if (selectedFunctionIndices.Count() > 1)
					{
						List<ResolveExpressionResult> overloadedFunctions;
						CopyFrom(
							overloadedFunctions,
							From(selectedFunctionIndices)
							.Select([&functions](vint index)
							{
								return functions[index];
							}));
						manager->errors.Add(WfErrors::CannotPickOverloadedFunctions(node, overloadedFunctions));
					}

					if (functionErrors.Count() > 0)
					{
						CopyFrom(manager->errors, functionErrors, true);
					}
					else
					{
						CopyFrom(manager->errors, nonFunctionErrors, true);
					}
					return nullptr;
				}
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

				ValidateSemanticClassMemberVisitor(Ptr<WfCustomType> _td, Ptr<WfClassDeclaration> _classDecl, WfLexicalScopeManager* _manager)
					:td(_td)
					, classDecl(_classDecl)
					, manager(_manager)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);
				}

				void Visit(WfVariableDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);
				}

				void Visit(WfEventDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);
				}

				void Visit(WfPropertyDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);

					auto scope = manager->nodeScopes[node];
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

				static void SelectConstructor(WfLexicalScopeManager* manager, WfConstructorDeclaration* ctorDecl, parsing::ParsingTreeCustomBase* node, ITypeDescriptor* td, List<Ptr<WfExpression>>& arguments)
				{
					List<ResolveExpressionResult> functions;
					if (auto ctors = td->GetConstructorGroup())
					{
						vint count = ctors->GetMethodCount();
						for (vint i = 0; i < count; i++)
						{
							auto ctor = ctors->GetMethod(i);
							functions.Add(ResolveExpressionResult::Method(ctor));
						}
					}

					vint selectedFunctionIndex = -1;
					SelectFunction(manager, node, nullptr, functions, arguments, selectedFunctionIndex);
					if (selectedFunctionIndex != -1)
					{
						auto ctor = functions[selectedFunctionIndex].methodInfo;
						auto call = dynamic_cast<WfBaseConstructorCall*>(node);
						manager->baseConstructorCallResolvings.Add({ctorDecl, td}, {call, ctor});
					}
				}

				void Visit(WfConstructorDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);

					auto scope = manager->nodeScopes[node].Obj();
					auto classScope = scope->parentScope.Obj();
					SortedList<ITypeDescriptor*> baseTypes, initTypes;
					{
						auto td = classScope->typeOfThisExpr;
						vint count = td->GetBaseTypeDescriptorCount();
						for (vint i = 0; i < count; i++)
						{
							auto baseTd = td->GetBaseTypeDescriptor(i);
							if (!baseTypes.Contains(baseTd))
							{
								baseTypes.Add(baseTd);
							}
						}
					}

					FOREACH(Ptr<WfBaseConstructorCall>, call, node->baseConstructorCalls)
					{
						if (auto scopeName = GetScopeNameFromReferenceType(classScope, call->type))
						{
							if (auto td = scopeName->typeDescriptor)
							{
								if (initTypes.Contains(td))
								{
									manager->errors.Add(WfErrors::DuplicatedBaseConstructorCall(call.Obj(), td));
								}
								else if (baseTypes.Contains(td))
								{
									initTypes.Add(td);
									SelectConstructor(manager, node, call.Obj(), td, call->arguments);
								}
								else
								{
									manager->errors.Add(WfErrors::WrongBaseConstructorCall(call.Obj(), td));
								}
							}
						}
					}

					{
						vint index = -1;
						for (vint i = 0; i < initTypes.Count(); i++)
						{
							while (baseTypes[++index] != initTypes[i])
							{
								auto td = baseTypes[index];
								List<Ptr<WfExpression>> arguments;
								SelectConstructor(manager, node, node, td, arguments);
							}
						}
						while (++index < baseTypes.Count())
						{
							auto td = baseTypes[index];
							List<Ptr<WfExpression>> arguments;
							SelectConstructor(manager, node, node, td, arguments);
						}
					}

					ValidateStatementSemantic(manager, node->statement);
				}

				void Visit(WfDestructorDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);
					ValidateStatementSemantic(manager, node->statement);
				}

				void Visit(WfClassDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);
				}

				void Visit(WfEnumDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);
				}

				void Visit(WfStructDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);
				}

				void Visit(WfVirtualDeclaration* node)override
				{
					FOREACH(Ptr<WfDeclaration>, decl, node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				static void Execute(Ptr<WfCustomType> td, Ptr<WfClassDeclaration> classDecl, Ptr<WfDeclaration> memberDecl, WfLexicalScopeManager* manager)
				{
					ValidateSemanticClassMemberVisitor visitor(td, classDecl, manager);
					memberDecl->Accept(&visitor);
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

				void Visit(List<Ptr<WfAttribute>>& attributes)
				{
					FOREACH(Ptr<WfAttribute>, attribute, attributes)
					{
						auto key = Pair<WString, WString>(attribute->category.value, attribute->name.value);
						vint index = manager->attributes.Keys().IndexOf(key);
						if (index == -1)
						{
							manager->errors.Add(WfErrors::AttributeNotExists(attribute.Obj()));
						}
						else
						{
							auto expectedType = manager->attributes.Values()[index];
							if (attribute->value)
							{
								ValidateConstantExpression(manager, attribute->value, expectedType);
							}
							else if (expectedType->GetTypeDescriptor() != description::GetTypeDescriptor<void>())
							{
								manager->errors.Add(WfErrors::AttributeMissValue(attribute.Obj()));
							}
						}
					}
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					FOREACH(Ptr<WfDeclaration>, declaration, node->declarations)
					{
						ValidateDeclarationSemantic(manager, declaration);
					}
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					if (node->statement)
					{
						ValidateStatementSemantic(manager, node->statement);
					}
					FOREACH(Ptr<WfFunctionArgument>, argument, node->arguments)
					{
						Visit(argument->attributes);
					}
				}

				void Visit(WfVariableDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
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

				void Visit(WfConstructorDeclaration* node)override
				{
				}

				void Visit(WfDestructorDeclaration* node)override
				{
				}

				void Visit(WfClassDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
					auto td = manager->declarationTypes[node].Cast<WfCustomType>();

					if (node->kind == WfClassKind::Interface)
					{
						FOREACH(Ptr<WfType>, baseType, node->baseTypes)
						{
							auto scopeName = GetScopeNameFromReferenceType(scope->parentScope.Obj(), baseType);
							auto baseTd = scopeName->typeDescriptor;
							auto ctor = FindInterfaceConstructor(baseTd);
							if (ctor == nullptr)
							{
								manager->errors.Add(WfErrors::WrongInterfaceBaseType(node, baseTd));
							}
						}
					}

					FOREACH(Ptr<WfDeclaration>, memberDecl, node->declarations)
					{
						ValidateClassMemberSemantic(manager, td, node, memberDecl);
					}
				}

				void Visit(WfEnumDeclaration* node)override
				{
					FOREACH(Ptr<WfEnumItem>, item, node->items)
					{
						Visit(item->attributes);
					}
				}

				void SearchForItself(WfStructDeclaration* node, ITypeDescriptor* target, ITypeDescriptor* current, List<WString>& path)
				{
					if (target == current)
					{
						manager->errors.Add(WfErrors::StructRecursivelyIncludeItself(
							node, 
							From(path).Aggregate([](const WString& a, const WString& b) {return a + L"::" + b; })
							));
						return;
					}

					if (current == nullptr)
					{
						current = target;
					}

					vint count = current->GetPropertyCount();
					for (vint i = 0; i < count; i++)
					{
						auto prop = current->GetProperty(i);
						auto propType = prop->GetReturn();
						auto propTd = prop->GetReturn()->GetTypeDescriptor();
						if (propType->GetDecorator()==ITypeInfo::TypeDescriptor && propTd->GetTypeDescriptorFlags() == TypeDescriptorFlags::Struct)
						{
							vint index = path.Add(prop->GetName());
							SearchForItself(node, target, propTd, path);
							path.RemoveAt(index);
						}
					}
				}

				void Visit(WfStructDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
					auto td = manager->declarationTypes[node].Cast<WfStruct>();
					FOREACH(Ptr<WfStructMember>, member, node->members)
					{
						auto memberTd = td->GetPropertyByName(member->name.value, false)->GetReturn()->GetTypeDescriptor();
						if ((memberTd->GetTypeDescriptorFlags() & TypeDescriptorFlags::ReferenceType) != TypeDescriptorFlags::Undefined)
						{
							manager->errors.Add(WfErrors::StructContainsNonValueType(member.Obj(), node));
						}
					}
					List<WString> path;
					path.Add(td->GetTypeName());
					SearchForItself(node, td.Obj(), nullptr, path);

					FOREACH(Ptr<WfStructMember>, member, node->members)
					{
						Visit(member->attributes);
					}
				}

				void Visit(WfVirtualDeclaration* node)override
				{
					FOREACH(Ptr<WfDeclaration>, decl, node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				static void Execute(Ptr<WfDeclaration> declaration, WfLexicalScopeManager* manager)
				{
					ValidateSemanticDeclarationVisitor visitor(manager);
					declaration->Accept(&visitor);
					visitor.Visit(declaration->attributes);
				}
			};

/***********************************************************************
ValidateSemantic(Statement)
***********************************************************************/

			class ExpandVirtualStatementVisitor : public Object, public WfVirtualStatement::IVisitor
			{
			public:
				WfLexicalScopeManager*				manager;

				ExpandVirtualStatementVisitor(WfLexicalScopeManager* _manager)
					:manager(_manager)
				{
				}

				void Visit(WfSwitchStatement* node)override
				{
					ExpandSwitchStatement(manager, node);
				}

				void Visit(WfForEachStatement* node)override
				{
					ExpandForEachStatement(manager, node);
				}

				void Visit(WfCoProviderStatement* node)override
				{
					throw 0;
				}
			};

			class ValidateSemanticStatementVisitor
				: public Object
				, public WfStatement::IVisitor
				, public WfVirtualStatement::IVisitor
				, public WfCoroutineStatement::IVisitor
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
					auto scope = manager->nodeScopes[node].Obj();
					auto functionScope = scope->FindFunctionScope();
					if (auto funcDecl = functionScope->ownerNode.Cast<WfFunctionDeclaration>())
					{
						if (funcDecl->statement.Cast<WfCoProviderStatement>())
						{
							auto providerSymbol = manager->nodeScopes[funcDecl->statement.Obj()]->symbols[L"$PROVIDER"][0];
							if (providerSymbol->typeInfo)
							{
								if (auto group = providerSymbol->typeInfo->GetTypeDescriptor()->GetMethodGroupByName(L"ReturnAndExit", true))
								{
									List<ResolveExpressionResult> functions;
									vint count = group->GetMethodCount();
									for (vint i = 0; i < count; i++)
									{
										auto method = group->GetMethod(i);
										if (method->IsStatic())
										{
											functions.Add(ResolveExpressionResult::Method(method));
										}
									}

									vint selectedFunctionIndex = -1;
									List<Ptr<WfExpression>> arguments;
									if (node->expression)
									{
										arguments.Add(node->expression);
									}
									SelectFunction(manager, node, nullptr, functions, arguments, selectedFunctionIndex);
									if (selectedFunctionIndex != -1)
									{
										manager->coOperatorResolvings.Add(node, functions[selectedFunctionIndex]);
									}
								}
								else
								{
									manager->errors.Add(WfErrors::CoOperatorNotExists(node, providerSymbol->typeInfo.Obj()));
								}
							}
						}
						else
						{
							auto returnType = CreateTypeInfoFromType(scope, funcDecl->returnType);
							if (node->expression)
							{
								if (returnType->GetTypeDescriptor() == description::GetTypeDescriptor<void>())
								{
									manager->errors.Add(WfErrors::CannotReturnExpression(node));
								}
								else
								{
									GetExpressionType(manager, node->expression, returnType);
								}
							}
							else if (returnType->GetDecorator() != ITypeInfo::TypeDescriptor || returnType->GetTypeDescriptor() != description::GetTypeDescriptor<void>())
							{
								manager->errors.Add(WfErrors::ReturnMissExpression(node, returnType.Obj()));
							}
						}
					}
					else
					{
						if (node->expression)
						{
							manager->errors.Add(WfErrors::CannotReturnExpression(node));
						}
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
						auto scope = manager->nodeScopes[node].Obj();
						auto symbol = scope->symbols[node->name.value][0];
						if (!IsNullAcceptableType(symbol->typeInfo.Obj()))
						{
							manager->errors.Add(WfErrors::NullCannotImplicitlyConvertToType(node->expression.Obj(), symbol->typeInfo.Obj()));
						}
						GetExpressionType(manager, node->expression, nullptr);
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

				void Visit(WfWhileStatement* node)override
				{
					Ptr<ITypeInfo> boolType = TypeInfoRetriver<bool>::CreateTypeInfo();
					GetExpressionType(manager, node->condition, boolType);
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
						ValidateStatementSemantic(manager, statement);
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

				void Visit(WfVirtualStatement* node)override
				{
					bool expanded = node->expandedStatement;
					vint errorCount = manager->errors.Count();
					node->Accept((WfVirtualStatement::IVisitor*)this);

					if (!expanded && manager->errors.Count() == errorCount)
					{
						ExpandVirtualStatementVisitor visitor(manager);
						node->Accept(&visitor);
						SetCodeRange(node->expandedStatement, node->codeRange);

						auto parentScope = manager->nodeScopes[node];
						if (parentScope->ownerNode == node)
						{
							parentScope = parentScope->parentScope;
						}

						ContextFreeStatementDesugar(manager, node->expandedStatement);
						BuildScopeForStatement(manager, parentScope, node->expandedStatement);
						if (!CheckScopes_DuplicatedSymbol(manager) || !CheckScopes_SymbolType(manager))
						{
							return;
						}
					}

					if (node->expandedStatement)
					{
						ValidateStatementSemantic(manager, node->expandedStatement);
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

				void Visit(WfForEachStatement* node)override
				{
					Ptr<ITypeInfo> elementType = GetEnumerableExpressionItemType(manager, node->collection, 0);
					if (elementType)
					{
						auto scope = manager->nodeScopes[node].Obj();
						auto symbol = scope->symbols[node->name.value][0];
						symbol->typeInfo = elementType;
						symbol->type = GetTypeFromTypeInfo(elementType.Obj());
					}
					ValidateStatementSemantic(manager, node->statement);
				}

				void Visit(WfCoProviderStatement* node)override
				{
					auto scope = manager->nodeScopes[node].Obj();
					auto symbol = scope->symbols[L"$PROVIDER"][0];
					ITypeDescriptor* selectedProviderTd = nullptr;
					List<WString> candidates;

					if (node->name.value == L"")
					{
						auto decl = scope->parentScope->ownerNode.Cast<WfFunctionDeclaration>();
						auto funcSymbol = manager->GetDeclarationSymbol(scope->parentScope.Obj(), decl.Obj());
						if (funcSymbol->typeInfo)
						{
							List<ITypeDescriptor*> unprocessed;
							unprocessed.Add(funcSymbol->typeInfo->GetElementType()->GetGenericArgument(0)->GetTypeDescriptor());

							for (vint i = 0; i < unprocessed.Count(); i++)
							{
								auto td = unprocessed[i];
								auto candidate = td->GetTypeName() + L"Coroutine";
								if ((selectedProviderTd = description::GetTypeDescriptor(candidate)))
								{
									break;
								}
								else
								{
									candidates.Add(candidate);
								}

								vint count = td->GetBaseTypeDescriptorCount();
								for (vint i = 0; i < count; i++)
								{
									auto baseTd = td->GetBaseTypeDescriptor(i);
									if (!unprocessed.Contains(baseTd))
									{
										unprocessed.Add(baseTd);
									}
								}
							}
						}
					}
					else
					{
						List<ResolveExpressionResult> results, resolveResults;
						auto providerName = node->name.value.Right(node->name.value.Length() - 1);

						if (manager->ResolveName(scope, providerName, resolveResults))
						{
							CopyFrom(results, resolveResults);

							for (vint i = results.Count() - 1; i >= 0; i--)
							{
								auto& result = results[i];
								ITypeDescriptor* providerTd = nullptr;

								if (result.scopeName && result.scopeName->typeDescriptor)
								{
									auto candidate = result.scopeName->typeDescriptor->GetTypeName() + L"Coroutine";
									providerTd = description::GetTypeDescriptor(candidate);
									if (providerTd)
									{
										selectedProviderTd = providerTd;
									}
									else
									{
										candidates.Add(candidate);
									}
								}

								if (!providerTd)
								{
									results.RemoveAt(i);
								}
							}

							if (results.Count() == 1)
							{
								goto FINISH_SEARCHING;
							}
							else if (results.Count() > 1)
							{
								manager->errors.Add(WfErrors::TooManyTargets(node, resolveResults, providerName));
								goto SKIP_SEARCHING;
							}
						}

						resolveResults.Clear();
						if (manager->ResolveName(scope, providerName + L"Coroutine", resolveResults))
						{
							CopyFrom(results, resolveResults);

							for (vint i = results.Count() - 1; i >= 0; i--)
							{
								auto& result = results[i];

								if (result.scopeName && result.scopeName->typeDescriptor)
								{
									selectedProviderTd = result.scopeName->typeDescriptor;
								}
								else
								{
									results.RemoveAt(i);
								}
							}

							if (results.Count() == 1)
							{
								goto FINISH_SEARCHING;
							}
							else if (results.Count() > 1)
							{
								manager->errors.Add(WfErrors::TooManyTargets(node, resolveResults, providerName));
								goto SKIP_SEARCHING;
							}
						}

						candidates.Add(providerName);
						candidates.Add(providerName + L"Coroutine");
					}

				FINISH_SEARCHING:
					if (selectedProviderTd)
					{
					symbol->typeInfo = MakePtr<TypeDescriptorTypeInfo>(selectedProviderTd, TypeInfoHint::Normal);
					}
					else
					{
						manager->errors.Add(WfErrors::CoProviderNotExists(node, candidates));
					}
				SKIP_SEARCHING:
				ValidateStatementSemantic(manager, node->statement);
				}

				void Visit(WfCoroutineStatement* node)override
				{
					node->Accept((WfCoroutineStatement::IVisitor*)this);
				}

				void Visit(WfCoPauseStatement* node)override
				{
					if (node->statement)
					{
						ValidateStatementSemantic(manager, node->statement);
					}
				}

				void Visit(WfCoOperatorStatement* node)override
				{
					auto scope = manager->nodeScopes[node].Obj();
					auto functionScope = scope->FindFunctionScope();
					if (auto funcDecl = functionScope->ownerNode.Cast<WfFunctionDeclaration>())
					{
						if (funcDecl->statement.Cast<WfCoProviderStatement>())
						{
							auto providerSymbol = manager->nodeScopes[funcDecl->statement.Obj()]->symbols[L"$PROVIDER"][0];
							if (providerSymbol->typeInfo)
							{
								List<IMethodGroupInfo*> groups;
								auto operatorName = node->opName.value.Right(node->opName.value.Length() - 1);
								if (auto group = providerSymbol->typeInfo->GetTypeDescriptor()->GetMethodGroupByName(operatorName + L"AndRead", true))
								{
									groups.Add(group);
								}
								if (node->varName.value == L"")
								{
									if (auto group = providerSymbol->typeInfo->GetTypeDescriptor()->GetMethodGroupByName(operatorName + L"AndPause", true))
									{
										groups.Add(group);
									}
								}

								if (groups.Count() == 0)
								{
									manager->errors.Add(WfErrors::CoOperatorNotExists(node, providerSymbol->typeInfo.Obj()));
								}
								else
								{
									List<ResolveExpressionResult> functions;
									FOREACH(IMethodGroupInfo*, group, groups)
									{
										vint count = group->GetMethodCount();
										for (vint i = 0; i < count; i++)
										{
											auto method = group->GetMethod(i);
											if (method->IsStatic())
											{
												functions.Add(ResolveExpressionResult::Method(method));
											}
										}
									}

									vint selectedFunctionIndex = -1;
									vint oldErrorCount = manager->errors.Count();
									SelectFunction(manager, node, nullptr, functions, node->arguments, selectedFunctionIndex);
									if (selectedFunctionIndex != -1)
									{
										manager->coOperatorResolvings.Add(node, functions[selectedFunctionIndex]);
										if (node->varName.value != L"" && manager->errors.Count() == oldErrorCount)
										{
											auto symbol = scope->symbols[node->varName.value][0];
											List<ITypeInfo*> types;

											FOREACH(Ptr<WfExpression>, argument, node->arguments)
											{
												vint index = manager->expressionResolvings.Keys().IndexOf(argument.Obj());
												if (index != -1)
												{
													auto type = manager->expressionResolvings.Values()[index].type;
													if (!types.Contains(type.Obj()))
													{
														types.Add(type.Obj());
														if (auto group = type->GetTypeDescriptor()->GetMethodGroupByName(L"CastResult", true))
														{
															vint count = group->GetMethodCount();
															for (vint i = 0; i < count; i++)
															{
																auto method = group->GetMethod(i);
																if (method->IsStatic())
																{
																	if (method->GetParameterCount() == 1 &&
																		method->GetParameter(0)->GetType()->GetTypeDescriptor() == description::GetTypeDescriptor<DescriptableObject>() &&
																		method->GetReturn()->GetTypeDescriptor() != description::GetTypeDescriptor<void>()
																		)
																	{
																		symbol->typeInfo = CopyTypeInfo(method->GetReturn());
																		break;
																	}
																}
															}
														}
													}
												}
											}

											if (!symbol->typeInfo)
											{
												manager->errors.Add(WfErrors::CoOperatorCannotResolveResultType(node, types));
											}
										}
									}
								}
							}
						}
					}
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

			class ExpandVirtualExpressionVisitor : public Object, public WfVirtualExpression::IVisitor
			{
			public:
				WfLexicalScopeManager*				manager;

				ExpandVirtualExpressionVisitor(WfLexicalScopeManager* _manager)
					:manager(_manager)
				{
				}

				void Visit(WfBindExpression* node)override
				{
					ExpandBindExpression(manager, node);
				}

				void Visit(WfFormatExpression* node)override
				{
				}

				void Visit(WfNewCoroutineExpression* node)override
				{
					ExpandNewCoroutineExpression(manager, node);
				}
			};

			class ValidateSemanticExpressionVisitor
				: public Object
				, public WfExpression::IVisitor
				, public WfVirtualExpression::IVisitor
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
								auto elementType = MakePtr<TypeDescriptorTypeInfo>(scope->typeOfThisExpr, TypeInfoHint::Normal);
								auto pointerType = MakePtr<RawPtrTypeInfo>(elementType);

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

													if (!lastConfigScope->GetOwnerClassMember())
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

										if (currentScope->functionConfig)
										{
											if (currentScope->functionConfig->lambda)
											{
												auto capture = manager->lambdaCaptures.Get(currentScope->ownerNode.Obj());
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
											bool inMethodBody = lastConfigScope->GetOwnerClassMember() && lastConfigScope->ownerNode.Cast<WfFunctionDeclaration>();
											bool inDtorBody = lastConfigScope->GetOwnerClassMember() && lastConfigScope->ownerNode.Cast<WfDestructorDeclaration>();
											bool inCtorBody = lastConfigScope->parentScope->GetOwnerClassMember() && lastConfigScope->parentScope->ownerNode.Cast<WfConstructorDeclaration>();

											if (!inMethodBody && !inDtorBody && !inCtorBody)
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
					FOREACH(ResolveExpressionResult, result, results)
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
							auto funcType = MakePtr<TypeDescriptorTypeInfo>(description::GetTypeDescriptor<IValueFunctionProxy>(), TypeInfoHint::Normal);
							auto genericType = MakePtr<GenericTypeInfo>(funcType);
							genericType->AddGenericArgument(bodyType);

							resultType = MakePtr<SharedPtrTypeInfo>(genericType);
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
					results.Add(ResolveExpressionResult::ReadonlyType(TypeInfoRetriver<double>::CreateTypeInfo()));
				}

				template<typename T>
				bool ValidateInteger(const WString& text, ITypeDescriptor*& resultTd)
				{
					T value;
					if (TypedValueSerializerProvider<T>::Deserialize(text, value))
					{
						resultTd = description::GetTypeDescriptor<T>();
						return true;
					}
					return false;
				}

				void Visit(WfIntegerExpression* node)override
				{
					ITypeDescriptor* typeDescriptor = nullptr;
#ifndef VCZH_64
					if (ValidateInteger<vint32_t>(node->value.value, typeDescriptor)) goto TYPE_FINISHED;
					if (ValidateInteger<vuint32_t>(node->value.value, typeDescriptor)) goto TYPE_FINISHED;
#endif
					if (ValidateInteger<vint64_t>(node->value.value, typeDescriptor)) goto TYPE_FINISHED;
					if (ValidateInteger<vuint64_t>(node->value.value, typeDescriptor)) goto TYPE_FINISHED;

					manager->errors.Add(WfErrors::IntegerLiteralOutOfRange(node));
					typeDescriptor = description::GetTypeDescriptor<vint>();

				TYPE_FINISHED:
					results.Add(ResolveExpressionResult::ReadonlyType(MakePtr<TypeDescriptorTypeInfo>(typeDescriptor, TypeInfoHint::Normal)));
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
					else if (node->op == WfBinaryOperator::Union)
					{
						auto typeA = GetExpressionType(manager, node->first, expectedType);
						auto typeB = GetExpressionType(manager, node->second, expectedType);

						if (typeA && typeB)
						{
							auto stringType = TypeInfoRetriver<WString>::CreateTypeInfo();
							if (CanConvertToType(typeA.Obj(), stringType.Obj(), false) && CanConvertToType(typeB.Obj(), stringType.Obj(), false))
							{
								results.Add(ResolveExpressionResult::ReadonlyType(stringType));
							}
							else if (auto type = GetMergedType(typeA, typeB))
							{
								if (type->GetTypeDescriptor()->GetTypeDescriptorFlags() != TypeDescriptorFlags::FlagEnum)
								{
									manager->errors.Add(WfErrors::IncorrectTypeForUnion(node->first.Obj(), type.Obj()));
								}
								results.Add(ResolveExpressionResult::ReadonlyType(type));
							}
							else
							{
								manager->errors.Add(WfErrors::CannotMergeTwoType(node, typeA.Obj(), typeB.Obj()));
							}
						}
					}
					else if (node->op == WfBinaryOperator::Intersect)
					{
						auto typeA = GetExpressionType(manager, node->first, expectedType);
						auto typeB = GetExpressionType(manager, node->second, expectedType);

						if (typeA && typeB)
						{
							if (auto type = GetMergedType(typeA, typeB))
							{
								if (type->GetTypeDescriptor()->GetTypeDescriptorFlags() != TypeDescriptorFlags::FlagEnum)
								{
									manager->errors.Add(WfErrors::IncorrectTypeForIntersect(node->first.Obj(), type.Obj()));
								}
								results.Add(ResolveExpressionResult::ReadonlyType(type));
							}
							else
							{
								manager->errors.Add(WfErrors::CannotMergeTwoType(node, typeA.Obj(), typeB.Obj()));
							}
						}
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

						auto enumerableType = MakePtr<TypeDescriptorTypeInfo>(description::GetTypeDescriptor<IValueEnumerable>(), TypeInfoHint::Normal);
						auto genericType = MakePtr<GenericTypeInfo>(enumerableType);
						genericType->AddGenericArgument(elementType);

						auto pointerType = MakePtr<SharedPtrTypeInfo>(genericType);
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
						FOREACH(Ptr<WfConstructorArgument>, argument, node->arguments)
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
								auto classType = MakePtr<TypeDescriptorTypeInfo>(description::GetTypeDescriptor<IValueDictionary>(), TypeInfoHint::Normal);
								auto genericType = MakePtr<GenericTypeInfo>(classType);
								genericType->AddGenericArgument(keyType);
								genericType->AddGenericArgument(valueType);

								auto pointerType = MakePtr<SharedPtrTypeInfo>(genericType);
								results.Add(ResolveExpressionResult::ReadonlyType(pointerType));
							}
						}
						else
						{
							if (keyType)
							{
								auto classType = MakePtr<TypeDescriptorTypeInfo>(description::GetTypeDescriptor<IValueList>(), TypeInfoHint::Normal);
								auto genericType = MakePtr<GenericTypeInfo>(classType);
								genericType->AddGenericArgument(keyType);

								auto pointerType = MakePtr<SharedPtrTypeInfo>(genericType);
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
							auto scope = manager->nodeScopes[node].Obj();
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

					auto classType = MakePtr<TypeDescriptorTypeInfo>(description::GetTypeDescriptor<IValueFunctionProxy>(), TypeInfoHint::Normal);
					auto genericType = MakePtr<GenericTypeInfo>(classType);
					genericType->AddGenericArgument(CreateTypeInfoFromType(scope, node->function->returnType));
					FOREACH(Ptr<WfFunctionArgument>, argument, node->function->arguments)
					{
						genericType->AddGenericArgument(scope->symbols[argument->name.value][0]->typeInfo);
					}

					auto pointerType = MakePtr<SharedPtrTypeInfo>(genericType);
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

				class NewInterfaceExpressionVisitor : public empty_visitor::DeclarationVisitor
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

					void Dispatch(WfVirtualDeclaration* node)override
					{
						FOREACH(Ptr<WfDeclaration>, decl, node->expandedDeclarations)
						{
							decl->Accept(this);
						}
					}

					void Visit(WfFunctionDeclaration* node)override
					{
						lastFunction = node;
						if (node->classMember->kind == WfClassMemberKind::Override)
						{
							overrideFunctions.Add(node);
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
						FOREACH(Ptr<WfDeclaration>, memberDecl, node->declarations)
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
										FOREACH(Ptr<WfFunctionDeclaration>, func, declVisitor.overrideFunctions)
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

				void Visit(WfVirtualExpression* node)override
				{
					bool expanded = node->expandedExpression;
					vint errorCount = manager->errors.Count();
					node->Accept((WfVirtualExpression::IVisitor*)this);

					if (!expanded && manager->errors.Count() == errorCount)
					{
						ExpandVirtualExpressionVisitor visitor(manager);
						node->Accept(&visitor);
						SetCodeRange(node->expandedExpression, node->codeRange);

						auto parentScope = manager->nodeScopes[node];
						if (parentScope->ownerNode == node)
						{
							parentScope = parentScope->parentScope;
						}

						ContextFreeExpressionDesugar(manager, node->expandedExpression);
						BuildScopeForExpression(manager, parentScope, node->expandedExpression);
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

				void Visit(WfFormatExpression* node)override
				{
					Ptr<ITypeInfo> typeInfo = TypeInfoRetriver<WString>::CreateTypeInfo();
					results.Add(ResolveExpressionResult::ReadonlyType(typeInfo));
				}

				void Visit(WfNewCoroutineExpression* node)override
				{
					Ptr<ITypeInfo> typeInfo = TypeInfoRetriver<Ptr<ICoroutine>>::CreateTypeInfo();
					results.Add(ResolveExpressionResult::ReadonlyType(typeInfo));
					ValidateStatementSemantic(manager, node->statement);
				}

				static void Execute(Ptr<WfExpression> expression, WfLexicalScopeManager* manager, Ptr<ITypeInfo> expectedType, List<ResolveExpressionResult>& results)
				{
					ValidateSemanticExpressionVisitor visitor(manager, expectedType, results);
					expression->Accept(&visitor);
				}
			};

/***********************************************************************
IsConstantExpression
***********************************************************************/

			class ValidateConstantExpressionVisitor
				: public Object
				, public WfExpression::IVisitor
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

				void Visit(WfThisExpression* node)override
				{
				}

				void Visit(WfTopQualifiedExpression* node)override
				{
				}

				void Visit(WfReferenceExpression* node)override
				{
					VisitReferenceExpression(node, node->name.value);
				}

				void Visit(WfOrderedNameExpression* node)override
				{
				}

				void Visit(WfOrderedLambdaExpression* node)override
				{
				}

				void Visit(WfMemberExpression* node)override
				{
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

				void Visit(WfUnaryExpression* node)override
				{
					isConstant = Call(node->operand);
				}

				void Visit(WfBinaryExpression* node)override
				{
					if (node->op == WfBinaryOperator::Intersect)
					{
						isConstant = Call(node->first) && Call(node->second);
					}
				}

				void Visit(WfLetExpression* node)override
				{
				}

				void Visit(WfIfExpression* node)override
				{
				}

				void Visit(WfRangeExpression* node)override
				{
					isConstant = Call(node->begin) && Call(node->end);
				}

				void Visit(WfSetTestingExpression* node)override
				{
				}

				void Visit(WfConstructorExpression* node)override
				{
					auto result = manager->expressionResolvings[node];
					bool isStruct = (result.type->GetTypeDescriptor()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Struct);

					FOREACH(Ptr<WfConstructorArgument>, argument, node->arguments)
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

				void Visit(WfInferExpression* node)override
				{
				}

				void Visit(WfTypeCastingExpression* node)override
				{
				}

				void Visit(WfTypeTestingExpression* node)override
				{
				}

				void Visit(WfTypeOfTypeExpression* node)override
				{
					isConstant = true;
				}

				void Visit(WfTypeOfExpressionExpression* node)override
				{
					isConstant = true;
				}

				void Visit(WfAttachEventExpression* node)override
				{
				}

				void Visit(WfDetachEventExpression* node)override
				{
				}

				void Visit(WfObserveExpression* node)override
				{
				}

				void Visit(WfCallExpression* node)override
				{
				}

				void Visit(WfFunctionExpression* node)override
				{
				}

				void Visit(WfNewClassExpression* node)override
				{
				}

				void Visit(WfNewInterfaceExpression* node)override
				{
				}

				void Visit(WfVirtualExpression* node)override
				{
					if (node->expandedExpression)
					{
						isConstant = Call(node->expandedExpression);
					}
				}

				static void Execute(Ptr<WfExpression> expression, WfLexicalScopeManager* manager, Ptr<ITypeInfo> expectedType)
				{
					if (GetExpressionType(manager, expression, expectedType))
					{
						ValidateConstantExpressionVisitor visitor(manager);
						expression->Accept(&visitor);
						if (!visitor.isConstant)
						{
							manager->errors.Add(WfErrors::ExpressionIsNotConstant(expression.Obj()));
						}
					}
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

			void ValidateClassMemberSemantic(WfLexicalScopeManager* manager, Ptr<typeimpl::WfCustomType> td, Ptr<WfClassDeclaration> classDecl, Ptr<WfDeclaration> memberDecl)
			{
				return ValidateSemanticClassMemberVisitor::Execute(td, classDecl, memberDecl, manager);
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
							vint index = manager->nodeScopes.Keys().IndexOf(decl.Obj());
							if (index == -1) continue;
							auto scope = manager->nodeScopes.Values()[index];
							if (scope->ownerNode == decl)
							{
								scope = scope->parentScope;
							}

							index = scope->symbols.Keys().IndexOf(decl->name.value);
							if (index == -1) continue;
							FOREACH(Ptr<WfLexicalSymbol>, symbol, scope->symbols.GetByIndex(index))
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
							FOREACH_INDEXER(ResolveExpressionResult, replaceResult, index, replaces)
							{
								results.Insert(i + index, replaceResult);
							}
						}
					}
				}
			}

			void ValidateConstantExpression(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType)
			{
				ValidateConstantExpressionVisitor::Execute(expression, manager, expectedType);
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
