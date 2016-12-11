#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace reflection::description;
			using namespace typeimpl;

/***********************************************************************
BuildGlobalNameFromTypeDescriptors
***********************************************************************/

			void BuildGlobalNameFromTypeDescriptors(WfLexicalScopeManager* manager)
			{
				for (vint i = 0; i < GetGlobalTypeManager()->GetTypeDescriptorCount(); i++)
				{
					ITypeDescriptor* typeDescriptor = GetGlobalTypeManager()->GetTypeDescriptor(i);
					WString name = typeDescriptor->GetTypeName();
					const wchar_t* reading = name.Buffer();
					Ptr<WfLexicalScopeName> currentName = manager->globalName;

					while (true)
					{
						WString fragment;
						const wchar_t* delimiter = wcsstr(reading, L"::");
						if (delimiter)
						{
							fragment = WString(reading, vint(delimiter - reading));
							reading = delimiter + 2;
						}
						else
						{
							fragment = reading;
							reading = 0;
						}

						currentName = currentName->AccessChild(fragment, true);
						if (!reading)
						{
							currentName->typeDescriptor = typeDescriptor;
							manager->typeNames.Add(typeDescriptor, currentName);
							break;
						}
					}
				}
			}

/***********************************************************************
BuildGlobalNameFromModules
***********************************************************************/

			class BuildClassMemberVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfLexicalScopeManager*			manager;
				Ptr<WfLexicalScopeName>			scopeName;
				Ptr<WfClassDeclaration>			classDecl;
				Ptr<WfCustomType>				td;
				Ptr<WfClassMember>				member;

				BuildClassMemberVisitor(WfLexicalScopeManager* _manager, Ptr<WfLexicalScopeName> _scopeName, Ptr<WfClassDeclaration> _classDecl, Ptr<WfCustomType> _td, Ptr<WfClassMember> _member)
					:manager(_manager)
					, scopeName(_scopeName)
					, classDecl(_classDecl)
					, td(_td)
					, member(_member)
				{
				}

				static void BuildClass(WfLexicalScopeManager* manager, Ptr<WfLexicalScopeName> scopeName, Ptr<WfClassDeclaration> declaration)
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

					Ptr<WfCustomType> td;
					switch (declaration->kind)
					{
					case WfClassKind::Class:
						td = MakePtr<WfClass>(typeName);
						break;
					case WfClassKind::Interface:
						td = MakePtr<WfInterface>(typeName);
						break;
					}
					manager->declarationTypes.Add(declaration, td);

					if (!scopeName->typeDescriptor)
					{
						scopeName->typeDescriptor = td.Obj();
						manager->typeNames.Add(td.Obj(), scopeName);
					}

					FOREACH(Ptr<WfClassMember>, member, declaration->members)
					{
						BuildClassMemberVisitor visitor(manager, scopeName, declaration, td, member);
						member->declaration->Accept(&visitor);
					}
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					if (member->kind == WfClassMemberKind::Static)
					{
						auto info = MakePtr<WfStaticMethod>();
						td->AddMember(node->name.value, info);
						manager->declarationMemberInfos.Add(node, info);
					}
					else
					{
						switch (classDecl->kind)
						{
						case WfClassKind::Class:
							{
								auto info = MakePtr<WfClassMethod>();
								td->AddMember(node->name.value, info);
								manager->declarationMemberInfos.Add(node, info);
							}
							break;
						case WfClassKind::Interface:
							{
								auto info = MakePtr<WfInterfaceMethod>();
								td->AddMember(node->name.value, info);
								manager->declarationMemberInfos.Add(node, info);
							}
							break;
						}
					}
				}

				void Visit(WfVariableDeclaration* node)override
				{
					auto info = MakePtr<WfField>(td.Obj(), node->name.value);
					td->AddMember(info);
					manager->declarationMemberInfos.Add(node, info);
				}

				void Visit(WfEventDeclaration* node)override
				{
					auto info = MakePtr<WfEvent>(td.Obj(), node->name.value);
					td->AddMember(info);
					manager->declarationMemberInfos.Add(node, info);
				}

				void Visit(WfPropertyDeclaration* node)override
				{
					auto info = MakePtr<WfProperty>(td.Obj(), node->name.value);
					td->AddMember(info);
					manager->declarationMemberInfos.Add(node, info);
				}

				void Visit(WfConstructorDeclaration* node)override
				{
					Ptr<ITypeInfo> typeInfo;
					{
						auto elementType = MakePtr<TypeDescriptorTypeInfo>(td.Obj(), TypeInfoHint::Normal);
						if (node->constructorType == WfConstructorType::RawPtr)
						{
							typeInfo = MakePtr<RawPtrTypeInfo>(elementType);
						}
						else
						{
							typeInfo = MakePtr<SharedPtrTypeInfo>(elementType);
						}
					}

					auto info = MakePtr<WfClassConstructor>(typeInfo);
					td->AddMember(info);
					manager->declarationMemberInfos.Add(node, info);
				}
				
				void Visit(WfDestructorDeclaration* node)override
				{
				}

				void Visit(WfClassDeclaration* node)override
				{
					auto newScopeName = scopeName->AccessChild(node->name.value, false);
					newScopeName->declarations.Add(node);
					BuildClass(manager, newScopeName, node);
				}
			};

			class BuildNameDeclarationVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfLexicalScopeManager*			manager;
				Ptr<WfLexicalScopeName>			scopeName;

				BuildNameDeclarationVisitor(WfLexicalScopeManager* _manager, Ptr<WfLexicalScopeName> _scopeName)
					:manager(_manager)
					, scopeName(_scopeName)
				{
				}

				static void BuildName(WfLexicalScopeManager* manager, Ptr<WfLexicalScopeName> name, Ptr<WfDeclaration> declaration)
				{
					auto scopeName = name->AccessChild(declaration->name.value, false);
					scopeName->declarations.Add(declaration);

					BuildNameDeclarationVisitor visitor(manager, scopeName);
					declaration->Accept(&visitor);
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					manager->namespaceNames.Add(node, scopeName);
					FOREACH(Ptr<WfDeclaration>, subDecl, node->declarations)
					{
						BuildName(manager, scopeName, subDecl);
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
					BuildClassMemberVisitor::BuildClass(manager, scopeName, node);
				}
			};

			void BuildGlobalNameFromModules(WfLexicalScopeManager* manager)
			{
				FOREACH(Ptr<WfModule>, module, manager->GetModules())
				{
					FOREACH(Ptr<WfDeclaration>, declaration, module->declarations)
					{
						BuildNameDeclarationVisitor::BuildName(manager, manager->globalName, declaration);
					}
				}
			}

/***********************************************************************
ValidateScopeName
***********************************************************************/

			class ValidateScopeNameDeclarationVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				enum Category
				{
					None,
					Type,
					Variable,
					Function,
					Event,
					Property,
					Namespace,
				};

				WfLexicalScopeManager*				manager;
				Ptr<WfLexicalScopeName>				name;
				Category							category;

				ValidateScopeNameDeclarationVisitor(WfLexicalScopeManager* _manager, Ptr<WfLexicalScopeName> _name)
					:manager(_manager)
					, name(_name)
					, category(name->typeDescriptor && name->imported ? Type : None)
				{
				}

				void AddError(WfDeclaration* node)
				{
					WString categoryName;
					switch (category)
					{
					case Type:
						categoryName = L"type";
						break;
					case Variable:
						categoryName = L"variable";
						break;
					case Function:
						categoryName = L"function";
						break;
					case Event:
						categoryName = L"event";
						break;
					case Property:
						categoryName = L"property";
						break;
					case Namespace:
						categoryName = L"namespace";
						break;
					default:
						CHECK_FAIL(L"ValidateScopeNameDeclarationVisitor::AddError(WfDeclaration*)#Internal error.");
					}
					manager->errors.Add(WfErrors::DuplicatedDeclaration(node, categoryName));
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					if (category == None)
					{
						category = Namespace;
					}
					else if (category != Namespace)
					{
						AddError(node);
					}
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					if (category == None)
					{
						category = Function;
					}
					else if (category != Function)
					{
						AddError(node);
					}
				}

				void Visit(WfVariableDeclaration* node)override
				{
					if (category == None)
					{
						category = Variable;
					}
					else
					{
						AddError(node);
					}
				}

				void Visit(WfEventDeclaration* node)override
				{
					if (category == None)
					{
						category = Event;
					}
					else
					{
						AddError(node);
					}
				}

				void Visit(WfPropertyDeclaration* node)override
				{
					if (category == None)
					{
						category = Property;
					}
					else
					{
						AddError(node);
					}
				}

				void Visit(WfConstructorDeclaration* node)override
				{
				}
				
				void Visit(WfDestructorDeclaration* node)override
				{
				}

				void Visit(WfClassDeclaration* node)override
				{
					if (category == None)
					{
						category = Type;
					}
					else
					{
						AddError(node);
					}
				}
			};

			void ValidateScopeName(WfLexicalScopeManager* manager, Ptr<WfLexicalScopeName> name)
			{
				ValidateScopeNameDeclarationVisitor visitor(manager, name);
				FOREACH(Ptr<WfDeclaration>, declaration, name->declarations)
				{
					declaration->Accept(&visitor);
				}

				FOREACH(Ptr<WfLexicalScopeName>, child, name->children.Values())
				{
					ValidateScopeName(manager, child);
				}
			}
		}
	}
}