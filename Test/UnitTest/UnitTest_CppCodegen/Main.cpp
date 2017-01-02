#include "../../../Import/Vlpp.h"

using namespace vl;

#if defined VCZH_MSVC
int wmain(int argc, wchar_t* argv[])
#elif defined VCZH_GCC
int main()
#endif
{
	unittest::UnitTest::RunAndDisposeTests();
	ThreadLocalStorage::DisposeStorages();
#if defined VCZH_MSVC && defined VCZH_CHECK_MEMORY_LEAKS
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}