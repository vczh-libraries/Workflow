# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Task 1

Refactor `WfLibraryRpc.(h|cpp)` and `RpcDualLifecycleMock.(h|cpp)`.

It seems that finalization needs to be executed explicitly, currently we are calling `DisconnectTrackedWrappers`. We should add `Finalize` to `IRpcDispatch`, `IRpcLifecycle`, `IRpcController`. First put all finalization code there, and then `IRpcDispatch::Finalize` will call `IRpcLifecycle::Finalize` which will therefore call `IRpcController`. So in any test cases we should just call `IRpcDispatch::Finalize` at the end of the test case, without bothering how to take care of others. And then delete unnecessary functions. Although a registered service cannot be "unregistered", but during finalization, all registered service should just gone. `Finalize` is special, after calling this function, all rpc interfaces should not be touched. But we do not need to check if finalize has been called or any other equivalent condition.

`IRpcWrapperBase::DisconnectFromLifecycle` should be called when a proxy is no longer used, it should not be delayed to the finalization.

The return value if `RpcDualLifecycleMock::AttachLocalObjectEvents` never gets used, so it should be `void`, and I think we should just make it `= 0;` in `RpcDualLifecycleMock` anyway, since all sub classes overrides it.

In `RunRpcTestCase`:
- `invokeLocalEvents` argument seems no longer used, we should remove it.
- In `attachLocalEvents` call back the last arguments seem never get called, because only `TestCasesRpc.cpp` uses it but the argument is ignored.
  - `RpcLocalObjectProperties::nativeEventDetachments` is passed to this argument but this list is never add things, so this one should also be removed.

In `RpcDualControllerMock`:
- `AcquireRemoteObject` and `ReleaseRemoteObject` should just be removed.
  - `AcquireRemoteObject` and `ReleaseRemoteObject` should not handle the case `ref.clientId == clientId`, because this function is supposed to send to the remote object when wrapper is created or released. I think it needs to handle this case because the generated `IRpcObjectOps::ObjectHold` in wrapper Workflow script calls this function, but this is wrong.
  - After removing the duty of handling local object, they just redirect the call to `ObjectHold`, and they should not handle local object cases, so they seem to be not useful anymore. So the wrapper should just call `ObjectHold` by itself because now it can use `IRpcDispatch` to do dispatching.
- Only wrapper constructor and destructor calls `IRpcDispatcher::SendToClient_ObjectOps::ObjectHold`.
  - Currently boxing and unboxing calls `AcquireRemoteObject` and `ReleaseRemoteObject`, which is not necessary, the wrapper RTTI should handle it well.
  - If `DisconnectFromLifecycle` is called, `ObjectHold` in the destructor could skip.
  - After this, I think `RpcDualControllerMock` no more depends on `RpcDualLifecycleMock`, this argument could just be removed.
- `RegisterLocalObject` and `UnregisterLocalObject` should be removed.
  - `ObjectHold` should call `IRpcLifecycle::ObjectHold`.
  - `IRpcLifecycle::ObjectHold` does not exist, it should take the responsibility of the original `RegisterLocalObject` and `UnregisterLocalObject`.
  - Inside `PtrToRef`, since there is no `RegisterLocalObject`, it should allocate the new object id by itself.
  - `IRpcLifecycle::ObjectHold` handle reference counting, when it comes to zero, all tracking to the specified local object should be removed. Therefore the next time `PtrToRef` is called on this object it gets a new id.
- By doing this, `RpcDualControllerMock` actually no need to exist anymore, it should just use `RpcControllerMock`, and `clientId`, `nextObjectId`, `localObjectProperties` will be moved back into `RpcDualLifecycleMock`.

- `IRpcLifeCycle` should be renamed to `IRpcLifecycle`, change the letter C from upper case to lower case, to align with everywhere else. Therefore if any variable has the name lifecycle, and it points to `IRpcLifeCycle` or any actual implementation, it should also keep the lowercase c.
