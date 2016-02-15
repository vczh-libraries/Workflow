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

/***********************************************************************
WfObservingDependency
***********************************************************************/

			WfObservingDependency::WfObservingDependency(WfObservingDependency& dependency)
				:dependencies(dependency.dependencies)
			{
				CopyFrom(inputObserves, dependency.inputObserves);
			}

			WfObservingDependency::WfObservingDependency(DependencyGroup& _dependencies)
				:dependencies(_dependencies)
			{
			}

			WfObservingDependency::WfObservingDependency(DependencyGroup& _dependencies, ObserveList& _inputObserves)
				:dependencies(_dependencies)
			{
				CopyFrom(inputObserves, _inputObserves);
			}

			void WfObservingDependency::AddInternal(WfExpression* observe, WfExpression* dependedObserve)
			{
				auto index = dependencies.Keys().IndexOf(dependedObserve);
				if (index == -1)
				{
					dependencies.Add(dependedObserve, observe);
				}
				else if (!dependencies.GetByIndex(index).Contains(observe))
				{
					dependencies.Add(dependedObserve, observe);
				}
			}

			void WfObservingDependency::Prepare(WfExpression* observe)
			{
				AddInternal(0, observe);

				if (!outputObserves.Contains(observe))
				{
					outputObserves.Add(observe);
				}
			}

			void WfObservingDependency::Add(WfExpression* observe)
			{
				Add(observe, *this);
			}

			void WfObservingDependency::Add(WfExpression* observe, WfObservingDependency& dependency)
			{
				Prepare(observe);
				FOREACH(WfExpression*, dependedObserve, dependency.inputObserves)
				{
					AddInternal(observe, dependedObserve);
				}
			}

			void WfObservingDependency::TurnToInput()
			{
				if (outputObserves.Count() > 0)
				{
					CopyFrom(inputObserves, outputObserves);
					outputObserves.Clear();
				}
			}

			void WfObservingDependency::Cleanup()
			{
				SortedList<WfExpression*> all;
				CopyFrom(all, From(dependencies.Keys()).Distinct());

				vint count = dependencies.Keys().Count();
				for (vint i = 0; i < count; i++)
				{
					const auto& values = dependencies.GetByIndex(i);
					if (values.Contains(0) && values.Count()>1)
					{
						dependencies.Remove(dependencies.Keys()[i], 0);
					}

					FOREACH(WfExpression*, value, values)
					{
						all.Remove(value);
					}
				}

				FOREACH(WfExpression*, observe, all)
				{
					dependencies.Add(0, observe);
				}
			}

/***********************************************************************
GetObservingDependency
***********************************************************************/

			class GetObservingDependencyVisitor : public Object, public WfExpression::IVisitor
			{
			public:
				WfLexicalScopeManager*				manager;
				WfObservingDependency&				dependency;

				GetObservingDependencyVisitor(WfLexicalScopeManager* _manager, WfObservingDependency& _dependency)
					:manager(_manager)
					, dependency(_dependency)
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
				}

				void Visit(WfMemberExpression* node)override
				{
					WfObservingDependency parent(dependency);
					GetObservingDependency(manager, node->parent, parent);
					parent.TurnToInput();

					if (dependency.inputObserves.Count() == 0)
					{
						auto memberResult = manager->expressionResolvings[node];
						if (memberResult.propertyInfo)
						{
							auto td = memberResult.propertyInfo->GetOwnerTypeDescriptor();
							auto ev = memberResult.propertyInfo->GetValueChangedEvent();
							if (!ev)
							{
								ev = td->GetEventByName(memberResult.propertyInfo->GetName() + L"Changed", true);
							}
							if (ev)
							{
								dependency.Add(node, parent);
							}
						}
					}
				}

				void Visit(WfChildExpression* node)override
				{
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
					GetObservingDependency(manager, node->expandedExpression, dependency);
				}

				void Visit(WfUnaryExpression* node)override
				{
					GetObservingDependency(manager, node->operand, dependency);
				}

				void Visit(WfBinaryExpression* node)override
				{
					GetObservingDependency(manager, node->first, dependency);
					GetObservingDependency(manager, node->second, dependency);
				}

				void Visit(WfLetExpression* node)override
				{
					FOREACH(Ptr<WfLetVariable>, var, node->variables)
					{
						GetObservingDependency(manager, var->value, dependency);
					}
					GetObservingDependency(manager, node->expression, dependency);
				}

				void Visit(WfIfExpression* node)override
				{
					GetObservingDependency(manager, node->condition, dependency);
					GetObservingDependency(manager, node->trueBranch, dependency);
					GetObservingDependency(manager, node->falseBranch, dependency);
				}

				void Visit(WfRangeExpression* node)override
				{
					GetObservingDependency(manager, node->begin, dependency);
					GetObservingDependency(manager, node->end, dependency);
				}

				void Visit(WfSetTestingExpression* node)override
				{
					GetObservingDependency(manager, node->element, dependency);
					GetObservingDependency(manager, node->collection, dependency);
				}

				void Visit(WfConstructorExpression* node)override
				{
					FOREACH(Ptr<WfConstructorArgument>, argument, node->arguments)
					{
						GetObservingDependency(manager, argument->key, dependency);
						if (argument->value)
						{
							GetObservingDependency(manager, argument->value, dependency);
						}
					}
				}

				void Visit(WfInferExpression* node)override
				{
					GetObservingDependency(manager, node->expression, dependency);
				}

				void Visit(WfTypeCastingExpression* node)override
				{
					GetObservingDependency(manager, node->expression, dependency);
				}

				void Visit(WfTypeTestingExpression* node)override
				{
					GetObservingDependency(manager, node->expression, dependency);
				}

				void Visit(WfTypeOfTypeExpression* node)override
				{
				}

				void Visit(WfTypeOfExpressionExpression* node)override
				{
					GetObservingDependency(manager, node->expression, dependency);
				}

				void Visit(WfAttachEventExpression* node)override
				{
				}

				void Visit(WfDetachEventExpression* node)override
				{
				}

				void Visit(WfBindExpression* node)override
				{
				}

				void Visit(WfObserveExpression* node)override
				{
					WfObservingDependency parent(dependency);
					GetObservingDependency(manager, node->parent, parent);
					parent.TurnToInput();

					dependency.Add(node, parent);
					dependency.TurnToInput();
					GetObservingDependency(manager, node->expression, dependency);
				}

				void Visit(WfCallExpression* node)override
				{
					GetObservingDependency(manager, node->function, dependency);
					FOREACH(Ptr<WfExpression>, argument, node->arguments)
					{
						GetObservingDependency(manager, argument, dependency);
					}
				}

				void Visit(WfFunctionExpression* node)override
				{
				}

				void Visit(WfNewTypeExpression* node)override
				{
					FOREACH(Ptr<WfExpression>, argument, node->arguments)
					{
						GetObservingDependency(manager, argument, dependency);
					}
				}
			};

			void GetObservingDependency(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, WfObservingDependency& dependency)
			{
				GetObservingDependencyVisitor visitor(manager, dependency);
				expression->Accept(&visitor);
			}

/***********************************************************************
CreateBindVariableReference
***********************************************************************/

			Ptr<WfExpression> CreateBindVariableReference(const WString& name)
			{
				auto ref = MakePtr<WfReferenceExpression>();
				ref->name.value = name;

				auto zero = MakePtr<WfIntegerExpression>();
				zero->value.value = L"0";

				auto index = MakePtr<WfBinaryExpression>();
				index->op = WfBinaryOperator::Index;
				index->first = ref;
				index->second = zero;

				return index;
			}

/***********************************************************************
ExpandObserveExpression
***********************************************************************/

			class ExpandObserveExpressionVisitor : public Object, public WfExpression::IVisitor
			{
			public:
				Dictionary<WfExpression*, WString>&		cacheNames;
				Dictionary<WString, WString>&			referenceReplacement;
				Ptr<WfExpression>						result;

				ExpandObserveExpressionVisitor(Dictionary<WfExpression*, WString>& _cacheNames, collections::Dictionary<WString, WString>& _referenceReplacement)
					:cacheNames(_cacheNames)
					, referenceReplacement(_referenceReplacement)
				{
				}

				Ptr<WfExpression> Expand(Ptr<WfExpression> expr)
				{
					return ExpandObserveExpression(expr.Obj(), cacheNames, referenceReplacement);
				}

				void Visit(WfTopQualifiedExpression* node)override
				{
					auto expr = MakePtr<WfTopQualifiedExpression>();
					expr->name.value = node->name.value;
					result = expr;
				}

				void Visit(WfReferenceExpression* node)override
				{
					vint index = referenceReplacement.Keys().IndexOf(node->name.value);
					if (index == -1)
					{
						auto expr = MakePtr<WfReferenceExpression>();
						expr->name.value = node->name.value;
						result = expr;
					}
					else
					{
						result = CreateBindVariableReference(referenceReplacement.Values()[index]);
					}
				}

				void Visit(WfOrderedNameExpression* node)override
				{
					auto expr = MakePtr<WfOrderedNameExpression>();
					expr->name.value = node->name.value;
					result = expr;
				}

				void Visit(WfOrderedLambdaExpression* node)override
				{
					auto expr = MakePtr<WfOrderedLambdaExpression>();
					expr->body = CopyExpression(node->body);
					result = expr;
				}

				void Visit(WfMemberExpression* node)override
				{
					auto expr = MakePtr<WfMemberExpression>();
					expr->parent = Expand(node->parent);
					expr->name.value = node->name.value;
					result = expr;
				}

				void Visit(WfChildExpression* node)override
				{
					auto expr = MakePtr<WfChildExpression>();
					expr->parent = Expand(node->parent);
					expr->name.value = node->name.value;
					result = expr;
				}

				void Visit(WfLiteralExpression* node)override
				{
					auto expr = MakePtr<WfLiteralExpression>();
					expr->value = node->value;
					result = expr;
				}

				void Visit(WfFloatingExpression* node)override
				{
					auto expr = MakePtr<WfFloatingExpression>();
					expr->value.value = node->value.value;
					result = expr;
				}

				void Visit(WfIntegerExpression* node)override
				{
					auto expr = MakePtr<WfIntegerExpression>();
					expr->value.value = node->value.value;
					result = expr;
				}

				void Visit(WfStringExpression* node)override
				{
					auto expr = MakePtr<WfStringExpression>();
					expr->value.value = node->value.value;
					result = expr;
				}

				void Visit(WfFormatExpression* node)override
				{
					auto expr = MakePtr<WfFormatExpression>();
					expr->value.value = node->value.value;
					expr->expandedExpression = Expand(node->expandedExpression);
					result = expr;
				}

				void Visit(WfUnaryExpression* node)override
				{
					auto expr = MakePtr<WfUnaryExpression>();
					expr->op = node->op;
					expr->operand = Expand(node->operand);
					result = expr;
				}

				void Visit(WfBinaryExpression* node)override
				{
					auto expr = MakePtr<WfBinaryExpression>();
					expr->op = node->op;
					expr->first = Expand(node->first);
					expr->second = Expand(node->second);
					result = expr;
				}

				void Visit(WfLetExpression* node)override
				{
					Dictionary<WString, WString> overrided;
					auto expr = MakePtr<WfLetExpression>();

					FOREACH(Ptr<WfLetVariable>, var, node->variables)
					{
						auto key = var->name.value;
						vint index = referenceReplacement.Keys().IndexOf(key);
						if (index != -1)
						{
							auto value = referenceReplacement.Values()[index];
							referenceReplacement.Remove(key);
							overrided.Add(key, value);
						}

						auto newVar = MakePtr<WfLetVariable>();
						newVar->name.value = key;
						newVar->value = Expand(var->value);
						expr->variables.Add(newVar);
					}
					expr->expression = Expand(node->expression);

					CopyFrom(referenceReplacement, overrided, true);
					result = expr;
				}

				void Visit(WfIfExpression* node)override
				{
					auto expr = MakePtr<WfIfExpression>();
					expr->condition = Expand(node->condition);
					expr->trueBranch = Expand(node->trueBranch);
					expr->falseBranch = Expand(node->falseBranch);
					result = expr;
				}

				void Visit(WfRangeExpression* node)override
				{
					auto expr = MakePtr<WfRangeExpression>();
					expr->begin = Expand(node->begin);
					expr->end = Expand(node->end);
					expr->beginBoundary = node->beginBoundary;
					expr->endBoundary = node->endBoundary;
					result = expr;
				}

				void Visit(WfSetTestingExpression* node)override
				{
					auto expr = MakePtr<WfSetTestingExpression>();
					expr->element = Expand(node->element);
					expr->collection = Expand(node->collection);
					result = expr;
				}

				void Visit(WfConstructorExpression* node)override
				{
					auto expr = MakePtr<WfConstructorExpression>();
					FOREACH(Ptr<WfConstructorArgument>, arg, node->arguments)
					{
						auto newArg = MakePtr<WfConstructorArgument>();
						newArg->key = Expand(arg->key);
						newArg->value = Expand(arg->value);
						expr->arguments.Add(newArg);
					}
					result = expr;
				}

				void Visit(WfInferExpression* node)override
				{
					auto expr = MakePtr<WfInferExpression>();
					expr->expression = Expand(node->expression);
					expr->type = CopyType(node->type);
					result = expr;
				}

				void Visit(WfTypeCastingExpression* node)override
				{
					auto expr = MakePtr<WfTypeCastingExpression>();
					expr->expression = Expand(node->expression);
					expr->type = CopyType(node->type);
					expr->strategy = node->strategy;
					result = expr;
				}

				void Visit(WfTypeTestingExpression* node)override
				{
					auto expr = MakePtr<WfTypeTestingExpression>();
					expr->expression = Expand(node->expression);
					expr->type = CopyType(node->type);
					expr->test = node->test;
					result = expr;
				}

				void Visit(WfTypeOfTypeExpression* node)override
				{
					auto expr = MakePtr<WfTypeOfTypeExpression>();
					expr->type = CopyType(node->type);
					result = expr;
				}

				void Visit(WfTypeOfExpressionExpression* node)override
				{
					auto expr = MakePtr<WfTypeOfExpressionExpression>();
					expr->expression = Expand(node->expression);
					result = expr;
				}

				void Visit(WfAttachEventExpression* node)override
				{
					auto expr = MakePtr<WfAttachEventExpression>();
					expr->event = Expand(node->event);
					expr->function = Expand(node->function);
					result = expr;
				}

				void Visit(WfDetachEventExpression* node)override
				{
					auto expr = MakePtr<WfDetachEventExpression>();
					expr->handler = Expand(node->handler);
					result = expr;
				}

				void Visit(WfBindExpression* node)override
				{
					auto expr = MakePtr<WfBindExpression>();
					expr->expression = CopyExpression(node->expression);
					expr->expandedExpression = CopyExpression(node->expandedExpression);
					result = expr;
				}

				void Visit(WfObserveExpression* node)override
				{
					if (cacheNames.Count() == 0)
					{
						auto expr = MakePtr<WfObserveExpression>();
						expr->parent = CopyExpression(node->parent);
						expr->name.value = node->name.value;
						expr->observeType = node->observeType;
						expr->expression = node->expression;
						FOREACH(Ptr<WfExpression>, ev, node->events)
						{
							expr->events.Add(CopyExpression(ev));
						}
						result = expr;
					}
					else if (node->observeType == WfObserveType::SimpleObserve)
					{
						auto expr = MakePtr<WfMemberExpression>();
						expr->parent = Expand(node->parent);
						expr->name.value = node->expression.Cast<WfReferenceExpression>()->name.value;
						result = expr;
					}
					else
					{
						auto var = MakePtr<WfLetVariable>();
						var->name.value = node->name.value;
						var->value = Expand(node->parent);

						auto expr = MakePtr<WfLetExpression>();
						expr->variables.Add(var);
						expr->expression = Expand(node->expression);
						result = expr;
					}
				}

				void Visit(WfCallExpression* node)override
				{
					auto expr = MakePtr<WfCallExpression>();
					expr->function = Expand(node->function);
					FOREACH(Ptr<WfExpression>, arg, node->arguments)
					{
						expr->arguments.Add(Expand(arg));
					}
					result = expr;
				}

				void Visit(WfFunctionExpression* node)override
				{
					auto expr = MakePtr<WfFunctionExpression>();
					expr->function = CopyDeclaration(node->function).Cast<WfFunctionDeclaration>();
					result = expr;
				}

				void Visit(WfNewTypeExpression* node)override
				{
					auto expr = MakePtr<WfNewTypeExpression>();
					expr->type = CopyType(node->type);
					FOREACH(Ptr<WfExpression>, arg, node->arguments)
					{
						expr->arguments.Add(Expand(arg));
					}
					FOREACH(Ptr<WfDeclaration>, decl, node->declarations)
					{
						expr->declarations.Add(CopyDeclaration(decl));
					}
					result = expr;
				}
			};

			Ptr<WfExpression> ExpandObserveExpression(WfExpression* expression, collections::Dictionary<WfExpression*, WString>& cacheNames, collections::Dictionary<WString, WString>& referenceReplacement, bool useCache)
			{
				if (expression)
				{
					if (useCache)
					{
						vint index = cacheNames.Keys().IndexOf(expression);
						if (index != -1)
						{
							return CreateBindVariableReference(cacheNames.Values()[index]);
						}
					}

					ExpandObserveExpressionVisitor visitor(cacheNames, referenceReplacement);
					expression->Accept(&visitor);
					return visitor.result;
				}
				else
				{
					return 0;
				}
			}

			Ptr<WfExpression> CopyExpression(Ptr<WfExpression> expression)
			{
				if (!expression)
				{
					return nullptr;
				}

				Dictionary<WfExpression*, WString> cacheNames;
				Dictionary<WString, WString> referenceReplacement;
				return ExpandObserveExpression(expression.Obj(), cacheNames, referenceReplacement);
			}

/***********************************************************************
CopyStatement
***********************************************************************/

			class CopyStatementVisitor : public Object, public WfStatement::IVisitor
			{
			public:
				Ptr<WfStatement>						result;

				void Visit(WfBreakStatement* node)override
				{
					auto stat = MakePtr<WfBreakStatement>();
					result = stat;
				}

				void Visit(WfContinueStatement* node)override
				{
					auto stat = MakePtr<WfContinueStatement>();
					result = stat;
				}

				void Visit(WfReturnStatement* node)override
				{
					auto stat = MakePtr<WfReturnStatement>();
					stat->expression = CopyExpression(node->expression);
					result = stat;
				}

				void Visit(WfDeleteStatement* node)override
				{
					auto stat = MakePtr<WfDeleteStatement>();
					stat->expression = CopyExpression(node->expression);
					result = stat;
				}

				void Visit(WfRaiseExceptionStatement* node)override
				{
					auto stat = MakePtr<WfRaiseExceptionStatement>();
					stat->expression = CopyExpression(node->expression);
					result = stat;
				}

				void Visit(WfIfStatement* node)override
				{
					auto stat = MakePtr<WfIfStatement>();
					stat->name.value = node->name.value;
					stat->type = CopyType(node->type);
					stat->expression = CopyExpression(node->expression);
					stat->trueBranch = CopyStatement(node->trueBranch);
					stat->falseBranch = CopyStatement(node->falseBranch);
					result = stat;
				}

				void Visit(WfSwitchStatement* node)override
				{
					auto stat = MakePtr<WfSwitchStatement>();
					stat->expression = CopyExpression(node->expression);
					stat->defaultBranch = CopyStatement(node->defaultBranch);
					FOREACH(Ptr<WfSwitchCase>, switchCase, node->caseBranches)
					{
						auto newCase = MakePtr<WfSwitchCase>();
						newCase->expression = CopyExpression(switchCase->expression);
						newCase->statement = CopyStatement(switchCase->statement);
						stat->caseBranches.Add(newCase);
					}
					result = stat;
				}

				void Visit(WfWhileStatement* node)override
				{
					auto stat = MakePtr<WfWhileStatement>();
					stat->condition = CopyExpression(node->condition);
					stat->statement = CopyStatement(node->statement);
					result = stat;
				}

				void Visit(WfForEachStatement* node)override
				{
					auto stat = MakePtr<WfForEachStatement>();
					stat->name.value = node->name.value;
					stat->direction = node->direction;
					stat->collection = CopyExpression(node->collection);
					stat->statement = CopyStatement(node->statement);
					result = stat;
				}

				void Visit(WfTryStatement* node)override
				{
					auto stat = MakePtr<WfTryStatement>();
					stat->name.value = node->name.value;
					stat->protectedStatement = CopyStatement(node->protectedStatement);
					stat->catchStatement = CopyStatement(node->catchStatement);
					stat->finallyStatement = CopyStatement(node->finallyStatement);
					result = stat;
				}

				void Visit(WfBlockStatement* node)override
				{
					auto stat = MakePtr<WfBlockStatement>();
					result = stat;
				}

				void Visit(WfExpressionStatement* node)override
				{
					auto stat = MakePtr<WfExpressionStatement>();
					stat->expression = CopyExpression(node->expression);
					result = stat;
				}

				void Visit(WfVariableStatement* node)override
				{
					auto stat = MakePtr<WfVariableStatement>();
					stat->variable = CopyDeclaration(node->variable).Cast<WfVariableDeclaration>();
					result = stat;
				}

			};

			Ptr<WfStatement> CopyStatement(Ptr<WfStatement> statement)
			{
				if (!statement)
				{
					return nullptr;
				}

				CopyStatementVisitor visitor;
				statement->Accept(&visitor);
				return visitor.result;
			}

/***********************************************************************
CopyDeclaration
***********************************************************************/

			class CopyDeclarationVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				Ptr<WfDeclaration>			result;

				void Visit(WfNamespaceDeclaration* node)override
				{
					auto ns = MakePtr<WfNamespaceDeclaration>();
					FOREACH(Ptr<WfDeclaration>, decl, node->declarations)
					{
						ns->declarations.Add(CopyDeclaration(decl));
					}
					result = ns;
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					auto func = MakePtr<WfFunctionDeclaration>();
					func->name.value = node->name.value;
					func->anonymity = node->anonymity;
					func->returnType = CopyType(node->returnType);
					FOREACH(Ptr<WfFunctionArgument>, arg, node->arguments)
					{
						auto newArg = MakePtr<WfFunctionArgument>();
						newArg->type = CopyType(arg->type);
						newArg->name.value = arg->name.value;
						func->arguments.Add(newArg);
					}
					func->statement = CopyStatement(node->statement);
					result = func;
				}

				void Visit(WfVariableDeclaration* node)override
				{
					auto var = MakePtr<WfVariableDeclaration>();
					var->name.value = node->name.value;
					var->type = CopyType(node->type);
					var->expression = CopyExpression(node->expression);
					result = var;
				}

				void Visit(WfEventDeclaration* node)override
				{
					auto ev = MakePtr<WfEventDeclaration>();
					FOREACH(Ptr<WfType>, type, node->arguments)
					{
						ev->arguments.Add(CopyType(type));
					}
					result = ev;
				}

				void Visit(WfPropertyDeclaration* node)override
				{
					auto prop = MakePtr<WfPropertyDeclaration>();
					prop->type = CopyType(node->type);
					prop->getter.value = node->getter.value;
					prop->setter.value = node->setter.value;
					prop->valueChangedEvent.value = node->valueChangedEvent.value;
					result = prop;
				}

				void Visit(WfClassDeclaration* node)override
				{
					auto classDecl = MakePtr<WfClassDeclaration>();
					classDecl->kind = node->kind;
					classDecl->interfaceType = node->interfaceType;
					FOREACH(Ptr<WfType>, type, node->baseTypes)
					{
						classDecl->baseTypes.Add(CopyType(type));
					}
					FOREACH(Ptr<WfClassMember>, member, node->members)
					{
						auto classMember = MakePtr<WfClassMember>();
						classMember->kind = member->kind;
						classMember->declaration = CopyDeclaration(member->declaration);
						classDecl->members.Add(classMember);
					}
					result = classDecl;
				}
			};

			Ptr<WfDeclaration> CopyDeclaration(Ptr<WfDeclaration> declaration)
			{
				if (!declaration)
				{
					return nullptr;
				}

				CopyDeclarationVisitor visitor;
				declaration->Accept(&visitor);
				return visitor.result;
			}

/***********************************************************************
DecodeObserveExpression
***********************************************************************/

			void DecodeObserveExpression(WfLexicalScopeManager* manager, WfExpression* observe, List<IEventInfo*>& events, WfExpression*& parent)
			{
				if (auto observeExpr = dynamic_cast<WfObserveExpression*>(observe))
				{
					parent = observeExpr->parent.Obj();
					FOREACH(Ptr<WfExpression>, eventExpr, observeExpr->events)
					{
						auto result = manager->expressionResolvings[eventExpr.Obj()];
						events.Add(result.eventInfo);
					}
				}
				else if (auto memberExpr = dynamic_cast<WfMemberExpression*>(observe))
				{
					parent = memberExpr->parent.Obj();
					auto result = manager->expressionResolvings[memberExpr];
					auto td = result.propertyInfo->GetOwnerTypeDescriptor();
					auto ev = result.propertyInfo->GetValueChangedEvent();
					if (!ev)
					{
						ev = td->GetEventByName(result.propertyInfo->GetName() + L"Changed", true);
					}
					events.Add(ev);
				}
			}

/***********************************************************************
CreateBindDefaultValue
***********************************************************************/

			Ptr<WfExpression> CreateBindDefaultValue(ITypeInfo* elementType)
			{
				if (elementType->GetTypeDescriptor()->GetValueSerializer())
				{
					auto stringExpr = MakePtr<WfStringExpression>();
					stringExpr->value.value = elementType->GetTypeDescriptor()->GetValueSerializer()->GetDefaultText();

					auto castExpr = MakePtr<WfTypeCastingExpression>();
					castExpr->strategy = WfTypeCastingStrategy::Strong;
					castExpr->expression = stringExpr;
					castExpr->type = GetTypeFromTypeInfo(elementType);

					return castExpr;
				}
				else
				{
					auto nullExpr = MakePtr<WfLiteralExpression>();
					nullExpr->value = WfLiteralValue::Null;

					auto inferExpr = MakePtr<WfInferExpression>();
					inferExpr->expression = nullExpr;
					inferExpr->type = GetTypeFromTypeInfo(elementType);

					return inferExpr;
				}
			}

/***********************************************************************
CreateBindWritableVariable
***********************************************************************/

			Ptr<WfVariableStatement> CreateBindWritableVariable(const WString& name, ITypeInfo* elementType)
			{
				auto ctorArg = MakePtr<WfConstructorArgument>();
				ctorArg->key = CreateBindDefaultValue(elementType);
				
				auto ctorExpr = MakePtr<WfConstructorExpression>();
				ctorExpr->arguments.Add(ctorArg);

				auto variable = MakePtr<WfVariableDeclaration>();
				variable->name.value = name;
				variable->expression = ctorExpr;
					
				auto variableStat = MakePtr<WfVariableStatement>();
				variableStat->variable = variable;
				return variableStat;
			}

/***********************************************************************
IValueListener::GetSubscription
***********************************************************************/

			Ptr<WfFunctionDeclaration> CreateListenerGetSubscriptionFunction()
			{
				auto func = MakePtr<WfFunctionDeclaration>();
				func->name.value = L"GetSubscription";
				func->anonymity = WfFunctionAnonymity::Named;
				{
					auto typeInfo = TypeInfoRetriver<IValueSubscription*>::CreateTypeInfo();
					func->returnType = GetTypeFromTypeInfo(typeInfo.Obj());
				}

				auto block = MakePtr<WfBlockStatement>();
				func->statement = block;
				{
					auto returnStat = MakePtr<WfReturnStatement>();
					returnStat->expression =  CreateBindVariableReference(L"<subscription>");
					block->statements.Add(returnStat);
				}
				return func;
			}

/***********************************************************************
IValueListener::GetStopped
***********************************************************************/

			Ptr<WfFunctionDeclaration> CreateListenerGetStoppedFunction()
			{
				auto func = MakePtr<WfFunctionDeclaration>();
				func->name.value = L"GetStopped";
				func->anonymity = WfFunctionAnonymity::Named;
				{
					auto typeInfo = TypeInfoRetriver<bool>::CreateTypeInfo();
					func->returnType = GetTypeFromTypeInfo(typeInfo.Obj());
				}

				auto block = MakePtr<WfBlockStatement>();
				func->statement = block;
				{
					auto listenersRef = MakePtr<WfReferenceExpression>();
					listenersRef->name.value = L"<bind-listeners>";

					auto keysExpr = MakePtr<WfMemberExpression>();
					keysExpr->parent = listenersRef;
					keysExpr->name.value = L"Keys";

					auto containsExpr = MakePtr<WfMemberExpression>();
					containsExpr->parent = keysExpr;
					containsExpr->name.value = L"Contains";

					auto callExpr = MakePtr<WfCallExpression>();
					callExpr->function = containsExpr;
					callExpr->arguments.Add(CreateBindVariableReference(L"<listener>"));

					auto notExpr = MakePtr<WfUnaryExpression>();
					notExpr->op = WfUnaryOperator::Not;
					notExpr->operand = callExpr;

					auto returnStat = MakePtr<WfReturnStatement>();
					returnStat->expression = notExpr;
					block->statements.Add(returnStat);
				}
				return func;
			}

/***********************************************************************
IValueListener::StopListening
***********************************************************************/

			Ptr<WfFunctionDeclaration> CreateListenerStopListeningFunction()
			{
				auto func = MakePtr<WfFunctionDeclaration>();
				func->name.value = L"StopListening";
				func->anonymity = WfFunctionAnonymity::Named;
				{
					auto typeInfo = TypeInfoRetriver<bool>::CreateTypeInfo();
					func->returnType = GetTypeFromTypeInfo(typeInfo.Obj());
				}

				auto block = MakePtr<WfBlockStatement>();
				func->statement = block;
				{
					auto ifStat = MakePtr<WfIfStatement>();
					block->statements.Add(ifStat);
					{
						auto listenersRef = MakePtr<WfReferenceExpression>();
						listenersRef->name.value = L"<bind-listeners>";

						auto keysExpr = MakePtr<WfMemberExpression>();
						keysExpr->parent = listenersRef;
						keysExpr->name.value = L"Keys";

						auto containsExpr = MakePtr<WfMemberExpression>();
						containsExpr->parent = keysExpr;
						containsExpr->name.value = L"Contains";

						auto callExpr = MakePtr<WfCallExpression>();
						callExpr->function = containsExpr;
						callExpr->arguments.Add(CreateBindVariableReference(L"<listener>"));
						ifStat->expression = callExpr;
					}

					auto ifBlock = MakePtr<WfBlockStatement>();
					ifStat->trueBranch = ifBlock;
					{
						auto listenersRef = MakePtr<WfReferenceExpression>();
						listenersRef->name.value = L"<bind-listeners>";

						auto removeExpr = MakePtr<WfMemberExpression>();
						removeExpr->parent = listenersRef;
						removeExpr->name.value = L"Remove";

						auto callExpr = MakePtr<WfCallExpression>();
						callExpr->function = removeExpr;
						callExpr->arguments.Add(CreateBindVariableReference(L"<listener>"));

						auto stat = MakePtr<WfExpressionStatement>();
						stat->expression = callExpr;
						ifBlock->statements.Add(stat);
					}
					{
						auto literal = MakePtr<WfLiteralExpression>();
						literal->value = WfLiteralValue::True;

						auto returnStat = MakePtr<WfReturnStatement>();
						returnStat->expression = literal;
						ifBlock->statements.Add(returnStat);
					}
				}
				{
					auto literal = MakePtr<WfLiteralExpression>();
					literal->value = WfLiteralValue::False;

					auto returnStat = MakePtr<WfReturnStatement>();
					returnStat->expression = literal;
					block->statements.Add(returnStat);
				}
				return func;
			}

/***********************************************************************
IValueSubscription::Subscribe
***********************************************************************/

			Ptr<WfFunctionDeclaration> CreateBindSubscribeFunction()
			{
				auto func = MakePtr<WfFunctionDeclaration>();
				func->name.value = L"Subscribe";
				func->anonymity = WfFunctionAnonymity::Named;
				{
					auto typeInfo = TypeInfoRetriver<Ptr<IValueListener>>::CreateTypeInfo();
					func->returnType = GetTypeFromTypeInfo(typeInfo.Obj());
				}
				{
					auto typeInfo = TypeInfoRetriver<Func<void(Value)>>::CreateTypeInfo();
					auto argument = MakePtr<WfFunctionArgument>();
					argument->name.value = L"<bind-callback>";
					argument->type = GetTypeFromTypeInfo(typeInfo.Obj());
					func->arguments.Add(argument);
				}

				auto block = MakePtr<WfBlockStatement>();
				func->statement = block;
				block->statements.Add(CreateBindWritableVariable(L"<listener>", TypeInfoRetriver<IValueListener*>::CreateTypeInfo().Obj()));
				{
					auto newListener = MakePtr<WfNewTypeExpression>();
					{
						auto typeInfo = TypeInfoRetriver<Ptr<IValueListener>>::CreateTypeInfo();
						newListener->type = GetTypeFromTypeInfo(typeInfo.Obj());
					}
					newListener->declarations.Add(CreateListenerGetSubscriptionFunction());
					newListener->declarations.Add(CreateListenerGetStoppedFunction());
					newListener->declarations.Add(CreateListenerStopListeningFunction());

					auto variable = MakePtr<WfVariableDeclaration>();
					variable->name.value = L"<listener-shared>";
					variable->expression = newListener;
					
					auto variableStat = MakePtr<WfVariableStatement>();
					variableStat->variable = variable;
					block->statements.Add(variableStat);
				}
				{
					auto listenerRef = MakePtr<WfReferenceExpression>();
					listenerRef->name.value = L"<listener-shared>";

					auto castExpr = MakePtr<WfTypeCastingExpression>();
					castExpr->strategy = WfTypeCastingStrategy::Strong;
					castExpr->expression = listenerRef;
					castExpr->type = GetTypeFromTypeInfo(TypeInfoRetriver<IValueListener*>::CreateTypeInfo().Obj());

					auto assign = MakePtr<WfBinaryExpression>();
					assign->op = WfBinaryOperator::Assign;
					assign->first = CreateBindVariableReference(L"<listener>");
					assign->second = castExpr;

					auto stat = MakePtr<WfExpressionStatement>();
					stat->expression = assign;
					block->statements.Add(stat);
				}
				{
					auto callbackRef = MakePtr<WfReferenceExpression>();
					callbackRef->name.value = L"<bind-callback>";

					auto mapRef = MakePtr<WfReferenceExpression>();
					mapRef->name.value = L"<bind-listeners>";

					auto func = MakePtr<WfMemberExpression>();
					func->parent = mapRef;
					func->name.value = L"Set";

					auto call = MakePtr<WfCallExpression>();
					call->function = func;
					call->arguments.Add(CreateBindVariableReference(L"<listener>"));
					call->arguments.Add(callbackRef);

					auto stat = MakePtr<WfExpressionStatement>();
					stat->expression = call;
					block->statements.Add(stat);
				}
				{
					auto listenerRef = MakePtr<WfReferenceExpression>();
					listenerRef->name.value = L"<listener-shared>";

					auto returnStat = MakePtr<WfReturnStatement>();
					returnStat->expression = listenerRef;
					block->statements.Add(returnStat);
				}
				return func;
			}

/***********************************************************************
ExpandObserveEvent
***********************************************************************/

			Ptr<WfExpression> ExpandObserveEvent(WfLexicalScopeManager* manager, const WString& cacheName, WfExpression* observe, vint eventIndex)
			{
				if (auto observeExpr = dynamic_cast<WfObserveExpression*>(observe))
				{
					if (observeExpr->observeType == WfObserveType::SimpleObserve)
					{
						auto expr = MakePtr<WfMemberExpression>();
						expr->parent = CreateBindVariableReference(cacheName);
						expr->name.value = observeExpr->events[eventIndex].Cast<WfReferenceExpression>()->name.value;
						return expr;
					}
					else
					{
						Dictionary<WfExpression*, WString> cacheNames;
						Dictionary<WString, WString> referenceReplacement;
						referenceReplacement.Add(observeExpr->name.value, cacheName);
						return ExpandObserveExpression(observeExpr->events[eventIndex].Obj(), cacheNames, referenceReplacement);
					}
				}
				else
				{
					auto memberExpr = dynamic_cast<WfMemberExpression*>(observe);
					auto result = manager->expressionResolvings[memberExpr];
					auto td = result.propertyInfo->GetOwnerTypeDescriptor();
					auto ev = result.propertyInfo->GetValueChangedEvent();
					if (!ev)
					{
						ev = td->GetEventByName(result.propertyInfo->GetName() + L"Changed", true);
					}
					auto eventName = ev->GetName();

					auto expr = MakePtr<WfMemberExpression>();
					expr->parent = CreateBindVariableReference(cacheName);
					expr->name.value = eventName;

					return expr;
				}
			}

/***********************************************************************
CreateBindAttachStatement
***********************************************************************/

			void CreateBindAttachStatement(Ptr<WfBlockStatement> block, WfLexicalScopeManager* manager, WfExpression* observe, const WString& cachedName, Group<WfExpression*, WString>& callbackNames, Group<WfExpression*, WString>& handlerNames)
			{
				vint index = callbackNames.Keys().IndexOf(observe);
				FOREACH_INDEXER(WString, callbackName, eventIndex, callbackNames.GetByIndex(index))
				{
					auto attach = MakePtr<WfAttachEventExpression>();
					attach->event = ExpandObserveEvent(manager, cachedName, observe, eventIndex);
					attach->function = CreateBindVariableReference(callbackName);

					auto assign = MakePtr<WfBinaryExpression>();
					assign->op = WfBinaryOperator::Assign;
					assign->first = CreateBindVariableReference(handlerNames.GetByIndex(index)[eventIndex]);
					assign->second = attach;

					auto stat = MakePtr<WfExpressionStatement>();
					stat->expression = assign;
					block->statements.Add(stat);
				}
			}

/***********************************************************************
CreateBindDetachStatement
***********************************************************************/

			void CreateBindDetachStatement(Ptr<WfBlockStatement> block, WfExpression* observe, Group<WfExpression*, WString>& handlerNames)
			{
				vint index = handlerNames.Keys().IndexOf(observe);
				FOREACH(WString, name, handlerNames.GetByIndex(index))
				{
					auto detach = MakePtr<WfDetachEventExpression>();
					detach->handler = CreateBindVariableReference(name);

					auto stat = MakePtr<WfExpressionStatement>();
					stat->expression = detach;
					block->statements.Add(stat);
				}
			}

/***********************************************************************
CreateBindCacheAssignStatement
***********************************************************************/

			void CreateBindCacheAssignStatement(Ptr<WfBlockStatement> block, WfExpression* parent, Dictionary<WfExpression*, WString>& cacheNames)
			{
				auto name = cacheNames[parent];

				auto assign = MakePtr<WfBinaryExpression>();
				assign->op = WfBinaryOperator::Assign;
				assign->first = CreateBindVariableReference(name);
				Dictionary<WString, WString> referenceReplacement;
				assign->second = ExpandObserveExpression(parent, cacheNames, referenceReplacement, false);

				auto stat = MakePtr<WfExpressionStatement>();
				stat->expression = assign;
				block->statements.Add(stat);
			}

/***********************************************************************
IValueSubscription::Update
***********************************************************************/

			Ptr<WfFunctionDeclaration> CreateBindUpdateFunction(const Dictionary<WString, Ptr<ITypeInfo>>& variableTypes, Group<WfExpression*, WString>& handlerNames)
			{
				auto func = MakePtr<WfFunctionDeclaration>();
				func->name.value = L"Update";
				func->anonymity = WfFunctionAnonymity::Named;
				{
					auto typeInfo = TypeInfoRetriver<bool>::CreateTypeInfo();
					func->returnType = GetTypeFromTypeInfo(typeInfo.Obj());
				}

				auto block = MakePtr<WfBlockStatement>();
				func->statement = block;
				{
					auto ifStat = MakePtr<WfIfStatement>();
					block->statements.Add(ifStat);
					{
						auto notExpr = MakePtr<WfUnaryExpression>();
						notExpr->op = WfUnaryOperator::Not;
						notExpr->operand = CreateBindVariableReference(L"<bind-closed>");
						ifStat->expression = notExpr;
					}

					auto ifBlock = MakePtr<WfBlockStatement>();
					ifStat->trueBranch = ifBlock;
					{
						auto ref = MakePtr<WfReferenceExpression>();
						ref->name.value = L"<bind-activator>";

						auto call = MakePtr<WfCallExpression>();
						call->function = ref;

						auto stat = MakePtr<WfExpressionStatement>();
						stat->expression = call;
						ifBlock->statements.Add(stat);
					}
					{
						auto literal = MakePtr<WfLiteralExpression>();
						literal->value = WfLiteralValue::True;

						auto returnStat = MakePtr<WfReturnStatement>();
						returnStat->expression = literal;
						ifBlock->statements.Add(returnStat);
					}
				}
				{
					auto literal = MakePtr<WfLiteralExpression>();
					literal->value = WfLiteralValue::False;

					auto returnStat = MakePtr<WfReturnStatement>();
					returnStat->expression = literal;
					block->statements.Add(returnStat);
				}

				return func;
			}

/***********************************************************************
IValueSubscription::Close
***********************************************************************/

			Ptr<WfFunctionDeclaration> CreateBindCloseFunction(const Dictionary<WString, Ptr<ITypeInfo>>& variableTypes, Group<WfExpression*, WString>& handlerNames)
			{
				auto func = MakePtr<WfFunctionDeclaration>();
				func->name.value = L"Close";
				func->anonymity = WfFunctionAnonymity::Named;
				{
					auto typeInfo = TypeInfoRetriver<bool>::CreateTypeInfo();
					func->returnType = GetTypeFromTypeInfo(typeInfo.Obj());
				}

				auto block = MakePtr<WfBlockStatement>();
				func->statement = block;
				{
					auto ifStat = MakePtr<WfIfStatement>();
					block->statements.Add(ifStat);
					{
						auto notExpr = MakePtr<WfUnaryExpression>();
						notExpr->op = WfUnaryOperator::Not;
						notExpr->operand = CreateBindVariableReference(L"<bind-closed>");
						ifStat->expression = notExpr;
					}

					auto ifBlock = MakePtr<WfBlockStatement>();
					ifStat->trueBranch = ifBlock;
					{
						auto literal = MakePtr<WfLiteralExpression>();
						literal->value = WfLiteralValue::True;

						auto assign = MakePtr<WfBinaryExpression>();
						assign->op = WfBinaryOperator::Assign;
						assign->first = CreateBindVariableReference(L"<bind-closed>");
						assign->second = literal;

						auto stat = MakePtr<WfExpressionStatement>();
						stat->expression = assign;
						ifBlock->statements.Add(stat);
					}
					FOREACH_INDEXER(WfExpression*, observe, index, handlerNames.Keys())
					{
						CreateBindDetachStatement(ifBlock, observe, handlerNames);
					}
					FOREACH_INDEXER(WString, name, index, variableTypes.Keys())
					{
						auto assign = MakePtr<WfBinaryExpression>();
						assign->op = WfBinaryOperator::Assign;
						assign->first = CreateBindVariableReference(name);
						assign->second = CreateBindDefaultValue(variableTypes.Values()[index].Obj());

						auto stat = MakePtr<WfExpressionStatement>();
						stat->expression = assign;
						ifBlock->statements.Add(stat);
					}
					{
						auto ref = MakePtr<WfReferenceExpression>();
						ref->name.value = L"<bind-listeners>";

						auto func = MakePtr<WfMemberExpression>();
						func->parent = ref;
						func->name.value = L"Clear";

						auto call = MakePtr<WfCallExpression>();
						call->function = func;

						auto stat = MakePtr<WfExpressionStatement>();
						stat->expression = call;
						ifBlock->statements.Add(stat);
					}
					{
						auto literal = MakePtr<WfLiteralExpression>();
						literal->value = WfLiteralValue::True;

						auto returnStat = MakePtr<WfReturnStatement>();
						returnStat->expression = literal;
						ifBlock->statements.Add(returnStat);
					}
				}
				{
					auto literal = MakePtr<WfLiteralExpression>();
					literal->value = WfLiteralValue::False;

					auto returnStat = MakePtr<WfReturnStatement>();
					returnStat->expression = literal;
					block->statements.Add(returnStat);
				}

				return func;
			}

/***********************************************************************
ExpandBindExpression
***********************************************************************/

			void ExpandBindExpression(WfLexicalScopeManager* manager, WfBindExpression* node)
			{
				Group<WfExpression*, WfExpression*> group;
				WfObservingDependency dependency(group);
				GetObservingDependency(manager, node->expression, dependency);
				dependency.Cleanup();

				auto lambdaBlock = MakePtr<WfBlockStatement>();
				{
					auto lambda = MakePtr<WfFunctionDeclaration>();
					lambda->anonymity = WfFunctionAnonymity::Anonymous;
					auto typeInfo = TypeInfoRetriver<Ptr<IValueSubscription>>::CreateTypeInfo();
					lambda->returnType = GetTypeFromTypeInfo(typeInfo.Obj());
					lambda->statement = lambdaBlock;

					auto callLambda = MakePtr<WfCallExpression>();
					node->expandedExpression = callLambda;

					auto lambdaExpr = MakePtr<WfFunctionExpression>();
					lambdaExpr->function = lambda;
					callLambda->function = lambdaExpr;
				}

				Dictionary<WfExpression*, WfExpression*> observeParents;
				Dictionary<WfExpression*, WString> cacheNames;
				Group<WfExpression*, WString> handlerNames, callbackNames;
				Dictionary<WString, Ptr<ITypeInfo>> variableTypes;

				FOREACH_INDEXER(WfExpression*, observe, observeIndex, dependency.dependencies.Keys())
				{
					if (!observe) continue;
					List<IEventInfo*> events;
					WfExpression* parent = 0;
					DecodeObserveExpression(manager, observe, events, parent);

					WString cacheName = L"<bind-cache>" + itow(observeIndex);
					cacheNames.Add(parent, cacheName);
					observeParents.Add(observe, parent);
					{
						auto elementType = manager->expressionResolvings[parent].type;
						variableTypes.Add(cacheName, elementType);
						lambdaBlock->statements.Add(CreateBindWritableVariable(cacheName, elementType.Obj()));
					}

					FOREACH_INDEXER(IEventInfo*, ev, eventIndex, events)
					{
						WString handlerName = L"<bind-handler>" + itow(observeIndex) + L"_" + itow(eventIndex);
						{
							handlerNames.Add(observe, handlerName);
							auto elementType = TypeInfoRetriver<Ptr<IEventHandler>>::CreateTypeInfo();
							variableTypes.Add(handlerName, elementType);
							lambdaBlock->statements.Add(CreateBindWritableVariable(handlerName, elementType.Obj()));
						}
						
						WString callbackName = L"<bind-callback>" + itow(observeIndex) + L"_" + itow(eventIndex);
						{
							callbackNames.Add(observe, callbackName);
							auto elementType = CopyTypeInfo(ev->GetHandlerType());
							variableTypes.Add(callbackName, elementType);
							lambdaBlock->statements.Add(CreateBindWritableVariable(callbackName, elementType.Obj()));
						}
					}
				}

				lambdaBlock->statements.Add(CreateBindWritableVariable(L"<bind-closed>", TypeInfoRetriver<bool>::CreateTypeInfo().Obj()));
				{
					auto typeInfo = TypeInfoRetriver<Dictionary<IValueListener*, Func<void(Value)>>>::CreateTypeInfo();
					auto variable = MakePtr<WfVariableDeclaration>();
					variable->name.value = L"<bind-listeners>";
					variable->type = GetTypeFromTypeInfo(typeInfo.Obj());
					variable->expression = MakePtr<WfConstructorExpression>();

					auto variableStat = MakePtr<WfVariableStatement>();
					variableStat->variable = variable;
					lambdaBlock->statements.Add(variableStat);
				}
				lambdaBlock->statements.Add(CreateBindWritableVariable(L"<subscription>", TypeInfoRetriver<IValueSubscription*>::CreateTypeInfo().Obj()));

				{
					Group<WfExpression*, WfExpression*> dependencies;
					FOREACH_INDEXER(WfExpression*, key, index, dependency.dependencies.Keys())
					{
						FOREACH(WfExpression*, value, dependency.dependencies.GetByIndex(index))
						{
							if (value)
							{
								dependencies.Add(value, key);
							}
						}
					}
					
					List<WfExpression*> sorted, all;
					CopyFrom(all, dependency.dependencies.Keys());
					all.Remove(0);
					sorted.Add(0);
					for (vint i = 0; i < sorted.Count(); i++)
					{
						auto observe = sorted[i];
						for (vint j = dependencies.Count() - 1; j >= 0; j--)
						{
							auto key = dependencies.Keys()[j];
							if (From(dependencies.GetByIndex(j))
								.All([&all](WfExpression* value){return !all.Contains(value); })
								)
							{
								all.Remove(key);
								dependencies.Remove(key);
								sorted.Add(key);
							}
						}
					}
					sorted.Remove(0);

					FOREACH(WfExpression*, observe, sorted)
					{
						auto parent = observeParents[observe];
						CreateBindCacheAssignStatement(lambdaBlock, parent, cacheNames);
					}
				}
				{
					auto func = MakePtr<WfFunctionDeclaration>();
					func->anonymity = WfFunctionAnonymity::Anonymous;
					func->returnType = GetTypeFromTypeInfo(TypeInfoRetriver<void>::CreateTypeInfo().Obj());
					
					auto block = MakePtr<WfBlockStatement>();
					func->statement = block;
					{
						auto var = MakePtr<WfVariableDeclaration>();
						var->name.value = L"<bind-activator-result>";
						Dictionary<WString, WString> referenceReplacement;
						var->expression = ExpandObserveExpression(node->expression.Obj(), cacheNames, referenceReplacement);

						auto varStat = MakePtr<WfVariableStatement>();
						varStat->variable = var;
						block->statements.Add(varStat);
					}
					{
						auto ref = MakePtr<WfReferenceExpression>();
						ref->name.value = L"<bind-listeners>";

						auto values = MakePtr<WfMemberExpression>();
						values->parent = ref;
						values->name.value = L"Values";

						auto forStat = MakePtr<WfForEachStatement>();
						block->statements.Add(forStat);
						forStat->name.value = L"<bind-callback>";
						forStat->collection = values;
						forStat->direction = WfForEachDirection::Normal;

						auto forBlock = MakePtr<WfBlockStatement>();
						forStat->statement = forBlock;

						auto refResult = MakePtr<WfReferenceExpression>();
						refResult->name.value = L"<bind-activator-result>";

						auto refFunction = MakePtr<WfReferenceExpression>();
						refFunction->name.value = L"<bind-callback>";

						auto cast = MakePtr<WfTypeCastingExpression>();
						cast->expression = refFunction;
						cast->type = GetTypeFromTypeInfo(TypeInfoRetriver<Func<void(Value)>>::CreateTypeInfo().Obj());
						cast->strategy = WfTypeCastingStrategy::Strong;

						auto call = MakePtr<WfCallExpression>();
						call->function = cast;
						call->arguments.Add(refResult);

						auto stat = MakePtr<WfExpressionStatement>();
						stat->expression = call;
						forBlock->statements.Add(stat);
					}
					
					auto funcExpr = MakePtr<WfFunctionExpression>();
					funcExpr->function = func;

					auto var = MakePtr<WfVariableDeclaration>();
					var->name.value = L"<bind-activator>";
					var->expression = funcExpr;

					auto varStat = MakePtr<WfVariableStatement>();
					varStat->variable = var;
					lambdaBlock->statements.Add(varStat);
				}
				FOREACH_INDEXER(WfExpression*, observe, index, callbackNames.Keys())
				{
					FOREACH_INDEXER(WString, callbackName, eventIndex, callbackNames.GetByIndex(index))
					{
						auto func = MakePtr<WfFunctionDeclaration>();
						func->anonymity = WfFunctionAnonymity::Anonymous;
						{
							auto genericType = variableTypes[callbackName]->GetElementType();
							func->returnType = GetTypeFromTypeInfo(genericType->GetGenericArgument(0));
							vint count = genericType->GetGenericArgumentCount();
							for (vint i = 1; i < count; i++)
							{
								auto arg = MakePtr<WfFunctionArgument>();
								arg->name.value = L"<bind-callback-argument>" + itow(i - 1);
								arg->type = GetTypeFromTypeInfo(genericType->GetGenericArgument(i));
								func->arguments.Add(arg);
							}
						}
						auto block = MakePtr<WfBlockStatement>();
						func->statement = block;
						{
							List<WfExpression*> affected;
							affected.Add(observe);
							for (vint i = 0; i < affected.Count(); i++)
							{
								auto current = affected[i];
								vint dependencyIndex = dependency.dependencies.Keys().IndexOf(current);
								if (dependencyIndex != -1)
								{
									FOREACH(WfExpression*, affectedObserve, dependency.dependencies.GetByIndex(dependencyIndex))
									{
										if (affectedObserve && !affected.Contains(affectedObserve))
										{
											affected.Add(affectedObserve);
										}
									}
								}
							}
							affected.Remove(observe);

							FOREACH(WfExpression*, affectedObserve, From(affected).Reverse())
							{
								CreateBindDetachStatement(block, affectedObserve, handlerNames);
							}
							FOREACH(WfExpression*, affectedObserve, affected)
							{
								auto parent = observeParents[affectedObserve];
								CreateBindCacheAssignStatement(block, parent, cacheNames);
							}
							FOREACH(WfExpression*, affectedObserve, affected)
							{
								WString cachedName = cacheNames[observeParents[affectedObserve]];
								CreateBindAttachStatement(block, manager, affectedObserve, cachedName, callbackNames, handlerNames);
							}
						}
						{
							auto ref = MakePtr<WfReferenceExpression>();
							ref->name.value = L"<bind-activator>";

							auto call = MakePtr<WfCallExpression>();
							call->function = ref;

							auto stat = MakePtr<WfExpressionStatement>();
							stat->expression = call;
							block->statements.Add(stat);
						}

						auto funcExpr = MakePtr<WfFunctionExpression>();
						funcExpr->function = func;

						auto assign = MakePtr<WfBinaryExpression>();
						assign->op = WfBinaryOperator::Assign;
						assign->first = CreateBindVariableReference(callbackName);
						assign->second = funcExpr;

						auto stat = MakePtr<WfExpressionStatement>();
						stat->expression = assign;
						lambdaBlock->statements.Add(stat);
					}
				}
				FOREACH_INDEXER(WfExpression*, observe, index, callbackNames.Keys())
				{
					WString cachedName = cacheNames[observeParents[observe]];
					CreateBindAttachStatement(lambdaBlock, manager, observe, cachedName, callbackNames, handlerNames);
				}
						
				auto newSubscription = MakePtr<WfNewTypeExpression>();
				{
					auto typeInfo = TypeInfoRetriver<Ptr<IValueSubscription>>::CreateTypeInfo();
					newSubscription->type = GetTypeFromTypeInfo(typeInfo.Obj());
				}
				newSubscription->declarations.Add(CreateBindSubscribeFunction());
				newSubscription->declarations.Add(CreateBindUpdateFunction(variableTypes, handlerNames));
				newSubscription->declarations.Add(CreateBindCloseFunction(variableTypes, handlerNames));

				{
					auto variable = MakePtr<WfVariableDeclaration>();
					variable->name.value = L"<subscription-shared>";
					variable->expression = newSubscription;
					
					auto variableStat = MakePtr<WfVariableStatement>();
					variableStat->variable = variable;
					lambdaBlock->statements.Add(variableStat);
				}
				{
					auto subscriptionRef = MakePtr<WfReferenceExpression>();
					subscriptionRef->name.value = L"<subscription-shared>";

					auto castExpr = MakePtr<WfTypeCastingExpression>();
					castExpr->strategy = WfTypeCastingStrategy::Strong;
					castExpr->expression = subscriptionRef;
					castExpr->type = GetTypeFromTypeInfo(TypeInfoRetriver<IValueSubscription*>::CreateTypeInfo().Obj());

					auto assign = MakePtr<WfBinaryExpression>();
					assign->op = WfBinaryOperator::Assign;
					assign->first = CreateBindVariableReference(L"<subscription>");
					assign->second = castExpr;

					auto stat = MakePtr<WfExpressionStatement>();
					stat->expression = assign;
					lambdaBlock->statements.Add(stat);
				}
				{
					auto subscriptionRef = MakePtr<WfReferenceExpression>();
					subscriptionRef->name.value = L"<subscription-shared>";

					auto returnStat = MakePtr<WfReturnStatement>();
					returnStat->expression = subscriptionRef;
					lambdaBlock->statements.Add(returnStat);
				}
			}
		}
	}
}