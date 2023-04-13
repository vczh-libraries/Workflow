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

			class WfCollectModuleVisitor : public traverse_visitor::AstVisitor
			{
			public:
				WfCppConfig*							config;
				Ptr<WfClassDeclaration>					surroundingClassDecl = nullptr;
				vint									skipCounter = 0;

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

					config->lambdaExprs.Add(Ptr(node), name);
					config->classClosures.Add(surroundingClassDecl, Ptr(node));
				}

				void Traverse(WfFunctionExpression* node)override
				{
					WString prefix = L"__vwsnf" + itow(config->lambdaExprs.Count() + 1) + L"_" + config->assemblyName + L"_";
					WString postfix = GetScopePostfix(config->manager->nodeScopes[node].Obj());
					WString name = prefix + postfix;

					config->lambdaExprs.Add(Ptr(node), name);
					config->classClosures.Add(surroundingClassDecl, Ptr(node));
				}

				void Traverse(WfNewInterfaceExpression* node)override
				{
					WString prefix = L"__vwsnc" + itow(config->classExprs.Count() + 1) + L"_" + config->assemblyName + L"_";
					WString postfix = GetScopePostfix(config->manager->nodeScopes[node].Obj());

					auto result = config->manager->expressionResolvings[node];
					auto td = result.constructorInfo->GetOwnerTypeDescriptor();
					WString name = prefix + postfix + config->ConvertType(td, L"_");

					config->classExprs.Add(Ptr(node), name);
					config->classClosures.Add(surroundingClassDecl, Ptr(node));
				}

				void Traverse(WfFunctionDeclaration* node)override
				{
					if (skipCounter == 1)
					{
						config->funcDecls.Add(Ptr(node));
					}
				}

				void Traverse(WfVariableDeclaration* node)override
				{
					if (skipCounter == 1)
					{
						config->varDecls.Add(Ptr(node));
					}
				}

				void Traverse(WfStaticInitDeclaration* node)override
				{
					config->staticInitDecls.Add(Ptr(node));
				}

				void Visit(WfClassDeclaration* node)override
				{
					config->classDecls.Add(surroundingClassDecl, Ptr(node));
					config->tdDecls.Add(config->manager->declarationTypes[node].Obj(), Ptr(node));

					if (!surroundingClassDecl)
					{
						WString file;
						if (auto att = config->attributeEvaluator->GetAttribute(node->attributes, L"cpp", L"File"))
						{
							auto attValue = config->attributeEvaluator->GetAttributeValue(att);
							CHECK_ERROR(attValue.type == runtime::WfInsType::String, L"Unexpected value in attribute: @cpp.File.");
							file = attValue.stringValue;
						}
						config->customFilesClasses.Add(file, Ptr(node));
					}

					auto oldSurroundingClassDecl = surroundingClassDecl;
					surroundingClassDecl = Ptr(node);
					skipCounter++;
					traverse_visitor::AstVisitor::Visit(node);
					skipCounter--;
					surroundingClassDecl = oldSurroundingClassDecl;
				}

				void Traverse(WfEnumDeclaration* node)override
				{
					config->enumDecls.Add(surroundingClassDecl, Ptr(node));
					config->tdDecls.Add(config->manager->declarationTypes[node].Obj(), Ptr(node));
				}

				void Traverse(WfStructDeclaration* node)override
				{
					config->structDecls.Add(surroundingClassDecl, Ptr(node));
					config->tdDecls.Add(config->manager->declarationTypes[node].Obj(), Ptr(node));
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					skipCounter++;
					traverse_visitor::AstVisitor::Visit(node);
					skipCounter--;
				}

				void Visit(WfVariableDeclaration* node)override
				{
					skipCounter++;
					traverse_visitor::AstVisitor::Visit(node);
					skipCounter--;
				}
			};

			void CollectModule(WfCppConfig* config, Ptr<WfModule> module)
			{
				WfCollectModuleVisitor(config).InspectInto(module.Obj());
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
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Dispatch(WfVirtualCseDeclaration* node)override
				{
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Execute(WfNewInterfaceExpression* node)
				{
					capture = config->manager->lambdaCaptures[node];
					for (auto memberDecl : node->declarations)
					{
						memberDecl->Accept(this);
					}
				}
			};

			Ptr<WfCppConfig::ClosureInfo> WfCppConfig::CollectClosureInfo(Ptr<WfExpression> closure)
			{
				using SymbolPair = Pair<WString, Ptr<analyzer::WfLexicalSymbol>>;

				auto info = Ptr(new ClosureInfo);
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
				for (auto module : manager->GetModules())
				{
					CollectModule(this, module);
				}

				for (auto lambda : lambdaExprs.Keys())
				{
					auto closureInfo = CollectClosureInfo(lambda);
					closureInfo->lambdaClassName = lambdaExprs[lambda.Obj()];
					closureInfos.Add(lambda, closureInfo);
				}

				for (auto classExpr : classExprs.Keys())
				{
					auto closureInfo = CollectClosureInfo(classExpr);
					closureInfo->lambdaClassName = classExprs[classExpr.Obj()];
					closureInfos.Add(classExpr, closureInfo);
				}
			}
		}
	}
}