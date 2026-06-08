# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

RpcCppJsonLifecycleMock needs an external DecideTypeId implementation. I would like to put this into RpcJsonLifecycle, and this functor could be set via its Register function. Put this new argument at 4th named _getTypeId. Use Func instead of function pointer.

The implementation of this function is generated in `TestCasesRpc.cpp`. But I would prefer a Workflow based implementation, called `rpcwrapper_GetTypeId(object):int`, just below `rpcwrapper_Create`.

By doing this, `RpcCppJsonLifecycleMock ` is no longer needed, instead just use `rpcwrapper_GetTypeId` on that new argument.

# TEST [CONFIRMED]

Existing RPC test cases cover the path that needs this change: `RunRpcTestCase_JsonRequest` creates `RpcJsonLifecycle` instances in meta-only reflection builds and exercises JSON request wrappers from generated Workflow/C++ code. Success criteria:

- `RpcJsonLifecycle::Register` accepts a fourth `_getTypeId` `Func` and uses it after the built-in lifecycle type-id checks.
- Generated RPC wrapper Workflow contains `rpcwrapper_GetTypeId(object):int` immediately after `rpcwrapper_Create`, and generated `TestCasesRpc.cpp` exposes the corresponding C++ implementation.
- `RpcCppJsonLifecycleMock` is removed and `RunRpcTestCase_JsonRequest` uses `RpcJsonLifecycle` directly with the generated get-type-id callback.
- The generated RPC C++ test project builds and at least the meta-only RPC test path passes; broader build/test scope follows `Project.md` when feasible.

# PROPOSALS [CONFIRMED]

- No.1 Move JSON type-id extension into `RpcJsonLifecycle` [CONFIRMED]

## No.1 Move JSON type-id extension into `RpcJsonLifecycle` [CONFIRMED]

Add a `Func<vint(reflection::IDescriptable*)>` callback to `RpcJsonLifecycle`, set it from the fourth `Register` argument named `_getTypeId`, and override `DecideTypeId` inside `RpcJsonLifecycle` so callers no longer need a derived test-only class. Generate a Workflow function `rpcwrapper_GetTypeId(object):int` next to `rpcwrapper_Create`, then pass it from `RunRpcTestCase_JsonRequest` through the new `Register` argument.

### CODE CHANGE

- Extend `RpcJsonLifecycle::Register` with a fourth `_getTypeId` argument of type `Func<vint(reflection::IDescriptable*)>` and move the custom `DecideTypeId` fallback into `RpcJsonLifecycle`.
- Generate `rpcwrapper_GetTypeId(obj : object) : int` immediately after `rpcwrapper_Create`; it performs leaf-first raw-interface type checks and returns the generated RPC type id or `RpcTypeId_NotFound`.
- Remove `RpcCppJsonLifecycleMock` and configure `RunRpcTestCase_JsonRequest` by passing a lambda that calls `instance.rpcwrapper_GetTypeId(BoxValue<IDescriptable*>(obj))` into `RpcJsonLifecycle::Register`.
- Stop generating the C++ type-id lambda in `TestCasesRpc.cpp`; use the generated Workflow helper from the shared test harness instead.

### CONFIRMED

- `CompilerTest_LoadAndCompile` regenerated `Test/SourceCppGenRpc/TestCasesRpc.cpp`, which now calls `RunRpcTestCase` without a generated C++ type-id lambda.
- Generated RPC wrapper Workflow contains `func rpcwrapper_GetTypeId(obj : object) : (int)` immediately after `rpcwrapper_Create`; the inheritance wrapper emits leaf-first checks such as `IDerived` before its bases.
- `rg` found no remaining `RpcCppJsonLifecycleMock` references in editable source/test files.
- `copilotBuild.ps1 -Configuration Debug -Platform x64` passed after regeneration. The first x64 build before regeneration failed only because stale generated RPC C++ still called the old 3-argument harness.
- `copilotBuild.ps1 -Configuration Debug -Platform Win32` passed.
- Unit tests passed: `LibraryTest` Win32/x64, `CompilerTest_GenerateMetadata` Win32/x64, `CompilerTest_LoadAndCompile` x64, `RuntimeTest` Win32/x64, `CppTest` Win32/x64, `CppTest_Metaonly` Win32/x64, and `CppTest_Reflection` Win32/x64.
- TypeScript checks passed: `Test/TypeScript/prepare.ps1` and `npm run build`.
