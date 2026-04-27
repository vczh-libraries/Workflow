# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow the instruction to commit and push for each task, before doing the next task.

## Task 1

In generated `rpclistener_*`, the last parameter should be `*` not `^`.
This includes the function for an interface, and the `rpclistener_Attach`.
This should help removing unnecessary conversion from `T*` to `Ptr<T>` in `TestCasesRpc.cpp`, as well as other places.

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

- Problem confirmation:
  - After changing the generated `rpclistener_*` signatures from `^` to `*`, the first `Debug|x64` build failed in `Test/SourceCppGenRpc/TestCasesRpc.cpp` and `Test/UnitTest/RuntimeTest/TestRpc.cpp` because the remaining attachment bridge still passed `Ptr<IDescriptable>` into `rpclistener_Attach(..., IDescriptable*)`.
  - This proved the requested change was not only a metadata-format change; the runtime bridge and generated RPC test harness also had to consume the new raw-pointer signature.
- Test idea:
  - Regenerate the RPC metadata and C++ outputs, then confirm every generated `rpclistener_*` and `rpclistener_Attach` uses raw-pointer last parameters and that all listener-attach bridges pass `obj` directly instead of wrapping it into `Ptr<T>`.
- Verification commands:
  - `& ..\..\.github\Scripts\copilotBuild.ps1 -Configuration Debug -Platform x64`
  - `& ..\..\.github\Scripts\copilotBuild.ps1 -Configuration Debug -Platform Win32`
  - `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable LibraryTest -Configuration Debug -Platform x64`
  - `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable LibraryTest -Configuration Debug -Platform Win32`
  - `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_GenerateMetadata -Configuration Debug -Platform x64`
  - `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_GenerateMetadata -Configuration Debug -Platform Win32`
  - `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_LoadAndCompile -Configuration Debug -Platform x64`
  - `& ..\..\.github\Scripts\copilotBuild.ps1 -Configuration Debug -Platform x64`
  - `& ..\..\.github\Scripts\copilotBuild.ps1 -Configuration Debug -Platform Win32`
  - `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable RuntimeTest -Configuration Debug -Platform x64`
  - `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable RuntimeTest -Configuration Debug -Platform Win32`
  - `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CppTest -Configuration Debug -Platform x64`
  - `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CppTest -Configuration Debug -Platform Win32`
  - `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Metaonly -Configuration Debug -Platform x64`
  - `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Metaonly -Configuration Debug -Platform Win32`
  - `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Reflection -Configuration Debug -Platform x64`
  - `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Reflection -Configuration Debug -Platform Win32`
  - `& ..\Tools\Tools\Build.ps1 Workflow`
- Success criteria:
  - `CompilerTest_LoadAndCompile` passes both internal runs with `Passed test files: 6/6` and `Passed test cases: 700/700`.
  - Generated RPC metadata uses `*` instead of `^` in every `rpclistener_*` and `rpclistener_Attach` last parameter.
  - Generated `TestCasesRpc.cpp` files in `Test/Generated/CppRpc32`, `Test/Generated/CppRpc64`, and `Test/SourceCppGenRpc` call `rpclistener_Attach(ref.typeId, mock, ref, obj);` directly.
  - `RuntimeTest` and all `CppTest*` projects pass on x64 and Win32.
  - The repo-wide `Build.ps1 Workflow` wrapper exits with code `0`.

# PROPOSALS

- No.1 Generate listeners with raw target pointers [CONFIRMED]

## No.1 Generate listeners with raw target pointers

### CODE CHANGE

- Changed `Source/Analyzer/WfAnalyzer_GenerateRpc.cpp` so generated `rpclistener_<Interface>` functions and `rpclistener_Attach` use raw-pointer last parameters instead of shared-pointer parameters.
- Changed the generated attach and wrapper call sites in the same file from shared-pointer casts to raw-pointer casts so the emitted Workflow metadata and generated C++ stay consistent with the new listener signatures.
- Changed `Test/UnitTest/CompilerTest_LoadAndCompile/TestRpcCompile.cpp` so the generated RPC harness writes `rpclistener_Attach(ref.typeId, mock, ref, obj);` instead of wrapping `obj` in `Ptr<IDescriptable>`.
- Changed `Test/UnitTest/RuntimeTest/TestRpc.cpp` so the dynamically loaded `rpclistener_Attach` function type and invocation both use `IDescriptable*`.
- Reran `CompilerTest_LoadAndCompile` to refresh the generated RPC metadata, Workflow assembly dumps, `Test/Generated/CppRpc{32,64}`, and `Test/SourceCppGenRpc` outputs.

### CONFIRMED

- The root cause was a partial type migration. The listener generator started producing raw-pointer signatures, but the two consumers that bridge local objects into `rpclistener_Attach` still assumed shared pointers.
- `rpclistener_*` only attaches event handlers to an existing object; it does not transfer ownership. Using `T*` is therefore the correct ownership model and removes the unnecessary `T* -> Ptr<T>` wrapping that previously appeared in generated RPC test harnesses.
- After updating the generator and both consumer bridges, regenerated RPC metadata now consistently uses raw-pointer listener parameters, the generated C++ harness passes raw pointers directly, both debug builds succeed, all required unit tests pass on x64 and Win32, and the repo-wide `Build.ps1 Workflow` wrapper succeeds with `EXIT:0`.
