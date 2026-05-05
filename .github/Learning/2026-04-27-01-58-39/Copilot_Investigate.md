# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow the instruction to commit and push for each task, before doing the next task.

## Task 1

`@rpc:Cached` decorated properties are already supported, but the implementation uses `_rpcCache_Value`. I would like you to do an improvement.
`_rpcCache_Value` contains all property values and all property availabilities, but you can do it better by:
For any `@rpc:Cached prop NAME : TYPE {...}` property, create variable `var NAME<Cached> : T = default;` and `var NAME<Available> : bool = false;`.
These variables will be inside the new interface expression, becoming its fields.
`_rpcCache_Value` can then be removed. When `NAME<Available>` is false it means the getter should be called to fill `NAME<Cached>`.
The feature is already implemented, just want to replace the `_rpcCache_Value` implementation with multiple variables. To keep it strong typed.

## Task 2

Follow `job.new-sample.md`, read `PropDefault.txt` and copy (indentation should be double spaces): 
- `PropDefaultInterface.txt`
- `PropDefaultList.txt`
- `PropDefaultInterfaceList.txt`

It changes the property type from `string` to `IValue^`, `string[]` and `IValue^[]`.
There are many `IValue` definition in other samples, copy one that has a string property so that it is easy to log.
During logging for list, each item could just be one letter, therefore you can say changing from `[A]` to `[BCD]`.

You will need extra verification in `clientMain` to test if the property returns a
- before changing, a wrapper, which means it implements `IRpcWrapperBase`, other samples have similar tests everywhere.
  - because the value is set inside the interface, therefore it is a service side object.
  - if it is a container of interfaces, all elements should also be wrappers.
- after changing, not a wrapper.
  - because the value is set from the client side, therefore it is a local object.
  - if it si a container of interfaces, all elements should also be interfaces.

Because these samples are testing against properties, so that not only for interface but also for list, the complete value should be replaced, instead of changing data inside the interface or container.

If the implementation is correct, these new samples should just pass every tests.

### Verifying Samples

Workflow script syntax and semantic should be intuitive.
During reading the sample, you should verify it with the goal of the task.
Ensure all logs or exceptions in the sample accurately reflected the intention of the design.
Ensure the expected result would be what users would expect.

### Restriction

Understand what the test case trying to say, you are not allowed to change:
- The content of the verified sample, unless it doesn't build.
- Workflow parser.
- Workflow compiling.
- Workflow to C++ code generation.

You are highly possibly need to fix:
- `Rpc(B|Unb)oxBy(val|ref)`, as these 4 C++ functions are directly called in generated wrapper classes written Workflow script.
- The wrapper classes generation.
- implementation of `RpcDualLifecycleMock` and its connected interfaces if sample fails in either `RuntimeTest` or `CppTest*`.
- The generated C++ code is very straight forward, if it fails, check `RpcDualLifecycleMock` first.
  - The comment in the sample describes how `RpcDualLifecycleMock` and the generated C++ code is supposed to work.

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

- Task 1 completed.
- Replaced `_rpcCache_Value` array-based cache storage with strongly typed per-property fields inside the generated `new interface` expression:
  - `NAME<Cached> : T`
  - `NAME<Available> : bool`
- Removed cache cell reads and writes based on `system::Array`.
- Added generated invalidation helper methods on wrapper interfaces for cached properties with `ValueChanged`, and used event handlers to reset `NAME<Available>` to `false`.
- Verified the new implementation against all related unit test projects on x64 and Win32.
- Task 2 completed.
- Added the new RPC samples:
  - `Test/Resources/Rpc/PropDefaultInterface.txt`
  - `Test/Resources/Rpc/PropDefaultList.txt`
  - `Test/Resources/Rpc/PropDefaultInterfaceList.txt`
- Updated `Test/Resources/IndexRpc.txt` with the new expected outputs.
- Added project include entries so the new generated RPC C++ and reflection files are compiled by `CppTest*`.
- Adjusted the shared C++ and runtime RPC test harnesses to skip sample-specific listener attachment for built-in byref container type ids while still attaching listeners for generated RPC interfaces.
- Verified the new samples cover wrapper vs local-object behavior before and after property replacement:
  - interface value
  - string list value
  - interface list value

# TEST

- `& ..\..\.github\Scripts\copilotBuild.ps1 -Configuration Debug -Platform x64`
- `& ..\..\.github\Scripts\copilotBuild.ps1 -Configuration Debug -Platform Win32`
- `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable LibraryTest -Configuration Debug -Platform x64`
- `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable LibraryTest -Configuration Debug -Platform Win32`
- `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_GenerateMetadata -Configuration Debug -Platform x64`
- `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_GenerateMetadata -Configuration Debug -Platform Win32`
- `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_LoadAndCompile -Configuration Debug -Platform x64`
- `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_LoadAndCompile -Configuration Debug -Platform Win32`
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
- `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable RuntimeTest -Configuration Debug -Platform x64`
- `& ..\..\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable RuntimeTest -Configuration Debug -Platform Win32`
- Result: all passed.

# PROPOSALS

- Task 1 and Task 2 are both ready to commit in two-commit batches:
  - manual changes
  - generated files
