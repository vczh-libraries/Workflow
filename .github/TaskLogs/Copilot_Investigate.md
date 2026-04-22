# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

I would like you to do some refactor to IRpc* interfaces, its reflections and RpcDualLifecycleMock:
- RpcDualLifecycleMock::SetPeer should be removed, I think the functionality could be replaced by calling `IRpcObjectOps::ObjectHold(ref, bool)`, instead of doing the reverse thing. But in order to do this, we need to change the ObjectHold signature to have 3 arguments(objectId, remoteClientId, hold). Since in each lifecycle implementation the same object is guaranteed to be the same wrapper object, so each lifecycle will talk to remote lifecycle's ObjectHold on its own wrapper construction/destruction. When a local object have all remoteClientId disconnected, it is "release-able". Currently ObjectHold calls Lifecycle, we need to do it reversely. But in RpcDualLifecycleMock it is easier, there is only two lifecycle, and one lifecycle will be registered with all ops from another lifecycle, so the peer pointer is totally unnecessary.
- RpcDualLifecycleMock::SetAdaptor and its internal adaptor pointer seems completely useless. Verify my statement and if it is true, delete them.
- Verify if any protected or private members are even needed, remove them if no. The first work might apply refactoring to some protected/private members.

In order to perform a solid refactorying, I would like you to only consider the first item first, pass all unit test, git commit and push, and then perform the rest, pass all unit test, git commit and push.

DO NOT ASK ME ANY QUESTION, I will not be watching you until finishing.

# UPDATES

# TEST

- First stage only covers the `RpcDualLifecycleMock::SetPeer` removal path.
- Validation idea:
	- Widen `system::IRpcObjectOps::ObjectHold` to accept `(ref, remoteClientId, hold)` in source reflection and generated RPC code.
	- Route remote wrapper creation through the lifecycle so wrapper construction is observable in one place.
	- Replace `RpcDualLifecycleMock` peer-to-peer remote release routing with cross-registered `ObjectHold(...)` notifications and remove all `SetPeer(...)` harness wiring.
- Existing coverage to confirm the behavior:
	- `RuntimeTest/TestRpc.cpp` runs all RPC runtime samples, including service request, local/wrapper mixing, and destructor-driven cleanup cases.
	- `CompilerTest_LoadAndCompile/TestRpcCompile.cpp` regenerates the RPC C++ harness, so emitted `SetPeer(...)` calls must disappear there.
	- `CompilerTest_GenerateMetadata` confirms the reflected `IRpcObjectOps.ObjectHold` signature change in metadata baselines.
	- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` confirm the regenerated RPC C++ wrappers still compile and run in all configurations.
- Success criteria:
	- No source or generated harness still calls `SetPeer(...)`.
	- Generated/reflected RPC metadata shows `ObjectHold(ref, remoteClientId, hold)`.
	- The full unit-test order from `Project.md` passes after regeneration and any required baseline refresh.

# PROPOSALS

- No.1 Remove `RpcDualLifecycleMock::SetPeer` by routing remote wrapper ownership through `ObjectHold(ref, remoteClientId, hold)`

## No.1 Remove `RpcDualLifecycleMock::SetPeer` by routing remote wrapper ownership through `ObjectHold(ref, remoteClientId, hold)`

### CODE CHANGE

- Widened `vl::rpc_controller::IRpcObjectOps::ObjectHold` from `(ref, hold)` to `(ref, remoteClientId, hold)` in the library interface, reflection proxy, and reflected metadata.
- Updated the RPC code generator so emitted Workflow wrappers, checked-in generated C++ files, merged RPC C++ sources, and reflection baselines all use the 3-argument `ObjectHold` signature.
- Removed `RpcDualLifecycleMock::SetPeer` and the stored `peer` pointer.
- Added `RpcDualObjectOps` as an explicit cross-registered bridge for dual-lifecycle tests so remote wrapper construction and destruction now call back into the owning lifecycle through `ObjectHold(...)` instead of direct peer-to-peer lifecycle calls.
- Kept callback registrations alive with `Ptr<>` ownership in `RpcLifecycleMock` and `RpcDualLifecycleMock` to prevent teardown-time use-after-free when wrappers release objects late in shutdown.
- Changed `RpcDualLifecycleMock` wrapper tracking so `TrackWrapper(...)` acquires the remote object and `DisconnectTrackedWrappers()` releases it, which makes remote ownership follow actual wrapper lifetime.
- Adjusted `RpcDualLifecycleMock` local-object release semantics so a tracked local object is no longer kept alive just because `PtrToRef(...)` returned its reference; releaseability now depends on active remote wrapper holds.
- Fixed the byval/byref proxy path in `Source/Library/WfLibraryRpc.cpp` by adding a tracked-proxy helper that only reuses already-tracked wrappers and otherwise constructs the expected wrapper type. This was required to keep `LibraryTest` green after the signature refactor regenerated RPC wrappers.
- Updated runtime and compiler RPC harness generation so no emitted source still calls `SetPeer(...)`; generated harnesses now create `RpcDualObjectOps` wrappers before `Register(...)`.

### CONFIRMED

- Stage 1 only is complete. `RpcDualLifecycleMock::SetAdaptor` and access cleanup were intentionally not started.
- No source or generated harness still depends on `SetPeer(...)`.
- Reflected/generated metadata now shows `ObjectHold(ref, remoteClientId, hold)` for both 32-bit and 64-bit outputs.
- Validation completed in the required order from `Project.md`:
	- Debug build Win32: passed.
	- Debug build x64: passed.
	- `LibraryTest` Win32: passed.
	- `LibraryTest` x64: passed.
	- `CompilerTest_GenerateMetadata` Win32: passed.
	- `CompilerTest_GenerateMetadata` x64: passed.
	- `CompilerTest_LoadAndCompile` x64: passed with `Passed test files: 6/6` and `Passed test cases: 586/586`.
	- Post-generation Debug build Win32: passed.
	- Post-generation Debug build x64: passed.
	- `RuntimeTest` Win32: passed with `Passed test files: 4/4` and `Passed test cases: 143/143`.
	- `RuntimeTest` x64: passed.
	- `CppTest` Win32 and x64: passed.
	- `CppTest_Metaonly` Win32 and x64: passed.
	- `CppTest_Reflection` Win32 and x64: passed, each final run reporting `Passed test files: 2/2` and `Passed test cases: 109/109`.
- The final batched post-regeneration command over `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` exited with code `0` after reaching `OK CppTest_Reflection x64`.

## No.2 Remove dead `SetAdapter(...)` storage and tighten `RpcDualLifecycleMock` internals

### CODE CHANGE

- Removed `RpcDualLifecycleMock::SetAdapter(...)` and deleted the stored `adapter` raw pointer from `Test/Source/RpcDualLifecycleMock.h/.cpp`.
- Kept `RpcDualLifeCycleAdapter` itself. It is still the object that reflected Workflow RPC wrappers receive as `system::IRpcLifeCycle*`, and it still provides the non-owning destructor boundary that disconnects tracked wrappers before the underlying mock goes away.
- Simplified `RpcDualLifecycleMock::CreateCallerProxy(...)` so built-in wrappers and universal wrappers use `this` as the lifecycle implementation. This keeps the mock as the runtime owner without storing a second lifecycle pointer.
- Tightened `RpcDualLifecycleMock` visibility: `clientId`, `nextObjectId`, wrapper-tracking helpers, `pendingProxyRef`, `wrapperEntries`, and the internal property keys moved to `private`; only `localObjectProps`, `idMap`, `DecideTypeId(...)`, and `DisconnectTrackedWrappers()` remain `protected` because derived runtime and generated mocks still use them.
- Updated the runtime RPC reflection harness and generated RPC test harnesses so wrapper factories capture their local `RpcDualLifeCycleAdapter` explicitly when calling reflected `rpcwrapper_Create(typeId, lc)`. This preserves the required reflected `IRpcLifeCycle*` argument without reintroducing stored adapter state.
- Regenerated the emitted RPC harness source in `Test/UnitTest/CompilerTest_LoadAndCompile/TestRpcCompile.cpp` and kept the checked-in generated outputs in sync:
	- `Test/SourceCppGenRpc/TestCasesRpc.cpp`
	- `Test/Generated/CppRpc32/TestCasesRpc.cpp`
	- `Test/Generated/CppRpc64/TestCasesRpc.cpp`

### CONFIRMED

- `RpcDualLifecycleMock::SetAdapter(...)` and its stored adapter pointer were dead and are now removed.
- `RpcDualLifeCycleAdapter` is not dead. A first attempt to replace adapter-boundary wrapper creation with `RpcDualLifecycleMock` directly caused `RuntimeTest/TestRpc.cpp` to fail in reflected `rpcwrapper_Create(...)` with `Argument "thisObject" cannot convert from "system::IRpcController*" to "system::IRpcLifeCycle*".`
- The root cause is the reflected wrapper-factory boundary, not wrapper lifetime ownership. Reflected Workflow wrappers still need an object whose reflected static type is `IRpcLifeCycle*`, so the local adapter object remains necessary at the call sites even though `RpcDualLifecycleMock` no longer stores it.
- No source or generated RPC harness still calls `SetAdapter(...)`.
- Validation completed again in the required order after the stage-2 edits and wrapper-factory fix:
	- Debug build Win32: passed.
	- Debug build x64: passed.
	- `LibraryTest` Win32: passed.
	- `LibraryTest` x64: passed.
	- `CompilerTest_GenerateMetadata` Win32: passed.
	- `CompilerTest_GenerateMetadata` x64: passed.
	- `CompilerTest_LoadAndCompile` x64: passed.
	- Post-generation Debug build Win32: passed.
	- Post-generation Debug build x64: passed.
	- `RuntimeTest` Win32: passed.
	- `RuntimeTest` x64: passed with `Passed test files: 4/4` and `Passed test cases: 143/143`.
	- `CppTest` Win32 and x64: passed.
	- `CppTest_Metaonly` Win32 and x64: passed.
	- `CppTest_Reflection` Win32 and x64: passed, with the final x64 run reporting `Passed test files: 2/2` and `Passed test cases: 109/109`.
