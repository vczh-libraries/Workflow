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
		}
	}
}