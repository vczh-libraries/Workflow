# !!!LEARNING!!!

# Orders

- Avoid explicit Workflow types/casts when implicit typing works [5]
- Read and aggregate RPC event exception maps through `IRpcLifecycle::ReadEventException` [3]
- Generated RPC test id maps must come from `rpc_GetIds()` / `SetIdMap` [2]
- Keep RPC-specific fixes out of generic C++ / Workflow codegen surfaces [2]
- Interpret `IRpcWrapperBase` as remote-wrapper identity [2]
- Use internal properties defensively in `RpcDualLifecycleMock` [2]
- Store and manage RPC wrappers as `IRpcWrapperBase` [2]
- Remove pure RPC redirection layers once dispatcher/ops can own the call [2]
- Keep RPC service lookup in `IRpcDispatcher` and register ops locally [2]
- Keep RPC byref boxing/unboxing interface-only [2]
- Split RPC JSON generation into dedicated analyzer/module files [2]
- Move reusable RPC controller/lifecycle code into `Source/Library/Rpc` with strict dependencies [2]
- Reuse production RPC helpers from `TestRpcCompile.cpp` [2]
- Treat `@rpc:IdString` and `@rpc:IdNumber` as internal generated metadata [2]
- Verify copied RPC metadata recompiles to the same metadata [2]
- Delegate predefined RPC JSON handling to C++ static helpers [2]
- Propagate RPC method exceptions via `system::RpcException` [2]
- Apply `RunRpcTestCase` changes to every harness variant [1]
- Keep RPC JSON test harness setup under `VCZH_DEBUG_NO_REFLECTION` [1]
- Use generated strong typed RPC ops for event listeners [1]
- Keep Workflow library helpers out-of-line with explicit `extern` [1]
- Use `CLASS_MEMBER_STATIC_EXTERNALMETHOD` for registering free functions as reflection static methods [1]
- Enable Workflow proxy support for all related implementable interfaces consistently [1]
- Workflow `new interface` on reflected C++ interfaces requires proxy registration (`BEGIN_INTERFACE_MEMBER`) [1]
- Workflow static method call syntax uses `::` not `.` [1]
- `BEGIN_INTERFACE_MEMBER` (proxy-enabled) requires `ValueInterfaceProxy<T>` specialization in C++ header [1]
- `WfTopQualifiedType` vs `WfReferenceType`: use `WfTopQualifiedType` only when global `::` qualification is needed [1]
- Workflow collection types (`system::Dictionary^`, `system::Array^`) use constructor-expression `{}` not constructor-call `()` [1]
- Do not fake reflected struct fields with placeholder `AddField` registrations [1]
- Generate RPC metadata from `ITypeDescriptor`, not original AST declarations [1]
- Resolve RPC default attributes during validation, before wrapper generation [1]
- Order generated RPC type checks with derived interfaces before bases [1]
- Check Workflow wrapper status through raw `object` when interface casts are illegal [1]
- Keep RPC ownership identity in the local-object tracker [1]
- Reflect concrete byref RPC collection wrappers as collection types [1]
- Limit destructive remote mutation detachment to tracked plain containers [1]
- Generate cached RPC properties as per-property typed fields [1]
- Mark RPC sample properties with the intended `@rpc:Dynamic` / `@rpc:Cached` attributes [1]
- Workflow `observe T[]` defaults to RPC byref semantics [1]
- RPC event workarounds should match the actual `Event<T>` behavior [1]
- Do not reflect RPC lifecycle mocks just to expose their controller [1]
- Finalize RPC dispatcher/lifecycle/controller explicitly before unloading Workflow context [1]
- Let wrapper constructors/destructors send `ObjectHold` through the dispatcher [1]
- Replace `RpcByvalLifecycleMock` with a real dual-lifecycle setup for wrapper tests [1]
- Use strong `cast`, not weak `as`, for protocol-owned values [1]
- Use `RpcByvalReturnValue` slots for JSON byval return lifetimes [1]
- Validate RPC JSON ops payloads as `JsonNode` values [1]
- Use pair arrays for RPC JSON map schemas [1]
- Keep RPC wrapper generation recursive across inherited interface members [1]
- Reject RPC internal transport structs from user RPC signatures [1]
- Workflow catch variables are `system::Exception^`; use `.Message` [1]
- Keep TypeScript RPC dispatcher envelopes separate from value schemas [1]

# Refinements

## Avoid explicit Workflow types/casts when implicit typing works

In generated or handwritten Workflow script, avoid explicit type syntax whenever the compiler can infer or implicitly convert correctly. Prefer `var v = e;` when the local type can be omitted, and prefer `var v : T = e;` over `var v = e over T;` when the type is unavoidable. Remove unnecessary `cast`, `as`, and `infer` expressions when implicit conversion works, and prefer `cast *` over spelling the target type when the surrounding context already expects that type.

## Generated RPC test id maps must come from `rpc_GetIds()` / `SetIdMap`

Generated `TestCasesRpc.cpp` must not rebuild lifecycle id maps from an assumed `RegisterService` order; that order is not enforceable. Follow the working runtime-test pattern: get the map from `instance.rpc_GetIds()`, convert it through `UnboxParameter<Dictionary<WString, vint>>(BoxParameter(...))`, and pass `idMap.Ref()` to both lifecycles via `SetIdMap`.

## Keep RPC-specific fixes out of generic C++ / Workflow codegen surfaces

When an RPC test exposes a failure, keep the fix in RPC metadata generation, RPC wrapper Workflow generation, `WfLibraryRpc*`, or the RPC test lifecycle unless the generic compiler/codegen surface is proven to own the bug. The user explicitly rejected broad changes to generic C++ wrapper/expression generation for RPC-only event and signature issues.

## Interpret `IRpcWrapperBase` as remote-wrapper identity

In RPC samples and lifecycle code, implementing `system::IRpcWrapperBase` means the object is a remote wrapper in the current lifecycle. Not implementing it means the object is local in the current lifecycle, though that same object can become a wrapper after crossing to another lifecycle. Use this distinction when validating wrapper/local behavior.

## Use internal properties defensively in `RpcDualLifecycleMock`

When assigning or reading local-object and wrapper tracker internal properties, treat unexpected absence or mismatched client ids as bugs. Use defensive `CHECK_ERROR` assertions instead of fallback behavior; silent recovery in test-only lifecycle code hides ownership bugs.

## Store and manage RPC wrappers as `IRpcWrapperBase`

All RPC wrappers, including predefined collection wrappers and generated interface wrappers, should implement and be stored as `IRpcWrapperBase` (`Ptr<IRpcWrapperBase>` or raw `IRpcWrapperBase*` as appropriate), not as generic `IDescriptable`. If a wrapper cast fails, that is an invariant failure.

## Remove pure RPC redirection layers once dispatcher/ops can own the call

Classes such as lifecycle adapters or object-op redirection wrappers should be removed when they only forward to another object without owning behavior. Prefer direct registration through the controller, dispatcher, or generated ops implementation that already has the required authority.

## Keep RPC service lookup in `IRpcDispatcher` and register ops locally

`IRpcDispatcher` should be the single authority for service lookup. Generated `IRpcObjectOps::RegisterService` can validate type ids and register service refs, but `IRpcObjectOps::RequestService` should not exist. Lifecycles should register their ops locally, ask the dispatcher for a service ref, and then use the ref's client id to decide local-vs-wrapper behavior.

## Keep RPC byref boxing/unboxing interface-only

`RpcBoxByref` / `RpcUnboxByref` are by design for shared pointers of interfaces. Primitive values, nullable values, and byval collections should bypass these calls and convert directly between their actual types and `object`; do not broaden the public API to make unrelated transport values fit.

## Split RPC JSON generation into dedicated analyzer/module files

Keep RPC JSON responsibilities separated from ordinary RPC wrapper generation. JSON d.ts generation belongs in the JSON d.ts analyzer file, JSON serialization helpers and JSON ops belong in the JSON serialization analyzer file, and generated JSON Workflow should live in separate `Wrapper_*_Json.txt` modules instead of being mixed into `Wrapper_*.txt`.

## Move reusable RPC controller/lifecycle code into `Source/Library/Rpc` with strict dependencies

Reusable RPC controller, lifecycle, and wrapper implementations belong under `Source/Library/Rpc` and library vcxitems, not copied through test projects. The base lifecycle layer must not depend on the dual-lifecycle test layer; the thin `RpcDualLifecycleMock` may depend on the base, but not the other way around.

## Use `CLASS_MEMBER_STATIC_EXTERNALMETHOD` for registering free functions as reflection static methods

When exposing existing namespace-scope free functions (e.g. `vl::rpc_controller::RpcBoxByref`) as static methods on a Workflow type descriptor (e.g. `system::IRpcLifeCycle`), use `CLASS_MEMBER_STATIC_EXTERNALMETHOD` in `WfLibraryReflection.cpp` instead of adding C++ static member wrapper functions to the class declaration. Example:
```cpp
CLASS_MEMBER_STATIC_EXTERNALMETHOD(RpcBoxByref, { L"trivial" _ L"lc" }, Value(*)(const Value&, IRpcLifeCycle*), vl::rpc_controller::RpcBoxByref)
```
This keeps the class declaration clean and avoids code duplication.

## Apply `RunRpcTestCase` changes to every harness variant

When the user says `RunRpcTestCase` needs to change, apply the change to every variant of that test harness. Do not add helper functions only to share code between variants unless the user explicitly asks for that; duplication is acceptable when it keeps JSON-value and flat harness behavior clear.

## Keep RPC JSON test harness setup under `VCZH_DEBUG_NO_REFLECTION`

Split RPC C++ test harness paths by representation: `RunRpcTestCase_JsonValue` owns JSON serializers, JSON object/event ops, JSON strong typed ops, JSON dispatchers, and `SetSerializer` calls under `VCZH_DEBUG_NO_REFLECTION`; `RunRpcTestCase_Flat` should use flat ops and should not require a serializer.

## Use generated strong typed RPC ops for event listeners

Generated RPC wrapper listeners should receive and call the generated strong typed ops object for local event dispatch instead of invoking generic dispatcher event APIs directly. Attach local event listeners only when the generated assembly has events, so no-event samples do not reference missing `rpclistener_Attach` symbols.

## Keep Workflow library helpers out-of-line with explicit `extern`

For namespace-level helper functions in `Source/Library/WfLibrary*.(h|cpp)` and `Source/Library/Rpc/WfLibrary*.(h|cpp)`, keep non-constant implementations in `.cpp` files instead of `inline` header definitions. Put explicit `extern` on forward declarations, even when C++ would not require it, to match the Workflow library declaration style.

## Enable Workflow proxy support for all related implementable interfaces consistently

When adding Workflow proxy support (`BEGIN_INTERFACE_MEMBER` + `BEGIN_INTERFACE_PROXY_SHAREDPTR`) for an interface hierarchy (e.g. RPC callback interfaces), enable it for all leaf/intermediate implementable interfaces consistently, not just the ones immediately needed by the current task. For example, `IRpcListOps` and `IRpcListEventOps` should get proxy support alongside `IRpcObjectOps` and `IRpcObjectEventOps`, even if list-related wrappers are not yet generated. `IRpcController` and `IRpcLifeCycle` remain `NOPROXY` because they are not implemented from Workflow.

## Workflow `new interface` on reflected C++ interfaces requires proxy registration (`BEGIN_INTERFACE_MEMBER`)

Workflow's `new InterfaceType^ { ... }` expression only works on interfaces that have proxy support registered in C++ reflection. If the interface uses `BEGIN_INTERFACE_MEMBER_NOPROXY`, the `new interface` expression will fail with semantic errors like `A6` or `A23`. The fix is to switch the interface to `BEGIN_INTERFACE_MEMBER` and provide a `ValueInterfaceProxy<T>` specialization -- not to introduce intermediate Workflow-defined interfaces as a workaround.

## Workflow static method call syntax uses `::` not `.`

In Workflow script, static method calls on type descriptors use `::` syntax, not `.` member-expression syntax. For example, call `system::IRpcLifeCycle::RpcBoxByref(value, lc)` not `system::IRpcLifeCycle.RpcBoxByref(value, lc)`. Using `.` triggers semantic error `A7` (type symbol used as an expression).

## `BEGIN_INTERFACE_MEMBER` (proxy-enabled) requires `ValueInterfaceProxy<T>` specialization in C++ header

Switching a reflected interface from `BEGIN_INTERFACE_MEMBER_NOPROXY` to `BEGIN_INTERFACE_MEMBER` in `WfLibraryReflection.cpp` requires a corresponding `ValueInterfaceProxy<T>` specialization (using `BEGIN_INTERFACE_PROXY_SHAREDPTR` / `INTERFACE_PROXY_FORWARD` macros) in the reflection header (`WfLibraryReflection.h`). Without this, the C++ build fails because the reflection machinery tries to use a missing `Create` member on the proxy template.

## `WfTopQualifiedType` vs `WfReferenceType`: use `WfTopQualifiedType` only when global `::` qualification is needed

`WfTopQualifiedType` emits a leading `::` in printed output, functioning like C++ global scope qualification. Use it when you explicitly need to reference a name from the global scope (e.g. to avoid ambiguity with a local name). In most generated code, `WfReferenceType` is the correct root node for type names, with `WfChildType` chaining for qualified fragments like `system::IRpcObjectOps`. Defaulting to `WfTopQualifiedType` when global qualification is unnecessary will produce names like `::MyType` that may not match what the parser expects in certain contexts.

## Workflow collection types (`system::Dictionary^`, `system::Array^`) use constructor-expression `{}` not constructor-call `()`

In Workflow, `system::Dictionary^` and `system::Array^` are interface types. You cannot use constructor-call syntax like `new (system::Dictionary^)()` because the parser reports `A6` (arguments not allowed in `new interface` expression). Instead, use the collection constructor expression syntax (empty `{}` in AST form). This applies to any generated code that allocates dictionaries or arrays in Workflow wrapper modules.

## Do not fake reflected struct fields with placeholder `AddField` registrations

For reflected struct types, especially Workflow attribute structs, fields should be real serializable struct fields registered with normal `STRUCT_MEMBER` patterns. Do not fake collection-typed attribute fields by calling `AddField` with placeholder `FieldInfoImpl` instances inside `BEGIN_STRUCT_MEMBER`; the reflectable attribute system is intended to use serializable primitive/value fields.

## Generate RPC metadata from `ITypeDescriptor`, not original AST declarations

RPC metadata should be recreated from reflected `ITypeDescriptor` instances. Input module ASTs may limit which `@rpc:Interface` types are selected, but the original AST declarations should not be used to copy attributes, find original members, or reconstruct metadata details.

## Resolve RPC default attributes during validation, before wrapper generation

Default RPC attributes such as `@rpc:Byval` / `@rpc:Byref` for collection arguments and `@rpc:Dynamic` / `@rpc:Cached` for properties should be filled into RPC metadata during validation. Wrapper generation should consume explicit metadata attributes and report `I*` generation errors if required metadata is missing, rather than re-deciding defaults.

## Order generated RPC type checks with derived interfaces before bases

When generated RPC test or wrapper code checks interface type ids, order derived/leaf interfaces before base interfaces. If a base type is checked first, derived objects can be treated as the base type. Use `PartialOrderPreprocessor` or equivalent inheritance ordering instead of relying on declaration order.

## Check Workflow wrapper status through raw `object` when interface casts are illegal

Workflow may reject explicit conversion from a typed interface value such as `IService^` to `system::IRpcWrapperBase^` unless the interface surface declares that relationship. To test wrapper status, keep the original value as raw `object` and perform the `IRpcWrapperBase^` check on that raw object.

## Keep RPC ownership identity in the local-object tracker

Persistent local-object ownership identity should stay with the local-object tracker data. Do not introduce another embedded property type just to record the owner; keep the tracker authoritative and rename it clearly when necessary (e.g. `RpcDualLocalObjectTracker`).

## Reflect concrete byref RPC collection wrappers as collection types

Nested byref collection wrappers must surface as their concrete collection wrapper descriptors, not only as `system::IRpcWrapperBase`. Add `Description<...>` inheritance, include the wrapper classes in the reflection type list, and register collection-interface base classes so nested element access, iteration, and conversions see the collection API.

## Limit destructive remote mutation detachment to tracked plain containers

When a remote side destructively mutates a locally tracked plain container, detach or clone only at that mutation point as needed to preserve local lifetimes. Do not add a broad public lifecycle interface for this test-lifecycle behavior, and do not apply the same treatment to observable lists when event behavior depends on shared identity.

## Generate cached RPC properties as per-property typed fields

For each generated `@rpc:Cached prop NAME : TYPE`, create strongly typed fields like `NAME<Cached> : TYPE` and `NAME<Available> : bool` inside the wrapper object. Avoid a generic `_rpcCache_Value` bucket that loses property-specific type information.

## Mark RPC sample properties with the intended `@rpc:Dynamic` / `@rpc:Cached` attributes

RPC samples should make property cache behavior explicit unless the sample is specifically testing defaults. `*_PropByref.txt` and `*_PropByval.txt` should put `@rpc:Byref` / `@rpc:Byval` on the property itself, while `*_PropDefault.txt` intentionally omits those attributes. Existing non-default property samples generally need `@rpc:Dynamic` unless they are testing `@rpc:Cached`.

## Workflow `observe T[]` defaults to RPC byref semantics

For RPC metadata defaults, `observe T[]` behaves like a collection that should default to byref, even when ordinary `T[]` would default byval for non-interface element types. In nested container syntax, remember Workflow's left association: for example `T[][]` becomes `observe (observe T[])[]` when both list levels are observable.

## RPC event workarounds should match the actual `Event<T>` behavior

Do not change generic expression generation just to compensate for RPC event dispatch unless `Event<T>` itself requires it. After the `Event<T>::operator()` move bug was fixed, generated RPC wrappers no longer need to introduce local variables solely to keep event arguments as lvalues before invoking multiple handlers.

## Do not reflect RPC lifecycle mocks just to expose their controller

RPC lifecycle mocks do not need reflection registration merely because Workflow needs a controller. `IRpcLifeCycle::GetController()` already returns the controller interface; cast or pass the lifecycle through the existing interface surface instead of reflecting the mock class.

## Finalize RPC dispatcher/lifecycle/controller explicitly before unloading Workflow context

RPC finalization should be explicit because runtime-test finalization can run Workflow code and must happen before unloading the Workflow context. Call dispatcher finalization from the test harness and let it finalize lifecycles/controllers in their implementation-defined order; after finalization, RPC interfaces should no longer be touched.

## Let wrapper constructors/destructors send `ObjectHold` through the dispatcher

Wrapper lifetime tracking belongs to wrapper construction/destruction. Constructors should send `ObjectHold(ref, currentClientId, true)` through the dispatcher, and destructors should send `ObjectHold(ref, currentClientId, false)` unless disconnected. `DisconnectFromLifecycle` should happen when a proxy is no longer usable, not be delayed until finalization.

## Replace `RpcByvalLifecycleMock` with a real dual-lifecycle setup for wrapper tests

`RpcByvalLifecycleMock` forced collection wrappers even when its only lifecycle owned the object, which violated lifecycle ownership contracts. Use two lifecycle instances so one side owns the object and the other side receives a wrapper through `PtrToRef` / `RefToPtr` or the boxing helpers.

## Use strong `cast`, not weak `as`, for protocol-owned values

When generated RPC JSON or ops code receives a value that the protocol guarantees to have a specific type, use strong `cast`. Do not use weak `as` plus fallback logic; if the protocol value is wrong, raising immediately is better than continuing with a null.

## Use `RpcByvalReturnValue` slots for JSON byval return lifetimes

For `@rpc:Byval` method returns, the callee side may need to keep recursive copied collections alive until the caller deserializes and unboxes them. Return a `RpcByvalReturnValue` containing the value and slot, cache the copy in generated ops, and have caller-side wrappers call `IRpcObjectOps::EndInvokeMethod(slot)` after finishing the return conversion.

## Delegate predefined RPC JSON handling to C++ static helpers

Generated `rpcjson_Serialize` / `rpcjson_Deserialize` should handle module-specific custom types, then delegate predefined primitive, byref collection, and internal transport struct handling to C++ static helpers registered on `IRpcLifeCycle`. In particular, `system::RpcException` and `system::RpcObjectReference` belong in predefined JSON handling, not in per-RPC generated serializers. This avoids repeating large predefined-type JSON code in every generated wrapper module.

## Reuse production RPC helpers from `TestRpcCompile.cpp`

`TestRpcCompile.cpp` should not duplicate production helpers such as `MangleRpcFullName` or `FindRpcTypeDescriptor`. Reuse the implementation used by RPC generation, and factor the test flow into compile/link/metadata-verification helpers so refactors do not touch generated outputs unnecessarily.

## Treat `@rpc:IdString` and `@rpc:IdNumber` as internal generated metadata

`@rpc:IdString(string)` and `@rpc:IdNumber(int)` are internal metadata attributes for generated ids. Ignore user-authored instances on input RPC declarations and do not copy them into metadata; instead, attach generated values to interfaces, methods, and events from the validated ordered-id list.

## Verify copied RPC metadata recompiles to the same metadata

When testing RPC metadata stability, copy the metadata module, clear the manager, recompile the copied metadata, and compare `WfPrint` output from the old and new metadata modules. This guards against repeated compilation perturbing `orderedIds` or generated metadata attributes.

## Read and aggregate RPC event exception maps through `IRpcLifecycle::ReadEventException`

RPC object-event and observable-list event transports return an internal exception map keyed by lifecycle client id. Broadcast paths must attempt every target lifecycle and merge all returned maps without stopping at the first exception. Generated object wrappers and C++ container wrappers should call `IRpcLifecycle::ReadEventException` (backed by `vl::rpc_controller::ReadEventException`) so null or empty maps do nothing, while non-empty maps raise `vl::Exception` with messages formatted as `clientId:message;...` including the trailing semicolon. For JSON event ops, `IRpcObjectEventOps::InvokeEvent` and `IRpcListEventOps::OnItemChanged` return `Value`; deserialize that value first, then call `ReadEventException`.

## Validate RPC JSON ops payloads as `JsonNode` values

When testing JSON RPC dispatcher paths, every `Value` argument, return value, or array element crossing the ops interfaces should be backed by `Ptr<JsonNode>`. Use a JSON dispatcher mock to `CHECK_ERROR` on non-JSON values so generator bugs surface immediately; generated void-return branches should return a concrete JSON null node instead of an empty `Value`.

## Use pair arrays for RPC JSON map schemas

RPC JSON TypeScript schemas represent maps as `[K, V][]`, including unknown map schemas and serialized dictionaries such as `RpcException[int]`. Keep list/observable-list unknown schemas separate from map unknown schemas, and update generated `.d.ts` declarations and JSON values together when the schema shape changes.

## Keep RPC wrapper generation recursive across inherited interface members

Generated RPC wrappers for interface inheritance must recursively include base-interface members, not just members declared on the current interface. This includes inherited cached-property invalidators/getters, inherited methods, and inherited events on derived wrapper interfaces and wrapper factories.

## Propagate RPC method exceptions via `system::RpcException`

Generated service-side `IRpcObjectOps::InvokeMethod` implementations should catch Workflow exceptions from the actual method call and return `system::RpcException { message = ex.Message }`. Caller-side wrappers should detect that internal transport result and raise the message locally, skipping normal return conversion and `EndInvokeMethod` on the exception path. JSON caller ops should run `rpcjson_Deserialize` on every method return value before calling `ReadMethodException`, even for primitive returns.

## Reject RPC internal transport structs from user RPC signatures

`system::RpcObjectReference` and `system::RpcException` are internal RPC transport structs. RPC validation should reject them in method return values, property values, method arguments, and event arguments with the appropriate H-series analyzer errors; user-authored RPC APIs should not expose these structs directly.

## Workflow catch variables are `system::Exception^`; use `.Message`

Workflow `catch (ex)` variables are exception objects. When a script needs the text message for string formatting or transport conversion, read `ex.Message` instead of assuming `ex` itself is already a string.

## Keep TypeScript RPC dispatcher envelopes separate from value schemas

`Test/TypeScript/Rpc.d.ts` owns the shared dispatcher envelope schema for `IRpcListOps`, `IRpcObjectOps`, `IRpcListEventOps`, and `IRpcObjectEventOps`. Generated `Serialization_*.d.ts` files own concrete value schemas; connect the two with a generic payload type such as `KnownTypeSchema | UnknownTypeSchema`, and only add `<T>` to envelope interfaces that actually carry serialized payloads. `SendToClient_*` requests include `targetClientId`, broadcast requests omit it, and all responses include both `sourceClientId` and `targetClientId`.
