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

/***********************************************************************
WfCppConfig
***********************************************************************/

			void WfCppConfig::Collect()
			{
				FOREACH(Ptr<WfModule>, module, manager->GetModules())
				{
					FOREACH(Ptr<WfDeclaration>, decl, module->declarations)
					{
						CollectDeclaration(this, decl, nullptr);
					}
				}
			}

			void WfCppConfig::Sort(collections::List<Ptr<WfStructDeclaration>>& structDecls)
			{
				List<ITypeDescriptor*> tds;
				FOREACH_INDEXER(Ptr<WfStructDeclaration>, decl, index, structDecls)
				{
					tds.Add(manager->declarationTypes[decl.Obj()].Obj());
				}

				for (vint i = 0; i < tds.Count(); i++)
				{
					for (vint j = i; i < tds.Count(); j++)
					{
						auto td = tds[j];
						vint count = td->GetPropertyCount();
						bool found = false;
						for (vint k = 0; k < count && !found; k++)
						{
							auto prop = td->GetProperty(k);
							auto propTd = prop->GetReturn()->GetTypeDescriptor();
							for (vint l = k + 1; l < tds.Count() && !found; l++)
							{
								found = tds[l] == propTd;
							}
						}

						if (!found)
						{
							if (j != i)
							{
								auto t = tds[j];
								tds.RemoveAt(j);
								tds.Insert(i, t);

								auto decl = structDecls[j];
								structDecls.RemoveAt(j);
								structDecls.Insert(i, decl);
							}

							break;
						}
					}
				}
			}

			WfCppConfig::WfCppConfig(analyzer::WfLexicalScopeManager* _manager)
				:manager(_manager)
				, regexSplitName(L"::")
				, regexSpecialName(L"/<(<category>/w+)/>(<name>/w*)")
				, assemblyNamespace(L"vl_workflow_global")
			{
				Collect();
				for (vint i = 0; i < structDecls.Count(); i++)
				{
					const auto& values = structDecls.GetByIndex(i);
					Sort(const_cast<List<Ptr<WfStructDeclaration>>&>(values));
				}
			}

			WfCppConfig::~WfCppConfig()
			{
			}

			WString WfCppConfig::ConvertName(const WString& name)
			{
				auto match = regexSpecialName.MatchHead(name);
				if (match)
				{
					return L"__vwsn_" + match->Groups()[L"category"][0].Value() + L"_" + match->Groups()[L"category"][0].Value();
				}
				else
				{
					return name;
				}
			}

			WString WfCppConfig::ConvertFullName(const WString& fullName)
			{
				List<Ptr<RegexMatch>> matches;
				regexSplitName.Split(fullName, false, matches);
				return WString(fullName[0] == L':' ? L"::" : L"") + From(matches)
					.Select([this](Ptr<RegexMatch> match)
					{
						return ConvertName(match->Result().Value());
					})
					.Aggregate([](const WString& a, const WString& b)
					{
						return a + L"::" + b;
					});
			}

			WString WfCppConfig::ConvertType(ITypeInfo* typeInfo)
			{
				switch (typeInfo->GetDecorator())
				{
				case ITypeInfo::RawPtr:
					return ConvertType(typeInfo->GetElementType()) + L"*";
				case ITypeInfo::SharedPtr:
					if (typeInfo->GetTypeDescriptor() == description::GetTypeDescriptor<IValueFunctionProxy>())
					{
						return ConvertType(typeInfo->GetElementType());
					}
					else if (typeInfo->GetTypeDescriptor() == description::GetTypeDescriptor<IValueEnumerable>())
					{
						return ConvertType(typeInfo->GetElementType());
					}
					else
					{
						return L"::vl::Ptr<" + ConvertType(typeInfo->GetElementType()) + L">";
					}
				case ITypeInfo::Nullable:
					return L"::vl::Nullable<" + ConvertType(typeInfo->GetElementType()) + L">";
				case ITypeInfo::Generic:
					if (typeInfo->GetTypeDescriptor() == description::GetTypeDescriptor<IValueFunctionProxy>())
					{
						WString type = L"::vl::Func<" + ConvertType(typeInfo->GetGenericArgument(0)) + L"(";
						vint count = typeInfo->GetGenericArgumentCount();
						for (vint i = 1; i < count; i++)
						{
							if (i > 1) type += L", ";
							type += ConvertType(typeInfo->GetGenericArgument(i));
						}
						type += L")>";
						return type;
					}
					else if(typeInfo->GetTypeDescriptor() == description::GetTypeDescriptor<IValueEnumerable>())
					{
						return L"::vl::collections::LazyList<" + ConvertType(typeInfo->GetGenericArgument(0)) + L">";
					}
					else
					{
						return ConvertType(typeInfo->GetElementType());
					}
				}
				return ConvertFullName(CppGetFullName(typeInfo->GetTypeDescriptor()));
			}

			WString WfCppConfig::DefaultValue(ITypeInfo* typeInfo)
			{
				switch (typeInfo->GetDecorator())
				{
				case ITypeInfo::RawPtr:
					return L"nullptr";
				case ITypeInfo::SharedPtr:
					return L"";
				case ITypeInfo::Nullable:
					return L"";
				case ITypeInfo::Generic:
					return L"";
				}
				auto td = typeInfo->GetTypeDescriptor();
				if ((td->GetTypeDescriptorFlags()&TypeDescriptorFlags::EnumType) != TypeDescriptorFlags::Undefined)
				{
					return L"static_cast<" + ConvertType(typeInfo) + L">(0)";
				}
				if (td == description::GetTypeDescriptor<vint8_t>()) return L"0";
				if (td == description::GetTypeDescriptor<vint16_t>()) return L"0";
				if (td == description::GetTypeDescriptor<vint32_t>()) return L"0";
				if (td == description::GetTypeDescriptor<vint64_t>()) return L"0";
				if (td == description::GetTypeDescriptor<vuint8_t>()) return L"0";
				if (td == description::GetTypeDescriptor<vuint16_t>()) return L"0";
				if (td == description::GetTypeDescriptor<vuint32_t>()) return L"0";
				if (td == description::GetTypeDescriptor<vuint64_t>()) return L"0";
				if (td == description::GetTypeDescriptor<bool>()) return L"false";
				if (td == description::GetTypeDescriptor<float>()) return L"0.0f";
				if (td == description::GetTypeDescriptor<double>()) return L"0.0";
				return L"";
			}

			WString WfCppConfig::WriteName(stream::StreamWriter& writer, const WString& fullName, collections::List<WString>& nss, WString& name)
			{
				List<Ptr<RegexMatch>> matches;
				regexSplitName.Split(fullName, false, matches);

				List<WString> nss2;
				CopyFrom(
					nss2,
					From(matches)
						.Select([this](Ptr<RegexMatch> match)
						{
							return ConvertName(match->Result().Value());
						})
					);

				vint commonPrefix = 0;
				for (vint i = 0; i < nss.Count() && i < nss2.Count() - 1; i++)
				{
					if (nss[i] == nss2[i])
					{
						commonPrefix++;
					}
					else
					{
						break;
					}
				}

				while (nss.Count() > commonPrefix)
				{
					for (vint i = 1; i < nss.Count(); i++)
					{
						writer.WriteChar(L'\t');
					}
					writer.WriteLine(L"}");
					nss.RemoveAt(nss.Count() - 1);
				}

				WString prefix;
				for (vint i = 0; i < nss.Count(); i++)
				{
					prefix += L'\t';
				}

				for (vint i = commonPrefix; i < nss2.Count() - 1; i++)
				{
					writer.WriteString(prefix);
					writer.WriteString(L"namespace ");
					writer.WriteLine(nss2[i]);

					writer.WriteString(prefix);
					writer.WriteLine(L"{");

					nss.Add(nss2[i]);
					prefix += L'\t';
				}

				name = nss2[nss.Count()];
				return prefix;
			}

			void WfCppConfig::WriteEnd(stream::StreamWriter& writer, collections::List<WString>& nss)
			{
				while (nss.Count() > 0)
				{
					for (vint i = 1; i < nss.Count(); i++)
					{
						writer.WriteChar(L'\t');
					}
					writer.WriteLine(L"}");
					nss.RemoveAt(0);
				}
			}

			void WfCppConfig::WriteFunctionHeader(stream::StreamWriter& writer, Ptr<WfFunctionDeclaration> decl, ITypeInfo* typeInfo, const WString& name)
			{
				writer.WriteString(ConvertType(typeInfo->GetElementType()->GetGenericArgument(0)));
				writer.WriteChar(L' ');
				writer.WriteString(name);
				writer.WriteString(L"(");
				for (vint i = 0; i < decl->arguments.Count(); i++)
				{
					if (i > 0)
					{
						writer.WriteString(L", ");
					}
					writer.WriteString(ConvertType(typeInfo->GetElementType()->GetGenericArgument(i + 1)));
					writer.WriteChar(L' ');
					writer.WriteString(decl->arguments[i]->name.value);
				}
				writer.WriteString(L")");
			}

/***********************************************************************
WfCppConfig::WriteHeader
***********************************************************************/

			void WfCppConfig::WriteHeader_Enum(stream::StreamWriter& writer, Ptr<WfEnumDeclaration> decl, const WString& name, const WString& prefix)
			{
				writer.WriteLine(prefix + L"enum class " + name + L" : vl::vuint64_t");
				writer.WriteLine(prefix + L"{");
				FOREACH(Ptr<WfEnumItem>, item, decl->items)
				{
					switch (item->kind)
					{
					case WfEnumItemKind::Constant:
						writer.WriteLine(prefix + L"\t" + ConvertName(item->name.value) + L" = " + item->number.value + L"UL,");
						break;
					case WfEnumItemKind::Intersection:
						writer.WriteString(prefix + L"\t" + ConvertName(item->name.value) + L" = ");
						FOREACH_INDEXER(Ptr<WfEnumItemIntersection>, enumInt, index, item->intersections)
						{
							if (index > 0)
							{
								writer.WriteString(L" | ");
							}
							writer.WriteString(ConvertName(enumInt->name.value));
						}
						writer.WriteLine(L",");
						break;
					}
				}
				writer.WriteLine(prefix + L"};");
			}

			void WfCppConfig::WriteHeader_Enum(stream::StreamWriter& writer, Ptr<WfEnumDeclaration> decl, collections::List<WString>& nss)
			{
				auto td = manager->declarationTypes[decl.Obj()].Obj();
				WString name;
				auto prefix = WriteName(writer, CppGetFullName(td), nss, name);
				WriteHeader_Enum(writer, decl, name, prefix);
			}

			void WfCppConfig::WriteHeader_Struct(stream::StreamWriter& writer, Ptr<WfStructDeclaration> decl, const WString& name, const WString& prefix)
			{
				auto td = manager->declarationTypes[decl.Obj()].Obj();
				writer.WriteLine(prefix + L"struct " + name);
				writer.WriteLine(prefix + L"{");
				FOREACH(Ptr<WfStructMember>, member, decl->members)
				{
					auto prop = td->GetPropertyByName(member->name.value, false);
					auto defaultValue = DefaultValue(prop->GetReturn());
					if (defaultValue != L"")
					{
						defaultValue = L" = " + defaultValue;
					}
					writer.WriteLine(prefix + L"\t" + ConvertType(prop->GetReturn()) + L" " + ConvertName(member->name.value) + defaultValue + L";");
				}
				writer.WriteLine(prefix + L"};");
			}

			void WfCppConfig::WriteHeader_Struct(stream::StreamWriter& writer, Ptr<WfStructDeclaration> decl, collections::List<WString>& nss)
			{
				auto td = manager->declarationTypes[decl.Obj()].Obj();
				WString name;
				auto prefix = WriteName(writer, CppGetFullName(td), nss, name);
				WriteHeader_Struct(writer, decl, name, prefix);
			}

			void WfCppConfig::WriteHeader_ClassPreDecl(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, const WString& name, const WString& prefix)
			{
				writer.WriteLine(prefix + L"class " + name + L";");
			}

			void WfCppConfig::WriteHeader_ClassPreDecl(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, collections::List<WString>& nss)
			{
				auto td = manager->declarationTypes[decl.Obj()].Obj();
				WString name;
				auto prefix = WriteName(writer, CppGetFullName(td), nss, name);
				WriteHeader_ClassPreDecl(writer, decl, name, prefix);
			}

			void WfCppConfig::WriteHeader_Class(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, const WString& name, const WString& prefix)
			{
				auto td = manager->declarationTypes[decl.Obj()].Obj();

				writer.WriteString(prefix + L"class " + name + L" : ");
				switch (decl->kind)
				{
				case WfClassKind::Class:
					{
						vint count = td->GetBaseTypeDescriptorCount();
						bool hasClassBase = Range(0, count)
							.Any([=](vint index)
							{
								return td->GetBaseTypeDescriptor(index)->GetTypeDescriptorFlags() == TypeDescriptorFlags::Class
									&& td != description::GetTypeDescriptor<DescriptableObject>();
							});

						if (!hasClassBase)
						{
							writer.WriteString(L"public virtual ::vl::Object, ");
						}
						for (vint i = 0; i < count; i++)
						{
							auto baseTd = td->GetBaseTypeDescriptor(i);
							switch (baseTd->GetTypeDescriptorFlags())
							{
							case TypeDescriptorFlags::Class:
								if (baseTd != description::GetTypeDescriptor<DescriptableObject>())
								{
									writer.WriteString(L"public " + ConvertFullName(CppGetFullName(baseTd)) + L", ");
								}
								break;
							case TypeDescriptorFlags::Interface:
								writer.WriteString(L"public virtual " + ConvertFullName(CppGetFullName(baseTd)) + L", ");
								break;
							}
						}
					}
					break;
				case WfClassKind::Interface:
					{
						vint count = td->GetBaseTypeDescriptorCount();
						for (vint i = 0; i < count; i++)
						{
							writer.WriteString(L"public virtual " + ConvertFullName(CppGetFullName(td->GetBaseTypeDescriptor(i))) + L", ");
						}
					}
					break;
				}
				writer.WriteLine(L"public ::vl::reflection::Description<" + name + L">");
				writer.WriteLine(prefix + L"{");
				writer.WriteLine(prefix + L"public:");

				{
					vint index = enumDecls.Keys().IndexOf(decl.Obj());
					if (index != -1)
					{
						FOREACH(Ptr<WfEnumDeclaration>, decl, enumDecls.GetByIndex(index))
						{
							WriteHeader_Enum(writer, decl, ConvertName(decl->name.value), prefix + L"\t");
							writer.WriteLine(L"");
						}
					}
				}

				{
					vint index = enumDecls.Keys().IndexOf(decl.Obj());
					if (index != -1)
					{
						FOREACH(Ptr<WfStructDeclaration>, decl, structDecls.GetByIndex(index))
						{
							WriteHeader_Struct(writer, decl, ConvertName(decl->name.value), prefix + L"\t");
							writer.WriteLine(L"");
						}
					}
				}

				{
					vint index = enumDecls.Keys().IndexOf(decl.Obj());
					if (index != -1)
					{
						FOREACH(Ptr<WfClassDeclaration>, decl, classDecls.GetByIndex(index))
						{
							WriteHeader_ClassPreDecl(writer, decl, ConvertName(decl->name.value), prefix + L"\t");
						}
						FOREACH(Ptr<WfClassDeclaration>, decl, classDecls.GetByIndex(index))
						{
							writer.WriteLine(L"");
							WriteHeader_Class(writer, decl, ConvertName(decl->name.value), prefix + L"\t");
						}
					}
				}

				writer.WriteLine(prefix + L"};");
			}

			void WfCppConfig::WriteHeader_Class(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, collections::List<WString>& nss)
			{
				auto td = manager->declarationTypes[decl.Obj()].Obj();
				WString name;
				auto prefix = WriteName(writer, CppGetFullName(td), nss, name);
				WriteHeader_Class(writer, decl, name, prefix);
			}

			void WfCppConfig::WriteHeader_Global(stream::StreamWriter& writer)
			{
				writer.WriteLine(L"namespace " + assemblyNamespace);
				writer.WriteLine(L"{");
				writer.WriteLine(L"\tclass " + assemblyName);
				writer.WriteLine(L"\t{");
				writer.WriteLine(L"\tpublic:");
				if (varDecls.Count() > 0)
				{
					writer.WriteLine(L"");
					FOREACH(Ptr<WfVariableDeclaration>, decl, varDecls)
					{
						auto scope = manager->nodeScopes[decl.Obj()].Obj();
						auto symbol = scope->symbols[decl->name.value][0];
						auto typeInfo = symbol->typeInfo;
						writer.WriteString(L"\t\t" + ConvertType(typeInfo.Obj()) + L" " + ConvertName(decl->name.value));
						auto defaultValue = DefaultValue(typeInfo.Obj());
						if (defaultValue != L"")
						{
							writer.WriteString(L" = " + defaultValue);
						}
						writer.WriteLine(L";");
					}
				}
				if (funcDecls.Count() > 0)
				{
					writer.WriteLine(L"");
					FOREACH(Ptr<WfFunctionDeclaration>, decl, funcDecls)
					{
						auto scope = manager->nodeScopes[decl.Obj()].Obj();
						auto symbol = manager->GetDeclarationSymbol(scope, decl.Obj());
						writer.WriteString(L"\t\t");
						WriteFunctionHeader(writer, decl, symbol->typeInfo.Obj(), ConvertName(decl->name.value));
						writer.WriteLine(L";");
					}
				}
				writer.WriteLine(L"");
				writer.WriteLine(L"\t\tstatic " + assemblyName + L"& Instance();");
				writer.WriteLine(L"\t};");
				writer.WriteLine(L"}");
			}

/***********************************************************************
WfCppConfig::WriteCpp
***********************************************************************/

			void WfCppConfig::WriteCpp_Global(stream::StreamWriter& writer)
			{
				WString storageName = assemblyNamespace + L"_" + assemblyName;
				writer.WriteLine(L"BEGIN_GLOBAL_STORAGE_CLASS(" + storageName + L")");
				writer.WriteLine(L"\t" + assemblyNamespace + L"::" + assemblyName + L" instance;");
				writer.WriteLine(L"\tINITIALIZE_GLOBAL_STORAGE_CLASS");
				if (varDecls.Count() > 0)
				{
					writer.WriteLine(L"");
					FOREACH(Ptr<WfVariableDeclaration>, decl, varDecls)
					{
						auto scope = manager->nodeScopes[decl.Obj()].Obj();
						auto symbol = scope->symbols[decl->name.value][0];
						auto typeInfo = symbol->typeInfo;
						if (decl->expression)
						{
							writer.WriteString(L"\t\tinstance." + ConvertName(decl->name.value) + L" = ");
							GenerateExpression(this, writer, decl->expression, L"\t\t");
							writer.WriteLine(L";");
						}
					}
				}
				writer.WriteLine(L"\tFINALIZE_GLOBAL_STORAGE_CLASS");
				writer.WriteLine(L"END_GLOBAL_STORAGE_CLASS(" + storageName + L")");
				writer.WriteLine(L"");

				writer.WriteLine(L"namespace vl_workflow_global");
				writer.WriteLine(L"{");
				FOREACH(Ptr<WfFunctionDeclaration>, decl, funcDecls)
				{
					auto scope = manager->nodeScopes[decl.Obj()].Obj();
					auto symbol = manager->GetDeclarationSymbol(scope, decl.Obj());
					writer.WriteString(L"\t");
					WriteFunctionHeader(writer, decl, symbol->typeInfo.Obj(), assemblyName + L"::" + ConvertName(decl->name.value));
					writer.WriteLine(L"");
					writer.WriteLine(L"\t{");
					writer.WriteLine(L"\t\tthrow 0;");
					writer.WriteLine(L"\t}");
					writer.WriteLine(L"");
				}
				writer.WriteLine(L"\t" + assemblyName + L"& " + assemblyName + L"::Instance()");
				writer.WriteLine(L"\t{");
				writer.WriteLine(L"\t\treturn Get" + storageName + L"().instance;");
				writer.WriteLine(L"\t}");
				writer.WriteLine(L"}");
			}

			void WfCppConfig::WriteCpp_LambdaExprDecl(stream::StreamWriter& writer, Ptr<WfExpression> lambda, collections::List<WString>& nss)
			{
			}

			void WfCppConfig::WriteCpp_LambdaExprImpl(stream::StreamWriter& writer, Ptr<WfExpression> lambda, collections::List<WString>& nss)
			{
			}

			void WfCppConfig::WriteCpp_ClassExprDecl(stream::StreamWriter& writer, Ptr<WfNewInterfaceExpression> lambda, collections::List<WString>& nss)
			{
			}

			void WfCppConfig::WriteCpp_ClassExprImpl(stream::StreamWriter& writer, Ptr<WfNewInterfaceExpression> lambda, collections::List<WString>& nss)
			{
			}

			void WfCppConfig::GetClassMembers(Ptr<WfClassDeclaration> decl, collections::List<Ptr<WfClassMember>>& members)
			{
			}

			void WfCppConfig::WriteCpp_ClassMember(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, Ptr<WfClassMember> member, collections::List<WString>& nss)
			{
			}
		}
	}
}