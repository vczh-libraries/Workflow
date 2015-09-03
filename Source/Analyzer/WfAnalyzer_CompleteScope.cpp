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

			class CompleteScopeForClassMemberVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				Ptr<WfClass>							td;
				Ptr<WfClassMember>						member;

				CompleteScopeForClassMemberVisitor(WfLexicalScopeManager* _manager, Ptr<WfClass> _td, Ptr<WfClassMember> _member)
					:manager(_manager)
					, td(_td)
					, member(_member)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
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

				static void Execute(WfLexicalScopeManager* manager, Ptr<WfClass> td, Ptr<WfClassMember> member)
				{
					CompleteScopeForClassMemberVisitor visitor(manager, td, member);
					member->declaration->Accept(&visitor);
				}
			};

/***********************************************************************
CompleteScopeForDeclaration
***********************************************************************/

			class CompleteScopeForDeclarationVisitor : public Object, public WfDeclaration::IVisitor
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

				static void Execute(WfLexicalScopeManager* manager, Ptr<WfDeclaration> declaration)
				{
					CompleteScopeForDeclarationVisitor visitor(manager, declaration);
					declaration->Accept(&visitor);
				}
			};

/***********************************************************************
CompleteScope
***********************************************************************/

			void CompleteScopeForClassMember(WfLexicalScopeManager* manager, Ptr<WfClass> td, Ptr<WfClassMember> member)
			{
				CompleteScopeForClassMemberVisitor::Execute(manager, td, member);
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
		}
	}
}