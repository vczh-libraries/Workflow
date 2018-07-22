#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;

			void WfCppConfig::WriteHeader_Enum(stream::StreamWriter& writer, Ptr<WfEnumDeclaration> decl, const WString& name, const WString& prefix, bool mainHeaderDefinition)
			{
				if (mainHeaderDefinition)
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
				else
				{
					auto td = manager->declarationTypes[decl.Obj()].Obj();
					writer.WriteString(prefix);
					writer.WriteString(L"using ");
					writer.WriteString(name);
					writer.WriteString(L" = ::");
					writer.WriteString(CppNameToHeaderEnumStructName(CppGetFullName(td), L"enum"));
					writer.WriteLine(L";");
				}
			}

			void WfCppConfig::WriteHeader_EnumOp(stream::StreamWriter& writer, Ptr<WfEnumDeclaration> decl, const WString& name, const WString& prefix)
			{
				const wchar_t* ops[] = { L"&", L"|" };
				for (auto op : ops)
				{
					writer.WriteString(prefix);
					writer.WriteString(L"inline ");
					writer.WriteString(name);
					writer.WriteString(L" operator");
					writer.WriteString(op);
					writer.WriteString(L" (");
					writer.WriteString(name);
					writer.WriteString(L" a, ");
					writer.WriteString(name);
					writer.WriteString(L" b) { return static_cast<");
					writer.WriteString(name);
					writer.WriteString(L">(static_cast<::vl::vuint64_t>(a) ");
					writer.WriteString(op);
					writer.WriteLine(L" static_cast<::vl::vuint64_t>(b)); }");
				}
			}

			void WfCppConfig::WriteHeader_Enum(stream::StreamWriter& writer, Ptr<WfEnumDeclaration> decl, collections::List<WString>& nss, bool mainHeaderDefinition)
			{
				auto td = manager->declarationTypes[decl.Obj()].Obj();
				if (mainHeaderDefinition)
				{
					WString name;
					auto prefix = WriteNamespace(writer, CppNameToHeaderEnumStructName(CppGetFullName(td), L"enum"), nss, name);
					WriteHeader_Enum(writer, decl, name, prefix, true);
					WriteHeader_EnumOp(writer, decl, name, prefix);
				}
				else
				{
					WString name;
					auto prefix = WriteNamespace(writer, CppGetFullName(td), nss, name);
					writer.WriteString(prefix);
					writer.WriteString(L"using ");
					writer.WriteString(name);
					writer.WriteString(L" = ::");
					writer.WriteString(CppNameToHeaderEnumStructName(CppGetFullName(td), L"enum"));
					writer.WriteLine(L";");
				}
			}

			void WfCppConfig::WriteHeader_MainHeaderEnums(stream::StreamWriter& writer, collections::List<WString>& nss)
			{
				List<Ptr<WfEnumDeclaration>> allEnums;
				CopyFrom(allEnums, Range<vint>(0, enumDecls.Count()).SelectMany([&](vint index) {return From(enumDecls.GetByIndex(index)); }));
				Sort<Ptr<WfEnumDeclaration>>(&allEnums[0], allEnums.Count(), [=](Ptr<WfEnumDeclaration> a, Ptr<WfEnumDeclaration> b)
				{
					auto tdA = manager->declarationTypes[a.Obj()].Obj();
					auto tdB = manager->declarationTypes[b.Obj()].Obj();
					return WString::Compare(tdA->GetTypeName(), tdB->GetTypeName());
				});

				FOREACH(Ptr<WfEnumDeclaration>, decl, allEnums)
				{
					WriteHeader_Enum(writer, decl, nss, true);
					writer.WriteLine(L"");
				}
			}
		}
	}
}