#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;
			using namespace regex;
			using namespace reflection;
			using namespace reflection::description;
			using namespace analyzer;

			class WfGenerateExpressionVisitor : public Object, public WfExpression::IVisitor
			{
			public:
				WfCppConfig*				config;
				stream::StreamWriter&		writer;

				WfGenerateExpressionVisitor(WfCppConfig* _config, stream::StreamWriter& _writer)
					:config(_config)
					, writer(_writer)
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
					writer.WriteString(L"::vl::reflection::description::BoxValue<");
					writer.WriteString(config->ConvertType(type));
					writer.WriteString(L">(");
					writeExpression();
					writer.WriteString(L")");
				}

				template<typename T>
				void WriteUnboxValue(ITypeInfo* type, const T& writeExpression)
				{
					writer.WriteString(L"::vl::reflection::description::UnboxValue<");
					writer.WriteString(config->ConvertType(type));
					writer.WriteString(L">(");
					writeExpression();
					writer.WriteString(L")");
				}

				template<typename T>
				void WriteBoxParameter(ITypeInfo* type, const T& writeExpression)
				{
					writer.WriteString(L"[&](){ ");
					writer.WriteString(config->ConvertType(type));
					writer.WriteString(L" __vwsn_temp__ = ");
					writeExpression();
					writer.WriteString(L"; return ::vl::reflection::description::BoxParameter<");
					writer.WriteString(config->ConvertType(type));
					writer.WriteString(L">(__vwsn_temp__); }()");
				}

				template<typename T>
				void WriteUnboxParameter(ITypeInfo* type, const T& writeExpression)
				{
					writer.WriteString(L"[&](){ ");
					writer.WriteString(config->ConvertType(type));
					writer.WriteString(L" __vwsn_temp__; ::vl::reflection::description::UnboxParameter<");
					writer.WriteString(config->ConvertType(type));
					writer.WriteString(L">(");
					writeExpression();
					writer.WriteString(L", __vwsn_temp__); return __vwsn_temp__; }()");
				}

				template<typename T>
				void ConvertValueType(ITypeDescriptor* fromTd, ITypeDescriptor* toTd, const T& writeExpression)
				{
					if (fromTd == description::GetTypeDescriptor<WString>())
					{
						writer.WriteString(L"[&](){ ");
						writer.WriteString(config->ConvertType(toTd));
						writer.WriteString(L" __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<");
						writer.WriteString(config->ConvertType(toTd));
						writer.WriteString(L">::Deserialize(");
						writeExpression();
						writer.WriteString(L", __vwsn_temp__); return __vwsn_temp__; }()");
					}
					else if (toTd == description::GetTypeDescriptor<WString>())
					{
						writer.WriteString(L"[&](){ ");
						writer.WriteString(config->ConvertType(toTd));
						writer.WriteString(L" __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<");
						writer.WriteString(config->ConvertType(fromTd));
						writer.WriteString(L">::Serialize(");
						writeExpression();
						writer.WriteString(L", __vwsn_temp__); return __vwsn_temp__; }()");
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
							auto toType = types[0];
							if (fromType->GetTypeDescriptor()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Object)
							{
								if (config->IsSpecialGenericType(toType))
								{
									WriteUnboxParameter(toType, writeExpression);
								}
								else
								{
									WriteUnboxValue(toType, writeExpression);
								}
								return;
							}
							else if (toType->GetTypeDescriptor()->GetTypeDescriptorFlags() == TypeDescriptorFlags::Object)
							{
								if (config->IsSpecialGenericType(fromType))
								{
									WriteBoxParameter(fromType, writeExpression);
								}
								else
								{
									WriteBoxValue(fromType, writeExpression);
								}
								return;
							}
							else
							{
								switch (fromType->GetDecorator())
								{
								case ITypeInfo::RawPtr:
									{
										switch (toType->GetDecorator())
										{
										case ITypeInfo::RawPtr:
											writer.WriteString(L"dynamic_cast<");
											writer.WriteString(config->ConvertType(toType));
											writer.WriteString(L">(");
											writeExpression();
											writer.WriteString(L")");
											return;
										case ITypeInfo::SharedPtr:
											writer.WriteString(L"::vl::Ptr<");
											writer.WriteString(config->ConvertType(toType));
											writer.WriteString(L">(");
											writeExpression();
											writer.WriteString(L")");
											return;
										}
									}
									break;
								case ITypeInfo::SharedPtr:
									{
										switch (toType->GetDecorator())
										{
										case ITypeInfo::RawPtr:
											writeExpression();
											writer.WriteString(L".Obj()");
											return;
										case ITypeInfo::SharedPtr:
											writeExpression();
											writer.WriteString(L".Cast<");
											writer.WriteString(config->ConvertType(toType->GetTypeDescriptor()));
											writer.WriteString(L">()");
											return;
										}
									}
									break;
								case ITypeInfo::Nullable:
									{
										switch (toType->GetDecorator())
										{
										case ITypeInfo::Nullable:
											writer.WriteString(L"[](");
											writer.WriteString(config->ConvertType(fromType));
											writer.WriteString(L" __vwsn_temp__){ if (__vwsn_temp__) return ");
											writer.WriteString(config->ConvertType(toType));
											writer.WriteString(L"(");
											ConvertValueType(fromType->GetTypeDescriptor(), toType->GetTypeDescriptor(), [&]()
											{
												writer.WriteString(L"__vwsn__temp");
											});
											writer.WriteString(L"); else return ");
											writer.WriteString(config->ConvertType(toType));
											writer.WriteString(L"(); }(");
											writeExpression();
											writer.WriteString(L")");
											return;
										case ITypeInfo::TypeDescriptor:
											ConvertValueType(fromType->GetTypeDescriptor(), toType->GetTypeDescriptor(), [&]()
											{
												writeExpression();
												writer.WriteString(L".Value()");
											});
											return;
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
											ConvertValueType(fromType->GetTypeDescriptor(), toType->GetTypeDescriptor(), writeExpression);
											writer.WriteString(L")");
											return;
										case ITypeInfo::TypeDescriptor:
											ConvertValueType(fromType->GetTypeDescriptor(), toType->GetTypeDescriptor(), writeExpression);
											return;
										}
									}
									break;
								}
							}
							writer.WriteString(L"/* NOT EXISTS: convert (");
							writer.WriteString(config->ConvertType(fromType));
							writer.WriteString(L") to (");
							writer.WriteString(config->ConvertType(toType));
							writer.WriteString(L") */ __vwsn_not_exists__(");
							writeExpression();
							writer.WriteString(L")");
						});
					}
					else
					{
						ConvertMultipleTypes(types + 1, typesLength - 1, writeExpression);
					}
				}

				void Call(Ptr<WfExpression> node, ITypeInfo* expectedType = nullptr)
				{
					GenerateExpression(config, writer, node, expectedType);
				}

				Ptr<WfCppConfig::ClosureInfo> GetClosureInfo(WfExpression* node)
				{
					Ptr<WfCppConfig::ClosureInfo> closureInfo;
					auto scope = config->manager->nodeScopes[node].Obj();

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
						scope = scope->parentScope.Obj();
					}

					return closureInfo;
				}

				void WriteGlobalType()
				{
					writer.WriteString(L"::");
					writer.WriteString(config->assemblyNamespace);
					writer.WriteString(L"::");
					writer.WriteString(config->assemblyName);
				}

				void WriteGlobalObject()
				{
					writer.WriteString(L"::");
					writer.WriteString(config->assemblyNamespace);
					writer.WriteString(L"::");
					writer.WriteString(config->assemblyName);
					writer.WriteString(L"::Instance()");
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

				void VisitSymbol(WfExpression* node, Ptr<WfLexicalSymbol> symbol)
				{
					if (auto varDecl = symbol->creatorNode.Cast<WfVariableDeclaration>())
					{
						auto ownerNode = symbol->ownerScope->ownerNode;
						if (ownerNode.Cast<WfNamespaceDeclaration>() || ownerNode.Cast<WfModule>())
						{
							WriteGlobalObject();
							writer.WriteString(L".");
							writer.WriteString(config->ConvertName(symbol->name));
							return;
						}
						else if(auto closureInfo = GetClosureInfo(node))
						{
							if (closureInfo->symbols.Values().Contains(symbol.Obj()))
							{
								writer.WriteString(L"::vl::__vwsn::This(this)->");
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
							writer.WriteString(L"(&");
							WriteGlobalObject();
							writer.WriteString(L", &");
							WriteGlobalType();
							writer.WriteString(L"::");
							writer.WriteString(config->ConvertName(symbol->name));
							writer.WriteString(L")");
							return;
						}
						else if (auto classExpr = ownerNode.Cast<WfNewInterfaceExpression>())
						{
							writer.WriteString(config->ConvertType(symbol->typeInfo.Obj()));
							writer.WriteString(L"(::vl::__vwsn::This(this), &");
							writer.WriteString(config->classExprs[classExpr.Obj()]);
							writer.WriteString(L"::");
							writer.WriteString(config->ConvertName(symbol->name));
							writer.WriteString(L")");
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

				template<typename TThis, typename TArguments>
				void WriteMethodTemplate(const WString& templateValue, IMethodInfo* methodInfo, const TThis& thisCallback, const TArguments& argumentsCallback)
				{
					WriteTemplate(templateValue, [&](const WString& item)
					{
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
							return argumentsCallback(methodInfo, CommaPosition::No);
						}
						else if (item == L", $Arguments")
						{
							return argumentsCallback(methodInfo, CommaPosition::Left);
						}
						else if (item == L"$Arguments, ")
						{
							return argumentsCallback(methodInfo, CommaPosition::Right);
						}
						return false;
					});
				}

				template<typename TThis>
				void WritePropertyTemplate(const WString& templateValue, IPropertyInfo* propertyInfo, const TThis& thisCallback)
				{
					WriteTemplate(templateValue, [&](const WString& item)
					{
						if (item == L"$Type")
						{
							config->ConvertType(propertyInfo->GetOwnerTypeDescriptor());
							return true;
						}
						else if (item == L"$Name")
						{
							config->ConvertName(propertyInfo->GetName());
							return true;
						}
						else if (item == L"$This")
						{
							return thisCallback(propertyInfo);
						}
						return false;
					});
				}

				template<typename TThis, typename TArguments>
				void WriteEventTemplate(const WString& templateValue, IEventInfo* eventInfo, const TThis& thisCallback, const TArguments& argumentsCallback)
				{
					WriteTemplate(templateValue, [&](const WString& item)
					{
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
							return argumentsCallback(eventInfo, CommaPosition::No);
						}
						else if (item == L"$Arguments")
						{
							return argumentsCallback(eventInfo, CommaPosition::No);
						}
						else if (item == L", $Arguments")
						{
							return argumentsCallback(eventInfo, CommaPosition::Left);
						}
						else if (item == L"$Arguments, ")
						{
							return argumentsCallback(eventInfo, CommaPosition::Right);
						}
						return false;
					});
				}

				template<typename TMethodThis, typename TPropertyThis>
				bool WriteReferenceTemplate(ResolveExpressionResult& result, const TMethodThis& methodThis, const TPropertyThis& propertyThis)
				{
					if (result.methodInfo)
					{
						if (CppExists(result.methodInfo))
						{
							auto methodInfo = result.methodInfo;
							WriteMethodTemplate(CppGetClosureTemplate(methodInfo), methodInfo, methodThis,
								[&](IMethodInfo*, CommaPosition)
								{
									return false;
								});
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
								WriteMethodTemplate(CppGetInvokeTemplate(methodInfo), methodInfo, methodThis,
									[&](IMethodInfo*, CommaPosition)
									{
										return true;
									});
							}
							else
							{
								auto templateValue = CppGetReferenceTemplate(result.propertyInfo);
								auto propertyInfo = result.propertyInfo;
								WritePropertyTemplate(CppGetReferenceTemplate(propertyInfo), propertyInfo, propertyThis);
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
							writer.WriteString(L"::vl::__vwsn::This(");
							VisitThisExpression(node, methodInfo->GetOwnerTypeDescriptor());
							writer.WriteString(L")");
							return true;
						},
						[&](IPropertyInfo* propertyInfo)
						{
							writer.WriteString(L"::vl::__vwsn::This(");
							VisitThisExpression(node, propertyInfo->GetOwnerTypeDescriptor());
							writer.WriteString(L")");
							return true;
						});

					if (!success)
					{
						if (result.symbol)
						{
							VisitSymbol(node, result.symbol);
						}
						else
						{
							if ((result.type->GetTypeDescriptor()->GetTypeDescriptorFlags() & TypeDescriptorFlags::EnumType) != TypeDescriptorFlags::Undefined)
							{
								auto enumType = result.type->GetTypeDescriptor()->GetEnumType();
								vint index = enumType->IndexOfItem(name);
								if (index != -1)
								{
									writer.WriteString(config->ConvertType(result.type.Obj()));
									writer.WriteString(L"::");
									writer.WriteString(name);
									return;
								}
							}
							CHECK_FAIL(L"WfGenerateExpressionVisitor::VisitReferenceExpression(WfExpression*, const WString&)#Internal error, cannot find any record of this expression.");
						}
					}
				}
				
				void WriteClosureArguments(Ptr<WfCppConfig::ClosureInfo> closureInfo, WfExpression* node)
				{
					FOREACH_INDEXER(Ptr<WfLexicalSymbol>, symbol, index, closureInfo->symbols.Values())
					{
						if (index > 0)
						{
							writer.WriteString(L", ");
						}
						VisitSymbol(node, symbol);
					}

					FOREACH_INDEXER(ITypeDescriptor*, thisType, index, closureInfo->thisTypes)
					{
						if (index > 0 || closureInfo->symbols.Count() > 0)
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
					WriteReferenceTemplate(result,
						[&](IMethodInfo* methodInfo)
						{
							writer.WriteString(L"::vl::__vwsn::This(");
							Call(node->parent);
							writer.WriteString(L")");
							return true;
						},
						[&](IPropertyInfo* propertyInfo)
						{
							writer.WriteString(L"::vl::__vwsn::This(");
							Call(node->parent);
							writer.WriteString(L")");
							return true;
						});
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
						writer.WriteString(L"nullptr");
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
					if (td == description::GetTypeDescriptor<float>())
					{
						writer.WriteString(node->value.value + L"f");
					}
					else if (td == description::GetTypeDescriptor<double>())
					{
						writer.WriteString(node->value.value);
					}
				}

				void Visit(WfIntegerExpression* node)override
				{
					auto result = config->manager->expressionResolvings[node];
					auto td = result.type->GetTypeDescriptor();
					if (td == description::GetTypeDescriptor<vint32_t>())
					{
						writer.WriteString(node->value.value);
					}
					else if (td == description::GetTypeDescriptor<vuint32_t>())
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
				}

				void Visit(WfStringExpression* node)override
				{
					writer.WriteString(L"::vl::WString(L\"");
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
					writer.WriteString(L"\", false)");
				}

				void Visit(WfFormatExpression* node)override
				{
					Call(node->expandedExpression);
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
						writer.WriteString(L"(! ");
						Call(node->operand, result.type.Obj());
						writer.WriteString(L")");
						break;
					}
				}

				void VisitBinaryExpression(WfBinaryExpression* node, const wchar_t* op, ITypeDescriptor* type = nullptr)
				{
					if (type)
					{
						writer.WriteString(L"static_cast<");
						writer.WriteString(config->ConvertType(type));
						writer.WriteString(L">");
					}
					writer.WriteString(L"(");
					Call(node->first);
					writer.WriteString(L" ");
					writer.WriteString(op);
					writer.WriteString(L" ");
					Call(node->second);
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
						if (auto binary = node->first.Cast<WfBinaryExpression>())
						{
							auto containerType = config->manager->expressionResolvings[binary->first.Obj()].type.Obj();
							auto keyType = config->manager->expressionResolvings[binary->second.Obj()].type.Obj();
							auto valueType = config->manager->expressionResolvings[node->second.Obj()].type.Obj();
							writer.WriteString(L"::vl::__vwsn::This(");
							Call(binary->first);
							writer.WriteString(L")->Set(");
							if (containerType->GetTypeDescriptor() == description::GetTypeDescriptor<IValueDictionary>())
							{
								WriteBoxParameter(keyType, [&]() {Call(binary->second); });
							}
							else
							{
								Call(binary->second);
							}
							writer.WriteString(L", ");
							WriteBoxParameter(valueType, [&]() {Call(node->second); });
							writer.WriteString(L")");
						}
						else if (auto member = node->first.Cast<WfMemberExpression>())
						{
							auto result = config->manager->expressionResolvings[member.Obj()];
							auto propInfo = result.propertyInfo;
							if (CppExists(propInfo))
							{
								if (propInfo->GetCpp() == nullptr && propInfo->GetSetter() != nullptr)
								{
									WriteMethodTemplate(CppGetInvokeTemplate(propInfo->GetSetter()), propInfo->GetSetter(),
										[&](IMethodInfo*)
										{
											writer.WriteString(L"::vl::__vwsn::This(");
											Call(member->parent);
											writer.WriteString(L")");
											return true;
										},
										[&](IMethodInfo*, CommaPosition cp)
										{
											if (cp == CommaPosition::Left) writer.WriteString(L", ");
											Call(node->second);
											if (cp == CommaPosition::Right) writer.WriteString(L", ");
											return true;
										});
								}
								else
								{
									writer.WriteString(L"(");
									WritePropertyTemplate(CppGetReferenceTemplate(propInfo), propInfo, 
										[&](IPropertyInfo*)
										{
											writer.WriteString(L"::vl::__vwsn::This(");
											Call(member->parent);
											writer.WriteString(L")");
											return true;
										});
									writer.WriteString(L")");
								}
							}
							else
							{
								WriteNotExists(propInfo);
							}
						}
						else
						{
							VisitBinaryExpression(node, L"=");
						}
					}
					else if (node->op == WfBinaryOperator::Index)
					{
						auto containerType = config->manager->expressionResolvings[node->first.Obj()].type.Obj();
						auto keyType = config->manager->expressionResolvings[node->second.Obj()].type.Obj();
						auto valueType = config->manager->expressionResolvings[node].type.Obj();
						WriteUnboxParameter(valueType, [&]()
						{
							writer.WriteString(L"::vl::__vwsn::This(");
							Call(node->first);
							writer.WriteString(L")->Get(");
							if (containerType->GetTypeDescriptor()->CanConvertTo(description::GetTypeDescriptor<IValueReadonlyDictionary>()))
							{
								WriteBoxParameter(keyType, [&]() {Call(node->second); });
							}
							else
							{
								Call(node->second);
							}
							writer.WriteString(L")");
						});
					}
					else if (node->op == WfBinaryOperator::Union)
					{
						auto type = config->manager->expressionResolvings[node].type;
						if (type->GetTypeDescriptor() == description::GetTypeDescriptor<WString>())
						{
							VisitBinaryExpression(node, L"+");
						}
						else
						{
							VisitBinaryExpression(node, L"&");
						}
					}
					else if (node->op == WfBinaryOperator::Intersect)
					{
						VisitBinaryExpression(node, L"|");
					}
					else if (node->op == WfBinaryOperator::FailedThen)
					{
						writer.WriteString(L"[&](){ try{ return ");
						Call(node->first);
						writer.WriteString(L"; } catch(...){ return ");
						Call(node->second);
						writer.WriteString(L"; } }()");
					}
					else
					{
						auto result = config->manager->expressionResolvings[node];
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
								VisitBinaryExpression(node, op, (BinaryNeedConvert(result.type->GetTypeDescriptor()) ? result.type->GetTypeDescriptor() : nullptr));
							}
							return;
						case WfBinaryOperator::EQ:
						case WfBinaryOperator::NE:
							{
								auto firstResult = config->manager->expressionResolvings[node->first.Obj()];
								auto secondResult = config->manager->expressionResolvings[node->second.Obj()];
								if (firstResult.type->GetDecorator() == ITypeInfo::RawPtr || firstResult.type->GetDecorator() == ITypeInfo::SharedPtr)
								{
									auto mergedType = GetMergedType(firstResult.type, secondResult.type);
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
								VisitBinaryExpression(node, op);
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
									td = nullptr;
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
								VisitBinaryExpression(node, op, td);
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
						auto symbol = scope->symbols[letVar->name.value][0];
						writer.WriteString(config->ConvertType(symbol->typeInfo.Obj()));
						writer.WriteString(L" ");
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
						Call(letVar->value);
					}
					writer.WriteString(L")");
				}

				void Visit(WfIfExpression* node)override
				{
					writer.WriteString(L"[&](){ if (");
					Call(node->condition);
					writer.WriteString(L") return ");
					Call(node->trueBranch);
					writer.WriteString(L"; else return ");
					Call(node->falseBranch);
					writer.WriteString(L"; }()");
				}

				void Visit(WfRangeExpression* node)override
				{
					auto result = config->manager->expressionResolvings[node];
					auto elementType = result.type->GetElementType()->GetGenericArgument(0);

					writer.WriteString(L"[&](");
					writer.WriteString(config->ConvertType(elementType));
					writer.WriteString(L" __vwsn_1, ");
					writer.WriteString(config->ConvertType(elementType));
					writer.WriteString(L" __vwsn_2){ return ::vl::collections::Range<");
					writer.WriteString(config->ConvertType(elementType));
					writer.WriteString(L">(__vwsn_1, __vwsn_2 - __vwsn_1); }(");
					Call(node->begin);
					if (node->beginBoundary == WfRangeBoundary::Exclusive)
					{
						writer.WriteString(L" + 1");
					}
					writer.WriteString(L", ");
					Call(node->end);
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

						writer.WriteString(L"[&](");
						writer.WriteString(config->ConvertType(resultElement.type.Obj()));
						writer.WriteString(L" __vwsn_1{ return ");

						Call(range->begin);
						writer.WriteString(range->beginBoundary == WfRangeBoundary::Inclusive ? L" <= " : L" < ");
						writer.WriteString(L" __vwsn_1 && __vwsn_1");
						writer.WriteString(range->endBoundary == WfRangeBoundary::Inclusive ? L" <= " : L" < ");
						Call(range->end);

						writer.WriteString(L"; }(");
						Call(node->element);
						writer.WriteString(L")");
					}
					else
					{
						auto result = config->manager->expressionResolvings[node->collection.Obj()];
						auto elementType = result.type->GetElementType()->GetGenericArgument(0);
						auto elementTypeCpp = elementType ? config->ConvertType(elementType) : config->ConvertType(description::GetTypeDescriptor<Value>());

						writer.WriteString(L"[&](");
						writer.WriteString(elementTypeCpp);
						writer.WriteString(L" __vwsn_1){ return ");
						if (result.type->GetTypeDescriptor() != description::GetTypeDescriptor<IValueEnumerable>())
						{
							writer.WriteString(L"::vl::reflection::description::GetLazyList<");
							writer.WriteString(elementTypeCpp);
							writer.WriteString(L"(");
						}
						Call(node->collection);
						if (result.type->GetTypeDescriptor() != description::GetTypeDescriptor<IValueEnumerable>())
						{
							writer.WriteString(L")");
						}
						writer.WriteString(L".Any([&](");
						writer.WriteString(elementTypeCpp);
						writer.WriteString(L" __vwsn_2){ return __vwsn_1 == __vwsn_2; }); }(");
						Call(node->element);
						writer.WriteString(L")");
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
							writer.WriteString(L"[&](){ auto __vwsn_temp__ = ");
							writer.WriteString(config->ConvertType(td));
							writer.WriteString(L"::Create();");

							FOREACH(Ptr<WfConstructorArgument>, argument, node->arguments)
							{
								writer.WriteString(L" __vwsn_temp__->Add(");
								WriteBoxParameter(elementType, [&]() {Call(argument->key); });
								writer.WriteString(L");");
							}

							writer.WriteString(L" return __vwsn_temp__; }()");
						}
						else if (result.type->GetTypeDescriptor() == description::GetTypeDescriptor<IValueDictionary>())
						{
							auto keyType = result.type->GetElementType()->GetGenericArgument(0);
							auto valueType = result.type->GetElementType()->GetGenericArgument(1);
							writer.WriteString(L"[&](){ auto __vwsn_temp__ = ");
							writer.WriteString(config->ConvertType(td));
							writer.WriteString(L"::Create();");

							FOREACH(Ptr<WfConstructorArgument>, argument, node->arguments)
							{
								writer.WriteString(L" __vwsn_temp__->Set(");
								WriteBoxParameter(keyType, [&]() {Call(argument->key); });
								writer.WriteString(L", ");
								WriteBoxParameter(valueType, [&]() {Call(argument->value); });
								writer.WriteString(L");");
							}

							writer.WriteString(L" return __vwsn_temp__; }()");
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
					auto scope = config->manager->nodeScopes[node].Obj();
					auto typeInfo = CreateTypeInfoFromType(scope, node->type);
					Call(node->expression, typeInfo.Obj());
				}

				void Visit(WfTypeCastingExpression* node)override
				{
					auto scope = config->manager->nodeScopes[node].Obj();
					auto typeInfo = CreateTypeInfoFromType(scope, node->type);
					Call(node->expression, typeInfo.Obj());
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
								writer.WriteString(L"(dynamic_cast<");
								writer.WriteString(config->ConvertType(type->GetTypeDescriptor()));
								writer.WriteString(L"*>(");
								Call(node->expression);
								writer.WriteString(L") != nullptr)");
								break;
							case WfTypeTesting::IsNotType:
								writer.WriteString(L"(dynamic_cast<");
								writer.WriteString(config->ConvertType(type->GetTypeDescriptor()));
								writer.WriteString(L"*>(");
								Call(node->expression);
								writer.WriteString(L") == nullptr)");
								break;
							}
						}
						break;
					case ITypeInfo::SharedPtr:
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
								break;
							case WfTypeTesting::IsType:
								writer.WriteString(L"static_cast<bool>(");
								Call(node->expression);
								writer.WriteString(L".Cast<");
								writer.WriteString(config->ConvertType(type->GetTypeDescriptor()));
								writer.WriteString(L">())");
								break;
							case WfTypeTesting::IsNotType:
								writer.WriteString(L"(! static_cast<bool>(");
								Call(node->expression);
								writer.WriteString(L".Cast<");
								writer.WriteString(config->ConvertType(type->GetTypeDescriptor()));
								writer.WriteString(L">()))");
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
										writer.WriteString(L"(dynamic_cast<");
										writer.WriteString(config->ConvertType(type->GetTypeDescriptor()));
										writer.WriteString(L"*>(");
										Call(node->expression);
										writer.WriteString(L".GetRawPtr()) ");
										writer.WriteString(node->test == WfTypeTesting::IsType ? L"!=" : L"==");
										writer.WriteString(L" nullptr)");
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
								}
								else
								{
									VisitThisExpression(node, result.eventInfo->GetOwnerTypeDescriptor());
								}
								writer.WriteString(L")");
								return true;
							},
							[&](IEventInfo*, CommaPosition)
							{
								Call(node->function);
								return true;
							});
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
								}
								else
								{
									VisitThisExpression(node, eventInfo->GetOwnerTypeDescriptor());
								}
								writer.WriteString(L")");
								return true;
							},
							[&](IEventInfo*, CommaPosition)
							{
								Call(node->handler);
								return true;
							});
					}
					else
					{
						WriteNotExists(result.eventInfo);
					}
				}

				void Visit(WfBindExpression* node)override
				{
					Call(node->expandedExpression);
				}

				void Visit(WfObserveExpression* node)override
				{
				}

				void Visit(WfCallExpression* node)override
				{
					auto thisCallback = [&](ITypeDescriptor* td)
					{
						writer.WriteString(L"::vl::__vwsn::This(");
						if (auto member = node->function.Cast<WfMemberExpression>())
						{
							Call(member->parent);
						}
						else
						{
							VisitThisExpression(node, td);
						}
						writer.WriteString(L")");
						return true;
					};

					auto argumentsCallback = [&](CommaPosition cp)
					{
						if (cp == CommaPosition::Left) writer.WriteString(L", ");
						FOREACH_INDEXER(Ptr<WfExpression>, argument, index, node->arguments)
						{
							if (index > 0) writer.WriteString(L", ");
							Call(argument);
						}
						if (cp == CommaPosition::Right) writer.WriteString(L", ");
						return true;
					};

					auto result = config->manager->expressionResolvings[node->function.Obj()];
					if (result.methodInfo)
					{
						WriteMethodTemplate(CppGetInvokeTemplate(result.methodInfo), result.methodInfo,
							[&](IMethodInfo* methodInfo) { return thisCallback(methodInfo->GetOwnerTypeDescriptor()); },
							[&](IMethodInfo*, CommaPosition cp) { return argumentsCallback(cp); }
							);
						return;
					}
					else if (result.eventInfo)
					{
						WriteEventTemplate(CppGetInvokeTemplate(result.eventInfo), result.eventInfo,
							[&](IEventInfo* eventInfo) { return thisCallback(eventInfo->GetOwnerTypeDescriptor()); },
							[&](IEventInfo*, CommaPosition cp) { return argumentsCallback(cp); }
							);
						return;
					}
					else if (result.symbol)
					{
						if (result.symbol->creatorNode.Cast<WfFunctionDeclaration>())
						{
							if (result.symbol->ownerScope->ownerNode.Cast<WfNewInterfaceExpression>())
							{
								writer.WriteString(L"::vl::__vwsn::This(this)->");
								writer.WriteString(config->ConvertName(result.symbol->name));
							}
							else
							{
								WriteGlobalObject();
								writer.WriteString(L".");
								writer.WriteString(config->ConvertName(result.symbol->name));
							}
							writer.WriteString(L"(");
							argumentsCallback(CommaPosition::No);
							writer.WriteString(L")");
							return;
						}
					}
					Call(node->function);
					writer.WriteString(L"(");
					argumentsCallback(CommaPosition::No);
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
						[&](IMethodInfo*, CommaPosition cp)
						{
							if (node->arguments.Count() > 0)
							{
								if (cp == CommaPosition::Left) writer.WriteString(L", ");
								FOREACH_INDEXER(Ptr<WfExpression>, argument, index, node->arguments)
								{
									if (index > 0) writer.WriteString(L", ");
									Call(argument);
								}
								if (cp == CommaPosition::Right) writer.WriteString(L", ");
							}
							return true;
						});

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
			};

			void GenerateExpression(WfCppConfig* config, stream::StreamWriter& writer, Ptr<WfExpression> node, reflection::description::ITypeInfo* expectedType)
			{
				auto result = config->manager->expressionResolvings[node.Obj()];
				WfGenerateExpressionVisitor visitor(config, writer);

				ITypeInfo* types[] = { result.type.Obj(), result.expectedType.Obj(), expectedType };
				visitor.ConvertMultipleTypes(types, sizeof(types) / sizeof(*types), [&]()
				{
					node->Accept(&visitor);
				});
			}
		}
	}
}