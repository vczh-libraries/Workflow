#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;

			void WfCppConfig::WriteCpp_LambdaExprDecl(stream::StreamWriter& writer, Ptr<WfExpression> lambda)
			{
				if (auto ordered = lambda.Cast<WfOrderedLambdaExpression>())
				{
					auto name = lambdaExprs[lambda.Obj()];
					writer.WriteLine(L"\tstruct " + name);
					writer.WriteLine(L"\t{");
					writer.WriteString(L"\t\t");
					WriteFunctionHeader(writer, ordered, L"operator()", true);
					writer.WriteLine(L" const;");
					writer.WriteLine(L"\t};");
				}
				else if (auto funcExpr = lambda.Cast<WfFunctionExpression>())
				{
					auto name = lambdaExprs[lambda.Obj()];
					writer.WriteLine(L"\tstruct " + name);
					writer.WriteLine(L"\t{");
					writer.WriteString(L"\t\t");
					WriteFunctionHeader(writer, funcExpr, L"operator()", true);
					writer.WriteLine(L" const;");
					writer.WriteLine(L"\t};");
				}
			}

			void WfCppConfig::WriteCpp_LambdaExprImpl(stream::StreamWriter& writer, Ptr<WfExpression> lambda)
			{
				if (auto ordered = lambda.Cast<WfOrderedLambdaExpression>())
				{
					auto name = lambdaExprs[lambda.Obj()];
					writer.WriteString(L"\t");
					WriteFunctionHeader(writer, ordered, name + L"::operator()", true);
					writer.WriteLine(L" const");
					writer.WriteLine(L"\t{");
					WriteFunctionBody(writer, ordered->body, L"\t\t");
					writer.WriteLine(L"\t}");
				}
				else if (auto funcExpr = lambda.Cast<WfFunctionExpression>())
				{
					auto name = lambdaExprs[lambda.Obj()];
					writer.WriteString(L"\t");
					WriteFunctionHeader(writer, funcExpr, name + L"::operator()", true);
					writer.WriteLine(L" const");
					writer.WriteLine(L"\t{");
					WriteFunctionBody(writer, funcExpr->function->statement, L"\t\t");
					writer.WriteLine(L"\t}");
				}
			}

			void WfCppConfig::WriteCpp_ClassExprDecl(stream::StreamWriter& writer, Ptr<WfNewInterfaceExpression> lambda)
			{
				auto result = manager->expressionResolvings[lambda.Obj()];
				auto td = result.constructorInfo->GetOwnerTypeDescriptor();
				auto name = classExprs[lambda.Obj()];
				writer.WriteLine(L"\tclass " + name + L" : public ::vl::Object, public virtual " + ConvertFullName(CppGetFullName(td)));
				writer.WriteLine(L"\t{");
				writer.WriteLine(L"\tpublic:");
				FOREACH(Ptr<WfClassMember>, member, lambda->members)
				{
					GenerateClassMemberDecl(this, writer, name, member, L"\t\t");
				}
				writer.WriteLine(L"\t};");
			}

			void WfCppConfig::WriteCpp_ClassExprImpl(stream::StreamWriter& writer, Ptr<WfNewInterfaceExpression> lambda)
			{
				auto name = classExprs[lambda.Obj()];
				FOREACH(Ptr<WfClassMember>, member, lambda->members)
				{
					if (GenerateClassMemberImpl(this, writer, name, name, member, L"\t"))
					{
						writer.WriteLine(L"");
					}
				}
			}
		}
	}
}