#include "WfAnalyzer_GenerateRpc.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;

			namespace rpc_generating
			{
				using namespace reflection::description;

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

				bool IsSharedInterfaceType(ITypeInfo* type)
				{
					if (!type || type->GetDecorator() != ITypeInfo::SharedPtr) return false;
					auto elementType = type->GetElementType();
					if (!elementType) return false;
					if (elementType->GetDecorator() == ITypeInfo::Generic)
					{
						elementType = elementType->GetElementType();
					}
					if (!elementType || elementType->GetDecorator() != ITypeInfo::TypeDescriptor) return false;
					auto td = elementType->GetTypeDescriptor();
					return td
						&& (td->GetTypeDescriptorFlags() & TypeDescriptorFlags::Interface) != TypeDescriptorFlags::Undefined;
				}

				IMethodInfo* FindRpcMethodInfo(WfLexicalScopeManager* manager, WfClassDeclaration* interfaceDecl, WfFunctionDeclaration* methodDecl, ITypeDescriptor* typeDescriptor)
				{
					if (auto index = manager->declarationMemberInfos.Keys().IndexOf(methodDecl); index != -1)
					{
						if (auto methodInfo = dynamic_cast<IMethodInfo*>(manager->declarationMemberInfos.Values()[index].Obj()))
						{
							return methodInfo;
						}
					}

					if (!typeDescriptor) return nullptr;
					auto group = typeDescriptor->GetMethodGroupByName(methodDecl->name.value, false);
					if (!group) return nullptr;

					vint overloadIndex = 0;
					for (auto declaration : interfaceDecl->declarations)
					{
						if (declaration.Obj() == methodDecl)
						{
							break;
						}
						if (auto previousMethod = declaration.Cast<WfFunctionDeclaration>())
						{
							if (previousMethod->name.value == methodDecl->name.value)
							{
								overloadIndex++;
							}
						}
					}
					return overloadIndex < group->GetMethodCount() ? group->GetMethod(overloadIndex) : nullptr;
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

				Ptr<WfExpression> CreateLifecycleHelperCall(const wchar_t* helperName, Ptr<WfExpression> value, Ptr<WfExpression> lifecycle)
				{
					return CreateCall(CreateQualifiedExpression(L"system::IRpcLifecycle::" + WString::Unmanaged(helperName)), value, lifecycle);
				}

				Ptr<WfExpression> CreateRpcBoxExpression(ITypeInfo* typeInfo, bool byref, Ptr<WfExpression> value, Ptr<WfExpression> lifecycle)
				{
					if (IsSharedInterfaceType(typeInfo))
					{
						return CreateLifecycleHelperCall(byref ? L"RpcBoxByref" : L"RpcBoxByval", value, lifecycle);
					}
					return value;
				}

				Ptr<WfExpression> CreateRpcCachedPropertyInitialValue(const RpcPropertyModel& propertyModel)
				{
					if (!propertyModel.typeInfo)
					{
						return CreateNull();
					}

					switch (propertyModel.typeInfo->GetDecorator())
					{
					case ITypeInfo::RawPtr:
					case ITypeInfo::SharedPtr:
					case ITypeInfo::Nullable:
						return CreateNull();
					default:
						return CreateDefaultValue(propertyModel.typeInfo);
					}
				}

				Ptr<WfExpression> CreateRpcUnboxExpression(ITypeInfo* typeInfo, Ptr<WfType> type, bool byref, Ptr<WfExpression> value, Ptr<WfExpression> lifecycle)
				{
					Ptr<WfExpression> unboxed;
					if (IsSharedInterfaceType(typeInfo))
					{
						auto serializable = byref
							? CreateCast(CreateTypeFromCpp<rpc_controller::RpcObjectReference>(), value)
							: value;
						unboxed = CreateLifecycleHelperCall(byref ? L"RpcUnboxByref" : L"RpcUnboxByval", serializable, lifecycle);
						return IsRpcStrongTypedCollection(typeInfo) || IsStrongTypedCollectionType(type.Obj())
							? CreateCast(CopyType(type.Obj()), unboxed)
							: CreateWeakCast(CopyType(type.Obj()), unboxed);
					}
					else
					{
						unboxed = value;
					}
					return CreateCast(CopyType(type.Obj()), unboxed);
				}

				Ptr<WfExpression> CreateRpcCopyByvalExpression(Ptr<WfExpression> value, Ptr<WfExpression> lifecycle)
				{
					return CreateLifecycleHelperCall(L"RpcCopyByval", value, lifecycle);
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

				bool IsRpcByvalReturn(const RpcMethodModel& methodModel)
				{
					return !IsVoidType(methodModel.returnType.Obj())
						&& !methodModel.returnByref
						&& (IsStrongTypedCollectionType(methodModel.returnType.Obj()) || IsRpcStrongTypedCollection(methodModel.returnTypeInfo));
				}

				void AddRpcByvalReturnValue(Ptr<WfBlockStatement> block, Ptr<WfExpression> value, Ptr<WfExpression> copiedValue)
				{
					AddStatement(block, CreateInferredVariableStatement(L"byvalReturnValue", CreateNewClass(CreateTypeFromCpp<Ptr<rpc_controller::RpcByvalReturnValue>>())));
					AddStatement(block, CreateExpressionStatement(CreateAssign(CreateMember(CreateReference(L"byvalReturnValue"), L"value"), value)));
					AddStatement(block, CreateExpressionStatement(CreateAssign(CreateMember(CreateReference(L"byvalReturnValue"), L"slot"), CreateReference(L"_slot"))));
					AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"_byvalReturnValues"), L"Set"), CreateReference(L"_slot"), copiedValue)));
					AddStatement(block, CreateExpressionStatement(CreateAssign(CreateReference(L"_slot"), CreateBinary(WfBinaryOperator::Add, CreateReference(L"_slot"), CreateInt(1)))));
					AddStatement(block, CreateReturn(CreateReference(L"byvalReturnValue")));
				}

				Ptr<WfExpression> CreateRpcConstantReference(const wchar_t* prefix, const WString& fullName)
				{
					return CreateReference(WString::Unmanaged(prefix) + MangleRpcFullName(fullName));
				}

				vint GetRpcId(WfLexicalScopeManager* manager, const WString& fullName)
				{
					auto id = manager->rpcMetadata->orderedIds.IndexOf(fullName);
					CHECK_ERROR(id != -1, L"RPC metadata ID list does not contain a generated RPC full name.");
					return id;
				}

				const wchar_t* GetRpcConstantPrefix(WfLexicalScopeManager* manager, const WString& fullName)
				{
					if (manager->rpcMetadata->typeNames.Keys().Contains(fullName))
					{
						return L"rpctype_";
					}
					if (manager->rpcMetadata->methodNames.Keys().Contains(fullName))
					{
						return L"rpcmethod_";
					}
					if (manager->rpcMetadata->eventNames.Keys().Contains(fullName))
					{
						return L"rpcevent_";
					}
					CHECK_FAIL(L"RPC metadata ID list contains an unknown generated RPC full name.");
				}

				void AddIdLookupEntry(Ptr<WfConstructorExpression> expression, Ptr<WfExpression> key, Ptr<WfExpression> value)
				{
					expression->arguments.Add(CreateConstructorArgument(key, value));
				}

				void CollectMangledNames(WfLexicalScopeManager* manager)
				{
					Dictionary<WString, WString> mangledNames;
					for (auto fullName : manager->rpcMetadata->orderedIds)
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
						interfaceModel.typeId = GetRpcId(manager, typeFullName);
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
								methodModel.methodId = GetRpcId(manager, methodFullName);
								methodModel.returnType = CopyType(methodDecl->returnType.Obj());
								auto methodInfo = FindRpcMethodInfo(manager, interfaceDecl, methodDecl.Obj(), typeDescriptor);
								if (methodInfo)
								{
									methodModel.returnTypeInfo = methodInfo->GetReturn();
								}
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
									methodModel.returnTypeInfo = property.typeInfo;
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
									if (methodInfo && i < methodInfo->GetParameterCount())
									{
										paramModel.typeInfo = methodInfo->GetParameter(i)->GetType();
									}
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
										paramModel.typeInfo = property.typeInfo;
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
								eventModel.eventId = GetRpcId(manager, eventFullName);

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
									paramModel.typeInfo = handlerGenericType && genericArgumentIndex < handlerGenericType->GetGenericArgumentCount()
										? handlerGenericType->GetGenericArgument(genericArgumentIndex)
										: nullptr;
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
					AddStatement(block, CreateInferredVariableStatement(L"target", CreateCast(CreateSharedType(interfaceModel.fullName), CreateCall(CreateMember(CreateReference(L"_lc"), L"RefToPtr"), CreateReference(L"ref")))));

					List<Ptr<WfExpression>> arguments;
					for (vint i = 0; i < methodModel.params.Count(); i++)
					{
						auto&& paramModel = methodModel.params[i];
						auto argument = CreateIndex(CreateReference(L"arguments"), CreateInt(i));
						arguments.Add(CreateRpcUnboxExpression(paramModel.typeInfo, paramModel.type, paramModel.byref, argument, CreateReference(L"_lc")));
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
						if (IsRpcByvalReturn(methodModel))
						{
							AddStatement(block, CreateInferredVariableStatement(L"copiedReturnValue", CreateRpcCopyByvalExpression(invoke, CreateReference(L"_lc"))));
							AddStatement(block, CreateInferredVariableStatement(L"boxedReturnValue", CreateRpcBoxExpression(methodModel.returnTypeInfo, methodModel.returnByref, CreateReference(L"copiedReturnValue"), CreateReference(L"_lc"))));
							AddRpcByvalReturnValue(block, CreateReference(L"boxedReturnValue"), CreateReference(L"copiedReturnValue"));
						}
						else
						{
							AddStatement(block, CreateReturn(CreateRpcBoxExpression(methodModel.returnTypeInfo, methodModel.returnByref, invoke, CreateReference(L"_lc"))));
						}
					}
					return block;
				}

				Ptr<WfStatement> BuildInvokeEventBranch(const RpcInterfaceModel& interfaceModel, const RpcEventModel& eventModel)
				{
					auto block = CreateBlock();
					AddStatement(block, CreateInferredVariableStatement(L"target", CreateCast(CreateSharedType(interfaceModel.fullName), CreateCall(CreateMember(CreateReference(L"_lc"), L"RefToPtr"), CreateReference(L"ref")))));

					auto invoke = Ptr(new WfCallExpression);
					invoke->function = CreateMember(CreateReference(L"target"), eventModel.name);
					for (vint i = 0; i < eventModel.params.Count(); i++)
					{
						auto&& paramModel = eventModel.params[i];
						auto argument = CreateIndex(CreateReference(L"arguments"), CreateInt(i));
						invoke->arguments.Add(CreateRpcUnboxExpression(paramModel.typeInfo, paramModel.type, paramModel.byref, argument, CreateReference(L"_lc")));
					}
					AddStatement(block, CreateExpressionStatement(invoke));
					return block;
				}

				bool HasRpcEvents(const List<RpcInterfaceModel>& interfaces)
				{
					for (auto&& interfaceModel : interfaces)
					{
						if (interfaceModel.events.Count() > 0)
						{
							return true;
						}
					}
					return false;
				}

				Ptr<WfStatement> BuildDispatchChain(const List<RpcInterfaceModel>& interfaces, bool forEvent, const WString& unknownIdVariable = WString::Empty)
				{
					auto switchStat = Ptr(new WfSwitchStatement);
					switchStat->expression = CreateReference(forEvent ? L"eventId" : L"methodId");
					switchStat->defaultBranch =
						unknownIdVariable == L""
						? CreateRaise(forEvent ? L"Unknown RPC event id." : L"Unknown RPC method id.")
						: CreateExpressionStatement(CreateAssign(CreateReference(unknownIdVariable), CreateBool(true)));

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

				Ptr<WfDeclaration> GenerateObjectOpsFactory(const List<RpcInterfaceModel>& interfaces)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpcops_IRpcObjectOps", CreateTypeFromCpp<Ptr<rpc_controller::IRpcObjectOps>>(), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateTypeFromCpp<rpc_controller::IRpcLifecycle*>()));
					auto newOps = CreateNewInterface(CreateTypeFromCpp<Ptr<rpc_controller::IRpcObjectOps>>()).Cast<WfNewInterfaceExpression>();
					newOps->declarations.Add(CreateVariableDeclaration(L"_lc", CreateTypeFromCpp<rpc_controller::IRpcLifecycle*>(), CreateReference(L"lc")));
					newOps->declarations.Add(CreateVariableDeclaration(L"_slot", CreatePredefinedType(WfPredefinedTypeName::Int), CreateInt(0)));
					newOps->declarations.Add(CreateVariableDeclaration(L"_byvalReturnValues", CreateTypeFromCpp<Dictionary<vint, Value>>(), CreateConstructor()));

					{
						auto invokeMethod = CreateFunctionDeclaration(L"InvokeMethod", CreatePredefinedType(WfPredefinedTypeName::Object), WfFunctionKind::Override);
						invokeMethod->arguments.Add(CreateFunctionArgument(L"ref", CreateTypeFromCpp<rpc_controller::RpcObjectReference>()));
						invokeMethod->arguments.Add(CreateFunctionArgument(L"methodId", CreatePredefinedType(WfPredefinedTypeName::Int)));
						invokeMethod->arguments.Add(CreateFunctionArgument(L"arguments", CreateTypeFromCpp<Ptr<IValueArray>>()));
						auto block = invokeMethod->statement.Cast<WfBlockStatement>();
						AddStatement(block, CreateVariableStatement(L"unknownId", CreatePredefinedType(WfPredefinedTypeName::Bool), CreateBool(false)));
						auto catchBlock = CreateBlock();
						AddStatement(catchBlock, CreateReturn(CreateRpcExceptionExpression(CreateMember(CreateReference(L"ex"), L"Message"))));
						AddStatement(block, CreateTryCatch(BuildDispatchChain(interfaces, false, L"unknownId"), L"ex", catchBlock));
						auto unknownIdBranch = CreateBlock();
						AddStatement(unknownIdBranch, CreateRaise(L"Unknown RPC method id."));
						AddStatement(block, CreateIf(CreateReference(L"unknownId"), unknownIdBranch));
						AddStatement(block, CreateReturn(CreateNull()));
						newOps->declarations.Add(invokeMethod);
					}

					{
						auto endInvokeMethod = CreateFunctionDeclaration(L"EndInvokeMethod", CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Override);
						endInvokeMethod->arguments.Add(CreateFunctionArgument(L"slot", CreatePredefinedType(WfPredefinedTypeName::Int)));
						AddStatement(endInvokeMethod->statement.Cast<WfBlockStatement>(), CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"_byvalReturnValues"), L"Remove"), CreateReference(L"slot"))));
						newOps->declarations.Add(endInvokeMethod);
					}

					{
						auto objectHold = CreateFunctionDeclaration(L"ObjectHold", CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Override);
						objectHold->arguments.Add(CreateFunctionArgument(L"ref", CreateTypeFromCpp<rpc_controller::RpcObjectReference>()));
						objectHold->arguments.Add(CreateFunctionArgument(L"remoteClientId", CreatePredefinedType(WfPredefinedTypeName::Int)));
						objectHold->arguments.Add(CreateFunctionArgument(L"hold", CreatePredefinedType(WfPredefinedTypeName::Bool)));
						auto trueBranch = CreateBlock();
						AddStatement(trueBranch, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"_lc"), L"LocalObjectHold"), CreateReference(L"ref"), CreateReference(L"remoteClientId"))));
						auto falseBranch = CreateBlock();
						AddStatement(falseBranch, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"_lc"), L"LocalObjectUnhold"), CreateReference(L"ref"), CreateReference(L"remoteClientId"))));
						AddStatement(objectHold->statement.Cast<WfBlockStatement>(), CreateIf(CreateReference(L"hold"), trueBranch, falseBranch));
						newOps->declarations.Add(objectHold);
					}

					AddStatement(functionDecl->statement.Cast<WfBlockStatement>(), CreateReturn(newOps));
					return functionDecl;
				}

				Ptr<WfDeclaration> GenerateObjectEventOpsFactory(const List<RpcInterfaceModel>& interfaces)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpcops_IRpcObjectEventOps", CreateTypeFromCpp<Ptr<rpc_controller::IRpcObjectEventOps>>(), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateTypeFromCpp<rpc_controller::IRpcLifecycle*>()));
					auto newOps = CreateNewInterface(CreateTypeFromCpp<Ptr<rpc_controller::IRpcObjectEventOps>>()).Cast<WfNewInterfaceExpression>();
					newOps->declarations.Add(CreateVariableDeclaration(L"_lc", CreateTypeFromCpp<rpc_controller::IRpcLifecycle*>(), CreateReference(L"lc")));

					{
						auto invokeEvent = CreateFunctionDeclaration(L"InvokeEvent", CreatePredefinedType(WfPredefinedTypeName::Object), WfFunctionKind::Override);
						invokeEvent->arguments.Add(CreateFunctionArgument(L"ref", CreateTypeFromCpp<rpc_controller::RpcObjectReference>()));
						invokeEvent->arguments.Add(CreateFunctionArgument(L"eventId", CreatePredefinedType(WfPredefinedTypeName::Int)));
						invokeEvent->arguments.Add(CreateFunctionArgument(L"arguments", CreateTypeFromCpp<Ptr<IValueArray>>()));
						auto block = invokeEvent->statement.Cast<WfBlockStatement>();
						if (!HasRpcEvents(interfaces))
						{
							AddStatement(block, CreateRaise(L"Unknown RPC event id."));
							newOps->declarations.Add(invokeEvent);
						}
						else
						{
							AddStatement(block, CreateVariableStatement(L"unknownId", CreatePredefinedType(WfPredefinedTypeName::Bool), CreateBool(false)));
							AddStatement(block, CreateVariableStatement(L"rpcEventExceptions", CreateRpcEventExceptionMapType(), CreateConstructor()));
							AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(CreateMember(CreateReference(L"_lc"), L"Controller"), L"SetEventSuppressedFlag"), CreateReference(L"ref"), CreateReference(L"eventId"), CreateBool(true))));
							auto finallyBlock = CreateBlock();
							AddStatement(finallyBlock, CreateExpressionStatement(CreateCall(CreateMember(CreateMember(CreateReference(L"_lc"), L"Controller"), L"SetEventSuppressedFlag"), CreateReference(L"ref"), CreateReference(L"eventId"), CreateBool(false))));
							auto catchBlock = CreateBlock();
							AddRpcEventExceptionMapSet(catchBlock, L"rpcEventExceptions", CreateMember(CreateReference(L"_lc"), L"ClientId"), CreateMember(CreateReference(L"ex"), L"Message"));
							AddStatement(block, CreateTry(BuildDispatchChain(interfaces, true, L"unknownId"), L"ex", catchBlock, finallyBlock));
							auto unknownIdBranch = CreateBlock();
							AddStatement(unknownIdBranch, CreateRaise(L"Unknown RPC event id."));
							AddStatement(block, CreateIf(CreateReference(L"unknownId"), unknownIdBranch));
							auto returnExceptionBranch = CreateBlock();
							AddStatement(returnExceptionBranch, CreateReturn(CreateReference(L"rpcEventExceptions")));
							auto returnNullBranch = CreateBlock();
							AddStatement(returnNullBranch, CreateReturn(CreateNull()));
							AddStatement(
								block,
								CreateIf(
									CreateBinary(WfBinaryOperator::GT, CreateMember(CreateReference(L"rpcEventExceptions"), L"Count"), CreateInt(0)),
									returnExceptionBranch,
									returnNullBranch));
							newOps->declarations.Add(invokeEvent);
						}
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

				void SortInterfaceModelsLeafFirst(const List<RpcInterfaceModel>& interfaces, List<const RpcInterfaceModel*>& sortedInterfaces)
				{
					sortedInterfaces.Clear();

					List<WString> typeFullNames;
					Group<WString, WString> dependencyGroup;
					for (auto&& interfaceModel : interfaces)
					{
						typeFullNames.Add(interfaceModel.fullName);
					}
					for (auto&& interfaceModel : interfaces)
					{
						for (auto&& baseFullName : interfaceModel.baseFullNames)
						{
							if (typeFullNames.Contains(baseFullName))
							{
								dependencyGroup.Add(baseFullName, interfaceModel.fullName);
							}
						}
					}

					PartialOrderingProcessor pop;
					pop.InitWithGroup(typeFullNames, dependencyGroup);
					pop.Sort();

					for (auto&& component : pop.components)
					{
						for (vint i = 0; i < component.nodeCount; i++)
						{
							auto interfaceModel = FindInterfaceModel(interfaces, typeFullNames[component.firstNode[i]]);
							CHECK_ERROR(interfaceModel, L"SortInterfaceModelsLeafFirst: Invalid RPC interface name.");
							sortedInterfaces.Add(interfaceModel);
						}
					}
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

				bool ContainsPropertyModel(const List<const RpcPropertyModel*>& properties, const WString& name)
				{
					for (auto propertyModel : properties)
					{
						if (propertyModel->name == name)
						{
							return true;
						}
					}
					return false;
				}

				bool ContainsMethodModel(const List<const RpcMethodModel*>& methods, const WString& fullName)
				{
					for (auto methodModel : methods)
					{
						if (methodModel->fullName == fullName)
						{
							return true;
						}
					}
					return false;
				}

				void CollectInterfaceProperties(const RpcInterfaceModel& interfaceModel, const List<RpcInterfaceModel>& interfaces, List<const RpcPropertyModel*>& properties)
				{
					for (auto&& baseFullName : interfaceModel.baseFullNames)
					{
						if (auto baseModel = FindInterfaceModel(interfaces, baseFullName))
						{
							CollectInterfaceProperties(*baseModel, interfaces, properties);
						}
					}

					for (auto&& propertyModel : interfaceModel.properties)
					{
						if (!ContainsPropertyModel(properties, propertyModel.name))
						{
							properties.Add(&propertyModel);
						}
					}
				}

				void CollectInterfaceMethods(const RpcInterfaceModel& interfaceModel, const List<RpcInterfaceModel>& interfaces, List<const RpcMethodModel*>& methods)
				{
					for (auto&& baseFullName : interfaceModel.baseFullNames)
					{
						if (auto baseModel = FindInterfaceModel(interfaces, baseFullName))
						{
							CollectInterfaceMethods(*baseModel, interfaces, methods);
						}
					}

					for (auto&& methodModel : interfaceModel.methods)
					{
						if (!ContainsMethodModel(methods, methodModel.fullName))
						{
							methods.Add(&methodModel);
						}
					}
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

				Ptr<WfDeclaration> GenerateWrapperInterface(const RpcInterfaceModel& interfaceModel, const List<RpcInterfaceModel>& interfaces)
				{
					List<const RpcPropertyModel*> properties;
					CollectInterfaceProperties(interfaceModel, interfaces, properties);

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
						auto baseType = CreateTypeFromCpp<rpc_controller::IRpcWrapperBase>();
						interfaceDecl->baseTypes.Add(baseType);
					}

					for (auto propertyModel : properties)
					{
						if (propertyModel->cached && propertyModel->valueChangedEvent != L"")
						{
							auto invalidateDecl = Ptr(new WfFunctionDeclaration);
							invalidateDecl->name.value = GetPropertyCacheResetFunctionName(*propertyModel);
							invalidateDecl->returnType = CreatePredefinedType(WfPredefinedTypeName::Void);
							invalidateDecl->functionKind = WfFunctionKind::Normal;
							invalidateDecl->anonymity = WfFunctionAnonymity::Named;
							interfaceDecl->declarations.Add(invalidateDecl);
						}
					}

					return interfaceDecl;
				}

				WString GetRpcOpsInvokeMethodName(const RpcMethodModel& methodModel);
				WString GetRpcOpsInvokeEventName(const RpcEventModel& eventModel);

				Ptr<WfDeclaration> GenerateListenerFactory(const RpcInterfaceModel& interfaceModel, const List<RpcInterfaceModel>& interfaces, const WString& opsInterfaceName)
				{
					List<const RpcEventModel*> events;
					CollectInterfaceEvents(interfaceModel, interfaces, events);
					if (events.Count() == 0)
					{
						return nullptr;
					}

					auto mangledName = MangleRpcFullName(interfaceModel.fullName);
					auto functionDecl = CreateFunctionDeclaration(L"rpclistener_" + mangledName, CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateTypeFromCpp<rpc_controller::IRpcLifecycle*>()));
					functionDecl->arguments.Add(CreateFunctionArgument(L"ref", CreateTypeFromCpp<rpc_controller::RpcObjectReference>()));
					functionDecl->arguments.Add(CreateFunctionArgument(L"target", CreateRawType(interfaceModel.fullName)));
					functionDecl->arguments.Add(CreateFunctionArgument(L"ops", CreateSharedType(opsInterfaceName)));
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
						auto invoke = CreateCall(
							CreateMember(CreateReference(L"ops"), GetRpcOpsInvokeEventName(*eventModel)),
							CreateReference(L"ref"));
						for (auto&& paramModel : eventModel->params)
						{
							invoke->arguments.Add(CreateReference(paramModel.name));
						}
						AddStatement(lambdaBody, CreateExpressionStatement(invoke));

						auto attach = Ptr(new WfAttachEventExpression);
						attach->event = CreateMember(CreateReference(L"target"), eventModel->name);
						attach->function = CreateFunctionExpression(CreateAnonymousLambda(eventModel->params, lambdaBody));
						AddStatement(block, CreateExpressionStatement(attach));
					}

					return functionDecl;
				}

				Ptr<WfDeclaration> GenerateListenerDispatcher(const List<RpcInterfaceModel>& interfaces, const WString& opsInterfaceName)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpclistener_Attach", CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"typeId", CreatePredefinedType(WfPredefinedTypeName::Int)));
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateTypeFromCpp<rpc_controller::IRpcLifecycle*>()));
					functionDecl->arguments.Add(CreateFunctionArgument(L"ref", CreateTypeFromCpp<rpc_controller::RpcObjectReference>()));
					functionDecl->arguments.Add(CreateFunctionArgument(L"obj", CreateTypeFromCpp<reflection::IDescriptable*>()));
					functionDecl->arguments.Add(CreateFunctionArgument(L"ops", CreateSharedType(opsInterfaceName)));
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
							AddStatement(caseBranch, CreateExpressionStatement(CreateCall(CreateReference(L"rpclistener_" + mangledName), CreateReference(L"lc"), CreateReference(L"ref"), CreateCast(CreateRawType(interfaceModel.fullName), CreateReference(L"obj")), CreateReference(L"ops"))));
						}
						AddStatement(caseBranch, CreateReturn(nullptr));
						switchCase->statement = caseBranch;
						switchStat->caseBranches.Add(switchCase);
					}

					AddStatement(block, switchStat);
					return functionDecl;
				}

				WString GetRpcOpsInterfaceName(const WString& assemblyName)
				{
					return L"rpcops_IOps_" + assemblyName;
				}

				WString GetRpcOpsInvokeMethodName(const RpcMethodModel& methodModel)
				{
					return L"InvokeMethod_" + MangleRpcFullName(methodModel.fullName);
				}

				WString GetRpcOpsInvokeEventName(const RpcEventModel& eventModel)
				{
					return L"InvokeEvent_" + MangleRpcFullName(eventModel.fullName);
				}

				WString GetRpcOpsArgumentName(const RpcParamModel& paramModel)
				{
					return L"arg_" + paramModel.name;
				}

				Ptr<WfFunctionDeclaration> CreateRpcOpsFunctionDeclaration(const WString& name, Ptr<WfType> returnType, WfFunctionKind kind)
				{
					auto functionDecl = Ptr(new WfFunctionDeclaration);
					functionDecl->name.value = name;
					functionDecl->returnType = returnType;
					functionDecl->functionKind = kind;
					functionDecl->anonymity = WfFunctionAnonymity::Named;
					if (kind == WfFunctionKind::Override)
					{
						functionDecl->statement = CreateBlock();
					}
					return functionDecl;
				}

				void AddRpcOpsFunctionArguments(Ptr<WfFunctionDeclaration> functionDecl, const List<RpcParamModel>& params)
				{
					functionDecl->arguments.Add(CreateFunctionArgument(L"ref", CreateTypeFromCpp<rpc_controller::RpcObjectReference>()));
					for (auto&& paramModel : params)
					{
						functionDecl->arguments.Add(CreateFunctionArgument(GetRpcOpsArgumentName(paramModel), CopyType(paramModel.type.Obj())));
					}
				}

				void AddRpcOpsArgumentsArray(
					Ptr<WfBlockStatement> block,
					const List<RpcParamModel>& params)
				{
					AddStatement(block, CreateVariableStatement(L"arguments", CreateTypeFromCpp<Ptr<IValueArray>>(), CreateConstructor()));
					AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"arguments"), L"Resize"), CreateInt(params.Count()))));

					for (vint i = 0; i < params.Count(); i++)
					{
						auto&& paramModel = params[i];
						auto value = CreateReference(GetRpcOpsArgumentName(paramModel));
						auto argument = CreateRpcBoxExpression(paramModel.typeInfo, paramModel.byref, value, CreateReference(L"_lc"));
						AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"arguments"), L"Set"), CreateInt(i), argument)));
					}
				}

				Ptr<WfExpression> CreateRpcOpsObjectOps()
				{
					return CreateCall(
						CreateMember(CreateMember(CreateReference(L"_lc"), L"Dispatcher"), L"SendToClient_ObjectOps"),
						CreateMember(CreateReference(L"ref"), L"clientId")
						);
				}

				Ptr<WfExpression> CreateRpcOpsObjectInvoke(const RpcMethodModel& methodModel, Ptr<WfExpression> objectOps)
				{
					return CreateCall(
						CreateMember(objectOps, L"InvokeMethod"),
						CreateReference(L"ref"),
						CreateRpcConstantReference(L"rpcmethod_", methodModel.fullName),
						CreateReference(L"arguments")
						);
				}

				Ptr<WfExpression> CreateRpcOpsObjectInvoke(const RpcMethodModel& methodModel)
				{
					return CreateRpcOpsObjectInvoke(methodModel, CreateRpcOpsObjectOps());
				}

				Ptr<WfExpression> CreateRpcOpsObjectEventInvoke(const RpcEventModel& eventModel)
				{
					return CreateCall(
						CreateMember(
							CreateCall(
								CreateMember(CreateMember(CreateReference(L"_lc"), L"Dispatcher"), L"BroadcastFromClient_ObjectEventOps"),
								CreateMember(CreateReference(L"_lc"), L"ClientId")
								),
							L"InvokeEvent"
							),
						CreateReference(L"ref"),
						CreateRpcConstantReference(L"rpcevent_", eventModel.fullName),
						CreateReference(L"arguments")
						);
				}

				Ptr<WfFunctionDeclaration> GenerateRpcOpsMethodImplementation(const RpcMethodModel& methodModel)
				{
					auto functionDecl = CreateRpcOpsFunctionDeclaration(GetRpcOpsInvokeMethodName(methodModel), CopyType(methodModel.returnType.Obj()), WfFunctionKind::Override);
					AddRpcOpsFunctionArguments(functionDecl, methodModel.params);
					auto block = functionDecl->statement.Cast<WfBlockStatement>();

					AddRpcOpsArgumentsArray(block, methodModel.params);
					if (IsVoidType(methodModel.returnType.Obj()))
					{
						auto invoke = CreateRpcOpsObjectInvoke(methodModel);
						AddStatement(block, CreateInferredVariableStatement(L"invokeResult", invoke));
						AddRpcMethodExceptionRaise(block, CreateReference(L"invokeResult"));
					}
					else if (IsRpcByvalReturn(methodModel))
					{
						AddStatement(block, CreateInferredVariableStatement(L"objectOps", CreateRpcOpsObjectOps()));
						auto invoke = CreateRpcOpsObjectInvoke(methodModel, CreateReference(L"objectOps"));
						AddStatement(block, CreateInferredVariableStatement(L"invokeResult", invoke));
						AddRpcMethodExceptionRaise(block, CreateReference(L"invokeResult"));
						AddStatement(block, CreateInferredVariableStatement(
							L"byvalReturnValue",
							CreateCast(CreateTypeFromCpp<Ptr<rpc_controller::RpcByvalReturnValue>>(), CreateReference(L"invokeResult"))));
						AddStatement(block, CreateInferredVariableStatement(
							L"result",
							CreateRpcUnboxExpression(
								methodModel.returnTypeInfo,
								methodModel.returnType,
								methodModel.returnByref,
								CreateMember(CreateReference(L"byvalReturnValue"), L"value"),
								CreateReference(L"_lc"))));
						AddStatement(block, CreateExpressionStatement(CreateCall(
							CreateMember(CreateReference(L"objectOps"), L"EndInvokeMethod"),
							CreateMember(CreateReference(L"byvalReturnValue"), L"slot"))));
						AddStatement(block, CreateReturn(CreateReference(L"result")));
					}
					else
					{
						auto invoke = CreateRpcOpsObjectInvoke(methodModel);
						AddStatement(block, CreateInferredVariableStatement(L"invokeResult", invoke));
						AddRpcMethodExceptionRaise(block, CreateReference(L"invokeResult"));
						AddStatement(block, CreateReturn(CreateRpcUnboxExpression(methodModel.returnTypeInfo, methodModel.returnType, methodModel.returnByref, CreateReference(L"invokeResult"), CreateReference(L"_lc"))));
					}

					return functionDecl;
				}

				Ptr<WfFunctionDeclaration> GenerateRpcOpsEventImplementation(const RpcEventModel& eventModel)
				{
					auto functionDecl = CreateRpcOpsFunctionDeclaration(GetRpcOpsInvokeEventName(eventModel), CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Override);
					AddRpcOpsFunctionArguments(functionDecl, eventModel.params);
					auto block = functionDecl->statement.Cast<WfBlockStatement>();

					AddRpcOpsArgumentsArray(block, eventModel.params);
					AddRpcEventExceptionRaise(block, CreateCast(CreateRpcEventExceptionMapType(), CreateRpcOpsObjectEventInvoke(eventModel)));
					return functionDecl;
				}

				Ptr<WfDeclaration> GenerateRpcOpsInterface(const WString& assemblyName, const List<RpcInterfaceModel>& interfaces)
				{
					auto interfaceDecl = Ptr(new WfClassDeclaration);
					interfaceDecl->name.value = GetRpcOpsInterfaceName(assemblyName);
					interfaceDecl->kind = WfClassKind::Interface;
					interfaceDecl->constructorType = WfConstructorType::SharedPtr;

					for (auto&& interfaceModel : interfaces)
					{
						for (auto&& methodModel : interfaceModel.methods)
						{
							auto methodDecl = CreateRpcOpsFunctionDeclaration(GetRpcOpsInvokeMethodName(methodModel), CopyType(methodModel.returnType.Obj()), WfFunctionKind::Normal);
							AddRpcOpsFunctionArguments(methodDecl, methodModel.params);
							interfaceDecl->declarations.Add(methodDecl);
						}
						for (auto&& eventModel : interfaceModel.events)
						{
							auto eventDecl = CreateRpcOpsFunctionDeclaration(GetRpcOpsInvokeEventName(eventModel), CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Normal);
							AddRpcOpsFunctionArguments(eventDecl, eventModel.params);
							interfaceDecl->declarations.Add(eventDecl);
						}
					}

					return interfaceDecl;
				}

				Ptr<WfDeclaration> GenerateRpcOpsFactory(const WString& assemblyName, const List<RpcInterfaceModel>& interfaces)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpcops_IOps_Create", CreateSharedType(GetRpcOpsInterfaceName(assemblyName)), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateTypeFromCpp<rpc_controller::IRpcLifecycle*>()));
					auto newOps = CreateNewInterface(CreateSharedType(GetRpcOpsInterfaceName(assemblyName))).Cast<WfNewInterfaceExpression>();
					newOps->declarations.Add(CreateVariableDeclaration(L"_lc", CreateTypeFromCpp<rpc_controller::IRpcLifecycle*>(), CreateReference(L"lc")));

					for (auto&& interfaceModel : interfaces)
					{
						for (auto&& methodModel : interfaceModel.methods)
						{
							newOps->declarations.Add(GenerateRpcOpsMethodImplementation(methodModel));
						}
						for (auto&& eventModel : interfaceModel.events)
						{
							newOps->declarations.Add(GenerateRpcOpsEventImplementation(eventModel));
						}
					}

					AddStatement(functionDecl->statement.Cast<WfBlockStatement>(), CreateReturn(newOps));
					return functionDecl;
				}

				Ptr<WfDeclaration> GenerateWrapperFactory(const RpcInterfaceModel& interfaceModel, const List<RpcInterfaceModel>& interfaces, const WString& opsInterfaceName)
				{
					auto mangledName = MangleRpcFullName(interfaceModel.fullName);
					auto wrapperInterfaceFullName = interfaceModel.fullName.Sub(0, interfaceModel.fullName.Length() - interfaceModel.interfaceName.Length()) + L"IRpcWrapper_" + interfaceModel.interfaceName;
					List<const RpcPropertyModel*> properties;
					CollectInterfaceProperties(interfaceModel, interfaces, properties);
					List<const RpcMethodModel*> methods;
					CollectInterfaceMethods(interfaceModel, interfaces, methods);
					List<const RpcEventModel*> events;
					CollectInterfaceEvents(interfaceModel, interfaces, events);

					auto functionDecl = CreateFunctionDeclaration(L"rpcwrapper_" + mangledName, CreateSharedType(wrapperInterfaceFullName), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateTypeFromCpp<rpc_controller::IRpcLifecycle*>()));
					functionDecl->arguments.Add(CreateFunctionArgument(L"proxyRef", CreateTypeFromCpp<rpc_controller::RpcObjectReference>()));
					functionDecl->arguments.Add(CreateFunctionArgument(L"ops", CreateSharedType(opsInterfaceName)));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();

					auto proxyExpr = CreateNewInterface(CreateSharedType(wrapperInterfaceFullName)).Cast<WfNewInterfaceExpression>();
					proxyExpr->declarations.Add(CreateVariableDeclaration(L"_lc", CreateTypeFromCpp<rpc_controller::IRpcLifecycle*>(), CreateReference(L"lc")));
					proxyExpr->declarations.Add(CreateVariableDeclaration(L"_ref", CreateTypeFromCpp<rpc_controller::RpcObjectReference>(), CreateReference(L"proxyRef")));
					proxyExpr->declarations.Add(CreateVariableDeclaration(L"_ops", CreateSharedType(opsInterfaceName), CreateReference(L"ops")));
					for (auto propertyModel : properties)
					{
						if (propertyModel->cached)
						{
							proxyExpr->declarations.Add(CreateVariableDeclaration(GetPropertyCacheValueName(*propertyModel), CopyType(propertyModel->type.Obj()), CreateRpcCachedPropertyInitialValue(*propertyModel)));
							proxyExpr->declarations.Add(CreateVariableDeclaration(GetPropertyCacheAvailableName(*propertyModel), CreatePredefinedType(WfPredefinedTypeName::Bool), CreateBool(false)));
						}
					}

					for (auto propertyModel : properties)
					{
						if (propertyModel->cached && propertyModel->valueChangedEvent != L"")
						{
							auto invalidateDecl = CreateFunctionDeclaration(GetPropertyCacheResetFunctionName(*propertyModel), CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Override);
							auto invalidateBlock = invalidateDecl->statement.Cast<WfBlockStatement>();
							AddStatement(invalidateBlock, CreateExpressionStatement(CreateAssign(CreateReference(GetPropertyCacheAvailableName(*propertyModel)), CreateBool(false))));
							proxyExpr->declarations.Add(invalidateDecl);
						}
					}

					// Generate DisconnectFromLifecycle override
					{
						auto disconnectDecl = CreateFunctionDeclaration(L"DisconnectFromLifecycle", CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Override);
						auto disconnectBlock = disconnectDecl->statement.Cast<WfBlockStatement>();
						AddStatement(disconnectBlock, CreateExpressionStatement(CreateAssign(CreateReference(L"_lc"), CreateNull())));
						AddStatement(disconnectBlock, CreateExpressionStatement(CreateAssign(CreateReference(L"_ops"), CreateNull())));
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

					for (auto methodModel : methods)
					{
						const RpcPropertyModel* cachedProperty = nullptr;
						if (methodModel->kind == RpcMethodKind::PropertyGetter)
						{
							for (auto propertyModel : properties)
							{
								if (propertyModel->cached && propertyModel->getterName == methodModel->name)
								{
									cachedProperty = propertyModel;
									break;
								}
							}
						}

						auto methodDecl = CreateFunctionDeclaration(methodModel->name, CopyType(methodModel->returnType.Obj()), WfFunctionKind::Override);
						for (auto&& paramModel : methodModel->params)
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

						auto invoke = Ptr(new WfCallExpression);
						invoke->function = CreateMember(CreateReference(L"_ops"), GetRpcOpsInvokeMethodName(*methodModel));
						invoke->arguments.Add(CreateReference(L"_ref"));
						for (auto&& paramModel : methodModel->params)
						{
							invoke->arguments.Add(CreateReference(paramModel.name));
						}

						if (IsVoidType(methodModel->returnType.Obj()))
						{
							AddStatement(methodBlock, CreateExpressionStatement(invoke));
						}
						else if (cachedProperty)
						{
							AddStatement(methodBlock, CreateExpressionStatement(CreateAssign(CreateReference(GetPropertyCacheValueName(*cachedProperty)), invoke)));
							AddStatement(methodBlock, CreateExpressionStatement(CreateAssign(CreateReference(GetPropertyCacheAvailableName(*cachedProperty)), CreateBool(true))));
							AddStatement(methodBlock, CreateReturn(CreatePropertyCacheValueRead(*cachedProperty)));
						}
						else
						{
							AddStatement(methodBlock, CreateReturn(invoke));
						}

						proxyExpr->declarations.Add(methodDecl);
					}

					AddStatement(block, CreateInferredVariableStatement(L"proxy", proxyExpr));
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

					for (auto propertyModel : properties)
					{
						if (propertyModel->cached && propertyModel->valueChangedEvent != L"")
						{
							if (auto eventModel = FindInterfaceEvent(events, propertyModel->valueChangedEvent))
							{
								auto lambdaBody = CreateBlock();
								auto invalidateTarget = CreateCast(CreateSharedType(wrapperInterfaceFullName), CreateCall(CreateMember(CreateReference(L"lc"), L"RefToPtr"), CreateReference(L"proxyRef")));
								AddStatement(lambdaBody, CreateExpressionStatement(CreateCall(CreateMember(invalidateTarget, GetPropertyCacheResetFunctionName(*propertyModel)))));

								auto attach = Ptr(new WfAttachEventExpression);
								attach->event = CreateMember(CreateCast(CreateSharedType(interfaceModel.fullName), CreateReference(L"proxy")), eventModel->name);
								attach->function = CreateFunctionExpression(CreateAnonymousLambda(eventModel->params, lambdaBody));
								AddStatement(block, CreateExpressionStatement(attach));
							}
						}
					}

					if (events.Count() > 0)
					{
						AddStatement(block, CreateExpressionStatement(CreateCall(CreateReference(L"rpclistener_" + mangledName), CreateReference(L"lc"), CreateReference(L"proxyRef"), CreateCast(CreateRawType(interfaceModel.fullName), CreateReference(L"proxy")), CreateReference(L"ops"))));
					}

					AddStatement(block, CreateReturn(CreateReference(L"proxy")));
					return functionDecl;
				}


				Ptr<WfDeclaration> GenerateWrapperDispatcher(const List<RpcInterfaceModel>& interfaces, const WString& opsInterfaceName)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpcwrapper_Create", CreateTypeFromCpp<Ptr<rpc_controller::IRpcWrapperBase>>(), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"ref", CreateTypeFromCpp<rpc_controller::RpcObjectReference>()));
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateTypeFromCpp<rpc_controller::IRpcLifecycle*>()));
					functionDecl->arguments.Add(CreateFunctionArgument(L"ops", CreateSharedType(opsInterfaceName)));
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
						AddStatement(caseBranch, CreateReturn(CreateCall(CreateReference(L"rpcwrapper_" + mangledName), CreateReference(L"lc"), CreateReference(L"ref"), CreateReference(L"ops"))));
						switchCase->statement = caseBranch;
						switchStat->caseBranches.Add(switchCase);
					}

					AddStatement(block, switchStat);
					return functionDecl;
				}

				Ptr<WfDeclaration> GenerateWrapperGetTypeId(const List<RpcInterfaceModel>& interfaces)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpcwrapper_GetTypeId", CreatePredefinedType(WfPredefinedTypeName::Int), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"obj", CreatePredefinedType(WfPredefinedTypeName::Object)));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();

					List<const RpcInterfaceModel*> sortedInterfaces;
					SortInterfaceModelsLeafFirst(interfaces, sortedInterfaces);
					for (auto interfaceModel : sortedInterfaces)
					{
						AddStatement(
							block,
							CreateIf(
								CreateIsType(CreateReference(L"obj"), CreateRawType(interfaceModel->fullName)),
								CreateReturn(CreateRpcConstantReference(L"rpctype_", interfaceModel->fullName))
								));
					}

					AddStatement(block, CreateReturn(CreateInt(rpc_controller::RpcTypeId_NotFound)));
					return functionDecl;
				}
			}

			Ptr<WfModule> GenerateModuleRpc(WfLexicalScopeManager* manager, WString assemblyName)
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
				auto opsInterfaceName = GetRpcOpsInterfaceName(assemblyName);

				vint id = 0;
				for (auto fullName : manager->rpcMetadata->orderedIds)
				{
					auto name = WString::Unmanaged(GetRpcConstantPrefix(manager, fullName)) + MangleRpcFullName(fullName);
					module->declarations.Add(CreateVariableDeclaration(name, CreatePredefinedType(WfPredefinedTypeName::Int), CreateInt(id++)));
				}

				{
					auto getIds = CreateFunctionDeclaration(L"rpc_GetIds", CreateTypeFromCpp<Dictionary<WString, vint>>(), WfFunctionKind::Normal);
					auto block = getIds->statement.Cast<WfBlockStatement>();
					AddStatement(block, CreateVariableStatement(L"result", CreateTypeFromCpp<Dictionary<WString, vint>>(), CreateConstructor()));
					id = 0;
					for (auto fullName : manager->rpcMetadata->orderedIds)
					{
						AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(rpc_generating::CreateReference(L"result"), L"Set"), CreateString(fullName), CreateInt(id++))));
					}
					AddStatement(block, CreateReturn(rpc_generating::CreateReference(L"result")));
					module->declarations.Add(getIds);
				}

				module->declarations.Add(GenerateIsInterfaceTypeIdHelper(interfaces));
				module->declarations.Add(GenerateIsCtorInterfaceTypeIdHelper(interfaces));
				module->declarations.Add(GenerateRpcOpsInterface(assemblyName, interfaces));
				module->declarations.Add(GenerateObjectOpsFactory(interfaces));
				module->declarations.Add(GenerateObjectEventOpsFactory(interfaces));
				module->declarations.Add(GenerateRpcOpsFactory(assemblyName, interfaces));

				List<Ptr<WfDeclaration>> wrapperDeclarations;
				Dictionary<WString, Ptr<WfNamespaceDeclaration>> wrapperNamespaces;
				for (auto&& interfaceModel : interfaces)
				{
					List<WString> namespaceFragments;
					SplitTypeFullName(interfaceModel.fullName, namespaceFragments);
					namespaceFragments.RemoveAt(namespaceFragments.Count() - 1);
					AddDeclarationToNamespaces(wrapperDeclarations, wrapperNamespaces, namespaceFragments, GenerateWrapperInterface(interfaceModel, interfaces));
				}
				for (auto declaration : wrapperDeclarations)
				{
					module->declarations.Add(declaration);
				}

				vint listenerCount = 0;
				for (auto&& interfaceModel : interfaces)
				{
					if (auto listener = GenerateListenerFactory(interfaceModel, interfaces, opsInterfaceName))
					{
						module->declarations.Add(listener);
						listenerCount++;
					}
				}
				if (listenerCount > 0)
				{
					module->declarations.Add(GenerateListenerDispatcher(interfaces, opsInterfaceName));
				}

				for (auto&& interfaceModel : interfaces)
				{
					module->declarations.Add(GenerateWrapperFactory(interfaceModel, interfaces, opsInterfaceName));
				}

				module->declarations.Add(GenerateWrapperDispatcher(interfaces, opsInterfaceName));
				module->declarations.Add(GenerateWrapperGetTypeId(interfaces));

				return module;
			}
		}
	}
}
