#include "WfRuntimeAssembly.h"

namespace vl
{
	using namespace reflection::description;
	using namespace workflow::runtime;
	using namespace workflow::typeimpl;
	using namespace collections;

	namespace stream
	{
		namespace internal
		{
			BEGIN_SERIALIZATION(WfInstructionDebugInfo)
				SERIALIZE(moduleCodes)
				SERIALIZE(instructionCodeMapping)
			END_SERIALIZATION

			BEGIN_SERIALIZATION(WfAssemblyFunction)
				SERIALIZE(name)
				SERIALIZE(argumentNames)
				SERIALIZE(capturedVariableNames)
				SERIALIZE(localVariableNames)
				SERIALIZE(firstInstruction)
				SERIALIZE(lastInstruction)
			END_SERIALIZATION

			SERIALIZE_ENUM(WfInsCode)
			SERIALIZE_ENUM(WfInsType)
			SERIALIZE_ENUM(Value::ValueType)

/***********************************************************************
Serizliation (Type)
***********************************************************************/

			template<>
			struct Serialization<ITypeDescriptor*>
			{
				static void IO(Reader& reader, ITypeDescriptor*& value)
				{
					WString id;
					reader << id;
					value = GetTypeDescriptor(id);
					CHECK_ERROR(value, L"Failed to load type.");
				}
					
				static void IO(Writer& writer, ITypeDescriptor*& value)
				{
					WString id = value->GetTypeName();
					writer << id;
				}
			};

			template<>
			struct Serialization<IMethodInfo*>
			{
				static void IO(Reader& reader, IMethodInfo*& value)
				{
					ITypeDescriptor* type = 0;
					WString name;
					List<WString> parameters;
					reader << type << name << parameters;
					auto group =
						name == L"#ctor" ? type->GetConstructorGroup() :
						type->GetMethodGroupByName(name, false);
					CHECK_ERROR(group, L"Failed to load method.");

					value = 0;
					vint count = group->GetMethodCount();
					for (vint i = 0; i < count; i++)
					{
						auto method = group->GetMethod(i);
						if (method->GetParameterCount() == parameters.Count())
						{
							bool found = true;
							for (vint j = 0; j < parameters.Count(); j++)
							{
								if (method->GetParameter(j)->GetName() != parameters[j])
								{
									found = false;
									break;
								}
							}

							if (found)
							{
								CHECK_ERROR(!value, L"Failed to load method.");
								value = method;
							}
						}
					}
					CHECK_ERROR(value, L"Failed to load method.");
				}
					
				static void IO(Writer& writer, IMethodInfo*& value)
				{
					auto type = value->GetOwnerTypeDescriptor();
					WString name =
						value->GetOwnerMethodGroup() == type->GetConstructorGroup() ? L"#ctor" :
						value->GetName();
					writer << type << name;

					List<WString> parameters;
					vint count = value->GetParameterCount();
					for (vint i = 0; i < count; i++)
					{
						parameters.Add(value->GetParameter(i)->GetName());
					}
					writer << parameters;
				}
			};

			template<>
			struct Serialization<IPropertyInfo*>
			{
				static void IO(Reader& reader, IPropertyInfo*& value)
				{
					ITypeDescriptor* type = 0;
					WString name;
					reader << type << name;
					value = type->GetPropertyByName(name, false);
					CHECK_ERROR(value, L"Failed to load property.");
				}
					
				static void IO(Writer& writer, IPropertyInfo*& value)
				{
					auto type = value->GetOwnerTypeDescriptor();
					WString name = value->GetName();
					writer << type << name;
				}
			};

			template<>
			struct Serialization<IEventInfo*>
			{
				static void IO(Reader& reader, IEventInfo*& value)
				{
					ITypeDescriptor* type = 0;
					WString name;
					reader << type << name;
					value = type->GetEventByName(name, false);
					CHECK_ERROR(value, L"Failed to load event.");
				}
					
				static void IO(Writer& writer, IEventInfo*& value)
				{
					auto type = value->GetOwnerTypeDescriptor();
					WString name = value->GetName();
					writer << type << name;
				}
			};

			template<>
			struct Serialization<Value>
			{
				static void IO(Reader& reader, Value& value)
				{
					WString id, text;
					reader << id << text;
					if (id == L"")
					{
						value = Value();
					}
					else
					{
						auto type = GetTypeDescriptor(id);
						if (type == GetTypeDescriptor<ITypeDescriptor>())
						{
							type = GetTypeDescriptor(text);
							CHECK_ERROR(type, L"Failed to load type.");
							value = Value::From(type);
						}
						else
						{
							type->GetValueSerializer()->Parse(text, value);
						}
					}
				}
					
				static void IO(Writer& writer, Value& value)
				{
					WString id;
					if (value.GetTypeDescriptor())
					{
						id = value.GetTypeDescriptor()->GetTypeName();
					}
					writer << id;

					if (value.GetTypeDescriptor() == GetTypeDescriptor<ITypeDescriptor>())
					{
						WString text = UnboxValue<ITypeDescriptor*>(value)->GetTypeName();
						writer << text;
					}
					else
					{
						WString text = value.GetText();
						writer << text;
					}
				}
			};

/***********************************************************************
Serialization (TypeImpl)
***********************************************************************/

			template<>
			struct Serialization<WfTypeImpl>
			{
				static void IO(Reader& reader, WfTypeImpl& value)
				{
					vint classCount = 0;
					reader << classCount;
					for (vint i = 0; i < classCount; i++)
					{
						WString typeName;
						reader << typeName;
						value.classes.Add(MakePtr<WfClass>(typeName));
					}
				}
					
				static void IO(Writer& writer, WfTypeImpl& value)
				{
					vint classCount = value.classes.Count();
					writer << classCount;
					for (vint i = 0; i < classCount; i++)
					{
						WString typeName = value.classes[i]->GetTypeName();
						writer << typeName;
					}
				}
			};

/***********************************************************************
Serialization (Instruction)
***********************************************************************/

			template<>
			struct Serialization<WfInstruction>
			{
				template<typename TIO>
				static void IO(TIO& io, WfInstruction& value)
				{
					io << value.code;
#define STREAMIO(NAME)						case WfInsCode::NAME: break;
#define STREAMIO_VALUE(NAME)				case WfInsCode::NAME: io << value.valueParameter; break;
#define STREAMIO_FUNCTION(NAME)				case WfInsCode::NAME: io << value.indexParameter; break;
#define STREAMIO_FUNCTION_COUNT(NAME)		case WfInsCode::NAME: io << value.indexParameter << value.countParameter; break;
#define STREAMIO_VARIABLE(NAME)				case WfInsCode::NAME: io << value.indexParameter; break;
#define STREAMIO_COUNT(NAME)				case WfInsCode::NAME: io << value.countParameter; break;
#define STREAMIO_FLAG_TYPEDESCRIPTOR(NAME)	case WfInsCode::NAME: io << value.flagParameter << value.typeDescriptorParameter; break;
#define STREAMIO_PROPERTY(NAME)				case WfInsCode::NAME: io << value.propertyParameter; break;
#define STREAMIO_METHOD_COUNT(NAME)			case WfInsCode::NAME: io << value.methodParameter << value.countParameter; break;
#define STREAMIO_EVENT(NAME)				case WfInsCode::NAME: io << value.eventParameter; break;
#define STREAMIO_LABEL(NAME)				case WfInsCode::NAME: io << value.indexParameter; break;
#define STREAMIO_TYPE(NAME)					case WfInsCode::NAME: io << value.typeParameter; break;

					switch (value.code)
					{
						INSTRUCTION_CASES(
							STREAMIO,
							STREAMIO_VALUE,
							STREAMIO_FUNCTION,
							STREAMIO_FUNCTION_COUNT,
							STREAMIO_VARIABLE,
							STREAMIO_COUNT,
							STREAMIO_FLAG_TYPEDESCRIPTOR,
							STREAMIO_PROPERTY,
							STREAMIO_METHOD_COUNT,
							STREAMIO_EVENT,
							STREAMIO_LABEL,
							STREAMIO_TYPE)
					}

#undef STREAMIO
#undef STREAMIO_VALUE
#undef STREAMIO_FUNCTION
#undef STREAMIO_FUNCTION_COUNT
#undef STREAMIO_VARIABLE
#undef STREAMIO_COUNT
#undef STREAMIO_FLAG_TYPEDESCRIPTOR
#undef STREAMIO_PROPERTY
#undef STREAMIO_METHOD_COUNT
#undef STREAMIO_EVENT
#undef STREAMIO_LABEL
#undef STREAMIO_TYPE
				}
			};
		}
	}

	namespace workflow
	{
		namespace runtime
		{

/***********************************************************************
WfInstructionDebugInfo
***********************************************************************/

			void WfInstructionDebugInfo::Initialize()
			{
				for (vint i = 0; i < instructionCodeMapping.Count(); i++)
				{
					const auto& range = instructionCodeMapping[i];
					if (range.codeIndex != -1)
					{
						codeInstructionMapping.Add(Tuple<vint, vint>(range.codeIndex, range.start.row), i);
					}
				}
			}

/***********************************************************************
WfAssembly
***********************************************************************/

			template<typename TIO>
			void WfAssembly::IO(TIO& io)
			{
				io
					<< insBeforeCodegen
					<< insAfterCodegen
					<< variableNames
					<< functionByName
					<< functions
					<< instructions
					<< typeImpl
					;
			}

			WfAssembly::WfAssembly()
			{
			}

			WfAssembly::WfAssembly(stream::IStream& input)
			{
				stream::internal::Reader reader(input);
				IO(reader);
				Initialize();
			}

			void WfAssembly::Initialize()
			{
				insBeforeCodegen->Initialize();
				insAfterCodegen->Initialize();
			}

			void WfAssembly::Serialize(stream::IStream& output)
			{
				stream::internal::Writer writer(output);
				IO(writer);
			}
		}
	}
}