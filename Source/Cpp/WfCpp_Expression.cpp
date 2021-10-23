#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;
			using namespace regex;
			using namespace stream;
			using namespace reflection;
			using namespace reflection::description;
			using namespace analyzer;

/***********************************************************************
Expression Helpers
***********************************************************************/

			template<typename T>
			void WriteBoxValue(WfCppConfig* config, stream::StreamWriter& writer, ITypeInfo* type, const T& writeExpression)
			{
				writer.WriteString(L"::vl::__vwsn::Box(");
				writeExpression();
				writer.WriteString(L")");
			}

			template<typename T>
			void WriteUnboxValue(WfCppConfig* config, stream::StreamWriter& writer, ITypeInfo* type, const T& writeExpression)
			{
				writer.WriteString(L"::vl::__vwsn::Unbox<");
				writer.WriteString(config->ConvertType(type));
				writer.WriteString(L">(");
				writeExpression();
				writer.WriteString(L")");
			}

			template<typename T>
			void WriteUnboxWeakValue(WfCppConfig* config, stream::StreamWriter& writer, ITypeInfo* type, const T& writeExpression)
			{
				writer.WriteString(L"::vl::__vwsn::UnboxWeak<");
				writer.WriteString(config->ConvertType(type));
				writer.WriteString(L">(");
				writeExpression();
				writer.WriteString(L")");
			}

			template<typename T>
			void ConvertValueType(WfCppConfig* config, stream::StreamWriter& writer, ITypeDescriptor* fromTd, ITypeDescriptor* toTd, const T& writeExpression)
			{
				if (fromTd == toTd)
				{
					writeExpression();
				}
				else if (fromTd == description::GetTypeDescriptor<WString>())
				{
					writer.WriteString(L"::vl::__vwsn::Parse<");
					writer.WriteString(config->ConvertType(toTd));
					writer.WriteString(L">(");
					writeExpression();
					writer.WriteString(L")");
				}
				else if (toTd == description::GetTypeDescriptor<WString>())
				{
					writer.WriteString(L"::vl::__vwsn::ToString(");
					writeExpression();
					writer.WriteString(L")");
				}
				else
				{
					writer.WriteString(L"static_cast<");
					writer.WriteString(config->ConvertType(toTd));
					writer.WriteString(L">(");
					writeExpression();
					writer.WriteString(L")");
				}
			}

			void ConvertType(WfCppConfig* config, stream::StreamWriter& writer, ITypeInfo* fromType, ITypeInfo* toType, const Func<void()>& writeExpression, bool strongCast)
			{
				if (fromType->GetTypeDescriptor()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Object)
				{
					if (strongCast)
					{
						WriteUnboxValue(config, writer, toType, writeExpression);
					}
					else
					{
						WriteUnboxWeakValue(config, writer, toType, writeExpression);
					}
					return;
				}
				else if (toType->GetTypeDescriptor()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Object)
				{
					WriteBoxValue(config, writer, fromType, writeExpression);
					return;
				}
				else
				{
					auto tdVe = description::GetTypeDescriptor<IValueEnumerable>();
					if (toType->GetTypeDescriptor() == tdVe)
					{
						if (toType->GetElementType()->GetDecorator() == ITypeInfo::Generic)
						{
							if ((fromType->GetTypeDescriptor() == tdVe && fromType->GetElementType()->GetDecorator() != ITypeInfo::Generic)
								|| fromType->GetTypeDescriptor()->CanConvertTo(description::GetTypeDescriptor<IValueReadonlyList>())
								|| fromType->GetTypeDescriptor()->CanConvertTo(description::GetTypeDescriptor<IValueReadonlyDictionary>())
								)
							{
								writer.WriteString(L"::vl::reflection::description::GetLazyList<");
								writer.WriteString(config->ConvertType(toType->GetElementType()->GetGenericArgument(0)));
								writer.WriteString(L">(");
								writeExpression();
								writer.WriteString(L")");
								return;
							}
						}
						else
						{
							if ((fromType->GetTypeDescriptor() == tdVe && fromType->GetElementType()->GetDecorator() == ITypeInfo::Generic)
								|| fromType->GetHint() == TypeInfoHint::NativeCollectionReference
								)
							{
								writer.WriteString(L"::vl::__vwsn::UnboxCollection<");
								writer.WriteString(config->ConvertType(tdVe));
								writer.WriteString(L">(");
								writeExpression();
								writer.WriteString(L")");
								return;
							}
						}
					}

					switch (fromType->GetDecorator())
					{
					case ITypeInfo::RawPtr:
						{
							switch (toType->GetDecorator())
							{
							case ITypeInfo::RawPtr:
								if (strongCast) writer.WriteString(L"::vl::__vwsn::Ensure(");
								if (fromType->GetTypeDescriptor()->CanConvertTo(toType->GetTypeDescriptor()))
								{
									writer.WriteString(L"static_cast<");
									writer.WriteString(config->ConvertType(toType->GetTypeDescriptor()));
									writer.WriteString(L"*>(");
									writeExpression();
									writer.WriteString(L")");
								}
								else
								{
									writer.WriteString(L"::vl::__vwsn::RawPtrCast<");
									writer.WriteString(config->ConvertType(toType->GetTypeDescriptor()));
									writer.WriteString(L">(");
									writeExpression();
									writer.WriteString(L")");
								}
								if (strongCast) writer.WriteString(L")");
								return;
							case ITypeInfo::SharedPtr:
								if (strongCast) writer.WriteString(L"::vl::__vwsn::Ensure(");
								if (fromType->GetTypeDescriptor()->CanConvertTo(toType->GetTypeDescriptor()))
								{
									writer.WriteString(L"::vl::Ptr<");
									writer.WriteString(config->ConvertType(toType->GetTypeDescriptor()));
									writer.WriteString(L">(");
									writeExpression();
									writer.WriteString(L")");
								}
								else
								{
									writer.WriteString(L"::vl::__vwsn::SharedPtrCast<");
									writer.WriteString(config->ConvertType(toType->GetTypeDescriptor()));
									writer.WriteString(L">(");
									writeExpression();
									writer.WriteString(L")");
								}
								if (strongCast) writer.WriteString(L")");
								return;
							default:;
							}
						}
						break;
					case ITypeInfo::SharedPtr:
						{
							switch (toType->GetDecorator())
							{
							case ITypeInfo::RawPtr:
								if (strongCast) writer.WriteString(L"::vl::__vwsn::Ensure(");
								if (fromType->GetTypeDescriptor()->CanConvertTo(toType->GetTypeDescriptor()))
								{
									writer.WriteString(L"static_cast<");
									writer.WriteString(config->ConvertType(toType->GetTypeDescriptor()));
									writer.WriteString(L"*>(");
									writeExpression();
									writer.WriteString(L".Obj())");
								}
								else
								{
									writer.WriteString(L"::vl::__vwsn::RawPtrCast<");
									writer.WriteString(config->ConvertType(toType->GetTypeDescriptor()));
									writer.WriteString(L">(");
									writeExpression();
									writer.WriteString(L".Obj())");
								}
								if (strongCast) writer.WriteString(L")");
								return;
							case ITypeInfo::SharedPtr:
								if (strongCast) writer.WriteString(L"::vl::__vwsn::Ensure(");
								if (fromType->GetTypeDescriptor()->CanConvertTo(toType->GetTypeDescriptor()))
								{
									writer.WriteString(L"::vl::Ptr<");
									writer.WriteString(config->ConvertType(toType->GetTypeDescriptor()));
									writer.WriteString(L">(");
									writeExpression();
									writer.WriteString(L")");
								}
								else
								{
									writer.WriteString(L"::vl::__vwsn::SharedPtrCast<");
									writer.WriteString(config->ConvertType(toType->GetTypeDescriptor()));
									writer.WriteString(L">(");
									writeExpression();
									writer.WriteString(L".Obj())");
								}
								if (strongCast) writer.WriteString(L")");
								return;
							default:;
							}
						}
						break;
					case ITypeInfo::Nullable:
						{
							switch (toType->GetDecorator())
							{
							case ITypeInfo::Nullable:
								if (strongCast) writer.WriteString(L"::vl::__vwsn::Ensure(");
								writer.WriteString(L"::vl::__vwsn::NullableCast<");
								writer.WriteString(config->ConvertType(toType->GetTypeDescriptor()));
								writer.WriteString(L">(");
								writeExpression();
								writer.WriteString(L")");
								if (strongCast) writer.WriteString(L")");
								return;
							case ITypeInfo::TypeDescriptor:
								ConvertValueType(config, writer, fromType->GetTypeDescriptor(), toType->GetTypeDescriptor(), [&]()
								{
									writeExpression();
									writer.WriteString(L".Value()");
								});
								return;
							default:;
							}
						}
						break;
					case ITypeInfo::TypeDescriptor:
						{
							switch (toType->GetDecorator())
							{
							case ITypeInfo::Nullable:
								writer.WriteString(config->ConvertType(toType));
								writer.WriteString(L"(");
								ConvertValueType(config, writer, fromType->GetTypeDescriptor(), toType->GetTypeDescriptor(), writeExpression);
								writer.WriteString(L")");
								return;
							case ITypeInfo::TypeDescriptor:
								ConvertValueType(config, writer, fromType->GetTypeDescriptor(), toType->GetTypeDescriptor(), writeExpression);
								return;
							default:;
							}
						}
						break;
					default:;
					}
				}
				writer.WriteString(L"/* NOT EXISTS: convert (");
				writer.WriteString(config->ConvertType(fromType));
				writer.WriteString(L") to (");
				writer.WriteString(config->ConvertType(toType));
				writer.WriteString(L") */ __vwsn_not_exists__(");
				writeExpression();
				writer.WriteString(L")");
			}

/***********************************************************************
WfGenerateExpressionVisitor
***********************************************************************/

			class WfGenerateExpressionVisitor
				: public Object
				, public WfExpression::IVisitor
			{
			public:
				WfCppConfig*				config;
				stream::StreamWriter&		writer;
				bool						useReturnValue;

				WfGenerateExpressionVisitor(WfCppConfig* _config, stream::StreamWriter& _writer, bool _useReturnValue)
					:config(_config)
					, writer(_writer)
					, useReturnValue(_useReturnValue)
				{
				}

				bool NeedConvertType(ITypeInfo* fromType, ITypeInfo*& toType)
				{
					if (toType == nullptr)
					{
						toType = fromType;
						return false;
					}
					if (IsSameType(fromType, toType))
					{
						return false;
					}
					else
					{
						return true;
					}
				}

				template<typename T>
				void WriteBoxValue(ITypeInfo* type, const T& writeExpression)
				{
					cppcodegen::WriteBoxValue(config, writer, type, writeExpression);
				}

				template<typename T>
				void WriteUnboxValue(ITypeInfo* type, const T& writeExpression)
				{
					cppcodegen::WriteUnboxValue(config, writer, type, writeExpression);
				}

				template<typename T>
				void WriteUnboxWeakValue(ITypeInfo* type, const T& writeExpression)
				{
					cppcodegen::WriteUnboxWeakValue(config, writer, type, writeExpression);
				}

				void ConvertMultipleTypes(ITypeInfo** types, vint typesLength, Func<void()> writeExpression)
				{
					if (typesLength == 1)
					{
						writeExpression();
					}
					else if (NeedConvertType(types[0], types[1]))
					{
						ConvertMultipleTypes(types + 1, typesLength - 1, [&]()
						{
							auto fromType = types[0];
							auto toType = types[1];
							ConvertType(config, writer, fromType, toType, writeExpression, false);
						});
					}
					else
					{
						return ConvertMultipleTypes(types + 1, typesLength - 1, writeExpression);
					}
				}

				void Call(Ptr<WfExpression> node, ITypeInfo* expectedType = nullptr, bool useReturnValue = true)
				{
					GenerateExpression(config, writer, node, expectedType, useReturnValue);
				}

				Ptr<WfCppConfig::ClosureInfo> GetClosureInfo(WfExpression* node, Ptr<WfLexicalSymbol> testCtorArgumentSymbol = nullptr)
				{
					Ptr<WfCppConfig::ClosureInfo> closureInfo;
					auto scope = config->manager->nodeScopes[node].Obj();

					if (dynamic_cast<WfOrderedLambdaExpression*>(node))
					{
						scope = scope->parentScope.Obj();
					}

					while (scope)
					{
						if (scope->functionConfig && scope->functionConfig->lambda)
						{
							auto source = scope->ownerNodeSource ? scope->ownerNodeSource : scope->ownerNode.Obj();
							if (auto ordered = dynamic_cast<WfOrderedLambdaExpression*>(source))
							{
								closureInfo = config->closureInfos[ordered];
								break;
							}
							else if (auto funcExpr = dynamic_cast<WfFunctionExpression*>(source))
							{
								closureInfo = config->closureInfos[funcExpr];
								break;
							}
							else if (auto classExpr = dynamic_cast<WfNewInterfaceExpression*>(source))
							{
								closureInfo = config->closureInfos[classExpr];
								break;
							}
						}
						else if (auto classExpr = scope->ownerNode.Cast<WfNewInterfaceExpression>())
						{
							auto info = config->closureInfos[classExpr.Obj()];
							if (info->ctorArgumentSymbols.Values().Contains(testCtorArgumentSymbol.Obj()))
							{
								closureInfo = info;
								break;
							}
						}
						scope = scope->parentScope.Obj();
					}

					return closureInfo;
				}

				void WriteNotExists(ITypeDescriptor* typeDescriptor)
				{
					writer.WriteString(L"/* NOT EXISTS: type(");
					writer.WriteString(typeDescriptor->GetTypeName());
					writer.WriteString(L") */ __vwsn_not_exists__");
				}

				void WriteNotExists(IMethodInfo* methodInfo)
				{
					writer.WriteString(L"/* NOT EXISTS: method(");
					writer.WriteString(methodInfo->GetName());
					writer.WriteString(L") of type (");
					writer.WriteString(methodInfo->GetOwnerTypeDescriptor()->GetTypeName());
					writer.WriteString(L") */ __vwsn_not_exists__");
				}

				void WriteNotExists(IPropertyInfo* propertyInfo)
				{
					writer.WriteString(L"/* NOT EXISTS: property(");
					writer.WriteString(propertyInfo->GetName());
					writer.WriteString(L") of type (");
					writer.WriteString(propertyInfo->GetOwnerTypeDescriptor()->GetTypeName());
					writer.WriteString(L") */ __vwsn_not_exists__");
				}

				void WriteNotExists(IEventInfo* eventInfo)
				{
					writer.WriteString(L"/* NOT EXISTS: event(");
					writer.WriteString(eventInfo->GetName());
					writer.WriteString(L") of type (");
					writer.WriteString(eventInfo->GetOwnerTypeDescriptor()->GetTypeName());
					writer.WriteString(L") */ __vwsn_not_exists__");
				}

				template<typename T>
				void WriteTemplate(const WString& templateValue, const T& callback)
				{
					List<Ptr<RegexMatch>> matches;
					config->regexTemplate.Cut(templateValue, false, matches);
					FOREACH(Ptr<RegexMatch>, match, matches)
					{
						WString item = match->Result().Value();
						if (match->Success())
						{
							if (!callback(item))
							{
								writer.WriteString(L"/* NOT EXISTS: ");
								writer.WriteString(item);
								writer.WriteString(L" */ __vwsn_not_exists__");
							}
						}
						else
						{
							writer.WriteString(item);
						}
					}
				}

				void VisitThisExpression(WfExpression* node, ITypeDescriptor* td)
				{
					if (auto closureInfo = GetClosureInfo(node))
					{
						FOREACH_INDEXER(ITypeDescriptor*, thisType, index, closureInfo->thisTypes)
						{
							if (thisType->CanConvertTo(td))
							{
								writer.WriteString(L"__vwsnthis_");
								writer.WriteString(itow(index));
								return;
							}
						}
					}
					writer.WriteString(L"this");
				}

				void VisitSymbol(WfExpression* node, Ptr<WfLexicalSymbol> symbol, bool forLambdaArgument)
				{
					if (auto varDecl = symbol->creatorNode.Cast<WfVariableDeclaration>())
					{
						auto ownerNode = symbol->ownerScope->ownerNode;
						if (ownerNode.Cast<WfNamespaceDeclaration>() || ownerNode.Cast<WfModule>())
						{
							writer.WriteString(L"GLOBAL_NAME ");
							writer.WriteString(config->ConvertName(symbol->name));
							return;
						}
						else if(auto closureInfo = GetClosureInfo(node, (forLambdaArgument ? nullptr : symbol)))
						{
							if (closureInfo->symbols.Values().Contains(symbol.Obj()))
							{
								writer.WriteString(L"this->");
								writer.WriteString(config->ConvertName(symbol->name));
								return;
							}
							else if (closureInfo->ctorArgumentSymbols.Values().Contains(symbol.Obj()))
							{
								writer.WriteString(L"__vwsnctor_");
								writer.WriteString(config->ConvertName(symbol->name));
								return;
							}
						}
						writer.WriteString(config->ConvertName(symbol->name));
						return;
					}
					else if (auto funcDecl = symbol->creatorNode.Cast<WfFunctionDeclaration>())
					{
						auto ownerNode = symbol->ownerScope->ownerNode;
						if (ownerNode.Cast<WfNamespaceDeclaration>() || ownerNode.Cast<WfModule>())
						{
							writer.WriteString(config->ConvertType(symbol->typeInfo.Obj()));
							writer.WriteString(L"(GLOBAL_OBJ, &GLOBAL_SYMBOL ");
							writer.WriteString(config->ConvertName(symbol->name));
							writer.WriteString(L")");
							return;
						}
						else if (auto classExpr = ownerNode.Cast<WfNewInterfaceExpression>())
						{
							writer.WriteString(config->ConvertType(symbol->typeInfo.Obj()));
							writer.WriteString(L"(this, &");
							writer.WriteString(config->classExprs[classExpr.Obj()]);
							writer.WriteString(L"::");
							writer.WriteString(config->ConvertName(symbol->name));
							writer.WriteString(L")");
							return;
						}
						else if (symbol->ownerScope->functionConfig && symbol->ownerScope->functionConfig->lambda && symbol->name == funcDecl->name.value)
						{
							auto scope = config->manager->nodeScopes[funcDecl.Obj()];
							auto closureInfo = config->closureInfos[dynamic_cast<WfFunctionExpression*>(scope->ownerNodeSource)];

							writer.WriteString(L"LAMBDA(::");
							writer.WriteString(config->assemblyNamespace);
							writer.WriteString(L"::");
							writer.WriteString(closureInfo->lambdaClassName);
							writer.WriteString(L"(");

							FOREACH_INDEXER(WString, symbolName, index, closureInfo->symbols.Keys())
							{
								if (index > 0)
								{
									writer.WriteString(L", ");
								}
								writer.WriteString(config->ConvertName(symbol->name));
							}

							FOREACH_INDEXER(ITypeDescriptor*, thisType, index, closureInfo->thisTypes)
							{
								if (index > 0 || closureInfo->symbols.Count() > 0)
								{
									writer.WriteString(L", ");
								}
								writer.WriteString(L" __vwsnctorthis_" + itow(index));
							}

							writer.WriteString(L"))");
							return;
						}
					}
					writer.WriteString(config->ConvertName(symbol->name));
				}

				enum class CommaPosition
				{
					Left,
					Right,
					No,
				};

				bool IsCppRefGenericType(ITypeInfo* type)
				{
					switch (type->GetHint())
					{
					case TypeInfoHint::Array:
					case TypeInfoHint::List:
					case TypeInfoHint::SortedList:
					case TypeInfoHint::ObservableList:
					case TypeInfoHint::Dictionary:
						return true;
					default:
						return false;
					}
				}

				template<typename TReturnValue>
				void WriteReturnValue(ITypeInfo* type, const TReturnValue& returnValueCallback, bool castReturnValue)
				{
					if (castReturnValue)
					{
						if (IsCppRefGenericType(type) || type->GetHint() == TypeInfoHint::NativeCollectionReference)
						{
							writer.WriteString(L"::vl::__vwsn::UnboxCollection<");
							writer.WriteString(config->ConvertType(type->GetTypeDescriptor()));
							writer.WriteString(L">(");
							returnValueCallback();
							writer.WriteString(L")");
							return;
						}
					}
					returnValueCallback();
				}

				template<typename TType, typename TInvoke, typename TArgument, typename TInfo>
				void WriteInvokeTemplate(vint count, ITypeInfo* returnType, const TType& typeCallback, const TInvoke& invokeCallback, const TArgument& argumentCallback, TInfo* info, bool castReturnValue)
				{
					if (Range<vint>(0, count).Any([&](vint index) {return IsCppRefGenericType(typeCallback(index)); }))
					{
						writer.WriteString(L"[&]()->decltype(auto){");
						for (vint i = 0; i < count; i++)
						{
							auto type = typeCallback(i);
							if (IsCppRefGenericType(type))
							{
								writer.WriteString(L" auto __vwsn_temp_x");
								writer.WriteString(itow(i));
								writer.WriteString(L" = ::vl::__vwsn::Box(");
								argumentCallback(info, i);
								writer.WriteString(L"); ");

								switch (type->GetHint())
								{
								case TypeInfoHint::Array:
									writer.WriteString(L"::vl::collections::Array<");
									break;
								case TypeInfoHint::List:
									writer.WriteString(L"::vl::collections::List<");
									break;
								case TypeInfoHint::SortedList:
									writer.WriteString(L"::vl::collections::SortedList<");
									break;
								case TypeInfoHint::ObservableList:
									writer.WriteString(L"::vl::collections::ObservableList<");
									break;
								case TypeInfoHint::Dictionary:
									writer.WriteString(L"::vl::collections::Dictionary<");
									break;
								default:;
								}

								vint count = type->GetElementType()->GetGenericArgumentCount();
								for (vint i = 0; i < count; i++)
								{
									if (i > 0) writer.WriteString(L", ");
									writer.WriteString(config->ConvertType(type->GetElementType()->GetGenericArgument(i)));
								}
								writer.WriteString(L"> __vwsn_temp_");
								writer.WriteString(itow(i));
								writer.WriteString(L"; ::vl::reflection::description::UnboxParameter(__vwsn_temp_x");
								writer.WriteString(itow(i));
								writer.WriteString(L", __vwsn_temp_");
								writer.WriteString(itow(i));
								writer.WriteString(L");");
							}
							else
							{
								writer.WriteString(L" auto __vwsn_temp_");
								writer.WriteString(itow(i));
								writer.WriteString(L" = ");
								argumentCallback(info, i);
								writer.WriteString(L";");
							}
						}
						writer.WriteString(L" return ");
						WriteReturnValue(returnType, [&]() { invokeCallback(true); }, castReturnValue);
						writer.WriteString(L"; }()");
					}
					else
					{
						WriteReturnValue(returnType, [&]() { invokeCallback(false); }, castReturnValue);
					}
				}

				template<typename TThis, typename TArgument>
				void WriteMethodTemplate(const WString& templateValue, IMethodInfo* methodInfo, const TThis& thisCallback, const TArgument& argumentCallback, bool castReturnValue)
				{
					WriteInvokeTemplate(methodInfo->GetParameterCount(), methodInfo->GetReturn(), [&](vint index) { return methodInfo->GetParameter(index)->GetType(); },
						[&](bool useTemporaryArgument)
						{
							WriteTemplate(templateValue, [&](const WString& item)
							{
								auto cp = CommaPosition::No;
								vint count = methodInfo->GetParameterCount();

								if (item == L"$Type")
								{
									writer.WriteString(config->ConvertType(methodInfo->GetOwnerTypeDescriptor()));
									return true;
								}
								else if (item == L"$Func")
								{
									writer.WriteString(config->ConvertFunctionType(methodInfo));
									return true;
								}
								else if (item == L"$Name")
								{
									writer.WriteString(config->ConvertName(methodInfo->GetName()));
									return true;
								}
								else if (item == L"$This")
								{
									if (!methodInfo->IsStatic())
									{
										return thisCallback(methodInfo);
									}
								}
								else if (item == L"$Arguments")
								{
									if (count == 0)
									{
										return true;
									}
								}
								else if (item == L", $Arguments")
								{
									if (count == 0)
									{
										return true;
									}
									cp = CommaPosition::Left;
								}
								else if (item == L"$Arguments, ")
								{
									if (count == 0)
									{
										return true;
									}
									cp = CommaPosition::Right;
								}
								else
								{
									return false;
								}

								if (count > 0)
								{
									if (cp == CommaPosition::Left) writer.WriteString(L", ");
									for (vint i = 0; i < count; i++)
									{
										if (i > 0) writer.WriteString(L", ");
										if (useTemporaryArgument)
										{
											writer.WriteString(L"__vwsn_temp_");
											writer.WriteString(itow(i));
										}
										else
										{
											argumentCallback(methodInfo, i);
										}
									}
									if (cp == CommaPosition::Right) writer.WriteString(L", ");
								}
								return true;
							});
						}, argumentCallback, methodInfo, castReturnValue);
				}

				template<typename TThis>
				void WritePropertyTemplate(const WString& templateValue, IPropertyInfo* propertyInfo, const TThis& thisCallback, bool castReturnValue)
				{
					WriteReturnValue(propertyInfo->GetReturn(),
						[&]()
						{
							WriteTemplate(templateValue, [&](const WString& item)
							{
								if (item == L"$Type")
								{
									writer.WriteString(config->ConvertType(propertyInfo->GetOwnerTypeDescriptor()));
									return true;
								}
								else if (item == L"$Name")
								{
									writer.WriteString(config->ConvertName(propertyInfo->GetName()));
									return true;
								}
								else if (item == L"$This")
								{
									return thisCallback(propertyInfo);
								}
								return false;
							});
						}, castReturnValue);
				}

				template<typename TThis, typename THandler, typename TArgument>
				void WriteEventTemplate(const WString& templateValue, IEventInfo* eventInfo, const TThis& thisCallback, const THandler& handlerCallback, const TArgument& argumentCallback)
				{
					auto handlerType = eventInfo->GetHandlerType()->GetElementType();
					WriteInvokeTemplate(handlerType->GetGenericArgumentCount() - 1, handlerType->GetGenericArgument(0), [&](vint index) { return handlerType->GetGenericArgument(index + 1); },
						[&](bool useTemporaryArgument)
						{
							WriteTemplate(templateValue, [&](const WString& item)
							{
								auto cp = CommaPosition::No;
								vint count = eventInfo->GetHandlerType()->GetElementType()->GetGenericArgumentCount() - 1;

								if (item == L"$Name")
								{
									writer.WriteString(config->ConvertName(eventInfo->GetName()));
									return true;
								}
								else if (item == L"$This")
								{
									return thisCallback(eventInfo);
								}
								else if (item == L"$Handler")
								{
									return handlerCallback(eventInfo);
								}
								else if (item == L"$Arguments")
								{
									if (count == 0)
									{
										return true;
									}
								}
								else if (item == L", $Arguments")
								{
									if (count == 0)
									{
										return true;
									}
									cp = CommaPosition::Left;
								}
								else if (item == L"$Arguments, ")
								{
									if (count == 0)
									{
										return true;
									}
									cp = CommaPosition::Right;
								}
								else
								{
									return false;
								}

								if (count > 0)
								{
									if (cp == CommaPosition::Left) writer.WriteString(L", ");
									for (vint i = 0; i < count; i++)
									{
										if (i > 0) writer.WriteString(L", ");
										if (useTemporaryArgument)
										{
											writer.WriteString(L"__vwsn_temp_");
											writer.WriteString(itow(i));
										}
										else
										{
											argumentCallback(eventInfo, i);
										}
									}
									if (cp == CommaPosition::Right) writer.WriteString(L", ");
								}
								return true;
							});
						}, argumentCallback, eventInfo, false);
				}

				template<typename TMethodThis, typename TPropertyThis>
				bool WriteReferenceTemplate(ResolveExpressionResult& result, const TMethodThis& methodThis, const TPropertyThis& propertyThis, bool castReturnValue)
				{
					if (result.methodInfo)
					{
						if (CppExists(result.methodInfo))
						{
							auto methodInfo = result.methodInfo;
							WriteMethodTemplate(CppGetClosureTemplate(methodInfo), methodInfo, methodThis, [&](IMethodInfo*, vint){}, castReturnValue);
						}
						else
						{
							WriteNotExists(result.methodInfo);
						}
						return true;
					}
					else if (result.propertyInfo)
					{
						if (CppExists(result.propertyInfo))
						{
							if (result.propertyInfo->GetCpp() == nullptr && result.propertyInfo->GetGetter() != nullptr)
							{
								auto methodInfo = result.propertyInfo->GetGetter();
								WriteMethodTemplate(CppGetInvokeTemplate(methodInfo), methodInfo, methodThis, [&](IMethodInfo*, vint){}, castReturnValue);
							}
							else
							{
								auto templateValue = CppGetReferenceTemplate(result.propertyInfo);
								auto propertyInfo = result.propertyInfo;
								WritePropertyTemplate(CppGetReferenceTemplate(propertyInfo), propertyInfo, propertyThis, castReturnValue);
							}
						}
						else
						{
							WriteNotExists(result.propertyInfo);
						}
						return true;
					}
					else
					{
						return false;
					}
				}

				void VisitReferenceExpression(WfExpression* node, const WString& name)
				{
					auto result = config->manager->expressionResolvings[node];
					bool success = WriteReferenceTemplate(result,
						[&](IMethodInfo* methodInfo)
						{
							VisitThisExpression(node, methodInfo->GetOwnerTypeDescriptor());
							return true;
						},
						[&](IPropertyInfo* propertyInfo)
						{
							auto isRef = (propertyInfo->GetOwnerTypeDescriptor()->GetTypeDescriptorFlags() & TypeDescriptorFlags::ReferenceType) != TypeDescriptorFlags::Undefined;
							VisitThisExpression(node, propertyInfo->GetOwnerTypeDescriptor());
							return true;
						}, useReturnValue);

					if (!success)
					{
						if (result.symbol)
						{
							VisitSymbol(node, result.symbol, false);
						}
						else
						{
							if ((result.type->GetTypeDescriptor()->GetTypeDescriptorFlags() & TypeDescriptorFlags::EnumType) != TypeDescriptorFlags::Undefined)
							{
								auto enumType = result.type->GetTypeDescriptor()->GetEnumType();
								Ptr<ITypeInfo> enumValueType;
								if (result.type->GetDecorator() == ITypeInfo::TypeDescriptor)
								{
									enumValueType = result.type;
								}
								else
								{
									enumValueType = MakePtr<TypeDescriptorTypeInfo>(result.type->GetTypeDescriptor(), TypeInfoHint::Normal);
								}
								vint index = enumType->IndexOfItem(name);
								if (index != -1)
								{
									ITypeInfo* types[] = { enumValueType.Obj(),result.type.Obj() };
									ConvertMultipleTypes(types, (sizeof(types) / sizeof(*types)), [=]()
									{
										writer.WriteString(config->ConvertType(result.type->GetTypeDescriptor()));
										writer.WriteString(L"::");
										writer.WriteString(name);
									});
									return;
								}
							}
							CHECK_FAIL(L"WfGenerateExpressionVisitor::VisitReferenceExpression(WfExpression*, const WString&)#Internal error, cannot find any record of this expression.");
						}
					}
				}
				
				void WriteClosureArguments(Ptr<WfCppConfig::ClosureInfo> closureInfo, WfExpression* node)
				{
					vint index = 0;

					FOREACH(Ptr<WfLexicalSymbol>, symbol, From(closureInfo->symbols.Values()).Union(closureInfo->ctorArgumentSymbols.Values()))
					{
						if (index++ > 0)
						{
							writer.WriteString(L", ");
						}
						VisitSymbol(node, symbol, true);
					}

					FOREACH(ITypeDescriptor*, thisType, closureInfo->thisTypes)
					{
						if (index++ > 0)
						{
							writer.WriteString(L", ");
						}
						VisitThisExpression(node, thisType);
					}
				}

				void Visit(WfThisExpression* node)override
				{
					auto result = config->manager->expressionResolvings[node];
					VisitThisExpression(node, result.type->GetTypeDescriptor());
				}

				void Visit(WfTopQualifiedExpression* node)override
				{
					VisitReferenceExpression(node, node->name.value);
				}

				void Visit(WfReferenceExpression* node)override
				{
					VisitReferenceExpression(node, node->name.value);
				}

				void Visit(WfOrderedNameExpression* node)override
				{
					VisitReferenceExpression(node, node->name.value);
				}

				void Visit(WfOrderedLambdaExpression* node)override
				{
					writer.WriteString(L"LAMBDA(::");
					writer.WriteString(config->assemblyNamespace);
					writer.WriteString(L"::");
					writer.WriteString(config->lambdaExprs[node]);
					writer.WriteString(L"(");

					auto closureInfo = config->closureInfos[node];
					WriteClosureArguments(closureInfo, node);

					writer.WriteString(L"))");
				}

				void Visit(WfMemberExpression* node)override
				{
					auto result = config->manager->expressionResolvings[node];
					auto parentResult = config->manager->expressionResolvings[node->parent.Obj()];
					WriteReferenceTemplate(result,
						[&](IMethodInfo* methodInfo)
						{
							writer.WriteString(L"::vl::__vwsn::This(");
							Call(node->parent);
							if (parentResult.type->GetDecorator() == ITypeInfo::SharedPtr)
							{
								writer.WriteString(L".Obj()");
							}
							writer.WriteString(L")");
							return true;
						},
						[&](IPropertyInfo* propertyInfo)
						{
							auto isRef = (propertyInfo->GetOwnerTypeDescriptor()->GetTypeDescriptorFlags() & TypeDescriptorFlags::ReferenceType) != TypeDescriptorFlags::Undefined;
							if (isRef) writer.WriteString(L"::vl::__vwsn::This(");
							Call(node->parent);
							if (parentResult.type->GetDecorator() == ITypeInfo::SharedPtr)
							{
								writer.WriteString(L".Obj()");
							}
							else if (parentResult.type->GetDecorator() == ITypeInfo::Nullable)
							{
								writer.WriteString(L".Value()");
							}
							if (isRef) writer.WriteString(L")");
							return true;
						}, useReturnValue);
				}

				void Visit(WfChildExpression* node)override
				{
					VisitReferenceExpression(node, node->name.value);
				}

				void Visit(WfLiteralExpression* node)override
				{
					switch (node->value)
					{
					case WfLiteralValue::Null:
						{
							auto result = config->manager->expressionResolvings[node];
							switch (result.type->GetDecorator())
							{
							case ITypeInfo::Nullable:
							case ITypeInfo::SharedPtr:
								writer.WriteString(config->ConvertType(result.type.Obj()));
								writer.WriteString(L"()");
								break;
							case ITypeInfo::TypeDescriptor:
								writer.WriteString(L"::vl::reflection::description::Value()");
								break;
							default:
								writer.WriteString(L"static_cast<");
								writer.WriteString(config->ConvertType(result.type.Obj()));
								writer.WriteString(L">(nullptr)");
							}
						}
						break;
					case WfLiteralValue::True:
						writer.WriteString(L"true");
						break;
					case WfLiteralValue::False:
						writer.WriteString(L"false");
						break;
					}
				}

				void Visit(WfFloatingExpression* node)override
				{
					auto result = config->manager->expressionResolvings[node];
					auto td = result.type->GetTypeDescriptor();

					writer.WriteString(L"static_cast<");
					writer.WriteString(config->ConvertType(td));
					writer.WriteString(L">(");
					if (td == description::GetTypeDescriptor<float>())
					{
						writer.WriteString(node->value.value + L"f");
					}
					else
					{
						writer.WriteString(node->value.value);
					}
					writer.WriteString(L")");
				}

				void Visit(WfIntegerExpression* node)override
				{
					auto result = config->manager->expressionResolvings[node];
					auto td = result.type->GetTypeDescriptor();

					writer.WriteString(L"static_cast<");
					writer.WriteString(config->ConvertType(td));
					writer.WriteString(L">(");
					if (td == description::GetTypeDescriptor<vuint32_t>())
					{
						writer.WriteString(node->value.value + L"U");
					}
					else if (td == description::GetTypeDescriptor<vint64_t>())
					{
						writer.WriteString(node->value.value + L"L");
					}
					else if (td == description::GetTypeDescriptor<vuint64_t>())
					{
						writer.WriteString(node->value.value + L"UL");
					}
					else
					{
						writer.WriteString(node->value.value);
					}
					writer.WriteString(L")");
				}

				void Visit(WfStringExpression* node)override
				{
					writer.WriteString(L"::vl::WString::Unmanaged(L\"");
					for (vint i = 0; i < node->value.value.Length(); i++)
					{
						auto c = node->value.value[i];
						switch (c)
						{
						case L'\'': writer.WriteString(L"\\\'"); break;
						case L'\"': writer.WriteString(L"\\\""); break;
						case L'\r': writer.WriteString(L"\\r"); break;
						case L'\n': writer.WriteString(L"\\n"); break;
						case L'\t': writer.WriteString(L"\\t"); break;
						default: writer.WriteChar(c);
						}
					}
					writer.WriteString(L"\")");
				}

				void Visit(WfUnaryExpression* node)override
				{
					auto result = config->manager->expressionResolvings[node];
					switch (node->op)
					{
					case WfUnaryOperator::Positive:
						writer.WriteString(L"(+ ");
						Call(node->operand, result.type.Obj());
						writer.WriteString(L")");
						break;
					case WfUnaryOperator::Negative:
						writer.WriteString(L"(- ");
						Call(node->operand, result.type.Obj());
						writer.WriteString(L")");
						break;
					case WfUnaryOperator::Not:
						{
							if (result.type->GetTypeDescriptor() == description::GetTypeDescriptor<bool>())
							{
								writer.WriteString(L"(! ");
							}
							else
							{
								if (BinaryNeedConvert(result.type->GetTypeDescriptor()))
								{
									writer.WriteString(L"static_cast<");
									writer.WriteString(config->ConvertType(result.type->GetTypeDescriptor()));
									writer.WriteString(L">");
								}
								writer.WriteString(L"(~ ");
							}
							Call(node->operand, result.type.Obj());
							writer.WriteString(L")");
						}
						break;
					}
				}

				void VisitBinaryExpression(WfBinaryExpression* node, const wchar_t* op, ITypeDescriptor* resultType, ITypeInfo* operandType)
				{
					if (resultType)
					{
						writer.WriteString(L"static_cast<");
						writer.WriteString(config->ConvertType(resultType));
						writer.WriteString(L">");
					}
					writer.WriteString(L"(");
					Call(node->first, operandType);
					writer.WriteString(L" ");
					writer.WriteString(op);
					writer.WriteString(L" ");
					Call(node->second, operandType);
					writer.WriteString(L")");
				}

				bool BinaryNeedConvert(ITypeDescriptor* type)
				{
					return type == description::GetTypeDescriptor<vint8_t>()
						|| type == description::GetTypeDescriptor<vint16_t>()
						|| type == description::GetTypeDescriptor<vuint8_t>()
						|| type == description::GetTypeDescriptor<vuint16_t>()
						;
				}

				void Visit(WfBinaryExpression* node)override
				{
					if (node->op == WfBinaryOperator::Assign)
					{
						auto result = config->manager->expressionResolvings[node->first.Obj()];
						if (result.propertyInfo)
						{
							auto propInfo = result.propertyInfo;
							auto member = node->first.Cast<WfMemberExpression>();

							if (CppExists(propInfo))
							{
								if (propInfo->GetCpp() == nullptr && propInfo->GetSetter() != nullptr)
								{
									WriteMethodTemplate(CppGetInvokeTemplate(propInfo->GetSetter()), propInfo->GetSetter(),
										[&](IMethodInfo*)
										{
											if (member)
											{
												writer.WriteString(L"::vl::__vwsn::This(");
												Call(member->parent);
												auto parentResult = config->manager->expressionResolvings[member->parent.Obj()];
												if (parentResult.type->GetDecorator() == ITypeInfo::SharedPtr)
												{
													writer.WriteString(L".Obj()");
												}
												writer.WriteString(L")");
											}
											else
											{
												VisitThisExpression(node->first.Obj(), propInfo->GetOwnerTypeDescriptor());
											}
											return true;
										},
										[&](IMethodInfo*, vint index)
										{
											Call(node->second);
										}, useReturnValue);
								}
								else
								{
									writer.WriteString(L"(");
									WritePropertyTemplate(CppGetReferenceTemplate(propInfo), propInfo,
										[&](IPropertyInfo*)
										{
											if (member)
											{
												writer.WriteString(L"::vl::__vwsn::This(");
												Call(member->parent);
												auto parentResult = config->manager->expressionResolvings[member->parent.Obj()];
												if (parentResult.type->GetDecorator() == ITypeInfo::SharedPtr)
												{
													writer.WriteString(L".Obj()");
												}
												writer.WriteString(L")");
											}
											else
											{
												VisitThisExpression(node->first.Obj(), propInfo->GetOwnerTypeDescriptor());
											}
											return true;
										}, true);
									writer.WriteString(L" = ");
									Call(node->second, propInfo->GetReturn());
									writer.WriteString(L")");
								}
							}
							else
							{
								WriteNotExists(propInfo);
							}
						}
						else if (auto binary = node->first.Cast<WfBinaryExpression>())
						{
							auto containerType = config->manager->expressionResolvings[binary->first.Obj()].type.Obj();
							if (IsCppRefGenericType(containerType))
							{
								Call(binary->first, nullptr, false);
								writer.WriteString(L".Set(");
								Call(binary->second);
								writer.WriteString(L", ");
								Call(node->second);
								writer.WriteString(L")");
							}
							else
							{
								auto keyType = config->manager->expressionResolvings[binary->second.Obj()].type.Obj();
								auto valueType = config->manager->expressionResolvings[node->second.Obj()].type.Obj();
								writer.WriteString(L"::vl::__vwsn::This(");
								Call(binary->first);
								writer.WriteString(L".Obj())->Set(");
								if (containerType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueDictionary>())
								{
									WriteBoxValue(keyType, [&]() {Call(binary->second); });
								}
								else
								{
									Call(binary->second);
								}
								writer.WriteString(L", ");
								WriteBoxValue(valueType, [&]() {Call(node->second); });
								writer.WriteString(L")");
							}
						}
						else
						{
							VisitBinaryExpression(node, L"=", nullptr, nullptr);
						}
					}
					else if (node->op == WfBinaryOperator::Index)
					{
						auto containerType = config->manager->expressionResolvings[node->first.Obj()].type.Obj();
						if (IsCppRefGenericType(containerType))
						{
							Call(node->first, nullptr, false);
							writer.WriteString(L"[");
							Call(node->second);
							writer.WriteString(L"]");
						}
						else
						{
							auto keyType = config->manager->expressionResolvings[node->second.Obj()].type.Obj();
							auto valueType = config->manager->expressionResolvings[node].type.Obj();
							WriteUnboxValue(valueType, [&]()
							{
								writer.WriteString(L"::vl::__vwsn::This(");
								Call(node->first);
								writer.WriteString(L".Obj())->Get(");
								if (containerType->GetTypeDescriptor()->CanConvertTo(description::GetTypeDescriptor<IValueReadonlyDictionary>()))
								{
									WriteBoxValue(keyType, [&]() {Call(node->second); });
								}
								else
								{
									Call(node->second);
								}
								writer.WriteString(L")");
							});
						}
					}
					else if (node->op == WfBinaryOperator::FlagAnd)
					{
						auto type = config->manager->expressionResolvings[node].type;
						if (type->GetTypeDescriptor() == description::GetTypeDescriptor<WString>())
						{
							VisitBinaryExpression(node, L"+", nullptr, type.Obj());
						}
						else
						{
							VisitBinaryExpression(node, L"&", nullptr, nullptr);
						}
					}
					else if (node->op == WfBinaryOperator::FlagOr)
					{
						VisitBinaryExpression(node, L"|", nullptr, nullptr);
					}
					else if (node->op == WfBinaryOperator::FailedThen)
					{
						auto firstResult = config->manager->expressionResolvings[node->first.Obj()];
						auto secondResult = config->manager->expressionResolvings[node->second.Obj()];
						auto mergedType = GetMergedType(firstResult.type, secondResult.type);

						writer.WriteString(L"[&](){ try{ return ");
						Call(node->first, mergedType.Obj());
						writer.WriteString(L"; } catch(...){ return ");
						Call(node->second, mergedType.Obj());
						writer.WriteString(L"; } }()");
					}
					else
					{
						auto result = config->manager->expressionResolvings[node];
						auto firstResult = config->manager->expressionResolvings[node->first.Obj()];
						auto secondResult = config->manager->expressionResolvings[node->second.Obj()];
						auto mergedType = GetMergedType(firstResult.type, secondResult.type);

						switch (node->op)
						{
						case WfBinaryOperator::Exp:
							writer.WriteString(L"static_cast<");
							writer.WriteString(config->ConvertType(result.type->GetTypeDescriptor()));
							writer.WriteString(L">(pow(static_cast<double>(");
							Call(node->first);
							writer.WriteString(L"), static_cast<double>(");
							Call(node->second);
							writer.WriteString(L")))");
							return;
						case WfBinaryOperator::Add:
						case WfBinaryOperator::Sub:
						case WfBinaryOperator::Mul:
						case WfBinaryOperator::Div:
						case WfBinaryOperator::Mod:
						case WfBinaryOperator::Shl:
						case WfBinaryOperator::Shr:
							{
								const wchar_t* op = nullptr;
								switch (node->op)
								{
								case WfBinaryOperator::Add: op = L"+"; break;
								case WfBinaryOperator::Sub: op = L"-"; break;
								case WfBinaryOperator::Mul: op = L"*"; break;
								case WfBinaryOperator::Div: op = L"/"; break;
								case WfBinaryOperator::Mod: op = L"%"; break;
								case WfBinaryOperator::Shl: op = L"<<"; break;
								case WfBinaryOperator::Shr: op = L">>"; break;
								default:;
								}
								VisitBinaryExpression(node, op, (BinaryNeedConvert(result.type->GetTypeDescriptor()) ? result.type->GetTypeDescriptor() : nullptr), result.type.Obj());
							}
							return;
						case WfBinaryOperator::EQ:
						case WfBinaryOperator::NE:
							{
								if (firstResult.type->GetDecorator() == ITypeInfo::RawPtr || firstResult.type->GetDecorator() == ITypeInfo::SharedPtr)
								{
									auto td = mergedType->GetTypeDescriptor();
									auto tdFirst = firstResult.type->GetTypeDescriptor();
									auto tdSecond = secondResult.type->GetTypeDescriptor();
									auto dFirst = firstResult.type->GetDecorator();
									auto dSecond = secondResult.type->GetDecorator();

									writer.WriteString(L"(");

									if (td != tdFirst)
									{
										writer.WriteString(L"static_cast<");
										writer.WriteString(config->ConvertType(tdFirst));
										writer.WriteString(L"*>(");
									}
									Call(node->first);
									if (dFirst == ITypeInfo::SharedPtr)
									{
										writer.WriteString(L".Obj()");
									}
									if (td != tdFirst)
									{
										writer.WriteString(L")");
									}

									if (node->op == WfBinaryOperator::EQ)
									{
										writer.WriteString(L" == ");
									}
									else
									{
										writer.WriteString(L" != ");
									}

									if (td != tdSecond)
									{
										writer.WriteString(L"static_cast<");
										writer.WriteString(config->ConvertType(tdSecond));
										writer.WriteString(L"*>(");
									}
									Call(node->second);
									if (dSecond == ITypeInfo::SharedPtr)
									{
										writer.WriteString(L".Obj()");
									}
									if (td != tdSecond)
									{
										writer.WriteString(L")");
									}

									writer.WriteString(L")");
									return;
								}
							}
						case WfBinaryOperator::LT:
						case WfBinaryOperator::GT:
						case WfBinaryOperator::LE:
						case WfBinaryOperator::GE:
							{
								const wchar_t* op = nullptr;
								switch (node->op)
								{
								case WfBinaryOperator::LT: op = L"<"; break;
								case WfBinaryOperator::GT: op = L">"; break;
								case WfBinaryOperator::LE: op = L"<="; break;
								case WfBinaryOperator::GE: op = L">="; break;
								case WfBinaryOperator::EQ: op = L"=="; break;
								case WfBinaryOperator::NE: op = L"!="; break;
								default:;
								}
								VisitBinaryExpression(node, op, nullptr, mergedType.Obj());
							}
							return;
						case WfBinaryOperator::Xor:
						case WfBinaryOperator::And:
						case WfBinaryOperator::Or:
							{
								auto td = result.type->GetTypeDescriptor();
								const wchar_t* op = nullptr;
								if (td == description::GetTypeDescriptor<bool>())
								{
									switch (node->op)
									{
									case WfBinaryOperator::Xor: op = L"^"; break;
									case WfBinaryOperator::And: op = L"&&"; break;
									case WfBinaryOperator::Or: op = L"||"; break;
									default:;
									}
									if (node->op != WfBinaryOperator::Xor)
									{
										td = nullptr;
									}
								}
								else
								{
									switch (node->op)
									{
									case WfBinaryOperator::Xor: op = L"^"; break;
									case WfBinaryOperator::And: op = L"&"; break;
									case WfBinaryOperator::Or: op = L"|"; break;
									default:;
									}
									if (!BinaryNeedConvert(td))
									{
										td = nullptr;
									}
								}
								VisitBinaryExpression(node, op, td, result.type.Obj());
							}
							return;
						default:;
						}
					}
				}

				void Visit(WfLetExpression* node)override
				{
					auto scope = config->manager->nodeScopes[node];
					writer.WriteString(L"[&](");
					FOREACH_INDEXER(Ptr<WfLetVariable>, letVar, index, node->variables)
					{
						if (index > 0)
						{
							writer.WriteString(L", ");
						}
						writer.WriteString(L"auto ");
						writer.WriteString(config->ConvertName(letVar->name.value));
					}
					writer.WriteString(L"){ return ");
					Call(node->expression);
					writer.WriteString(L"; }(");
					FOREACH_INDEXER(Ptr<WfLetVariable>, letVar, index, node->variables)
					{
						if (index > 0)
						{
							writer.WriteString(L", ");
						}
						auto symbol = scope->symbols[letVar->name.value][0];
						Call(letVar->value, symbol->typeInfo.Obj());
					}
					writer.WriteString(L")");
				}

				void Visit(WfIfExpression* node)override
				{
					auto firstResult = config->manager->expressionResolvings[node->trueBranch.Obj()];
					auto secondResult = config->manager->expressionResolvings[node->falseBranch.Obj()];
					auto mergedType = GetMergedType(firstResult.type, secondResult.type);

					writer.WriteString(L"(");
					Call(node->condition);
					writer.WriteString(L" ? ");
					Call(node->trueBranch, mergedType.Obj());
					writer.WriteString(L" : ");
					Call(node->falseBranch, mergedType.Obj());
					writer.WriteString(L")");
				}

				void Visit(WfRangeExpression* node)override
				{
					auto result = config->manager->expressionResolvings[node];
					auto elementType = result.type->GetElementType()->GetGenericArgument(0);

					writer.WriteString(L"::vl::__vwsn::Range(");
					Call(node->begin, elementType);
					if (node->beginBoundary == WfRangeBoundary::Exclusive)
					{
						writer.WriteString(L" + 1");
					}
					writer.WriteString(L", ");
					Call(node->end, elementType);
					if (node->endBoundary == WfRangeBoundary::Inclusive)
					{
						writer.WriteString(L" + 1");
					}
					writer.WriteString(L")");
				}

				void Visit(WfSetTestingExpression* node)override
				{
					if (auto range = node->collection.Cast<WfRangeExpression>())
					{
						auto resultElement = config->manager->expressionResolvings[node->element.Obj()];
						auto resultBegin = config->manager->expressionResolvings[range->begin.Obj()];
						auto resultEnd = config->manager->expressionResolvings[range->end.Obj()];

						writer.WriteString(L"[&](auto __vwsn_1){ return ");
						if (node->test == WfSetTesting::NotIn)
						{
							writer.WriteString(L"!");
						}
						writer.WriteString(L"(");

						Call(range->begin);
						writer.WriteString(range->beginBoundary == WfRangeBoundary::Inclusive ? L" <= " : L" < ");
						writer.WriteString(L" __vwsn_1 && __vwsn_1");
						writer.WriteString(range->endBoundary == WfRangeBoundary::Inclusive ? L" <= " : L" < ");
						Call(range->end);

						writer.WriteString(L"); }(");
						Call(node->element, resultElement.type.Obj());
						writer.WriteString(L")");
					}
					else
					{
						auto result = config->manager->expressionResolvings[node->collection.Obj()];
						auto elementType = result.type->GetElementType()->GetGenericArgument(0);

						if (node->test == WfSetTesting::NotIn)
						{
							writer.WriteString(L"(! ");
						}

						writer.WriteString(L"::vl::__vwsn::InSet(");
						Call(node->element, elementType);
						writer.WriteString(L", ");
						Call(node->collection);
						writer.WriteString(L")");

						if (node->test == WfSetTesting::NotIn)
						{
							writer.WriteString(L")");
						}
					}
				}

				void Visit(WfConstructorExpression* node)override
				{
					auto result = config->manager->expressionResolvings[node];
					auto td = result.type->GetTypeDescriptor();
					if (node->arguments.Count() == 0)
					{
						if ((td->GetTypeDescriptorFlags() & TypeDescriptorFlags::StructType) != TypeDescriptorFlags::Undefined)
						{
							writer.WriteString(config->ConvertType(result.type.Obj()) + L"{}");
						}
						else
						{
							writer.WriteString(config->ConvertType(td) + L"::Create()");
						}
					}
					else
					{
						if (result.type->GetTypeDescriptor() == description::GetTypeDescriptor<IValueList>())
						{
							auto elementType = result.type->GetElementType()->GetGenericArgument(0);
							writer.WriteString(L"(::vl::__vwsn::CreateList()");

							FOREACH(Ptr<WfConstructorArgument>, argument, node->arguments)
							{
								writer.WriteString(L".Add(");
								Call(argument->key);
								writer.WriteString(L")");
							}

							writer.WriteString(L").list");
						}
						else if (result.type->GetTypeDescriptor() == description::GetTypeDescriptor<IValueObservableList>())
						{
							auto elementType = result.type->GetElementType()->GetGenericArgument(0);
							writer.WriteString(L"(::vl::__vwsn::CreateObservableList()");

							FOREACH(Ptr<WfConstructorArgument>, argument, node->arguments)
							{
								writer.WriteString(L".Add(");
								Call(argument->key);
								writer.WriteString(L")");
							}

							writer.WriteString(L").list");
						}
						else if (result.type->GetTypeDescriptor() == description::GetTypeDescriptor<IValueDictionary>())
						{
							auto keyType = result.type->GetElementType()->GetGenericArgument(0);
							auto valueType = result.type->GetElementType()->GetGenericArgument(1);
							writer.WriteString(L"(::vl::__vwsn::CreateDictionary()");

							FOREACH(Ptr<WfConstructorArgument>, argument, node->arguments)
							{
								writer.WriteString(L".Add(");
								Call(argument->key);
								writer.WriteString(L", ");
								Call(argument->value);
								writer.WriteString(L")");
							}

							writer.WriteString(L").dictionary");
						}
						else
						{
							writer.WriteString(L"[&](){ ");
							writer.WriteString(config->ConvertType(td));
							writer.WriteString(L" __vwsn_temp__;");

							FOREACH(Ptr<WfConstructorArgument>, argument, node->arguments)
							{
								writer.WriteString(L" __vwsn_temp__.");
								writer.WriteString(argument->key.Cast<WfReferenceExpression>()->name.value);
								writer.WriteString(L" = ");
								Call(argument->value);
								writer.WriteString(L";");
							}

							writer.WriteString(L" return __vwsn_temp__; }()");
						}
					}
				}

				void Visit(WfInferExpression* node)override
				{
					Call(node->expression);
				}

				void Visit(WfTypeCastingExpression* node)override
				{
					auto scope = config->manager->nodeScopes[node].Obj();
					auto typeInfo = CreateTypeInfoFromType(scope, node->type);
					bool strongCast = node->strategy == WfTypeCastingStrategy::Strong;
					auto result = config->manager->expressionResolvings[node->expression.Obj()];
					ConvertType(config, writer, result.type.Obj(), typeInfo.Obj(), [&]() {Call(node->expression); }, strongCast);
				}

				void Visit(WfTypeTestingExpression* node)override
				{
					auto result = config->manager->expressionResolvings[node->expression.Obj()];
					Ptr<ITypeInfo> type;
					if (node->type)
					{
						auto scope = config->manager->nodeScopes[node].Obj();
						type = CreateTypeInfoFromType(scope, node->type);
					}

					switch (result.type->GetDecorator())
					{
					case ITypeInfo::RawPtr:
						{
							switch (node->test)
							{
							case WfTypeTesting::IsNull:
								writer.WriteString(L"(");
								Call(node->expression);
								writer.WriteString(L" == nullptr)");
								break;
							case WfTypeTesting::IsNotNull:
								writer.WriteString(L"(");
								Call(node->expression);
								writer.WriteString(L" != nullptr)");
								break;
							case WfTypeTesting::IsType:
							case WfTypeTesting::IsNotType:
								if (type->GetDecorator() != ITypeInfo::RawPtr)
								{
									writer.WriteString(node->test == WfTypeTesting::IsType ? L"false" : L"true");
								}
								else
								{
									writer.WriteString(L"(::vl::__vwsn::RawPtrCast<");
									writer.WriteString(config->ConvertType(type->GetTypeDescriptor()));
									writer.WriteString(L">(");
									Call(node->expression);
									writer.WriteString(L") ");
									writer.WriteString(node->test == WfTypeTesting::IsType ? L"!=" : L"==");
									writer.WriteString(L" nullptr)");
								}
								break;
							}
						}
						break;
					case ITypeInfo::SharedPtr:
						{
							if (result.type->GetElementType()->GetDecorator() == ITypeInfo::Generic)
							{
								auto toCode = [&]()
								{
									return GenerateToStream([&](StreamWriter& writer)
									{
										WfPrint(node, WString::Empty, writer);
									});
								};

								if (config->IsSpecialGenericType(result.type.Obj()))
								{
									writer.WriteString(L"/* NOT SUPPORTS: testing against non-reference generic type: ");
									writer.WriteString(toCode());
									writer.WriteString(L" */ __vwsn_not_exists__");
									return;
								}
							}
							switch (node->test)
							{
							case WfTypeTesting::IsNull:
								writer.WriteString(L"(! static_cast<bool>(");
								Call(node->expression);
								writer.WriteString(L"))");
								break;
							case WfTypeTesting::IsNotNull:
								writer.WriteString(L"static_cast<bool>(");
								Call(node->expression);
								writer.WriteString(L")");
								break;
							case WfTypeTesting::IsType:
							case WfTypeTesting::IsNotType:
								if (type->GetDecorator() != ITypeInfo::SharedPtr)
								{
									writer.WriteString(node->test == WfTypeTesting::IsType ? L"false" : L"true");
								}
								else
								{
									writer.WriteString(L"(::vl::__vwsn::RawPtrCast<");
									writer.WriteString(config->ConvertType(type->GetTypeDescriptor()));
									writer.WriteString(L">(");
									Call(node->expression);
									writer.WriteString(L".Obj()) ");
									writer.WriteString(node->test == WfTypeTesting::IsType ? L"!=" : L"==");
									writer.WriteString(L" nullptr)");
								}
								break;
							}
						}
						break;
					case ITypeInfo::Nullable:
						{
							switch (node->test)
							{
							case WfTypeTesting::IsNull:
								writer.WriteString(L"(! static_cast<bool>(");
								Call(node->expression);
								writer.WriteString(L"))");
								break;
							case WfTypeTesting::IsNotNull:
								writer.WriteString(L"static_cast<bool>(");
								Call(node->expression);
								writer.WriteString(L")");
								break;
							case WfTypeTesting::IsType:
								if (type->GetTypeDescriptor() == result.type->GetTypeDescriptor())
								{
									writer.WriteString(L"static_cast<bool>(");
									Call(node->expression);
									writer.WriteString(L")");
								}
								else
								{
									writer.WriteString(L"false");
								}
								break;
							case WfTypeTesting::IsNotType:
								if (type->GetTypeDescriptor() == result.type->GetTypeDescriptor())
								{
									writer.WriteString(L"false");
								}
								else
								{
									writer.WriteString(L"true");
								}
								break;
							}
						}
						break;
					case ITypeInfo::TypeDescriptor:
						{
							if (result.type->GetTypeDescriptor()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Object)
							{
								switch (node->test)
								{
								case WfTypeTesting::IsNull:
									Call(node->expression);
									writer.WriteString(L".IsNull()");
									break;
								case WfTypeTesting::IsNotNull:
									writer.WriteString(L"(! ");
									Call(node->expression);
									writer.WriteString(L".IsNull())");
									break;
								case WfTypeTesting::IsType:
								case WfTypeTesting::IsNotType:
									if ((type->GetTypeDescriptor()->GetTypeDescriptorFlags() & TypeDescriptorFlags::ReferenceType) != TypeDescriptorFlags::Undefined)
									{
										writer.WriteString(L"[&](){ auto __vwsn_temp__ = ");
										Call(node->expression);
										writer.WriteString(L"; return ");
										if ((type->GetDecorator() == ITypeInfo::RawPtr) == (node->test == WfTypeTesting::IsType))
										{
											writer.WriteString(L"!");
										}
										writer.WriteString(L"__vwsn_temp__.GetSharedPtr() ");
										writer.WriteString(node->test == WfTypeTesting::IsType ? L"&&" : L"||");
										writer.WriteString(L" ::vl::__vwsn::RawPtrCast<");
										writer.WriteString(config->ConvertType(type->GetTypeDescriptor()));
										writer.WriteString(L">(__vwsn_temp__.GetRawPtr()) ");
										writer.WriteString(node->test == WfTypeTesting::IsType ? L"!=" : L"==");
										writer.WriteString(L" nullptr; }()");
									}
									else
									{
										writer.WriteString(L"(dynamic_cast<::vl::reflection::description::IValueType::TypedBox<");
										writer.WriteString(config->ConvertType(type->GetTypeDescriptor()));
										writer.WriteString(L">*>(");
										Call(node->expression);
										writer.WriteString(L".GetBoxedValue().Obj()) ");
										writer.WriteString(node->test == WfTypeTesting::IsType ? L"!=" : L"==");
										writer.WriteString(L" nullptr)");
									}
									break;
								}
							}
							else if ((type->GetTypeDescriptor() == result.type->GetTypeDescriptor()) == (node->test == WfTypeTesting::IsType))
							{
								writer.WriteString(L"true");
							}
							else
							{
								writer.WriteString(L"false");
							}
						}
						break;
					default:;
					}
				}

				void Visit(WfTypeOfTypeExpression* node)override
				{
					writer.WriteString(L"/* NOT SUPPORTS: typeof() */ __vwsn_not_exists__");
				}

				void Visit(WfTypeOfExpressionExpression* node)override
				{
					writer.WriteString(L"/* NOT SUPPORTS: type() */ __vwsn_not_exists__");
				}

				void Visit(WfAttachEventExpression* node)override
				{
					auto result = config->manager->expressionResolvings[node->event.Obj()];
					if (CppExists(result.eventInfo))
					{
						WriteEventTemplate(CppGetAttachTemplate(result.eventInfo), result.eventInfo,
							[&](IEventInfo*)
							{
								writer.WriteString(L"::vl::__vwsn::This(");
								if (auto member = node->event.Cast<WfMemberExpression>())
								{
									Call(member->parent);
									auto parentResult = config->manager->expressionResolvings[member->parent.Obj()];
									if (parentResult.type->GetDecorator() == ITypeInfo::SharedPtr)
									{
										writer.WriteString(L".Obj()");
									}
								}
								else
								{
									VisitThisExpression(node, result.eventInfo->GetOwnerTypeDescriptor());
								}
								writer.WriteString(L")");
								return true;
							},
							[&](IEventInfo*)
							{
								Call(node->function);
								return true;
							},
							[&](IEventInfo*, vint) {});
					}
					else
					{
						WriteNotExists(result.eventInfo);
					}
				}

				void Visit(WfDetachEventExpression* node)override
				{
					auto result = config->manager->expressionResolvings[node->event.Obj()];
					if (CppExists(result.eventInfo))
					{
						WriteEventTemplate(CppGetDetachTemplate(result.eventInfo), result.eventInfo,
							[&](IEventInfo* eventInfo)
							{
								writer.WriteString(L"::vl::__vwsn::This(");
								if (auto member = node->event.Cast<WfMemberExpression>())
								{
									Call(member->parent);
									auto parentResult = config->manager->expressionResolvings[member->parent.Obj()];
									if (parentResult.type->GetDecorator() == ITypeInfo::SharedPtr)
									{
										writer.WriteString(L".Obj()");
									}
								}
								else
								{
									VisitThisExpression(node, eventInfo->GetOwnerTypeDescriptor());
								}
								writer.WriteString(L")");
								return true;
							},
							[&](IEventInfo*)
							{
								Call(node->handler);
								return true;
							},
							[&](IEventInfo*, vint) {});
					}
					else
					{
						WriteNotExists(result.eventInfo);
					}
				}

				void Visit(WfObserveExpression* node)override
				{
				}

				void Visit(WfCallExpression* node)override
				{
					auto thisCallback = [&](ITypeDescriptor* td)
					{
						if (auto member = node->function.Cast<WfMemberExpression>())
						{
							writer.WriteString(L"::vl::__vwsn::This(");
							Call(member->parent);
							auto parentResult = config->manager->expressionResolvings[member->parent.Obj()];
							if (parentResult.type->GetDecorator() == ITypeInfo::SharedPtr)
							{
								writer.WriteString(L".Obj()");
							}
							writer.WriteString(L")");
						}
						else
						{
							VisitThisExpression(node, td);
						}
						return true;
					};

					auto argumentCallback = [&](IMethodInfo* methodInfo, ITypeInfo* typeInfo, vint index)
					{
						auto type = methodInfo
							? methodInfo->GetParameter(index)->GetType()
							: typeInfo->GetElementType()->GetGenericArgument(index + 1)
							;
						Call(node->arguments[index], type);
					};

					auto result = config->manager->expressionResolvings[node->function.Obj()];
					if (result.methodInfo)
					{
						WriteMethodTemplate(CppGetInvokeTemplate(result.methodInfo), result.methodInfo,
							[&](IMethodInfo* methodInfo) { return thisCallback(methodInfo->GetOwnerTypeDescriptor()); },
							[&](IMethodInfo* methodInfo, vint index) { return argumentCallback(methodInfo, nullptr, index); },
							useReturnValue);
						return;
					}
					else if (result.eventInfo)
					{
						WriteEventTemplate(CppGetInvokeTemplate(result.eventInfo), result.eventInfo,
							[&](IEventInfo* eventInfo) { return thisCallback(eventInfo->GetOwnerTypeDescriptor()); },
							[&](IEventInfo*) { return false; },
							[&](IEventInfo* eventInfo, vint index) { return argumentCallback(nullptr, eventInfo->GetHandlerType(), index); }
							);
						return;
					}
					else if (result.symbol)
					{
						if (auto funcDecl = result.symbol->creatorNode.Cast<WfFunctionDeclaration>())
						{
							if (result.symbol->ownerScope->ownerNode.Cast<WfNewInterfaceExpression>())
							{
								writer.WriteString(L"this->");
								writer.WriteString(config->ConvertName(result.symbol->name));
							}
							else if (result.symbol->ownerScope->functionConfig && result.symbol->ownerScope->functionConfig->lambda && result.symbol->name == funcDecl->name.value)
							{
								writer.WriteString(L"(*this)");
							}
							else
							{
								writer.WriteString(L"GLOBAL_NAME ");
								writer.WriteString(config->ConvertName(result.symbol->name));
							}
							writer.WriteString(L"(");
							for (vint i = 0; i < node->arguments.Count(); i++)
							{
								if (i > 0) writer.WriteString(L", ");
								argumentCallback(nullptr, result.symbol->typeInfo.Obj(), i);
							}
							writer.WriteString(L")");
							return;
						}
					}
					Call(node->function);
					writer.WriteString(L"(");
					for (vint i = 0; i < node->arguments.Count(); i++)
					{
						if (i > 0) writer.WriteString(L", ");
						argumentCallback(nullptr, result.type.Obj(), i);
					}
					writer.WriteString(L")");
				}

				void Visit(WfFunctionExpression* node)override
				{
					writer.WriteString(L"LAMBDA(::");
					writer.WriteString(config->assemblyNamespace);
					writer.WriteString(L"::");
					writer.WriteString(config->lambdaExprs[node]);
					writer.WriteString(L"(");

					auto closureInfo = config->closureInfos[node];
					WriteClosureArguments(closureInfo, node);

					writer.WriteString(L"))");
				}

				void Visit(WfNewClassExpression* node)override
				{
					auto result = config->manager->expressionResolvings[node];
					auto ctor = result.constructorInfo;

					if (ctor->GetReturn()->GetDecorator() == ITypeInfo::SharedPtr)
					{
						writer.WriteString(L"::vl::Ptr<");
						writer.WriteString(config->ConvertType(ctor->GetReturn()->GetTypeDescriptor()));
						writer.WriteString(L">(");
					}

					WriteMethodTemplate(CppGetInvokeTemplate(ctor), ctor, [&](IMethodInfo*) { return false; },
						[&](IMethodInfo*, vint index)
						{
							Call(node->arguments[index]);
						}, useReturnValue);

					if (ctor->GetReturn()->GetDecorator() == ITypeInfo::SharedPtr)
					{
						writer.WriteString(L")");
					}
				}

				void Visit(WfNewInterfaceExpression* node)override
				{
					auto result = config->manager->expressionResolvings[node];
					auto ctor = result.constructorInfo;

					if (ctor->GetReturn()->GetDecorator() == ITypeInfo::SharedPtr)
					{
						writer.WriteString(L"::vl::Ptr<");
						writer.WriteString(config->ConvertType(ctor->GetReturn()->GetTypeDescriptor()));
						writer.WriteString(L">(");
					}
					else
					{
						writer.WriteString(L"static_cast<");
						writer.WriteString(config->ConvertType(ctor->GetReturn()->GetTypeDescriptor()));
						writer.WriteString(L"*>(");
					}

					writer.WriteString(L"new ::");
					writer.WriteString(config->assemblyNamespace);
					writer.WriteString(L"::");
					writer.WriteString(config->classExprs[node]);
					writer.WriteString(L"(");

					auto closureInfo = config->closureInfos[node];
					WriteClosureArguments(closureInfo, node);

					writer.WriteString(L"))");
				}

				void Visit(WfVirtualCfeExpression* node)override
				{
					Call(node->expandedExpression);
				}

				void Visit(WfVirtualCseExpression* node)override
				{
					Call(node->expandedExpression);
				}
			};

			void GenerateExpression(WfCppConfig* config, stream::StreamWriter& writer, Ptr<WfExpression> node, reflection::description::ITypeInfo* expectedType, bool useReturnValue)
			{
				WfGenerateExpressionVisitor visitor(config, writer, useReturnValue);
				if (useReturnValue)
				{
					auto result = config->manager->expressionResolvings[node.Obj()];
					ITypeInfo* types[] = { result.type.Obj(), result.expectedType.Obj(), expectedType };
					visitor.ConvertMultipleTypes(types, sizeof(types) / sizeof(*types), [&]()
					{
						node->Accept(&visitor);
					});
				}
				else
				{
					node->Accept(&visitor);
				}
			}
		}
	}
}
