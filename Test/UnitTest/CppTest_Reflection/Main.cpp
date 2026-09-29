#include <VlppGlrParser.h>
#include "../../Source/CppTypes.h"

using namespace vl;
using namespace vl::reflection::description;
using namespace test;

extern void LoadTestCaseTypes();
extern void LoadTestCaseRpcTypes();

void LoadTypes()
{
	LoadPredefinedTypes();
	LoadParsing2Types();
	XmlAstLoadTypes();
	JsonAstLoadTypes();
	WfLoadLibraryTypes();
	LoadCppTypes();
	LoadTestCaseTypes();
	LoadTestCaseRpcTypes();
	CHECK_ERROR(GetGlobalTypeManager()->Load(), L"Failed to load types");
}

void UnloadTypes()
{
	CHECK_ERROR(GetGlobalTypeManager()->Unload(), L"Failed to unload types");
	CHECK_ERROR(ResetGlobalTypeManager(), L"Failed to reset type manager");
}

template<typename T>
int UnitTestMain(int argc, T* argv[])
{
	LoadTypes();
	int result = vl::unittest::UnitTest::RunAndDisposeTests(argc, argv);
	UnloadTypes();
	ThreadLocalStorage::DisposeStorages();
	FinalizeGlobalStorage();
	vl::unittest::UnitTest::DumpMemoryLeak(argc, argv);
	return result;
}

#if defined VCZH_MSVC
int wmain(int argc, wchar_t* argv[])
{
	return UnitTestMain(argc, argv);
}
#elif defined VCZH_GCC
int main(int argc, char* argv[])
{
	return UnitTestMain(argc, argv);
}
#endif

#if defined VCZH_WASM
#include <emscripten.h>
#include <emscripten/bind.h>

EM_JS(int, WasmReportFailure, (const char16_t* text, vint length), {
	return globalThis["vlConsoleFailure"](HEAPU16, text, length);
});

vint WasmMain()
{
	wchar_t name[] = L"UnitTest";
	wchar_t mode[] = L"/D";
	wchar_t* arguments[] = { name, mode };
	return UnitTestMain(2, arguments);
}

vint wasm_main()
{
	try
	{
		WString message;
		try
		{
			return WasmMain();
		}
		catch (const vl::unittest::UnitTestAssertError& error)
		{
			message = error.message;
		}
		catch (const vl::unittest::UnitTestConfigError& error)
		{
			message = error.message;
		}
		catch (const vl::unittest::UnitTestJustCrashError&)
		{
			message = L"The unit test framework stopped after a failure.";
		}
		catch (const Error& error)
		{
			message = error.Description();
		}
		catch (const Exception& error)
		{
			message = error.Message();
		}
		catch (const std::exception& error)
		{
			message = atow(error.what());
		}
		catch (...)
		{
			message = L"Unknown C++ exception.";
		}
		auto text = wtou16(message);
		WasmReportFailure(text.Buffer(), text.Length());
	}
	catch (...)
	{
		// Diagnostics can allocate too; no C++ exception may cross this boundary.
		constexpr char16_t text[] = u"Unable to format the C++ failure diagnostic.";
		WasmReportFailure(text, sizeof(text) / sizeof(*text) - 1);
	}
	return 1;
}

EMSCRIPTEN_BINDINGS(CppApplication)
{
	emscripten::function("wasm_main", &wasm_main);
}
#endif
