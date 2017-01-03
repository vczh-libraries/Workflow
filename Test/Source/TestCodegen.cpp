#include "Helper.h"

TEST_CASE(TestCodegen)
{
	Ptr<ParsingTable> table = GetWorkflowTable();
	List<WString> codegenNames;
	LoadSampleIndex(L"Codegen", codegenNames);

	WfLexicalScopeManager manager(table);
	FOREACH(WString, codegenName, codegenNames)
	{
		const wchar_t* reading = codegenName.Buffer();
		vint index = wcschr(reading, L'=') - reading;
		WString itemName = codegenName.Sub(0, index);
		WString itemResult = codegenName.Sub(index + 1, codegenName.Length() - index - 1);
		if (itemName.Length() > 3 && itemName.Sub(itemName.Length() - 3, 3) == L"@32")
		{
#ifdef VCZH_64
			continue;
#endif
			itemName = itemName.Sub(0, itemName.Length() - 3);
		}
		else if (itemName.Length() > 3 && itemName.Sub(itemName.Length() - 3, 3) == L"@64")
		{
#ifndef VCZH_64
			continue;
#endif
			itemName = itemName.Sub(0, itemName.Length() - 3);
		}

		UnitTest::PrintInfo(itemName);
		WString sample = LoadSample(L"Codegen", itemName);
		List<Ptr<ParsingError>> errors;
		Ptr<ParsingTreeNode> node = WfParseModuleAsParsingTreeNode(sample, table, errors);
		TEST_ASSERT(node);

		manager.Clear(true, true);
		{
			List<RegexToken> tokens;
			Ptr<WfModule> module = WfConvertParsingTreeNode(node, tokens).Cast<WfModule>();
			manager.AddModule(module);
			manager.Rebuild(true);
			MemoryStream stream;
			{
				StreamWriter writer(stream);
				WfPrint(module, L"", writer);
			}
			{
				stream.SeekFromBegin(0);
				StreamReader reader(stream);
				LogSampleParseResult(L"Codegen", itemName, reader.ReadToEnd(), node, &manager);
			}
			TEST_ASSERT(manager.errors.Count() == 0);
		}

		{
			WfCppConfig config(&manager);
			config.assemblyName = itemName;

			{
				FileStream headerFile(GetCppOutputPath() + config.assemblyName + L".h", FileStream::WriteOnly);
				Utf8Encoder headerEncoder;
				EncoderStream headerStream(headerFile, headerEncoder);
				StreamWriter headerWriter(headerStream);
				List<WString> nss;

				headerWriter.WriteLine(L"/***********************************************************************");
				headerWriter.WriteLine(L"Generated from ../Resources/Codegen/" + itemName + L".txt");
				headerWriter.WriteLine(L"***********************************************************************/");
				headerWriter.WriteLine(L"");
				headerWriter.WriteLine(L"#ifndef VCZH_WORKFLOW_CPP_GENERATED_" + wupper(config.assemblyName));
				headerWriter.WriteLine(L"#define VCZH_WORKFLOW_CPP_GENERATED_" + wupper(config.assemblyName));
				headerWriter.WriteLine(L"");
				headerWriter.WriteLine(L"#include \"../Source/CppTypes.h\"");
				headerWriter.WriteLine(L"");

				if (config.enumDecls.Keys().Contains(nullptr))
				{
					FOREACH(Ptr<WfEnumDeclaration>, decl, config.enumDecls[nullptr])
					{
						config.WriteHeader_Enum(headerWriter, decl, nss);
						headerWriter.WriteLine(L"");
					}
				}

				if (config.structDecls.Keys().Contains(nullptr))
				{
					FOREACH(Ptr<WfStructDeclaration>, decl, config.structDecls[nullptr])
					{
						config.WriteHeader_Struct(headerWriter, decl, nss);
						headerWriter.WriteLine(L"");
					}
				}

				if (config.classDecls.Keys().Contains(nullptr))
				{
					headerWriter.WriteLine(L"#pragma warning(push)");
					headerWriter.WriteLine(L"#pragma warning(disable:4250)");
					FOREACH(Ptr<WfClassDeclaration>, decl, config.classDecls[nullptr])
					{
						config.WriteHeader_ClassPreDecl(headerWriter, decl, nss);
					}
					FOREACH(Ptr<WfClassDeclaration>, decl, config.classDecls[nullptr])
					{
						headerWriter.WriteLine(L"");
						config.WriteHeader_Class(headerWriter, decl, nss);
					}
					headerWriter.WriteLine(L"#pragma warning(pop)");
				}
				config.WriteEnd(headerWriter, nss);

				headerWriter.WriteLine(L"");
				headerWriter.WriteLine(L"/***********************************************************************");
				headerWriter.WriteLine(L"Global Variables and Functions");
				headerWriter.WriteLine(L"***********************************************************************/");
				headerWriter.WriteLine(L"");
				config.WriteHeader_Global(headerWriter);
				headerWriter.WriteLine(L"");
				headerWriter.WriteLine(L"#endif");
			}

			{
				FileStream cppFile(GetCppOutputPath() + config.assemblyName + L".cpp", FileStream::WriteOnly);
				Utf8Encoder cppEncoder;
				EncoderStream cppStream(cppFile, cppEncoder);
				StreamWriter cppWriter(cppStream);
				List<WString> nss;

				cppWriter.WriteLine(L"/***********************************************************************");
				cppWriter.WriteLine(L"Generated from ../Resources/Codegen/" + itemName + L".txt");
				cppWriter.WriteLine(L"***********************************************************************/");
				cppWriter.WriteLine(L"");
				cppWriter.WriteLine(L"#include \"" + config.assemblyName + L".h\"");
				cppWriter.WriteLine(L"");

				cppWriter.WriteLine(L"/***********************************************************************");
				cppWriter.WriteLine(L"Global Variables and Functions");
				cppWriter.WriteLine(L"***********************************************************************/");
				cppWriter.WriteLine(L"");
				config.WriteCpp_Global(cppWriter);
				cppWriter.WriteLine(L"");

				if (config.lambdaExprs.Count() > 0)
				{
					cppWriter.WriteLine(L"/***********************************************************************");
					cppWriter.WriteLine(L"Lambda Functions (Declaration)");
					cppWriter.WriteLine(L"***********************************************************************/");
					cppWriter.WriteLine(L"");
					FOREACH(Ptr<WfExpression>, expr, config.lambdaExprs)
					{
						config.WriteCpp_LambdaExprDecl(cppWriter, expr, nss);
						cppWriter.WriteLine(L"");
					}
				}

				if (config.classExprs.Count() > 0)
				{
					cppWriter.WriteLine(L"/***********************************************************************");
					cppWriter.WriteLine(L"Lambda Classes (Declaration)");
					cppWriter.WriteLine(L"***********************************************************************/");
					cppWriter.WriteLine(L"");
					FOREACH(Ptr<WfNewInterfaceExpression>, expr, config.classExprs)
					{
						config.WriteCpp_ClassExprDecl(cppWriter, expr, nss);
						cppWriter.WriteLine(L"");
					}
				}

				if (config.lambdaExprs.Count() > 0)
				{
					cppWriter.WriteLine(L"/***********************************************************************");
					cppWriter.WriteLine(L"Lambda Functions (Implementation)");
					cppWriter.WriteLine(L"***********************************************************************/");
					cppWriter.WriteLine(L"");
					FOREACH(Ptr<WfExpression>, expr, config.lambdaExprs)
					{
						config.WriteCpp_LambdaExprImpl(cppWriter, expr, nss);
						cppWriter.WriteLine(L"");
					}
				}

				if (config.classExprs.Count() > 0)
				{
					cppWriter.WriteLine(L"/***********************************************************************");
					cppWriter.WriteLine(L"Lambda Classes (Implementation)");
					cppWriter.WriteLine(L"***********************************************************************/");
					cppWriter.WriteLine(L"");
					FOREACH(Ptr<WfNewInterfaceExpression>, expr, config.classExprs)
					{
						config.WriteCpp_ClassExprImpl(cppWriter, expr, nss);
						cppWriter.WriteLine(L"");
					}
				}

				FOREACH(Ptr<WfClassDeclaration>, key, config.classDecls.Keys())
				{
					FOREACH(Ptr<WfClassDeclaration>, decl, config.classDecls[key.Obj()])
					{
						cppWriter.WriteLine(L"/***********************************************************************");
						cppWriter.WriteLine(L"Class (" + CppGetFullName(manager.declarationTypes[decl.Obj()].Obj()) + L")");
						cppWriter.WriteLine(L"***********************************************************************/");
						cppWriter.WriteLine(L"");

						List<Ptr<WfClassMember>> members;
						config.GetClassMembers(decl, members);
						FOREACH(Ptr<WfClassMember>, member, members)
						{
							config.WriteCpp_ClassMember(cppWriter, decl, member, nss);
							cppWriter.WriteLine(L"");
						}
					}
				}

				config.WriteEnd(cppWriter, nss);
			}
		}
		
		{
			Ptr<WfAssembly> assembly = GenerateAssembly(&manager);
			TEST_ASSERT(assembly);
			LogSampleCodegenResult(L"Codegen", itemName, assembly);
			LogSampleAssemblyBinary(L"Codegen", itemName, assembly);

			WfRuntimeThreadContext context(assembly);
			TEST_ASSERT(context.status == WfRuntimeExecutionStatus::Finished);

			{
				vint functionIndex = assembly->functionByName[L"<initialize>"][0];
				context.PushStackFrame(functionIndex, 0);
				TEST_ASSERT(context.status == WfRuntimeExecutionStatus::Ready);

				while (context.status != WfRuntimeExecutionStatus::Finished)
				{
					auto action = context.Execute(nullptr);
					TEST_ASSERT(action != WfRuntimeExecutionAction::Nop);
				}
				TEST_ASSERT(context.Execute(nullptr) == WfRuntimeExecutionAction::Nop);
				Value result;
				TEST_ASSERT(context.PopValue(result) == WfRuntimeThreadContextError::Success);
			}

			{
				vint functionIndex = assembly->functionByName[L"main"][0];
				context.PushStackFrame(functionIndex, 0);
				TEST_ASSERT(context.status == WfRuntimeExecutionStatus::Ready);

				while (context.status != WfRuntimeExecutionStatus::Finished)
				{
					auto action = context.Execute(nullptr);
					TEST_ASSERT(action != WfRuntimeExecutionAction::Nop);
				}
				TEST_ASSERT(context.Execute(nullptr) == WfRuntimeExecutionAction::Nop);
			}

			Value result;
			WString actual;
			TEST_ASSERT(context.PopValue(result) == WfRuntimeThreadContextError::Success);
			result.GetTypeDescriptor()->GetSerializableType()->Serialize(result, actual);
			UnitTest::PrintInfo(L"    expected: " + itemResult);
			UnitTest::PrintInfo(L"    actual: " + actual);
			TEST_ASSERT(actual == itemResult);
			TEST_ASSERT(context.PopValue(result) == WfRuntimeThreadContextError::EmptyStack);
		}
	}
}

TEST_CASE(TestWorkflow)
{
	List<Ptr<ParsingError>> errors;
	List<WString> moduleCodes;
	moduleCodes.Add(LR"workflow(
module test;

func main():string
{
	return "Hello, world!";
}
)workflow");

	auto table = GetWorkflowTable();
	auto assembly = Compile(table, moduleCodes, errors);
	TEST_ASSERT(errors.Count() == 0);

	WfRuntimeThreadContext context(assembly);
	context.PushStackFrame(assembly->functionByName[L"<initialize>"][0], 0);
	context.ExecuteToEnd();
	context.PushStackFrame(assembly->functionByName[L"main"][0], 0);
	context.ExecuteToEnd();

	Value result;
	WString actual;
	TEST_ASSERT(context.PopValue(result) == WfRuntimeThreadContextError::Success);
	result.GetTypeDescriptor()->GetSerializableType()->Serialize(result, actual);
	TEST_ASSERT(actual == L"Hello, world!");
}
