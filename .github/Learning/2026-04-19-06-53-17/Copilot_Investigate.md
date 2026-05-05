# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Follow job.new-sample.md to add a new Rpc\LocalAndWrapper.txt sample.

The sample tests that when interface objects cross RPC boundaries:
- Remote objects are wrapped (serviceReceived_Obj1 != clientObj1, clientReceived_Obj2 != serviceObj2)
- Local objects are recognized and unwrapped (serviceReceived_Obj2 == serviceObj2, clientReceived_Obj1 == clientObj1)

The sample defines three @rpc:Interface interfaces (IObj1, IObj2, IService with @rpc:Ctor) in the RpcWrapperTest namespace. IService has Exchange1 (takes IObj1^, returns IObj2^) and Exchange2 (takes IObj2^, returns IObj1^).

Expected result: `[false][true][true][false]`

Fixes are allowed to:
- IRpcLifeCycle implementation and connected interfaces
- Runtime library (RpcBoxByval, RpcUnboxByval, etc.)
- Test mock code (RpcDualLifecycleMock, etc.)

NOT allowed to change:
- Workflow parser
- Workflow compiler
- Workflow to C++ code generation

# UPDATES

## UPDATE

You should ensure that clientMain calls request service ends up a wrapper, that's how it connects to the client lc while the actual service connects to the service lc. If the service side calls request service, it should get the real object, as the real object is registered to the service lc. All changes to RpcDualLifecycleMock should be reasonable for any future cases.

## UPDATE

Additional crashes found during verification:
1. `SafeAggregationCast<IDescriptable>` on aggregated `WfInterfaceInstance` causes "Found multiple ways" error. Fixed by using `dynamic_cast<IDescriptable*>` in `RpcBoxByref` and `RpcBoxByvalInternal`.
2. CppTest_Reflection cleanup crash: `SafeAggregationCast<IRpcWrapperBase>()` returns nullptr in reflection-enabled builds. Fixed by storing `IRpcWrapperBase*` via `SafeAggregationCast` at creation time in `CreateCallerProxy`, using stored pointers in destructor.
3. `RpcDualObjectTracker` has raw `mock` pointer that becomes dangling when tracked objects outlive the mock. Fixed by clearing `RpcLifecycleTracker` internal property in `UntrackLocalObject` before removing from `localObjects`.
4. RuntimeTest crash: `WfRuntimeGlobalContext` circular reference with Workflow objects holding lambdas that reference the context. Fixed by clearing `globalVariables` before releasing mocks in test cleanup, and adding `~RpcWorkflowLifecycleMock()` destructor to disconnect wrappers while `globalContext` is still alive.

# TEST [CONFIRMED]

The test is the sample itself. Success criteria:
- Sample compiles in CompilerTest_LoadAndCompile
- RuntimeTest passes with expected result `[false][true][true][false]`
- CppTest, CppTest_Metaonly, CppTest_Reflection all pass

# PROPOSALS

- No.1 [CONFIRMED] Fix RpcBoxByvalInternal/RpcUnboxByvalInternal and RpcDualLifecycleMock

## No.1 Fix RpcBoxByvalInternal/RpcUnboxByvalInternal and RpcDualLifecycleMock

This is a two-part fix:

**Part A: Fix `RpcBoxByvalInternal` and `RpcUnboxByvalInternal` in `Source/Library/WfLibraryRpc.cpp`**

The generated Workflow compiler emits `RpcBoxByval`/`RpcUnboxByval` calls for interface parameters (`IObj1^`, `IObj2^`). These functions currently only handle collections (dicts/lists) and pass everything else through. They need to also handle interface types by converting them to/from `RpcObjectReference` using `PtrToRef`/`RefToPtr`.

- In `RpcBoxByvalInternal`: After the collection checks, if the value is a SharedPtr to an `IDescriptable`, call `lc->PtrToRef(obj)` and return the boxed `RpcObjectReference`.
- In `RpcUnboxByvalInternal`: After the collection checks, if the value is a boxed `RpcObjectReference`, call `lc->RefToPtr(ref)` and return the boxed result.

**Part B: Fix `RpcDualLifecycleMock` in `Test/Source/RpcDualLifecycleMock.cpp`**

Three changes to the mock:

1. Add a `pendingProxyRef` member and a `wrapperRefs` dictionary. `pendingProxyRef` is set temporarily during `CreateCallerProxy` so that the generated wrapper factory's `RequestService(typeId)` call returns the known remote ref instead of going through the objectCallback.

2. Override `RequestService(vint typeId)` from `IRpcObjectOps`/`IRpcController` to check `pendingProxyRef` first.

3. In `CreateCallerProxy`: Set `pendingProxyRef` before calling `universalWrapperFactory`, clear after, and track the created wrapper in `wrapperRefs`.

4. In `PtrToRef`: Check `wrapperRefs` first. If the object is a known wrapper proxy, return the stored remote ref directly instead of creating a new local object.

### CODE CHANGE

**Source/Library/WfLibraryRpc.cpp:**
- `RpcBoxByref`: Changed `SafeAggregationCast<IDescriptable>` to `dynamic_cast<IDescriptable*>(raw)` then `Ptr<IDescriptable>(obj)` to avoid "Found multiple ways" error on aggregated WfInterfaceInstance.
- `RpcBoxByvalInternal`: After collection checks (dict/list), added interface type handling: if the value is a raw pointer that can be `dynamic_cast<IDescriptable*>`, box it via `lc->PtrToRef(Ptr<IDescriptable>(obj))`.
- `RpcUnboxByvalInternal`: Before collection checks, added: if the value is a boxed `RpcObjectReference`, unbox via `lc->RefToPtr(GetRpcObjectReference(serializable))`.

**Test/Source/RpcDualLifecycleMock.h:**
- Added `pendingProxyRef` (RpcObjectReference), `wrapperRefs` (Dictionary<const IDescriptable*, RpcObjectReference>), `wrapperProxies` (List<Ptr<IDescriptable>>), `wrapperBases` (List<IRpcWrapperBase*>).
- Added `RequestService(vint typeId)` override declaration.

**Test/Source/RpcDualLifecycleMock.cpp:**
- Destructor: Iterates `wrapperBases` to call `DisconnectFromLifecycle()`, then clears wrapperBases/wrapperProxies/wrapperRefs. Untracks all localObjects (clearing RpcLifecycleTracker internal property). Then clears ownedObjects and services.
- `CreateCallerProxy`: Sets `pendingProxyRef = ref` before calling `universalWrapperFactory`, clears after. Uses `SafeAggregationCast<IRpcWrapperBase>` to get `wrapperBase`, stores in `wrapperRefs`, `wrapperProxies`, and `wrapperBases`.
- New `RequestService(vint typeId)` override: Returns `pendingProxyRef` if `clientId != 0`, otherwise delegates to base class.
- `PtrToRef`: Checks `wrapperRefs` first — if object is a known wrapper proxy, returns stored remote ref directly.
- `UntrackLocalObject`: Clears `RpcLifecycleTracker` internal property AFTER removing from `localObjects` (to prevent re-entrancy from tracker destructor).

**Test/UnitTest/RuntimeTest/TestRpc.cpp:**
- Added `~RpcWorkflowLifecycleMock()` destructor that disconnects wrappers, clears services/ownedObjects/localObjects, and explicitly releases `wrapperCreateFunc` and `globalContext` — all while the Workflow runtime context is still alive.
- Added explicit cleanup order in test case: release ops, adapters, clear `globalContext->globalVariables`, release mocks, then release globalContext.

### CONFIRMED

All 4 test projects pass with exit code 0:
- RuntimeTest: 140/140 cases, 4/4 files (previously 139/140 pre-existing)
- CppTest: 106/106 cases, 2/2 files
- CppTest_Metaonly: 106/106 cases, 2/2 files
- CppTest_Reflection: 106/106 cases, 2/2 files
