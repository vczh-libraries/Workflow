#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;

			void WfCppConfig::WritePushCompileOptions(stream::StreamWriter& writer)
			{
				writer.WriteLine(L"#if defined( _MSC_VER)");
				writer.WriteLine(L"#pragma warning(push)");
				writer.WriteLine(L"#pragma warning(disable:4250)");
				writer.WriteLine(L"#elif defined(__GNUC__)");
				writer.WriteLine(L"#pragma GCC diagnostic push");
				writer.WriteLine(L"#pragma GCC diagnostic ignored \"-Wparentheses-equality\"");
				writer.WriteLine(L"#elif defined(__clang__)");
				writer.WriteLine(L"#pragma clang diagnostic push");
				writer.WriteLine(L"#pragma clang diagnostic ignored \"-Wparentheses-equality\"");
				writer.WriteLine(L"#endif");
			}

			void WfCppConfig::WritePopCompileOptions(stream::StreamWriter& writer)
			{
				writer.WriteLine(L"#if defined( _MSC_VER)");
				writer.WriteLine(L"#pragma warning(pop)");
				writer.WriteLine(L"#elif defined(__GNUC__)");
				writer.WriteLine(L"#pragma GCC diagnostic pop");
				writer.WriteLine(L"#elif defined(__clang__)");
				writer.WriteLine(L"#pragma clang diagnostic pop");
				writer.WriteLine(L"#endif");
			}
			
			void WfCppConfig::WriteCpp_PushMacros(stream::StreamWriter& writer)
			{
				writer.WriteString(L"#define GLOBAL_SYMBOL ");
				writer.WriteString(L"::");
				writer.WriteString(assemblyNamespace);
				writer.WriteString(L"::");
				writer.WriteString(assemblyName);
				writer.WriteLine(L"::");

				writer.WriteString(L"#define GLOBAL_NAME ");
				writer.WriteString(L"::");
				writer.WriteString(assemblyNamespace);
				writer.WriteString(L"::");
				writer.WriteString(assemblyName);
				writer.WriteLine(L"::Instance().");

				writer.WriteString(L"#define GLOBAL_OBJ ");
				writer.WriteString(L"&::");
				writer.WriteString(assemblyNamespace);
				writer.WriteString(L"::");
				writer.WriteString(assemblyName);
				writer.WriteLine(L"::Instance()");

				writer.WriteLine(L"#define USERIMPL(...)");
			}

			void WfCppConfig::WriteCpp_PopMacros(stream::StreamWriter& writer)
			{
				writer.WriteLine(L"#undef GLOBAL_SYMBOL");
				writer.WriteLine(L"#undef GLOBAL_NAME");
				writer.WriteLine(L"#undef GLOBAL_OBJ");
				writer.WriteLine(L"#undef USERIMPL");
			}

			void WfCppConfig::WriteHeader(stream::StreamWriter& writer, bool multiFile)
			{
				WritePushCompileOptions(writer);
				writer.WriteLine(L"");

				if (closureInfos.Count() > 0)
				{
					Dictionary<WString, Ptr<WfExpression>> reversedClosures;
					CopyFrom(
						reversedClosures,
						From(closureInfos)
							.Select([](Pair<Ptr<WfExpression>, Ptr<ClosureInfo>> pair)
							{
								return Pair<WString, Ptr<WfExpression>>(pair.value->lambdaClassName, pair.key);
							})
						);

					writer.WriteString(L"namespace ");
					writer.WriteLine(assemblyNamespace);
					writer.WriteLine(L"{");
					FOREACH(Ptr<WfExpression>, closure, reversedClosures.Values())
					{
						WriteHeader_ClosurePreDecl(writer, closure);
					}
					writer.WriteLine(L"}");
					writer.WriteLine(L"");
				}

				List<WString> nss;

				if (enumDecls.Keys().Contains(nullptr))
				{
					FOREACH(Ptr<WfEnumDeclaration>, decl, enumDecls[nullptr])
					{
						WriteHeader_Enum(writer, decl, nss);
						writer.WriteLine(L"");
					}
				}

				if (structDecls.Keys().Contains(nullptr))
				{
					FOREACH(Ptr<WfStructDeclaration>, decl, structDecls[nullptr])
					{
						WriteHeader_Struct(writer, decl, nss);
						writer.WriteLine(L"");
					}
				}

				if (classDecls.Keys().Contains(nullptr))
				{
					FOREACH(Ptr<WfClassDeclaration>, decl, classDecls[nullptr])
					{
						WriteHeader_ClassPreDecl(writer, decl, nss);
					}
					writer.WriteLine(L"");
					if (multiFile)
					{
						vint index = topLevelClassDeclsForFiles.Keys().IndexOf(L"");
						if (index != -1)
						{
							FOREACH(Ptr<WfClassDeclaration>, decl, topLevelClassDeclsForFiles.GetByIndex(index))
							{
								WriteHeader_TopLevelClass(writer, decl, nss);
								writer.WriteLine(L"");
							}
						}
					}
					else
					{
						FOREACH(Ptr<WfClassDeclaration>, decl, classDecls[nullptr])
						{
							WriteHeader_TopLevelClass(writer, decl, nss);
							writer.WriteLine(L"");
						}
					}
				}
				WriteNamespaceEnd(writer, nss);

				WriteHeader_Global(writer);

				if (!multiFile)
				{
					if (manager->declarationTypes.Count() > 0)
					{
						writer.WriteLine(L"");
						WriteHeader_Reflection(writer);
					}
				}

				writer.WriteLine(L"");
				WritePopCompileOptions(writer);
			}

			void WfCppConfig::WriteCpp(stream::StreamWriter& writer, bool multiFile)
			{
				WritePushCompileOptions(writer);
				writer.WriteLine(L"");
				WriteCpp_PushMacros(writer);

				writer.WriteLine(L"");
				WriteCpp_Global(writer);
				writer.WriteLine(L"");

				if (classDecls.Keys().Contains(nullptr))
				{
					List<WString> nss;

					if (multiFile)
					{
						vint index = topLevelClassDeclsForFiles.Keys().IndexOf(L"");
						if (index != -1)
						{
							FOREACH(Ptr<WfClassDeclaration>, decl, topLevelClassDeclsForFiles.GetByIndex(index))
							{
								WriteCpp_Class(writer, decl, nss);
							}
						}
					}
					else
					{
						FOREACH(Ptr<WfClassDeclaration>, decl, classDecls[nullptr])
						{
							WriteCpp_Class(writer, decl, nss);
						}
					}

					WriteNamespaceEnd(writer, nss);
				}

				WriteCpp_PopMacros(writer);

				if (manager->declarationTypes.Count() > 0)
				{
					writer.WriteLine(L"");
					WriteCpp_Reflection(writer);
				}

				writer.WriteLine(L"");
				WritePopCompileOptions(writer);
			}

			void WfCppConfig::WriteSubHeader(stream::StreamWriter& writer, const WString& fileName)
			{
				WritePushCompileOptions(writer);
				writer.WriteLine(L"");
				List<WString> nss;

				FOREACH(Ptr<WfClassDeclaration>, decl, topLevelClassDeclsForFiles.Get(fileName))
				{
					WriteHeader_TopLevelClass(writer, decl, nss);
					writer.WriteLine(L"");
				}

				WriteNamespaceEnd(writer, nss);
				WritePopCompileOptions(writer);
			}

			void WfCppConfig::WriteSubCpp(stream::StreamWriter& writer, const WString& fileName)
			{
				WritePushCompileOptions(writer);
				writer.WriteLine(L"");
				WriteCpp_PushMacros(writer);
				writer.WriteLine(L"");
				List<WString> nss;

				FOREACH(Ptr<WfClassDeclaration>, decl, topLevelClassDeclsForFiles.Get(fileName))
				{
					WriteCpp_Class(writer, decl, nss);
				}

				WriteNamespaceEnd(writer, nss);
				WriteCpp_PopMacros(writer);
				writer.WriteLine(L"");
				WritePopCompileOptions(writer);
			}
		}
	}
}
