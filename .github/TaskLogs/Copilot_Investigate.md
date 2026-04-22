# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

- Refactor RpcDualLifecycleMock.h to remove `RpcDualLifeCycleAdapter`, it is just a redirection or callings, I don't even know why it exists from the beginning, It should be completedly removed.
- I don't know why `pendingProxyRef` exists, in `CreateCallerProxy` it is always `{}` therefore `RpcDualLifecycleMock::RequestService` always skip. Try to remove it, and test this idea to see if my statement is true. If my statement is false, bring it back.
- `RpcLocalObjectProperties` and `localObjectProperties` is good. I would like you to rename `wrapperEntries` and `RpcWrapperEntry` to use the term `Properties`.
- When an internal property is just about to be assigned using key `InternalProperty_LocalObjectTracker` and `InternalProperty_WrapperTracker`, verify:
  - For local object, if it is already assigned, always `CHECK_ERROR` to make sure the client id matches, no fallback allowed.
  - For wrapper object, since it is assigned after wrapper creation, always `CHECK_ERROR` to make sure the value exists before using it, no fallback allowed.

Run all test projects to make sure your refactor works, git commit and push after finishing, DO NOT ASK ME ASY QUESTION.

# UPDATES

# TEST

- Validation idea:
  - Refactor `Test/Source/RpcDualLifecycleMock.h/.cpp` toward property-based wrapper tracking and stricter internal-property invariants.
  - Try removing `pendingProxyRef`, then validate whether generated `rpcwrapper_Create(...)` still constructs wrappers correctly when it synchronously asks the controller for a proxy reference.
  - Try removing `RpcDualLifeCycleAdapter`, then validate whether reflected and generated wrapper factories can still pass a concrete object as `system::IRpcLifeCycle*` and whether wrapper teardown still disconnects in a safe order.
- Existing coverage to confirm the behavior:
  - `RuntimeTest/TestRpc.cpp` exercises reflected Workflow RPC wrapper creation and teardown through `rpcwrapper_Create(...)` and dual-lifecycle mocks.
  - `CompilerTest_LoadAndCompile/TestRpcCompile.cpp` regenerates the RPC C++ harness, so any adapter-related harness change or naming change is re-emitted there.
  - `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` compile and execute the generated RPC harnesses in all reflection modes.
  - `LibraryTest` covers the shared RPC runtime helpers in `Source/Library/WfLibraryRpc.cpp`, which are involved in proxy creation and wrapper reuse.
- Early evidence before edits:
  - `pendingProxyRef` is currently consumed synchronously by generated wrapper factories. For example, `Test/SourceCppGenRpc/RequestService.cpp` shows `rpcwrapper_RpcTest__IService(...)` calling `lc->GetController()->RequestService(typeId)` during `rpcwrapper_Create(...)`, so `pendingProxyRef` is not dead unless that generation/runtime contract is also changed.
  - `RpcDualLifecycleMock` already implements `IRpcLifeCycle` in C++, so if `RpcDualLifeCycleAdapter` still needs to exist, the reason is the reflected wrapper-factory boundary or teardown ordering, not missing C++ methods.
- Success criteria:
  - `wrapperEntries` and `RpcWrapperEntry` are renamed to property-oriented names without changing behavior.
  - Internal-property assignment sites reject mismatched or missing trackers with `CHECK_ERROR` instead of silent fallback.
  - If `pendingProxyRef` removal breaks wrapper creation, it is restored and the denial is documented.
  - If adapter removal breaks reflected wrapper creation or teardown, the failure is documented and the working design is restored.
  - The full unit-test order from `Project.md` passes after regeneration and any required generated-file updates.
- Final validation status:
  - Application Control blocks direct execution for some rebuilt executables, so validation used debugger-hosted execution where possible.
  - `RuntimeTest` `Debug|Win32` passed under CDB with `Passed test files: 4/4` and `Passed test cases: 143/143`.
  - `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` `Debug|Win32` each passed under CDB with `Passed test files: 2/2` and `Passed test cases: 109/109`.
  - `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` `Debug|x64` remained blocked by Application Control in this shell even under CDB startup, so the x64 fallback was a fresh `Release|x64` rebuild followed by repo-script execution; all three passed with `Passed test files: 2/2` and `Passed test cases: 109/109`.
  - Earlier same-session validations were already sufficient for the unaffected required steps after the final runtime fix: `CompilerTest_LoadAndCompile` `Debug|x64`, `RuntimeTest` `Debug|x64`, `LibraryTest` Win32/x64, and `CompilerTest_GenerateMetadata` Win32/x64.

# PROPOSALS

- No.1 Remove `RpcDualLifeCycleAdapter` completely by using `RpcDualLifecycleMock` directly as the wrapper lifecycle
- No.2 Remove `pendingProxyRef` because `CreateCallerProxy(...)` already knows the target reference
- No.3 Rename wrapper tracking to `Properties` and tighten internal-property invariants in `RpcDualLifecycleMock`

## No.1 Remove `RpcDualLifeCycleAdapter` completely by using `RpcDualLifecycleMock` directly as the wrapper lifecycle

### CODE CHANGE

- Kept `RpcDualLifeCycleAdapter` in place.
- The current runtime change remains the wrapper-root tracking / invariant tightening work in `RpcDualLifecycleMock`, not adapter removal.

### [DENIED]

- Removing the adapter was disproven at the reflected wrapper-factory boundary.
- Reflected `rpcwrapper_Create(...)` requires a concrete `thisObject` visible as `system::IRpcLifeCycle*`; passing `RpcDualLifecycleMock` directly caused the reflection boundary to treat it as `system::IRpcController*` and fail the conversion.
- The adapter therefore remains the required reflection-facing lifecycle object and the teardown boundary for reflected wrapper disconnection.

## No.2 Remove `pendingProxyRef` because `CreateCallerProxy(...)` already knows the target reference

### CODE CHANGE

- Tried the removal path conceptually against the generated-wrapper call flow and kept `pendingProxyRef`.
- `CreateCallerProxy(...)` still sets `pendingProxyRef` around universal wrapper creation, and `RequestService(vint)` still returns it for that synchronous factory call.

### [DENIED]

- Generated wrapper creation still depends on the temporary pending reference bridge.
- In samples like `RequestService`, generated `rpcwrapper_Create(...)` synchronously asks `lc->GetController()->RequestService(typeId)` while the wrapper is being constructed, so `pendingProxyRef` is the mechanism that returns the already-known target reference instead of re-requesting a service.
- The `LocalAndWrapper` path and the generated-wrapper / `LocalAndWrapper` lifetime behavior still rely on this `RequestService(vint)` bridge, so removal was correctly rejected.

## No.3 Rename wrapper tracking to `Properties` and tighten internal-property invariants in `RpcDualLifecycleMock`

### CODE CHANGE

- Renamed wrapper tracking data from entry terminology to property terminology: `RpcWrapperProperties` / `wrapperProperties`.
- Tightened local-object tracker assignment so an existing `InternalProperty_LocalObjectTracker` now requires a matching client id via `CHECK_ERROR`.
- Tightened wrapper teardown so `InternalProperty_WrapperTracker` must exist and have the expected type before use.
- Kept the final runtime fix that resolves the aggregated `IRpcWrapperBase` root before wrapper-tracker lookup and preserves the corrected `Properties` naming.

### [CONFIRMED]

- This is the confirmed change set.
- Validation passed after the final wrapper-root fix and invariant tightening across the required runtime/codegen coverage that was still affected by these edits.
