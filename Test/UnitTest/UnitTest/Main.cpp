#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <windows.h>
#endif

#include "../../Source/TestFunctions.h"

#if defined VCZH_MSVC
using namespace vl::filesystem;
#endif

Ptr<ParsingTable> workflowTable;

#define BEGIN_TIMER\
		DateTime beginTime = DateTime::LocalTime();\
		DateTime endTime;\

#define PRINT_TIMER\
		endTime = DateTime::LocalTime();\
		unittest::UnitTest::PrintInfo(L"Time elapsed: " + ftow((endTime.totalMilliseconds - beginTime.totalMilliseconds) / 1000.0) + L" seconds");\
		beginTime = endTime;\

Ptr<ParsingTable> GetWorkflowTable()
{
	if (!workflowTable)
	{
		BEGIN_TIMER

		unittest::UnitTest::PrintInfo(L"GetWorkFlowTable()");
		auto table = WfLoadTable();
		unittest::UnitTest::PrintInfo(L"Finished WfLoadTable()");
		PRINT_TIMER

		MemoryStream stream;
		table->Serialize(stream);
		stream.SeekFromBegin(0);
		unittest::UnitTest::PrintInfo(L"Finished serializing parsing table: " + i64tow(stream.Size()) + L" bytes");
		PRINT_TIMER

		Ptr<ParsingTable> deserializedTable = new ParsingTable(stream);
		TEST_ASSERT(stream.Position() == stream.Size());
		unittest::UnitTest::PrintInfo(L"Finished deserializing parsing table");
		PRINT_TIMER

		deserializedTable->Initialize();
		unittest::UnitTest::PrintInfo(L"Finished initializing parsing table");
		PRINT_TIMER

		workflowTable = deserializedTable;
	}
	return workflowTable;
}

#if defined VCZH_MSVC
WString GetExePath()
{
	wchar_t buffer[65536];
	GetModuleFileName(NULL, buffer, sizeof(buffer)/sizeof(*buffer));
	vint pos=-1;
	vint index=0;
	while(buffer[index])
	{
		if(buffer[index]==L'\\')
		{
			pos=index;
		}
		index++;
	}
	return WString(buffer, pos + 1);
}
#endif

WString GetTestResourcePath()
{
#if defined VCZH_MSVC
#ifdef _WIN64
	return GetExePath() + L"..\\..\\..\\Resources\\";
#else
	return GetExePath() + L"..\\..\\Resources\\";
#endif
#elif defined VCZH_GCC
	return L"../Resources/";
#endif
}

WString GetTestOutputPath()
{
#if defined VCZH_MSVC
#ifdef _WIN64
	return GetExePath() + L"..\\..\\..\\Output\\";
#else
	return GetExePath() + L"..\\..\\Output\\";
#endif
#elif defined VCZH_GCC
	return L"../Output/";
#endif
}

void LoadSampleIndex(const WString& sampleName, List<WString>& itemNames)
{
	FileStream fileStream(GetTestOutputPath() + L"Index" + sampleName + L".txt", FileStream::ReadOnly);
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
	FileStream fileStream(GetTestOutputPath() + sampleName + L"\\" + itemName + L".txt", FileStream::ReadOnly);
#elif defined VCZH_GCC
	FileStream fileStream(GetTestOutputPath() + sampleName + L"/" + itemName + L".txt", FileStream::ReadOnly);
#endif
	BomDecoder decoder;
	DecoderStream decoderStream(fileStream, decoder);
	StreamReader reader(decoderStream);
	return reader.ReadToEnd();
}

void LogSampleParseResult(const WString& sampleName, const WString& itemName, const WString& sample, Ptr<ParsingTreeNode> node, WfLexicalScopeManager* manager)
{
	FileStream fileStream(GetTestOutputPath() + L"Parsing." + sampleName + L"." + itemName + L".txt", FileStream::WriteOnly);
	BomEncoder encoder(BomEncoder::Utf16);
	EncoderStream encoderStream(fileStream, encoder);
	StreamWriter writer(encoderStream);
	writer.WriteLine(sample);

	if (manager && manager->errors.Count() > 0)
	{
		writer.WriteLine(L"========================================================");
		FOREACH(Ptr<ParsingError>, error, manager->errors)
		{
			writer.WriteLine(L"Line: " + itow(error->codeRange.start.row + 1) + L", Column: " + itow(error->codeRange.start.column + 1) + L", Message: " + error->errorMessage);
		}
	}

	writer.WriteLine(L"========================================================");
	Log(node.Obj(), L"", writer);
}

void LogSampleCodegenResult(const WString& sampleName, const WString& itemName, Ptr<WfAssembly> assembly)
{
	FileStream fileStream(GetTestOutputPath() + L"Assembly." + sampleName + L"." + itemName + L".txt", FileStream::WriteOnly);
	BomEncoder encoder(BomEncoder::Utf16);
	EncoderStream encoderStream(fileStream, encoder);
	StreamWriter writer(encoderStream);
	
	writer.WriteLine(L"========================================================");
	writer.WriteLine(L"Global Variables:");
	writer.WriteLine(L"========================================================");
	FOREACH(WString, name, assembly->variableNames)
	{
		writer.WriteLine(name);
	}
	writer.WriteLine(L"");
	
	writer.WriteLine(L"========================================================");
	writer.WriteLine(L"Functions:");
	writer.WriteLine(L"========================================================");
	FOREACH(Ptr<WfAssemblyFunction>, function, assembly->functions)
	{
		writer.WriteLine(function->name + L" (" + itow(function->firstInstruction) + L" .. " + itow(function->lastInstruction) + L")");

		writer.WriteLine(L"    Arguments:");
		FOREACH(WString, name, function->argumentNames)
		{
			writer.WriteLine(L"        " + name);
		}

		writer.WriteLine(L"    Captured Variables:");
		FOREACH(WString, name, function->capturedVariableNames)
		{
			writer.WriteLine(L"        " + name);
		}

		writer.WriteLine(L"    Local Variables:");
		FOREACH(WString, name, function->localVariableNames)
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
		case Value::Text:
			return L"Text";
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
			return L"Bool";
		default:
			return L"Unknown";
		}
	};

	auto formatValue = [&formatFlag](const Value& value)->WString
	{
		return L"<" +
			value.GetText() + L", " +
			formatFlag(value.GetValueType()) +
			(value.GetTypeDescriptor() ? L", " + value.GetTypeDescriptor()->GetTypeName() : L"") +
			L">";
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
				auto function=From(assembly->functions)
					.Where([&ins,index](Ptr<WfAssemblyFunction> function)
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
				auto function=From(assembly->functions)
					.Where([&ins,index](Ptr<WfAssemblyFunction> function)
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
#define LOG_METHOD_COUNT(NAME)			case WfInsCode::NAME: writer.WriteLine(formatText(itow(index), 5) + L": " + formatText(L"    " L ## #NAME, 18) + L": methodInfo = " + ins.methodParameter->GetName() + L"<" + ins.methodParameter->GetOwnerTypeDescriptor()->GetTypeName() + L">, stackPatternCount = " + itow(ins.countParameter)); break;
#define LOG_EVENT(NAME)					case WfInsCode::NAME: writer.WriteLine(formatText(itow(index), 5) + L": " + formatText(L"    " L ## #NAME, 18) + L": eventInfo = " + ins.eventParameter->GetName() + L"<" + ins.eventParameter->GetOwnerTypeDescriptor()->GetTypeName() + L">"); break;
#define LOG_LABEL(NAME)					case WfInsCode::NAME: writer.WriteLine(formatText(itow(index), 5) + L": " + formatText(L"    " L ## #NAME, 18) + L": label = " + itow(ins.indexParameter)); break;
#define LOG_TYPE(NAME)					case WfInsCode::NAME: writer.WriteLine(formatText(itow(index), 5) + L": " + formatText(L"    " L ## #NAME, 18) + L": type = " + formatType(ins.typeParameter)); break;

	FOREACH_INDEXER(WfInstruction, ins, index, assembly->instructions)
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
				LOG_METHOD_COUNT,
				LOG_EVENT,
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
#undef LOG_METHOD_COUNT
#undef LOG_EVENT
#undef LOG_LABEL
#undef LOG_TYPE
}

namespace test
{
	struct Point
	{
		int									x = 0;
		int									y = 0;
	};

	class ObservableValue : public Object, public Description<ObservableValue>
	{
	protected:
		vint								value;
		WString								name;
	public:
		Event<void(vint, vint)>				ValueChanged;

		ObservableValue()
			:value(0)
		{
		}

		ObservableValue(vint _value)
			:value(_value)
		{
		}

		ObservableValue(vint _value, Nullable<double> x)
			:value(_value)
		{
		}

		ObservableValue(vint _value, Nullable<bool> x)
			:value(_value)
		{
		}

		static Ptr<ObservableValue> Create(vint value, const WString& name)
		{
			auto ov = MakePtr<ObservableValue>(value);
			ov->name = name;
			return ov;
		}

		static ObservableValue* CreatePtr(vint value, const WString& name)
		{
			auto ov = new ObservableValue(value);
			ov->name = name;
			return ov;
		}

		vint GetValue()
		{
			return value;
		}

		void SetValue(vint newValue)
		{
			vint oldValue = value;
			value = newValue;
			ValueChanged(oldValue, newValue);
		}

		WString GetName()
		{
			return name;
		}

		void SetName(const WString& newName)
		{
			name = newName;
		}

		WString GetDisplayName()
		{
			return L"This is " + name;
		}
	};
}

namespace vl
{
	namespace reflection
	{
		namespace description
		{
			using namespace test;

#define _ ,
#define UNITTEST_TYPELIST(F)\
			F(test::Point)\
			F(test::ObservableValue)\

			UNITTEST_TYPELIST(DECL_TYPE_INFO)
			UNITTEST_TYPELIST(IMPL_CPP_TYPE_INFO)

			BEGIN_STRUCT_MEMBER(Point)
				STRUCT_MEMBER(x)
				STRUCT_MEMBER(y)
			END_STRUCT_MEMBER(Point)

			BEGIN_CLASS_MEMBER(ObservableValue)
				CLASS_MEMBER_CONSTRUCTOR(Ptr<ObservableValue>(), NO_PARAMETER)
				CLASS_MEMBER_CONSTRUCTOR(Ptr<ObservableValue>(vint), { L"value" })
				CLASS_MEMBER_CONSTRUCTOR(ObservableValue*(vint, Nullable<double>), { L"value" _ L"x" })
				CLASS_MEMBER_CONSTRUCTOR(ObservableValue*(vint, Nullable<bool>), { L"value" _ L"x" })
				CLASS_MEMBER_STATIC_METHOD(Create, { L"value" _ L"name" })
				CLASS_MEMBER_STATIC_METHOD(CreatePtr, { L"value" _ L"name" })

				CLASS_MEMBER_EVENT(ValueChanged)
				CLASS_MEMBER_PROPERTY_EVENT_FAST(Value, ValueChanged)
				CLASS_MEMBER_PROPERTY_FAST(Name)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(DisplayName);
			END_CLASS_MEMBER(ObservableValue)

			class UnitTestTypeLoader : public Object, public ITypeLoader
			{
			public:
				void Load(ITypeManager* manager)
				{
					UNITTEST_TYPELIST(ADD_TYPE_INFO)
				}

				void Unload(ITypeManager* manager)
				{
				}
			};

			bool LoadUnitTestTypes()
			{
				ITypeManager* manager = GetGlobalTypeManager();
				if(manager)
				{
					Ptr<ITypeLoader> loader = new UnitTestTypeLoader;
					return manager->AddTypeLoader(loader);
				}
				return false;
			}

#undef _
		}
	}
}

void LoadTypes()
{
	LoadPredefinedTypes();
	LoadParsingTypes();
	XmlLoadTypes();
	JsonLoadTypes();
	WfLoadTypes();
	LoadUnitTestTypes();
	TEST_ASSERT(GetGlobalTypeManager()->Load());
}

void UnloadTypes()
{
	TEST_ASSERT(GetGlobalTypeManager()->Unload());
	TEST_ASSERT(ResetGlobalTypeManager());
}

#if defined VCZH_MSVC
int wmain(int argc, wchar_t* argv[])
#elif defined VCZH_GCC
int main()
#endif
{
#if defined VCZH_MSVC
	Folder folder(GetTestOutputPath());
	if (!folder.Exists())
	{
		TEST_ASSERT(folder.Create(false) == true);
	}
#endif
	LoadTypes();
	unittest::UnitTest::RunAndDisposeTests();
	UnloadTypes();
	workflowTable = 0;
	ThreadLocalStorage::DisposeStorages();
#ifdef VCZH_MSVC
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}
