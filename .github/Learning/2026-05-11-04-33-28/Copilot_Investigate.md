# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

- you have to follow `REPO-ROOT/.github/Guidelines/Coding.md` when coding.
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.
  - It is important to do task one by one strictly, by me designing tasks in this way, we can achieve:
  - Easy-to-understand commits for file changing that is easy to review.
  - Limit side effects so that you don't have to deal with massive of issues at the same time.
- When I say `RunRpcTestCase` needs to change, usually it means all version of `RunRpcTestCase`.
  - No need to create helper functions just to share code between them unless explicitly instructed.

## Task 1

Remote `inline` of functions in `WfLibraryRpc.h`, extern them and put implementation in `WfLibraryRpc.cpp`.
Verify all `WfLibrary*.(h|cpp)`, even C++ not requires but we want to have `extern` on all function forward declarations.

# UPDATES

# TEST [CONFIRMED]

Static source verification and existing unit tests will cover this task:

- `WfLibraryRpc.h` should no longer contain non-constant inline function definitions for RPC boxing and event-exception helpers.
- `WfLibraryRpc.cpp` should contain the moved helper implementations.
- All free-function declarations in `Source/Library/WfLibrary*.h` and `Source/Library/Rpc/WfLibrary*.h` should explicitly begin with `extern`.
- The solution should build for Debug x64 and Debug Win32.
- The required unit test projects in `Project.md` should pass.

# PROPOSALS

- No.1 Move RPC helper definitions out of the header and normalize `extern` declarations [CONFIRMED]

## No.1 Move RPC helper definitions out of the header and normalize `extern` declarations

### CODE CHANGE
- Moved `BoxRpcObjectReference`, `BoxRpcException`, `CreateRpcEventExceptionMap`, `MergeRpcEventExceptionMap`, `BoxRpcEventExceptionMap`, and `UnboxRpcEventExceptionMap` from inline definitions in `WfLibraryRpc.h` to implementations in `WfLibraryRpc.cpp`.
- Added explicit `extern` to the moved RPC helper declarations and the existing free-function forward declarations in `WfLibraryRpc.h`.
- Added explicit `extern` to the RPC JSON helper declarations in `WfLibraryRpcJson.h`.
- Added explicit `extern` to the local helper forward declarations in `WfLibraryRpcWrappers.cpp`.

### CONFIRMED

- Static scan found no non-`constexpr` inline function definitions in `Source/Library/WfLibrary*.h` or `Source/Library/Rpc/WfLibrary*.h`.
- Static scan found no namespace-level function forward declarations without `extern` in `Source/Library/WfLibrary*.(h|cpp)` or `Source/Library/Rpc/WfLibrary*.(h|cpp)`.
- `copilotBuild.ps1 -Configuration Debug -Platform x64`: passed with 0 warnings and 0 errors.
- `copilotBuild.ps1 -Configuration Debug -Platform Win32`: passed with 0 warnings and 0 errors.
- `LibraryTest` x64 and Win32: passed, 2/2 files and 14/14 cases.
- `CompilerTest_GenerateMetadata` x64 and Win32: passed, 1/1 files and 2/2 cases.
- `CompilerTest_LoadAndCompile` x64: passed, 6/6 files and 709/709 cases.
- `RuntimeTest` x64 and Win32: passed, 4/4 files and 261/261 cases.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` x64 and Win32: each passed, 2/2 files and 227/227 cases.
- `Test/TypeScript/prepare.ps1`: passed.
- `npm run build` in `Test/TypeScript`: passed.
- `Build.ps1 Workflow`: passed and regenerated release amalgamation files.
- `git diff --check`: passed.
