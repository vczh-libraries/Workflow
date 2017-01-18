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

			void LoadTypes(WfCppConfig* config, List<ITypeDescriptor*>& tds)
			{
				CopyFrom(
					tds,
					From(config->manager->declarationTypes.Values())
						.OrderBy([](Ptr<ITypeDescriptor> a, Ptr<ITypeDescriptor> b)
						{
							return WString::Compare(a->GetTypeName(), b->GetTypeName());
						})
						.Select([](Ptr<ITypeDescriptor> td)
						{
							return td.Obj();
						})
					);
			}

			void WfCppConfig::WriteHeader_Reflection(stream::StreamWriter& writer)
			{
				List<ITypeDescriptor*> tds;
				LoadTypes(this, tds);

				writer.WriteLine(L"namespace vl");
				writer.WriteLine(L"{");
				writer.WriteLine(L"\tnamespace reflection");
				writer.WriteLine(L"\t{");
				writer.WriteLine(L"\t\tnamespace description");
				writer.WriteLine(L"\t\t{");
				writer.WriteLine(L"#ifndef VCZH_DEBUG_NO_REFLECTION");

				FOREACH(ITypeDescriptor*, td, tds)
				{
					writer.WriteString(L"\t\t\tDECL_TYPE_INFO(");
					writer.WriteString(ConvertType(td));
					writer.WriteLine(L")");
				}

				writer.WriteLine(L"#endif");
				writer.WriteLine(L"");

				writer.WriteString(L"\t\t\textern bool Load");
				writer.WriteString(assemblyName);
				writer.WriteLine(L"Types();");
				writer.WriteLine(L"\t\t}");
				writer.WriteLine(L"\t}");
				writer.WriteLine(L"}");
			}

			void WfCppConfig::WriteCpp_Reflection(stream::StreamWriter& writer)
			{
				List<ITypeDescriptor*> tds;
				LoadTypes(this, tds);

				writer.WriteLine(L"namespace vl");
				writer.WriteLine(L"{");
				writer.WriteLine(L"\tnamespace reflection");
				writer.WriteLine(L"\t{");
				writer.WriteLine(L"\t\tnamespace description");
				writer.WriteLine(L"\t\t{");
				writer.WriteLine(L"#ifndef VCZH_DEBUG_NO_REFLECTION");

				FOREACH(ITypeDescriptor*, td, tds)
				{
					writer.WriteString(L"\t\t\tIMPL_CPP_TYPE_INFO(");
					writer.WriteString(ConvertType(td));
					writer.WriteLine(L")");
				}
				writer.WriteLine(L"");

				FOREACH(ITypeDescriptor*, td, tds)
				{
					writer.WriteLine(L"");
				}

				writer.WriteString(L"\t\t\tclass ");
				writer.WriteString(assemblyName);
				writer.WriteLine(L"TypeLoader : public Object, public ITypeLoader");
				writer.WriteLine(L"\t\t\t{");
				writer.WriteLine(L"\t\t\tpublic:");
				writer.WriteLine(L"\t\t\t\tvoid Load(ITypeManager* manager)");
				writer.WriteLine(L"\t\t\t\t{");

				FOREACH(ITypeDescriptor*, td, tds)
				{
					writer.WriteString(L"\t\t\t\t\tADD_TYPE_INFO(");
					writer.WriteString(ConvertType(td));
					writer.WriteLine(L")");
				}
				writer.WriteLine(L"\t\t\t\t}");
				writer.WriteLine(L"");
				writer.WriteLine(L"\t\t\t\tvoid Unload(ITypeManager* manager)");
				writer.WriteLine(L"\t\t\t\t{");
				writer.WriteLine(L"\t\t\t\t}");
				writer.WriteLine(L"\t\t\t};");

				writer.WriteLine(L"#endif");
				writer.WriteLine(L"");

				writer.WriteString(L"\t\t\tbool Load");
				writer.WriteString(assemblyName);
				writer.WriteLine(L"Types()");
				writer.WriteLine(L"\t\t\t{");
				writer.WriteLine(L"#ifndef VCZH_DEBUG_NO_REFLECTION");
				writer.WriteLine(L"\t\t\t\tif (auto manager = GetGlobalTypeManager())");
				writer.WriteLine(L"\t\t\t\t{");
				writer.WriteString(L"\t\t\t\t\treturn manager->AddTypeLoader(MakePtr<");
				writer.WriteString(assemblyName);
				writer.WriteLine(L"TypeLoader>());");
				writer.WriteLine(L"\t\t\t\t}");
				writer.WriteLine(L"#endif");
				writer.WriteLine(L"\t\t\t\treturn false;");
				writer.WriteLine(L"\t\t\t}");

				writer.WriteLine(L"\t\t}");
				writer.WriteLine(L"\t}");
				writer.WriteLine(L"}");
			}
		}
	}
}