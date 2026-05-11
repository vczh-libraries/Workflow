#include "WfAnalyzer.h"
#include "WfAnalyzer_GenerateRpc.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;

			namespace rpc_generating
			{
				using namespace reflection::description;

				WString MangleRpcFullName(const WString& fullName);
				Ptr<WfType> CopyType(WfType* type);
				Ptr<WfType> CreatePredefinedType(WfPredefinedTypeName name);
				Ptr<WfType> CreateQualifiedType(const WString& fullName);
				Ptr<WfType> CreateTopQualifiedType(const WString& fullName);
				Ptr<WfType> CreateRawType(const WString& fullName);
				Ptr<WfType> CreateSharedType(const WString& fullName);
				Ptr<WfType> CreateNullableType(const WString& fullName);
				Ptr<WfType> CreateMapType(Ptr<WfType> keyType, Ptr<WfType> valueType);
				Ptr<WfExpression> CreateNull();
				Ptr<WfExpression> CreateIsNull(Ptr<WfExpression> expression);
				Ptr<WfExpression> CreateIsNotNull(Ptr<WfExpression> expression);
				Ptr<WfExpression> CreateBool(bool value);
				Ptr<WfExpression> CreateInt(vint value);
				Ptr<WfExpression> CreateString(const WString& value);
				Ptr<WfExpression> CreateReference(const WString& name);
				Ptr<WfExpression> CreateQualifiedExpression(const WString& fullName);
				Ptr<WfExpression> CreateMember(Ptr<WfExpression> parent, const WString& name);
				Ptr<WfExpression> CreateBinary(WfBinaryOperator op, Ptr<WfExpression> first, Ptr<WfExpression> second);
				Ptr<WfExpression> CreateAssign(Ptr<WfExpression> left, Ptr<WfExpression> right);
				Ptr<WfExpression> CreateIndex(Ptr<WfExpression> collection, Ptr<WfExpression> index);
				Ptr<WfExpression> CreateCast(Ptr<WfType> type, Ptr<WfExpression> expression);
				Ptr<WfExpression> CreateWeakCast(Ptr<WfType> type, Ptr<WfExpression> expression);
				Ptr<WfExpression> CreateInfer(Ptr<WfExpression> expression, Ptr<WfType> type);

				Ptr<WfConstructorArgument> CreateConstructorArgument(Ptr<WfExpression> key, Ptr<WfExpression> value);
				Ptr<WfConstructorExpression> CreateConstructor();
				Ptr<WfExpression> CreateRpcExceptionExpression(Ptr<WfExpression> message);
				Ptr<WfType> CreateRpcEventExceptionMapType();
				Ptr<WfExpression> CreateNewClass(Ptr<WfType> type);
				Ptr<WfExpression> CreateNewInterface(Ptr<WfType> type);
				Ptr<WfStatement> CreateExpressionStatement(Ptr<WfExpression> expression);
				Ptr<WfStatement> CreateReturn(Ptr<WfExpression> expression);
				Ptr<WfStatement> CreateRaise(const WString& message);
				Ptr<WfStatement> CreateRaise(Ptr<WfExpression> expression);
				Ptr<WfStatement> CreateTry(Ptr<WfStatement> protectedStatement, const WString& name, Ptr<WfStatement> catchStatement, Ptr<WfStatement> finallyStatement);
				Ptr<WfStatement> CreateTryCatch(Ptr<WfStatement> protectedStatement, const WString& name, Ptr<WfStatement> catchStatement);
				Ptr<WfVariableDeclaration> CreateVariableDeclaration(const WString& name, Ptr<WfType> type, Ptr<WfExpression> expression);
				Ptr<WfStatement> CreateVariableStatement(const WString& name, Ptr<WfType> type, Ptr<WfExpression> expression);
				Ptr<WfStatement> CreateInferredVariableStatement(const WString& name, Ptr<WfExpression> expression);
				Ptr<WfForEachStatement> CreateForEach(const WString& name, Ptr<WfExpression> collection, Ptr<WfStatement> body);
				Ptr<WfBlockStatement> CreateBlock();
				void AddStatement(Ptr<WfBlockStatement> block, Ptr<WfStatement> statement);
				void AddRpcMethodExceptionRaise(Ptr<WfBlockStatement> block, Ptr<WfExpression> value);
				void AddRpcEventExceptionMapSet(Ptr<WfBlockStatement> block, const WString& mapName, Ptr<WfExpression> clientId, Ptr<WfExpression> message);
				void AddRpcEventExceptionRaise(Ptr<WfBlockStatement> block, Ptr<WfExpression> value);
				Ptr<WfFunctionArgument> CreateFunctionArgument(const WString& name, Ptr<WfType> type);
				void AddSwitchCase(Ptr<WfSwitchStatement> switchStat, Ptr<WfExpression> expression, Ptr<WfStatement> statement);
				bool IsSharedInterfaceType(ITypeInfo* type);
				bool IsVoidType(WfType* type);
				bool IsRpcByvalReturn(const RpcMethodModel& methodModel);
				Ptr<WfExpression> CreateRpcBoxExpression(ITypeInfo* typeInfo, bool byref, Ptr<WfExpression> value, Ptr<WfExpression> lifecycle);
				Ptr<WfExpression> CreateRpcUnboxExpression(ITypeInfo* typeInfo, Ptr<WfType> type, bool byref, Ptr<WfExpression> value, Ptr<WfExpression> lifecycle);
				Ptr<WfExpression> CreateRpcCopyByvalExpression(Ptr<WfExpression> value, Ptr<WfExpression> lifecycle);
				void AddRpcByvalReturnValue(Ptr<WfBlockStatement> block, Ptr<WfExpression> value, Ptr<WfExpression> copiedValue);
				Ptr<WfExpression> CreateRpcConstantReference(const wchar_t* prefix, const WString& fullName);
				void CollectMangledNames(WfLexicalScopeManager* manager);
				List<RpcInterfaceModel> BuildInterfaceModels(WfLexicalScopeManager* manager);
				bool HasRpcEvents(const List<RpcInterfaceModel>& interfaces);
				Ptr<WfStatement> BuildRegisterService();
				WString GetRpcOpsInterfaceName(const WString& assemblyName);
				WString GetRpcOpsInvokeMethodName(const RpcMethodModel& methodModel);
				WString GetRpcOpsInvokeEventName(const RpcEventModel& eventModel);
				WString GetRpcOpsArgumentName(const RpcParamModel& paramModel);
				Ptr<WfFunctionDeclaration> CreateRpcOpsFunctionDeclaration(const WString& name, Ptr<WfType> returnType, WfFunctionKind kind);
				void AddRpcOpsFunctionArguments(Ptr<WfFunctionDeclaration> functionDecl, const List<RpcParamModel>& params);
				Ptr<WfExpression> CreateRpcOpsObjectOps();
				Ptr<WfExpression> CreateRpcOpsObjectInvoke(const RpcMethodModel& methodModel, Ptr<WfExpression> objectOps);
				Ptr<WfExpression> CreateRpcOpsObjectInvoke(const RpcMethodModel& methodModel);
				Ptr<WfExpression> CreateRpcOpsObjectEventInvoke(const RpcEventModel& eventModel);

				enum class RpcJsonPrimitiveKind
				{
					Number,
					Boolean,
					String,
				};

				struct RpcJsonPrimitiveModel
				{
					WString						fullName;
					WString						keyword;
					RpcJsonPrimitiveKind		kind = RpcJsonPrimitiveKind::Number;
				};

				struct RpcJsonFieldModel
				{
					WString						name;
					Ptr<WfType>					type;
				};

				struct RpcJsonTypeModel
				{
					WString						fullName;
					Ptr<WfType>					type;
					Ptr<List<RpcJsonFieldModel>>	fields;
				};

				Ptr<WfExpression> CreateJsonArrayItem(const WString& arrayName, vint index)
				{
					return CreateIndex(CreateMember(CreateReference(arrayName), L"items"), CreateInt(index));
				}

				Ptr<WfExpression> CreateJsonArrayItemContent(const WString& arrayName, vint index, const WString& jsonType)
				{
					return CreateMember(CreateMember(CreateCast(CreateSharedType(jsonType), CreateJsonArrayItem(arrayName, index)), L"content"), L"value");
				}

				void AddRpcJsonTypesFromDeclarations(
					const List<Ptr<WfDeclaration>>& declarations,
					const WString& prefix,
					List<RpcJsonTypeModel>& enums,
					List<RpcJsonTypeModel>& structs)
				{
					for (auto declaration : declarations)
					{
						auto fullName = prefix == L"" ? declaration->name.value : prefix + L"::" + declaration->name.value;
						if (auto namespaceDecl = declaration.Cast<WfNamespaceDeclaration>())
						{
							AddRpcJsonTypesFromDeclarations(namespaceDecl->declarations, fullName, enums, structs);
						}
						else if (declaration.Cast<WfEnumDeclaration>())
						{
							RpcJsonTypeModel model;
							model.fullName = fullName;
							model.type = CreateTopQualifiedType(fullName);
							enums.Add(model);
						}
						else if (auto structDecl = declaration.Cast<WfStructDeclaration>())
						{
							RpcJsonTypeModel model;
							model.fullName = fullName;
							model.type = CreateTopQualifiedType(fullName);
							model.fields = Ptr(new List<RpcJsonFieldModel>);
							for (auto member : structDecl->members)
							{
								RpcJsonFieldModel field;
								field.name = member->name.value;
								field.type = CopyType(member->type.Obj());
								model.fields->Add(field);
							}
							structs.Add(model);
						}
					}
				}

				void AddRpcBuiltinJsonType(List<RpcJsonTypeModel>& structs, const WString& fullName, ITypeDescriptor* td)
				{
					for (auto&& model : structs)
					{
						if (model.fullName == fullName)
						{
							return;
						}
					}

					if (!td)
					{
						return;
					}

					RpcJsonTypeModel model;
					model.fullName = fullName;
					model.type = CreateTopQualifiedType(fullName);
					model.fields = Ptr(new List<RpcJsonFieldModel>);
					for (vint i = 0; i < td->GetPropertyCount(); i++)
					{
						auto propertyInfo = td->GetProperty(i);
						RpcJsonFieldModel field;
						field.name = propertyInfo->GetName();
						field.type = GetTypeFromTypeInfo(propertyInfo->GetReturn());
						model.fields->Add(field);
					}
					structs.Add(model);
				}

				void CollectRpcJsonTypes(WfLexicalScopeManager* manager, List<RpcJsonTypeModel>& enums, List<RpcJsonTypeModel>& structs)
				{
					if (manager->rpcMetadata && manager->rpcMetadata->metadataModule)
					{
						AddRpcJsonTypesFromDeclarations(manager->rpcMetadata->metadataModule->declarations, L"", enums, structs);
					}
				}

				void CollectRpcJsonPrimitives(List<RpcJsonPrimitiveModel>& primitives)
				{
					auto add = [&](const wchar_t* fullName, const wchar_t* keyword, RpcJsonPrimitiveKind kind)
					{
						RpcJsonPrimitiveModel model;
						model.fullName = WString::Unmanaged(fullName);
						model.keyword = WString::Unmanaged(keyword);
						model.kind = kind;
						primitives.Add(model);
					};

					add(L"system::UInt8", L"UInt8", RpcJsonPrimitiveKind::Number);
					add(L"system::UInt16", L"UInt16", RpcJsonPrimitiveKind::Number);
					add(L"system::UInt32", L"UInt32", RpcJsonPrimitiveKind::Number);
					add(L"system::UInt64", L"UInt64", RpcJsonPrimitiveKind::Number);
					add(L"system::Int8", L"Int8", RpcJsonPrimitiveKind::Number);
					add(L"system::Int16", L"Int16", RpcJsonPrimitiveKind::Number);
					add(L"system::Int32", L"Int32", RpcJsonPrimitiveKind::Number);
					add(L"system::Int64", L"Int64", RpcJsonPrimitiveKind::Number);
					add(L"system::Single", L"Single", RpcJsonPrimitiveKind::Number);
					add(L"system::Double", L"Double", RpcJsonPrimitiveKind::Number);
					add(L"system::Boolean", L"Boolean", RpcJsonPrimitiveKind::Boolean);
					add(L"system::Char", L"Char", RpcJsonPrimitiveKind::String);
					add(L"system::String", L"String", RpcJsonPrimitiveKind::String);
					add(L"system::DateTime", L"DateTime", RpcJsonPrimitiveKind::String);
					add(L"system::Locale", L"Locale", RpcJsonPrimitiveKind::String);
				}

				Ptr<WfStatement> CreateWhile(Ptr<WfExpression> condition, Ptr<WfStatement> body)
				{
					auto statement = Ptr(new WfWhileStatement);
					statement->condition = condition;
					statement->statement = body;
					return statement;
				}

				Ptr<WfType> CreateRpcJsonSerializeCallbackType()
				{
					auto type = Ptr(new WfFunctionType);
					type->result = CreateSharedType(L"system::JsonNode");
					type->arguments.Add(CreatePredefinedType(WfPredefinedTypeName::Object));
					return type;
				}

				Ptr<WfType> CreateRpcJsonDeserializeCallbackType()
				{
					auto type = Ptr(new WfFunctionType);
					type->result = CreatePredefinedType(WfPredefinedTypeName::Object);
					type->arguments.Add(CreateSharedType(L"system::JsonNode"));
					return type;
				}

				WString GetRpcJsonSerializeEnumFunctionName(const WString& fullName)
				{
					return L"rpcjson_Serialize_Enum_" + MangleRpcFullName(fullName);
				}

				WString GetRpcJsonDeserializeEnumFunctionName(const WString& fullName)
				{
					return L"rpcjson_Deserialize_Enum_" + MangleRpcFullName(fullName);
				}

				WString GetRpcJsonSerializeStructFunctionName(const WString& fullName)
				{
					return L"rpcjson_Serialize_Struct_" + MangleRpcFullName(fullName);
				}

				WString GetRpcJsonDeserializeStructFunctionName(const WString& fullName)
				{
					return L"rpcjson_Deserialize_Struct_" + MangleRpcFullName(fullName);
				}

				WString GetRpcJsonPredefinedFullName(WfLexicalScopeManager* manager, WfPredefinedTypeName name)
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

				WString GetRpcJsonTypeFullName(WfLexicalScopeManager* manager, WfType* type)
				{
					if (!type) return L"";
					if (auto predefined = dynamic_cast<WfPredefinedType*>(type))
					{
						return GetRpcJsonPredefinedFullName(manager, predefined->name);
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
						auto parent = GetRpcJsonTypeFullName(manager, child->parent.Obj());
						return parent == L"" ? child->name.value : parent + L"::" + child->name.value;
					}
					if (auto raw = dynamic_cast<WfRawPointerType*>(type))
					{
						return GetRpcJsonTypeFullName(manager, raw->element.Obj());
					}
					if (auto shared = dynamic_cast<WfSharedPointerType*>(type))
					{
						return GetRpcJsonTypeFullName(manager, shared->element.Obj());
					}
					if (auto nullable = dynamic_cast<WfNullableType*>(type))
					{
						return GetRpcJsonTypeFullName(manager, nullable->element.Obj());
					}
					return L"";
				}

				RpcJsonPrimitiveModel* FindRpcJsonPrimitive(List<RpcJsonPrimitiveModel>& primitives, const WString& fullName)
				{
					for (vint i = 0; i < primitives.Count(); i++)
					{
						if (primitives[i].fullName == fullName)
						{
							return &primitives[i];
						}
					}
					return nullptr;
				}

				const RpcJsonTypeModel* FindRpcJsonType(const List<RpcJsonTypeModel>& types, const WString& fullName)
				{
					for (vint i = 0; i < types.Count(); i++)
					{
						if (types[i].fullName == fullName)
						{
							return &types[i];
						}
					}
					return nullptr;
				}

				struct RpcJsonGenerationContext
				{
					WfLexicalScopeManager*			manager = nullptr;
					List<RpcJsonPrimitiveModel>*	primitives = nullptr;
					List<RpcJsonTypeModel>*		enums = nullptr;
					List<RpcJsonTypeModel>*		structs = nullptr;
					vint						tempIndex = 0;
				};

				WString AllocateRpcJsonTemp(RpcJsonGenerationContext& context, const wchar_t* prefix)
				{
					return WString::Unmanaged(prefix) + itow(context.tempIndex++);
				}

				Ptr<WfExpression> CreateRpcJsonToken(Ptr<WfExpression> value)
				{
					auto constructor = CreateConstructor();
					constructor->arguments.Add(CreateConstructorArgument(CreateReference(L"value"), value));
					return constructor;
				}

				WString AddRpcJsonLiteral(Ptr<WfBlockStatement> block, RpcJsonGenerationContext& context, const WString& literal)
				{
					auto nodeName = AllocateRpcJsonTemp(context, L"jsonLiteral");
					AddStatement(block, CreateInferredVariableStatement(nodeName, CreateNewClass(CreateSharedType(L"system::JsonLiteral"))));
					AddStatement(block, CreateExpressionStatement(CreateAssign(CreateMember(CreateReference(nodeName), L"value"), CreateQualifiedExpression(L"system::JsonLiteralValue::" + literal))));
					return nodeName;
				}

				WString AddRpcJsonBooleanLiteral(Ptr<WfBlockStatement> block, RpcJsonGenerationContext& context, Ptr<WfExpression> value)
				{
					auto nodeName = AllocateRpcJsonTemp(context, L"jsonLiteral");
					AddStatement(block, CreateInferredVariableStatement(nodeName, CreateNewClass(CreateSharedType(L"system::JsonLiteral"))));
					AddStatement(block, CreateIf(
						value,
						CreateExpressionStatement(CreateAssign(CreateMember(CreateReference(nodeName), L"value"), CreateQualifiedExpression(L"system::JsonLiteralValue::True"))),
						CreateExpressionStatement(CreateAssign(CreateMember(CreateReference(nodeName), L"value"), CreateQualifiedExpression(L"system::JsonLiteralValue::False")))));
					return nodeName;
				}

				WString AddRpcJsonString(Ptr<WfBlockStatement> block, RpcJsonGenerationContext& context, Ptr<WfExpression> value)
				{
					auto nodeName = AllocateRpcJsonTemp(context, L"jsonString");
					AddStatement(block, CreateInferredVariableStatement(nodeName, CreateNewClass(CreateSharedType(L"system::JsonString"))));
					AddStatement(block, CreateExpressionStatement(CreateAssign(CreateMember(CreateReference(nodeName), L"content"), CreateRpcJsonToken(value))));
					return nodeName;
				}

				WString AddRpcJsonNumber(Ptr<WfBlockStatement> block, RpcJsonGenerationContext& context, Ptr<WfExpression> value)
				{
					auto nodeName = AllocateRpcJsonTemp(context, L"jsonNumber");
					AddStatement(block, CreateInferredVariableStatement(nodeName, CreateNewClass(CreateSharedType(L"system::JsonNumber"))));
					AddStatement(block, CreateExpressionStatement(CreateAssign(CreateMember(CreateReference(nodeName), L"content"), CreateRpcJsonToken(value))));
					return nodeName;
				}

				void AddRpcJsonArrayItem(Ptr<WfBlockStatement> block, const WString& arrayName, Ptr<WfExpression> value)
				{
					AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(CreateMember(CreateReference(arrayName), L"items"), L"Add"), value)));
				}

				void AddRpcJsonObjectField(Ptr<WfBlockStatement> block, RpcJsonGenerationContext& context, Ptr<WfExpression> object, const WString& fieldName, Ptr<WfExpression> value)
				{
					auto fieldVar = AllocateRpcJsonTemp(context, L"jsonField");
					AddStatement(block, CreateInferredVariableStatement(fieldVar, CreateNewClass(CreateSharedType(L"system::JsonObjectField"))));
					AddStatement(block, CreateExpressionStatement(CreateAssign(CreateMember(CreateReference(fieldVar), L"name"), CreateRpcJsonToken(CreateString(fieldName)))));
					AddStatement(block, CreateExpressionStatement(CreateAssign(CreateMember(CreateReference(fieldVar), L"value"), value)));
					AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(CreateMember(object, L"fields"), L"Add"), CreateReference(fieldVar))));
				}

				WString AddRpcJsonObjectFieldLookup(Ptr<WfBlockStatement> block, RpcJsonGenerationContext& context, Ptr<WfExpression> object, const WString& fieldName, bool required = true)
				{
					auto resultVar = AllocateRpcJsonTemp(context, L"jsonFieldValue");
					auto fieldVar = AllocateRpcJsonTemp(context, L"jsonField");
					AddStatement(block, CreateVariableStatement(resultVar, CreateSharedType(L"system::JsonNode"), CreateNull()));
					auto forBlock = CreateBlock();
					AddStatement(forBlock, CreateIf(
						CreateBinary(WfBinaryOperator::EQ, CreateMember(CreateMember(CreateReference(fieldVar), L"name"), L"value"), CreateString(fieldName)),
						CreateExpressionStatement(CreateAssign(CreateReference(resultVar), CreateMember(CreateReference(fieldVar), L"value")))));
					AddStatement(block, CreateForEach(fieldVar, CreateMember(object, L"fields"), forBlock));
					if (required)
					{
						AddStatement(block, CreateIf(CreateIsNull(CreateReference(resultVar)), CreateRaise(L"JSON object field not found: " + fieldName)));
					}
					return resultVar;
				}

				WString AddKnownRpcJsonSerializeValue(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, Ptr<WfExpression> value, WfType* type);
				WString AddKnownRpcJsonDeserializeValue(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, Ptr<WfExpression> node, WfType* type);

				Ptr<WfType> CreateWritableRpcJsonCollectionType(WfType* type)
				{
					if (auto enumerable = dynamic_cast<WfEnumerableType*>(type))
					{
						auto listType = Ptr(new WfMapType);
						listType->writability = WfMapWritability::Writable;
						listType->value = CopyType(enumerable->element.Obj());
						return listType;
					}
					if (auto map = dynamic_cast<WfMapType*>(type))
					{
						auto mapType = Ptr(new WfMapType);
						mapType->writability = WfMapWritability::Writable;
						mapType->key = map->key ? CopyType(map->key.Obj()) : nullptr;
						mapType->value = CopyType(map->value.Obj());
						return mapType;
					}
					if (auto observable = dynamic_cast<WfObservableListType*>(type))
					{
						auto listType = Ptr(new WfObservableListType);
						listType->element = CopyType(observable->element.Obj());
						return listType;
					}
					return CopyType(type);
				}

				Ptr<WfExpression> CreateRpcJsonSerializedArgument(vint index)
				{
					return CreateIndex(CreateReference(L"arguments"), CreateInt(index));
				}

				WString AddUnknownRpcJsonSerializeValue(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, Ptr<WfExpression> value)
				{
					auto valueName = AllocateRpcJsonTemp(context, L"jsonValue");
					auto resultName = AllocateRpcJsonTemp(context, L"jsonNode");
					AddStatement(block, CreateInferredVariableStatement(valueName, value));
					AddStatement(block, CreateInferredVariableStatement(resultName, CreateCall(CreateReference(L"rpcjson_Serialize"), CreateReference(valueName))));
					return resultName;
				}

				WString AddKnownRpcJsonSerializeValue(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, Ptr<WfExpression> value, WfType* type)
				{
					if (auto nullable = dynamic_cast<WfNullableType*>(type))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonNode");
						AddStatement(block, CreateVariableStatement(resultName, CreateSharedType(L"system::JsonNode"), CreateNull()));
						auto nullBranch = CreateBlock();
						auto nullNode = AddRpcJsonLiteral(nullBranch, context, L"Null");
						AddStatement(nullBranch, CreateExpressionStatement(CreateAssign(CreateReference(resultName), CreateReference(nullNode))));
						auto valueBranch = CreateBlock();
						auto valueNode = AddKnownRpcJsonSerializeValue(context, valueBranch, CreateCast(CopyType(nullable->element.Obj()), CopyExpression(value, true)), nullable->element.Obj());
						AddStatement(valueBranch, CreateExpressionStatement(CreateAssign(CreateReference(resultName), CreateReference(valueNode))));
						AddStatement(block, CreateIf(CreateIsNull(value), nullBranch, valueBranch));
						return resultName;
					}

					auto fullName = GetRpcJsonTypeFullName(context.manager, type);
					if (auto primitive = FindRpcJsonPrimitive(*context.primitives, fullName))
					{
						switch (primitive->kind)
						{
						case RpcJsonPrimitiveKind::Number:
							return AddRpcJsonNumber(block, context, CreateCast(CreatePredefinedType(WfPredefinedTypeName::String), value));
						case RpcJsonPrimitiveKind::Boolean:
							return AddRpcJsonBooleanLiteral(block, context, value);
						case RpcJsonPrimitiveKind::String:
							return AddRpcJsonString(block, context, CreateCast(CreatePredefinedType(WfPredefinedTypeName::String), value));
						default:
							CHECK_FAIL(L"Internal error: Unknown primitive kind.");
						}
					}

					if (fullName == L"system::JsonNode")
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonNode");
						AddStatement(block, CreateInferredVariableStatement(resultName, value));
						return resultName;
					}

					if (fullName == L"system::Object" || fullName == L"system::Interface")
					{
						return AddUnknownRpcJsonSerializeValue(context, block, value);
					}

					if (FindRpcJsonType(*context.enums, fullName))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonNode");
						AddStatement(block, CreateInferredVariableStatement(resultName, CreateCall(CreateReference(GetRpcJsonSerializeEnumFunctionName(fullName)), value)));
						return resultName;
					}

					if (FindRpcJsonType(*context.structs, fullName))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonNode");
						AddStatement(block, CreateInferredVariableStatement(resultName, CreateCall(CreateReference(GetRpcJsonSerializeStructFunctionName(fullName)), value)));
						return resultName;
					}

					if (auto enumerable = dynamic_cast<WfEnumerableType*>(type))
					{
						auto arrayName = AllocateRpcJsonTemp(context, L"jsonArray");
						AddStatement(block, CreateInferredVariableStatement(arrayName, CreateNewClass(CreateSharedType(L"system::JsonArray"))));
						auto forBlock = CreateBlock();
						auto itemNode = AddKnownRpcJsonSerializeValue(context, forBlock, CreateReference(L"item"), enumerable->element.Obj());
						AddRpcJsonArrayItem(forBlock, arrayName, CreateReference(itemNode));
						AddStatement(block, CreateForEach(L"item", value, forBlock));
						return arrayName;
					}

					if (auto map = dynamic_cast<WfMapType*>(type))
					{
						auto arrayName = AllocateRpcJsonTemp(context, L"jsonArray");
						AddStatement(block, CreateInferredVariableStatement(arrayName, CreateNewClass(CreateSharedType(L"system::JsonArray"))));
						auto forBlock = CreateBlock();
						if (map->key)
						{
							auto pairName = AllocateRpcJsonTemp(context, L"jsonArray");
							AddStatement(forBlock, CreateInferredVariableStatement(pairName, CreateNewClass(CreateSharedType(L"system::JsonArray"))));
							auto keyNode = AddKnownRpcJsonSerializeValue(context, forBlock, CreateReference(L"key"), map->key.Obj());
							AddRpcJsonArrayItem(forBlock, pairName, CreateReference(keyNode));
							auto valueNode = AddKnownRpcJsonSerializeValue(context, forBlock, CreateIndex(value, CreateReference(L"key")), map->value.Obj());
							AddRpcJsonArrayItem(forBlock, pairName, CreateReference(valueNode));
							AddRpcJsonArrayItem(forBlock, arrayName, CreateReference(pairName));
							AddStatement(block, CreateForEach(L"key", CreateMember(value, L"Keys"), forBlock));
						}
						else
						{
							auto itemNode = AddKnownRpcJsonSerializeValue(context, forBlock, CreateReference(L"item"), map->value.Obj());
							AddRpcJsonArrayItem(forBlock, arrayName, CreateReference(itemNode));
							AddStatement(block, CreateForEach(L"item", value, forBlock));
						}
						return arrayName;
					}

					if (auto observable = dynamic_cast<WfObservableListType*>(type))
					{
						auto arrayName = AllocateRpcJsonTemp(context, L"jsonArray");
						AddStatement(block, CreateInferredVariableStatement(arrayName, CreateNewClass(CreateSharedType(L"system::JsonArray"))));
						auto forBlock = CreateBlock();
						auto itemNode = AddKnownRpcJsonSerializeValue(context, forBlock, CreateReference(L"item"), observable->element.Obj());
						AddRpcJsonArrayItem(forBlock, arrayName, CreateReference(itemNode));
						AddStatement(block, CreateForEach(L"item", value, forBlock));
						return arrayName;
					}

					return AddUnknownRpcJsonSerializeValue(context, block, value);
				}

				WString AddKnownRpcJsonDeserializeValue(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, Ptr<WfExpression> node, WfType* type)
				{
					if (auto nullable = dynamic_cast<WfNullableType*>(type))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
						auto literalName = AllocateRpcJsonTemp(context, L"jsonLiteral");
						AddStatement(block, CreateVariableStatement(resultName, CopyType(type), CreateNull()));
						AddStatement(block, CreateInferredVariableStatement(literalName, CreateWeakCast(CreateSharedType(L"system::JsonLiteral"), CopyExpression(node, true))));
						auto assignBranch = CreateBlock();
						auto valueName = AddKnownRpcJsonDeserializeValue(context, assignBranch, node, nullable->element.Obj());
						AddStatement(assignBranch, CreateExpressionStatement(CreateAssign(CreateReference(resultName), CreateReference(valueName))));
						AddStatement(block, CreateIf(
							CreateBinary(
								WfBinaryOperator::Or,
								CreateIsNull(CreateReference(literalName)),
								CreateBinary(WfBinaryOperator::NE, CreateMember(CreateReference(literalName), L"value"), CreateQualifiedExpression(L"system::JsonLiteralValue::Null"))),
							assignBranch));
						return resultName;
					}

					auto fullName = GetRpcJsonTypeFullName(context.manager, type);
					if (auto primitive = FindRpcJsonPrimitive(*context.primitives, fullName))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
						auto deserialized = CreateCall(CreateReference(L"rpcjson_Deserialize"), node);
						auto value =
							primitive->kind == RpcJsonPrimitiveKind::Boolean || fullName == L"system::String"
							? CreateCast(CreateTopQualifiedType(fullName), deserialized)
							: CreateCast(CreateTopQualifiedType(fullName), CreateCast(CreatePredefinedType(WfPredefinedTypeName::String), deserialized));
						AddStatement(block, CreateInferredVariableStatement(resultName, value));
						return resultName;
					}

					if (fullName == L"system::JsonNode")
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
						AddStatement(block, CreateInferredVariableStatement(resultName, node));
						return resultName;
					}

					if (fullName == L"system::Object" || fullName == L"system::Interface")
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
						AddStatement(block, CreateInferredVariableStatement(resultName, CreateCall(CreateReference(L"rpcjson_Deserialize"), node)));
						return resultName;
					}

					if (FindRpcJsonType(*context.enums, fullName))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
						AddStatement(block, CreateInferredVariableStatement(resultName, CreateCall(CreateReference(GetRpcJsonDeserializeEnumFunctionName(fullName)), node)));
						return resultName;
					}

					if (FindRpcJsonType(*context.structs, fullName))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
						AddStatement(block, CreateInferredVariableStatement(resultName, CreateCall(CreateReference(GetRpcJsonDeserializeStructFunctionName(fullName)), node)));
						return resultName;
					}

					if (auto enumerable = dynamic_cast<WfEnumerableType*>(type))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
						auto arrayName = AllocateRpcJsonTemp(context, L"jsonArray");
						AddStatement(block, CreateVariableStatement(resultName, CreateWritableRpcJsonCollectionType(type), CreateConstructor()));
						AddStatement(block, CreateInferredVariableStatement(arrayName, CreateCast(CreateSharedType(L"system::JsonArray"), node)));
						auto forBlock = CreateBlock();
						auto itemValue = AddKnownRpcJsonDeserializeValue(context, forBlock, CreateReference(L"item"), enumerable->element.Obj());
						AddStatement(forBlock, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(resultName), L"Add"), CreateReference(itemValue))));
						AddStatement(block, CreateForEach(L"item", CreateMember(CreateReference(arrayName), L"items"), forBlock));
						return resultName;
					}

					if (auto map = dynamic_cast<WfMapType*>(type))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
						auto arrayName = AllocateRpcJsonTemp(context, L"jsonArray");
						AddStatement(block, CreateVariableStatement(resultName, CreateWritableRpcJsonCollectionType(type), CreateConstructor()));
						AddStatement(block, CreateInferredVariableStatement(arrayName, CreateCast(CreateSharedType(L"system::JsonArray"), node)));
						auto forBlock = CreateBlock();
						if (map->key)
						{
							auto pairName = AllocateRpcJsonTemp(context, L"jsonArray");
							AddStatement(forBlock, CreateInferredVariableStatement(pairName, CreateCast(CreateSharedType(L"system::JsonArray"), CreateReference(L"item"))));
							auto keyValue = AddKnownRpcJsonDeserializeValue(context, forBlock, CreateJsonArrayItem(pairName, 0), map->key.Obj());
							auto itemValue = AddKnownRpcJsonDeserializeValue(context, forBlock, CreateJsonArrayItem(pairName, 1), map->value.Obj());
							AddStatement(forBlock, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(resultName), L"Set"), CreateReference(keyValue), CreateReference(itemValue))));
						}
						else
						{
							auto itemValue = AddKnownRpcJsonDeserializeValue(context, forBlock, CreateReference(L"item"), map->value.Obj());
							AddStatement(forBlock, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(resultName), L"Add"), CreateReference(itemValue))));
						}
						AddStatement(block, CreateForEach(L"item", CreateMember(CreateReference(arrayName), L"items"), forBlock));
						return resultName;
					}

					if (auto observable = dynamic_cast<WfObservableListType*>(type))
					{
						auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
						auto arrayName = AllocateRpcJsonTemp(context, L"jsonArray");
						AddStatement(block, CreateVariableStatement(resultName, CreateWritableRpcJsonCollectionType(type), CreateConstructor()));
						AddStatement(block, CreateInferredVariableStatement(arrayName, CreateCast(CreateSharedType(L"system::JsonArray"), node)));
						auto forBlock = CreateBlock();
						auto itemValue = AddKnownRpcJsonDeserializeValue(context, forBlock, CreateReference(L"item"), observable->element.Obj());
						AddStatement(forBlock, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(resultName), L"Add"), CreateReference(itemValue))));
						AddStatement(block, CreateForEach(L"item", CreateMember(CreateReference(arrayName), L"items"), forBlock));
						return resultName;
					}

					auto resultName = AllocateRpcJsonTemp(context, L"jsonValue");
					AddStatement(block, CreateInferredVariableStatement(resultName, CreateCast(CopyType(type), CreateCall(CreateReference(L"rpcjson_Deserialize"), node))));
					return resultName;
				}

				void AddRpcJsonUnknownTupleReturn(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, const WString& keyword, Ptr<WfExpression> valueNode)
				{
					auto arrayName = AllocateRpcJsonTemp(context, L"jsonArray");
					auto keywordNode = AddRpcJsonString(block, context, CreateString(keyword));
					AddStatement(block, CreateInferredVariableStatement(arrayName, CreateNewClass(CreateSharedType(L"system::JsonArray"))));
					AddRpcJsonArrayItem(block, arrayName, CreateReference(keywordNode));
					AddRpcJsonArrayItem(block, arrayName, valueNode);
					AddStatement(block, CreateReturn(CreateReference(arrayName)));
				}

				Ptr<WfDeclaration> GenerateRpcJsonSerializeEnum(RpcJsonGenerationContext& context, const RpcJsonTypeModel& enumModel)
				{
					auto functionDecl = CreateFunctionDeclaration(GetRpcJsonSerializeEnumFunctionName(enumModel.fullName), CreateSharedType(L"system::JsonNode"), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"value", CopyType(enumModel.type.Obj())));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();
					auto nodeName = AddRpcJsonNumber(block, context, CreateCast(
						CreatePredefinedType(WfPredefinedTypeName::String),
						CreateCast(CreateQualifiedType(L"system::UInt64"), CreateReference(L"value"))));
					AddStatement(block, CreateReturn(CreateReference(nodeName)));
					return functionDecl;
				}

				Ptr<WfDeclaration> GenerateRpcJsonDeserializeEnum(RpcJsonGenerationContext& context, const RpcJsonTypeModel& enumModel)
				{
					auto functionDecl = CreateFunctionDeclaration(GetRpcJsonDeserializeEnumFunctionName(enumModel.fullName), CopyType(enumModel.type.Obj()), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"node", CreateSharedType(L"system::JsonNode")));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();
					AddStatement(block, CreateReturn(CreateCast(
						CopyType(enumModel.type.Obj()),
						CreateCast(CreateQualifiedType(L"system::UInt64"), CreateMember(CreateMember(CreateCast(CreateSharedType(L"system::JsonNumber"), CreateReference(L"node")), L"content"), L"value")))));
					return functionDecl;
				}

				void AddRpcJsonStructFields(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, const RpcJsonTypeModel& structModel, const WString& objectName, const WString& valueName)
				{
					for (auto&& field : *structModel.fields.Obj())
					{
						auto fieldNode = AddKnownRpcJsonSerializeValue(context, block, CreateMember(CreateReference(valueName), field.name), field.type.Obj());
						AddRpcJsonObjectField(block, context, CreateReference(objectName), field.name, CreateReference(fieldNode));
					}
				}

				Ptr<WfDeclaration> GenerateRpcJsonSerializeStruct(RpcJsonGenerationContext& context, const RpcJsonTypeModel& structModel)
				{
					auto functionDecl = CreateFunctionDeclaration(GetRpcJsonSerializeStructFunctionName(structModel.fullName), CreateSharedType(L"system::JsonNode"), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"value", CopyType(structModel.type.Obj())));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();
					AddStatement(block, CreateInferredVariableStatement(L"object", CreateNewClass(CreateSharedType(L"system::JsonObject"))));
					AddRpcJsonStructFields(context, block, structModel, L"object", L"value");
					AddStatement(block, CreateReturn(CreateReference(L"object")));
					return functionDecl;
				}

				Ptr<WfDeclaration> GenerateRpcJsonDeserializeStruct(RpcJsonGenerationContext& context, const RpcJsonTypeModel& structModel)
				{
					auto functionDecl = CreateFunctionDeclaration(GetRpcJsonDeserializeStructFunctionName(structModel.fullName), CopyType(structModel.type.Obj()), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"node", CreateSharedType(L"system::JsonNode")));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();
					AddStatement(block, CreateInferredVariableStatement(L"object", CreateCast(CreateSharedType(L"system::JsonObject"), CreateReference(L"node"))));
					auto constructor = CreateConstructor();
					for (auto&& field : *structModel.fields.Obj())
					{
						auto fieldNode = AddRpcJsonObjectFieldLookup(block, context, CreateReference(L"object"), field.name);
						auto fieldValue = AddKnownRpcJsonDeserializeValue(context, block, CreateReference(fieldNode), field.type.Obj());
						constructor->arguments.Add(CreateConstructorArgument(CreateReference(field.name), CreateReference(fieldValue)));
					}
					AddStatement(block, CreateReturn(constructor));
					return functionDecl;
				}

				void AddUnknownSerializeCollectionCase(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, const WString& varName, const WString& typeFullName, const WString& keyword)
				{
					AddStatement(block, CreateInferredVariableStatement(varName, CreateWeakCast(CreateSharedType(typeFullName), CreateReference(L"value"))));
					auto trueBranch = CreateBlock();
					AddStatement(trueBranch, CreateInferredVariableStatement(L"object", CreateNewClass(CreateSharedType(L"system::JsonObject"))));
					auto keywordNode = AddRpcJsonString(trueBranch, context, CreateString(keyword));
					AddRpcJsonObjectField(trueBranch, context, CreateReference(L"object"), L"$", CreateReference(keywordNode));
					AddStatement(trueBranch, CreateInferredVariableStatement(L"values", CreateNewClass(CreateSharedType(L"system::JsonArray"))));
					auto forBlock = CreateBlock();
					AddRpcJsonArrayItem(forBlock, L"values", CreateCall(CreateReference(L"rpcjson_Serialize"), CreateReference(L"item")));
					AddStatement(trueBranch, CreateForEach(L"item", CreateReference(varName), forBlock));
					AddRpcJsonObjectField(trueBranch, context, CreateReference(L"object"), L"values", CreateReference(L"values"));
					AddStatement(trueBranch, CreateReturn(CreateReference(L"object")));
					AddStatement(block, CreateIf(CreateIsNotNull(CreateReference(varName)), trueBranch));
				}

				void AddUnknownSerializeMapCase(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, const WString& varName, const WString& typeFullName)
				{
					AddStatement(block, CreateInferredVariableStatement(varName, CreateWeakCast(CreateSharedType(typeFullName), CreateReference(L"value"))));
					auto trueBranch = CreateBlock();
					AddStatement(trueBranch, CreateInferredVariableStatement(L"object", CreateNewClass(CreateSharedType(L"system::JsonObject"))));
					auto keywordNode = AddRpcJsonString(trueBranch, context, CreateString(L"map"));
					AddRpcJsonObjectField(trueBranch, context, CreateReference(L"object"), L"$", CreateReference(keywordNode));
					AddStatement(trueBranch, CreateInferredVariableStatement(L"values", CreateNewClass(CreateSharedType(L"system::JsonArray"))));
					auto forBlock = CreateBlock();
					AddRpcJsonArrayItem(forBlock, L"values", CreateCall(CreateReference(L"rpcjson_Serialize"), CreateReference(L"key")));
					AddRpcJsonArrayItem(forBlock, L"values", CreateCall(CreateReference(L"rpcjson_Serialize"), CreateCall(CreateMember(CreateReference(varName), L"Get"), CreateReference(L"key"))));
					AddStatement(trueBranch, CreateForEach(L"key", CreateMember(CreateReference(varName), L"Keys"), forBlock));
					AddRpcJsonObjectField(trueBranch, context, CreateReference(L"object"), L"values", CreateReference(L"values"));
					AddStatement(trueBranch, CreateReturn(CreateReference(L"object")));
					AddStatement(block, CreateIf(CreateIsNotNull(CreateReference(varName)), trueBranch));
				}

				void AddUnknownSerializePrimitiveCase(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, const RpcJsonPrimitiveModel& primitive)
				{
					if (primitive.kind == RpcJsonPrimitiveKind::Boolean || primitive.fullName == L"system::String")
					{
						return;
					}

					auto varName = L"value_" + MangleRpcFullName(primitive.fullName);
					auto nullableType = CreateNullableType(primitive.fullName);
					AddStatement(block, CreateInferredVariableStatement(varName, CreateWeakCast(nullableType, CreateReference(L"value"))));
					auto trueBranch = CreateBlock();
					auto varValue = CreateCast(CreateTopQualifiedType(primitive.fullName), CreateReference(varName));
					WString nodeName;
					switch (primitive.kind)
					{
					case RpcJsonPrimitiveKind::Number:
						nodeName = AddRpcJsonNumber(trueBranch, context, CreateCast(CreatePredefinedType(WfPredefinedTypeName::String), varValue));
						break;
					case RpcJsonPrimitiveKind::String:
						nodeName = AddRpcJsonString(trueBranch, context, CreateCast(CreatePredefinedType(WfPredefinedTypeName::String), varValue));
						break;
					default:
						CHECK_FAIL(L"Internal error: Unexpected primitive kind.");
					}
					AddRpcJsonUnknownTupleReturn(context, trueBranch, primitive.keyword, CreateReference(nodeName));
					AddStatement(block, CreateIf(CreateIsNotNull(CreateReference(varName)), trueBranch));
				}

				void AddUnknownSerializeEnumCase(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, const RpcJsonTypeModel& enumModel)
				{
					auto varName = L"value_" + MangleRpcFullName(enumModel.fullName);
					auto nullableType = Ptr(new WfNullableType);
					nullableType->element = CopyType(enumModel.type.Obj());
					AddStatement(block, CreateInferredVariableStatement(varName, CreateWeakCast(nullableType, CreateReference(L"value"))));
					auto trueBranch = CreateBlock();
					auto valueNode = AllocateRpcJsonTemp(context, L"jsonNode");
					AddStatement(trueBranch, CreateInferredVariableStatement(valueNode, CreateCall(CreateReference(GetRpcJsonSerializeEnumFunctionName(enumModel.fullName)), CreateCast(CopyType(enumModel.type.Obj()), CreateReference(varName)))));
					AddRpcJsonUnknownTupleReturn(context, trueBranch, enumModel.fullName, CreateReference(valueNode));
					AddStatement(block, CreateIf(CreateIsNotNull(CreateReference(varName)), trueBranch));
				}

				void AddUnknownSerializeStructCase(RpcJsonGenerationContext& context, Ptr<WfBlockStatement> block, const RpcJsonTypeModel& structModel)
				{
					auto varName = L"value_" + MangleRpcFullName(structModel.fullName);
					auto nullableType = Ptr(new WfNullableType);
					nullableType->element = CopyType(structModel.type.Obj());
					AddStatement(block, CreateInferredVariableStatement(varName, CreateWeakCast(nullableType, CreateReference(L"value"))));
					auto trueBranch = CreateBlock();
					AddStatement(trueBranch, CreateInferredVariableStatement(L"object", CreateNewClass(CreateSharedType(L"system::JsonObject"))));
					auto keywordNode = AddRpcJsonString(trueBranch, context, CreateString(structModel.fullName));
					AddRpcJsonObjectField(trueBranch, context, CreateReference(L"object"), L"$", CreateReference(keywordNode));
					auto varValueName = AllocateRpcJsonTemp(context, L"value");
					AddStatement(trueBranch, CreateInferredVariableStatement(varValueName, CreateCast(CopyType(structModel.type.Obj()), CreateReference(varName))));
					AddRpcJsonStructFields(context, trueBranch, structModel, L"object", varValueName);
					AddStatement(trueBranch, CreateReturn(CreateReference(L"object")));
					AddStatement(block, CreateIf(CreateIsNotNull(CreateReference(varName)), trueBranch));
				}

				Ptr<WfDeclaration> GenerateRpcJsonSerialize(RpcJsonGenerationContext& context)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpcjson_Serialize", CreateSharedType(L"system::JsonNode"), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"value", CreatePredefinedType(WfPredefinedTypeName::Object)));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();
					for (auto&& enumModel : *context.enums)
					{
						AddUnknownSerializeEnumCase(context, block, enumModel);
					}
					for (auto&& structModel : *context.structs)
					{
						AddUnknownSerializeStructCase(context, block, structModel);
					}
					AddStatement(block, CreateReturn(CreateCall(
						CreateQualifiedExpression(L"system::IRpcLifecycle::JsonSerializePredefinedTypes"),
						CreateReference(L"value"),
						CreateReference(L"rpcjson_Serialize"))));
					return functionDecl;
				}

				void AddUnknownDeserializePrimitiveCase(Ptr<WfSwitchStatement> switchStat, const RpcJsonPrimitiveModel& primitive)
				{
					if (primitive.kind == RpcJsonPrimitiveKind::Boolean || primitive.fullName == L"system::String")
					{
						return;
					}

					auto jsonType = primitive.kind == RpcJsonPrimitiveKind::Number ? L"system::JsonNumber" : L"system::JsonString";
					AddSwitchCase(
						switchStat,
						CreateString(primitive.keyword),
						CreateReturn(CreateCast(CreateTopQualifiedType(primitive.fullName), CreateJsonArrayItemContent(L"array", 1, jsonType))));
				}

				void AddUnknownDeserializeEnumCase(Ptr<WfSwitchStatement> switchStat, const RpcJsonTypeModel& enumModel)
				{
					AddSwitchCase(
						switchStat,
						CreateString(enumModel.fullName),
						CreateReturn(CreateCall(CreateReference(GetRpcJsonDeserializeEnumFunctionName(enumModel.fullName)), CreateJsonArrayItem(L"array", 1))));
				}

				void AddUnknownDeserializeStructCase(Ptr<WfSwitchStatement> switchStat, const RpcJsonTypeModel& structModel)
				{
					AddSwitchCase(
						switchStat,
						CreateString(structModel.fullName),
						CreateReturn(CreateCall(CreateReference(GetRpcJsonDeserializeStructFunctionName(structModel.fullName)), CreateReference(L"object"))));
				}

				Ptr<WfStatement> CreateUnknownDeserializeListCase(RpcJsonGenerationContext& context, const WString& variableType)
				{
					auto block = CreateBlock();
					AddStatement(block, CreateVariableStatement(L"result", CreateSharedType(variableType), CreateConstructor()));
					auto valuesNode = AddRpcJsonObjectFieldLookup(block, context, CreateReference(L"object"), L"values");
					AddStatement(block, CreateInferredVariableStatement(L"values", CreateCast(CreateSharedType(L"system::JsonArray"), CreateReference(valuesNode))));
					auto forBlock = CreateBlock();
					AddStatement(forBlock, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"result"), L"Add"), CreateCall(CreateReference(L"rpcjson_Deserialize"), CreateReference(L"item")))));
					AddStatement(block, CreateForEach(L"item", CreateMember(CreateReference(L"values"), L"items"), forBlock));
					AddStatement(block, CreateReturn(CreateReference(L"result")));
					return block;
				}

				Ptr<WfStatement> CreateUnknownDeserializeMapCase(RpcJsonGenerationContext& context)
				{
					auto block = CreateBlock();
					AddStatement(block, CreateVariableStatement(L"result", CreateSharedType(L"system::Dictionary"), CreateConstructor()));
					auto valuesNode = AddRpcJsonObjectFieldLookup(block, context, CreateReference(L"object"), L"values");
					AddStatement(block, CreateInferredVariableStatement(L"values", CreateCast(CreateSharedType(L"system::JsonArray"), CreateReference(valuesNode))));
					AddStatement(block, CreateInferredVariableStatement(L"index", CreateInt(0)));
					auto whileBlock = CreateBlock();
					AddStatement(whileBlock, CreateExpressionStatement(CreateCall(
						CreateMember(CreateReference(L"result"), L"Set"),
						CreateCall(CreateReference(L"rpcjson_Deserialize"), CreateIndex(CreateMember(CreateReference(L"values"), L"items"), CreateReference(L"index"))),
						CreateCall(CreateReference(L"rpcjson_Deserialize"), CreateIndex(CreateMember(CreateReference(L"values"), L"items"), CreateBinary(WfBinaryOperator::Add, CreateReference(L"index"), CreateInt(1)))))));
					AddStatement(whileBlock, CreateExpressionStatement(CreateAssign(CreateReference(L"index"), CreateBinary(WfBinaryOperator::Add, CreateReference(L"index"), CreateInt(2)))));
					AddStatement(block, CreateWhile(CreateBinary(WfBinaryOperator::LT, CreateReference(L"index"), CreateMember(CreateMember(CreateReference(L"values"), L"items"), L"Count")), whileBlock));
					AddStatement(block, CreateReturn(CreateReference(L"result")));
					return block;
				}

				Ptr<WfDeclaration> GenerateRpcJsonDeserialize(RpcJsonGenerationContext& context)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpcjson_Deserialize", CreatePredefinedType(WfPredefinedTypeName::Object), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"node", CreateSharedType(L"system::JsonNode")));
					auto block = functionDecl->statement.Cast<WfBlockStatement>();

					if (context.enums->Count() > 0)
					{
						AddStatement(block, CreateInferredVariableStatement(L"array", CreateWeakCast(CreateSharedType(L"system::JsonArray"), CreateReference(L"node"))));
						auto arrayBranch = CreateBlock();
						AddStatement(arrayBranch, CreateInferredVariableStatement(L"keyword", CreateJsonArrayItemContent(L"array", 0, L"system::JsonString")));
						auto arraySwitch = Ptr(new WfSwitchStatement);
						arraySwitch->expression = CreateReference(L"keyword");
						for (auto&& enumModel : *context.enums)
						{
							AddUnknownDeserializeEnumCase(arraySwitch, enumModel);
						}
						AddStatement(arrayBranch, arraySwitch);
						AddStatement(block, CreateIf(CreateIsNotNull(CreateReference(L"array")), arrayBranch));
					}

					AddStatement(block, CreateInferredVariableStatement(L"object", CreateWeakCast(CreateSharedType(L"system::JsonObject"), CreateReference(L"node"))));
					auto objectBranch = CreateBlock();
					{
						auto keywordNode = AddRpcJsonObjectFieldLookup(objectBranch, context, CreateReference(L"object"), L"$", false);
						auto keywordBranch = CreateBlock();
						AddStatement(keywordBranch, CreateInferredVariableStatement(L"keyword", CreateMember(CreateMember(CreateCast(CreateSharedType(L"system::JsonString"), CreateReference(keywordNode)), L"content"), L"value")));
						auto objectSwitch = Ptr(new WfSwitchStatement);
						objectSwitch->expression = CreateReference(L"keyword");
						for (auto&& structModel : *context.structs)
						{
							AddUnknownDeserializeStructCase(objectSwitch, structModel);
						}
						AddStatement(keywordBranch, objectSwitch);
						AddStatement(objectBranch, CreateIf(CreateIsNotNull(CreateReference(keywordNode)), keywordBranch));
					}
					AddStatement(block, CreateIf(CreateIsNotNull(CreateReference(L"object")), objectBranch));
					AddStatement(block, CreateReturn(CreateCall(
						CreateQualifiedExpression(L"system::IRpcLifecycle::JsonDeserializePredefinedTypes"),
						CreateReference(L"node"),
						CreateReference(L"rpcjson_Deserialize"))));
					return functionDecl;
				}

				void AddRpcJsonDeclarations(WfLexicalScopeManager* manager, Ptr<WfModule> module)
				{
					List<RpcJsonPrimitiveModel> primitives;
					List<RpcJsonTypeModel> enums;
					List<RpcJsonTypeModel> structs;
					CollectRpcJsonPrimitives(primitives);
					CollectRpcJsonTypes(manager, enums, structs);

					RpcJsonGenerationContext context;
					context.manager = manager;
					context.primitives = &primitives;
					context.enums = &enums;
					context.structs = &structs;

					for (auto&& enumModel : enums)
					{
						module->declarations.Add(GenerateRpcJsonSerializeEnum(context, enumModel));
						module->declarations.Add(GenerateRpcJsonDeserializeEnum(context, enumModel));
					}
					for (auto&& structModel : structs)
					{
						module->declarations.Add(GenerateRpcJsonSerializeStruct(context, structModel));
						module->declarations.Add(GenerateRpcJsonDeserializeStruct(context, structModel));
					}
					module->declarations.Add(GenerateRpcJsonSerialize(context));
					module->declarations.Add(GenerateRpcJsonDeserialize(context));
				}

				Ptr<WfDeclaration> GenerateRpcSerializerFactoryJson()
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpcops_IRpcSerializer", CreateSharedType(L"system::IRpcSerializer"), WfFunctionKind::Normal);
					auto newSerializer = CreateNewInterface(CreateSharedType(L"system::IRpcSerializer")).Cast<WfNewInterfaceExpression>();

					{
						auto serialize = CreateFunctionDeclaration(L"Serialize", CreatePredefinedType(WfPredefinedTypeName::Object), WfFunctionKind::Override);
						serialize->arguments.Add(CreateFunctionArgument(L"value", CreatePredefinedType(WfPredefinedTypeName::Object)));
						auto block = serialize->statement.Cast<WfBlockStatement>();
						AddStatement(block, CreateInferredVariableStatement(L"result", CreateCall(CreateReference(L"rpcjson_Serialize"), CreateReference(L"value"))));
						AddStatement(block, CreateReturn(CreateReference(L"result")));
						newSerializer->declarations.Add(serialize);
					}

					{
						auto deserialize = CreateFunctionDeclaration(L"Deserialize", CreatePredefinedType(WfPredefinedTypeName::Object), WfFunctionKind::Override);
						deserialize->arguments.Add(CreateFunctionArgument(L"value", CreatePredefinedType(WfPredefinedTypeName::Object)));
						auto block = deserialize->statement.Cast<WfBlockStatement>();
						AddStatement(block, CreateReturn(CreateCall(
							CreateReference(L"rpcjson_Deserialize"),
							CreateCast(CreateSharedType(L"system::JsonNode"), CreateReference(L"value")))));
						newSerializer->declarations.Add(deserialize);
					}

					AddStatement(functionDecl->statement.Cast<WfBlockStatement>(), CreateReturn(newSerializer));
					return functionDecl;
				}

				WString AddRpcJsonSerializeValue(WfLexicalScopeManager* manager, vint& tempIndex, Ptr<WfBlockStatement> block, Ptr<WfExpression> value, WfType* type)
				{
					List<RpcJsonPrimitiveModel> primitives;
					List<RpcJsonTypeModel> enums;
					List<RpcJsonTypeModel> structs;
					CollectRpcJsonPrimitives(primitives);
					CollectRpcJsonTypes(manager, enums, structs);

					RpcJsonGenerationContext context;
					context.manager = manager;
					context.primitives = &primitives;
					context.enums = &enums;
					context.structs = &structs;
					context.tempIndex = tempIndex;

					auto result = AddKnownRpcJsonSerializeValue(context, block, value, type);
					tempIndex = context.tempIndex;
					return result;
				}

				WString AddRpcJsonDeserializeValue(WfLexicalScopeManager* manager, vint& tempIndex, Ptr<WfBlockStatement> block, Ptr<WfExpression> node, WfType* type)
				{
					List<RpcJsonPrimitiveModel> primitives;
					List<RpcJsonTypeModel> enums;
					List<RpcJsonTypeModel> structs;
					CollectRpcJsonPrimitives(primitives);
					CollectRpcJsonTypes(manager, enums, structs);

					RpcJsonGenerationContext context;
					context.manager = manager;
					context.primitives = &primitives;
					context.enums = &enums;
					context.structs = &structs;
					context.tempIndex = tempIndex;

					auto result = AddKnownRpcJsonDeserializeValue(context, block, node, type);
					tempIndex = context.tempIndex;
					return result;
				}

				Ptr<WfType> CreateRpcJsonTransferType(ITypeInfo* typeInfo, bool byref, WfType* type)
				{
					if (IsSharedInterfaceType(typeInfo))
					{
						return byref
							? CreateQualifiedType(L"system::RpcObjectReference")
							: CreatePredefinedType(WfPredefinedTypeName::Object);
					}
					return CopyType(type);
				}

				Ptr<WfExpression> CreateRpcJsonTransferExpression(ITypeInfo* typeInfo, bool byref, Ptr<WfExpression> value, Ptr<WfExpression> lifecycle)
				{
					if (IsSharedInterfaceType(typeInfo))
					{
						return CreateRpcBoxExpression(typeInfo, byref, value, lifecycle);
					}
					return value;
				}

				Ptr<WfExpression> CreateRpcJsonTransferExpression(const RpcParamModel& paramModel, Ptr<WfExpression> value, Ptr<WfExpression> lifecycle)
				{
					return CreateRpcJsonTransferExpression(paramModel.typeInfo, paramModel.byref, value, lifecycle);
				}

				Ptr<WfExpression> CreateJsonDispatchArgument(
					WfLexicalScopeManager* manager,
					vint& tempIndex,
					Ptr<WfBlockStatement> block,
					Ptr<WfExpression> node,
					const RpcParamModel& paramModel)
				{
					auto transferType = CreateRpcJsonTransferType(paramModel.typeInfo, paramModel.byref, paramModel.type.Obj());
					auto valueName = AddRpcJsonDeserializeValue(manager, tempIndex, block, node, transferType.Obj());
					if (IsSharedInterfaceType(paramModel.typeInfo))
					{
						return CreateRpcUnboxExpression(paramModel.typeInfo, paramModel.type, paramModel.byref, CreateReference(valueName), CreateReference(L"_lc"));
					}
					return CreateReference(valueName);
				}

				Ptr<WfStatement> BuildInvokeMethodBranchJson(WfLexicalScopeManager* manager, const RpcInterfaceModel& interfaceModel, const RpcMethodModel& methodModel)
				{
					auto block = CreateBlock();
					AddStatement(block, CreateInferredVariableStatement(L"target", CreateCast(CreateSharedType(interfaceModel.fullName), CreateCall(CreateMember(CreateReference(L"_lc"), L"RefToPtr"), CreateReference(L"ref")))));

					List<Ptr<WfExpression>> arguments;
					vint tempIndex = 0;
					for (vint i = 0; i < methodModel.params.Count(); i++)
					{
						auto&& paramModel = methodModel.params[i];
						arguments.Add(CreateJsonDispatchArgument(
							manager,
							tempIndex,
							block,
							CreateCast(CreateSharedType(L"system::JsonNode"), CreateRpcJsonSerializedArgument(i)),
							paramModel));
					}

					auto invokeTarget = CreateMember(CreateReference(L"target"), methodModel.name);
					auto invoke = Ptr(new WfCallExpression);
					invoke->function = invokeTarget;
					for (auto argument : arguments)
					{
						invoke->arguments.Add(argument);
					}

					if (IsVoidType(methodModel.returnType.Obj()))
					{
						AddStatement(block, CreateExpressionStatement(invoke));
						RpcJsonGenerationContext context;
						context.manager = manager;
						context.tempIndex = tempIndex;
						auto nodeName = AddRpcJsonLiteral(block, context, L"Null");
						tempIndex = context.tempIndex;
						AddStatement(block, CreateReturn(CreateReference(nodeName)));
					}
					else
					{
						auto transferType = CreateRpcJsonTransferType(methodModel.returnTypeInfo, methodModel.returnByref, methodModel.returnType.Obj());
						if (IsRpcByvalReturn(methodModel))
						{
							AddStatement(block, CreateInferredVariableStatement(L"copiedReturnValue", CreateRpcCopyByvalExpression(invoke, CreateReference(L"_lc"))));
							auto transferValue = CreateRpcJsonTransferExpression(methodModel.returnTypeInfo, methodModel.returnByref, CreateReference(L"copiedReturnValue"), CreateReference(L"_lc"));
							auto nodeName = AddRpcJsonSerializeValue(manager, tempIndex, block, transferValue, transferType.Obj());
							AddRpcByvalReturnValue(block, CreateReference(nodeName), CreateReference(L"copiedReturnValue"));
						}
						else
						{
							auto transferValue = CreateRpcJsonTransferExpression(methodModel.returnTypeInfo, methodModel.returnByref, invoke, CreateReference(L"_lc"));
							auto nodeName = AddRpcJsonSerializeValue(manager, tempIndex, block, transferValue, transferType.Obj());
							AddStatement(block, CreateReturn(CreateReference(nodeName)));
						}
					}
					return block;
				}

				Ptr<WfStatement> BuildInvokeEventBranchJson(WfLexicalScopeManager* manager, const RpcInterfaceModel& interfaceModel, const RpcEventModel& eventModel)
				{
					auto block = CreateBlock();
					AddStatement(block, CreateInferredVariableStatement(L"target", CreateCast(CreateSharedType(interfaceModel.fullName), CreateCall(CreateMember(CreateReference(L"_lc"), L"RefToPtr"), CreateReference(L"ref")))));

					auto invoke = Ptr(new WfCallExpression);
					invoke->function = CreateMember(CreateReference(L"target"), eventModel.name);
					vint tempIndex = 0;
					for (vint i = 0; i < eventModel.params.Count(); i++)
					{
						auto&& paramModel = eventModel.params[i];
						invoke->arguments.Add(CreateJsonDispatchArgument(
							manager,
							tempIndex,
							block,
							CreateCast(CreateSharedType(L"system::JsonNode"), CreateRpcJsonSerializedArgument(i)),
							paramModel));
					}
					AddStatement(block, CreateExpressionStatement(invoke));
					return block;
				}

				Ptr<WfStatement> BuildDispatchChainJson(WfLexicalScopeManager* manager, const List<RpcInterfaceModel>& interfaces, bool forEvent, const WString& unknownIdVariable = WString::Empty)
				{
					auto switchStat = Ptr(new WfSwitchStatement);
					switchStat->expression = CreateReference(forEvent ? L"eventId" : L"methodId");
					switchStat->defaultBranch =
						unknownIdVariable == L""
						? CreateRaise(forEvent ? L"Unknown RPC event id." : L"Unknown RPC method id.")
						: CreateExpressionStatement(CreateAssign(CreateReference(unknownIdVariable), CreateBool(true)));

					for (auto&& interfaceModel : interfaces)
					{
						if (forEvent)
						{
							for (auto&& eventModel : interfaceModel.events)
							{
								auto switchCase = Ptr(new WfSwitchCase);
								switchCase->expression = CreateRpcConstantReference(L"rpcevent_", eventModel.fullName);
								switchCase->statement = BuildInvokeEventBranchJson(manager, interfaceModel, eventModel);
								switchStat->caseBranches.Add(switchCase);
							}
						}
						else
						{
							for (auto&& methodModel : interfaceModel.methods)
							{
								auto switchCase = Ptr(new WfSwitchCase);
								switchCase->expression = CreateRpcConstantReference(L"rpcmethod_", methodModel.fullName);
								switchCase->statement = BuildInvokeMethodBranchJson(manager, interfaceModel, methodModel);
								switchStat->caseBranches.Add(switchCase);
							}
						}
					}
					return switchStat;
				}

				Ptr<WfDeclaration> GenerateObjectOpsFactoryJson(WfLexicalScopeManager* manager, const List<RpcInterfaceModel>& interfaces)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpcops_IRpcObjectOpsJson", CreateSharedType(L"system::IRpcObjectOps"), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateRawType(L"system::IRpcLifecycle")));
					auto newOps = CreateNewInterface(CreateSharedType(L"system::IRpcObjectOps")).Cast<WfNewInterfaceExpression>();
					newOps->declarations.Add(CreateVariableDeclaration(L"_lc", CreateRawType(L"system::IRpcLifecycle"), CreateReference(L"lc")));
					newOps->declarations.Add(CreateVariableDeclaration(L"_slot", CreatePredefinedType(WfPredefinedTypeName::Int), CreateInt(0)));
					newOps->declarations.Add(CreateVariableDeclaration(L"_byvalReturnValues", CreateMapType(CreatePredefinedType(WfPredefinedTypeName::Int), CreatePredefinedType(WfPredefinedTypeName::Object)), CreateConstructor()));

					{
						auto invokeMethod = CreateFunctionDeclaration(L"InvokeMethod", CreatePredefinedType(WfPredefinedTypeName::Object), WfFunctionKind::Override);
						invokeMethod->arguments.Add(CreateFunctionArgument(L"ref", CreateQualifiedType(L"system::RpcObjectReference")));
						invokeMethod->arguments.Add(CreateFunctionArgument(L"methodId", CreatePredefinedType(WfPredefinedTypeName::Int)));
						invokeMethod->arguments.Add(CreateFunctionArgument(L"arguments", CreateSharedType(L"system::Array")));
						auto block = invokeMethod->statement.Cast<WfBlockStatement>();
						AddStatement(block, CreateVariableStatement(L"unknownId", CreatePredefinedType(WfPredefinedTypeName::Bool), CreateBool(false)));
						auto catchBlock = CreateBlock();
						AddStatement(catchBlock, CreateReturn(CreateCall(CreateReference(L"rpcjson_Serialize"), CreateRpcExceptionExpression(CreateMember(CreateReference(L"ex"), L"Message")))));
						AddStatement(block, CreateTryCatch(BuildDispatchChainJson(manager, interfaces, false, L"unknownId"), L"ex", catchBlock));
						auto unknownIdBranch = CreateBlock();
						AddStatement(unknownIdBranch, CreateRaise(L"Unknown RPC method id."));
						AddStatement(block, CreateIf(CreateReference(L"unknownId"), unknownIdBranch));
						AddStatement(block, CreateReturn(CreateCall(CreateReference(L"rpcjson_Serialize"), CreateNull())));
						newOps->declarations.Add(invokeMethod);
					}

					{
						auto endInvokeMethod = CreateFunctionDeclaration(L"EndInvokeMethod", CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Override);
						endInvokeMethod->arguments.Add(CreateFunctionArgument(L"slot", CreatePredefinedType(WfPredefinedTypeName::Int)));
						AddStatement(endInvokeMethod->statement.Cast<WfBlockStatement>(), CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"_byvalReturnValues"), L"Remove"), CreateReference(L"slot"))));
						newOps->declarations.Add(endInvokeMethod);
					}

					{
						auto objectHold = CreateFunctionDeclaration(L"ObjectHold", CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Override);
						objectHold->arguments.Add(CreateFunctionArgument(L"ref", CreateQualifiedType(L"system::RpcObjectReference")));
						objectHold->arguments.Add(CreateFunctionArgument(L"remoteClientId", CreatePredefinedType(WfPredefinedTypeName::Int)));
						objectHold->arguments.Add(CreateFunctionArgument(L"hold", CreatePredefinedType(WfPredefinedTypeName::Bool)));
						auto trueBranch = CreateBlock();
						AddStatement(trueBranch, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"_lc"), L"LocalObjectHold"), CreateReference(L"ref"), CreateReference(L"remoteClientId"))));
						auto falseBranch = CreateBlock();
						AddStatement(falseBranch, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"_lc"), L"LocalObjectUnhold"), CreateReference(L"ref"), CreateReference(L"remoteClientId"))));
						AddStatement(objectHold->statement.Cast<WfBlockStatement>(), CreateIf(CreateReference(L"hold"), trueBranch, falseBranch));
						newOps->declarations.Add(objectHold);
					}

					{
						auto registerService = CreateFunctionDeclaration(L"RegisterService", CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Override);
						registerService->arguments.Add(CreateFunctionArgument(L"typeId", CreatePredefinedType(WfPredefinedTypeName::Int)));
						registerService->arguments.Add(CreateFunctionArgument(L"service", CreateSharedType(L"system::Interface")));
						auto block = registerService->statement.Cast<WfBlockStatement>();
						AddStatement(block, BuildRegisterService());
						newOps->declarations.Add(registerService);
					}

					AddStatement(functionDecl->statement.Cast<WfBlockStatement>(), CreateReturn(newOps));
					return functionDecl;
				}

				Ptr<WfDeclaration> GenerateObjectEventOpsFactoryJson(WfLexicalScopeManager* manager, const List<RpcInterfaceModel>& interfaces)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpcops_IRpcObjectEventOpsJson", CreateSharedType(L"system::IRpcObjectEventOps"), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateRawType(L"system::IRpcLifecycle")));
					auto newOps = CreateNewInterface(CreateSharedType(L"system::IRpcObjectEventOps")).Cast<WfNewInterfaceExpression>();
					newOps->declarations.Add(CreateVariableDeclaration(L"_lc", CreateRawType(L"system::IRpcLifecycle"), CreateReference(L"lc")));

					{
						auto invokeEvent = CreateFunctionDeclaration(L"InvokeEvent", CreatePredefinedType(WfPredefinedTypeName::Object), WfFunctionKind::Override);
						invokeEvent->arguments.Add(CreateFunctionArgument(L"ref", CreateQualifiedType(L"system::RpcObjectReference")));
						invokeEvent->arguments.Add(CreateFunctionArgument(L"eventId", CreatePredefinedType(WfPredefinedTypeName::Int)));
						invokeEvent->arguments.Add(CreateFunctionArgument(L"arguments", CreateSharedType(L"system::Array")));
						auto block = invokeEvent->statement.Cast<WfBlockStatement>();
						if (!HasRpcEvents(interfaces))
						{
							AddStatement(block, CreateRaise(L"Unknown RPC event id."));
							newOps->declarations.Add(invokeEvent);
						}
						else
						{
							AddStatement(block, CreateVariableStatement(L"unknownId", CreatePredefinedType(WfPredefinedTypeName::Bool), CreateBool(false)));
							AddStatement(block, CreateVariableStatement(L"rpcEventExceptions", CreateRpcEventExceptionMapType(), CreateConstructor()));
							AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(CreateMember(CreateReference(L"_lc"), L"Controller"), L"SetEventSuppressedFlag"), CreateReference(L"ref"), CreateReference(L"eventId"), CreateBool(true))));
							auto finallyBlock = CreateBlock();
							AddStatement(finallyBlock, CreateExpressionStatement(CreateCall(CreateMember(CreateMember(CreateReference(L"_lc"), L"Controller"), L"SetEventSuppressedFlag"), CreateReference(L"ref"), CreateReference(L"eventId"), CreateBool(false))));
							auto catchBlock = CreateBlock();
							AddRpcEventExceptionMapSet(catchBlock, L"rpcEventExceptions", CreateMember(CreateReference(L"_lc"), L"ClientId"), CreateMember(CreateReference(L"ex"), L"Message"));
							AddStatement(block, CreateTry(BuildDispatchChainJson(manager, interfaces, true, L"unknownId"), L"ex", catchBlock, finallyBlock));
							auto unknownIdBranch = CreateBlock();
							AddStatement(unknownIdBranch, CreateRaise(L"Unknown RPC event id."));
							AddStatement(block, CreateIf(CreateReference(L"unknownId"), unknownIdBranch));
							auto returnExceptionBranch = CreateBlock();
							AddStatement(returnExceptionBranch, CreateReturn(CreateCall(CreateReference(L"rpcjson_Serialize"), CreateReference(L"rpcEventExceptions"))));
							auto returnNullBranch = CreateBlock();
							AddStatement(returnNullBranch, CreateReturn(CreateCall(CreateReference(L"rpcjson_Serialize"), CreateNull())));
							AddStatement(
								block,
								CreateIf(
									CreateBinary(WfBinaryOperator::GT, CreateMember(CreateReference(L"rpcEventExceptions"), L"Count"), CreateInt(0)),
									returnExceptionBranch,
									returnNullBranch));
							newOps->declarations.Add(invokeEvent);
						}
					}

					AddStatement(functionDecl->statement.Cast<WfBlockStatement>(), CreateReturn(newOps));
					return functionDecl;
				}

				void AddRpcOpsArgumentsArrayJson(
					WfLexicalScopeManager* manager,
					Ptr<WfBlockStatement> block,
					const List<RpcParamModel>& params,
					vint& tempIndex)
				{
					AddStatement(block, CreateVariableStatement(L"arguments", CreateSharedType(L"system::Array"), CreateConstructor()));
					AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"arguments"), L"Resize"), CreateInt(params.Count()))));

					for (vint i = 0; i < params.Count(); i++)
					{
						auto&& paramModel = params[i];
						auto value = CreateReference(GetRpcOpsArgumentName(paramModel));
						auto transferValue = CreateRpcJsonTransferExpression(paramModel, value, CreateReference(L"_lc"));
						auto transferType = CreateRpcJsonTransferType(paramModel.typeInfo, paramModel.byref, paramModel.type.Obj());
						auto nodeName = AddRpcJsonSerializeValue(manager, tempIndex, block, transferValue, transferType.Obj());
						AddStatement(block, CreateExpressionStatement(CreateCall(CreateMember(CreateReference(L"arguments"), L"Set"), CreateInt(i), CreateReference(nodeName))));
					}
				}

				Ptr<WfFunctionDeclaration> GenerateRpcOpsMethodImplementationJson(WfLexicalScopeManager* manager, const RpcMethodModel& methodModel)
				{
					auto functionDecl = CreateRpcOpsFunctionDeclaration(GetRpcOpsInvokeMethodName(methodModel), CopyType(methodModel.returnType.Obj()), WfFunctionKind::Override);
					AddRpcOpsFunctionArguments(functionDecl, methodModel.params);
					auto block = functionDecl->statement.Cast<WfBlockStatement>();

					vint tempIndex = 0;
					AddRpcOpsArgumentsArrayJson(manager, block, methodModel.params, tempIndex);
					if (IsVoidType(methodModel.returnType.Obj()))
					{
						auto invoke = CreateRpcOpsObjectInvoke(methodModel);
						AddStatement(block, CreateInferredVariableStatement(L"invokeResult", invoke));
						AddStatement(block, CreateInferredVariableStatement(
							L"jsonResult",
							CreateCast(CreateSharedType(L"system::JsonNode"), CreateReference(L"invokeResult"))));
						AddStatement(block, CreateInferredVariableStatement(
							L"methodResult",
							CreateCall(CreateReference(L"rpcjson_Deserialize"), CreateReference(L"jsonResult"))));
						AddRpcMethodExceptionRaise(block, CreateReference(L"methodResult"));
					}
					else if (IsRpcByvalReturn(methodModel))
					{
						AddStatement(block, CreateInferredVariableStatement(L"objectOps", CreateRpcOpsObjectOps()));
						auto invoke = CreateRpcOpsObjectInvoke(methodModel, CreateReference(L"objectOps"));
						AddStatement(block, CreateInferredVariableStatement(L"invokeResult", invoke));
						AddStatement(block, CreateInferredVariableStatement(
							L"byvalReturnValue",
							CreateWeakCast(CreateSharedType(L"system::RpcByvalReturnValue"), CreateReference(L"invokeResult"))));
						AddStatement(block, CreateVariableStatement(L"jsonResult", CreateSharedType(L"system::JsonNode"), CreateNull()));
						auto exceptionBranch = CreateBlock();
						AddStatement(exceptionBranch, CreateExpressionStatement(CreateAssign(
							CreateReference(L"jsonResult"),
							CreateCast(CreateSharedType(L"system::JsonNode"), CreateReference(L"invokeResult")))));
						auto returnBranch = CreateBlock();
						AddStatement(returnBranch, CreateExpressionStatement(CreateAssign(
							CreateReference(L"jsonResult"),
							CreateCast(CreateSharedType(L"system::JsonNode"), CreateMember(CreateReference(L"byvalReturnValue"), L"value")))));
						AddStatement(block, CreateIf(CreateIsNull(CreateReference(L"byvalReturnValue")), exceptionBranch, returnBranch));
						AddStatement(block, CreateInferredVariableStatement(
							L"methodResult",
							CreateCall(CreateReference(L"rpcjson_Deserialize"), CreateReference(L"jsonResult"))));
						AddRpcMethodExceptionRaise(block, CreateReference(L"methodResult"));
						AddStatement(block, CreateInferredVariableStatement(
							L"strongByvalReturnValue",
							CreateCast(CreateSharedType(L"system::RpcByvalReturnValue"), CreateReference(L"byvalReturnValue"))));
						auto transferType = CreateRpcJsonTransferType(methodModel.returnTypeInfo, methodModel.returnByref, methodModel.returnType.Obj());
						auto valueName = AddRpcJsonDeserializeValue(manager, tempIndex, block, CreateReference(L"jsonResult"), transferType.Obj());
						AddStatement(block, CreateInferredVariableStatement(
							L"result",
							IsSharedInterfaceType(methodModel.returnTypeInfo)
								? CreateRpcUnboxExpression(methodModel.returnTypeInfo, methodModel.returnType, methodModel.returnByref, CreateReference(valueName), CreateReference(L"_lc"))
								: CreateReference(valueName)));
						AddStatement(block, CreateExpressionStatement(CreateCall(
							CreateMember(CreateReference(L"objectOps"), L"EndInvokeMethod"),
							CreateMember(CreateReference(L"strongByvalReturnValue"), L"slot"))));
						AddStatement(block, CreateReturn(CreateReference(L"result")));
					}
					else
					{
						auto invoke = CreateRpcOpsObjectInvoke(methodModel);
						AddStatement(block, CreateInferredVariableStatement(L"invokeResult", invoke));
						AddStatement(block, CreateInferredVariableStatement(L"jsonResult", CreateCast(CreateSharedType(L"system::JsonNode"), CreateReference(L"invokeResult"))));
						AddStatement(block, CreateInferredVariableStatement(
							L"methodResult",
							CreateCall(CreateReference(L"rpcjson_Deserialize"), CreateReference(L"jsonResult"))));
						AddRpcMethodExceptionRaise(block, CreateReference(L"methodResult"));
						auto transferType = CreateRpcJsonTransferType(methodModel.returnTypeInfo, methodModel.returnByref, methodModel.returnType.Obj());
						auto valueName = AddRpcJsonDeserializeValue(manager, tempIndex, block, CreateReference(L"jsonResult"), transferType.Obj());
						if (IsSharedInterfaceType(methodModel.returnTypeInfo))
						{
							AddStatement(block, CreateReturn(CreateRpcUnboxExpression(methodModel.returnTypeInfo, methodModel.returnType, methodModel.returnByref, CreateReference(valueName), CreateReference(L"_lc"))));
						}
						else
						{
							AddStatement(block, CreateReturn(CreateReference(valueName)));
						}
					}

					return functionDecl;
				}

				Ptr<WfFunctionDeclaration> GenerateRpcOpsEventImplementationJson(WfLexicalScopeManager* manager, const RpcEventModel& eventModel)
				{
					auto functionDecl = CreateRpcOpsFunctionDeclaration(GetRpcOpsInvokeEventName(eventModel), CreatePredefinedType(WfPredefinedTypeName::Void), WfFunctionKind::Override);
					AddRpcOpsFunctionArguments(functionDecl, eventModel.params);
					auto block = functionDecl->statement.Cast<WfBlockStatement>();

					vint tempIndex = 0;
					AddRpcOpsArgumentsArrayJson(manager, block, eventModel.params, tempIndex);
					AddStatement(block, CreateInferredVariableStatement(L"invokeResult", CreateRpcOpsObjectEventInvoke(eventModel)));
					AddStatement(block, CreateInferredVariableStatement(
						L"jsonResult",
						CreateCast(CreateSharedType(L"system::JsonNode"), CreateReference(L"invokeResult"))));
					AddStatement(block, CreateInferredVariableStatement(
						L"eventResult",
						CreateCall(CreateReference(L"rpcjson_Deserialize"), CreateReference(L"jsonResult"))));
					AddRpcEventExceptionRaise(block, CreateCast(CreateRpcEventExceptionMapType(), CreateReference(L"eventResult")));
					return functionDecl;
				}

				Ptr<WfDeclaration> GenerateRpcOpsFactoryJson(WfLexicalScopeManager* manager, const WString& assemblyName, const List<RpcInterfaceModel>& interfaces)
				{
					auto functionDecl = CreateFunctionDeclaration(L"rpcops_IOps_CreateJson", CreateSharedType(GetRpcOpsInterfaceName(assemblyName)), WfFunctionKind::Normal);
					functionDecl->arguments.Add(CreateFunctionArgument(L"lc", CreateRawType(L"system::IRpcLifecycle")));
					auto newOps = CreateNewInterface(CreateSharedType(GetRpcOpsInterfaceName(assemblyName))).Cast<WfNewInterfaceExpression>();
					newOps->declarations.Add(CreateVariableDeclaration(L"_lc", CreateRawType(L"system::IRpcLifecycle"), CreateReference(L"lc")));

					for (auto&& interfaceModel : interfaces)
					{
						for (auto&& methodModel : interfaceModel.methods)
						{
							newOps->declarations.Add(GenerateRpcOpsMethodImplementationJson(manager, methodModel));
						}
						for (auto&& eventModel : interfaceModel.events)
						{
							newOps->declarations.Add(GenerateRpcOpsEventImplementationJson(manager, eventModel));
						}
					}

					AddStatement(functionDecl->statement.Cast<WfBlockStatement>(), CreateReturn(newOps));
					return functionDecl;
				}
			}

			Ptr<WfModule> GenerateModuleRpcJson(WfLexicalScopeManager* manager, WString assemblyName)
			{
				using namespace rpc_generating;

				if (!manager || !manager->rpcMetadata || !manager->rpcMetadata->metadataModule)
				{
					return nullptr;
				}

				CollectMangledNames(manager);
				if (manager->errors.Count() > 0)
				{
					return nullptr;
				}

				auto interfaces = BuildInterfaceModels(manager);
				if (manager->errors.Count() > 0)
				{
					return nullptr;
				}

				auto module = Ptr(new WfModule);
				module->moduleType = WfModuleType::Module;
				module->name.value = L"RpcMetadataJson";

				AddRpcJsonDeclarations(manager, module);
				module->declarations.Add(GenerateRpcSerializerFactoryJson());
				module->declarations.Add(GenerateObjectOpsFactoryJson(manager, interfaces));
				module->declarations.Add(GenerateObjectEventOpsFactoryJson(manager, interfaces));
				module->declarations.Add(GenerateRpcOpsFactoryJson(manager, assemblyName, interfaces));

				return module;
			}
		}
	}
}
