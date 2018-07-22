#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;

			void WfCppConfig::WriteHeader_Struct(stream::StreamWriter& writer, Ptr<WfStructDeclaration> decl, const WString& name, const WString& prefix, bool mainHeaderDefinition)
			{
				if (mainHeaderDefinition)
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
						writer.WriteLine(prefix + L"\t" + ConvertType(prop->GetReturn(), true) + L" " + ConvertName(member->name.value) + defaultValue + L";");
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
					writer.WriteString(CppNameToHeaderEnumStructName(CppGetFullName(td), L"struct"));
					writer.WriteLine(L";");
				}
			}

			void WfCppConfig::WriteHeader_StructOp(stream::StreamWriter& writer, Ptr<WfStructDeclaration> decl, const WString& name, const WString& prefix)
			{
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

			void WfCppConfig::WriteHeader_Struct(stream::StreamWriter& writer, Ptr<WfStructDeclaration> decl, collections::List<WString>& nss, bool mainHeaderDefinition)
			{
				auto td = manager->declarationTypes[decl.Obj()].Obj();
				if (mainHeaderDefinition)
				{
					WString name;
					auto prefix = WriteNamespace(writer, CppNameToHeaderEnumStructName(CppGetFullName(td), L"struct"), nss, name);
					WriteHeader_Struct(writer, decl, name, prefix, true);
					WriteHeader_StructOp(writer, decl, name, prefix);
				}
				else
				{
					WString name;
					auto prefix = WriteNamespace(writer, CppGetFullName(td), nss, name);
					writer.WriteString(prefix);
					writer.WriteString(L"using ");
					writer.WriteString(name);
					writer.WriteString(L" = ::");
					writer.WriteString(CppNameToHeaderEnumStructName(CppGetFullName(td), L"struct"));
					writer.WriteLine(L";");
				}
			}

			void WfCppConfig::WriteHeader_MainHeaderStructs(stream::StreamWriter& writer, collections::List<WString>& nss)
			{
				List<Ptr<WfStructDeclaration>> allStructs;
				CopyFrom(allStructs, Range<vint>(0, structDecls.Count()).SelectMany([&](vint index) {return From(structDecls.GetByIndex(index)); }));
				SortDeclsByName(allStructs);

				Group<Ptr<WfStructDeclaration>, Ptr<WfStructDeclaration>> depGroup;
				FOREACH(Ptr<WfStructDeclaration>, decl, allStructs)
				{
					auto td = manager->declarationTypes[decl.Obj()].Obj();
					vint count = td->GetPropertyCount();
					for (vint i = 0; i < count; i++)
					{
						auto propType = td->GetProperty(i)->GetReturn();
						if (propType->GetDecorator() == ITypeInfo::TypeDescriptor)
						{
							auto propTd = propType->GetTypeDescriptor();
							vint index = tdDecls.Keys().IndexOf(propTd);
							if (index != -1)
							{
								depGroup.Add(decl, tdDecls.Values()[index].Cast<WfStructDeclaration>());
							}
						}
					}
				}

				PartialOrderingProcessor pop;
				pop.InitWithGroup(allStructs, depGroup);
				pop.Sort();

				for (vint i = 0; i < pop.components.Count(); i++)
				{
					auto& component = pop.components[i];
					CHECK_ERROR(component.nodeCount == 1, L"WfCppConfig::WriteHeader_MainHeaderStructs(StreamWriter&, List<WString>&)#Internal error: Unexpected circle dependency found, which should be cought by the Workflow semantic analyzer.");
					auto decl = allStructs[component.firstNode[0]];
					WriteHeader_Struct(writer, decl, nss, true);
					writer.WriteLine(L"");
				}
			}
		}
	}
}