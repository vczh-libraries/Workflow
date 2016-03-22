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
			struct WfReaderContext
			{
				Dictionary<vint, ITypeDescriptor*>				tdIndex;
				Dictionary<vint, IMethodInfo*>					miIndex;
				Dictionary<vint, IPropertyInfo*>				piIndex;
				Dictionary<vint, IEventInfo*>					eiIndex;
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
					FOREACH_INDEXER(ITypeDescriptor*, td, index, prepare.tds)
					{
						tdIndex.Add(td, index);
					}
					FOREACH_INDEXER(IMethodInfo*, mi, index, prepare.mis)
					{
						miIndex.Add(mi, index);
					}
					FOREACH_INDEXER(IPropertyInfo*, pi, index, prepare.pis)
					{
						piIndex.Add(pi, index);
					}
					FOREACH_INDEXER(IEventInfo*, ei, index, prepare.eis)
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

			static void CollectMetadata(WfTypeImpl* typeImpl, WfWriterContextPrepare& prepare)
			{
				FOREACH(Ptr<WfClass>, td, typeImpl->classes)
				{
					CollectTd(td.Obj(), prepare);
				}
				FOREACH(Ptr<WfInterface>, td, typeImpl->interfaces)
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
#define COLLECTMETADATA_VALUE(NAME)					case WfInsCode::NAME: if (auto td = ins.valueParameter.GetTypeDescriptor()) TD(td); break;
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
							COLLECTMETADATA_TYPE)
					}
				}

				FOREACH(IMethodInfo*, mi, prepare.mis)
				{
					CollectTd(mi, prepare);
				}
				FOREACH(IPropertyInfo*, pi, prepare.pis)
				{
					CollectTd(pi, prepare);
				}
				FOREACH(IEventInfo*, ei, prepare.eis)
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
					CHECK_ERROR(value, L"Failed to load type.");
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
					auto typeInfoImpl = MakePtr<TypeInfoImpl>(static_cast<ITypeInfo::Decorator>(decorator));
					typeInfo = typeInfoImpl;
					
					switch (typeInfoImpl->GetDecorator())
					{
					case ITypeInfo::RawPtr:
					case ITypeInfo::SharedPtr:
					case ITypeInfo::Nullable:
						{
							Ptr<ITypeInfo> elementType;
							IOType(reader, elementType);
							typeInfoImpl->SetElementType(elementType);
						}
						break;
					case ITypeInfo::Generic:
						{
							Ptr<ITypeInfo> elementType;
							IOType(reader, elementType);
							typeInfoImpl->SetElementType(elementType);

							vint count = 0;
							reader << count;
							for (vint i = 0; i < count; i++)
							{
								Ptr<ITypeInfo> argumentType;
								IOType(reader, argumentType);
								typeInfoImpl->AddGenericArgument(argumentType);
							}
						}
						break;
					case ITypeInfo::TypeDescriptor:
						{
							vint index;
							reader << index;
							typeInfoImpl->SetTypeDescriptor(reader.context->tdIndex[index]);
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
						CHECK_ERROR(value, L"Failed to load method.");
					}

					vint methodFlag = -1;
					reader << methodFlag;
					CHECK_ERROR(0 <= methodFlag && methodFlag <= 3, L"Failed to load method.");

					vint methodCount = group->GetMethodCount();
					switch (methodFlag)
					{
					case 0:
						{
							CHECK_ERROR(methodCount == 1, L"Failed to load method.");
							value = group->GetMethod(0);
						}
						break;
					case 1:
						{
							vint count = -1;
							reader << count;
							for (vint i = 0; i < methodCount; i++)
							{
								auto method = group->GetMethod(i);
								if (method->GetParameterCount() == count)
								{
									CHECK_ERROR(!value, L"Failed to load method.");
									value = method;
								}
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
									CHECK_ERROR(!value, L"Failed to load method.");
									value = method;
								}
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
										CHECK_ERROR(!value, L"Failed to load method.");
										value = method;
									}
								}
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
					CHECK_ERROR(value, L"Failed to load property.");
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
					CHECK_ERROR(value, L"Failed to load event.");
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
			struct Serialization<Value>
			{
				static void IO(WfReader& reader, Value& value)
				{
					vint typeFlag = -1;
					reader << typeFlag;
					CHECK_ERROR(0 <= typeFlag && typeFlag <= 2, L"Failed to load value.");
					if (typeFlag == 0)
					{
						value = Value();
						return;
					}

					vint typeIndex = -1;
					reader << typeIndex;
					auto type = reader.context->tdIndex[typeIndex];

					if (typeFlag == 1)
					{
						value = Value::From(type);
						return;
					}

					WString text;
					reader << text;
					type->GetValueSerializer()->Parse(text, value);
				}
					
				static void IO(WfWriter& writer, Value& value)
				{
					vint typeFlag = 0;
					if (value.IsNull())
					{
						writer << typeFlag;
					}
					else
					{
						auto type = value.GetTypeDescriptor();
						if (type == GetTypeDescriptor<ITypeDescriptor>())
						{
							typeFlag = 1;
							type = UnboxValue<ITypeDescriptor*>(value);
						}
						else
						{
							typeFlag = 2;
						}
						vint typeIndex = writer.context->tdIndex[type];
						writer << typeFlag << typeIndex;

						if (typeFlag == 2)
						{
							auto text = value.GetText();
							writer << text;
						}
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
						info->AddParameter(new ParameterInfoImpl(info, name, type));
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
					info = new WfClassConstructor(nullptr);
					IOMethodBase(reader, info.Obj());
				}

				static void IOClassConstructor(WfWriter& writer, WfClassConstructor* info)
				{
					IOMethodBase(writer, info);
				}

				//----------------------------------------------------

				static void IOInterfaceConstructor(WfReader& reader, Ptr<WfInterfaceConstructor>& info)
				{
					Ptr<ITypeInfo> type;
					IOType(reader, type);
					info = new WfInterfaceConstructor(type);
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
								auto info = MakePtr<WfStaticMethod>();
								td->AddMember(methodName, info);
								IOStaticMethod(reader, info.Obj());
							}
							else if (isClass)
							{
								auto info = MakePtr<WfClassMethod>();
								td->AddMember(methodName, info);
								IOClassMethod(reader, info.Obj());
							}
							else
							{
								auto info = MakePtr<WfInterfaceMethod>();
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
							Ptr<ITypeInfo> fieldType;
							IOType(reader, fieldType);

							auto info = MakePtr<WfField>(td, propName);
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
							isProperty = false;
							WString propName = prop->GetName();
							writer << isProperty << propName;
							IOType(writer, prop->GetReturn());
						}
					}
				}

				//----------------------------------------------------

				static void IOClass(WfReader& reader, WfClass* td)
				{
					IOCustomType(reader, td, true);
				}

				static void IOClass(WfWriter& writer, WfClass* td)
				{
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

				static void IO(WfReader& reader, WfTypeImpl& value)
				{
					// fill types
					FOREACH(Ptr<WfClass>, td, value.classes)
					{
						IOClass(reader, td.Obj());
					}
					FOREACH(Ptr<WfInterface>, td, value.interfaces)
					{
						IOInterface(reader, td.Obj());
					}
				}
					
				static void IO(WfWriter& writer, WfTypeImpl& value)
				{
					// fill types
					FOREACH(Ptr<WfClass>, td, value.classes)
					{
						IOClass(writer, td.Obj());
					}
					FOREACH(Ptr<WfInterface>, td, value.interfaces)
					{
						IOInterface(writer, td.Obj());
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

				template<typename TType>
				static void IOCustomTypeList(WfReader& reader, List<Ptr<TType>>& types)
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
						WString typeName = types[i]->GetTypeName();
						writer << typeName;
					}
					
					for (vint i = 0; i < typeCount; i++)
					{
						vint index = -writer.context->tdIndex.Count() - 1;
						writer.context->tdIndex.Add(types[i].Obj(), index);
					}
				}

				//----------------------------------------------------

				static void IOPrepare(WfReader& reader, WfAssembly& value)
				{
					reader.context = new WfReaderContext;
					bool hasTypeImpl = false;
					reader << hasTypeImpl;
					if (hasTypeImpl)
					{
						value.typeImpl = new WfTypeImpl;
						IOCustomTypeList(reader, value.typeImpl->classes);
						IOCustomTypeList(reader, value.typeImpl->interfaces);
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
				}

				static void IOPrepare(WfWriter& writer, WfAssembly& value)
				{
					writer.context = new WfWriterContext;
					bool hasTypeImpl = value.typeImpl != nullptr;
					writer << hasTypeImpl;
					if (hasTypeImpl)
					{
						IOCustomTypeList(writer, value.typeImpl->classes);
						IOCustomTypeList(writer, value.typeImpl->interfaces);
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
					FOREACH(ITypeDescriptor*, td, prepare.tds)
					{
						writer << td;
					}

					if (hasTypeImpl)
					{
						Serialization<WfTypeImpl>::IO(writer, *value.typeImpl.Obj());
						GetGlobalTypeManager()->AddTypeLoader(value.typeImpl);
					}

					FOREACH(IMethodInfo*, mi, prepare.mis)
					{
						writer << mi;
					}
					FOREACH(IPropertyInfo*, pi, prepare.pis)
					{
						writer << pi;
					}
					FOREACH(IEventInfo*, ei, prepare.eis)
					{
						writer << ei;
					}
				}

				//----------------------------------------------------

				template<typename TIO>
				static void IO(TIO& io, WfAssembly& value)
				{
					IOPrepare(io, value);
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

			WfAssembly::WfAssembly(stream::IStream& input)
			{
				stream::internal::WfReader reader(input);
				stream::internal::Serialization<WfAssembly>::IO(reader, *this);
				Initialize();
			}

			void WfAssembly::Initialize()
			{
				insBeforeCodegen->Initialize();
				insAfterCodegen->Initialize();
			}

			void WfAssembly::Serialize(stream::IStream& output)
			{
				stream::internal::WfWriter writer(output);
				stream::internal::Serialization<WfAssembly>::IO(writer, *this);
			}
		}
	}
}
