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
				Dictionary<vint, WfExpression*>					orderedObserves;
				Group<WfExpression*, CallbackInfo>				observeCallbackInfos;
			};

/***********************************************************************
CreateBindingContext
***********************************************************************/

			class CreateBindingContextVisitor : public Object, public WfExpression::IVisitor
			{
			public:
				WfLexicalScopeManager*				manager;
				BindContext&						context;

				CreateBindingContextVisitor(WfLexicalScopeManager* _manager, BindContext& _context)
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
						FOREACH(WfExpression*, observe, context.exprCauses.GetByIndex(index))
						{
							context.observeAffects.Add(observe, expr);
							context.observeCauses.Add(expr, observe);
						}
					}
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
						FOREACH(WfExpression*, observe, context.exprCauses.GetByIndex(index))
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
									return letVar->name.value = node->name.value;
								})
								.First();
							context.renames.Add(node, letVar->value.Obj());
							DirectDepend(node, letVar->value.Obj());
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
					FOREACH(Ptr<WfLetVariable>, var, node->variables)
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
					FOREACH(Ptr<WfConstructorArgument>, argument, node->arguments)
					{
						DirectDepend(node, argument->key.Obj());
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
					FOREACH(Ptr<WfExpression>, eventExpr, node->events)
					{
						auto result = manager->expressionResolvings[eventExpr.Obj()];
						context.observeEvents.Add(node, result.eventInfo);
					}
					Call(node->expression.Obj());
				}

				void Visit(WfCallExpression* node)override
				{
					DirectDepend(node, node->function.Obj());
					FOREACH(Ptr<WfExpression>, argument, node->arguments)
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
					FOREACH(Ptr<WfExpression>, argument, node->arguments)
					{
						DirectDepend(node, argument.Obj());
					}
				}

				void Visit(WfNewInterfaceExpression* node)override
				{
					// root expression, nothing to do
				}

				void Visit(WfVirtualExpression* node)override
				{
					DirectDepend(node, node->expandedExpression.Obj());
				}
			};

/***********************************************************************
Copy(Type|Expression|Statement|Declaration)
***********************************************************************/

			Ptr<WfType> CopyType(Ptr<WfType> type)
			{
				return copy_visitor::ModuleVisitor().CreateField(type);
			}

			Ptr<WfExpression> CopyExpression(Ptr<WfExpression> expression)
			{
				return copy_visitor::ModuleVisitor().CreateField(expression);
			}

			Ptr<WfStatement> CopyStatement(Ptr<WfStatement> statement)
			{
				return copy_visitor::ModuleVisitor().CreateField(statement);
			}

			Ptr<WfDeclaration> CopyDeclaration(Ptr<WfDeclaration> declaration)
			{
				return copy_visitor::ModuleVisitor().CreateField(declaration);
			}

/***********************************************************************
ExpandObserveExpression
***********************************************************************/

			Ptr<WfExpression> CreateReference(const WString& name)
			{
				auto ref = MakePtr<WfReferenceExpression>();
				ref->name.value = name;
				return ref;
			}

			class ExpandObserveExpressionVisitor
				: public copy_visitor::ExpressionVisitor
				, public copy_visitor::VirtualExpressionVisitor
			{
			public:
				BindContext&									context;

				ExpandObserveExpressionVisitor(BindContext& _context)
					:context(_context)
				{
				}

				static Ptr<WfExpression> Execute(WfExpression* expression, BindContext& context)
				{
					if (!expression)
					{
						return nullptr;
					}

					ExpandObserveExpressionVisitor visitor(context);
					expression->Accept(&visitor);
					return visitor.result.Cast<WfExpression>();
				}

				vl::Ptr<WfExpression> CreateField(vl::Ptr<WfExpression> from)override
				{
					return Execute(from.Obj(), context);
				}

				vl::Ptr<WfType> CreateField(vl::Ptr<WfType> from)override
				{
					return CopyType(from);
				}

				vl::Ptr<WfStatement> CreateField(vl::Ptr<WfStatement> from)override
				{
					return CopyStatement(from);
				}

				vl::Ptr<WfDeclaration> CreateField(vl::Ptr<WfDeclaration> from)override
				{
					return CopyDeclaration(from);
				}

				vl::Ptr<vl::parsing::ParsingTreeCustomBase> Dispatch(WfVirtualExpression* node)override
				{
					node->Accept((WfVirtualExpression::IVisitor*)this);
					return result;
				}

				void Visit(WfReferenceExpression* node)override
				{
					vint index = context.GetCachedExpressionIndexRecursively(node, false);
					if (index == -1)
					{
						result = CopyExpression(node);
					}
					else
					{
						result = CreateReference(context.GetCacheVariableName(index));
					}
				}

				void Visit(WfLetExpression* node)override
				{
					auto letExpr = MakePtr<WfLetExpression>();
					FOREACH(Ptr<WfLetVariable>, var, node->variables)
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
						expr->parent = CreateField(node->parent);
						expr->name.value = node->expression.Cast<WfReferenceExpression>()->name.value;
						result = expr;
					}
					else
					{
						auto var = MakePtr<WfLetVariable>();
						var->name.value = node->name.value;
						var->value = CreateField(node->parent);

						auto expr = MakePtr<WfLetExpression>();
						expr->variables.Add(var);
						expr->expression = CreateField(node->expression);
						result = expr;
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
					auto value = valueType->CreateDefault();
					switch (GetTypeFlag(elementType))
					{
					case TypeFlag::Enum:
						{
							auto intExpr = MakePtr<WfIntegerExpression>();
							intExpr->value.value = u64tow(elementType->GetTypeDescriptor()->GetEnumType()->FromEnum(value));

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
							elementType->GetTypeDescriptor()->GetSerializableType()->Serialize(value, stringExpr->value.value);
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
								auto stringExpr = MakePtr<WfStringExpression>();
								elementType->GetTypeDescriptor()->GetSerializableType()->Serialize(value, stringExpr->value.value);

								auto castExpr = MakePtr<WfTypeCastingExpression>();
								castExpr->strategy = WfTypeCastingStrategy::Strong;
								castExpr->expression = stringExpr;
								castExpr->type = GetTypeFromTypeInfo(elementType);

								return castExpr;
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

			Ptr<WfDeclaration> AssignNormalMember(Ptr<WfDeclaration> decl)
			{
				decl->classMember = MakePtr<WfClassMember>();
				decl->classMember->kind = WfClassMemberKind::Normal;
				return decl;
			}

			Ptr<WfDeclaration> AssignOverrideMember(Ptr<WfDeclaration> decl)
			{
				decl->classMember = MakePtr<WfClassMember>();
				decl->classMember->kind = WfClassMemberKind::Override;
				return decl;
			}

/***********************************************************************
ExpandObserveEvent
***********************************************************************/

			Ptr<WfExpression> ExpandObserveEvent(WfLexicalScopeManager* manager, WfExpression* observe, vint eventIndex, BindContext& context)
			{
				auto cacheName = context.GetCacheVariableName(context.GetCachedExpressionIndexRecursively(observe, true));
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
				FOREACH(CallbackInfo, callbackInfo, info.observeCallbackInfos[observe])
				{
					auto attach = MakePtr<WfAttachEventExpression>();
					attach->event = ExpandObserveEvent(manager, observe, callbackInfo.eventIndex, context);
					attach->function = CreateReference(callbackInfo.callbackName);

					auto assign = MakePtr<WfBinaryExpression>();
					assign->op = WfBinaryOperator::Assign;
					assign->first = CreateReference(callbackInfo.handlerName);
					assign->second = attach;

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
				FOREACH(CallbackInfo, callbackInfo, info.observeCallbackInfos[observe])
				{
					auto detach = MakePtr<WfDetachEventExpression>();
					detach->event = ExpandObserveEvent(manager, observe, callbackInfo.eventIndex, context);
					detach->handler = CreateReference(callbackInfo.handlerName);

					auto stat = MakePtr<WfExpressionStatement>();
					stat->expression = detach;
					block->statements.Add(stat);
				}
			}

/***********************************************************************
CreateBindCacheAssignStatement
***********************************************************************/

			void CreateBindCacheAssignStatement(Ptr<WfBlockStatement> block, WfExpression* observe, BindContext& context)
			{
				auto parent = context.observeParents[observe];
				auto cacheName = context.GetCacheVariableName(context.GetCachedExpressionIndexRecursively(parent, true));

				auto assign = MakePtr<WfBinaryExpression>();
				assign->op = WfBinaryOperator::Assign;
				assign->first = CreateReference(cacheName);
				assign->second = ExpandObserveExpressionVisitor::Execute(parent, context);

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
						Group<WfExpression*, WfExpression*> observeCauses;
						CopyFrom(observeCauses, context.observeCauses);

						SortedList<WfExpression*> freeObserves;
						while (observeCauses.Count() > 0)
						{
							CopyFrom(
								freeObserves,
								From(observeCauses.Keys())
									.Where([&](WfExpression* observe)
									{
										return From(observeCauses[observe])
											.All([&](WfExpression* depended)
											{
												return !observeCauses.Contains(depended);
											});
									})
								);

							FOREACH(WfExpression*, observe, info.orderedObserves.Values())
							{
								if (freeObserves.Contains(observe))
								{
									CreateBindCacheAssignStatement(ifBlock, observe, context);
								}
							}

							FOREACH(WfExpression*, observe, freeObserves)
							{
								observeCauses.Remove(observe);
							}
							freeObserves.Clear();
						}
					}
					{
						FOREACH(WfExpression*, observe, info.orderedObserves.Values())
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

			Ptr<WfFunctionDeclaration> CreateBindCloseFunction(WfLexicalScopeManager* manager, BindCallbackInfo& info)
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
					FOREACH(WfExpression*, observe, info.orderedObserves.Values())
					{
						WString cachedName = info.cacheNames[info.observeParents[observe]];
						CreateBindDetachStatement(ifBlock, manager, observe, info);
					}

					SortedList<WString> callbackFunctions;
					for (vint i = 0; i < info.observeCallbackInfos.Count(); i++)
					{
						const auto& values = info.observeCallbackInfos.GetByIndex(i);
						FOREACH(CallbackInfo, callbackInfo, values)
						{
							callbackFunctions.Add(callbackInfo.callbackName);
						}
					}
					FOREACH_INDEXER(WString, name, index, info.variableTypes.Keys())
					{
						if (!callbackFunctions.Contains(name))
						{
							auto assign = MakePtr<WfBinaryExpression>();
							assign->op = WfBinaryOperator::Assign;
							assign->first = CreateReference(name);
							assign->second = CreateDefaultValue(info.variableTypes.Values()[index].Obj());

							auto stat = MakePtr<WfExpressionStatement>();
							stat->expression = assign;
							ifBlock->statements.Add(stat);
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
ExpandBindExpression
***********************************************************************/

			void ExpandBindExpression(WfLexicalScopeManager* manager, WfBindExpression* node)
			{
				Group<WfExpression*, WfExpression*> group;
				WfObservingDependency dependency(group);
				GetObservingDependency(manager, node->expression, dependency);
				dependency.Cleanup();

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
						stream::MemoryStream stream;
						{
							stream::StreamWriter writer(stream);
							WfPrint(observe, WString::Empty, writer);
						}
						stream.SeekFromBegin(0);
						{
							stream::StreamReader reader(stream);
							return reader.ReadToEnd();
						}
					};

					CopyFrom(
						orderedObserves,
						From(dependency.dependencies.Keys())
							.Where([](WfExpression* expr)
							{
								return expr != nullptr;
							})
							.OrderBy([&](WfExpression* a, WfExpression* b)
							{
								auto codeA = printExpression(a);
								auto codeB = printExpression(b);
								auto compare = WString::Compare(codeA, codeB);
								if (compare) return compare;
								return a->codeRange.start.index - b->codeRange.start.index;
							})
						);

					FOREACH_INDEXER(WfExpression*, observe, observeIndex, orderedObserves)
					{
						List<IEventInfo*> events;
						WfExpression* parent = 0;
						DecodeObserveExpression(manager, observe, events, parent);

						WString cacheName = L"<bind-cache>" + itow(observeIndex);
						bcInfo.cacheNames.Add(parent, cacheName);
						bcInfo.observeParents.Add(observe, parent);
						bcInfo.orderedObserves.Add(observeIndex, observe);
						{
							auto elementType = manager->expressionResolvings[parent].type;
							bcInfo.variableTypes.Add(cacheName, elementType);
							newSubscription->declarations.Add(AssignNormalMember(CreateWritableVariable(cacheName, elementType.Obj())));
						}

						FOREACH_INDEXER(IEventInfo*, ev, eventIndex, events)
						{
							WString handlerName = L"<bind-handler>" + itow(observeIndex) + L"_" + itow(eventIndex);
							{
								auto elementType = TypeInfoRetriver<Ptr<IEventHandler>>::CreateTypeInfo();
								bcInfo.variableTypes.Add(handlerName, elementType);
								newSubscription->declarations.Add(AssignNormalMember(CreateWritableVariable(handlerName, elementType.Obj())));
							}
						
							WString callbackName = L"<bind-callback>" + itow(observeIndex) + L"_" + itow(eventIndex);
							{
								auto elementType = CopyTypeInfo(ev->GetHandlerType());
								bcInfo.variableTypes.Add(callbackName, elementType);
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
					newSubscription->declarations.Add(AssignNormalMember(CreateWritableVariable(L"<bind-opened>", TypeInfoRetriver<bool>::CreateTypeInfo().Obj())));
					newSubscription->declarations.Add(AssignNormalMember(CreateWritableVariable(L"<bind-closed>", TypeInfoRetriver<bool>::CreateTypeInfo().Obj())));
					{
						auto func = MakePtr<WfFunctionDeclaration>();
						func->name.value = L"<bind-activator>";
						func->anonymity = WfFunctionAnonymity::Named;
						func->returnType = GetTypeFromTypeInfo(TypeInfoRetriver<void>::CreateTypeInfo().Obj());
					
						auto block = MakePtr<WfBlockStatement>();
						func->statement = block;
						{
							auto var = MakePtr<WfVariableDeclaration>();
							var->name.value = L"<bind-activator-result>";
							Dictionary<WString, WString> referenceReplacement;
							var->expression = ExpandObserveExpression(node->expression.Obj(), bcInfo.cacheNames, referenceReplacement);

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
					FOREACH(WfExpression*, observe, bcInfo.orderedObserves.Values())
					{
						FOREACH(CallbackInfo, callbackInfo, bcInfo.observeCallbackInfos[observe])
						{
							auto func = MakePtr<WfFunctionDeclaration>();
							func->name.value = callbackInfo.callbackName;
							func->anonymity = WfFunctionAnonymity::Named;
							{
								auto genericType = bcInfo.variableTypes[callbackInfo.callbackName]->GetElementType();
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
									CreateBindDetachStatement(block, manager, affectedObserve, bcInfo);
								}
								FOREACH(WfExpression*, affectedObserve, affected)
								{
									CreateBindCacheAssignStatement(block, affectedObserve, bcInfo);
								}
								FOREACH(WfExpression*, affectedObserve, affected)
								{
									CreateBindAttachStatement(block, manager, affectedObserve, bcInfo);
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
				newSubscription->declarations.Add(AssignOverrideMember(CreateBindOpenFunction(manager, dependency, bcInfo)));
				newSubscription->declarations.Add(AssignOverrideMember(CreateBindUpdateFunction(bcInfo)));
				newSubscription->declarations.Add(AssignOverrideMember(CreateBindCloseFunction(manager, bcInfo)));
			}
		}
	}
}