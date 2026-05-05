# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Task 1

The goal of the refactoring is to rename `RpcDualLifecycleMock` class to `RpcLifecycleBase` class in new pair of file `Source\Library\WfLibraryRpcLifecycle.(h|cpp)`.
Everything from `RpcDualLifecycleMock.(h|cpp)` is going to move to the new file except `RpcDualDispatcherMock` class, and a very small piece from `RpcDualLifecycleMock`.
According to the research repor `TODO_RPC_Dual.md` (especially the `## Recommended Split` section), a very tiny few thin layer from `RpcDualLifecycleMock` also needs to stay, because `RpcDualDispatcherMock` still needs to talk to `RpcDualLifecycleMock` to, for example, put the dispatcher pointer insode lifecycle. So the new very tiny thin layer class `RpcDualLifecycleMock` would inherit from `RpcLifecycleBase`. Almost all members, as well as helper types, will be move to `WfLibraryRpcLifecycle.(h|cpp)` along with the refactor.

The dependency after refactoring is strict. `WfLibraryRpcLifecycle.(h|cpp)` should not have any knowledge from the remaining `RpcDualLifecycleMock.(h|cpp)`.

Further more, you need to fix all CHECK_ERROR or CHECK_FAIL which have actual messages to align with the rest of the source code:
- At the beginning and the end of the function, define and undef ERROR_MESSAGE_PREFIX, to include the full class and function name.
- Use it inside CHECK_ERROR or CHECK_FAIL.
- You must check out other CHECK_ERROR samples in Source or Import to see how they are used.
- Ignore `CHECK_FAIL(L"Not Supported|);` or `CHECK_FAIL(L"Not Supported!");`
- `RpcByvalKeepAliveProperty` and `RpcByvalKeepAlive` from `WfLibraryRpc.cpp` seems to be a semantic equivalent copy from part of construction in the current `RpcDualDispatcherMock`. After moving almost all constructions to `WfLibraryRpcLifecycle.(h|cpp)`, you should review the code and see if `RpcByvalKeepAliveProperty` and `RpcByvalKeepAlive` could be removed or replaced.
  - In order to keep the dependency right, predefined wrappers for containers in `WfLibraryRpc.(h|cpp)` could be moved to a new pair of file `WfLibraryRpcWrappers.(h|cpp)`.

All new `WfLibraryRpc*(.h|cpp)` should be put in `Source\Library`, and they need to be in the library vcxitems instead of in each test projects' vcxproj files.

No reflection registration is needed, you can skip CompilerTest_GenerateMetadata and CompilerTest_LoadAndCompiler and Build.ps1, to shortern your test. This change should not affect the compiler and any code generation.

# UPDATES

# TEST

Use existing RPC behavior tests plus project/source searches to confirm the refactor:

- Build `Test\UnitTest` in `Debug|x64` and `Debug|Win32` using `.github\Scripts\copilotBuild.ps1`.
- Run `LibraryTest`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` in both `Debug|x64` and `Debug|Win32` using `.github\Scripts\copilotExecute.ps1`.
- Confirm `Source\Library\WfLibraryRpcLifecycle.(h|cpp)` contains no dependency on `rpc_controller_test`, `RpcDualLifecycleMock`, or `RpcDualDispatcherMock`.
- Confirm the remaining `RpcDualLifecycleMock.(h|cpp)` is only the dual dispatcher and a thin class deriving from `vl::rpc_controller::RpcLifecycleBase`.
- Skip `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompiler`, and `Build.ps1` as requested because this refactor does not affect reflection registration or code generation.

# PROPOSALS

- No.1 [CONFIRMED] Move generic lifecycle mechanics into `RpcLifecycleBase`

## No.1 [CONFIRMED] Move generic lifecycle mechanics into `RpcLifecycleBase`

### CODE CHANGE

- Added `Source\Library\WfLibraryRpcLifecycle.h` and `Source\Library\WfLibraryRpcLifecycle.cpp`.
  - `vl::rpc_controller::RpcLifecycleBase` now owns the generic lifecycle implementation previously stored in `RpcDualLifecycleMock`.
  - Helper types moved with it and were renamed to generic lifecycle names:
    - `RpcLocalObjectTracker`
    - `RpcWrapperTracker`
    - `RpcLocalObjectProperties`
  - `RpcLifecycleBase` exposes `SetDispatcher`, `SetIdMap`, and `RegisterWrapperFactory` for the remaining thin integration layer.
  - `AttachLocalObjectEvents` remains a virtual hook implemented by `RpcControllerDefault`, preserving event attachment without depending on the test mock.
- Reduced `Test\Source\RpcDualLifecycleMock.h` to:
  - the `RpcDualDispatcherMock` declaration;
  - a thin `RpcDualLifecycleMock : vl::rpc_controller::RpcLifecycleBase` compatibility layer.
- Reduced `Test\Source\RpcDualLifecycleMock.cpp` to only the dual dispatcher implementation.
  - The dispatcher now configures each lifecycle through `SetDispatcher`.
- Added the new library files to `Test\UnitTest\VlppWorkflow_Library\VlppWorkflow_Library.vcxitems` and `.filters`, so the shared library items own them instead of individual test projects.
- Updated moved `CHECK_ERROR` and `CHECK_FAIL` calls with `ERROR_MESSAGE_PREFIX` scoped to their functions.
- Reviewed `RpcByvalKeepAliveProperty` and `RpcByvalKeepAlive` in `WfLibraryRpc.cpp`.
  - They intentionally remain there because they keep by-value serialized wrapper graphs alive during generic RPC conversion.
  - Replacing them with lifecycle tracking would add the wrong dependency direction and would not cover the same temporary serialization lifetime.

### DEPENDENCY CHECK

- `Source\Library\WfLibraryRpcLifecycle.(h|cpp)` has no dependency on `rpc_controller_test`, `RpcDualLifecycleMock`, or `RpcDualDispatcherMock`.
- The remaining `RpcDualLifecycleMock` is a thin subclass of `RpcLifecycleBase`.
- `RpcDualDispatcherMock` is still test-owned and only uses the generic public lifecycle surface.

### TEST RESULT

- `Debug|x64` build: passed.
- `Debug|Win32` build: passed.
- `Debug|x64` unit tests passed:
  - `LibraryTest`: 2/2 files, 14/14 cases.
  - `RuntimeTest`: 4/4 files, 257/257 cases.
  - `CppTest`: 2/2 files, 223/223 cases.
  - `CppTest_Metaonly`: 2/2 files, 223/223 cases.
  - `CppTest_Reflection`: 2/2 files, 223/223 cases.
- `Debug|Win32` unit tests passed:
  - `LibraryTest`: 2/2 files, 14/14 cases.
  - `RuntimeTest`: 4/4 files, 257/257 cases.
  - `CppTest`: 2/2 files, 223/223 cases.
  - `CppTest_Metaonly`: 2/2 files, 223/223 cases.
  - `CppTest_Reflection`: 2/2 files, 223/223 cases.
- Skipped `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompiler`, and `Build.ps1` as requested.
