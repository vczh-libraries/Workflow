#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace reflection::description;

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
							break;
						}
					}
				}
			}

/***********************************************************************
BuildGlobalNameFromModules
***********************************************************************/

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

				void Visit(WfClassDeclaration* node)override
				{
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
					Namespace,
				};

				WfLexicalScopeManager*				manager;
				Ptr<WfLexicalScopeName>				name;
				Category							category;

				ValidateScopeNameDeclarationVisitor(WfLexicalScopeManager* _manager, Ptr<WfLexicalScopeName> _name)
					:manager(_manager)
					, name(_name)
					, category(_name->typeDescriptor ? Type : None)
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

				void Visit(WfClassDeclaration* node)override
				{
					throw 0;
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