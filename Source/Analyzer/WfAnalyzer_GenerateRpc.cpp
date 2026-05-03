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
					ITypeInfo*				typeInfo = nullptr;
					bool					byref = false;
					bool					cached = false;
					WString					getterName;
					WString					setterName;
					WString					valueChangedEvent;
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
					bool						ctor = false;
					WfClassDeclaration*			interfaceDecl = nullptr;
					List<WString>				baseFullNames;
					List<RpcPropertyModel>		properties;
					List<RpcMethodModel>		methods;
					List<RpcEventModel>			events;
				};

				enum class RpcJsonPrimitiveKind
				{
					Number,
					Boolean,
					String,
				};

				struct RpcJsonPrimitiveModel
				{
					WString						fullName;
					WString						keyword;
					RpcJsonPrimitiveKind		kind = RpcJsonPrimitiveKind::Number;
				};

				struct RpcJsonFieldModel
				{
					WString						name;
					Ptr<WfType>					type;
				};

				struct RpcJsonTypeModel
				{
					WString						fullName;
					Ptr<WfType>					type;
					Ptr<List<RpcJsonFieldModel>>	fields;
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

				bool HasAttribute(IAttributeBag* bag, ITypeDescriptor* attrTd)
				{
					if (!bag || !attrTd)
					{
						return false;
					}

					for (vint i = 0; i < bag->GetAttributeCount(); i++)
					{
						if (bag->GetAttribute(i)->GetAttributeType() == attrTd)
						{
							return true;
						}
					}
					return false;
				}

				bool IsStrongTypedCollectionType(WfType* type)
				{
					return dynamic_cast<WfEnumerableType*>(type) != nullptr
						|| dynamic_cast<WfMapType*>(type) != nullptr
						|| dynamic_cast<WfObservableListType*>(type) != nullptr;
				}

				bool IsRpcStrongTypedCollection(ITypeInfo* type)
				{
					if (!type || type->GetDecorator() != ITypeInfo::SharedPtr) return false;
					auto genericType = type->GetElementType();
					if (!genericType || genericType->GetDecorator() != ITypeInfo::Generic) return false;

					auto collectionType = genericType->GetElementType();
					if (!collectionType || collectionType->GetDecorator() != ITypeInfo::TypeDescriptor) return false;

					auto collectionTd = collectionType->GetTypeDescriptor();
					if (collectionTd == GetTypeDescriptor<IValueEnumerable>()
						|| collectionTd == GetTypeDescriptor<IValueReadonlyList>()
						|| collectionTd == GetTypeDescriptor<IValueList>()
						|| collectionTd == GetTypeDescriptor<IValueObservableList>())
					{
						return genericType->GetGenericArgumentCount() == 1;
					}

					if (collectionTd == GetTypeDescriptor<IValueReadonlyDictionary>()
						|| collectionTd == GetTypeDescriptor<IValueDictionary>())
					{
						return genericType->GetGenericArgumentCount() == 2;
					}

					switch (type->GetHint())
					{
					case TypeInfoHint::LazyList:
					case TypeInfoHint::Array:
					case TypeInfoHint::List:
						return genericType->GetGenericArgumentCount() == 1;
					case TypeInfoHint::Dictionary:
						return genericType->GetGenericArgumentCount() == 2;
					default:
						return false;
					}
				}

				ITypeInfo* GetRpcStrongTypedCollectionElementType(ITypeInfo* type)
				{
					if (!IsRpcStrongTypedCollection(type)) return nullptr;
					auto genericType = type->GetElementType();
					if (!genericType || genericType->GetDecorator() != ITypeInfo::Generic) return nullptr;
					switch (genericType->GetGenericArgumentCount())
					{
					case 1:
						return genericType->GetGenericArgument(0);
					case 2:
						return genericType->GetGenericArgument(1);
					default:
						return nullptr;
					}
				}

				bool IsRpcObservableStrongTypedCollection(ITypeInfo* type)
				{
					if (!IsRpcStrongTypedCollection(type)) return false;
					auto genericType = type->GetElementType();
					if (!genericType || genericType->GetDecorator() != ITypeInfo::Generic) return false;
					auto collectionType = genericType->GetElementType();
					if (!collectionType || collectionType->GetDecorator() != ITypeInfo::TypeDescriptor) return false;
					return collectionType->GetTypeDescriptor() == GetTypeDescriptor<IValueObservableList>();
				}

				bool IsRpcInterfaceSharedType(ITypeInfo* type, const SortedList<ITypeDescriptor*>& rpcInterfaceTds)
				{
					if (!type || type->GetDecorator() != ITypeInfo::SharedPtr) return false;
					auto elementType = type->GetElementType();
					if (!elementType || elementType->GetDecorator() != ITypeInfo::TypeDescriptor) return false;
					auto td = elementType->GetTypeDescriptor();
					return td
						&& (td->GetTypeDescriptorFlags() & TypeDescriptorFlags::Interface) != TypeDescriptorFlags::Undefined
						&& rpcInterfaceTds.Contains(td);
				}

				bool IsRpcStrongTypedCollectionContainingRpcInterface(ITypeInfo* type, const SortedList<ITypeDescriptor*>& rpcInterfaceTds)
				{
					auto elementType = GetRpcStrongTypedCollectionElementType(type);
					if (!elementType) return false;
					return IsRpcInterfaceSharedType(elementType, rpcInterfaceTds)
						|| IsRpcStrongTypedCollectionContainingRpcInterface(elementType, rpcInterfaceTds);
				}

				bool IsDefaultRpcTransferByref(ITypeInfo* type, const SortedList<ITypeDescriptor*>& rpcInterfaceTds)
				{
					return IsRpcStrongTypedCollection(type)
						&& (IsRpcObservableStrongTypedCollection(type)
							|| IsRpcStrongTypedCollectionContainingRpcInterface(type, rpcInterfaceTds));
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
						else if (fullName[i] == L'.' || fullName[i] == L'(' || fullName[i] == L')' || fullName[i] == L',')
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

				ITypeDescriptor* FindRpcTypeDescriptor(WfLexicalScopeManager* manager, const WString& fullName)
				{
					for (vint i = 0; i < manager->declarationTypes.Count(); i++)
					{
						auto typeDescriptor = manager->declarationTypes.Values()[i].Obj();
						if (typeDescriptor && typeDescriptor->GetTypeName() == fullName)
						{
							return typeDescriptor;
						}
					}
					return nullptr;
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

				Ptr<WfType> CreateTopQualifiedType(const WString& fullName)
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
							auto top = Ptr(new WfTopQualifiedType);
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

				Ptr<WfType> CreateNullableType(const WString& fullName)
				{
					auto type = Ptr(new WfNullableType);
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

				Ptr<WfExpression> CreateIsNull(Ptr<WfExpression> expression)
				{
					auto testing = Ptr(new WfTypeTestingExpression);
					testing->test = WfTypeTesting::IsNull;
					testing->expression = expression;
					return testing;
				}

				Ptr<WfExpression> CreateIsNotNull(Ptr<WfExpression> expression)
				{
					auto testing = Ptr(new WfTypeTestingExpression);
					testing->test = WfTypeTesting::IsNotNull;
					testing->expression = expression;
					return testing;
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

				Ptr<WfExpression> CreateUnary(WfUnaryOperator op, Ptr<WfExpression> operand)
				{
					auto expression = Ptr(new WfUnaryExpression);
					expression->op = op;
					expression->operand = operand;
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

				Ptr<WfExpression> CreateWeakCast(Ptr<WfType> type, Ptr<WfExpression> expression)
				{
					auto cast = Ptr(new WfTypeCastingExpression);
					cast->strategy = WfTypeCastingStrategy::Weak;
					cast->type = type;
					cast->expression = expression;
					return cast;
				}

				Ptr<WfExpression> CreateInfer(Ptr<WfExpression> expression, Ptr<WfType> type)
				{
					auto infer = Ptr(new WfInferExpression);
					infer->expression = expression;
					infer->type = type;
					return infer;
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

				Ptr<WfStatement> CreateTryFinally(Ptr<WfStatement> protectedStatement, Ptr<WfStatement> finallyStatement)
				{
					auto statement = Ptr(new WfTryStatement);
					statement->protectedStatement = protectedStatement;
					statement->finallyStatement = finallyStatement;
					return statement;
				}

				Ptr<WfForEachStatement> CreateForEach(const WString& name, Ptr<WfExpression> collection, Ptr<WfStatement> body)
				{
					auto statement = Ptr(new WfForEachStatement);
					statement->name.value = name;
					statement->direction = WfForEachDirection::Normal;
					statement->collection = collection;
					statement->statement = body;
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

				void AddSwitchCase(Ptr<WfSwitchStatement> switchStat, Ptr<WfExpression> expression, Ptr<WfStatement> statement)
				{
					auto switchCase = Ptr(new WfSwitchCase);
					switchCase->expression = expression;
					switchCase->statement = statement;
					switchStat->caseBranches.Add(switchCase);
				}

				Ptr<WfExpression> CreateLifecycleHelperCall(const wchar_t* helperName, Ptr<WfExpression> value, Ptr<WfExpression> lifecycle)
				{
					return CreateCall(CreateQualifiedExpression(L"system::IRpcLifecycle::" + WString::Unmanaged(helperName)), value, lifecycle);
				}

				bool IsVoidType(WfType* type)
				{
					if (auto predefined = dynamic_cast<WfPredefinedType*>(type))
					{
						return predefined->name == WfPredefinedTypeName::Void;
					}
					if (auto child = dynamic_cast<WfChildType*>(type))
					{
						if (child->name.value == L"Void")
						{
							if (auto top = dynamic_cast<WfTopQualifiedType*>(child->parent.Obj()))
							{
								return top->name.value == L"system";
							}
						}
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
					auto rpcByrefAttrTd = GetTypeDescriptor<vl::__vwsn::att_rpc_Byref>();
					auto rpcCachedAttrTd = GetTypeDescriptor<vl::__vwsn::att_rpc_Cached>();
					auto rpcDynamicAttrTd = GetTypeDescriptor<vl::__vwsn::att_rpc_Dynamic>();
					SortedList<ITypeDescriptor*> rpcInterfaceTds;

					for (auto typeFullName : manager->rpcMetadata->typeFullNames)
					{
						if (auto td = FindRpcTypeDescriptor(manager, typeFullName))
						{
							if (!rpcInterfaceTds.Contains(td))
							{
								rpcInterfaceTds.Add(td);
							}
						}
					}

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
						interfaceModel.ctor = HasRpcAttribute(interfaceDecl->attributes, L"Ctor");
						interfaceModel.interfaceDecl = interfaceDecl;
						auto typeDescriptor = FindRpcTypeDescriptor(manager, typeFullName);

						if (typeDescriptor)
						{
							for (vint i = 0; i < typeDescriptor->GetBaseTypeDescriptorCount(); i++)
							{
								auto baseTypeDescriptor = typeDescriptor->GetBaseTypeDescriptor(i);
								if (!baseTypeDescriptor)
								{
									continue;
								}

								auto baseFullName = baseTypeDescriptor->GetTypeName();
								if (manager->rpcMetadata->typeFullNames.Contains(baseFullName))
								{
									interfaceModel.baseFullNames.Add(baseFullName);
								}
							}
						}

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
								propertyModel.cached = !HasRpcAttribute(propertyDecl->attributes, L"Dynamic");
								propertyModel.getterName = propertyDecl->getter.value;
								propertyModel.setterName = propertyDecl->setter.value;
								propertyModel.valueChangedEvent = propertyDecl->valueChangedEvent.value;
								if (!HasRpcAttribute(propertyDecl->attributes, L"Cached") && !HasRpcAttribute(propertyDecl->attributes, L"Dynamic"))
								{
									manager->errors.Add(WfErrors::RpcWrapperGenerationRequiresPropertyMode(propertyDecl.Obj(), typeFullName + L"." + propertyDecl->name.value));
								}
								interfaceModel.properties.Add(std::move(propertyModel));

								auto propertyIndex = interfaceModel.properties.Count() - 1;
								auto&& insertedProperty = interfaceModel.properties[propertyIndex];
								if (insertedProperty.getterName != L"")
								{
									getterPropertyIndexes.Add(insertedProperty.getterName, propertyIndex);
								}
								if (insertedProperty.setterName != L"")
								{
									setterPropertyIndexes.Add(insertedProperty.setterName, propertyIndex);
								}
							}
						}

						if (typeDescriptor)
						{
							for (vint i = 0; i < typeDescriptor->GetPropertyCount(); i++)
							{
								auto propertyInfo = typeDescriptor->GetProperty(i);
								if (!propertyInfo || propertyInfo->GetOwnerTypeDescriptor() != typeDescriptor)
								{
									continue;
								}

								vint propertyIndex = -1;
								for (vint j = 0; j < interfaceModel.properties.Count(); j++)
								{
									if (interfaceModel.properties[j].name == propertyInfo->GetName())
									{
										propertyIndex = j;
										break;
									}
								}

								if (propertyIndex != -1)
								{
									auto&& propertyModel = interfaceModel.properties[propertyIndex];
									propertyModel.typeInfo = propertyInfo->GetReturn();
								}
								else
								{
									RpcPropertyModel propertyModel;
									propertyModel.name = propertyInfo->GetName();
									propertyModel.type = GetTypeFromTypeInfo(propertyInfo->GetReturn());
									propertyModel.typeInfo = propertyInfo->GetReturn();
									propertyModel.byref = HasAttribute(propertyInfo, rpcByrefAttrTd);
									propertyModel.cached = !HasAttribute(propertyInfo, rpcDynamicAttrTd);
									if (auto getter = propertyInfo->GetGetter())
									{
										propertyModel.getterName = getter->GetName();
									}
									if (auto setter = propertyInfo->GetSetter())
									{
										propertyModel.setterName = setter->GetName();
									}
									if (auto valueChangedEvent = propertyInfo->GetValueChangedEvent())
									{
										propertyModel.valueChangedEvent = valueChangedEvent->GetName();
									}
									if (!HasAttribute(propertyInfo, rpcCachedAttrTd) && !HasAttribute(propertyInfo, rpcDynamicAttrTd))
									{
										propertyModel.cached = true;
									}
									interfaceModel.properties.Add(std::move(propertyModel));

									propertyIndex = interfaceModel.properties.Count() - 1;
									auto&& insertedProperty = interfaceModel.properties[propertyIndex];
									if (insertedProperty.getterName != L"")
									{
										getterPropertyIndexes.Add(insertedProperty.getterName, propertyIndex);
									}
									if (insertedProperty.setterName != L"")
									{
										setterPropertyIndexes.Add(insertedProperty.setterName, propertyIndex);
									}
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
								if (IsStrongTypedCollectionType(methodModel.returnType.Obj())
									&& !methodModel.returnByref
									&& !HasRpcAttribute(methodDecl->attributes, L"Byval"))
								{
									manager->errors.Add(WfErrors::RpcWrapperGenerationRequiresCollectionReturnTransfer(methodDecl.Obj(), typeFullName + L"." + methodDecl->name.value));
								}

								if (auto getterIndex = getterPropertyIndexes.Keys().IndexOf(methodModel.name); getterIndex != -1)
								{
									auto&& property = interfaceModel.properties[getterPropertyIndexes.Values()[getterIndex]];
									methodModel.kind = RpcMethodKind::PropertyGetter;
									methodModel.returnType = CopyType(property.type.Obj());
								}
								else if (auto setterIndex = setterPropertyIndexes.Keys().IndexOf(methodModel.name); setterIndex != -1)
								{
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
									if (IsStrongTypedCollectionType(paramModel.type.Obj())
										&& !paramModel.byref
										&& !HasRpcAttribute(argumentDecl->attributes, L"Byval"))
									{
										manager->errors.Add(WfErrors::RpcWrapperGenerationRequiresCollectionParameterTransfer(argumentDecl.Obj(), typeFullName + L"." + methodDecl->name.value + L"(" + argumentDecl->name.value + L")"));
									}

									if (methodModel.kind == RpcMethodKind::PropertySetter && i == 0)
									{
										auto setterIndex = setterPropertyIndexes.Keys().IndexOf(methodModel.name);
										auto&& property = interfaceModel.properties[setterPropertyIndexes.Values()[setterIndex]];
										paramModel.type = CopyType(property.type.Obj());
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

								ITypeInfo* handlerGenericType = nullptr;
								if (typeDescriptor)
								{
									for (vint j = 0; j < typeDescriptor->GetEventCount(); j++)
									{
										auto eventInfo = typeDescriptor->GetEvent(j);
										if (eventInfo && eventInfo->GetOwnerTypeDescriptor() == typeDescriptor && eventInfo->GetName() == eventModel.name)
										{
											auto handlerType = eventInfo->GetHandlerType();
											if (handlerType && handlerType->GetDecorator() == ITypeInfo::SharedPtr)
											{
												auto genericType = handlerType->GetElementType();
												if (genericType && genericType->GetDecorator() == ITypeInfo::Generic)
												{
													handlerGenericType = genericType;
												}
											}
											break;
										}
									}
								}

								for (vint i = 0; i < eventDecl->arguments.Count(); i++)
								{
									RpcParamModel paramModel;
									paramModel.name = L"arg" + itow(i);
									paramModel.type = CopyType(eventDecl->arguments[i].Obj());
									auto genericArgumentIndex = i + 1;
									paramModel.byref = handlerGenericType && genericArgumentIndex < handlerGenericType->GetGenericArgumentCount()
										? IsDefaultRpcTransferByref(handlerGenericType->GetGenericArgument(genericArgumentIndex), rpcInterfaceTds)
										: dynamic_cast<WfObservableListType*>(eventDecl->arguments[i].Obj()) != nullptr;
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
						invoke->arguments.Add(CreateCast(CopyType(paramModel.type.Obj()), CreateLifecycleHelperCall(paramModel.byref ? L"RpcUnboxByref" : L"RpcUnboxByval", CreateIndex(CreateReference(L"arguments"), CreateInt(i)), CreateReference(L"_lc"))));
					}
					AddStatement(block, CreateExpressionStatement(invoke));
					return block;
				}

				Ptr<WfStatement> BuildDispatchChain(const List<RpcInterfaceModel>& interfaces, bool forEvent)
				{
					auto switchStat = Ptr(new WfSwitchStatement);
					switchStat->expression = CreateReference(forEvent ? L"eventId" : L"methodId");
					switchStat->defaultBranch = CreateRaise(forEvent ? L"Unknown RPC event id." : L"Unknown RPC method id.");

					for (auto&& interfaceModel : interfaces)
					{
						if (forEvent)
						{
							for (auto&& eventModel : interfaceModel.events)
							{
								auto switchCase = Ptr(new WfSwitchCase);
								switchCase->expression = CreateRpcConstantReference(L"rpcevent_", eventModel.fullName);
								switchCase->statement = BuildInvokeEventBranch(interfaceModel, eventModel);
								switchStat->caseBranches.Add(switchCase);
							}
						}
						else
						{
							for (auto&& methodModel : interfaceModel.methods)
							{
								auto switchCase = Ptr(new WfSwitchCase);
								switchCase->expression = CreateRpcConstantReference(L"rpcmethod_", methodModel.fullName);
								switchCase->statement = BuildInvokeMethodBranch(interfaceModel, methodModel);
								switchStat->caseBranches.Add(switchCase);
							}
						}
					}
					return switchStat;
				}

				Ptr<WfDeclaration> GenerateIsInterfaceTypeIdHelper(const List<RpcInterfaceModel>& interfaces)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpcwrapper_IsInterfaceTypeId", CreatePredefinedType(WfPredefinedTypeName::Bool), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"typeId", CreatePredefinedType(WfPredefinedTypeName::Int)));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();
					auto isCollectionTypeId = CreateBinary(
						WfBinaryOperator::And,
						CreateBinary(WfBinaryOperator::GE, CreateReference(L"typeId"), CreateInt(rpc_controller::RpcTypeId_IValueReadonlyList)),
						CreateBinary(WfBinaryOperator::LE, CreateReference(L"typeId"), CreateInt(rpc_controller::RpcTypeId_IValueEnumerable))
					);
					AddStatement(block, CreateIf(isCollectionTypeId, CreateReturn(CreateBool(true))));
					auto switchStat = Ptr(new WfSwitchStatement);
					switchStat->expression = CreateReference(L"typeId");
					switchStat->defaultBranch = CreateReturn(CreateBool(false));

					auto addKnownType = [&](Ptr<WfExpression> expression)
					{
						auto switchCase = Ptr(new WfSwitchCase);
						switchCase->expression = expression;
						switchCase->statement = CreateReturn(CreateBool(true));
						switchStat->caseBranches.Add(switchCase);
					};

					for (auto&& interfaceModel : interfaces)
					{
						addKnownType(CreateRpcConstantReference(L"rpctype_", interfaceModel.fullName));
					}

					AddStatement(block, switchStat);
					return functionDecl;
				}

				Ptr<WfDeclaration> GenerateIsCtorInterfaceTypeIdHelper(const List<RpcInterfaceModel>& interfaces)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpcwrapper_IsCtorInterfaceTypeId", CreatePredefinedType(WfPredefinedTypeName::Bool), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"typeId", CreatePredefinedType(WfPredefinedTypeName::Int)));
					auto switchStat = Ptr(new WfSwitchStatement);
					switchStat->expression = CreateReference(L"typeId");
					switchStat->defaultBranch = CreateReturn(CreateBool(false));

					for (auto&& interfaceModel : interfaces)
					{
						if (!interfaceModel.ctor)
						{
							continue;
						}

						auto switchCase = Ptr(new WfSwitchCase);
						switchCase->expression = CreateRpcConstantReference(L"rpctype_", interfaceModel.fullName);
						switchCase->statement = CreateReturn(CreateBool(true));
						switchStat->caseBranches.Add(switchCase);
					}

					AddStatement(functionDecl->statement.Cast<WfBlockStatement>(), switchStat);
					return functionDecl;
				}

				Ptr<WfStatement> BuildRegisterService()
				{
					auto block = CreateBlock();
					auto registerBranch = CreateBlock();
					AddStatement(
						registerBranch,
						CreateExpressionStatement(
							CreateCall(
								CreateMember(CreateMember(CreateReference(L"_lc"), L"Dispatcher"), L"RegisterService"),
								CreateReference(L"typeId"),
								CreateCall(CreateMember(CreateReference(L"_lc"), L"PtrToRef"), CreateReference(L"service"))
								)
							)
						);
					auto nonCtorBranch = CreateBlock();
					AddStatement(nonCtorBranch, CreateRaise(L"RPC service type id is not an @rpc:Ctor interface."));
					auto invalidTypeIdBranch = CreateBlock();
					AddStatement(invalidTypeIdBranch, CreateRaise(L"RPC service type id does not exist."));
					auto invalidRegisterBranch = CreateBlock();
					AddStatement(
						invalidRegisterBranch,
						CreateIf(
							CreateCall(CreateReference(L"rpcwrapper_IsInterfaceTypeId"), CreateReference(L"typeId")),
							nonCtorBranch,
							invalidTypeIdBranch
							)
						);
					AddStatement(
						block,
						CreateIf(
							CreateCall(CreateReference(L"rpcwrapper_IsCtorInterfaceTypeId"), CreateReference(L"typeId")),
							registerBranch,
							invalidRegisterBranch
						)
					);
					return block;
				}

				Ptr<WfDeclaration> GenerateObjectOpsFactory(WfLexicalScopeManager* manager, const List<RpcInterfaceModel>& interfaces)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpc_IRpcObjectOps", CreateSharedType(L"system::IRpcObjectOps"), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateRawType(L"system::IRpcLifecycle")));
					auto newOps = CreateNewInterface(CreateSharedType(L"system::IRpcObjectOps")).Cast<WfNewInterfaceExpression>();
					newOps->declarations.Add(CreateVariableDeclaration(L"_lc", CreateRawType(L"system::IRpcLifecycle"), CreateReference(L"lc")));

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
						objectHold->arguments.Add(CreateFunctionArgument(L"remoteClientId", CreatePredefinedType(WfPredefinedTypeName::Int)));
						objectHold->arguments.Add(CreateFunctionArgument(L"hold", CreatePredefinedType(WfPredefinedTypeName::Bool)));
						auto trueBranch = CreateBlock();
						AddStatement(trueBranch, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"_lc"), L"LocalObjectHold"), CreateReference(L"ref"), CreateReference(L"remoteClientId"))));
						auto falseBranch = CreateBlock();
						AddStatement(falseBranch, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"_lc"), L"LocalObjectUnhold"), CreateReference(L"ref"), CreateReference(L"remoteClientId"))));
						AddStatement(objectHold->statement.Cast<WfBlockStatement>(), CreateIf(CreateReference(L"hold"), trueBranch, falseBranch));
						newOps->declarations.Add(objectHold);
					}

					{
						auto registerService = CreateFunctionDeclaration(L"RegisterService", CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Override);
						registerService->arguments.Add(CreateFunctionArgument(L"typeId", CreatePredefinedType(WfPredefinedTypeName::Int)));
						registerService->arguments.Add(CreateFunctionArgument(L"service", CreateSharedType(L"system::Interface")));
						auto block = registerService->statement.Cast<WfBlockStatement>();
						AddStatement(block, BuildRegisterService());
						newOps->declarations.Add(registerService);
					}

					AddStatement(functionDecl->statement.Cast<WfBlockStatement>(), CreateReturn(newOps));
					return functionDecl;
				}

				Ptr<WfDeclaration> GenerateObjectEventOpsFactory(WfLexicalScopeManager* manager, const List<RpcInterfaceModel>& interfaces)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpc_IRpcObjectEventOps", CreateSharedType(L"system::IRpcObjectEventOps"), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateRawType(L"system::IRpcLifecycle")));
					auto newOps = CreateNewInterface(CreateSharedType(L"system::IRpcObjectEventOps")).Cast<WfNewInterfaceExpression>();
					newOps->declarations.Add(CreateVariableDeclaration(L"_lc", CreateRawType(L"system::IRpcLifecycle"), CreateReference(L"lc")));

					{
						auto invokeEvent = CreateFunctionDeclaration(L"InvokeEvent", CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Override);
						invokeEvent->arguments.Add(CreateFunctionArgument(L"ref", CreateQualifiedType(L"system::RpcObjectReference")));
						invokeEvent->arguments.Add(CreateFunctionArgument(L"eventId", CreatePredefinedType(WfPredefinedTypeName::Int)));
						invokeEvent->arguments.Add(CreateFunctionArgument(L"arguments", CreateSharedType(L"system::Array")));
						auto block = invokeEvent->statement.Cast<WfBlockStatement>();
						AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(CreateMember(CreateReference(L"_lc"), L"Controller"), L"SetEventSuppressedFlag"), CreateReference(L"ref"), CreateReference(L"eventId"), CreateBool(true))));
						auto finallyBlock = CreateBlock();
						AddStatement(finallyBlock, CreateExpressionStatement(CreateCall(CreateMember(CreateMember(CreateReference(L"_lc"), L"Controller"), L"SetEventSuppressedFlag"), CreateReference(L"ref"), CreateReference(L"eventId"), CreateBool(false))));
						AddStatement(block, CreateTryFinally(BuildDispatchChain(interfaces, true), finallyBlock));
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

				const RpcInterfaceModel* FindInterfaceModel(const List<RpcInterfaceModel>& interfaces, const WString& fullName)
				{
					for (auto&& interfaceModel : interfaces)
					{
						if (interfaceModel.fullName == fullName)
						{
							return &interfaceModel;
						}
					}
					return nullptr;
				}

				bool ContainsEventModel(const List<const RpcEventModel*>& events, const WString& fullName)
				{
					for (auto eventModel : events)
					{
						if (eventModel->fullName == fullName)
						{
							return true;
						}
					}
					return false;
				}

				void CollectInterfaceEvents(const RpcInterfaceModel& interfaceModel, const List<RpcInterfaceModel>& interfaces, List<const RpcEventModel*>& events)
				{
					for (auto&& baseFullName : interfaceModel.baseFullNames)
					{
						if (auto baseModel = FindInterfaceModel(interfaces, baseFullName))
						{
							CollectInterfaceEvents(*baseModel, interfaces, events);
						}
					}

					for (auto&& eventModel : interfaceModel.events)
					{
						if (!ContainsEventModel(events, eventModel.fullName))
						{
							events.Add(&eventModel);
						}
					}
				}

				bool HasInterfaceEvents(const RpcInterfaceModel& interfaceModel, const List<RpcInterfaceModel>& interfaces)
				{
					List<const RpcEventModel*> events;
					CollectInterfaceEvents(interfaceModel, interfaces, events);
					return events.Count() > 0;
				}

				const RpcEventModel* FindInterfaceEvent(const List<const RpcEventModel*>& events, const WString& name)
				{
					for (auto eventModel : events)
					{
						if (eventModel->name == name)
						{
							return eventModel;
						}
					}
					return nullptr;
				}

				WString GetPropertyCacheAvailableName(const RpcPropertyModel& propertyModel)
				{
					return propertyModel.name + L"<Available>";
				}

				WString GetPropertyCacheValueName(const RpcPropertyModel& propertyModel)
				{
					return propertyModel.name + L"<Cached>";
				}

				WString GetPropertyCacheResetFunctionName(const RpcPropertyModel& propertyModel)
				{
					return L"_rpcInvalidate_" + propertyModel.name;
				}

				Ptr<WfExpression> CreatePropertyCacheAvailableRead(const RpcPropertyModel& propertyModel)
				{
					return CreateReference(GetPropertyCacheAvailableName(propertyModel));
				}

				Ptr<WfExpression> CreatePropertyCacheValueRead(const RpcPropertyModel& propertyModel)
				{
					return CreateReference(GetPropertyCacheValueName(propertyModel));
				}

				Ptr<WfDeclaration> GenerateWrapperInterface(const RpcInterfaceModel& interfaceModel)
				{
					auto interfaceDecl = Ptr(new WfClassDeclaration);
					interfaceDecl->name.value = L"IRpcWrapper_" + interfaceModel.interfaceName;
					interfaceDecl->kind = WfClassKind::Interface;
					interfaceDecl->constructorType = WfConstructorType::SharedPtr;

					{
						auto baseType = Ptr(new WfReferenceType);
						baseType->name.value = interfaceModel.fullName;
						auto qualType = CreateQualifiedType(interfaceModel.fullName);
						interfaceDecl->baseTypes.Add(qualType);
					}

					{
						auto baseType = CreateQualifiedType(L"system::IRpcWrapperBase");
						interfaceDecl->baseTypes.Add(baseType);
					}

					for (auto&& propertyModel : interfaceModel.properties)
					{
						if (propertyModel.cached && propertyModel.valueChangedEvent != L"")
						{
							auto invalidateDecl = Ptr(new WfFunctionDeclaration);
							invalidateDecl->name.value = GetPropertyCacheResetFunctionName(propertyModel);
							invalidateDecl->returnType = CreatePredefinedType(WfPredefinedTypeName::Void);
							invalidateDecl->functionKind = WfFunctionKind::Normal;
							invalidateDecl->anonymity = WfFunctionAnonymity::Named;
							interfaceDecl->declarations.Add(invalidateDecl);
						}
					}

					return interfaceDecl;
				}

				Ptr<WfDeclaration> GenerateListenerFactory(const RpcInterfaceModel& interfaceModel, const List<RpcInterfaceModel>& interfaces)
				{
					List<const RpcEventModel*> events;
					CollectInterfaceEvents(interfaceModel, interfaces, events);
					if (events.Count() == 0)
					{
						return nullptr;
					}

					auto mangledName = MangleRpcFullName(interfaceModel.fullName);
					auto functionDecl = CreateFunctionDeclaration(L"rpclistener_" + mangledName, CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateRawType(L"system::IRpcLifecycle")));
					functionDecl->arguments.Add(CreateFunctionArgument(L"ref", CreateQualifiedType(L"system::RpcObjectReference")));
					functionDecl->arguments.Add(CreateFunctionArgument(L"target", CreateRawType(interfaceModel.fullName)));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();

					for (auto eventModel : events)
					{
						auto lambdaBody = CreateBlock();

						AddStatement(
							lambdaBody,
							CreateIf(
								CreateCall(
									CreateMember(CreateMember(CreateReference(L"lc"), L"Controller"), L"GetEventSuppressedFlag"),
									CreateReference(L"ref"),
									CreateRpcConstantReference(L"rpcevent_", eventModel->fullName)
									),
								CreateReturn(nullptr)
								)
							);
						AddStatement(lambdaBody, CreateVariableStatement(L"arguments", CreateSharedType(L"system::Array"), CreateConstructor()));
						AddStatement(lambdaBody, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"arguments"), L"Resize"), CreateInt(eventModel->params.Count()))));
						for (vint i = 0; i < eventModel->params.Count(); i++)
						{
							auto&& paramModel = eventModel->params[i];
							AddStatement(lambdaBody, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"arguments"), L"Set"), CreateInt(i), CreateLifecycleHelperCall(paramModel.byref ? L"RpcBoxByref" : L"RpcBoxByval", CreateReference(paramModel.name), CreateReference(L"lc")))));
						}
						AddStatement(
							lambdaBody,
							CreateExpressionStatement(
								CreateCall(
									CreateMember(
										CreateCall(
											CreateMember(CreateMember(CreateReference(L"lc"), L"Dispatcher"), L"BroadcastFromClient_ObjectEventOps"),
											CreateMember(CreateReference(L"lc"), L"ClientId")
											),
										L"InvokeEvent"
										),
									CreateReference(L"ref"),
									CreateRpcConstantReference(L"rpcevent_", eventModel->fullName),
									CreateReference(L"arguments")
									)
								)
							);

						auto attach = Ptr(new WfAttachEventExpression);
						attach->event = CreateMember(CreateReference(L"target"), eventModel->name);
						attach->function = CreateFunctionExpression(CreateAnonymousLambda(eventModel->params, lambdaBody));
						AddStatement(block, CreateExpressionStatement(attach));
					}

					return functionDecl;
				}

				Ptr<WfDeclaration> GenerateListenerDispatcher(const List<RpcInterfaceModel>& interfaces)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpclistener_Attach", CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"typeId", CreatePredefinedType(WfPredefinedTypeName::Int)));
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateRawType(L"system::IRpcLifecycle")));
					functionDecl->arguments.Add(CreateFunctionArgument(L"ref", CreateQualifiedType(L"system::RpcObjectReference")));
					functionDecl->arguments.Add(CreateFunctionArgument(L"obj", CreateRawType(L"system::Interface")));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();

					auto switchStat = Ptr(new WfSwitchStatement);
					switchStat->expression = CreateReference(L"typeId");
					switchStat->defaultBranch = CreateRaise(L"Unknown RPC type id for listener attachment.");

					for (auto&& interfaceModel : interfaces)
					{
						auto switchCase = Ptr(new WfSwitchCase);
						switchCase->expression = CreateRpcConstantReference(L"rpctype_", interfaceModel.fullName);
						auto caseBranch = CreateBlock();
						if (HasInterfaceEvents(interfaceModel, interfaces))
						{
							auto mangledName = MangleRpcFullName(interfaceModel.fullName);
							AddStatement(caseBranch, CreateExpressionStatement(CreateCall(CreateReference(L"rpclistener_" + mangledName), CreateReference(L"lc"), CreateReference(L"ref"), CreateCast(CreateRawType(interfaceModel.fullName), CreateReference(L"obj")))));
						}
						AddStatement(caseBranch, CreateReturn(nullptr));
						switchCase->statement = caseBranch;
						switchStat->caseBranches.Add(switchCase);
					}

					AddStatement(block, switchStat);
					return functionDecl;
				}

				Ptr<WfDeclaration> GenerateWrapperFactory(const RpcInterfaceModel& interfaceModel, const List<RpcInterfaceModel>& interfaces)
				{
					auto mangledName = MangleRpcFullName(interfaceModel.fullName);
					auto wrapperInterfaceFullName = interfaceModel.fullName.Sub(0, interfaceModel.fullName.Length() - interfaceModel.interfaceName.Length()) + L"IRpcWrapper_" + interfaceModel.interfaceName;
					List<const RpcEventModel*> events;
					CollectInterfaceEvents(interfaceModel, interfaces, events);

					auto functionDecl = CreateFunctionDeclaration(L"rpcwrapper_" + mangledName, CreateSharedType(wrapperInterfaceFullName), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateRawType(L"system::IRpcLifecycle")));
					functionDecl->arguments.Add(CreateFunctionArgument(L"proxyRef", CreateQualifiedType(L"system::RpcObjectReference")));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();

					auto proxyExpr = CreateNewInterface(CreateSharedType(wrapperInterfaceFullName)).Cast<WfNewInterfaceExpression>();
					proxyExpr->declarations.Add(CreateVariableDeclaration(L"_lc", CreateRawType(L"system::IRpcLifecycle"), CreateReference(L"lc")));
					proxyExpr->declarations.Add(CreateVariableDeclaration(L"_ref", CreateQualifiedType(L"system::RpcObjectReference"), CreateReference(L"proxyRef")));
					for (auto&& propertyModel : interfaceModel.properties)
					{
						if (propertyModel.cached)
						{
							proxyExpr->declarations.Add(CreateVariableDeclaration(GetPropertyCacheValueName(propertyModel), CopyType(propertyModel.type.Obj()), propertyModel.typeInfo ? CreateDefaultValue(propertyModel.typeInfo) : CreateNull()));
							proxyExpr->declarations.Add(CreateVariableDeclaration(GetPropertyCacheAvailableName(propertyModel), CreatePredefinedType(WfPredefinedTypeName::Bool), CreateBool(false)));
						}
					}

					for (auto&& propertyModel : interfaceModel.properties)
					{
						if (propertyModel.cached && propertyModel.valueChangedEvent != L"")
						{
							auto invalidateDecl = CreateFunctionDeclaration(GetPropertyCacheResetFunctionName(propertyModel), CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Override);
							auto invalidateBlock = invalidateDecl->statement.Cast<WfBlockStatement>();
							AddStatement(invalidateBlock, CreateExpressionStatement(CreateAssign(CreateReference(GetPropertyCacheAvailableName(propertyModel)), CreateBool(false))));
							proxyExpr->declarations.Add(invalidateDecl);
						}
					}

					// Generate DisconnectFromLifecycle override
					{
						auto disconnectDecl = CreateFunctionDeclaration(L"DisconnectFromLifecycle", CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Override);
						auto disconnectBlock = disconnectDecl->statement.Cast<WfBlockStatement>();
						AddStatement(disconnectBlock, CreateExpressionStatement(CreateAssign(CreateReference(L"_lc"), CreateNull())));
						proxyExpr->declarations.Add(disconnectDecl);
					}

					// Generate destructor: delete{}
					{
						auto dtorDecl = Ptr(new WfDestructorDeclaration);
						auto dtorBlock = CreateBlock();
						dtorDecl->statement = dtorBlock;

						// if (_lc is not null) { _lc.Dispatcher.SendToClient_ObjectOps(_ref.clientId).ObjectHold(_ref, _lc.ClientId, false); }
						auto condition = CreateIsNotNull(CreateReference(L"_lc"));
						auto trueBranch = CreateBlock();
						AddStatement(
							trueBranch,
							CreateExpressionStatement(
								CreateCall(
									CreateMember(
										CreateCall(
											CreateMember(CreateMember(CreateReference(L"_lc"), L"Dispatcher"), L"SendToClient_ObjectOps"),
											CreateMember(CreateReference(L"_ref"), L"clientId")
											),
										L"ObjectHold"
										),
									CreateReference(L"_ref"),
									CreateMember(CreateReference(L"_lc"), L"ClientId"),
									CreateBool(false)
									)
								)
							);
						AddStatement(dtorBlock, CreateIf(condition, trueBranch));

						proxyExpr->declarations.Add(dtorDecl);
					}

					for (auto&& methodModel : interfaceModel.methods)
					{
						const RpcPropertyModel* cachedProperty = nullptr;
						if (methodModel.kind == RpcMethodKind::PropertyGetter)
						{
							for (auto&& propertyModel : interfaceModel.properties)
							{
								if (propertyModel.cached && propertyModel.getterName == methodModel.name)
								{
									cachedProperty = &propertyModel;
									break;
								}
							}
						}

						auto methodDecl = CreateFunctionDeclaration(methodModel.name, CopyType(methodModel.returnType.Obj()), WfFunctionKind::Override);
						for (auto&& paramModel : methodModel.params)
						{
							methodDecl->arguments.Add(CreateFunctionArgument(paramModel.name, CopyType(paramModel.type.Obj())));
						}

						auto methodBlock = methodDecl->statement.Cast<WfBlockStatement>();

						// null check: if (_lc is null) raise "..."
						{
							auto condition = CreateIsNull(CreateReference(L"_lc"));
							auto trueBranch = CreateRaise(L"RPC wrapper has been disconnected from lifecycle.");
							AddStatement(methodBlock, CreateIf(condition, trueBranch));
						}

						if (cachedProperty)
						{
							auto trueBranch = CreateBlock();
							AddStatement(trueBranch, CreateReturn(CreatePropertyCacheValueRead(*cachedProperty)));
							AddStatement(methodBlock, CreateIf(CreatePropertyCacheAvailableRead(*cachedProperty), trueBranch));
						}

						AddStatement(methodBlock, CreateVariableStatement(L"arguments", CreateSharedType(L"system::Array"), CreateConstructor()));
						AddStatement(methodBlock, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"arguments"), L"Resize"), CreateInt(methodModel.params.Count()))));
						for (vint i = 0; i < methodModel.params.Count(); i++)
						{
							auto&& paramModel = methodModel.params[i];
							AddStatement(methodBlock, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"arguments"), L"Set"), CreateInt(i), CreateLifecycleHelperCall(paramModel.byref ? L"RpcBoxByref" : L"RpcBoxByval", CreateReference(paramModel.name), CreateReference(L"_lc")))));
						}

						auto invoke = CreateCall(
							CreateMember(
								CreateCall(
									CreateMember(CreateMember(CreateReference(L"_lc"), L"Dispatcher"), L"SendToClient_ObjectOps"),
									CreateMember(CreateReference(L"_ref"), L"clientId")
									),
								L"InvokeMethod"
								),
							CreateReference(L"_ref"),
							CreateRpcConstantReference(L"rpcmethod_", methodModel.fullName),
							CreateReference(L"arguments")
							);
						if (IsVoidType(methodModel.returnType.Obj()))
						{
							AddStatement(methodBlock, CreateExpressionStatement(invoke));
						}
						else if (cachedProperty)
						{
							AddStatement(methodBlock, CreateExpressionStatement(CreateAssign(CreateReference(GetPropertyCacheValueName(*cachedProperty)), CreateCast(CopyType(methodModel.returnType.Obj()), CreateLifecycleHelperCall(methodModel.returnByref ? L"RpcUnboxByref" : L"RpcUnboxByval", invoke, CreateReference(L"_lc"))))));
							AddStatement(methodBlock, CreateExpressionStatement(CreateAssign(CreateReference(GetPropertyCacheAvailableName(*cachedProperty)), CreateBool(true))));
							AddStatement(methodBlock, CreateReturn(CreatePropertyCacheValueRead(*cachedProperty)));
						}
						else
						{
							AddStatement(methodBlock, CreateReturn(CreateCast(CopyType(methodModel.returnType.Obj()), CreateLifecycleHelperCall(methodModel.returnByref ? L"RpcUnboxByref" : L"RpcUnboxByval", invoke, CreateReference(L"_lc")))));
						}

						proxyExpr->declarations.Add(methodDecl);
					}

					AddStatement(block, CreateVariableStatement(L"proxy", CreateSharedType(wrapperInterfaceFullName), proxyExpr));
					AddStatement(
						block,
						CreateExpressionStatement(
							CreateCall(
								CreateMember(
									CreateCall(
										CreateMember(CreateMember(CreateReference(L"lc"), L"Dispatcher"), L"SendToClient_ObjectOps"),
										CreateMember(CreateReference(L"proxyRef"), L"clientId")
										),
									L"ObjectHold"
									),
								CreateReference(L"proxyRef"),
								CreateMember(CreateReference(L"lc"), L"ClientId"),
								CreateBool(true)
								)
							)
						);

					for (auto&& propertyModel : interfaceModel.properties)
					{
						if (propertyModel.cached && propertyModel.valueChangedEvent != L"")
						{
							if (auto eventModel = FindInterfaceEvent(events, propertyModel.valueChangedEvent))
							{
								auto lambdaBody = CreateBlock();
								auto invalidateTarget = CreateCast(CreateSharedType(wrapperInterfaceFullName), CreateCall(CreateMember(CreateReference(L"lc"), L"RefToPtr"), CreateReference(L"proxyRef")));
								AddStatement(lambdaBody, CreateExpressionStatement(CreateCall(CreateMember(invalidateTarget, GetPropertyCacheResetFunctionName(propertyModel)))));

								auto attach = Ptr(new WfAttachEventExpression);
								attach->event = CreateMember(CreateCast(CreateSharedType(interfaceModel.fullName), CreateReference(L"proxy")), eventModel->name);
								attach->function = CreateFunctionExpression(CreateAnonymousLambda(eventModel->params, lambdaBody));
								AddStatement(block, CreateExpressionStatement(attach));
							}
						}
					}

					if (events.Count() > 0)
					{
						AddStatement(block, CreateExpressionStatement(CreateCall(CreateReference(L"rpclistener_" + mangledName), CreateReference(L"lc"), CreateReference(L"proxyRef"), CreateCast(CreateRawType(interfaceModel.fullName), CreateReference(L"proxy")))));
					}

					AddStatement(block, CreateReturn(CreateReference(L"proxy")));
					return functionDecl;
				}

				Ptr<WfExpression> CreateJsonArrayItem(const WString& arrayName, vint index)
				{
					return CreateIndex(CreateMember(CreateReference(arrayName), L"items"), CreateInt(index));
				}

				Ptr<WfExpression> CreateJsonArrayItemContent(const WString& arrayName, vint index, const WString& jsonType)
				{
					return CreateMember(CreateMember(CreateCast(CreateSharedType(jsonType), CreateJsonArrayItem(arrayName, index)), L"content"), L"value");
				}

				void AddRpcJsonTypesFromDeclarations(
					const List<Ptr<WfDeclaration>>& declarations,
					const WString& prefix,
					List<RpcJsonTypeModel>& enums,
					List<RpcJsonTypeModel>& structs)
				{
					for (auto declaration : declarations)
					{
						auto fullName = prefix == L"" ? declaration->name.value : prefix + L"::" + declaration->name.value;
						if (auto namespaceDecl = declaration.Cast<WfNamespaceDeclaration>())
						{
							AddRpcJsonTypesFromDeclarations(namespaceDecl->declarations, fullName, enums, structs);
						}
						else if (declaration.Cast<WfEnumDeclaration>())
						{
							RpcJsonTypeModel model;
							model.fullName = fullName;
							model.type = CreateTopQualifiedType(fullName);
							enums.Add(model);
						}
						else if (auto structDecl = declaration.Cast<WfStructDeclaration>())
						{
							RpcJsonTypeModel model;
							model.fullName = fullName;
							model.type = CreateTopQualifiedType(fullName);
							model.fields = Ptr(new List<RpcJsonFieldModel>);
							for (auto member : structDecl->members)
							{
								RpcJsonFieldModel field;
								field.name = member->name.value;
								field.type = CopyType(member->type.Obj());
								model.fields->Add(field);
							}
							structs.Add(model);
						}
					}
				}

				void AddRpcObjectReferenceJsonType(List<RpcJsonTypeModel>& structs)
				{
					auto fullName = WString::Unmanaged(L"system::RpcObjectReference");
					for (auto&& model : structs)
					{
						if (model.fullName == fullName)
						{
							return;
						}
					}

					auto td = GetTypeDescriptor<vl::rpc_controller::RpcObjectReference>();
					if (!td)
					{
						return;
					}

					RpcJsonTypeModel model;
					model.fullName = fullName;
					model.type = CreateTopQualifiedType(fullName);
					model.fields = Ptr(new List<RpcJsonFieldModel>);
					for (vint i = 0; i < td->GetPropertyCount(); i++)
					{
						auto propertyInfo = td->GetProperty(i);
						RpcJsonFieldModel field;
						field.name = propertyInfo->GetName();
						field.type = GetTypeFromTypeInfo(propertyInfo->GetReturn());
						model.fields->Add(field);
					}
					structs.Add(model);
				}

				void CollectRpcJsonTypes(WfLexicalScopeManager* manager, List<RpcJsonTypeModel>& enums, List<RpcJsonTypeModel>& structs)
				{
					if (manager->rpcMetadata && manager->rpcMetadata->metadataModule)
					{
						AddRpcJsonTypesFromDeclarations(manager->rpcMetadata->metadataModule->declarations, L"", enums, structs);
					}
					AddRpcObjectReferenceJsonType(structs);
				}

				void CollectRpcJsonPrimitives(List<RpcJsonPrimitiveModel>& primitives)
				{
					auto add = [&](const wchar_t* fullName, const wchar_t* keyword, RpcJsonPrimitiveKind kind)
					{
						RpcJsonPrimitiveModel model;
						model.fullName = WString::Unmanaged(fullName);
						model.keyword = WString::Unmanaged(keyword);
						model.kind = kind;
						primitives.Add(model);
					};

					add(L"system::UInt8", L"UInt8", RpcJsonPrimitiveKind::Number);
					add(L"system::UInt16", L"UInt16", RpcJsonPrimitiveKind::Number);
					add(L"system::UInt32", L"UInt32", RpcJsonPrimitiveKind::Number);
					add(L"system::UInt64", L"UInt64", RpcJsonPrimitiveKind::Number);
					add(L"system::Int8", L"Int8", RpcJsonPrimitiveKind::Number);
					add(L"system::Int16", L"Int16", RpcJsonPrimitiveKind::Number);
					add(L"system::Int32", L"Int32", RpcJsonPrimitiveKind::Number);
					add(L"system::Int64", L"Int64", RpcJsonPrimitiveKind::Number);
					add(L"system::Single", L"Single", RpcJsonPrimitiveKind::Number);
					add(L"system::Double", L"Double", RpcJsonPrimitiveKind::Number);
					add(L"system::Boolean", L"Boolean", RpcJsonPrimitiveKind::Boolean);
					add(L"system::Char", L"Char", RpcJsonPrimitiveKind::String);
					add(L"system::String", L"String", RpcJsonPrimitiveKind::String);
					add(L"system::DateTime", L"DateTime", RpcJsonPrimitiveKind::String);
					add(L"system::Locale", L"Locale", RpcJsonPrimitiveKind::String);
				}

				Ptr<WfStatement> CreateWhile(Ptr<WfExpression> condition, Ptr<WfStatement> body)
				{
					auto statement = Ptr(new WfWhileStatement);
					statement->condition = condition;
					statement->statement = body;
					return statement;
				}

				WString GetRpcJsonSerializeEnumFunctionName(const WString& fullName)
				{
					return L"rpcjson_Serialize_Enum_" + MangleRpcFullName(fullName);
				}

				WString GetRpcJsonDeserializeEnumFunctionName(const WString& fullName)
				{
					return L"rpcjson_Deserialize_Enum_" + MangleRpcFullName(fullName);
				}

				WString GetRpcJsonSerializeStructFunctionName(const WString& fullName)
				{
					return L"rpcjson_Serialize_Struct_" + MangleRpcFullName(fullName);
				}

				WString GetRpcJsonDeserializeStructFunctionName(const WString& fullName)
				{
					return L"rpcjson_Deserialize_Struct_" + MangleRpcFullName(fullName);
				}

				WString GetRpcJsonPredefinedFullName(WfLexicalScopeManager* manager, WfPredefinedTypeName name)
				{
					switch (name)
					{
					case WfPredefinedTypeName::Int:
						return manager && manager->cputdSInt ? manager->cputdSInt->GetTypeName() : WString::Unmanaged(L"system::Int64");
					case WfPredefinedTypeName::UInt:
						return manager && manager->cputdUInt ? manager->cputdUInt->GetTypeName() : WString::Unmanaged(L"system::UInt64");
					case WfPredefinedTypeName::Float:
						return L"system::Single";
					case WfPredefinedTypeName::Double:
						return L"system::Double";
					case WfPredefinedTypeName::String:
						return L"system::String";
					case WfPredefinedTypeName::Char:
						return L"system::Char";
					case WfPredefinedTypeName::Bool:
						return L"system::Boolean";
					case WfPredefinedTypeName::Object:
						return L"system::Object";
					case WfPredefinedTypeName::Interface:
						return L"system::Interface";
					default:
						return L"";
					}
				}

				WString GetRpcJsonTypeFullName(WfLexicalScopeManager* manager, WfType* type)
				{
					if (!type) return L"";
					if (auto predefined = dynamic_cast<WfPredefinedType*>(type))
					{
						return GetRpcJsonPredefinedFullName(manager, predefined->name);
					}
					if (auto top = dynamic_cast<WfTopQualifiedType*>(type))
					{
						return top->name.value;
					}
					if (auto reference = dynamic_cast<WfReferenceType*>(type))
					{
						return reference->name.value;
					}
					if (auto child = dynamic_cast<WfChildType*>(type))
					{
						auto parent = GetRpcJsonTypeFullName(manager, child->parent.Obj());
						return parent == L"" ? child->name.value : parent + L"::" + child->name.value;
					}
					if (auto raw = dynamic_cast<WfRawPointerType*>(type))
					{
						return GetRpcJsonTypeFullName(manager, raw->element.Obj());
					}
					if (auto shared = dynamic_cast<WfSharedPointerType*>(type))
					{
						return GetRpcJsonTypeFullName(manager, shared->element.Obj());
					}
					if (auto nullable = dynamic_cast<WfNullableType*>(type))
					{
						return GetRpcJsonTypeFullName(manager, nullable->element.Obj());
					}
					return L"";
				}

				RpcJsonPrimitiveModel* FindRpcJsonPrimitive(List<RpcJsonPrimitiveModel>& primitives, const WString& fullName)
				{
					for (vint i = 0; i < primitives.Count(); i++)
					{
						if (primitives[i].fullName == fullName)
						{
							return &primitives[i];
						}
					}
					return nullptr;
				}

				const RpcJsonTypeModel* FindRpcJsonType(const List<RpcJsonTypeModel>& types, const WString& fullName)
				{
					for (vint i = 0; i < types.Count(); i++)
					{
						if (types[i].fullName == fullName)
						{
							return &types[i];
						}
					}
					return nullptr;
				}

				struct RpcJsonGenerationContext
				{
					WfLexicalScopeManager*			manager = nullptr;
					List<RpcJsonPrimitiveModel>*	primitives = nullptr;
					List<RpcJsonTypeModel>*		enums = nullptr;
					List<RpcJsonTypeModel>*		structs = nullptr;
					vint						tempIndex = 0;
				};

				WString AllocateRpcJsonTemp(RpcJsonGenerationContext& context, const wchar_t* prefix)
				{
					return WString::Unmanaged(prefix) + itow(context.tempIndex++);
				}

				Ptr<WfExpression> CreateRpcJsonToken(Ptr<WfExpression> value)
				{
					auto constructor = CreateConstructor();
					constructor->arguments.Add(CreateConstructorArgument(CreateReference(L"value"), value));
					return constructor;
				}

				WString AddRpcJsonLiteral(Ptr<WfBlockStatement> block, RpcJsonGenerationContext& context, const WString& literal)
				{
					auto nodeName = AllocateRpcJsonTemp(context, L"jsonLiteral");
					AddStatement(block, CreateVariableStatement(nodeName, CreateSharedType(L"system::JsonLiteral"), CreateNewClass(CreateSharedType(L"system::JsonLiteral"))));
					AddStatement(block, CreateExpressionStatement(CreateAssign(CreateMember(CreateReference(nodeName), L"value"), CreateQualifiedExpression(L"system::JsonLiteralValue::" + literal))));
					return nodeName;
				}

				WString AddRpcJsonBooleanLiteral(Ptr<WfBlockStatement> block, RpcJsonGenerationContext& context, Ptr<WfExpression> value)
				{
					auto nodeName = AllocateRpcJsonTemp(context, L"jsonLiteral");
					AddStatement(block, CreateVariableStatement(nodeName, CreateSharedType(L"system::JsonLiteral"), CreateNewClass(CreateSharedType(L"system::JsonLiteral"))));
					AddStatement(block, CreateIf(
						value,
						CreateExpressionStatement(CreateAssign(CreateMember(CreateReference(nodeName), L"value"), CreateQualifiedExpression(L"system::JsonLiteralValue::True"))),
						CreateExpressionStatement(CreateAssign(CreateMember(CreateReference(nodeName), L"value"), CreateQualifiedExpression(L"system::JsonLiteralValue::False")))));
					return nodeName;
				}

				WString AddRpcJsonString(Ptr<WfBlockStatement> block, RpcJsonGenerationContext& context, Ptr<WfExpression> value)
				{
					auto nodeName = AllocateRpcJsonTemp(context, L"jsonString");
					AddStatement(block, CreateVariableStatement(nodeName, CreateSharedType(L"system::JsonString"), CreateNewClass(CreateSharedType(L"system::JsonString"))));
					AddStatement(block, CreateExpressionStatement(CreateAssign(CreateMember(CreateReference(nodeName), L"content"), CreateRpcJsonToken(value))));
					return nodeName;
				}

				WString AddRpcJsonNumber(Ptr<WfBlockStatement> block, RpcJsonGenerationContext& context, Ptr<WfExpression> value)
				{
					auto nodeName = AllocateRpcJsonTemp(context, L"jsonNumber");
					AddStatement(block, CreateVariableStatement(nodeName, CreateSharedType(L"system::JsonNumber"), CreateNewClass(CreateSharedType(L"system::JsonNumber"))));
					AddStatement(block, CreateExpressionStatement(CreateAssign(CreateMember(CreateReference(nodeName), L"content"), CreateRpcJsonToken(value))));
					return nodeName;
				}

				void AddRpcJsonArrayItem(Ptr<WfBlockStatement> block, const WString& arrayName, Ptr<WfExpression> value)
				{
					AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(CreateMember(CreateReference(arrayName), L"items"), L"Add"), value)));
				}

				void AddRpcJsonObjectField(Ptr<WfBlockStatement> block, RpcJsonGenerationContext& context, Ptr<WfExpression> object, const WString& fieldName, Ptr<WfExpression> value)
				{
					auto fieldVar = AllocateRpcJsonTemp(context, L"jsonField");
					AddStatement(block, CreateVariableStatement(fieldVar, CreateSharedType(L"system::JsonObjectField"), CreateNewClass(CreateSharedType(L"system::JsonObjectField"))));
					AddStatement(block, CreateExpressionStatement(CreateAssign(CreateMember(CreateReference(fieldVar), L"name"), CreateRpcJsonToken(CreateString(fieldName)))));
					AddStatement(block, CreateExpressionStatement(CreateAssign(CreateMember(CreateReference(fieldVar), L"value"), value)));
					AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(CreateMember(object, L"fields"), L"Add"), CreateReference(fieldVar))));
				}

				WString AddRpcJsonObjectFieldLookup(Ptr<WfBlockStatement> block, RpcJsonGenerationContext& context, Ptr<WfExpression> object, const WString& fieldName)
				{
					auto resultVar = AllocateRpcJsonTemp(context, L"jsonFieldValue");
					auto fieldVar = AllocateRpcJsonTemp(context, L"jsonField");
					AddStatement(block, CreateVariableStatement(resultVar, CreateSharedType(L"system::JsonNode"), CreateNull()));
					auto forBlock = CreateBlock();
					AddStatement(forBlock, CreateIf(
						CreateBinary(WfBinaryOperator::EQ, CreateMember(CreateMember(CreateReference(fieldVar), L"name"), L"value"), CreateString(fieldName)),
						CreateExpressionStatement(CreateAssign(CreateReference(resultVar), CreateMember(CreateReference(fieldVar), L"value")))));
					AddStatement(block, CreateForEach(fieldVar, CreateMember(object, L"fields"), forBlock));
					AddStatement(block, CreateIf(CreateIsNull(CreateReference(resultVar)), CreateRaise(L"JSON object field not found: " + fieldName)));
					return resultVar;
				}

				WString AddKnownRpcJsonSerializeValue(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, Ptr<WfExpression> value, WfType* type);
				WString AddKnownRpcJsonDeserializeValue(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, Ptr<WfExpression> node, WfType* type);

				Ptr<WfType> CreateWritableRpcJsonCollectionType(WfType* type)
				{
					if (auto enumerable = dynamic_cast<WfEnumerableType*>(type))
					{
						auto listType = Ptr(new WfMapType);
						listType->writability = WfMapWritability::Writable;
						listType->value = CopyType(enumerable->element.Obj());
						return listType;
					}
					if (auto map = dynamic_cast<WfMapType*>(type))
					{
						auto mapType = Ptr(new WfMapType);
						mapType->writability = WfMapWritability::Writable;
						mapType->key = map->key ? CopyType(map->key.Obj()) : nullptr;
						mapType->value = CopyType(map->value.Obj());
						return mapType;
					}
					if (auto observable = dynamic_cast<WfObservableListType*>(type))
					{
						auto listType = Ptr(new WfObservableListType);
						listType->element = CopyType(observable->element.Obj());
						return listType;
					}
					return CopyType(type);
				}

				WString AddKnownRpcJsonSerializeValue(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, Ptr<WfExpression> value, WfType* type)
				{
					if (auto nullable = dynamic_cast<WfNullableType*>(type))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonNode");
						AddStatement(block, CreateVariableStatement(resultName, CreateSharedType(L"system::JsonNode"), CreateNull()));
						auto nullBranch = CreateBlock();
						auto nullNode = AddRpcJsonLiteral(nullBranch, context, L"Null");
						AddStatement(nullBranch, CreateExpressionStatement(CreateAssign(CreateReference(resultName), CreateReference(nullNode))));
						auto valueBranch = CreateBlock();
						auto valueNode = AddKnownRpcJsonSerializeValue(context, valueBranch, CreateCast(CopyType(nullable->element.Obj()), CopyExpression(value, true)), nullable->element.Obj());
						AddStatement(valueBranch, CreateExpressionStatement(CreateAssign(CreateReference(resultName), CreateReference(valueNode))));
						AddStatement(block, CreateIf(CreateIsNull(value), nullBranch, valueBranch));
						return resultName;
					}

					auto fullName = GetRpcJsonTypeFullName(context.manager, type);
					if (auto primitive = FindRpcJsonPrimitive(*context.primitives, fullName))
					{
						switch (primitive->kind)
						{
						case RpcJsonPrimitiveKind::Number:
							return AddRpcJsonNumber(block, context, CreateCast(CreatePredefinedType(WfPredefinedTypeName::String), value));
						case RpcJsonPrimitiveKind::Boolean:
							return AddRpcJsonBooleanLiteral(block, context, value);
						case RpcJsonPrimitiveKind::String:
							return AddRpcJsonString(block, context, CreateCast(CreatePredefinedType(WfPredefinedTypeName::String), value));
						default:
							CHECK_FAIL(L"Internal error: Unknown primitive kind.");
						}
					}

					if (fullName == L"system::JsonNode")
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonNode");
						AddStatement(block, CreateVariableStatement(resultName, CreateSharedType(L"system::JsonNode"), value));
						return resultName;
					}

					if (fullName == L"system::Object" || fullName == L"system::Interface")
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonNode");
						AddStatement(block, CreateVariableStatement(resultName, CreateSharedType(L"system::JsonNode"), CreateCall(CreateReference(L"rpcjson_Serialize"), value)));
						return resultName;
					}

					if (FindRpcJsonType(*context.enums, fullName))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonNode");
						AddStatement(block, CreateVariableStatement(resultName, CreateSharedType(L"system::JsonNode"), CreateCall(CreateReference(GetRpcJsonSerializeEnumFunctionName(fullName)), value)));
						return resultName;
					}

					if (FindRpcJsonType(*context.structs, fullName))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonNode");
						AddStatement(block, CreateVariableStatement(resultName, CreateSharedType(L"system::JsonNode"), CreateCall(CreateReference(GetRpcJsonSerializeStructFunctionName(fullName)), value)));
						return resultName;
					}

					if (auto enumerable = dynamic_cast<WfEnumerableType*>(type))
					{
						auto arrayName = AllocateRpcJsonTemp(context, L"jsonArray");
						AddStatement(block, CreateVariableStatement(arrayName, CreateSharedType(L"system::JsonArray"), CreateNewClass(CreateSharedType(L"system::JsonArray"))));
						auto forBlock = CreateBlock();
						auto itemNode = AddKnownRpcJsonSerializeValue(context, forBlock, CreateReference(L"item"), enumerable->element.Obj());
						AddRpcJsonArrayItem(forBlock, arrayName, CreateReference(itemNode));
						AddStatement(block, CreateForEach(L"item", value, forBlock));
						return arrayName;
					}

					if (auto map = dynamic_cast<WfMapType*>(type))
					{
						auto arrayName = AllocateRpcJsonTemp(context, L"jsonArray");
						AddStatement(block, CreateVariableStatement(arrayName, CreateSharedType(L"system::JsonArray"), CreateNewClass(CreateSharedType(L"system::JsonArray"))));
						auto forBlock = CreateBlock();
						if (map->key)
						{
							auto pairName = AllocateRpcJsonTemp(context, L"jsonArray");
							AddStatement(forBlock, CreateVariableStatement(pairName, CreateSharedType(L"system::JsonArray"), CreateNewClass(CreateSharedType(L"system::JsonArray"))));
							auto keyNode = AddKnownRpcJsonSerializeValue(context, forBlock, CreateReference(L"key"), map->key.Obj());
							AddRpcJsonArrayItem(forBlock, pairName, CreateReference(keyNode));
							auto valueNode = AddKnownRpcJsonSerializeValue(context, forBlock, CreateIndex(value, CreateReference(L"key")), map->value.Obj());
							AddRpcJsonArrayItem(forBlock, pairName, CreateReference(valueNode));
							AddRpcJsonArrayItem(forBlock, arrayName, CreateReference(pairName));
							AddStatement(block, CreateForEach(L"key", CreateMember(value, L"Keys"), forBlock));
						}
						else
						{
							auto itemNode = AddKnownRpcJsonSerializeValue(context, forBlock, CreateReference(L"item"), map->value.Obj());
							AddRpcJsonArrayItem(forBlock, arrayName, CreateReference(itemNode));
							AddStatement(block, CreateForEach(L"item", value, forBlock));
						}
						return arrayName;
					}

					if (auto observable = dynamic_cast<WfObservableListType*>(type))
					{
						auto arrayName = AllocateRpcJsonTemp(context, L"jsonArray");
						AddStatement(block, CreateVariableStatement(arrayName, CreateSharedType(L"system::JsonArray"), CreateNewClass(CreateSharedType(L"system::JsonArray"))));
						auto forBlock = CreateBlock();
						auto itemNode = AddKnownRpcJsonSerializeValue(context, forBlock, CreateReference(L"item"), observable->element.Obj());
						AddRpcJsonArrayItem(forBlock, arrayName, CreateReference(itemNode));
						AddStatement(block, CreateForEach(L"item", value, forBlock));
						return arrayName;
					}

					auto resultName = AllocateRpcJsonTemp(context, L"jsonNode");
					AddStatement(block, CreateVariableStatement(resultName, CreateSharedType(L"system::JsonNode"), CreateCall(CreateReference(L"rpcjson_Serialize"), value)));
					return resultName;
				}

				WString AddKnownRpcJsonDeserializeValue(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, Ptr<WfExpression> node, WfType* type)
				{
					if (auto nullable = dynamic_cast<WfNullableType*>(type))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
						auto literalName = AllocateRpcJsonTemp(context, L"jsonLiteral");
						AddStatement(block, CreateVariableStatement(resultName, CopyType(type), CreateNull()));
						AddStatement(block, CreateVariableStatement(literalName, CreateSharedType(L"system::JsonLiteral"), CreateWeakCast(CreateSharedType(L"system::JsonLiteral"), node)));
						auto assignBranch = CreateBlock();
						auto valueName = AddKnownRpcJsonDeserializeValue(context, assignBranch, node, nullable->element.Obj());
						AddStatement(assignBranch, CreateExpressionStatement(CreateAssign(CreateReference(resultName), CreateReference(valueName))));
						AddStatement(block, CreateIf(
							CreateBinary(
								WfBinaryOperator::Or,
								CreateIsNull(CreateReference(literalName)),
								CreateBinary(WfBinaryOperator::NE, CreateMember(CreateReference(literalName), L"value"), CreateQualifiedExpression(L"system::JsonLiteralValue::Null"))),
							assignBranch));
						return resultName;
					}

					auto fullName = GetRpcJsonTypeFullName(context.manager, type);
					if (auto primitive = FindRpcJsonPrimitive(*context.primitives, fullName))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
						Ptr<WfExpression> value;
						switch (primitive->kind)
						{
						case RpcJsonPrimitiveKind::Number:
							value = CreateCast(CreateTopQualifiedType(fullName), CreateMember(CreateMember(CreateCast(CreateSharedType(L"system::JsonNumber"), node), L"content"), L"value"));
							break;
						case RpcJsonPrimitiveKind::Boolean:
							value = CreateBinary(WfBinaryOperator::EQ, CreateMember(CreateCast(CreateSharedType(L"system::JsonLiteral"), node), L"value"), CreateQualifiedExpression(L"system::JsonLiteralValue::True"));
							break;
						case RpcJsonPrimitiveKind::String:
							value = CreateCast(CreateTopQualifiedType(fullName), CreateMember(CreateMember(CreateCast(CreateSharedType(L"system::JsonString"), node), L"content"), L"value"));
							break;
						default:
							CHECK_FAIL(L"Internal error: Unknown primitive kind.");
						}
						AddStatement(block, CreateVariableStatement(resultName, CopyType(type), value));
						return resultName;
					}

					if (fullName == L"system::JsonNode")
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
						AddStatement(block, CreateVariableStatement(resultName, CopyType(type), node));
						return resultName;
					}

					if (fullName == L"system::Object" || fullName == L"system::Interface")
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
						AddStatement(block, CreateVariableStatement(resultName, CopyType(type), CreateCall(CreateReference(L"rpcjson_Deserialize"), node)));
						return resultName;
					}

					if (FindRpcJsonType(*context.enums, fullName))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
						AddStatement(block, CreateVariableStatement(resultName, CopyType(type), CreateCall(CreateReference(GetRpcJsonDeserializeEnumFunctionName(fullName)), node)));
						return resultName;
					}

					if (FindRpcJsonType(*context.structs, fullName))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
						AddStatement(block, CreateVariableStatement(resultName, CopyType(type), CreateCall(CreateReference(GetRpcJsonDeserializeStructFunctionName(fullName)), node)));
						return resultName;
					}

					if (auto enumerable = dynamic_cast<WfEnumerableType*>(type))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
						auto arrayName = AllocateRpcJsonTemp(context, L"jsonArray");
						AddStatement(block, CreateVariableStatement(resultName, CreateWritableRpcJsonCollectionType(type), CreateConstructor()));
						AddStatement(block, CreateVariableStatement(arrayName, CreateSharedType(L"system::JsonArray"), CreateCast(CreateSharedType(L"system::JsonArray"), node)));
						auto forBlock = CreateBlock();
						auto itemValue = AddKnownRpcJsonDeserializeValue(context, forBlock, CreateReference(L"item"), enumerable->element.Obj());
						AddStatement(forBlock, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(resultName), L"Add"), CreateReference(itemValue))));
						AddStatement(block, CreateForEach(L"item", CreateMember(CreateReference(arrayName), L"items"), forBlock));
						return resultName;
					}

					if (auto map = dynamic_cast<WfMapType*>(type))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
						auto arrayName = AllocateRpcJsonTemp(context, L"jsonArray");
						AddStatement(block, CreateVariableStatement(resultName, CreateWritableRpcJsonCollectionType(type), CreateConstructor()));
						AddStatement(block, CreateVariableStatement(arrayName, CreateSharedType(L"system::JsonArray"), CreateCast(CreateSharedType(L"system::JsonArray"), node)));
						auto forBlock = CreateBlock();
						if (map->key)
						{
							auto pairName = AllocateRpcJsonTemp(context, L"jsonArray");
							AddStatement(forBlock, CreateVariableStatement(pairName, CreateSharedType(L"system::JsonArray"), CreateCast(CreateSharedType(L"system::JsonArray"), CreateReference(L"item"))));
							auto keyValue = AddKnownRpcJsonDeserializeValue(context, forBlock, CreateJsonArrayItem(pairName, 0), map->key.Obj());
							auto itemValue = AddKnownRpcJsonDeserializeValue(context, forBlock, CreateJsonArrayItem(pairName, 1), map->value.Obj());
							AddStatement(forBlock, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(resultName), L"Set"), CreateReference(keyValue), CreateReference(itemValue))));
						}
						else
						{
							auto itemValue = AddKnownRpcJsonDeserializeValue(context, forBlock, CreateReference(L"item"), map->value.Obj());
							AddStatement(forBlock, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(resultName), L"Add"), CreateReference(itemValue))));
						}
						AddStatement(block, CreateForEach(L"item", CreateMember(CreateReference(arrayName), L"items"), forBlock));
						return resultName;
					}

					if (auto observable = dynamic_cast<WfObservableListType*>(type))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
						auto arrayName = AllocateRpcJsonTemp(context, L"jsonArray");
						AddStatement(block, CreateVariableStatement(resultName, CreateWritableRpcJsonCollectionType(type), CreateConstructor()));
						AddStatement(block, CreateVariableStatement(arrayName, CreateSharedType(L"system::JsonArray"), CreateCast(CreateSharedType(L"system::JsonArray"), node)));
						auto forBlock = CreateBlock();
						auto itemValue = AddKnownRpcJsonDeserializeValue(context, forBlock, CreateReference(L"item"), observable->element.Obj());
						AddStatement(forBlock, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(resultName), L"Add"), CreateReference(itemValue))));
						AddStatement(block, CreateForEach(L"item", CreateMember(CreateReference(arrayName), L"items"), forBlock));
						return resultName;
					}

					auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
					AddStatement(block, CreateVariableStatement(resultName, CopyType(type), CreateCast(CopyType(type), CreateCall(CreateReference(L"rpcjson_Deserialize"), node))));
					return resultName;
				}

				void AddRpcJsonUnknownTupleReturn(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, const WString& keyword, Ptr<WfExpression> valueNode)
				{
					auto arrayName = AllocateRpcJsonTemp(context, L"jsonArray");
					auto keywordNode = AddRpcJsonString(block, context, CreateString(keyword));
					AddStatement(block, CreateVariableStatement(arrayName, CreateSharedType(L"system::JsonArray"), CreateNewClass(CreateSharedType(L"system::JsonArray"))));
					AddRpcJsonArrayItem(block, arrayName, CreateReference(keywordNode));
					AddRpcJsonArrayItem(block, arrayName, valueNode);
					AddStatement(block, CreateReturn(CreateReference(arrayName)));
				}

				Ptr<WfDeclaration> GenerateRpcJsonSerializeEnum(RpcJsonGenerationContext& context, const RpcJsonTypeModel& enumModel)
				{
					auto functionDecl = CreateFunctionDeclaration(GetRpcJsonSerializeEnumFunctionName(enumModel.fullName), CreateSharedType(L"system::JsonNode"), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"value", CopyType(enumModel.type.Obj())));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();
					auto nodeName = AddRpcJsonNumber(block, context, CreateCast(
						CreatePredefinedType(WfPredefinedTypeName::String),
						CreateCast(CreateQualifiedType(L"system::UInt64"), CreateReference(L"value"))));
					AddStatement(block, CreateReturn(CreateReference(nodeName)));
					return functionDecl;
				}

				Ptr<WfDeclaration> GenerateRpcJsonDeserializeEnum(RpcJsonGenerationContext& context, const RpcJsonTypeModel& enumModel)
				{
					auto functionDecl = CreateFunctionDeclaration(GetRpcJsonDeserializeEnumFunctionName(enumModel.fullName), CopyType(enumModel.type.Obj()), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"node", CreateSharedType(L"system::JsonNode")));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();
					AddStatement(block, CreateReturn(CreateCast(
						CopyType(enumModel.type.Obj()),
						CreateCast(CreateQualifiedType(L"system::UInt64"), CreateMember(CreateMember(CreateCast(CreateSharedType(L"system::JsonNumber"), CreateReference(L"node")), L"content"), L"value")))));
					return functionDecl;
				}

				void AddRpcJsonStructFields(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, const RpcJsonTypeModel& structModel, const WString& objectName, const WString& valueName)
				{
					for (auto&& field : *structModel.fields.Obj())
					{
						auto fieldNode = AddKnownRpcJsonSerializeValue(context, block, CreateMember(CreateReference(valueName), field.name), field.type.Obj());
						AddRpcJsonObjectField(block, context, CreateReference(objectName), field.name, CreateReference(fieldNode));
					}
				}

				Ptr<WfDeclaration> GenerateRpcJsonSerializeStruct(RpcJsonGenerationContext& context, const RpcJsonTypeModel& structModel)
				{
					auto functionDecl = CreateFunctionDeclaration(GetRpcJsonSerializeStructFunctionName(structModel.fullName), CreateSharedType(L"system::JsonNode"), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"value", CopyType(structModel.type.Obj())));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();
					AddStatement(block, CreateVariableStatement(L"object", CreateSharedType(L"system::JsonObject"), CreateNewClass(CreateSharedType(L"system::JsonObject"))));
					AddRpcJsonStructFields(context, block, structModel, L"object", L"value");
					AddStatement(block, CreateReturn(CreateReference(L"object")));
					return functionDecl;
				}

				Ptr<WfDeclaration> GenerateRpcJsonDeserializeStruct(RpcJsonGenerationContext& context, const RpcJsonTypeModel& structModel)
				{
					auto functionDecl = CreateFunctionDeclaration(GetRpcJsonDeserializeStructFunctionName(structModel.fullName), CopyType(structModel.type.Obj()), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"node", CreateSharedType(L"system::JsonNode")));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();
					AddStatement(block, CreateVariableStatement(L"object", CreateSharedType(L"system::JsonObject"), CreateCast(CreateSharedType(L"system::JsonObject"), CreateReference(L"node"))));
					auto constructor = CreateConstructor();
					for (auto&& field : *structModel.fields.Obj())
					{
						auto fieldNode = AddRpcJsonObjectFieldLookup(block, context, CreateReference(L"object"), field.name);
						auto fieldValue = AddKnownRpcJsonDeserializeValue(context, block, CreateReference(fieldNode), field.type.Obj());
						constructor->arguments.Add(CreateConstructorArgument(CreateReference(field.name), CreateReference(fieldValue)));
					}
					AddStatement(block, CreateReturn(CreateInfer(constructor, CopyType(structModel.type.Obj()))));
					return functionDecl;
				}

				void AddUnknownSerializeCollectionCase(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, const WString& varName, const WString& typeFullName, const WString& keyword)
				{
					AddStatement(block, CreateVariableStatement(varName, CreateSharedType(typeFullName), CreateWeakCast(CreateSharedType(typeFullName), CreateReference(L"value"))));
					auto trueBranch = CreateBlock();
					AddStatement(trueBranch, CreateVariableStatement(L"object", CreateSharedType(L"system::JsonObject"), CreateNewClass(CreateSharedType(L"system::JsonObject"))));
					auto keywordNode = AddRpcJsonString(trueBranch, context, CreateString(keyword));
					AddRpcJsonObjectField(trueBranch, context, CreateReference(L"object"), L"$", CreateReference(keywordNode));
					AddStatement(trueBranch, CreateVariableStatement(L"values", CreateSharedType(L"system::JsonArray"), CreateNewClass(CreateSharedType(L"system::JsonArray"))));
					auto forBlock = CreateBlock();
					AddRpcJsonArrayItem(forBlock, L"values", CreateCall(CreateReference(L"rpcjson_Serialize"), CreateReference(L"item")));
					AddStatement(trueBranch, CreateForEach(L"item", CreateReference(varName), forBlock));
					AddRpcJsonObjectField(trueBranch, context, CreateReference(L"object"), L"values", CreateReference(L"values"));
					AddStatement(trueBranch, CreateReturn(CreateReference(L"object")));
					AddStatement(block, CreateIf(CreateIsNotNull(CreateReference(varName)), trueBranch));
				}

				void AddUnknownSerializeMapCase(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, const WString& varName, const WString& typeFullName)
				{
					AddStatement(block, CreateVariableStatement(varName, CreateSharedType(typeFullName), CreateWeakCast(CreateSharedType(typeFullName), CreateReference(L"value"))));
					auto trueBranch = CreateBlock();
					AddStatement(trueBranch, CreateVariableStatement(L"object", CreateSharedType(L"system::JsonObject"), CreateNewClass(CreateSharedType(L"system::JsonObject"))));
					auto keywordNode = AddRpcJsonString(trueBranch, context, CreateString(L"map"));
					AddRpcJsonObjectField(trueBranch, context, CreateReference(L"object"), L"$", CreateReference(keywordNode));
					AddStatement(trueBranch, CreateVariableStatement(L"values", CreateSharedType(L"system::JsonArray"), CreateNewClass(CreateSharedType(L"system::JsonArray"))));
					auto forBlock = CreateBlock();
					AddRpcJsonArrayItem(forBlock, L"values", CreateCall(CreateReference(L"rpcjson_Serialize"), CreateReference(L"key")));
					AddRpcJsonArrayItem(forBlock, L"values", CreateCall(CreateReference(L"rpcjson_Serialize"), CreateCall(CreateMember(CreateReference(varName), L"Get"), CreateReference(L"key"))));
					AddStatement(trueBranch, CreateForEach(L"key", CreateMember(CreateReference(varName), L"Keys"), forBlock));
					AddRpcJsonObjectField(trueBranch, context, CreateReference(L"object"), L"values", CreateReference(L"values"));
					AddStatement(trueBranch, CreateReturn(CreateReference(L"object")));
					AddStatement(block, CreateIf(CreateIsNotNull(CreateReference(varName)), trueBranch));
				}

				void AddUnknownSerializePrimitiveCase(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, const RpcJsonPrimitiveModel& primitive)
				{
					if (primitive.kind == RpcJsonPrimitiveKind::Boolean || primitive.fullName == L"system::String")
					{
						return;
					}

					auto varName = L"value_" + MangleRpcFullName(primitive.fullName);
					auto nullableType = CreateNullableType(primitive.fullName);
					AddStatement(block, CreateVariableStatement(varName, CopyType(nullableType.Obj()), CreateWeakCast(nullableType, CreateReference(L"value"))));
					auto trueBranch = CreateBlock();
					auto varValue = CreateCast(CreateTopQualifiedType(primitive.fullName), CreateReference(varName));
					WString nodeName;
					switch (primitive.kind)
					{
					case RpcJsonPrimitiveKind::Number:
						nodeName = AddRpcJsonNumber(trueBranch, context, CreateCast(CreatePredefinedType(WfPredefinedTypeName::String), varValue));
						break;
					case RpcJsonPrimitiveKind::String:
						nodeName = AddRpcJsonString(trueBranch, context, CreateCast(CreatePredefinedType(WfPredefinedTypeName::String), varValue));
						break;
					default:
						CHECK_FAIL(L"Internal error: Unexpected primitive kind.");
					}
					AddRpcJsonUnknownTupleReturn(context, trueBranch, primitive.keyword, CreateReference(nodeName));
					AddStatement(block, CreateIf(CreateIsNotNull(CreateReference(varName)), trueBranch));
				}

				void AddUnknownSerializeEnumCase(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, const RpcJsonTypeModel& enumModel)
				{
					auto varName = L"value_" + MangleRpcFullName(enumModel.fullName);
					auto nullableType = Ptr(new WfNullableType);
					nullableType->element = CopyType(enumModel.type.Obj());
					AddStatement(block, CreateVariableStatement(varName, CopyType(nullableType.Obj()), CreateWeakCast(nullableType, CreateReference(L"value"))));
					auto trueBranch = CreateBlock();
					auto valueNode = AllocateRpcJsonTemp(context, L"jsonNode");
					AddStatement(trueBranch, CreateVariableStatement(valueNode, CreateSharedType(L"system::JsonNode"), CreateCall(CreateReference(GetRpcJsonSerializeEnumFunctionName(enumModel.fullName)), CreateCast(CopyType(enumModel.type.Obj()), CreateReference(varName)))));
					AddRpcJsonUnknownTupleReturn(context, trueBranch, enumModel.fullName, CreateReference(valueNode));
					AddStatement(block, CreateIf(CreateIsNotNull(CreateReference(varName)), trueBranch));
				}

				void AddUnknownSerializeStructCase(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, const RpcJsonTypeModel& structModel)
				{
					auto varName = L"value_" + MangleRpcFullName(structModel.fullName);
					auto nullableType = Ptr(new WfNullableType);
					nullableType->element = CopyType(structModel.type.Obj());
					AddStatement(block, CreateVariableStatement(varName, CopyType(nullableType.Obj()), CreateWeakCast(nullableType, CreateReference(L"value"))));
					auto trueBranch = CreateBlock();
					AddStatement(trueBranch, CreateVariableStatement(L"object", CreateSharedType(L"system::JsonObject"), CreateNewClass(CreateSharedType(L"system::JsonObject"))));
					auto keywordNode = AddRpcJsonString(trueBranch, context, CreateString(structModel.fullName));
					AddRpcJsonObjectField(trueBranch, context, CreateReference(L"object"), L"$", CreateReference(keywordNode));
					auto varValueName = AllocateRpcJsonTemp(context, L"value");
					AddStatement(trueBranch, CreateVariableStatement(varValueName, CopyType(structModel.type.Obj()), CreateCast(CopyType(structModel.type.Obj()), CreateReference(varName))));
					AddRpcJsonStructFields(context, trueBranch, structModel, L"object", varValueName);
					AddStatement(trueBranch, CreateReturn(CreateReference(L"object")));
					AddStatement(block, CreateIf(CreateIsNotNull(CreateReference(varName)), trueBranch));
				}

				Ptr<WfDeclaration> GenerateRpcJsonSerialize(RpcJsonGenerationContext& context)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpcjson_Serialize", CreateSharedType(L"system::JsonNode"), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"value", CreatePredefinedType(WfPredefinedTypeName::Object)));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();
					auto nullBranch = CreateBlock();
					auto nullNode = AddRpcJsonLiteral(nullBranch, context, L"Null");
					AddStatement(nullBranch, CreateReturn(CreateReference(nullNode)));
					AddStatement(block, CreateIf(CreateIsNull(CreateReference(L"value")), nullBranch));

					auto boolType = CreateNullableType(L"system::Boolean");
					AddStatement(block, CreateVariableStatement(L"value_system__Boolean", CopyType(boolType.Obj()), CreateWeakCast(boolType, CreateReference(L"value"))));
					auto boolBranch = CreateBlock();
					auto boolNode = AddRpcJsonBooleanLiteral(boolBranch, context, CreateCast(CreateTopQualifiedType(L"system::Boolean"), CreateReference(L"value_system__Boolean")));
					AddStatement(boolBranch, CreateReturn(CreateReference(boolNode)));
					AddStatement(block, CreateIf(CreateIsNotNull(CreateReference(L"value_system__Boolean")), boolBranch));

					auto stringType = CreateNullableType(L"system::String");
					AddStatement(block, CreateVariableStatement(L"value_system__String", CopyType(stringType.Obj()), CreateWeakCast(stringType, CreateReference(L"value"))));
					auto stringBranch = CreateBlock();
					auto stringNode = AddRpcJsonString(stringBranch, context, CreateCast(CreateTopQualifiedType(L"system::String"), CreateReference(L"value_system__String")));
					AddStatement(stringBranch, CreateReturn(CreateReference(stringNode)));
					AddStatement(block, CreateIf(CreateIsNotNull(CreateReference(L"value_system__String")), stringBranch));

					for (auto&& primitive : *context.primitives)
					{
						AddUnknownSerializePrimitiveCase(context, block, primitive);
					}
					AddUnknownSerializeCollectionCase(context, block, L"observableList", L"system::ObservableList", L"oblist");
					AddUnknownSerializeMapCase(context, block, L"dictionary", L"system::Dictionary");
					AddUnknownSerializeMapCase(context, block, L"readonlyDictionary", L"system::ReadonlyDictionary");
					AddUnknownSerializeCollectionCase(context, block, L"list", L"system::List", L"list");
					AddUnknownSerializeCollectionCase(context, block, L"readonlyList", L"system::ReadonlyList", L"list");
					AddUnknownSerializeCollectionCase(context, block, L"enumerable", L"system::Enumerable", L"list");

					for (auto&& enumModel : *context.enums)
					{
						AddUnknownSerializeEnumCase(context, block, enumModel);
					}
					for (auto&& structModel : *context.structs)
					{
						AddUnknownSerializeStructCase(context, block, structModel);
					}

					AddStatement(block, CreateRaise(L"Unsupported RPC JSON serialization value."));
					return functionDecl;
				}

				void AddUnknownDeserializePrimitiveCase(Ptr<WfSwitchStatement> switchStat, const RpcJsonPrimitiveModel& primitive)
				{
					if (primitive.kind == RpcJsonPrimitiveKind::Boolean || primitive.fullName == L"system::String")
					{
						return;
					}

					auto jsonType = primitive.kind == RpcJsonPrimitiveKind::Number ? L"system::JsonNumber" : L"system::JsonString";
					AddSwitchCase(
						switchStat,
						CreateString(primitive.keyword),
						CreateReturn(CreateCast(CreateTopQualifiedType(primitive.fullName), CreateJsonArrayItemContent(L"array", 1, jsonType))));
				}

				void AddUnknownDeserializeEnumCase(Ptr<WfSwitchStatement> switchStat, const RpcJsonTypeModel& enumModel)
				{
					AddSwitchCase(
						switchStat,
						CreateString(enumModel.fullName),
						CreateReturn(CreateCall(CreateReference(GetRpcJsonDeserializeEnumFunctionName(enumModel.fullName)), CreateJsonArrayItem(L"array", 1))));
				}

				void AddUnknownDeserializeStructCase(Ptr<WfSwitchStatement> switchStat, const RpcJsonTypeModel& structModel)
				{
					AddSwitchCase(
						switchStat,
						CreateString(structModel.fullName),
						CreateReturn(CreateCall(CreateReference(GetRpcJsonDeserializeStructFunctionName(structModel.fullName)), CreateReference(L"object"))));
				}

				Ptr<WfStatement> CreateUnknownDeserializeListCase(RpcJsonGenerationContext& context, const WString& variableType)
				{
					auto block = CreateBlock();
					AddStatement(block, CreateVariableStatement(L"result", CreateSharedType(variableType), CreateConstructor()));
					auto valuesNode = AddRpcJsonObjectFieldLookup(block, context, CreateReference(L"object"), L"values");
					AddStatement(block, CreateVariableStatement(L"values", CreateSharedType(L"system::JsonArray"), CreateCast(CreateSharedType(L"system::JsonArray"), CreateReference(valuesNode))));
					auto forBlock = CreateBlock();
					AddStatement(forBlock, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"result"), L"Add"), CreateCall(CreateReference(L"rpcjson_Deserialize"), CreateReference(L"item")))));
					AddStatement(block, CreateForEach(L"item", CreateMember(CreateReference(L"values"), L"items"), forBlock));
					AddStatement(block, CreateReturn(CreateReference(L"result")));
					return block;
				}

				Ptr<WfStatement> CreateUnknownDeserializeMapCase(RpcJsonGenerationContext& context)
				{
					auto block = CreateBlock();
					AddStatement(block, CreateVariableStatement(L"result", CreateSharedType(L"system::Dictionary"), CreateConstructor()));
					auto valuesNode = AddRpcJsonObjectFieldLookup(block, context, CreateReference(L"object"), L"values");
					AddStatement(block, CreateVariableStatement(L"values", CreateSharedType(L"system::JsonArray"), CreateCast(CreateSharedType(L"system::JsonArray"), CreateReference(valuesNode))));
					AddStatement(block, CreateVariableStatement(L"index", CreatePredefinedType(WfPredefinedTypeName::Int), CreateInt(0)));
					auto whileBlock = CreateBlock();
					AddStatement(whileBlock, CreateExpressionStatement(CreateCall(
						CreateMember(CreateReference(L"result"), L"Set"),
						CreateCall(CreateReference(L"rpcjson_Deserialize"), CreateIndex(CreateMember(CreateReference(L"values"), L"items"), CreateReference(L"index"))),
						CreateCall(CreateReference(L"rpcjson_Deserialize"), CreateIndex(CreateMember(CreateReference(L"values"), L"items"), CreateBinary(WfBinaryOperator::Add, CreateReference(L"index"), CreateInt(1)))))));
					AddStatement(whileBlock, CreateExpressionStatement(CreateAssign(CreateReference(L"index"), CreateBinary(WfBinaryOperator::Add, CreateReference(L"index"), CreateInt(2)))));
					AddStatement(block, CreateWhile(CreateBinary(WfBinaryOperator::LT, CreateReference(L"index"), CreateMember(CreateMember(CreateReference(L"values"), L"items"), L"Count")), whileBlock));
					AddStatement(block, CreateReturn(CreateReference(L"result")));
					return block;
				}

				Ptr<WfDeclaration> GenerateRpcJsonDeserialize(RpcJsonGenerationContext& context)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpcjson_Deserialize", CreatePredefinedType(WfPredefinedTypeName::Object), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"node", CreateSharedType(L"system::JsonNode")));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();
					AddStatement(block, CreateIf(CreateIsNull(CreateReference(L"node")), CreateReturn(CreateNull())));

					AddStatement(block, CreateVariableStatement(L"literal", CreateSharedType(L"system::JsonLiteral"), CreateWeakCast(CreateSharedType(L"system::JsonLiteral"), CreateReference(L"node"))));
					auto literalBranch = CreateBlock();
					AddStatement(literalBranch, CreateIf(
						CreateBinary(WfBinaryOperator::EQ, CreateMember(CreateReference(L"literal"), L"value"), CreateQualifiedExpression(L"system::JsonLiteralValue::Null")),
						CreateReturn(CreateNull())));
					AddStatement(literalBranch, CreateIf(
						CreateBinary(WfBinaryOperator::EQ, CreateMember(CreateReference(L"literal"), L"value"), CreateQualifiedExpression(L"system::JsonLiteralValue::True")),
						CreateReturn(CreateBool(true))));
					AddStatement(literalBranch, CreateIf(
						CreateBinary(WfBinaryOperator::EQ, CreateMember(CreateReference(L"literal"), L"value"), CreateQualifiedExpression(L"system::JsonLiteralValue::False")),
						CreateReturn(CreateBool(false))));
					AddStatement(literalBranch, CreateRaise(L"Unsupported JSON literal."));
					AddStatement(block, CreateIf(CreateIsNotNull(CreateReference(L"literal")), literalBranch));

					AddStatement(block, CreateVariableStatement(L"stringNode", CreateSharedType(L"system::JsonString"), CreateWeakCast(CreateSharedType(L"system::JsonString"), CreateReference(L"node"))));
					AddStatement(block, CreateIf(CreateIsNotNull(CreateReference(L"stringNode")), CreateReturn(CreateMember(CreateMember(CreateReference(L"stringNode"), L"content"), L"value"))));

					AddStatement(block, CreateVariableStatement(L"array", CreateSharedType(L"system::JsonArray"), CreateWeakCast(CreateSharedType(L"system::JsonArray"), CreateReference(L"node"))));
					auto arrayBranch = CreateBlock();
					AddStatement(arrayBranch, CreateVariableStatement(L"keyword", CreatePredefinedType(WfPredefinedTypeName::String), CreateJsonArrayItemContent(L"array", 0, L"system::JsonString")));
					auto arraySwitch = Ptr(new WfSwitchStatement);
					arraySwitch->expression = CreateReference(L"keyword");
					arraySwitch->defaultBranch = CreateRaise(L"Unknown RPC JSON array schema.");
					for (auto&& primitive : *context.primitives)
					{
						AddUnknownDeserializePrimitiveCase(arraySwitch, primitive);
					}
					for (auto&& enumModel : *context.enums)
					{
						AddUnknownDeserializeEnumCase(arraySwitch, enumModel);
					}
					AddStatement(arrayBranch, arraySwitch);
					AddStatement(block, CreateIf(CreateIsNotNull(CreateReference(L"array")), arrayBranch));

					AddStatement(block, CreateVariableStatement(L"object", CreateSharedType(L"system::JsonObject"), CreateWeakCast(CreateSharedType(L"system::JsonObject"), CreateReference(L"node"))));
					auto objectBranch = CreateBlock();
					auto keywordNode = AddRpcJsonObjectFieldLookup(objectBranch, context, CreateReference(L"object"), L"$");
					AddStatement(objectBranch, CreateVariableStatement(L"keyword", CreatePredefinedType(WfPredefinedTypeName::String), CreateMember(CreateMember(CreateCast(CreateSharedType(L"system::JsonString"), CreateReference(keywordNode)), L"content"), L"value")));
					auto objectSwitch = Ptr(new WfSwitchStatement);
					objectSwitch->expression = CreateReference(L"keyword");
					objectSwitch->defaultBranch = CreateRaise(L"Unknown RPC JSON object schema.");
					AddSwitchCase(objectSwitch, CreateString(L"list"), CreateUnknownDeserializeListCase(context, L"system::List"));
					AddSwitchCase(objectSwitch, CreateString(L"oblist"), CreateUnknownDeserializeListCase(context, L"system::ObservableList"));
					AddSwitchCase(objectSwitch, CreateString(L"map"), CreateUnknownDeserializeMapCase(context));
					for (auto&& structModel : *context.structs)
					{
						AddUnknownDeserializeStructCase(objectSwitch, structModel);
					}
					AddStatement(objectBranch, objectSwitch);
					AddStatement(block, CreateIf(CreateIsNotNull(CreateReference(L"object")), objectBranch));

					AddStatement(block, CreateRaise(L"Unsupported RPC JSON node."));
					return functionDecl;
				}

				void AddRpcJsonDeclarations(WfLexicalScopeManager* manager, Ptr<WfModule> module)
				{
					List<RpcJsonPrimitiveModel> primitives;
					List<RpcJsonTypeModel> enums;
					List<RpcJsonTypeModel> structs;
					CollectRpcJsonPrimitives(primitives);
					CollectRpcJsonTypes(manager, enums, structs);

					RpcJsonGenerationContext context;
					context.manager = manager;
					context.primitives = &primitives;
					context.enums = &enums;
					context.structs = &structs;

					for (auto&& enumModel : enums)
					{
						module->declarations.Add(GenerateRpcJsonSerializeEnum(context, enumModel));
						module->declarations.Add(GenerateRpcJsonDeserializeEnum(context, enumModel));
					}
					for (auto&& structModel : structs)
					{
						module->declarations.Add(GenerateRpcJsonSerializeStruct(context, structModel));
						module->declarations.Add(GenerateRpcJsonDeserializeStruct(context, structModel));
					}
					module->declarations.Add(GenerateRpcJsonSerialize(context));
					module->declarations.Add(GenerateRpcJsonDeserialize(context));
				}

				Ptr<WfDeclaration> GenerateWrapperDispatcher(const List<RpcInterfaceModel>& interfaces)
				{
					auto returnType = Ptr(new WfSharedPointerType);
					returnType->element = CreateQualifiedType(L"system::IRpcWrapperBase");
					auto functionDecl = CreateFunctionDeclaration(L"rpcwrapper_Create", returnType, WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"ref", CreateQualifiedType(L"system::RpcObjectReference")));
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateRawType(L"system::IRpcLifecycle")));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();

					auto switchStat = Ptr(new WfSwitchStatement);
					switchStat->expression = CreateMember(CreateReference(L"ref"), L"typeId");
					switchStat->defaultBranch = CreateRaise(L"Unknown RPC type id for wrapper creation.");

					for (auto&& interfaceModel : interfaces)
					{
						auto switchCase = Ptr(new WfSwitchCase);
						switchCase->expression = CreateRpcConstantReference(L"rpctype_", interfaceModel.fullName);
						auto caseBranch = CreateBlock();
						auto mangledName = MangleRpcFullName(interfaceModel.fullName);
						AddStatement(caseBranch, CreateReturn(CreateCall(CreateReference(L"rpcwrapper_" + mangledName), CreateReference(L"lc"), CreateReference(L"ref"))));
						switchCase->statement = caseBranch;
						switchStat->caseBranches.Add(switchCase);
					}

					AddStatement(block, switchStat);
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
				if (manager->errors.Count() > 0)
				{
					return nullptr;
				}
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

				{
					auto getIds = CreateFunctionDeclaration(L"rpc_GetIds", CreateMapType(CreatePredefinedType(WfPredefinedTypeName::String), CreatePredefinedType(WfPredefinedTypeName::Int)), WfFunctionKind::Normal);
					auto block = getIds->statement.Cast<WfBlockStatement>();
					AddStatement(block, CreateVariableStatement(L"result", CreateMapType(CreatePredefinedType(WfPredefinedTypeName::String), CreatePredefinedType(WfPredefinedTypeName::Int)), CreateConstructor()));
					id = 0;
					for (auto fullName : manager->rpcMetadata->typeFullNames)
					{
						AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"result"), L"Set"), CreateString(fullName), CreateInt(id++))));
					}
					for (auto fullName : manager->rpcMetadata->methodFullNames)
					{
						AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"result"), L"Set"), CreateString(fullName), CreateInt(id++))));
					}
					for (auto fullName : manager->rpcMetadata->eventFullNames)
					{
						AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"result"), L"Set"), CreateString(fullName), CreateInt(id++))));
					}
					AddStatement(block, CreateReturn(CreateReference(L"result")));
					module->declarations.Add(getIds);
				}

				AddRpcJsonDeclarations(manager, module);

				module->declarations.Add(GenerateIsInterfaceTypeIdHelper(interfaces));
				module->declarations.Add(GenerateIsCtorInterfaceTypeIdHelper(interfaces));
				module->declarations.Add(GenerateObjectOpsFactory(manager, interfaces));
				module->declarations.Add(GenerateObjectEventOpsFactory(manager, interfaces));

				List<Ptr<WfDeclaration>> wrapperDeclarations;
				Dictionary<WString, Ptr<WfNamespaceDeclaration>> wrapperNamespaces;
				for (auto&& interfaceModel : interfaces)
				{
					List<WString> namespaceFragments;
					SplitTypeFullName(interfaceModel.fullName, namespaceFragments);
					namespaceFragments.RemoveAt(namespaceFragments.Count() - 1);
					AddDeclarationToNamespaces(wrapperDeclarations, wrapperNamespaces, namespaceFragments, GenerateWrapperInterface(interfaceModel));
				}
				for (auto declaration : wrapperDeclarations)
				{
					module->declarations.Add(declaration);
				}

				vint listenerCount = 0;
				for (auto&& interfaceModel : interfaces)
				{
					if (auto listener = GenerateListenerFactory(interfaceModel, interfaces))
					{
						module->declarations.Add(listener);
						listenerCount++;
					}
				}
				if (listenerCount > 0)
				{
					module->declarations.Add(GenerateListenerDispatcher(interfaces));
				}

				for (auto&& interfaceModel : interfaces)
				{
					module->declarations.Add(GenerateWrapperFactory(interfaceModel, interfaces));
				}

				module->declarations.Add(GenerateWrapperDispatcher(interfaces));

				return module;
			}
		}
	}
}
