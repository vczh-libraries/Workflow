# Workflow Interface Based RPC (Generated Wrappers)

This document describes the generated C++ functions produced from Workflow RPC metadata and how an application should wire them into an RPC lifecycle. Attribute rules and serializable type rules are covered by `TODO_RPC_Definition.md`.

## When to Call Generated Functions

Call `rpc_GetIds()` before a lifecycle starts handling RPC objects. It returns the dictionary from RPC type and member names to numeric ids. The lifecycle uses this dictionary to decide whether a local object can be represented by an RPC type id and to resolve service names.

Create the generated ops objects once for each lifecycle:

- `rpcops_IRpcObjectOps(lc)` creates the callee-side object operations.
- `rpcops_IRpcObjectEventOps(lc)` creates the receive-side object event operations.
- `rpcops_IOps_Create(lc)` creates strongly typed caller-side method operations used by generated wrappers.

Register these objects with the lifecycle's controller, together with the runtime-provided list operations and list event operations. The generated object ops receive remote method calls, object holds, object unholds, and service registrations. The generated object event ops receive remote object events. The generated caller-side ops are passed to wrappers so wrapper methods can send method calls through the dispatcher.

Register a wrapper factory that calls `rpcwrapper_Create(ref, lc, ops)`. The lifecycle calls this factory when `RefToPtr(ref)` sees that `ref.clientId` belongs to another client. `rpcwrapper_Create` switches on `ref.typeId` and returns the generated wrapper interface for that remote object.

Generated wrappers implement the original RPC interfaces and `IRpcWrapperBase`. Call interface methods on them exactly as normal C++ interface methods. A wrapper method checks that it is still connected to a lifecycle, then delegates to the generated caller-side ops. The wrapper destructor sends an object-unhold message to the owner lifecycle unless the wrapper has already been disconnected.

Call `IRpcLifecycle::RegisterService(fullName, service)` on the service-owning lifecycle to expose a singleton service for an `@rpc:Ctor` interface. The generated `IRpcObjectOps::RegisterService` implementation validates the type id, converts the service object to `RpcObjectReference`, and registers it with the dispatcher. Call `IRpcLifecycle::RequestService(fullName)` from a client lifecycle to get the service as either a local object or a generated wrapper.

The helper predicates `rpcwrapper_IsInterfaceTypeId(typeId)` and `rpcwrapper_IsCtorInterfaceTypeId(typeId)` are generated for lifecycle and ops validation. Application code usually does not call them directly unless it is implementing custom lifecycle integration.

## When to Call Generated JSON Serialization Functions

Use the JSON variants when the RPC payload boundary should carry `JsonNode` values instead of direct reflected `Value` objects.

Create `rpcops_IRpcSerializer()` and set it on the lifecycle before registering JSON object or list operations. The serializer delegates to generated `rpcjson_Serialize(value)` and `rpcjson_Deserialize(node)` for unknown values.

Use these JSON ops instead of the non-JSON ops on a JSON lifecycle:

- `rpcops_IRpcObjectOpsJson(lc)`
- `rpcops_IRpcObjectEventOpsJson(lc)`
- `rpcops_IOps_CreateJson(lc)`

The wrapper factory still calls `rpcwrapper_Create(ref, lc, ops)`, but the `ops` argument should be the JSON caller-side ops. Generated wrappers do not need a separate JSON wrapper type; the difference is in the ops object they delegate to.

The generated `rpcjson_Serialize_*` and `rpcjson_Deserialize_*` functions are for known enum, struct, and collection types. The generated `rpcjson_Serialize(value)` and `rpcjson_Deserialize(node)` functions are for unknown values, including collection elements and other values whose exact static type is not known at the call site. `RpcObjectReference` serialization is always generated because interface references and byref collections depend on it.

## How Generated Functions Finish Their Work

A wrapper method finishes synchronously. It boxes all arguments according to the RPC byval/byref rules, sends `InvokeMethod(ref, methodId, arguments)` through `IRpcDispatcher::SendToClient_ObjectOps(ref.clientId)`, waits for the target lifecycle to finish the call, then unboxes and returns the result.

On the receiving lifecycle, generated `IRpcObjectOps::InvokeMethod` switches on `methodId`, converts `ref` back to the target object through `RefToPtr(ref)`, unboxes arguments, calls the real object method, boxes the return value, and returns it to the caller. Unknown method ids raise an exception.

Generated `IRpcObjectOps::InvokeMethod` catches exceptions raised by the real object method and returns `system::RpcException` with the exception message. Unknown method ids are local dispatch errors and escape directly instead of being encoded as `system::RpcException`. User RPC signatures cannot return or accept `system::RpcException`, so caller-side ops can distinguish this transport value from successful method results. A generated caller-side op calls `system::IRpcLifecycle::ReadMethodException` on the raw `InvokeMethod` result before unboxing; if it is `system::RpcException` or a JSON value representing `system::RpcException`, the helper raises the contained message and skips any `EndInvokeMethod` cleanup.

When the return value is an `@rpc:Byval` collection, generated object ops return `system::RpcByvalReturnValue` instead of the raw boxed collection. Its `value` field contains the boxed transfer value, and its `slot` field identifies a recursive copy of the returned collection cached in the callee-side ops object. Generated caller-side ops cast the result directly to `system::RpcByvalReturnValue^`, unbox `value` into the real return variable, call `EndInvokeMethod(slot)` on the same object ops, and then return that variable. Non-byval return values keep the direct return path.

Object lifetime is completed through hold and unhold messages. Wrapper creation sends `ObjectHold(..., true)` to the owner lifecycle. Wrapper destruction sends `ObjectHold(..., false)`. The owner lifecycle updates local-object interest counts through `LocalObjectHold` and `LocalObjectUnhold`.

A generated local event listener first checks the lifecycle controller's event suppression flag. If the event is not suppressed, it boxes the event arguments and broadcasts them through `IRpcDispatcher::BroadcastFromClient_ObjectEventOps(clientId)`. The returned `system::RpcException[int]` map is an internal transport value keyed by lifecycle client id. If the map is empty, the listener finishes normally; if it is non-empty, the listener raises the contained exception messages as a normal Workflow exception without exposing client ids to script catch variables.

On the receive side, generated `IRpcObjectEventOps::InvokeEvent` returns `system::RpcException[int]`. It sets the suppression flag, raises the local event, catches any exception into a one-entry map keyed by the receiver lifecycle's client id, and clears the flag in a `finally` block. Unknown event ids are local dispatch errors and escape directly instead of being encoded into the returned map. If an RPC definition has no events, generated JSON event ops collapse `InvokeEvent` to a direct unknown-event-id raise.

Predefined observable-list wrappers use the same exception map for `IRpcListEventOps::OnItemChanged`. The receive-side bridge replays the remote list notification under the item-changed suppression flag, catches local handler exceptions into a one-entry map keyed by the receiver lifecycle's client id, and the send-side native list handler calls `IRpcLifecycle::ReadEventException` on the returned map.

Service registration finishes by validating that the type id exists and represents an `@rpc:Ctor` interface. The generated ops convert the service pointer to a reference with `PtrToRef`, then register that reference in the dispatcher. Service lookup finishes through the lifecycle: it asks the dispatcher for the service reference and converts that reference to a local pointer or wrapper.

## How Generated JSON Serialization Functions Finish Their Work

JSON caller-side ops box arguments first, serialize each boxed value to a `JsonNode`, and put those nodes into the argument array. They send the method call through the same dispatcher path, receive a `JsonNode` result, deserialize it, unbox it to the expected return type, and return it.

JSON callee-side object ops deserialize each incoming `JsonNode` argument before unboxing it to the declared Workflow type. After calling the target method, they box the return value, serialize it to `JsonNode`, and return that node to the caller.

When a JSON callee-side object op catches an exception raised by the target object method, it serializes `system::RpcException` using the unknown-struct JSON schema and returns that `JsonNode`. Unknown method ids are local dispatch errors and are not serialized as `system::RpcException`. JSON caller-side ops call `system::IRpcLifecycle::ReadMethodException` before the normal known-type deserialization path; if the raw result represents `system::RpcException`, they raise the contained message. For byval collection returns, this check happens before casting the result to `system::RpcByvalReturnValue^`, so `EndInvokeMethod` is not called on exceptional results.

For an `@rpc:Byval` collection return, JSON object ops still serialize the transfer value as a `JsonNode`, but that node is stored in `system::RpcByvalReturnValue.value` and the recursive copied collection is cached by `slot` on the callee. JSON caller-side ops cast the `InvokeMethod` result directly to `system::RpcByvalReturnValue^`, deserialize `value`, unbox the real return value, call `EndInvokeMethod(slot)`, and then return the unboxed result. Non-byval JSON returns still receive a direct `JsonNode`.

JSON event sending follows the same argument rule. The generated listener boxes all event values into `arguments`, checks whether `lc.Serializer` exists, and, when it does, serializes every argument in place before broadcasting. JSON event receiving deserializes the argument nodes before raising the local event under the suppression flag. Event exception maps still use `system::RpcException[int]`: JSON receive-side ops serialize the map using existing unknown-value support, and JSON send-side ops deserialize it before deciding whether to raise a normal Workflow exception.

Known-type JSON serialization finishes by constructing the schema described in `TODO_RPC_Json.md`: primitives become JSON primitives or tagged primitive arrays as required, enums become numbers, structs become JSON objects, and collections become arrays or tagged collection objects. Unknown-type serialization includes enough type information in the JSON value for `rpcjson_Deserialize(node)` to reconstruct a reflected value.

Deserialization validates the JSON shape it expects. Missing struct fields, unknown type tags, or unsupported reflected values raise exceptions. Interface references and byref collection references serialize through `RpcObjectReference`; deserialization converts the reference back through the lifecycle path when the value is unboxed.
