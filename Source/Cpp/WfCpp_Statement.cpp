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

				void Call(Ptr<WfStatement> node, WString prefixDelta = WString::Unmanaged(L"\t"))
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
						writer.WriteString(L"throw ::vl::Exception(");

						auto result = config->manager->expressionResolvings[node->expression.Obj()];
						bool throwString = result.type->GetTypeDescriptor() == description::GetTypeDescriptor<WString>();
						if (!throwString)
						{
							writer.WriteString(L"::vl::__vwsn::This(");
						}
						GenerateExpression(config, writer, node->expression, result.type.Obj());
						if (!throwString)
						{
							writer.WriteString(L".Obj())->GetMessage()");
						}

						writer.WriteLine(L");");
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
					while (node)
					{
						writer.WriteString(L"if (");
						if (node->type)
						{
							auto result = config->manager->expressionResolvings[node->expression.Obj()];
							auto scope = config->manager->nodeScopes[node].Obj();
							auto typeInfo = CreateTypeInfoFromType(scope, node->type);
							writer.WriteString(L"auto ");
							writer.WriteString(config->ConvertName(node->name.value));
							writer.WriteString(L" = ");
							ConvertType(config, writer, result.type.Obj(), typeInfo.Obj(), [&]() {GenerateExpression(config, writer, node->expression, nullptr); }, false);
						}
						else
						{
							GenerateExpression(config, writer, node->expression, TypeInfoRetriver<bool>::CreateTypeInfo().Obj());
						}
						writer.WriteLine(L")");
						Call(node->trueBranch);
						if (node->falseBranch)
						{
							writer.WriteString(prefix);
							if (auto ifStat = node->falseBranch.Cast<WfIfStatement>())
							{
								writer.WriteString(L"else ");
								node = ifStat.Obj();
								continue;
							}
							else
							{
								writer.WriteLine(L"else");
								Call(node->falseBranch);
							}
						}
						break;
					}
				}

				void Visit(WfWhileStatement* node)override
				{
					writer.WriteString(prefix);
					writer.WriteString(L"while (");
					GenerateExpression(config, writer, node->condition, TypeInfoRetriver<bool>::CreateTypeInfo().Obj());
					writer.WriteLine(L")");
					Call(node->statement);
				}

				void Visit(WfTryStatement* node)override
				{
					auto exName = L"__vwsne_" + itow(functionRecord->exprCounter++);

					WString tryPrefix = prefix;
					if (node->finallyStatement)
					{
						auto blockName = L"__vwsnb_" + itow(functionRecord->blockCounter++);
						tryPrefix += L"\t";

						writer.WriteString(prefix);
						writer.WriteLine(L"{");

						writer.WriteString(tryPrefix);
						writer.WriteString(L"auto ");
						writer.WriteString(blockName);
						writer.WriteLine(L" = [&]()");
						GenerateStatement(config, functionRecord, writer, node->finallyStatement, tryPrefix, WString::Unmanaged(L"\t"), returnType);
						writer.WriteString(tryPrefix);
						writer.WriteLine(L";");

						writer.WriteString(tryPrefix);
						writer.WriteString(L"::vl::__vwsn::RunOnExit<::vl::RemoveCVR<decltype(");
						writer.WriteString(blockName);
						writer.WriteString(L")>::Type> ");
						writer.WriteString(blockName);
						writer.WriteString(L"_dtor(&");
						writer.WriteString(blockName);
						writer.WriteLine(L");");
					}
					WString bodyPrefix = tryPrefix + L"\t";

					writer.WriteString(tryPrefix);
					writer.WriteLine(L"try");
					writer.WriteString(tryPrefix);
					writer.WriteLine(L"{");
					GenerateStatement(config, functionRecord, writer, node->protectedStatement, bodyPrefix, WString::Unmanaged(L"\t"), returnType);
					writer.WriteString(tryPrefix);
					writer.WriteLine(L"}");

					writer.WriteString(tryPrefix);
					writer.WriteString(L"catch(const ::vl::Exception&");
					if (node->catchStatement)
					{
						writer.WriteString(L" ");
						writer.WriteString(exName);
					}
					writer.WriteLine(L")");
					writer.WriteString(tryPrefix);
					writer.WriteLine(L"{");
					if (node->catchStatement)
					{
						writer.WriteString(bodyPrefix);
						writer.WriteString(L"auto ");
						writer.WriteString(config->ConvertName(node->name.value));
						writer.WriteString(L" = ::vl::reflection::description::IValueException::Create(");
						writer.WriteString(exName);
						writer.WriteLine(L".Message());");
						GenerateStatement(config, functionRecord, writer, node->catchStatement, bodyPrefix, WString::Unmanaged(L"\t"), returnType);
					}
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

					auto oldPrefix = prefix;
					if (node->endLabel.value != L"")
					{
						auto name = config->ConvertName(node->endLabel.value, L"__vwsnl_" + itow(functionRecord->labelCounter++) + L"_");
						functionRecord->labelNames.Add(node->endLabel.value, name);
						writer.WriteString(prefixBlock);
						writer.WriteLine(L"\t{");
						prefix += L"\t";
					}

					for (auto statement : node->statements)
					{
						statement = SearchUntilNonVirtualStatement(statement);

						if (statement.Cast<WfBlockStatement>())
						{
							Call(statement);
						}
						else
						{
							Call(statement, WString::Empty);
						}
					}

					if (node->endLabel.value != L"")
					{
						prefix = oldPrefix;
						writer.WriteString(prefixBlock);
						writer.WriteLine(L"\t}");
						writer.WriteString(prefixBlock);
						writer.WriteString(L"\t");
						writer.WriteString(functionRecord->labelNames[node->endLabel.value]);
						writer.WriteLine(L":;");
						functionRecord->labelNames.Remove(node->endLabel.value);
					}
					writer.WriteString(prefixBlock);
					writer.WriteLine(L"}");
				}

				void Visit(WfGotoStatement* node)override
				{
					writer.WriteString(prefix);
					writer.WriteString(L"goto ");
					writer.WriteString(functionRecord->labelNames[node->label.value]);
					writer.WriteLine(L";");
				}

				void Visit(WfExpressionStatement* node)override
				{
					writer.WriteString(prefix);
					GenerateExpression(config, writer, node->expression, nullptr, false);
					writer.WriteLine(L";");
				}

				void Visit(WfVariableStatement* node)override
				{
					auto scope = config->manager->nodeScopes[node->variable.Obj()];
					auto symbol = scope->symbols[node->variable->name.value][0].Obj();

					writer.WriteString(prefix);
					if (node->variable->expression)
					{
						writer.WriteString(L"auto");
					}
					else
					{
						writer.WriteString(config->ConvertType(symbol->typeInfo.Obj()));
					}
					writer.WriteString(L" ");
					writer.WriteString(config->ConvertName(node->variable->name.value));

					if (node->variable->expression)
					{
						writer.WriteString(L" = ");
						GenerateExpression(config, writer, node->variable->expression, symbol->typeInfo.Obj());
					}
					writer.WriteLine(L";");
				}

				void Visit(WfVirtualCseStatement* node)override
				{
					node->expandedStatement->Accept(this);
				}

				void Visit(WfCoroutineStatement* node)override
				{
					CHECK_FAIL(L"WfGenerateStatementVisitor::Visit(WfCoroutineStatement*)#Internal error, All coroutine statements do not generate C++ code.");
				}

				void Visit(WfStateMachineStatement* node)override
				{
					CHECK_FAIL(L"WfGenerateStatementVisitor::Visit(WfStateMachineStatement*)#Internal error, All state machine statements do not generate C++ code.");
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