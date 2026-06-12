# Workflow Interface Based RPC (JSON Request Routing)

This document describes the runtime meaning of the generic JSON dispatcher envelopes declared in `Test/TypeScript/Rpc.d.ts`. It is independent of any particular sample or transport. Generated interface and serialization rules are covered by `TODO_RPC_Definition.md`, `TODO_RPC_GeneratedWrappers.md`, and `TODO_RPC_Json.md`.

Every JSON RPC envelope has:

- `rpcMethod`: a string beginning with `Request:` or `Response:`.
- `rpcRequestId`: the caller-allocated id for matching a response to a request.
- `sourceClientId`: the lifecycle client id that created the envelope.

Direct requests and ordinary responses also have `targetClientId`. Broadcast requests omit `targetClientId` because the transport coordinator expands the request to multiple clients.

## Request Kinds

There are three request kinds at the JSON message dispatcher boundary.

1. `Direct`

   The request is sent to exactly one `targetClientId`. The receiver translates the request to its local ops object and sends exactly one response with the same `rpcRequestId`. Direct requests are synchronous from the caller's point of view.

2. `Broadcast`

   The request is sent to the transport coordinator, which sends it to all relevant clients except the originating client. Each receiver translates the request locally and sends a response to the coordinator. The coordinator waits for all expected responses, merges the returned data, and sends one response to the original caller with the original `rpcRequestId`.

3. `BroadcastAndDrop`

   The request is sent to the transport coordinator, which sends it to all relevant clients except the originating client and stores enough state to replay the declaration to future clients. Receivers translate the request locally but do not send a response. The original caller receives no response; the local `JsonRequest` result is `null`.

## Requests and Responses

### `Request:IObjectOps_InvokeMethod`

Kind: `Direct`.

The caller sends the request to `targetClientId`, normally the same client id as `ref.clientId`. The receiver calls its local object ops `InvokeMethod(ref, methodId, arguments)` and returns `Response:IObjectOps_InvokeMethod` to the caller.

The response carries:

- the same `rpcRequestId`,
- `sourceClientId` equal to the receiver,
- `targetClientId` equal to the original caller,
- `response` containing the serialized method result, serialized `system_RpcException`, or `system_RpcByvalReturnValue<T>` for byval collection returns.

Unknown method ids and malformed references are local dispatch errors. User-code exceptions are transported as `system_RpcException` according to the generated ops rules.

### `Request:IObjectOps_EndInvokeMethod`

Kind: `Direct`.

The caller sends this to the same client that returned a byval collection slot. The receiver calls local object ops `EndInvokeMethod(slot)` and returns `Response:IObjectOps_EndInvokeMethod` with the same `rpcRequestId`. There is no `response` field.

### `Request:IObjectOps_ObjectHold`

Kind: `Direct`.

The caller sends this to the owner of `ref.clientId` when creating or releasing a wrapper interest. The receiver calls local object ops `ObjectHold(ref, remoteClientId, hold)` and returns `Response:IObjectOps_ObjectHold` with the same `rpcRequestId`. There is no `response` field.

`remoteClientId` is the lifecycle whose interest changes. The receiver should validate that `ref` belongs to the receiving lifecycle before mutating local-object hold state.

### `Request:IObjectEventOps_InvokeEvent`

Kind: `Broadcast`.

The originating lifecycle has already observed or raised the event locally, so the coordinator must not send the broadcast back to that originating client. Each receiving lifecycle calls local object event ops `InvokeEvent(ref, eventId, arguments)`, which replays the event under event-suppression rules and returns the serialized form of `null | [number, system_RpcException][]`.

Each receiver sends `Response:Broadcast_Response` to the coordinator with:

- the redirected broadcast `rpcRequestId` chosen by the coordinator,
- `sourceClientId` equal to that receiver,
- `targetClientId` equal to the coordinator,
- `response` equal to the event exception map or `null`.

The coordinator sends one `Response:Broadcast_Response` to the original caller with:

- the original caller's `rpcRequestId`,
- `sourceClientId` equal to the coordinator client id,
- `targetClientId` equal to the original caller,
- `response` equal to `null` if every receiver returned `null`, otherwise a merged event exception map.

The coordinator must attempt every receiver even when earlier receivers report exceptions. When clients disconnect during an active broadcast, the coordinator removes them from the expected response set and completes the broadcast if all remaining receivers have responded.

### `Request:IRpcDispatcher_DeclareRemoteService`

Kind: `BroadcastAndDrop`.

This request declares that `sourceClientId` owns a service reference. It carries `ref: system_RpcObjectReference`, and `ref.clientId` must equal `sourceClientId`. The receiver calls `IRpcLifecycle::DeclareRemoteService(ref)` and stores the full reference by `ref.typeId`.

No `Response:*` envelope is created for this request. A caller-side dispatcher returns `null` immediately after sending or caching the message. A receiver-side translator also returns `null`.

The coordinator caches every service declaration and replays cached declarations to newly connected clients after the new client has learned the coordinator client id. The replayed request keeps the original `sourceClientId` and the original `ref`; it is not rewritten to the coordinator client id.

## Lifecycle Handling

`IRpcLifecycle::RegisterLocalService(typeId, service)` is a pre-initialization operation. It creates a full `RpcObjectReference` for the service object, stores the local service by type id, adds the owner hold, and calls `IRpcDispatcher::DeclareLocalService(ref)`.

For JSON transport, `IRpcDispatcher::DeclareLocalService(ref)` creates `Request:IRpcDispatcher_DeclareRemoteService` and sends it as `BroadcastAndDrop`. A lifecycle receiving this request stores `ref` through `IRpcLifecycle::DeclareRemoteService(ref)`.

`IRpcLifecycle::GetTypeIdFromName(typeName)` resolves names through the lifecycle id map and returns `RpcTypeId_NotFound` when the name is unknown. `IRpcLifecycle::RequestService(typeName)` uses this function, returns a local registered service first, otherwise looks up the stored remote `RpcObjectReference` by type id and calls `RefToPtr(ref)`.

Before a lifecycle is initialized, a client may receive service declaration requests. It should cache `Request:IRpcDispatcher_DeclareRemoteService` and reject all other RPC messages. During initialization it processes cached declarations before requesting required remote services. After initialization, later service declarations are processed immediately.

If a client is waiting for required service type names, it should compare each incoming declaration's `ref.typeId` with `GetTypeIdFromName(typeName)`. When all required services have been declared, the wait completes.

## Expected Sequences

### Transport Coordinator Startup

1. Start the transport layer enough for local endpoints to connect.
2. Connect the coordinator endpoint first and record its client id.
3. Report the coordinator client id to clients as an out-of-band transport login message before replaying RPC declarations.
4. Connect any local service-owning clients.
5. Let local service-owning clients register services and send `Request:IRpcDispatcher_DeclareRemoteService`.
6. Start accepting remote clients only after the coordinator client id is known and local service declarations have been sent.

The coordinator endpoint is not a service owner. It routes broadcasts, caches service declarations, and consolidates broadcast responses.

### New Client Startup

1. Connect to the transport and learn the client's own client id.
2. Register the JSON channel reader before waiting for the coordinator login message.
3. Learn the coordinator client id.
4. Cache any `Request:IRpcDispatcher_DeclareRemoteService` messages that arrive before lifecycle initialization.
5. Initialize the lifecycle, process cached declarations, send local declarations if this client owns services, and wait for required remote service names if needed.
6. Begin ordinary direct method calls and event broadcasts.

### Direct Method Call

1. Caller allocates `rpcRequestId` and sends a direct request to `targetClientId`.
2. Receiver translates and executes the local operation.
3. Receiver sends the matching direct response.
4. Caller matches by `rpcRequestId`; while waiting, it may process nested incoming requests and buffer unrelated responses.

### Event Broadcast

1. Originating lifecycle raises or observes the event locally.
2. Originating caller sends `Request:IObjectEventOps_InvokeEvent` to the coordinator.
3. Coordinator chooses a redirected request id, sends the request to every expected receiver except the originator, and records the original `(sourceClientId, rpcRequestId)`.
4. Receivers replay the event under suppression and respond to the coordinator.
5. Coordinator merges responses and sends one `Response:Broadcast_Response` to the originator using the original request id.

### Service Declaration

1. Service owner registers a local service before lifecycle initialization.
2. Its dispatcher sends or caches `Request:IRpcDispatcher_DeclareRemoteService`.
3. Coordinator caches the declaration and broadcasts it without waiting for responses.
4. Receivers store `ref.typeId -> ref`.
5. Future clients receive the cached declaration during startup.

## Error Handling Rules

Unknown `rpcMethod` values are transport or implementation errors. Do not silently drop them.

A request kind mismatch is an error: object ops are direct, object events are broadcast, and service declarations are broadcast-and-drop.

Only `Request:IRpcDispatcher_DeclareRemoteService` may be accepted before lifecycle initialization. Other RPC messages before initialization indicate a startup-order violation.

Broadcast-and-drop requests must not produce responses. A client waiting for a response to this request will deadlock a correct implementation.
