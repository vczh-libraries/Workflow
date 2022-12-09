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
			using namespace typeimpl;

/***********************************************************************
Helper Functions
***********************************************************************/

			IMethodInfo* FindInterfaceConstructor(ITypeDescriptor* type)
			{
				if (auto ctors = type->GetConstructorGroup())
				{
					auto proxyTd = description::GetTypeDescriptor<IValueInterfaceProxy>();
					for (vint i = 0; i < ctors->GetMethodCount(); i++)
					{
						IMethodInfo* info = ctors->GetMethod(i);
						if (info->GetParameterCount() == 1)
						{
							ITypeInfo* parameterType = info->GetParameter(0)->GetType();
							if (parameterType->GetDecorator() == ITypeInfo::SharedPtr)
							{
								parameterType = parameterType->GetElementType();
								if (parameterType->GetDecorator() == ITypeInfo::TypeDescriptor && parameterType->GetTypeDescriptor() == proxyTd)
								{
									return info;
								}
							}
						}
					}
				}
				return nullptr;
			}

			ITypeInfo* GetFunctionType(const ResolveExpressionResult& result)
			{
				if (result.eventInfo)
				{
					return result.eventInfo->GetHandlerType();
				}
				else if (result.type)
				{
					return result.type.Obj();
				}
				else
				{
					return nullptr;
				}
			}

			Ptr<ITypeInfo> SelectFunction(WfLexicalScopeManager* manager, glr::ParsingAstBase* node, Ptr<WfExpression> functionExpression, List<ResolveExpressionResult>& functions, List<Ptr<WfExpression>>& arguments, vint& selectedFunctionIndex)
			{
				selectedFunctionIndex = -1;

				List<bool> resolvables;
				List<Ptr<ITypeInfo>> types;
				for (auto argument : arguments)
				{
					if (!argument || IsExpressionDependOnExpectedType(manager, argument, true))
					{
						resolvables.Add(false);
						types.Add(nullptr);
					}
					else
					{
						resolvables.Add(true);
						types.Add(GetExpressionType(manager, argument, nullptr));
					}
				}

				List<glr::ParsingError> functionErrors, nonFunctionErrors;
				List<Pair<vint, vint>> selectedFunctionIndices;
				ITypeDescriptor* functionFd = description::GetTypeDescriptor<IValueFunctionProxy>();
				for (vint i = 0; i < functions.Count(); i++)
				{
					bool failed = false;
					vint convertCount = 0;
					auto result = functions[i];
					ITypeInfo* expressionType = GetFunctionType(result);

					if (expressionType->GetDecorator() == ITypeInfo::SharedPtr)
					{
						ITypeInfo* genericType = expressionType->GetElementType();
						if (genericType->GetDecorator() != ITypeInfo::Generic) goto FUNCTION_TYPE_FAILED;
						ITypeInfo* functionType = genericType->GetElementType();
						if (functionType->GetDecorator() != ITypeInfo::TypeDescriptor || functionType->GetTypeDescriptor() != functionFd) goto FUNCTION_TYPE_FAILED;
								
						if (genericType->GetGenericArgumentCount() != types.Count() + 1)
						{
							functionErrors.Add(WfErrors::FunctionArgumentCountMismatched(node, arguments.Count(), result));
							failed = true;
						}
						else
						{
							for (vint j = 0; j < types.Count(); j++)
							{
								if (resolvables[j] && types[j])
								{
									ITypeInfo* argumentType = genericType->GetGenericArgument(j + 1);
									if (CanConvertToType(types[j].Obj(), argumentType, false))
									{
										if (!IsSameType(types[j].Obj(), argumentType))
										{
											convertCount++;
										}
									}
									else
									{
										functionErrors.Add(WfErrors::FunctionArgumentTypeMismatched(node, result, j + 1, types[j].Obj(), argumentType));
										failed = true;
									}
								}
							}
						}
					}
					else
					{
						goto FUNCTION_TYPE_FAILED;
					}

					goto FUNCTION_TYPE_FINISHED;
				FUNCTION_TYPE_FAILED:
					nonFunctionErrors.Add(WfErrors::ExpressionIsNotFunction(functionExpression.Obj(), result.type.Obj()));
					failed = true;
				FUNCTION_TYPE_FINISHED:
					if (!failed)
					{
						selectedFunctionIndices.Add({ i,convertCount });
					}
				}

				if (selectedFunctionIndices.Count() > 1)
				{
					vint minScore = From(selectedFunctionIndices)
						.Select([](Pair<vint, vint> p)
						{
							return p.value;
						})
						.Min();

					for (vint i = selectedFunctionIndices.Count() - 1; i >= 0; i--)
					{
						if (selectedFunctionIndices[i].value != minScore)
						{
							selectedFunctionIndices.RemoveAt(i);
						}
					}
				}

				if (selectedFunctionIndices.Count() == 1)
				{
					selectedFunctionIndex = selectedFunctionIndices[0].key;
					ITypeInfo* genericType = GetFunctionType(functions[selectedFunctionIndex])->GetElementType();
					for (vint i = 0; i < types.Count(); i++)
					{
						ITypeInfo* argumentType = genericType->GetGenericArgument(i + 1);
						if (resolvables[i])
						{
							vint index = manager->expressionResolvings.Keys().IndexOf(arguments[i].Obj());
							if (index != -1)
							{
								auto resolvingResult = manager->expressionResolvings.Values()[index];
								CHECK_ERROR(resolvingResult.expectedType == nullptr, L"GetExpressionType should not set expectedType if it is null");
								resolvingResult.expectedType = CopyTypeInfo(argumentType);
								manager->expressionResolvings.Set(arguments[i], resolvingResult);
							}
						}
						else
						{
							if (arguments[i])
							{
								GetExpressionType(manager, arguments[i], CopyTypeInfo(argumentType));
							}
						}
					}
					return CopyTypeInfo(genericType->GetGenericArgument(0));
				}
				else
				{
					if (selectedFunctionIndices.Count() > 1)
					{
						List<ResolveExpressionResult> overloadedFunctions;
						CopyFrom(
							overloadedFunctions,
							From(selectedFunctionIndices)
							.Select([&functions](Pair<vint, vint> p)
							{
								return functions[p.key];
							}));
						manager->errors.Add(WfErrors::CannotPickOverloadedFunctions(node, overloadedFunctions));
					}

					if (functionErrors.Count() > 0)
					{
						CopyFrom(manager->errors, functionErrors, true);
					}
					else
					{
						CopyFrom(manager->errors, nonFunctionErrors, true);
					}
					return nullptr;
				}
			}

/***********************************************************************
ValidateSemantic
***********************************************************************/
			
			void ValidateModuleSemantic(WfLexicalScopeManager* manager, Ptr<WfModule> module)
			{
				for (auto declaration : module->declarations)
				{
					ValidateDeclarationSemantic(manager, declaration);
				}
			}

/***********************************************************************
GetExpressionScopeName
***********************************************************************/

			Ptr<WfLexicalScopeName> GetExpressionScopeName(WfLexicalScopeManager* manager, WfExpression* expression)
			{
				List<ResolveExpressionResult> results;
				ValidateExpressionSemantic(manager, expression, 0, results);
				if (results.Count() == 0) return 0;

				for (vint i = results.Count() - 1; i >= 0; i--)
				{
					auto& result = results[i];
					if (!result.scopeName)
					{
						results.RemoveAt(i);
					}
				}
				if (results.Count() == 0)
				{
					manager->errors.Add(WfErrors::ExpressionIsNotScopeName(expression));
					return 0;
				}
				else if (results.Count() > 1)
				{
					manager->errors.Add(WfErrors::TooManyTargets(expression, results, GetExpressionName(expression)));
					return 0;
				}

				auto result = results[0];
				manager->expressionResolvings.Add(Ptr(expression), result);
				return result.scopeName;
			}

			Ptr<WfLexicalScopeName> GetExpressionScopeName(WfLexicalScopeManager* manager, Ptr<WfExpression> expression)
			{
				return GetExpressionScopeName(manager, expression.Obj());
			}

/***********************************************************************
GetExpressionEventInfo
***********************************************************************/

			reflection::description::IEventInfo* GetExpressionEventInfo(WfLexicalScopeManager* manager, WfExpression* expression)
			{
				List<ResolveExpressionResult> results;
				ValidateExpressionSemantic(manager, expression, 0, results);
				if (results.Count() == 0) return 0;

				for (vint i = results.Count() - 1; i >= 0; i--)
				{
					auto& result = results[i];
					if (!result.eventInfo)
					{
						results.RemoveAt(i);
					}
				}
				if (results.Count() == 0)
				{
					manager->errors.Add(WfErrors::ExpressionIsNotEvent(expression));
					return 0;
				}
				else if (results.Count() > 1)
				{
					manager->errors.Add(WfErrors::TooManyTargets(expression, results, GetExpressionName(expression)));
					return 0;
				}

				auto result = results[0];
				manager->expressionResolvings.Add(Ptr(expression), result);
				return result.eventInfo;
			}

			reflection::description::IEventInfo* GetExpressionEventInfo(WfLexicalScopeManager* manager, Ptr<WfExpression> expression)
			{
				return GetExpressionEventInfo(manager, expression.Obj());
			}

/***********************************************************************
GetExpressionTypes
***********************************************************************/

			void GetExpressionTypes(WfLexicalScopeManager* manager, WfExpression* expression, Ptr<reflection::description::ITypeInfo> expectedType, bool allowEvent, collections::List<ResolveExpressionResult>& results)
			{
				ValidateExpressionSemantic(manager, expression, expectedType, results);
				if (results.Count() == 0) return;

				Ptr<WfLexicalScopeName> scopeName;
				IEventInfo* eventInfo = 0;
				for (vint i = results.Count() - 1; i >= 0; i--)
				{
					auto& result = results[i];
					if (result.scopeName && !scopeName)
					{
						scopeName = result.scopeName;
					}
					if (result.eventInfo && !eventInfo)
					{
						eventInfo = result.eventInfo;
					}
					if (!result.type && !(allowEvent && eventInfo))
					{
						results.RemoveAt(i);
					}
				}
				if (results.Count() == 0)
				{
					if (scopeName)
					{
						manager->errors.Add(WfErrors::ScopeNameIsNotExpression(expression, scopeName));
					}
					if (eventInfo)
					{
						manager->errors.Add(WfErrors::EventIsNotExpression(expression, eventInfo));
					}
				}

				if (expectedType)
				{
					List<Ptr<ITypeInfo>> failedTypes;
					for (vint i = results.Count() - 1; i >= 0; i--)
					{
						auto& result = results[i];
						if (!CanConvertToType(result.type.Obj(), expectedType.Obj(), false))
						{
							failedTypes.Add(result.type);
							results.RemoveAt(i);
						}
					}

					if (results.Count() == 0)
					{
						for (auto type : failedTypes)
						{
							manager->errors.Add(WfErrors::ExpressionCannotImplicitlyConvertToType(expression, type.Obj(), expectedType.Obj()));
						}
					}
				}
			}

			void GetExpressionTypes(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType, bool allowEvent, collections::List<ResolveExpressionResult>& results)
			{
				return GetExpressionTypes(manager, expression.Obj(), expectedType, allowEvent, results);
			}

/***********************************************************************
GetExpressionType
***********************************************************************/

			Ptr<reflection::description::ITypeInfo> GetExpressionType(WfLexicalScopeManager* manager, WfExpression* expression, Ptr<reflection::description::ITypeInfo> expectedType)
			{
				List<ResolveExpressionResult> results;
				GetExpressionTypes(manager, expression, expectedType, false, results);
				
				if (results.Count() > 1)
				{
					manager->errors.Add(WfErrors::TooManyTargets(expression, results, GetExpressionName(expression)));
					return expectedType;
				}
				else if (results.Count() == 1)
				{
					auto result = results[0];
					result.expectedType = expectedType;
					manager->expressionResolvings.Add(Ptr(expression), result);
					return expectedType ? expectedType : result.type;
				}
				else
				{
					return expectedType;
				}
			}

			Ptr<reflection::description::ITypeInfo> GetExpressionType(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType)
			{
				return GetExpressionType(manager, expression.Obj(), expectedType);
			}

/***********************************************************************
GetLeftValueExpressionType
***********************************************************************/

			Ptr<reflection::description::ITypeInfo>	GetLeftValueExpressionType(WfLexicalScopeManager* manager, WfExpression* expression)
			{
				List<ResolveExpressionResult> results;
				GetExpressionTypes(manager, expression, nullptr, false, results);

				if (results.Count() > 1)
				{
					manager->errors.Add(WfErrors::TooManyTargets(expression, results, GetExpressionName(expression)));
				}
				else if (results.Count() == 1)
				{
					if (results[0].writableType)
					{
						auto result = results[0];
						manager->expressionResolvings.Add(Ptr(expression), result);
						return result.writableType;
					}
					else
					{
						manager->errors.Add(WfErrors::ExpressionIsNotLeftValue(expression, results[0]));
					}
				}
				return nullptr;
			}

			Ptr<reflection::description::ITypeInfo>	GetLeftValueExpressionType(WfLexicalScopeManager* manager, Ptr<WfExpression> expression)
			{
				return GetLeftValueExpressionType(manager, expression.Obj());
			}

/***********************************************************************
GetEnumerableExpressionItemType
***********************************************************************/

			Ptr<reflection::description::ITypeInfo>	GetEnumerableExpressionItemType(WfLexicalScopeManager* manager, WfExpression* expression, Ptr<reflection::description::ITypeInfo> expectedType)
			{
				Ptr<ITypeInfo> collectionType = GetExpressionType(manager, expression, expectedType);
				if (collectionType)
				{
					if (collectionType->GetTypeDescriptor()->CanConvertTo(description::GetTypeDescriptor<IValueEnumerable>()))
					{
						if (collectionType->GetDecorator() == ITypeInfo::SharedPtr)
						{
							ITypeInfo* genericType = collectionType->GetElementType();
							if (genericType->GetDecorator() == ITypeInfo::Generic && genericType->GetGenericArgumentCount() == 1)
							{
								return CopyTypeInfo(genericType->GetGenericArgument(0));
							}
						}
						return TypeInfoRetriver<Value>::CreateTypeInfo();
					}

					manager->errors.Add(WfErrors::ExpressionIsNotCollection(expression, collectionType.Obj()));
				}
				return nullptr;
			}

			Ptr<reflection::description::ITypeInfo>	GetEnumerableExpressionItemType(WfLexicalScopeManager* manager, Ptr<WfExpression> expression, Ptr<reflection::description::ITypeInfo> expectedType)
			{
				return GetEnumerableExpressionItemType(manager, expression.Obj(), expectedType);
			}
		}
	}
}
