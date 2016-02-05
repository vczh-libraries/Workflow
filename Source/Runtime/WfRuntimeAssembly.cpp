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
Serizliation (Metadata)
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
Serialization (CollectTypeDescriptors)
***********************************************************************/

			static void CollectTypeDescriptors(ITypeDescriptor* td, SortedList<ITypeDescriptor*>& tds)
			{
				if (!tds.Contains(td))
				{
					tds.Add(td);
				}
			}

			static void CollectTypeDescriptors(ITypeInfo* typeInfo, SortedList<ITypeDescriptor*>& tds)
			{
				switch (typeInfo->GetDecorator())
				{
				case ITypeInfo::RawPtr:
				case ITypeInfo::SharedPtr:
				case ITypeInfo::Nullable:
					CollectTypeDescriptors(typeInfo->GetElementType(), tds);
					break;
				case ITypeInfo::Generic:
					{
						CollectTypeDescriptors(typeInfo->GetElementType(), tds);
						vint count = typeInfo->GetGenericArgumentCount();
						for (vint i = 0; i < count; i++)
						{
							CollectTypeDescriptors(typeInfo->GetGenericArgument(i), tds);
						}
					}
					break;
				case ITypeInfo::TypeDescriptor:
					CollectTypeDescriptors(typeInfo->GetTypeDescriptor(), tds);
					break;
				}
			}

			static void CollectTypeDescriptors(IMethodInfo* info, SortedList<ITypeDescriptor*>& tds)
			{
				CollectTypeDescriptors(info->GetReturn(), tds);
				vint count = info->GetParameterCount();
				for (vint i = 0; i < count; i++)
				{
					CollectTypeDescriptors(info->GetParameter(i)->GetType(), tds);
				}
			}

			static void CollectTypeDescriptors(IEventInfo* info, SortedList<ITypeDescriptor*>& tds)
			{
				CollectTypeDescriptors(info->GetHandlerType(), tds);
			}

			static void CollectTypeDescriptors(IPropertyInfo* info, SortedList<ITypeDescriptor*>& tds)
			{
				CollectTypeDescriptors(info->GetReturn(), tds);
			}

			static void CollectTypeDescriptors(WfCustomType* td, SortedList<ITypeDescriptor*>& tds)
			{
				vint baseCount = td->GetBaseTypeDescriptorCount();
				for (vint i = 0; i < baseCount; i++)
				{
					auto baseType = td->GetBaseTypeDescriptor(i);
					CollectTypeDescriptors(baseType, tds);
				}

				vint methodGroupCount = td->GetMethodGroupCount();
				for (vint i = 0; i < methodGroupCount; i++)
				{
					auto group = td->GetMethodGroup(i);
					vint methodCount = group->GetMethodCount();
					for (vint j = 0; j < methodCount; j++)
					{
						auto method = group->GetMethod(j);
						CollectTypeDescriptors(method, tds);
					}
				}

				vint propertyCount = td->GetPropertyCount();
				for (vint i = 0; i < propertyCount; i++)
				{
					CollectTypeDescriptors(td->GetProperty(i), tds);
				}

				vint eventCount = td->GetEventCount();
				for (vint i = 0; i < eventCount; i++)
				{
					CollectTypeDescriptors(td->GetEvent(i), tds);
				}
			}

			static void CollectTypeDescriptors(WfTypeImpl* typeImpl, SortedList<ITypeDescriptor*>& tds)
			{
				FOREACH(Ptr<WfClass>, td, typeImpl->classes)
				{
					CollectTypeDescriptors(td.Obj(), tds);
				}
				FOREACH(Ptr<WfInterface>, td, typeImpl->interfaces)
				{
					CollectTypeDescriptors(td.Obj(), tds);
				}
			}

/***********************************************************************
Serialization (TypeImpl)
***********************************************************************/

			template<>
			struct Serialization<WfTypeImpl>
			{
				static void IOType(Reader& reader, Ptr<ITypeInfo>& typeInfo, Dictionary<vint, ITypeDescriptor*>& tdIndex)
				{
					vint decorator = 0;
					reader << decorator;
					auto typeInfoImpl = MakePtr<TypeInfoImpl>(static_cast<ITypeInfo::Decorator>(decorator));
					typeInfo = typeInfoImpl;
					
					switch (typeInfoImpl->GetDecorator())
					{
					case ITypeInfo::RawPtr:
					case ITypeInfo::SharedPtr:
					case ITypeInfo::Nullable:
						{
							Ptr<ITypeInfo> elementType;
							IOType(reader, elementType, tdIndex);
							typeInfoImpl->SetElementType(elementType);
						}
						break;
					case ITypeInfo::Generic:
						{
							Ptr<ITypeInfo> elementType;
							IOType(reader, elementType, tdIndex);
							typeInfoImpl->SetElementType(elementType);

							vint count = 0;
							reader << count;
							for (vint i = 0; i < count; i++)
							{
								Ptr<ITypeInfo> argumentType;
								IOType(reader, argumentType, tdIndex);
								typeInfoImpl->AddGenericArgument(argumentType);
							}
						}
						break;
					case ITypeInfo::TypeDescriptor:
						{
							vint index;
							reader << index;
							typeInfoImpl->SetTypeDescriptor(tdIndex[index]);
						}
						break;
					}
				}
					
				static void IOType(Writer& writer, ITypeInfo* typeInfo, Dictionary<ITypeDescriptor*, vint>& tdIndex)
				{
					vint decorator = static_cast<vint>(typeInfo->GetDecorator());
					writer << decorator;

					switch (typeInfo->GetDecorator())
					{
					case ITypeInfo::RawPtr:
					case ITypeInfo::SharedPtr:
					case ITypeInfo::Nullable:
						IOType(writer, typeInfo->GetElementType(), tdIndex);
						break;
					case ITypeInfo::Generic:
						{
							IOType(writer, typeInfo->GetElementType(), tdIndex);
							vint count = typeInfo->GetGenericArgumentCount();
							writer << count;
							for (vint i = 0; i < count; i++)
							{
								IOType(writer, typeInfo->GetGenericArgument(i), tdIndex);
							}
						}
						break;
					case ITypeInfo::TypeDescriptor:
						{
							vint index = tdIndex[typeInfo->GetTypeDescriptor()];
							writer << index;
						}
						break;
					}
				}

				//----------------------------------------------------

				static void IOMethodBase(Reader& reader, WfMethodBase* info, Dictionary<vint, ITypeDescriptor*>& tdIndex)
				{
					Ptr<ITypeInfo> type;
					IOType(reader, type, tdIndex);
					info->SetReturn(type);

					vint count = 0;
					reader << count;
					for (vint i = 0; i < count; i++)
					{
						WString name;
						IOType(reader, type, tdIndex);
						reader << name;
						info->AddParameter(new ParameterInfoImpl(info, name, type));
					}
				}
					
				static void IOMethodBase(Writer& writer, WfMethodBase* info, Dictionary<ITypeDescriptor*, vint>& tdIndex)
				{
					IOType(writer, info->GetReturn(), tdIndex);

					vint count = info->GetParameterCount();
					writer << count;
					for (vint i = 0; i < count; i++)
					{
						auto parameter = info->GetParameter(i);
						IOType(writer, parameter->GetType(), tdIndex);

						WString name = parameter->GetName();
						writer << name;
					}
				}

				//----------------------------------------------------

				static void IOStaticMethod(Reader& reader, WfStaticMethod* info, Dictionary<vint, ITypeDescriptor*>& tdIndex)
				{
					reader << info->functionIndex;
					IOMethodBase(reader, info, tdIndex);
				}

				static void IOStaticMethod(Writer& writer, WfStaticMethod* info, Dictionary<ITypeDescriptor*, vint>& tdIndex)
				{
					writer << info->functionIndex;
					IOMethodBase(writer, info, tdIndex);
				}

				//----------------------------------------------------

				static void IOInterfaceMethod(Reader& reader, WfInterfaceMethod* info, Dictionary<vint, ITypeDescriptor*>& tdIndex)
				{
					IOMethodBase(reader, info, tdIndex);
				}

				static void IOInterfaceMethod(Writer& writer, WfInterfaceMethod* info, Dictionary<ITypeDescriptor*, vint>& tdIndex)
				{
					IOMethodBase(writer, info, tdIndex);
				}

				//----------------------------------------------------

				static void IOCustomType(Reader& reader, WfCustomType* td, Dictionary<vint, ITypeDescriptor*>& tdIndex, bool isClass)
				{
					// methods
					vint methodGroupCount = 0;
					reader << methodGroupCount;
					for (vint i = 0; i < methodGroupCount; i++)
					{
						vint methodCount = 0;
						WString methodName;
						reader << methodCount << methodName;
						for (vint j = 0; j < methodCount; j++)
						{
							bool isStaticMethod = false;
							reader << isStaticMethod;

							if (isStaticMethod)
							{
								auto info = MakePtr<WfStaticMethod>();
								td->AddMember(methodName, info);
								IOStaticMethod(reader, info.Obj(), tdIndex);
							}
							else if (isClass)
							{
								throw 0;
							}
							else
							{
								auto info = MakePtr<WfInterfaceMethod>();
								td->AddMember(methodName, info);
								IOInterfaceMethod(reader, info.Obj(), tdIndex);
							}
						}
					}

					// events
					vint eventCount = 0;
					reader << eventCount;
					for (vint i = 0; i < eventCount; i++)
					{
						WString eventName;
						reader << eventName;

						Ptr<ITypeInfo> eventType;
						IOType(reader, eventType, tdIndex);

						auto info = MakePtr<WfEvent>(td, eventName);
						info->SetHandlerType(eventType);
						td->AddMember(info);
					}

					// properties
					vint propertyCount = 0;
					reader << propertyCount;
					for (vint i = 0; i < propertyCount; i++)
					{
						bool isProperty = false;
						WString propName;
						reader << isProperty << propName;

						if (isProperty)
						{
							auto info = MakePtr<WfProperty>(td, propName);

							WString getterName, setterName, eventName;
							reader << getterName << setterName << eventName;

							if (getterName != L"")
							{
								info->SetGetter(dynamic_cast<MethodInfoImpl*>(td->GetMethodGroupByName(getterName, false)->GetMethod(0)));
							}
							if (setterName != L"")
							{
								info->SetSetter(dynamic_cast<MethodInfoImpl*>(td->GetMethodGroupByName(setterName, false)->GetMethod(0)));
							}
							if (eventName != L"")
							{
								info->SetValueChangedEvent(dynamic_cast<EventInfoImpl*>(td->GetEventByName(eventName, false)));
							}

							td->AddMember(info);
						}
						else
						{
							throw 0;
						}
					}
				}
					
				static void IOCustomType(Writer& writer, WfCustomType* td, Dictionary<ITypeDescriptor*, vint>& tdIndex, bool isClass)
				{
					// methods
					vint methodGroupCount = td->GetMethodGroupCount();
					writer << methodGroupCount;
					for (vint i = 0; i < methodGroupCount; i++)
					{
						auto group = td->GetMethodGroup(i);
						vint methodCount = group->GetMethodCount();
						WString methodName = group->GetName();
						writer << methodCount << methodName;
						for (vint j = 0; j < methodCount; j++)
						{
							auto method = group->GetMethod(j);
							bool isStaticMethod = false;

							if (auto staticMethod = dynamic_cast<WfStaticMethod*>(method))
							{
								isStaticMethod = true;
								writer << isStaticMethod;
								IOStaticMethod(writer, staticMethod, tdIndex);
							}
							else if (isClass)
							{
								throw 0;
							}
							else
							{
								auto interfaceMethod = dynamic_cast<WfInterfaceMethod*>(method);
								writer << isStaticMethod;
								IOInterfaceMethod(writer, interfaceMethod, tdIndex);
							}
						}
					}

					// events
					vint eventCount = td->GetEventCount();
					writer << eventCount;
					for (vint i = 0; i < eventCount; i++)
					{
						auto info = td->GetEvent(i);
						WString eventName = info->GetName();
						writer << eventName;
						IOType(writer, info->GetHandlerType(), tdIndex);
					}

					// properties
					vint propertyCount = td->GetPropertyCount();
					writer << propertyCount;
					for (vint i = 0; i < propertyCount; i++)
					{
						auto propInfo = td->GetProperty(i);
						bool isProperty = false;

						if (auto prop = dynamic_cast<WfProperty*>(propInfo))
						{
							isProperty = true;
							WString propName = prop->GetName();
							writer << isProperty << propName;

							auto getterName = prop->GetGetter() ? prop->GetGetter()->GetName() : L"";
							auto setterName = prop->GetSetter() ? prop->GetSetter()->GetName() : L"";
							auto eventName = prop->GetValueChangedEvent() ? prop->GetValueChangedEvent()->GetName() : L"";
							writer << getterName << setterName << eventName;
						}
						else
						{
							throw 0;
						}
					}
				}

				//----------------------------------------------------

				static void IOClass(Reader& reader, WfClass* td, Dictionary<vint, ITypeDescriptor*>& tdIndex)
				{
					IOCustomType(reader, td, tdIndex, true);
				}

				static void IOClass(Writer& writer, WfClass* td, Dictionary<ITypeDescriptor*, vint>& tdIndex)
				{
					IOCustomType(writer, td, tdIndex, true);
				}

				//----------------------------------------------------

				static void IOInterface(Reader& reader, WfInterface* td, Dictionary<vint, ITypeDescriptor*>& tdIndex)
				{
					IOCustomType(reader, td, tdIndex, false);
				}

				static void IOInterface(Writer& writer, WfInterface* td, Dictionary<ITypeDescriptor*, vint>& tdIndex)
				{
					IOCustomType(writer, td, tdIndex, false);
				}

				//----------------------------------------------------

				template<typename TType>
				static void IOCustomTypeList(Reader& reader, List<Ptr<TType>>& types, Dictionary<vint, ITypeDescriptor*>& tdIndex)
				{
					vint typeCount = 0;
					reader << typeCount;
					for (vint i = 0; i < typeCount; i++)
					{
						WString typeName;
						reader << typeName;
						types.Add(MakePtr<TType>(typeName));
					}

					for (vint i = 0; i < typeCount; i++)
					{
						tdIndex.Add(tdIndex.Count(), types[i].Obj());
					}
				}

				template<typename TType>
				static void IOCustomTypeList(Writer& writer, List<Ptr<TType>>& types, Dictionary<ITypeDescriptor*, vint>& tdIndex)
				{
					vint typeCount = types.Count();
					writer << typeCount;
					for (vint i = 0; i < typeCount; i++)
					{
						WString typeName = types[i]->GetTypeName();
						writer << typeName;
					}
					
					for (vint i = 0; i < typeCount; i++)
					{
						tdIndex.Add(types[i].Obj(), tdIndex.Count());
					}
				}

				//----------------------------------------------------

				static void IO(Reader& reader, WfTypeImpl& value)
				{
					bool hasTypeImpl = false;
					reader << hasTypeImpl;
					if (hasTypeImpl)
					{
						// types
						Dictionary<vint, ITypeDescriptor*> tdIndex;
						IOCustomTypeList(reader, value.classes, tdIndex);
						IOCustomTypeList(reader, value.interfaces, tdIndex);
					
						// used type descriptors
						vint tdCount = 0;
						reader << tdCount;
						for (vint i = 0; i < tdCount; i++)
						{
							WString typeName;
							reader << typeName;
							tdIndex.Add((i * -1) - 1, GetTypeDescriptor(typeName));
						}

						// fill types
						FOREACH(Ptr<WfClass>, td, value.classes)
						{
							IOClass(reader, td.Obj(), tdIndex);
						}
						FOREACH(Ptr<WfInterface>, td, value.interfaces)
						{
							IOInterface(reader, td.Obj(), tdIndex);
						}
					}
				}
					
				static void IO(Writer& writer, WfTypeImpl& value)
				{
					auto pvalue = &value; // clang++: -Wtautological-undefined-compare
					bool hasTypeImpl = pvalue != nullptr;
					writer << hasTypeImpl;
					if (hasTypeImpl)
					{
						// types
						Dictionary<ITypeDescriptor*, vint> tdIndex;
						IOCustomTypeList(writer, value.classes, tdIndex);
						IOCustomTypeList(writer, value.interfaces, tdIndex);

						// used type descriptors
						SortedList<ITypeDescriptor*> tds;
						CollectTypeDescriptors(&value, tds);
						for (vint i = tds.Count() - 1; i >= 0; i--)
						{
							if (tdIndex.Keys().Contains(tds[i]))
							{
								tds.RemoveAt(i);
							}
						}

						vint tdCount = tds.Count();
						writer << tdCount;
						for (vint i = 0; i < tdCount; i++)
						{
							tdIndex.Add(tds[i], (i * -1) - 1);
							WString typeName = tds[i]->GetTypeName();
							writer << typeName;
						}

						// fill types
						FOREACH(Ptr<WfClass>, td, value.classes)
						{
							IOClass(writer, td.Obj(), tdIndex);
						}
						FOREACH(Ptr<WfInterface>, td, value.interfaces)
						{
							IOInterface(writer, td.Obj(), tdIndex);
						}
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
#define STREAMIO_FUNCTION_COUNT(NAME)		case WfInsCode::NAME: io << value.indexParameter << value.countParameter; break;
#define STREAMIO_VARIABLE(NAME)				case WfInsCode::NAME: io << value.indexParameter; break;
#define STREAMIO_COUNT(NAME)				case WfInsCode::NAME: io << value.countParameter; break;
#define STREAMIO_FLAG_TYPEDESCRIPTOR(NAME)	case WfInsCode::NAME: io << value.flagParameter << value.typeDescriptorParameter; break;
#define STREAMIO_PROPERTY(NAME)				case WfInsCode::NAME: io << value.propertyParameter; break;
#define STREAMIO_METHOD(NAME)				case WfInsCode::NAME: io << value.methodParameter; break;
#define STREAMIO_METHOD_COUNT(NAME)			case WfInsCode::NAME: io << value.methodParameter << value.countParameter; break;
#define STREAMIO_EVENT(NAME)				case WfInsCode::NAME: io << value.eventParameter; break;
#define STREAMIO_LABEL(NAME)				case WfInsCode::NAME: io << value.indexParameter; break;
#define STREAMIO_TYPE(NAME)					case WfInsCode::NAME: io << value.typeParameter; break;

					switch (value.code)
					{
						INSTRUCTION_CASES(
							STREAMIO,
							STREAMIO_VALUE,
							STREAMIO_FUNCTION_COUNT,
							STREAMIO_VARIABLE,
							STREAMIO_COUNT,
							STREAMIO_FLAG_TYPEDESCRIPTOR,
							STREAMIO_PROPERTY,
							STREAMIO_METHOD,
							STREAMIO_METHOD_COUNT,
							STREAMIO_EVENT,
							STREAMIO_LABEL,
							STREAMIO_TYPE)
					}

#undef STREAMIO
#undef STREAMIO_VALUE
#undef STREAMIO_FUNCTION_COUNT
#undef STREAMIO_VARIABLE
#undef STREAMIO_COUNT
#undef STREAMIO_FLAG_TYPEDESCRIPTOR
#undef STREAMIO_PROPERTY
#undef STREAMIO_METHOD
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
				io << typeImpl;
				if (typeImpl)
				{
					GetGlobalTypeManager()->AddTypeLoader(typeImpl);
				}
				io	<< insBeforeCodegen
					<< insAfterCodegen
					<< variableNames
					<< functionByName
					<< functions
					<< instructions
					;
				if (typeImpl)
				{
					GetGlobalTypeManager()->RemoveTypeLoader(typeImpl);
				}
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
