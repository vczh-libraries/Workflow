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

static bool DecodeRpcName(const WString& rpcLine, WString& itemName)
{
	const wchar_t* reading = rpcLine.Buffer();
	auto eq = wcschr(reading, L'=');
	if (!eq) return false;
	vint index = eq - reading;
	itemName = rpcLine.Sub(0, index);
	return true;
}

TEST_FILE
{
	WfLexicalScopeManager manager(GetWorkflowParser(), testCpuArchitecture);

	TEST_CATEGORY(L"RPC compilation")
	{
		List<WString> rpcNames;
		LoadSampleIndex(L"Rpc", rpcNames);

		for (auto rpcLine : rpcNames)
		{
			WString itemName;
			if (!DecodeRpcName(rpcLine, itemName)) continue;

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

					auto outputFolder = FilePath(GetTestOutputBasePath()) / (WString::Unmanaged(L"RpcMetadata") + bits);
					Folder folder(outputFolder);
					if (!folder.Exists())
					{
						folder.Create(false);
					}

					auto outputPath = outputFolder / (L"Wrapper_" + itemName + L".txt");
					FileStream fileStream(outputPath.GetFullPath(), FileStream::WriteOnly);
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
