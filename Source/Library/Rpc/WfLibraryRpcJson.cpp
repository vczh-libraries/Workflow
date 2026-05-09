#include "WfLibraryRpcJson.h"
#include "../WfLibraryCppHelper.h"

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

			void AddJsonObjectField(Ptr<JsonObject> object, const WString& name, Ptr<JsonNode> value)
			{
				auto field = Ptr(new JsonObjectField);
				field->name.value = name;
				field->value = value;
				object->fields.Add(field);
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
					return BoxRpcObjectReference(RpcObjectReference{
						__vwsn::Parse<vint>(GetJsonNumber(GetJsonObjectField(object, WString::Unmanaged(L"clientId")))),
						__vwsn::Parse<vint>(GetJsonNumber(GetJsonObjectField(object, WString::Unmanaged(L"objectId")))),
						__vwsn::Parse<vint>(GetJsonNumber(GetJsonObjectField(object, WString::Unmanaged(L"typeId")))),
						});
				}
				if (keyword == L"system::RpcException")
				{
					return BoxRpcException(RpcException{
						GetJsonString(GetJsonObjectField(object, WString::Unmanaged(L"message"))),
						});
				}
				return {};
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
	}
}
