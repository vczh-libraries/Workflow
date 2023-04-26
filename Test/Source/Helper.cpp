#ifdef _MSC_VER
#include <windows.h>
#endif

#include "Helper.h"
#include "../../Source/Parser/Generated/WorkflowAst_Json.h"

WfCpuArchitecture testCpuArchitecture = WfCpuArchitecture::AsExecutable;
Ptr<workflow::Parser> workflowParser;

#define BEGIN_TIMER\
		DateTime beginTime = DateTime::LocalTime();\
		DateTime endTime\

#define PRINT_TIMER\
		endTime = DateTime::LocalTime();\
		TEST_PRINT(L"Time elapsed: " + ftow((endTime.totalMilliseconds - beginTime.totalMilliseconds) / 1000.0) + L" seconds");\
		beginTime = endTime\

workflow::Parser& GetWorkflowParser()
{
	if (!workflowParser)
	{
		BEGIN_TIMER;
		workflowParser =Ptr(new workflow::Parser);
		TEST_PRINT(L"new workflow::Parser()");
		PRINT_TIMER;
	}
	return *workflowParser.Obj();
}

void ReleaseWorkflowTable()
{
	workflowParser = nullptr;
}

#if defined VCZH_MSVC
WString GetExePath()
{
	wchar_t buffer[65536];
	GetModuleFileName(NULL, buffer, sizeof(buffer) / sizeof(*buffer));
	vint pos = -1;
	vint index = 0;
	while (buffer[index])
	{
		if (buffer[index] == L'\\')
		{
			pos = index;
		}
		index++;
	}
	return WString::CopyFrom(buffer, pos + 1);
}
#endif

WString GetTestResourcePath()
{
#if defined VCZH_MSVC
#ifdef VCZH_64
	return GetExePath() + L"..\\..\\..\\Resources\\";
#else
	return GetExePath() + L"..\\..\\Resources\\";
#endif
#elif defined VCZH_GCC
	return L"../../Resources/";
#endif
}

WString GetTestOutputBasePath()
{
#if defined VCZH_MSVC
#ifdef VCZH_64
	return GetExePath() + L"..\\..\\..\\Generated\\";
#else
	return GetExePath() + L"..\\..\\Generated\\";
#endif
#elif defined VCZH_GCC
	return L"../../Generated/";
#endif
}

const wchar_t* GetBits()
{
	switch (testCpuArchitecture)
	{
	case WfCpuArchitecture::x86: return L"32";
	case WfCpuArchitecture::x64: return L"64";
	default:
#ifdef VCZH_64
		return L"64";
#else
		return L"32";
#endif
	}
}

WString GetCppOutputPath()
{
#if defined VCZH_MSVC
	return GetTestOutputBasePath() + L"Cpp" + GetBits() + L"\\";
#elif defined VCZH_GCC
	return GetTestOutputBasePath() + L"Cpp" + GetBits() + L"/";
#endif
}

WString GetCppOutputPath32()
{
#if defined VCZH_MSVC
	return GetTestOutputBasePath() + L"Cpp32\\";
#elif defined VCZH_GCC
	return GetTestOutputBasePath() + L"Cpp32/";
#endif
}

WString GetCppOutputPath64()
{
#if defined VCZH_MSVC
	return GetTestOutputBasePath() + L"Cpp64\\";
#elif defined VCZH_GCC
	return GetTestOutputBasePath() + L"Cpp64/";
#endif
}

WString GetCppMergePath()
{
#if defined VCZH_MSVC
#ifdef VCZH_64
	return GetExePath() + L"..\\..\\..\\SourceCppGen\\";
#else
	return GetExePath() + L"..\\..\\SourceCppGen\\";
#endif
#elif defined VCZH_GCC
	return L"../../SourceCppGen/";
#endif
}

WString GetWorkflowOutputPath()
{
#if defined VCZH_MSVC
	return GetTestOutputBasePath() + L"Workflow" + GetBits() + L"\\";
#elif defined VCZH_GCC
	return GetTestOutputBasePath() + L"Workflow" + GetBits() + L"/";
#endif
}

void LoadSampleIndex(const WString& sampleName, List<WString>& itemNames)
{
	FileStream fileStream(GetTestResourcePath() + L"Index" + sampleName + L".txt", FileStream::ReadOnly);
	BomDecoder decoder;
	DecoderStream decoderStream(fileStream, decoder);
	StreamReader reader(decoderStream);

	while (!reader.IsEnd())
	{
		itemNames.Add(reader.ReadLine());
	}
}

WString LoadSample(const WString& sampleName, const WString& itemName)
{
#if defined VCZH_MSVC
	FileStream fileStream(GetTestResourcePath() + sampleName + L"\\" + itemName + L".txt", FileStream::ReadOnly);
#elif defined VCZH_GCC
	FileStream fileStream(GetTestResourcePath() + sampleName + L"/" + itemName + L".txt", FileStream::ReadOnly);
#endif
	BomDecoder decoder;
	DecoderStream decoderStream(fileStream, decoder);
	StreamReader reader(decoderStream);
	return reader.ReadToEnd();
}

void LoadSampleAssemblyBinary(const WString& sampleName, const WString& itemName, Ptr<WfAssembly>& assembly)
{
	auto path = GetWorkflowOutputPath() + L"Assembly." + sampleName + L"." + itemName + L".bin";
	FileStream fileStream(path, FileStream::ReadOnly);
	WfAssemblyLoadErrors errors;
	assembly = WfAssembly::Deserialize(fileStream, errors);
	TEST_ASSERT(assembly);
}

void LogSampleParseResult(const WString& sampleName, const WString& itemName, const WString& sample, Ptr<glr::ParsingAstBase> typedNode, WfLexicalScopeManager* manager)
{
	FileStream fileStream(GetWorkflowOutputPath() + L"Parsing." + sampleName + L"." + itemName + L".txt", FileStream::WriteOnly);
	BomEncoder encoder(BomEncoder::Utf8);
	EncoderStream encoderStream(fileStream, encoder);
	StreamWriter writer(encoderStream);

	writer.WriteLine(L"========================================================");
	writer.WriteLine(L"Original");
	writer.WriteLine(L"========================================================");
	writer.WriteLine(sample);

	if (typedNode)
	{
		writer.WriteLine(L"========================================================");
		writer.WriteLine(L"Printed");
		writer.WriteLine(L"========================================================");
		if (auto expr = typedNode.Cast<WfExpression>())
		{
			WfPrint(expr, L"", writer);
		}
		else if (auto stat = typedNode.Cast<WfStatement>())
		{
			WfPrint(stat, L"", writer);
		}
		else if (auto decl = typedNode.Cast<WfDeclaration>())
		{
			WfPrint(decl, L"", writer);
		}
		else if (auto module = typedNode.Cast<WfModule>())
		{
			WfPrint(module, L"", writer);
		}
		writer.WriteLine(L"");
	}

	if (manager && manager->errors.Count() > 0)
	{
		writer.WriteLine(L"========================================================");
		writer.WriteLine(L"Errors");
		writer.WriteLine(L"========================================================");
		for (auto error : manager->errors)
		{
			writer.WriteLine(L"Line: " + itow(error.codeRange.start.row + 1) + L", Column: " + itow(error.codeRange.start.column + 1) + L", Message: " + error.message);
		}
	}

	writer.WriteLine(L"========================================================");
	writer.WriteLine(L"AST");
	writer.WriteLine(L"========================================================");
	if (auto typeAst = typedNode.Cast<WfType>())
	{
		json_visitor::AstVisitor(writer).Print(typeAst.Obj());
	}
	else if (auto exprAst = typedNode.Cast<WfExpression>())
	{
		json_visitor::AstVisitor(writer).Print(exprAst.Obj());
	}
	else if (auto statAst = typedNode.Cast<WfStatement>())
	{
		json_visitor::AstVisitor(writer).Print(statAst.Obj());
	}
	else if (auto declAst = typedNode.Cast<WfDeclaration>())
	{
		json_visitor::AstVisitor(writer).Print(declAst.Obj());
	}
	else if (auto moduleAst = typedNode.Cast<WfModule>())
	{
		json_visitor::AstVisitor(writer).Print(moduleAst.Obj());
	}
}

void LogSampleCodegenResult(const WString& sampleName, const WString& itemName, Ptr<WfAssembly> assembly)
{
	FileStream fileStream(GetWorkflowOutputPath() + L"Assembly." + sampleName + L"." + itemName + L".txt", FileStream::WriteOnly);
	BomEncoder encoder(BomEncoder::Utf8);
	EncoderStream encoderStream(fileStream, encoder);
	StreamWriter writer(encoderStream);

	writer.WriteLine(L"========================================================");
	writer.WriteLine(L"Global Variables:");
	writer.WriteLine(L"========================================================");
	for (auto name : assembly->variableNames)
	{
		writer.WriteLine(name);
	}
	writer.WriteLine(L"");

	writer.WriteLine(L"========================================================");
	writer.WriteLine(L"Functions:");
	writer.WriteLine(L"========================================================");
	for (auto function : assembly->functions)
	{
		writer.WriteLine(function->name + L" (" + itow(function->firstInstruction) + L" .. " + itow(function->lastInstruction) + L")");

		writer.WriteLine(L"    Arguments:");
		for (auto name : function->argumentNames)
		{
			writer.WriteLine(L"        " + name);
		}

		writer.WriteLine(L"    Captured Variables:");
		for (auto name : function->capturedVariableNames)
		{
			writer.WriteLine(L"        " + name);
		}

		writer.WriteLine(L"    Local Variables:");
		for (auto name : function->localVariableNames)
		{
			writer.WriteLine(L"        " + name);
		}

		writer.WriteLine(L"");
	}

	writer.WriteLine(L"========================================================");
	writer.WriteLine(L"Instructions:");
	writer.WriteLine(L"========================================================");

	auto formatText = [](const WString& text, vint length)
	{
		WString result = text;
		while (result.Length() < length)
		{
			result += L" ";
		}
		return result;
	};

	auto formatFlag = [](Value::ValueType type)->WString
	{
		switch (type)
		{
		case Value::RawPtr:
			return L"RawPtr";
		case Value::SharedPtr:
			return L"SharedPtr";
		case Value::BoxedValue:
			return L"BoxedValue";
		default:
			return L"Null";
		}
	};

	auto formatType = [](WfInsType type)->WString
	{
		switch (type)
		{
		case WfInsType::Bool:
			return L"Bool";
		case WfInsType::I1:
			return L"I1";
		case WfInsType::I2:
			return L"I2";
		case WfInsType::I4:
			return L"I4";
		case WfInsType::I8:
			return L"I8";
		case WfInsType::U1:
			return L"U1";
		case WfInsType::U2:
			return L"U2";
		case WfInsType::U4:
			return L"U4";
		case WfInsType::U8:
			return L"U8";
		case WfInsType::F4:
			return L"F4";
		case WfInsType::F8:
			return L"F8";
		case WfInsType::String:
			return L"String";
		default:
			return L"Unknown";
		}
	};

	auto formatValue = [&](const WfRuntimeValue& value)->WString
	{
		if (value.typeDescriptor)
		{
			switch (value.type)
			{
			case WfInsType::U8:			return L"<enum: " + u64tow(value.u8Value) + L">";
			case WfInsType::Unknown:	return L"<type: " + value.typeDescriptor->GetTypeName() + L">";
			default:;
			}
			CHECK_FAIL(L"Unexpected type in WfRuntimeValue with typeDescriptor!");
		}
		switch (value.type)
		{
		case WfInsType::Bool:			return L"<" + formatType(value.type) + L", " + (value.boolValue ? L"true" : L"false") + L">";
		case WfInsType::I1:				return L"<" + formatType(value.type) + L", " + i64tow(value.i1Value) + L">";
		case WfInsType::I2:				return L"<" + formatType(value.type) + L", " + i64tow(value.i2Value) + L">";
		case WfInsType::I4:				return L"<" + formatType(value.type) + L", " + i64tow(value.i4Value) + L">";
		case WfInsType::I8:				return L"<" + formatType(value.type) + L", " + i64tow(value.i8Value) + L">";
		case WfInsType::U1:				return L"<" + formatType(value.type) + L", " + u64tow(value.u1Value) + L">";
		case WfInsType::U2:				return L"<" + formatType(value.type) + L", " + u64tow(value.u2Value) + L">";
		case WfInsType::U4:				return L"<" + formatType(value.type) + L", " + u64tow(value.u4Value) + L">";
		case WfInsType::U8:				return L"<" + formatType(value.type) + L", " + u64tow(value.u8Value) + L">";
		case WfInsType::F4:				return L"<" + formatType(value.type) + L", " + ftow(value.f4Value) + L">";
		case WfInsType::F8:				return L"<" + formatType(value.type) + L", " + ftow(value.f8Value) + L">";
		case WfInsType::String:			return L"<" + formatType(value.type) + L", " + value.stringValue + L">";
		default:;
		}
		return L"<null>";
	};

	auto formatVarName = [assembly](const WfInstruction& ins, vint index)->WString
	{
		switch (ins.code)
		{
		case WfInsCode::LoadGlobalVar:
		case WfInsCode::StoreGlobalVar:
			return L"(" + assembly->variableNames[ins.indexParameter] + L")";
		case WfInsCode::LoadLocalVar:
		case WfInsCode::StoreLocalVar:
		{
			auto function = From(assembly->functions)
				.Where([&ins, index](Ptr<WfAssemblyFunction> function)
			{
				return function->firstInstruction <= index && index <= function->lastInstruction;
			})
				.First();
			if (ins.indexParameter < function->argumentNames.Count())
			{
				return L"(" + function->argumentNames[ins.indexParameter] + L")";
			}
			else
			{
				return L"(" + function->localVariableNames[ins.indexParameter - function->argumentNames.Count()] + L")";
			}
		}
		case WfInsCode::LoadCapturedVar:
		{
			auto function = From(assembly->functions)
				.Where([&ins, index](Ptr<WfAssemblyFunction> function)
			{
				return function->firstInstruction <= index && index <= function->lastInstruction;
			})
				.First();
			return L"(" + function->capturedVariableNames[ins.indexParameter] + L")";
		}
		default:;
		}
		return L"";
	};

#define LOG(NAME)						case WfInsCode::NAME: writer.WriteLine(formatText(itow(index), 5) + L": " + formatText(L"    " L ## #NAME, 18)); break;
#define LOG_VALUE(NAME)					case WfInsCode::NAME: writer.WriteLine(formatText(itow(index), 5) + L": " + formatText(L"    " L ## #NAME, 18) + L": value = " + formatValue(ins.valueParameter)); break;
#define LOG_FUNCTION(NAME)				case WfInsCode::NAME: writer.WriteLine(formatText(itow(index), 5) + L": " + formatText(L"    " L ## #NAME, 18) + L": func = " + itow(ins.indexParameter) + L"(" + assembly->functions[ins.indexParameter]->name + L")"); break;
#define LOG_FUNCTION_COUNT(NAME)		case WfInsCode::NAME: writer.WriteLine(formatText(itow(index), 5) + L": " + formatText(L"    " L ## #NAME, 18) + L": func = " + itow(ins.indexParameter) + L"(" + assembly->functions[ins.indexParameter]->name + L"), stackPatternCount = " + itow(ins.countParameter)); break;
#define LOG_VARIABLE(NAME)				case WfInsCode::NAME: writer.WriteLine(formatText(itow(index), 5) + L": " + formatText(L"    " L ## #NAME, 18) + L": var = " + itow(ins.indexParameter) + formatVarName(ins, index)); break;
#define LOG_COUNT(NAME)					case WfInsCode::NAME: writer.WriteLine(formatText(itow(index), 5) + L": " + formatText(L"    " L ## #NAME, 18) + L": stackPatternCount = " + itow(ins.countParameter)); break;
#define LOG_FLAG_TYPEDESCRIPTOR(NAME)	case WfInsCode::NAME: writer.WriteLine(formatText(itow(index), 5) + L": " + formatText(L"    " L ## #NAME, 18) + L": flag = " + formatFlag(ins.flagParameter) + L", typeDescriptor = " + ins.typeDescriptorParameter->GetTypeName()); break;
#define LOG_PROPERTY(NAME)				case WfInsCode::NAME: writer.WriteLine(formatText(itow(index), 5) + L": " + formatText(L"    " L ## #NAME, 18) + L": propertyInfo = " + ins.propertyParameter->GetName() + L"<" + ins.propertyParameter->GetOwnerTypeDescriptor()->GetTypeName() + L">"); break;
#define LOG_METHOD(NAME)				case WfInsCode::NAME: writer.WriteLine(formatText(itow(index), 5) + L": " + formatText(L"    " L ## #NAME, 18) + L": methodInfo = " + ins.methodParameter->GetName() + L"<" + ins.methodParameter->GetOwnerTypeDescriptor()->GetTypeName() + L">"); break;
#define LOG_METHOD_COUNT(NAME)			case WfInsCode::NAME: writer.WriteLine(formatText(itow(index), 5) + L": " + formatText(L"    " L ## #NAME, 18) + L": methodInfo = " + ins.methodParameter->GetName() + L"<" + ins.methodParameter->GetOwnerTypeDescriptor()->GetTypeName() + L">, stackPatternCount = " + itow(ins.countParameter)); break;
#define LOG_EVENT(NAME)					case WfInsCode::NAME: writer.WriteLine(formatText(itow(index), 5) + L": " + formatText(L"    " L ## #NAME, 18) + L": eventInfo = " + ins.eventParameter->GetName() + L"<" + ins.eventParameter->GetOwnerTypeDescriptor()->GetTypeName() + L">"); break;
#define LOG_EVENT_COUNT(NAME)			case WfInsCode::NAME: writer.WriteLine(formatText(itow(index), 5) + L": " + formatText(L"    " L ## #NAME, 18) + L": eventInfo = " + ins.eventParameter->GetName() + L"<" + ins.eventParameter->GetOwnerTypeDescriptor()->GetTypeName() + L">, stackPatternCount = " + itow(ins.countParameter)); break;
#define LOG_LABEL(NAME)					case WfInsCode::NAME: writer.WriteLine(formatText(itow(index), 5) + L": " + formatText(L"    " L ## #NAME, 18) + L": label = " + itow(ins.indexParameter)); break;
#define LOG_TYPE(NAME)					case WfInsCode::NAME: writer.WriteLine(formatText(itow(index), 5) + L": " + formatText(L"    " L ## #NAME, 18) + L": type = " + formatType(ins.typeParameter)); break;

	for (auto [ins, index] : indexed(assembly->instructions))
	{
		auto range = assembly->insAfterCodegen->instructionCodeMapping[index];
		if (range.codeIndex != -1)
		{
			auto code = assembly->insAfterCodegen->moduleCodes[range.codeIndex];
			auto sample = code.Sub(range.start.index, range.end.index - range.start.index + 1);
			stream::StringReader reader(sample);
			while (!reader.IsEnd())
			{
				auto line = reader.ReadLine();
				writer.WriteLine(L"//  " + line);
				break;
			}
		}
		switch (ins.code)
		{
			INSTRUCTION_CASES(
				LOG,
				LOG_VALUE,
				LOG_FUNCTION,
				LOG_FUNCTION_COUNT,
				LOG_VARIABLE,
				LOG_COUNT,
				LOG_FLAG_TYPEDESCRIPTOR,
				LOG_PROPERTY,
				LOG_METHOD,
				LOG_METHOD_COUNT,
				LOG_EVENT,
				LOG_EVENT_COUNT,
				LOG_LABEL,
				LOG_TYPE)
		}
	}

#undef LOG
#undef LOG_VALUE
#undef LOG_FUNCTION
#undef LOG_FUNCTION_COUNT
#undef LOG_VARIABLE
#undef LOG_COUNT
#undef LOG_FLAG_TYPEDESCRIPTOR
#undef LOG_PROPERTY
#undef LOG_METHOD
#undef LOG_METHOD_COUNT
#undef LOG_EVENT
#undef LOG_EVENT_COUNT
#undef LOG_LABEL
#undef LOG_TYPE
}

void LogSampleAssemblyBinary(const WString& sampleName, const WString& itemName, Ptr<WfAssembly> assembly)
{
	auto path = GetWorkflowOutputPath() + L"Assembly." + sampleName + L"." + itemName + L".bin";
	FileStream fileStream(path, FileStream::WriteOnly);
	assembly->Serialize(fileStream);
	TEST_PRINT(L"    serialized: " + i64tow(fileStream.Size()) + L" bytes");
}