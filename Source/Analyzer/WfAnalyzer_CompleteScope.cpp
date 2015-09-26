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
				Ptr<WfCustomType>						td;
				Ptr<WfClassDeclaration>					classDecl;
				Ptr<WfClassMember>						member;

				CompleteScopeForClassMemberVisitor(WfLexicalScopeManager* _manager, Ptr<WfCustomType> _td, Ptr<WfClassDeclaration> _classDecl, Ptr<WfClassMember> _member)
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
					throw 0;
				}

				void Visit(WfEventDeclaration* node)override
				{
					auto scope = manager->declarationScopes[node];
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
					auto scope = manager->declarationScopes[node];
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

				void Visit(WfClassDeclaration* node)override
				{
					CompleteScopeForDeclaration(manager, node);
				}

				static void Execute(WfLexicalScopeManager* manager, Ptr<WfCustomType> td, Ptr<WfClassDeclaration> classDecl, Ptr<WfClassMember> member)
				{
					CompleteScopeForClassMemberVisitor visitor(manager, td, classDecl, member);
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

				void Visit(WfClassDeclaration* node)override
				{
					auto scope = manager->declarationScopes[node];
					auto td = manager->declarationTypes[node].Cast<WfCustomType>();
					FOREACH(Ptr<WfType>, baseType, node->baseTypes)
					{
						if (auto typeInfo = CreateTypeInfoFromType(scope.Obj(), baseType))
						{
							td->AddBaseType(typeInfo->GetTypeDescriptor());
						}
					}
					FOREACH(Ptr<WfClassMember>, member, node->members)
					{
						CompleteScopeForClassMember(manager, td, node, member);
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

			void CompleteScopeForClassMember(WfLexicalScopeManager* manager, Ptr<WfCustomType> td, Ptr<WfClassDeclaration> classDecl, Ptr<WfClassMember> member)
			{
				CompleteScopeForClassMemberVisitor::Execute(manager, td, classDecl, member);
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