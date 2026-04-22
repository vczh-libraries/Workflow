# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

`idMap.Set(fullName, serviceTypeIds[nextServiceTypeIdIndex++]);` in `LocalRpcMock` in `TestCasesRpc.cpp` is incorrect. `rpc_GetIds` needs to be used here to translate id to string. But `rpc_GetIds` cannot be used anywhere else. `SetIdMap` might be able to use. The problem is that, the implementation assuming an forced order to call `RegisterService` which is impossible to enforce. You can use `BoxParameter` and `UnboxParameter` to convert between Dictionary and IValueDictionary.

You will need to fix the code generating `TestCasesRpc.cpp` to fix the issue, run all test projects to make sure your change is correct. After finishing everything, git commit and push, DO NOT ASK ME ANY QUESTION.

# UPDATES

## UPDATE

The incorrect behavior is owned by the generator in `Test/UnitTest/CompilerTest_LoadAndCompile/TestRpcCompile.cpp`, not by `RpcDualLifecycleMock`.

## UPDATE

The old generated `LocalRpcMock` rebuilt `idMap` by assuming `RegisterService` is called in the same forced order as the emitted `registerServiceTypeId(...)` list. That assumption is not enforceable and is the root cause.

## UPDATE

The nearby working implementation is `Test/UnitTest/RuntimeTest/TestRpc.cpp`, which already seeds the lifecycle id map from `rpc_GetIds()` and `SetIdMap(...)`. The generator should follow that pattern.

## UPDATE

The correct reflection conversion in this harness is:

- `auto idMap = UnboxParameter<Dictionary<WString, vint>>(BoxParameter(instance.rpc_GetIds()));`
- `lc1->SetIdMap(idMap.Ref());`
- `lc2->SetIdMap(idMap.Ref());`

Using plain `Box(...)` or passing the `Unboxed<Dictionary<...>>` wrapper directly to `SetIdMap(...)` does not compile.

# TEST [CONFIRMED]

- `Test/UnitTest/CompilerTest_LoadAndCompile/TestRpcCompile.cpp` must stop generating any RPC service-id binding logic that depends on `RegisterService` call order.
- The generated `TestCasesRpc.cpp` must seed both lifecycle mocks from the authoritative `rpc_GetIds()` result using `BoxParameter`, `UnboxParameter`, and `SetIdMap(...)`.
- The generated harness must keep `rpc_GetIds()` available elsewhere and only use it here for initial id-map setup.
- The leaf-first RPC inheritance ordering test must still pass.
- Required verification order from `Project.md` must pass after regeneration.

Verification completed:

- Debug build Win32: passed.
- Debug build x64: passed.
- `LibraryTest` Win32 and x64: batch passed.
- `CompilerTest_GenerateMetadata` Win32 and x64: batch passed.
- `CompilerTest_LoadAndCompile` x64: passed with `Passed test files: 6/6` and `Passed test cases: 586/586`.
- Post-generation Debug build Win32: passed.
- Post-generation Debug build x64: passed.
- `RuntimeTest` Win32 and x64: batch passed, final run reported `Passed test files: 4/4` and `Passed test cases: 143/143`.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` Win32: passed, each final run reported `Passed test files: 2/2` and `Passed test cases: 109/109`.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` x64: passed, each final run reported `Passed test files: 2/2` and `Passed test cases: 109/109`.
- `c:\Code\VczhLibraries\Tools\Tools\Build.ps1 Workflow`: confirmed with `__STATUS:OK`.

# PROPOSALS

- No.1 Generate the RPC C++ test harness id map from `rpc_GetIds()` instead of service registration order [CONFIRMED]

## No.1 Generate the RPC C++ test harness id map from `rpc_GetIds()` instead of service registration order

### CODE CHANGE

- Removed the service-order recovery helpers from `Test/UnitTest/CompilerTest_LoadAndCompile/TestRpcCompile.cpp`:
	- `IsIdentifierChar`
	- `TryReadQuotedString`
	- `FindNamedFunctionDeclaration`
	- `CollectRegisterServiceTypeFullNames`
	- `CollectRpcServiceTypeFullNames`
- Removed `rpcServiceTypeFullNamesPerItem` and all generated `registerServiceTypeId(...)` plumbing.
- Kept the existing leaf-first RPC interface ordering logic and its focused unit test.
- Changed the generated `RunRpcTestCase` signature to only accept `expected` and `decideTypeId`.
- Changed the generated harness to load the authoritative mapping once from `instance.rpc_GetIds()`, convert it with `UnboxParameter<Dictionary<WString, vint>>(BoxParameter(...))`, and call `SetIdMap(idMap.Ref())` on both lifecycle mocks before `serviceMain(...)` runs.
- Removed the generated `LocalRpcMock` fields and methods that tracked service ids by registration order:
	- `serviceTypeIds`
	- `nextServiceTypeIdIndex`
	- `AddServiceTypeId`
	- overridden `RegisterService`
	- `ValidateServiceTypeIds`
	- `CopyPeerServiceTypeIds`
- Regenerated:
	- `Test/Generated/CppRpc32/TestCasesRpc.cpp`
	- `Test/Generated/CppRpc64/TestCasesRpc.cpp`
	- `Test/SourceCppGenRpc/TestCasesRpc.cpp`

### CONFIRMED

- The generated RPC C++ harness no longer assumes any forced `RegisterService` ordering.
- The authoritative string-to-id mapping now comes from `rpc_GetIds()` and is copied into both lifecycle mocks with `SetIdMap(...)`.
- `rpc_GetIds()` is not used elsewhere in the generated harness.
- The focused inheritance-order compiler test still passes.
- The full required debug verification matrix passed.
- The final repo-level `Build.ps1 Workflow` verification also passed.
