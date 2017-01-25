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

			void WfCppConfig::SortClosure(Dictionary<WString, Ptr<WfExpression>>& reversedLambdaExprs, Dictionary<WString, Ptr<WfNewInterfaceExpression>>& reversedClassExprs)
			{
				CopyFrom(
					reversedLambdaExprs,
					From(lambdaExprs)
						.Select([](Pair<Ptr<WfExpression>, WString> pair)
						{
							return Pair<WString, Ptr<WfExpression>>(pair.value, pair.key);
						})
					);

				CopyFrom(
					reversedClassExprs,
					From(classExprs)
						.Select([](Pair<Ptr<WfNewInterfaceExpression>, WString> pair)
						{
							return Pair<WString, Ptr<WfNewInterfaceExpression>>(pair.value, pair.key);
						})
					);
			}

			void WfCppConfig::WriteHeader_Global(stream::StreamWriter& writer)
			{
				Dictionary<WString, Ptr<WfExpression>> reversedLambdaExprs;
				Dictionary<WString, Ptr<WfNewInterfaceExpression>> reversedClassExprs;
				SortClosure(reversedLambdaExprs, reversedClassExprs);

				writer.WriteLine(L"/***********************************************************************");
				writer.WriteLine(L"Global Variables and Functions");
				writer.WriteLine(L"***********************************************************************/");
				writer.WriteLine(L"");
				writer.WriteLine(L"namespace " + assemblyNamespace);
				writer.WriteLine(L"{");
				writer.WriteLine(L"\tclass " + assemblyName);
				writer.WriteLine(L"\t{");
				writer.WriteLine(L"\tpublic:");
				if (varDecls.Count() > 0)
				{
					writer.WriteLine(L"");
					FOREACH(Ptr<WfVariableDeclaration>, decl, varDecls)
					{
						auto scope = manager->nodeScopes[decl.Obj()].Obj();
						auto symbol = scope->symbols[decl->name.value][0];
						auto typeInfo = symbol->typeInfo;
						writer.WriteString(L"\t\t" + ConvertType(typeInfo.Obj()) + L" " + ConvertName(decl->name.value));
						auto defaultValue = DefaultValue(typeInfo.Obj());
						if (defaultValue != L"")
						{
							writer.WriteString(L" = ");
							writer.WriteString(defaultValue);
						}
						writer.WriteLine(L";");
					}
				}
				if (funcDecls.Count() > 0)
				{
					writer.WriteLine(L"");
					FOREACH(Ptr<WfFunctionDeclaration>, decl, funcDecls)
					{
						writer.WriteString(L"\t\t");
						WriteFunctionHeader(writer, decl, ConvertName(decl->name.value), true);
						writer.WriteLine(L";");
					}
				}
				writer.WriteLine(L"");
				writer.WriteLine(L"\t\tstatic " + assemblyName + L"& Instance();");
				writer.WriteLine(L"\t};");

				if (reversedLambdaExprs.Count() + reversedClassExprs.Count() > 0)
				{
					writer.WriteLine(L"");
					writer.WriteLine(L"/***********************************************************************");
					writer.WriteLine(L"Closures");
					writer.WriteLine(L"***********************************************************************/");

					FOREACH(Ptr<WfExpression>, expr, reversedLambdaExprs.Values())
					{
						writer.WriteLine(L"");
						WriteHeader_LambdaExprDecl(writer, expr);
					}

					FOREACH(Ptr<WfNewInterfaceExpression>, expr, reversedClassExprs.Values())
					{
						writer.WriteLine(L"");
						WriteHeader_ClassExprDecl(writer, expr);
					}
				}

				writer.WriteLine(L"}");
			}

			void WfCppConfig::WriteCpp_Global(stream::StreamWriter& writer)
			{
				Dictionary<WString, Ptr<WfExpression>> reversedLambdaExprs;
				Dictionary<WString, Ptr<WfNewInterfaceExpression>> reversedClassExprs;
				SortClosure(reversedLambdaExprs, reversedClassExprs);

				writer.WriteLine(L"/***********************************************************************");
				writer.WriteLine(L"Global Variables");
				writer.WriteLine(L"***********************************************************************/");
				writer.WriteLine(L"");

				WString storageName = assemblyNamespace + L"_" + assemblyName;
				writer.WriteLine(L"BEGIN_GLOBAL_STORAGE_CLASS(" + storageName + L")");
				writer.WriteLine(L"\t" + assemblyNamespace + L"::" + assemblyName + L" instance;");
				writer.WriteLine(L"\tINITIALIZE_GLOBAL_STORAGE_CLASS");
				if (varDecls.Count() > 0)
				{
					writer.WriteLine(L"");
					FOREACH(Ptr<WfVariableDeclaration>, decl, varDecls)
					{
						auto scope = manager->nodeScopes[decl.Obj()].Obj();
						auto symbol = scope->symbols[decl->name.value][0];
						auto typeInfo = symbol->typeInfo;
						if (decl->expression)
						{
							writer.WriteString(L"\t\tinstance." + ConvertName(decl->name.value) + L" = ");
							GenerateExpression(this, writer, decl->expression, typeInfo.Obj());
							writer.WriteLine(L";");
						}
					}
				}
				writer.WriteLine(L"\tFINALIZE_GLOBAL_STORAGE_CLASS");
				if (varDecls.Count() > 0)
				{
					writer.WriteLine(L"");
					FOREACH(Ptr<WfVariableDeclaration>, decl, varDecls)
					{
						auto scope = manager->nodeScopes[decl.Obj()].Obj();
						auto symbol = scope->symbols[decl->name.value][0];
						auto typeInfo = symbol->typeInfo;
						switch (typeInfo->GetDecorator())
						{
						case ITypeInfo::RawPtr:
						case ITypeInfo::SharedPtr:
							writer.WriteLine(L"\t\tinstance." + ConvertName(decl->name.value) + L" = nullptr;");
							break;
						case ITypeInfo::Nullable:
							writer.WriteString(L"\t\tinstance." + ConvertName(decl->name.value) + L" = ");
							ConvertType(typeInfo.Obj());
							writer.WriteLine(L"();");
							break;
						default:
							if (typeInfo->GetTypeDescriptor() == description::GetTypeDescriptor<WString>())
							{
								writer.WriteLine(L"\t\tinstance." + ConvertName(decl->name.value) + L" = ::vl::WString::Empty;");
							}
							else if (typeInfo->GetTypeDescriptor() == description::GetTypeDescriptor<Value>())
							{
								writer.WriteLine(L"\t\tinstance." + ConvertName(decl->name.value) + L" = ::vl::reflection::description::Value();");
							}
						}
					}
				}
				writer.WriteLine(L"END_GLOBAL_STORAGE_CLASS(" + storageName + L")");
				writer.WriteLine(L"");

				writer.WriteString(L"namespace ");
				writer.WriteLine(assemblyNamespace);
				writer.WriteLine(L"{");

				writer.WriteLine(L"/***********************************************************************");
				writer.WriteLine(L"Global Functions");
				writer.WriteLine(L"***********************************************************************/");

				FOREACH(Ptr<WfFunctionDeclaration>, decl, funcDecls)
				{
					writer.WriteLine(L"");
					writer.WriteString(L"\t");
					auto returnType = WriteFunctionHeader(writer, decl, assemblyName + L"::" + ConvertName(decl->name.value), true);
					writer.WriteLine(L"");
					WriteFunctionBody(writer, decl->statement, L"\t", returnType);
				}

				writer.WriteLine(L"");
				writer.WriteLine(L"\t" + assemblyName + L"& " + assemblyName + L"::Instance()");
				writer.WriteLine(L"\t{");
				writer.WriteLine(L"\t\treturn Get" + storageName + L"().instance;");
				writer.WriteLine(L"\t}");

				if (reversedLambdaExprs.Count() + reversedClassExprs.Count() > 0)
				{
					writer.WriteLine(L"");
					writer.WriteLine(L"/***********************************************************************");
					writer.WriteLine(L"Closures");
					writer.WriteLine(L"***********************************************************************/");

					FOREACH(Ptr<WfExpression>, expr, reversedLambdaExprs.Values())
					{
						writer.WriteLine(L"");
						writer.WriteLine(L"\t//-------------------------------------------------------------------");
						writer.WriteLine(L"");
						WriteCpp_LambdaExprImpl(writer, expr);
					}

					if (reversedClassExprs.Count() > 0)
					{
						writer.WriteLine(L"");
						FOREACH(Ptr<WfNewInterfaceExpression>, expr, reversedClassExprs.Values())
						{
							writer.WriteLine(L"\t//-------------------------------------------------------------------");
							writer.WriteLine(L"");
							WriteCpp_ClassExprImpl(writer, expr);
						}
					}
				}

				writer.WriteLine(L"}");
			}
		}
	}
}