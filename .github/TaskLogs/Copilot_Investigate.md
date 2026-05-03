# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Task 1

Currently there are JSON .d.ts generation code and JSON serialization in generated wrapper Workflow script mixing with other RPC stuff together.

Move JSON .d.ts generation crom WfAnalyzer_ValidateRpc.cpp to WfAnalyizer_GenerateRpc_JsonDts.cpp.
Move JSON serialization from WfAnalyzer_GenerateRpc.cpp to WfAnalyzer_GenerateRpc.JsonSerialization.cpp. But the cpp file might need to extern a function in WfAnalyzer_GenerateRpc.cpp (instead of WfAnalyzer.h) so that it could fill function declarations to the generated wrapper Workflow script module (pass the module in and the function fill declarations for rpcjson_* functions)

This task should be easy, just copy paste code, and if you do it correctly, no generated file should be affected

So we can have 4 WfAnalyzer*Rpc*.cpp files, and responsibility are well separated.

# UPDATES

# TEST [CONFIRMED]

Build and run the Workflow unit-test pipeline because the task moves C++ source between translation units and changes the MSBuild source list.

Success criteria:
- Debug Win32 and x64 builds pass.
- Required unit test projects pass in the order from `Project.md`.
- `CompilerTest_LoadAndCompile` does not leave generated RPC metadata/script/d.ts files changed, because this is intended as a responsibility-only move with no generated output changes.

# PROPOSALS

- No.1 Split JSON RPC generation into dedicated source files [CONFIRMED]

## No.1 Split JSON RPC generation into dedicated source files

Move generated wrapper JSON serialization helpers from `WfAnalyzer_GenerateRpc.cpp` into `WfAnalyzer_GenerateRpc.JsonSerialization.cpp`, with `WfAnalyzer_GenerateRpc.cpp` declaring the local generator hook it calls while building the wrapper module. Move RPC JSON `.d.ts` generation from `WfAnalyzer_ValidateRPC.cpp` into `WfAnalyzer_GenerateRpc_JsonDts.cpp`. Register both new files in `VlppWorkflow_Compiler.vcxitems` and `.filters`.

### CODE CHANGE

- Moved generated wrapper JSON serialization helpers from `Source/Analyzer/WfAnalyzer_GenerateRpc.cpp` to `Source/Analyzer/WfAnalyzer_GenerateRpc.JsonSerialization.cpp`.
- Kept the wrapper module orchestration in `WfAnalyzer_GenerateRpc.cpp`, with a local declaration for `AddRpcJsonDeclarations(WfLexicalScopeManager*, Ptr<WfModule>)` so the JSON serialization translation unit can fill the generated `rpcjson_*` declarations into the module.
- Moved RPC JSON `.d.ts` generation from `Source/Analyzer/WfAnalyzer_ValidateRPC.cpp` to `Source/Analyzer/WfAnalyzer_GenerateRpc_JsonDts.cpp`.
- Added both new files to `Test/UnitTest/VlppWorkflow_Compiler/VlppWorkflow_Compiler.vcxitems` and `VlppWorkflow_Compiler.vcxitems.filters` under the analyzer source list.

### CONFIRMED

The proposal is confirmed because the code now has four separated RPC analyzer source files: `WfAnalyzer_GenerateRpc.cpp`, `WfAnalyzer_GenerateRpc.JsonSerialization.cpp`, `WfAnalyzer_GenerateRpc_JsonDts.cpp`, and `WfAnalyzer_ValidateRPC.cpp`. The split keeps the public analyzer header unchanged and only exposes the JSON serialization hook locally between the RPC generation translation units. The full Debug build and unit-test set passed for both x64 and Win32 where applicable, and `CompilerTest_LoadAndCompile` left no generated files changed, matching the expected "copy-paste responsibility split only" behavior.

Verification performed:

- `copilotBuild.ps1 -Configuration Debug -Platform x64`: passed.
- `copilotBuild.ps1 -Configuration Debug -Platform Win32`: passed.
- `LibraryTest`: x64 and Win32 passed, 14/14 cases.
- `CompilerTest_GenerateMetadata`: x64 and Win32 passed, 2/2 cases.
- `CompilerTest_LoadAndCompile`: passed, 699/699 cases, no generated files changed.
- `RuntimeTest`: x64 and Win32 passed, 257/257 cases.
- `CppTest`: x64 and Win32 passed, 223/223 cases.
- `CppTest_Metaonly`: x64 and Win32 passed, 223/223 cases.
- `CppTest_Reflection`: x64 and Win32 passed, 223/223 cases.
