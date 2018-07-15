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
					AddDeclFile(node);
				}

				void Traverse(WfStructDeclaration* node)override
				{
					config->structDecls.Add(surroundingClassDecl, node);
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
		}
	}
}