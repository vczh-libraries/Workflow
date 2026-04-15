# !!!TASK!!!

# PROBLEM DESCRIPTION
# Problem
- References:
  - [Definition](./TODO_RPC_Definition.md)
  - [Scenarios](./TODO_RPC_Scenarios.md)

- Add `IRpcController` and `IRpcLifeCycle` to the `WfLibraryRpc.(h|cpp).
  - Caller side `IValue*` implementation to call list ops and listen to list event ops.
    - Each `IValue*` collection (enumeratle, list, dictionary, observable) should have a wrapper implementation, accepting `IRpcLifeCycle` and a `RpcObjectReference` arguments in their constructors. Only writable versions are needed because they also implement readonly version interfaces.
    - `RpcByref*` inherits from `IValue*`, meaning they implement `@rpc:Byref` containers. When convert between objects and serializable objects, they should use the `byref` helper function.
  - Callee side list ops implementation to call `IValue*`, attach to all events to call list event ops.
    - Callee side implementations accepting `IRpcLifeCycle` argument in their constructors.
  - No object ops implementations are introduced in this task.
  - Boxing and unboxing helper functions:
    - `RpcBoxByref`, `RpcUnboxByref`, `RpcBoxByval`, `RpcUnboxByval`.
    - `Box` converts from trivial objects to serializable objects. `Unbox` do the reverse.
    - Trivial objects means objects in their original form, like an `@rpc:Interface` interface implementation, `IValue*` collections, enums, structs.
    - Serializable objects means objects in their controller form. `@rpc:Interface` and `byref` `IValue*` collections become `RpcObjectReference`. `byval` `IValue*` collections become their `IValue*` version of serializable objects (this happens recursively, so boxing/unboxing byval collections could just be recursive functions).
    - All these functions convert between `Value` to `Value`, taking `IRpcLifeCycle` as their second argument.
    - All values passing to `IRpcController` should be serializable objects. All values passing to or returning from the interface of wrappers or `@rpc:Interface` implementations should be trivial objects. The `BoxValue` and `UnboxValue` function would help to process non-interface values.

# UPDATES

# INSIGHTS AND REASONING

## Existing structure / evidence
- `Source\Library\WfLibraryRpc.h/.cpp` currently contain only an empty `vl::rpc_controller` namespace (no types yet).
- `Source\Library\WfLibraryReflection.h` already includes `WfLibraryRpc.h`, and the library type-loading is driven by `WORKFLOW_LIBRARY_TYPES(F)` / `IMPL_TYPE_INFO_RENAME(...)` / `BEGIN_*_MEMBER(...)` blocks in `WfLibraryReflection.(h|cpp)`.
- `TODO_RPC_Definition.md` defines the controller-layer Ops interfaces (`IRpcListOps`, `IRpcListEventOps`, ...), plus `RpcObjectReference`, `IRpcController`, and `IRpcLifeCycle`.
- `TODO_RPC_Scenarios.md` describes required byref list/dict proxy behavior (Scenario 8/16) and observable change forwarding (Scenario 19).
- `Import\VlppReflection.h` documents that observable list change args match `reflection::description::IValueObservableList::ItemChanged(index, oldCount, newCount)` (see `typedef void ItemChangedProc(vint index, vint oldCount, vint newCount);`).
- `TODO_RPC.md` adds two additional constraints for this task:
  - Everything declared in `WfLibraryRpc.h` should be reflected via `WfLibraryReflection.(h|cpp)`.
  - Unit tests will validate that caller-side byref wrappers call into `IRpcController` list ops, mutate the underlying real collections, and that observable list events are forwarded back.

## Design overview
This task adds the **library-side runtime abstractions** needed by generated RPC wrappers and unit tests:

1. **Controller-facing untyped RPC surface** (`IRpcController` and its base Ops interfaces), using `Value` as the universal payload type.
2. **A lifecycle bridge** (`IRpcLifeCycle`) that converts between "trivial" runtime objects (real interface implementations / real `IValue*` collections) and "serializable" controller-layer representations (`RpcObjectReference` and recursively boxed values).
3. **Byref collection proxies** (caller-side `IValue*` implementations) that forward list/dict/enumerator operations to `IRpcController::List*` / `Dict*` / `Enum*`, and (for observable lists) receive `IRpcListEventOps::OnItemChanged` to raise local `ItemChanged`.
4. **Standard callee-side list ops / list event implementations** to delegate incoming list operations to real `IValue*` collections and to bridge `ItemChanged` from real observable lists back to the controller.
5. **Value-to-Value boxing/unboxing helpers**:
   - `RpcBoxByref`, `RpcUnboxByref` for byref semantics.
   - `RpcBoxByval`, `RpcUnboxByval` for byval semantics.

No interface-object invocation (`IRpcObjectOps`) implementations are introduced here (as required).

## Review-driven clarifications (applied)
The initial design is directionally correct; the following clarifications tighten the spec so future implementation and unit tests are unambiguous:

1) Lock interface signatures to the definition and fix spec typos
- `IRpcLifeCycle::Controller` returns `IRpcController*` (non-owning). Controller and lifecycle should not own each other.
- `IRpcLifeCycle::PtrToRef` returns `RpcObjectReference`.
- `TODO_RPC_Definition.md` contains a typo where it says `RefObjectReference`; it should be `RpcObjectReference`.

2) Make the `IRpcIdSync` hierarchy explicit and address diamond inheritance
- `IRpcObjectOps` and `IRpcObjectEventOps` inherit `IRpcIdSync`; `IRpcListOps` and `IRpcListEventOps` do not.
- Since `IRpcController` inherits all four Ops interfaces, C++ interface inheritance must be **virtual** (or otherwise avoid duplicated `IRpcIdSync`) so it compiles and reflects cleanly.

3) Specify object/collection/enumerator registries: ownership, storage type, and cleanup rules
- `IRpcLifeCycle` implementation owns the registries that map `RpcObjectReference` (typically by `objectId`) to:
  - real local objects / local byref collections
  - created enumerators
  - cached caller-side proxies
- Use concrete, heterogeneous-capable storage (e.g. separate dictionaries for `Ptr<reflection::description::IValueList>`, `Ptr<reflection::description::IValueDictionary>`, `Ptr<reflection::description::IValueEnumerator>`), instead of ambiguous wording like “`Ptr<IValue*>`”.
- Define precise removal rules (e.g. on `UnregisterLocalObject` and/or when `ReleaseRemoteObject` drops the last remote reference) to prevent leaks and stale dispatch.

4) Clarify observable list event dispatcher wiring and safe lifetime management
- Caller constructs an `IRpcListEventOps` dispatcher and passes it as `listEventCallback` in `IRpcController::Register`.
- `RpcByrefObservableList` registers/unregisters itself to that dispatcher; the dispatcher routes `OnItemChanged` to the correct proxy.
- Use an explicit safe mechanism suitable for this codebase (intrusive `Ptr<T>`): typically an `objectId`-keyed registry holding raw pointers that are explicitly deregistered in proxy destructors.
- Do not assume observable changes are batched; forward each `OnItemChanged` individually.
- Align with `Import\VlppReflection.h`: `IValueObservableList::ItemChanged` uses parameter name `index`.

5) Define null-callback behavior for `IRpcController::Register`
- Unit tests may pass `nullptr` for object ops callbacks; either allow null and require the controller to guard every call, or treat null as a programming error. This task will assume **null is allowed** and all uses are guarded.

6) Define enumerator lifecycle end-to-end
- `EnumCreate` returns an acquired reference; `RpcByrefEnumerator` releases its remote reference in its destructor.
- Callee-side enumerator registry cleanup is tied to `ReleaseRemoteObject` for that enumerator `RpcObjectReference`.

7) Make boxing/unboxing rules first-class and consistent
- For non-interface values, use `BoxValue` / `UnboxValue`.
- Treat interface values and byref collections separately via lifecycle conversion (`PtrToRef` / `RefToPtr`) and `RpcBoxByref` / `RpcUnboxByref`.
- For byref collection ops, both caller proxies and callee-side standard implementations must consistently apply `RpcBoxByref` / `RpcUnboxByref` to element payloads.
- For byval recursive boxing/unboxing, state an explicit **acyclic** precondition (or specify cycle detection) to avoid infinite recursion.

8) Define DictGetKeys / DictGetValues return handling
- Treat results as snapshot collections. Specify where element-level boxing/unboxing happens (in `Rpc(Un)boxBy(val|ref)`, not in the controller).

9) Resolve proxy surface ambiguities
- If `RpcByrefArray` exists, define supported operation subset and deterministic failure (`CHECK_FAIL`) for unsupported list mutations.
- If standalone `RpcByrefEnumerable` proxies are needed, document `CreateEnumerator()` flow (`EnumCreate` then wrap as `RpcByrefEnumerator`).

10) Specify `typeId` derivation for byref collection proxies
- Document how `typeId` is obtained from the strong-typed Workflow collection type descriptor and mapped through the `IRpcIdSync` id table.

11) Reflection registration requirements
- Commit to the reflected naming scheme (e.g. `system::rpc_controller::*`) and `IMPL_TYPE_INFO_RENAME` usage.
- Specify which interfaces should use `BEGIN_INTERFACE_MEMBER` vs `BEGIN_INTERFACE_MEMBER_NOPROXY` (callback-only vs script-implementable).
- Ensure reflected methods have argument names where required for Workflow compilation.

12) Testing section should reference the task’s required wiring
- Align with `TODO_RPC.md`: `RpcByvalLifecycleMock.(h|cpp)` structure, controller mock redirecting to callee list ops passed to `Register`, and per-container-type `TEST_CASE` coverage.
- For unimplemented object ops in this task, define a deterministic stub policy (`CHECK_FAIL`) so partial rollout fails loudly.

## Core types and interfaces to add (in `WfLibraryRpc.h`)
All types below live under `namespace vl::rpc_controller` (matching current stub), and should be reflected in `WfLibraryReflection.(h|cpp)`.

### `RpcObjectReference`
A reflected `struct` holding:
- `vint clientId;`
- `vint objectId;`
- `vint typeId;`

This struct is the only identifier that crosses the controller boundary.

### Ops interfaces (as in TODO_RPC_Definition.md)
Define (reflected) interfaces matching the definition document:
- `IRpcIdSync` (`SyncIds(Dictionary<WString,vint> ids)` conceptually; actual signature uses Workflow’s `int[string]` mapping representation in C++ reflection types)
- `IRpcListOps` (Enum*, List*, Dict* methods)
- `IRpcListEventOps` (`OnItemChanged`)
- `IRpcObjectOps` / `IRpcObjectEventOps` (declared but not implemented in this task)
- `IRpcController : IRpcListOps, IRpcObjectOps, IRpcListEventOps, IRpcObjectEventOps` with:
  - `Register(...)` (stores callback pointers, returns ID map)
  - `RegisterLocalObject(typeId)`, `UnregisterLocalObject(ref)`
  - `AcquireRemoteObject(ref)`, `ReleaseRemoteObject(ref)`

### `IRpcLifeCycle`
Define (reflected) interface as specified:
- `Controller` property returns `IRpcController*` (non-owning). Controller and lifecycle should not own each other.
- `RefToPtr(ref)` converts `RpcObjectReference` to a trivial pointer (creates/reuses proxies).
- `PtrToRef(obj)` converts a trivial pointer to `RpcObjectReference` (registers local objects if needed).

Design note (collections): byref `IValue*` collections are also “objects” that need lifecycle conversion, so `IRpcLifeCycle` is expected to support both:
- `@rpc:Interface` instances
- byref `IValue*` collection instances

## Boxing / unboxing layer (Value -> Value)

### Concepts
- **Trivial objects**: values in their “native” runtime representation, e.g.:
  - `@rpc:Interface` implementations (as reflected interface shared pointers)
  - `IValue*` collections (`IValueArray/IValueList/IValueDictionary/IValueObservableList/IValueEnumerable/IValueEnumerator`)
  - enums, structs, primitives
- **Serializable objects**: controller-layer payloads:
  - `RpcObjectReference` for interface values and for byref collections
  - recursively serializable values for byval collections

Rule enforced by helpers:
- Values going into `IRpcController` calls must be **serializable**.
- Values returned from / passed into user-facing proxies and real implementations must be **trivial**.

### Functions
Add free functions (and reflect them via a helper type if needed):
- `Value RpcBoxByref(const Value& trivial, IRpcLifeCycle* lc);`
- `Value RpcUnboxByref(const Value& serializable, IRpcLifeCycle* lc);`
- `Value RpcBoxByval(const Value& trivial, IRpcLifeCycle* lc);`
- `Value RpcUnboxByval(const Value& serializable, IRpcLifeCycle* lc);`

High-level behavior:
- For primitives/enums/structs: pass-through (or use `BoxValue`/`UnboxValue` helpers where type conversion is required).
- For interface values (`Value::SharedPtr` with an interface type descriptor):
  - `Box`: use `lc->PtrToRef(...)`, return `RpcObjectReference` boxed into `Value`.
  - `Unbox`: use `lc->RefToPtr(...)`.
- For collection values:
  - **Byref boxing** (`RpcBoxByref`): register the collection as a local object (via lifecycle/controller) and return a `RpcObjectReference` (serializable).
  - **Byref unboxing** (`RpcUnboxByref`): convert `RpcObjectReference` into a caller-side byref collection proxy (`RpcByref*`) that implements the correct `IValue*` interface.
  - **Byval boxing/unboxing**: recursively walk the container, boxing/unboxing elements using byval recursion. This produces a new `IValue*` tree whose elements are themselves serializable/trivial as appropriate.

Important design constraint from `TODO_RPC.md`:
- `Rpc(Unb|B)oxBy(Val|Ref)` should rely on `BoxValue` / `UnboxValue` for non-interface values, and treat reference types (interfaces and byref collections) as a separate branch.

## Caller-side byref collection proxies (`RpcByref*`)

### Scope
Provide concrete implementations (C++ classes) of the relevant `reflection::description::IValue*` interfaces that forward operations to controller list ops:
- `RpcByrefEnumerable` : `IValueEnumerable`
- `RpcByrefEnumerator` : `IValueEnumerator`
- `RpcByrefList` / `RpcByrefArray` : `IValueList` / `IValueArray` (depending on how `T[]` is represented; tests will explicitly cover Array vs List)
- `RpcByrefObservableList` : `IValueObservableList`
- `RpcByrefDictionary` : `IValueDictionary`

Each proxy’s constructor takes:
- `IRpcLifeCycle* lifeCycle`
- `RpcObjectReference ref`

Only writable interfaces are implemented explicitly; readonly interfaces are satisfied via inheritance.

### Forwarding rules
- All operations that send a `Value` argument to the controller must first convert from trivial -> serializable using the **byref** boxing helper (so nested collection elements are treated as byref too):
  - `controller.ListSet(ref, index, RpcBoxByref(value, lc))`
  - `controller.DictSet(ref, RpcBoxByref(key, lc), RpcBoxByref(value, lc))`
- All operations that return an element (`ListGet`, `DictGet`, `EnumGetCurrent`) must convert serializable -> trivial via **byref** unboxing.

### Observable list event reception
`RpcByrefObservableList` must surface the local `ItemChanged(index, oldCount, newCount)` event.

Design approach:
- Introduce a caller-side `IRpcListEventOps` implementation (dispatcher) that the controller calls when remote events arrive.
- The dispatcher maintains a map from `RpcObjectReference` to active `RpcByrefObservableList` proxies (weak references / safe pointers).
- `RpcByrefObservableList` registers/unregisters itself with the dispatcher on construction/destruction.
- On `OnItemChanged(ref, ...)`, the dispatcher finds the proxy and raises `ItemChanged(...)` on it.

This matches Scenario 19 in `TODO_RPC_Scenarios.md` and the signature documented in `Import\VlppReflection.h`.

## Callee-side list ops + list event bridge

### `IRpcListOps` standard implementation
Provide a standard callee-side implementation of `IRpcListOps` that:
- Looks up the real target collection/enumerator by `ref.objectId`.
- For each operation, translates arguments and results:
  - Incoming values from controller are **serializable** -> unbox to trivial before applying to real collections.
  - Outgoing values from real collections are **trivial** -> box to serializable before returning.

It requires access to:
- A mapping `objectId -> Ptr<IValue*>` for registered byref collections.
- A mapping for enumerators created by `EnumCreate`.

The mapping is populated when boxing byref collections (see Boxing layer): the component that allocates the `RpcObjectReference` must also register `(objectId -> realCollection)` so later `List*`/`Dict*` calls can dispatch.

### `IRpcListEventOps` standard implementation (callee side)
Provide a standard implementation that monitors real `IValueObservableList` instances:
- On association of a `RpcObjectReference` with a real observable list, attach to its `ItemChanged` event.
- When it fires, call `lifeCycle->Controller()->OnItemChanged(ref, index, oldCount, newCount)` (controller routes to remote holders).
- Detach handlers when the object is unregistered/unavailable.

The constructor accepts `IRpcLifeCycle*` per requirement, so the implementation can access the controller via `lifeCycle->Controller`.

## Reflection exposure (`WfLibraryReflection.(h|cpp)`)
Because `WfLibraryReflection.h` already includes `WfLibraryRpc.h`, the reflection step should:
- Extend `WORKFLOW_LIBRARY_TYPES(F)` to include at least:
  - `rpc_controller::RpcObjectReference`
  - `rpc_controller::IRpcIdSync`
  - `rpc_controller::IRpcListOps`
  - `rpc_controller::IRpcListEventOps`
  - `rpc_controller::IRpcObjectOps`
  - `rpc_controller::IRpcObjectEventOps`
  - `rpc_controller::IRpcController`
  - `rpc_controller::IRpcLifeCycle`
  - A helper type (e.g. `rpc_controller::RpcBoxing`) to expose `RpcBoxByref/RpcUnboxByref/RpcBoxByval/RpcUnboxByval` to Workflow if codegen needs to call them.
- Add `IMPL_TYPE_INFO_RENAME(...)` mappings to place them under a stable Workflow namespace (e.g. `system::rpc_controller::*`).
- Add `BEGIN_STRUCT_MEMBER` / `BEGIN_INTERFACE_MEMBER` registrations.

## Expected unit test shape (from TODO_RPC.md)
LibraryTest should validate:
- `RpcBoxByref` creates caller-side proxies (as `IValue*`) that forward to list ops.
- Mutating the proxy mutates the real underlying collection.
- For observable lists, mutating the real underlying collection triggers `ItemChanged` on the proxy via list event ops.

## Key non-goals / boundaries
- No implementation of interface-object ops (`IRpcObjectOps`) or object event ops is required here.
- No protocol/serialization format is defined here; `Value` is treated as the universal payload.

# AFFECTED PROJECTS
- Build the solution in folder `REPO-ROOT\Test\UnitTest` (`Debug|x64`).
- Always Run UnitTest project `LibraryTest` (`Debug|x64`).
- Always Run UnitTest project `CompilerTest_GenerateMetadata` (`Debug|x64`) because `Source\Library` changes can affect reflected types.
  - If reflection metadata baselines change, update `REPO-ROOT\Test\Resources\Baseline\Reflection64.txt` from `REPO-ROOT\Test\UnitTest\Generated\Reflection64.txt` and rerun.
- Always Run UnitTest project `CompilerTest_LoadAndCompile` (`Debug|x64`).
- Always Run UnitTest project `CppTest` (`Debug|x64`).
- Always Run UnitTest project `CppTest_Metaonly` (`Debug|x64`).
- Always Run UnitTest project `CppTest_Reflection` (`Debug|x64`).

- Build the solution in folder `REPO-ROOT\Test\UnitTest` (`Debug|Win32`).
- Always Run UnitTest project `LibraryTest` (`Debug|Win32`).
- Always Run UnitTest project `CompilerTest_GenerateMetadata` (`Debug|Win32`).
  - If reflection metadata baselines change, update `REPO-ROOT\Test\Resources\Baseline\Reflection32.txt` from `REPO-ROOT\Test\UnitTest\Generated\Reflection32.txt` and rerun.
- Always Run UnitTest project `CppTest` (`Debug|Win32`).
- Always Run UnitTest project `CppTest_Metaonly` (`Debug|Win32`).
- Always Run UnitTest project `CppTest_Reflection` (`Debug|Win32`).

# !!!FINISHED!!!
