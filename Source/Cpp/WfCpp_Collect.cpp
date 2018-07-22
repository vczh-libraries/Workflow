#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;
			using namespace regex;
			using namespace reflection::description;
			using namespace analyzer;

/***********************************************************************
CollectModule
***********************************************************************/

			class WfCollectModuleVisitor : public traverse_visitor::ModuleVisitor
			{
			public:
				WfCppConfig*							config;
				WfClassDeclaration*						surroundingClassDecl = nullptr;
				WfFunctionDeclaration*					funcDeclToSkip = nullptr;
				WfVariableDeclaration*					varDeclToSkip = nullptr;

				WfCollectModuleVisitor(WfCppConfig* _config)
					:config(_config)
				{
				}

				WString GetScopePostfix(WfLexicalScope* scope)
				{
					WString name;
					while (scope)
					{
						if (auto decl = scope->ownerNode.Cast<WfDeclaration>())
						{
							name = config->ConvertName(decl->name.value) + L"_" + name;
						}
						scope = scope->parentScope.Obj();
					}
					return name;
				}

				void Traverse(WfOrderedLambdaExpression* node)override
				{
					WString prefix = L"__vwsno" + itow(config->lambdaExprs.Count() + 1) + L"_" + config->assemblyName + L"_";
					WString postfix = GetScopePostfix(config->manager->nodeScopes[node].Obj());
					WString name = prefix + postfix;

					config->lambdaExprs.Add(node, name);
					config->classClosures.Add(surroundingClassDecl, node);
				}

				void Traverse(WfFunctionExpression* node)override
				{
					WString prefix = L"__vwsnf" + itow(config->lambdaExprs.Count() + 1) + L"_" + config->assemblyName + L"_";
					WString postfix = GetScopePostfix(config->manager->nodeScopes[node].Obj());
					WString name = prefix + postfix;

					config->lambdaExprs.Add(node, name);
					config->classClosures.Add(surroundingClassDecl, node);
				}

				void Traverse(WfNewInterfaceExpression* node)override
				{
					WString prefix = L"__vwsnc" + itow(config->classExprs.Count() + 1) + L"_" + config->assemblyName + L"_";
					WString postfix = GetScopePostfix(config->manager->nodeScopes[node].Obj());

					auto result = config->manager->expressionResolvings[node];
					auto td = result.constructorInfo->GetOwnerTypeDescriptor();
					WString name = prefix + postfix + config->ConvertType(td, L"_");

					config->classExprs.Add(node, name);
					config->classClosures.Add(surroundingClassDecl, node);
				}

				void Traverse(WfFunctionDeclaration* node)override
				{
					if (node != funcDeclToSkip && !node->classMember)
					{
						config->funcDecls.Add(node);
					}
				}

				void Traverse(WfVariableDeclaration* node)override
				{
					if (node != varDeclToSkip && !node->classMember)
					{
						config->varDecls.Add(node);
					}
				}

				void AddDeclFile(WfDeclaration* node)
				{
					if (surroundingClassDecl)
					{
						auto fileName = config->declFiles[surroundingClassDecl];
						config->declFiles.Add(node, fileName);
					}
					else
					{
						config->declFiles.Add(node, L"");
					}
				}

				void Visit(WfClassDeclaration* node)override
				{
					config->classDecls.Add(surroundingClassDecl, node);
					config->tdDecls.Add(config->manager->declarationTypes[node].Obj(), node);

					if (surroundingClassDecl)
					{
						AddDeclFile(node);
					}
					else
					{
						WString file;
						if (auto att = config->attributeEvaluator->GetAttribute(node->attributes, L"cpp", L"File"))
						{
							file = UnboxValue<WString>(config->attributeEvaluator->GetAttributeValue(att));
						}
						config->topLevelClassDeclsForCustomFiles.Add(file, node);
						config->declFiles.Add(node, file);
					}

					auto td = config->manager->declarationTypes[node].Obj();
					vint count = td->GetBaseTypeDescriptorCount();
					for (vint i = 0; i < count; i++)
					{
						auto baseTd = td->GetBaseTypeDescriptor(i);
						auto scopeName = config->manager->typeNames[baseTd];
						if (scopeName->declarations.Count() > 0)
						{
							config->declDependencies.Add(node, scopeName->declarations[0]);
						}
					}

					auto oldSurroundingClassDecl = surroundingClassDecl;
					surroundingClassDecl = node;
					traverse_visitor::DeclarationVisitor::Visit(node);
					surroundingClassDecl = oldSurroundingClassDecl;
				}

				void Traverse(WfEnumDeclaration* node)override
				{
					config->enumDecls.Add(surroundingClassDecl, node);
					config->tdDecls.Add(config->manager->declarationTypes[node].Obj(), node);
					AddDeclFile(node);
				}

				void Traverse(WfStructDeclaration* node)override
				{
					config->structDecls.Add(surroundingClassDecl, node);
					config->tdDecls.Add(config->manager->declarationTypes[node].Obj(), node);
					AddDeclFile(node);

					auto td = config->manager->declarationTypes[node].Obj();
					vint count = td->GetPropertyCount();
					for (vint i = 0; i < count; i++)
					{
						auto propTd = td->GetProperty(i)->GetReturn()->GetTypeDescriptor();
						auto scopeName = config->manager->typeNames[propTd];
						if (scopeName->declarations.Count() > 0)
						{
							config->declDependencies.Add(node, scopeName->declarations[0]);
						}
					}
				}

				void Visit(WfFunctionExpression* node)override
				{
					auto old = funcDeclToSkip;
					funcDeclToSkip = node->function.Obj();
					traverse_visitor::ExpressionVisitor::Visit(node);
					funcDeclToSkip = old;
				}

				void Visit(WfVariableStatement* node)override
				{
					auto old = varDeclToSkip;
					varDeclToSkip = node->variable.Obj();
					traverse_visitor::StatementVisitor::Visit(node);
					varDeclToSkip = old;
				}
			};

			void CollectModule(WfCppConfig* config, Ptr<WfModule> module)
			{
				WfCollectModuleVisitor(config).VisitField(module.Obj());
			}

			void PostCollect(WfCppConfig* config)
			{
				// prepare candidates
				List<Ptr<WfClassDeclaration>> candidates;
				{
					vint index = config->classDecls.Keys().IndexOf(nullptr);
					if (index != -1)
					{
						CopyFrom(
							candidates,
							From(config->classDecls.GetByIndex(index))
								.Where([&](Ptr<WfClassDeclaration> decl)
								{
									return config->declFiles[decl.Obj()] == L"";
								})
							);
					}
				}

				// prepare dependencies
				Group<Ptr<WfClassDeclaration>, Ptr<WfClassDeclaration>> deps;
				{
					Dictionary<Ptr<WfClassDeclaration>, Ptr<WfClassDeclaration>> rootClasses;
					{
						List<Ptr<WfClassDeclaration>> unprocessed;
						{
							vint index = config->classDecls.Keys().IndexOf(nullptr);
							if (index != -1)
							{
								CopyFrom(
									unprocessed,
									From(config->classDecls.GetByIndex(index))
								);
							}
						}

						FOREACH(Ptr<WfClassDeclaration>, decl, unprocessed)
						{
							rootClasses.Add(decl, decl);
						}

						for (vint i = 0; i < unprocessed.Count(); i++)
						{
							auto decl = unprocessed[i];
							vint index = config->classDecls.Keys().IndexOf(decl.Obj());
							if (index != -1)
							{
								CopyFrom(
									unprocessed,
									From(config->classDecls.GetByIndex(index)),
									true
									);

								FOREACH(Ptr<WfClassDeclaration>, subDecl, config->classDecls.GetByIndex(index))
								{
									auto value = rootClasses[decl.Obj()];
									rootClasses.Add(subDecl, value);
								}
							}
						}
					}
					FOREACH(Ptr<WfClassDeclaration>, decl, candidates)
					{
						List<Ptr<WfClassDeclaration>> unprocessed;
						unprocessed.Add(decl);

						for (vint i = 0; i < unprocessed.Count(); i++)
						{
							auto decl = unprocessed[i];
							vint index = config->classDecls.Keys().IndexOf(decl.Obj());
							if (index != -1)
							{
								CopyFrom(
									unprocessed,
									From(config->classDecls.GetByIndex(index)),
									true
									);
							}

							index = config->declDependencies.Keys().IndexOf(decl.Obj());
							if (index != -1)
							{
								FOREACH(Ptr<WfDeclaration>, dep, config->declDependencies.GetByIndex(index))
								{
									if (auto classDecl = dep.Cast<WfClassDeclaration>())
									{
										deps.Add(decl, rootClasses[classDecl.Obj()]);
									}
								}
							}
						}
					}
				}

				auto removeDeps = [&](const auto& typesToRemove)
				{
					for (vint i = deps.Count() - 1; i >= 0; i--)
					{
						auto key = deps.Keys()[i].Obj();
						for (vint j = 0; j < typesToRemove.Count(); j++)
						{
							deps.Remove(key, typesToRemove[j].Obj());
						}
					}

					for (vint j = 0; j < typesToRemove.Count(); j++)
					{
						candidates.Remove(typesToRemove[j].Obj());
					}
				};

				// select types in default header
				while (true)
				{
					List<Ptr<WfClassDeclaration>> noDeps;
					CopyFrom(noDeps, From(candidates).Except(deps.Keys()));
					if (noDeps.Count() == 0) break;
					removeDeps(noDeps);
				}

				List<WString> customs;
				CopyFrom(customs, config->topLevelClassDeclsForCustomFiles.Keys());
				customs.Remove(L"");
				vint fileIndex = 0;

				while (true)
				{
					{
						// select custom headers without extra unprocessed dependencies
						List<WString> noDeps;
						CopyFrom(
							noDeps,
							From(customs)
								.Where([&](const WString& custom)
								{
									return From(config->topLevelClassDeclsForCustomFiles[custom])
										.All([&](Ptr<WfClassDeclaration> decl)
										{
											return !candidates.Contains(decl.Obj());
										});
								})
							);
						if (noDeps.Count() == 0) break;
						FOREACH(WString, noDep, noDeps)
						{
							customs.Remove(noDep);
							removeDeps(config->topLevelClassDeclsForCustomFiles[noDep]);
						}
					}
					{
						// put non-custom types to a new non-custom header
						List<Ptr<WfClassDeclaration>> noDeps;
						CopyFrom(noDeps, From(candidates).Except(deps.Keys()));
						if (noDeps.Count() == 0) break;
						removeDeps(noDeps);

						fileIndex++;
						FOREACH(Ptr<WfClassDeclaration>, decl, noDeps)
						{
							config->topLevelClassDeclsForHeaderFiles.Add(fileIndex, decl);
							config->topLevelClassDeslcForHeaderFilesReversed.Add(decl, fileIndex);
						}
					}
				}

				fileIndex++;
				FOREACH(Ptr<WfClassDeclaration>, decl, candidates)
				{
					config->topLevelClassDeclsForHeaderFiles.Add(fileIndex, decl);
					config->topLevelClassDeslcForHeaderFilesReversed.Add(decl, fileIndex);
				}
			}
		}
	}
}