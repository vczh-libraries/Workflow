#include "Helper.h"
#include "CppTypes.h"

void LoadMultipleSamples(WfLexicalScopeManager* manager, const WString& sampleName)
{
	Ptr<ParsingTable> table = GetWorkflowTable();
	List<WString> itemNames;
	LoadSampleIndex(sampleName, itemNames);
	FOREACH(WString, itemName, itemNames)
	{
		WString sample = LoadSample(sampleName, itemName);

		List<Ptr<ParsingError>> errors;
		Ptr<ParsingTreeNode> node = WfParseModuleAsParsingTreeNode(sample, table, errors);
		TEST_ASSERT(node);

		List<RegexToken> tokens;
		Ptr<WfModule> module = WfConvertParsingTreeNode(node, tokens).Cast<WfModule>();
		manager->AddModule(module);

		MemoryStream stream;
		{
			StreamWriter writer(stream);
			WfPrint(module, L"", writer);
		}
		{
			stream.SeekFromBegin(0);
			StreamReader reader(stream);
			LogSampleParseResult(sampleName, itemName, reader.ReadToEnd(), node, module);
		}
	}
}

TEST_CASE(TestBuildGlobalName)
{
	WfLexicalScopeManager manager(GetWorkflowTable());
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
		TEST_ASSERT(name->parent ==parent.Obj());
	}
	{
		auto parent = manager.globalName->children[L"system"]->children[L"reflection"];
		auto name = parent->children[L"TypeDescriptor"];
		TEST_ASSERT(name->typeDescriptor == GetTypeDescriptor<ITypeDescriptor>());
		TEST_ASSERT(name->declarations.Count() == 0);
		TEST_ASSERT(name->parent ==parent.Obj());
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
}

TEST_CASE(TestAnalyzerError)
{
	Ptr<ParsingTable> table = GetWorkflowTable();
	List<WString> itemNames;
	LoadSampleIndex(L"AnalyzerError", itemNames);

	WfLexicalScopeManager manager(table);
	manager.attributes.Add({ L"test",L"Int" }, TypeInfoRetriver<vint>::CreateTypeInfo());
	manager.attributes.Add({ L"test",L"List" }, TypeInfoRetriver<List<vint>>::CreateTypeInfo());
	manager.attributes.Add({ L"test",L"Map" }, TypeInfoRetriver<Dictionary<WString, vint>>::CreateTypeInfo());
	manager.attributes.Add({ L"test",L"Range" }, TypeInfoRetriver<LazyList<vint>>::CreateTypeInfo());
	manager.attributes.Add({ L"test",L"Point" }, TypeInfoRetriver<test::Point>::CreateTypeInfo());

	FOREACH(WString, itemName, itemNames)
	{
		UnitTest::PrintInfo(itemName);
		WString sample = LoadSample(L"AnalyzerError", itemName);
		List<Ptr<ParsingError>> errors;
		Ptr<ParsingTreeNode> node = WfParseModuleAsParsingTreeNode(sample, table, errors);
		TEST_ASSERT(errors.Count() == 0);
		TEST_ASSERT(node);

		manager.Clear(true, true);
		List<RegexToken> tokens;
		Ptr<WfModule> module = WfConvertParsingTreeNode(node, tokens).Cast<WfModule>();
		manager.AddModule(module);
		manager.Rebuild(true);
		MemoryStream stream;
		{
			StreamWriter writer(stream);
			WfPrint(module, L"", writer);
		}
		{
			stream.SeekFromBegin(0);
			StreamReader reader(stream);
			LogSampleParseResult(L"AnalyzerError", itemName, reader.ReadToEnd(), node, module, &manager);
		}

		const wchar_t* reading = itemName.Buffer();
		vint index = wcschr(reading, L'_') - reading;
		WString errorCode = itemName.Left(index);
		TEST_ASSERT(manager.errors.Count() > 0);
		TEST_ASSERT(manager.errors[0]->errorMessage.Left(index + 1) == errorCode + L":");
	}
}