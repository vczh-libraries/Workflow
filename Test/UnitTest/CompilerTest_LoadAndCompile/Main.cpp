#include "../../Source/Helper.h"

#if defined VCZH_MSVC
using namespace vl;
using namespace vl::stream;
using namespace vl::filesystem;
using namespace vl::reflection;
using namespace vl::reflection::description;
#endif

#ifdef VCZH_64
#define REFLECTION_BIN L"Reflection64.bin"
#define REFLECTION_OUTPUT L"Reflection64[2].txt"
#define REFLECTION_BASELINE L"Reflection64.txt"
#else
#define REFLECTION_BIN L"Reflection32.bin"
#define REFLECTION_OUTPUT L"Reflection32[2].txt"
#define REFLECTION_BASELINE L"Reflection32.txt"
#endif

#define INSTALL_SERIALIZABLE_TYPE(TYPE)\
	serializableTypes.Add(TypeInfo<TYPE>::content.typeName, MakePtr<SerializableType<TYPE>>());

BEGIN_GLOBAL_STORAGE_CLASS(MetaonlyTypeDescriptors)
	Ptr<ITypeLoader>		typeLoader;

INITIALIZE_GLOBAL_STORAGE_CLASS
	collections::Dictionary<WString, Ptr<ISerializableType>> serializableTypes;
	REFLECTION_PREDEFINED_SERIALIZABLE_TYPES(INSTALL_SERIALIZABLE_TYPE)
	FileStream fileStream(GetTestOutputBasePath() + REFLECTION_BIN, FileStream::ReadOnly);
	typeLoader = LoadMetaonlyTypes(fileStream, serializableTypes);

FINALIZE_GLOBAL_STORAGE_CLASS
	typeLoader = nullptr;

END_GLOBAL_STORAGE_CLASS(MetaonlyTypeDescriptors)

//TEST_FILE
//{
//	TEST_CASE(L"Run LoadMetaonlyTypes()")
//	{
//		{
//			FileStream fileStream(GetTestOutputBasePath() + REFLECTION_OUTPUT, FileStream::WriteOnly);
//			BomEncoder encoder(BomEncoder::Utf16);
//			EncoderStream encoderStream(fileStream, encoder);
//			StreamWriter writer(encoderStream);
//			LogTypeManager(writer);
//		}
//		{
//			auto first = File(GetTestOutputBasePath() + REFLECTION_BASELINE).ReadAllTextByBom();
//			auto second = File(GetTestOutputBasePath() + REFLECTION_OUTPUT).ReadAllTextByBom();
//			TEST_ASSERT(first == second);
//		}
//	});
//}

void LoadTypes()
{
	GetGlobalTypeManager()->AddTypeLoader(GetMetaonlyTypeDescriptors().typeLoader);
	CHECK_ERROR(GetGlobalTypeManager()->Load(), L"Failed to load types");
}

void UnloadTypes()
{
	CHECK_ERROR(GetGlobalTypeManager()->Unload(), L"Failed to unload types");
	CHECK_ERROR(ResetGlobalTypeManager(), L"Failed to reset type manager");
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
	LoadTypes();
	int result = unittest::UnitTest::RunAndDisposeTests(argc, argv);
	UnloadTypes();
	FinalizeGlobalStorage();
	ReleaseWorkflowTable();
	ThreadLocalStorage::DisposeStorages();
#if defined VCZH_MSVC && defined VCZH_CHECK_MEMORY_LEAKS
	_CrtDumpMemoryLeaks();
#endif
	return result;
}
