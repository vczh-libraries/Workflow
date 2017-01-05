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