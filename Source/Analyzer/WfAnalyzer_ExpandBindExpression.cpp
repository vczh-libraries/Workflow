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
			using namespace stream;

/***********************************************************************
SearchUntilNonVirtualStatement
***********************************************************************/

			class SearchUntilNonVirtualStatementVisitor : public empty_visitor::StatementVisitor
			{
			public:
				Ptr<WfStatement>		result;

				SearchUntilNonVirtualStatementVisitor(Ptr<WfStatement> _result)
					:result(_result)
				{
				}

				void Dispatch(WfVirtualCseStatement* node)override
				{
					result = node->expandedStatement;
					result->Accept(this);
				}

				void Dispatch(WfCoroutineStatement* node)override
				{
				}

				void Dispatch(WfStateMachineStatement* node)override
				{
				}
			};

			Ptr<WfStatement> SearchUntilNonVirtualStatement(Ptr<WfStatement> statement)
			{
				SearchUntilNonVirtualStatementVisitor visitor(statement);
				statement->Accept(&visitor);
				return visitor.result;
			}

/***********************************************************************
Copy(Type|Expression|Statement|Declaration)
***********************************************************************/

			class CopyDeclarationWithExpandVirtualVisitor : empty_visitor::DeclarationVisitor
			{
			public:
				bool						expanded = false;
				List<Ptr<WfDeclaration>>&	decls;

				CopyDeclarationWithExpandVirtualVisitor(List<Ptr<WfDeclaration>>& _decls)
					:decls(_decls)
				{
				}

				void Dispatch(WfVirtualCfeDeclaration* node)override
				{
					expanded = true;
					for (auto decl : node->expandedDeclarations)
					{
						Execute(decls, decl);
					}
				}

				void Dispatch(WfVirtualCseDeclaration* node)override
				{
					expanded = true;
					for (auto decl : node->expandedDeclarations)
					{
						Execute(decls, decl);
					}
				}

				static void Execute(List<Ptr<WfDeclaration>>& decls, Ptr<WfDeclaration> decl)
				{
					CopyDeclarationWithExpandVirtualVisitor visitor(decls);
					decl->Accept(&visitor);
					if (!visitor.expanded)
					{
						decls.Add(decl);
					}
				}
			};

			void CopyWithExpandVirtualVisitor::Expand(collections::List<Ptr<WfDeclaration>>& decls)
			{
				if (expandVirtualAst)
				{
					List<Ptr<WfDeclaration>> copied;
					CopyFrom(copied, decls);
					decls.Clear();

					for (auto decl : copied)
					{
						CopyDeclarationWithExpandVirtualVisitor::Execute(decls, decl);
					}
				}
			}

			CopyWithExpandVirtualVisitor::CopyWithExpandVirtualVisitor(bool _expandVirtualAst)
				:expandVirtualAst(_expandVirtualAst)
			{
			}

			void CopyWithExpandVirtualVisitor::Visit(WfVirtualCfeExpression* node)
			{
				if (!expandVirtualAst || !node->expandedExpression)
				{
					return copy_visitor::AstVisitor::Visit(node);
				}
				node->expandedExpression->Accept(this);
			}

			void CopyWithExpandVirtualVisitor::Visit(WfVirtualCseExpression* node)
			{
				if (!expandVirtualAst || !node->expandedExpression)
				{
					return copy_visitor::AstVisitor::Visit(node);
				}
				node->expandedExpression->Accept(this);
			}

			void CopyWithExpandVirtualVisitor::Visit(WfVirtualCseStatement* node)
			{
				if (!expandVirtualAst || !node->expandedStatement)
				{
					return copy_visitor::AstVisitor::Visit(node);
				}
				node->expandedStatement->Accept(this);
			}

			void CopyWithExpandVirtualVisitor::Visit(WfNamespaceDeclaration* node)
			{
				copy_visitor::AstVisitor::Visit(node);
				Expand(result.Cast<WfNamespaceDeclaration>()->declarations);
			}

			void CopyWithExpandVirtualVisitor::Visit(WfClassDeclaration* node)
			{
				copy_visitor::AstVisitor::Visit(node);
				Expand(result.Cast<WfClassDeclaration>()->declarations);
			}

			void CopyWithExpandVirtualVisitor::Visit(WfNewInterfaceExpression* node)
			{
				copy_visitor::AstVisitor::Visit(node);
				Expand(result.Cast<WfNewInterfaceExpression>()->declarations);
			}

			Ptr<WfType> CopyType(Ptr<WfType> type)
			{
				return CopyWithExpandVirtualVisitor(false).CopyNode(type.Obj());
			}

			Ptr<WfExpression> CopyExpression(Ptr<WfExpression> expression, bool expandVirtualExprStat)
			{
				return CopyWithExpandVirtualVisitor(expandVirtualExprStat).CopyNode(expression.Obj());
			}

			Ptr<WfStatement> CopyStatement(Ptr<WfStatement> statement, bool expandVirtualExprStat)
			{
				return CopyWithExpandVirtualVisitor(expandVirtualExprStat).CopyNode(statement.Obj());
			}

			Ptr<WfDeclaration> CopyDeclaration(Ptr<WfDeclaration> declaration, bool expandVirtualExprStat)
			{
				return CopyWithExpandVirtualVisitor(expandVirtualExprStat).CopyNode(declaration.Obj());
			}

			Ptr<WfModule> CopyModule(Ptr<WfModule> module, bool expandVirtualExprStat)
			{
				return CopyWithExpandVirtualVisitor(expandVirtualExprStat).CopyNode(module.Obj());
			}

/***********************************************************************
observing expressions:
	WfObserveExpression
	WfMemberExpression that detects the event
***********************************************************************/

			class BindContext : public Object
			{
				typedef collections::List<WfExpression*>							ExprList;
				typedef collections::Dictionary<WfExpression*, WfExpression*>		ExprMap;
				typedef collections::Group<WfExpression*, WfExpression*>			ExprGroup;
				typedef collections::Group<WfExpression*, IEventInfo*>				ExprEventGroup;
			public:
				ExprMap											observeParents;
				ExprEventGroup									observeEvents;
				ExprList										orderedObserves;
				ExprList										cachedExprs;	// expression that need to cache its value
				ExprMap											renames;		// expression -> the expression being renamed

				ExprGroup										exprAffects;	// observe expression -> all expressions that it can cause to change
				ExprGroup										exprCauses;		// expression -> observe expressions that cause it to change
				ExprGroup										observeAffects;	// observe expression -> all observe expressions that it can cause to change
				ExprGroup										observeCauses;	// observe expression -> observe expressions that cause it to change

				WString GetCacheVariableName(vint index)
				{
					return L"<bind-cache>" + itow(index);
				}

				vint GetCachedExpressionIndex(WfExpression* expression, bool ensureExists)
				{
					vint index = cachedExprs.IndexOf(expression);
					if (ensureExists)
					{
						CHECK_ERROR(index != -1, L"BindContext::GetCachedExpressionIndex(WfExpression*, bool)#Cached expression not exists.");
					}
					return index;
				}

				vint GetCachedExpressionIndexRecursively(WfExpression* expression, bool ensureExists)
				{
					WfExpression* cache = expression;
					while (true)
					{
						vint index = renames.Keys().IndexOf(cache);
						if (index == -1)
						{
							return GetCachedExpressionIndex(cache, ensureExists);
						}
						else
						{
							cache = renames.Values()[index];
						}
					}
				}
			};

			struct CallbackInfo
			{
				WfExpression*									observe;
				IEventInfo*										eventInfo;
				vint											eventIndex;
				WString											handlerName;
				WString											callbackName;
			};

			struct BindCallbackInfo
			{
				Dictionary<WString, Ptr<ITypeInfo>>				handlerVariables;
				Dictionary<WString, Ptr<ITypeInfo>>				callbackFunctions;
				Group<WfExpression*, CallbackInfo>				observeCallbackInfos;
			};

/***********************************************************************
CreateBindContext
***********************************************************************/

			class CreateBindContextVisitor : public Object, public WfExpression::IVisitor
			{
			public:
				WfLexicalScopeManager*				manager;
				BindContext&						context;

				CreateBindContextVisitor(WfLexicalScopeManager* _manager, BindContext& _context)
					:manager(_manager)
					, context(_context)
				{
				}

				void Call(WfExpression* node)
				{
					node->Accept(this);
				}

				void ObservableDepend(WfExpression* expr, WfExpression* parent)
				{
					if (context.observeParents.Keys().Contains(expr))
					{
						CHECK_ERROR(context.observeParents[expr] == parent, L"CreateBindContextVisitor::ObservableDepend assigned different parents to the same expression.");
						return;
					}
					context.orderedObserves.Add(expr);
					context.observeParents.Add(expr, parent);
					DirectDepend(expr, parent, false);
					{
						auto cache = parent;
						while (true)
						{
							vint index = context.renames.Keys().IndexOf(cache);
							if (index == -1)
							{
								index = context.cachedExprs.IndexOf(cache);
								if (index == -1)
								{
									context.cachedExprs.Add(cache);
								}
								break;
							}
							else
							{
								cache = context.renames.Values()[index];
							}
						}
					}

					vint index = context.exprCauses.Keys().IndexOf(parent);
					if (index != -1)
					{
						for (auto observe : context.exprCauses.GetByIndex(index))
						{
							context.observeAffects.Add(observe, expr);
							context.observeCauses.Add(expr, observe);
						}
					}

					context.exprAffects.Add(expr, expr);
					context.exprCauses.Add(expr, expr);
				}

				void DirectDepend(WfExpression* expr, WfExpression* depended, bool processDepended = true)
				{
					if (processDepended)
					{
						Call(depended);
					}

					vint index = context.exprCauses.Keys().IndexOf(depended);
					if (index != -1)
					{
						for (auto observe : context.exprCauses.GetByIndex(index))
						{
							context.exprCauses.Add(expr, observe);
							context.exprAffects.Add(observe, expr);
						}
					}
				}

				void Visit(WfThisExpression* node)override
				{
					// root expression, nothing to do
				}

				void Visit(WfTopQualifiedExpression* node)override
				{
					// root expression, nothing to do
				}

				void Visit(WfReferenceExpression* node)override
				{
					auto result = manager->expressionResolvings[node];
					if (result.symbol)
					{
						auto scope = result.symbol->ownerScope;
						if (auto letExpr = dynamic_cast<WfLetExpression*>(scope->ownerNode.Obj()))
						{
							auto letVar = From(letExpr->variables)
								.Where([=](const Ptr<WfLetVariable>& letVar)
								{
									return letVar->name.value == node->name.value;
								})
								.First();
							context.renames.Add(node, letVar->value.Obj());
							DirectDepend(node, letVar->value.Obj(), false);
						}
						else if (auto observeExpr = dynamic_cast<WfObserveExpression*>(scope->ownerNode.Obj()))
						{
							context.renames.Add(node, observeExpr->parent.Obj());
							DirectDepend(node, observeExpr->parent.Obj());
						}
					}
				}

				void Visit(WfOrderedNameExpression* node)override
				{
					// root expression, nothing to do
				}

				void Visit(WfOrderedLambdaExpression* node)override
				{
					// root expression, nothing to do
				}

				void Visit(WfMemberExpression* node)override
				{
					Call(node->parent.Obj());

					auto scope = manager->nodeScopes[node].Obj();
					while (scope)
					{
						if (scope->ownerNode.Cast<WfObserveExpression>())
						{
							break;
						}
						scope = scope->parentScope.Obj();
					}

					if (!scope)
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
								ObservableDepend(node, node->parent.Obj());
								context.observeEvents.Add(node, ev);
								return;
							}
						}
					}

					DirectDepend(node, node->parent.Obj(), false);
				}

				void Visit(WfChildExpression* node)override
				{
					DirectDepend(node, node->parent.Obj());
				}

				void Visit(WfLiteralExpression* node)override
				{
					// root expression, nothing to do
				}

				void Visit(WfFloatingExpression* node)override
				{
					// root expression, nothing to do
				}

				void Visit(WfIntegerExpression* node)override
				{
					// root expression, nothing to do
				}

				void Visit(WfStringExpression* node)override
				{
					// root expression, nothing to do
				}

				void Visit(WfUnaryExpression* node)override
				{
					DirectDepend(node, node->operand.Obj());
				}

				void Visit(WfBinaryExpression* node)override
				{
					DirectDepend(node, node->first.Obj());
					DirectDepend(node, node->second.Obj());
				}

				void Visit(WfLetExpression* node)override
				{
					for (auto var : node->variables)
					{
						DirectDepend(node, var->value.Obj());
					}
					DirectDepend(node, node->expression.Obj());
				}

				void Visit(WfIfExpression* node)override
				{
					DirectDepend(node, node->condition.Obj());
					DirectDepend(node, node->trueBranch.Obj());
					DirectDepend(node, node->falseBranch.Obj());
				}

				void Visit(WfRangeExpression* node)override
				{
					DirectDepend(node, node->begin.Obj());
					DirectDepend(node, node->end.Obj());
				}

				void Visit(WfSetTestingExpression* node)override
				{
					DirectDepend(node, node->collection.Obj());
					DirectDepend(node, node->element.Obj());
				}

				void Visit(WfConstructorExpression* node)override
				{
					auto result = manager->expressionResolvings[node];
					bool isStruct = (result.type->GetTypeDescriptor()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Struct);

					for (auto argument : node->arguments)
					{
						if (!isStruct)
						{
							DirectDepend(node, argument->key.Obj());
						}
						DirectDepend(node, argument->value.Obj());
					}
				}

				void Visit(WfInferExpression* node)override
				{
					DirectDepend(node, node->expression.Obj());
				}

				void Visit(WfTypeCastingExpression* node)override
				{
					DirectDepend(node, node->expression.Obj());
				}

				void Visit(WfTypeTestingExpression* node)override
				{
					DirectDepend(node, node->expression.Obj());
				}

				void Visit(WfTypeOfTypeExpression* node)override
				{
					// root expression, nothing to do
				}

				void Visit(WfTypeOfExpressionExpression* node)override
				{
					// root expression, nothing to do
				}

				void Visit(WfAttachEventExpression* node)override
				{
					DirectDepend(node, node->event.Obj());
					DirectDepend(node, node->function.Obj());
				}

				void Visit(WfDetachEventExpression* node)override
				{
					DirectDepend(node, node->event.Obj());
					DirectDepend(node, node->handler.Obj());
				}

				void Visit(WfObserveExpression* node)override
				{
					Call(node->parent.Obj());
					ObservableDepend(node, node->parent.Obj());
					Call(node->expression.Obj());
					for (auto eventExpr : node->events)
					{
						auto result = manager->expressionResolvings[eventExpr.Obj()];
						context.observeEvents.Add(node, result.eventInfo);
						Call(eventExpr.Obj());
					}
				}

				void Visit(WfCallExpression* node)override
				{
					DirectDepend(node, node->function.Obj());
					for (auto argument : node->arguments)
					{
						DirectDepend(node, argument.Obj());
					}
				}

				void Visit(WfFunctionExpression* node)override
				{
					// root expression, nothing to do
				}

				void Visit(WfNewClassExpression* node)override
				{
					for (auto argument : node->arguments)
					{
						DirectDepend(node, argument.Obj());
					}
				}

				void Visit(WfNewInterfaceExpression* node)override
				{
					// root expression, nothing to do
				}

				void Visit(WfVirtualCfeExpression* node)override
				{
					DirectDepend(node, node->expandedExpression.Obj());
				}

				void Visit(WfVirtualCseExpression* node)override
				{
					DirectDepend(node, node->expandedExpression.Obj());
				}
			};

/***********************************************************************
ExpandObserveExpression
***********************************************************************/

			Ptr<WfExpression> CreateReference(const WString& name)
			{
				auto ref = MakePtr<WfReferenceExpression>();
				ref->name.value = name;
				return ref;
			}

			class ExpandObserveExpressionVisitor : public CopyWithExpandVirtualVisitor
			{
			public:
				BindContext&									context;

				ExpandObserveExpressionVisitor(BindContext& _context)
					:CopyWithExpandVirtualVisitor(true)
					, context(_context)
				{
				}

				vl::Ptr<WfExpression> CopyNode(WfExpression* node)override
				{
					return Execute(node, context);
				}

				static Ptr<WfExpression> Execute(WfExpression* expression, BindContext& context, bool expandImmediately = true)
				{
					if (!expression)
					{
						return nullptr;
					}

					{
						vint index = context.GetCachedExpressionIndexRecursively(expression, false);
						if (index != -1)
						{
							if (expandImmediately)
							{
								return CreateReference(context.GetCacheVariableName(index));
							}
							else
							{
								expression = context.cachedExprs[index];
							}
						}
					}

					ExpandObserveExpressionVisitor visitor(context);
					expression->Accept(&visitor);
					return visitor.result.Cast<WfExpression>();
				}

				void Visit(WfLetExpression* node)override
				{
					auto letExpr = MakePtr<WfLetExpression>();
					for (auto var : node->variables)
					{
						if (context.GetCachedExpressionIndexRecursively(var->value.Obj(), false) == -1)
						{
							auto letVar = MakePtr<WfLetVariable>();
							letVar->name.value = var->name.value;
							letVar->value = Execute(var->value.Obj(), context);
							letExpr->variables.Add(letVar);
						}
					}

					if (letExpr->variables.Count() == 0)
					{
						result = Execute(node->expression.Obj(), context);
					}
					else
					{
						letExpr->expression = Execute(node->expression.Obj(), context);
						result = letExpr;
					}
				}

				void Visit(WfObserveExpression* node)override
				{
					if (node->observeType == WfObserveType::SimpleObserve)
					{
						auto expr = MakePtr<WfMemberExpression>();
						expr->parent = CopyNode(node->parent.Obj());
						expr->name.value = node->expression.Cast<WfReferenceExpression>()->name.value;
						result = expr;
					}
					else
					{
						result = CopyNode(node->expression.Obj());
					}
				}
			};

/***********************************************************************
CreateDefaultValue
***********************************************************************/

			Ptr<WfExpression> CreateDefaultValue(ITypeInfo* elementType)
			{
				auto valueType = elementType->GetTypeDescriptor()->GetValueType();
				if (elementType->GetDecorator()==ITypeInfo::TypeDescriptor && valueType != nullptr)
				{
					switch (GetTypeFlag(elementType))
					{
					case TypeFlag::Enum:
						{
							auto intExpr = MakePtr<WfIntegerExpression>();
							intExpr->value.value = L"0";

							auto inferExpr = MakePtr<WfTypeCastingExpression>();
							inferExpr->strategy = WfTypeCastingStrategy::Strong;
							inferExpr->expression = intExpr;
							inferExpr->type = GetTypeFromTypeInfo(CreateTypeInfoFromTypeFlag(TypeFlag::U8).Obj());

							auto castExpr = MakePtr<WfTypeCastingExpression>();
							castExpr->strategy = WfTypeCastingStrategy::Strong;
							castExpr->expression = inferExpr;
							castExpr->type = GetTypeFromTypeInfo(elementType);

							return castExpr;
						}
					case TypeFlag::String:
						{
							auto stringExpr = MakePtr<WfStringExpression>();
							return stringExpr;
						}
						break;
					case TypeFlag::Struct:
						if (elementType->GetTypeDescriptor()->GetSerializableType() == nullptr)
						{
							auto ctorExpr = MakePtr<WfConstructorExpression>();

							auto castExpr = MakePtr<WfTypeCastingExpression>();
							castExpr->strategy = WfTypeCastingStrategy::Strong;
							castExpr->expression = ctorExpr;
							castExpr->type = GetTypeFromTypeInfo(elementType);

							return castExpr;
						}
					default:
						{
							auto td = elementType->GetTypeDescriptor();
							if (td == description::GetTypeDescriptor<bool>())
							{
								auto expr = MakePtr<WfLiteralExpression>();
								expr->value = WfLiteralValue::False;
								return expr;
							}
							if (td == description::GetTypeDescriptor<WString>())
							{
								auto expr = MakePtr<WfStringExpression>();
								return expr;
							}
							else if (td == description::GetTypeDescriptor<float>()
								|| td == description::GetTypeDescriptor<double>())
							{
								auto valueExpr = MakePtr<WfFloatingExpression>();
								valueExpr->value.value = L"0";

								auto inferExpr = MakePtr<WfInferExpression>();
								inferExpr->expression = valueExpr;
								inferExpr->type = GetTypeFromTypeInfo(elementType);

								return inferExpr;
							}
							else if (td == description::GetTypeDescriptor<vint8_t>()
								|| td == description::GetTypeDescriptor<vint16_t>()
								|| td == description::GetTypeDescriptor<vint32_t>()
								|| td == description::GetTypeDescriptor<vint64_t>()
								|| td == description::GetTypeDescriptor<vuint8_t>()
								|| td == description::GetTypeDescriptor<vuint16_t>()
								|| td == description::GetTypeDescriptor<vuint32_t>()
								|| td == description::GetTypeDescriptor<vuint64_t>())
							{
								auto valueExpr = MakePtr<WfIntegerExpression>();
								valueExpr->value.value = L"0";

								auto inferExpr = MakePtr<WfInferExpression>();
								inferExpr->expression = valueExpr;
								inferExpr->type = GetTypeFromTypeInfo(elementType);

								return inferExpr;
							}
							else
							{
								// Consider adding a \"default (type)\" expression if CreateDefault() cannot be liminated from the compiler.
								CHECK_FAIL(L"All serializable types should have been handled!");
								/*
								auto stringExpr = MakePtr<WfStringExpression>();
								elementType->GetTypeDescriptor()->GetSerializableType()->Serialize(valueType->CreateDefault(), stringExpr->value.value);

								auto castExpr = MakePtr<WfTypeCastingExpression>();
								castExpr->strategy = WfTypeCastingStrategy::Strong;
								castExpr->expression = stringExpr;
								castExpr->type = GetTypeFromTypeInfo(elementType);

								return castExpr;
								*/
							}
						}
					}
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

			Ptr<WfVariableDeclaration> CreateWritableVariable(const WString& name, ITypeInfo* type, Ptr<WfExpression> value = nullptr)
			{
				auto decl = MakePtr<WfVariableDeclaration>();
				decl->name.value = name;
				decl->type = GetTypeFromTypeInfo(type);
				decl->expression = value ? value : CreateDefaultValue(type);
				return decl;
			}

			Ptr<WfVariableStatement> CreateWritableVariableStatement(const WString& name, ITypeInfo* type, Ptr<WfExpression> value = nullptr)
			{
				auto stat = MakePtr<WfVariableStatement>();
				stat->variable = CreateWritableVariable(name, type, value);
				return stat;
			}

/***********************************************************************
IValueSubscription::Subscribe
***********************************************************************/

			Ptr<WfFunctionDeclaration> AssignNormalMember(Ptr<WfFunctionDeclaration> decl)
			{
				decl->functionKind = WfFunctionKind::Normal;
				return decl;
			}

			Ptr<WfFunctionDeclaration> AssignOverrideMember(Ptr<WfFunctionDeclaration> decl)
			{
				decl->functionKind = WfFunctionKind::Override;
				return decl;
			}

/***********************************************************************
ExpandObserveEvent
***********************************************************************/

			Ptr<WfExpression> ExpandObserveEvent(WfLexicalScopeManager* manager, WfExpression* observe, vint eventIndex, BindContext& context)
			{
				auto cacheName = context.GetCacheVariableName(context.GetCachedExpressionIndexRecursively(context.observeParents[observe], true));
				if (auto observeExpr = dynamic_cast<WfObserveExpression*>(observe))
				{
					if (observeExpr->observeType == WfObserveType::SimpleObserve)
					{
						auto expr = MakePtr<WfMemberExpression>();
						expr->parent = CreateReference(cacheName);
						expr->name.value = observeExpr->events[eventIndex].Cast<WfReferenceExpression>()->name.value;
						return expr;
					}
					else
					{
						return ExpandObserveExpressionVisitor::Execute(observeExpr->events[eventIndex].Obj(), context);
					}
				}
				else
				{
					auto eventName = context.observeEvents[observe][0]->GetName();

					auto expr = MakePtr<WfMemberExpression>();
					expr->parent = CreateReference(cacheName);
					expr->name.value = eventName;

					return expr;
				}
			}

/***********************************************************************
CreateBindAttachStatement
***********************************************************************/

			void CreateBindAttachStatement(Ptr<WfBlockStatement> block, WfLexicalScopeManager* manager, WfExpression* observe, BindContext& context, BindCallbackInfo& info)
			{
				for (auto callbackInfo : info.observeCallbackInfos[observe])
				{
					auto attach = MakePtr<WfAttachEventExpression>();
					attach->event = ExpandObserveEvent(manager, observe, callbackInfo.eventIndex, context);
					attach->function = CreateReference(callbackInfo.callbackName);

					auto nullExpr = MakePtr<WfLiteralExpression>();
					nullExpr->value = WfLiteralValue::Null;

					auto protect = MakePtr<WfBinaryExpression>();
					protect->first = attach;
					protect->second = nullExpr;
					protect->op = WfBinaryOperator::FailedThen;

					auto assign = MakePtr<WfBinaryExpression>();
					assign->op = WfBinaryOperator::Assign;
					assign->first = CreateReference(callbackInfo.handlerName);
					assign->second = protect;

					auto stat = MakePtr<WfExpressionStatement>();
					stat->expression = assign;
					block->statements.Add(stat);
				}
			}

/***********************************************************************
CreateBindDetachStatement
***********************************************************************/

			void CreateBindDetachStatement(Ptr<WfBlockStatement> block, WfLexicalScopeManager* manager, WfExpression* observe, BindContext& context, BindCallbackInfo& info)
			{
				for (auto callbackInfo : info.observeCallbackInfos[observe])
				{
					auto testNull = MakePtr<WfTypeTestingExpression>();
					testNull->expression = CreateReference(callbackInfo.handlerName);
					testNull->test = WfTypeTesting::IsNotNull;

					auto ifStat = MakePtr<WfIfStatement>();
					ifStat->expression = testNull;

					auto trueBlock = MakePtr<WfBlockStatement>();
					ifStat->trueBranch = trueBlock;

					block->statements.Add(ifStat);
					{
						auto detach = MakePtr<WfDetachEventExpression>();
						detach->event = ExpandObserveEvent(manager, observe, callbackInfo.eventIndex, context);
						detach->handler = CreateReference(callbackInfo.handlerName);

						auto stat = MakePtr<WfExpressionStatement>();
						stat->expression = detach;
						trueBlock->statements.Add(stat);
					}
					{
						auto nullExpr = MakePtr<WfLiteralExpression>();
						nullExpr->value = WfLiteralValue::Null;

						auto assignExpr = MakePtr<WfBinaryExpression>();
						assignExpr->first = CreateReference(callbackInfo.handlerName);
						assignExpr->second = nullExpr;
						assignExpr->op = WfBinaryOperator::Assign;

						auto stat = MakePtr<WfExpressionStatement>();
						stat->expression = assignExpr;
						trueBlock->statements.Add(stat);
					}
				}
			}

/***********************************************************************
CreateBindCacheAssignStatement
***********************************************************************/

			void CreateBindCacheAssignStatement(WfLexicalScopeManager* manager, Ptr<WfBlockStatement> block, WfExpression* observe, BindContext& context)
			{
				auto parent = context.observeParents[observe];
				auto cacheName = context.GetCacheVariableName(context.GetCachedExpressionIndexRecursively(parent, true));

				auto protect = MakePtr<WfBinaryExpression>();
				protect->first = ExpandObserveExpressionVisitor::Execute(parent, context, false);
				protect->second = CreateDefaultValue(manager->expressionResolvings[parent].type.Obj());
				protect->op = WfBinaryOperator::FailedThen;

				auto assign = MakePtr<WfBinaryExpression>();
				assign->op = WfBinaryOperator::Assign;
				assign->first = CreateReference(cacheName);
				assign->second = protect;

				auto stat = MakePtr<WfExpressionStatement>();
				stat->expression = assign;
				block->statements.Add(stat);
			}

/***********************************************************************
IValueSubscription::Open
***********************************************************************/

			Ptr<WfFunctionDeclaration> CreateBindOpenFunction(WfLexicalScopeManager* manager, BindContext& context, BindCallbackInfo& info)
			{
				auto func = MakePtr<WfFunctionDeclaration>();
				func->name.value = L"Open";
				func->functionKind = WfFunctionKind::Normal;
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
						notExpr->operand = CreateReference(L"<bind-opened>");
						ifStat->expression = notExpr;
					}

					auto ifBlock = MakePtr<WfBlockStatement>();
					ifStat->trueBranch = ifBlock;
					{
						auto literal = MakePtr<WfLiteralExpression>();
						literal->value = WfLiteralValue::True;

						auto assign = MakePtr<WfBinaryExpression>();
						assign->op = WfBinaryOperator::Assign;
						assign->first = CreateReference(L"<bind-opened>");
						assign->second = literal;

						auto stat = MakePtr<WfExpressionStatement>();
						stat->expression = assign;
						ifBlock->statements.Add(stat);
					}
					{
						SortedList<vint> assignedParents;
						SortedList<WfExpression*> observes;
						CopyFrom(observes, context.observeParents.Keys());

						SortedList<WfExpression*> freeObserves;
						while (observes.Count() > 0)
						{
							CopyFrom(
								freeObserves,
								From(observes)
									.Where([&](WfExpression* observe)
									{
										return !context.observeCauses.Keys().Contains(observe) ||
											From(context.observeCauses[observe])
											.All([&](WfExpression* depended)
											{
												return !observes.Contains(depended);
											});
									})
								);

							for (auto observe : context.orderedObserves)
							{
								if (freeObserves.Contains(observe))
								{
									auto parent = context.GetCachedExpressionIndexRecursively(context.observeParents[observe], true);
									if (!assignedParents.Contains(parent))
									{
										assignedParents.Add(parent);
										CreateBindCacheAssignStatement(manager, ifBlock, observe, context);
									}
								}
							}

							for (auto observe : freeObserves)
							{
								observes.Remove(observe);
							}
							freeObserves.Clear();
						}
					}
					{
						for (auto observe : context.orderedObserves)
						{
							CreateBindAttachStatement(ifBlock, manager, observe, context, info);
						}
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
IValueSubscription::Update
***********************************************************************/

			Ptr<WfFunctionDeclaration> CreateBindUpdateFunction(BindCallbackInfo& info)
			{
				auto func = MakePtr<WfFunctionDeclaration>();
				func->name.value = L"Update";
				func->functionKind = WfFunctionKind::Normal;
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
						notExpr->operand = CreateReference(L"<bind-closed>");

						auto andExpr = MakePtr<WfBinaryExpression>();
						andExpr->op = WfBinaryOperator::And;
						andExpr->first = CreateReference(L"<bind-opened>");
						andExpr->second = notExpr;

						ifStat->expression = andExpr;
					}

					auto ifBlock = MakePtr<WfBlockStatement>();
					ifStat->trueBranch = ifBlock;
					{
						auto ref = CreateReference(L"<bind-activator>");

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

			Ptr<WfFunctionDeclaration> CreateBindCloseFunction(WfLexicalScopeManager* manager, BindContext& context, BindCallbackInfo& info)
			{
				auto func = MakePtr<WfFunctionDeclaration>();
				func->name.value = L"Close";
				func->functionKind = WfFunctionKind::Normal;
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
						notExpr->operand = CreateReference(L"<bind-closed>");
						ifStat->expression = notExpr;
					}

					auto ifBlock = MakePtr<WfBlockStatement>();
					ifStat->trueBranch = ifBlock;
					{
						auto literal = MakePtr<WfLiteralExpression>();
						literal->value = WfLiteralValue::True;

						auto assign = MakePtr<WfBinaryExpression>();
						assign->op = WfBinaryOperator::Assign;
						assign->first = CreateReference(L"<bind-closed>");
						assign->second = literal;

						auto stat = MakePtr<WfExpressionStatement>();
						stat->expression = assign;
						ifBlock->statements.Add(stat);
					}
					for (auto observe : context.orderedObserves)
					{
						CreateBindDetachStatement(ifBlock, manager, observe, context, info);
					}

					for (vint i = 0; i < context.cachedExprs.Count(); i++)
					{
						auto cacheName = context.GetCacheVariableName(i);
						auto type = manager->expressionResolvings[context.cachedExprs[i]].type;

						auto assign = MakePtr<WfBinaryExpression>();
						assign->op = WfBinaryOperator::Assign;
						assign->first = CreateReference(cacheName);
						assign->second = CreateDefaultValue(type.Obj());

						auto stat = MakePtr<WfExpressionStatement>();
						stat->expression = assign;
						ifBlock->statements.Add(stat);
					}
					for (vint i = 0; i < info.handlerVariables.Count(); i++)
					{
						auto cacheName = info.handlerVariables.Keys()[i];
						auto result = info.handlerVariables.Values()[i];

						auto assign = MakePtr<WfBinaryExpression>();
						assign->op = WfBinaryOperator::Assign;
						assign->first = CreateReference(cacheName);
						assign->second = CreateDefaultValue(result.Obj());

						auto stat = MakePtr<WfExpressionStatement>();
						stat->expression = assign;
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
				BindContext context;
				CreateBindContextVisitor(manager, context).Call(node->expression.Obj());
				BindCallbackInfo bcInfo;
						
				auto newSubscription = MakePtr<WfNewInterfaceExpression>();
				node->expandedExpression = newSubscription;
				{
					auto typeInfo = TypeInfoRetriver<Ptr<IValueSubscription>>::CreateTypeInfo();
					newSubscription->type = GetTypeFromTypeInfo(typeInfo.Obj());
				}
				{
					// stable symbol order by sorting them by code
					List<WfExpression*> orderedObserves;

					auto printExpression = [](WfExpression* observe)
					{
						return GenerateToStream([&](StreamWriter& writer)
						{
							WfPrint(observe, WString::Empty, writer);
						});
					};

					for (auto [parent, index] : indexed(context.cachedExprs))
					{
						WString cacheName = context.GetCacheVariableName(index);
						{
							auto elementType = manager->expressionResolvings[parent].type;
							newSubscription->declarations.Add(CreateWritableVariable(cacheName, elementType.Obj()));
						}
					}

					for (auto [observe, observeIndex] : indexed(context.orderedObserves))
					{
						const auto& events = context.observeEvents[observe];
						for (auto [ev, eventIndex] : indexed(events))
						{
							WString handlerName = L"<bind-handler>" + itow(observeIndex) + L"_" + itow(eventIndex);
							{
								auto elementType = TypeInfoRetriver<Ptr<IEventHandler>>::CreateTypeInfo();
								bcInfo.handlerVariables.Add(handlerName, elementType);
								newSubscription->declarations.Add(CreateWritableVariable(handlerName, elementType.Obj()));
							}
						
							WString callbackName = L"<bind-callback>" + itow(observeIndex) + L"_" + itow(eventIndex);
							{
								auto elementType = CopyTypeInfo(ev->GetHandlerType());
								bcInfo.callbackFunctions.Add(callbackName, elementType);
							}

							CallbackInfo callbackInfo;
							callbackInfo.observe = observe;
							callbackInfo.eventInfo = ev;
							callbackInfo.eventIndex = eventIndex;
							callbackInfo.callbackName = callbackName;
							callbackInfo.handlerName = handlerName;
							bcInfo.observeCallbackInfos.Add(observe, callbackInfo);
						}
					}
					newSubscription->declarations.Add(CreateWritableVariable(L"<bind-opened>", TypeInfoRetriver<bool>::CreateTypeInfo().Obj()));
					newSubscription->declarations.Add(CreateWritableVariable(L"<bind-closed>", TypeInfoRetriver<bool>::CreateTypeInfo().Obj()));
					{
						auto func = MakePtr<WfFunctionDeclaration>();
						func->name.value = L"<bind-activator>";
						func->functionKind = WfFunctionKind::Normal;
						func->anonymity = WfFunctionAnonymity::Named;
						func->returnType = GetTypeFromTypeInfo(TypeInfoRetriver<void>::CreateTypeInfo().Obj());
					
						auto block = MakePtr<WfBlockStatement>();
						func->statement = block;
						{
							auto var = MakePtr<WfVariableDeclaration>();
							var->name.value = L"<bind-activator-result>";
							var->expression = ExpandObserveExpressionVisitor::Execute(node->expression.Obj(), context);

							auto varStat = MakePtr<WfVariableStatement>();
							varStat->variable = var;
							block->statements.Add(varStat);
						}
						{
							auto call = MakePtr<WfCallExpression>();
							call->function = CreateReference(L"ValueChanged");
							call->arguments.Add(CreateReference(L"<bind-activator-result>"));

							auto stat = MakePtr<WfExpressionStatement>();
							stat->expression = call;
							block->statements.Add(stat);
						}
					
						newSubscription->declarations.Add(AssignNormalMember(func));
					}
					for (auto observe : context.orderedObserves)
					{
						for (auto callbackInfo : bcInfo.observeCallbackInfos[observe])
						{
							auto func = MakePtr<WfFunctionDeclaration>();
							func->name.value = callbackInfo.callbackName;
							func->functionKind = WfFunctionKind::Normal;
							func->anonymity = WfFunctionAnonymity::Named;
							{
								auto genericType = bcInfo.callbackFunctions[callbackInfo.callbackName]->GetElementType();
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
									vint dependencyIndex = context.observeAffects.Keys().IndexOf(current);
									if (dependencyIndex != -1)
									{
										for (auto affectedObserve : context.observeAffects.GetByIndex(dependencyIndex))
										{
											if (affectedObserve && !affected.Contains(affectedObserve))
											{
												affected.Add(affectedObserve);
											}
										}
									}
								}
								affected.Remove(observe);

								for (auto affectedObserve : From(affected).Reverse())
								{
									CreateBindDetachStatement(block, manager, affectedObserve, context, bcInfo);
								}
								{
									SortedList<vint> assignedParents;
									for (auto affectedObserve : affected)
									{
										auto parent = context.GetCachedExpressionIndexRecursively(context.observeParents[affectedObserve], true);
										if (!assignedParents.Contains(parent))
										{
											assignedParents.Add(parent);
											CreateBindCacheAssignStatement(manager, block, affectedObserve, context);
										}
									}
								}
								for (auto affectedObserve : affected)
								{
									CreateBindAttachStatement(block, manager, affectedObserve, context, bcInfo);
								}
							}
							{
								auto ref = CreateReference(L"<bind-activator>");

								auto call = MakePtr<WfCallExpression>();
								call->function = ref;

								auto stat = MakePtr<WfExpressionStatement>();
								stat->expression = call;
								block->statements.Add(stat);
							}

							newSubscription->declarations.Add(AssignNormalMember(func));
						}
					}
				}
				newSubscription->declarations.Add(AssignOverrideMember(CreateBindOpenFunction(manager, context, bcInfo)));
				newSubscription->declarations.Add(AssignOverrideMember(CreateBindUpdateFunction(bcInfo)));
				newSubscription->declarations.Add(AssignOverrideMember(CreateBindCloseFunction(manager, context, bcInfo)));
			}
		}
	}
}