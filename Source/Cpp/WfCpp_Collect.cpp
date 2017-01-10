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
WfCollectExpressionVisitor
***********************************************************************/

			class WfCollectExpressionVisitor : public Object, public WfExpression::IVisitor
			{
			public:
				WfCppConfig*				config;

				WfCollectExpressionVisitor(WfCppConfig* _config)
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

				void Visit(WfThisExpression* node)override
				{
				}

				void Visit(WfTopQualifiedExpression* node)override
				{
				}

				void Visit(WfReferenceExpression* node)override
				{
				}

				void Visit(WfOrderedNameExpression* node)override
				{
				}

				void Visit(WfOrderedLambdaExpression* node)override
				{
					WString prefix = L"__vwsno" + itow(config->lambdaExprs.Count() + 1) + L"_" + config->assemblyName + L"_";
					WString postfix = GetScopePostfix(config->manager->nodeScopes[node].Obj());
					WString name = prefix + postfix;

					config->lambdaExprs.Add(node, name);
					CollectExpression(config, node->body);
				}

				void Visit(WfMemberExpression* node)override
				{
					CollectExpression(config, node->parent);
				}

				void Visit(WfChildExpression* node)override
				{
					CollectExpression(config, node->parent);
				}

				void Visit(WfLiteralExpression* node)override
				{
				}

				void Visit(WfFloatingExpression* node)override
				{
				}

				void Visit(WfIntegerExpression* node)override
				{
				}

				void Visit(WfStringExpression* node)override
				{
				}

				void Visit(WfFormatExpression* node)override
				{
					CollectExpression(config, node->expandedExpression);
				}

				void Visit(WfUnaryExpression* node)override
				{
					CollectExpression(config, node->operand);
				}

				void Visit(WfBinaryExpression* node)override
				{
					CollectExpression(config, node->first);
					CollectExpression(config, node->second);
				}

				void Visit(WfLetExpression* node)override
				{
					FOREACH(Ptr<WfLetVariable>, var, node->variables)
					{
						CollectExpression(config, var->value);
					}
					CollectExpression(config, node->expression);
				}

				void Visit(WfIfExpression* node)override
				{
					CollectExpression(config, node->condition);
					CollectExpression(config, node->trueBranch);
					CollectExpression(config, node->falseBranch);
				}

				void Visit(WfRangeExpression* node)override
				{
					CollectExpression(config, node->begin);
					CollectExpression(config, node->end);
				}

				void Visit(WfSetTestingExpression* node)override
				{
					CollectExpression(config, node->element);
					CollectExpression(config, node->collection);
				}

				void Visit(WfConstructorExpression* node)override
				{
					FOREACH(Ptr<WfConstructorArgument>, argument, node->arguments)
					{
						CollectExpression(config, argument->key);
						CollectExpression(config, argument->value);
					}
				}

				void Visit(WfInferExpression* node)override
				{
					CollectExpression(config, node->expression);
				}

				void Visit(WfTypeCastingExpression* node)override
				{
					CollectExpression(config, node->expression);
				}

				void Visit(WfTypeTestingExpression* node)override
				{
					CollectExpression(config, node->expression);
				}

				void Visit(WfTypeOfTypeExpression* node)override
				{
				}

				void Visit(WfTypeOfExpressionExpression* node)override
				{
					CollectExpression(config, node->expression);
				}

				void Visit(WfAttachEventExpression* node)override
				{
					CollectExpression(config, node->event);
					CollectExpression(config, node->function);
				}

				void Visit(WfDetachEventExpression* node)override
				{
					CollectExpression(config, node->event);
					CollectExpression(config, node->handler);
				}

				void Visit(WfBindExpression* node)override
				{
					CollectExpression(config, node->expandedExpression);
				}

				void Visit(WfObserveExpression* node)override
				{
					CollectExpression(config, node->expression);
				}

				void Visit(WfCallExpression* node)override
				{
					CollectExpression(config, node->function);
					FOREACH(Ptr<WfExpression>, argument, node->arguments)
					{
						CollectExpression(config, argument);
					}
				}

				void Visit(WfFunctionExpression* node)override
				{
					WString prefix = L"__vwsnf" + itow(config->lambdaExprs.Count() + 1) + L"_" + config->assemblyName + L"_";
					WString postfix = GetScopePostfix(config->manager->nodeScopes[node].Obj());
					WString name = prefix + postfix;

					config->lambdaExprs.Add(node, name);
					CollectStatement(config, node->function->statement);
				}

				void Visit(WfNewClassExpression* node)override
				{
					FOREACH(Ptr<WfExpression>, argument, node->arguments)
					{
						CollectExpression(config, argument);
					}
				}

				void Visit(WfNewInterfaceExpression* node)override
				{
					WString prefix = L"__vwsnc" + itow(config->classExprs.Count() + 1) + L"_" + config->assemblyName + L"_";
					WString postfix = GetScopePostfix(config->manager->nodeScopes[node].Obj());
					
					auto result = config->manager->expressionResolvings[node];
					auto td = result.constructorInfo->GetOwnerTypeDescriptor();
					WString name = prefix + postfix + config->ConvertFullName(CppGetFullName(td), L"_");

					config->classExprs.Add(node, name);
					FOREACH(Ptr<WfClassMember>, member, node->members)
					{
						CollectClassMember(config, member, nullptr);
					}
				}
			};

/***********************************************************************
WfCollectStatementVisitor
***********************************************************************/

			class WfCollectStatementVisitor : public Object, public WfStatement::IVisitor
			{
			public:
				WfCppConfig*				config;

				WfCollectStatementVisitor(WfCppConfig* _config)
					:config(_config)
				{
				}

				void Visit(WfBreakStatement* node)override
				{
				}

				void Visit(WfContinueStatement* node)override
				{
				}

				void Visit(WfReturnStatement* node)override
				{
					CollectExpression(config, node->expression);
				}

				void Visit(WfDeleteStatement* node)override
				{
					CollectExpression(config, node->expression);
				}

				void Visit(WfRaiseExceptionStatement* node)override
				{
					CollectExpression(config, node->expression);
				}

				void Visit(WfIfStatement* node)override
				{
					CollectExpression(config, node->expression);
					CollectStatement(config, node->trueBranch);
					CollectStatement(config, node->falseBranch);
				}

				void Visit(WfSwitchStatement* node)override
				{
					CollectExpression(config, node->expression);
					FOREACH(Ptr<WfSwitchCase>, branch, node->caseBranches)
					{
						CollectExpression(config, branch->expression);
						CollectStatement(config, branch->statement);
					}
				}

				void Visit(WfWhileStatement* node)override
				{
					CollectExpression(config, node->condition);
					CollectStatement(config, node->statement);
				}

				void Visit(WfForEachStatement* node)override
				{
					CollectExpression(config, node->collection);
					CollectStatement(config, node->statement);
				}

				void Visit(WfTryStatement* node)override
				{
					CollectStatement(config, node->protectedStatement);
					CollectStatement(config, node->catchStatement);
					CollectStatement(config, node->finallyStatement);
				}

				void Visit(WfBlockStatement* node)override
				{
					FOREACH(Ptr<WfStatement>, stat, node->statements)
					{
						CollectStatement(config, stat);
					}
				}

				void Visit(WfExpressionStatement* node)override
				{
					CollectExpression(config, node->expression);
				}

				void Visit(WfVariableStatement* node)override
				{
					CollectExpression(config, node->variable->expression);
				}
			};

/***********************************************************************
WfCollectClassMemberVisitor
***********************************************************************/

			class WfCollectClassMemberVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfCppConfig*				config;
				Ptr<WfClassDeclaration>		classDecl;

				WfCollectClassMemberVisitor(WfCppConfig* _config, Ptr<WfClassDeclaration> _classDecl)
					:config(_config)
					, classDecl(_classDecl)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					CollectStatement(config, node->statement);
				}

				void Visit(WfVariableDeclaration* node)override
				{
					CollectExpression(config, node->expression);
				}

				void Visit(WfEventDeclaration* node)override
				{
				}

				void Visit(WfPropertyDeclaration* node)override
				{
				}

				void Visit(WfConstructorDeclaration* node)override
				{
					CollectStatement(config, node->statement);
				}

				void Visit(WfDestructorDeclaration* node)override
				{
					CollectStatement(config, node->statement);
				}

				void Visit(WfClassDeclaration* node)override
				{
					CollectDeclaration(config, node, classDecl);
				}

				void Visit(WfEnumDeclaration* node)override
				{
					CollectDeclaration(config, node, classDecl);
				}

				void Visit(WfStructDeclaration* node)override
				{
					CollectDeclaration(config, node, classDecl);
				}
			};

/***********************************************************************
WfCollectDeclarationVisitor
***********************************************************************/

			class WfCollectDeclarationVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfCppConfig*				config;
				Ptr<WfClassDeclaration>		classDecl;

				WfCollectDeclarationVisitor(WfCppConfig* _config, Ptr<WfClassDeclaration> _classDecl)
					:config(_config)
					, classDecl(_classDecl)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					FOREACH(Ptr<WfDeclaration>, decl, node->declarations)
					{
						decl->Accept(this);
					}
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					if (!classDecl)
					{
						config->funcDecls.Add(node);
					}
					CollectStatement(config, node->statement);
				}

				void Visit(WfVariableDeclaration* node)override
				{
					if (!classDecl)
					{
						config->varDecls.Add(node);
					}
					CollectExpression(config, node->expression);
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
					config->classDecls.Add(classDecl, node);
					FOREACH(Ptr<WfClassMember>, member, node->members)
					{
						CollectClassMember(config, member, node);
					}
				}

				void Visit(WfEnumDeclaration* node)override
				{
					config->enumDecls.Add(classDecl, node);
				}

				void Visit(WfStructDeclaration* node)override
				{
					config->structDecls.Add(classDecl, node);
				}
			};

/***********************************************************************
WfCppConfig::Collect
***********************************************************************/

			void CollectExpression(WfCppConfig* config, Ptr<WfExpression> node)
			{
				if (node)
				{
					WfCollectExpressionVisitor visitor(config);
					node->Accept(&visitor);
				}
			}

			void CollectStatement(WfCppConfig* config, Ptr<WfStatement> node)
			{
				if (node)
				{
					WfCollectStatementVisitor visitor(config);
					node->Accept(&visitor);
				}
			}

			void CollectClassMember(WfCppConfig* config, Ptr<WfClassMember> node, Ptr<WfClassDeclaration> classDecl)
			{
				if (node)
				{
					WfCollectClassMemberVisitor visitor(config, classDecl);
					node->declaration->Accept(&visitor);
				}
			}

			void CollectDeclaration(WfCppConfig* config, Ptr<WfDeclaration> node, Ptr<WfClassDeclaration> classDecl)
			{
				if (node)
				{
					WfCollectDeclarationVisitor visitor(config, classDecl);
					node->Accept(&visitor);
				}
			}
		}
	}
}