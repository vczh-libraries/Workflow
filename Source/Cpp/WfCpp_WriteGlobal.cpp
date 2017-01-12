#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;

			void WfCppConfig::WriteHeader_Global(stream::StreamWriter& writer)
			{
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
				writer.WriteLine(L"}");
			}

			void WfCppConfig::WriteCpp_Global(stream::StreamWriter& writer)
			{
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
				writer.WriteLine(L"END_GLOBAL_STORAGE_CLASS(" + storageName + L")");
				writer.WriteLine(L"");

				writer.WriteLine(L"namespace vl_workflow_global");
				writer.WriteLine(L"{");
				FOREACH(Ptr<WfFunctionDeclaration>, decl, funcDecls)
				{
					writer.WriteString(L"\t");
					WriteFunctionHeader(writer, decl, assemblyName + L"::" + ConvertName(decl->name.value), true);
					writer.WriteLine(L"");
					writer.WriteLine(L"\t{");
					WriteFunctionBody(writer, decl->statement, L"\t\t");
					writer.WriteLine(L"\t}");
					writer.WriteLine(L"");
				}
				writer.WriteLine(L"\t" + assemblyName + L"& " + assemblyName + L"::Instance()");
				writer.WriteLine(L"\t{");
				writer.WriteLine(L"\t\treturn Get" + storageName + L"().instance;");
				writer.WriteLine(L"\t}");

				Dictionary<WString, Ptr<WfExpression>> reversedLambdaExprs;
				CopyFrom(
					reversedLambdaExprs,
					From(lambdaExprs)
						.Select([](Pair<Ptr<WfExpression>, WString> pair)
					{
						return Pair<WString, Ptr<WfExpression>>(pair.value, pair.key);
					})
				);

				Dictionary<WString, Ptr<WfNewInterfaceExpression>> reversedClassExprs;
				CopyFrom(
					reversedClassExprs,
					From(classExprs)
						.Select([](Pair<Ptr<WfNewInterfaceExpression>, WString> pair)
					{
						return Pair<WString, Ptr<WfNewInterfaceExpression>>(pair.value, pair.key);
					})
				);

				FOREACH(Ptr<WfExpression>, expr, reversedLambdaExprs.Values())
				{
					writer.WriteLine(L"");
					WriteCpp_LambdaExprDecl(writer, expr);
				}

				FOREACH(Ptr<WfNewInterfaceExpression>, expr, reversedClassExprs.Values())
				{
					writer.WriteLine(L"");
					WriteCpp_ClassExprDecl(writer, expr);
				}

				FOREACH(Ptr<WfExpression>, expr, reversedLambdaExprs.Values())
				{
					writer.WriteLine(L"");
					WriteCpp_LambdaExprImpl(writer, expr);
				}

				if (reversedClassExprs.Count() > 0)
				{
					writer.WriteLine(L"");
					FOREACH(Ptr<WfNewInterfaceExpression>, expr, reversedClassExprs.Values())
					{
						WriteCpp_ClassExprImpl(writer, expr);
					}
				}
				writer.WriteLine(L"}");
			}
		}
	}
}