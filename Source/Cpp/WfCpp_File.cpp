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
			}

			void WfCppConfig::WriteCpp_PopMacros(stream::StreamWriter& writer)
			{
				writer.WriteLine(L"#undef GLOBAL_SYMBOL");
				writer.WriteLine(L"#undef GLOBAL_NAME");
				writer.WriteLine(L"#undef GLOBAL_OBJ");
			}

			void WfCppConfig::WriteHeader(stream::StreamWriter& writer, bool multiFile)
			{
				WritePushCompileOptions(writer);
				writer.WriteLine(L"");

				{
					Dictionary<WString, Ptr<WfExpression>> reversedLambdaExprs;
					Dictionary<WString, Ptr<WfNewInterfaceExpression>> reversedClassExprs;
					SortClosure(reversedLambdaExprs, reversedClassExprs);
					
					if (reversedLambdaExprs.Count() + reversedClassExprs.Count() > 0)
					{
						writer.WriteString(L"namespace ");
						writer.WriteLine(assemblyNamespace);
						writer.WriteLine(L"{");

						for (auto expr : reversedLambdaExprs.Values())
						{
							WriteHeader_ClosurePreDecl(writer, expr);
						}

						for (auto expr : reversedClassExprs.Values())
						{
							WriteHeader_ClosurePreDecl(writer, expr);
						}

						writer.WriteLine(L"}");
						writer.WriteLine(L"");
					}
				}

				List<WString> nss;

				if (enumDecls.Count() > 0)
				{
					WriteHeader_MainHeaderEnums(writer, nss);
					if (enumDecls.Keys().Contains(nullptr))
					{
						for (auto decl : enumDecls[nullptr])
						{
							WriteHeader_Enum(writer, decl, nss, false);
						}
						writer.WriteLine(L"");
					}
				}

				if (structDecls.Count() > 0)
				{
					WriteHeader_MainHeaderStructs(writer, nss);
					if (structDecls.Keys().Contains(nullptr))
					{
						for (auto decl : structDecls[nullptr])
						{
							WriteHeader_Struct(writer, decl, nss, false);
						}
						writer.WriteLine(L"");
					}
				}

				if (classDecls.Keys().Contains(nullptr))
				{
					for (auto decl : classDecls[nullptr])
					{
						WriteHeader_ClassPreDecl(writer, decl, nss);
					}
					writer.WriteLine(L"");
					if (multiFile)
					{
						vint index = headerFilesClasses.Keys().IndexOf(0);
						if (index != -1)
						{
							for (auto decl : headerFilesClasses.GetByIndex(index))
							{
								WriteHeader_Class(writer, decl, nss);
								writer.WriteLine(L"");
							}
						}
					}
					else
					{
						for (auto decl : classDecls[nullptr])
						{
							WriteHeader_Class(writer, decl, nss);
							writer.WriteLine(L"");
						}
					}
				}
				WriteNamespaceEnd(writer, nss);

				WriteHeader_Global(writer);

				writer.WriteLine(L"");
				WritePopCompileOptions(writer);
			}

			void WfCppConfig::WriteNonCustomSubHeader(stream::StreamWriter& writer, vint fileIndex)
			{
				WritePushCompileOptions(writer);
				writer.WriteLine(L"");
				List<WString> nss;

				if (classDecls.Keys().Contains(nullptr))
				{
					vint index = headerFilesClasses.Keys().IndexOf(fileIndex);
					if (index != -1)
					{
						for (auto decl : headerFilesClasses.GetByIndex(index))
						{
							WriteHeader_Class(writer, decl, nss);
							writer.WriteLine(L"");
						}
					}
				}
				WriteNamespaceEnd(writer, nss);

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
						vint index = customFilesClasses.Keys().IndexOf(L"");
						if (index != -1)
						{
							for (auto decl : customFilesClasses.GetByIndex(index))
							{
								WriteCpp_Class(writer, decl, nss);
							}
						}
					}
					else
					{
						for (auto decl : classDecls[nullptr])
						{
							WriteCpp_Class(writer, decl, nss);
						}
					}

					WriteNamespaceEnd(writer, nss);
				}

				WriteCpp_PopMacros(writer);

				writer.WriteLine(L"");
				WritePopCompileOptions(writer);
			}

			void WfCppConfig::WriteSubHeader(stream::StreamWriter& writer, const WString& fileName)
			{
				WritePushCompileOptions(writer);
				writer.WriteLine(L"");
				List<WString> nss;

				for (auto decl : customFilesClasses.Get(fileName))
				{
					WriteHeader_Class(writer, decl, nss);
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

				if (From(customFilesClasses.Get(fileName))
					.Any([=](Ptr<WfClassDeclaration> decl)
					{
						return IsClassHasUserImplMethods(decl, true);
					}))
				{
					writer.WriteLine(L"/* USER_CONTENT_BEGIN(custom global declarations) */");
					writer.WriteLine(L"/* USER_CONTENT_END() */");
					writer.WriteLine(L"");
				}

				List<WString> nss;

				for (auto decl : customFilesClasses.Get(fileName))
				{
					WriteCpp_Class(writer, decl, nss);
				}

				WriteNamespaceEnd(writer, nss);
				WriteCpp_PopMacros(writer);
				writer.WriteLine(L"");
				WritePopCompileOptions(writer);
			}

			void WfCppConfig::WriteReflectionHeader(stream::StreamWriter& writer, bool multiFile)
			{
				WritePushCompileOptions(writer);
				if (manager->declarationTypes.Count() > 0)
				{
					writer.WriteLine(L"");
					WriteHeader_Reflection(writer);
					writer.WriteLine(L"");
				}
				WritePopCompileOptions(writer);
			}

			void WfCppConfig::WriteReflectionCpp(stream::StreamWriter& writer, bool multiFile)
			{
				WritePushCompileOptions(writer);
				if (manager->declarationTypes.Count() > 0)
				{
					writer.WriteLine(L"");
					WriteCpp_Reflection(writer);
					writer.WriteLine(L"");
				}
				WritePopCompileOptions(writer);
			}
		}
	}
}
