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

The controller interface is a **symmetric** abstraction layer between typed wrappers and the protocol. At this layer all values are `object` (maps to `Value` in C++). Interface instances crossing process boundaries are represented as `RpcObjectReference` structs.

The design is split into four Ops interfaces that separate concerns:
- `IWorkflowRpcObjectOps`: interface method/property/event operations.
- `IWorkflowRpcObjectEventOps`: event notification delivery.
- `IWorkflowRpcListOps`: byref collection (list/dictionary/enumerator) operations.
- `IWorkflowRpcListEventOps`: observable collection change notifications.

`IWorkflowRpcController` inherits all four and adds registration and lifecycle management. Each client (process) has one `IWorkflowRpcController` instance. A client can be both caller and callee simultaneously.

**Symmetry:** The same `IWorkflowRpcObjectOps` interface is used in both directions:
- **Caller side**: CallerWrapper calls `controller.InvokeMethod(ref, ...)`. The controller (which inherits `IWorkflowRpcObjectOps`) routes the call to the callee.
- **Callee side**: The callee registers an `IWorkflowRpcObjectOps` callback via `Register(...)`. The controller invokes `objectCallback.InvokeMethod(ref, ...)` when an incoming request arrives.

The same symmetry applies to `IWorkflowRpcObjectEventOps`:
- **Callee side (outgoing)**: When a real event fires, the callee's objectCallback calls `controller.InvokeEvent(ref, eventId, args)` to push the notification.
- **Caller side (incoming)**: The controller invokes `eventCallback.InvokeEvent(ref, eventId, args)` on the subscriber, which raises the local proxy event.

**Standard implementations:** `IWorkflowRpcListOps` and `IWorkflowRpcListEventOps` have standard default implementations that operate on real list/dictionary objects. The `Register` function does not need callbacks for these.

### RpcObjectReference

`RpcObjectReference` is used everywhere as the object identifier. There are no local tokens — all operations take `RpcObjectReference` directly. The controller routes based on `clientId`: if it matches the local client, the operation is handled locally; otherwise it is forwarded to the remote client over the protocol.

```
struct RpcObjectReference
{
    clientId : int;
    objectId : int;
}
```

### Interface: IWorkflowRpcObjectFactory

Factory for creating singleton service instances (`@rpc:Ctor`). Registered on a callee's controller via `RegisterService`. Only one client may register a factory for a given `typeId`; duplicate registrations throw.

The factory receives the controller so it can register sub-objects during creation. It creates the real object, calls `controller.RegisterObject(obj)`, and returns the reference.

```
interface IWorkflowRpcObjectFactory
{
    func Create(controller : IWorkflowRpcController^) : RpcObjectReference;
}
```

### Interface: IWorkflowRpcListOps

Operations on byref collections. Has a **standard implementation** provided by the controller that delegates to the real `IValueList` / `IValueDictionary` / `IValueEnumerator` objects. CallerWrapper creates proxies that call these methods; the controller routes to the owner where the standard implementation accesses the real collection.

```
interface IWorkflowRpcListOps
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

### Interface: IWorkflowRpcObjectOps

Operations on `@rpc:Interface` objects. The CallerWrapper calls these on the controller (outgoing). The callee implements these in a callback registered via `Register` (incoming).

- `InvokeMethod` / `InvokeMethodAsync`: dispatch to the real method. The async variant returns `system::Async^`; the callee's controller awaits the async result and sends it back.
- `GetProperty` / `SetProperty`: dispatch to the real property getter/setter. Caching (`@rpc:Cached` / `@rpc:Dynamic`) is handled by the CallerWrapper, not the controller.
- `SubscribeEvent`: subscribe to an event. Returns a subscription ID generated by the callee's callback. No per-subscription handler is needed — notifications are delivered through `IWorkflowRpcObjectEventOps.InvokeEvent`.
- `UnsubscribeEvent`: unsubscribe using the subscription ID.

```
interface IWorkflowRpcObjectOps
{
    func InvokeMethod(ref : RpcObjectReference, methodId : int, arguments : object[]) : object;
    func InvokeMethodAsync(ref : RpcObjectReference, methodId : int, arguments : object[]) : system::Async^;
    func GetProperty(ref : RpcObjectReference, propertyId : int) : object;
    func SetProperty(ref : RpcObjectReference, propertyId : int, value : object) : void;
    func SubscribeEvent(ref : RpcObjectReference, eventId : int) : int;
    func UnsubscribeEvent(ref : RpcObjectReference, subscriptionId : int) : void;
}
```

### Interface: IWorkflowRpcListEventOps

Change notifications for observable byref collections. Has a **standard implementation** provided by the controller. When a registered observable collection changes, the callee-side standard implementation calls these methods on the controller to push notifications. The caller-side standard implementation receives them and updates local proxies.

```
interface IWorkflowRpcListEventOps
{
    func OnListItemInserted(ref : RpcObjectReference, index : int, value : object) : void;
    func OnListItemRemoved(ref : RpcObjectReference, index : int, count : int) : void;
    func OnListItemSet(ref : RpcObjectReference, index : int, value : object) : void;
    func OnListCleared(ref : RpcObjectReference) : void;
    func OnDictItemSet(ref : RpcObjectReference, key : object, value : object) : void;
    func OnDictItemRemoved(ref : RpcObjectReference, key : object) : void;
    func OnDictCleared(ref : RpcObjectReference) : void;
}
```

### Interface: IWorkflowRpcObjectEventOps

Delivers event notifications for `@rpc:Interface` objects. Both directions use the same interface:
- **Callee outgoing:** When a real event fires, the callee's objectCallback calls `controller.InvokeEvent(ref, eventId, args)` to send the notification to subscribers.
- **Caller incoming:** The controller calls `eventCallback.InvokeEvent(ref, eventId, args)` on the subscriber.

```
interface IWorkflowRpcObjectEventOps
{
    func InvokeEvent(ref : RpcObjectReference, eventId : int, arguments : object[]) : void;
}
```

### Interface: IWorkflowRpcController

Main controller interface. One instance per client. Inherits all four Ops interfaces for routing operations. Adds registration, lifecycle, and service management.

**Registration:**
- `Register`: set up the callee-side callbacks. Called once during initialization. The controller uses the standard implementation for `IWorkflowRpcListOps` and `IWorkflowRpcListEventOps`, so only object ops and event ops callbacks are needed.
- `RegisterObject`: register a local object (interface instance, list, or dictionary) in the controller's table. Returns `RpcObjectReference` with the local `clientId` and a new `objectId`. The registered `objectCallback` is responsible for dispatching operations to this object based on its `objectId`.

**Object Lifecycle:**
- `AcquireObject`: increment the reference count on the owning client. Called when a new holder obtains a reference.
- `ReleaseObject`: decrement the reference count. When count reaches 0 and no local references remain, the object may be cleaned up.
- **Convention:** When an `RpcObjectReference` is returned from a method call or event notification, it represents an acquired reference that the receiver owns. The receiver is responsible for calling `ReleaseObject` when done (typically in the proxy destructor).
- For smart pointer languages (C++/Workflow): proxy destructors call `ReleaseObject` automatically.
- For GC languages (TypeScript/Python): explicit dispose, weak reference hooks, or session-based bulk release.

**Service (@rpc:Ctor):**
- `RegisterService`: register a singleton factory for a `typeId`. Only one client may register per `typeId`; duplicate registrations throw.
- `RequestService`: request the singleton service. Routes to the registered client, invokes the factory (on first call), and returns the `RpcObjectReference`. Subsequent calls return the same instance.

```
interface IWorkflowRpcController
    : IWorkflowRpcListOps
    , IWorkflowRpcObjectOps
    , IWorkflowRpcListEventOps
    , IWorkflowRpcObjectEventOps
{
    func Register(
        objectCallback : IWorkflowRpcObjectOps^,
        eventCallback : IWorkflowRpcObjectEventOps^
    ) : void;

    // ===== Object Registration =====

    func RegisterObject(object : object) : RpcObjectReference;

    // ===== Object Lifecycle =====

    func AcquireObject(ref : RpcObjectReference) : void;
    func ReleaseObject(ref : RpcObjectReference) : void;

    // ===== Service (@rpc:Ctor) =====

    func RegisterService(typeId : int, factory : IWorkflowRpcObjectFactory^) : void;
    func RequestService(typeId : int) : RpcObjectReference;
}
```
