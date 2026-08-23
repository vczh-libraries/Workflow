#include "../../Source/TestCasesRpcStdio_Service.h"

#ifdef VCZH_MSVC
#include <crtdbg.h>
#endif

using namespace vl;
using namespace vl::rpc_controller_test;

#if defined VCZH_MSVC
int wmain(int argc, wchar_t* argv[])
#elif defined VCZH_GCC
int main(int argc, char* argv[])
#endif
{
	CHECK_ERROR(argc == 2, L"Usage: RpcStdioTest_Service <rpc-test-case-name>");
#if defined VCZH_MSVC
	auto itemName = WString::Unmanaged(argv[1]);
#elif defined VCZH_GCC
	auto itemName = atow(AString::Unmanaged(argv[1]));
#endif
	RunTestCasesRpcStdio_Service(itemName);
	ThreadLocalStorage::DisposeStorages();
	FinalizeGlobalStorage();
#if defined VCZH_MSVC && VCZH_CHECK_MEMORY_LEAKS
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}
