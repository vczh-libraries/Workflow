# General Instruction

## Solution to Work On

You are working on the solution `REPO-ROOT/Test/UnitTest/UnitTest.sln`,
therefore `SOLUTION-ROOT` is `REPO-ROOT/Test/UnitTest`.

## Files not Allowed to Modify

Files in these folders (recursively) are not allowed to modify.
You can only change them using what is described in the `Code Generation Projects` section.
If you encounter any error that prevent these files from being generated,
always fix the root cause.
- `REPO-ROOT/Test/Generated`
- `REPO-ROOT/Source/Parser/Generated`

Files in `REPO-ROOT/Import` and `REPO-ROOT/Release` (recursively) are also not allowed to modify.
These files are prepared for foreign dependencies.

## Repo Specific Guidelines of Coding

In `REPO-ROOT/.github/Rules`:
- `new-sample.md`: Guidelines of Adding New Workflow Sample
- `new-sample-rpc.md`: Extra Guidelines of Adding New Workflow Sample in RPC Category
- `document-and-commit.md`: Guidelines of Documentation and Git Commit Maintenance

## Projects for Verification

Here is a list of unit test projects in `REPO-ROOT/Test/UnitTest/{NAME}/{NAME}.vcxproj` folder, you are required to run all of them:
- `LibraryTest`: Unit test for libraries implemented in C++ that offers to Workflow script language
- `CompilerTest_GenerateMetadata`: Generate binary metadata that is used by `CompilerTest_GenerateMetadata`
  - This project is special, it is required to run only when files in these folders are changed:
    - `REPO-ROOT/Source/Library`
    - `REPO-ROOT/Source/Parser`
    - `REPO-ROOT/Test/Source`
- `CompilerTest_LoadAndCompile`: Workflow compiler test
  - Test cases are in `REPO-ROOT/Test/Resources/Index*.txt`. Each index file lists files in the corresponding sub folder in `REPO-ROOT/Test/Resources`.
  - The compiler would generate binary files for both x86 and x64.
  - Binary files are Workflow virtual machine instructions, the difference is that some native C++ types and Workflow type mappings vary between x86 and x64, e.g., `vint` in C++ and `int` in Workflow, as well as a little bit others.
- `CppTest`: Generated C++ code from executable test cases.
- `CppTest_Metaonly`: Generated C++ code from executable test cases.
- `CppTest_Reflection`: Generated C++ code from executable test cases.

In `REPO-ROOT/Test/TypeScript` there is a TypeScript package, it will becomes available after running `CompilerTest_LoadAndCompiler` and `CppTest`. You need to run `prepare.ps1` followed by `npm run build` and ensure you don't see any error. This project verifies if JSON serialization of Workflow RPC is properly implemented.

When any *.h or *.cpp file is changed, unit test is required to run.
When any test case fails, you must fix the issue immediately, even those errors are unrelated to the issue you are working on.

### Project Execution Order

`CompilerTest_GenerateMetadata` read all reflectable types in C++ code and generate metadata of type information.
`CompilerTest_LoadAndCompile` load these metadata so that reflectable C++ types are visible to Workflow script, generating C++ source code from some Workflow scripts and they are to be executed later.

**IMPORTANT** You should run `CompilerTest_LoadAndCompile` for at least ones, because some compiler generated binaries are not covered by git. When the project is updated from other computers, those files might be out-dated. Either `CompilerTest_LoadAndCompiler` or `Build.ps1 Workflow` will fix this issue.

The correct order to run them is:
- Build debug Win32 and x64.
- Run `LibraryTest` for Win32 and x64.
- Run `CompilerTest_GenerateMetadata` for Win32 and x64.
- Run `CompilerTest_LoadAndCompile` for x64.
  - If it updates any C++ source code, build debug Win32 and x64 again.
- Run `RuntimeTest` for Win32 and x64.
- Run `CppTest`, `CppTest_Metaonly` and `CppTest_Reflection` for Win32 and x64.
- When you believe that all test projects should run, call `..\Tools\Tools\Build.ps1 Workflow` to test against everything.
  - It does not write any building or execution log.
  - It stops at the first failure.
  - If all test projects pass, some files in the `Release` could be changed, this is expected.
  - If you used this script, then you will be required to always run this script after you finishing a complete user instructions given to you.
    - If the user instruction asks you to do git push, run it before the last git push.

### Baseline Comparison

- `CompilerTest_GenerateMetadata`:
  - It generates `Test/UnitTest/Generated/Reflection{32,64}.bin` and `Test/UnitTest/Generated/Reflection{32,64}.txt`,
    then compares the generated `.txt` against the baseline in `Test/Resources/Baseline/Reflection{32,64}.txt`.
- `CompilerTest_LoadAndCompile`:
  - For `Runtime` test cases that use `@rpc:Interface` attributes, it generates RPC metadata and writes it to
    `Test/Generated/RpcMetadata{32,64}/{itemName}.txt`, then compares against the baseline in
    `Test/Resources/Baseline/RpcMetadata{32,64}/{itemName}.txt`.
  - If the RPC metadata generation logic has changed, this comparison will fail. This is expected.

When generated files are expected to change, baseline comparison will fail. You need to override baseline files with generated files, and run the test projects again.

### Workflow Test Samples

`Test/Resources/Index*.txt` is an index of all files in `Test/Resources/*/`, here are all important sample categories:
- `AnalyzerScope`: Compiled in `TestAnalyzer.cpp`, testing against multiple modules.
- `AnalyzerError`: Compiled in `TestAnalyzer.cpp`, testing against Workflow compile errors.
- `Runtime`: Compiled in `TestRuntimeCompile.cpp` and loaded in `TestRuntime.cpp`, testing against attribute metadata.
- `Codegen`: Compiled in `TestCodegen.cpp` and executed in `TestRuntime.cpp`, testing against binary generation and virtual machine execution.
- `Rpc`: Compiled in `TestRpcCompile.cpp` and executed in `TestRpc.cpp`, testing against RPC feature.
  - Each `Rpc` sample is split into two files:
    - `Rpc/SAMPLE.txt` contains only RPC definitions and is used to generate RPC metadata and wrappers.
    - `Rpc/SAMPLE_Test.txt` contains the executable test logic, including globals, helpers, `serviceMain`, and `clientMain`; it is loaded together with `SAMPLE.txt` when linking the test assembly.
  - Only `SAMPLE` appears in `IndexRpc.txt`; both files should be included under `Resource Files\Rpc` in `CompilerTest_LoadAndCompile`.
- `Debugger`: Compiled and executed in `TestDebugger.cpp`, testing against Workflow virtual machine debugger feature.
- Others: Compiled in `TestSamples.cpp`, test against the Workflow parser.

`Codegen` and `Rpc` generates C++ source files from each sample, affecting `CppTest`, `CppTest_Metaonly`, `CppTest_Reflection`.
- Changing the compiler or samples will end up updating C++ source files in these projects, causing them need to rebuild.
- Generated C++ source code will be added to `Generated_(Cpp|Reflection)(Rpc)?.vcxitems`, they will be consumed by `CppTest*` unit test projects.
- `Codegen` will also generate `TestCases.cpp`, `Rpc` will also generate `TestCasesRpc.cpp`, they are referenced by `CppTest*` unit test projects.
- Different `CppTest*` compile the same set of source code using different reflection options, source files used by them should be compatible with all reflection options:
  - Avoid using any reflection features.
  - The only exception are type reflection registration files for workflow generated types, they are referenced in `Generated_Reflection(Rpc)?.vcxitems` with preprocessor applied.

### Debugging Workflow Sample Compile Errors

When any test sample fail in `CompilerTest_LoadAndCompile`, one of the reason will be the sample itself has illegal Workflow script.
To address this issue, you can look for `Test/Generated/Workflow(32|64)/Parsing.CATEGORY.SAMPLE.txt`.
Compile errors will appear in this file.

### Debugging RuntimeTest Project

Many test cases load binary files compiled from `CompileTest_LoadAndCompile` and run it with a Workflow virtual machine.
When you are not confidence about the cause, you could attach a debugger to catch any exception, and you might find your self inside a Workflow virtual machine instructions execution loop.
To have a big picture of instructions, you can find Test\Generated\Workflow(32|64)\Assembly.CATEGORY.SAMPLE.txt,
along with the `Printed` section in Test\Generated\Workflow(32|64)\Parsing.CATEGORY.SAMPLE.txt,
it maps instruction to source code.
The `Printed` section logs desugar-ed workflow script input.

### General Debugging Tips

When a test project fails, you are strongly recommended to restart it with a debugger.
Except that when you are able to see Workflow compile errors in `Test/Generated/Workflow(32|64)/Parsing.CATEGORY.SAMPLE.txt`, you can skip the debugging if you don't need it.
If `CompilerTest_LoadAndCompiler` succeeded but subsequent test projects fail:
- You could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
- Pass all unit test, fix any test failure including pre-existings.

## CLI Test Projects

`ChatBotServer` and `ChatBotClient` are test projects based on Workflow RPC with `vl::inter_process::HttpServer`:
- Start `ChatBotServer`, wait for a message to print.
  - It listens to `http://localhost:8888/WorkflowChatBot` with using `RpcChannel` as the name for `vl::inter_process::IChannel`.
  - Type `exit` to exit the server, which should cause all clients to exit automatically.
- Start multiple `ChatBotClient`, enter the user name.
  - Type `exit` to exit the client.
  - Type anything to chat.
These CLI projects are interactive, they won't exit until all client typed `exit` or the server typed `exit`.

### When to Test

`ChatBotServedr` and `ChatBotClient` are strongly recommended to run when Workflow RPC is changed or new release from `VlppOS` comes.

### Recommended Test Process

To make sure related code works, you are going to run the below complete process and ensure a complete success:
- Start `ChatBotServer` with three `ChatBotClient`, using user names `Tom`, `Jerry`, and `Spike`.
- Only after when a client joined with a name you can start the next client.
- When the second client joined, the first client should be notified.
- When the third client joined, the first two clients should be notified.
- Chat on each client as a round, do 3 round.
- `exit` on one client, the others should be notified.
- Exit the server, the rest of the clients should also exit.

## Code Generation Tools

### REPO-ROOT/../Tools/Tools/GlrParserGen.exe

This executable needs to run if any file in the following folders are changed:
- `REPO-ROOT/Source/Parser/Syntax`

There is a `Parser.xml` file in these folder.
You need to offer the absolute path of `Parser.xml` to the tool as a command-line argument.
Only run necessary `Parser.xml` in folders that are changed.

## Linux Specific

`REPO-ROOT/Test/Linux` stores linux configurations for:
- `CompilerTest_GenerateMetadata`: `CompilerTest_GenerateMetadata.vcxproj`.
- `CompilerTest_LoadAndCompile`: `CompilerTest_LoadAndCompile.vcxproj`.
- `CppTest`: `CppTest.vcxproj`.
- `CppTest_Metaonly`: `CppTest_Metaonly.vcxproj`.
- `CppTest_Reflection`: `CppTest_Reflection.vcxproj`.
- `LibraryTest`: `LibraryTest.vcxproj`.
- `RuntimeTest`: `RuntimeTest.vcxproj`.

You need to build, test and debug in that specific folder, otherwise the unit test will not function properly.
On Linux, only configuration "debug x64" is available, no need to build or run projects with other configurations.
Unlike Windows, building have to be done in each folder separately.
