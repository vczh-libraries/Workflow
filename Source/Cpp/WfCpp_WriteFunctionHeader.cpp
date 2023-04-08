#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;
			using namespace reflection::description;

			ITypeInfo* WfCppConfig::WriteFunctionHeader(stream::StreamWriter& writer, ITypeInfo* typeInfo, collections::List<WString>& arguments, const WString& name, bool writeReturnType)
			{
				ITypeInfo* returnType = nullptr;
				if (writeReturnType)
				{
					returnType = typeInfo->GetElementType()->GetGenericArgument(0);
					writer.WriteString(ConvertType(returnType));
					writer.WriteChar(L' ');
				}

				writer.WriteString(name);
				writer.WriteString(L"(");
				for (vint i = 0; i < arguments.Count(); i++)
				{
					if (i > 0)
					{
						writer.WriteString(L", ");
					}
					writer.WriteString(ConvertArgumentType(typeInfo->GetElementType()->GetGenericArgument(i + 1)));
					writer.WriteChar(L' ');
					writer.WriteString(ConvertName(arguments[i]));
				}
				writer.WriteString(L")");

				return returnType;
			}

			ITypeInfo* WfCppConfig::WriteFunctionHeader(stream::StreamWriter& writer, Ptr<WfOrderedLambdaExpression> ordered, const WString& name, bool writeReturnType)
			{
				auto result = manager->expressionResolvings[ordered.Obj()];
				auto typeInfo = result.type.Obj();

				auto scope = manager->nodeScopes[ordered.Obj()].Obj();
				List<WString> arguments;
				CopyFrom(
					arguments,
					Range<vint>(0, scope->symbols.Count())
						.Select([scope](vint index)
						{
							return scope->symbols.GetByIndex(index)[0]->name;
						})
						.OrderBySelf()
					);

				return WriteFunctionHeader(writer, typeInfo, arguments, name, writeReturnType);
			}

			ITypeInfo* WfCppConfig::WriteFunctionHeader(stream::StreamWriter& writer, Ptr<WfFunctionExpression> funcExpr, const WString& name, bool writeReturnType)
			{
				auto result = manager->expressionResolvings[funcExpr.Obj()];
				auto typeInfo = result.type.Obj();

				List<WString> arguments;
				CopyFrom(
					arguments,
					From(funcExpr->function->arguments)
						.Select([](Ptr<WfFunctionArgument> argument)
						{
							return argument->name.value;
						})
					);
				return WriteFunctionHeader(writer, typeInfo, arguments, name, writeReturnType);
			}

			ITypeInfo* WfCppConfig::WriteFunctionHeader(stream::StreamWriter& writer, Ptr<WfFunctionDeclaration> decl, const WString& name, bool writeReturnType)
			{
				vint index1 = manager->declarationMemberInfos.Keys().IndexOf(decl.Obj());
				vint index2 = manager->interfaceMethodImpls.Keys().IndexOf(decl.Obj());
				auto methodInfo =
					index1 != -1 ? dynamic_cast<IMethodInfo*>(manager->declarationMemberInfos.Values()[index1].Obj()) :
					index2 != -1 ? manager->interfaceMethodImpls.Values()[index2] :
					nullptr;

				List<WString> arguments;
				CopyFrom(
					arguments,
					From(decl->arguments)
						.Select([](Ptr<WfFunctionArgument> argument)
						{
							return argument->name.value;
						})
					);

				if (methodInfo)
				{
					return WriteFunctionHeader(writer, methodInfo, arguments, name, writeReturnType);
				}
				else
				{
					auto scope = manager->nodeScopes[decl.Obj()].Obj();
					auto symbol = manager->GetDeclarationSymbol(scope, decl.Obj());
					auto typeInfo = symbol->typeInfo.Obj();

					return WriteFunctionHeader(writer, typeInfo, arguments, name, writeReturnType);
				}
			}

			ITypeInfo* WfCppConfig::WriteFunctionHeader(stream::StreamWriter& writer, IMethodInfo* methodInfo, collections::List<WString>& arguments, const WString& name, bool writeReturnType)
			{
				ITypeInfo* returnType = nullptr;
				if (writeReturnType)
				{
					returnType = methodInfo->GetReturn();
					writer.WriteString(ConvertType(returnType));
					writer.WriteChar(L' ');
				}
				writer.WriteString(name);
				writer.WriteString(L"(");
				vint count = methodInfo->GetParameterCount();
				for (vint i = 0; i < count; i++)
				{
					if (i > 0)
					{
						writer.WriteString(L", ");
					}
					writer.WriteString(ConvertArgumentType(methodInfo->GetParameter(i)->GetType()));
					writer.WriteChar(L' ');
					if (arguments.Count() == 0)
					{
						writer.WriteString(ConvertName(methodInfo->GetParameter(i)->GetName()));
					}
					else
					{
						writer.WriteString(ConvertName(arguments[i]));
					}
				}
				writer.WriteString(L")");
				return returnType;
			}

			ITypeInfo* WfCppConfig::WriteFunctionHeader(stream::StreamWriter& writer, IMethodInfo* methodInfo, const WString& name, bool writeReturnType)
			{
				List<WString> arguments;
				return WriteFunctionHeader(writer, methodInfo, arguments, name, writeReturnType);
			}
		}
	}
}