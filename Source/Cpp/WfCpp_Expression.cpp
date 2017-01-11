#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;
			using namespace reflection;
			using namespace reflection::description;
			using namespace analyzer;

			class WfGenerateExpressionVisitor : public Object, public WfExpression::IVisitor
			{
			public:
				WfCppConfig*				config;
				stream::StreamWriter&		writer;
				WString						prefix;

				WfGenerateExpressionVisitor(WfCppConfig* _config, stream::StreamWriter& _writer, const WString& _prefix)
					:config(_config)
					, writer(_writer)
					, prefix(_prefix)
				{
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

				void VisitThisExpression(WfExpression* node, ITypeDescriptor* td)
				{
					if (auto closureInfo = GetClosureInfo(node))
					{
						auto index = closureInfo->thisTypes.IndexOf(td);
						if (index != -1)
						{
							writer.WriteString(L"__vwsnthis_");
							writer.WriteString(itow(index));
							return;
						}
					}
					writer.WriteString(L"this");
				}

				void VisitReferenceExpression(WfExpression* node, const WString& name)
				{
					auto result = config->manager->expressionResolvings[node];
					if (result.symbol)
					{
						if (auto varDecl = result.symbol->creatorNode.Cast<WfVariableDeclaration>())
						{
							auto ownerNode = result.symbol->ownerScope->ownerNode;
							if (ownerNode.Cast<WfNamespaceDeclaration>() || ownerNode.Cast<WfModule>())
							{
								writer.WriteString(L"::");
								writer.WriteString(config->assemblyNamespace);
								writer.WriteString(L"::");
								writer.WriteString(config->assemblyName);
								writer.WriteString(L"::Instance().");
								writer.WriteString(config->ConvertName(result.symbol->name));
								return;
							}
							else
							{
								auto closureInfo = GetClosureInfo(node);
								if (closureInfo->symbols.Values().Contains(result.symbol.Obj()))
								{
									writer.WriteString(L"this->");
									writer.WriteString(config->ConvertName(result.symbol->name));
									return;
								}
							}
						}
						else if (auto funcDecl = result.symbol->creatorNode.Cast<WfFunctionDeclaration>())
						{
							auto ownerNode = result.symbol->ownerScope->ownerNode;
							if (ownerNode.Cast<WfNamespaceDeclaration>() || ownerNode.Cast<WfModule>())
							{
								return;
							}
							else if (auto classExpr = ownerNode.Cast<WfNewInterfaceExpression>())
							{
								return;
							}
						}
						writer.WriteString(config->ConvertName(result.symbol->name));
					}
					else if (result.methodInfo)
					{
					}
					else if (result.propertyInfo)
					{
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
					throw 0;
				}

				void Visit(WfMemberExpression* node)override
				{
					throw 0;
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
					throw 0;
				}

				void Visit(WfUnaryExpression* node)override
				{
					throw 0;
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
							writer.WriteString(config->ConvertFullName(CppGetFullName(td)) + L"::Create()");
						}
					}
					else
					{
						throw 0;
					}
				}

				void Visit(WfInferExpression* node)override
				{
					throw 0;
				}

				void Visit(WfTypeCastingExpression* node)override
				{
					throw 0;
				}

				void Visit(WfTypeTestingExpression* node)override
				{
					throw 0;
				}

				void Visit(WfTypeOfTypeExpression* node)override
				{
					throw 0;
				}

				void Visit(WfTypeOfExpressionExpression* node)override
				{
					throw 0;
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
					throw 0;
				}

				void Visit(WfObserveExpression* node)override
				{
					throw 0;
				}

				void Visit(WfCallExpression* node)override
				{
					throw 0;
				}

				void Visit(WfFunctionExpression* node)override
				{
					throw 0;
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

			void GenerateExpression(WfCppConfig* config, stream::StreamWriter& writer, Ptr<WfExpression> node, const WString& prefix)
			{
				WfGenerateExpressionVisitor visitor(config, writer, prefix);
				node->Accept(&visitor);
			}
		}
	}
}