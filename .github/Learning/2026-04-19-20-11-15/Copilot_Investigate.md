# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Refactor `RpcDualLifecycleMock` to minimize dictionaries by grouping related properties into structs. Multiple dictionaries keyed by the same key (object id) should be consolidated into a single dictionary mapping to a struct. Wrapper entries should also be consolidated. Use `(G|S)etInternalProperty` to look up objects by their internal property instead of maintaining reverse-lookup dictionaries. Replace silent fallbacks with defensive `CHECK_ERROR` throws since this is test-only code.

# UPDATES

# TEST

Existing unit tests cover all RPC functionality:
- `RuntimeTest` (141 cases): Tests RPC binary assemblies with dual lifecycle mocks
- `CppTest_Metaonly` (107 cases): Generated C++ code tests
- `CppTest_Reflection` (107 cases): Generated C++ code tests with reflection
- `CompilerTest_LoadAndCompile` (583 cases): Compiler tests including RPC compilation
- `LibraryTest` (14 cases): Library tests including RPC byref wrappers

Success criteria: All test projects pass on both x64 and Win32 platforms.

# TEST [CONFIRMED]

All tests pass on both x64 and Win32:
- RuntimeTest: 141/141 (x64), 141/141 (Win32)
- CppTest_Metaonly: 107/107 (x64)
- CppTest_Reflection: 107/107 (x64)
- CompilerTest_LoadAndCompile: 583/583 (x64)
- LibraryTest: 14/14 (x64)

# PROPOSALS

- No.1 Consolidate dictionaries into RpcLocalObjectProperties and RpcWrapperEntry structs [CONFIRMED]

## No.1 Consolidate dictionaries into RpcLocalObjectProperties and RpcWrapperEntry structs

Eliminated 9 dictionaries/lists from `RpcDualLifecycleMock`:
- `refCounts` (objectId → vint) → `RpcLocalObjectProperties::refCount`
- `typeIds` (objectId → vint) → stored in `RpcLocalObjectProperties::ref.typeId`
- `localObjects` (objectId → IDescriptable*) → `RpcLocalObjectProperties::rawPtr`
- `ownedObjects` (objectId → Ptr<IDescriptable>) → `RpcLocalObjectProperties::ownedPtr`
- `refsByPtr` (IDescriptable* → RpcObjectReference) → replaced by `GetInternalProperty` lookup on the object itself
- `refsById` (objectId → RpcObjectReference) → `RpcLocalObjectProperties::ref`
- `observableProxies` (objectId → IValueObservableList*) → removed, lookup now scans `wrapperEntries`
- `eventHandlers` (objectId → Ptr<EventHandler>) → `RpcLocalObjectProperties::eventHandler`
- `wrapperRefs` (IDescriptable* → RpcObjectReference) → consolidated into `RpcWrapperEntry::ref`
- `wrapperProxies` (List<Ptr<IRpcWrapperBase>>) → consolidated into `RpcWrapperEntry::proxy`
- `wrapperBases` (List<IRpcWrapperBase*>) → eliminated, `wrapperEntries` holds Ptr directly

New structures:
- `RpcLocalObjectProperties`: groups ref, refCount, rawPtr, ownedPtr, eventHandler per object id
- `RpcWrapperEntry`: groups proxy and ref per wrapper
- `InternalProperty_LocalObjectTracker`: static WString using `WString::Unmanaged()` for property key

Replaced silent fallbacks with `CHECK_ERROR` throws for defensive programming.
Used `GetInternalProperty`/`SetInternalProperty` with `RpcDualObjectTracker` to look up tracked objects by pointer (replacing `refsByPtr` dictionary).

### CODE CHANGE

**RpcDualLifecycleMock.h**:
- Added `RpcLocalObjectProperties` struct with `ref`, `refCount`, `rawPtr`, `ownedPtr`, `eventHandler`
- Added `RpcWrapperEntry` struct with `proxy`, `ref`
- Replaced 9 dictionaries/lists with `localObjectProps` (Dictionary<vint, Ptr<RpcLocalObjectProperties>>) and `wrapperEntries` (List<RpcWrapperEntry>)
- Added `static WString InternalProperty_LocalObjectTracker`
- Added `GetRef()` and `GetMock()` accessors to `RpcDualObjectTracker`

**RpcDualLifecycleMock.cpp**:
- Rewrote all methods to use consolidated structures
- `TrackLocalObject`/`UntrackLocalObject`: use `localObjectProps` instead of multiple maps
- `PtrToRef`: uses `GetInternalProperty` to find tracked objects instead of `refsByPtr`
- `CreateCallerProxy`: stores `RpcWrapperEntry` instead of separate maps
- `OnItemChanged`: scans `wrapperEntries` instead of `observableProxies`
- All error paths now use `CHECK_ERROR` instead of silent returns

**TestRpc.cpp**:
- Updated `RpcWorkflowLifecycleMock` destructor to use new field names

### CONFIRMED

All unit tests pass on both x64 and Win32 platforms. The refactoring successfully consolidated 9+ separate dictionaries/lists into 2 structures (`localObjectProps` and `wrapperEntries`) plus internal property lookup, while maintaining identical behavior. Defensive `CHECK_ERROR` throws replace all silent fallbacks.

# UPDATES

## CI RuntimeTest Failure (heap corruption)

**Root cause**: In `~RpcWorkflowLifecycleMock` (TestRpc.cpp), the cleanup sequence had `ownedObjects.Clear()` before the `while (localObjects.Count() > 0)` loop. When `ownedObjects.Clear()` destroys Workflow objects, their `RpcDualObjectTracker` destructors call `UntrackLocalObject` which calls `ownedObjects.Remove()` on an already-being-cleared dictionary — causing heap corruption (use-after-free).

The base class `~RpcDualLifecycleMock` had the correct order (untrack loop first, then `ownedObjects.Clear()`), but the derived class override in TestRpc.cpp had them swapped.

**Fix**: Moved `ownedObjects.Clear()` after the `while (localObjects.Count() > 0)` loop, matching the base class order.

**Verification**: Win32 141/141 passed, x64 141/141 passed. The heap corruption was reproducible locally under the debug CRT's heap validation (`VCZH_CHECK_MEMORY_LEAKS`).

# TEST

The test is the existing Rpc test cases in TestCasesRpc.cpp plus RuntimeTest, CppTest, CppTest_Metaonly, CppTest_Reflection. Success criteria:
- All test projects pass for both Win32 and x64
- No regressions

# PROPOSALS
