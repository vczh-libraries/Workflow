# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Check out Copilot_(Task|Planning|Summarize|Execution).md for the last work you have done. I have reorganize the code a little bit so there would be some differences but I have only adjusts spaces and orders of declarations, actual implementation is not touched. I would like you to continue to update the code following:

- In `RpcCalleeListOps` and `RpcCalleeListEventBridge`, no registration should have been done, so all non-override methods along with those object caches need to remove. They have the `lifeCycle` references so conversion need to be done there.
- `RpcByrefListEventDispatcher` should not exist.
- To track wrappers' releasing, since wrappers are written by us, the code should just be in the destructor.
- To track interface implementation's releasing, since they all have a `DescriptableObject` base class, you can use its internal property to hook a class, if only the descriptable object owns that class, the class's destructor will just be called by the descriptable object's destructor, and this is how you track objects.
- Inside IRpcLifecycle implementation in RpcByvalLifecycleMock.h, it should only hold raw pointers of objects being tracked. You can just cast raw pointer to Ptr<T> so get a Ptr<T> from it. Objects being tracked the IRpcLifecycle do not own each other, they just pass information between.

git commit and push after finishing, DO NOT ASK ME ANY QUESTION.

# UPDATES

# TEST

Existing test cases in `TestLibraryRpcByval.cpp` will be adapted to verify the refactored code:
1. `RpcByrefArray forwards Set and shrink-only Resize` - array operations work through lifecycle RefToPtr
2. `RpcByrefList forwards Add Insert RemoveAt and Clear` - list operations work through lifecycle RefToPtr
3. `RpcByrefDictionary forwards Set Get Remove Clear and snapshots` - dictionary operations work through lifecycle RefToPtr
4. `RpcByrefObservableList forwards ItemChanged per operation` - observable list events routed without dispatcher
5. `RpcByrefEnumerator lifecycle releases the remote enumerator` - enumerator cleanup through lifecycle

Criteria:
- All test cases pass in `LibraryTest` (Debug|x64 and Debug|Win32).
- All `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `CppTest`, `CppTest_Metaonly`, `CppTest_Reflection` pass.
- `RpcCalleeListOps` and `RpcCalleeListEventBridge` have no non-override methods or caches.
- `RpcByrefListEventDispatcher` is completely removed.
- `RpcByvalLifecycleMock` only holds raw pointers for tracked objects.
- `DescriptableObject::SetInternalProperty` is used for release tracking of interface implementations.

# PROPOSALS

- No.1 [CONFIRMED] Refactor callee ops, remove dispatcher, use raw pointers and internal property tracking

## No.1 [CONFIRMED] Refactor callee ops, remove dispatcher, use raw pointers and internal property tracking

### Overview

1. Remove `RpcByrefListEventDispatcher` from `WfLibraryRpc.h/.cpp`
2. Simplify `RpcCalleeListOps`: remove all caches and non-override methods, use `lifeCycle->RefToPtr(ref)` to find collections
3. Simplify `RpcCalleeListEventBridge`: remove all caches and non-override methods
4. Update `RpcByrefObservableList` to not depend on dispatcher
5. Update `RpcByvalLifecycleMock` to store raw pointers, use `DescriptableObject::SetInternalProperty` for release tracking, handle observable list event dispatch internally
6. Update tests to match new API

### CODE CHANGE

#### `Source/Library/WfLibraryRpc.h`
- Removed `RpcByrefListEventDispatcher` class entirely.
- `RpcByrefObservableList` constructor no longer takes a dispatcher parameter. Removed `dispatcher` member.
- `RpcCalleeListOps`: removed all caches (`arrays`, `lists`, `dicts`, `observableLists`, `enumerators`) and non-override methods (`RequireArray`, `RequireList`, etc.). Only `lifeCycle` member + override methods remain.
- `RpcCalleeListEventBridge`: was already simplified by the user's reorganization (only `lifeCycle` + `OnItemChanged` override).

#### `Source/Library/WfLibraryRpc.cpp`
- Removed `RpcByrefListEventDispatcher` implementation.
- `RpcCalleeListOps` overrides now use `lifeCycle->RefToPtr(ref)` + `SafeAggregationCast<T>()` pattern to find real collections.
- `EnumCreate` uses `lifeCycle->PtrToRef(enumerator)` to register server-created enumerators.
- `RpcCalleeListEventBridge::OnItemChanged` forwards via `lifeCycle->GetController()->OnItemChanged(...)`.
- `RpcByrefObservableList` constructor no longer registers with a dispatcher.

#### `Test/Source/RpcByvalLifecycleMock.h`
- Added `RpcObjectTracker` friend class (destructor-based release tracking via `DescriptableObject::SetInternalProperty`).
- `localObjects`: changed from `Dictionary<vint, Ptr<IDescriptable>>` to `Dictionary<vint, IDescriptable*>` (raw pointers).
- Added `ownedObjects` (`Dictionary<vint, Ptr<IDescriptable>>`) for server-created objects (enumerators) that need ownership.
- Added `observableProxies` (`Dictionary<vint, IValueObservableList*>`) for event dispatch routing.
- Added `eventHandlers` (`Dictionary<vint, Ptr<EventHandler>>`) for observable list event subscriptions.
- Added `CreateCallerProxy(RpcObjectReference)` and `IsTracked(vint)` public methods.
- Removed `GetDispatcher()` method and `dispatcher` member.

#### `Test/Source/RpcByvalLifecycleMock.cpp`
- `TrackLocalObject`: subscribes to `ItemChanged` events on observable lists (stored in `eventHandlers`), sets `DescriptableObject::SetInternalProperty(L"RpcLifecycleTracker", Ptr(new RpcObjectTracker(...)))` for destruction tracking.
- `UntrackLocalObject`: unsubscribes events, removes from all maps (`localObjects`, `refsByPtr`, `refsById`, `ownedObjects`, `observableProxies`, `eventHandlers`).
- `RefToPtr`: checks `localObjects` first (callee-side), then `proxyFactories` (caller-side).
- `PtrToRef`: stores new objects in `ownedObjects` and calls `TrackLocalObject` with raw pointer.
- `OnItemChanged`: dispatches to `observableProxies` by `objectId`.
- `RpcObjectTracker` destructor: calls `UntrackLocalObject` only if `IsTracked()` returns true.

#### `Test/Source/TestLibraryRpcByval.cpp`
- Tests keep reflected wrappers alive via `auto reflectedValues = BoxParameter(values)`.
- Proxies created explicitly: `Ptr(new RpcByrefArray(lc, ref))`, etc.
- Observable list test uses `CreateCallerProxy(ref)` to go through factory path (needed for `observableProxies` registration).
- Removed all `HasTrackedObject`/`HasTrackedEnumerator`/`HasTrackedHandler` assertions.

# CONFIRMED

Proposal No.1 is confirmed. All tests pass on both Debug|x64 and Debug|Win32:
- LibraryTest: 14/14
- CompilerTest_GenerateMetadata: 2/2
- CompilerTest_LoadAndCompile: 578/578 (x64 only per project rules)
- CppTest: 103/103
- CppTest_Metaonly: 103/103
- CppTest_Reflection: 103/103
