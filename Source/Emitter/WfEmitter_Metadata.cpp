#include "WfEmitter.h"

namespace vl
{
	namespace workflow
	{
		namespace emitter
		{
			using namespace collections;
			using namespace reflection::description;
			using namespace analyzer;
			using namespace runtime;
			using namespace typeimpl;

/***********************************************************************
GenerateGlobalDeclarationMetadata
***********************************************************************/

			void GenerateFunctionDeclarationMetadata(WfCodegenContext& context, WfFunctionDeclaration* node, Ptr<WfAssemblyFunction> meta)
			{
				for (auto argument : node->arguments)
				{
					meta->argumentNames.Add(argument->name.value);
				}
				{
					vint index = context.manager->lambdaCaptures.Keys().IndexOf(node);
					if (index != -1)
					{
						auto capture = context.manager->lambdaCaptures.Values()[index];
						for (auto symbol : capture->symbols)
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

				GenerateGlobalClassMemberMetadataVisitor(WfCodegenContext& _context, const WString& _namePrefix, Ptr<WfClassDeclaration> _classDecl)
					:context(_context)
					, namePrefix(_namePrefix)
					, classDecl(_classDecl)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					if (node->functionKind == WfFunctionKind::Static)
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
					auto meta = Ptr(new WfAssemblyFunction);
					meta->name = namePrefix + L"#ctor";
					for (auto argument : node->arguments)
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
					auto meta = Ptr(new WfAssemblyFunction);
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

				void Visit(WfEnumDeclaration* node)override
				{
				}

				void Visit(WfStructDeclaration* node)override
				{
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
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
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
					for (auto decl : node->declarations)
					{
						GenerateGlobalDeclarationMetadata(context, decl, namePrefix + node->name.value + L"::");
					}
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					auto meta = Ptr(new WfAssemblyFunction);
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
					for (auto memberDecl : node->declarations)
					{
						GenerateGlobalClassMemberMetadataVisitor visitor(context, namePrefix + node->name.value + L"::", node);
						memberDecl->Accept(&visitor);
					}
				}

				void Visit(WfEnumDeclaration* node)override
				{
				}

				void Visit(WfStructDeclaration* node)override
				{
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
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
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