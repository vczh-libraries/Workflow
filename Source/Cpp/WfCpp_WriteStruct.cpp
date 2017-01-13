#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;

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

				using opt = Tuple<const wchar_t*, const wchar_t*, const wchar_t*>;
				opt ops[] = {
					opt(L"==", L"false", L"true"),
					opt(L"!=", L"true", L"false")
					};
				for (auto op : ops)
				{
					writer.WriteString(prefix);
					writer.WriteString(L"inline bool operator");
					writer.WriteString(op.f0);
					writer.WriteString(L" (const ");
					writer.WriteString(name);
					writer.WriteString(L"& a, const ");
					writer.WriteString(name);
					writer.WriteLine(L"& b)");

					writer.WriteString(prefix);
					writer.WriteLine(L"{");

					FOREACH(Ptr<WfStructMember>, member, decl->members)
					{
						writer.WriteString(prefix);
						writer.WriteString(L"\tif (a.");
						writer.WriteString(ConvertName(member->name.value));
						writer.WriteString(L" != b.");
						writer.WriteString(ConvertName(member->name.value));
						writer.WriteString(L") return ");
						writer.WriteString(op.f1);
						writer.WriteLine(L";");
					}
					writer.WriteString(prefix);
					writer.WriteString(L"\treturn ");
					writer.WriteString(op.f2);
					writer.WriteLine(L";");

					writer.WriteString(prefix);
					writer.WriteLine(L"}");
				}
			}

			void WfCppConfig::WriteHeader_Struct(stream::StreamWriter& writer, Ptr<WfStructDeclaration> decl, collections::List<WString>& nss)
			{
				auto td = manager->declarationTypes[decl.Obj()].Obj();
				WString name;
				auto prefix = WriteNamespace(writer, CppGetFullName(td), nss, name);
				WriteHeader_Struct(writer, decl, name, prefix);
			}
		}
	}
}