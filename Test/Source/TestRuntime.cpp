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

#define DECODE_CODEGEN_NAME(EXIT_STATEMENT)\
	WString itemName, itemResult;\
	bool cppCodegen;\
	if (!DecodeCodegenName(codegenName, itemName, itemResult, cppCodegen))\
	{\
		EXIT_STATEMENT;\
	}\

TEST_FILE
{
	TEST_CASE(L"Hello, world!")
	{
		List<glr::ParsingError> errors;
		List<WString> moduleCodes;
		moduleCodes.Add(LR"workflow(
module test;

/* types conflicted with BindNull */
interface A{}
class B{}

var a = new A^{};
var b = new B^();

func main():string
{
	return "Hello, world!";
}
)workflow");

		auto&& parser = GetWorkflowParser();
		auto assembly = Compile(parser, WfCpuArchitecture::AsExecutable, moduleCodes, errors);
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
	});

	TEST_CATEGORY(L"Runtime with Binary Assembly")
	{
		auto&& parser = GetWorkflowParser();
		List<WString> codegenNames, reflectableAssemblies;
		Dictionary<WString, WString> assemblyEntries;
		LoadSampleIndex(L"Codegen", codegenNames);

		for (auto codegenName : codegenNames)
		{
			TEST_CASE(codegenName)
			{
				DECODE_CODEGEN_NAME(return)

				TEST_PRINT(itemName);
				Ptr<WfAssembly> assembly;
				LoadSampleAssemblyBinary(L"Codegen", itemName, assembly);
				
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
				TEST_PRINT(L"    expected  : " + itemResult);
				TEST_PRINT(L"    actual    : " + actual);
				TEST_ASSERT(actual == itemResult);
				TEST_ASSERT(context.PopValue(result) == WfRuntimeThreadContextError::EmptyStack);
			});
		}
	});
}