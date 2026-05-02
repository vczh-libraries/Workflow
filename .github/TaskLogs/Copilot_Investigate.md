# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Task 1

I would like to move RpcControllerMock.(h|cpp) to Source\Library with these changes:
- Now this pair of file needs to be in the library vcxitems instead of in each test projects' vcxproj files
- Rename it to WfLibraryRpcController.(h|cpp)
- Rename the class to vl::rpc_controller::RpcControllerDefault.
- You should adjust the guard macro naming and the header of the header file align with all other library header files.

Further more, you need to fix all CHECK_ERROR or CHECK_FAIL which have actual messages to align with the rest of the source code:
- At the beginning and the end of the function, define and undef ERROR_MESSAGE_PREFIX, to include the full class and function name.
- Use it inside CHECK_ERROR or CHECK_FAIL.
- You must check out other CHECK_ERROR samples in Source or Import to see how they are used.
- Ignore `CHECK_FAIL(L"Not Supported|);` or `CHECK_FAIL(L"Not Supported!");`

No reflection registration is needed, you can skip CompilerTest_GenerateMetadata and CompilerTest_LoadAndCompiler and Build.ps1, to shortern your test. This change should not affect the compiler and any code generation.

# UPDATES

# TEST [CONFIRMED]

Use the existing RPC and generated C++ runtime tests to confirm the library-level controller remains linkable and behavior-compatible after moving the controller implementation out of each test project:

- Build `Test\UnitTest` in `Debug|x64` and `Debug|Win32` using `.github\Scripts\copilotBuild.ps1`.
- Run `LibraryTest`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` in both `Debug|x64` and `Debug|Win32` using `.github\Scripts\copilotExecute.ps1`.
- Confirm no tracked source/project references still use `RpcControllerMock`, except the original request copied in this document.
- Skip `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompiler`, and `Build.ps1` as requested because this change does not require reflection registration or code generation.

Result:

- `Debug|x64` build: passed with 0 warnings and 0 errors.
- `Debug|Win32` build: passed with 0 warnings and 0 errors.
- `LibraryTest Debug|x64`: passed 2/2 test files, 14/14 test cases.
- `RuntimeTest Debug|x64`: passed 4/4 test files, 257/257 test cases.
- `CppTest Debug|x64`: passed 2/2 test files, 223/223 test cases.
- `CppTest_Metaonly Debug|x64`: passed 2/2 test files, 223/223 test cases.
- `CppTest_Reflection Debug|x64`: passed 2/2 test files, 223/223 test cases.
- `LibraryTest Debug|Win32`: passed 2/2 test files, 14/14 test cases.
- `RuntimeTest Debug|Win32`: passed 4/4 test files, 257/257 test cases.
- `CppTest Debug|Win32`: passed 2/2 test files, 223/223 test cases.
- `CppTest_Metaonly Debug|Win32`: passed 2/2 test files, 223/223 test cases.
- `CppTest_Reflection Debug|Win32`: passed 2/2 test files, 223/223 test cases.

# PROPOSALS

- No.1 Move the default RPC controller into the library project [CONFIRMED]

## No.1 Move the default RPC controller into the library project

### CODE CHANGE

Moved `Test\Source\RpcControllerMock.h` and `Test\Source\RpcControllerMock.cpp` to `Source\Library\WfLibraryRpcController.h` and `Source\Library\WfLibraryRpcController.cpp`, renamed the class to `vl::rpc_controller::RpcControllerDefault`, updated the header banner and guard macro to match library files, and added the implementation to `VlppWorkflow_Library.vcxitems` and its filters.

Removed the old per-test-project `RpcControllerMock` entries from `LibraryTest`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` project files and filters. Updated `RpcByvalLifecycleMock` and `RpcDualLifecycleMock` to include `WfLibraryRpcController.h` and use `RpcControllerDefault`.

Updated the controller's `CHECK_ERROR` calls with an `ERROR_MESSAGE_PREFIX` scoped around `RpcControllerDefault::SetSuppressedFlag`, following the existing `Source` and `Import` style for prefixed error messages.

### CONFIRMED

The moved controller compiles as part of the shared library items and remains available to all affected test projects without per-project source entries. The selected x64 and Win32 builds and unit tests all passed, and a tracked-source/project-file search confirms there are no remaining `RpcControllerMock` references outside the copied problem statement.
