# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

In RpcDualJsonDispatcherMock.cpp, delete GetExePath, move GetJsonValueOutputPath to Helper.h/cpp. Remember that there is no x86 on Linux so this function is over complicated, just do what other path functions do.

# UPDATES

# TEST [CONFIRMED]

Confirm by static inspection that `Test/Source/RpcDualJsonDispatcherMock.cpp` no longer defines `GetExePath` or `GetJsonValueOutputPath`, and that `GetJsonValueOutputPath` is declared in `Test/Source/Helper.h` and implemented in `Test/Source/Helper.cpp` beside the other test path helpers.

Build and run the relevant Workflow unit-test path after changing C++ source files. Because JSON value output feeds the TypeScript package, run `Test/TypeScript/prepare.ps1` followed by `npm run build`.

Success means all required C++ builds/tests and the TypeScript type check pass, and the Linux implementation of `GetJsonValueOutputPath` only emits the x64 JSON value path.

# PROPOSALS

- No.1 MOVE JSON VALUE OUTPUT PATH TO TEST HELPER [CONFIRMED]

## No.1 MOVE JSON VALUE OUTPUT PATH TO TEST HELPER

Remove the local executable path helper from `Test/Source/RpcDualJsonDispatcherMock.cpp`. Declare `GetJsonValueOutputPath` in `Test/Source/Helper.h`, implement it in `Test/Source/Helper.cpp`, and keep its Windows layout consistent with the other helper path functions. For GCC/Linux, return the `JsonValues64` path directly because Linux tests do not run x86.

### CODE CHANGE

Updated `Test/Source/RpcDualJsonDispatcherMock.cpp`, `Test/Source/Helper.h`, and `Test/Source/Helper.cpp`:
- Removed the dispatcher-local `GetExePath` and `GetJsonValueOutputPath` implementations.
- Added a narrow `GetJsonValueOutputPath` declaration in `RpcDualJsonDispatcherMock.cpp` so `DumpJsonValues` uses the shared test helper without pulling reflection-only helper declarations into `CppTest`.
- Declared `GetJsonValueOutputPath` in `Helper.h`.
- Implemented `GetJsonValueOutputPath` in `Helper.cpp` next to the other path helpers.
- Kept Windows Win32/x64 output paths unchanged and simplified GCC/Linux to return `../../TypeScript/JsonValues64/`.
- Let `Helper.cpp` compile a minimal no-reflection branch for `CppTest`, containing only the shared JSON output path helper and its executable-path dependency.
- Added `Test/Source/Helper.cpp` to `Test/UnitTest/CppTest/CppTest.vcxproj` and `.filters` so `CppTest` links the shared helper.

### CONFIRMED

- Debug Win32 build passed.
- Debug x64 build passed.
- `LibraryTest` passed on Win32 and x64.
- `CompilerTest_GenerateMetadata` passed on Win32 and x64.
- `CompilerTest_LoadAndCompile` passed on x64.
- `RuntimeTest` passed on Win32 and x64.
- `CppTest` passed on Win32 and x64.
- `CppTest_Metaonly` passed on Win32 and x64.
- `CppTest_Reflection` passed on Win32 and x64.
- `Test/TypeScript/prepare.ps1` passed.
- `npm run build` in `Test/TypeScript` passed.
- `Tools/Tools/Build.ps1 Workflow` passed.
- Static inspection confirmed `RpcDualJsonDispatcherMock.cpp` no longer defines `GetExePath` or `GetJsonValueOutputPath`, and the shared helper lives in `Helper.h/cpp`.
