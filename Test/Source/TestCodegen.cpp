#include "TestFunctions.h"

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
			LogSampleParseResult(L"Codegen", itemName, reader.ReadToEnd(), node);
		}
		TEST_ASSERT(manager.errors.Count() == 0);
		
		Ptr<WfAssembly> assembly = GenerateAssembly(&manager);
		TEST_ASSERT(assembly);
		LogSampleCodegenResult(L"Codegen", itemName, assembly);
		{
			MemoryStream stream;
			assembly->Serialize(stream);
			UnitTest::PrintInfo(L"    serialized: " + i64tow(stream.Size()) + L" bytes");
			stream.SeekFromBegin(0);
			assembly = new WfAssembly(stream);
		}

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
		TEST_ASSERT(context.PopValue(result) == WfRuntimeThreadContextError::Success);
		UnitTest::PrintInfo(L"    expected: " + itemResult);
		UnitTest::PrintInfo(L"    actual: " + result.GetText());
		TEST_ASSERT(result.GetText() == itemResult);
		TEST_ASSERT(context.PopValue(result) == WfRuntimeThreadContextError::EmptyStack);
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
	TEST_ASSERT(context.PopValue(result) == WfRuntimeThreadContextError::Success);
	TEST_ASSERT(result.GetText() == L"Hello, world!");
}
