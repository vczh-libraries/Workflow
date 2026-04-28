# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow the instruction to commit and push for each task, before doing the next task.

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

## General Instruction

If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
- Pass all unit test, fix any test failure including pre-existings.
- After finishing everything, git commit and git push to the current branch.
- If in any task you are adding new test sample, or modifying any C++ or Workflow generation code, causing a huge amount of files generated from test samples to change:
  - Two commits are required. First commit only has all modified files and files you created directly, second commit has all new files that not created by you (aka auto generated)
  - Otherwise, One commit is good.
  - Typical files that are generated: `Test\Generated`, `Test\SourceCppGen`, `Test\SourceCppGenRpc`, `Test\UnitTest\Generated_*`.
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
