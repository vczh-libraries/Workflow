# Workflow Interface Based RPC (Attribute Definition)

## Attributes

All attributes here do not have argument.

- `@rpc:Interface` can be used on an interface.
  - `interface` in this document does not include generic interface: `$interface IDerivbed:IBased<TYPE>;`.
- `@rpc:Ctor` can be used only when `@rpc:Interface` is on that type.
- `@rpc:Byval` and `@rpc:Byref` on a property, method, parameter.
- `@rpc:Cached` and `@rpc:Dynamic` on a property.

## Serializable Types

All types below count as serializable types:
- Predefined primitive types.
- Struct types.
  - If it is reflection serializable, use the serialization.
  - Otherwise, trigger the fallback serialization.
- Enum types, including flags.
  - Serialized to its interger value.
- Strong typed collection types, its element type, key type, value type should all be serializable.
  - Strong typed collection types here mean the type uses Workflow's collection type syntax.
  - Weak typed collection types, like `system::Enumerable` and all others, are just trivial interfaces, they are not serializable because they do not have `@rpc:Interface`.
- Interface with `@rpc:Interface`. Only shared pointer `T^` is acceptable, `T*` is not.

## Compile Errors:

- There are multiple types of property definitions, all count as properties.
- `FULL-NAME` means a full name of a type.
  - If the target is a member, it becomes `type.member`.
  - If the target is a parameter, it becomes `type.method(parameter)`.
- A method is serializable only when all argument types and the return type are serializable.
  - If the return type is `void`, it is also serializable.
- When generating helper functions for an error, multiple error messages with the same format can share one helper function. Only text in `xxx` becomes a parameter (or if `xxx` has only one choice in this helper function, it should not be a parameter).
  - The error code should be `H\d+`.
  - Each helper function will be assigned with a unique error code.
- (AST) does not belong to the error message, it is a hint that this error should be checked by traversing the AST, otherwise it should be checked against `ITypeDescriptor`.

## @rpc:Interface

- (AST) `@rpc:Interface` can only apply to an interface definition, but not `FULL-NAME`.
  - Triggered when it is applied to anything else.
- `@rpc:Interface` cannot be applied to interface `FULL-NAME` because its base type `FULL-NAME` is not serializable.
  - Triggered when any base type does not apply with `@rpc:Interface`.
  - Triggered for each inqualify base type.
- `@rpc:Interface` cannot be applied to interface `FULL-NAME` because its member `MEMBER-NAME` is not serializable.
  - Triggered when the interface type has unserializable members.
  - Triggered for each inqualify member.

## @rpc:Ctor

- (AST) `@rpc:Ctor` can only apply to an interface definition with `@rpc:Interface`.
  - Triggered when it is applied to anything else.
  - Triggered when it is applied to an interface without `@rpc:Interface`.

## @rpc:Byval

- (AST) `@rpc:Byval` can only apply to a property, a method or a parameter.
  - Triggered when it is applied to anything else.
- (AST) `@rpc:Byval` can only be used inside an interface type with `@rpc:Interface`.
  - Not triggered any of the above error is triggered on the same attribute.
  - Triggered when the member it applies to is not in an interface with `@rpc:Interface`.
- `@rpc:Byval` cannot be used on member `MEMBER-NAME` because it does not have a strong typed collection types.
  - For property or parameter, it means the type.
  - For method, it means the return type.
  - Not triggered any of the above error is triggered on the same attribute.
  - Triggered when the member is inqualify.

## @rpc:Byref

- (AST) `@rpc:Byref` can only apply to a property, a method or a parameter.
  - Triggered when it is applied to anything else.
- (AST) `@rpc:Byref` cannot be used on member `MEMBER-NAME` because it already has `@rpc:Byval`.
  - Not triggered any of the above error is triggered on the same attribute.
  - Triggered when it is applied to a member with `@rpc:Byval`.
- (AST) `@rpc:Byref` can only be used inside an interface type with `@rpc:Interface`.
  - Not triggered any of the above error is triggered on the same attribute.
  - Triggered when the member it applies to is not in an interface with `@rpc:Interface`.
- `@rpc:Byref` cannot be used on member `MEMBER-NAME` because it does not have a strong typed collection types.
  - For property or parameter, it means the type.
  - For method, it means the return type.
  - Not triggered any of the above error is triggered on the same attribute.
  - Triggered when the member is inqualify.

## @rpc:Cached

- (AST) `@rpc:Cached` can only apply to a property.
  - Triggered when it is applied to anything else.
- `@rpc:Cached` can only be used inside an interface type with `@rpc:Interface`.
  - Not triggered any of the above error is triggered on the same attribute.
  - Triggered when the member it applies to is not in an interface with `@rpc:Interface`.

## @rpc:Dynamic

- (AST) `@rpc:Dynamic` can only apply to a property.
  - Triggered when it is applied to anything else.
- (AST) `@rpc:Dynamic` cannot be used on member `MEMBER-NAME` because it already has `@rpc:Cached`.
  - Not triggered any of the above error is triggered on the same attribute.
  - Triggered when it is applied to a member with `@rpc:Cached`.
- `@rpc:Dynamic` can only be used inside an interface type with `@rpc:Interface`.
  - Not triggered any of the above error is triggered on the same attribute.
  - Triggered when the member it applies to is not in an interface with `@rpc:Interface`.
  
## Semantic

## @rpc:Interface

## @rpc:Ctor

## @rpc:Byval

- This is the default option, for any property, parameter, method (means return type) is a strong typed collection type.
- Event arguments will always be `@rpc:Byval`.
- The whole collection is sent to a client, and it doesn't keep track on changes from the other side.
  - It only works when the element types are not interfaces.
  - If its element types are collection type, they are passed **by value** as well.

## @rpc:Byref

- The collection is treated as remote object.
  - If its element types are collection type, they are passed **by reference** as well.

## @rpc:Cached

- This is the default option, for any property.
- When property value is not cached, trigger actual RPC action and cache the result.
- When property value is cached, return the result immediately.
- Cached will be cleared if the associate event (if exists) is triggered.
- Client could proactivaly send property values to cache.

## @rpc:Dynamic

- The getter will trigger actual RPC action immediately.

## Controller Interface

For scenario analysis proving this design works, see [TODO_RPC_Scenarios.md](./TODO_RPC_Scenarios.md).

### Overview

The controller interface is a **symmetric** abstraction layer between typed wrappers and the protocol. At this layer all values are `object` (maps to `Value` in C++). All `object` values flowing through these interfaces are serializable values (primitives, structs, enums, or containers of serializable values recursively), or `RpcObjectReference` structs. An `object` value at this layer is **never** an actual registered interface instance — those are always represented as `RpcObjectReference`.

The design is split into four Ops interfaces that separate concerns:
- `IRpcObjectOps`: interface method invocation and service requests.
- `IRpcObjectEventOps`: interface event notification delivery.
- `IRpcListOps`: byref collection (list/dictionary/enumerator) operations.
- `IRpcListEventOps`: `IValueObservableList` change notifications.

All Ops interfaces that use method/event IDs inherit from `IRpcIdSync`, which provides the shared ID mapping (`int[string]`) between the central server and all clients.

`IRpcController` inherits all four and adds registration and object lifecycle management. Each client (process) has one `IRpcController` instance. A client can be both caller and callee simultaneously.

**Caller side — `IRpcController`:** The controller is the caller's API. Through its base interfaces it provides:
- `IRpcObjectOps`: invoke methods on a remote object and request services.
- `IRpcListOps`: invoke operations on a remote byref collection.
- `IRpcObjectEventOps`: broadcast events on a local object to all remote holders.
- `IRpcListEventOps`: broadcast change notifications on a local observable collection to remote holders.

**Callee side — `Register` callbacks:** The `Register` function accepts callbacks (raw pointers `*`, not shared pointers `^`, since the controller does not own the callback lifetime) that handle the reverse direction, and also returns the `int[string]` ID mapping:
- `objectCallback : IRpcObjectOps*`: handles incoming requests to invoke methods on a local object, and to serve `RequestService` queries.
- `listCallback : IRpcListOps*`: handles incoming requests for list/dict operations on a local byref collection.
- `eventCallback : IRpcObjectEventOps*`: handles incoming event notifications from a remote object.
- `listEventCallback : IRpcListEventOps*`: handles incoming change notifications from a remote observable collection.

Standard implementations exist for `listCallback` and `listEventCallback`. After `Register` returns, the controller calls `SyncIds(ids)` on `objectCallback` and `eventCallback` so they know the ID mapping.

**Event model:** There is no explicit subscribe/unsubscribe at the controller level. The C++ object model does not support subscription callbacks. Instead, when a callee registers an object, the objectCallback automatically subscribes to all events on the real object. When an event fires, the callee pushes it via `controller.InvokeEvent(ref, eventId, args)`. The controller routes notifications to all remote clients that hold acquired references to that object. Releasing a reference stops event delivery.

### RpcObjectReference

`RpcObjectReference` is used everywhere as the object identifier. There are no local tokens — all operations take `RpcObjectReference` directly. The controller routes based on `clientId`: if it matches the local client, the operation is handled locally; otherwise it is forwarded to the remote client over the protocol.

The `typeId` field identifies the type of the remote object (the `@rpc:Interface` type, or a collection type for byref collections). It uses the same integer ID scheme as the rest of the protocol (assigned by the central server). `RegisterLocalObject(typeId)` is semantically a constructor for `RpcObjectReference` — the controller does not hold or manage the actual object, only assigns IDs.

```
struct RpcObjectReference
{
    clientId : int;
    objectId : int;
    typeId : int;
}
```

### Interface: IRpcIdSync

Base interface for Ops interfaces that use integer IDs for methods, properties, events, and types. The central server assigns string-to-int mappings. When a client calls `Register`, the controller calls `SyncIds` on the registered `objectCallback` and `eventCallback` so they know the mapping. `Register` also returns the mapping to the caller.

```
interface IRpcIdSync
{
    func SyncIds(ids : int[string]) : void;
}
```

### Interface: IRpcListOps

Operations on byref collections. Has a **standard implementation** that delegates to the real `IValueList` / `IValueDictionary` / `IValueEnumerator` objects. The standard implementation can be passed to `Register` as `listCallback`. CallerWrapper creates proxies that call these methods on the controller; the controller routes to the owner where the registered `listCallback` accesses the real collection.

```
interface IRpcListOps
{
    // ===== Enumerator operations (T{}) =====

    func EnumCreate(ref : RpcObjectReference) : RpcObjectReference;
    func EnumNext(enumerator : RpcObjectReference) : bool;
    func EnumGetCurrent(enumerator : RpcObjectReference) : object;

    // ===== List / Array operations (T[]) =====

    func ListGetCount(ref : RpcObjectReference) : int;
    func ListGet(ref : RpcObjectReference, index : int) : object;
    func ListSet(ref : RpcObjectReference, index : int, value : object) : void;
    func ListAdd(ref : RpcObjectReference, value : object) : int;
    func ListInsert(ref : RpcObjectReference, index : int, value : object) : int;
    func ListRemoveAt(ref : RpcObjectReference, index : int) : bool;
    func ListClear(ref : RpcObjectReference) : void;
    func ListContains(ref : RpcObjectReference, value : object) : bool;
    func ListIndexOf(ref : RpcObjectReference, value : object) : int;

    // ===== Dictionary operations (V[K]) =====

    func DictGetCount(ref : RpcObjectReference) : int;
    func DictGet(ref : RpcObjectReference, key : object) : object;
    func DictSet(ref : RpcObjectReference, key : object, value : object) : void;
    func DictRemove(ref : RpcObjectReference, key : object) : bool;
    func DictClear(ref : RpcObjectReference) : void;
    func DictContainsKey(ref : RpcObjectReference, key : object) : bool;
    func DictGetKeys(ref : RpcObjectReference) : object[];
    func DictGetValues(ref : RpcObjectReference) : object[];
}
```

### Interface: IRpcObjectOps

Operations on `@rpc:Interface` objects. The CallerWrapper calls these on the controller (outgoing). The callee implements these in a callback registered via `Register` (incoming). Inherits `IRpcIdSync` for ID mapping.

- `InvokeMethod` / `InvokeMethodAsync`: dispatch to the real method. Property getters and setters are invoked through these methods — there are no separate property operations at the controller level. Caching (`@rpc:Cached` / `@rpc:Dynamic`) is handled by the CallerWrapper, not the controller. The async variant returns `system::Async^`; the callee's controller awaits the async result and sends it back.
- `ObjectHold(ref, hold)`: the callee's objectCallback calls this to inform the controller whether a remote object can be released. When `hold` is `true`, the controller must not release the object even if ref count reaches 0. When `hold` is `false`, ordinary ref-count rules apply. This is informational, not a release request — only `ReleaseRemoteObject` actually decrements the count.
- `RequestService(typeId)`: request the singleton service for the given `@rpc:Ctor` type. On the caller side, the controller routes the request to the client that provides the service. On the callee side, the objectCallback returns the `RpcObjectReference` for the requested service.

There is no `SubscribeEvent` / `UnsubscribeEvent`. Events are automatically forwarded to all remote clients holding acquired references. See the **Event model** in Overview.

```
interface IRpcObjectOps : IRpcIdSync
{
    func InvokeMethod(ref : RpcObjectReference, methodId : int, arguments : object[]) : object;
    func InvokeMethodAsync(ref : RpcObjectReference, methodId : int, arguments : object[]) : system::Async^;
    func ObjectHold(ref : RpcObjectReference, hold : bool) : void;
    func RequestService(typeId : int) : RpcObjectReference;
}
```

### Interface: IRpcListEventOps

Change notifications for `IValueObservableList` byref collections only. This interface mirrors the single `ItemChanged` event of `IValueObservableList`. Has a **standard implementation** that can be passed to `Register` as `listEventCallback`. On the callee side, the standard implementation monitors the real observable list and calls `controller.OnItemChanged(...)` to push notifications. On the caller side, the standard implementation receives notifications and fires local observable events on the proxy.

```
interface IRpcListEventOps
{
    func OnItemChanged(ref : RpcObjectReference, index : int, oldCount : int, newCount : int) : void;
}
```

### Interface: IRpcObjectEventOps

Delivers event notifications for `@rpc:Interface` objects. Inherits `IRpcIdSync` for ID mapping. Both directions use the same interface:
- **Callee outgoing:** When a real event fires, the callee's objectCallback calls `controller.InvokeEvent(ref, eventId, args)` to send the notification to all holders.
- **Caller incoming:** The controller calls `eventCallback.InvokeEvent(ref, eventId, args)` on the caller, which raises the local proxy event.

```
interface IRpcObjectEventOps : IRpcIdSync
{
    func InvokeEvent(ref : RpcObjectReference, eventId : int, arguments : object[]) : void;
}
```

### Interface: IRpcController

Main controller interface. One instance per client. Inherits all four Ops interfaces for routing operations. Adds registration and object lifecycle management. Service management (`@rpc:Ctor`) is handled through `IRpcObjectOps.RequestService` rather than the controller.

**Registration:**
- `Register`: set up all callee-side callbacks (raw pointers `*`, since the controller does not own callback lifetime). Returns `int[string]` ID mapping. After returning, the controller calls `SyncIds(ids)` on `objectCallback` and `eventCallback`. Standard implementations exist for `listCallback` and `listEventCallback` that operate on real collection objects.
- `RegisterLocalObject`: allocate a new `RpcObjectReference` with the local `clientId`, a new `objectId`, and the given `typeId`. The controller does not hold or manage the actual object — it only assigns IDs. The registered callbacks are responsible for dispatching operations to the real object based on `objectId`.
- `UnregisterLocalObject`: inform the controller that a local object is no longer available. Any remote clients still holding references will get errors on subsequent operations.

**Remote Object Lifecycle:**
- `AcquireRemoteObject`: increment the reference count on the owning client. Called when a new holder obtains a reference.
- `ReleaseRemoteObject`: decrement the reference count. When count reaches 0, the owning client may clean up the object.
- **Convention:** When an `RpcObjectReference` is returned from a method call or event notification, it represents an acquired reference that the receiver owns. The receiver is responsible for calling `ReleaseRemoteObject` when done (typically in the proxy destructor).
- For smart pointer languages (C++/Workflow): proxy destructors call `ReleaseRemoteObject` automatically.
- For GC languages (TypeScript/Python): explicit dispose, weak reference hooks, or session-based bulk release.

```
interface IRpcController
    : IRpcListOps
    , IRpcObjectOps
    , IRpcListEventOps
    , IRpcObjectEventOps
{
    func Register(
        objectCallback : IRpcObjectOps*,
        eventCallback : IRpcObjectEventOps*,
        listCallback : IRpcListOps*,
        listEventCallback : IRpcListEventOps*
    ) : int[string];

    // ===== Local Object Registration =====

    func RegisterLocalObject(typeId : int) : RpcObjectReference;
    func UnregisterLocalObject(ref : RpcObjectReference) : void;

    // ===== Remote Object Lifecycle =====

    func AcquireRemoteObject(ref : RpcObjectReference) : void;
    func ReleaseRemoteObject(ref : RpcObjectReference) : void;
}
```

### Interface: IRpcLifeCycle

This is a caller/callee side interface. It converts between `interface^` and `RefObjectReference`. Both local or remote objects will be properly handled.

Implementation of this interface need to maintain such bidirectional relationship with high performance data structure. Ensure that same objects result in same pointers. Proper `IRpcController` methods should be called in the implementation.

Test projects could also implement dummy or simpler life cycle management for special testing purpose.

```
interface IRpcLifeCycle
{
  prop Controller : IRpcController* {const}
  func RefToPtr(ref : RpcObjectReference) : interface^;
  func PtrToRef(obj : interface^) : RefObjectReference;
}
```
