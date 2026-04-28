@ -1,335 +0,0 @@
# Workflow Interface Based RPC (Attribute Definition)

## Attributes

All attributes here do not have argument.

- `@rpc:Interface` can be used on an interface.
  - `interface` in this document does not include generic interface: `$interface IDerivbed:IBased<TYPE>;`.
- `@rpc:Ctor` can be used only when `@rpc:Interface` is on that type.
- `@rpc:Byval` and `@rpc:Byref` on a property, method, parameter.
- `@rpc:Cached` and `@rpc:Dynamic` on a property.
- `@rpc:Id` on types, events and methods. This attribute will be generated in RPC Workflow metadata.
  - This will be implemented in the future.

## Serializable Types

All types below count as serializable types, but serialization itself is optional:
- Predefined primitive types.
- Struct types.
  - If it is reflection serializable, use the serialization.
  - Otherwise, trigger the fallback serialization (generated).
- Enum types, including flags.
  - Serialized to its interger value.
- Nullable types.
  - Nullable could be applied to primitive types, struct types and enum types.
  - Also all nullable types should just be serializable.
- Strong typed collection types, its element type, key type, value type should all be serializable.
  - Strong typed collection types here mean the type uses Workflow's collection type syntax.
    - `T{}`, `T[]`, `V[K]` and `observe T[]`.
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

- Interface with this attribute could have implementation exposed as a singleton.
- RPC interface only allow registering singleton implementing `@rpc:Ctor` marked interfaces.
- RPC interface has dedicated methods for acquiring such singleton.

## Default options between @rpc:Byval and @rpc:Byref

- `Compile Errors` already limit these attributes on properties / return values / parameters of strong typed collection.
- If none is offered, the following collection types will by default `@rpc:Byref`, others will by default `@rpc:Byval`:
  - `observe T[]`.
  - Strong typed collections whose element or value is an interface, directly or through nested strong typed collections.
- During generating RPC Workflow metadata, `@rpc:Byval` and `@rpc:Byref` from the property will be copied to getter's return value and setter's parameter.
- Event arguments will pick the default value, and no customization could apply (because there is no syntax for putting attributes on event arguments).

## @rpc:Byval

- The whole collection is sent to a client, and it doesn't keep track on changes from the other side.
- All nested collections in this collection share the same behavior.

## @rpc:Byref

- The collection is treated as remote object.
- All nested collections in this collection share the same behavior.

## @rpc:Cached

- This is the default option, for any property.
- When property value is not cached, trigger actual RPC action and cache the result.
- When property value is cached, return the result immediately.
- Cached will be cleared if the associate event (if exists) is triggered.
- Client could proactivaly send property values to cache.

## @rpc:Dynamic

- The getter will trigger actual RPC action immediately.

# Preventing Event Forwarding Deadloops

When an RPC interface (or an `observe T[]`) is shared between two sides, both sides end up with a listener attached to the same logical event:

- On the side that owns the local object, `rpclistener_*` (for `IRpcObjectEventOps`) or the equivalent `IValueObservableList::ItemChanged` hookup (for `IRpcListEventOps`) forwards every raised event to the remote peer via `IRpcController::InvokeEvent` / `IRpcListEventOps::OnItemChanged`.
- On the receiving side, the dispatcher (`IRpcObjectEventOps::InvokeEvent` for object events, `IRpcListEventOps::OnItemChanged` for list events) raises the event on the local wrapper proxy. The wrapper proxy has the same `rpclistener_*` (or `ItemChanged`) hookup attached at construction time, so without further protection it would forward the event right back to the originating side, causing an infinite ping-pong.

To break the loop, an `IRpcController` implementation must suppress the immediate bounce-back of an event that is already being delivered. The contract is:

- While `InvokeEvent` (or `OnItemChanged`) is dispatching event `E` on object reference `R`, the controller marks `(R, E)` as "currently forwarding".
- When the dispatcher resolves `R` to its local target (the wrapper proxy on this side), the controller records `(R, E)` as a pending suppression for that local target's controller.
- When the wrapper proxy raises the event and its `rpclistener_*` calls `InvokeEvent` / `OnItemChanged` again with the same `(R, E)`, the controller consumes the pending suppression and returns immediately without dispatching, breaking the cycle.
- After the original dispatch completes, the "currently forwarding" mark for `(R, E)` is cleared.

This rule applies symmetrically to:
- `IRpcObjectEventOps::InvokeEvent` for events declared on `@rpc:Interface` interfaces (whose listeners are emitted as `rpclistener_*` in generated wrappers, both Workflow-script and C++).
- `IRpcListEventOps::OnItemChanged` for `IValueObservableList` (whose `ItemChanged` listener is hand-written in C++ but follows the same forwarding shape).

The cycle prevention is a property of the `IRpcController` implementation, not of the wire protocol or the generated wrapper code. Generated wrappers always attach the listener on both the local object and the wrapper proxy; it is the controller's responsibility to ensure each end-to-end event delivery results in exactly one bounce, not an unbounded chain.

The reference test harness `RpcDualLifecycleMock` implements this with two pieces of state:
- A static `forwardingEvents` stack tracking `(ref, eventId)` currently being dispatched.
- A per-instance `suppressedEvents` list of pending one-shot suppressions, populated at the moment `RefToPtr` resolves the target during a forwarding dispatch, and consumed at the entry of the next `InvokeEvent` / `OnItemChanged` for the same `(ref, eventId)`.
