#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;
			using namespace reflection;
			using namespace reflection::description;

			void LoadTypes(WfCppConfig* config, List<ITypeDescriptor*>& tds)
			{
				CopyFrom(
					tds,
					From(config->manager->declarationTypes.Values())
						.OrderBy([](Ptr<ITypeDescriptor> a, Ptr<ITypeDescriptor> b)
						{
							return WString::Compare(a->GetTypeName(), b->GetTypeName());
						})
						.Select([](Ptr<ITypeDescriptor> td)
						{
							return td.Obj();
						})
					);
			}

			void WfCppConfig::WriteHeader_Reflection(stream::StreamWriter& writer)
			{
				List<ITypeDescriptor*> tds;
				LoadTypes(this, tds);

				writer.WriteLine(L"namespace vl");
				writer.WriteLine(L"{");
				writer.WriteLine(L"\tnamespace reflection");
				writer.WriteLine(L"\t{");
				writer.WriteLine(L"\t\tnamespace description");
				writer.WriteLine(L"\t\t{");
				writer.WriteLine(L"#ifndef VCZH_DEBUG_NO_REFLECTION");

				FOREACH(ITypeDescriptor*, td, tds)
				{
					writer.WriteString(L"\t\t\tDECL_TYPE_INFO(");
					writer.WriteString(ConvertType(td));
					writer.WriteLine(L")");
				}

				List<ITypeDescriptor*> tdInterfaces;
				CopyFrom(
					tdInterfaces,
					From(tds)
						.Where([](ITypeDescriptor* td)
						{
							return td->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface;
						})
					);
				{
					vint processed = 0;
					while (processed < tdInterfaces.Count())
					{
						vint count = tdInterfaces.Count();
						for (vint i = processed; i < count; i++)
						{
							auto td = tdInterfaces[i];
							if (Range<vint>(0, td->GetBaseTypeDescriptorCount())
								.All([&](vint baseIndex)
								{
									return tdInterfaces.IndexOf(td->GetBaseTypeDescriptor(baseIndex)) < processed;
								}))
							{
								tdInterfaces.RemoveAt(i);
								tdInterfaces.Insert(processed++, td);
							}
						}
					}
				}

				if (tdInterfaces.Count() > 0)
				{
					FOREACH(ITypeDescriptor*, td, tdInterfaces)
					{
						List<ITypeDescriptor*> baseTds;
						CopyFrom(
							baseTds,
							Range<vint>(0, td->GetBaseTypeDescriptorCount())
								.Select([&](vint index)
								{
									return td->GetBaseTypeDescriptor(index);
								})
								.Where([&](ITypeDescriptor* baseTd)
								{
									return baseTd->GetTypeDescriptorFlags() != TypeDescriptorFlags::IDescriptable;
								})
							);

						writer.WriteLine(L"");
						if (baseTds.Count() > 0)
						{
							writer.WriteString(L"\t\t\tBEGIN_INTERFACE_PROXY_SHAREDPTR(");
							writer.WriteString(ConvertType(td));
							FOREACH(ITypeDescriptor*, baseTd, baseTds)
							{
								writer.WriteString(L", ");
								writer.WriteString(ConvertType(baseTd));
							}
							writer.WriteLine(L")");
						}
						else
						{
							writer.WriteString(L"\t\t\tBEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(");
							writer.WriteString(ConvertType(td));
							writer.WriteLine(L")");
						}

						vint methodGroupCount = td->GetMethodGroupCount();
						for (vint i = 0; i < methodGroupCount; i++)
						{
							auto methodGroup = td->GetMethodGroup(i);
							vint methodCount = methodGroup->GetMethodCount();
							for (vint j = 0; j < methodCount; j++)
							{
								auto methodInfo = methodGroup->GetMethod(j);
								if (!methodInfo->IsStatic())
								{
									writer.WriteString(L"\t\t\t\t");
									WriteFunctionHeader(writer, methodInfo, ConvertName(methodInfo->GetName()), true);
									writer.WriteLine(L" override");
									writer.WriteLine(L"\t\t\t\t{");

									if (methodInfo->GetReturn()->GetTypeDescriptor() == description::GetTypeDescriptor<void>())
									{
										writer.WriteString(L"\t\t\t\t\tINVOKE_INTERFACE_PROXY");
									}
									else
									{
										writer.WriteString(L"\t\t\t\t\tINVOKEGET_INTERFACE_PROXY");
									}

									if (methodInfo->GetParameterCount() > 0)
									{
										writer.WriteString(L"(");
										writer.WriteString(ConvertName(methodInfo->GetName()));
										vint parameterCount = methodInfo->GetParameterCount();
										for (vint k = 0; k < parameterCount; k++)
										{
											writer.WriteString(L", ");
											writer.WriteString(methodInfo->GetParameter(k)->GetName());
										}
										writer.WriteLine(L");");
									}
									else
									{
										writer.WriteString(L"_NOPARAMS(");
										writer.WriteString(ConvertName(methodInfo->GetName()));
										writer.WriteLine(L");");
									}

									writer.WriteLine(L"\t\t\t\t}");
								}
							}
						}

						writer.WriteString(L"\t\t\tEND_INTERFACE_PROXY(");
						writer.WriteString(ConvertType(td));
						writer.WriteLine(L")");
					}
				}

				writer.WriteLine(L"#endif");
				writer.WriteLine(L"");

				writer.WriteString(L"\t\t\textern bool Load");
				writer.WriteString(assemblyName);
				writer.WriteLine(L"Types();");
				writer.WriteLine(L"\t\t}");
				writer.WriteLine(L"\t}");
				writer.WriteLine(L"}");
			}

			void WfCppConfig::WriteCpp_Reflection(stream::StreamWriter& writer)
			{
				List<ITypeDescriptor*> tds;
				LoadTypes(this, tds);

				writer.WriteLine(L"namespace vl");
				writer.WriteLine(L"{");
				writer.WriteLine(L"\tnamespace reflection");
				writer.WriteLine(L"\t{");
				writer.WriteLine(L"\t\tnamespace description");
				writer.WriteLine(L"\t\t{");
				writer.WriteLine(L"#ifndef VCZH_DEBUG_NO_REFLECTION");

				FOREACH(ITypeDescriptor*, td, tds)
				{
					writer.WriteString(L"\t\t\tIMPL_CPP_TYPE_INFO(");
					writer.WriteString(ConvertType(td));
					writer.WriteLine(L")");
				}
				writer.WriteLine(L"");

				writer.WriteLine(L"#define _ ,");
				FOREACH(ITypeDescriptor*, td, tds)
				{
					switch (td->GetTypeDescriptorFlags())
					{
					case TypeDescriptorFlags::NormalEnum:
					case TypeDescriptorFlags::FlagEnum:
						{
							writer.WriteString(L"\t\t\tBEGIN_ENUM_ITEM");
							if (td->GetTypeDescriptorFlags() == TypeDescriptorFlags::FlagEnum)
							{
								writer.WriteString(L"_MERGABLE");
							}
							writer.WriteString(L"(");
							writer.WriteString(ConvertType(td));
							writer.WriteLine(L")");

							auto enumType = td->GetEnumType();
							vint count = enumType->GetItemCount();
							for (vint i = 0; i < count; i++)
							{
								writer.WriteString(L"\t\t\t\tENUM_CLASS_ITEM(");
								writer.WriteString(ConvertName(enumType->GetItemName(i)));
								writer.WriteLine(L")");
							}

							writer.WriteString(L"\t\t\tEND_ENUM_ITEM(");
							writer.WriteString(ConvertType(td));
							writer.WriteLine(L")");
						}
						break;
					case TypeDescriptorFlags::Struct:
						{
							writer.WriteString(L"\t\t\tBEGIN_STRUCT_MEMBER(");
							writer.WriteString(ConvertType(td));
							writer.WriteLine(L")");

							vint count = td->GetPropertyCount();
							for (vint i = 0; i < count; i++)
							{
								writer.WriteString(L"\t\t\t\tSTRUCT_MEMBER(");
								writer.WriteString(ConvertName(td->GetProperty(i)->GetName()));
								writer.WriteLine(L")");
							}

							writer.WriteString(L"\t\t\tEND_STRUCT_MEMBER(");
							writer.WriteString(ConvertType(td));
							writer.WriteLine(L")");
						}
						break;
					case TypeDescriptorFlags::Interface:
					case TypeDescriptorFlags::Class:
						{
							if (td->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface)
							{
								writer.WriteString(L"\t\t\tBEGIN_INTERFACE_MEMBER(");
							}
							else
							{
								writer.WriteString(L"\t\t\tBEGIN_CLASS_MEMBER(");
							}
							writer.WriteString(ConvertType(td));
							writer.WriteLine(L")");

							vint methodGroupCount = td->GetMethodGroupCount();
							for (vint i = 0; i < methodGroupCount; i++)
							{
								auto methodGroup = td->GetMethodGroup(i);
								vint methodCount = methodGroup->GetMethodCount();
								for (vint j = 0; j < methodCount; j++)
								{
									auto methodInfo = methodGroup->GetMethod(j);
									if (methodInfo->IsStatic())
									{
										writer.WriteString(L"\t\t\t\tCLASS_MEMBER_STATIC_METHOD");
									}
									else
									{
										writer.WriteString(L"\t\t\t\tCLASS_MEMBER_METHOD");
									}
									if (methodCount > 1)
									{
										writer.WriteString(L"_OVERLOAD");
									}
									writer.WriteString(L"(");
									writer.WriteString(ConvertName(methodInfo->GetName()));

									vint parameterCount = methodInfo->GetParameterCount();
									if (parameterCount > 0)
									{
										writer.WriteString(L", {");
										for (vint k = 0; k < parameterCount; k++)
										{
											if (k > 0)
											{
												writer.WriteString(L" _");
											}
											writer.WriteString(L" L\"");
											writer.WriteString(ConvertName(methodInfo->GetParameter(k)->GetName()));
											writer.WriteString(L"\"");
										}
										writer.WriteString(L" }");
									}
									else
									{
										writer.WriteString(L", NO_PARAMETER");
									}
									if (methodCount > 1)
									{
										writer.WriteString(L", ");
										auto typeDecorator = methodInfo->IsStatic() ? WString(L"(*)", false) : L"(" + ConvertType(td) + L"::*)";
										writer.WriteString(ConvertFunctionType(methodInfo, typeDecorator));
									}
									writer.WriteLine(L")");
								}
							}

							vint eventCount = td->GetEventCount();
							for (vint i = 0; i < eventCount; i++)
							{
								auto eventInfo = td->GetEvent(i);
								writer.WriteString(L"\t\t\t\tCLASS_MEMBER_EVENT(");
								writer.WriteString(ConvertName(eventInfo->GetName()));
								writer.WriteLine(L")");
							}

							vint propertyCount = td->GetPropertyCount();
							for (vint i = 0; i < propertyCount; i++)
							{
								auto propertyInfo = td->GetProperty(i);
								if (auto getter = propertyInfo->GetGetter())
								{
									if (auto eventInfo = propertyInfo->GetValueChangedEvent())
									{
										if (auto setter = propertyInfo->GetSetter())
										{
											writer.WriteString(L"\t\t\t\tCLASS_MEMBER_PROPERTY_EVENT(");
											writer.WriteString(ConvertName(propertyInfo->GetName()));
											writer.WriteString(L", ");
											writer.WriteString(ConvertName(getter->GetName()));
											writer.WriteString(L", ");
											writer.WriteString(ConvertName(setter->GetName()));
											writer.WriteString(L", ");
											writer.WriteString(ConvertName(eventInfo->GetName()));
											writer.WriteLine(L")");
										}
										else
										{
											writer.WriteString(L"\t\t\t\tCLASS_MEMBER_PROPERTY_EVENT_READONLY(");
											writer.WriteString(ConvertName(propertyInfo->GetName()));
											writer.WriteString(L", ");
											writer.WriteString(ConvertName(getter->GetName()));
											writer.WriteString(L", ");
											writer.WriteString(ConvertName(eventInfo->GetName()));
											writer.WriteLine(L")");
										}
									}
									else
									{
										if (auto setter = propertyInfo->GetSetter())
										{
											writer.WriteString(L"\t\t\t\tCLASS_MEMBER_PROPERTY(");
											writer.WriteString(ConvertName(propertyInfo->GetName()));
											writer.WriteString(L", ");
											writer.WriteString(ConvertName(getter->GetName()));
											writer.WriteString(L", ");
											writer.WriteString(ConvertName(setter->GetName()));
											writer.WriteLine(L")");
										}
										else
										{
											writer.WriteString(L"\t\t\t\tCLASS_MEMBER_PROPERTY_READONLY(");
											writer.WriteString(ConvertName(propertyInfo->GetName()));
											writer.WriteString(L", ");
											writer.WriteString(ConvertName(getter->GetName()));
											writer.WriteLine(L")");
										}
									}
								}
								else
								{
									writer.WriteString(L"\t\t\t\tCLASS_MEMBER_FIELD(");
									writer.WriteString(ConvertName(propertyInfo->GetName()));
									writer.WriteLine(L")");
								}
							}

							if (td->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface)
							{
								writer.WriteString(L"\t\t\tEND_INTERFACE_MEMBER(");
							}
							else
							{
								writer.WriteString(L"\t\t\tEND_CLASS_MEMBER(");
							}
							writer.WriteString(ConvertType(td));
							writer.WriteLine(L")");
						}
						break;
					default:;
					}
					writer.WriteLine(L"");
				}
				writer.WriteLine(L"#undef _");

				writer.WriteString(L"\t\t\tclass ");
				writer.WriteString(assemblyName);
				writer.WriteLine(L"TypeLoader : public Object, public ITypeLoader");
				writer.WriteLine(L"\t\t\t{");
				writer.WriteLine(L"\t\t\tpublic:");
				writer.WriteLine(L"\t\t\t\tvoid Load(ITypeManager* manager)");
				writer.WriteLine(L"\t\t\t\t{");

				FOREACH(ITypeDescriptor*, td, tds)
				{
					writer.WriteString(L"\t\t\t\t\tADD_TYPE_INFO(");
					writer.WriteString(ConvertType(td));
					writer.WriteLine(L")");
				}
				writer.WriteLine(L"\t\t\t\t}");
				writer.WriteLine(L"");
				writer.WriteLine(L"\t\t\t\tvoid Unload(ITypeManager* manager)");
				writer.WriteLine(L"\t\t\t\t{");
				writer.WriteLine(L"\t\t\t\t}");
				writer.WriteLine(L"\t\t\t};");

				writer.WriteLine(L"#endif");
				writer.WriteLine(L"");

				writer.WriteString(L"\t\t\tbool Load");
				writer.WriteString(assemblyName);
				writer.WriteLine(L"Types()");
				writer.WriteLine(L"\t\t\t{");
				writer.WriteLine(L"#ifndef VCZH_DEBUG_NO_REFLECTION");
				writer.WriteLine(L"\t\t\t\tif (auto manager = GetGlobalTypeManager())");
				writer.WriteLine(L"\t\t\t\t{");
				writer.WriteString(L"\t\t\t\t\treturn manager->AddTypeLoader(MakePtr<");
				writer.WriteString(assemblyName);
				writer.WriteLine(L"TypeLoader>());");
				writer.WriteLine(L"\t\t\t\t}");
				writer.WriteLine(L"#endif");
				writer.WriteLine(L"\t\t\t\treturn false;");
				writer.WriteLine(L"\t\t\t}");

				writer.WriteLine(L"\t\t}");
				writer.WriteLine(L"\t}");
				writer.WriteLine(L"}");
			}
		}
	}
}