# MAI Report

# Scope

Original task document: `TODO_RPC_Tasks.md`.

Compared executions:
- `master`: implementation commit `7dccd8ba4f03c334e84f4fe789134ef0f2a546ab` (`Redirect RPC list ops through object ops`). The current `master` tip `a90295036fef5ae5120d5f24940423aebe68777e` only updates `TODO_RPC.md`, so the source review uses the second-last commit as requested.
- `MAI-test`: implementation commit `48ce94e6b694b26497f5d43b673778cc24d8759f` (`Add RPC list/dict caller-callee wrappers`).
- Common base: `c9e812d9ceb9c15c9e8b6b8e62cb2cd3929b3347`.

Testing note: `..\Tools\Tools\Build.ps1 Workflow` resolves the fixed sibling folder named `Workflow`, so it cannot branch-isolate the temporary `MAI-test` worktree. To make sure each branch tested its own files, I used the repo-local scripts from each worktree:
- `.github\Scripts\copilotBuild.ps1`
- `.github\Scripts\copilotExecute.ps1`
- `Test\TypeScript\prepare.ps1` followed by `npm run build`

# Test Results

## master

Builds:
- `Debug|x64`: passed, `Build succeeded`, `0 Warning(s)`, `0 Error(s)`.
- `Debug|Win32`: passed, `Build succeeded`, `0 Warning(s)`, `0 Error(s)`.

Unit tests:
- `LibraryTest` `Debug|Win32`: passed, `2/2` files, `14/14` cases.
- `LibraryTest` `Debug|x64`: passed, `2/2` files, `14/14` cases.
- `CompilerTest_GenerateMetadata` `Debug|Win32`: passed, `1/1` files, `2/2` cases.
- `CompilerTest_GenerateMetadata` `Debug|x64`: passed, `1/1` files, `2/2` cases.
- `CompilerTest_LoadAndCompile` `Debug|x64`: passed, `6/6` files, `709/709` cases.
- `RuntimeTest` `Debug|Win32`: passed, `4/4` files, `261/261` cases.
- `RuntimeTest` `Debug|x64`: passed, `4/4` files, `261/261` cases.
- `CppTest` `Debug|Win32`: passed, `2/2` files, `227/227` cases.
- `CppTest` `Debug|x64`: passed, `2/2` files, `227/227` cases.
- `CppTest_Metaonly` `Debug|Win32`: passed, `2/2` files, `227/227` cases.
- `CppTest_Metaonly` `Debug|x64`: passed, `2/2` files, `227/227` cases.
- `CppTest_Reflection` `Debug|Win32`: passed, `2/2` files, `227/227` cases.
- `CppTest_Reflection` `Debug|x64`: passed, `2/2` files, `227/227` cases.

TypeScript:
- `Test\TypeScript\prepare.ps1`: passed.
- `npm run build`: passed.

The `master` worktree stayed clean after test execution.

## MAI-test

Builds:
- `Debug|x64`: passed, `Build succeeded`, `0 Warning(s)`, `0 Error(s)`.
- `Debug|Win32`: passed, `Build succeeded`, `0 Warning(s)`, `0 Error(s)`.

Unit tests that passed:
- `LibraryTest` `Debug|Win32`: passed, `2/2` files, `14/14` cases.
- `LibraryTest` `Debug|x64`: passed, `2/2` files, `14/14` cases.
- `CompilerTest_GenerateMetadata` `Debug|Win32`: passed, `1/1` files, `2/2` cases.
- `CompilerTest_GenerateMetadata` `Debug|x64`: passed, `1/1` files, `2/2` cases.
- `CompilerTest_LoadAndCompile` `Debug|x64`: passed, `6/6` files, `709/709` cases.
- `RuntimeTest` `Debug|Win32`: passed, `4/4` files, `261/261` cases.
- `RuntimeTest` `Debug|x64`: passed, `4/4` files, `261/261` cases.
- `CppTest_Metaonly` `Debug|Win32`: passed, `2/2` files, `227/227` cases.
- `CppTest_Metaonly` `Debug|x64`: passed, `2/2` files, `227/227` cases.
- `CppTest_Reflection` `Debug|Win32`: passed, `2/2` files, `227/227` cases.
- `CppTest_Reflection` `Debug|x64`: passed, `2/2` files, `227/227` cases.

Unit tests that failed:
- `CppTest` `Debug|Win32`: failed with exit code `3`.
- `CppTest` `Debug|x64`: failed with exit code `3`.

Both `CppTest` failures stop at:
- Test case: `Rpc:Collection_InByref_OutByref`.
- Error: `Unsupported RPC JSON serialization value.`

TypeScript:
- `Test\TypeScript\prepare.ps1`: passed.
- `npm run build`: passed.

The `MAI-test` worktree stayed clean before this report file was added.

# MAI-test Failure Root Cause

The failing sample is `Test\Resources\Rpc\Collection_InByref_OutByref.txt` plus `Collection_InByref_OutByref_Test.txt`. It sends a byref list to a service, mutates that list on the service side, returns it, then mutates it again on the client side. The normal generated C++ RPC test uses the JSON object-op path.

In the existing byref list wrapper, list element values are already boxed and serialized before calling `IRpcListOps`. For example, in `Source\Library\Rpc\WfLibraryRpcWrappers.cpp`, `RpcByrefList::Add` does:
- `SerializeValue(serializer, RpcBoxValueByref(value, lifecycle))`
- then calls `GetRemoteListOps(...)->ListAdd(...)`

MAI-test changes `IRpcListOps` to redirect through `IRpcObjectOps`, but `RpcCallerListOps::ListAdd`, `ListSet`, `ListInsert`, `ListContains`, `ListIndexOf`, and dictionary value/key operations serialize again:
- `RpcCallerListOps::ListAdd` calls `SerializeValue(serializer, RpcBoxValueByref(value, lifecycle))` before invoking object ops.

That double boxing/serialization sends an unsupported wrapper value into the generated JSON serializer. The exception is thrown at `Source\Library\Rpc\WfLibraryRpcJson.cpp`, where `JsonSerializePredefinedTypes` falls through to `CHECK_FAIL(L"Unsupported RPC JSON serialization value.")`.

`master` avoids this by making `RpcCallerListOps` pass list/dictionary element `Value` arguments through unchanged when redirecting to object ops. It only handles primitive method arguments/results and exception/result decoding around the object-op call.

# Task Compliance

## master

`master` substantially follows the task:
- Defines predefined list type, method, and event IDs in `Source\Library\Rpc\WfLibraryRpc.h`.
- Renames the list event bridge concept to `RpcCalleeListEventOps`.
- Adds `RpcCalleeObjectOpsForList` and `RpcCalleeObjectEventOpsForList`.
- Adds `RpcCallerListOps` and `RpcCallerListEventOps`.
- Replaces caller-side mock list implementations with the new caller wrappers.
- Removes the old standalone TypeScript list-op/list-event request and response declarations from `Test\TypeScript\Rpc.d.ts`.
- Adds the predefined list type/method/event IDs to `Test\TypeScript\Rpc.d.ts`.
- Updates `Release\VlppWorkflowLibrary.h` and `Release\VlppWorkflowLibrary.cpp` consistently with the source change.
- Passes all tested build, C++ unit, generated C++ RPC, and TypeScript checks.

Minor quality note: `RpcCallerListOps` uses helper/result plumbing that could be simplified. `InvokeListMethod` checks the method result and some callers then call `ReadMethodResult` again. This did not cause a test failure, but it is small redundant complexity.

## MAI-test

`MAI-test` is incomplete and has a behavioral regression:
- It defines C++ predefined method/event IDs, but uses a different negative range and different names (`EnumCreate`, `ListGetCount`, etc.) from the `master` implementation.
- It does not copy any predefined list type/method/event IDs to `Test\TypeScript\Rpc.d.ts`.
- It leaves the obsolete standalone TypeScript list-op and list-event request/response declarations in place.
- It adds object/list caller and callee wrappers, but the caller wrapper serializes list/dictionary arguments again even though the byref list wrappers already serialized them.
- Its callee object/list bridge is not serializer-aware, so primitive list method arguments and results are not encoded/decoded consistently with JSON object ops.
- It leaves `WrapOps` in `Test\Source\RpcDualLifecycleMock.cpp` and `Test\Source\RpcDualJsonDispatcherMock.cpp`, despite the task saying `WrapOps` should likely become unnecessary and should be removed if possible.
- It does not update the generated `Release` files after changing `Source`.
- It fails `CppTest` in both Win32 and x64.

# Overall Comparison

`master` is the better execution. It is more complete, updates the downstream TypeScript and Release surfaces mentioned by the task, removes more obsolete scaffolding, and passes the tested verification path.

`MAI-test` has useful partial structure, but it should not be accepted as-is. The normal generated C++ RPC JSON path is broken by double serialization in `RpcCallerListOps`, and several explicit task items are missing: TypeScript cleanup/predefined IDs, Release regeneration, and removal of obsolete `WrapOps` scaffolding.

