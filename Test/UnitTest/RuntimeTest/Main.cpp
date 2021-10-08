#include "../../Source/CppTypes.h"
#include "../../Source/Helper.h"

#if defined VCZH_MSVC
using namespace vl::filesystem;
#endif

void LoadTypes()
{
	LoadPredefinedTypes();
	LoadParsingTypes();
	XmlLoadTypes();
	JsonLoadTypes();
	WfLoadLibraryTypes();
	WfLoadTypes();
	LoadCppTypes();
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
			CHECK_ERROR(folder.Create(false) == true, L"Failed to create GetTestOutputPath()");
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
			FOREACH(File, file, files)
			{
				CHECK_ERROR(file.Delete() == true, L"Failed to delete file");
			}
		}
	}
#endif
	LoadTypes();
	int result = unittest::UnitTest::RunAndDisposeTests(argc, argv);
	UnloadTypes();
	ReleaseWorkflowTable();
	ThreadLocalStorage::DisposeStorages();
#if defined VCZH_MSVC && defined VCZH_CHECK_MEMORY_LEAKS
	_CrtDumpMemoryLeaks();
#endif
	return result;
}
