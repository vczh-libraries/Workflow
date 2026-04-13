#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			namespace rpc_compiling
			{
				using namespace collections;
				using namespace reflection;
				using namespace reflection::description;

/***********************************************************************
ValidateModuleRPC_Ast
***********************************************************************/

				enum class RpcDeferredKind
				{
					Cached,
					Dynamic,
					ByvalProperty,
					ByrefProperty,
					ByvalMethod,
					ByrefMethod,
					ByvalParameter,
					ByrefParameter,
				};

				struct RpcDeferredCheck
				{
					RpcDeferredKind							kind = RpcDeferredKind::Cached;
					WfAttribute*							attribute = nullptr;
					ITypeDescriptor*						ownerTd = nullptr;
					WfPropertyDeclaration*					propertyDecl = nullptr;
					WfFunctionDeclaration*					methodDecl = nullptr;
					WfFunctionArgument*						parameterDecl = nullptr;
				};

				struct RpcPhase1Context
				{
					SortedList<ITypeDescriptor*>			workflowRpcInterfaceTds;
					Dictionary<Pair<ITypeDescriptor*, ITypeDescriptor*>, WfType*>
														baseTypeNodeMap;
					List<RpcDeferredCheck>					deferredChecks;
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

				bool HasEarlierRpcAttribute(List<Ptr<WfAttribute>>& attributes, WfAttribute* currentAttribute, const wchar_t* name)
				{
					for (auto attribute : attributes)
					{
						if (attribute.Obj() == currentAttribute)
						{
							break;
						}

						if (IsRpcAttribute(attribute.Obj(), name))
						{
							return true;
						}
					}
					return false;
				}

				ITypeDescriptor* GetClassTypeDescriptor(WfLexicalScopeManager* manager, WfClassDeclaration* classDecl)
				{
					if (!classDecl)
					{
						return nullptr;
					}
					return manager->declarationTypes[classDecl].Obj();
				}

				WfPropertyDeclaration* GetRpcPropertyDeclaration(WfDeclaration* declaration)
				{
					if (auto propertyDecl = dynamic_cast<WfPropertyDeclaration*>(declaration))
					{
						return propertyDecl;
					}

					if (auto autoPropertyDecl = dynamic_cast<WfAutoPropertyDeclaration*>(declaration))
					{
						for (auto expandedDeclaration : autoPropertyDecl->expandedDeclarations)
						{
							if (auto propertyDecl = expandedDeclaration.Cast<WfPropertyDeclaration>())
							{
								return propertyDecl.Obj();
							}
						}
					}

					return nullptr;
				}

				WString GetDeclarationFullName(WfLexicalScopeManager* manager, WfDeclaration* declaration, WfClassDeclaration* ownerClassDecl)
				{
					if (auto classDecl = dynamic_cast<WfClassDeclaration*>(declaration))
					{
						if (auto td = GetClassTypeDescriptor(manager, classDecl))
						{
							return td->GetTypeName();
						}
					}

					if (auto ownerTd = GetClassTypeDescriptor(manager, ownerClassDecl))
					{
						if (declaration)
						{
							return ownerTd->GetTypeName() + L"." + declaration->name.value;
						}
						return ownerTd->GetTypeName();
					}

					if (declaration && declaration->name.value != L"")
					{
						return declaration->name.value;
					}

					return L"<anonymous>";
				}

				WString GetAttributeTargetFullName(WfLexicalScopeManager* manager, WfDeclaration* declaration, WfFunctionArgument* argument, WfClassDeclaration* ownerClassDecl)
				{
					auto ownerTd = GetClassTypeDescriptor(manager, ownerClassDecl);
					auto ownerPrefix = ownerTd ? ownerTd->GetTypeName() + L"." : WString();

					if (argument && declaration)
					{
						return ownerPrefix + declaration->name.value + L"(" + argument->name.value + L")";
					}
					else if (declaration)
					{
						return ownerPrefix + declaration->name.value;
					}
					else if (argument)
					{
						return ownerPrefix + argument->name.value;
					}
					else
					{
						return ownerTd ? ownerTd->GetTypeName() : WString(L"<unknown>");
					}
				}

				bool ContainsFunctionType(WfType* type)
				{
					class V : public Object, public WfType::IVisitor
					{
					public:
						bool found = false;

						void Visit(WfFunctionType* node)override { found = true; }
						void Visit(WfRawPointerType* node)override { node->element->Accept(this); }
						void Visit(WfSharedPointerType* node)override { node->element->Accept(this); }
						void Visit(WfNullableType* node)override { node->element->Accept(this); }
						void Visit(WfEnumerableType* node)override { node->element->Accept(this); }
						void Visit(WfMapType* node)override { if (node->key) node->key->Accept(this); node->value->Accept(this); }
						void Visit(WfObservableListType* node)override { node->element->Accept(this); }
						void Visit(WfChildType* node)override { node->parent->Accept(this); }
						void Visit(WfPredefinedType* node)override {}
						void Visit(WfTopQualifiedType* node)override {}
						void Visit(WfReferenceType* node)override {}
					};

					V visitor;
					type->Accept(&visitor);
					return visitor.found;
				}

				bool IsGenericRpcInterface(WfClassDeclaration* classDecl)
				{
					if (!classDecl || classDecl->kind != WfClassKind::Interface)
					{
						return false;
					}

					for (auto baseType : classDecl->baseTypes)
					{
						if (ContainsFunctionType(baseType.Obj()))
						{
							return true;
						}
					}
					return false;
				}

				bool IsAstRpcInterface(WfClassDeclaration* classDecl)
				{
					return classDecl
						&& classDecl->kind == WfClassKind::Interface
						&& !IsGenericRpcInterface(classDecl)
						&& HasRpcAttribute(classDecl->attributes, L"Interface");
				}

				void CollectAllWorkflowRpcInterfaceTds(WfLexicalScopeManager* manager, SortedList<ITypeDescriptor*>& rpcInterfaceTds)
				{
					for (vint i = 0; i < manager->declarationTypes.Count(); i++)
					{
						auto classDecl = manager->declarationTypes.Keys()[i].Cast<WfClassDeclaration>();
						if (!classDecl || !IsAstRpcInterface(classDecl.Obj()))
						{
							continue;
						}

						auto td = manager->declarationTypes.Values()[i].Obj();
						if (td && !rpcInterfaceTds.Contains(td))
						{
							rpcInterfaceTds.Add(td);
						}
					}
				}

				void BuildMemberDeclarationMap(WfLexicalScopeManager* manager, Dictionary<IMemberInfo*, Ptr<WfDeclaration>>& memberDeclMap)
				{
					for (vint i = 0; i < manager->declarationMemberInfos.Count(); i++)
					{
						auto memberInfo = manager->declarationMemberInfos.Values()[i].Obj();
						if (memberInfo && !memberDeclMap.Keys().Contains(memberInfo))
						{
							memberDeclMap.Add(memberInfo, manager->declarationMemberInfos.Keys()[i]);
						}
					}
				}

				Ptr<WfFunctionArgument> FindFunctionArgument(WfFunctionDeclaration* functionDecl, const WString& parameterName, vint index)
				{
					if (!functionDecl)
					{
						return nullptr;
					}

					for (auto argument : functionDecl->arguments)
					{
						if (argument->name.value == parameterName)
						{
							return argument;
						}
					}

					if (0 <= index && index < functionDecl->arguments.Count())
					{
						return functionDecl->arguments[index];
					}

					return nullptr;
				}

				bool HasAttribute(IAttributeBag* bag, ITypeDescriptor* attrTd)
				{
					if (!bag || !attrTd)
					{
						return false;
					}

					for (vint i = 0; i < bag->GetAttributeCount(); i++)
					{
						if (bag->GetAttribute(i)->GetAttributeType() == attrTd) return true;
					}
					return false;
				}

				bool IsRpcInterfaceTd(ITypeDescriptor* td, ITypeDescriptor* rpcInterfaceAttrTd, const SortedList<ITypeDescriptor*>& rpcInterfaceTds)
				{
					if (!td) return false;
					if (rpcInterfaceTds.Contains(td)) return true;
					return HasAttribute(td, rpcInterfaceAttrTd);
				}

				bool IsStrongTypedCollection(ITypeInfo* type)
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

				bool IsSerializableType(ITypeInfo* type, ITypeDescriptor* rpcInterfaceAttrTd, const SortedList<ITypeDescriptor*>& rpcInterfaceTds)
				{
					if (!type) return false;
					switch (type->GetDecorator())
					{
					case ITypeInfo::SharedPtr:
						{
							auto e = type->GetElementType();
							if (!e) return false;

							if (e->GetDecorator() == ITypeInfo::Generic)
							{
								if (!IsStrongTypedCollection(type)) return false;
								for (vint i = 0; i < e->GetGenericArgumentCount(); i++)
								{
									if (!IsSerializableType(e->GetGenericArgument(i), rpcInterfaceAttrTd, rpcInterfaceTds)) return false;
								}
								return true;
							}

							if (e->GetDecorator() == ITypeInfo::TypeDescriptor)
							{
								auto td = e->GetTypeDescriptor();
								return (td->GetTypeDescriptorFlags() & TypeDescriptorFlags::Interface) != TypeDescriptorFlags::Undefined
									&& IsRpcInterfaceTd(td, rpcInterfaceAttrTd, rpcInterfaceTds);
							}

							return false;
						}

					case ITypeInfo::TypeDescriptor:
						{
							auto td = type->GetTypeDescriptor();
							if (td == GetTypeDescriptor<void>()) return false;
							switch (td->GetTypeDescriptorFlags())
							{
							case TypeDescriptorFlags::Primitive:
							case TypeDescriptorFlags::Struct:
							case TypeDescriptorFlags::FlagEnum:
							case TypeDescriptorFlags::NormalEnum:
								return true;
							default:
								return false;
							}
						}

					default:
						return false;
					}
				}

				bool IsSerializableReturnType(ITypeInfo* type, ITypeDescriptor* rpcInterfaceAttrTd, const SortedList<ITypeDescriptor*>& rpcInterfaceTds)
				{
					if (!type) return false;
					if (type->GetDecorator() == ITypeInfo::TypeDescriptor && type->GetTypeDescriptor() == GetTypeDescriptor<void>()) return true;
					return IsSerializableType(type, rpcInterfaceAttrTd, rpcInterfaceTds);
				}

				WString GetDeferredAttributeName(RpcDeferredKind kind)
				{
					switch (kind)
					{
					case RpcDeferredKind::Cached:			return L"Cached";
					case RpcDeferredKind::Dynamic:			return L"Dynamic";
					case RpcDeferredKind::ByvalProperty:
					case RpcDeferredKind::ByvalMethod:
					case RpcDeferredKind::ByvalParameter:	return L"Byval";
					case RpcDeferredKind::ByrefProperty:
					case RpcDeferredKind::ByrefMethod:
					case RpcDeferredKind::ByrefParameter:	return L"Byref";
					default:								return L"";
					}
				}

				void ValidateModuleRPC_Ast(WfLexicalScopeManager* manager, Ptr<WfModule> module, RpcPhase1Context& context)
				{
					class Visitor
						: public Object
						, public WfDeclaration::IVisitor
						, public WfVirtualCseDeclaration::IVisitor
					{
					public:
						WfLexicalScopeManager*					manager;
						RpcPhase1Context&						context;
						WfClassDeclaration*						currentClassDecl = nullptr;

						Visitor(WfLexicalScopeManager* _manager, RpcPhase1Context& _context)
							:manager(_manager)
							, context(_context)
						{
						}

						void VisitDeclaration(Ptr<WfDeclaration> declaration)
						{
							ProcessAttributes(declaration->attributes, declaration.Obj(), nullptr);
							declaration->Accept(this);
						}

						void ProcessAttributes(List<Ptr<WfAttribute>>& attributes, WfDeclaration* declaration, WfFunctionArgument* argument)
						{
							for (auto attribute : attributes)
							{
								auto name = attribute->name.value;
								if (attribute->category.value != L"rpc")
								{
									continue;
								}

								if (name == L"Interface")
								{
									auto classDecl = dynamic_cast<WfClassDeclaration*>(declaration);
									if (!classDecl || classDecl->kind != WfClassKind::Interface || IsGenericRpcInterface(classDecl))
									{
										manager->errors.Add(WfErrors::RpcInterfaceCanOnlyApplyToInterface(attribute.Obj(), GetDeclarationFullName(manager, declaration, currentClassDecl)));
										continue;
									}

									auto td = manager->declarationTypes[classDecl].Obj();
									if (td && !context.workflowRpcInterfaceTds.Contains(td))
									{
										context.workflowRpcInterfaceTds.Add(td);

										auto scope = manager->nodeScopes[classDecl];
										for (auto baseType : classDecl->baseTypes)
										{
											auto scopeName = GetScopeNameFromReferenceType(scope->parentScope.Obj(), baseType);
											if (scopeName && scopeName->typeDescriptor)
											{
												auto key = Pair(td, scopeName->typeDescriptor);
												if (!context.baseTypeNodeMap.Keys().Contains(key))
												{
													context.baseTypeNodeMap.Add(key, baseType.Obj());
												}
											}
										}
									}
								}
								else if (name == L"Ctor")
								{
									auto classDecl = dynamic_cast<WfClassDeclaration*>(declaration);
									if (!classDecl || !IsAstRpcInterface(classDecl))
									{
										manager->errors.Add(WfErrors::RpcCtorCanOnlyApplyToRpcInterface(attribute.Obj()));
									}
								}
								else if (name == L"Byval" || name == L"Byref")
								{
									auto propertyDecl = GetRpcPropertyDeclaration(declaration);
									auto isProperty = propertyDecl != nullptr;
									auto isMethod = dynamic_cast<WfFunctionDeclaration*>(declaration) != nullptr;
									auto isParameter = argument != nullptr && isMethod;
									if (!isProperty && !isMethod && !isParameter)
									{
										manager->errors.Add(WfErrors::RpcAttributeCanOnlyApplyToPropertyMethodOrParameter(attribute.Obj(), name));
										continue;
									}

									if (name == L"Byref" && HasEarlierRpcAttribute(attributes, attribute.Obj(), L"Byval"))
									{
										manager->errors.Add(WfErrors::RpcAttributeCannotCoexistWithOther(attribute.Obj(), name, L"Byval", GetAttributeTargetFullName(manager, declaration, argument, currentClassDecl)));
										continue;
									}

									if (name == L"Byval" && HasEarlierRpcAttribute(attributes, attribute.Obj(), L"Byref"))
									{
										manager->errors.Add(WfErrors::RpcAttributeCannotCoexistWithOther(attribute.Obj(), name, L"Byref", GetAttributeTargetFullName(manager, declaration, argument, currentClassDecl)));
										continue;
									}

									if (!IsAstRpcInterface(currentClassDecl))
									{
										manager->errors.Add(WfErrors::RpcAttributeCanOnlyBeUsedInsideRpcInterface(attribute.Obj(), name));
										continue;
									}

									RpcDeferredCheck check;
									check.attribute = attribute.Obj();
									check.ownerTd = GetClassTypeDescriptor(manager, currentClassDecl);
									check.propertyDecl = propertyDecl;
									check.methodDecl = dynamic_cast<WfFunctionDeclaration*>(declaration);
									check.parameterDecl = argument;
									check.kind =
										isParameter ? (name == L"Byval" ? RpcDeferredKind::ByvalParameter : RpcDeferredKind::ByrefParameter) :
										isMethod ? (name == L"Byval" ? RpcDeferredKind::ByvalMethod : RpcDeferredKind::ByrefMethod) :
										(name == L"Byval" ? RpcDeferredKind::ByvalProperty : RpcDeferredKind::ByrefProperty);
									context.deferredChecks.Add(check);
								}
								else if (name == L"Cached" || name == L"Dynamic")
								{
									auto propertyDecl = GetRpcPropertyDeclaration(declaration);
									if (!propertyDecl)
									{
										manager->errors.Add(WfErrors::RpcAttributeCanOnlyApplyToProperty(attribute.Obj(), name));
										continue;
									}

									if (name == L"Dynamic" && HasEarlierRpcAttribute(attributes, attribute.Obj(), L"Cached"))
									{
										manager->errors.Add(WfErrors::RpcAttributeCannotCoexistWithOther(attribute.Obj(), name, L"Cached", GetAttributeTargetFullName(manager, declaration, argument, currentClassDecl)));
										continue;
									}

									if (name == L"Cached" && HasEarlierRpcAttribute(attributes, attribute.Obj(), L"Dynamic"))
									{
										manager->errors.Add(WfErrors::RpcAttributeCannotCoexistWithOther(attribute.Obj(), name, L"Dynamic", GetAttributeTargetFullName(manager, declaration, argument, currentClassDecl)));
										continue;
									}

									RpcDeferredCheck check;
									check.kind = name == L"Cached" ? RpcDeferredKind::Cached : RpcDeferredKind::Dynamic;
									check.attribute = attribute.Obj();
									check.ownerTd = GetClassTypeDescriptor(manager, currentClassDecl);
									check.propertyDecl = propertyDecl;
									context.deferredChecks.Add(check);
								}
							}
						}

						void Visit(WfNamespaceDeclaration* node)override
						{
							for (auto declaration : node->declarations)
							{
								VisitDeclaration(declaration);
							}
						}

						void Visit(WfFunctionDeclaration* node)override
						{
							for (auto argument : node->arguments)
							{
								ProcessAttributes(argument->attributes, node, argument.Obj());
							}
						}

						void Visit(WfVariableDeclaration* node)override
						{
						}

						void Visit(WfEventDeclaration* node)override
						{
						}

						void Visit(WfPropertyDeclaration* node)override
						{
						}

						void Visit(WfStaticInitDeclaration* node)override
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
							auto oldClassDecl = currentClassDecl;
							currentClassDecl = node;
							for (auto declaration : node->declarations)
							{
								VisitDeclaration(declaration);
							}
							currentClassDecl = oldClassDecl;
						}

						void Visit(WfEnumDeclaration* node)override
						{
							for (auto item : node->items)
							{
								ProcessAttributes(item->attributes, nullptr, nullptr);
							}
						}

						void Visit(WfStructDeclaration* node)override
						{
							for (auto member : node->members)
							{
								ProcessAttributes(member->attributes, nullptr, nullptr);
							}
						}

						void Visit(WfVirtualCfeDeclaration* node)override
						{
							for (auto declaration : node->expandedDeclarations)
							{
								VisitDeclaration(declaration);
							}
						}

						void Visit(WfVirtualCseDeclaration* node)override
						{
							node->Accept((WfVirtualCseDeclaration::IVisitor*)this);
							for (auto declaration : node->expandedDeclarations)
							{
								VisitDeclaration(declaration);
							}
						}

						void Visit(WfStateMachineDeclaration* node)override
						{
						}
					};

					Visitor visitor(manager, context);
					for (auto declaration : module->declarations)
					{
						visitor.VisitDeclaration(declaration);
					}
				}

/***********************************************************************
ValidateModuleRPC_Reflection
***********************************************************************/

				void ValidateModuleRPC_Reflection(WfLexicalScopeManager* manager, const RpcPhase1Context& context)
				{
					auto rpcInterfaceAttrTd = GetTypeDescriptor<vl::__vwsn::att_rpc_Interface>();
					SortedList<ITypeDescriptor*> allWorkflowRpcInterfaceTds;
					for (auto td : context.workflowRpcInterfaceTds)
					{
						if (!allWorkflowRpcInterfaceTds.Contains(td))
						{
							allWorkflowRpcInterfaceTds.Add(td);
						}
					}
					CollectAllWorkflowRpcInterfaceTds(manager, allWorkflowRpcInterfaceTds);

					Dictionary<IMemberInfo*, Ptr<WfDeclaration>> memberDeclMap;
					BuildMemberDeclarationMap(manager, memberDeclMap);

					for (auto td : context.workflowRpcInterfaceTds)
					{
						for (vint i = 0; i < td->GetBaseTypeDescriptorCount(); i++)
						{
							auto baseTd = td->GetBaseTypeDescriptor(i);
							if (!IsRpcInterfaceTd(baseTd, rpcInterfaceAttrTd, allWorkflowRpcInterfaceTds))
							{
								auto key = Pair(td, baseTd);
								auto index = context.baseTypeNodeMap.Keys().IndexOf(key);
								if (index != -1)
								{
									manager->errors.Add(WfErrors::RpcInterfaceBaseNotSerializable(context.baseTypeNodeMap.Values()[index], td->GetTypeName(), baseTd->GetTypeName()));
								}
							}
						}

						for (vint i = 0; i < td->GetPropertyCount(); i++)
						{
							auto propertyInfo = td->GetProperty(i);
							if (propertyInfo->GetOwnerTypeDescriptor() != td)
							{
								continue;
							}

							if (!IsSerializableType(propertyInfo->GetReturn(), rpcInterfaceAttrTd, allWorkflowRpcInterfaceTds))
							{
								auto declIndex = memberDeclMap.Keys().IndexOf(propertyInfo);
								if (declIndex != -1)
								{
									manager->errors.Add(WfErrors::RpcInterfaceMemberNotSerializable(memberDeclMap.Values()[declIndex].Obj(), td->GetTypeName(), td->GetTypeName() + L"." + propertyInfo->GetName()));
								}
							}
						}

						for (vint i = 0; i < td->GetMethodGroupCount(); i++)
						{
							auto group = td->GetMethodGroup(i);
							if (group->GetOwnerTypeDescriptor() != td)
							{
								continue;
							}

							for (vint j = 0; j < group->GetMethodCount(); j++)
							{
								auto methodInfo = group->GetMethod(j);
								auto declIndex = memberDeclMap.Keys().IndexOf(methodInfo);
								if (declIndex == -1)
								{
									continue;
								}

								auto functionDecl = memberDeclMap.Values()[declIndex].Cast<WfFunctionDeclaration>();
								if (!functionDecl)
								{
									continue;
								}

								for (vint k = 0; k < methodInfo->GetParameterCount(); k++)
								{
									auto parameterInfo = methodInfo->GetParameter(k);
									if (!IsSerializableType(parameterInfo->GetType(), rpcInterfaceAttrTd, allWorkflowRpcInterfaceTds))
									{
										if (auto argument = FindFunctionArgument(functionDecl.Obj(), parameterInfo->GetName(), k))
										{
											manager->errors.Add(WfErrors::RpcInterfaceMemberNotSerializable(argument.Obj(), td->GetTypeName(), td->GetTypeName() + L"." + methodInfo->GetName() + L"(" + parameterInfo->GetName() + L")"));
										}
									}
								}

								if (!IsSerializableReturnType(methodInfo->GetReturn(), rpcInterfaceAttrTd, allWorkflowRpcInterfaceTds))
								{
									manager->errors.Add(WfErrors::RpcInterfaceMemberNotSerializable(functionDecl.Obj(), td->GetTypeName(), td->GetTypeName() + L"." + methodInfo->GetName()));
								}
							}
						}
					}

					for (auto&& check : context.deferredChecks)
					{
						auto attributeName = GetDeferredAttributeName(check.kind);
						switch (check.kind)
						{
						case RpcDeferredKind::Cached:
						case RpcDeferredKind::Dynamic:
							if (!IsRpcInterfaceTd(check.ownerTd, rpcInterfaceAttrTd, allWorkflowRpcInterfaceTds))
							{
								manager->errors.Add(WfErrors::RpcAttributeCanOnlyBeUsedInsideRpcInterface(check.attribute, attributeName));
							}
							break;
						default:
							{
								ITypeInfo* targetType = nullptr;
								WString memberName;

								switch (check.kind)
								{
								case RpcDeferredKind::ByvalProperty:
								case RpcDeferredKind::ByrefProperty:
									{
										auto propertyInfo = manager->declarationMemberInfos[check.propertyDecl].Cast<IPropertyInfo>();
										targetType = propertyInfo ? propertyInfo->GetReturn() : nullptr;
										if (propertyInfo && check.ownerTd)
										{
											memberName = check.ownerTd->GetTypeName() + L"." + propertyInfo->GetName();
										}
									}
									break;
								case RpcDeferredKind::ByvalMethod:
								case RpcDeferredKind::ByrefMethod:
									{
										auto methodInfo = manager->declarationMemberInfos[check.methodDecl].Cast<IMethodInfo>();
										targetType = methodInfo ? methodInfo->GetReturn() : nullptr;
										if (methodInfo && check.ownerTd)
										{
											memberName = check.ownerTd->GetTypeName() + L"." + methodInfo->GetName();
										}
									}
									break;
								case RpcDeferredKind::ByvalParameter:
								case RpcDeferredKind::ByrefParameter:
									{
										auto methodInfo = manager->declarationMemberInfos[check.methodDecl].Cast<IMethodInfo>();
										if (methodInfo)
										{
											for (vint i = 0; i < methodInfo->GetParameterCount(); i++)
											{
												auto parameterInfo = methodInfo->GetParameter(i);
												if (parameterInfo->GetName() == check.parameterDecl->name.value)
												{
													targetType = parameterInfo->GetType();
													if (check.ownerTd)
													{
														memberName = check.ownerTd->GetTypeName() + L"." + methodInfo->GetName() + L"(" + parameterInfo->GetName() + L")";
													}
													break;
												}
											}
										}
									}
									break;
								default:
									break;
								}

								if (!IsStrongTypedCollection(targetType))
								{
									manager->errors.Add(WfErrors::RpcAttributeRequiresStrongTypedCollection(check.attribute, attributeName, memberName));
								}
							}
							break;
						}
					}
				}

/***********************************************************************
ValidateModuleRPC_GenerateMetadata
***********************************************************************/

				Ptr<WfAttribute> CopyAttribute(Ptr<WfAttribute> src)
				{
					auto attr = Ptr(new WfAttribute);
					attr->category.value = src->category.value;
					attr->name.value = src->name.value;
					if (src->value)
					{
						attr->value = CopyExpression(src->value, false);
					}
					return attr;
				}

				void CopyAttributes(List<Ptr<WfAttribute>>& dst, const List<Ptr<WfAttribute>>& src)
				{
					for (auto attr : src)
					{
						dst.Add(CopyAttribute(attr));
					}
				}

				void CollectSerializableTypesFromTypeInfo(
					ITypeInfo* typeInfo,
					SortedList<ITypeDescriptor*>& collectedEnums,
					SortedList<ITypeDescriptor*>& collectedStructs,
					List<ITypeDescriptor*>& structVisitOrder,
					const SortedList<ITypeDescriptor*>& rpcInterfaceTds)
				{
					if (!typeInfo) return;
					switch (typeInfo->GetDecorator())
					{
					case ITypeInfo::SharedPtr:
						{
							auto e = typeInfo->GetElementType();
							if (!e) return;
							if (e->GetDecorator() == ITypeInfo::Generic)
							{
								for (vint i = 0; i < e->GetGenericArgumentCount(); i++)
								{
									CollectSerializableTypesFromTypeInfo(e->GetGenericArgument(i), collectedEnums, collectedStructs, structVisitOrder, rpcInterfaceTds);
								}
							}
						}
						break;
					case ITypeInfo::TypeDescriptor:
						{
							auto td = typeInfo->GetTypeDescriptor();
							if (!td) return;
							switch (td->GetTypeDescriptorFlags())
							{
							case TypeDescriptorFlags::FlagEnum:
							case TypeDescriptorFlags::NormalEnum:
								if (!collectedEnums.Contains(td))
								{
									collectedEnums.Add(td);
								}
								break;
							case TypeDescriptorFlags::Struct:
								if (!collectedStructs.Contains(td))
								{
									collectedStructs.Add(td);
									// Recursively collect types used by struct members
									for (vint i = 0; i < td->GetPropertyCount(); i++)
									{
										CollectSerializableTypesFromTypeInfo(td->GetProperty(i)->GetReturn(), collectedEnums, collectedStructs, structVisitOrder, rpcInterfaceTds);
									}
									structVisitOrder.Add(td);
								}
								break;
							default:
								break;
							}
						}
						break;
					default:
						break;
					}
				}

				void CollectTypesFromInterface(
					ITypeDescriptor* td,
					SortedList<ITypeDescriptor*>& collectedEnums,
					SortedList<ITypeDescriptor*>& collectedStructs,
					List<ITypeDescriptor*>& structVisitOrder,
					const SortedList<ITypeDescriptor*>& rpcInterfaceTds)
				{
					for (vint i = 0; i < td->GetPropertyCount(); i++)
					{
						auto propertyInfo = td->GetProperty(i);
						if (propertyInfo->GetOwnerTypeDescriptor() != td) continue;
						CollectSerializableTypesFromTypeInfo(propertyInfo->GetReturn(), collectedEnums, collectedStructs, structVisitOrder, rpcInterfaceTds);
					}

					for (vint i = 0; i < td->GetMethodGroupCount(); i++)
					{
						auto group = td->GetMethodGroup(i);
						if (group->GetOwnerTypeDescriptor() != td) continue;
						for (vint j = 0; j < group->GetMethodCount(); j++)
						{
							auto methodInfo = group->GetMethod(j);
							CollectSerializableTypesFromTypeInfo(methodInfo->GetReturn(), collectedEnums, collectedStructs, structVisitOrder, rpcInterfaceTds);
							for (vint k = 0; k < methodInfo->GetParameterCount(); k++)
							{
								CollectSerializableTypesFromTypeInfo(methodInfo->GetParameter(k)->GetType(), collectedEnums, collectedStructs, structVisitOrder, rpcInterfaceTds);
							}
						}
					}

					for (vint i = 0; i < td->GetEventCount(); i++)
					{
						auto eventInfo = td->GetEvent(i);
						if (eventInfo->GetOwnerTypeDescriptor() != td) continue;
						auto handlerType = eventInfo->GetHandlerType();
						if (handlerType && handlerType->GetDecorator() == ITypeInfo::SharedPtr)
						{
							auto genericType = handlerType->GetElementType();
							if (genericType && genericType->GetDecorator() == ITypeInfo::Generic)
							{
								for (vint j = 1; j < genericType->GetGenericArgumentCount(); j++)
								{
									CollectSerializableTypesFromTypeInfo(genericType->GetGenericArgument(j), collectedEnums, collectedStructs, structVisitOrder, rpcInterfaceTds);
								}
							}
						}
					}
				}

				Ptr<WfEnumDeclaration> GenerateEnumDecl(ITypeDescriptor* td)
				{
					auto enumType = td->GetEnumType();
					if (!enumType) return nullptr;

					auto decl = Ptr(new WfEnumDeclaration);
					List<WString> fragments;
					GetTypeFragments(td, fragments);
					decl->name.value = fragments[fragments.Count() - 1];
					decl->kind = enumType->IsFlagEnum() ? WfEnumKind::Flag : WfEnumKind::Normal;

					// Collect items sorted by value
					List<Pair<WString, vuint64_t>> items;
					for (vint i = 0; i < enumType->GetItemCount(); i++)
					{
						items.Add({ enumType->GetItemName(i), enumType->GetItemValue(i) });
					}
					if (items.Count() > 1)
					{
						Sort(&items[0], items.Count(), [](const Pair<WString, vuint64_t>& a, const Pair<WString, vuint64_t>& b)
						{
							return (a.value <=> b.value);
						});
					}

					if (enumType->IsFlagEnum())
					{
						// Build a map from power-of-2 values to item names
						Dictionary<vuint64_t, WString> powerOf2Names;

						// First pass: add constant items (0 and powers of 2)
						for (auto&& [name, value] : items)
						{
							if (value == 0 || (value != 0 && (value & (value - 1)) == 0))
							{
								auto item = Ptr(new WfEnumItem);
								item->name.value = name;
								item->kind = WfEnumItemKind::Constant;
								item->number.value = u64tow(value);
								decl->items.Add(item);
								powerOf2Names.Add(value, name);
							}
						}

						// Second pass: add intersection items (composite values)
						for (auto&& [name, value] : items)
						{
							if (value != 0 && (value & (value - 1)) != 0)
							{
								auto item = Ptr(new WfEnumItem);
								item->name.value = name;
								item->kind = WfEnumItemKind::Intersection;
								for (vint bit = 0; bit < 64; bit++)
								{
									auto mask = (vuint64_t)1 << bit;
									if (value & mask)
									{
										auto nameIndex = powerOf2Names.Keys().IndexOf(mask);
										if (nameIndex != -1)
										{
											auto intersection = Ptr(new WfEnumItemIntersection);
											intersection->name.value = powerOf2Names.Values()[nameIndex];
											item->intersections.Add(intersection);
										}
									}
								}
								decl->items.Add(item);
							}
						}
					}
					else
					{
						for (auto&& [name, value] : items)
						{
							auto item = Ptr(new WfEnumItem);
							item->name.value = name;
							item->kind = WfEnumItemKind::Constant;
							item->number.value = u64tow(value);
							decl->items.Add(item);
						}
					}
					return decl;
				}

				Ptr<WfStructDeclaration> GenerateStructDecl(ITypeDescriptor* td)
				{
					auto decl = Ptr(new WfStructDeclaration);
					List<WString> fragments;
					GetTypeFragments(td, fragments);
					decl->name.value = fragments[fragments.Count() - 1];

					for (vint i = 0; i < td->GetPropertyCount(); i++)
					{
						auto propertyInfo = td->GetProperty(i);
						auto member = Ptr(new WfStructMember);
						member->name.value = propertyInfo->GetName();
						member->type = GetTypeFromTypeInfo(propertyInfo->GetReturn());
						decl->members.Add(member);
					}
					return decl;
				}

				WfClassDeclaration* FindOriginalClassDecl(WfLexicalScopeManager* manager, ITypeDescriptor* td)
				{
					for (vint i = 0; i < manager->declarationTypes.Count(); i++)
					{
						if (manager->declarationTypes.Values()[i].Obj() == td)
						{
							auto decl = manager->declarationTypes.Keys()[i].Cast<WfClassDeclaration>();
							if (decl)
							{
								return decl.Obj();
							}
						}
					}
					return nullptr;
				}

				Ptr<WfDeclaration> FindOriginalMemberDecl(WfLexicalScopeManager* manager, IMemberInfo* memberInfo)
				{
					for (vint i = 0; i < manager->declarationMemberInfos.Count(); i++)
					{
						if (manager->declarationMemberInfos.Values()[i].Obj() == memberInfo)
						{
							return manager->declarationMemberInfos.Keys()[i];
						}
					}
					return nullptr;
				}

				Ptr<WfClassDeclaration> GenerateInterfaceDecl(WfLexicalScopeManager* manager, ITypeDescriptor* td)
				{
					auto originalClassDecl = FindOriginalClassDecl(manager, td);
					if (!originalClassDecl) return nullptr;

					auto decl = Ptr(new WfClassDeclaration);
					List<WString> fragments;
					GetTypeFragments(td, fragments);
					decl->name.value = fragments[fragments.Count() - 1];
					decl->kind = WfClassKind::Interface;
					decl->constructorType = originalClassDecl->constructorType;

					// Copy attributes from the original interface declaration
					CopyAttributes(decl->attributes, originalClassDecl->attributes);

					// Copy base types from the original declaration
					for (auto baseType : originalClassDecl->baseTypes)
					{
						decl->baseTypes.Add(CopyType(baseType));
					}

					// Generate properties from reflection
					for (vint i = 0; i < td->GetPropertyCount(); i++)
					{
						auto propertyInfo = td->GetProperty(i);
						if (propertyInfo->GetOwnerTypeDescriptor() != td) continue;

						auto propDecl = Ptr(new WfPropertyDeclaration);
						propDecl->name.value = propertyInfo->GetName();
						propDecl->type = GetTypeFromTypeInfo(propertyInfo->GetReturn());

						if (auto getter = propertyInfo->GetGetter())
						{
							propDecl->getter.value = getter->GetName();
						}
						if (auto setter = propertyInfo->GetSetter())
						{
							propDecl->setter.value = setter->GetName();
						}
						if (auto valueChangedEvent = propertyInfo->GetValueChangedEvent())
						{
							propDecl->valueChangedEvent.value = valueChangedEvent->GetName();
						}

						// Copy attributes from original property declaration
						auto originalMemberDecl = FindOriginalMemberDecl(manager, propertyInfo);
						if (originalMemberDecl)
						{
							CopyAttributes(propDecl->attributes, originalMemberDecl->attributes);
						}

						decl->declarations.Add(propDecl);
					}

					// Generate events from reflection
					for (vint i = 0; i < td->GetEventCount(); i++)
					{
						auto eventInfo = td->GetEvent(i);
						if (eventInfo->GetOwnerTypeDescriptor() != td) continue;

						auto eventDecl = Ptr(new WfEventDeclaration);
						eventDecl->name.value = eventInfo->GetName();

						// Extract event argument types from handler type
						auto handlerType = eventInfo->GetHandlerType();
						if (handlerType && handlerType->GetDecorator() == ITypeInfo::SharedPtr)
						{
							auto genericType = handlerType->GetElementType();
							if (genericType && genericType->GetDecorator() == ITypeInfo::Generic)
							{
								// Skip first generic argument (return type is void)
								for (vint j = 1; j < genericType->GetGenericArgumentCount(); j++)
								{
									auto argType = GetTypeFromTypeInfo(genericType->GetGenericArgument(j));
									if (argType)
									{
										eventDecl->arguments.Add(argType);
									}
								}
							}
						}

						// Copy attributes from original event declaration
						auto originalMemberDecl = FindOriginalMemberDecl(manager, eventInfo);
						if (originalMemberDecl)
						{
							CopyAttributes(eventDecl->attributes, originalMemberDecl->attributes);
						}

						decl->declarations.Add(eventDecl);
					}

					// Generate methods from reflection
					for (vint i = 0; i < td->GetMethodGroupCount(); i++)
					{
						auto group = td->GetMethodGroup(i);
						if (group->GetOwnerTypeDescriptor() != td) continue;

						for (vint j = 0; j < group->GetMethodCount(); j++)
						{
							auto methodInfo = group->GetMethod(j);

							// Skip methods that are property getters/setters
							if (methodInfo->GetOwnerProperty()) continue;

							auto funcDecl = Ptr(new WfFunctionDeclaration);
							funcDecl->name.value = methodInfo->GetName();
							funcDecl->anonymity = WfFunctionAnonymity::Named;
							funcDecl->functionKind = WfFunctionKind::Normal;
							funcDecl->returnType = GetTypeFromTypeInfo(methodInfo->GetReturn());

							// Find original function declaration to copy argument attributes
							auto originalMemberDecl = FindOriginalMemberDecl(manager, methodInfo);
							auto originalFuncDecl = originalMemberDecl ? originalMemberDecl.Cast<WfFunctionDeclaration>() : Ptr<WfFunctionDeclaration>();

							for (vint k = 0; k < methodInfo->GetParameterCount(); k++)
							{
								auto parameterInfo = methodInfo->GetParameter(k);
								auto argument = Ptr(new WfFunctionArgument);
								argument->name.value = parameterInfo->GetName();
								argument->type = GetTypeFromTypeInfo(parameterInfo->GetType());

								// Copy attributes from original function argument
								if (originalFuncDecl && k < originalFuncDecl->arguments.Count())
								{
									CopyAttributes(argument->attributes, originalFuncDecl->arguments[k]->attributes);
								}

								funcDecl->arguments.Add(argument);
							}

							// Copy attributes from original method declaration
							if (originalMemberDecl)
							{
								CopyAttributes(funcDecl->attributes, originalMemberDecl->attributes);
							}

							decl->declarations.Add(funcDecl);
						}
					}

					return decl;
				}

				Ptr<WfNamespaceDeclaration> EnsureNamespace(
					List<Ptr<WfDeclaration>>& rootDeclarations,
					Dictionary<WString, Ptr<WfNamespaceDeclaration>>& namespaceMap,
					const List<WString>& fragments)
				{
					if (fragments.Count() <= 1)
					{
						return nullptr;
					}

					WString currentPath;
					Ptr<WfNamespaceDeclaration> currentNs;
					List<Ptr<WfDeclaration>>* currentList = &rootDeclarations;

					for (vint i = 0; i < fragments.Count() - 1; i++)
					{
						if (currentPath.Length() > 0)
						{
							currentPath = currentPath + L"::" + fragments[i];
						}
						else
						{
							currentPath = fragments[i];
						}

						auto index = namespaceMap.Keys().IndexOf(currentPath);
						if (index != -1)
						{
							currentNs = namespaceMap.Values()[index];
							currentList = &currentNs->declarations;
						}
						else
						{
							auto ns = Ptr(new WfNamespaceDeclaration);
							ns->name.value = fragments[i];
							currentList->Add(ns);
							namespaceMap.Add(currentPath, ns);
							currentNs = ns;
							currentList = &ns->declarations;
						}
					}

					return currentNs;
				}

				void AddDeclToModule(
					List<Ptr<WfDeclaration>>& rootDeclarations,
					Dictionary<WString, Ptr<WfNamespaceDeclaration>>& namespaceMap,
					ITypeDescriptor* td,
					Ptr<WfDeclaration> decl)
				{
					List<WString> fragments;
					GetTypeFragments(td, fragments);

					auto ns = EnsureNamespace(rootDeclarations, namespaceMap, fragments);
					if (ns)
					{
						ns->declarations.Add(decl);
					}
					else
					{
						rootDeclarations.Add(decl);
					}
				}

				void CollectOrderedRpcInterfaces(
					WfLexicalScopeManager* manager,
					const List<Ptr<WfDeclaration>>& declarations,
					const SortedList<ITypeDescriptor*>& rpcInterfaceTds,
					List<ITypeDescriptor*>& orderedInterfaces)
				{
					for (auto decl : declarations)
					{
						if (auto classDecl = decl.Cast<WfClassDeclaration>())
						{
							auto td = GetClassTypeDescriptor(manager, classDecl.Obj());
							if (td && rpcInterfaceTds.Contains(td) && !orderedInterfaces.Contains(td))
							{
								orderedInterfaces.Add(td);
							}
							CollectOrderedRpcInterfaces(manager, classDecl->declarations, rpcInterfaceTds, orderedInterfaces);
						}
						else if (auto nsDecl = decl.Cast<WfNamespaceDeclaration>())
						{
							CollectOrderedRpcInterfaces(manager, nsDecl->declarations, rpcInterfaceTds, orderedInterfaces);
						}
					}
				}

				void ValidateModuleRPC_GenerateMetadata(WfLexicalScopeManager* manager, Ptr<WfModule> module, const RpcPhase1Context& context)
				{
					// Collect all enum and struct types used by RPC interfaces
					SortedList<ITypeDescriptor*> collectedEnums;
					SortedList<ITypeDescriptor*> collectedStructs;
					List<ITypeDescriptor*> structVisitOrder;

					for (auto td : context.workflowRpcInterfaceTds)
					{
						CollectTypesFromInterface(td, collectedEnums, collectedStructs, structVisitOrder, context.workflowRpcInterfaceTds);
					}

					// Sort enums by name
					List<ITypeDescriptor*> sortedEnums;
					for (auto td : collectedEnums)
					{
						sortedEnums.Add(td);
					}
					if (sortedEnums.Count() > 1)
					{
						Sort(&sortedEnums[0], sortedEnums.Count(), [](ITypeDescriptor* a, ITypeDescriptor* b)
						{
							return wcscmp(a->GetTypeName().Buffer(), b->GetTypeName().Buffer()) <=> 0;
						});
					}

					// Collect RPC interfaces in AST order
					List<ITypeDescriptor*> orderedInterfaces;
					CollectOrderedRpcInterfaces(manager, module->declarations, context.workflowRpcInterfaceTds, orderedInterfaces);

					// Build the metadata module
					auto metadataModule = Ptr(new WfModule);
					metadataModule->moduleType = WfModuleType::Module;
					metadataModule->name.value = L"RpcMetadata";

					List<Ptr<WfDeclaration>> rootDeclarations;
					Dictionary<WString, Ptr<WfNamespaceDeclaration>> namespaceMap;

					// Add enums (sorted by name)
					for (auto td : sortedEnums)
					{
						auto decl = GenerateEnumDecl(td);
						if (decl)
						{
							AddDeclToModule(rootDeclarations, namespaceMap, td, decl);
						}
					}

					// Add structs (sorted by visiting order - dependency correct)
					for (auto td : structVisitOrder)
					{
						auto decl = GenerateStructDecl(td);
						if (decl)
						{
							AddDeclToModule(rootDeclarations, namespaceMap, td, decl);
						}
					}

					// Add interfaces (sorted by AST order)
					for (auto td : orderedInterfaces)
					{
						auto decl = GenerateInterfaceDecl(manager, td);
						if (decl)
						{
							AddDeclToModule(rootDeclarations, namespaceMap, td, decl);
						}
					}

					for (auto decl : rootDeclarations)
					{
						metadataModule->declarations.Add(decl);
					}

					manager->rpcMetadata = metadataModule;
				}
			}

			void ValidateModuleRPC(WfLexicalScopeManager* manager, Ptr<WfModule> module)
			{
				rpc_compiling::RpcPhase1Context context;

				// Phase 1 collects workflow-defined RPC interfaces and enqueues attribute occurrences for phase 2.
				rpc_compiling::ValidateModuleRPC_Ast(manager, module, context);

				// Phase 2 uses reflection (ITypeDescriptor/ITypeInfo) to validate the remaining rules.
				rpc_compiling::ValidateModuleRPC_Reflection(manager, context);

				// Phase 3 generates rpc metadata module.
				if (context.workflowRpcInterfaceTds.Count() > 0 && manager->errors.Count() == 0)
				{
					rpc_compiling::ValidateModuleRPC_GenerateMetadata(manager, module, context);
				}
			}
		}
	}
}
