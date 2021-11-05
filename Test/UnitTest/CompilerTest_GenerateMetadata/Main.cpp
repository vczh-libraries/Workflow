#include <VlppParser.h>
#include "../../../Source/Library/WfLibraryReflection.h"
#include "../../Source/CppTypes.h"
#ifdef VCZH_MSVC
#include <windows.h>
#endif

using namespace vl;
using namespace vl::collections;
using namespace vl::filesystem;
using namespace vl::stream;
using namespace vl::reflection;
using namespace vl::reflection::description;

#if defined VCZH_MSVC
WString GetExePath()
{
	wchar_t buffer[65536];
	GetModuleFileName(NULL, buffer, sizeof(buffer) / sizeof(*buffer));
	vint pos = -1;
	vint index = 0;
	while (buffer[index])
	{
		if (buffer[index] == L'\\' || buffer[index] == L'/')
		{
			pos = index;
		}
		index++;
	}
	return WString::CopyFrom(buffer, pos + 1);
}

WString GetTestOutputPath()
{
#ifdef _WIN64
	return GetExePath() + L"../../../Output/";
#else
	return GetExePath() + L"../../Output/";
#endif
}
#elif defined VCZH_GCC
WString GetTestOutputPath()
{
	return L"../../Output/";
}
#endif

#ifdef VCZH_64
#define REFLECTION_BIN L"Reflection64.bin"
#define REFLECTION_OUTPUT L"Reflection64.txt"
#define REFLECTION_BASELINE L"Reflection64.txt"
#else
#define REFLECTION_BIN L"Reflection32.bin"
#define REFLECTION_OUTPUT L"Reflection32.txt"
#define REFLECTION_BASELINE L"Reflection32.txt"
#endif

TEST_FILE
{
	TEST_CASE_ASSERT(Folder(GetTestOutputPath()).Exists());

	TEST_CASE(L"Run GenerateMetaonlyTypes()")
	{
		TEST_ASSERT(LoadPredefinedTypes());
		TEST_ASSERT(LoadParsingTypes());
		TEST_ASSERT(XmlLoadTypes());
		TEST_ASSERT(JsonLoadTypes());
		TEST_ASSERT(WfLoadLibraryTypes());
		TEST_ASSERT(LoadCppTypes());
		GetGlobalTypeManager()->Load();
		{
			FileStream fileStream(GetTestOutputPath() + REFLECTION_BIN, FileStream::WriteOnly);
			GenerateMetaonlyTypes(fileStream);
		}
		{
			FileStream fileStream(GetTestOutputPath() + REFLECTION_OUTPUT, FileStream::WriteOnly);
			BomEncoder encoder(BomEncoder::Utf8);
			EncoderStream encoderStream(fileStream, encoder);
			StreamWriter writer(encoderStream);
			LogTypeManager(writer);
		}
		{
			List<WString> first, second;
			File(GetTestOutputPath() + REFLECTION_OUTPUT).ReadAllLinesByBom(first);
			File(GetTestOutputPath() + L"../Resources/Baseline/" REFLECTION_BASELINE).ReadAllLinesByBom(second);
			TEST_ASSERT(CompareEnumerable(first, second) == 0);
		}
		TEST_ASSERT(ResetGlobalTypeManager());
	});
}

#ifdef VCZH_MSVC
int wmain(int argc, wchar_t* argv[])
#elif defined VCZH_GCC
int main(int argc, char* argv[])
#endif
{
#if defined VCZH_MSVC
	{
		Folder folder(GetTestOutputPath());
		if (!folder.Exists())
		{
			folder.Create(false);
		}
	}
#endif
	int result = unittest::UnitTest::RunAndDisposeTests(argc, argv);
	FinalizeGlobalStorage();
#if defined VCZH_MSVC && defined VCZH_CHECK_MEMORY_LEAKS
	_CrtDumpMemoryLeaks();
#endif
	return result;
}