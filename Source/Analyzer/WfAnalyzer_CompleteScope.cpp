#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace parsing;
			using namespace reflection;
			using namespace reflection::description;
			using namespace typeimpl;

/***********************************************************************
CompleteScopeForClassMember
***********************************************************************/

			class CompleteScopeForClassMemberVisitor
				: public Object
				, public WfDeclaration::IVisitor
				, public WfVirtualCseDeclaration::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				Ptr<WfCustomType>						td;
				Ptr<WfClassDeclaration>					classDecl;

				CompleteScopeForClassMemberVisitor(WfLexicalScopeManager* _manager, Ptr<WfCustomType> _td, Ptr<WfClassDeclaration> _classDecl)
					:manager(_manager)
					, td(_td)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
					auto info = manager->declarationMemberInfos[node].Cast<WfMethodBase>();

					FOREACH(Ptr<WfFunctionArgument>, argument, node->arguments)
					{
						if (auto typeInfo = CreateTypeInfoFromType(scope.Obj(), argument->type))
						{
							auto paramInfo = MakePtr<ParameterInfoImpl>(info.Obj(), argument->name.value, typeInfo);
							info->AddParameter(paramInfo);
						}
					}

					if (auto typeInfo = CreateTypeInfoFromType(scope.Obj(), node->returnType))
					{
						info->SetReturn(typeInfo);
					}
				}

				void Visit(WfVariableDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
					if (auto typeInfo = CreateTypeInfoFromType(scope.Obj(), node->type))
					{
						auto info = manager->declarationMemberInfos[node].Cast<WfField>();
						info->SetReturn(typeInfo);
					}
				}

				void Visit(WfEventDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
					auto type = MakePtr<WfFunctionType>();
					{
						auto voidType = MakePtr<WfPredefinedType>();
						voidType->name = WfPredefinedTypeName::Void;
						type->result = voidType;
					}
					FOREACH(Ptr<WfType>, argument, node->arguments)
					{
						type->arguments.Add(argument);
					}

					if (auto typeInfo = CreateTypeInfoFromType(scope.Obj(), type))
					{
						auto info = manager->declarationMemberInfos[node].Cast<WfEvent>();
						info->SetHandlerType(typeInfo);
					}
				}

				void Visit(WfPropertyDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
					auto info = manager->declarationMemberInfos[node].Cast<WfProperty>();

					if (node->getter.value != L"")
					{
						info->SetGetter(dynamic_cast<MethodInfoImpl*>(td->GetMethodGroupByName(node->getter.value, false)->GetMethod(0)));
					}

					if (node->setter.value != L"")
					{
						info->SetSetter(dynamic_cast<MethodInfoImpl*>(td->GetMethodGroupByName(node->setter.value, false)->GetMethod(0)));
					}

					if (node->valueChangedEvent.value != L"")
					{
						info->SetValueChangedEvent(dynamic_cast<EventInfoImpl*>(td->GetEventByName(node->valueChangedEvent.value, false)));
					}
				}

				void Visit(WfConstructorDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
					auto info = manager->declarationMemberInfos[node].Cast<WfClassConstructor>();

					FOREACH(Ptr<WfFunctionArgument>, argument, node->arguments)
					{
						if (auto typeInfo = CreateTypeInfoFromType(scope.Obj(), argument->type))
						{
							auto paramInfo = MakePtr<ParameterInfoImpl>(info.Obj(), argument->name.value, typeInfo);
							info->AddParameter(paramInfo);
						}
					}
				}

				void Visit(WfDestructorDeclaration* node)override
				{
				}

				void Visit(WfClassDeclaration* node)override
				{
					CompleteScopeForDeclaration(manager, node);
				}

				void Visit(WfEnumDeclaration* node)override
				{
					CompleteScopeForDeclaration(manager, node);
				}

				void Visit(WfStructDeclaration* node)override
				{
					CompleteScopeForDeclaration(manager, node);
				}

				void Visit(WfVirtualCfeDeclaration* node)override
				{
					FOREACH(Ptr<WfDeclaration>, decl, node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Visit(WfVirtualCseDeclaration* node)override
				{
					node->Accept((WfVirtualCseDeclaration::IVisitor*)this);
				}

				void Visit(WfStateMachineDeclaration* node)
				{
					auto scope = manager->nodeScopes[node];
					FOREACH(Ptr<WfStateInput>, input, node->inputs)
					{
						auto method = manager->stateInputMethods[input.Obj()];
						method->SetReturn(TypeInfoRetriver<void>::CreateTypeInfo());

						FOREACH(Ptr<WfFunctionArgument>, argument, input->arguments)
						{
							if (auto typeInfo = CreateTypeInfoFromType(scope.Obj(), argument->type))
							{
								auto field = manager->stateInputArguments[argument.Obj()];
								field->SetReturn(typeInfo);

								method->AddParameter(MakePtr<ParameterInfoImpl>(method.Obj(), argument->name.value, typeInfo));
							}
						}
					}

					FOREACH(Ptr<WfStateDeclaration>, state, node->states)
					{
						FOREACH(Ptr<WfFunctionArgument>, argument, state->arguments)
						{
							if (auto typeInfo = CreateTypeInfoFromType(scope.Obj(), argument->type))
							{
								auto field = manager->stateDeclArguments[argument.Obj()];
								field->SetReturn(typeInfo);
							}
						}
					}

					auto& smInfo = manager->stateMachineInfos[node];
					smInfo.createCoroutineMethod->AddParameter(MakePtr<ParameterInfoImpl>(smInfo.createCoroutineMethod.Obj(), L"<state>startState", TypeInfoRetriver<vint>::CreateTypeInfo()));
					smInfo.createCoroutineMethod->SetReturn(TypeInfoRetriver<void>::CreateTypeInfo());
				}

				static void Execute(WfLexicalScopeManager* manager, Ptr<WfCustomType> td, Ptr<WfClassDeclaration> classDecl, Ptr<WfDeclaration> memberDecl)
				{
					CompleteScopeForClassMemberVisitor visitor(manager, td, classDecl);
					memberDecl->Accept(&visitor);
				}
			};

/***********************************************************************
CompleteScopeForDeclaration
***********************************************************************/

			class CompleteScopeForDeclarationVisitor
				: public Object
				, public WfDeclaration::IVisitor
				, public WfVirtualCseDeclaration::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				Ptr<WfDeclaration>						declaration;

				CompleteScopeForDeclarationVisitor(WfLexicalScopeManager* _manager, Ptr<WfDeclaration> _declaration)
					:manager(_manager)
					, declaration(_declaration)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					FOREACH(Ptr<WfDeclaration>, decl, node->declarations)
					{
						CompleteScopeForDeclaration(manager, decl);
					}
				}

				void Visit(WfFunctionDeclaration* node)override
				{
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

					if (node->baseTypes.Count() > 0)
					{
						FOREACH(Ptr<WfType>, baseType, node->baseTypes)
						{
							if (auto scopeName = GetScopeNameFromReferenceType(scope->parentScope.Obj(), baseType))
							{
								if (scopeName->typeDescriptor)
								{
									td->AddBaseType(scopeName->typeDescriptor);
								}
							}
						}
					}
					else if (node->kind == WfClassKind::Class)
					{
						td->AddBaseType(description::GetTypeDescriptor<DescriptableObject>());
					}
					else if (node->kind == WfClassKind::Interface)
					{
						td->AddBaseType(description::GetTypeDescriptor<IDescriptable>());
					}

					if (node->kind == WfClassKind::Interface)
					{
						switch (node->constructorType)
						{
						case WfConstructorType::SharedPtr:
							{
								auto elementType = MakePtr<TypeDescriptorTypeInfo>(td.Obj(), TypeInfoHint::Normal);
								auto pointerType = MakePtr<SharedPtrTypeInfo>(elementType);

								auto ctor = MakePtr<WfInterfaceConstructor>(pointerType);
								td->AddMember(ctor);
							}
							break;
						case WfConstructorType::RawPtr:
							{
								auto elementType = MakePtr<TypeDescriptorTypeInfo>(td.Obj(), TypeInfoHint::Normal);
								auto pointerType = MakePtr<RawPtrTypeInfo>(elementType);

								auto ctor = MakePtr<WfInterfaceConstructor>(pointerType);
								td->AddMember(ctor);
							}
							break;
						default:;
						}
					}

					FOREACH(Ptr<WfDeclaration>, memberDecl, node->declarations)
					{
						CompleteScopeForClassMember(manager, td, node, memberDecl);
					}
				}

				void Visit(WfEnumDeclaration* node)override
				{
					auto td = manager->declarationTypes[node].Cast<WfEnum>();
					Dictionary<WString, vuint64_t> items;
					FOREACH(Ptr<WfEnumItem>, item, node->items)
					{
						vuint64_t value = 0;
						switch (item->kind)
						{
						case WfEnumItemKind::Constant:
							TypedValueSerializerProvider<vuint64_t>::Deserialize(item->number.value, value);
							break;
						case WfEnumItemKind::Intersection:
							FOREACH(Ptr<WfEnumItemIntersection>, itemInt, item->intersections)
							{
								value |= items[itemInt->name.value];
							}
							break;
						}
						td->AddEnumItem(item->name.value, value);
						items.Add(item->name.value, value);
					}
				}

				void Visit(WfStructDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
					auto td = manager->declarationTypes[node].Cast<WfStruct>();
					FOREACH(Ptr<WfStructMember>, member, node->members)
					{
						if (auto typeInfo = CreateTypeInfoFromType(scope.Obj(), member->type))
						{
							auto field = MakePtr<WfStructField>(td.Obj(), member->name.value);
							field->SetReturn(typeInfo);
							td->AddMember(field);
						}
					}
				}

				void Visit(WfVirtualCfeDeclaration* node)override
				{
					FOREACH(Ptr<WfDeclaration>, decl, node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Visit(WfVirtualCseDeclaration* node)override
				{
					node->Accept((WfVirtualCseDeclaration::IVisitor*)this);
					FOREACH(Ptr<WfDeclaration>, decl, node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Visit(WfStateMachineDeclaration* node)override
				{
				}

				static void Execute(WfLexicalScopeManager* manager, Ptr<WfDeclaration> declaration)
				{
					CompleteScopeForDeclarationVisitor visitor(manager, declaration);
					declaration->Accept(&visitor);
				}
			};

/***********************************************************************
CheckBaseClass
***********************************************************************/

			class CheckBaseClassDeclarationVisitor
				: public empty_visitor::DeclarationVisitor
				, public empty_visitor::VirtualCseDeclarationVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				SortedList<ITypeDescriptor*>			checkedInterfaces;
				SortedList<ITypeDescriptor*>			traversedInterfaces;

				CheckBaseClassDeclarationVisitor(WfLexicalScopeManager* _manager)
					:manager(_manager)
				{
				}

				void Dispatch(WfVirtualCfeDeclaration* node)override
				{
					FOREACH(Ptr<WfDeclaration>, decl, node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Dispatch(WfVirtualCseDeclaration* node)override
				{
					node->Accept((WfVirtualCseDeclaration::IVisitor*)this);
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					FOREACH(Ptr<WfDeclaration>, decl, node->declarations)
					{
						decl->Accept(this);
					}
				}

				void CheckDuplicatedBaseClass(WfClassDeclaration* node, ITypeDescriptor* td)
				{
					List<ITypeDescriptor*> baseTypes;
					SortedList<ITypeDescriptor*> duplicatedTypes;
					baseTypes.Add(td);

					for (vint i = 0; i < baseTypes.Count(); i++)
					{
						auto currentTd = baseTypes[i];
						vint count = currentTd->GetBaseTypeDescriptorCount();
						for (vint j = 0; j < count; j++)
						{
							auto baseTd = currentTd->GetBaseTypeDescriptor(j);
							if (baseTd->GetTypeDescriptorFlags() == TypeDescriptorFlags::Class && baseTd != description::GetTypeDescriptor<DescriptableObject>())
							{
								if (baseTypes.Contains(baseTd))
								{
									if (!duplicatedTypes.Contains(baseTd))
									{
										duplicatedTypes.Add(baseTd);
										manager->errors.Add(WfErrors::DuplicatedBaseClass(node, baseTd));
									}
								}
								else
								{
									baseTypes.Add(baseTd);
								}
							}
						}
					}
				}

				void CheckDuplicatedBaseInterface(WfClassDeclaration* node, ITypeDescriptor* td)
				{
					if (traversedInterfaces.Contains(td))
					{
						manager->errors.Add(WfErrors::DuplicatedBaseInterface(node, td));
					}
					else
					{
						if (checkedInterfaces.Contains(td))
						{
							return;
						}
						checkedInterfaces.Add(td);

						vint index = traversedInterfaces.Add(td);
						vint count = td->GetBaseTypeDescriptorCount();
						for (vint i = 0; i < count; i++)
						{
							CheckDuplicatedBaseInterface(node, td->GetBaseTypeDescriptor(i));
						}
						traversedInterfaces.RemoveAt(index);
					}
				}

				void Visit(WfClassDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
					auto td = manager->declarationTypes[node].Obj();

					FOREACH(Ptr<WfType>, baseType, node->baseTypes)
					{
						if (auto scopeName = GetScopeNameFromReferenceType(scope->parentScope.Obj(), baseType))
						{
							if (auto baseTd = scopeName->typeDescriptor)
							{
								bool isClass = baseTd->GetTypeDescriptorFlags() == TypeDescriptorFlags::Class;
								bool isInterface = baseTd->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface;

								switch (node->kind)
								{
								case WfClassKind::Class:
									{
										if (!isClass || !baseTd->IsAggregatable())
										{
											if (!dynamic_cast<WfClass*>(baseTd))
											{
												manager->errors.Add(WfErrors::WrongBaseTypeOfClass(node, baseTd));
											}
										}
									}
									break;
								case WfClassKind::Interface:
									{
										if (!isInterface)
										{
											manager->errors.Add(WfErrors::WrongBaseTypeOfInterface(node, baseTd));
										}
									}
									break;
								}
							}
						}
					}

					if (node->kind == WfClassKind::Class)
					{
						CheckDuplicatedBaseClass(node, td);
					}
					else
					{
						CheckDuplicatedBaseInterface(node, td);
					}

					FOREACH(Ptr<WfDeclaration>, memberDecl, node->declarations)
					{
						memberDecl->Accept(this);
					}
				}

				static void Execute(WfLexicalScopeManager* manager, Ptr<WfDeclaration> declaration)
				{
					CompleteScopeForDeclarationVisitor visitor(manager, declaration);
					declaration->Accept(&visitor);
				}
			};

/***********************************************************************
CompleteScope
***********************************************************************/

			void CompleteScopeForClassMember(WfLexicalScopeManager* manager, Ptr<WfCustomType> td, Ptr<WfClassDeclaration> classDecl, Ptr<WfDeclaration> memberDecl)
			{
				CompleteScopeForClassMemberVisitor::Execute(manager, td, classDecl, memberDecl);
			}

			void CompleteScopeForDeclaration(WfLexicalScopeManager* manager, Ptr<WfDeclaration> declaration)
			{
				CompleteScopeForDeclarationVisitor::Execute(manager, declaration);
			}

			void CompleteScopeForModule(WfLexicalScopeManager* manager, Ptr<WfModule> module)
			{
				FOREACH(Ptr<WfDeclaration>, declaration, module->declarations)
				{
					CompleteScopeForDeclaration(manager, declaration);
				}
			}

/***********************************************************************
CheckScopes_SymbolType
***********************************************************************/

			bool CheckScopes_SymbolType(WfLexicalScopeManager* manager)
			{
				SortedList<WfLexicalScope*> analyzedScopes;
				vint errorCount = manager->errors.Count();
				FOREACH(Ptr<WfLexicalScope>, scope, manager->nodeScopes.Values())
				{
					if (!analyzedScopes.Contains(scope.Obj()))
					{
						analyzedScopes.Add(scope.Obj());

						for (vint i = 0; i < scope->symbols.Count(); i++)
						{
							FOREACH(Ptr<WfLexicalSymbol>, symbol, scope->symbols.GetByIndex(i))
							{
								if (symbol->type)
								{
									vint currentErrorCount = manager->errors.Count();
									symbol->typeInfo = CreateTypeInfoFromType(scope.Obj(), symbol->type);
									if (symbol->creatorNode)
									{
										for (vint i = currentErrorCount; i < manager->errors.Count(); i++)
										{
											manager->errors[i]->parsingTree = symbol->creatorNode.Obj();
										}
									}
								}
							}
						}
					}
				}
				return errorCount == manager->errors.Count();
			}

/***********************************************************************
CheckScopes_BaseType
***********************************************************************/

			bool CheckScopes_BaseType(WfLexicalScopeManager* manager)
			{
				vint errorCount = manager->errors.Count();
				CheckBaseClassDeclarationVisitor visitor(manager);
				FOREACH(Ptr<WfModule>, module, manager->GetModules())
				{
					FOREACH(Ptr<WfDeclaration>, declaration, module->declarations)
					{
						declaration->Accept(&visitor);
					}
				}
				return errorCount == manager->errors.Count();
			}
		}
	}
}