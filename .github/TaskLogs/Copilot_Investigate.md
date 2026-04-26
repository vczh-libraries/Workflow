# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow the instruction to commit and push for each task, before doing the next task.

## Task 1

In generated wrapper Workflow scripts:
- Rename `rpcsvc_RaiseInvalidServiceTypeId` to `rpcwrapper_IsInterfaceTypeId`.
  - It returns `bool` instead of raising an exception, the exception should be raised at the caller side.
  - The case from -1 to -7 could becomes an if statement before the switch case says `if (typeId in range [-7, -1]) ...`.
- Rename `rpcsvc_EnsureCtorServiceTypeId` to `rpcwrapper_IsCtorInterfaceTypeId`.
  - It returns `bool` instead of raising an exception, the exception should be raised at the caller side.
- Therefore in `RegisterService` it could says `if rpcwrapper_IsCtorInterfaceTypeId do the work else if rpcwrapper_IsInterfaceTypeId raise not a ctor interface otherwise raise invalid type id`.
- You would also need to fix other codegen places that affected by this change.

## General Instruction

If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
- Pass all unit test, fix any test failure including pre-existings.
- After finishing everything, git commit and git push to the current branch.
- If in any task you are adding new test sample, or modifying any C++ or Workflow generation code, causing a huge amount of files generated from test samples to change:
  - Two commits are required. First commit only has all modified files and files you created directly, second commit has all new files that not created by you (aka auto generated)
  - Otherwise, One commit is good.
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.

# UPDATES

# TEST [CONFIRMED]

- Existing targeted coverage is `Test/UnitTest/CompilerTest_LoadAndCompile/TestRpcCompile.cpp`, which already inspects the generated wrapper module text for the service-registration helpers and `RequestService`.
- Full confirmation also requires the repository-mandated sequence for `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` on both `Win32` and `x64`, followed by `..\Tools\Tools\Build.ps1 Workflow`.
- Success criteria:
  - The generated wrapper declares `rpcwrapper_IsInterfaceTypeId` and `rpcwrapper_IsCtorInterfaceTypeId` instead of the old throwing helpers.
  - `RegisterService` raises `RPC service type id is not an @rpc:Ctor interface.` only after `rpcwrapper_IsInterfaceTypeId(typeId)` succeeds, and raises `RPC service type id does not exist.` otherwise.
  - `RequestService` still only checks whether the service is registered and raises `RPC service is not registered.` when no service is bound.
  - All regenerated wrapper, assembly, and generated C++ outputs build and the full unit-test sequence passes.
- Confirmed by:
  - `LibraryTest` on `Win32` and `x64`
  - `CompilerTest_GenerateMetadata` on `Win32` and `x64`
  - `CompilerTest_LoadAndCompile` on `x64`, which regenerated the wrapper, assembly, and C++ outputs
  - Rebuilding `Debug|Win32` and `Debug|x64` after regeneration
  - `RuntimeTest` on `Win32` and `x64`
  - `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` on `Win32` and `x64`
  - `..\Tools\Tools\Build.ps1 Workflow`

# PROPOSALS

- No.1 Replace throwing type-id helpers with boolean wrapper predicates

## No.1 Replace throwing type-id helpers with boolean wrapper predicates

### CODE CHANGE

- Replaced the generated wrapper helper `rpcsvc_RaiseInvalidServiceTypeId` with `rpcwrapper_IsInterfaceTypeId`, returning `bool` and recognizing built-in collection service ids through a range check for `[-7, -1]` before the generated `switch`.
- Replaced `rpcsvc_EnsureCtorServiceTypeId` with `rpcwrapper_IsCtorInterfaceTypeId`, returning `bool` instead of throwing.
- Reworked generated `RegisterService` so it now:
  - registers or unregisters the service only when `rpcwrapper_IsCtorInterfaceTypeId(typeId)` succeeds
  - raises `RPC service type id is not an @rpc:Ctor interface.` when the type id is a known interface but not a ctor interface
  - raises `RPC service type id does not exist.` when the type id is unknown
- Updated the focused compiler test that inspects generated wrapper text to assert the new helper names, the built-in-id range check, the new `RegisterService` branching, and the absence of the old helper names.

### CONFIRMED

- The generator emits the new boolean predicates and all downstream generated wrapper, assembly, and C++ files were regenerated consistently.
- The focused wrapper test now checks the new helper names and `RegisterService` behavior directly.
- The full required unit-test and build sequence passed after regeneration, so the change is validated across Workflow runtime, metadata generation, wrapper generation, and generated C++ integration.
