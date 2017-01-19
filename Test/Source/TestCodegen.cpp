#include "Helper.h"

bool DecodeCodegenName(const WString& codegenName, WString& itemName, WString& itemResult, bool& cppCodegen)
{
	const wchar_t* reading = codegenName.Buffer();
	vint index = wcschr(reading, L'=') - reading;
	itemName = codegenName.Sub(0, index);
	itemResult = codegenName.Sub(index + 1, codegenName.Length() - index - 1);
	cppCodegen = true;

	if (itemName.Length() > 0 && itemName[0] == L'-')
	{
		cppCodegen = false;
		itemName = itemName.Sub(1, itemName.Length() - 1);
	}
	if (itemName.Length() > 3 && itemName.Sub(itemName.Length() - 3, 3) == L"@32")
	{
#ifdef VCZH_64
		return false;
#endif
		itemName = itemName.Sub(0, itemName.Length() - 3);
	}
	else if (itemName.Length() > 3 && itemName.Sub(itemName.Length() - 3, 3) == L"@64")
	{
#ifndef VCZH_64
		return false;
#endif
		itemName = itemName.Sub(0, itemName.Length() - 3);
	}
	return true;
}

#define DECODE_CODEGEN_NAME\
	WString itemName, itemResult;\
	bool cppCodegen;\
	if (!DecodeCodegenName(codegenName, itemName, itemResult, cppCodegen))\
	{\
		continue;\
	}

TEST_CASE(TestCodegen)
{
	Ptr<ParsingTable> table = GetWorkflowTable();
	List<WString> codegenNames, reflectableAssemblies;
	LoadSampleIndex(L"Codegen", codegenNames);

	WfLexicalScopeManager manager(table);
	FOREACH(WString, codegenName, codegenNames)
	{
		DECODE_CODEGEN_NAME

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
			LogSampleParseResult(L"Codegen", itemName, sample, node, module, &manager);
			TEST_ASSERT(manager.errors.Count() == 0);
		}
		Ptr<WfAssembly> assembly = GenerateAssembly(&manager);
		TEST_ASSERT(assembly);

		if (cppCodegen)
		{
			WfCppConfig config(&manager, itemName);
			if (manager.declarationTypes.Count() > 0)
			{
				reflectableAssemblies.Add(itemName);
			}

			{
				FileStream headerFile(GetCppOutputPath() + config.assemblyName + L".h", FileStream::WriteOnly);
				Utf8Encoder headerEncoder;
				EncoderStream headerStream(headerFile, headerEncoder);
				StreamWriter headerWriter(headerStream);

				headerWriter.WriteLine(L"/***********************************************************************");
				headerWriter.WriteLine(L"Generated from ../Resources/Codegen/" + itemName + L".txt");
				headerWriter.WriteLine(L"***********************************************************************/");
				headerWriter.WriteLine(L"");
				headerWriter.WriteLine(L"#ifndef VCZH_WORKFLOW_CPP_GENERATED_" + wupper(config.assemblyName));
				headerWriter.WriteLine(L"#define VCZH_WORKFLOW_CPP_GENERATED_" + wupper(config.assemblyName));
				headerWriter.WriteLine(L"");
				headerWriter.WriteLine(L"#include \"../Source/CppTypes.h\"");
				headerWriter.WriteLine(L"");
				config.WriteHeader(headerWriter);
				headerWriter.WriteLine(L"");
				headerWriter.WriteLine(L"#endif");
			}

			{
				FileStream cppFile(GetCppOutputPath() + config.assemblyName + L".cpp", FileStream::WriteOnly);
				Utf8Encoder cppEncoder;
				EncoderStream cppStream(cppFile, cppEncoder);
				StreamWriter cppWriter(cppStream);

				cppWriter.WriteLine(L"/***********************************************************************");
				cppWriter.WriteLine(L"Generated from ../Resources/Codegen/" + itemName + L".txt");
				cppWriter.WriteLine(L"***********************************************************************/");
				cppWriter.WriteLine(L"");
				cppWriter.WriteLine(L"#include \"" + config.assemblyName + L".h\"");
				cppWriter.WriteLine(L"");
				config.WriteCpp(cppWriter);
			}
		}
		
		{
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

	{
		FileStream fileStream(GetCppOutputPath() + L"TestCases.cpp", FileStream::WriteOnly);
		Utf8Encoder encoder;
		EncoderStream encoderStream(fileStream, encoder);
		StreamWriter writer(encoderStream);

		FOREACH(WString, codegenName, codegenNames)
		{
			DECODE_CODEGEN_NAME
			if (!cppCodegen) continue;

			writer.WriteString(L"#include \"");
			writer.WriteString(itemName);
			writer.WriteLine(L".h\"");
		}

		writer.WriteLine(L"");
		writer.WriteLine(L"using namespace vl;");
		writer.WriteLine(L"using namespace vl::console;");
		writer.WriteLine(L"using namespace vl::reflection::description;");

		writer.WriteLine(L"");
		writer.WriteLine(L"void LoadTestCaseTypes()");
		writer.WriteLine(L"{");
		FOREACH(WString, name, reflectableAssemblies)
		{
			writer.WriteString(L"\t Load");
			writer.WriteString(name);
			writer.WriteLine(L"Types();");
		}
		writer.WriteLine(L"}");

		FOREACH(WString, codegenName, codegenNames)
		{
			DECODE_CODEGEN_NAME
			if (!cppCodegen) continue;

			writer.WriteLine(L"");

			writer.WriteString(L"TEST_CASE(");
			writer.WriteString(itemName);
			writer.WriteLine(L")");
			writer.WriteLine(L"{");

			writer.WriteString(L"\tWString expected = L\"");
			writer.WriteString(itemResult);
			writer.WriteLine(L"\";");

			writer.WriteString(L"\tWString actual = ::vl_workflow_global::");
			writer.WriteString(itemName);
			writer.WriteLine(L"::Instance().main();");

			writer.WriteLine(L"\tConsole::WriteLine(L\"    expected : \" + expected);");
			writer.WriteLine(L"\tConsole::WriteLine(L\"    actual   : \" + actual);");
			writer.WriteLine(L"\tTEST_ASSERT(actual == expected);");

			writer.WriteLine(L"}");
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
