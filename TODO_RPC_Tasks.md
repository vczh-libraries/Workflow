# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Task 1

Refactor `WfLibraryRpc.(h|cpp)` and `RpcDualLifecycleMock.(h|cpp)`.

It seems that finalization needs to be executed explicitly, currently we are calling `DisconnectTrackedWrappers`. We should add `Finalize` to `IRpcDispatcher`, `IRpcLifecycle`, `IRpcController`. So in any test cases we should just call `IRpcDispatcher::Finalize` at the end of the test case, without bothering how to take care of others. And then delete unnecessary functions. The exact finalization order is left to the Dual and Byval test implementations, to do whatever they need. Explicit finalization exists because in `RuntimeTest` project, finalization runs Workflow code, so it should be done before unloading Workflow context. Although a registered service cannot be "unregistered", but during finalization, all registered services should just gone. `Finalize` is special, after calling this function, all rpc interfaces should not be touched. But we do not need to check if finalize has been called or any other equivalent condition.

`IRpcWrapperBase::DisconnectFromLifecycle` should be called when a proxy is no longer used, it should not be delayed to the finalization. It only resets the internal lifecycle pointer to null, which is already implemented in predefined container wrappers and generated wrappers, so this is fine. If `DisconnectFromLifecycle` is called, `ObjectHold` in the destructor could skip.

The return value if `RpcDualLifecycleMock::AttachLocalObjectEvents` never gets used, so it should be `void`, and I think we should just make it `= 0;` in `RpcDualLifecycleMock` anyway, since all sub classes overrides it.

In `RunRpcTestCase`:
- `invokeLocalEvents` argument seems no longer used, we should remove it.
- In `attachLocalEvents` call back the last arguments seem never get called, because only `TestCasesRpc.cpp` uses it but the argument is ignored.
  - `RpcLocalObjectProperties::nativeEventDetachments` is passed to this argument but this list is never add things, so this one should also be removed.

In `RpcDualControllerMock`:
- `AcquireRemoteObject` and `ReleaseRemoteObject` should just be removed.
  - `AcquireRemoteObject` and `ReleaseRemoteObject` should not handle the case `ref.clientId == clientId`, because this function is supposed to send to the remote object when wrapper is created or released. I think it needs to handle this case because the generated `IRpcObjectOps::ObjectHold` in wrapper Workflow script calls this function, but this is wrong.
  - After removing the duty of handling local object, they just redirect the call to `ObjectHold`, and they should not handle local object cases, so they seem to be not useful anymore. So the wrapper should just call `ObjectHold` by itself because now it can use `IRpcDispatcher` to do dispatching.
- Only wrapper constructor and destructor calls `IRpcDispatcher::SendToClient_ObjectOps::ObjectHold`.
  - Currently boxing and unboxing calls `AcquireRemoteObject` and `ReleaseRemoteObject`, which is not necessary, the wrapper RTTI should handle it well.
  - After this, I think `RpcDualControllerMock` no more depends on `RpcDualLifecycleMock`, this argument could just be removed.
- `RegisterLocalObject` and `UnregisterLocalObject` should be removed.
  - `IRpcObjectOps::ObjectHold` should call `IRpcLifecycle::LocalObjectHold` when `hold` is true, and `IRpcLifecycle::LocalObjectUnhold` when `hold` is false.
  - `IRpcLifecycle::LocalObjectHold` and `IRpcLifecycle::LocalObjectUnhold` do not exist, they should be added.
  - `IRpcLifecycle::LocalObjectHold` and `IRpcLifecycle::LocalObjectUnhold` should take two arguments: `ref` and `remoteClientId`, just like `IRpcObjectOps::ObjectHold`, so we know which client is asking for what.
  - `IRpcLifecycle::LocalObjectHold` and `IRpcLifecycle::LocalObjectUnhold` should only track local objects.
  - They should take the responsibility of the original `RegisterLocalObject` and `UnregisterLocalObject`.
  - Inside `PtrToRef`, since there is no `RegisterLocalObject`, it should allocate the new object id by itself.
  - `IRpcLifecycle::LocalObjectHold` and `IRpcLifecycle::LocalObjectUnhold` handle reference counting, when it comes to zero, all tracking to the specified local object should be removed. Therefore the next time `PtrToRef` is called on this object it gets a new id.
  - An object registered by `RegisterService` will by default have 1 refcount, therefore even if all remote clients untrack it, it will not be actually unregistered.
- By doing this, `RpcDualControllerMock` actually no need to exist anymore, it should just use `RpcControllerMock`, and `clientId`, `nextObjectId`, `localObjectProperties` will be moved back into `RpcDualLifecycleMock`.

- You will need to fix `RpcByvalControllerMock` accordingly to the above change.

- `IRpcLifeCycle` should be renamed to `IRpcLifecycle`, change the letter C from upper case to lower case, to align with everywhere else. Therefore if any variable has the name lifecycle, and it points to `IRpcLifeCycle` or any actual implementation, it should also keep the lowercase c.

## Hints

- The listed scope at the beginning is only the main target. This refactor will probably also affect reflection registration, generated RPC analyzer code, generated test harness code, byval tests, generated files, and metadata baselines.
- Change everywhere needed to eventually remove `(Acquire|Release)RemoteObject`. As now `IRpcLifecycle` is going to track the ref counter by the new added `IRpcLifecycle::LocalObjectHold` and `IRpcLifecycle::LocalObjectUnhold` anyway.
- Wrapper constructors and destructors will call `IRpcDispatcher::SendToClient_ObjectOps::ObjectHold`, so reference tracking should still happen through the dispatcher and object ops boundary.
