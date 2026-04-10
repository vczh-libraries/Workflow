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
						.OrderByKey([](auto&& a)
						{
							return a->GetTypeName();
						})
						.Select([](Ptr<ITypeDescriptor> td)
						{
							return td.Obj();
						})
					);
			}

			void WfCppConfig::WriteHeader_Reflection(stream::StreamWriter& writer)
			{
				writer.WriteLine(L"/***********************************************************************");
				writer.WriteLine(L"Reflection");
				writer.WriteLine(L"***********************************************************************/");
				writer.WriteLine(L"");
				List<ITypeDescriptor*> tds;
				LoadTypes(this, tds);

				writer.WriteLine(L"namespace vl");
				writer.WriteLine(L"{");
				writer.WriteLine(L"\tnamespace reflection");
				writer.WriteLine(L"\t{");
				writer.WriteLine(L"\t\tnamespace description");
				writer.WriteLine(L"\t\t{");
				writer.WriteLine(L"#ifndef VCZH_DEBUG_NO_REFLECTION");

				for (auto td : tds)
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
					writer.WriteLine(L"");
					writer.WriteLine(L"#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA");
					for (auto td : tdInterfaces)
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
							for (auto baseTd : baseTds)
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
											writer.WriteString(ConvertName(methodInfo->GetParameter(k)->GetName()));
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
					writer.WriteLine(L"#endif");
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
				writer.WriteLine(L"/***********************************************************************");
				writer.WriteLine(L"Reflection");
				writer.WriteLine(L"***********************************************************************/");
				writer.WriteLine(L"");
				List<ITypeDescriptor*> tds;
				LoadTypes(this, tds);

				Dictionary<ITypeDescriptor*, Ptr<WfDeclaration>> typeDecls;
				for (auto [decl, index] : indexed(manager->declarationTypes.Keys()))
				{
					auto td = manager->declarationTypes.Values()[index].Obj();
					if (!typeDecls.Keys().Contains(td))
					{
						typeDecls.Add(td, decl);
					}
				}

				Dictionary<IMemberInfo*, Ptr<WfDeclaration>> memberDecls;
				for (auto [decl, index] : indexed(manager->declarationMemberInfos.Keys()))
				{
					auto memberInfo = manager->declarationMemberInfos.Values()[index].Obj();
					if (!memberDecls.Keys().Contains(memberInfo))
					{
						memberDecls.Add(memberInfo, decl);
					}
				}

				writer.WriteLine(L"namespace vl");
				writer.WriteLine(L"{");
				writer.WriteLine(L"\tnamespace reflection");
				writer.WriteLine(L"\t{");
				writer.WriteLine(L"\t\tnamespace description");
				writer.WriteLine(L"\t\t{");
				writer.WriteLine(L"#ifndef VCZH_DEBUG_NO_REFLECTION");

				for (auto td : tds)
				{
					writer.WriteString(L"\t\t\tIMPL_CPP_TYPE_INFO(");
					WString type = ConvertType(td);
					if (type.Length() > 2 && type.Sub(0, 2) == L"::")
					{
						type = type.Sub(2, type.Length() - 2);
					}
					writer.WriteString(type);
					writer.WriteLine(L")");
				}
				writer.WriteLine(L"");

				writer.WriteLine(L"#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA");
				writer.WriteLine(L"#define _ ,");

				auto FindTypeDecl = [&](ITypeDescriptor* td) -> Ptr<WfDeclaration>
				{
					if (auto index = typeDecls.Keys().IndexOf(td); index != -1)
					{
						return typeDecls.Values()[index];
					}
					return nullptr;
				};

				auto FindMemberDecl = [&](IMemberInfo* memberInfo) -> Ptr<WfDeclaration>
				{
					if (auto index = memberDecls.Keys().IndexOf(memberInfo); index != -1)
					{
						return memberDecls.Values()[index];
					}
					return nullptr;
				};

				auto ResolveAttributeTd = [&](Ptr<WfAttribute> att)
				{
					return manager->ResolveWorkflowAttribute(att->category.value, att->name.value);
				};

				auto WriteAttributeMacro = [&](const wchar_t* indent, const wchar_t* macroName, Ptr<WfAttribute> att, const wchar_t* paramName)
				{
					auto info = ResolveAttributeTd(att);
					if (!info.exists || info.attributeType == nullptr) return;

					writer.WriteString(indent);
					writer.WriteString(macroName);
					writer.WriteString(L"(");

					if (paramName)
					{
						writer.WriteString(L"L\"");
						writer.WriteString(ConvertName(paramName));
						writer.WriteString(L"\", ");
					}

					writer.WriteString(ConvertType(info.attributeType));
					if (info.hasArgument)
					{
						auto value = attributeEvaluator->GetAttributeValue(att);
						CHECK_ERROR(value.type == runtime::WfInsType::String, L"Attribute argument must be serialized as WString here.");
						writer.WriteString(L", ");
						WriteWStringLiteralUnmanaged(writer, value.stringValue);
					}

					writer.WriteLine(L")");
				};

				auto WriteDeclarationAttributes = [&](const wchar_t* indent, const wchar_t* macroName, Ptr<WfDeclaration> decl)
				{
					if (!decl) return;
					for (auto att : decl->attributes)
					{
						WriteAttributeMacro(indent, macroName, att, nullptr);
					}
				};

				auto WriteArgumentAttributes = [&](const wchar_t* indent, Ptr<WfFunctionArgument> arg)
				{
					if (!arg) return;
					for (auto att : arg->attributes)
					{
						WriteAttributeMacro(indent, L"ATTRIBUTE_PARAMETER", att, arg->name.value.Buffer());
					}
				};

				auto FindFunctionArg = [&](WfFunctionDeclaration* functionDecl, const WString& name) -> Ptr<WfFunctionArgument>
				{
					return From(functionDecl->arguments)
						.Where([&](Ptr<WfFunctionArgument> arg)
						{
							return arg->name.value == name;
						})
						.First(Ptr<WfFunctionArgument>());
				};

				auto FindCtorArg = [&](WfConstructorDeclaration* ctorDecl, const WString& name) -> Ptr<WfFunctionArgument>
				{
					return From(ctorDecl->arguments)
						.Where([&](Ptr<WfFunctionArgument> arg)
						{
							return arg->name.value == name;
						})
						.First(Ptr<WfFunctionArgument>());
				};

				for (auto td : tds)
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
							WriteDeclarationAttributes(L"\t\t\t\t", L"ATTRIBUTE_TYPE", FindTypeDecl(td));

							vint count = td->GetPropertyCount();
							for (vint i = 0; i < count; i++)
							{
								auto propertyInfo = td->GetProperty(i);
								writer.WriteString(L"\t\t\t\tSTRUCT_MEMBER(");
								writer.WriteString(ConvertName(propertyInfo->GetName()));
								writer.WriteLine(L")");
								WriteDeclarationAttributes(L"\t\t\t\t", L"ATTRIBUTE_MEMBER", FindMemberDecl(propertyInfo));
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
							WriteDeclarationAttributes(L"\t\t\t\t", L"ATTRIBUTE_TYPE", FindTypeDecl(td));

							vint baseCount = td->GetBaseTypeDescriptorCount();
							for (vint i = 0; i < baseCount; i++)
							{
								writer.WriteString(L"\t\t\t\tCLASS_MEMBER_BASE(");
								writer.WriteString(ConvertType(td->GetBaseTypeDescriptor(i)));
								writer.WriteLine(L")");
							}

							if (td->GetTypeDescriptorFlags() == TypeDescriptorFlags::Class)
							{
								auto ctorGroup = td->GetConstructorGroup();
								vint methodCount = ctorGroup->GetMethodCount();
								for (vint j = 0; j < methodCount; j++)
								{
									auto methodInfo = ctorGroup->GetMethod(j);
									auto memberDecl = FindMemberDecl(methodInfo);
									vint parameterCount = methodInfo->GetParameterCount();

									writer.WriteString(L"\t\t\t\tCLASS_MEMBER_CONSTRUCTOR(");
									writer.WriteString(ConvertType(methodInfo->GetReturn()));
									writer.WriteString(L"(");
									for (vint k = 0; k < parameterCount; k++)
									{
										if (k > 0)
										{
											writer.WriteString(L", ");
										}
										writer.WriteString(ConvertArgumentType(methodInfo->GetParameter(k)->GetType()));
									}
									writer.WriteString(L")");

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
										writer.WriteLine(L" })");
									}
									else
									{
										writer.WriteLine(L", NO_PARAMETER)");
									}

									WriteDeclarationAttributes(L"\t\t\t\t", L"ATTRIBUTE_MEMBER", memberDecl);
									if (auto ctorDecl = memberDecl.Cast<WfConstructorDeclaration>())
									{
										for (vint k = 0; k < parameterCount; k++)
										{
											WriteArgumentAttributes(L"\t\t\t\t", FindCtorArg(ctorDecl.Obj(), methodInfo->GetParameter(k)->GetName()));
										}
									}
								}
							}

							vint methodGroupCount = td->GetMethodGroupCount();
							for (vint i = 0; i < methodGroupCount; i++)
							{
								auto methodGroup = td->GetMethodGroup(i);
								vint methodCount = methodGroup->GetMethodCount();
								for (vint j = 0; j < methodCount; j++)
								{
									auto methodInfo = methodGroup->GetMethod(j);
									auto memberDecl = FindMemberDecl(methodInfo);
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
										auto typeDecorator = methodInfo->IsStatic() ? WString::Unmanaged(L"(*)") : L"(" + ConvertType(td) + L"::*)";
										writer.WriteString(ConvertFunctionType(methodInfo, typeDecorator));
									}
									writer.WriteLine(L")");

									WriteDeclarationAttributes(L"\t\t\t\t", L"ATTRIBUTE_MEMBER", memberDecl);
									if (auto functionDecl = memberDecl.Cast<WfFunctionDeclaration>())
									{
										for (vint k = 0; k < parameterCount; k++)
										{
											WriteArgumentAttributes(L"\t\t\t\t", FindFunctionArg(functionDecl.Obj(), methodInfo->GetParameter(k)->GetName()));
										}
									}
								}
							}

							vint eventCount = td->GetEventCount();
							for (vint i = 0; i < eventCount; i++)
							{
								auto eventInfo = td->GetEvent(i);
								writer.WriteString(L"\t\t\t\tCLASS_MEMBER_EVENT(");
								writer.WriteString(ConvertName(eventInfo->GetName()));
								writer.WriteLine(L")");
								WriteDeclarationAttributes(L"\t\t\t\t", L"ATTRIBUTE_MEMBER", FindMemberDecl(eventInfo));
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
									else if (auto setter = propertyInfo->GetSetter())
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
								else
								{
									writer.WriteString(L"\t\t\t\tCLASS_MEMBER_FIELD(");
									writer.WriteString(ConvertName(propertyInfo->GetName()));
									writer.WriteLine(L")");
								}

								WriteDeclarationAttributes(L"\t\t\t\t", L"ATTRIBUTE_MEMBER", FindMemberDecl(propertyInfo));
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

				for (auto td : tds)
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
				writer.WriteLine(L"#endif");
				writer.WriteLine(L"");

				writer.WriteString(L"\t\t\tbool Load");
				writer.WriteString(assemblyName);
				writer.WriteLine(L"Types()");
				writer.WriteLine(L"\t\t\t{");
				writer.WriteLine(L"#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA");
				writer.WriteLine(L"\t\t\t\tif (auto manager = GetGlobalTypeManager())");
				writer.WriteLine(L"\t\t\t\t{");
				writer.WriteString(L"\t\t\t\t\treturn manager->AddTypeLoader(Ptr(new ");
				writer.WriteString(assemblyName);
				writer.WriteLine(L"TypeLoader));");
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
