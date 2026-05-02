# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Task 1

The goal of this task is to continue the refactoring `RpcDualLifecycleMock` and its base class `RpcLifecycleBase`, check out `TODO_RPC_Dual.md` for context of the last work.
- `SetDispatcher` should be moved to `RpcDualLifecycleMock` as setting a dispatcher is a "dual" scenario.
- Remove `RpcByvalLifecycleMock`, replace it with `RpcDualLifecycleMock`.
  - This `RpcByvalLifecycleMock` implementation actually break the contract defined by the interface.
  - It forced all collection interfaces to be wrapped regardless is the current lifecycle ownes that object.
    - And because there is only one lifecycle in that test, it actually owns.
  - By replacing it with `RpcDualLifecycleMock`, the test case in `LibraryTest` actually needs to change.
  - You will have to call `PtrToRef` from lifecycle1, and call `RefToPtr` from lifecycle2.
    - Maybe boxing or unboxing functions are needed instead of these pair of functions, you have to figure it out.
  - In this way, the wrapper belongs to lifecycle2 and the original container belongs to lifecycle1, that's how wrappers are created expectedly.
- `LocalRpcMock` should be moved outside of `RunRpcTestCase` because it does not depend on the `TInstance` template argument.
  - Rename it to `RpcCppLifecycleMock`.

No reflection registration is needed, you can skip CompilerTest_GenerateMetadata and CompilerTest_LoadAndCompiler and Build.ps1, to shortern your test. This change should not affect the compiler and any code generation.
