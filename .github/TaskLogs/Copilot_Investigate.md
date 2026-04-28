# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

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

# UPDATES

# TEST [CONFIRMED]

The requested changes are structural and covered by existing build and unit-test coverage:

- Task 1 should compile all projects that use RPC test mocks after `RpcLifecycleMock` is renamed to `RpcControllerMock`, reflection registration is removed, and lifecycle mocks no longer inherit `IRpcController`.
- Task 1 should pass RPC-related tests in `LibraryTest`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`, because those tests exercise byval collection wrappers and dual RPC lifecycles.
- Task 1 should regenerate reflection metadata without `vl::rpc_controller_test::RpcLifecycleMock` and update the baseline if the metadata test confirms that expected removal.
- Task 2 should compile with `WfErrors` declarations and definitions in the same order.

Success criteria:

- Debug Win32 and x64 builds succeed.
- Required unit-test projects pass in the order from `Project.md`.
- Static inspection confirms no source/project references to `RpcLifecycleMock` remain except historical task documents, and `WfErrors` places all H RPC errors before I C++ errors in declaration and definition order.

# PROPOSALS

- No.1 Split RPC lifecycle mocks from controller mocks [CONFIRMED]
- No.2 Reorder `WfErrors` H and Cpp groups [CONFIRMED]

## No.1 Split RPC lifecycle mocks from controller mocks

### CODE CHANGE

Rename `RpcLifecycleMock` to `RpcControllerMock`, remove its reflection registration, and make it implement only the shared `IRpcController` forwarding behavior. Add `RpcByvalControllerMock` and `RpcDualControllerMock` owned by the corresponding lifecycle mocks, return them from `GetController()`, and move controller-specific reference-count/object-id behavior into those controller mocks.

Implemented in `Test/Source` and affected unit-test project files:

- `RpcControllerMock` is no longer reflected and implements the shared controller callback forwarding behavior.
- `RpcByvalLifecycleMock` and `RpcDualLifecycleMock` now implement `IRpcLifeCycle` without inheriting `IRpcController`.
- `RpcByvalControllerMock` and `RpcDualControllerMock` own controller-only object/reference operations and are returned through covariant `GetController()` overrides.
- Existing test helpers call `GetController()->Register(...)` instead of relying on lifecycle/controller inheritance.
- Reflection generated outputs and baselines were regenerated so `vl::rpc_controller_test::RpcLifecycleMock` is no longer present.

### CONFIRMED

Task 1 is confirmed.

Debug Win32 and x64 builds passed with `0 Warning(s)` and `0 Error(s)`.

All required unit tests passed:

- `LibraryTest`: Win32 and x64.
- `CompilerTest_GenerateMetadata`: Win32 and x64, with reflection outputs and baselines updated.
- `CompilerTest_LoadAndCompile`: x64; it did not update `Test\SourceCppGen*`.
- `RuntimeTest`: Win32 and x64.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`: Win32 and x64.

## No.2 Reorder `WfErrors` H and Cpp groups

### CODE CHANGE

Move the RPC attribute-checking `WfErrors` declarations/definitions before C++ code-generation errors, keeping each group internally ordered by error number and keeping header/cpp order identical.

Implemented in `Source/Analyzer/WfAnalyzer.h` and `Source/Analyzer/WfAnalyzer_Errors.cpp`:

- Moved `RpcGeneratedNameConflict` (`H9`) before `RpcWrapperGenerationRequiresPropertyMode` (`I0`).
- Kept `RpcWrapperGenerationRequiresPropertyMode`, `RpcWrapperGenerationRequiresCollectionReturnTransfer`, `RpcWrapperGenerationRequiresCollectionParameterTransfer`, and `RpcMangledNameConflict` as the `I0` to `I3` group.
- Left the `CppUnableToDecideClassOrder` and `CppUnableToSeparateCustomFile` C++ code-generation errors after the I group.

### CONFIRMED

Task 2 is confirmed.

Debug Win32 and x64 builds passed with `0 Warning(s)` and `0 Error(s)`.

All required unit tests passed:

- `LibraryTest`: Win32 and x64.
- `CompilerTest_GenerateMetadata`: Win32 and x64.
- `CompilerTest_LoadAndCompile`: x64.
- `RuntimeTest`: Win32 and x64.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`: Win32 and x64.

The repository-level final verification `..\Tools\Tools\Build.ps1 Workflow` also passed. It updated the generated release mirror files `Release\VlppWorkflowCompiler.h` and `Release\VlppWorkflowCompiler.cpp` with the same declaration and definition reorder.
