# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

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

These functions are not used yet, so there will be no way to know if they function or not, but running current unit test you are only able to know if they compile. Actual testing will be done in the future, but try your best to make a correct Workflow script.

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

`Schema` will be typing the result from the generated `rpcjson_Serialize` function.

All structs are named after `StructSchema_Namespace_Name_To_Type`.

You are going to create a function in `WfAnalyzer_ValidateRpc.cpp` after `ValidateModuleRPC`, say `GenerateDtsFromRpcMetadata`. And put a `WString dts;` in `WfLexicalScopeManager::rpcMetadata`, therefore you are able to save that file in `CompilerTest_LoadAndCompile`.

# UPDATES

## Task 1

- Added generated Workflow helpers in `WfAnalyzer_GenerateRpc.cpp` for RPC JSON serialization and deserialization:
  - `rpcjson_Serialize(value : object) : system::JsonNode^`
  - `rpcjson_Deserialize(node : system::JsonNode^) : object`
- The generated script now handles:
  - null as JSON null
  - predefined primitive values as `["Keyword", "string"]`
  - enum values as `["full type name", number]`
  - struct values, including `system::RpcObjectReference`, as objects with `$` type names
  - list, observable-list, and map-like collections as typed objects with `values`
- Added metadata collection for all enum and struct declarations in the RPC metadata script, with a duplicate guard for `RpcObjectReference`.
- Added JSON token construction helpers because `JsonString.content`, `JsonNumber.content`, and `JsonObjectField.name` are `system::ParsingToken` values, not raw strings.
- Added `../../Import/VlppGlrParser.h` to generated RPC C++ wrapper includes in `TestRpcCompile.cpp`, because the generated wrappers now mention JSON parser runtime types.

## Task 2

- Added `Metadata_<sample>.txt` emission in `TestRpcCompile.cpp`, printing `manager.rpcMetadata->metadataModule` into `Test\Generated\RpcMetadata32` or `Test\Generated\RpcMetadata64` beside the existing wrapper script output.
- Kept the existing runtime metadata baseline output unchanged.

## Task 3

- Added `WfRpcMetadata::dts` and `GenerateDtsFromRpcMetadata(manager)` so RPC validation now builds a TypeScript declaration for the generated JSON serialization schema.
- The DTS generator emits:
  - `PredefinedSchema` primitive tuples matching `rpcjson_Serialize`
  - `EnumSchema` tuple unions for RPC metadata enums
  - `Schema` as the exported top-level serialized JSON shape
  - `ListSchema`, `MapSchema`, and `ObservableSchema` interfaces
  - `StructSchema_Namespace_Name` interfaces for RPC metadata structs, including `system::RpcObjectReference` once
- Added `Serialization_<sample>.d.ts` emission in `TestRpcCompile.cpp` for both `Test\Generated\RpcMetadata32` and `Test\Generated\RpcMetadata64`.

# TEST

## Task 1

All commands passed.

- `copilotBuild.ps1 -Configuration Debug -Platform x64`
- `copilotBuild.ps1 -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable LibraryTest -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_GenerateMetadata -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_LoadAndCompile -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Reflection -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Metaonly -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable RuntimeTest -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable LibraryTest -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_GenerateMetadata -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_LoadAndCompile -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Reflection -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Metaonly -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable RuntimeTest -Configuration Debug -Platform Win32`

## Task 2

All commands passed.

- `copilotBuild.ps1 -Configuration Debug -Platform x64`
- `copilotBuild.ps1 -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_LoadAndCompile -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_LoadAndCompile -Configuration Debug -Platform Win32`
- `copilotBuild.ps1 -Configuration Debug -Platform x64`
- `copilotBuild.ps1 -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable LibraryTest -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_GenerateMetadata -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Reflection -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Metaonly -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable RuntimeTest -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable LibraryTest -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_GenerateMetadata -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Reflection -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Metaonly -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable RuntimeTest -Configuration Debug -Platform Win32`

## Task 3

All commands passed.

- `copilotBuild.ps1 -Configuration Debug -Platform x64`
- `copilotBuild.ps1 -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_LoadAndCompile -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_LoadAndCompile -Configuration Debug -Platform Win32`
- `copilotBuild.ps1 -Configuration Debug -Platform x64`
- `copilotBuild.ps1 -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable LibraryTest -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_GenerateMetadata -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Reflection -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Metaonly -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable RuntimeTest -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable LibraryTest -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_GenerateMetadata -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Reflection -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Metaonly -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable RuntimeTest -Configuration Debug -Platform Win32`

# PROPOSALS
