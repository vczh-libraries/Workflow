# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.
- If you can't fix a runtime issue within a few rounds of guess-and-edit, you need to debug the process.

## Task 1

There are some code implementing wrappers for byref collections in `WfLibraryRpc.(h|cpp)`, I would like you to extract them to `WfLibraryRpcWrappers.(h|cpp)`. Including wrappers, and default implementation for `IRpcListOps` and `IRpcListEventOps`, meanwhile all interface definition should not move.

All `WfLibraryRpc*.*` should be contained in `Source/Library/Rpc`. And in `VlppWorkflow_Library` project, they should be put in one more nested solution explorer folder called `Rpc`.

To speed up testing, you can skip `CompilerTest_LoadAndCompile` and `Build.ps1` as this change should not affect the compiler.

## Task 2

In `WfLibraryRpcWrappers.h`, a new `IRpcSerializer` is introduced, and it should be reflected as well. It has two member:
- `Serialize` from `Value` to `Value`.
- `Deserialize` from `Value` to `Value`.

The direction is defined like this:
- A `Value` will be put into wrappers, and wrappers call `Serialize`, before passing this value to `IRpcList(Event)?Ops`.
- A `Value` will be received by `IRpcList(Event)?Ops`, and ops call `Deserialize`, before passing this value to wrappers.

Therefore all wrappers and ops implementation in `WfLibraryRpcWrapper.h` should take the new `IRpcSerializer*` in their constructor:
- When it is null, skip the call.
- When it is not null, call it.
- They are applied to collection elements only. Return value for `Contains` or `Add`, they are strong typed and known typed, they do not serialize.

But in any current test projects we only pass `nullptr`, no `IRpcSerializer` will be implemented at the moment.

To speed up testing, you can skip `CompilerTest_LoadAndCompile` and `Build.ps1` as this change should not affect the compiler.

You can checkout multiple pairs of currently implemented files in this pattern: `Wrapper_*.txt` and `Wrapper_*_Json.txt`, they describes how serialization is used for rpc interfaces. But rpc interfaces are strong typed, collection wrappers are weak types for elements, that is the only difference. Unlike strong typed serialization, weak type serialization always transform `Value` to `Value`, skipping the serialization is easy (by just not calling it). So here we have one less layer than rpc interfaces.

## Task 3

In generated `Wrapper_*_Json.txt` we need one more function
`func rpcops_IRpcSerializer() : (system::IRpcSerializer^)`
and implement `(S|Des)erialize` using `rpcjson_(S|Des)erialize`.

But in any current test projects we only pass `nullptr`, actual testing will be in the future.

# UPDATES

# TEST [CONFIRMED]

- Task 1 succeeds when the solution builds and the affected library/metadata unit tests pass after moving all `WfLibraryRpc*.*` files under `Source/Library/Rpc` and updating the `VlppWorkflow_Library` item/filter files. `CompilerTest_LoadAndCompile` and `Build.ps1` are skipped for this task by request.
- Task 2 succeeds when `IRpcSerializer` appears in reflected metadata, all wrapper and default ops constructors accept the serializer pointer, collection element values are serialized/deserialized at wrapper/ops boundaries, and the requested build/test subset passes with all current call sites passing `nullptr`.
- Task 3 succeeds when generated `Wrapper_*_Json.txt` files contain `rpcops_IRpcSerializer`, its override methods delegate through `rpcjson_Serialize` and `rpcjson_Deserialize`, generated C++ artifacts compile, and the generator/runtime/generated-C++ unit tests pass.

# PROPOSALS

- No.1 Extract byref collection wrappers to WfLibraryRpcWrappers [CONFIRMED]
- No.2 Add reflected IRpcSerializer to byref collection wrappers [CONFIRMED]
- No.3 Generate JSON RPC serializer factory [CONFIRMED]

## No.1 Extract byref collection wrappers to WfLibraryRpcWrappers

### CODE CHANGE

- Moved `WfLibraryRpc.h`, `WfLibraryRpcController.(h|cpp)`, and `WfLibraryRpcLifecycle.(h|cpp)` under `Source/Library/Rpc`.
- Split byref collection wrapper declarations and default `IRpcListOps` / `IRpcListEventOps` implementation declarations into `Source/Library/Rpc/WfLibraryRpcWrappers.h`.
- Moved the existing wrapper and default list ops implementation into `Source/Library/Rpc/WfLibraryRpcWrappers.cpp`, leaving `Source/Library/Rpc/WfLibraryRpc.cpp` as the interface translation unit.
- Kept RPC interface definitions in `WfLibraryRpc.h`.
- Updated library includes, the RPC test mock include, and `VlppWorkflow_Library.vcxitems(.filters)` so all `WfLibraryRpc*.*` project items live in the nested `Library\Rpc` solution explorer folder.

### CONFIRMED

The extraction is confirmed by building `Test/UnitTest/UnitTest.sln` through `copilotBuild.ps1` for `Debug|Win32` and `Debug|x64`, both succeeding with 0 warnings and 0 errors. The requested unit-test subset was then executed through `copilotExecute.ps1` for `LibraryTest`, `CompilerTest_GenerateMetadata`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` on both `Debug|Win32` and `Debug|x64`; all 12 executions succeeded. `CompilerTest_LoadAndCompile` and `Build.ps1` were intentionally skipped for Task 1 as requested.

## No.2 Add reflected IRpcSerializer to byref collection wrappers

### CODE CHANGE

- Added `IRpcSerializer` to `WfLibraryRpcWrappers.h` with `Serialize(Value):Value` and `Deserialize(Value):Value`, and registered it as `system::IRpcSerializer` in reflection metadata.
- Added `IRpcSerializer*` constructor arguments and members to byref collection caller wrappers, `RpcCalleeListOps`, and `RpcCalleeListEventBridge`; current test call sites pass `nullptr`.
- Serialized boxed collection element values before wrapper calls into `IRpcListOps`, and deserialized returned element values before unboxing them back to wrapper callers.
- Deserialized incoming element values in `RpcCalleeListOps` before forwarding to the real collection, and serialized boxed element values returned by the default ops.
- Added an internal `RpcLifecycleBase::RefToPtr(ref, serializer)` path so nested collection proxies such as enumerators, keys, and values can be constructed and tracked with the same serializer.
- Regenerated reflection metadata and baselines to include `system::IRpcSerializer`.

### CONFIRMED

The serializer change is confirmed by building `Test/UnitTest/UnitTest.sln` through `copilotBuild.ps1` for `Debug|Win32` and `Debug|x64`, both succeeding with 0 warnings and 0 errors. The requested unit-test subset was executed through `copilotExecute.ps1` for `LibraryTest`, `CompilerTest_GenerateMetadata`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` on both `Debug|Win32` and `Debug|x64`; all 12 executions succeeded. `CompilerTest_LoadAndCompile` and `Build.ps1` were intentionally skipped for Task 2 as requested.

## No.3 Generate JSON RPC serializer factory

### CODE CHANGE

- Added `rpcops_IRpcSerializer` generation to RPC JSON metadata modules.
- Generated `Serialize(value)` to return `rpcjson_Serialize(value)`.
- Generated `Deserialize(value)` to cast the input to `system::JsonNode^` and return `rpcjson_Deserialize(...)`.
- Regenerated RPC wrapper metadata, generated Workflow assembly dumps, and generated C++ RPC artifacts that now expose the serializer factory and implementation.

### CONFIRMED

The generator change is confirmed by running `CompilerTest_LoadAndCompile` for `Debug|x64`; it succeeded after regenerating the expected wrapper and generated C++ artifacts. `Test/UnitTest/UnitTest.sln` was then built through `copilotBuild.ps1` for `Debug|Win32` and `Debug|x64`, both succeeding with 0 warnings and 0 errors. The unit-test subset was executed through `copilotExecute.ps1` for `LibraryTest`, `CompilerTest_GenerateMetadata`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` on both `Debug|Win32` and `Debug|x64`; all 12 executions succeeded. A final `CompilerTest_LoadAndCompile` `Debug|x64` run also succeeded.
