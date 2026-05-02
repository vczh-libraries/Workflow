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

static WString MangleRpcFullName(const WString& fullName)
{
	WString mangled;
	for (vint i = 0; i < fullName.Length(); i++)
	{
		if (i + 1 < fullName.Length() && fullName[i] == L':' && fullName[i + 1] == L':')
		{
			mangled += L"__";
			i++;
		}
		else if (fullName[i] == L'.')
		{
			mangled += L"_";
		}
		else
		{
			mangled += WString::FromChar(fullName[i]);
		}
	}
	return mangled;
}

static WString MakeRpcCppAssemblyName(const WString& itemName)
{
	return L"Rpc_" + itemName;
}

struct RpcEventBridgeInfo : Object
{
	WString				interfaceFullName;
	WString				eventName;
	WString				eventIdName;
	WString				handlerType;
	List<WString>		argumentTypes;
	List<WString>		argumentValueTypes;
};

static ITypeDescriptor* FindRpcTypeDescriptor(WfLexicalScopeManager& manager, const WString& fullName);

static Ptr<List<Ptr<RpcEventBridgeInfo>>> CollectRpcEventBridgeInfos(WfLexicalScopeManager& manager, const WString& itemName)
{
	auto eventInfos = Ptr(new List<Ptr<RpcEventBridgeInfo>>());
	WfCppConfig cppConfig(&manager, MakeRpcCppAssemblyName(itemName), L"vl_workflow_global");

	for (auto eventFullName : manager.rpcMetadata->eventFullNames)
	{
		vint delimiter = -1;
		for (vint i = 0; i < eventFullName.Length(); i++)
		{
			if (eventFullName[i] == L'.')
			{
				delimiter = i;
			}
		}
		CHECK_ERROR(delimiter != -1 && delimiter < eventFullName.Length() - 1, L"CollectRpcEventBridgeInfos: Invalid RPC event full name.");

		auto bridgeInfo = Ptr(new RpcEventBridgeInfo);
		bridgeInfo->interfaceFullName = eventFullName.Sub(0, delimiter);
		bridgeInfo->eventName = eventFullName.Sub(delimiter + 1, eventFullName.Length() - delimiter - 1);
		bridgeInfo->eventIdName = MangleRpcFullName(eventFullName);

		auto td = FindRpcTypeDescriptor(manager, bridgeInfo->interfaceFullName);
		CHECK_ERROR(td, L"CollectRpcEventBridgeInfos: Failed to find RPC interface type descriptor.");
		auto eventInfo = td->GetEventByName(bridgeInfo->eventName, false);
		CHECK_ERROR(eventInfo, L"CollectRpcEventBridgeInfos: Failed to find event metadata.");

		auto handlerType = eventInfo->GetHandlerType();
		bridgeInfo->handlerType = cppConfig.ConvertType(handlerType);
		auto functionType = handlerType->GetElementType();
		for (vint i = 1; i < functionType->GetGenericArgumentCount(); i++)
		{
			auto argumentType = functionType->GetGenericArgument(i);
			bridgeInfo->argumentTypes.Add(cppConfig.ConvertArgumentType(argumentType));
			bridgeInfo->argumentValueTypes.Add(cppConfig.ConvertType(argumentType));
		}
		eventInfos->Add(bridgeInfo);
	}

	return eventInfos;
}

static ITypeDescriptor* FindRpcTypeDescriptor(WfLexicalScopeManager& manager, const WString& fullName)
{
	for (auto [decl, index] : indexed(manager.declarationTypes.Keys()))
	{
		if (!manager.declarationTypes.Keys()[index].Cast<WfClassDeclaration>())
		{
			continue;
		}

		auto td = manager.declarationTypes.Values()[index].Obj();
		if (td && td->GetTypeName() == fullName)
		{
			return td;
		}
	}
	return nullptr;
}

static void SortRpcTypeFullNamesLeafFirst(WfLexicalScopeManager& manager, const List<WString>& typeFullNames, List<WString>& sortedTypeFullNames)
{
	sortedTypeFullNames.Clear();

	Group<WString, WString> dependencyGroup;
	for (auto typeFullName : typeFullNames)
	{
		auto td = FindRpcTypeDescriptor(manager, typeFullName);
		if (!td)
		{
			continue;
		}

		for (vint i = 0; i < td->GetBaseTypeDescriptorCount(); i++)
		{
			auto baseTd = td->GetBaseTypeDescriptor(i);
			if (!baseTd)
			{
				continue;
			}

			auto baseFullName = baseTd->GetTypeName();
			if (typeFullNames.Contains(baseFullName))
			{
				dependencyGroup.Add(baseFullName, typeFullName);
			}
		}
	}

	PartialOrderingProcessor pop;
	pop.InitWithGroup(typeFullNames, dependencyGroup);
	pop.Sort();

	for (auto&& component : pop.components)
	{
		for (vint i = 0; i < component.nodeCount; i++)
		{
			sortedTypeFullNames.Add(typeFullNames[component.firstNode[i]]);
		}
	}
}

static WfPropertyDeclaration* FindRpcProperty(WfClassDeclaration* interfaceDecl, const WString& name)
{
	for (auto declaration : interfaceDecl->declarations)
	{
		if (auto propertyDecl = declaration.Cast<WfPropertyDeclaration>())
		{
			if (propertyDecl->name.value == name)
			{
				return propertyDecl.Obj();
			}
		}
	}
	return nullptr;
}

static WfFunctionDeclaration* FindRpcMethod(WfClassDeclaration* interfaceDecl, const WString& name)
{
	for (auto declaration : interfaceDecl->declarations)
	{
		if (auto methodDecl = declaration.Cast<WfFunctionDeclaration>())
		{
			if (methodDecl->name.value == name)
			{
				return methodDecl.Obj();
			}
		}
	}
	return nullptr;
}

static void RemoveRpcAttribute(List<Ptr<WfAttribute>>& attributes, const WString& name)
{
	for (vint i = attributes.Count() - 1; i >= 0; i--)
	{
		auto attribute = attributes[i];
		if (attribute->category.value == L"rpc" && attribute->name.value == name)
		{
			attributes.RemoveAt(i);
		}
	}
}

TEST_FILE
{
	WfLexicalScopeManager manager(GetWorkflowParser(), testCpuArchitecture);

	TEST_CATEGORY(L"RPC compilation")
	{
		List<WString> rpcNames, reflectableAssemblies;
		Dictionary<WString, WString> assemblyNames;
		Dictionary<WString, WString> assemblyEntries;
		Dictionary<WString, Ptr<List<WString>>> rpcTypeFullNamesPerItem;
		Dictionary<WString, Ptr<List<Ptr<RpcEventBridgeInfo>>>> rpcEventBridgeInfosPerItem;
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
					SortRpcTypeFullNamesLeafFirst(
						manager,
						manager.rpcMetadata->typeFullNames,
						*typeFullNames.Obj());
					rpcTypeFullNamesPerItem.Add(itemName, typeFullNames);
					rpcEventBridgeInfosPerItem.Add(itemName, CollectRpcEventBridgeInfos(manager, itemName));

					auto bits = GetBitsFromArchitecture();
					auto wrapperString = GenerateToStream([&](StreamWriter& writer)
					{
						WfPrint(wrapperModule, L"", writer);
					});
					auto metadataString = GenerateToStream([&](StreamWriter& writer)
					{
						WfPrint(manager.rpcMetadata->metadataModule, L"", writer);
					});
					auto dtsString = manager.rpcMetadata->dts;

					auto outputFolder = FilePath(GetTestOutputBasePath()) / (WString::Unmanaged(L"RpcMetadata") + bits);
					Folder folder(outputFolder);
					if (!folder.Exists())
					{
						folder.Create(false);
					}

					auto metadataPath = outputFolder / (L"Metadata_" + itemName + L".txt");
					{
						FileStream fileStream(metadataPath.GetFullPath(), FileStream::WriteOnly);
						BomEncoder encoder(BomEncoder::Utf8);
						EncoderStream encoderStream(fileStream, encoder);
						StreamWriter writer(encoderStream);
						writer.WriteString(metadataString);
					}

					auto dtsPath = outputFolder / (L"Serialization_" + itemName + L".d.ts");
					{
						FileStream fileStream(dtsPath.GetFullPath(), FileStream::WriteOnly);
						BomEncoder encoder(BomEncoder::Utf8);
						EncoderStream encoderStream(fileStream, encoder);
						StreamWriter writer(encoderStream);
						writer.WriteString(dtsString);
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
					auto input = Ptr(new WfCppInput(MakeRpcCppAssemblyName(itemName)));
					input->multiFile = WfCppFileSwitch::OnDemand;
					input->reflection = WfCppFileSwitch::OnDemand;
					input->comment = L"Source: ../Resources/Rpc/" + itemName + L".txt";
					input->includeFileName = itemName + L"Includes";
					input->reflectionFileName = itemName + L"Reflection";
					input->defaultFileName = itemName;
					input->normalIncludes.Add(L"../Source/CppTypes.h");
					input->normalIncludes.Add(L"../../Import/VlppGlrParser.h");

					auto output = GenerateCppFiles(input, &manager);
					TEST_ASSERT(manager.errors.Count() == 0);
					assemblyNames.Add(itemName, input->assemblyName);
					if (output->containsReflectionInfo)
					{
						reflectableAssemblies.Add(input->assemblyName);
					}
					if (output->reflection)
					{
						assemblyEntries.Add(itemName, input->reflectionFileName);
					}
					else
					{
						assemblyEntries.Add(itemName, output->entryFileName);
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
			writer.WriteLine(L"#include \"../Source/TestCasesRpc.h\"");
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

				// Generate RunRpcTestCase call with DecideTypeId lambda
				writer.WriteString(L"\tRunRpcTestCase<::vl_workflow_global::");
				writer.WriteString(assemblyNames[itemName]);
				writer.WriteString(L">(L\"");
				writer.WriteString(itemResult);
				writer.WriteLine(L"\",");

				writer.WriteLine(L"\t\t[](IDescriptable* obj) -> vint");
				writer.WriteLine(L"\t\t{");
				writer.WriteString(L"\t\t\tauto& instance = ::vl_workflow_global::");
				writer.WriteString(assemblyNames[itemName]);
				writer.WriteLine(L"::Instance();");
				if (rpcTypeFullNamesPerItem.Keys().Contains(itemName))
				{
					auto&& typeFullNames = *rpcTypeFullNamesPerItem.Get(itemName).Obj();
					for (auto&& fullName : typeFullNames)
					{
						writer.WriteString(L"\t\t\tif (dynamic_cast<::");
						writer.WriteString(fullName);
						writer.WriteString(L"*>(obj)) return instance.rpctype_");
						writer.WriteString(MangleRpcFullName(fullName));
						writer.WriteLine(L";");
					}
				}
				writer.WriteLine(L"\t\t\treturn RpcTypeId_NotFound;");
				writer.WriteLine(L"\t\t},");

				if (rpcEventBridgeInfosPerItem.Keys().Contains(itemName) && rpcEventBridgeInfosPerItem.Get(itemName)->Count() > 0)
				{
					writer.WriteLine(L"\t\t[](RpcDualLifecycleMock* mock, RpcObjectReference ref, IDescriptable* obj)");
					writer.WriteLine(L"\t\t{");
					writer.WriteString(L"\t\t\t::vl_workflow_global::");
					writer.WriteString(assemblyNames[itemName]);
					writer.WriteLine(L"::Instance().rpclistener_Attach(ref.typeId, mock, ref, obj);");
					writer.WriteLine(L"\t\t});");
				}
				else
				{
					writer.WriteLine(L"\t\tnullptr);");
				}

				writer.WriteLine(L"});");
			}
			writer.WriteLine(L"}");
		});
#endif
	});
}
