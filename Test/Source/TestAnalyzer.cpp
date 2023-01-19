#include "Helper.h"
#include "CppTypes.h"

extern WfCpuArchitecture testCpuArchitecture;

void LoadMultipleSamples(WfLexicalScopeManager* manager, const WString& sampleName)
{
	List<WString> itemNames;
	LoadSampleIndex(sampleName, itemNames);
	for (auto itemName : itemNames)
	{
		WString sample = LoadSample(sampleName, itemName);
		auto module = ParseModule(sample, GetWorkflowParser());
		manager->AddModule(module);

		auto sampleModule = GenerateToStream([&](StreamWriter& writer)
		{
			WfPrint(module, L"", writer);
		});
		LogSampleParseResult(sampleName, itemName, sampleModule, module);
	}
}

TEST_FILE
{
	TEST_CASE(L"Test building global name")
	{
		WfLexicalScopeManager manager(GetWorkflowParser(), testCpuArchitecture);
		LoadMultipleSamples(&manager, L"AnalyzerScope");
		manager.Rebuild(false);

		{
			auto parent = manager.globalName;
			auto name = parent->children[L"system"];
			TEST_ASSERT(name->typeDescriptor == 0);
			TEST_ASSERT(name->declarations.Count() == 0);
			TEST_ASSERT(name->parent == parent.Obj());
		}
		{
			auto parent = manager.globalName->children[L"system"];
			auto name = parent->children[L"Object"];
			TEST_ASSERT(name->typeDescriptor == GetTypeDescriptor<Value>());
			TEST_ASSERT(name->declarations.Count() == 0);
			TEST_ASSERT(name->parent == parent.Obj());
		}
		{
			auto parent = manager.globalName->children[L"system"]->children[L"reflection"];
			auto name = parent->children[L"TypeDescriptor"];
			TEST_ASSERT(name->typeDescriptor == GetTypeDescriptor<ITypeDescriptor>());
			TEST_ASSERT(name->declarations.Count() == 0);
			TEST_ASSERT(name->parent == parent.Obj());
		}
		{
			auto parent = manager.globalName;
			auto name = parent->children[L"test"];
			TEST_ASSERT(name->typeDescriptor == 0);
			TEST_ASSERT(name->declarations.Count() == 2);
			TEST_ASSERT(manager.namespaceNames[name->declarations[0].Cast<WfNamespaceDeclaration>().Obj()] == name);
			TEST_ASSERT(manager.namespaceNames[name->declarations[1].Cast<WfNamespaceDeclaration>().Obj()] == name);
			TEST_ASSERT(name->parent == parent.Obj());
		}
		{
			auto parent = manager.globalName->children[L"test"];
			auto name = parent->children[L"itoa"];
			TEST_ASSERT(name->typeDescriptor == 0);
			TEST_ASSERT(name->declarations.Count() == 1);
			TEST_ASSERT(name->parent == parent.Obj());

			auto function = name->declarations[0].Cast<WfFunctionDeclaration>();
			TEST_ASSERT(function->name.value == L"itoa");
		}
		{
			auto parent = manager.globalName;
			auto name = parent->children[L"main"];
			TEST_ASSERT(name->typeDescriptor == 0);
			TEST_ASSERT(name->declarations.Count() == 1);
			TEST_ASSERT(name->parent == parent.Obj());

			auto function = name->declarations[0].Cast<WfFunctionDeclaration>();
			TEST_ASSERT(function->name.value == L"main");
		}
	});

	TEST_CATEGORY(L"Test against illegal scripts")
	{
		List<WString> itemNames;
		LoadSampleIndex(L"AnalyzerError", itemNames);

		WfLexicalScopeManager manager(GetWorkflowParser(), testCpuArchitecture);
		switch (testCpuArchitecture)
		{
		case WfCpuArchitecture::x86:
			manager.attributes.Add({ L"test",L"Int" }, TypeInfoRetriver<vint32_t>::CreateTypeInfo());
			manager.attributes.Add({ L"test",L"List" }, TypeInfoRetriver<List<vint32_t>>::CreateTypeInfo());
			manager.attributes.Add({ L"test",L"Map" }, TypeInfoRetriver<Dictionary<WString, vint32_t>>::CreateTypeInfo());
			manager.attributes.Add({ L"test",L"Range" }, TypeInfoRetriver<LazyList<vint32_t>>::CreateTypeInfo());
			break;
		case WfCpuArchitecture::x64:
			manager.attributes.Add({ L"test",L"Int" }, TypeInfoRetriver<vint64_t>::CreateTypeInfo());
			manager.attributes.Add({ L"test",L"List" }, TypeInfoRetriver<List<vint64_t>>::CreateTypeInfo());
			manager.attributes.Add({ L"test",L"Map" }, TypeInfoRetriver<Dictionary<WString, vint64_t>>::CreateTypeInfo());
			manager.attributes.Add({ L"test",L"Range" }, TypeInfoRetriver<LazyList<vint64_t>>::CreateTypeInfo());
			break;
		default:
			CHECK_FAIL(L"The CPU architecture is unspecified");
		}
		manager.attributes.Add({ L"test",L"Point" }, TypeInfoRetriver<test::Point>::CreateTypeInfo());

		for (auto itemName : itemNames)
		{
			TEST_CASE(itemName)
			{
				manager.Clear(true, true);
				WString sample = LoadSample(L"AnalyzerError", itemName);
				auto module = ParseModule(sample, GetWorkflowParser());
				TEST_ASSERT(module);
				manager.AddModule(sample);
				manager.Rebuild(true);

				if (manager.errors.Count() == 0)
				{
					WfCppConfig(&manager, L"Assembly", L"Namespace");
				}

				auto sampleModule = GenerateToStream([&](StreamWriter& writer)
				{
					WfPrint(module, L"", writer);
				});
				LogSampleParseResult(L"AnalyzerError", itemName, sampleModule, module, &manager);

				const wchar_t* reading = itemName.Buffer();
				vint index = wcschr(reading, L'_') - reading;
				WString errorCode = itemName.Left(index);
				TEST_ASSERT(manager.errors.Count() > 0);
				TEST_ASSERT(manager.errors[0].message.Left(index + 1) == errorCode + L":");
			});
		}
	});
}