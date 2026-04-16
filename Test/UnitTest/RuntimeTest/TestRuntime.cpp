#include "../../Source/Helper.h"

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
		{
			auto td = GetTypeDescriptor(L"MyClass");
			TEST_ASSERT(td != nullptr);

			// Type-level: @test:Int(1)
			{
				auto att = FindAttribute(td, L"system::workflow_attributes::att_test_Int");
				TEST_ASSERT(att != nullptr);
				TEST_ASSERT(att->GetAttributeValueCount() == 1);
				TEST_ASSERT(UnboxValue<vint>(att->GetAttributeValue(0)) == 1);
			}

			// Constructors: @test:Int(10) new() and @test:Int(20) new(a:int)
			{
				auto ctorGroup = td->GetConstructorGroup();
				TEST_ASSERT(ctorGroup != nullptr);
				bool foundCtor0 = false;
				bool foundCtor1 = false;
				for (vint i = 0; i < ctorGroup->GetMethodCount(); i++)
				{
					auto ctor = ctorGroup->GetMethod(i);
					auto att = FindAttribute(ctor, L"system::workflow_attributes::att_test_Int");
					if (att)
					{
						auto val = UnboxValue<vint>(att->GetAttributeValue(0));
						if (ctor->GetParameterCount() == 0)
						{
							TEST_ASSERT(val == 10);
							foundCtor0 = true;
						}
						else if (ctor->GetParameterCount() == 1)
						{
							TEST_ASSERT(val == 20);
							foundCtor1 = true;
						}
					}
				}
				TEST_ASSERT(foundCtor0);
				TEST_ASSERT(foundCtor1);
			}

			// Method: @test:Range("hello") func GetX()
			{
				auto mg = td->GetMethodGroupByName(L"GetX", false);
				TEST_ASSERT(mg != nullptr);
				auto method = mg->GetMethod(0);
				auto att = FindAttribute(method, L"system::workflow_attributes::att_test_Range");
				TEST_ASSERT(att != nullptr);
				TEST_ASSERT(UnboxValue<WString>(att->GetAttributeValue(0)) == L"hello");
			}

			// Method: @test:List("setter") func SetX(value : int)
			{
				auto mg = td->GetMethodGroupByName(L"SetX", false);
				TEST_ASSERT(mg != nullptr);
				auto method = mg->GetMethod(0);
				auto att = FindAttribute(method, L"system::workflow_attributes::att_test_List");
				TEST_ASSERT(att != nullptr);
				TEST_ASSERT(UnboxValue<WString>(att->GetAttributeValue(0)) == L"setter");
			}

			// Property: @test:Map(2.5) prop X
			{
				auto prop = td->GetPropertyByName(L"X", false);
				TEST_ASSERT(prop != nullptr);
				auto att = FindAttribute(prop, L"system::workflow_attributes::att_test_Map");
				TEST_ASSERT(att != nullptr);
				TEST_ASSERT(UnboxValue<double>(att->GetAttributeValue(0)) == 2.5);
			}

			// Event: @test:Int(100) event XChanged
			{
				auto ev = td->GetEventByName(L"XChanged", false);
				TEST_ASSERT(ev != nullptr);
				auto att = FindAttribute(ev, L"system::workflow_attributes::att_test_Int");
				TEST_ASSERT(att != nullptr);
				TEST_ASSERT(UnboxValue<vint>(att->GetAttributeValue(0)) == 100);
			}
		}

		{
			auto td = GetTypeDescriptor(L"MyInterface");
			TEST_ASSERT(td != nullptr);

			// Type-level: @test:List("my-interface")
			{
				auto att = FindAttribute(td, L"system::workflow_attributes::att_test_List");
				TEST_ASSERT(att != nullptr);
				TEST_ASSERT(UnboxValue<WString>(att->GetAttributeValue(0)) == L"my-interface");
			}

			// Method: @test:Int(42) func DoSomething()
			{
				auto mg = td->GetMethodGroupByName(L"DoSomething", false);
				TEST_ASSERT(mg != nullptr);
				auto method = mg->GetMethod(0);
				auto att = FindAttribute(method, L"system::workflow_attributes::att_test_Int");
				TEST_ASSERT(att != nullptr);
				TEST_ASSERT(UnboxValue<vint>(att->GetAttributeValue(0)) == 42);
			}

			// Property: @test:Range("prop-value") prop Value
			{
				auto prop = td->GetPropertyByName(L"Value", false);
				TEST_ASSERT(prop != nullptr);
				auto att = FindAttribute(prop, L"system::workflow_attributes::att_test_Range");
				TEST_ASSERT(att != nullptr);
				TEST_ASSERT(UnboxValue<WString>(att->GetAttributeValue(0)) == L"prop-value");
			}

			// Event: @test:Map(3.14) event ValueChanged
			{
				auto ev = td->GetEventByName(L"ValueChanged", false);
				TEST_ASSERT(ev != nullptr);
				auto att = FindAttribute(ev, L"system::workflow_attributes::att_test_Map");
				TEST_ASSERT(att != nullptr);
				TEST_ASSERT(UnboxValue<double>(att->GetAttributeValue(0)) == 3.14);
			}
		}

		{
			auto td = GetTypeDescriptor(L"MyStruct");
			TEST_ASSERT(td != nullptr);

			// Type-level: @test:Range("my-struct")
			{
				auto att = FindAttribute(td, L"system::workflow_attributes::att_test_Range");
				TEST_ASSERT(att != nullptr);
				TEST_ASSERT(UnboxValue<WString>(att->GetAttributeValue(0)) == L"my-struct");
			}

			// Field: @test:Int(7) a : int
			{
				auto prop = td->GetPropertyByName(L"a", false);
				TEST_ASSERT(prop != nullptr);
				auto att = FindAttribute(prop, L"system::workflow_attributes::att_test_Int");
				TEST_ASSERT(att != nullptr);
				TEST_ASSERT(UnboxValue<vint>(att->GetAttributeValue(0)) == 7);
			}

			// Field: @test:List("field-b") b : string
			{
				auto prop = td->GetPropertyByName(L"b", false);
				TEST_ASSERT(prop != nullptr);
				auto att = FindAttribute(prop, L"system::workflow_attributes::att_test_List");
				TEST_ASSERT(att != nullptr);
				TEST_ASSERT(UnboxValue<WString>(att->GetAttributeValue(0)) == L"field-b");
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

	TEST_CASE(L"Runtime attributes from binary")
	{
		Ptr<WfAssembly> assembly;
		LoadSampleAssemblyBinary(L"Runtime", L"Attributes", assembly);
		TEST_ASSERT(assembly);
		TEST_ASSERT(assembly->typeImpl);

		GetGlobalTypeManager()->AddTypeLoader(assembly->typeImpl);
		AssertRuntimeAttributes();
		GetGlobalTypeManager()->RemoveTypeLoader(assembly->typeImpl);
	});
}