# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.
- If you can't fix a runtime issue within a few rounds of guess-and-edit, you need to debug the process.

## Task 1

In generated functions `rpcops_IOps_Create(Json)?` and `rpcops_IRpcObject(Event)?OpsJson`, currently casting between `object` and byval collection types is done by `as` weak cast, I would like you to do `cast` strong cast. As there is no rescue when the underlying protocol gets wrong, raising an exception is always better. Check if other places has similar issue.

Rename `WfAnalyzer_GenerateRpc.JsonSerialization.cpp` to `WfAnalyzer_GenerateRpc_JsonSerialization.cpp`.
`rpcjson_*`, `rpcops_IOps_CreateJson`, `rpcops_IRpcObjectOpsJson` and `rpcops_IRpcObjectEventOpsJson` should be in `WfAnalyzer_GenerateRpc_JsonSerialization.cpp`, not in `WfAnalyzer_GenerateRpc.cpp`.

## Task 2

Currently `rpcjson_*`, `rpcops_IOps_CreateJson`, `rpcops_IRpcObjectOpsJson` and `rpcops_IRpcObjectEventOpsJson` are also in `Wrapper_*.txt`. I would like you to move this part (aka JSON specific thing) to `Wrapper_*_Json.txt`, using module name `RpcMetadataJson`.

A `GenerateModuleRpcJson` function will be made in `WfAnalyzer_GenerateRpc_JsonSerialization.cpp`, therefore `CompilerTest_LoadAndCompile` should call this function too, and now you get two wrapper Workflow script files, link them together in later compiling.

# UPDATES

- Moved JSON-specific RPC Workflow wrapper generation out of `WfAnalyzer_GenerateRpc.cpp` and into the renamed `WfAnalyzer_GenerateRpc_JsonSerialization.cpp`.
- Added `GenerateModuleRpcJson`, producing a separate `RpcMetadataJson` module for `rpcjson_*`, `rpcops_IOps_CreateJson`, `rpcops_IRpcObjectOpsJson`, and `rpcops_IRpcObjectEventOpsJson`.
- Updated `CompilerTest_LoadAndCompile` to write both `Wrapper_*.txt` and `Wrapper_*_Json.txt`, then add both generated modules to the lexical scope manager before compiling generated wrappers.
- Changed RPC unboxing casts for strong-typed/byval collection transfers from weak `as` casts to strong `cast` casts. Nullable non-collection RPC interface references remain weak casts so valid null values keep working.
- Updated the Visual Studio item/filter files for the source rename from `WfAnalyzer_GenerateRpc.JsonSerialization.cpp` to `WfAnalyzer_GenerateRpc_JsonSerialization.cpp`.

# TEST

- Built `UnitTest.sln` via `.github\Scripts\copilotBuild.ps1 -Configuration Debug -Platform Win32`: passed with 0 warnings and 0 errors.
- Built `UnitTest.sln` via `.github\Scripts\copilotBuild.ps1 -Configuration Debug -Platform x64`: passed with 0 warnings and 0 errors.
- Ran `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` via `.github\Scripts\copilotExecute.ps1 -Mode UnitTest` on Debug Win32 and Debug x64: all passed.
- Confirmed generated collection wrappers emit `cast` around `RpcUnboxByval`, JSON wrappers are emitted as `Wrapper_*_Json.txt` with `module RpcMetadataJson`, and normal `Wrapper_*.txt` no longer contains JSON RPC wrapper declarations.

# PROPOSALS
