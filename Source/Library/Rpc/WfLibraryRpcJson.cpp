#include "WfLibraryRpcJson.h"
#include "WfLibraryRpcWrappers.h"
#include "../WfLibraryCppHelper.h"
#include "../WfLibraryReflection.h"

namespace vl
{
	namespace rpc_controller
	{
		using namespace collections;
		using namespace glr::json;
		using namespace reflection;
		using namespace reflection::description;

		namespace
		{
			Ptr<JsonLiteral> CreateJsonLiteral(JsonLiteralValue value)
			{
				auto node = Ptr(new JsonLiteral);
				node->value = value;
				return node;
			}

			Ptr<JsonString> CreateJsonString(const WString& value)
			{
				auto node = Ptr(new JsonString);
				node->content.value = value;
				return node;
			}

			Ptr<JsonNumber> CreateJsonNumber(const WString& value)
			{
				auto node = Ptr(new JsonNumber);
				node->content.value = value;
				return node;
			}

			Ptr<JsonArray> CreateJsonArray()
			{
				return Ptr(new JsonArray);
			}

			Ptr<JsonObject> CreateJsonObject()
			{
				return Ptr(new JsonObject);
			}

			Ptr<JsonObject> GetJsonObject(Ptr<JsonNode> node)
			{
				auto object = node.Cast<JsonObject>();
				CHECK_ERROR(object, L"JSON object is expected.");
				return object;
			}

			Ptr<JsonArray> GetJsonArray(Ptr<JsonNode> node)
			{
				auto array = node.Cast<JsonArray>();
				CHECK_ERROR(array, L"JSON array is expected.");
				return array;
			}

			void AddJsonObjectField(Ptr<JsonObject> object, const WString& name, Ptr<JsonNode> value)
			{
				auto field = Ptr(new JsonObjectField);
				field->name.value = name;
				field->value = value;
				object->fields.Add(field);
			}

			void SetJsonObjectField(Ptr<JsonObject> object, const WString& name, Ptr<JsonNode> value)
			{
				for (auto field : object->fields)
				{
					if (field->name.value == name)
					{
						field->value = value;
						return;
					}
				}
				AddJsonObjectField(object, name, value);
			}

			Ptr<JsonNode> GetJsonObjectField(Ptr<JsonObject> object, const WString& name)
			{
				for (auto field : object->fields)
				{
					if (field->name.value == name)
					{
						return field->value;
					}
				}
				auto message = L"JSON object field not found: " + name;
				CHECK_FAIL(message.Buffer());
			}

			Ptr<JsonNode> FindJsonObjectField(Ptr<JsonObject> object, const WString& name)
			{
				for (auto field : object->fields)
				{
					if (field->name.value == name)
					{
						return field->value;
					}
				}
				return nullptr;
			}

			WString GetJsonString(Ptr<JsonNode> node)
			{
				auto stringNode = node.Cast<JsonString>();
				CHECK_ERROR(stringNode, L"JSON string is expected.");
				return stringNode->content.value;
			}

			WString GetJsonNumber(Ptr<JsonNode> node)
			{
				auto numberNode = node.Cast<JsonNumber>();
				CHECK_ERROR(numberNode, L"JSON number is expected.");
				return numberNode->content.value;
			}

			vint GetJsonInt(Ptr<JsonNode> node)
			{
				return __vwsn::Parse<vint>(GetJsonNumber(node));
			}

			bool GetJsonBool(Ptr<JsonNode> node)
			{
				auto literal = node.Cast<JsonLiteral>();
				CHECK_ERROR(literal, L"JSON boolean is expected.");
				if (literal->value == JsonLiteralValue::True) return true;
				if (literal->value == JsonLiteralValue::False) return false;
				CHECK_FAIL(L"JSON boolean is expected.");
				return false;
			}

			Ptr<JsonNumber> CreateJsonNumber(vint value)
			{
				return CreateJsonNumber(__vwsn::ToString(value));
			}

			Ptr<JsonLiteral> CreateJsonBool(bool value)
			{
				return CreateJsonLiteral(value ? JsonLiteralValue::True : JsonLiteralValue::False);
			}

			Ptr<JsonNode> CreateUnknownTuple(const WString& keyword, Ptr<JsonNode> value)
			{
				auto array = CreateJsonArray();
				array->items.Add(CreateJsonString(keyword));
				array->items.Add(value);
				return Ptr<JsonNode>(array);
			}

			template<typename T>
			bool TrySerializeNumberPrimitive(const Value& value, const WString& keyword, Ptr<JsonNode>& result)
			{
				auto primitive = __vwsn::UnboxWeak<Nullable<T>>(value);
				if (primitive)
				{
					result = CreateUnknownTuple(keyword, CreateJsonNumber(__vwsn::ToString(primitive.Value())));
					return true;
				}
				return false;
			}

			template<typename T>
			bool TrySerializeStringPrimitive(const Value& value, const WString& keyword, Ptr<JsonNode>& result)
			{
				auto primitive = __vwsn::UnboxWeak<Nullable<T>>(value);
				if (primitive)
				{
					result = CreateUnknownTuple(keyword, CreateJsonString(__vwsn::ToString(primitive.Value())));
					return true;
				}
				return false;
			}

			Ptr<JsonNode> SerializeCollection(const WString& keyword, Ptr<IValueEnumerable> enumerable, const RpcJsonSerializeCallback& rpcjson_Serialize)
			{
				auto object = CreateJsonObject();
				AddJsonObjectField(object, WString::Unmanaged(L"$"), CreateJsonString(keyword));
				auto values = CreateJsonArray();
				auto enumerator = enumerable->CreateEnumerator();
				while (enumerator->Next())
				{
					values->items.Add(rpcjson_Serialize(enumerator->GetCurrent()));
				}
				AddJsonObjectField(object, WString::Unmanaged(L"values"), values);
				return Ptr<JsonNode>(object);
			}

			Ptr<JsonNode> SerializeMap(Ptr<IValueReadonlyDictionary> dictionary, const RpcJsonSerializeCallback& rpcjson_Serialize)
			{
				auto object = CreateJsonObject();
				AddJsonObjectField(object, WString::Unmanaged(L"$"), CreateJsonString(WString::Unmanaged(L"map")));
				auto values = CreateJsonArray();
				auto keys = dictionary->GetKeys();
				for (vint i = 0; i < keys->GetCount(); i++)
				{
					auto key = keys->Get(i);
					auto pair = CreateJsonArray();
					pair->items.Add(rpcjson_Serialize(key));
					pair->items.Add(rpcjson_Serialize(dictionary->Get(key)));
					values->items.Add(pair);
				}
				AddJsonObjectField(object, WString::Unmanaged(L"values"), values);
				return Ptr<JsonNode>(object);
			}

			Ptr<JsonNode> TrySerializeCollectionTypes(const Value& value, const RpcJsonSerializeCallback& rpcjson_Serialize)
			{
				if (auto observableList = __vwsn::UnboxWeak<Ptr<IValueObservableList>>(value))
				{
					return SerializeCollection(WString::Unmanaged(L"oblist"), observableList, rpcjson_Serialize);
				}
				if (auto dictionary = __vwsn::UnboxWeak<Ptr<IValueDictionary>>(value))
				{
					return SerializeMap(dictionary, rpcjson_Serialize);
				}
				if (auto readonlyDictionary = __vwsn::UnboxWeak<Ptr<IValueReadonlyDictionary>>(value))
				{
					return SerializeMap(readonlyDictionary, rpcjson_Serialize);
				}
				if (auto list = __vwsn::UnboxWeak<Ptr<IValueList>>(value))
				{
					return SerializeCollection(WString::Unmanaged(L"list"), list, rpcjson_Serialize);
				}
				if (auto readonlyList = __vwsn::UnboxWeak<Ptr<IValueReadonlyList>>(value))
				{
					return SerializeCollection(WString::Unmanaged(L"list"), readonlyList, rpcjson_Serialize);
				}
				if (auto enumerable = __vwsn::UnboxWeak<Ptr<IValueEnumerable>>(value))
				{
					return SerializeCollection(WString::Unmanaged(L"list"), enumerable, rpcjson_Serialize);
				}
				return nullptr;
			}

			Ptr<JsonNode> TrySerializeRpcTypes(const Value& value)
			{
				if (auto ref = __vwsn::UnboxWeak<Nullable<RpcObjectReference>>(value))
				{
					auto object = CreateJsonObject();
					AddJsonObjectField(object, WString::Unmanaged(L"$"), CreateJsonString(WString::Unmanaged(L"system::RpcObjectReference")));
					AddJsonObjectField(object, WString::Unmanaged(L"clientId"), CreateJsonNumber(__vwsn::ToString(ref.Value().clientId)));
					AddJsonObjectField(object, WString::Unmanaged(L"objectId"), CreateJsonNumber(__vwsn::ToString(ref.Value().objectId)));
					AddJsonObjectField(object, WString::Unmanaged(L"typeId"), CreateJsonNumber(__vwsn::ToString(ref.Value().typeId)));
					return Ptr<JsonNode>(object);
				}
				if (auto exception = __vwsn::UnboxWeak<Nullable<RpcException>>(value))
				{
					auto object = CreateJsonObject();
					AddJsonObjectField(object, WString::Unmanaged(L"$"), CreateJsonString(WString::Unmanaged(L"system::RpcException")));
					AddJsonObjectField(object, WString::Unmanaged(L"message"), CreateJsonString(exception.Value().message));
					return Ptr<JsonNode>(object);
				}
				return nullptr;
			}

			Value DeserializeList(Ptr<JsonObject> object, bool observable, const RpcJsonDeserializeCallback& rpcjson_Deserialize)
			{
				auto values = GetJsonObjectField(object, WString::Unmanaged(L"values")).Cast<JsonArray>();
				CHECK_ERROR(values, L"JSON array is expected.");
				if (observable)
				{
					auto result = IValueObservableList::Create();
					for (auto item : values->items)
					{
						result->Add(rpcjson_Deserialize(item));
					}
					return BoxValue(result);
				}
				else
				{
					auto result = IValueList::Create();
					for (auto item : values->items)
					{
						result->Add(rpcjson_Deserialize(item));
					}
					return BoxValue(result);
				}
			}

			Value DeserializeMap(Ptr<JsonObject> object, const RpcJsonDeserializeCallback& rpcjson_Deserialize)
			{
				auto values = GetJsonObjectField(object, WString::Unmanaged(L"values")).Cast<JsonArray>();
				CHECK_ERROR(values, L"JSON array is expected.");
				auto result = IValueDictionary::Create();
				for (auto item : values->items)
				{
					auto pair = item.Cast<JsonArray>();
					CHECK_ERROR(pair, L"JSON array is expected.");
					result->Set(rpcjson_Deserialize(pair->items[0]), rpcjson_Deserialize(pair->items[1]));
				}
				return BoxValue(result);
			}

			Value TryDeserializeRpcTypes(Ptr<JsonObject> object, const WString& keyword)
			{
				if (keyword == L"system::RpcObjectReference")
				{
					return BoxValue(RpcObjectReference{
						__vwsn::Parse<vint>(GetJsonNumber(GetJsonObjectField(object, WString::Unmanaged(L"clientId")))),
						__vwsn::Parse<vint>(GetJsonNumber(GetJsonObjectField(object, WString::Unmanaged(L"objectId")))),
						__vwsn::Parse<vint>(GetJsonNumber(GetJsonObjectField(object, WString::Unmanaged(L"typeId")))),
						});
				}
				if (keyword == L"system::RpcException")
				{
					return BoxValue(RpcException{
						GetJsonString(GetJsonObjectField(object, WString::Unmanaged(L"message"))),
						});
				}
				return {};
			}

			RpcObjectReference GetRpcObjectReferenceFromJson(Ptr<JsonNode> node)
			{
				auto object = GetJsonObject(node);
				return {
					GetJsonInt(GetJsonObjectField(object, WString::Unmanaged(L"clientId"))),
					GetJsonInt(GetJsonObjectField(object, WString::Unmanaged(L"objectId"))),
					GetJsonInt(GetJsonObjectField(object, WString::Unmanaged(L"typeId"))),
					};
			}

			Ptr<JsonObject> CreateRpcObjectReferenceJson(RpcObjectReference ref)
			{
				auto object = CreateJsonObject();
				AddJsonObjectField(object, WString::Unmanaged(L"clientId"), CreateJsonNumber(ref.clientId));
				AddJsonObjectField(object, WString::Unmanaged(L"objectId"), CreateJsonNumber(ref.objectId));
				AddJsonObjectField(object, WString::Unmanaged(L"typeId"), CreateJsonNumber(ref.typeId));
				return object;
			}

			Ptr<JsonNode> ValueToJsonNode(const Value& value)
			{
				if (value.GetValueType() == Value::SharedPtr)
				{
					if (auto node = value.GetSharedPtr().Cast<JsonNode>())
					{
						return node;
					}
				}
				CHECK_FAIL(L"RPC JSON value should be a Ptr<JsonNode>.");
				return nullptr;
			}

			Ptr<JsonArray> ValueArrayToJsonArray(Ptr<IValueArray> arguments)
			{
				auto array = CreateJsonArray();
				for (vint i = 0; i < arguments->GetCount(); i++)
				{
					array->items.Add(ValueToJsonNode(arguments->Get(i)));
				}
				return array;
			}

			Ptr<IValueArray> JsonArrayToValueArray(Ptr<JsonNode> node)
			{
				auto array = GetJsonArray(node);
				auto arguments = IValueArray::Create();
				arguments->Resize(array->items.Count());
				for (vint i = 0; i < array->items.Count(); i++)
				{
					arguments->Set(i, BoxValue(array->items[i]));
				}
				return arguments;
			}

			Ptr<JsonObject> CreateRpcMessage(const WString& method, vint requestId, vint sourceClientId)
			{
				auto object = CreateJsonObject();
				AddJsonObjectField(object, WString::Unmanaged(L"rpcMethod"), CreateJsonString(method));
				AddJsonObjectField(object, WString::Unmanaged(L"rpcRequestId"), CreateJsonNumber(requestId));
				AddJsonObjectField(object, WString::Unmanaged(L"sourceClientId"), CreateJsonNumber(sourceClientId));
				return object;
			}

			vint ReadSourceClientId(Ptr<JsonObject> object)
			{
				return GetJsonInt(GetJsonObjectField(object, WString::Unmanaged(L"sourceClientId")));
			}

			vint ReadTargetClientId(Ptr<JsonObject> object)
			{
				return GetJsonInt(GetJsonObjectField(object, WString::Unmanaged(L"targetClientId")));
			}

			Ptr<JsonNode> MethodResultToJsonResponse(const Value& result)
			{
				if (result.GetValueType() == Value::SharedPtr)
				{
					if (auto byvalReturnValue = result.GetSharedPtr().Cast<RpcByvalReturnValue>())
					{
						auto object = CreateJsonObject();
						AddJsonObjectField(object, WString::Unmanaged(L"value"), ValueToJsonNode(byvalReturnValue->value));
						AddJsonObjectField(object, WString::Unmanaged(L"slot"), CreateJsonNumber(byvalReturnValue->slot));
						return object;
					}
				}
				return ValueToJsonNode(result);
			}

			Value JsonResponseToMethodResult(Ptr<JsonNode> node)
			{
				if (auto object = node.Cast<JsonObject>())
				{
					auto valueNode = FindJsonObjectField(object, WString::Unmanaged(L"value"));
					auto slotNode = FindJsonObjectField(object, WString::Unmanaged(L"slot"));
					if (valueNode && slotNode)
					{
						auto byvalReturnValue = Ptr(new RpcByvalReturnValue);
						byvalReturnValue->value = BoxValue(valueNode);
						byvalReturnValue->slot = GetJsonInt(slotNode);
						return BoxValue(byvalReturnValue);
					}
				}
				return BoxValue(node);
			}

			Ptr<JsonNode> SerializePredefinedValue(const Value& value)
			{
				RpcJsonSerializeCallback rpcjson_Serialize;
				rpcjson_Serialize = Func<Ptr<JsonNode>(const Value&)>([&rpcjson_Serialize](const Value& item)
				{
					return JsonSerializePredefinedTypes(item, rpcjson_Serialize);
				});
				return JsonSerializePredefinedTypes(value, rpcjson_Serialize);
			}

			Value DeserializePredefinedValue(Ptr<JsonNode> node)
			{
				RpcJsonDeserializeCallback rpcjson_Deserialize;
				rpcjson_Deserialize = Func<Value(Ptr<JsonNode>)>([&rpcjson_Deserialize](Ptr<JsonNode> item)
				{
					return JsonDeserializePredefinedTypes(BoxValue(item), rpcjson_Deserialize);
				});
				return JsonDeserializePredefinedTypes(BoxValue(node), rpcjson_Deserialize);
			}

			Ptr<JsonObject> CreatePlainRpcException(const RpcException& exception)
			{
				auto result = CreateJsonObject();
				AddJsonObjectField(result, WString::Unmanaged(L"message"), CreateJsonString(exception.message));
				return result;
			}

			RpcException ReadPlainRpcException(Ptr<JsonNode> node)
			{
				auto object = GetJsonObject(node);
				return { GetJsonString(GetJsonObjectField(object, WString::Unmanaged(L"message"))) };
			}

			Ptr<JsonNode> CreateEventExceptionResponse(Ptr<JsonNode> serialized)
			{
				auto value = DeserializePredefinedValue(serialized);
				if (value.IsNull())
				{
					return CreateJsonLiteral(JsonLiteralValue::Null);
				}

				auto exceptions = UnboxValue<RpcEventExceptionMap>(value);
				auto result = CreateJsonArray();
				auto keys = exceptions->GetKeys();
				for (vint i = 0; i < keys->GetCount(); i++)
				{
					auto key = keys->Get(i);
					auto resultPair = CreateJsonArray();
					resultPair->items.Add(CreateJsonNumber(UnboxValue<vint>(key)));
					resultPair->items.Add(CreatePlainRpcException(UnboxValue<RpcException>(exceptions->Get(key))));
					result->items.Add(resultPair);
				}
				return result;
			}

			Ptr<JsonNode> CreateSerializedEventExceptionMap(Ptr<JsonNode> response)
			{
				if (auto literal = response.Cast<JsonLiteral>())
				{
					CHECK_ERROR(literal->value == JsonLiteralValue::Null, L"RPC event exception response should be null or an array.");
					return SerializePredefinedValue({});
				}

				auto array = GetJsonArray(response);
				auto exceptions = IValueDictionary::Create();
				for (auto item : array->items)
				{
					auto pair = GetJsonArray(item);
					CHECK_ERROR(pair->items.Count() == 2, L"RPC event exception response pair is expected.");
					exceptions->Set(BoxValue(GetJsonInt(pair->items[0])), BoxValue(ReadPlainRpcException(pair->items[1])));
				}
				return SerializePredefinedValue(BoxValue(exceptions));
			}
		}

		Ptr<JsonNode> JsonSerializePredefinedTypes(const Value& value, const RpcJsonSerializeCallback& rpcjson_Serialize)
		{
			if (value.IsNull())
			{
				return CreateJsonLiteral(JsonLiteralValue::Null);
			}

			if (auto primitive = __vwsn::UnboxWeak<Nullable<bool>>(value))
			{
				return CreateJsonLiteral(primitive.Value() ? JsonLiteralValue::True : JsonLiteralValue::False);
			}

			if (auto primitive = __vwsn::UnboxWeak<Nullable<WString>>(value))
			{
				return CreateJsonString(primitive.Value());
			}

			Ptr<JsonNode> result;
			if (TrySerializeNumberPrimitive<vuint8_t>(value, WString::Unmanaged(L"UInt8"), result)) return result;
			if (TrySerializeNumberPrimitive<vuint16_t>(value, WString::Unmanaged(L"UInt16"), result)) return result;
			if (TrySerializeNumberPrimitive<vuint32_t>(value, WString::Unmanaged(L"UInt32"), result)) return result;
			if (TrySerializeNumberPrimitive<vuint64_t>(value, WString::Unmanaged(L"UInt64"), result)) return result;
			if (TrySerializeNumberPrimitive<vint8_t>(value, WString::Unmanaged(L"Int8"), result)) return result;
			if (TrySerializeNumberPrimitive<vint16_t>(value, WString::Unmanaged(L"Int16"), result)) return result;
			if (TrySerializeNumberPrimitive<vint32_t>(value, WString::Unmanaged(L"Int32"), result)) return result;
			if (TrySerializeNumberPrimitive<vint64_t>(value, WString::Unmanaged(L"Int64"), result)) return result;
			if (TrySerializeNumberPrimitive<float>(value, WString::Unmanaged(L"Single"), result)) return result;
			if (TrySerializeNumberPrimitive<double>(value, WString::Unmanaged(L"Double"), result)) return result;
			if (TrySerializeStringPrimitive<wchar_t>(value, WString::Unmanaged(L"Char"), result)) return result;
			if (TrySerializeStringPrimitive<DateTime>(value, WString::Unmanaged(L"DateTime"), result)) return result;
			if (TrySerializeStringPrimitive<Locale>(value, WString::Unmanaged(L"Locale"), result)) return result;

			if (auto rpcType = TrySerializeRpcTypes(value))
			{
				return rpcType;
			}

			if (auto collection = TrySerializeCollectionTypes(value, rpcjson_Serialize))
			{
				return collection;
			}

			CHECK_FAIL(L"Unsupported RPC JSON serialization value.");
		}

		Value JsonDeserializePredefinedTypes(const Value& value, const RpcJsonDeserializeCallback& rpcjson_Deserialize)
		{
			if (value.IsNull())
			{
				return {};
			}

			auto node = UnboxValue<Ptr<JsonNode>>(value);
			if (!node)
			{
				return {};
			}

			if (auto literal = node.Cast<JsonLiteral>())
			{
				switch (literal->value)
				{
				case JsonLiteralValue::Null:
					return {};
				case JsonLiteralValue::True:
					return BoxValue(true);
				case JsonLiteralValue::False:
					return BoxValue(false);
				default:
					CHECK_FAIL(L"Unsupported JSON literal.");
				}
			}

			if (auto stringNode = node.Cast<JsonString>())
			{
				return BoxValue(stringNode->content.value);
			}

			if (auto numberNode = node.Cast<JsonNumber>())
			{
				return BoxValue(numberNode->content.value);
			}

			if (auto array = node.Cast<JsonArray>())
			{
				if (array->items.Count() == 2)
				{
					if (auto keywordNode = array->items[0].Cast<JsonString>())
					{
						auto keyword = keywordNode->content.value;
						auto item = array->items[1];
						if (keyword == L"UInt8") return BoxValue(__vwsn::Parse<vuint8_t>(GetJsonNumber(item)));
						if (keyword == L"UInt16") return BoxValue(__vwsn::Parse<vuint16_t>(GetJsonNumber(item)));
						if (keyword == L"UInt32") return BoxValue(__vwsn::Parse<vuint32_t>(GetJsonNumber(item)));
						if (keyword == L"UInt64") return BoxValue(__vwsn::Parse<vuint64_t>(GetJsonNumber(item)));
						if (keyword == L"Int8") return BoxValue(__vwsn::Parse<vint8_t>(GetJsonNumber(item)));
						if (keyword == L"Int16") return BoxValue(__vwsn::Parse<vint16_t>(GetJsonNumber(item)));
						if (keyword == L"Int32") return BoxValue(__vwsn::Parse<vint32_t>(GetJsonNumber(item)));
						if (keyword == L"Int64") return BoxValue(__vwsn::Parse<vint64_t>(GetJsonNumber(item)));
						if (keyword == L"Single") return BoxValue(__vwsn::Parse<float>(GetJsonNumber(item)));
						if (keyword == L"Double") return BoxValue(__vwsn::Parse<double>(GetJsonNumber(item)));
						if (keyword == L"Char") return BoxValue(__vwsn::Parse<wchar_t>(GetJsonString(item)));
						if (keyword == L"DateTime") return BoxValue(__vwsn::Parse<DateTime>(GetJsonString(item)));
						if (keyword == L"Locale") return BoxValue(__vwsn::Parse<Locale>(GetJsonString(item)));
					}
				}

				auto result = IValueList::Create();
				for (auto item : array->items)
				{
					result->Add(rpcjson_Deserialize(item));
				}
				return BoxValue(result);
			}

			if (auto object = node.Cast<JsonObject>())
			{
				if (auto keywordNode = FindJsonObjectField(object, WString::Unmanaged(L"$")))
				{
					auto keyword = GetJsonString(keywordNode);
					if (auto value = TryDeserializeRpcTypes(object, keyword); !value.IsNull()) return value;
					if (keyword == L"list") return DeserializeList(object, false, rpcjson_Deserialize);
					if (keyword == L"oblist") return DeserializeList(object, true, rpcjson_Deserialize);
					if (keyword == L"map") return DeserializeMap(object, rpcjson_Deserialize);
					CHECK_FAIL(L"Unknown RPC JSON object schema.");
				}

				auto result = IValueDictionary::Create();
				for (auto field : object->fields)
				{
					result->Set(BoxValue(field->name.value), rpcjson_Deserialize(field->value));
				}
				return BoxValue(result);
			}

			CHECK_FAIL(L"Unsupported RPC JSON node.");
		}

/***********************************************************************
* RpcJsonObjectOps
***********************************************************************/

		RpcJsonObjectOps::RpcJsonObjectOps(IRpcJsonMessageDispatcher* _dispatcher)
			: dispatcher(_dispatcher)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcJsonObjectOps::RpcJsonObjectOps(...)#"
			CHECK_ERROR(dispatcher, ERROR_MESSAGE_PREFIX L"Dispatcher is required.");
#undef ERROR_MESSAGE_PREFIX
		}

		RpcJsonObjectOps::RpcJsonObjectOps(vint _sourceClientId, vint _targetClientId, IRpcJsonMessageDispatcher* _dispatcher)
			: sourceClientId(_sourceClientId)
			, targetClientId(_targetClientId)
			, dispatcher(_dispatcher)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcJsonObjectOps::RpcJsonObjectOps(...)#"
			CHECK_ERROR(dispatcher, ERROR_MESSAGE_PREFIX L"Dispatcher is required.");
#undef ERROR_MESSAGE_PREFIX
		}

		RpcJsonObjectOps::~RpcJsonObjectOps()
		{
		}

		Value RpcJsonObjectOps::InvokeMethod(RpcObjectReference ref, vint methodId, Ptr<IValueArray> arguments)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcJsonObjectOps::InvokeMethod(RpcObjectReference, vint, Ptr<IValueArray>)#"
			CHECK_ERROR(dispatcher, ERROR_MESSAGE_PREFIX L"Dispatcher is required.");
			auto request = CreateRpcMessage(WString::Unmanaged(L"Request:IObjectOps_InvokeMethod"), dispatcher->AllocateRequestId(), sourceClientId);
			AddJsonObjectField(request, WString::Unmanaged(L"targetClientId"), CreateJsonNumber(targetClientId == RpcClientId_Invalid ? ref.clientId : targetClientId));
			AddJsonObjectField(request, WString::Unmanaged(L"ref"), CreateRpcObjectReferenceJson(ref));
			AddJsonObjectField(request, WString::Unmanaged(L"methodId"), CreateJsonNumber(methodId));
			AddJsonObjectField(request, WString::Unmanaged(L"arguments"), ValueArrayToJsonArray(arguments));

			auto response = GetJsonObject(dispatcher->OnJsonRequest(request, false));
			CHECK_ERROR(GetJsonString(GetJsonObjectField(response, WString::Unmanaged(L"rpcMethod"))) == WString::Unmanaged(L"Response:IObjectOps_InvokeMethod"), ERROR_MESSAGE_PREFIX L"Unexpected response method.");
			CHECK_ERROR(ReadRequestId(response) == ReadRequestId(request), ERROR_MESSAGE_PREFIX L"Unexpected response request id.");
			return JsonResponseToMethodResult(GetJsonObjectField(response, WString::Unmanaged(L"response")));
#undef ERROR_MESSAGE_PREFIX
		}

		void RpcJsonObjectOps::EndInvokeMethod(vint slot)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcJsonObjectOps::EndInvokeMethod(vint)#"
			CHECK_ERROR(dispatcher, ERROR_MESSAGE_PREFIX L"Dispatcher is required.");
			auto request = CreateRpcMessage(WString::Unmanaged(L"Request:IObjectOps_EndInvokeMethod"), dispatcher->AllocateRequestId(), sourceClientId);
			AddJsonObjectField(request, WString::Unmanaged(L"targetClientId"), CreateJsonNumber(targetClientId));
			AddJsonObjectField(request, WString::Unmanaged(L"slot"), CreateJsonNumber(slot));

			auto response = GetJsonObject(dispatcher->OnJsonRequest(request, false));
			CHECK_ERROR(GetJsonString(GetJsonObjectField(response, WString::Unmanaged(L"rpcMethod"))) == WString::Unmanaged(L"Response:IObjectOps_EndInvokeMethod"), ERROR_MESSAGE_PREFIX L"Unexpected response method.");
			CHECK_ERROR(ReadRequestId(response) == ReadRequestId(request), ERROR_MESSAGE_PREFIX L"Unexpected response request id.");
#undef ERROR_MESSAGE_PREFIX
		}

		void RpcJsonObjectOps::ObjectHold(RpcObjectReference ref, vint remoteClientId, bool hold)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcJsonObjectOps::ObjectHold(RpcObjectReference, vint, bool)#"
			CHECK_ERROR(dispatcher, ERROR_MESSAGE_PREFIX L"Dispatcher is required.");
			auto request = CreateRpcMessage(WString::Unmanaged(L"Request:IObjectOps_ObjectHold"), dispatcher->AllocateRequestId(), sourceClientId == RpcClientId_Invalid ? remoteClientId : sourceClientId);
			AddJsonObjectField(request, WString::Unmanaged(L"targetClientId"), CreateJsonNumber(targetClientId == RpcClientId_Invalid ? ref.clientId : targetClientId));
			AddJsonObjectField(request, WString::Unmanaged(L"ref"), CreateRpcObjectReferenceJson(ref));
			AddJsonObjectField(request, WString::Unmanaged(L"remoteClientId"), CreateJsonNumber(remoteClientId));
			AddJsonObjectField(request, WString::Unmanaged(L"hold"), CreateJsonBool(hold));

			auto response = GetJsonObject(dispatcher->OnJsonRequest(request, false));
			CHECK_ERROR(GetJsonString(GetJsonObjectField(response, WString::Unmanaged(L"rpcMethod"))) == WString::Unmanaged(L"Response:IObjectOps_ObjectHold"), ERROR_MESSAGE_PREFIX L"Unexpected response method.");
			CHECK_ERROR(ReadRequestId(response) == ReadRequestId(request), ERROR_MESSAGE_PREFIX L"Unexpected response request id.");
#undef ERROR_MESSAGE_PREFIX
		}

		Ptr<JsonNode> RpcJsonObjectOps::Translate(Ptr<JsonNode> message, IRpcObjectOps* ops, IRpcLifecycle* lifecycle)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcJsonObjectOps::Translate(Ptr<JsonNode>, IRpcObjectOps*)#"
			(void)lifecycle;
			CHECK_ERROR(ops, ERROR_MESSAGE_PREFIX L"Object ops is required.");
			auto request = GetJsonObject(message);
			auto rpcMethod = GetJsonString(GetJsonObjectField(request, WString::Unmanaged(L"rpcMethod")));
			auto requestId = ReadRequestId(request);
			auto sourceClientId = ReadSourceClientId(request);
			auto targetClientId = ReadTargetClientId(request);
			auto responseMethod =
				rpcMethod == WString::Unmanaged(L"Request:IObjectOps_InvokeMethod") ? WString::Unmanaged(L"Response:IObjectOps_InvokeMethod") :
				rpcMethod == WString::Unmanaged(L"Request:IObjectOps_EndInvokeMethod") ? WString::Unmanaged(L"Response:IObjectOps_EndInvokeMethod") :
				rpcMethod == WString::Unmanaged(L"Request:IObjectOps_ObjectHold") ? WString::Unmanaged(L"Response:IObjectOps_ObjectHold") :
				WString::Empty;
			CHECK_ERROR(responseMethod != WString::Empty, ERROR_MESSAGE_PREFIX L"Unexpected RPC method.");
			auto response = CreateRpcMessage(responseMethod, requestId, lifecycle ? lifecycle->GetClientId() : targetClientId);
			AddJsonObjectField(response, WString::Unmanaged(L"targetClientId"), CreateJsonNumber(sourceClientId));

			if (rpcMethod == WString::Unmanaged(L"Request:IObjectOps_InvokeMethod"))
			{
				auto result = ops->InvokeMethod(
					GetRpcObjectReferenceFromJson(GetJsonObjectField(request, WString::Unmanaged(L"ref"))),
					GetJsonInt(GetJsonObjectField(request, WString::Unmanaged(L"methodId"))),
					JsonArrayToValueArray(GetJsonObjectField(request, WString::Unmanaged(L"arguments")))
					);
				AddJsonObjectField(response, WString::Unmanaged(L"response"), MethodResultToJsonResponse(result));
			}
			else if (rpcMethod == WString::Unmanaged(L"Request:IObjectOps_EndInvokeMethod"))
			{
				ops->EndInvokeMethod(GetJsonInt(GetJsonObjectField(request, WString::Unmanaged(L"slot"))));
			}
			else if (rpcMethod == WString::Unmanaged(L"Request:IObjectOps_ObjectHold"))
			{
				ops->ObjectHold(
					GetRpcObjectReferenceFromJson(GetJsonObjectField(request, WString::Unmanaged(L"ref"))),
					GetJsonInt(GetJsonObjectField(request, WString::Unmanaged(L"remoteClientId"))),
					GetJsonBool(GetJsonObjectField(request, WString::Unmanaged(L"hold")))
					);
			}
			else
			{
				CHECK_FAIL(ERROR_MESSAGE_PREFIX L"Unexpected RPC method.");
			}
			return response;
#undef ERROR_MESSAGE_PREFIX
		}

/***********************************************************************
* RpcJsonObjectEventOps
***********************************************************************/

		RpcJsonObjectEventOps::RpcJsonObjectEventOps(IRpcJsonMessageDispatcher* _dispatcher)
			: dispatcher(_dispatcher)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcJsonObjectEventOps::RpcJsonObjectEventOps(...)#"
			CHECK_ERROR(dispatcher, ERROR_MESSAGE_PREFIX L"Dispatcher is required.");
#undef ERROR_MESSAGE_PREFIX
		}

		RpcJsonObjectEventOps::RpcJsonObjectEventOps(vint _sourceClientId, IRpcJsonMessageDispatcher* _dispatcher)
			: sourceClientId(_sourceClientId)
			, dispatcher(_dispatcher)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcJsonObjectEventOps::RpcJsonObjectEventOps(...)#"
			CHECK_ERROR(dispatcher, ERROR_MESSAGE_PREFIX L"Dispatcher is required.");
#undef ERROR_MESSAGE_PREFIX
		}

		RpcJsonObjectEventOps::~RpcJsonObjectEventOps()
		{
		}

		Value RpcJsonObjectEventOps::InvokeEvent(RpcObjectReference ref, vint eventId, Ptr<IValueArray> arguments)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcJsonObjectEventOps::InvokeEvent(RpcObjectReference, vint, Ptr<IValueArray>)#"
			CHECK_ERROR(dispatcher, ERROR_MESSAGE_PREFIX L"Dispatcher is required.");
			auto request = CreateRpcMessage(WString::Unmanaged(L"Request:IObjectEventOps_InvokeEvent"), dispatcher->AllocateRequestId(), sourceClientId);
			AddJsonObjectField(request, WString::Unmanaged(L"ref"), CreateRpcObjectReferenceJson(ref));
			AddJsonObjectField(request, WString::Unmanaged(L"eventId"), CreateJsonNumber(eventId));
			AddJsonObjectField(request, WString::Unmanaged(L"arguments"), ValueArrayToJsonArray(arguments));

			auto response = GetJsonObject(dispatcher->OnJsonRequest(request, true));
			CHECK_ERROR(GetJsonString(GetJsonObjectField(response, WString::Unmanaged(L"rpcMethod"))) == WString::Unmanaged(L"Response:Broadcast_Response"), ERROR_MESSAGE_PREFIX L"Unexpected response method.");
			CHECK_ERROR(ReadRequestId(response) == ReadRequestId(request), ERROR_MESSAGE_PREFIX L"Unexpected response request id.");
			return BoxValue(CreateSerializedEventExceptionMap(GetJsonObjectField(response, WString::Unmanaged(L"response"))));
#undef ERROR_MESSAGE_PREFIX
		}

		Ptr<JsonNode> RpcJsonObjectEventOps::Translate(Ptr<JsonNode> message, IRpcObjectEventOps* ops, IRpcLifecycle* lifecycle)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcJsonObjectEventOps::Translate(Ptr<JsonNode>, IRpcObjectEventOps*)#"
			CHECK_ERROR(ops, ERROR_MESSAGE_PREFIX L"Object event ops is required.");
			auto request = GetJsonObject(message);
			auto rpcMethod = GetJsonString(GetJsonObjectField(request, WString::Unmanaged(L"rpcMethod")));
			CHECK_ERROR(rpcMethod == WString::Unmanaged(L"Request:IObjectEventOps_InvokeEvent"), ERROR_MESSAGE_PREFIX L"Unexpected RPC method.");
			auto requestId = ReadRequestId(request);
			auto sourceClientId = ReadSourceClientId(request);
			auto response = CreateRpcMessage(WString::Unmanaged(L"Response:Broadcast_Response"), requestId, lifecycle ? lifecycle->GetClientId() : RpcClientId_Invalid);
			AddJsonObjectField(response, WString::Unmanaged(L"targetClientId"), CreateJsonNumber(sourceClientId));

			auto result = ops->InvokeEvent(
				GetRpcObjectReferenceFromJson(GetJsonObjectField(request, WString::Unmanaged(L"ref"))),
				GetJsonInt(GetJsonObjectField(request, WString::Unmanaged(L"eventId"))),
				JsonArrayToValueArray(GetJsonObjectField(request, WString::Unmanaged(L"arguments")))
				);
			AddJsonObjectField(response, WString::Unmanaged(L"response"), CreateEventExceptionResponse(ValueToJsonNode(result)));
			return response;
#undef ERROR_MESSAGE_PREFIX
		}

/***********************************************************************
* RpcJsonDispatcher
***********************************************************************/

		RpcJsonDispatcher::RpcJsonDispatcher(vint _sourceClientId, IRpcJsonMessageDispatcher* _dispatcher)
			: sourceClientId(_sourceClientId)
			, dispatcher(_dispatcher)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcJsonDispatcher::RpcJsonDispatcher(vint, IRpcJsonMessageDispatcher*)#"
			CHECK_ERROR(sourceClientId != RpcClientId_Invalid, ERROR_MESSAGE_PREFIX L"Source client id is required.");
			CHECK_ERROR(dispatcher, ERROR_MESSAGE_PREFIX L"Dispatcher is required.");
#undef ERROR_MESSAGE_PREFIX
		}

		void RpcJsonDispatcher::Finalize()
		{
		}

		void RpcJsonDispatcher::Initialize()
		{
		}

		void RpcJsonDispatcher::DeclareLocalService(vint typeId, vint clientId)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcJsonDispatcher::DeclareLocalService(vint, vint)#"
			CHECK_ERROR(dispatcher, ERROR_MESSAGE_PREFIX L"Dispatcher is required.");
			CHECK_ERROR(clientId == sourceClientId, ERROR_MESSAGE_PREFIX L"Client id does not match this dispatcher.");
			auto request = CreateRpcMessage(WString::Unmanaged(L"Request:IRpcDispatcher_DeclareLocalService"), dispatcher->AllocateRequestId(), sourceClientId);
			AddJsonObjectField(request, WString::Unmanaged(L"typeId"), CreateJsonNumber(typeId));
			AddJsonObjectField(request, WString::Unmanaged(L"clientId"), CreateJsonNumber(clientId));

			auto response = GetJsonObject(dispatcher->OnJsonRequest(request, true));
			CHECK_ERROR(GetJsonString(GetJsonObjectField(response, WString::Unmanaged(L"rpcMethod"))) == WString::Unmanaged(L"Response:Broadcast_Response"), ERROR_MESSAGE_PREFIX L"Unexpected response method.");
			CHECK_ERROR(ReadRequestId(response) == ReadRequestId(request), ERROR_MESSAGE_PREFIX L"Unexpected response request id.");
#undef ERROR_MESSAGE_PREFIX
		}

		IRpcObjectEventOps* RpcJsonDispatcher::BroadcastFromClient_ObjectEventOps(vint selfClientId)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcJsonDispatcher::BroadcastFromClient_ObjectEventOps(vint)#"
			CHECK_ERROR(selfClientId == sourceClientId, ERROR_MESSAGE_PREFIX L"Client id does not match this dispatcher.");
			if (!objectEventOps)
			{
				objectEventOps = Ptr(new RpcJsonObjectEventOps(sourceClientId, dispatcher));
			}
			return objectEventOps.Obj();
#undef ERROR_MESSAGE_PREFIX
		}

		IRpcObjectOps* RpcJsonDispatcher::SendToClient_ObjectOps(vint targetClientId)
		{
			if (auto index = objectOps.Keys().IndexOf(targetClientId); index != -1)
			{
				return objectOps.Values()[index].Obj();
			}

			auto ops = Ptr(new RpcJsonObjectOps(sourceClientId, targetClientId, dispatcher));
			objectOps.Set(targetClientId, ops);
			return ops.Obj();
		}

		Ptr<JsonNode> RpcJsonDispatcher::Translate(Ptr<JsonNode> message, IRpcDispatcher* dispatcher, IRpcLifecycle* lifecycle)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcJsonDispatcher::Translate(Ptr<JsonNode>, IRpcDispatcher*, IRpcLifecycle*)#"
			(void)dispatcher;
			CHECK_ERROR(lifecycle, ERROR_MESSAGE_PREFIX L"Lifecycle is required.");
			auto request = GetJsonObject(message);
			auto rpcMethod = GetJsonString(GetJsonObjectField(request, WString::Unmanaged(L"rpcMethod")));
			CHECK_ERROR(rpcMethod == WString::Unmanaged(L"Request:IRpcDispatcher_DeclareLocalService"), ERROR_MESSAGE_PREFIX L"Unexpected RPC method.");
			auto requestId = ReadRequestId(request);
			auto sourceClientId = ReadSourceClientId(request);
			auto typeId = GetJsonInt(GetJsonObjectField(request, WString::Unmanaged(L"typeId")));
			auto clientId = GetJsonInt(GetJsonObjectField(request, WString::Unmanaged(L"clientId")));
			CHECK_ERROR(clientId == sourceClientId, ERROR_MESSAGE_PREFIX L"Client id does not match the message source.");

			lifecycle->DeclareRemoteService(typeId, clientId);

			auto response = CreateRpcMessage(WString::Unmanaged(L"Response:Broadcast_Response"), requestId, lifecycle->GetClientId());
			AddJsonObjectField(response, WString::Unmanaged(L"targetClientId"), CreateJsonNumber(sourceClientId));
			return response;
#undef ERROR_MESSAGE_PREFIX
		}

/***********************************************************************
* RpcJsonLifecycle
***********************************************************************/

		RpcJsonLifecycle::RpcJsonLifecycle(vint _clientId, RpcJsonDispatcher* _dispatcher)
			: RpcLifecycleBase(_clientId)
			, dispatcher(_dispatcher)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcJsonLifecycle::RpcJsonLifecycle(vint, RpcJsonDispatcher*)#"
			CHECK_ERROR(dispatcher, ERROR_MESSAGE_PREFIX L"Dispatcher is required.");
#undef ERROR_MESSAGE_PREFIX
		}

		void RpcJsonLifecycle::Register(
			Ptr<IRpcSerializer> _serializer,
			Ptr<IRpcObjectOps> _objectOps,
			Ptr<IRpcObjectEventOps> _objectEventOps,
			Func<vint(IDescriptable*)> _getTypeId,
			Func<void(RpcObjectReference, IDescriptable*)> _eventAttacher
			)
		{
			serializer = _serializer;
			getTypeId = _getTypeId;
			eventAttacher = _eventAttacher;
			listOps = Ptr(new RpcCalleeListOps(this, serializer.Obj()));
			listEventOps = Ptr(new RpcCalleeListEventOps(this, serializer.Obj()));
			objectOpsForList = Ptr(new RpcCalleeObjectOpsForList(listOps, _objectOps, serializer.Obj()));
			objectEventOpsForList = Ptr(new RpcCalleeObjectEventOpsForList(listEventOps, _objectEventOps, serializer.Obj()));
			GetController()->Register(objectOpsForList, objectEventOpsForList);
		}

		vint RpcJsonLifecycle::DecideTypeId(IDescriptable* obj)const
		{
			auto result = RpcLifecycleBase::DecideTypeId(obj);
			if (result != RpcTypeId_NotFound) return result;
			return getTypeId ? getTypeId(obj) : RpcTypeId_NotFound;
		}

		IRpcSerializer* RpcJsonLifecycle::GetSerializer()
		{
			return serializer.Obj();
		}

		IRpcDispatcher* RpcJsonLifecycle::GetDispatcher()
		{
			return dispatcher;
		}

		void RpcJsonLifecycle::AttachLocalObjectEvents(RpcObjectReference ref, IDescriptable* obj)
		{
			if (eventAttacher)
			{
				eventAttacher(ref, obj);
			}
		}

/***********************************************************************
* Request Id
***********************************************************************/

		vint ReadRequestId(Ptr<JsonNode> message)
		{
			return GetJsonInt(GetJsonObjectField(GetJsonObject(message), WString::Unmanaged(L"rpcRequestId")));
		}

		void WriteRequestId(Ptr<JsonNode> message, vint requestId)
		{
			SetJsonObjectField(GetJsonObject(message), WString::Unmanaged(L"rpcRequestId"), CreateJsonNumber(requestId));
		}
	}
}
