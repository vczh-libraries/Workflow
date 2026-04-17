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

static void AssertLines(const WString& expectedString, const WString& actualString)
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
	Dictionary<WString, Ptr<WfModule>> rpcMetadatas;
	WfLexicalScopeManager manager(GetWorkflowParser(), testCpuArchitecture);

	TEST_CATEGORY(L"Runtime compilation")
	{
		List<WString> runtimeNames;
		LoadSampleIndex(L"Runtime", runtimeNames);

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

				if (manager.rpcMetadata && manager.rpcMetadata->metadataModule)
				{
					auto bits = GetBitsFromArchitecture();
					auto metadataString = GenerateToStream([&](StreamWriter& writer)
					{
						WfPrint(manager.rpcMetadata->metadataModule, L"", writer);
					});

					auto outputFolder = GetTestOutputBasePath() + L"RpcMetadata" + bits + L"\\";
					{
						Folder folder(outputFolder);
						if (!folder.Exists())
						{
							folder.Create(false);
						}
					}
					auto outputPath = outputFolder + itemName + L".txt";
					{
						FileStream fileStream(outputPath, FileStream::WriteOnly);
						BomEncoder encoder(BomEncoder::Utf8);
						EncoderStream encoderStream(fileStream, encoder);
						StreamWriter writer(encoderStream);
						writer.WriteString(metadataString);
					}

					auto baselinePath = GetTestResourcePath() + L"Baseline\\RpcMetadata" + bits + L"\\" + itemName + L".txt";
					auto expectedString = File(baselinePath).ReadAllTextByBom();
					AssertLines(expectedString, metadataString);

					rpcMetadatas.Set(itemName, manager.rpcMetadata->metadataModule);
				}
			});
		}
	});

	TEST_CATEGORY(L"RPC metadata recompilation")
	{
		for (auto [itemName, metadataModule] : rpcMetadatas)
		{
			TEST_CASE(itemName)
			{
				TEST_PRINT(L"Recompiling RPC metadata: " + itemName);
				manager.Clear(true, true);
				manager.AddModule(metadataModule);
				manager.Rebuild(true);
				TEST_ASSERT(manager.errors.Count() == 0);
			});
		}
	});
}
