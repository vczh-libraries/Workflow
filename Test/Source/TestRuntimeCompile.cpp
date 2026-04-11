#include "Helper.h"

extern WfCpuArchitecture testCpuArchitecture;

TEST_FILE
{
	TEST_CATEGORY(L"Runtime compilation")
	{
		List<WString> runtimeNames;
		LoadSampleIndex(L"Runtime", runtimeNames);

		WfLexicalScopeManager manager(GetWorkflowParser(), testCpuArchitecture);
		for (auto itemName : runtimeNames)
		{
			TEST_CASE(itemName)
			{
				TEST_PRINT(itemName);
				WString sample = LoadSample(L"Runtime", itemName);
				manager.Clear(true, true);
				{
					auto module = ParseModule(sample, GetWorkflowParser());
					TEST_ASSERT(module);
					TEST_ASSERT(manager.errors.Count() == 0);

					manager.AddModule(module);
					manager.Rebuild(true);
					LogSampleParseResult(L"Runtime", itemName, sample, module, &manager);
					TEST_ASSERT(manager.errors.Count() == 0);
				}
				Ptr<WfAssembly> assembly = GenerateAssembly(&manager);
				TEST_ASSERT(assembly);

				LogSampleCodegenResult(L"Runtime", itemName, assembly);
				LogSampleAssemblyBinary(L"Runtime", itemName, assembly);
			});
		}
	});
}
