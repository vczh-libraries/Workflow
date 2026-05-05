# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Do not use `Dictionary<WString, vint>` in `TestCasesRpc.cpp`, if `rpc_GetIds` only serves for this purpose, maybe a better way is not to call it at all in `TestCaeseRpc.cpp`, the compile script generates variables for each type, use them directly with if-else statements. But keep `rpc_GetIds` generated, it is useful in somewhere else.

Pay attention to inheritance. Although there is no test samples about it, but I would like you to do that. Use `PartialOrderPreprocessor`, put inheritance relationship in, figure out the correction direction and make sure leaf types appear eariler. The reason is simple, if IDerived inherits from IBase, testing IBase first ends up everything returning IBase. Test cases for inheritance will be added in the future.

Run unit test to ensure your change works. git push after finishing.

# UPDATES

## UPDATE

I am actually asking you to change the code which generates TestCasesRpc.cpp, so the sorting should be there. RpcDualLifecycleMock doesnt need to be change, unless the Dictionary is declared.

# TEST [CONFIRMED]

- The generator in `Test/UnitTest/CompilerTest_LoadAndCompile/TestRpcCompile.cpp` must emit `TestCasesRpc.cpp` without building a local `Dictionary<WString, vint>` from `instance.rpc_GetIds()`.
- The generated RPC harness must keep `rpc_GetIds` available elsewhere and use generated `rpctype_...` globals directly when registering or deciding type ids.
- RPC interface matching order must be leaf-first for inheritance, so a derived interface is checked before its base.
- A focused compiler-side unit test is required to prove the inheritance ordering for `ILeaf : IDerived : IBase`.
- Verification requires the project order from `Project.md`: build Debug Win32 and x64, run `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`.
- The repo-wide `..\Tools\Tools\Build.ps1 Workflow` wrapper must also be attempted, and any failure must be classified as either code-related or environment-related.

# PROPOSALS

- No.1 Generate direct RPC type-id dispatch in `TestCasesRpc.cpp` and sort RPC interfaces leaf first [CONFIRMED]

## No.1 Generate direct RPC type-id dispatch in `TestCasesRpc.cpp` and sort RPC interfaces leaf first

### CODE CHANGE

- Added `MangleRpcFullName`, `FindRpcTypeDescriptor`, and `SortRpcTypeFullNamesLeafFirst` in `Test/UnitTest/CompilerTest_LoadAndCompile/TestRpcCompile.cpp`.
- Added `TEST_CASE(L"Inherited interfaces are ordered leaf first")` to prove `ILeaf`, `IDerived`, and `IBase` are emitted in the correct order.
- Replaced the direct copy of `manager.rpcMetadata->typeFullNames` with `SortRpcTypeFullNamesLeafFirst(...)` before generating RPC test cases.
- Rewrote the generated `RunRpcTestCase` template to accept a registration callback and a direct `decideTypeId` callback, populate the inherited `idMap` from a local subclass helper, and avoid calling `instance.rpc_GetIds()`.
- Rewrote emitted RPC test-case lambdas to register and return `instance.rpctype_<mangled full name>` directly.
- Regenerated `Test/SourceCppGenRpc/TestCasesRpc.cpp`, `Test/Generated/CppRpc32/TestCasesRpc.cpp`, and `Test/Generated/CppRpc64/TestCasesRpc.cpp` from the updated generator.
- Reverted earlier experimentation in `Test/Source/RpcDualLifecycleMock.*`; the final proposal does not require source changes there.

### CONFIRMED

- The final generated `TestCasesRpc.cpp` no longer builds a local `Dictionary<WString, vint>` and no longer calls `instance.rpc_GetIds()` for the RPC C++ test harness.
- `rpc_GetIds` generation remains untouched outside this harness.
- The inheritance-order unit test passed in `CompilerTest_LoadAndCompile`, confirming `ILeaf < IDerived < IBase`.
- The Debug verification matrix passed:
  - `LibraryTest` Win32 and x64: `14/14`.
  - `CompilerTest_GenerateMetadata` Win32 and x64: `2/2`.
  - `CompilerTest_LoadAndCompile` x64: `586/586`.
  - `RuntimeTest` Win32 and x64: `143/143`.
  - `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` Win32 and x64 all completed successfully in the post-merge run; the combined command exited with code `0`, and the captured output showed the RPC generated-C++ cases passing with `109/109` for the final runs.
- The repo-wide wrapper `..\Tools\Tools\Build.ps1 Workflow` was attempted as required by `Project.md`. It progressed through the release build and test sequence until `Release\RuntimeTest.exe`, then stopped with `This command cannot be run due to the error: An Application Control policy has blocked this file.` This is an environment policy issue rather than a regression from this proposal.
