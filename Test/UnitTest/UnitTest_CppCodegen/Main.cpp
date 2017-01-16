#include "../../../Import/Vlpp.h"
#include "../../Source/CppTypes.h"

using namespace vl;
using namespace vl::reflection::description;
using namespace test;

void LoadTypes()
{
#ifndef VCZH_DEBUG_NO_REFLECTION
	LoadPredefinedTypes();
	LoadParsingTypes();
	XmlLoadTypes();
	JsonLoadTypes();
	LoadCppTypes();
	TEST_ASSERT(GetGlobalTypeManager()->Load());
#endif
}

void UnloadTypes()
{
#ifndef VCZH_DEBUG_NO_REFLECTION
	TEST_ASSERT(GetGlobalTypeManager()->Unload());
	TEST_ASSERT(ResetGlobalTypeManager());
#endif
}

#if defined VCZH_MSVC
int wmain(int argc, wchar_t* argv[])
#elif defined VCZH_GCC
int main()
#endif
{
	LoadTypes();
	unittest::UnitTest::RunAndDisposeTests();
	UnloadTypes();
	ThreadLocalStorage::DisposeStorages();
#if defined VCZH_MSVC && defined VCZH_CHECK_MEMORY_LEAKS
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}