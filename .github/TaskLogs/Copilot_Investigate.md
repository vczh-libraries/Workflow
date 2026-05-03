# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Task 1

Previously we have done JSON serialization functions in generated wrapper Workflow scripts, as well as .d.ts schema.
Your goal is to follow `TODO_RPC_Json.md` to rework them.
There is a super big design change. So in the `### Original Request` I pasted the request for the existing implementation.

### Original Request

**IMPORTANT**: Everything below are already done and subject to rework. DO NOT mix them into the goal of the task.

> I would like you to implement JSON serialization for RPC awared primitive types.
> In generated wrapper Workflow script for RPC, create following functions:
>
> 1) `func rpcjson_Serialize(value : object) : system::JsonNode^`
> 2) `func rpcjson_Deserialize(node : system::JsonNode^) : object`
>
> For all predefined primitive types, they can be translated to string by using `cast string`, and they can be casted back, so the JSON representation would be: `["keyword", "string"]`. The keyword would be primitive names without `system::`, for example, when building Workflow in x64, `int` becomes `Int64`. Those names could be seen in `VlppReflection.cpp:4987`.
>
> For `null`, use the JSON null value.
>
> For enum, use `["enum type full name", number]`.
>
> For struct, use `{ "$":"struct type full name", field:value, ...}`. Including `RpcObjectReference`.
>
> enum and struct here includes every types that are in the rpc metadata workflow script, not just new types defined in this module. Be careful if the wrapper already include `RpcObjectReference` you don't generate double `RpcObjectReference` serialization.
>
> For collections, use `{ "$":"list/map/observable-list", values:[...]]`. List becomes an array, dictionary becomes an array of two-element-array for key and value.
>
> In the serialize function, we first test if it is null, and then try to cast it to all collection interfaces, and then try to cast it to all `T?` for any primitive type `T` so that it could always succeed but you know it is null when it returns null.
>
> In the deserialization function, the JSON format already have type information embedded in them, it should be easy.
>
> These functions are not used yet, so there will be no way to know if they function or not, but running current unit test you are only able to know if they compile. Actual testing will be done in the future, but try your best to make a correct Workflow script.
>
> Currently only wrapper Workflow scripts are printed to Test\Generated\RpcMetadata(32|64), I would like you to also print the metadata Workflow scripts, which is created in the `WfLexicalScopeManager::rpcMetadata`, also to these folder but with the `Metadata_` prefix.
>
> Also in `RpcMetadata(32|64)` project, generates `.d.ts` files for serialization, the structure like:
> ```
> export type Schema =
>   | predefined types ...
>   ...
>   | ListSchema
>   | MapSchema
>   | ObservableSchema
>   | struct1
>   | ...
>   ;
>
> interface for ListSchema, MapSchema, ObservableSchema, struct1, ...
> ```
>
> `Schema` will be typing the result from the generated `rpcjson_Serialize` function.
>
> All structs are named after `StructSchema_Namespace_Name_To_Type`.
>
> You are going to create a function in `WfAnalyzer_ValidateRpc.cpp` after `ValidateModuleRPC`, say `GenerateDtsFromRpcMetadata`. And put a `WString dts;` in `WfLexicalScopeManager::rpcMetadata`, therefore you are able to save that file in `CompilerTest_LoadAndCompile`.

# UPDATES

- Reworked generated RPC JSON Workflow support according to `TODO_RPC_Json.md`.
  - Known schemas now serialize primitives directly, enums as numbers, structs as plain objects, lists/observable lists as arrays, and dictionaries as arrays of key/value tuples.
  - Unknown schemas now serialize `null`, booleans, and strings as direct JSON literals, numeric primitive values as tagged tuples, enums as tagged number tuples, structs as objects with `$`, collections as `{ "$": "list" | "map" | "oblist", values: [...] }`, and maps with flat alternating key/value entries.
  - Generated dedicated known enum/struct serializer/deserializer helpers and removed the old generated JSON construction helper functions.
  - Kept `RpcObjectReference` included exactly once when it is already present in metadata.
- Reworked `.d.ts` generation to match the new known/unknown schema split.
  - Added unknown primitive, enum, struct, and collection schema types.
  - Known enum and struct declarations are now emitted separately under the known-types section.
  - Known collection types are represented with normal TypeScript arrays / tuple arrays.
- The only TODO point not directly expressed as an independent weak-cast probe is arbitrary interface-to-reference serialization from `rpcjson_Serialize(value: object)`: generated wrappers keep the object reference private, so current behavior depends on existing RPC boxing paths producing `RpcObjectReference` before unknown JSON serialization sees an interface value.

# TEST

- Passed: `..\..\.github\Scripts\copilotBuild.ps1 -Configuration Debug -Platform x64`
- Passed: `..\..\.github\Scripts\copilotBuild.ps1 -Configuration Debug -Platform Win32`
- Passed: `..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_LoadAndCompile -Configuration Debug -Platform x64`
- Passed: `..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_LoadAndCompile -Configuration Debug -Platform Win32`
- Passed: `..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_GenerateMetadata -Configuration Debug -Platform x64`
- Passed: `..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_GenerateMetadata -Configuration Debug -Platform Win32`
- Passed: `..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable LibraryTest -Configuration Debug -Platform x64`
- Passed: `..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable LibraryTest -Configuration Debug -Platform Win32`
- Passed: `..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable RuntimeTest -Configuration Debug -Platform x64`
- Passed: `..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable RuntimeTest -Configuration Debug -Platform Win32`
- Passed: `..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CppTest -Configuration Debug -Platform x64`
- Passed: `..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CppTest -Configuration Debug -Platform Win32`
- Passed: `..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Metaonly -Configuration Debug -Platform x64`
- Passed: `..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Metaonly -Configuration Debug -Platform Win32`
- Passed: `..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Reflection -Configuration Debug -Platform x64`
- Passed: `..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Reflection -Configuration Debug -Platform Win32`

# PROPOSALS

- None.
