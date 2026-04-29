# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Task 1

Refactor `RpcDualLifecycleMock.(h|cpp).

- `controller.localObjectProperties` is readonly in `RpcDualLifecycleMock`, Make a `const ...& RpcDualControllerMock::GetLocalObjectProperties();`.
- I don't think `RpcDualControllerModk::pendingProxyRef` is even useful, we should delete it. It is changed in `RpcDualLifecycleMock::CreateCallerProxy`, but `universalWrapperFactory` does not call `RequestService`, which means `RequestService` is supposed to always see an empty value. Delete this variable and the meaningless code using it.
- Now the only two private `RpcDualControllerMock` members usage in `RpcDualLifecycleMock` is eliminated, we should remove the `friend` declaration.
- In `RpcDualLifecycleMock`:
  - `wrapperProperties` should be a dictionary whose key is the `ref`. Since this is the only way how it search data.
  - `suppressedEvents` should be a SortedList, and by making `RpcDualEventDispatch` comparible, `IsSameEvent` could be deleted.
    - The idea of `suppressedEvents` actually looks wired. Because when we received an event, but the object is not used in this lifecycle (by finding no record of this local object or wrapper), the event could just be discarded. So maybe we could just delete `suppressedEvents` and code using it directly.
  - `forwardingEvents` should be a SortedList, pushing and poping is not necessary, just add and remove.
    - `forwardingEvents` should not be static. And if the intention is to enable communication between lifecycles, this would be totally wrong to do it in this way. Lifecycles do not talk to each other through anything other than 4 ops interfaces.
  - `try-catch` in `RpcDualControllerMock::InvokeEvent` is unnecessary, there should be no exception, catching it just hide the problem. If any exception is raised, it is a bug (so far).
  - `services` should not be necessary, because the actual service map is stored in generated `IRpcObjectOps` in each wrapper Workflow script. `RpcDualControllerMock` is not supposed to maintain anything.
    - `DisconnectServices` looks like need to be moved to the destructor in generated `IRpcObjectOps` implementations.
  - `UnregisterAllLocalObjects` could be moved to `RpcDualControllerMock`, better inside its destructor.
