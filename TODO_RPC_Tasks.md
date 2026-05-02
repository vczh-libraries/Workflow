# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Task 1

I would like you to implement JSON serialization for RPC awared primitive types.
In generated wrapper Workflow script for RPC, create following functions:

1) `func rpcjson_Serialize(value : object) : system::JsonNode^`
2) `func rpcjson_Deserialize(node : system::JsonNode^) : object`

For all predefined primitive types, they can be translated to string by using `cast string`, and they can be casted back, so the JSON representation would be: `["keyword", "string"]`. The keyword would be primitive names without `system::`, for example, when building Workflow in x64, `int` becomes `Int64`. Those names could be seen in `VlppReflection.cpp:4987`.

For `null`, use the JSON null value.

For enum, use `["enum type full name", number]`.

For struct, use `{ "$":"struct type full name", field:value, ...}`. Including `RpcObjectReference`.

enum and struct here includes every types that are in the rpc metadata workflow script, not just new types defined in this module. Be careful if the wrapper already include `RpcObjectReference` you don't generate double `RpcObjectReference` serialization.

For collections, use `{ "$":"list/map/observable-list", values:[...]]`. List becomes an array, dictionary becomes an array of two-element-array for key and value.

In the serialize function, we first test if it is null, and then try to cast it to all collection interfaces, and then try to cast it to all `T?` for any primitive type `T` so that it could always succeed but you know it is null when it returns null.

In the deserialization function, the JSON format already have type information embedded in them, it should be easy.

## Task 2

Currently only wrapper Workflow scripts are printed to Test\Generated\RpcMetadata(32|64), I would like you to also print the metadata Workflow scripts, which is created in the `WfLexicalScopeManager::rpcMetadata`, also to these folder but with the `Metadata_` prefix.

## Task 3

Also in `RpcMetadata(32|64)` project, generates `.d.ts` files for serialization, the structure like:
```
export type Schema =
  | predefined types ...
  ...
  | ListSchema
  | MapSchema
  | ObservableSchema
  | struct1
  | ...
  ;

interface for ListSchema, MapSchema, ObservableSchema, struct1, ...
```

All structs are named after `StructSchema_Namespace_Name_To_Type`.

You are going to create a function in `WfAnalyzer_ValidateRpc.cpp` after `ValidateModuleRPC`, say `GenerateDtsFromRpcMetadata`. And put a `WString dts;` in `WfLexicalScopeManager::rpcMetadata`, therefore you are able to save that file in `CompilerTest_LoadAndCompile`.