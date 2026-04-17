#include "WfAnalyzer.h"
#include "../Parser/Generated/WorkflowAst_Copy.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;

			namespace rpc_generating
			{
				using namespace copy_visitor;
				using namespace reflection::description;

				struct RpcParamModel
				{
					WString					name;
					Ptr<WfType>				type;
					bool					byref = false;
				};

				enum class RpcMethodKind
				{
					Normal,
					PropertyGetter,
					PropertySetter,
				};

				struct RpcPropertyModel
				{
					WString					name;
					Ptr<WfType>				type;
					bool					byref = false;
					WString					getterName;
					WString					setterName;
				};

				struct RpcMethodModel
				{
					WString					fullName;
					WString					name;
					Ptr<WfType>				returnType;
					bool					returnByref = false;
					RpcMethodKind			kind = RpcMethodKind::Normal;
					vint					methodId = -1;
					List<RpcParamModel>		params;
				};

				struct RpcEventModel
				{
					WString					fullName;
					WString					name;
					vint					eventId = -1;
					List<RpcParamModel>		params;
				};

				struct RpcInterfaceModel
				{
					WString						fullName;
					WString						interfaceName;
					vint						typeId = -1;
					WfClassDeclaration*			interfaceDecl = nullptr;
					List<RpcPropertyModel>		properties;
					List<RpcMethodModel>		methods;
					List<RpcEventModel>			events;
				};

				bool IsRpcAttribute(WfAttribute* attribute, const wchar_t* name)
				{
					return attribute
						&& attribute->category.value == L"rpc"
						&& attribute->name.value == name;
				}

				bool HasRpcAttribute(List<Ptr<WfAttribute>>& attributes, const wchar_t* name)
				{
					for (auto attribute : attributes)
					{
						if (IsRpcAttribute(attribute.Obj(), name))
						{
							return true;
						}
					}
					return false;
				}

				void SplitTypeFullName(const WString& typeFullName, List<WString>& fragments)
				{
					vint start = 0;
					while (start <= typeFullName.Length())
					{
						vint separator = -1;
						for (vint i = start; i + 1 < typeFullName.Length(); i++)
						{
							if (typeFullName[i] == L':' && typeFullName[i + 1] == L':')
							{
								separator = i;
								break;
							}
						}
						if (separator == -1)
						{
							fragments.Add(typeFullName.Sub(start, typeFullName.Length() - start));
							break;
						}

						fragments.Add(typeFullName.Sub(start, separator - start));
						start = separator + 2;
					}
				}

				WString MangleRpcFullName(const WString& fullName)
				{
					WString mangled;
					for (vint i = 0; i < fullName.Length(); i++)
					{
						if (i + 1 < fullName.Length() && fullName[i] == L':' && fullName[i + 1] == L':')
						{
							mangled += L"__";
							i++;
						}
						else if (fullName[i] == L'.')
						{
							mangled += L"_";
						}
						else
						{
							mangled += WString::FromChar(fullName[i]);
						}
					}
					return mangled;
				}

				WfDeclaration* FindRpcDeclaration(WfLexicalScopeManager* manager, const WString& fullName)
				{
					if (auto index = manager->rpcMetadata->typeNames.Keys().IndexOf(fullName); index != -1)
					{
						return manager->rpcMetadata->typeNames.Values()[index];
					}
					if (auto index = manager->rpcMetadata->methodNames.Keys().IndexOf(fullName); index != -1)
					{
						return manager->rpcMetadata->methodNames.Values()[index];
					}
					if (auto index = manager->rpcMetadata->eventNames.Keys().IndexOf(fullName); index != -1)
					{
						return manager->rpcMetadata->eventNames.Values()[index];
					}
					return nullptr;
				}

				template<typename TDecl>
				WString FindFullName(const Dictionary<WString, TDecl*>& names, TDecl* declaration)
				{
					for (vint i = 0; i < names.Count(); i++)
					{
						if (names.Values()[i] == declaration)
						{
							return names.Keys()[i];
						}
					}
					return L"";
				}

				Ptr<WfType> CopyType(WfType* type)
				{
					AstVisitor copier;
					return copier.CopyNode(type);
				}

				Ptr<WfType> CreatePredefinedType(WfPredefinedTypeName name)
				{
					auto type = Ptr(new WfPredefinedType);
					type->name = name;
					return type;
				}

				Ptr<WfType> CreateQualifiedType(const WString& fullName)
				{
					List<WString> fragments;
					SplitTypeFullName(fullName, fragments);
					if (fragments.Count() == 0)
					{
						return nullptr;
					}

					Ptr<WfType> type;
					for (vint i = 0; i < fragments.Count(); i++)
					{
						if (i == 0)
						{
							auto top = Ptr(new WfReferenceType);
							top->name.value = fragments[i];
							type = top;
						}
						else
						{
							auto child = Ptr(new WfChildType);
							child->parent = type;
							child->name.value = fragments[i];
							type = child;
						}
					}
					return type;
				}

				Ptr<WfType> CreateSharedType(const WString& fullName)
				{
					auto type = Ptr(new WfSharedPointerType);
					type->element = CreateQualifiedType(fullName);
					return type;
				}

				Ptr<WfType> CreateRawType(const WString& fullName)
				{
					auto type = Ptr(new WfRawPointerType);
					type->element = CreateQualifiedType(fullName);
					return type;
				}

				Ptr<WfType> CreateMapType(Ptr<WfType> keyType, Ptr<WfType> valueType)
				{
					auto type = Ptr(new WfMapType);
					type->writability = WfMapWritability::Writable;
					type->key = keyType;
					type->value = valueType;
					return type;
				}

				Ptr<WfExpression> CreateNull()
				{
					auto expression = Ptr(new WfLiteralExpression);
					expression->value = WfLiteralValue::Null;
					return expression;
				}

				Ptr<WfExpression> CreateBool(bool value)
				{
					auto expression = Ptr(new WfLiteralExpression);
					expression->value = value ? WfLiteralValue::True : WfLiteralValue::False;
					return expression;
				}

				Ptr<WfExpression> CreateInt(vint value)
				{
					auto expression = Ptr(new WfIntegerExpression);
					expression->value.value = itow(value);
					return expression;
				}

				Ptr<WfExpression> CreateString(const WString& value)
				{
					auto expression = Ptr(new WfStringExpression);
					expression->value.value = value;
					return expression;
				}

				Ptr<WfExpression> CreateReference(const WString& name)
				{
					auto expression = Ptr(new WfReferenceExpression);
					expression->name.value = name;
					return expression;
				}

				Ptr<WfExpression> CreateThis()
				{
					return Ptr(new WfThisExpression);
				}

				Ptr<WfExpression> CreateQualifiedExpression(const WString& fullName)
				{
					List<WString> fragments;
					SplitTypeFullName(fullName, fragments);
					if (fragments.Count() == 0)
					{
						return nullptr;
					}

					Ptr<WfExpression> expression = CreateReference(fragments[0]);
					for (vint i = 1; i < fragments.Count(); i++)
					{
						auto child = Ptr(new WfChildExpression);
						child->parent = expression;
						child->name.value = fragments[i];
						expression = child;
					}
					return expression;
				}

				Ptr<WfExpression> CreateMember(Ptr<WfExpression> parent, const WString& name)
				{
					auto expression = Ptr(new WfMemberExpression);
					expression->parent = parent;
					expression->name.value = name;
					return expression;
				}

				Ptr<WfExpression> CreateBinary(WfBinaryOperator op, Ptr<WfExpression> first, Ptr<WfExpression> second)
				{
					auto expression = Ptr(new WfBinaryExpression);
					expression->op = op;
					expression->first = first;
					expression->second = second;
					return expression;
				}

				Ptr<WfExpression> CreateAssign(Ptr<WfExpression> left, Ptr<WfExpression> right)
				{
					return CreateBinary(WfBinaryOperator::Assign, left, right);
				}

				Ptr<WfExpression> CreateIndex(Ptr<WfExpression> collection, Ptr<WfExpression> index)
				{
					return CreateBinary(WfBinaryOperator::Index, collection, index);
				}

				Ptr<WfExpression> CreateCast(Ptr<WfType> type, Ptr<WfExpression> expression)
				{
					auto cast = Ptr(new WfTypeCastingExpression);
					cast->strategy = WfTypeCastingStrategy::Strong;
					cast->type = type;
					cast->expression = expression;
					return cast;
				}

				template<typename ...TArgs>
				Ptr<WfCallExpression> CreateCall(Ptr<WfExpression> function, TArgs... arguments)
				{
					auto expression = Ptr(new WfCallExpression);
					expression->function = function;
					if constexpr (sizeof...(arguments) > 0)
					{
						(expression->arguments.Add(arguments), ...);
					}
					return expression;
				}

				Ptr<WfConstructorArgument> CreateConstructorArgument(Ptr<WfExpression> key, Ptr<WfExpression> value)
				{
					auto argument = Ptr(new WfConstructorArgument);
					argument->key = key;
					argument->value = value;
					return argument;
				}

				Ptr<WfConstructorExpression> CreateConstructor()
				{
					return Ptr(new WfConstructorExpression);
				}

				Ptr<WfExpression> CreateNewClass(Ptr<WfType> type)
				{
					auto expression = Ptr(new WfNewClassExpression);
					expression->type = type;
					return expression;
				}

				Ptr<WfExpression> CreateNewInterface(Ptr<WfType> type)
				{
					auto expression = Ptr(new WfNewInterfaceExpression);
					expression->type = type;
					return expression;
				}

				Ptr<WfStatement> CreateExpressionStatement(Ptr<WfExpression> expression)
				{
					auto statement = Ptr(new WfExpressionStatement);
					statement->expression = expression;
					return statement;
				}

				Ptr<WfStatement> CreateReturn(Ptr<WfExpression> expression)
				{
					auto statement = Ptr(new WfReturnStatement);
					statement->expression = expression;
					return statement;
				}

				Ptr<WfStatement> CreateRaise(const WString& message)
				{
					auto statement = Ptr(new WfRaiseExceptionStatement);
					statement->expression = CreateString(message);
					return statement;
				}

				Ptr<WfVariableDeclaration> CreateVariableDeclaration(const WString& name, Ptr<WfType> type, Ptr<WfExpression> expression)
				{
					auto declaration = Ptr(new WfVariableDeclaration);
					declaration->name.value = name;
					declaration->type = type;
					declaration->expression = expression;
					return declaration;
				}

				Ptr<WfStatement> CreateVariableStatement(const WString& name, Ptr<WfType> type, Ptr<WfExpression> expression)
				{
					auto statement = Ptr(new WfVariableStatement);
					statement->variable = CreateVariableDeclaration(name, type, expression);
					return statement;
				}

				Ptr<WfStatement> CreateIf(Ptr<WfExpression> condition, Ptr<WfStatement> trueBranch, Ptr<WfStatement> falseBranch = nullptr)
				{
					auto statement = Ptr(new WfIfStatement);
					statement->expression = condition;
					statement->trueBranch = trueBranch;
					statement->falseBranch = falseBranch;
					return statement;
				}

				Ptr<WfBlockStatement> CreateBlock()
				{
					return Ptr(new WfBlockStatement);
				}

				void AddStatement(Ptr<WfBlockStatement> block, Ptr<WfStatement> statement)
				{
					block->statements.Add(statement);
				}

				Ptr<WfFunctionArgument> CreateFunctionArgument(const WString& name, Ptr<WfType> type)
				{
					auto argument = Ptr(new WfFunctionArgument);
					argument->name.value = name;
					argument->type = type;
					return argument;
				}

				Ptr<WfFunctionDeclaration> CreateFunctionDeclaration(const WString& name, Ptr<WfType> returnType, WfFunctionKind kind, WfFunctionAnonymity anonymity = WfFunctionAnonymity::Named)
				{
					auto declaration = Ptr(new WfFunctionDeclaration);
					declaration->name.value = name;
					declaration->returnType = returnType;
					declaration->functionKind = kind;
					declaration->anonymity = anonymity;
					declaration->statement = CreateBlock();
					return declaration;
				}

				Ptr<WfExpression> CreateFunctionExpression(Ptr<WfFunctionDeclaration> declaration)
				{
					auto expression = Ptr(new WfFunctionExpression);
					expression->function = declaration;
					return expression;
				}

				Ptr<WfClassDeclaration> CreateClassDeclaration(const WString& name)
				{
					auto declaration = Ptr(new WfClassDeclaration);
					declaration->name.value = name;
					declaration->kind = WfClassKind::Class;
					declaration->constructorType = WfConstructorType::Undefined;
					return declaration;
				}

				Ptr<WfConstructorDeclaration> CreateConstructorDeclaration(WfConstructorType constructorType = WfConstructorType::SharedPtr)
				{
					auto declaration = Ptr(new WfConstructorDeclaration);
					declaration->constructorType = constructorType;
					declaration->statement = CreateBlock();
					return declaration;
				}

				Ptr<WfExpression> CreateLifecycleHelperCall(const wchar_t* helperName, Ptr<WfExpression> value, Ptr<WfExpression> lifeCycle)
				{
					return CreateCall(CreateQualifiedExpression(L"system::IRpcLifeCycle::" + WString::Unmanaged(helperName)), value, lifeCycle);
				}

				bool IsVoidType(WfType* type)
				{
					if (auto predefined = dynamic_cast<WfPredefinedType*>(type))
					{
						return predefined->name == WfPredefinedTypeName::Void;
					}
					return false;
				}

				Ptr<WfExpression> CreateRpcConstantReference(const wchar_t* prefix, const WString& fullName)
				{
					return CreateReference(WString::Unmanaged(prefix) + MangleRpcFullName(fullName));
				}

				void AddIdLookupEntry(Ptr<WfConstructorExpression> expression, Ptr<WfExpression> key, Ptr<WfExpression> value)
				{
					expression->arguments.Add(CreateConstructorArgument(key, value));
				}

				void CollectMangledNames(WfLexicalScopeManager* manager)
				{
					Dictionary<WString, WString> mangledNames;
					for (auto fullName : manager->rpcMetadata->typeFullNames)
					{
						auto mangled = MangleRpcFullName(fullName);
						auto index = mangledNames.Keys().IndexOf(mangled);
						if (index != -1 && mangledNames.Values()[index] != fullName)
						{
							manager->errors.Add(WfErrors::RpcMangledNameConflict(FindRpcDeclaration(manager, fullName), mangled, mangledNames.Values()[index], fullName));
							return;
						}
						if (index == -1)
						{
							mangledNames.Add(mangled, fullName);
						}
					}

					for (auto fullName : manager->rpcMetadata->methodFullNames)
					{
						auto mangled = MangleRpcFullName(fullName);
						auto index = mangledNames.Keys().IndexOf(mangled);
						if (index != -1 && mangledNames.Values()[index] != fullName)
						{
							manager->errors.Add(WfErrors::RpcMangledNameConflict(FindRpcDeclaration(manager, fullName), mangled, mangledNames.Values()[index], fullName));
							return;
						}
						if (index == -1)
						{
							mangledNames.Add(mangled, fullName);
						}
					}

					for (auto fullName : manager->rpcMetadata->eventFullNames)
					{
						auto mangled = MangleRpcFullName(fullName);
						auto index = mangledNames.Keys().IndexOf(mangled);
						if (index != -1 && mangledNames.Values()[index] != fullName)
						{
							manager->errors.Add(WfErrors::RpcMangledNameConflict(FindRpcDeclaration(manager, fullName), mangled, mangledNames.Values()[index], fullName));
							return;
						}
						if (index == -1)
						{
							mangledNames.Add(mangled, fullName);
						}
					}
				}

				List<RpcInterfaceModel> BuildInterfaceModels(WfLexicalScopeManager* manager)
				{
					List<RpcInterfaceModel> interfaces;
					auto typeCount = manager->rpcMetadata->typeFullNames.Count();
					auto methodCount = manager->rpcMetadata->methodFullNames.Count();

					for (auto typeFullName : manager->rpcMetadata->typeFullNames)
					{
						auto typeIndex = manager->rpcMetadata->typeNames.Keys().IndexOf(typeFullName);
						if (typeIndex == -1)
						{
							continue;
						}

						auto interfaceDecl = manager->rpcMetadata->typeNames.Values()[typeIndex];
						List<WString> fragments;
						SplitTypeFullName(typeFullName, fragments);

						RpcInterfaceModel interfaceModel;
						interfaceModel.fullName = typeFullName;
						interfaceModel.interfaceName = fragments[fragments.Count() - 1];
						interfaceModel.typeId = manager->rpcMetadata->typeFullNames.IndexOf(typeFullName);
						interfaceModel.interfaceDecl = interfaceDecl;

						Dictionary<WString, vint> getterPropertyIndexes;
						Dictionary<WString, vint> setterPropertyIndexes;

						for (auto declaration : interfaceDecl->declarations)
						{
							if (auto propertyDecl = declaration.Cast<WfPropertyDeclaration>())
							{
								RpcPropertyModel propertyModel;
								propertyModel.name = propertyDecl->name.value;
								propertyModel.type = CopyType(propertyDecl->type.Obj());
								propertyModel.byref = HasRpcAttribute(propertyDecl->attributes, L"Byref");
								propertyModel.getterName = propertyDecl->getter.value;
								propertyModel.setterName = propertyDecl->setter.value;
								interfaceModel.properties.Add(std::move(propertyModel));

								auto propertyIndex = interfaceModel.properties.Count() - 1;
								if (propertyModel.getterName != L"")
								{
									getterPropertyIndexes.Add(propertyModel.getterName, propertyIndex);
								}
								if (propertyModel.setterName != L"")
								{
									setterPropertyIndexes.Add(propertyModel.setterName, propertyIndex);
								}
							}
						}

						for (auto declaration : interfaceDecl->declarations)
						{
							if (auto methodDecl = declaration.Cast<WfFunctionDeclaration>())
							{
								auto methodFullName = FindFullName(manager->rpcMetadata->methodNames, methodDecl.Obj());
								if (methodFullName == L"")
								{
									continue;
								}

								RpcMethodModel methodModel;
								methodModel.fullName = methodFullName;
								methodModel.name = methodDecl->name.value;
								methodModel.methodId = typeCount + manager->rpcMetadata->methodFullNames.IndexOf(methodFullName);
								methodModel.returnType = CopyType(methodDecl->returnType.Obj());
								methodModel.returnByref = HasRpcAttribute(methodDecl->attributes, L"Byref");

								if (auto getterIndex = getterPropertyIndexes.Keys().IndexOf(methodModel.name); getterIndex != -1)
								{
									auto&& property = interfaceModel.properties[getterPropertyIndexes.Values()[getterIndex]];
									methodModel.kind = RpcMethodKind::PropertyGetter;
									methodModel.returnType = CopyType(property.type.Obj());
									methodModel.returnByref = property.byref;
								}
								else if (auto setterIndex = setterPropertyIndexes.Keys().IndexOf(methodModel.name); setterIndex != -1)
								{
									auto&& property = interfaceModel.properties[setterPropertyIndexes.Values()[setterIndex]];
									methodModel.kind = RpcMethodKind::PropertySetter;
									methodModel.returnByref = false;
								}

								for (vint i = 0; i < methodDecl->arguments.Count(); i++)
								{
									auto argumentDecl = methodDecl->arguments[i];
									RpcParamModel paramModel;
									paramModel.name = argumentDecl->name.value;
									paramModel.type = CopyType(argumentDecl->type.Obj());
									paramModel.byref = HasRpcAttribute(argumentDecl->attributes, L"Byref");

									if (methodModel.kind == RpcMethodKind::PropertySetter && i == 0)
									{
										auto setterIndex = setterPropertyIndexes.Keys().IndexOf(methodModel.name);
										auto&& property = interfaceModel.properties[setterPropertyIndexes.Values()[setterIndex]];
										paramModel.type = CopyType(property.type.Obj());
										paramModel.byref = property.byref;
									}

									methodModel.params.Add(std::move(paramModel));
								}

								interfaceModel.methods.Add(std::move(methodModel));
							}
							else if (auto eventDecl = declaration.Cast<WfEventDeclaration>())
							{
								auto eventFullName = FindFullName(manager->rpcMetadata->eventNames, eventDecl.Obj());
								if (eventFullName == L"")
								{
									continue;
								}

								RpcEventModel eventModel;
								eventModel.fullName = eventFullName;
								eventModel.name = eventDecl->name.value;
								eventModel.eventId = typeCount + methodCount + manager->rpcMetadata->eventFullNames.IndexOf(eventFullName);

								for (vint i = 0; i < eventDecl->arguments.Count(); i++)
								{
									RpcParamModel paramModel;
									paramModel.name = L"arg" + itow(i);
									paramModel.type = CopyType(eventDecl->arguments[i].Obj());
									paramModel.byref = false;
									eventModel.params.Add(std::move(paramModel));
								}

								interfaceModel.events.Add(std::move(eventModel));
							}
						}

						interfaces.Add(std::move(interfaceModel));
					}

					return interfaces;
				}

				void AddDeclarationToNamespaces(
					List<Ptr<WfDeclaration>>& rootDeclarations,
					Dictionary<WString, Ptr<WfNamespaceDeclaration>>& namespaceMap,
					const List<WString>& namespaceFragments,
					Ptr<WfDeclaration> declaration)
				{
					auto currentDeclarations = &rootDeclarations;
					WString currentPath;

					for (auto fragment : namespaceFragments)
					{
						currentPath = currentPath == L"" ? fragment : currentPath + L"::" + fragment;
						auto index = namespaceMap.Keys().IndexOf(currentPath);
						if (index == -1)
						{
							auto namespaceDecl = Ptr(new WfNamespaceDeclaration);
							namespaceDecl->name.value = fragment;
							currentDeclarations->Add(namespaceDecl);
							namespaceMap.Add(currentPath, namespaceDecl);
							currentDeclarations = &namespaceDecl->declarations;
						}
						else
						{
							currentDeclarations = &namespaceMap.Values()[index]->declarations;
						}
					}

					currentDeclarations->Add(declaration);
				}

				void AppendSyncIdsBody(Ptr<WfBlockStatement> block, WfLexicalScopeManager* manager)
				{
					vint id = 0;
					for (auto fullName : manager->rpcMetadata->typeFullNames)
					{
						AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"ids"), L"Set"), CreateString(fullName), CreateInt(id++))));
					}
					for (auto fullName : manager->rpcMetadata->methodFullNames)
					{
						AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"ids"), L"Set"), CreateString(fullName), CreateInt(id++))));
					}
					for (auto fullName : manager->rpcMetadata->eventFullNames)
					{
						AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"ids"), L"Set"), CreateString(fullName), CreateInt(id++))));
					}
				}

				Ptr<WfStatement> BuildInvokeMethodBranch(const RpcInterfaceModel& interfaceModel, const RpcMethodModel& methodModel)
				{
					auto block = CreateBlock();
					AddStatement(block, CreateVariableStatement(L"target", CreateSharedType(interfaceModel.fullName), CreateCast(CreateSharedType(interfaceModel.fullName), CreateCall(CreateMember(CreateReference(L"_lc"), L"RefToPtr"), CreateReference(L"ref")))));

					List<Ptr<WfExpression>> arguments;
					for (vint i = 0; i < methodModel.params.Count(); i++)
					{
						auto&& paramModel = methodModel.params[i];
						arguments.Add(CreateCast(CopyType(paramModel.type.Obj()), CreateLifecycleHelperCall(paramModel.byref ? L"RpcUnboxByref" : L"RpcUnboxByval", CreateIndex(CreateReference(L"arguments"), CreateInt(i)), CreateReference(L"_lc"))));
					}

					auto invokeTarget = CreateMember(CreateReference(L"target"), methodModel.name);
					auto invoke = Ptr(new WfCallExpression);
					invoke->function = invokeTarget;
					for (auto argument : arguments)
					{
						invoke->arguments.Add(argument);
					}

					if (IsVoidType(methodModel.returnType.Obj()))
					{
						AddStatement(block, CreateExpressionStatement(invoke));
						AddStatement(block, CreateReturn(CreateNull()));
					}
					else
					{
						AddStatement(block, CreateReturn(CreateLifecycleHelperCall(methodModel.returnByref ? L"RpcBoxByref" : L"RpcBoxByval", invoke, CreateReference(L"_lc"))));
					}
					return block;
				}

				Ptr<WfStatement> BuildInvokeEventBranch(const RpcInterfaceModel& interfaceModel, const RpcEventModel& eventModel)
				{
					auto block = CreateBlock();
					AddStatement(block, CreateVariableStatement(L"target", CreateSharedType(interfaceModel.fullName), CreateCast(CreateSharedType(interfaceModel.fullName), CreateCall(CreateMember(CreateReference(L"_lc"), L"RefToPtr"), CreateReference(L"ref")))));

					auto invoke = Ptr(new WfCallExpression);
					invoke->function = CreateMember(CreateReference(L"target"), eventModel.name);
					for (vint i = 0; i < eventModel.params.Count(); i++)
					{
						auto&& paramModel = eventModel.params[i];
						invoke->arguments.Add(CreateCast(CopyType(paramModel.type.Obj()), CreateLifecycleHelperCall(L"RpcUnboxByval", CreateIndex(CreateReference(L"arguments"), CreateInt(i)), CreateReference(L"_lc"))));
					}
					AddStatement(block, CreateExpressionStatement(invoke));
					return block;
				}

				Ptr<WfStatement> BuildDispatchChain(const List<RpcInterfaceModel>& interfaces, bool forEvent)
				{
					Ptr<WfStatement> branch = CreateRaise(forEvent ? L"Unknown RPC event id." : L"Unknown RPC method id.");
					for (vint i = interfaces.Count() - 1; i >= 0; i--)
					{
						auto&& interfaceModel = interfaces[i];
						if (forEvent)
						{
							for (vint j = interfaceModel.events.Count() - 1; j >= 0; j--)
							{
								auto condition = CreateBinary(
									WfBinaryOperator::EQ,
									CreateReference(L"eventId"),
									CreateRpcConstantReference(L"rpcevent_", interfaceModel.events[j].fullName)
								);
								branch = CreateIf(condition, BuildInvokeEventBranch(interfaceModel, interfaceModel.events[j]), branch);
							}
						}
						else
						{
							for (vint j = interfaceModel.methods.Count() - 1; j >= 0; j--)
							{
								auto condition = CreateBinary(
									WfBinaryOperator::EQ,
									CreateReference(L"methodId"),
									CreateRpcConstantReference(L"rpcmethod_", interfaceModel.methods[j].fullName)
								);
								branch = CreateIf(condition, BuildInvokeMethodBranch(interfaceModel, interfaceModel.methods[j]), branch);
							}
						}
					}
					return branch;
				}

				Ptr<WfDeclaration> GenerateObjectOpsFactory(WfLexicalScopeManager* manager, const List<RpcInterfaceModel>& interfaces)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpc_IRpcObjectOps", CreateSharedType(L"system::IRpcObjectOps"), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateRawType(L"system::IRpcLifeCycle")));
					auto newOps = CreateNewInterface(CreateSharedType(L"system::IRpcObjectOps")).Cast<WfNewInterfaceExpression>();
					newOps->declarations.Add(CreateVariableDeclaration(L"_lc", CreateRawType(L"system::IRpcLifeCycle"), CreateReference(L"lc")));
					newOps->declarations.Add(CreateVariableDeclaration(L"_holds", CreateSharedType(L"system::Dictionary"), CreateConstructor()));

					{
						auto sync = CreateFunctionDeclaration(L"SyncIds", CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Override);
						sync->arguments.Add(CreateFunctionArgument(L"ids", CreateSharedType(L"system::Dictionary")));
						AppendSyncIdsBody(sync->statement.Cast<WfBlockStatement>(), manager);
						newOps->declarations.Add(sync);
					}

					{
						auto invokeMethod = CreateFunctionDeclaration(L"InvokeMethod", CreatePredefinedType(WfPredefinedTypeName::Object), WfFunctionKind::Override);
						invokeMethod->arguments.Add(CreateFunctionArgument(L"ref", CreateQualifiedType(L"system::RpcObjectReference")));
						invokeMethod->arguments.Add(CreateFunctionArgument(L"methodId", CreatePredefinedType(WfPredefinedTypeName::Int)));
						invokeMethod->arguments.Add(CreateFunctionArgument(L"arguments", CreateSharedType(L"system::Array")));
						AddStatement(invokeMethod->statement.Cast<WfBlockStatement>(), BuildDispatchChain(interfaces, false));
						newOps->declarations.Add(invokeMethod);
					}

					{
						auto invokeMethodAsync = CreateFunctionDeclaration(L"InvokeMethodAsync", CreateSharedType(L"system::Async"), WfFunctionKind::Override);
						invokeMethodAsync->arguments.Add(CreateFunctionArgument(L"ref", CreateQualifiedType(L"system::RpcObjectReference")));
						invokeMethodAsync->arguments.Add(CreateFunctionArgument(L"methodId", CreatePredefinedType(WfPredefinedTypeName::Int)));
						invokeMethodAsync->arguments.Add(CreateFunctionArgument(L"arguments", CreateSharedType(L"system::Array")));
						AddStatement(invokeMethodAsync->statement.Cast<WfBlockStatement>(), CreateRaise(L"InvokeMethodAsync is not supported."));
						newOps->declarations.Add(invokeMethodAsync);
					}

					{
						auto objectHold = CreateFunctionDeclaration(L"ObjectHold", CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Override);
						objectHold->arguments.Add(CreateFunctionArgument(L"ref", CreateQualifiedType(L"system::RpcObjectReference")));
						objectHold->arguments.Add(CreateFunctionArgument(L"hold", CreatePredefinedType(WfPredefinedTypeName::Bool)));
						auto trueBranch = CreateBlock();
						AddStatement(trueBranch, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"_holds"), L"Set"), CreateReference(L"ref"), CreateCall(CreateMember(CreateReference(L"_lc"), L"RefToPtr"), CreateReference(L"ref")))));
						auto falseBranch = CreateBlock();
						AddStatement(falseBranch, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"_holds"), L"Remove"), CreateReference(L"ref"))));
						AddStatement(objectHold->statement.Cast<WfBlockStatement>(), CreateIf(CreateReference(L"hold"), trueBranch, falseBranch));
						newOps->declarations.Add(objectHold);
					}

					{
						auto requestService = CreateFunctionDeclaration(L"RequestService", CreateQualifiedType(L"system::RpcObjectReference"), WfFunctionKind::Override);
						requestService->arguments.Add(CreateFunctionArgument(L"typeId", CreatePredefinedType(WfPredefinedTypeName::Int)));
						AddStatement(requestService->statement.Cast<WfBlockStatement>(), CreateReturn(CreateCall(CreateMember(CreateReference(L"_lc"), L"PtrToRef"), CreateCall(CreateMember(CreateReference(L"_lc"), L"RequestService"), CreateIndex(CreateReference(L"rpcIdToName"), CreateReference(L"typeId"))))));
						newOps->declarations.Add(requestService);
					}

					AddStatement(functionDecl->statement.Cast<WfBlockStatement>(), CreateReturn(newOps));
					return functionDecl;
				}

				Ptr<WfDeclaration> GenerateObjectEventOpsFactory(WfLexicalScopeManager* manager, const List<RpcInterfaceModel>& interfaces)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpc_IRpcObjectEventOps", CreateSharedType(L"system::IRpcObjectEventOps"), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateRawType(L"system::IRpcLifeCycle")));
					auto newOps = CreateNewInterface(CreateSharedType(L"system::IRpcObjectEventOps")).Cast<WfNewInterfaceExpression>();
					newOps->declarations.Add(CreateVariableDeclaration(L"_lc", CreateRawType(L"system::IRpcLifeCycle"), CreateReference(L"lc")));

					{
						auto sync = CreateFunctionDeclaration(L"SyncIds", CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Override);
						sync->arguments.Add(CreateFunctionArgument(L"ids", CreateSharedType(L"system::Dictionary")));
						AppendSyncIdsBody(sync->statement.Cast<WfBlockStatement>(), manager);
						newOps->declarations.Add(sync);
					}

					{
						auto invokeEvent = CreateFunctionDeclaration(L"InvokeEvent", CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Override);
						invokeEvent->arguments.Add(CreateFunctionArgument(L"ref", CreateQualifiedType(L"system::RpcObjectReference")));
						invokeEvent->arguments.Add(CreateFunctionArgument(L"eventId", CreatePredefinedType(WfPredefinedTypeName::Int)));
						invokeEvent->arguments.Add(CreateFunctionArgument(L"arguments", CreateSharedType(L"system::Array")));
						AddStatement(invokeEvent->statement.Cast<WfBlockStatement>(), BuildDispatchChain(interfaces, true));
						newOps->declarations.Add(invokeEvent);
					}

					AddStatement(functionDecl->statement.Cast<WfBlockStatement>(), CreateReturn(newOps));
					return functionDecl;
				}

				Ptr<WfFunctionDeclaration> CreateAnonymousLambda(const List<RpcParamModel>& params, Ptr<WfBlockStatement> body)
				{
					auto functionDecl = CreateFunctionDeclaration(L"", CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Normal, WfFunctionAnonymity::Anonymous);
					functionDecl->statement = body;
					for (auto&& param : params)
					{
						functionDecl->arguments.Add(CreateFunctionArgument(param.name, CopyType(param.type.Obj())));
					}
					return functionDecl;
				}

				Ptr<WfDeclaration> GenerateWrapperFactory(const RpcInterfaceModel& interfaceModel)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpcwrapper_" + interfaceModel.interfaceName, CreateSharedType(interfaceModel.fullName), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateRawType(L"system::IRpcLifeCycle")));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();

					AddStatement(block, CreateVariableStatement(L"controller", CreateRawType(L"system::IRpcController"), CreateMember(CreateReference(L"lc"), L"Controller")));
					AddStatement(block, CreateVariableStatement(L"proxyRef", CreateQualifiedType(L"system::RpcObjectReference"), CreateCall(CreateMember(CreateReference(L"controller"), L"RequestService"), CreateRpcConstantReference(L"rpctype_", interfaceModel.fullName))));

					auto proxyExpr = CreateNewInterface(CreateSharedType(interfaceModel.fullName)).Cast<WfNewInterfaceExpression>();
					proxyExpr->declarations.Add(CreateVariableDeclaration(L"_lc", CreateRawType(L"system::IRpcLifeCycle"), CreateReference(L"lc")));
					proxyExpr->declarations.Add(CreateVariableDeclaration(L"_controller", CreateRawType(L"system::IRpcController"), CreateReference(L"controller")));
					proxyExpr->declarations.Add(CreateVariableDeclaration(L"_ref", CreateQualifiedType(L"system::RpcObjectReference"), CreateReference(L"proxyRef")));

					for (auto&& methodModel : interfaceModel.methods)
					{
						auto methodDecl = CreateFunctionDeclaration(methodModel.name, CopyType(methodModel.returnType.Obj()), WfFunctionKind::Override);
						for (auto&& paramModel : methodModel.params)
						{
							methodDecl->arguments.Add(CreateFunctionArgument(paramModel.name, CopyType(paramModel.type.Obj())));
						}

						auto methodBlock = methodDecl->statement.Cast<WfBlockStatement>();
						AddStatement(methodBlock, CreateVariableStatement(L"arguments", CreateSharedType(L"system::Array"), CreateConstructor()));
						AddStatement(methodBlock, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"arguments"), L"Resize"), CreateInt(methodModel.params.Count()))));
						for (vint i = 0; i < methodModel.params.Count(); i++)
						{
							auto&& paramModel = methodModel.params[i];
							AddStatement(methodBlock, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"arguments"), L"Set"), CreateInt(i), CreateLifecycleHelperCall(paramModel.byref ? L"RpcBoxByref" : L"RpcBoxByval", CreateReference(paramModel.name), CreateReference(L"_lc")))));
						}

						auto invoke = CreateCall(CreateMember(CreateReference(L"_controller"), L"InvokeMethod"), CreateReference(L"_ref"), CreateRpcConstantReference(L"rpcmethod_", methodModel.fullName), CreateReference(L"arguments"));
						if (IsVoidType(methodModel.returnType.Obj()))
						{
							AddStatement(methodBlock, CreateExpressionStatement(invoke));
						}
						else
						{
							AddStatement(methodBlock, CreateReturn(CreateCast(CopyType(methodModel.returnType.Obj()), CreateLifecycleHelperCall(methodModel.returnByref ? L"RpcUnboxByref" : L"RpcUnboxByval", invoke, CreateReference(L"_lc")))));
						}

						proxyExpr->declarations.Add(methodDecl);
					}

					AddStatement(block, CreateVariableStatement(L"proxy", CreateSharedType(interfaceModel.fullName), proxyExpr));

					for (auto&& eventModel : interfaceModel.events)
					{
						auto lambdaBody = CreateBlock();
						AddStatement(lambdaBody, CreateVariableStatement(L"arguments", CreateSharedType(L"system::Array"), CreateConstructor()));
						AddStatement(lambdaBody, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"arguments"), L"Resize"), CreateInt(eventModel.params.Count()))));
						for (vint i = 0; i < eventModel.params.Count(); i++)
						{
							auto&& paramModel = eventModel.params[i];
							AddStatement(lambdaBody, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"arguments"), L"Set"), CreateInt(i), CreateLifecycleHelperCall(L"RpcBoxByval", CreateReference(paramModel.name), CreateReference(L"lc")))));
						}
						AddStatement(lambdaBody, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"controller"), L"InvokeEvent"), CreateReference(L"proxyRef"), CreateRpcConstantReference(L"rpcevent_", eventModel.fullName), CreateReference(L"arguments"))));

						auto attach = Ptr(new WfAttachEventExpression);
						attach->event = CreateMember(CreateReference(L"proxy"), eventModel.name);
						attach->function = CreateFunctionExpression(CreateAnonymousLambda(eventModel.params, lambdaBody));
						AddStatement(block, CreateExpressionStatement(attach));
					}

					AddStatement(block, CreateReturn(CreateReference(L"proxy")));
					return functionDecl;
				}
			}

			Ptr<WfModule> GenerateModuleRpc(WfLexicalScopeManager* manager)
			{
				using namespace rpc_generating;

				if (!manager || !manager->rpcMetadata || !manager->rpcMetadata->metadataModule)
				{
					return nullptr;
				}

				CollectMangledNames(manager);
				if (manager->errors.Count() > 0)
				{
					return nullptr;
				}

				auto interfaces = BuildInterfaceModels(manager);
				auto module = Ptr(new WfModule);
				module->moduleType = WfModuleType::Module;
				module->name.value = L"RpcMetadata";

				vint id = 0;
				for (auto fullName : manager->rpcMetadata->typeFullNames)
				{
					module->declarations.Add(CreateVariableDeclaration(L"rpctype_" + MangleRpcFullName(fullName), CreatePredefinedType(WfPredefinedTypeName::Int), CreateInt(id++)));
				}
				for (auto fullName : manager->rpcMetadata->methodFullNames)
				{
					module->declarations.Add(CreateVariableDeclaration(L"rpcmethod_" + MangleRpcFullName(fullName), CreatePredefinedType(WfPredefinedTypeName::Int), CreateInt(id++)));
				}
				for (auto fullName : manager->rpcMetadata->eventFullNames)
				{
					module->declarations.Add(CreateVariableDeclaration(L"rpcevent_" + MangleRpcFullName(fullName), CreatePredefinedType(WfPredefinedTypeName::Int), CreateInt(id++)));
				}

				auto nameToId = CreateConstructor();
				id = 0;
				for (auto fullName : manager->rpcMetadata->typeFullNames)
				{
					AddIdLookupEntry(nameToId, CreateString(fullName), CreateInt(id++));
				}
				for (auto fullName : manager->rpcMetadata->methodFullNames)
				{
					AddIdLookupEntry(nameToId, CreateString(fullName), CreateInt(id++));
				}
				for (auto fullName : manager->rpcMetadata->eventFullNames)
				{
					AddIdLookupEntry(nameToId, CreateString(fullName), CreateInt(id++));
				}
				module->declarations.Add(CreateVariableDeclaration(L"rpcNameToId", CreateMapType(CreatePredefinedType(WfPredefinedTypeName::String), CreatePredefinedType(WfPredefinedTypeName::Int)), nameToId));

				auto idToName = CreateConstructor();
				id = 0;
				for (auto fullName : manager->rpcMetadata->typeFullNames)
				{
					AddIdLookupEntry(idToName, CreateInt(id++), CreateString(fullName));
				}
				for (auto fullName : manager->rpcMetadata->methodFullNames)
				{
					AddIdLookupEntry(idToName, CreateInt(id++), CreateString(fullName));
				}
				for (auto fullName : manager->rpcMetadata->eventFullNames)
				{
					AddIdLookupEntry(idToName, CreateInt(id++), CreateString(fullName));
				}
				module->declarations.Add(CreateVariableDeclaration(L"rpcIdToName", CreateMapType(CreatePredefinedType(WfPredefinedTypeName::Int), CreatePredefinedType(WfPredefinedTypeName::String)), idToName));

				module->declarations.Add(GenerateObjectOpsFactory(manager, interfaces));
				module->declarations.Add(GenerateObjectEventOpsFactory(manager, interfaces));

				List<Ptr<WfDeclaration>> wrapperDeclarations;
				Dictionary<WString, Ptr<WfNamespaceDeclaration>> wrapperNamespaces;
				for (auto&& interfaceModel : interfaces)
				{
					List<WString> namespaceFragments;
					SplitTypeFullName(interfaceModel.fullName, namespaceFragments);
					namespaceFragments.RemoveAt(namespaceFragments.Count() - 1);
					AddDeclarationToNamespaces(wrapperDeclarations, wrapperNamespaces, namespaceFragments, GenerateWrapperFactory(interfaceModel));
				}
				for (auto declaration : wrapperDeclarations)
				{
					module->declarations.Add(declaration);
				}

				return module;
			}
		}
	}
}
