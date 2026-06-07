#include "../../Source/Helper.h"
#include "../../../Source/Cpp/WfMergeCpp.h"

#if defined VCZH_MSVC
using namespace vl;
using namespace vl::stream;
using namespace vl::filesystem;
using namespace vl::reflection;
using namespace vl::reflection::description;
using namespace vl::workflow::analyzer;
using namespace vl::workflow::cppcodegen;
#endif

#define INSTALL_SERIALIZABLE_TYPE(TYPE)\
	serializableTypes.Add(TypeInfo<TYPE>::content.typeName, Ptr(new SerializableType<TYPE>));

Ptr<ITypeLoader> testTypeLoader;

const wchar_t* REFLECTION_BIN()
{
	switch (testCpuArchitecture)
	{
	case WfCpuArchitecture::x86: return L"Reflection32.bin";
	case WfCpuArchitecture::x64: return L"Reflection64.bin";
	default:
		CHECK_FAIL(L"The CPU architecture is specified.");
	}
}

const wchar_t* REFLECTION_OUTPUT()
{
	switch (testCpuArchitecture)
	{
	case WfCpuArchitecture::x86: return L"Reflection32[2].txt";
	case WfCpuArchitecture::x64: return L"Reflection64[2].txt";
	default:
		CHECK_FAIL(L"The CPU architecture is specified.");
	}
}

const wchar_t* REFLECTION_BASELINE()
{
	switch (testCpuArchitecture)
	{
	case WfCpuArchitecture::x86: return L"Reflection32.txt";
	case WfCpuArchitecture::x64: return L"Reflection64.txt";
	default:
		CHECK_FAIL(L"The CPU architecture is specified.");
	}
}

void LoadTypes()
{
	{
		collections::Dictionary<WString, Ptr<ISerializableType>> serializableTypes;
		REFLECTION_PREDEFINED_SERIALIZABLE_TYPES(INSTALL_SERIALIZABLE_TYPE)
		FileStream fileStream(GetTestOutputBasePath() + REFLECTION_BIN(), FileStream::ReadOnly);
		testTypeLoader = LoadMetaonlyTypes(fileStream, serializableTypes);
	}
	GetGlobalTypeManager()->AddTypeLoader(testTypeLoader);
	CHECK_ERROR(GetGlobalTypeManager()->Load(), L"Failed to load types");
}

void LoadTypes64()
{
	{
		collections::Dictionary<WString, Ptr<ISerializableType>> serializableTypes;
		REFLECTION_PREDEFINED_SERIALIZABLE_TYPES(INSTALL_SERIALIZABLE_TYPE)
			FileStream fileStream(GetTestOutputBasePath() + REFLECTION_BIN(), FileStream::ReadOnly);
		testTypeLoader = LoadMetaonlyTypes(fileStream, serializableTypes);
	}
	GetGlobalTypeManager()->AddTypeLoader(testTypeLoader);
	CHECK_ERROR(GetGlobalTypeManager()->Load(), L"Failed to load types");
}

void UnloadTypes()
{
	CHECK_ERROR(GetGlobalTypeManager()->Unload(), L"Failed to unload types");
	CHECK_ERROR(ResetGlobalTypeManager(), L"Failed to reset type manager");
	testTypeLoader = nullptr;
}

void AssertLines(const WString& expectedString, const WString& actualString)
{
	List<WString> expected, actual;
	{
		StringReader reader(expectedString);
		while (!reader.IsEnd())
		{
			expected.Add(reader.ReadLine());
		}
	}
	{
		StringReader reader(actualString);
		while (!reader.IsEnd())
		{
			actual.Add(reader.ReadLine());
		}
	}

	TEST_ASSERT(expected.Count() == actual.Count());
	for (vint i = 0; i < expected.Count(); i++)
	{
		auto se = expected[i];
		auto sa = actual[i];
		TEST_ASSERT(se == sa);
	}
}

TEST_FILE
{
	TEST_CASE(L"Run LoadMetaonlyTypes()")
	{
		{
			FileStream fileStream(GetTestOutputBasePath() + REFLECTION_OUTPUT(), FileStream::WriteOnly);
			BomEncoder encoder(BomEncoder::Utf8);
			EncoderStream encoderStream(fileStream, encoder);
			StreamWriter writer(encoderStream);
			LogTypeManager(writer);
		}
		{
			auto expectedString = File(GetTestOutputBasePath() + REFLECTION_BASELINE()).ReadAllTextByBom();
			auto actualString = File(GetTestOutputBasePath() + REFLECTION_OUTPUT()).ReadAllTextByBom();
			AssertLines(expectedString, actualString);
		}
	});
}

void FillFileNames(const WString& path, SortedList<WString>& fileNames)
{
	Folder folder(path);
	CHECK_ERROR(folder.Exists(), L"Folder does not exist");

	List<File> files;
	CHECK_ERROR(folder.GetFiles(files), L"Failed to enumerate files");

	for (auto file : files)
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
#if defined VCZH_MSVC
	{
		Folder folder(GetTestOutputBasePath());
		if (!folder.Exists())
		{
			CHECK_ERROR(folder.Create(false) == true, L"Failed to create GetTestOutputBasePath()");
		}
	}
	auto createOrCleanFolder = [](const WString& path, const wchar_t* msgCreate, const wchar_t* msgGetFiles, const wchar_t* msgDelete)
	{
		Folder folder(path);
		if (!folder.Exists())
		{
			CHECK_ERROR(folder.Create(true) == true, msgCreate);
		}
		else
		{
			List<File> files;
			CHECK_ERROR(folder.GetFiles(files) == true, msgGetFiles);
			for (auto file : files)
			{
				CHECK_ERROR(file.Delete() == true, msgDelete);
			}
		}
	};

#define OUTPUT_FOLDERS(FOLDER)\
	FOLDER(GetCppOutputPath32)\
	FOLDER(GetCppOutputPath64)\
	FOLDER(GetCppOutputPath32Rpc)\
	FOLDER(GetCppOutputPath64Rpc)\
	FOLDER(GetWorkflowOutputPath32)\
	FOLDER(GetWorkflowOutputPath64)\
	FOLDER(GetRpcMetadataOutputPath32)\
	FOLDER(GetRpcMetadataOutputPath64)

#define CLEAN_OUTPUT_FOLDER(FUNC)\
	createOrCleanFolder(FUNC(),\
		L"Failed to create output folder.",\
		L"Failed to get files from output folder.",\
		L"Failed to delete file from output folder.");

	OUTPUT_FOLDERS(CLEAN_OUTPUT_FOLDER)

	createOrCleanFolder(GetAppOutputPath32(L"ChatBot"),
		L"Failed to create app output folder.",
		L"Failed to get files from app output folder.",
		L"Failed to delete file from app output folder.");
	createOrCleanFolder(GetAppOutputPath64(L"ChatBot"),
		L"Failed to create app output folder.",
		L"Failed to get files from app output folder.",
		L"Failed to delete file from app output folder.");
	{
		Folder folder(GetAppMergePath(L"ChatBot"));
		if (!folder.Exists())
		{
			CHECK_ERROR(folder.Create(true) == true, L"Failed to create app merge folder.");
		}
	}

#undef CLEAN_OUTPUT_FOLDER
#undef OUTPUT_FOLDERS
#endif
	int result32 = 0;
	int result64 = 0;

	{
		console::Console::WriteLine(L"<x86>");
		testCpuArchitecture = WfCpuArchitecture::x86;
		LoadTypes();
		result32 = unittest::UnitTest::RunAndDisposeTests(argc, argv);
		UnloadTypes();
	}

	{
		console::Console::WriteLine(L"<x64>");
		testCpuArchitecture = WfCpuArchitecture::x64;
		LoadTypes();
		result64 = unittest::UnitTest::RunAndDisposeTests(argc, argv);
		UnloadTypes();
	}

	{
		console::Console::WriteLine(L"<Merging>");
		SortedList<WString> fileNames32;
		SortedList<WString> fileNames64;
		{
			FillFileNames(GetCppOutputPath32(), fileNames32);
			FillFileNames(GetCppOutputPath64(), fileNames64);
			CHECK_ERROR(CompareEnumerable(fileNames32, fileNames64) == 0, L"File names in x64 and x86 folder are different.");
		}

		for (auto fileName : fileNames32)
		{
			console::Console::WriteLine(L"    " + fileName);
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

	{
		console::Console::WriteLine(L"<Merging Rpc>");
		SortedList<WString> fileNames32;
		SortedList<WString> fileNames64;
		{
			FillFileNames(GetCppOutputPath32Rpc(), fileNames32);
			FillFileNames(GetCppOutputPath64Rpc(), fileNames64);
			CHECK_ERROR(CompareEnumerable(fileNames32, fileNames64) == 0, L"Rpc file names in x64 and x86 folder are different.");
		}

		for (auto fileName : fileNames32)
		{
			console::Console::WriteLine(L"    " + fileName);
			auto file32 = GetCppOutputPath32Rpc() + fileName;
			auto file64 = GetCppOutputPath64Rpc() + fileName;
			auto fileOutput = GetCppMergePathRpc() + fileName;

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

	{
		console::Console::WriteLine(L"<Merging Apps: ChatBot>");
		SortedList<WString> fileNames32;
		SortedList<WString> fileNames64;
		{
			FillFileNames(GetAppOutputPath32(L"ChatBot"), fileNames32);
			FillFileNames(GetAppOutputPath64(L"ChatBot"), fileNames64);
			CHECK_ERROR(CompareEnumerable(fileNames32, fileNames64) == 0, L"App file names in x64 and x86 folder are different.");
		}

		for (auto fileName : fileNames32)
		{
			console::Console::WriteLine(L"    " + fileName);
			auto file32 = GetAppOutputPath32(L"ChatBot") + fileName;
			auto file64 = GetAppOutputPath64(L"ChatBot") + fileName;
			auto fileOutput = GetAppMergePath(L"ChatBot") + fileName;

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

	ReleaseWorkflowTable();
	ThreadLocalStorage::DisposeStorages();
	unittest::UnitTest::DumpMemoryLeak(argc, argv);
	if (result32 != 0)
	{
		unittest::UnitTest::PrintMessage(L"Win32 test run failed, please go back and check the result", unittest::UnitTest::MessageKind::Error);
	}
	if (result64 != 0)
	{
		unittest::UnitTest::PrintMessage(L"x64 test run failed, please go back and check the result", unittest::UnitTest::MessageKind::Error);
	}
	return result32 + result64;
}
