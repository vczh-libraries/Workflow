# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Task 1

Refactor `WfLibraryRpc.(h|cpp)` and `RpcDualLifecycleMock.(h|cpp)`.
I have manually edited `WfLibraryRpc.h` to modify interface definitions, you are going to fix everything including reflection and code generation, and apply refactor to `RpcDualLifecycleMock` series classes.

The target message dispatching and event suppression design is defined in [TODO_RPC_Definition.md](TODO_RPC_Definition.md#message-dispatching-and-event-suppression). This task list should follow that definition.

### Introduction to New Interfaces

- `IRpcOperations`: Instead of `IRpcController` inheriting from 4 ops interfaces, it now inherits from `IRpcOperations`, and all operations are not base types anymore, they are accessible via methods.
- `IRpcDispatcher`: Currently `RpcControllerMock` implements ops interfaces with implementation that only doing redirection, they could not be deleted.
  - `IRpcObjectOps::RegisterService` should stay as the generated validation and registration hook. It validates the type id, converts the service to a ref through the lifecycle, and calls `IRpcDispatcher::RegisterService` to store the ref.
  - Remove `IRpcObjectOps::RequestService`. Service lookup should have only one authority: `IRpcDispatcher::RequestService`.
- `IRpcDispatcher` exposes event broadcasting and point-to-point calls with dedicated methods:
  - `BroadcastFromClient_ListEventOps(selfClientId)` returns list event ops for all clients except `selfClientId`.
  - `BroadcastFromClient_ObjectEventOps(selfClientId)` returns object event ops for all clients except `selfClientId`.
  - `SendToClient_ListOps(targetClientId)` returns list ops for the specified client.
  - `SendToClient_ObjectOps(targetClientId)` returns object ops for the specified client.
- In `RpcDualLifecycleMock.h`, a `RpcDualDispatcherMock` is implemented like this:
  - It maintains a map for typeid -> ref.
  - `RpcDualLifecycleMock::RegisterLocalObjectOps` aware code can now be completely deleted, it just go to `IRpcDispatcher::RequestService` to query for the ref directly. And later convert the ref to wrapper or local object using the current way.
  - The constructor takes two `RpcDualLifecycleMock` because in the unit test there will be only two `IRpcLifeCycle` instances.
  - `BroadcastFromClient_ListEventOps` and `BroadcastFromClient_ObjectEventOps` are supposed to call all other clients except the specified client id, but in the `RpcDualDispatcherMock`, since there is only two `RpcDualLifecycleMock` registered in it, it could just find the lifecycle **whose client id is not the specified one** and return the corresponding event ops from its controller.
  - `SendToClient_ListOps` and `SendToClient_ObjectOps` doing the opposite thing to broadcast methods, they return the corresponding ops from the controller **whose client id is the specified client id**.
  - DO not make `RpcDualLifecycleMock` too complex, just stores two `RpcDualLifecycleMock*` in it, that's enough.
- `IRpcDispatcher::IsRegisteredService` checks if a ref is a registered service, and when it returns true, `IRpcController::UnregisterLocalObject` should just do nothing, because after a service is registered, it can never be unregistered, so such object just can't be unregistered. No exception need to raise.

### IRpcController::Register breaking changes

Here is a snapshot in `TestCasesRpc.h`
```C++
lc1->RegisterLocalObjectOps(oo1);
lc2->RegisterLocalObjectOps(oo2);

lc2->GetController()->Register(oo1, oeo1, lo1, leo1);
lc1->GetController()->Register(oo2, oeo2, lo2, leo2);
```

Change it to
```C++
lc1->GetController()->Register(oo1, oeo1, lo1, leo1);
lc2->GetController()->Register(oo2, oeo2, lo2, leo2);
RpcDualDispatcherMock dispatcher(lc1.Obj(), lc2.Obj());
```
This is an example, feel free to do any change

There will be no more `RegisterLocalObjectOps`, because when `IRpcLifeCycle::RequestService` is called, it should ask `IRpcDispatcher` for the ref, and the ref.clientId should tell if it is a local object or a remote object. `RegisterLocalObjectOps` is currently used like a test, so it can be removed.

Currently all ops interfaces are registered to the opposite lifecycle, but now the logic is changed. They should register to the local lifecycle, so that its `IRpcOperations` base classes could return the current object.

`IRpcObjectOps::RequestService` should be removed from the interface, reflection registration, generated `rpc_IRpcObjectOps`, and mock/controller redirection code. `IRpcLifeCycle::RequestService` should resolve `fullName` to `typeId`, call `IRpcDispatcher::RequestService(typeId)`, and then call `RefToPtr(ref)`. `RefToPtr` should use `ref.clientId` to decide whether the ref is local or remote.

### New Way to Implement Wrappers and Event Broadcasting

In the current implementation, ops interfaces are registered to the opposite controller because this is how two lifecycle talks to each other. But this is no more.

There are scenarios that different lifecycle communicates, it would require many rewrite of wrapper Workflow script generating code, and also predefined wrappers for container interfaces.

#### 1. Calling methods of remote objects

When an object is a wrapper, calling methods actually send the call to the remote lifecycle who owns this object.
Now we have a new way. Because the message is sending to only one client, so we call `IRpcDispatcher::SendToClient_ListOps(ref.clientId)->xxx` for list operations, and `IRpcDispatcher::SendToClient_ObjectOps(ref.clientId)->xxx` for object operations.

#### 2. Invoking events of objects

An object could have multiple wrappers in different lifecycles. So when an event is raised, no matter who is raising it, all clients need to know.
When raising an event of a local object, the message will be sent to all other clients.
When raising an event of a wrapper, the message will be sent to all other clients, including the client that owns this object.
So we use `IRpcDispatcher::BroadcastFromClient_ObjectEventOps(the caller lifecycle's client id)->InvokeEvent(...)` for object events, and `IRpcDispatcher::BroadcastFromClient_ListEventOps(the caller lifecycle's client id)->OnItemChanged(...)` for list events.

Tricky thing is that, in the test case we know we only have two lifecycles, that is the name `Dual` mean. So in `RpcDualDispatcherMock` we only need to do the opposite thing of `SendToClient_*`, which is returnning the event ops from the lifecycle who is not the `selfClientId`.

#### 3. Others

Extra scenarios is inside `IRpcDispatcher` which is the central dispatcher of all (and in the dual implementation we only have to) clients. So it would exactly know which one to talk to.

### Extra Rules

- There is no need to verify if an ops interface is registered, we should just use it like we know it will never be null.
- If a function is only called in the destructor in the same class, we should just move the code to the destructor, and remove the function.
- In a destructor, there is no need to do things like, clear its list fields, reset its pointer fields, anything like that. After the destructor the object is not accessible anyway, those are just unnecessary dumb code.
- If an `IRpcXXX` does not implement `IRpcYYY`, any `RpcDualXXXMock` should not implement `IRpcYYY`.
- DO NOT create unnecessary redirection. If you know which object to call, just call it. Until it is required by the semantic of the interface.
- `IRpcDispatcher` is supposed to be the only way we send messages to other lifecycles.
  - Review the current implementation precisely, and keep it in this way.
  - Any lifecycle/controller implementation should not store any object that does not belong to this client. It should only use `IRpcDispatcher` to send messages.
- Besides of `RpcDualDispatcherMock`, `RpcDualControllerMock`, `RpcDualLifecycleMock`, I don't think any other **dual** specific mocks should be created.

I would like to see code in simple structure, and the above information is telling you why we can keep things simple, and I am offering you the idea of how to implement them.

There are 3 constants at the beginning of `WfLibraryRpc.h`, search `WfLibraryRpc.cpp` and all mocks to see if there is any related magic number, and use these constants instead. Not magic number is allowed.

## Additional Design Notes

### Event Suppression for Broadcasted Events

Although we do not always know who will call an event, when an event is called because `InvokeEvent` or `OnItemChanged` is received, we exactly know the request is from a remote client. Therefore suppression should happen at this exact moment:
- Mark the incoming event as suppressed.
- Raise the event locally.
- Unmark the event after the local event invocation finishes.

The current generated wrapper shape in `Test/Generated/RpcMetadata32/Wrapper_Event.txt` has two important locations:
- `rpc_IRpcObjectEventOps(...).InvokeEvent` receives `(ref, eventId, arguments)`, resolves `target = _lc.RefToPtr(ref)`, unboxes arguments, and raises the local event on `target`.
- `rpclistener_*` is attached to both real local objects and generated wrappers. Its handler already captures `lc`, `ref`, and the generated event id constant, then boxes arguments and forwards the event.

So the forwarding handler should know suppression by asking the local controller, not by asking `IRpcDispatcher`. The lifecycle is already passed into `rpclistener_*`, and generated Workflow code can reach the controller through `lc.Controller`. A concrete `IRpcController` API can be:
- `SetEventSuppressedFlag(ref, eventId, bool)`
- `GetEventSuppressedFlag(ref, eventId)`
- `SetItemChangedSuppressedFlag(ref, bool)`
- `GetItemChangedSuppressedFlag(ref)`

These methods have already been added to `IRpcController` in `WfLibraryRpc.h` as part of the target interface shape. The later implementation task still needs to add reflection registration, concrete controller implementations, and generated-code calls.

Although the setter receives a `bool`, the controller should store suppression as a counter internally: setting `true` increments, setting `false` decrements, and `Get*` returns true when the count is greater than zero. This keeps nested or reentrant remote event replay from clearing the flag too early.

The object-event receive path should call `lc.Controller.SetEventSuppressedFlag(ref, eventId, true)` before raising the local event, and call `lc.Controller.SetEventSuppressedFlag(ref, eventId, false)` in a `finally` block after the local event invocation finishes. This can be emitted directly in `rpc_IRpcObjectEventOps(...).InvokeEvent`, or implemented by a controller/lifecycle bridge that wraps the generated object event ops, as long as the generated `rpclistener_*` handler observes the same controller suppression state.

The generated `rpclistener_*` handler should check suppression before boxing arguments. If `lc.Controller.GetEventSuppressedFlag(ref, eventId)` returns true, it returns immediately. Otherwise it boxes arguments and broadcasts through `IRpcDispatcher::BroadcastFromClient_ObjectEventOps(...)`.

List events should use the same pattern without an event id. The receive-side `OnItemChanged` calls `lc.Controller.SetItemChangedSuppressedFlag(ref, true)`, raises the local list notification, and calls `lc.Controller.SetItemChangedSuppressedFlag(ref, false)` in a `finally` block. The native list event handler checks `lc.Controller.GetItemChangedSuppressedFlag(ref)` before calling `IRpcDispatcher::BroadcastFromClient_ListEventOps(...)`.

### Responses to Feasibility Notes

- For generated wrappers needing dispatcher or client information: generated wrappers currently bring the lifecycle of the current object, so the lifecycle is enough to provide whatever dispatcher or client information the generated code needs.
- For `RpcControllerMock` redirection and `IRpcOperations`: this can be done by changing every code path that calls ops from the lifecycle/controller interface to use the new `GetListOps`, `GetObjectOps`, `GetListEventOps`, and `GetObjectEventOps` shape.
- For registered service lifetime and unregister behavior: a service shared pointer will be owned by the controller, so no service destructor should be called until the controller is going to destroy. Therefore `UnregisterLocalObject` doing nothing for registered services is still fine.

# UPDATES
- Refactored the RPC interfaces around `IRpcOperations` and `IRpcDispatcher`, removed `IRpcObjectOps::RequestService`, and reflected the new lifecycle/controller/dispatcher surface.
- Reworked `RpcControllerMock` to expose local operation objects directly and to store event suppression state with counters.
- Reworked dual RPC lifecycle tests around a simple `RpcDualDispatcherMock` that owns service refs and routes point-to-point calls or broadcasts by client id.
- Updated byref wrappers, generated RPC Workflow code, and native list event bridges to send cross-client messages only through `IRpcDispatcher`.
- Added receive-side object/list event suppression and generated listener suppression checks before rebroadcasting events.
- Updated service lifetime cleanup in runtime tests so registered services remain pinned during normal unregister, then are explicitly released while Workflow globals are still alive.
- Replaced related invalid client/object/type magic values with the RPC constants where applicable and regenerated RPC metadata/source/baselines.

# TEST
- `Test/UnitTest/UnitTest.sln` Debug Win32 build: passed.
- `Test/UnitTest/UnitTest.sln` Debug x64 build: passed.
- `CompilerTest_GenerateMetadata` Debug Win32: passed.
- `CompilerTest_GenerateMetadata` Debug x64: passed.
- `CompilerTest_LoadAndCompile` Debug x64: passed.
- Post-generation `Test/UnitTest/UnitTest.sln` Debug Win32 build: passed.
- Post-generation `Test/UnitTest/UnitTest.sln` Debug x64 build: passed.
- `LibraryTest` Debug Win32: passed, 14/14.
- `LibraryTest` Debug x64: passed, 14/14.
- `RuntimeTest` Debug Win32: passed, 257/257.
- `RuntimeTest` Debug x64: passed, 257/257.
- `CppTest` Debug Win32: passed, 223/223.
- `CppTest` Debug x64: passed, 223/223.
- `CppTest_Metaonly` Debug Win32: passed, 223/223.
- `CppTest_Metaonly` Debug x64: passed, 223/223.
- `CppTest_Reflection` Debug Win32: passed, 223/223.
- `CppTest_Reflection` Debug x64: passed, 223/223.

# PROPOSALS
- None.
