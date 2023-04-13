#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace reflection::description;
			using namespace typeimpl;

/***********************************************************************
BuildGlobalNameFromModules
***********************************************************************/

			class BuildClassMemberVisitor
				: public Object
				, public WfDeclaration::IVisitor
				, public WfVirtualCseDeclaration::IVisitor
			{
			public:
				WfLexicalScopeManager*			manager;
				Ptr<WfLexicalScopeName>			scopeName;
				WfClassDeclaration*				classDecl;
				Ptr<WfCustomType>				td;

				BuildClassMemberVisitor(WfLexicalScopeManager* _manager, Ptr<WfLexicalScopeName> _scopeName, WfClassDeclaration* _classDecl, Ptr<WfCustomType> _td)
					:manager(_manager)
					, scopeName(_scopeName)
					, classDecl(_classDecl)
					, td(_td)
				{
				}

				static WString GetTypeName(WfLexicalScopeManager* manager, Ptr<WfLexicalScopeName> scopeName)
				{
					WString typeName = scopeName->name;
					{
						WfLexicalScopeName* name = scopeName->parent;
						while (name && name != manager->globalName.Obj())
						{
							typeName = name->name + L"::" + typeName;
							name = name->parent;
						}
					}
					return typeName;
				}

				static void AddCustomType(WfLexicalScopeManager* manager, Ptr<WfLexicalScopeName> scopeName, WfDeclaration* declaration, Ptr<ITypeDescriptor> td)
				{
					manager->declarationTypes.Add(Ptr(declaration), td);

					if (!scopeName->typeDescriptor)
					{
						scopeName->typeDescriptor = td.Obj();
						manager->typeNames.Add(td.Obj(), scopeName);
					}
				}

				static void BuildClass(WfLexicalScopeManager* manager, Ptr<WfLexicalScopeName> scopeName, WfClassDeclaration* declaration)
				{
					WString typeName = GetTypeName(manager, scopeName);
					Ptr<WfCustomType> td;
					switch (declaration->kind)
					{
					case WfClassKind::Class:
						td = Ptr(new WfClass(typeName));
						break;
					case WfClassKind::Interface:
						td = Ptr(new WfInterface(typeName));
						break;
					default:;
					}
					AddCustomType(manager, scopeName, declaration, td);

					for (auto memberDecl : declaration->declarations)
					{
						BuildClassMemberVisitor visitor(manager, scopeName, declaration, td);
						memberDecl->Accept(&visitor);
					}
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					if (node->functionKind == WfFunctionKind::Static)
					{
						auto info = Ptr(new WfStaticMethod);
						td->AddMember(node->name.value, info);
						manager->declarationMemberInfos.Add(Ptr(node), info);
					}
					else
					{
						switch (classDecl->kind)
						{
						case WfClassKind::Class:
							{
								auto info = Ptr(new WfClassMethod);
								td->AddMember(node->name.value, info);
								manager->declarationMemberInfos.Add(Ptr(node), info);
							}
							break;
						case WfClassKind::Interface:
							{
								auto info = Ptr(new WfInterfaceMethod);
								td->AddMember(node->name.value, info);
								manager->declarationMemberInfos.Add(Ptr(node), info);
							}
							break;
						default:;
						}
					}
				}

				void Visit(WfVariableDeclaration* node)override
				{
					auto info = Ptr(new WfField(td.Obj(), node->name.value));
					td->AddMember(info);
					manager->declarationMemberInfos.Add(Ptr(node), info);
				}

				void Visit(WfEventDeclaration* node)override
				{
					auto info = Ptr(new WfEvent(td.Obj(), node->name.value));
					td->AddMember(info);
					manager->declarationMemberInfos.Add(Ptr(node), info);
				}

				void Visit(WfPropertyDeclaration* node)override
				{
					auto info = Ptr(new WfProperty(td.Obj(), node->name.value));
					td->AddMember(info);
					manager->declarationMemberInfos.Add(Ptr(node), info);
				}

				void Visit(WfStaticInitDeclaration* node)override
				{
				}

				void Visit(WfConstructorDeclaration* node)override
				{
					Ptr<ITypeInfo> typeInfo;
					{
						auto elementType = Ptr(new TypeDescriptorTypeInfo(td.Obj(), TypeInfoHint::Normal));
						if (node->constructorType == WfConstructorType::RawPtr)
						{
							typeInfo = Ptr(new RawPtrTypeInfo(elementType));
						}
						else
						{
							typeInfo = Ptr(new SharedPtrTypeInfo(elementType));
						}
					}

					auto info = Ptr(new WfClassConstructor(typeInfo));
					td->AddMember(info);
					manager->declarationMemberInfos.Add(Ptr(node), info);
				}
				
				void Visit(WfDestructorDeclaration* node)override
				{
				}

				void Visit(WfClassDeclaration* node)override
				{
					auto newScopeName = scopeName->AccessChild(node->name.value, false);
					newScopeName->declarations.Add(Ptr(node));
					BuildClass(manager, newScopeName, node);
				}

				void Visit(WfEnumDeclaration* node)override
				{
					BuildNameForDeclaration(manager, scopeName, Ptr(node));
				}

				void Visit(WfStructDeclaration* node)override
				{
					BuildNameForDeclaration(manager, scopeName, Ptr(node));
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
					for (auto input : node->inputs)
					{
						auto info = Ptr(new WfClassMethod);
						td->AddMember(input->name.value, info);
						manager->stateInputMethods.Add(input, info);

						for (auto argument : input->arguments)
						{
							auto info = Ptr(new WfField(td.Obj(), L"<stateip-" + input->name.value + L">" + argument->name.value));
							td->AddMember(info);
							manager->stateInputArguments.Add(argument, info);
						}
					}
					
					for (auto state : node->states)
					{
						for (auto argument : state->arguments)
						{
							auto info = Ptr(new WfField(td.Obj(), L"<statesp-" + state->name.value + L">" + argument->name.value));
							td->AddMember(info);
							manager->stateDeclArguments.Add(argument, info);
						}
					}

					auto smInfo = Ptr(new WfStateMachineInfo);
					{
						smInfo->createCoroutineMethod = Ptr(new WfClassMethod);
						td->AddMember(L"<state>CreateCoroutine", smInfo->createCoroutineMethod);
					}
					manager->stateMachineInfos.Add(Ptr(node), smInfo);
				}
			};

			class BuildNameDeclarationVisitor
				: public empty_visitor::DeclarationVisitor
				, public empty_visitor::VirtualCseDeclarationVisitor
			{
			public:
				WfLexicalScopeManager*			manager;
				Ptr<WfLexicalScopeName>			scopeName;

				BuildNameDeclarationVisitor(WfLexicalScopeManager* _manager, Ptr<WfLexicalScopeName> _scopeName)
					:manager(_manager)
					, scopeName(_scopeName)
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
					node->Accept((WfVirtualCseDeclaration::IVisitor*)this);
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					manager->namespaceNames.Add(Ptr(node), scopeName);
					for (auto subDecl : node->declarations)
					{
						BuildNameForDeclaration(manager, scopeName, subDecl);
					}
				}

				void Visit(WfClassDeclaration* node)override
				{
					BuildClassMemberVisitor::BuildClass(manager, scopeName, node);
				}

				void Visit(WfEnumDeclaration* node)override
				{
					auto td = Ptr(new WfEnum(node->kind == WfEnumKind::Flag, BuildClassMemberVisitor::GetTypeName(manager, scopeName)));
					BuildClassMemberVisitor::AddCustomType(manager, scopeName, node, td);
				}

				void Visit(WfStructDeclaration* node)override
				{
					auto td = Ptr(new WfStruct(BuildClassMemberVisitor::GetTypeName(manager, scopeName)));
					BuildClassMemberVisitor::AddCustomType(manager, scopeName, node, td);
				}
			};

			void BuildNameForDeclaration(WfLexicalScopeManager* manager, Ptr<WfLexicalScopeName> name, Ptr<WfDeclaration> decl)
			{
				auto scopeName = name->AccessChild(decl->name.value, false);
				scopeName->declarations.Add(decl);

				BuildNameDeclarationVisitor visitor(manager, scopeName);
				decl->Accept(&visitor);
			}

			void BuildGlobalNameFromModules(WfLexicalScopeManager* manager)
			{
				for (auto module : manager->GetModules())
				{
					for (auto decl : module->declarations)
					{
						BuildNameForDeclaration(manager, manager->globalName, decl);
					}
				}
			}
		}
	}
}