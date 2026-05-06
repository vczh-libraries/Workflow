#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
/***********************************************************************
GenerateDtsFromRpcMetadata
***********************************************************************/

			namespace rpc_dts_generating
			{
				using namespace collections;
				using namespace reflection;
				using namespace reflection::description;

				enum class DtsPrimitiveKind
				{
					Number,
					Boolean,
					String,
				};

				struct DtsPrimitiveModel
				{
					WString					keyword;
					DtsPrimitiveKind		kind = DtsPrimitiveKind::Number;
				};

				struct DtsEnumItemModel
				{
					WString					name;
					vuint64_t				value = 0;
				};

				struct DtsEnumModel
				{
					WString						fullName;
					Ptr<List<DtsEnumItemModel>>	items;
				};

				struct DtsFieldModel
				{
					WString					name;
					Ptr<WfType>				type;
				};

				struct DtsStructModel
				{
					WString					fullName;
					Ptr<List<DtsFieldModel>> fields;
				};

				struct DtsContext
				{
					WfLexicalScopeManager*	manager = nullptr;
					List<DtsEnumModel>*		enums = nullptr;
					List<DtsStructModel>*	structs = nullptr;
				};

				void AppendChar(WString& text, wchar_t c)
				{
					text += WString::CopyFrom(&c, 1);
				}

				bool IsDtsIdentifierStart(wchar_t c)
				{
					return (L'a' <= c && c <= L'z')
						|| (L'A' <= c && c <= L'Z')
						|| c == L'_'
						|| c == L'$';
				}

				bool IsDtsIdentifierPart(wchar_t c)
				{
					return IsDtsIdentifierStart(c) || (L'0' <= c && c <= L'9');
				}

				WString EscapeDtsString(const WString& value)
				{
					WString result = L"\"";
					for (vint i = 0; i < value.Length(); i++)
					{
						auto c = value[i];
						switch (c)
						{
						case L'\\': result += L"\\\\"; break;
						case L'"': result += L"\\\""; break;
						case L'\r': result += L"\\r"; break;
						case L'\n': result += L"\\n"; break;
						case L'\t': result += L"\\t"; break;
						default:
							AppendChar(result, c);
							break;
						}
					}
					result += L"\"";
					return result;
				}

				WString GetDtsPropertyName(const WString& name)
				{
					if (name.Length() > 0 && IsDtsIdentifierStart(name[0]))
					{
						bool isIdentifier = true;
						for (vint i = 1; i < name.Length(); i++)
						{
							if (!IsDtsIdentifierPart(name[i]))
							{
								isIdentifier = false;
								break;
							}
						}
						if (isIdentifier)
						{
							return name;
						}
					}
					return EscapeDtsString(name);
				}

				WString MakeDtsTypeName(const WString& fullName)
				{
					WString result;
					for (vint i = 0; i < fullName.Length(); i++)
					{
						auto c = fullName[i];
						if ((result.Length() == 0 && IsDtsIdentifierStart(c)) || (result.Length() > 0 && IsDtsIdentifierPart(c)))
						{
							AppendChar(result, c);
						}
						else if (result.Length() == 0 || result[result.Length() - 1] != L'_')
						{
							result += L"_";
						}
					}
					if (result.Length() == 0)
					{
						return L"_";
					}
					if (result[result.Length() - 1] == L'_')
					{
						result = result.Left(result.Length() - 1);
					}
					return result;
				}

				WString MakeUnknownStructDtsTypeName(const WString& fullName)
				{
					return L"UnknownType_" + MakeDtsTypeName(fullName);
				}

				bool TryGetPrimitiveModel(const WString& fullName, DtsPrimitiveModel& primitive)
				{
					auto tryName = [&](const wchar_t* name, DtsPrimitiveKind kind)
					{
						if (fullName == WString::Unmanaged(L"system::") + name)
						{
							primitive.keyword = WString::Unmanaged(name);
							primitive.kind = kind;
							return true;
						}
						return false;
					};

					return tryName(L"UInt8", DtsPrimitiveKind::Number)
						|| tryName(L"UInt16", DtsPrimitiveKind::Number)
						|| tryName(L"UInt32", DtsPrimitiveKind::Number)
						|| tryName(L"UInt64", DtsPrimitiveKind::Number)
						|| tryName(L"Int8", DtsPrimitiveKind::Number)
						|| tryName(L"Int16", DtsPrimitiveKind::Number)
						|| tryName(L"Int32", DtsPrimitiveKind::Number)
						|| tryName(L"Int64", DtsPrimitiveKind::Number)
						|| tryName(L"Single", DtsPrimitiveKind::Number)
						|| tryName(L"Double", DtsPrimitiveKind::Number)
						|| tryName(L"Boolean", DtsPrimitiveKind::Boolean)
						|| tryName(L"Char", DtsPrimitiveKind::String)
						|| tryName(L"String", DtsPrimitiveKind::String)
						|| tryName(L"DateTime", DtsPrimitiveKind::String)
						|| tryName(L"Locale", DtsPrimitiveKind::String);
				}

				WString GetPredefinedFullName(WfLexicalScopeManager* manager, WfPredefinedTypeName name)
				{
					switch (name)
					{
					case WfPredefinedTypeName::Int:
						return manager && manager->cputdSInt ? manager->cputdSInt->GetTypeName() : WString::Unmanaged(L"system::Int64");
					case WfPredefinedTypeName::UInt:
						return manager && manager->cputdUInt ? manager->cputdUInt->GetTypeName() : WString::Unmanaged(L"system::UInt64");
					case WfPredefinedTypeName::Float:
						return L"system::Single";
					case WfPredefinedTypeName::Double:
						return L"system::Double";
					case WfPredefinedTypeName::String:
						return L"system::String";
					case WfPredefinedTypeName::Char:
						return L"system::Char";
					case WfPredefinedTypeName::Bool:
						return L"system::Boolean";
					case WfPredefinedTypeName::Object:
						return L"system::Object";
					case WfPredefinedTypeName::Interface:
						return L"system::Interface";
					default:
						return L"";
					}
				}

				WString GetTypeFullName(WfLexicalScopeManager* manager, WfType* type)
				{
					if (!type)
					{
						return L"";
					}
					if (auto predefined = dynamic_cast<WfPredefinedType*>(type))
					{
						return GetPredefinedFullName(manager, predefined->name);
					}
					if (auto top = dynamic_cast<WfTopQualifiedType*>(type))
					{
						return top->name.value;
					}
					if (auto reference = dynamic_cast<WfReferenceType*>(type))
					{
						return reference->name.value;
					}
					if (auto child = dynamic_cast<WfChildType*>(type))
					{
						auto parent = GetTypeFullName(manager, child->parent.Obj());
						return parent == L"" ? child->name.value : parent + L"::" + child->name.value;
					}
					if (auto raw = dynamic_cast<WfRawPointerType*>(type))
					{
						return GetTypeFullName(manager, raw->element.Obj());
					}
					if (auto shared = dynamic_cast<WfSharedPointerType*>(type))
					{
						return GetTypeFullName(manager, shared->element.Obj());
					}
					if (auto nullable = dynamic_cast<WfNullableType*>(type))
					{
						return GetTypeFullName(manager, nullable->element.Obj());
					}
					return L"";
				}

				vint FindStruct(const List<DtsStructModel>& structs, const WString& fullName)
				{
					for (vint i = 0; i < structs.Count(); i++)
					{
						if (structs[i].fullName == fullName)
						{
							return i;
						}
					}
					return -1;
				}

				vint FindEnum(const List<DtsEnumModel>& enums, const WString& fullName)
				{
					for (vint i = 0; i < enums.Count(); i++)
					{
						if (enums[i].fullName == fullName)
						{
							return i;
						}
					}
					return -1;
				}

				WString GetDtsTypeFromFullName(DtsContext& context, const WString& fullName)
				{
					DtsPrimitiveModel primitive;
					if (TryGetPrimitiveModel(fullName, primitive))
					{
						switch (primitive.kind)
						{
						case DtsPrimitiveKind::Number:
							return L"number";
						case DtsPrimitiveKind::Boolean:
							return L"boolean";
						case DtsPrimitiveKind::String:
							return L"string";
						default:
							CHECK_FAIL(L"Internal error: Unknown primitive kind.");
						}
					}
					if (context.enums && FindEnum(*context.enums, fullName) != -1)
					{
						return L"number";
					}
					if (context.structs && FindStruct(*context.structs, fullName) != -1)
					{
						return MakeDtsTypeName(fullName);
					}
					return L"UnknownTypeSchema";
				}

				WString GetDtsTypeFromType(DtsContext& context, WfType* type)
				{
					if (!type)
					{
						return L"UnknownTypeSchema";
					}
					if (auto nullable = dynamic_cast<WfNullableType*>(type))
					{
						return GetDtsTypeFromType(context, nullable->element.Obj()) + L" | null";
					}
					if (auto raw = dynamic_cast<WfRawPointerType*>(type))
					{
						return GetDtsTypeFromType(context, raw->element.Obj());
					}
					if (auto shared = dynamic_cast<WfSharedPointerType*>(type))
					{
						return GetDtsTypeFromType(context, shared->element.Obj());
					}
					if (auto enumerable = dynamic_cast<WfEnumerableType*>(type))
					{
						return GetDtsTypeFromType(context, enumerable->element.Obj()) + L"[]";
					}
					if (auto map = dynamic_cast<WfMapType*>(type))
					{
						if (map->key)
						{
							return L"[" + GetDtsTypeFromType(context, map->key.Obj()) + L", " + GetDtsTypeFromType(context, map->value.Obj()) + L"][]";
						}
						return GetDtsTypeFromType(context, map->value.Obj()) + L"[]";
					}
					if (auto observable = dynamic_cast<WfObservableListType*>(type))
					{
						return GetDtsTypeFromType(context, observable->element.Obj()) + L"[]";
					}
					return GetDtsTypeFromFullName(context, GetTypeFullName(context.manager, type));
				}

				void AddDtsTypesFromDeclarations(
					const List<Ptr<WfDeclaration>>& declarations,
					const WString& prefix,
					List<DtsEnumModel>& enums,
					List<DtsStructModel>& structs)
				{
					for (auto declaration : declarations)
					{
						auto fullName = prefix == L"" ? declaration->name.value : prefix + L"::" + declaration->name.value;
						if (auto namespaceDecl = declaration.Cast<WfNamespaceDeclaration>())
						{
							AddDtsTypesFromDeclarations(namespaceDecl->declarations, fullName, enums, structs);
						}
						else if (auto enumDecl = declaration.Cast<WfEnumDeclaration>())
						{
							if (FindEnum(enums, fullName) == -1)
							{
								DtsEnumModel model;
								model.fullName = fullName;
								model.items = Ptr(new List<DtsEnumItemModel>);
								Dictionary<WString, vuint64_t> values;
								for (auto item : enumDecl->items)
								{
									DtsEnumItemModel itemModel;
									itemModel.name = item->name.value;
									if (item->kind == WfEnumItemKind::Constant)
									{
										itemModel.value = wtou64(item->number.value);
									}
									else
									{
										for (auto intersection : item->intersections)
										{
											auto index = values.Keys().IndexOf(intersection->name.value);
											if (index != -1)
											{
												itemModel.value |= values.Values()[index];
											}
										}
									}
									values.Add(itemModel.name, itemModel.value);
									model.items->Add(itemModel);
								}
								enums.Add(model);
							}
						}
						else if (auto structDecl = declaration.Cast<WfStructDeclaration>())
						{
							if (FindStruct(structs, fullName) != -1)
							{
								continue;
							}

							DtsStructModel model;
							model.fullName = fullName;
							model.fields = Ptr(new List<DtsFieldModel>);
							for (auto member : structDecl->members)
							{
								DtsFieldModel field;
								field.name = member->name.value;
								field.type = CopyType(member->type.Obj());
								model.fields->Add(field);
							}
							structs.Add(model);
						}
					}
				}

				void AddRpcObjectReferenceDtsStruct(List<DtsStructModel>& structs)
				{
					auto fullName = WString::Unmanaged(L"system::RpcObjectReference");
					if (FindStruct(structs, fullName) != -1)
					{
						return;
					}

					auto td = GetTypeDescriptor<vl::rpc_controller::RpcObjectReference>();
					if (!td)
					{
						return;
					}

					DtsStructModel model;
					model.fullName = fullName;
					model.fields = Ptr(new List<DtsFieldModel>);
					for (vint i = 0; i < td->GetPropertyCount(); i++)
					{
						auto propertyInfo = td->GetProperty(i);
						DtsFieldModel field;
						field.name = propertyInfo->GetName();
						field.type = GetTypeFromTypeInfo(propertyInfo->GetReturn());
						model.fields->Add(field);
					}
					structs.Add(model);
				}

				const wchar_t* GetDtsPrimitiveTypeScriptType(DtsPrimitiveKind kind)
				{
					switch (kind)
					{
					case DtsPrimitiveKind::Number:
						return L"number";
					case DtsPrimitiveKind::Boolean:
						return L"boolean";
					case DtsPrimitiveKind::String:
						return L"string";
					default:
						CHECK_FAIL(L"Internal error: Unknown primitive kind.");
					}
				}

				void AppendUnknownTypePrimitiveSchema(WString& dts)
				{
					const Pair<const wchar_t*, DtsPrimitiveKind> primitiveNames[] =
					{
						{ L"UInt8", DtsPrimitiveKind::Number },
						{ L"UInt16", DtsPrimitiveKind::Number },
						{ L"UInt32", DtsPrimitiveKind::Number },
						{ L"UInt64", DtsPrimitiveKind::Number },
						{ L"Int8", DtsPrimitiveKind::Number },
						{ L"Int16", DtsPrimitiveKind::Number },
						{ L"Int32", DtsPrimitiveKind::Number },
						{ L"Int64", DtsPrimitiveKind::Number },
						{ L"Single", DtsPrimitiveKind::Number },
						{ L"Double", DtsPrimitiveKind::Number },
						{ L"Char", DtsPrimitiveKind::String },
						{ L"DateTime", DtsPrimitiveKind::String },
						{ L"Locale", DtsPrimitiveKind::String },
					};

					dts += L"export type UnknownType_PrimitiveSchema =\r\n";
					for (auto&& [name, kind] : primitiveNames)
					{
						dts += L"  | [" + EscapeDtsString(WString::Unmanaged(name)) + L", " + WString::Unmanaged(GetDtsPrimitiveTypeScriptType(kind)) + L"]\r\n";
					}
					dts += L"  | null\r\n";
					dts += L"  | true\r\n";
					dts += L"  | false\r\n";
					dts += L"  | string\r\n";
					dts += L"  ;\r\n\r\n";
				}

				void AppendTypeListEnum(WString& dts, const List<DtsEnumModel>& enums)
				{
					if (enums.Count() == 0)
					{
						dts += L"export type TypeList_Enum = never;\r\n\r\n";
						return;
					}

					dts += L"export type TypeList_Enum =\r\n";
					for (auto&& model : enums)
					{
						dts += L"  | " + EscapeDtsString(model.fullName) + L"\r\n";
					}
					dts += L"  ;\r\n\r\n";
				}

				void AppendUnknownTypeEnumSchema(WString& dts)
				{
					dts += L"export type UnknownType_EnumSchema = [TypeList_Enum, number];\r\n\r\n";
				}

				void AppendUnknownTypeList(WString& dts)
				{
					dts += L"export interface UnknownType_List\r\n";
					dts += L"{\r\n";
					dts += L"  " + EscapeDtsString(L"$") + L": " + EscapeDtsString(L"list") + L" | " + EscapeDtsString(L"oblist") + L";\r\n";
					dts += L"  values: UnknownTypeSchema[];\r\n";
					dts += L"}\r\n\r\n";
				}

				void AppendUnknownTypeMap(WString& dts)
				{
					dts += L"export interface UnknownType_Map\r\n";
					dts += L"{\r\n";
					dts += L"  " + EscapeDtsString(L"$") + L": " + EscapeDtsString(L"map") + L";\r\n";
					dts += L"  values: [UnknownTypeSchema, UnknownTypeSchema][];\r\n";
					dts += L"}\r\n\r\n";
				}

				void AppendUnknownTypeSchemaUnion(WString& dts, const List<DtsStructModel>& structs)
				{
					dts += L"export type UnknownTypeSchema =\r\n";
					dts += L"  | UnknownType_PrimitiveSchema\r\n";
					dts += L"  | UnknownType_EnumSchema\r\n";
					dts += L"  | UnknownType_List\r\n";
					dts += L"  | UnknownType_Map\r\n";
					for (auto&& model : structs)
					{
						dts += L"  | " + MakeUnknownStructDtsTypeName(model.fullName) + L"\r\n";
					}
					dts += L"  ;\r\n\r\n";
				}

				void AppendUnknownStructSchemas(WString& dts, DtsContext& context)
				{
					for (auto&& model : *context.structs)
					{
						dts += L"export interface " + MakeUnknownStructDtsTypeName(model.fullName) + L" extends " + MakeDtsTypeName(model.fullName) + L"\r\n";
						dts += L"{\r\n";
						dts += L"  " + EscapeDtsString(L"$") + L": " + EscapeDtsString(model.fullName) + L";\r\n";
						dts += L"}\r\n\r\n";
					}
				}

				void AppendKnownEnums(WString& dts, const List<DtsEnumModel>& enums)
				{
					for (auto&& model : enums)
					{
						dts += L"export enum " + MakeDtsTypeName(model.fullName) + L"\r\n";
						dts += L"{\r\n";
						for (auto&& item : *model.items.Obj())
						{
							dts += L"  " + GetDtsPropertyName(item.name) + L" = " + u64tow(item.value) + L",\r\n";
						}
						dts += L"}\r\n\r\n";
					}
				}

				void AppendKnownStructs(WString& dts, DtsContext& context)
				{
					for (auto&& model : *context.structs)
					{
						dts += L"export interface " + MakeDtsTypeName(model.fullName) + L"\r\n";
						dts += L"{\r\n";
						for (auto&& field : *model.fields.Obj())
						{
							dts += L"  " + GetDtsPropertyName(field.name) + L": " + GetDtsTypeFromType(context, field.type.Obj()) + L";\r\n";
						}
						dts += L"}\r\n\r\n";
					}
				}

				void AppendKnownTypeSchema(WString& dts, const List<DtsStructModel>& structs)
				{
					dts += L"// All enum_type_full_name is omitted because in known type enums are just numbers\r\n";
					dts += L"export type KnownTypeSchema =\r\n";
					dts += L"  | number\r\n";
					dts += L"  | true\r\n";
					dts += L"  | false\r\n";
					dts += L"  | string\r\n";
					dts += L"  | KnownTypeSchema[]\r\n";
					dts += L"  | [KnownTypeSchema, KnownTypeSchema][]\r\n";
					for (auto&& model : structs)
					{
						dts += L"  | " + MakeDtsTypeName(model.fullName) + L"\r\n";
					}
					dts += L"  ;\r\n";
				}
			}

			WString GenerateDtsFromRpcMetadata(WfLexicalScopeManager* manager)
			{
				using namespace rpc_dts_generating;

				WString dts;
				if (!manager || !manager->rpcMetadata || !manager->rpcMetadata->metadataModule)
				{
					return dts;
				}

				List<DtsEnumModel> enums;
				List<DtsStructModel> structs;
				AddDtsTypesFromDeclarations(manager->rpcMetadata->metadataModule->declarations, L"", enums, structs);
				AddRpcObjectReferenceDtsStruct(structs);

				DtsContext context;
				context.manager = manager;
				context.enums = &enums;
				context.structs = &structs;

				AppendUnknownTypePrimitiveSchema(dts);
				AppendTypeListEnum(dts, enums);
				AppendUnknownTypeEnumSchema(dts);
				AppendUnknownTypeList(dts);
				AppendUnknownTypeMap(dts);
				AppendUnknownTypeSchemaUnion(dts, structs);
				AppendUnknownStructSchemas(dts, context);
				dts += L"// below are all known types\r\n\r\n";
				AppendKnownEnums(dts, enums);
				AppendKnownStructs(dts, context);
				AppendKnownTypeSchema(dts, structs);
				return dts;
			}
		}
	}
}
