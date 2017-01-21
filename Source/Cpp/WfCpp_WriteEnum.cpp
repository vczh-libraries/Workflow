#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;

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

			void WfCppConfig::WriteHeader_Enum(stream::StreamWriter& writer, Ptr<WfEnumDeclaration> decl, collections::List<WString>& nss)
			{
				auto td = manager->declarationTypes[decl.Obj()].Obj();
				WString name;
				auto prefix = WriteNamespace(writer, CppGetFullName(td), nss, name);
				WriteHeader_Enum(writer, decl, name, prefix);
				WriteHeader_EnumOp(writer, decl, name, prefix);
			}
		}
	}
}