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

The controller interface is the abstraction layer between typed wrappers and the protocol. At this layer all values are `object` (maps to `Value` in C++). Interface instances crossing process boundaries are represented as `RpcObjectReference` structs. The controller manages object tokens (local ints), lifecycle (distributed reference counting), and routing (local dispatch vs. remote call).

Each client (process) has one `IWorkflowRpcController` instance. The controller handles both outgoing calls (caller side) and incoming dispatch (callee side). A client can be both caller and callee simultaneously.

### Object Token System

- Every registered object (interface or byref collection) gets a local **object token** (`int`), meaningful only within one client's controller.
- `RpcObjectReference` is a globally-unique struct `(clientId, objectId)` used when references cross process boundaries.
- `ResolveToken` converts a local token to a global reference.
- `ImportReference` converts a global reference to a local token (and acquires a remote reference).
- The controller does **not** inspect argument values. CallerWrapper converts interface arguments to `RpcObjectReference` before calling the controller. CalleeDispatcher detects `RpcObjectReference` in received arguments and imports them.

### Struct: RpcObjectReference

Globally-unique reference to an object. Serializable as a regular struct. Used inside `object[]` arguments and return values when interface instances cross process boundaries.

```
struct RpcObjectReference
{
    clientId : int;
    objectId : int;
}
```

### Interface: IWorkflowRpcEventHandler

Callback interface for receiving event notifications. Created by the CallerWrapper (on the caller side) or by the controller (as a bridge on the callee side). Arguments are `object[]` containing serialized event arguments. Interface arguments in events are represented as `RpcObjectReference`.

```
interface IWorkflowRpcEventHandler
{
    func Invoke(arguments : object[]) : void;
}
```

### Interface: IWorkflowRpcInterfaceDispatcher

Dispatches incoming operations to a local interface object. Generated for each `@rpc:Interface` type on the callee side. The controller calls these methods when a remote client invokes operations on a locally registered object.

- `InvokeMethod`: dispatches to the real method. If the method returns `IAsync^`, the controller (not the dispatcher) handles async completion.
- `GetProperty` / `SetProperty`: dispatches to the real property getter/setter.
- `SubscribeEvent`: subscribes a handler to the real object's event. The handler is a bridge created by the controller that forwards notifications to the remote subscriber.
- `UnsubscribeEvent`: detaches from the real event.

```
interface IWorkflowRpcInterfaceDispatcher
{
    func InvokeMethod(methodId : int, arguments : object[]) : object;
    func GetProperty(propertyId : int) : object;
    func SetProperty(propertyId : int, value : object) : void;
    func SubscribeEvent(eventId : int, handler : IWorkflowRpcEventHandler^) : int;
    func UnsubscribeEvent(subscriptionId : int) : void;
}
```

### Interface: IWorkflowRpcObjectFactory

Factory for creating instances of `@rpc:Ctor` interfaces. Registered on the callee's controller via `RegisterConstructor`. When a remote client calls `CreateObject`, the controller invokes the factory.

The factory receives the controller so it can register sub-objects or set up bidirectional communication during creation.

```
interface IWorkflowRpcObjectFactory
{
    func Create(controller : IWorkflowRpcController^) : IWorkflowRpcInterfaceDispatcher^;
}
```

### Interface: IWorkflowRpcController

Main controller interface. One instance per client.

**Registration (callee side):**
- `RegisterInterfaceObject`: register a local interface object with its dispatcher. Returns object token.
- `RegisterList`: register a local list for byref access. Returns object token.
- `RegisterDictionary`: register a local dictionary for byref access. Returns object token.
- `RegisterConstructor`: register a factory for `@rpc:Ctor` types.

**Token / Reference Translation (both sides):**
- `ResolveToken`: convert local token to `RpcObjectReference` for sending to another client.
- `ImportReference`: convert `RpcObjectReference` to a local token. Also acquires a remote reference.

**Object Lifecycle:**
- `AcquireObject`: increment the remote reference count. Prevents cleanup on the owner.
- `ReleaseObject`: decrement the remote reference count. When count reaches 0 and no local references remain, the object may be cleaned up.
- For smart pointer languages (C++/Workflow): proxy destructors call `ReleaseObject` automatically.
- For GC languages (TypeScript/Python): explicit dispose, weak reference hooks, or session-based bulk release.

**Constructor (@rpc:Ctor):**
- `CreateObject`: request creation of a remote object. Returns object token.

**Interface Operations:**
- `InvokeMethod`: blocking method invocation. Arguments and return value are `object`.
- `InvokeMethodAsync`: async method invocation. Returns `system::Async^` (backed by `IFuture`). The caller awaits it; the controller resolves the future when the remote response arrives.
- `GetProperty` / `SetProperty`: property access. Caching (`@rpc:Cached` / `@rpc:Dynamic`) is handled by the CallerWrapper, not the controller.
- `SubscribeEvent` / `UnsubscribeEvent`: remote event subscription.

**Byref List Operations:**
- For proxying `@rpc:Byref` list/array collections. Each operation is routed to the owner of the list.

**Byref Dictionary Operations:**
- For proxying `@rpc:Byref` dictionary/map collections. Each operation is routed to the owner of the dictionary.

```
interface IWorkflowRpcController
{
    // ===== Registration (callee side) =====

    func RegisterInterfaceObject(dispatcher : IWorkflowRpcInterfaceDispatcher^) : int;
    func RegisterList(list : object) : int;
    func RegisterDictionary(dict : object) : int;
    func RegisterConstructor(typeId : int, factory : IWorkflowRpcObjectFactory^) : void;

    // ===== Token / Reference Translation =====

    func ResolveToken(objectToken : int) : RpcObjectReference;
    func ImportReference(ref : RpcObjectReference) : int;

    // ===== Object Lifecycle =====

    func AcquireObject(objectToken : int) : void;
    func ReleaseObject(objectToken : int) : void;

    // ===== Constructor (@rpc:Ctor) =====

    func CreateObject(typeId : int) : int;

    // ===== Interface Operations =====

    func InvokeMethod(objectToken : int, methodId : int, arguments : object[]) : object;
    func InvokeMethodAsync(objectToken : int, methodId : int, arguments : object[]) : system::Async^;
    func GetProperty(objectToken : int, propertyId : int) : object;
    func SetProperty(objectToken : int, propertyId : int, value : object) : void;
    func SubscribeEvent(objectToken : int, eventId : int, handler : IWorkflowRpcEventHandler^) : int;
    func UnsubscribeEvent(subscriptionId : int) : void;

    // ===== Byref List Operations =====

    func ListGetCount(objectToken : int) : int;
    func ListGet(objectToken : int, index : int) : object;
    func ListSet(objectToken : int, index : int, value : object) : void;
    func ListAdd(objectToken : int, value : object) : int;
    func ListInsert(objectToken : int, index : int, value : object) : int;
    func ListRemoveAt(objectToken : int, index : int) : bool;
    func ListClear(objectToken : int) : void;
    func ListContains(objectToken : int, value : object) : bool;
    func ListIndexOf(objectToken : int, value : object) : int;

    // ===== Byref Dictionary Operations =====

    func DictGetCount(objectToken : int) : int;
    func DictGet(objectToken : int, key : object) : object;
    func DictSet(objectToken : int, key : object, value : object) : void;
    func DictRemove(objectToken : int, key : object) : bool;
    func DictClear(objectToken : int) : void;
    func DictContainsKey(objectToken : int, key : object) : bool;
    func DictGetKeys(objectToken : int) : object[];
    func DictGetValues(objectToken : int) : object[];
}
```
