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

			void WfCppConfig::WriteHeader_ClassPreDecl(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, const WString& name, const WString& prefix)
			{
				writer.WriteLine(prefix + L"class " + name + L";");
			}

			void WfCppConfig::WriteHeader_ClassPreDecl(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, collections::List<WString>& nss)
			{
				auto td = manager->declarationTypes[decl.Obj()].Obj();
				WString name;
				auto prefix = WriteNamespace(writer, CppGetFullName(td), nss, name);
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
						auto baseTd = td->GetBaseTypeDescriptor(index);
						return baseTd->GetTypeDescriptorFlags() == TypeDescriptorFlags::Class
							&& baseTd != description::GetTypeDescriptor<DescriptableObject>();
					});

					if (!hasClassBase)
					{
						writer.WriteString(L"public ::vl::Object, ");
					}
					for (vint i = 0; i < count; i++)
					{
						auto baseTd = td->GetBaseTypeDescriptor(i);
						switch (baseTd->GetTypeDescriptorFlags())
						{
						case TypeDescriptorFlags::Class:
							if (baseTd != description::GetTypeDescriptor<DescriptableObject>())
							{
								writer.WriteString(L"public " + ConvertType(baseTd) + L", ");
							}
							break;
						case TypeDescriptorFlags::Interface:
							writer.WriteString(L"public virtual " + ConvertType(baseTd) + L", ");
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
						writer.WriteString(L"public virtual " + ConvertType(td->GetBaseTypeDescriptor(i)) + L", ");
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
					vint index = structDecls.Keys().IndexOf(decl.Obj());
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
					vint index = classDecls.Keys().IndexOf(decl.Obj());
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

				writer.WriteLine(L"");
				FOREACH(Ptr<WfClassMember>, member, decl->members)
				{
					GenerateClassMemberDecl(this, writer, ConvertName(decl->name.value), member, prefix + L"\t", false);
				}

				writer.WriteLine(prefix + L"};");
			}

			void WfCppConfig::WriteHeader_Class(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, collections::List<WString>& nss)
			{
				auto td = manager->declarationTypes[decl.Obj()].Obj();
				WString name;
				auto prefix = WriteNamespace(writer, CppGetFullName(td), nss, name);
				WriteHeader_Class(writer, decl, name, prefix);
			}

			bool WfCppConfig::WriteCpp_ClassMember(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, Ptr<WfClassMember> member, collections::List<WString>& nss)
			{
				List<WString> nss2;
				GetClassNamespace(decl, nss2);
				auto prefix = WriteNamespace(writer, nss, nss2);
				return GenerateClassMemberImpl(this, writer, GetClassBaseName(decl), ConvertName(decl->name.value), member, prefix);
			}
		}
	}
}