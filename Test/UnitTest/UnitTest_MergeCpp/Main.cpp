#include "../../../Source/Cpp/WfCpp.h"
#include <Windows.h>

using namespace vl;
using namespace vl::console;
using namespace vl::collections;
using namespace vl::filesystem;
using namespace vl::stream;
using namespace vl::workflow::cppcodegen;

#if defined VCZH_MSVC
WString GetExePath()
{
	wchar_t buffer[65536];
	GetModuleFileName(NULL, buffer, sizeof(buffer) / sizeof(*buffer));
	vint pos = -1;
	vint index = 0;
	while (buffer[index])
	{
		if (buffer[index] == L'\\')
		{
			pos = index;
		}
		index++;
	}
	return WString(buffer, pos + 1);
}
#endif

WString GetTestOutputPath()
{
#if defined VCZH_MSVC
#ifdef VCZH_64
	return GetExePath() + L"..\\..\\..\\Output\\";
#else
	return GetExePath() + L"..\\..\\Output\\";
#endif
#elif defined VCZH_GCC
	return L"../Output/";
#endif
}

WString GetCppOutputPath32()
{
#if defined VCZH_MSVC
	return GetTestOutputPath() + L"x32\\";
#elif defined VCZH_GCC
	return GetTestOutputPath() + L"x32/";
#endif
}

WString GetCppOutputPath64()
{
#if defined VCZH_MSVC
	return GetTestOutputPath() + L"x64\\";
#elif defined VCZH_GCC
	return GetTestOutputPath() + L"x64/";
#endif
}

WString GetCppMergePath()
{
#if defined VCZH_MSVC
#ifdef VCZH_64
	return GetExePath() + L"..\\..\\..\\SourceCppGen\\";
#else
	return GetExePath() + L"..\\..\\SourceCppGen\\";
#endif
#elif defined VCZH_GCC
	return L"../SourceCppGen/";
#endif
}

void FillFileNames(const WString& path, SortedList<WString>& fileNames)
{
	Folder folder(path);
	TEST_ASSERT(folder.Exists());

	List<File> files;
	TEST_ASSERT(folder.GetFiles(files));

	FOREACH(File, file, files)
	{
		fileNames.Add(file.GetFilePath().GetName());
	}
}

#if defined VCZH_MSVC
int wmain(int argc, wchar_t* argv[])
#elif defined VCZH_GCC
int main(int argc, char* argv[])
#endif
{
	{
		SortedList<WString> fileNames32;
		SortedList<WString> fileNames64;
		{
			FillFileNames(GetCppOutputPath32(), fileNames32);
			FillFileNames(GetCppOutputPath64(), fileNames64);
			CHECK_ERROR(CompareEnumerable(fileNames32, fileNames64) == 0, L"File names in x64 and x86 folder are different.");
		}

		FOREACH(WString, fileName, fileNames32)
		{
			Console::WriteLine(fileName);
			auto file32 = GetCppOutputPath32() + fileName;
			auto file64 = GetCppOutputPath64() + fileName;
			auto fileOutput = GetCppMergePath() + fileName;

			auto code = MergeCppMultiPlatform(File(file32).ReadAllTextByBom(), File(file64).ReadAllTextByBom());

			File file(fileOutput);
			if (file.Exists())
			{
				code = MergeCppFileContent(file.ReadAllTextByBom(), code);
			}

			if (file.Exists())
			{
				auto originalCode = file.ReadAllTextByBom();
				if (originalCode == code)
				{
					continue;
				}
			}

			file.WriteAllText(code, false, BomEncoder::Mbcs);
		}
	}
#if defined VCZH_MSVC && defined VCZH_CHECK_MEMORY_LEAKS
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}
