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
			struct WfDeserializationException
			{
			};

			struct WfReaderContext
			{
				Dictionary<vint, ITypeDescriptor*>				tdIndex;
				Dictionary<vint, IMethodInfo*>					miIndex;
				Dictionary<vint, IPropertyInfo*>				piIndex;
				Dictionary<vint, IEventInfo*>					eiIndex;
				WfAssemblyLoadErrors&							errors;

				WfReaderContext(WfAssemblyLoadErrors& _errors)
					:errors(_errors)
				{
				}
			};

			struct WfWriterContextPrepare
			{
				SortedList<ITypeDescriptor*>					tds;
				SortedList<IMethodInfo*>						mis;
				SortedList<IPropertyInfo*>						pis;
				SortedList<IEventInfo*>							eis;
			};

			struct WfWriterContext
			{
				Dictionary<ITypeDescriptor*, vint>				tdIndex;
				Dictionary<IMethodInfo*, vint>					miIndex;
				Dictionary<IPropertyInfo*, vint>				piIndex;
				Dictionary<IEventInfo*, vint>					eiIndex;

				void Initialize(WfWriterContextPrepare& prepare)
				{
					for (auto [td, index] : indexed(prepare.tds))
					{
						tdIndex.Add(td, index);
					}
					for (auto [mi, index] : indexed(prepare.mis))
					{
						miIndex.Add(mi, index);
					}
					for (auto [pi, index] : indexed(prepare.pis))
					{
						piIndex.Add(pi, index);
					}
					for (auto [ei, index] : indexed(prepare.eis))
					{
						eiIndex.Add(ei, index);
					}
				}
			};

			using WfReader = Reader<Ptr<WfReaderContext>>;
			using WfWriter = Writer<Ptr<WfWriterContext>>;

/***********************************************************************
Serialization (CollectMetadata)
***********************************************************************/

			static void CollectTd(ITypeDescriptor* td, WfWriterContextPrepare& prepare)
			{
				if (!prepare.tds.Contains(td))
				{
					prepare.tds.Add(td);
				}
			}

			static void CollectTd(ITypeInfo* typeInfo, WfWriterContextPrepare& prepare)
			{
				switch (typeInfo->GetDecorator())
				{
				case ITypeInfo::RawPtr:
				case ITypeInfo::SharedPtr:
				case ITypeInfo::Nullable:
					CollectTd(typeInfo->GetElementType(), prepare);
					break;
				case ITypeInfo::Generic:
					{
						CollectTd(typeInfo->GetElementType(), prepare);
						vint count = typeInfo->GetGenericArgumentCount();
						for (vint i = 0; i < count; i++)
						{
							CollectTd(typeInfo->GetGenericArgument(i), prepare);
						}
					}
					break;
				case ITypeInfo::TypeDescriptor:
					CollectTd(typeInfo->GetTypeDescriptor(), prepare);
					break;
				}
			}

			static void CollectTd(IMethodInfo* info, WfWriterContextPrepare& prepare)
			{
				CollectTd(info->GetOwnerTypeDescriptor(), prepare);
				CollectTd(info->GetReturn(), prepare);
				vint count = info->GetParameterCount();
				for (vint i = 0; i < count; i++)
				{
					CollectTd(info->GetParameter(i)->GetType(), prepare);
				}
			}

			static void CollectTd(IEventInfo* info, WfWriterContextPrepare& prepare)
			{
				CollectTd(info->GetOwnerTypeDescriptor(), prepare);
				CollectTd(info->GetHandlerType(), prepare);
			}

			static void CollectTd(IPropertyInfo* info, WfWriterContextPrepare& prepare)
			{
				CollectTd(info->GetOwnerTypeDescriptor(), prepare);
				CollectTd(info->GetReturn(), prepare);
			}

			static void CollectTd(WfCustomType* td, WfWriterContextPrepare& prepare)
			{
				CollectTd((ITypeDescriptor*)td, prepare);

				vint baseCount = td->GetBaseTypeDescriptorCount();
				for (vint i = 0; i < baseCount; i++)
				{
					auto baseType = td->GetBaseTypeDescriptor(i);
					CollectTd(baseType, prepare);
				}

				if(auto group = td->GetConstructorGroup())
				{
					vint methodCount = group->GetMethodCount();
					for (vint j = 0; j < methodCount; j++)
					{
						auto method = group->GetMethod(j);
						CollectTd(method, prepare);
					}
				}

				vint methodGroupCount = td->GetMethodGroupCount();
				for (vint i = 0; i < methodGroupCount; i++)
				{
					auto group = td->GetMethodGroup(i);
					vint methodCount = group->GetMethodCount();
					for (vint j = 0; j < methodCount; j++)
					{
						auto method = group->GetMethod(j);
						CollectTd(method, prepare);
					}
				}

				vint propertyCount = td->GetPropertyCount();
				for (vint i = 0; i < propertyCount; i++)
				{
					CollectTd(td->GetProperty(i), prepare);
				}

				vint eventCount = td->GetEventCount();
				for (vint i = 0; i < eventCount; i++)
				{
					CollectTd(td->GetEvent(i), prepare);
				}
			}

			static void CollectTd(WfStruct* td, WfWriterContextPrepare& prepare)
			{
				CollectTd((ITypeDescriptor*)td, prepare);

				vint propertyCount = td->GetPropertyCount();
				for (vint i = 0; i < propertyCount; i++)
				{
					CollectTd(td->GetProperty(i), prepare);
				}
			}

			static void CollectTd(WfEnum* td, WfWriterContextPrepare& prepare)
			{
				CollectTd((ITypeDescriptor*)td, prepare);
			}

			static void CollectMetadata(WfTypeImpl* typeImpl, WfWriterContextPrepare& prepare)
			{
				for (auto td : typeImpl->classes)
				{
					CollectTd(td.Obj(), prepare);
				}
				for (auto td : typeImpl->interfaces)
				{
					CollectTd(td.Obj(), prepare);
				}
				for (auto td : typeImpl->structs)
				{
					CollectTd(td.Obj(), prepare);
				}
				for (auto td : typeImpl->enums)
				{
					CollectTd(td.Obj(), prepare);
				}
			}

			static void CollectMetadata(collections::List<WfInstruction>& instructions, WfWriterContextPrepare& prepare)
			{
#define TD(X)										do{ if (!prepare.tds.Contains(X)) prepare.tds.Add(X); }while(0)
#define MI(X)										do{ if (!prepare.mis.Contains(X)) prepare.mis.Add(X); }while(0)
#define PI(X)										do{ if (!prepare.pis.Contains(X)) prepare.pis.Add(X); }while(0)
#define EI(X)										do{ if (!prepare.eis.Contains(X)) prepare.eis.Add(X); }while(0)
#define COLLECTMETADATA(NAME)						case WfInsCode::NAME: break;
#define COLLECTMETADATA_VALUE(NAME)					case WfInsCode::NAME: if (auto td = ins.valueParameter.typeDescriptor) TD(td); break;
#define COLLECTMETADATA_FUNCTION(NAME)				case WfInsCode::NAME: break;
#define COLLECTMETADATA_FUNCTION_COUNT(NAME)		case WfInsCode::NAME: break;
#define COLLECTMETADATA_VARIABLE(NAME)				case WfInsCode::NAME: break;
#define COLLECTMETADATA_COUNT(NAME)					case WfInsCode::NAME: break;
#define COLLECTMETADATA_FLAG_TYPEDESCRIPTOR(NAME)	case WfInsCode::NAME: TD(ins.typeDescriptorParameter); break;
#define COLLECTMETADATA_PROPERTY(NAME)				case WfInsCode::NAME: PI(ins.propertyParameter); break;
#define COLLECTMETADATA_METHOD(NAME)				case WfInsCode::NAME: MI(ins.methodParameter); break;
#define COLLECTMETADATA_METHOD_COUNT(NAME)			case WfInsCode::NAME: MI(ins.methodParameter); break;
#define COLLECTMETADATA_EVENT(NAME)					case WfInsCode::NAME: EI(ins.eventParameter); break;
#define COLLECTMETADATA_EVENT_COUNT(NAME)			case WfInsCode::NAME: EI(ins.eventParameter); break;
#define COLLECTMETADATA_LABEL(NAME)					case WfInsCode::NAME: break;
#define COLLECTMETADATA_TYPE(NAME)					case WfInsCode::NAME: break;

				for (vint i = 0; i < instructions.Count(); i++)
				{
					auto& ins = instructions[i];
					switch (ins.code)
					{
						INSTRUCTION_CASES(
							COLLECTMETADATA,
							COLLECTMETADATA_VALUE,
							COLLECTMETADATA_FUNCTION,
							COLLECTMETADATA_FUNCTION_COUNT,
							COLLECTMETADATA_VARIABLE,
							COLLECTMETADATA_COUNT,
							COLLECTMETADATA_FLAG_TYPEDESCRIPTOR,
							COLLECTMETADATA_PROPERTY,
							COLLECTMETADATA_METHOD,
							COLLECTMETADATA_METHOD_COUNT,
							COLLECTMETADATA_EVENT,
							COLLECTMETADATA_EVENT_COUNT,
							COLLECTMETADATA_LABEL,
							COLLECTMETADATA_TYPE
							)
					}
				}

				for (auto mi : prepare.mis)
				{
					CollectTd(mi, prepare);
				}
				for (auto pi : prepare.pis)
				{
					CollectTd(pi, prepare);
				}
				for (auto ei : prepare.eis)
				{
					CollectTd(ei, prepare);
				}

#undef COLLECTMETADATA
#undef COLLECTMETADATA_VALUE
#undef COLLECTMETADATA_FUNCTION
#undef COLLECTMETADATA_FUNCTION_COUNT
#undef COLLECTMETADATA_VARIABLE
#undef COLLECTMETADATA_COUNT
#undef COLLECTMETADATA_FLAG_TYPEDESCRIPTOR
#undef COLLECTMETADATA_PROPERTY
#undef COLLECTMETADATA_METHOD
#undef COLLECTMETADATA_METHOD_COUNT
#undef COLLECTMETADATA_EVENT
#undef COLLECTMETADATA_EVENT_COUNT
#undef COLLECTMETADATA_LABEL
#undef COLLECTMETADATA_TYPE
#undef MI
#undef TD
#undef PI
#undef EI
			}

/***********************************************************************
Serizliation (Data Structures)
***********************************************************************/

			BEGIN_SERIALIZATION(glr::ParsingTextPos)
				SERIALIZE(index)
				SERIALIZE(row)
				SERIALIZE(column)
			END_SERIALIZATION

			BEGIN_SERIALIZATION(glr::ParsingTextRange)
				SERIALIZE(start)
				SERIALIZE(end)
				SERIALIZE(codeIndex)
			END_SERIALIZATION

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
Serizliation (ITypeDescriptor)
***********************************************************************/

			template<>
			struct Serialization<ITypeDescriptor*>
			{
				static void IO(WfReader& reader, ITypeDescriptor*& value)
				{
					WString id;
					reader << id;
					value = GetTypeDescriptor(id);
					if (!value)
					{
						reader.context->errors.unresolvedTypes.Add(id);
					}
				}
					
				static void IO(WfWriter& writer, ITypeDescriptor*& value)
				{
					WString id = value->GetTypeName();
					writer << id;
				}
			};

/***********************************************************************
Serizliation (ITypeInfo)
***********************************************************************/

			template<>
			struct Serialization<ITypeInfo>
			{
				static void IOType(WfReader& reader, Ptr<ITypeInfo>& typeInfo)
				{
					vint decorator = 0;
					reader << decorator;
					switch (static_cast<ITypeInfo::Decorator>(decorator))
					{
					case ITypeInfo::RawPtr:
						{
							Ptr<ITypeInfo> elementType;
							IOType(reader, elementType);
							typeInfo = Ptr(new RawPtrTypeInfo(elementType));
						}
						break;
					case ITypeInfo::SharedPtr:
						{
							Ptr<ITypeInfo> elementType;
							IOType(reader, elementType);
							typeInfo = Ptr(new SharedPtrTypeInfo(elementType));
						}
						break;
					case ITypeInfo::Nullable:
						{
							Ptr<ITypeInfo> elementType;
							IOType(reader, elementType);
							typeInfo = Ptr(new NullableTypeInfo(elementType));
						}
						break;
					case ITypeInfo::Generic:
						{
							Ptr<ITypeInfo> elementType;
							IOType(reader, elementType);
							auto genericType = Ptr(new GenericTypeInfo(elementType));
							typeInfo = genericType;

							vint count = 0;
							reader << count;
							for (vint i = 0; i < count; i++)
							{
								Ptr<ITypeInfo> argumentType;
								IOType(reader, argumentType);
								genericType->AddGenericArgument(argumentType);
							}
						}
						break;
					case ITypeInfo::TypeDescriptor:
						{
							vint hint = 0;
							reader << hint;

							vint index;
							reader << index;
							typeInfo = Ptr(new TypeDescriptorTypeInfo(reader.context->tdIndex[index], static_cast<TypeInfoHint>(hint)));
						}
						break;
					}
				}
					
				static void IOType(WfWriter& writer, ITypeInfo* typeInfo)
				{
					vint decorator = static_cast<vint>(typeInfo->GetDecorator());
					writer << decorator;

					switch (typeInfo->GetDecorator())
					{
					case ITypeInfo::RawPtr:
					case ITypeInfo::SharedPtr:
					case ITypeInfo::Nullable:
						IOType(writer, typeInfo->GetElementType());
						break;
					case ITypeInfo::Generic:
						{
							IOType(writer, typeInfo->GetElementType());
							vint count = typeInfo->GetGenericArgumentCount();
							writer << count;
							for (vint i = 0; i < count; i++)
							{
								IOType(writer, typeInfo->GetGenericArgument(i));
							}
						}
						break;
					case ITypeInfo::TypeDescriptor:
						{
							vint hint = static_cast<vint>(typeInfo->GetHint());
							writer << hint;

							vint index = writer.context->tdIndex[typeInfo->GetTypeDescriptor()];
							writer << index;
						}
						break;
					}
				}
			};

/***********************************************************************
Serizliation (Metadata)
***********************************************************************/

			template<>
			struct Serialization<IMethodInfo*>
			{
				static void IO(WfReader& reader, IMethodInfo*& value)
				{
					value = nullptr;
					vint typeIndex = -1;
					WString name;
					reader << typeIndex << name;
					auto type = reader.context->tdIndex[typeIndex];
					auto group = name == L"#ctor"
						? type->GetConstructorGroup()
						: type->GetMethodGroupByName(name, false);

					if (!group)
					{
						reader.context->errors.unresolvedMembers.Add(L"method: " + type->GetTypeName() + L"::" + name + L"(...): *");
						return;
					}

					vint methodFlag = -1;
					reader << methodFlag;
					if (0 > methodFlag || methodFlag > 3)
					{
						reader.context->errors.unresolvedMembers.Add(L"method: " + type->GetTypeName() + L"::" + name + L"(...): *");
						return;
					}

					vint methodCount = group->GetMethodCount();
					switch (methodFlag)
					{
					case 0:
						{
							if (methodCount > 1)
							{
								reader.context->errors.unresolvedMembers.Add(L"method: " + type->GetTypeName() + L"::" + name + L"(...): *; This is caused by a change to this class. When the current assembly was compiled, this imported method didn't have overloadings.");
								return;
							}
							value = group->GetMethod(0);
						}
						break;
					case 1:
						{
							vint count = -1;
							reader << count;

							WString parameters;
							for (vint i = 0; i < count; i++)
							{
								if (i == 0)
								{
									parameters = L"*";
								}
								else
								{
									parameters += L", *";
								}
							}

							for (vint i = 0; i < methodCount; i++)
							{
								auto method = group->GetMethod(i);
								if (method->GetParameterCount() == count)
								{
									if (value)
									{
										reader.context->errors.unresolvedMembers.Add(L"method: " + type->GetTypeName() + L"::" + name + L"(" + parameters + L"): *; This is caused by a change to this class. When the current assembly was compiled, this imported method didn't have overloadings with the same amount of parameters.");
										return;
									}
									value = method;
								}
							}

							if (!value)
							{
								reader.context->errors.unresolvedMembers.Add(L"method: " + type->GetTypeName() + L"::" + name + L"(" + parameters + L"): *; A qualified method doesn't exist.");
								return;
							}
						}
						break;
					case 2:
						{
							Ptr<ITypeInfo> returnType;
							Serialization<ITypeInfo>::IOType(reader, returnType);
							auto signature = returnType->GetTypeFriendlyName();
							for (vint i = 0; i < methodCount; i++)
							{
								auto method = group->GetMethod(i);
								if (method->GetReturn()->GetTypeFriendlyName() == signature)
								{
									if (value)
									{
										reader.context->errors.unresolvedMembers.Add(L"method: " + type->GetTypeName() + L"::" + name + L"(...): " + signature + L"; This is caused by a change to this class. When the current assembly was compiled, this imported method didn't have overloadings with the same return type.");
										return;
									}
									value = method;
								}
							}

							if (!value)
							{
								reader.context->errors.unresolvedMembers.Add(L"method: " + type->GetTypeName() + L"::" + name + L"(...): " + signature + L"; A qualified method doesn't exist.");
								return;
							}
						}
						break;
					case 3:
						{
							vint count = -1;
							reader << count;
							List<WString> signatures;
							for (vint i = 0; i < count; i++)
							{
								Ptr<ITypeInfo> type;
								Serialization<ITypeInfo>::IOType(reader, type);
								signatures.Add(type->GetTypeFriendlyName());
							}

							WString parameters;
							for (vint i = 0; i < count; i++)
							{
								if (i == 0)
								{
									parameters = signatures[0];
								}
								else
								{
									parameters += L", " + signatures[i];
								}
							}

							for (vint i = 0; i < methodCount; i++)
							{
								auto method = group->GetMethod(i);
								if (method->GetParameterCount() == count)
								{
									bool found = true;
									for (vint j = 0; j < count; j++)
									{
										if (method->GetParameter(j)->GetType()->GetTypeFriendlyName() != signatures[j])
										{
											found = false;
											break;
										}
									}

									if (found)
									{
										if (value)
										{
											reader.context->errors.unresolvedMembers.Add(L"method: " + type->GetTypeName() + L"::" + name + L"(" + parameters + L"): *; This is caused by a change to this class. When the current assembly was compiled, this imported method didn't have overloadings with the same parameter types.");
											return;
										}
										value = method;
									}
								}
							}

							if (!value)
							{
								reader.context->errors.unresolvedMembers.Add(L"method: " + type->GetTypeName() + L"::" + name + L"(" + parameters + L"): *; A qualified method doesn't exist.");
								return;
							}
						}
						break;
					}
				}
					
				static void IO(WfWriter& writer, IMethodInfo*& value)
				{
					auto type = value->GetOwnerTypeDescriptor();
					vint typeIndex = writer.context->tdIndex[type];
					auto group = value->GetOwnerMethodGroup();
					WString name = group == type->GetConstructorGroup()
						? L"#ctor"
						: value->GetName();
					writer << typeIndex << name;

					vint methodFlag = 0;
					if (group->GetMethodCount() == 1)
					{
						writer << methodFlag;
						return;
					}

					vint count = value->GetParameterCount();
					{
						vint satisfied = 0;
						for (vint i = 0; i < group->GetMethodCount(); i++)
						{
							if (group->GetMethod(i)->GetParameterCount() == count)
							{
								satisfied++;
							}
						}

						if (satisfied == 1)
						{
							methodFlag = 1;
							writer << methodFlag << count;
							return;
						}
					}

					auto returnType = value->GetReturn();
					{
						auto signature = returnType->GetTypeFriendlyName();
						vint satisfied = 0;
						for (vint i = 0; i < group->GetMethodCount(); i++)
						{
							if (group->GetMethod(i)->GetReturn()->GetTypeFriendlyName() == signature)
							{
								satisfied++;
							}
						}

						if (satisfied == 1)
						{
							methodFlag = 2;
							writer << methodFlag;
							Serialization<ITypeInfo>::IOType(writer, returnType);
							return;
						}
					}

					methodFlag = 3;
					writer << methodFlag << count;
					for (vint i = 0; i < count; i++)
					{
						Serialization<ITypeInfo>::IOType(writer, value->GetParameter(i)->GetType());
					}
				}
			};

			template<>
			struct Serialization<IPropertyInfo*>
			{
				static void IO(WfReader& reader, IPropertyInfo*& value)
				{
					vint typeIndex = -1;
					WString name;
					reader << typeIndex << name;
					auto type = reader.context->tdIndex[typeIndex];
					value = type->GetPropertyByName(name, false);
					if (!value)
					{
						reader.context->errors.unresolvedMembers.Add(L"property: " + type->GetTypeName() + L"::" + name);
					}
				}
					
				static void IO(WfWriter& writer, IPropertyInfo*& value)
				{
					auto type = value->GetOwnerTypeDescriptor();
					vint typeIndex = writer.context->tdIndex[type];
					WString name = value->GetName();
					writer << typeIndex << name;
				}
			};

			template<>
			struct Serialization<IEventInfo*>
			{
				static void IO(WfReader& reader, IEventInfo*& value)
				{
					vint typeIndex = -1;
					WString name;
					reader << typeIndex << name;
					auto type = reader.context->tdIndex[typeIndex];
					value = type->GetEventByName(name, false);
					if (!value)
					{
						reader.context->errors.unresolvedMembers.Add(L"event: " + type->GetTypeName() + L"::" + name);
					}
				}
					
				static void IO(WfWriter& writer, IEventInfo*& value)
				{
					auto type = value->GetOwnerTypeDescriptor();
					vint typeIndex = writer.context->tdIndex[type];
					WString name = value->GetName();
					writer << typeIndex << name;
				}
			};

			template<>
			struct Serialization<WfRuntimeValue>
			{
				static void IO(WfReader& reader, WfRuntimeValue& value)
				{
					vint typeIndex = -1;
					reader << typeIndex;
					value.typeDescriptor = typeIndex == -1 ? nullptr : reader.context->tdIndex[typeIndex];
					reader << value.type;
					value.stringValue = WString();

					switch (value.type)
					{
					case WfInsType::Bool:			reader << value.boolValue; break;
					case WfInsType::I1:				{ vint64_t intValue = 0; reader << intValue; value.i1Value = (vint8_t)intValue; break; }
					case WfInsType::I2:				{ vint64_t intValue = 0; reader << intValue; value.i2Value = (vint16_t)intValue; break; }
					case WfInsType::I4:				{ vint64_t intValue = 0; reader << intValue; value.i4Value = (vint32_t)intValue; break; }
					case WfInsType::I8:				{ vint64_t intValue = 0; reader << intValue; value.i8Value = (vint64_t)intValue; break; }
					case WfInsType::U1:				{ vuint64_t intValue = 0; reader << intValue; value.u1Value = (vuint8_t)intValue; break; }
					case WfInsType::U2:				{ vuint64_t intValue = 0; reader << intValue; value.u2Value = (vuint16_t)intValue; break; }
					case WfInsType::U4:				{ vuint64_t intValue = 0; reader << intValue; value.u4Value = (vuint32_t)intValue; break; }
					case WfInsType::U8:				{ vuint64_t intValue = 0; reader << intValue; value.u8Value = (vuint64_t)intValue; break; }
					case WfInsType::F4:				reader << value.f4Value; break;
					case WfInsType::F8:				reader << value.f8Value; break;
					case WfInsType::String:			reader << value.stringValue; break;
					default:;
					}
				}
					
				static void IO(WfWriter& writer, WfRuntimeValue& value)
				{
					vint typeIndex = -1;
					if (value.typeDescriptor) typeIndex = writer.context->tdIndex[value.typeDescriptor];
					writer << typeIndex;
					writer << value.type;

					switch (value.type)
					{
					case WfInsType::Bool:			writer << value.boolValue; break;
					case WfInsType::I1:				{ vint64_t intValue = value.i1Value; writer << intValue; break; }
					case WfInsType::I2:				{ vint64_t intValue = value.i2Value; writer << intValue; break; }
					case WfInsType::I4:				{ vint64_t intValue = value.i4Value; writer << intValue; break; }
					case WfInsType::I8:				{ vint64_t intValue = value.i8Value; writer << intValue; break; }
					case WfInsType::U1:				{ vuint64_t intValue = value.u1Value; writer << intValue; break; }
					case WfInsType::U2:				{ vuint64_t intValue = value.u2Value; writer << intValue; break; }
					case WfInsType::U4:				{ vuint64_t intValue = value.u4Value; writer << intValue; break; }
					case WfInsType::U8:				{ vuint64_t intValue = value.u8Value; writer << intValue; break; }
					case WfInsType::F4:				writer << value.f4Value; break;
					case WfInsType::F8:				writer << value.f8Value; break;
					case WfInsType::String:			writer << value.stringValue; break;
					default:;
					}
				}
			};

/***********************************************************************
Serialization (TypeImpl)
***********************************************************************/

			template<>
			struct Serialization<WfTypeImpl>
			{
				static void IOType(WfReader& reader, Ptr<ITypeInfo>& typeInfo)
				{
					Serialization<ITypeInfo>::IOType(reader, typeInfo);
				}
					
				static void IOType(WfWriter& writer, ITypeInfo* typeInfo)
				{
					Serialization<ITypeInfo>::IOType(writer, typeInfo);
				}

				//----------------------------------------------------

				static void IOMethodBase(WfReader& reader, WfMethodBase* info)
				{
					Ptr<ITypeInfo> type;
					IOType(reader, type);
					info->SetReturn(type);

					vint count = 0;
					reader << count;
					for (vint i = 0; i < count; i++)
					{
						WString name;
						IOType(reader, type);
						reader << name;
						info->AddParameter(Ptr(new ParameterInfoImpl(info, name, type)));
					}
				}
					
				static void IOMethodBase(WfWriter& writer, WfMethodBase* info)
				{
					IOType(writer, info->GetReturn());

					vint count = info->GetParameterCount();
					writer << count;
					for (vint i = 0; i < count; i++)
					{
						auto parameter = info->GetParameter(i);
						IOType(writer, parameter->GetType());

						WString name = parameter->GetName();
						writer << name;
					}
				}

				//----------------------------------------------------

				static void IOStaticMethod(WfReader& reader, WfStaticMethod* info)
				{
					reader << info->functionIndex;
					IOMethodBase(reader, info);
				}

				static void IOStaticMethod(WfWriter& writer, WfStaticMethod* info)
				{
					writer << info->functionIndex;
					IOMethodBase(writer, info);
				}

				//----------------------------------------------------

				static void IOClassMethod(WfReader& reader, WfClassMethod* info)
				{
					reader << info->functionIndex;
					IOMethodBase(reader, info);
				}

				static void IOClassMethod(WfWriter& writer, WfClassMethod* info)
				{
					writer << info->functionIndex;
					IOMethodBase(writer, info);
				}

				//----------------------------------------------------

				static void IOClassConstructor(WfReader& reader, Ptr<WfClassConstructor>& info)
				{
					info = Ptr(new WfClassConstructor(nullptr));
					reader << info->functionIndex;
					IOMethodBase(reader, info.Obj());
				}

				static void IOClassConstructor(WfWriter& writer, WfClassConstructor* info)
				{
					writer << info->functionIndex;
					IOMethodBase(writer, info);
				}

				//----------------------------------------------------

				static void IOInterfaceConstructor(WfReader& reader, Ptr<WfInterfaceConstructor>& info)
				{
					Ptr<ITypeInfo> type;
					IOType(reader, type);
					info = Ptr(new WfInterfaceConstructor(type));
				}

				static void IOInterfaceConstructor(WfWriter& writer, WfInterfaceConstructor* info)
				{
					IOType(writer, info->GetReturn());
				}

				//----------------------------------------------------

				static void IOInterfaceMethod(WfReader& reader, WfInterfaceMethod* info)
				{
					IOMethodBase(reader, info);
				}

				static void IOInterfaceMethod(WfWriter& writer, WfInterfaceMethod* info)
				{
					IOMethodBase(writer, info);
				}

				//----------------------------------------------------

				static void IOCustomType(WfReader& reader, WfCustomType* td, bool isClass)
				{
					// constructors
					{
						vint methodCount = 0;
						reader << methodCount;

						for (vint i = 0; i < methodCount; i++)
						{
							if (isClass)
							{
								Ptr<WfClassConstructor> ctor;
								IOClassConstructor(reader, ctor);
								td->AddMember(ctor);
							}
							else
							{
								Ptr<WfInterfaceConstructor> ctor;
								IOInterfaceConstructor(reader, ctor);
								td->AddMember(ctor);
							}
						}
					}

					// base types
					vint baseCount = 0;
					reader << baseCount;
					for (vint i = 0; i < baseCount; i++)
					{
						vint index = 0;
						reader << index;
						auto baseTd = reader.context->tdIndex[index];
						td->AddBaseType(baseTd);
					}

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
								auto info = Ptr(new WfStaticMethod);
								td->AddMember(methodName, info);
								IOStaticMethod(reader, info.Obj());
							}
							else if (isClass)
							{
								auto info = Ptr(new WfClassMethod);
								td->AddMember(methodName, info);
								IOClassMethod(reader, info.Obj());
							}
							else
							{
								auto info = Ptr(new WfInterfaceMethod);
								td->AddMember(methodName, info);
								IOInterfaceMethod(reader, info.Obj());
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
						IOType(reader, eventType);

						auto info = Ptr(new WfEvent(td, eventName));
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
							auto info = Ptr(new WfProperty(td, propName));

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
							Ptr<ITypeInfo> fieldType;
							IOType(reader, fieldType);

							auto info = Ptr(new WfField(td, propName));
							info->SetReturn(fieldType);
							td->AddMember(info);
						}
					}
				}
					
				static void IOCustomType(WfWriter& writer, WfCustomType* td, bool isClass)
				{
					// constructors
					{
						vint methodCount = 0;
						if (auto group = td->GetConstructorGroup())
						{
							vint methodCount = group->GetMethodCount();
							writer << methodCount;

							for (vint i = 0; i < methodCount; i++)
							{
								if (isClass)
								{
									auto ctor = dynamic_cast<WfClassConstructor*>(group->GetMethod(i));
									IOClassConstructor(writer, ctor);
								}
								else
								{
									auto ctor = dynamic_cast<WfInterfaceConstructor*>(group->GetMethod(i));
									IOInterfaceConstructor(writer, ctor);
								}
							}
						}
						else
						{
							writer << methodCount;
						}
					}

					// base types
					vint baseCount = td->GetBaseTypeDescriptorCount();
					writer << baseCount;
					for (vint i = 0; i < baseCount; i++)
					{
						auto baseTd = td->GetBaseTypeDescriptor(i);
						vint index = writer.context->tdIndex[baseTd];
						writer << index;
					}

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
								IOStaticMethod(writer, staticMethod);
							}
							else if (isClass)
							{
								writer << isStaticMethod;
								IOClassMethod(writer, dynamic_cast<WfClassMethod*>(method));
							}
							else
							{
								auto interfaceMethod = dynamic_cast<WfInterfaceMethod*>(method);
								writer << isStaticMethod;
								IOInterfaceMethod(writer, interfaceMethod);
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
						IOType(writer, info->GetHandlerType());
					}

					// properties
					vint propertyCount = td->GetPropertyCount();
					writer << propertyCount;
					for (vint i = 0; i < propertyCount; i++)
					{
						auto propInfo = td->GetProperty(i);
						bool isProperty = false;

						if (dynamic_cast<WfProperty*>(propInfo))
						{
							isProperty = true;
							WString propName = propInfo->GetName();
							writer << isProperty << propName;

							auto getterName = propInfo->GetGetter() ? propInfo->GetGetter()->GetName() : L"";
							auto setterName = propInfo->GetSetter() ? propInfo->GetSetter()->GetName() : L"";
							auto eventName = propInfo->GetValueChangedEvent() ? propInfo->GetValueChangedEvent()->GetName() : L"";
							writer << getterName << setterName << eventName;
						}
						else
						{
							isProperty = false;
							WString propName = propInfo->GetName();
							writer << isProperty << propName;
							IOType(writer, propInfo->GetReturn());
						}
					}
				}

				//----------------------------------------------------

				static void IOClass(WfReader& reader, WfClass* td)
				{
					reader << td->destructorFunctionIndex;
					IOCustomType(reader, td, true);
				}

				static void IOClass(WfWriter& writer, WfClass* td)
				{
					writer << td->destructorFunctionIndex;
					IOCustomType(writer, td, true);
				}

				//----------------------------------------------------

				static void IOInterface(WfReader& reader, WfInterface* td)
				{
					IOCustomType(reader, td, false);
				}

				static void IOInterface(WfWriter& writer, WfInterface* td)
				{
					IOCustomType(writer, td, false);
				}

				//----------------------------------------------------

				static void IOStruct(WfReader& reader, WfStruct* td)
				{
					vint count;
					reader << count;

					for (vint i = 0; i < count; i++)
					{
						WString name;
						reader << name;

						Ptr<ITypeInfo> typeInfo;
						IOType(reader, typeInfo);

						auto field = Ptr(new WfStructField(td, name));
						field->SetReturn(typeInfo);
						td->AddMember(field);
					}
				}

				static void IOStruct(WfWriter& writer, WfStruct* td)
				{
					vint count = td->GetPropertyCount();
					writer << count;

					for (vint i = 0; i < count; i++)
					{
						auto prop = td->GetProperty(i);

						WString name = prop->GetName();
						writer << name;

						ITypeInfo* typeInfo = prop->GetReturn();
						IOType(writer, typeInfo);
					}
				}

				//----------------------------------------------------

				static void IOEnum(WfReader& reader, WfEnum* td)
				{
					vint count;
					reader << count;

					auto et = td->GetEnumType();
					for (vint i = 0; i < count; i++)
					{
						WString name;
						vint64_t value;
						reader << name << value;
						td->AddEnumItem(name, (vuint64_t)value);
					}
				}

				static void IOEnum(WfWriter& writer, WfEnum* td)
				{
					auto et = td->GetEnumType();

					vint count = et->GetItemCount();
					writer << count;
					
					for (vint i = 0; i < count; i++)
					{
						WString name = et->GetItemName(i);
						vint64_t value = (vint64_t)et->GetItemValue(i);
						writer << name << value;
					}
				}

				//----------------------------------------------------

				static void IO(WfReader& reader, WfTypeImpl& value)
				{
					// fill types
					for (auto td : value.classes)
					{
						IOClass(reader, td.Obj());
					}
					for (auto td : value.interfaces)
					{
						IOInterface(reader, td.Obj());
					}
					for (auto td : value.structs)
					{
						IOStruct(reader, td.Obj());
					}
					for (auto td : value.enums)
					{
						IOEnum(reader, td.Obj());
					}
				}
					
				static void IO(WfWriter& writer, WfTypeImpl& value)
				{
					// fill types
					for (auto td : value.classes)
					{
						IOClass(writer, td.Obj());
					}
					for (auto td : value.interfaces)
					{
						IOInterface(writer, td.Obj());
					}
					for (auto td : value.structs)
					{
						IOStruct(writer, td.Obj());
					}
					for (auto td : value.enums)
					{
						IOEnum(writer, td.Obj());
					}
				}
			};

/***********************************************************************
Serialization (Instruction)
***********************************************************************/

			template<>
			struct Serialization<WfInstruction>
			{
				static void IO(WfReader& reader, WfInstruction& value)
				{
					reader << value.code;
#define IO(X)								do{ reader << (X); }while(0)
#define TD(X)								do{ vint index = -1; reader << index; X = reader.context->tdIndex[index]; }while(0)
#define MI(X)								do{ vint index = -1; reader << index; X = reader.context->miIndex[index]; }while(0)
#define PI(X)								do{ vint index = -1; reader << index; X = reader.context->piIndex[index]; }while(0)
#define EI(X)								do{ vint index = -1; reader << index; X = reader.context->eiIndex[index]; }while(0)
#define STREAMIO(NAME)						case WfInsCode::NAME: break;
#define STREAMIO_VALUE(NAME)				case WfInsCode::NAME: IO(value.valueParameter); break;
#define STREAMIO_FUNCTION(NAME)				case WfInsCode::NAME: IO(value.indexParameter); break;
#define STREAMIO_FUNCTION_COUNT(NAME)		case WfInsCode::NAME: IO(value.indexParameter); IO(value.countParameter); break;
#define STREAMIO_VARIABLE(NAME)				case WfInsCode::NAME: IO(value.indexParameter); break;
#define STREAMIO_COUNT(NAME)				case WfInsCode::NAME: IO(value.countParameter); break;
#define STREAMIO_FLAG_TYPEDESCRIPTOR(NAME)	case WfInsCode::NAME: IO(value.flagParameter); TD(value.typeDescriptorParameter); break;
#define STREAMIO_PROPERTY(NAME)				case WfInsCode::NAME: PI(value.propertyParameter); break;
#define STREAMIO_METHOD(NAME)				case WfInsCode::NAME: MI(value.methodParameter); break;
#define STREAMIO_METHOD_COUNT(NAME)			case WfInsCode::NAME: MI(value.methodParameter); IO(value.countParameter); break;
#define STREAMIO_EVENT(NAME)				case WfInsCode::NAME: EI(value.eventParameter); break;
#define STREAMIO_EVENT_COUNT(NAME)			case WfInsCode::NAME: EI(value.eventParameter); IO(value.countParameter); break;
#define STREAMIO_LABEL(NAME)				case WfInsCode::NAME: IO(value.indexParameter); break;
#define STREAMIO_TYPE(NAME)					case WfInsCode::NAME: IO(value.typeParameter); break;

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
							STREAMIO_METHOD,
							STREAMIO_METHOD_COUNT,
							STREAMIO_EVENT,
							STREAMIO_EVENT_COUNT,
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
#undef STREAMIO_METHOD
#undef STREAMIO_METHOD_COUNT
#undef STREAMIO_EVENT
#undef STREAMIO_EVENT_COUNT
#undef STREAMIO_LABEL
#undef STREAMIO_TYPE
#undef MI
#undef TD
#undef PI
#undef EI
#undef IO
				}

				static void IO(WfWriter& writer, WfInstruction& value)
				{
					writer << value.code;
#define IO(X)								do{ writer << (X); }while(0)
#define TD(X)								do{ vint index = writer.context->tdIndex[X]; writer << index; }while(0)
#define MI(X)								do{ vint index = writer.context->miIndex[X]; writer << index; }while(0)
#define PI(X)								do{ vint index = writer.context->piIndex[X]; writer << index; }while(0)
#define EI(X)								do{ vint index = writer.context->eiIndex[X]; writer << index; }while(0)
#define STREAMIO(NAME)						case WfInsCode::NAME: break;
#define STREAMIO_VALUE(NAME)				case WfInsCode::NAME: IO(value.valueParameter); break;
#define STREAMIO_FUNCTION(NAME)				case WfInsCode::NAME: IO(value.indexParameter); break;
#define STREAMIO_FUNCTION_COUNT(NAME)		case WfInsCode::NAME: IO(value.indexParameter); IO(value.countParameter); break;
#define STREAMIO_VARIABLE(NAME)				case WfInsCode::NAME: IO(value.indexParameter); break;
#define STREAMIO_COUNT(NAME)				case WfInsCode::NAME: IO(value.countParameter); break;
#define STREAMIO_FLAG_TYPEDESCRIPTOR(NAME)	case WfInsCode::NAME: IO(value.flagParameter); TD(value.typeDescriptorParameter); break;
#define STREAMIO_PROPERTY(NAME)				case WfInsCode::NAME: PI(value.propertyParameter); break;
#define STREAMIO_METHOD(NAME)				case WfInsCode::NAME: MI(value.methodParameter); break;
#define STREAMIO_METHOD_COUNT(NAME)			case WfInsCode::NAME: MI(value.methodParameter); IO(value.countParameter); break;
#define STREAMIO_EVENT(NAME)				case WfInsCode::NAME: EI(value.eventParameter); break;
#define STREAMIO_EVENT_COUNT(NAME)			case WfInsCode::NAME: EI(value.eventParameter); IO(value.countParameter); break;
#define STREAMIO_LABEL(NAME)				case WfInsCode::NAME: IO(value.indexParameter); break;
#define STREAMIO_TYPE(NAME)					case WfInsCode::NAME: IO(value.typeParameter); break;

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
							STREAMIO_METHOD,
							STREAMIO_METHOD_COUNT,
							STREAMIO_EVENT,
							STREAMIO_EVENT_COUNT,
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
#undef STREAMIO_METHOD
#undef STREAMIO_METHOD_COUNT
#undef STREAMIO_EVENT
#undef STREAMIO_EVENT_COUNT
#undef STREAMIO_LABEL
#undef STREAMIO_TYPE
#undef MI
#undef TD
#undef PI
#undef EI
#undef IO
				}
			};

/***********************************************************************
Serialization (Assembly)
***********************************************************************/

			template<>
			struct Serialization<WfAssembly>
			{
				//----------------------------------------------------

				static void IOCustomType(WfReader& reader, Ptr<WfEnum>& type)
				{
					bool isFlags;
					WString typeName;
					if (GetTypeDescriptor(typeName))
					{
						reader.context->errors.duplicatedTypes.Add(typeName);
					}
					reader << isFlags << typeName;
					type = Ptr(new WfEnum(isFlags, typeName));
				}

				static void IOCustomType(WfWriter& writer, Ptr<WfEnum>& type)
				{
					bool isFlags = type->GetTypeDescriptorFlags() == TypeDescriptorFlags::FlagEnum;
					WString typeName = type->GetTypeName();
					writer << isFlags << typeName;
				}

				template<typename TType>
				static void IOCustomType(WfReader& reader, Ptr<TType>& type)
				{
					WString typeName;
					reader << typeName;
					if (GetTypeDescriptor(typeName))
					{
						reader.context->errors.duplicatedTypes.Add(typeName);
					}
					type = Ptr(new TType(typeName));
				}

				template<typename TType>
				static void IOCustomType(WfWriter& writer, Ptr<TType>& type)
				{
					WString typeName = type->GetTypeName();
					writer << typeName;
				}

				//----------------------------------------------------

				template<typename TType>
				static void IOCustomTypeList(WfReader& reader, List<Ptr<TType>>& types)
				{
					vint typeCount = 0;
					reader << typeCount;
					for (vint i = 0; i < typeCount; i++)
					{
						Ptr<TType> type;
						IOCustomType(reader, type);
						types.Add(type);
					}

					for (vint i = 0; i < typeCount; i++)
					{
						vint index = -reader.context->tdIndex.Count() - 1;
						reader.context->tdIndex.Add(index, types[i].Obj());
					}
				}

				template<typename TType>
				static void IOCustomTypeList(WfWriter& writer, List<Ptr<TType>>& types)
				{
					vint typeCount = types.Count();
					writer << typeCount;
					for (vint i = 0; i < typeCount; i++)
					{
						auto type = types[i];
						IOCustomType(writer, type);
					}
					
					for (vint i = 0; i < typeCount; i++)
					{
						vint index = -writer.context->tdIndex.Count() - 1;
						writer.context->tdIndex.Add(types[i].Obj(), index);
					}
				}

				//----------------------------------------------------

				static void IOPrepare(WfReader& reader, WfAssembly& value, WfAssemblyLoadErrors& errors)
				{
					reader.context = Ptr(new WfReaderContext(errors));
					bool hasTypeImpl = false;
					reader << hasTypeImpl;
					if (hasTypeImpl)
					{
						value.typeImpl = Ptr(new WfTypeImpl);
						IOCustomTypeList(reader, value.typeImpl->classes);
						IOCustomTypeList(reader, value.typeImpl->interfaces);
						IOCustomTypeList(reader, value.typeImpl->structs);
						IOCustomTypeList(reader, value.typeImpl->enums);
					}

					vint tdCount = -1;
					vint miCount = -1;
					vint piCount = -1;
					vint eiCount = -1;
					reader << tdCount << miCount << piCount << eiCount;
					for (vint i = 0; i < tdCount; i++)
					{
						ITypeDescriptor* td = nullptr;
						reader << td;
						reader.context->tdIndex.Add(i, td);
					}

					if (errors.unresolvedTypes.Count() + errors.duplicatedTypes.Count() > 0)
					{
						throw WfDeserializationException();
					}

					if (hasTypeImpl)
					{
						Serialization<WfTypeImpl>::IO(reader, *value.typeImpl.Obj());
						GetGlobalTypeManager()->AddTypeLoader(value.typeImpl);
					}

					for (vint i = 0; i < miCount; i++)
					{
						IMethodInfo* mi = nullptr;
						reader << mi;
						reader.context->miIndex.Add(i, mi);
					}
					for (vint i = 0; i < piCount; i++)
					{
						IPropertyInfo* pi = nullptr;
						reader << pi;
						reader.context->piIndex.Add(i, pi);
					}
					for (vint i = 0; i < eiCount; i++)
					{
						IEventInfo* ei = nullptr;
						reader << ei;
						reader.context->eiIndex.Add(i, ei);
					}

					if (errors.unresolvedMembers.Count() > 0)
					{
						throw WfDeserializationException();
					}
				}

				static void IOPrepare(WfWriter& writer, WfAssembly& value, WfAssemblyLoadErrors&)
				{
					writer.context = Ptr(new WfWriterContext);
					bool hasTypeImpl = value.typeImpl != nullptr;
					writer << hasTypeImpl;
					if (hasTypeImpl)
					{
						IOCustomTypeList(writer, value.typeImpl->classes);
						IOCustomTypeList(writer, value.typeImpl->interfaces);
						IOCustomTypeList(writer, value.typeImpl->structs);
						IOCustomTypeList(writer, value.typeImpl->enums);
					}

					WfWriterContextPrepare prepare;
					if (hasTypeImpl)
					{
						CollectMetadata(value.typeImpl.Obj(), prepare);
					}
					CollectMetadata(value.instructions, prepare);
					for (vint i = prepare.tds.Count() - 1; i >= 0; i--)
					{
						if (writer.context->tdIndex.Keys().Contains(prepare.tds[i]))
						{
							prepare.tds.RemoveAt(i);
						}
					}
					writer.context->Initialize(prepare);

					vint tdCount = prepare.tds.Count();
					vint miCount = prepare.mis.Count();
					vint piCount = prepare.pis.Count();
					vint eiCount = prepare.eis.Count();
					writer << tdCount << miCount << piCount << eiCount;
					for (auto td : prepare.tds)
					{
						writer << td;
					}

					if (hasTypeImpl)
					{
						Serialization<WfTypeImpl>::IO(writer, *value.typeImpl.Obj());
						GetGlobalTypeManager()->AddTypeLoader(value.typeImpl);
					}

					for (auto mi : prepare.mis)
					{
						writer << mi;
					}
					for (auto pi : prepare.pis)
					{
						writer << pi;
					}
					for (auto ei : prepare.eis)
					{
						writer << ei;
					}
				}

				//----------------------------------------------------

				template<typename TIO>
				static void IO(TIO& io, WfAssembly& value, WfAssemblyLoadErrors& errors)
				{
					IOPrepare(io, value, errors);
					io	<< value.insBeforeCodegen
						<< value.insAfterCodegen
						<< value.variableNames
						<< value.functionByName
						<< value.functions
						<< value.instructions
						;
					if (value.typeImpl)
					{
						GetGlobalTypeManager()->RemoveTypeLoader(value.typeImpl);
					}
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

			WfAssembly::WfAssembly()
			{
			}

			void WfAssembly::Initialize()
			{
				insBeforeCodegen->Initialize();
				insAfterCodegen->Initialize();
			}

			Ptr<WfAssembly> WfAssembly::Deserialize(stream::IStream& input, WfAssemblyLoadErrors& errors)
			{
				try
				{
					auto assembly = Ptr(new WfAssembly);
					stream::internal::WfReader reader(input);
					stream::internal::Serialization<WfAssembly>::IO(reader, *assembly.Obj(), errors);
					assembly->Initialize();
					return assembly;
				}
				catch (stream::internal::WfDeserializationException)
				{
					return {};
				}
			}

			void WfAssembly::Serialize(stream::IStream& output)
			{
				WfAssemblyLoadErrors dummy;
				stream::internal::WfWriter writer(output);
				stream::internal::Serialization<WfAssembly>::IO(writer, *this, dummy);
			}
		}
	}
}
