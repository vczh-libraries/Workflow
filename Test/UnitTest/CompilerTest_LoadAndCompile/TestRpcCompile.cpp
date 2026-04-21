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

static bool IsIdentifierChar(wchar_t c)
{
	return
		(c >= L'a' && c <= L'z') ||
		(c >= L'A' && c <= L'Z') ||
		(c >= L'0' && c <= L'9') ||
		c == L'_';
}

static bool TryReadQuotedString(const WString& text, vint& index, WString& value)
{
	if (index >= text.Length() || text[index] != L'"')
	{
		return false;
	}

	value = WString::Empty;
	index++;
	while (index < text.Length())
	{
		auto c = text[index++];
		if (c == L'\\')
		{
			if (index >= text.Length())
			{
				return false;
			}

			auto escaped = text[index++];
			switch (escaped)
			{
			case L'r': value += WString::FromChar(L'\r'); break;
			case L'n': value += WString::FromChar(L'\n'); break;
			case L't': value += WString::FromChar(L'\t'); break;
			default: value += WString::FromChar(escaped); break;
			}
		}
		else if (c == L'"')
		{
			return true;
		}
		else
		{
			value += WString::FromChar(c);
		}
	}
	return false;
}

static WfFunctionDeclaration* FindNamedFunctionDeclaration(const List<Ptr<WfDeclaration>>& declarations, const WString& name)
{
	for (auto&& declaration : declarations)
	{
		if (auto functionDecl = declaration.Cast<WfFunctionDeclaration>())
		{
			if (functionDecl->name.value == name)
			{
				return functionDecl.Obj();
			}
		}
		else if (auto namespaceDecl = declaration.Cast<WfNamespaceDeclaration>())
		{
			if (auto functionDecl = FindNamedFunctionDeclaration(namespaceDecl->declarations, name))
			{
				return functionDecl;
			}
		}
	}
	return nullptr;
}

static void CollectRegisterServiceTypeFullNames(const WString& text, List<WString>& serviceTypeFullNames)
{
	constexpr const wchar_t* registerServiceName = L"RegisterService";
	constexpr vint registerServiceNameLength = 15;

	for (vint i = 0; i + registerServiceNameLength <= text.Length(); i++)
	{
		if (
			(i > 0 && IsIdentifierChar(text[i - 1])) ||
			wcsncmp(text.Buffer() + i, registerServiceName, registerServiceNameLength) != 0 ||
			(i + registerServiceNameLength < text.Length() && IsIdentifierChar(text[i + registerServiceNameLength]))
			)
		{
			continue;
		}

		auto index = i + registerServiceNameLength;
		while (index < text.Length() && (text[index] == L' ' || text[index] == L'\r' || text[index] == L'\n' || text[index] == L'\t'))
		{
			index++;
		}

		if (index >= text.Length() || text[index] != L'(')
		{
			continue;
		}

		index++;
		while (index < text.Length() && (text[index] == L' ' || text[index] == L'\r' || text[index] == L'\n' || text[index] == L'\t'))
		{
			index++;
		}

		WString serviceTypeFullName;
		if (TryReadQuotedString(text, index, serviceTypeFullName))
		{
			serviceTypeFullNames.Add(serviceTypeFullName);
		}
	}
}

static void CollectRpcServiceTypeFullNames(const Ptr<WfModule>& module, List<WString>& serviceTypeFullNames)
{
	serviceTypeFullNames.Clear();
	if (!module)
	{
		return;
	}

	if (auto functionDecl = FindNamedFunctionDeclaration(module->declarations, L"serviceMain"))
	{
		auto statementText = GenerateToStream([&](StreamWriter& writer)
		{
			WfPrint(functionDecl->statement, L"", writer);
		});
		CollectRegisterServiceTypeFullNames(statementText, serviceTypeFullNames);
	}
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

TEST_FILE
{
	WfLexicalScopeManager manager(GetWorkflowParser(), testCpuArchitecture);

	TEST_CATEGORY(L"RPC compilation")
	{
		List<WString> rpcNames, reflectableAssemblies;
		Dictionary<WString, WString> assemblyEntries;
		Dictionary<WString, Ptr<List<WString>>> rpcTypeFullNamesPerItem;
		Dictionary<WString, Ptr<List<WString>>> rpcServiceTypeFullNamesPerItem;
		LoadSampleIndex(L"Rpc", rpcNames);

		TEST_CASE(L"Inherited interfaces are ordered leaf first")
		{
			manager.Clear(true, true);

			auto sample = WString::Unmanaged(LR"WORKFLOW(module Rpc;
using system::*;

namespace RpcInheritanceOrderTest
{
	@rpc:Interface
	interface IBase
	{
	}

	@rpc:Interface
	interface IDerived : IBase
	{
	}

	@rpc:Interface
	interface ILeaf : IDerived
	{
	}
}
)WORKFLOW");

			auto module = ParseModule(sample, GetWorkflowParser());
			TEST_ASSERT(module);
			TEST_ASSERT(manager.errors.Count() == 0);

			manager.AddModule(module);
			manager.Rebuild(true);
			TEST_ASSERT(manager.errors.Count() == 0);
			TEST_ASSERT(manager.rpcMetadata && manager.rpcMetadata->metadataModule);

			List<WString> sortedTypeFullNames;
			SortRpcTypeFullNamesLeafFirst(manager, manager.rpcMetadata->typeFullNames, sortedTypeFullNames);

			auto baseIndex = sortedTypeFullNames.IndexOf(L"RpcInheritanceOrderTest::IBase");
			auto derivedIndex = sortedTypeFullNames.IndexOf(L"RpcInheritanceOrderTest::IDerived");
			auto leafIndex = sortedTypeFullNames.IndexOf(L"RpcInheritanceOrderTest::ILeaf");
			TEST_ASSERT(baseIndex != -1);
			TEST_ASSERT(derivedIndex != -1);
			TEST_ASSERT(leafIndex != -1);
			TEST_ASSERT(leafIndex < derivedIndex);
			TEST_ASSERT(derivedIndex < baseIndex);
		});

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

					auto serviceTypeFullNames = Ptr(new List<WString>());
					CollectRpcServiceTypeFullNames(module, *serviceTypeFullNames.Obj());
					rpcServiceTypeFullNamesPerItem.Add(itemName, serviceTypeFullNames);

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

			// Generate the RunRpcTestCase template function
			writer.WriteLine(L"");
			writer.WriteLine(L"template<typename TInstance>");
			writer.WriteLine(L"void RunRpcTestCase(const WString& expected, void(*registerServiceTypeIds)(const Func<void(vint)>&), vint(*decideTypeId)(IDescriptable*))");
			writer.WriteLine(L"{");
			writer.WriteLine(L"\tclass LocalRpcMock : public RpcDualLifecycleMock");
			writer.WriteLine(L"\t{");
			writer.WriteLine(L"\tpublic:");
			writer.WriteLine(L"\t\tList<vint> serviceTypeIds;");
			writer.WriteLine(L"\t\tvint nextServiceTypeIdIndex = 0;");
			writer.WriteLine(L"\t\tvint(*decideTypeIdCallback)(IDescriptable*) = nullptr;");
			writer.WriteLine(L"\t\tusing RpcDualLifecycleMock::RpcDualLifecycleMock;");
			writer.WriteLine(L"\t\tvoid AddServiceTypeId(vint typeId)");
			writer.WriteLine(L"\t\t{");
			writer.WriteLine(L"\t\t\tserviceTypeIds.Add(typeId);");
			writer.WriteLine(L"\t\t}");
			writer.WriteLine(L"\t\tvoid RegisterService(const WString& fullName, Ptr<IDescriptable> service) override");
			writer.WriteLine(L"\t\t{");
			writer.WriteLine(L"\t\t\tRpcLifecycleMock::RegisterService(fullName, service);");
			writer.WriteLine(L"\t\t\tCHECK_ERROR(nextServiceTypeIdIndex < serviceTypeIds.Count(), L\"LocalRpcMock::RegisterService: Missing service type id.\");");
			writer.WriteLine(L"\t\t\tidMap.Set(fullName, serviceTypeIds[nextServiceTypeIdIndex++]);");
			writer.WriteLine(L"\t\t}");
			writer.WriteLine(L"\t\tvoid ValidateServiceTypeIds() const");
			writer.WriteLine(L"\t\t{");
			writer.WriteLine(L"\t\t\tCHECK_ERROR(nextServiceTypeIdIndex == serviceTypeIds.Count(), L\"LocalRpcMock::ValidateServiceTypeIds: Unused service type ids.\");");
			writer.WriteLine(L"\t\t}");
			writer.WriteLine(L"\t\tvoid CopyPeerServiceTypeIds()");
			writer.WriteLine(L"\t\t{");
			writer.WriteLine(L"\t\t\tauto localPeer = dynamic_cast<LocalRpcMock*>(peer);");
			writer.WriteLine(L"\t\t\tCHECK_ERROR(localPeer != nullptr, L\"LocalRpcMock::CopyPeerServiceTypeIds: Missing peer.\");");
			writer.WriteLine(L"\t\t\tfor (auto&& [fullName, typeId] : localPeer->idMap)");
			writer.WriteLine(L"\t\t\t{");
			writer.WriteLine(L"\t\t\t\tidMap.Set(fullName, typeId);");
			writer.WriteLine(L"\t\t\t}");
			writer.WriteLine(L"\t\t}");
			writer.WriteLine(L"\t\tvint DecideTypeId(IDescriptable* obj)const override");
			writer.WriteLine(L"\t\t{");
			writer.WriteLine(L"\t\t\tauto result = RpcDualLifecycleMock::DecideTypeId(obj);");
			writer.WriteLine(L"\t\t\tif (result != RpcTypeId_NotFound) return result;");
			writer.WriteLine(L"\t\t\treturn decideTypeIdCallback(obj);");
			writer.WriteLine(L"\t\t}");
			writer.WriteLine(L"\t};");
			writer.WriteLine(L"");
			writer.WriteLine(L"\tauto& instance = TInstance::Instance();");
			writer.WriteLine(L"");
			writer.WriteLine(L"\tauto lc1 = Ptr(new LocalRpcMock(1));");
			writer.WriteLine(L"\tauto lc2 = Ptr(new LocalRpcMock(2));");
			writer.WriteLine(L"\tauto registerServiceTypeId = Func<void(vint)>([&](vint typeId) { lc1->AddServiceTypeId(typeId); });");
			writer.WriteLine(L"\tregisterServiceTypeIds(registerServiceTypeId);");
			writer.WriteLine(L"\tlc1->decideTypeIdCallback = decideTypeId;");
			writer.WriteLine(L"\tlc2->decideTypeIdCallback = decideTypeId;");
			writer.WriteLine(L"\tauto adapter1 = Ptr(new RpcDualLifeCycleAdapter(lc1.Obj()));");
			writer.WriteLine(L"\tauto adapter2 = Ptr(new RpcDualLifeCycleAdapter(lc2.Obj()));");
			writer.WriteLine(L"\tlc1->SetPeer(lc2.Obj());");
			writer.WriteLine(L"\tlc2->SetPeer(lc1.Obj());");
			writer.WriteLine(L"\tlc1->SetAdapter(adapter1.Obj());");
			writer.WriteLine(L"\tlc2->SetAdapter(adapter2.Obj());");
			writer.WriteLine(L"");
			writer.WriteLine(L"\tauto lo1 = Ptr(new RpcCalleeListOps(adapter1.Obj()));");
			writer.WriteLine(L"\tauto leo1 = Ptr(new RpcCalleeListEventBridge(adapter1.Obj()));");
			writer.WriteLine(L"\tauto lo2 = Ptr(new RpcCalleeListOps(adapter2.Obj()));");
			writer.WriteLine(L"\tauto leo2 = Ptr(new RpcCalleeListEventBridge(adapter2.Obj()));");
			writer.WriteLine(L"");
			writer.WriteLine(L"\tauto oo1 = instance.rpc_IRpcObjectOps(adapter1.Obj());");
			writer.WriteLine(L"\tauto oeo1 = instance.rpc_IRpcObjectEventOps(adapter1.Obj());");
			writer.WriteLine(L"\tauto oo2 = instance.rpc_IRpcObjectOps(adapter2.Obj());");
			writer.WriteLine(L"\tauto oeo2 = instance.rpc_IRpcObjectEventOps(adapter2.Obj());");
			writer.WriteLine(L"");
			writer.WriteLine(L"\tlc2->Register(oo1, oeo1, lo1, leo1);");
			writer.WriteLine(L"\tlc1->Register(oo2, oeo2, lo2, leo2);");
			writer.WriteLine(L"\tlc1->RegisterWrapperFactory([&](vint typeId, IRpcLifeCycle* lc) { return instance.rpcwrapper_Create(typeId, lc); });");
			writer.WriteLine(L"\tlc2->RegisterWrapperFactory([&](vint typeId, IRpcLifeCycle* lc) { return instance.rpcwrapper_Create(typeId, lc); });");
			writer.WriteLine(L"");
			writer.WriteLine(L"\tinstance.serviceMain(adapter1.Obj());");
			writer.WriteLine(L"\tlc1->ValidateServiceTypeIds();");
			writer.WriteLine(L"\tlc2->CopyPeerServiceTypeIds();");
			writer.WriteLine(L"");
			writer.WriteLine(L"\tauto actual = instance.clientMain(adapter2.Obj());");
			writer.WriteLine(L"");
			writer.WriteLine(L"\tConsole::WriteLine(L\"    expected : \" + expected);");
			writer.WriteLine(L"\tConsole::WriteLine(L\"    actual   : \" + actual);");
			writer.WriteLine(L"\tTEST_ASSERT(actual == expected);");
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
				writer.WriteString(itemName);
				writer.WriteString(L">(L\"");
				writer.WriteString(itemResult);
				writer.WriteLine(L"\",");

				writer.WriteLine(L"\t\t[](const Func<void(vint)>& registerServiceTypeId)");
				writer.WriteLine(L"\t\t{");
				writer.WriteString(L"\t\t\tauto& instance = ::vl_workflow_global::");
				writer.WriteString(itemName);
				writer.WriteLine(L"::Instance();");
				if (rpcServiceTypeFullNamesPerItem.Keys().Contains(itemName))
				{
					auto&& typeFullNames = *rpcServiceTypeFullNamesPerItem.Get(itemName).Obj();
					for (auto&& fullName : typeFullNames)
					{
						writer.WriteString(L"\t\t\tregisterServiceTypeId(instance.rpctype_");
						writer.WriteString(MangleRpcFullName(fullName));
						writer.WriteLine(L");");
					}
				}
				writer.WriteLine(L"\t\t},");

				writer.WriteLine(L"\t\t[](IDescriptable* obj) -> vint");
				writer.WriteLine(L"\t\t{");
				writer.WriteString(L"\t\t\tauto& instance = ::vl_workflow_global::");
				writer.WriteString(itemName);
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
				writer.WriteLine(L"\t\t});");

				writer.WriteLine(L"});");
			}
			writer.WriteLine(L"}");
		});
#endif
	});
}
