#include "../../Source/CppTypes.h"

using namespace vl;
using namespace vl::reflection::description;
using namespace test;

#if defined VCZH_MSVC
int wmain(int argc, wchar_t* argv[])
#elif defined VCZH_GCC
int main(int argc, char* argv[])
#endif
{
	int result = unittest::UnitTest::RunAndDisposeTests(argc, argv);
	ThreadLocalStorage::DisposeStorages();
	FinalizeGlobalStorage();
	unittest::UnitTest::DumpMemoryLeak(argc, argv);
	return result;
}
