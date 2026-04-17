#include "../../Source/Helper.h"

extern WfCpuArchitecture testCpuArchitecture;

static const wchar_t* GetBitsFromArchitecture()
{
	switch (testCpuArchitecture)
	{
	case WfCpuArchitecture::x86: return L"32";
	case WfCpuArchitecture::x64: return L"64";
	default:
		CHECK_FAIL(L"The CPU architecture is specified.");
	}
}

TEST_FILE
{
	WfLexicalScopeManager manager(GetWorkflowParser(), testCpuArchitecture);

	TEST_CATEGORY(L"RPC compilation")
	{
		List<WString> rpcNames;
		LoadSampleIndex(L"Rpc", rpcNames);

		for (auto itemName : rpcNames)
		{
			TEST_CASE(itemName)
			{
				TEST_PRINT(itemName);
				auto sample = LoadSample(L"Rpc", itemName);
				Ptr<WfModule> wrapperModule;

				manager.Clear(true, true);
				{
					auto module = ParseModule(sample, GetWorkflowParser());
					TEST_ASSERT(module);
					TEST_ASSERT(manager.errors.Count() == 0);

					manager.AddModule(module);
					manager.Rebuild(true);
					TEST_ASSERT(manager.errors.Count() == 0);
					TEST_ASSERT(manager.rpcMetadata && manager.rpcMetadata->metadataModule);

					wrapperModule = GenerateModuleRpc(&manager);
					TEST_ASSERT(wrapperModule);
					TEST_ASSERT(manager.errors.Count() == 0);

					auto bits = GetBitsFromArchitecture();
					auto wrapperString = GenerateToStream([&](StreamWriter& writer)
					{
						WfPrint(wrapperModule, L"", writer);
					});

					auto outputFolder = GetTestOutputBasePath() + L"RpcMetadata" + bits + L"\\";
					Folder folder(outputFolder);
					if (!folder.Exists())
					{
						folder.Create(false);
					}

					auto outputPath = outputFolder + L"Wrapper_" + itemName + L".txt";
					FileStream fileStream(outputPath, FileStream::WriteOnly);
					BomEncoder encoder(BomEncoder::Utf8);
					EncoderStream encoderStream(fileStream, encoder);
					StreamWriter writer(encoderStream);
					writer.WriteString(wrapperString);
				}

				manager.Clear(true, true);
				{
					auto module = ParseModule(sample, GetWorkflowParser());
					TEST_ASSERT(module);
					TEST_ASSERT(manager.errors.Count() == 0);

					manager.AddModule(module);
					manager.AddModule(wrapperModule);
					manager.Rebuild(true, nullptr, false);
					LogSampleParseResult(L"Rpc", itemName, sample, module, &manager);
					TEST_ASSERT(manager.errors.Count() == 0);
				}

				auto assembly = GenerateAssembly(&manager);
				TEST_ASSERT(assembly);

				LogSampleCodegenResult(L"Rpc", itemName, assembly);
				LogSampleAssemblyBinary(L"Rpc", itemName, assembly);
			});
		}
	});
}
