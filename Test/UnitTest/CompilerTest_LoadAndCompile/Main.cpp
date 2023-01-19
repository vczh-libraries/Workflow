#include "../../Source/Helper.h"

#if defined VCZH_MSVC
using namespace vl;
using namespace vl::stream;
using namespace vl::filesystem;
using namespace vl::reflection;
using namespace vl::reflection::description;
using namespace vl::workflow::analyzer;
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
			auto first = File(GetTestOutputBasePath() + REFLECTION_BASELINE()).ReadAllTextByBom();
			auto second = File(GetTestOutputBasePath() + REFLECTION_OUTPUT()).ReadAllTextByBom();
			TEST_ASSERT(first == second);
		}
	});
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
	{
		Folder folder(GetWorkflowOutputPath());
		if (!folder.Exists())
		{
			CHECK_ERROR(folder.Create(false) == true, L"Failed to create GetWorkflowOutputPath()");
		}
		else
		{
			List<File> files;
			CHECK_ERROR(folder.GetFiles(files) == true, L"Failed to get files from GetWorkflowOutputPath()");
			for (auto file : files)
			{
				CHECK_ERROR(file.Delete() == true, L"Failed to delete file");
			}
		}
	}
	{
		Folder folder(GetCppOutputPath());
		if (!folder.Exists())
		{
			CHECK_ERROR(folder.Create(false) == true, L"Failed to create GetCppOutputPath()");
		}
		else
		{
			List<File> files;
			CHECK_ERROR(folder.GetFiles(files) == true, L"Failed to get files from GetCppOutputPath()");
			for (auto file : files)
			{
				CHECK_ERROR(file.Delete() == true, L"Failed to delete file");
			}
		}
	}
#endif
	int result32 = 0;
	int result64 = 0;

	{
		testCpuArchitecture = WfCpuArchitecture::x86;
		LoadTypes();
		result32 = unittest::UnitTest::RunAndDisposeTests(argc, argv);
		UnloadTypes();
	}

	{
		testCpuArchitecture = WfCpuArchitecture::x64;
		LoadTypes();
		result64 = unittest::UnitTest::RunAndDisposeTests(argc, argv);
		UnloadTypes();
	}
	ReleaseWorkflowTable();
	ThreadLocalStorage::DisposeStorages();
#if defined VCZH_MSVC && defined VCZH_CHECK_MEMORY_LEAKS
	_CrtDumpMemoryLeaks();
#endif
	return result32 + result64;
}
