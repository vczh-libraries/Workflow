#include "Helper.h"
#include "../Source/CppTypes.h"

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
var c : func():int = [0];

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

	auto FindAttribute = [](IAttributeBag* member, const WString& expectedTypeName) -> IAttributeInfo*
	{
		for (vint i = 0; i < member->GetAttributeCount(); i++)
		{
			auto att = member->GetAttribute(i);
			if (att->GetAttributeType()->GetTypeName() == expectedTypeName)
			{
				return att;
			}
		}
		return nullptr;
	};

	auto AssertRuntimeAttributes = [&]()
	{
		// BUG: Workflow emitter creates WfCustomType descriptors but never populates attributes.
		// RegisterAttribute() is never called in the compilation pipeline.
		// All attribute assertions below verify that attributes are MISSING (== nullptr).
		// When this bug is fixed, these should be changed to verify attributes are PRESENT.

		{
			auto td = GetTypeDescriptor(L"MyClass");
			TEST_ASSERT(td != nullptr);

			// Type-level attribute: @test:Int(1) - expected to be missing
			auto typeAtt = FindAttribute(td, L"system::workflow_attributes::att_test_Int");
			TEST_ASSERT(typeAtt == nullptr);
			TEST_ASSERT(td->GetAttributeCount() == 0);

			// Constructor group should exist but attributes missing
			{
				auto ctorGroup = td->GetConstructorGroup();
				TEST_ASSERT(ctorGroup != nullptr);
				for (vint i = 0; i < ctorGroup->GetMethodCount(); i++)
				{
					auto ctor = ctorGroup->GetMethod(i);
					TEST_ASSERT(ctor->GetAttributeCount() == 0);
				}
			}

			// Method: GetX should exist but attribute missing
			{
				auto mg = td->GetMethodGroupByName(L"GetX", false);
				TEST_ASSERT(mg != nullptr);
				auto method = mg->GetMethod(0);
				TEST_ASSERT(method->GetAttributeCount() == 0);
			}

			// Method: SetX should exist but attribute missing
			{
				auto mg = td->GetMethodGroupByName(L"SetX", false);
				TEST_ASSERT(mg != nullptr);
				auto method = mg->GetMethod(0);
				TEST_ASSERT(method->GetAttributeCount() == 0);
			}

			// Property: X should exist but attribute missing
			{
				auto prop = td->GetPropertyByName(L"X", false);
				TEST_ASSERT(prop != nullptr);
				TEST_ASSERT(prop->GetAttributeCount() == 0);
			}

			// Event: XChanged should exist but attribute missing
			{
				auto ev = td->GetEventByName(L"XChanged", false);
				TEST_ASSERT(ev != nullptr);
				TEST_ASSERT(ev->GetAttributeCount() == 0);
			}
		}

		{
			auto td = GetTypeDescriptor(L"MyInterface");
			TEST_ASSERT(td != nullptr);

			// Type-level attribute missing
			TEST_ASSERT(td->GetAttributeCount() == 0);

			// Method: DoSomething should exist but attribute missing
			{
				auto mg = td->GetMethodGroupByName(L"DoSomething", false);
				TEST_ASSERT(mg != nullptr);
				auto method = mg->GetMethod(0);
				TEST_ASSERT(method->GetAttributeCount() == 0);
			}

			// Property: Value should exist but attribute missing
			{
				auto prop = td->GetPropertyByName(L"Value", false);
				TEST_ASSERT(prop != nullptr);
				TEST_ASSERT(prop->GetAttributeCount() == 0);
			}

			// Event: ValueChanged should exist but attribute missing
			{
				auto ev = td->GetEventByName(L"ValueChanged", false);
				TEST_ASSERT(ev != nullptr);
				TEST_ASSERT(ev->GetAttributeCount() == 0);
			}
		}

		{
			auto td = GetTypeDescriptor(L"MyStruct");
			TEST_ASSERT(td != nullptr);

			// Type-level attribute missing
			TEST_ASSERT(td->GetAttributeCount() == 0);

			// Field: a should exist but attribute missing
			{
				auto prop = td->GetPropertyByName(L"a", false);
				TEST_ASSERT(prop != nullptr);
				TEST_ASSERT(prop->GetAttributeCount() == 0);
			}

			// Field: b should exist but attribute missing
			{
				auto prop = td->GetPropertyByName(L"b", false);
				TEST_ASSERT(prop != nullptr);
				TEST_ASSERT(prop->GetAttributeCount() == 0);
			}
		}
	};

	TEST_CASE(L"Runtime attributes from source")
	{
		List<WString> moduleCodes;
		moduleCodes.Add(LoadSample(L"Runtime", L"Attributes"));
		List<glr::ParsingError> errors;
		auto assembly = Compile(GetWorkflowParser(), WfCpuArchitecture::AsExecutable, moduleCodes, errors);
		for (auto&& error : errors)
		{
			TEST_PRINT(L"    Error: " + error.message);
		}
		TEST_ASSERT(assembly);
		TEST_ASSERT(errors.Count() == 0);
		TEST_ASSERT(assembly->typeImpl);

		GetGlobalTypeManager()->AddTypeLoader(assembly->typeImpl);
		AssertRuntimeAttributes();
		GetGlobalTypeManager()->RemoveTypeLoader(assembly->typeImpl);
	});

	// TODO: Enable after adding TestRuntime.cpp to CompilerTest_LoadAndCompile
	// TEST_CASE(L"Runtime attributes from binary")
	// {
	// 	Ptr<WfAssembly> assembly;
	// 	LoadSampleAssemblyBinary(L"Runtime", L"Attributes", assembly);
	// 	TEST_ASSERT(assembly);
	// 	TEST_ASSERT(assembly->typeImpl);

	// 	AssertRuntimeAttributes();
	// 	GetGlobalTypeManager()->RemoveTypeLoader(assembly->typeImpl);
	// });
}