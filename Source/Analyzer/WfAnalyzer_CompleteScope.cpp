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
					auto scope = manager->declarationScopes[node];
					auto info = manager->declarationMemberInfos[node].Cast<WfStaticMethod>();

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
					throw 0;
				}

				void Visit(WfEventDeclaration* node)override
				{
					throw 0;
				}

				void Visit(WfPropertyDeclaration* node)override
				{
					throw 0;
				}

				void Visit(WfClassDeclaration* node)override
				{
					CompleteScopeForDeclaration(manager, node);
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

				void Visit(WfEventDeclaration* node)override
				{
				}

				void Visit(WfPropertyDeclaration* node)override
				{
					throw 0;
				}

				void Visit(WfClassDeclaration* node)override
				{
					switch (node->kind)
					{
					case WfClassKind::Class:
						{
							auto td = manager->declarationTypes[node].Cast<WfClass>();
							FOREACH(Ptr<WfClassMember>, member, node->members)
							{
								CompleteScopeForClassMember(manager, td, member);
							}
						}
						break;
					case WfClassKind::Interface:
						{
							throw 0;
						}
						break;
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