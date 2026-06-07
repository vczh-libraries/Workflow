#include "../../Source/Helper.h"

extern WfCpuArchitecture testCpuArchitecture;

static WString GetChatBotApiPath()
{
#if defined VCZH_MSVC
	return GetTestResourcePath() + L"..\\Apps\\ChatBot\\ChatAPI.txt";
#elif defined VCZH_GCC
	return GetTestResourcePath() + L"../Apps/ChatBot/ChatAPI.txt";
#endif
}

TEST_FILE
{
	TEST_CATEGORY(L"App generation")
	{
		TEST_CASE(L"ChatBot")
		{
			WfLexicalScopeManager manager(GetWorkflowParser(), testCpuArchitecture);
			auto sample = File(GetChatBotApiPath()).ReadAllTextByBom();

			manager.Clear(true, true);
			auto module = ParseModule(sample, GetWorkflowParser());
			TEST_ASSERT(module);
			TEST_ASSERT(manager.errors.Count() == 0);

			manager.AddModule(module);
			manager.Rebuild(true);
			TEST_ASSERT(manager.errors.Count() == 0);

#ifdef VCZH_MSVC
			auto input = Ptr(new WfCppInput(L"ChatBotApp"));
			input->multiFile = WfCppFileSwitch::OnDemand;
			input->reflection = WfCppFileSwitch::OnDemand;
			input->comment = L"Source: ../Apps/ChatBot/ChatAPI.txt";
			input->normalIncludes.Add(L"../../../../../Source/Library/WfLibraryReflection.h");

			auto output = GenerateCppFiles(input, &manager);
			TEST_ASSERT(manager.errors.Count() == 0);

			for (auto [fileName, index] : indexed(output->cppFiles.Keys()))
			{
				WString code = output->cppFiles.Values()[index];
				File file(GetAppOutputPath(L"ChatBot") + fileName);
				file.WriteAllText(code, false, BomEncoder::Mbcs);
			}
#endif
		});
	});
}
