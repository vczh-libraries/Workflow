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

					for (auto argument : node->arguments)
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
					for (auto argument : node->arguments)
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

					for (auto argument : node->arguments)
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
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Visit(WfVirtualCseDeclaration* node)override
				{
					node->Accept((WfVirtualCseDeclaration::IVisitor*)this);
				}

				void Visit(WfStateMachineDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
					for (auto input : node->inputs)
					{
						auto method = manager->stateInputMethods[input.Obj()];
						method->SetReturn(TypeInfoRetriver<void>::CreateTypeInfo());

						for (auto argument : input->arguments)
						{
							if (auto typeInfo = CreateTypeInfoFromType(scope.Obj(), argument->type))
							{
								auto field = manager->stateInputArguments[argument.Obj()];
								field->SetReturn(typeInfo);

								method->AddParameter(MakePtr<ParameterInfoImpl>(method.Obj(), argument->name.value, typeInfo));
							}
						}
					}

					for (auto state : node->states)
					{
						for (auto argument : state->arguments)
						{
							if (auto typeInfo = CreateTypeInfoFromType(scope.Obj(), argument->type))
							{
								auto field = manager->stateDeclArguments[argument.Obj()];
								field->SetReturn(typeInfo);
							}
						}
					}

					auto& smInfo = manager->stateMachineInfos[node];
					smInfo->createCoroutineMethod->AddParameter(MakePtr<ParameterInfoImpl>(smInfo->createCoroutineMethod.Obj(), L"<state>startState", TypeInfoRetriver<vint>::CreateTypeInfo()));
					smInfo->createCoroutineMethod->SetReturn(TypeInfoRetriver<void>::CreateTypeInfo());
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
					for (auto decl : node->declarations)
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
						for (auto baseType : node->baseTypes)
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

					for (auto memberDecl : node->declarations)
					{
						CompleteScopeForClassMember(manager, td, node, memberDecl);
					}
				}

				void Visit(WfEnumDeclaration* node)override
				{
					auto td = manager->declarationTypes[node].Cast<WfEnum>();
					Dictionary<WString, vuint64_t> items;
					for (auto item : node->items)
					{
						vuint64_t value = 0;
						switch (item->kind)
						{
						case WfEnumItemKind::Constant:
							TypedValueSerializerProvider<vuint64_t>::Deserialize(item->number.value, value);
							break;
						case WfEnumItemKind::Intersection:
							for (auto itemInt : item->intersections)
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
					for (auto member : node->members)
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
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Visit(WfVirtualCseDeclaration* node)override
				{
					node->Accept((WfVirtualCseDeclaration::IVisitor*)this);
					for (auto decl : node->expandedDeclarations)
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
				for (auto declaration : module->declarations)
				{
					CompleteScopeForDeclaration(manager, declaration);
				}
			}
		}
	}
}
