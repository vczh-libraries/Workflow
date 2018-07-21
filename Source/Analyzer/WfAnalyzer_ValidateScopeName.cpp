#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{

/***********************************************************************
ValidateScopeName
***********************************************************************/

			class ValidateScopeNameDeclarationVisitor
				: public Object
				, public WfDeclaration::IVisitor
				, public WfVirtualCseDeclaration::IVisitor
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
					CHECK_FAIL(L"ValidateScopeNameDeclarationVisitor::Visit(WfEventDeclaration*)#Internal error, ValidateDeclarationStructure function should check correctly.");
				}

				void Visit(WfPropertyDeclaration* node)override
				{
					CHECK_FAIL(L"ValidateScopeNameDeclarationVisitor::Visit(WfPropertyDeclaration*)#Internal error, ValidateDeclarationStructure function should check correctly.");
				}

				void Visit(WfConstructorDeclaration* node)override
				{
					CHECK_FAIL(L"ValidateScopeNameDeclarationVisitor::Visit(WfConstructorDeclaration*)#Internal error, ValidateDeclarationStructure function should check correctly.");
				}
				
				void Visit(WfDestructorDeclaration* node)override
				{
					CHECK_FAIL(L"ValidateScopeNameDeclarationVisitor::Visit(WfDestructorDeclaration*)#Internal error, ValidateDeclarationStructure function should check correctly.");
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

				void Visit(WfEnumDeclaration* node)override
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

				void Visit(WfStructDeclaration* node)override
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

				void Visit(WfStateMachineDeclaration* node)override
				{
					CHECK_FAIL(L"ValidateScopeNameDeclarationVisitor::Visit(WfStateMachineDeclaration*)#Internal error, ValidateDeclarationStructure function should check correctly.");
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