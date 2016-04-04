#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace reflection;
			using namespace reflection::description;
			using namespace runtime;
			using namespace typeimpl;

/***********************************************************************
GenerateGlobalDeclarationMetadata
***********************************************************************/

			void GenerateFunctionDeclarationMetadata(WfCodegenContext& context, WfFunctionDeclaration* node, Ptr<WfAssemblyFunction> meta)
			{
				FOREACH(Ptr<WfFunctionArgument>, argument, node->arguments)
				{
					meta->argumentNames.Add(argument->name.value);
				}
				{
					vint index = context.manager->lambdaCaptures.Keys().IndexOf(node);
					if (index != -1)
					{
						auto capture = context.manager->lambdaCaptures.Values()[index];
						FOREACH(Ptr<WfLexicalSymbol>, symbol, capture->symbols)
						{
							meta->capturedVariableNames.Add(L"<captured>" + symbol->name);
						}
					}

					auto scope = context.manager->nodeScopes[node];
					vint count = context.GetThisStackCount(scope.Obj());
					for (vint i = 0; i < count; i++)
					{
						meta->capturedVariableNames.Add(L"<captured-this>" + itow(i));
					}
				}
			}

			class GenerateGlobalClassMemberMetadataVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfCodegenContext&						context;
				WString									namePrefix;
				Ptr<WfClassDeclaration>					classDecl;
				Ptr<WfClassMember>						member;

				GenerateGlobalClassMemberMetadataVisitor(WfCodegenContext& _context, const WString& _namePrefix, Ptr<WfClassDeclaration> _classDecl, Ptr<WfClassMember> _member)
					:context(_context)
					, namePrefix(_namePrefix)
					, classDecl(_classDecl)
					, member(_member)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					if (member->kind == WfClassMemberKind::Static)
					{
						GenerateGlobalDeclarationMetadata(context, node, namePrefix);
						auto scope = context.manager->nodeScopes[node].Obj();
						auto symbol = context.manager->GetDeclarationSymbol(scope, node);
						auto index = context.globalFunctions[symbol.Obj()];
						auto info = context.manager->declarationMemberInfos[node].Cast<WfStaticMethod>();
						info->functionIndex = index;
					}
					else if (classDecl->kind == WfClassKind::Class)
					{
						GenerateGlobalDeclarationMetadata(context, node, namePrefix);
						auto scope = context.manager->nodeScopes[node].Obj();
						auto symbol = context.manager->GetDeclarationSymbol(scope, node);
						auto index = context.globalFunctions[symbol.Obj()];
						auto info = context.manager->declarationMemberInfos[node].Cast<WfClassMethod>();
						info->functionIndex = index;
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

				void Visit(WfConstructorDeclaration* node)override
				{
					auto meta = MakePtr<WfAssemblyFunction>();
					meta->name = namePrefix + L"#ctor";
					FOREACH(Ptr<WfFunctionArgument>, argument, node->arguments)
					{
						meta->argumentNames.Add(argument->name.value);
					}
					meta->capturedVariableNames.Add(L"<captured-this>0");

					vint index = context.assembly->functions.Add(meta);
					context.assembly->functionByName.Add(meta->name, index);
					context.constructors.Add(node, index);

					auto info = context.manager->declarationMemberInfos[node].Cast<WfClassConstructor>();
					info->functionIndex = index;
				}

				void Visit(WfDestructorDeclaration* node)override
				{
					auto meta = MakePtr<WfAssemblyFunction>();
					meta->name = namePrefix + L"#dtor";
					meta->capturedVariableNames.Add(L"<captured-this>0");

					vint index = context.assembly->functions.Add(meta);
					context.assembly->functionByName.Add(meta->name, index);
					context.destructors.Add(node, index);

					auto info = context.manager->declarationTypes[classDecl.Obj()].Cast<WfClass>();
					info->destructorFunctionIndex = index;
				}

				void Visit(WfClassDeclaration* node)override
				{
					GenerateGlobalDeclarationMetadata(context, node, namePrefix);
				}
			};

			class GenerateGlobalDeclarationMetadataVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfCodegenContext&						context;
				WString									namePrefix;

				GenerateGlobalDeclarationMetadataVisitor(WfCodegenContext& _context, const WString& _namePrefix)
					:context(_context)
					, namePrefix(_namePrefix)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					FOREACH(Ptr<WfDeclaration>, decl, node->declarations)
					{
						GenerateGlobalDeclarationMetadata(context, decl, namePrefix + node->name.value + L"::");
					}
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					auto meta = MakePtr<WfAssemblyFunction>();
					meta->name = namePrefix + node->name.value;
					GenerateFunctionDeclarationMetadata(context, node, meta);

					vint index = context.assembly->functions.Add(meta);
					context.assembly->functionByName.Add(meta->name, index);

					auto scope = context.manager->nodeScopes[node].Obj();
					auto symbol = context.manager->GetDeclarationSymbol(scope, node);
					context.globalFunctions.Add(symbol.Obj(), index);
				}

				void Visit(WfVariableDeclaration* node)override
				{
					vint index = context.assembly->variableNames.Add(namePrefix + node->name.value);

					auto scope = context.manager->nodeScopes[node].Obj();
					auto symbol = scope->symbols[node->name.value][0];
					context.globalVariables.Add(symbol.Obj(), index);
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
					FOREACH(Ptr<WfClassMember>, member, node->members)
					{
						GenerateGlobalClassMemberMetadataVisitor visitor(context, namePrefix + node->name.value + L"::", node, member);
						member->declaration->Accept(&visitor);
					}
				}
			};

			void GenerateGlobalDeclarationMetadata(WfCodegenContext& context, Ptr<WfDeclaration> declaration, const WString& namePrefix)
			{
				GenerateGlobalDeclarationMetadataVisitor visitor(context, namePrefix);
				declaration->Accept(&visitor);
			}
		}
	}
}