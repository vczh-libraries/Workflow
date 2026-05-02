# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

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

# UPDATES

- Reviewed `TODO_RPC_Dual.md`, RPC lifecycle code, RPC mock tests, and the existing project/test guidance before making code changes.
- Moved dispatcher assignment out of `RpcLifecycleBase` and into `RpcDualLifecycleMock`; the base now only exposes the dispatcher storage to derived lifecycle implementations.
- Deleted `RpcByvalLifecycleMock` and switched the LibraryTest by-value collection coverage to a real dual-lifecycle setup using `RpcDualLifecycleMock` and `RpcDualDispatcherMock`.
- Updated LibraryTest helpers so lifecycle1 owns/registers the original reflected collection via `PtrToRef`, while lifecycle2 creates the remote wrapper via `RefToPtr`.
- Moved the old `LocalRpcMock` out of `RunRpcTestCase`, renamed it to `RpcCppLifecycleMock`, and kept the C++ RPC tests on top of the dual lifecycle mock.
- Updated the LibraryTest project files to remove `RpcByvalLifecycleMock` and include `RpcDualLifecycleMock`.

# TEST

- Build: `copilotBuild.ps1 -Configuration Debug -Platform x64` from `Test/UnitTest` passed: 0 errors, 0 warnings.
- Build: `copilotBuild.ps1 -Configuration Debug -Platform Win32` from `Test/UnitTest` passed: 0 errors, 0 warnings.
- UnitTest: `LibraryTest` Debug x64 passed: 2/2 files, 14/14 cases.
- UnitTest: `LibraryTest` Debug Win32 passed: 2/2 files, 14/14 cases.
- UnitTest: `RuntimeTest` Debug x64 passed: 4/4 files, 257/257 cases.
- UnitTest: `RuntimeTest` Debug Win32 passed: 4/4 files, 257/257 cases.
- UnitTest: `CppTest` Debug x64 passed: 2/2 files, 223/223 cases.
- UnitTest: `CppTest` Debug Win32 passed: 2/2 files, 223/223 cases.
- UnitTest: `CppTest_Metaonly` Debug x64 passed: 2/2 files, 223/223 cases.
- UnitTest: `CppTest_Metaonly` Debug Win32 passed: 2/2 files, 223/223 cases.
- UnitTest: `CppTest_Reflection` Debug x64 passed: 2/2 files, 223/223 cases.
- UnitTest: `CppTest_Reflection` Debug Win32 passed: 2/2 files, 223/223 cases.
- Skipped `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, and `Build.ps1` as requested because this refactor does not affect reflection registration, compiler tests, or code generation.
- Hygiene: `git diff --check` reported no whitespace errors; only normal CRLF conversion warnings from the Windows checkout.

# PROPOSALS

- The requested refactoring is complete. `RpcDualLifecycleMock` is now the test mock responsible for dispatcher hookup and wrapper scenarios, and the by-value collection tests exercise wrappers by crossing from lifecycle1 ownership to lifecycle2 proxy creation.
