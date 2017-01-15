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
			using namespace analyzer;

			class WfGenerateStatementVisitor : public Object, public WfStatement::IVisitor
			{
			public:
				WfCppConfig*				config;
				Ptr<FunctionRecord>			functionRecord;
				stream::StreamWriter&		writer;
				WString						prefixBlock;
				WString						prefix;
				ITypeInfo*					returnType;

				WfGenerateStatementVisitor(WfCppConfig* _config, Ptr<FunctionRecord> _functionRecord, stream::StreamWriter& _writer, const WString& _prefixBlock, const WString& _prefix, ITypeInfo* _returnType)
					:config(_config)
					, functionRecord(_functionRecord)
					, writer(_writer)
					, prefixBlock(_prefixBlock)
					, prefix(_prefix)
					, returnType(_returnType)
				{
				}

				void Call(Ptr<WfStatement> node, WString prefixDelta = WString(L"\t", false))
				{
					GenerateStatement(config, functionRecord, writer, node, prefix, prefixDelta, returnType);
				}

				void Visit(WfBreakStatement* node)override
				{
					writer.WriteString(prefix);
					writer.WriteLine(L"break;");
				}

				void Visit(WfContinueStatement* node)override
				{
					writer.WriteString(prefix);
					writer.WriteLine(L"continue;");
				}

				void Visit(WfReturnStatement* node)override
				{
					writer.WriteString(prefix);
					if (node->expression)
					{
						writer.WriteString(L"return ");
						GenerateExpression(config, writer, node->expression, returnType);
						writer.WriteLine(L";");
					}
					else
					{
						writer.WriteLine(L"return;");
					}
				}

				void Visit(WfDeleteStatement* node)override
				{
					writer.WriteString(prefix);
					writer.WriteString(L"::vl::__vwsn::This(");
					GenerateExpression(config, writer, node->expression, nullptr);
					writer.WriteLine(L")->Dispose(true);");
				}

				void Visit(WfRaiseExceptionStatement* node)override
				{
					if (node->expression)
					{
						writer.WriteString(prefix);
						writer.WriteString(L"throw ::vl::reflection::description::TypeDescriptorException(");
						GenerateExpression(config, writer, node->expression, TypeInfoRetriver<WString>::CreateTypeInfo().Obj());
						writer.WriteLine(L", false);");
					}
					else
					{
						writer.WriteString(prefix);
						writer.WriteLine(L"throw;");
					}
				}

				void Visit(WfIfStatement* node)override
				{
					writer.WriteString(prefix);
					writer.WriteString(L"if (");
					GenerateExpression(config, writer, node->expression, TypeInfoRetriver<bool>::CreateTypeInfo().Obj());
					writer.WriteLine(L")");
					Call(node->trueBranch);
					if (node->falseBranch)
					{
						writer.WriteString(prefix);
						writer.WriteLine(L"else");
						Call(node->falseBranch);
					}
				}

				void Visit(WfSwitchStatement* node)override
				{
					auto exprName = L"__vwsne_" + itow(functionRecord->exprCounter++);
					auto result = config->manager->expressionResolvings[node->expression.Obj()];

					writer.WriteString(prefix);
					writer.WriteLine(L"{");
					writer.WriteString(prefix);
					writer.WriteString(L"\t");
					writer.WriteString(config->ConvertType(result.type.Obj()));
					writer.WriteString(L" ");
					writer.WriteString(exprName);
					writer.WriteString(L" = ");
					GenerateExpression(config, writer, node->expression, result.type.Obj());
					writer.WriteLine(L";");

					FOREACH_INDEXER(Ptr<WfSwitchCase>, switchCase, index, node->caseBranches)
					{
						writer.WriteString(prefix);
						writer.WriteString(L"\t");
						if (index > 0)
						{
							writer.WriteString(L"else ");
						}
						writer.WriteString(L"if (");
						writer.WriteString(exprName);
						writer.WriteString(L" == ");
						GenerateExpression(config, writer, switchCase->expression, result.type.Obj());
						writer.WriteLine(L")");
						GenerateStatement(config, functionRecord, writer, switchCase->statement, prefix + L"\t", WString(L"\t", false), returnType);
					}

					if (node->defaultBranch)
					{
						writer.WriteString(prefix);
						writer.WriteString(L"\telse ");
						GenerateStatement(config, functionRecord, writer, node->defaultBranch, prefix + L"\t", WString(L"\t", false), returnType);
					}

					writer.WriteString(prefix);
					writer.WriteLine(L"}");
				}

				void Visit(WfWhileStatement* node)override
				{
					writer.WriteString(prefix);
					writer.WriteString(L"while (");
					GenerateExpression(config, writer, node->condition, TypeInfoRetriver<bool>::CreateTypeInfo().Obj());
					writer.WriteLine(L")");
					Call(node->statement);
				}

				void Visit(WfForEachStatement* node)override
				{
					auto result = config->manager->expressionResolvings[node->collection.Obj()];
					auto elementType = result.type->GetElementType()->GetGenericArgument(0);
					auto elementTypeCpp = elementType ? config->ConvertType(elementType) : config->ConvertType(description::GetTypeDescriptor<Value>());

					auto typeName = L"__vwsnt_" + itow(functionRecord->typeCounter++);
					writer.WriteString(prefix);
					writer.WriteString(L"using ");
					writer.WriteString(typeName);
					writer.WriteString(L" = ");
					writer.WriteString(elementTypeCpp);
					writer.WriteLine(L";");

					writer.WriteString(prefix);
					writer.WriteString(L"FOREACH(");
					writer.WriteString(typeName);
					writer.WriteString(L", ");
					writer.WriteString(config->ConvertName(node->name.value));
					writer.WriteString(L", ");
					if (result.type->GetTypeDescriptor() != description::GetTypeDescriptor<IValueEnumerable>())
					{
						writer.WriteString(L" ::vl::reflection::description::GetLazyList<");
						writer.WriteString(elementTypeCpp);
						writer.WriteString(L"<(");
					}
					GenerateExpression(config, writer, node->collection, nullptr);
					if (result.type->GetTypeDescriptor() != description::GetTypeDescriptor<IValueEnumerable>())
					{
						writer.WriteString(L")");
					}
					if (node->direction == WfForEachDirection::Reversed)
					{
						writer.WriteString(L".Reverse()");
					}
					writer.WriteLine(L");");

					Call(node->statement);
				}

				void Visit(WfTryStatement* node)override
				{
					auto blockName = L"__vwsnb_" + itow(functionRecord->blockCounter++);
					auto exName = L"__vwsne_" + itow(functionRecord->exprCounter++);

					WString tryPrefix = prefix;
					if (node->finallyStatement)
					{
						tryPrefix += L"\t";
						writer.WriteString(prefix);
						writer.WriteLine(L"{");
						writer.WriteString(tryPrefix);
						writer.WriteString(L"auto ");
						writer.WriteString(blockName);
						writer.WriteLine(L" = [&]()");
						GenerateStatement(config, functionRecord, writer, node->finallyStatement, tryPrefix, WString(L"\t", false), returnType);
						writer.WriteString(tryPrefix);
						writer.WriteLine(L";");
					}
					WString bodyPrefix = tryPrefix + L"\t";

					writer.WriteString(tryPrefix);
					writer.WriteLine(L"try");
					writer.WriteString(tryPrefix);
					writer.WriteLine(L"{");
					GenerateStatement(config, functionRecord, writer, node->protectedStatement, bodyPrefix, WString(L"\t", false), returnType);
					writer.WriteString(bodyPrefix);
					writer.WriteString(blockName);
					writer.WriteLine(L"();");
					writer.WriteString(tryPrefix);
					writer.WriteLine(L"}");

					writer.WriteString(tryPrefix);
					writer.WriteString(L"catch(const ::vl::Exception& ");
					writer.WriteString(exName);
					writer.WriteLine(L")");
					writer.WriteString(tryPrefix);
					writer.WriteLine(L"{");
					if (node->catchStatement)
					{
						GenerateStatement(config, functionRecord, writer, node->catchStatement, bodyPrefix, WString(L"\t", false), returnType);
					}
					writer.WriteString(bodyPrefix);
					writer.WriteString(blockName);
					writer.WriteLine(L"();");
					writer.WriteString(tryPrefix);
					writer.WriteLine(L"}");


					if (node->finallyStatement)
					{
						writer.WriteString(prefix);
						writer.WriteLine(L"}");
					}
				}

				void Visit(WfBlockStatement* node)override
				{
					writer.WriteString(prefixBlock);
					writer.WriteLine(L"{");
					FOREACH(Ptr<WfStatement>, statement, node->statements)
					{
						if (statement.Cast<WfBlockStatement>())
						{
							Call(statement);
						}
						else
						{
							Call(statement, WString::Empty);
						}
					}
					writer.WriteString(prefixBlock);
					writer.WriteLine(L"}");
				}

				void Visit(WfExpressionStatement* node)override
				{
					writer.WriteString(prefix);
					GenerateExpression(config, writer, node->expression, nullptr);
					writer.WriteLine(L";");
				}

				void Visit(WfVariableStatement* node)override
				{
					auto scope = config->manager->nodeScopes[node->variable.Obj()];
					auto symbol = scope->symbols[node->variable->name.value][0].Obj();

					writer.WriteString(prefix);
					writer.WriteString(config->ConvertType(symbol->typeInfo.Obj()));
					writer.WriteString(L" ");
					writer.WriteString(config->ConvertName(node->variable->name.value));

					if (node->variable->expression)
					{
						writer.WriteString(L" = ");
						GenerateExpression(config, writer, node->variable->expression, symbol->typeInfo.Obj());
					}
					writer.WriteLine(L";");
				}
			};

			void GenerateStatement(WfCppConfig* config, Ptr<FunctionRecord> functionRecord, stream::StreamWriter& writer, Ptr<WfStatement> node, const WString& prefix, const WString& prefixDelta, reflection::description::ITypeInfo* returnType)
			{
				WfGenerateStatementVisitor visitor(config, functionRecord, writer, prefix, prefix + prefixDelta, returnType);
				node->Accept(&visitor);
			}
		}
	}
}