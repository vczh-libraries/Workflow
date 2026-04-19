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

When any *.h or *.cpp file is changed, unit test is required to run.
When any test case fails, you must fix the issue immediately, even those errors are unrelated to the issue you are working on.

### Project Execution Order

`CompilerTest_GenerateMetadata` read all reflectable types in C++ code and generate metadata of type information.
`CompilerTest_LoadAndCompile` load these metadata so that reflectable C++ types are visible to Workflow script, generating C++ source code from some Workflow scripts and they are to be executed later.

The correct order to run them is:
- Build debug Win32 and x64.
- Run `LibraryTest` for Win32 and x64.
- Run `CompilerTest_GenerateMetadata` for Win32 and x64.
- Run `CompilerTest_LoadAndCompile` for x64.
  - If it updates any C++ source code, build debug Win32 and x64 again.
- Run `RuntimeTest` for Win32 and x64.
- Run `CppTest`, `CppTest_Metaonly` and `CppTest_Reflection` for Win32 and x64.

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

`Test/Resources/Index*.txt` is an index of all files in `Test/Resources/*`.
- `AnalyzerScope`: Compiled in `TestAnalyzer.cpp`, testing against multiple modules.
- `AnalyzerError`: Compiled in `TestAnalyzer.cpp`, testing against Workflow compile errors.
- `Runtime`: Compiled in `TestRuntimeCompile.cpp` and loaded in `TestRuntime.cpp`, testing against attribute metadata.
- `Codegen`: Compiled in `TestCodegen.cpp` and executed in `TestRuntime.cpp`, testing against binary generation and virtual machine execution.
- `Rpc`: Compiled in `TestRpcCompile.cpp` and executed in `TestRpc.cpp`, testing against RPC feature.
- `Debugger`: Compiled and executed in `TestDebugger.cpp`, testing against Workflow virtual machine debugger feature.
- Others: Compiled in `TestSamples.cpp`, test against the Workflow parser.

`Codegen` and `Rpc` generates C++ source files from each sample, affecting `CppTest`, `CppTest_Metaonly`, `CppTest_Reflection`.
- Changing the compiler or samples will end up updating C++ source files in these projects, causing them need to rebuild.
- Generated C++ source code will be added to `Generated_(Cpp|Reflection)(Rpc)?.vcxitems`, they will be consumed by `CppTest*` unit test projects.
- `Codegen` will also generate `TestCases.cpp`, `Rpc` will also generate `TestCasesRpc.cpp`, they are referenced by `CppTest*` unit test projects.
- Different `CppTest*` compile the same set of source code using different reflection options, source files used by them should be compatible with all reflection options:
  - Avoid using any reflection features.
  - The only exception are type reflection registration files for workflow generated types, they are referenced in `Generated_Reflection(Rpc)?.vcxitems` with preprocessor applied.

### Code Generation Tools

#### REPO-ROOT/../Tools/Tools/GlrParserGen.exe

This executable needs to run if any file in the following folders are changed:
- `REPO-ROOT/Source/Parser/Syntax`

There is a `Parser.xml` file in these folder.
You need to offer the absolute path of `Parser.xml` to the tool as a command-line argument.
Only run necessary `Parser.xml` in folders that are changed.
