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
					writer.WriteString(L"::vl::reflection::description::BoxParameter<");
					writer.WriteString(config->ConvertType(type));
					writer.WriteString(L">(");
					writeExpression();
					writer.WriteString(L")");
				}

				template<typename T>
				void WriteUnboxParameter(ITypeInfo* type, const T& writeExpression)
				{
					writer.WriteString(L"[&](){ ");
					writer.WriteString(config->ConvertType(type));
					writer.WriteString(L" __vwsn__temp__; ::vl::reflection::description::UnboxParameter<");
					writer.WriteString(config->ConvertType(type));
					writer.WriteString(L">(");
					writeExpression();
					writer.WriteString(L", __vwsn__temp__); return __vwsn__temp__; }()");
				}

				template<typename T>
				void ConvertValueType(ITypeDescriptor* fromTd, ITypeDescriptor* toTd, const T& writeExpression)
				{
					if (fromTd == description::GetTypeDescriptor<WString>())
					{
						writer.WriteString(L"[&](){ ");
						writer.WriteString(config->ConvertType(toTd));
						writer.WriteString(L" __vwsn__temp__; ::vl::reflection::description::TypedValueSerializerProvider<");
						writer.WriteString(config->ConvertType(toTd));
						writer.WriteString(L">::Deserialize(");
						writeExpression();
						writer.WriteString(L", __vwsn__temp__); return __vwsn__temp__; }()");
					}
					else if (toTd == description::GetTypeDescriptor<WString>())
					{
						writer.WriteString(L"[&](){ ");
						writer.WriteString(config->ConvertType(toTd));
						writer.WriteString(L" __vwsn__temp__; ::vl::reflection::description::TypedValueSerializerProvider<");
						writer.WriteString(config->ConvertType(fromTd));
						writer.WriteString(L">::Serialize(");
						writeExpression();
						writer.WriteString(L", __vwsn__temp__); return __vwsn__temp__; }()");
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
											writer.WriteString(L" __vwsn__temp__){ if (__vwsn__temp__) return ");
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
						auto index = closureInfo->thisTypes.IndexOf(td);
						if (index != -1)
						{
							writer.WriteString(L"__vwsnthis_");
							writer.WriteString(L"itow(index)");
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
						else
						{
							auto closureInfo = GetClosureInfo(node);
							if (closureInfo->symbols.Values().Contains(symbol.Obj()))
							{
								writer.WriteString(L"this->");
								writer.WriteString(config->ConvertName(symbol->name));
								return;
							}
						}
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
							writer.WriteString(L"(this, &");
							writer.WriteString(GetClosureInfo(classExpr.Obj())->lambdaClassName);
							writer.WriteString(L"::");
							writer.WriteString(config->ConvertName(symbol->name));
							writer.WriteString(L")");
							return;
						}
					}
					writer.WriteString(config->ConvertName(symbol->name));
				}

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
							return argumentsCallback(methodInfo, false);
						}
						else if (item == L", $Arguments")
						{
							return argumentsCallback(methodInfo, true);
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

				template<typename TMethodThis, typename TPropertyThis>
				bool WriteReferenceTemplate(ResolveExpressionResult& result, const TMethodThis& methodThis, const TPropertyThis& propertyThis)
				{
					if (result.methodInfo)
					{
						if (CppExists(result.methodInfo))
						{
							auto methodInfo = result.methodInfo;
							WriteMethodTemplate(CppGetClosureTemplate(methodInfo), methodInfo, methodThis,
								[&](IMethodInfo*, bool)
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
								WriteMethodTemplate(CppGetClosureTemplate(methodInfo), methodInfo, methodThis,
									[&](IMethodInfo*, bool)
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
							VisitThisExpression(node, methodInfo->GetOwnerTypeDescriptor());
							return true;
						},
						[&](IPropertyInfo* propertyInfo)
						{
							VisitThisExpression(node, propertyInfo->GetOwnerTypeDescriptor());
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
							Call(node->parent);
							return true;
						},
						[&](IPropertyInfo* propertyInfo)
						{
							Call(node->parent);
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
					writer.WriteString(L"L\"");
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
					writer.WriteString(L"\"");
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

				void Visit(WfBinaryExpression* node)override
				{
					throw 0;
				}

				void Visit(WfLetExpression* node)override
				{
					throw 0;
				}

				void Visit(WfIfExpression* node)override
				{
					throw 0;
				}

				void Visit(WfRangeExpression* node)override
				{
					throw 0;
				}

				void Visit(WfSetTestingExpression* node)override
				{
					throw 0;
				}

				void Visit(WfConstructorExpression* node)override
				{
					auto result = config->manager->expressionResolvings[node];
					auto td = result.type->GetTypeDescriptor();
					if (node->arguments.Count() == 0)
					{
						if ((td->GetTypeDescriptorFlags()&TypeDescriptorFlags::StructType)!=TypeDescriptorFlags::Undefined)
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
						throw 0;
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
					throw 0;
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
					throw 0;
				}

				void Visit(WfDetachEventExpression* node)override
				{
					throw 0;
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
					throw 0;
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
					throw 0;
				}

				void Visit(WfNewInterfaceExpression* node)override
				{
					throw 0;
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