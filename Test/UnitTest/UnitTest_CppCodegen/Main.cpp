#include "../../../Import/VlppParser.h"
#include "../../Source/CppTypes.h"

using namespace vl;
using namespace vl::reflection::description;
using namespace test;

extern void LoadTestCaseTypes();

void LoadTypes()
{
#ifndef VCZH_DEBUG_NO_REFLECTION
	LoadPredefinedTypes();
	LoadParsingTypes();
	XmlLoadTypes();
	JsonLoadTypes();
	LoadCppTypes();
	LoadTestCaseTypes();
	CHECK_ERROR(GetGlobalTypeManager()->Load(), L"Failed to load types");
#endif
}

void UnloadTypes()
{
#ifndef VCZH_DEBUG_NO_REFLECTION
	CHECK_ERROR(GetGlobalTypeManager()->Unload(), L"Failed to unload types");
	CHECK_ERROR(ResetGlobalTypeManager(), L"Failed to reset type manager");
#endif
}

#if defined VCZH_MSVC
int wmain(int argc, wchar_t* argv[])
#elif defined VCZH_GCC
int main(int argc, char* argv[])
#endif
{
	LoadTypes();
	int result = unittest::UnitTest::RunAndDisposeTests(argc, argv);
	UnloadTypes();
	ThreadLocalStorage::DisposeStorages();
	FinalizeGlobalStorage();
#if defined VCZH_MSVC && defined VCZH_CHECK_MEMORY_LEAKS
	_CrtDumpMemoryLeaks();
#endif
	return result;
}