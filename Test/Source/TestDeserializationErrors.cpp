#include "Helper.h"

TEST_CASE(TestUnresolvedTypesError)
{
	const wchar_t* scriptA = LR"(
module A;

class A
{
}
)";

	const wchar_t* scriptB = LR"(
module B;

class B
{
  prop a: A^ = null{}
}
)";

	MemoryStream streamA, streamB;
	{
		List<WString> codes;
		List<Ptr<ParsingError>> errors;
		codes.Add(scriptA);
		auto assembly = Compile(GetWorkflowTable(), codes, errors);
		TEST_ASSERT(assembly);
		assembly->Serialize(streamA);
		streamA.SeekFromBegin(0);
	}
	{
		Ptr<WfAssembly> assemblyA;
		{
			WfAssemblyLoadErrors errors;
			assemblyA = WfAssembly::Deserialize(streamA, errors);
			TEST_ASSERT(assemblyA);
			TEST_ASSERT(errors.unresolvedTypes.Count() == 0);
			TEST_ASSERT(errors.duplicatedTypes.Count() == 0);
		}
		WfRuntimeGlobalContext globalContext(assemblyA);

		List<WString> codes;
		List<Ptr<ParsingError>> errors;
		codes.Add(scriptB);
		auto assembly = Compile(GetWorkflowTable(), codes, errors);
		TEST_ASSERT(assembly);
		assembly->Serialize(streamB);
		streamB.SeekFromBegin(0);
	}

	WfAssemblyLoadErrors errors;
	auto assemblyB = WfAssembly::Deserialize(streamB, errors);
	TEST_ASSERT(!assemblyB);
	TEST_ASSERT(errors.unresolvedTypes.Count() == 1);
	TEST_ASSERT(errors.duplicatedTypes.Count() == 0);
	TEST_ASSERT(errors.unresolvedTypes[0] == L"A");
}

TEST_CASE(TestDuplicatedTypesError)
{
	const wchar_t* scriptA = LR"(
module A;

class A
{
}
)";

	MemoryStream streamA;
	{
		List<WString> codes;
		List<Ptr<ParsingError>> errors;
		codes.Add(scriptA);
		auto assembly = Compile(GetWorkflowTable(), codes, errors);
		TEST_ASSERT(assembly);
		assembly->Serialize(streamA);
		streamA.SeekFromBegin(0);
	}

	WfAssemblyLoadErrors errors;
	auto assemblyA = WfAssembly::Deserialize(streamA, errors);
	TEST_ASSERT(assemblyA);
	TEST_ASSERT(errors.unresolvedTypes.Count() == 0);
	TEST_ASSERT(errors.duplicatedTypes.Count() == 0);

	auto assemblyB = WfAssembly::Deserialize(streamA, errors);
	TEST_ASSERT(!assemblyB);
	TEST_ASSERT(errors.unresolvedTypes.Count() == 0);
	TEST_ASSERT(errors.duplicatedTypes.Count() == 1);
	TEST_ASSERT(errors.duplicatedTypes[0] == L"A");
}