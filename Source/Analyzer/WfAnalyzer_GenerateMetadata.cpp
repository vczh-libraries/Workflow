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
					vint index = context.manager->functionLambdaCaptures.Keys().IndexOf(node);
					if (index != -1)
					{
						FOREACH(Ptr<WfLexicalSymbol>, symbol, context.manager->functionLambdaCaptures.GetByIndex(index))
						{
							meta->capturedVariableNames.Add(L"<captured>" + symbol->name);
						}
					}
				}
			}

			class GenerateGlobalClassMemberMetadataVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfCodegenContext&						context;
				WString									namePrefix;
				Ptr<WfClassMember>						member;

				GenerateGlobalClassMemberMetadataVisitor(WfCodegenContext& _context, const WString& _namePrefix, Ptr<WfClassMember> _member)
					:context(_context)
					, namePrefix(_namePrefix)
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
						auto scope = context.manager->declarationScopes[node].Obj();
						auto symbol = context.manager->GetDeclarationSymbol(scope, node);
						auto index = context.globalFunctions[symbol.Obj()];
						auto info = context.manager->declarationMemberInfos[node].Cast<WfStaticMethod>();
						info->functionIndex = index;
					}
					else
					{
						throw 0;
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

					auto scope = context.manager->declarationScopes[node].Obj();
					auto symbol = context.manager->GetDeclarationSymbol(scope, node);
					context.globalFunctions.Add(symbol.Obj(), index);
				}

				void Visit(WfVariableDeclaration* node)override
				{
					vint index = context.assembly->variableNames.Add(namePrefix + node->name.value);

					auto scope = context.manager->declarationScopes[node].Obj();
					auto symbol = scope->symbols[node->name.value][0];
					context.globalVariables.Add(symbol.Obj(), index);
				}

				void Visit(WfEventDeclaration* node)override
				{
				}

				void Visit(WfPropertyDeclaration* node)override
				{
				}

				void Visit(WfClassDeclaration* node)override
				{
					switch (node->kind)
					{
					case WfClassKind::Class:
						{
							FOREACH(Ptr<WfClassMember>, member, node->members)
							{
								GenerateGlobalClassMemberMetadataVisitor visitor(context, namePrefix + node->name.value + L"::", member);
								member->declaration->Accept(&visitor);
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
			};

			void GenerateGlobalDeclarationMetadata(WfCodegenContext& context, Ptr<WfDeclaration> declaration, const WString& namePrefix)
			{
				GenerateGlobalDeclarationMetadataVisitor visitor(context, namePrefix);
				declaration->Accept(&visitor);
			}
		}
	}
}