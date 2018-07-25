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

				void Visit(WfClassDeclaration* node)override
				{
					config->classDecls.Add(surroundingClassDecl, node);
					config->tdDecls.Add(config->manager->declarationTypes[node].Obj(), node);

					if (!surroundingClassDecl)
					{
						WString file;
						if (auto att = config->attributeEvaluator->GetAttribute(node->attributes, L"cpp", L"File"))
						{
							file = UnboxValue<WString>(config->attributeEvaluator->GetAttributeValue(att));
						}
						config->customFilesClasses.Add(file, node);
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
				}

				void Traverse(WfStructDeclaration* node)override
				{
					config->structDecls.Add(surroundingClassDecl, node);
					config->tdDecls.Add(config->manager->declarationTypes[node].Obj(), node);
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

/***********************************************************************
WfCppConfig::CollectClosureInfo
***********************************************************************/

			class WfCppCollectClassExprInfoVisitor : public empty_visitor::DeclarationVisitor
			{
			public:
				WfCppConfig*							config;
				vint									variableCount = 0;
				Ptr<analyzer::WfLexicalCapture>			capture;

				WfCppCollectClassExprInfoVisitor(WfCppConfig* _config)
					:config(_config)
				{
				}

				void Visit(WfVariableDeclaration* node)override
				{
					variableCount++;
				}

				void Dispatch(WfVirtualCfeDeclaration* node)override
				{
					FOREACH(Ptr<WfDeclaration>, decl, node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Dispatch(WfVirtualCseDeclaration* node)override
				{
					FOREACH(Ptr<WfDeclaration>, decl, node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Execute(WfNewInterfaceExpression* node)
				{
					capture = config->manager->lambdaCaptures[node];
					FOREACH(Ptr<WfDeclaration>, memberDecl, node->declarations)
					{
						memberDecl->Accept(this);
					}
				}
			};

			Ptr<WfCppConfig::ClosureInfo> WfCppConfig::CollectClosureInfo(Ptr<WfExpression> closure)
			{
				using SymbolPair = Pair<WString, Ptr<analyzer::WfLexicalSymbol>>;

				auto info = MakePtr<ClosureInfo>();
				WfLexicalScope* scope = nullptr;

				if (auto ordered = closure.Cast<WfOrderedLambdaExpression>())
				{
					// stable symbol order by sorting them by name
					CopyFrom(
						info->symbols,
						From(manager->lambdaCaptures[ordered.Obj()]->symbols)
							.Select([](Ptr<WfLexicalSymbol> symbol)
							{
								return SymbolPair(symbol->name, symbol);
							})
						);
					scope = manager->nodeScopes[ordered.Obj()].Obj();
				}
				else if (auto funcExpr = closure.Cast<WfFunctionExpression>())
				{
					// stable symbol order by sorting them by name
					CopyFrom(
						info->symbols,
						From(manager->lambdaCaptures[funcExpr->function.Obj()]->symbols)
							.Select([](Ptr<WfLexicalSymbol> symbol)
							{
								return SymbolPair(symbol->name, symbol);
							})
						);
					scope = manager->nodeScopes[funcExpr->function.Obj()].Obj();
				}
				else if (auto classExpr = closure.Cast<WfNewInterfaceExpression>())
				{
					WfCppCollectClassExprInfoVisitor visitor(this);
					visitor.Execute(classExpr.Obj());

					if (visitor.capture)
					{
						// stable symbol order by sorting them by name
						CopyFrom(
							info->symbols,
							From(visitor.capture->symbols)
								.Skip(visitor.variableCount)
								.Select([](Ptr<WfLexicalSymbol> symbol)
								{
									return SymbolPair(symbol->name, symbol);
								})
							);

						CopyFrom(
							info->ctorArgumentSymbols,
							From(visitor.capture->ctorArgumentSymbols)
								.Select([](Ptr<WfLexicalSymbol> symbol)
								{
									return SymbolPair(symbol->name, symbol);
								})
							);
					}

					scope = manager->nodeScopes[classExpr.Obj()].Obj();
				}

				Ptr<WfLexicalFunctionConfig> methodConfig;
				while (scope)
				{
					if (scope->typeOfThisExpr)
					{
						if (methodConfig)
						{
							info->thisTypes.Add(scope->typeOfThisExpr);
							if (!methodConfig->parentThisAccessable)
							{
								break;
							}
							methodConfig = nullptr;
						}
					}

					if (scope->functionConfig)
					{
						if (scope->functionConfig->thisAccessable)
						{
							methodConfig = scope->functionConfig;
						}
					}
					scope = scope->parentScope.Obj();
				}

				return info;
			}

/***********************************************************************
WfCppConfig::Collect
***********************************************************************/

			void WfCppConfig::Collect()
			{
				FOREACH(Ptr<WfModule>, module, manager->GetModules())
				{
					CollectModule(this, module);
				}

				FOREACH(Ptr<WfExpression>, lambda, lambdaExprs.Keys())
				{
					auto closureInfo = CollectClosureInfo(lambda);
					closureInfo->lambdaClassName = lambdaExprs[lambda.Obj()];
					closureInfos.Add(lambda, closureInfo);
				}

				FOREACH(Ptr<WfNewInterfaceExpression>, classExpr, classExprs.Keys())
				{
					auto closureInfo = CollectClosureInfo(classExpr);
					closureInfo->lambdaClassName = classExprs[classExpr.Obj()];
					closureInfos.Add(classExpr, closureInfo);
				}
			}
		}
	}
}