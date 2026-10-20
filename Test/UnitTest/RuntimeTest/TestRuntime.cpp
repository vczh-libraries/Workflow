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

	TEST_CASE(L"Workflow method callbacks accept native argument lists")
	{
		List<glr::ParsingError> errors;
		List<WString> moduleCodes;
		moduleCodes.Add(LR"workflow(
module method_callbacks;

class MethodCallbackTarget
{
  var offset : int = 1000;

  func Combine(first : int, second : int) : int
  {
    return offset + first * 100 + second;
  }

  static func Empty() : string
  {
    return "empty";
  }
}

var target = new MethodCallbackTarget^();

func GetCallback() : func(int, int):int
{
  return target.Combine;
}

func GetEmptyCallback() : func():string
{
  return MethodCallbackTarget::Empty;
}
)workflow");

		auto&& parser = GetWorkflowParser();
		auto assembly = Compile(parser, WfCpuArchitecture::AsExecutable, moduleCodes, errors);
		TEST_ASSERT(errors.Count() == 0);

		auto context = Ptr(new WfRuntimeGlobalContext(assembly));
		LoadFunction<void()>(context, L"<initialize>")();
		auto callback = LoadFunction<Func<vint(vint, vint)>()>(context, L"GetCallback")();
		TEST_ASSERT(callback(12, 34) == 2234);
		TEST_ASSERT(callback(4, 56) == 1456);
		auto emptyCallback = LoadFunction<Func<WString()>()>(context, L"GetEmptyCallback")();
		TEST_ASSERT(emptyCallback() == L"empty");
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

	TEST_CASE(L"Ordinary strong null casts preserve the VM baseline")
	{
		Ptr<WfAssembly> assembly;
		LoadSampleAssemblyBinary(L"Codegen", L"OrdinaryCastBaseline", assembly);
		auto context = Ptr(new WfRuntimeGlobalContext(assembly));
		LoadFunction<void()>(context, L"<initialize>")();
		for (auto name : { L"StrongRawNull", L"StrongSharedNull", L"StrongNullableNull" })
		{
			TEST_ASSERT(LoadFunction<bool()>(context, name)());
		}
	});

	TEST_CASE(L"An escaped variant cannot be extracted by an ordinary VM cast")
	{
		Ptr<WfAssembly> assembly;
		LoadSampleAssemblyBinary(L"Codegen", L"VariantCasts", assembly);
		auto context = Ptr(new WfRuntimeGlobalContext(assembly));
		LoadFunction<void()>(context, L"<initialize>")();
		TEST_EXCEPTION(LoadFunction<vint()>(context, L"EscapedPayload")(), WfRuntimeException, [](const WfRuntimeException& exception)
		{
			TEST_ASSERT(!exception.IsFatal());
		});
	});

	TEST_CASE(L"Variant instruction type operands survive independent assembly loads")
	{
		Ptr<WfAssembly> first, second;
		LoadSampleAssemblyBinary(L"Codegen", L"VariantInstructionTypes", first);
		LoadSampleAssemblyBinary(L"Codegen", L"VariantInstructionTypes", second);
		TEST_ASSERT(first->instructions.Count() == second->instructions.Count());
		vint boxes = 0;
		vint casts = 0;
		for (vint i = 0; i < first->instructions.Count(); i++)
		{
			auto& instruction = first->instructions[i];
			auto type = instruction.typeInfoParameter;
			if (!type) continue;
			auto reloaded = second->instructions[i].typeInfoParameter;
			TEST_ASSERT(type != reloaded);
			TEST_ASSERT(IsSameTypeInfo(type.Obj(), reloaded.Obj()));
			if (instruction.code == WfInsCode::VBox)
			{
				boxes++;
				TEST_ASSERT(type->GetDecorator() == ITypeInfo::RawPtr);
				TEST_ASSERT(type->GetTypeDescriptor()->GetTypeName() == L"test::ObservableValue");
			}
			else if (instruction.code == WfInsCode::VCast)
			{
				casts++;
				if (type->GetDecorator() == ITypeInfo::Variant)
				{
					TEST_ASSERT(!type->GetTypeDescriptor());
					TEST_ASSERT(!type->GetElementType());
					TEST_ASSERT(type->GetGenericArgumentCount() == 2);
					TEST_ASSERT(type->GetGenericArgument(0)->GetTypeFriendlyName() == L"test::PointClass*");
					TEST_ASSERT(type->GetGenericArgument(1)->GetTypeFriendlyName() == L"test::ObservableValue*");
				}
				else
				{
					TEST_ASSERT(type->GetTypeFriendlyName() == L"test::ObservableValue*");
				}
			}
			else
			{
				TEST_ASSERT(false);
			}
		}
		TEST_ASSERT(boxes == 1);
		TEST_ASSERT(casts == 2);
	});

}
