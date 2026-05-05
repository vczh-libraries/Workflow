@ -1,335 +0,0 @@
# Workflow Interface Based RPC (JSON Serialization Schema)

There are two kinds of JSON schema for RPC serializable types:
- Schema for known types. They are used when the context gives you enough information therefore you know what type is expected without having to look into actual values, including `object` or `system::JsonNode`.
  - Functions in wrapper Workflow Script:
    - `func rpcjson_Serialize_Enum_Type_Full_Name(value : enum::type::full::name) : (system::JsonNode^)`
    - `func rpcjson_Serialize_Struct_Type_Full_Name(value : struct::type::full::name) : (system::JsonNode^)`
    - `rpcjson_Deserialize_*` in reversed form.
- Schema for unknown types.
  - Typically needed when access elements in collections transferred using `@rpc:Byref`.
  - Functions in wrapper Workflow Script:
    - `func rpcjson_Serialize(value : object) : (system::JsonNode^)`
    - `func rpcjson_Deserialize(node : system::JsonNode^) : (object)`

Pay attention to the `RpcObjectReference` type, it is treated like a struct. But serialization for `RpcObjectReference` is always needed.
Types listed here should include every typed appear in `WfLexicalScopeManager::rpcMetadata`, but when `RpcObjectReference` also appears in the list, do not duplicate `RpcObjectReference` processing.

## Schema for Known Types

1) primitive types

Use the second element in `UnknownType_PrimitiveSchema`.

2) eum types

Use `number`.

3) struct types

`{ field: value, ...}`
- `field` is a string, which is required in JSON anyway.
- `value` is schema for known types.

4) collection types

For list: array of schema for known types.
For dictionary: array of `[key, value]`, both are schema for known types.

## Schema for Unknown Types

1) primitive types

```TypeScript
export type UnknownType_PrimitiveSchema =
  | ["UInt8", number]
  | ["UInt16", number]
  | ["UInt32", number]
  | ["UInt64", number]
  | ["Int8", number]
  | ["Int16", number]
  | ["Int32", number]
  | ["Int64", number]
  | ["Single", number]
  | ["Double", number]
  | ["Boolean", boolean]
  | ["Char", string]
  | ["String", string]
  | ["DateTime", string]
  | ["Locale", string]
  | null
  ;
```

2) enum types

`["enum::type::full::name", number]`.

3) struct types

`{ "$": "struct::type::full::name", strong-type-version-of-this-struct...}`

4) collection types

`{ "$": "list" | "map" | "oblist", value: [elements ...]}`
- For list, put each elements in the list.
- For dictionary, put key1, value1, key2, value2, ...

## Implementation of rpcjson_Serialize

Check if the value is null or bool or string, they can be serialized directly.
And then try to weak cast it to any `PrimitiveType?` type.
And then try to weak cast it to any collection interface type.
And then try to weak cast it to any `InterfaceType^` type.

Weak casting failure results in `null` instead of an exception.
Throw an exception if all type testings fail.

## Implementation of rpcjson_Deserialize

First check if it is `JsonArray` or `JsonObject` or `JsonLiteral` or `JsonString`,
and then check the first element of array or "$" field of object,
and we know the exact type.
Throw an exception if all type testings fail.

## JSON Return Values for Byval Collections

When an RPC method returns an `@rpc:Byval` collection, JSON serialization still uses the same known-type or unknown-type schema for the collection value. The generated object ops put the resulting `JsonNode` in `system::RpcByvalReturnValue.value` and use `system::RpcByvalReturnValue.slot` to keep the recursive copied collection alive until the caller calls `EndInvokeMethod(slot)`. Non-byval JSON returns still return the `JsonNode` directly.

## Other Strict Rules

DO NOT generate any helper function that is not mentioned here, especially which just builds any `system::JsonNode^`.
You are going to repeat JSON AST building code in each function.
For JSON related functions, stick to the list in the beginning of this document.


## Expected format of generated .d.ts files

```TypeScript
export type UnknownType_PrimitiveSchema =
  | ["UInt8", number]
  | ["UInt16", number]
  | ["UInt32", number]
  | ["UInt64", number]
  | ["Int8", number]
  | ["Int16", number]
  | ["Int32", number]
  | ["Int64", number]
  | ["Single", number]
  | ["Double", number]
  | ["Char", string]
  | ["DateTime", string]
  | ["Locale", string]
  | null
  | true
  | false
  | string
  ;

export type TypeList_Enum =
  | "enum::type::full::name"
  ...
  ;

export type UnknownType_EnumSchema = [TypeList_Enum, number];

export interface UnknownType_List
{
  "$": "list" | "map" | "oblist";
  values: UnknownTypeSchema[];
}

export interface UnknownType_struct_type_full_name : extends struct_type_full_name
{
  "$": "struct::type::full::name",
}

export type UnknownTypeSchema =
  | UnknownType_PrimitiveSchema
  | UnknownType_EnumSchema
  | UnknownType_List
  | UnknownType_struct_type_full_name
  | ...
  ...

// below are all known types

export enum enum_type_full_name
{
  item: number-literal,
  ...
}

export interface struct_type_full_name
{
  field: value_type;
}
```
