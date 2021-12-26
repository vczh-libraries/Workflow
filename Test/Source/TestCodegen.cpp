#include "Helper.h"

bool DecodeCodegenName(const WString& codegenName, WString& itemName, WString& itemResult, bool& cppCodegen)
{
	const wchar_t* reading = codegenName.Buffer();
	vint index = wcschr(reading, L'=') - reading;
	itemName = codegenName.Sub(0, index);
	itemResult = codegenName.Sub(index + 1, codegenName.Length() - index - 1);
	cppCodegen = true;

	if (itemName.Length() > 0 && itemName[0] == L'-')
	{
		cppCodegen = false;
		itemName = itemName.Sub(1, itemName.Length() - 1);
	}
	if (itemName.Length() > 3 && itemName.Sub(itemName.Length() - 3, 3) == L"@32")
	{
#ifdef VCZH_64
		return false;
#endif
		itemName = itemName.Sub(0, itemName.Length() - 3);
	}
	else if (itemName.Length() > 3 && itemName.Sub(itemName.Length() - 3, 3) == L"@64")
	{
#ifndef VCZH_64
		return false;
#endif
		itemName = itemName.Sub(0, itemName.Length() - 3);
	}
	return true;
}

#define DECODE_CODEGEN_NAME(EXIT_STATEMENT)\
	WString itemName, itemResult;\
	bool cppCodegen;\
	if (!DecodeCodegenName(codegenName, itemName, itemResult, cppCodegen))\
	{\
		EXIT_STATEMENT;\
	}\

TEST_FILE
{
	TEST_CATEGORY(L"Code generation")
	{
		List<WString> codegenNames, reflectableAssemblies;
		Dictionary<WString, WString> assemblyEntries;
		LoadSampleIndex(L"Codegen", codegenNames);

		WfLexicalScopeManager manager(GetWorkflowParser());
		for (auto codegenName : codegenNames)
		{
			TEST_CASE(codegenName)
			{
				DECODE_CODEGEN_NAME(return)

				TEST_PRINT(itemName);
				WString sample = LoadSample(L"Codegen", itemName);
				manager.Clear(true, true);
				{
					auto module = ParseModule(sample, GetWorkflowParser());
					TEST_ASSERT(module);
					TEST_ASSERT(manager.errors.Count() == 0);

					manager.AddModule(module);
					manager.Rebuild(true);
					LogSampleParseResult(L"Codegen", itemName, sample, module, &manager);
					TEST_ASSERT(manager.errors.Count() == 0);
				}
				Ptr<WfAssembly> assembly = GenerateAssembly(&manager);
				TEST_ASSERT(assembly);

#ifdef VCZH_MSVC
				if (cppCodegen)
				{
					auto input = MakePtr<WfCppInput>(itemName);
					input->multiFile = WfCppFileSwitch::OnDemand;
					input->reflection = WfCppFileSwitch::OnDemand;
					input->comment = L"Source: ../Resources/Codegen/" + itemName + L".txt";
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
						File file(GetCppOutputPath() + fileName);
						file.WriteAllText(code, false, BomEncoder::Mbcs);
					}
				}
#endif
				LogSampleCodegenResult(L"Codegen", itemName, assembly);
				LogSampleAssemblyBinary(L"Codegen", itemName, assembly);
			});
		}

#ifdef VCZH_MSVC
		TEST_CASE(L"TestCases.cpp")
		{
			FileStream fileStream(GetCppOutputPath() + L"TestCases.cpp", FileStream::WriteOnly);
			Utf8Encoder encoder;
			EncoderStream encoderStream(fileStream, encoder);
			StreamWriter writer(encoderStream);

			for (auto codegenName : codegenNames)
			{
				DECODE_CODEGEN_NAME(continue)
				if (!cppCodegen) continue;

				writer.WriteString(L"#include \"");
				writer.WriteString(assemblyEntries[itemName]);
				writer.WriteLine(L".h\"");
			}

			writer.WriteLine(L"");
			writer.WriteLine(L"using namespace vl;");
			writer.WriteLine(L"using namespace vl::console;");
			writer.WriteLine(L"using namespace vl::reflection::description;");

			writer.WriteLine(L"");
			writer.WriteLine(L"void LoadTestCaseTypes()");
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

			for (auto codegenName : codegenNames)
			{
				DECODE_CODEGEN_NAME(continue)
				if (!cppCodegen) continue;

				writer.WriteLine(L"");

				writer.WriteString(L"TEST_CASE(L\"");
				writer.WriteString(itemName);
				writer.WriteLine(L"\")");
				writer.WriteLine(L"{");

				writer.WriteString(L"\tWString expected = L\"");
				writer.WriteString(itemResult);
				writer.WriteLine(L"\";");

				writer.WriteString(L"\tWString actual = ::vl_workflow_global::");
				writer.WriteString(itemName);
				writer.WriteLine(L"::Instance().main();");

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