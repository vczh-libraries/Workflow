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

Here is a list of unit test projects in `REPO-ROOT/Test/UnitTest/{NAME}/{NAME}.vcxproj` folder, you are required to run all of them in order:
- `LibraryTest`: Unit test for libraries implemented in C++ that offers to Workflow script language
- `CompilerTest_GenerateMetadata`: Generate binary metadata that is used by `CompilerTest_LoadAndCompile`
  - This project is special, it is required to run only when files in these folders are changed:
    - `REPO-ROOT/Source/Library`
    - `REPO-ROOT/Source/Parser`
    - `REPO-ROOT/Test/Source`
  - It generates `Test/UnitTest/Generated/Reflection{32,64}.bin` and `Test/UnitTest/Generated/Reflection{32,64}.txt`,
    then compares the generated `.txt` against the baseline in `Test/Resources/Baseline/Reflection{32,64}.txt`.
  - If the schema of reflected types has changed, this comparison will fail. This is expected.
    To update the baseline, copy the generated `.txt` files from `Test/UnitTest/Generated/` to `Test/Resources/Baseline/`, then re-run.
- `CompilerTest_LoadAndCompile`: Workflow compiler test
  - Test cases are in `REPO-ROOT/Test/Resources/Index*.txt`. Each index file lists files in the corresponding sub folder in `REPO-ROOT/Test/Resources`.
  - The compiler would generate binary files for both x86 and x64.
  - Binary files are Workflow virtual machine instructions, the difference is that some native C++ types and Workflow type mappings vary between x86 and x64, e.g., `vint` in C++ and `int` in Workflow, as well as a little bit others.
  - Only test cases in category `Codegen` and `Debugger` are executable, test cases in other categories are for testing Workflow compiler errors.
- `CppTest`: Generated C++ code from executable test cases.
- `CppTest_Metaonly`: Generated C++ code from executable test cases.
- `CppTest_Reflection`: Generated C++ code from executable test cases.
When any *.h or *.cpp file is changed, unit test is required to run.

You are required to build and run all unit test projects with both `Debug|x64` and `Debug|Win32`. Except for `CompilerTest_LoadAndCompiler` which only needs `Debug|x64`.

When any test case fails, you must fix the issue immediately, even those errors are unrelated to the issue you are working on.

### Code Generation Tools

#### REPO-ROOT/../Tools/Tools/GlrParserGen.exe

This executable needs to run if any file in the following folders are changed:
- `REPO-ROOT/Source/Parser/Syntax`

There is a `Parser.xml` file in these folder.
You need to offer the absolute path of `Parser.xml` to the tool as a command-line argument.
Only run necessary `Parser.xml` in folders that are changed.
