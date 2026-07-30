#include <VlppGlrParser.h>
#include "../../../Source/Library/WfLibraryReflection.h"
#include "../../Source/CppTypes.h"
#ifdef VCZH_MSVC
#define _WINSOCKAPI_
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
	return GetExePath() + L"../../../Generated/";
#else
	return GetExePath() + L"../../Generated/";
#endif
}
#elif defined VCZH_GCC
WString GetTestOutputPath()
{
	return L"../../Generated/";
}
#endif

#ifdef VCZH_64
#define REFLECTION_BIN L"Reflection64.bin"
#define REFLECTION_OUTPUT L"Reflection64.txt"
#define REFLECTION_BASELINE L"Reflection64.txt"
#define REFLECTION_CPP_TYPES_BIN L"ReflectionCppTypes64.bin"
#define REFLECTION_CPP_TYPES_OUTPUT L"ReflectionCppTypes64.txt"
#define REFLECTION_CPP_TYPES_BASELINE L"ReflectionCppTypes64.txt"
#else
#define REFLECTION_BIN L"Reflection32.bin"
#define REFLECTION_OUTPUT L"Reflection32.txt"
#define REFLECTION_BASELINE L"Reflection32.txt"
#define REFLECTION_CPP_TYPES_BIN L"ReflectionCppTypes32.bin"
#define REFLECTION_CPP_TYPES_OUTPUT L"ReflectionCppTypes32.txt"
#define REFLECTION_CPP_TYPES_BASELINE L"ReflectionCppTypes32.txt"
#endif

void LogTypesAndUpdateBaseline(const WString& outputName, const WString& baselineName)
{
	{
		FileStream fileStream(GetTestOutputPath() + outputName, FileStream::WriteOnly);
		BomEncoder encoder(BomEncoder::Utf8);
		EncoderStream encoderStream(fileStream, encoder);
		StreamWriter writer(encoderStream);
		LogTypeManager(writer);
	}
	{
		List<WString> first, second;
		File metadataFile(GetTestOutputPath() + outputName);
		File baselineFile(GetTestOutputPath() + L"../Resources/Baseline/" + baselineName);

		metadataFile.ReadAllLinesByBom(first);
		if (baselineFile.Exists())
		{
			baselineFile.ReadAllLinesByBom(second);
		}
		if (CompareEnumerable(first, second) != 0)
		{
			baselineFile.WriteAllLines(first, false, BomEncoder::Utf8);
		}
	}
}

TEST_FILE
{
	TEST_CASE_ASSERT(Folder(GetTestOutputPath()).Exists());

	TEST_CASE(L"Run GenerateMetaonlyTypes()")
	{
		TEST_ASSERT(LoadPredefinedTypes());
		TEST_ASSERT(LoadParsing2Types());
		TEST_ASSERT(XmlAstLoadTypes());
		TEST_ASSERT(JsonAstLoadTypes());
		TEST_ASSERT(WfLoadLibraryTypes());
		auto manager = GetGlobalTypeManager();
		TEST_ASSERT(manager->Load());
		{
			List<ITypeDescriptor*> excludedTypes;
			FileStream fileStream(GetTestOutputPath() + REFLECTION_BIN, FileStream::WriteOnly);
			GenerateMetaonlyTypes(excludedTypes, fileStream);
		}
		LogTypesAndUpdateBaseline(REFLECTION_OUTPUT, REFLECTION_BASELINE);

		List<ITypeDescriptor*> baseTypes;
		CollectRegisteredTypes(baseTypes);
		TEST_ASSERT(LoadCppTypes());
		{
			FileStream fileStream(GetTestOutputPath() + REFLECTION_CPP_TYPES_BIN, FileStream::WriteOnly);
			GenerateMetaonlyTypes(baseTypes, fileStream);
		}
		LogTypesAndUpdateBaseline(REFLECTION_CPP_TYPES_OUTPUT, REFLECTION_CPP_TYPES_BASELINE);
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
	unittest::UnitTest::DumpMemoryLeak(argc, argv);
	return result;
}
