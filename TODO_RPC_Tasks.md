# Repro

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
