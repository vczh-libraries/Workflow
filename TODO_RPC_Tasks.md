# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `Rules\verify-and-commit.md` to finish the current task, before doing the next task.

## Task 1

- Remove the reflection for `RpcLifecycleMock`. The reason that adding the reflection will gone once the rest of the task is implemented currently.
- Rename `RpcLifecycleMock` to `RpcControllerMock`, including the header and cpp file name.
- Instead of `RpcByvalLifecycleMock` and `RpcDualLifecycleMock` inheriting from `RpcLifecycleMock`:
  - They create sub classes of `RpcControllerMock` internally and return it to the `GetController` function. `IRpcLifecycle` does not inherit `IRpcController` so this step should be easy.
  - Both class implements 4 remaining function in `RpcControllerMock`, now you need to create:
    - `RpcByvalControllerMock` and `RpcDualControllerMock` and move necessary members to them.
    - In these controller mocks friend their own lifecycle mocks.
    - Lifecycle mocks return them to the `GetController` function.
    - This is a chance to split implementations, rely on it wisely. The ideal situation will be lifecycle mocks don't need to access internal members in controller mocks completely, but if you have no choice (having to duplicate data in both controller and lifecycle is not acceptable, by the way), you can share limited members.

## Task 2

Reorder WfError::xxx static functions in the cpp file, to move all H on top of all I on to of all Cpp. In each group order by their number. Keep the order of declarations in WfError using the same order in their cpp file.
