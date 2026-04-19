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

static bool DecodeRpcName(const WString& rpcLine, WString& itemName, WString& itemResult)
{
	const wchar_t* reading = rpcLine.Buffer();
	auto eq = wcschr(reading, L'=');
	if (!eq) return false;
	vint index = eq - reading;
	itemName = rpcLine.Sub(0, index);
	itemResult = rpcLine.Sub(index + 1, rpcLine.Length() - index - 1);
	return true;
}

TEST_FILE
{
	WfLexicalScopeManager manager(GetWorkflowParser(), testCpuArchitecture);

	TEST_CATEGORY(L"RPC compilation")
	{
		List<WString> rpcNames, reflectableAssemblies;
		Dictionary<WString, WString> assemblyEntries;
		Dictionary<WString, Ptr<List<WString>>> rpcTypeFullNamesPerItem;
		LoadSampleIndex(L"Rpc", rpcNames);

		for (auto rpcLine : rpcNames)
		{
			WString itemName, itemResult;
			if (!DecodeRpcName(rpcLine, itemName, itemResult)) continue;

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

					auto typeFullNames = Ptr(new List<WString>());
					CopyFrom(*typeFullNames.Obj(), manager.rpcMetadata->typeFullNames);
					rpcTypeFullNamesPerItem.Add(itemName, typeFullNames);

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

#ifdef VCZH_MSVC
				{
					auto input = Ptr(new WfCppInput(itemName));
					input->multiFile = WfCppFileSwitch::OnDemand;
					input->reflection = WfCppFileSwitch::OnDemand;
					input->comment = L"Source: ../Resources/Rpc/" + itemName + L".txt";
					input->normalIncludes.Add(L"../Source/CppTypes.h");

					auto output = GenerateCppFiles(input, &manager);
					TEST_ASSERT(manager.errors.Count() == 0);
					if (output->containsReflectionInfo)
					{
						reflectableAssemblies.Add(input->assemblyName);
					}
					if (output->reflection)
					{
						assemblyEntries.Add(input->assemblyName, input->reflectionFileName);
					}
					else
					{
						assemblyEntries.Add(input->assemblyName, output->entryFileName);
					}

					for (auto [fileName, index] : indexed(output->cppFiles.Keys()))
					{
						WString code = output->cppFiles.Values()[index];
						File file(GetCppOutputPathRpc() + fileName);
						file.WriteAllText(code, false, BomEncoder::Mbcs);
					}
				}
#endif
				LogSampleCodegenResult(L"Rpc", itemName, assembly);
				LogSampleAssemblyBinary(L"Rpc", itemName, assembly);
			});
		}

#ifdef VCZH_MSVC
		TEST_CASE(L"TestCasesRpc.cpp")
		{
			FileStream fileStream(GetCppOutputPathRpc() + L"TestCasesRpc.cpp", FileStream::WriteOnly);
			Utf8Encoder encoder;
			EncoderStream encoderStream(fileStream, encoder);
			StreamWriter writer(encoderStream);

			for (auto rpcLine : rpcNames)
			{
				WString itemName, itemResult;
				if (!DecodeRpcName(rpcLine, itemName, itemResult)) continue;

				writer.WriteString(L"#include \"");
				writer.WriteString(assemblyEntries[itemName]);
				writer.WriteLine(L".h\"");
			}

			writer.WriteLine(L"#include \"../Source/RpcDualLifecycleMock.h\"");
			writer.WriteLine(L"");
			writer.WriteLine(L"using namespace vl;");
			writer.WriteLine(L"using namespace vl::collections;");
			writer.WriteLine(L"using namespace vl::console;");
			writer.WriteLine(L"using namespace vl::reflection;");
			writer.WriteLine(L"using namespace vl::reflection::description;");
			writer.WriteLine(L"using namespace vl::rpc_controller;");
			writer.WriteLine(L"using namespace vl::rpc_controller_test;");
			writer.WriteLine(L"");
			writer.WriteLine(L"void LoadTestCaseRpcTypes()");
			writer.WriteLine(L"{");
			for (auto name : reflectableAssemblies)
			{
				writer.WriteString(L"\t Load");
				writer.WriteString(name);
				writer.WriteLine(L"Types();");
			}
			writer.WriteLine(L"}");

			writer.WriteLine(L"");
			writer.WriteLine(L"TEST_FILE");
			writer.WriteLine(L"{");

			for (auto rpcLine : rpcNames)
			{
				WString itemName, itemResult;
				if (!DecodeRpcName(rpcLine, itemName, itemResult)) continue;

				writer.WriteLine(L"");

				writer.WriteString(L"TEST_CASE(L\"Rpc:");
				writer.WriteString(itemName);
				writer.WriteLine(L"\")");
				writer.WriteLine(L"{");

				// Generate a local subclass that overrides DecideTypeId with dynamic_cast
				if (rpcTypeFullNamesPerItem.Keys().Contains(itemName))
				{
					auto&& typeFullNames = *rpcTypeFullNamesPerItem.Get(itemName).Obj();
					if (typeFullNames.Count() > 0)
					{
						writer.WriteLine(L"\tclass LocalRpcMock : public RpcDualLifecycleMock");
						writer.WriteLine(L"\t{");
						writer.WriteLine(L"\tpublic:");
						writer.WriteLine(L"\t\tusing RpcDualLifecycleMock::RpcDualLifecycleMock;");
						writer.WriteLine(L"\t\tvint DecideTypeId(IDescriptable* obj)const override");
						writer.WriteLine(L"\t\t{");
						writer.WriteLine(L"\t\t\tauto result = RpcDualLifecycleMock::DecideTypeId(obj);");
						writer.WriteLine(L"\t\t\tif (result != RpcTypeId_NotFound) return result;");
						for (auto&& fullName : typeFullNames)
						{
							writer.WriteString(L"\t\t\tif (dynamic_cast<::");
							writer.WriteString(fullName);
							writer.WriteString(L"*>(obj)) return idMap.Get(L\"");
							writer.WriteString(fullName);
							writer.WriteLine(L"\");");
						}
						writer.WriteLine(L"\t\t\treturn RpcTypeId_NotFound;");
						writer.WriteLine(L"\t\t}");
						writer.WriteLine(L"\t};");
						writer.WriteLine(L"");
					}
				}

				writer.WriteString(L"\tWString expected = L\"");
				writer.WriteString(itemResult);
				writer.WriteLine(L"\";");

				writer.WriteLine(L"");
				writer.WriteString(L"\tauto& instance = ::vl_workflow_global::");
				writer.WriteString(itemName);
				writer.WriteLine(L"::Instance();");

				writer.WriteLine(L"");

				// Get the id map from rpc_GetIds
				writer.WriteLine(L"\tauto idDictObj = instance.rpc_GetIds();");
				writer.WriteLine(L"\tDictionary<WString, vint> idMap;");
				writer.WriteLine(L"\t{");
				writer.WriteLine(L"\t\tauto keys = idDictObj->GetKeys();");
				writer.WriteLine(L"\t\tauto values = idDictObj->GetValues();");
				writer.WriteLine(L"\t\tfor (vint i = 0; i < idDictObj->GetCount(); i++)");
				writer.WriteLine(L"\t\t{");
				writer.WriteLine(L"\t\t\tauto key = UnboxValue<WString>(keys->Get(i));");
				writer.WriteLine(L"\t\t\tauto val = UnboxValue<vint>(values->Get(i));");
				writer.WriteLine(L"\t\t\tidMap.Set(key, val);");
				writer.WriteLine(L"\t\t}");
				writer.WriteLine(L"\t}");
				writer.WriteLine(L"");

				// Create two lifecycle mocks with adapters (use LocalRpcMock if types exist)
				bool hasTypes = rpcTypeFullNamesPerItem.Keys().Contains(itemName) && rpcTypeFullNamesPerItem.Get(itemName)->Count() > 0;
				auto mockType = hasTypes ? L"LocalRpcMock" : L"RpcDualLifecycleMock";

				writer.WriteString(L"\tauto lc1 = Ptr(new ");
				writer.WriteString(mockType);
				writer.WriteLine(L"(1));");
				writer.WriteString(L"\tauto lc2 = Ptr(new ");
				writer.WriteString(mockType);
				writer.WriteLine(L"(2));");
				writer.WriteLine(L"\tauto adapter1 = Ptr(new RpcDualLifeCycleAdapter(lc1.Obj()));");
				writer.WriteLine(L"\tauto adapter2 = Ptr(new RpcDualLifeCycleAdapter(lc2.Obj()));");
				writer.WriteLine(L"\tlc1->SetPeer(lc2.Obj());");
				writer.WriteLine(L"\tlc2->SetPeer(lc1.Obj());");
				writer.WriteLine(L"\tlc1->SetIdMap(idMap);");
				writer.WriteLine(L"\tlc2->SetIdMap(idMap);");
				writer.WriteLine(L"\tlc1->SetAdapter(adapter1.Obj());");
				writer.WriteLine(L"\tlc2->SetAdapter(adapter2.Obj());");
				writer.WriteLine(L"");

				// Register wrapper factories for C++ compiled code
				if (rpcTypeFullNamesPerItem.Keys().Contains(itemName))
				{
					auto&& typeFullNames = *rpcTypeFullNamesPerItem.Get(itemName).Obj();
					for (auto&& fullName : typeFullNames)
					{
						// Derive simple name: for "RpcTest::IService", get "IService"
						auto lastColon = wcsrchr(fullName.Buffer(), L':');
						WString simpleName;
						if (lastColon)
						{
							simpleName = WString(lastColon + 1);
						}
						else
						{
							simpleName = fullName;
						}

						writer.WriteLine(L"\t{");
						writer.WriteString(L"\t\tauto typeId = idMap.Get(L\"");
						writer.WriteString(fullName);
						writer.WriteLine(L"\");");

						writer.WriteString(L"\t\tlc1->RegisterWrapperFactory(typeId, [&instance](IRpcLifeCycle* lc) -> Ptr<IDescriptable> { return instance.rpcwrapper_");
						writer.WriteString(simpleName);
						writer.WriteLine(L"(lc); });");

						writer.WriteString(L"\t\tlc2->RegisterWrapperFactory(typeId, [&instance](IRpcLifeCycle* lc) -> Ptr<IDescriptable> { return instance.rpcwrapper_");
						writer.WriteString(simpleName);
						writer.WriteLine(L"(lc); });");

						writer.WriteLine(L"\t}");
					}
					writer.WriteLine(L"");
				}

				// Create list ops default implementations
				writer.WriteLine(L"\tauto lo1 = Ptr(new RpcCalleeListOps(adapter1.Obj()));");
				writer.WriteLine(L"\tauto leo1 = Ptr(new RpcCalleeListEventBridge(adapter1.Obj()));");
				writer.WriteLine(L"\tauto lo2 = Ptr(new RpcCalleeListOps(adapter2.Obj()));");
				writer.WriteLine(L"\tauto leo2 = Ptr(new RpcCalleeListEventBridge(adapter2.Obj()));");
				writer.WriteLine(L"");

				// Create object ops implementations from the generated C++ code
				writer.WriteString(L"\tauto oo1 = instance.rpc_IRpcObjectOps(adapter1.Obj());");
				writer.WriteLine(L"");
				writer.WriteString(L"\tauto oeo1 = instance.rpc_IRpcObjectEventOps(adapter1.Obj());");
				writer.WriteLine(L"");
				writer.WriteString(L"\tauto oo2 = instance.rpc_IRpcObjectOps(adapter2.Obj());");
				writer.WriteLine(L"");
				writer.WriteString(L"\tauto oeo2 = instance.rpc_IRpcObjectEventOps(adapter2.Obj());");
				writer.WriteLine(L"");
				writer.WriteLine(L"");

				// Register cross: ops from lc1 go to lc2, and vice versa
				writer.WriteLine(L"\tlc2->Register(oo1, oeo1, lo1, leo1);");
				writer.WriteLine(L"\tlc1->Register(oo2, oeo2, lo2, leo2);");
				writer.WriteLine(L"");

				// Run serviceMain with lc1
				writer.WriteLine(L"\tinstance.serviceMain(adapter1.Obj());");
				writer.WriteLine(L"");

				// Run clientMain with lc2 and get the result
				writer.WriteLine(L"\tauto actual = instance.clientMain(adapter2.Obj());");
				writer.WriteLine(L"");

				writer.WriteLine(L"\tConsole::WriteLine(L\"    expected : \" + expected);");
				writer.WriteLine(L"\tConsole::WriteLine(L\"    actual   : \" + actual);");
				writer.WriteLine(L"\tTEST_ASSERT(actual == expected);");

				writer.WriteLine(L"});");
			}
			writer.WriteLine(L"}");
		});
#endif
	});
}
