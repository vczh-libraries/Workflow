#include <Vlpp.h>
#include <Windows.h>

using namespace vl;

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
	return WString::CopyFrom(buffer, pos + 1);
}

WString GetJsonRequestOutputPath()
{
#if defined VCZH_MSVC
#ifdef VCZH_64
	return GetExePath() + L"..\\..\\..\\TypeScript\\JsonRequest64\\";
#else
	return GetExePath() + L"..\\..\\TypeScript\\JsonRequest32\\";
#endif
#elif defined VCZH_GCC
	return L"../../TypeScript/JsonRequest64/";
#endif
}

WString GetJsonValuesOutputPath()
{
#if defined VCZH_MSVC
#ifdef VCZH_64
	return GetExePath() + L"..\\..\\..\\TypeScript\\JsonValues64\\";
#else
	return GetExePath() + L"..\\..\\TypeScript\\JsonValues32\\";
#endif
#elif defined VCZH_GCC
	return L"../../TypeScript/JsonValues64/";
#endif
}
