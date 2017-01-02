#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;
			using namespace regex;
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

			WfCppConfig::WfCppConfig(analyzer::WfLexicalScopeManager* _manager)
				:manager(_manager)
				, regexSplitName(L"::")
				, regexSpecialName(L"/<(<category>/w+)/>(<name>/w*)")
				, assemblyNamespace(L"vl_workflow_global")
			{
				Collect();
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
				return From(matches)
					.Select([this](Ptr<RegexMatch> match)
					{
						return L"::" + ConvertName(match->Result().Value());
					})
					.Aggregate([](const WString& a, const WString& b)
					{
						return a + b;
					});
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
					writer.WriteLine(nss[i]);

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
				}
				nss.Clear();
			}

/***********************************************************************
WfCppConfig::WriteHeader
***********************************************************************/

			void WfCppConfig::WriteHeader_Global(stream::StreamWriter& writer, collections::List<WString>& nss)
			{
				WriteEnd(writer, nss);
				writer.WriteLine(L"namespace " + assemblyNamespace);
				writer.WriteLine(L"{");
				writer.WriteLine(L"\tclass " + assemblyName);
				writer.WriteLine(L"\t{");
				writer.WriteLine(L"\tpublic:");
				writer.WriteLine(L"");
				writer.WriteLine(L"\t\tstatic " + assemblyName + L"& Instance();");
				writer.WriteLine(L"\t};");
				writer.WriteLine(L"}");
			}

			void WfCppConfig::WriteHeader_Enum(stream::StreamWriter& writer, Ptr<WfEnumDeclaration> decl, const WString& name, const WString& prefix)
			{
				writer.WriteLine(prefix + L"enum class " + name + L" : vl::vuint64_t");
				writer.WriteLine(prefix + L"{");
				FOREACH(Ptr<WfEnumItem>, item, decl->items)
				{
					switch (item->kind)
					{
					case WfEnumItemKind::Constant:
						writer.WriteLine(prefix + L"\t" + item->name.value + L" = " + item->number.value + L"UL,");
						break;
					case WfEnumItemKind::Intersection:
						writer.WriteString(prefix + L"\t" + item->name.value + L" = ");
						FOREACH_INDEXER(Ptr<WfEnumItemIntersection>, enumInt, index, item->intersections)
						{
							if (index > 0)
							{
								writer.WriteString(L" | ");
							}
							writer.WriteString(enumInt->name.value);
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
			}

			void WfCppConfig::WriteHeader_Struct(stream::StreamWriter& writer, Ptr<WfStructDeclaration> decl, collections::List<WString>& nss)
			{
			}

			void WfCppConfig::WriteHeader_ClassPreDecls(stream::StreamWriter& writer, const WString& prefix)
			{
			}

			void WfCppConfig::WriteHeader_ClassPreDecls(stream::StreamWriter& writer, collections::List<WString>& nss)
			{
			}

			void WfCppConfig::WriteHeader_Class(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, const WString& name, const WString& prefix)
			{
			}

			void WfCppConfig::WriteHeader_Class(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, collections::List<WString>& nss)
			{
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
				writer.WriteLine(L"\tFINALIZE_GLOBAL_STORAGE_CLASS");
				writer.WriteLine(L"END_GLOBAL_STORAGE_CLASS(" + storageName + L")");
				writer.WriteLine(L"");

				writer.WriteLine(L"namespace vl_workflow_global");
				writer.WriteLine(L"{");
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