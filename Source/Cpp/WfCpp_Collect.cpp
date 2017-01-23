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
				Ptr<WfClassDeclaration>		memberOfClass;

				WfCollectExpressionVisitor(WfCppConfig* _config, Ptr<WfClassDeclaration> _memberOfClass)
					:config(_config)
					, memberOfClass(_memberOfClass)
				{
				}

				void Call(Ptr<WfExpression> node)
				{
					if (node)
					{
						node->Accept(this);
					}
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
					config->classClosures.Add(memberOfClass, node);
					Call(node->body);
				}

				void Visit(WfMemberExpression* node)override
				{
					Call(node->parent);
				}

				void Visit(WfChildExpression* node)override
				{
					Call(node->parent);
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
					Call(node->expandedExpression);
				}

				void Visit(WfUnaryExpression* node)override
				{
					Call(node->operand);
				}

				void Visit(WfBinaryExpression* node)override
				{
					Call(node->first);
					Call(node->second);
				}

				void Visit(WfLetExpression* node)override
				{
					FOREACH(Ptr<WfLetVariable>, var, node->variables)
					{
						Call(var->value);
					}
					Call(node->expression);
				}

				void Visit(WfIfExpression* node)override
				{
					Call(node->condition);
					Call(node->trueBranch);
					Call(node->falseBranch);
				}

				void Visit(WfRangeExpression* node)override
				{
					Call(node->begin);
					Call(node->end);
				}

				void Visit(WfSetTestingExpression* node)override
				{
					Call(node->element);
					Call(node->collection);
				}

				void Visit(WfConstructorExpression* node)override
				{
					FOREACH(Ptr<WfConstructorArgument>, argument, node->arguments)
					{
						Call(argument->key);
						Call(argument->value);
					}
				}

				void Visit(WfInferExpression* node)override
				{
					Call(node->expression);
				}

				void Visit(WfTypeCastingExpression* node)override
				{
					Call(node->expression);
				}

				void Visit(WfTypeTestingExpression* node)override
				{
					Call(node->expression);
				}

				void Visit(WfTypeOfTypeExpression* node)override
				{
				}

				void Visit(WfTypeOfExpressionExpression* node)override
				{
					Call(node->expression);
				}

				void Visit(WfAttachEventExpression* node)override
				{
					Call(node->event);
					Call(node->function);
				}

				void Visit(WfDetachEventExpression* node)override
				{
					Call(node->event);
					Call(node->handler);
				}

				void Visit(WfBindExpression* node)override
				{
					Call(node->expandedExpression);
				}

				void Visit(WfObserveExpression* node)override
				{
					Call(node->expression);
				}

				void Visit(WfCallExpression* node)override
				{
					Call(node->function);
					FOREACH(Ptr<WfExpression>, argument, node->arguments)
					{
						Call(argument);
					}
				}

				void Visit(WfFunctionExpression* node)override
				{
					WString prefix = L"__vwsnf" + itow(config->lambdaExprs.Count() + 1) + L"_" + config->assemblyName + L"_";
					WString postfix = GetScopePostfix(config->manager->nodeScopes[node].Obj());
					WString name = prefix + postfix;

					config->lambdaExprs.Add(node, name);
					config->classClosures.Add(memberOfClass, node);
					CollectStatement(config, node->function->statement, memberOfClass);
				}

				void Visit(WfNewClassExpression* node)override
				{
					FOREACH(Ptr<WfExpression>, argument, node->arguments)
					{
						Call(argument);
					}
				}

				void Visit(WfNewInterfaceExpression* node)override
				{
					WString prefix = L"__vwsnc" + itow(config->classExprs.Count() + 1) + L"_" + config->assemblyName + L"_";
					WString postfix = GetScopePostfix(config->manager->nodeScopes[node].Obj());
					
					auto result = config->manager->expressionResolvings[node];
					auto td = result.constructorInfo->GetOwnerTypeDescriptor();
					WString name = prefix + postfix + config->ConvertType(td, L"_");

					config->classExprs.Add(node, name);
					config->classClosures.Add(memberOfClass, node);
					FOREACH(Ptr<WfClassMember>, member, node->members)
					{
						CollectClassMember(config, member, nullptr, memberOfClass);
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
				Ptr<WfClassDeclaration>		memberOfClass;

				WfCollectStatementVisitor(WfCppConfig* _config, Ptr<WfClassDeclaration> _memberOfClass)
					:config(_config)
					, memberOfClass(_memberOfClass)
				{
				}

				void Call(Ptr<WfStatement> node)
				{
					if (node)
					{
						node->Accept(this);
					}
				}

				void Call(Ptr<WfExpression> node)
				{
					CollectExpression(config, node, memberOfClass);
				}

				void Visit(WfBreakStatement* node)override
				{
				}

				void Visit(WfContinueStatement* node)override
				{
				}

				void Visit(WfReturnStatement* node)override
				{
					Call(node->expression);
				}

				void Visit(WfDeleteStatement* node)override
				{
					Call(node->expression);
				}

				void Visit(WfRaiseExceptionStatement* node)override
				{
					Call(node->expression);
				}

				void Visit(WfIfStatement* node)override
				{
					Call(node->expression);
					Call(node->trueBranch);
					Call(node->falseBranch);
				}

				void Visit(WfSwitchStatement* node)override
				{
					Call(node->expression);
					FOREACH(Ptr<WfSwitchCase>, branch, node->caseBranches)
					{
						Call(branch->expression);
						Call(branch->statement);
					}
				}

				void Visit(WfWhileStatement* node)override
				{
					Call(node->condition);
					Call(node->statement);
				}

				void Visit(WfForEachStatement* node)override
				{
					Call(node->collection);
					Call(node->statement);
				}

				void Visit(WfTryStatement* node)override
				{
					Call(node->protectedStatement);
					Call(node->catchStatement);
					Call(node->finallyStatement);
				}

				void Visit(WfBlockStatement* node)override
				{
					FOREACH(Ptr<WfStatement>, stat, node->statements)
					{
						Call(stat);
					}
				}

				void Visit(WfExpressionStatement* node)override
				{
					Call(node->expression);
				}

				void Visit(WfVariableStatement* node)override
				{
					Call(node->variable->expression);
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
				Ptr<WfClassDeclaration>		memberOfClass;

				WfCollectClassMemberVisitor(WfCppConfig* _config, Ptr<WfClassDeclaration> _classDecl, Ptr<WfClassDeclaration> _memberOfClass)
					:config(_config)
					, classDecl(_classDecl)
					, memberOfClass(_memberOfClass)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					CollectStatement(config, node->statement, memberOfClass);
				}

				void Visit(WfVariableDeclaration* node)override
				{
					CollectExpression(config, node->expression, memberOfClass);
				}

				void Visit(WfEventDeclaration* node)override
				{
				}

				void Visit(WfPropertyDeclaration* node)override
				{
				}

				void Visit(WfConstructorDeclaration* node)override
				{
					CollectStatement(config, node->statement, memberOfClass);
				}

				void Visit(WfDestructorDeclaration* node)override
				{
					CollectStatement(config, node->statement, memberOfClass);
				}

				void Visit(WfClassDeclaration* node)override
				{
					CollectDeclaration(config, node, classDecl, memberOfClass);
				}

				void Visit(WfEnumDeclaration* node)override
				{
					CollectDeclaration(config, node, classDecl, memberOfClass);
				}

				void Visit(WfStructDeclaration* node)override
				{
					CollectDeclaration(config, node, classDecl, memberOfClass);
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
				Ptr<WfClassDeclaration>		memberOfClass;

				WfCollectDeclarationVisitor(WfCppConfig* _config, Ptr<WfClassDeclaration> _classDecl, Ptr<WfClassDeclaration> _memberOfClass)
					:config(_config)
					, classDecl(_classDecl)
					, memberOfClass(_memberOfClass)
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
					CollectStatement(config, node->statement, memberOfClass);
				}

				void Visit(WfVariableDeclaration* node)override
				{
					if (!classDecl)
					{
						config->varDecls.Add(node);
					}
					CollectExpression(config, node->expression, memberOfClass);
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

				void AddDeclFile(WfDeclaration* node)
				{
					if (classDecl)
					{
						auto fileName = config->declFiles[classDecl.Obj()];
						config->declFiles.Add(node, fileName);
					}
					else
					{
						config->declFiles.Add(node, L"");
					}
				}

				void Visit(WfClassDeclaration* node)override
				{
					config->classDecls.Add(classDecl, node);

					if (classDecl)
					{
						AddDeclFile(node);
					}
					else
					{
						WString file;
						if (auto att = config->manager->GetAttribute(node->attributes, L"cpp", L"File"))
						{
							file = UnboxValue<WString>(config->manager->GetAttributeValue(att));
						}
						config->topLevelClassDeclsForFiles.Add(file, node);
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

					FOREACH(Ptr<WfClassMember>, member, node->members)
					{
						CollectClassMember(config, member, node, node);
					}
				}

				void Visit(WfEnumDeclaration* node)override
				{
					config->enumDecls.Add(classDecl, node);
					AddDeclFile(node);
				}

				void Visit(WfStructDeclaration* node)override
				{
					config->structDecls.Add(classDecl, node);
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
			};

/***********************************************************************
WfCppConfig::Collect
***********************************************************************/

			void CollectExpression(WfCppConfig* config, Ptr<WfExpression> node, Ptr<WfClassDeclaration> memberOfClass)
			{
				if (node)
				{
					WfCollectExpressionVisitor visitor(config, memberOfClass);
					node->Accept(&visitor);
				}
			}

			void CollectStatement(WfCppConfig* config, Ptr<WfStatement> node, Ptr<WfClassDeclaration> memberOfClass)
			{
				if (node)
				{
					WfCollectStatementVisitor visitor(config, memberOfClass);
					node->Accept(&visitor);
				}
			}

			void CollectClassMember(WfCppConfig* config, Ptr<WfClassMember> node, Ptr<WfClassDeclaration> classDecl, Ptr<WfClassDeclaration> memberOfClass)
			{
				if (node)
				{
					WfCollectClassMemberVisitor visitor(config, classDecl, memberOfClass);
					node->declaration->Accept(&visitor);
				}
			}

			void CollectDeclaration(WfCppConfig* config, Ptr<WfDeclaration> node, Ptr<WfClassDeclaration> classDecl, Ptr<WfClassDeclaration> memberOfClass)
			{
				if (node)
				{
					WfCollectDeclarationVisitor visitor(config, classDecl, memberOfClass);
					node->Accept(&visitor);
				}
			}
		}
	}
}