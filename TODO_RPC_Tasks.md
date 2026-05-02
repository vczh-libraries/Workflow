# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Task 1

The goal of this task is to continue the refactoring `RpcDualLifecycleMock` and its base class `RpcLifecycleBase`, check out `TODO_RPC_Dual.md` for context of the last work.
- `SetDispatcher` should be moved to `RpcDualLifecycleMock` as setting a dispatcher is a "dual" scenario.

No reflection registration is needed, you can skip CompilerTest_GenerateMetadata and CompilerTest_LoadAndCompiler and Build.ps1, to shortern your test. This change should not affect the compiler and any code generation.
