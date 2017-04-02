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

			class GetObservingDependencyVisitor
				: public traverse_visitor::ExpressionVisitor
				, public traverse_visitor::VirtualExpressionVisitor
			{
			public:
				WfLexicalScopeManager*				manager;
				WfObservingDependency&				dependency;

				GetObservingDependencyVisitor(WfLexicalScopeManager* _manager, WfObservingDependency& _dependency)
					:manager(_manager)
					, dependency(_dependency)
				{
				}

				void VisitField(WfExpression* node)override
				{
					node->Accept(this);
				}

				void VisitField(WfType* node)override
				{
					// No need to traverse inside a type
				}

				void VisitField(WfStatement* node)override
				{
					// No need to traverse inside a statement
				}

				void VisitField(WfDeclaration* node)override
				{
					// No need to traverse inside a declaration
				}

				void Dispatch(WfVirtualExpression* node)override
				{
					node->Accept(static_cast<traverse_visitor::VirtualExpressionVisitor*>(this));
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

				void Visit(WfObserveExpression* node)override
				{
					WfObservingDependency parent(dependency);
					GetObservingDependency(manager, node->parent, parent);
					parent.TurnToInput();

					dependency.Add(node, parent);
					dependency.TurnToInput();
					GetObservingDependency(manager, node->expression, dependency);
				}
			};

			void GetObservingDependency(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, WfObservingDependency& dependency)
			{
				GetObservingDependencyVisitor visitor(manager, dependency);
				expression->Accept(&visitor);
			}

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
				Dictionary<WfExpression*, WString>&		cacheNames;
				Dictionary<WString, WString>&			referenceReplacement;

				ExpandObserveExpressionVisitor(Dictionary<WfExpression*, WString>& _cacheNames, collections::Dictionary<WString, WString>& _referenceReplacement)
					:cacheNames(_cacheNames)
					, referenceReplacement(_referenceReplacement)
				{
				}

				vl::Ptr<WfExpression> CreateField(vl::Ptr<WfExpression> from)override
				{
					return ExpandObserveExpression(from.Obj(), cacheNames, referenceReplacement);
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
					vint index = referenceReplacement.Keys().IndexOf(node->name.value);
					if (index == -1)
					{
						result = CopyExpression(node);
					}
					else
					{
						result = CreateReference(referenceReplacement.Values()[index]);
					}
				}

				void Visit(WfOrderedLambdaExpression* node)override
				{
					result = CopyExpression(node);
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
						newVar->value = CreateField(var->value);
						expr->variables.Add(newVar);
					}

					expr->expression = CreateField(node->expression);
					CopyFrom(referenceReplacement, overrided, true);

					result = expr;
				}

				void Visit(WfObserveExpression* node)override
				{
					if (cacheNames.Count() == 0)
					{
						result = CopyExpression(node);
					}
					else if (node->observeType == WfObserveType::SimpleObserve)
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

				void Visit(WfBindExpression* node)override
				{
					result = CopyExpression(node);
				}
			};

			Ptr<WfExpression> ExpandObserveExpression(WfExpression* expression, collections::Dictionary<WfExpression*, WString>& cacheNames, collections::Dictionary<WString, WString>& referenceReplacement, bool useCache)
			{
				if (!expression)
				{
					return nullptr;
				}

				if (useCache)
				{
					vint index = cacheNames.Keys().IndexOf(expression);
					if (index != -1)
					{
						return CreateReference(cacheNames.Values()[index]);
					}
				}

				ExpandObserveExpressionVisitor visitor(cacheNames, referenceReplacement);
				expression->Accept(&visitor);
				return visitor.result.Cast<WfExpression>();
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

							auto inferExpr = MakePtr<WfInferExpression>();
							inferExpr->expression = inferExpr;
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
					returnStat->expression =  CreateReference(L"<subscription>");
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
					auto listenersRef = CreateReference(L"<bind-listeners>");

					auto keysExpr = MakePtr<WfMemberExpression>();
					keysExpr->parent = listenersRef;
					keysExpr->name.value = L"Keys";

					auto containsExpr = MakePtr<WfMemberExpression>();
					containsExpr->parent = keysExpr;
					containsExpr->name.value = L"Contains";

					auto callExpr = MakePtr<WfCallExpression>();
					callExpr->function = containsExpr;
					callExpr->arguments.Add(MakePtr<WfThisExpression>());

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
						auto listenersRef = CreateReference(L"<bind-listeners>");

						auto keysExpr = MakePtr<WfMemberExpression>();
						keysExpr->parent = listenersRef;
						keysExpr->name.value = L"Keys";

						auto containsExpr = MakePtr<WfMemberExpression>();
						containsExpr->parent = keysExpr;
						containsExpr->name.value = L"Contains";

						auto callExpr = MakePtr<WfCallExpression>();
						callExpr->function = containsExpr;
						callExpr->arguments.Add(MakePtr<WfThisExpression>());
						ifStat->expression = callExpr;
					}

					auto ifBlock = MakePtr<WfBlockStatement>();
					ifStat->trueBranch = ifBlock;
					{
						auto listenersRef = CreateReference(L"<bind-listeners>");

						auto removeExpr = MakePtr<WfMemberExpression>();
						removeExpr->parent = listenersRef;
						removeExpr->name.value = L"Remove";

						auto callExpr = MakePtr<WfCallExpression>();
						callExpr->function = removeExpr;
						callExpr->arguments.Add(MakePtr<WfThisExpression>());

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
				{
					auto notOpenedRef = MakePtr<WfUnaryExpression>();
					notOpenedRef->op = WfUnaryOperator::Not;
					notOpenedRef->operand = CreateReference(L"<bind-opened>");

					auto ifStat = MakePtr<WfIfStatement>();
					block->statements.Add(ifStat);
					ifStat->expression = notOpenedRef;

					auto block = MakePtr<WfBlockStatement>();
					ifStat->trueBranch = block;

					{
						auto assignExpr = MakePtr<WfBinaryExpression>();
						assignExpr->op = WfBinaryOperator::Assign;
						assignExpr->first = CreateReference(L"<bind-opened>");

						auto trueValue = MakePtr<WfLiteralExpression>();
						trueValue->value = WfLiteralValue::True;
						assignExpr->second = trueValue;

						auto stat = MakePtr<WfExpressionStatement>();
						stat->expression = assignExpr;
						block->statements.Add(stat);
					}
					{
						auto callExpr = MakePtr<WfCallExpression>();
						callExpr->function = CreateReference(L"<bind-initialize>");

						auto stat = MakePtr<WfExpressionStatement>();
						stat->expression = callExpr;
						block->statements.Add(stat);
					}
				}
				{
					auto thisExpr = MakePtr<WfThisExpression>();
					block->statements.Add(CreateWritableVariableStatement(L"<subscription>", TypeInfoRetriver<IValueSubscription*>::CreateTypeInfo().Obj(), thisExpr));
				}
				{
					auto newListener = MakePtr<WfNewInterfaceExpression>();
					{
						auto typeInfo = TypeInfoRetriver<Ptr<IValueListener>>::CreateTypeInfo();
						newListener->type = GetTypeFromTypeInfo(typeInfo.Obj());
					}
					newListener->declarations.Add(AssignOverrideMember(CreateListenerGetSubscriptionFunction()));
					newListener->declarations.Add(AssignOverrideMember(CreateListenerGetStoppedFunction()));
					newListener->declarations.Add(AssignOverrideMember(CreateListenerStopListeningFunction()));

					auto variable = MakePtr<WfVariableDeclaration>();
					variable->name.value = L"<listener-shared>";
					variable->expression = newListener;
					
					auto variableStat = MakePtr<WfVariableStatement>();
					variableStat->variable = variable;
					block->statements.Add(variableStat);
				}
				{
					auto callbackRef = CreateReference(L"<bind-callback>");

					auto mapRef = CreateReference(L"<bind-listeners>");

					auto func = MakePtr<WfMemberExpression>();
					func->parent = mapRef;
					func->name.value = L"Set";

					auto call = MakePtr<WfCallExpression>();
					call->function = func;
					call->arguments.Add(CreateReference(L"<listener-shared>"));
					call->arguments.Add(callbackRef);

					auto stat = MakePtr<WfExpressionStatement>();
					stat->expression = call;
					block->statements.Add(stat);
				}
				{
					auto listenerRef = CreateReference(L"<listener-shared>");

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
						expr->parent = CreateReference(cacheName);
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
					expr->parent = CreateReference(cacheName);
					expr->name.value = eventName;

					return expr;
				}
			}

/***********************************************************************
ObserveInfo
***********************************************************************/

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
				Dictionary<WfExpression*, WfExpression*>		observeParents;
				Dictionary<WfExpression*, WString>				cacheNames;
				Dictionary<WString, Ptr<ITypeInfo>>				variableTypes;

				Dictionary<vint, WfExpression*>					orderedObserves;
				Group<WfExpression*, CallbackInfo>				observeCallbackInfos;
			};

/***********************************************************************
CreateBindAttachStatement
***********************************************************************/

			void CreateBindAttachStatement(Ptr<WfBlockStatement> block, WfLexicalScopeManager* manager, WfExpression* observe, BindCallbackInfo& info)
			{
				auto cachedName = info.cacheNames[info.observeParents[observe]];
				FOREACH(CallbackInfo, callbackInfo, info.observeCallbackInfos[observe])
				{
					auto attach = MakePtr<WfAttachEventExpression>();
					attach->event = ExpandObserveEvent(manager, cachedName, observe, callbackInfo.eventIndex);
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

			void CreateBindDetachStatement(Ptr<WfBlockStatement> block, WfLexicalScopeManager* manager, WfExpression* observe, BindCallbackInfo& info)
			{
				auto cachedName = info.cacheNames[info.observeParents[observe]];
				FOREACH(CallbackInfo, callbackInfo, info.observeCallbackInfos[observe])
				{
					auto detach = MakePtr<WfDetachEventExpression>();
					detach->event = ExpandObserveEvent(manager, cachedName, observe, callbackInfo.eventIndex);
					detach->handler = CreateReference(callbackInfo.handlerName);

					auto stat = MakePtr<WfExpressionStatement>();
					stat->expression = detach;
					block->statements.Add(stat);
				}
			}

/***********************************************************************
CreateBindCacheAssignStatement
***********************************************************************/

			void CreateBindCacheAssignStatement(Ptr<WfBlockStatement> block, WfExpression* observe, BindCallbackInfo& info)
			{
				auto parent = info.observeParents[observe];
				auto name = info.cacheNames[parent];

				auto assign = MakePtr<WfBinaryExpression>();
				assign->op = WfBinaryOperator::Assign;
				assign->first = CreateReference(name);
				Dictionary<WString, WString> referenceReplacement;
				assign->second = ExpandObserveExpression(parent, info.cacheNames, referenceReplacement, false);

				auto stat = MakePtr<WfExpressionStatement>();
				stat->expression = assign;
				block->statements.Add(stat);
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
						ifStat->expression = notExpr;
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
						auto ref = CreateReference(L"<bind-listeners>");

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
						auto typeInfo = TypeInfoRetriver<Dictionary<Ptr<IValueListener>, Func<void(Value)>>>::CreateTypeInfo();
						auto decl = MakePtr<WfVariableDeclaration>();
						decl->name.value = L"<bind-listeners>";
						decl->type = GetTypeFromTypeInfo(typeInfo.Obj());
						decl->expression = MakePtr<WfConstructorExpression>();
						newSubscription->declarations.Add(AssignNormalMember(decl));
					}
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
							auto ref = CreateReference(L"<bind-listeners>");

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

							auto refResult = CreateReference(L"<bind-activator-result>");

							auto refFunction = CreateReference(L"<bind-callback>");

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
					{
						auto func = MakePtr<WfFunctionDeclaration>();
						func->name.value = L"<bind-initialize>";
						func->anonymity = WfFunctionAnonymity::Named;
						func->returnType = GetTypeFromTypeInfo(TypeInfoRetriver<void>::CreateTypeInfo().Obj());
					
						auto block = MakePtr<WfBlockStatement>();
						func->statement = block;
						{
							Group<WfExpression*, WfExpression*> reversedDependencies;
							FOREACH_INDEXER(WfExpression*, key, index, dependency.dependencies.Keys())
							{
								FOREACH(WfExpression*, value, dependency.dependencies.GetByIndex(index))
								{
									if (value)
									{
										reversedDependencies.Add(value, key);
									}
								}
							}

							SortedList<WfExpression*> freeObserves;
							while (reversedDependencies.Count() > 0)
							{
								CopyFrom(
									freeObserves,
									From(reversedDependencies.Keys())
										.Where([&](WfExpression* observe)
										{
											return From(reversedDependencies[observe])
												.All([&](WfExpression* parent)
											{
												return !reversedDependencies.Contains(parent);
											});
										})
									);

								FOREACH(WfExpression*, observe, bcInfo.orderedObserves.Values())
								{
									if (freeObserves.Contains(observe))
									{
										CreateBindCacheAssignStatement(block, observe, bcInfo);
									}
								}

								FOREACH(WfExpression*, observe, freeObserves)
								{
									reversedDependencies.Remove(observe);
								}
								freeObserves.Clear();
							}
						}
						{
							FOREACH(WfExpression*, observe, bcInfo.orderedObserves.Values())
							{
								CreateBindAttachStatement(block, manager, observe, bcInfo);
							}
						}
					
						newSubscription->declarations.Add(AssignNormalMember(func));
					}
				}
				newSubscription->declarations.Add(AssignOverrideMember(CreateBindSubscribeFunction()));
				newSubscription->declarations.Add(AssignOverrideMember(CreateBindUpdateFunction(bcInfo)));
				newSubscription->declarations.Add(AssignOverrideMember(CreateBindCloseFunction(manager, bcInfo)));
			}
		}
	}
}