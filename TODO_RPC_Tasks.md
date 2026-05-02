# Repro

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
