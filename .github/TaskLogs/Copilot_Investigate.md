# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

In TestRpcCompiler.cpp, you are going to change 3 lines of `input->*FileName = ...` to have a Rpc prefix, that is, input->*FileName = L"Rpc" + itemName ...;

Now many generated files will be renamed, you will have to fix Generated_(Cpp|Reflection)Rpc.vcxproj. Previously there are two Event.cpp and Overloading.cpp appear at the same time causing obj files have to be renamed, this time it should be not necessary now.

Read generated files carefully, and make sure outdated files are deleted (some may be automatically deleted, so you can count how many files are added and removed and you will know the diff). TestCasesRpc.cpp needs to be changed to use the new file name too.

So you are going to test CompilerTest_LoadAndCompiler and 3 CppTest* projects to make sure the change actually works.

Before doing the task, run `git clean -xdf` on the repo root to delete unnecessary temporary files
You need to follow [verify-and-commit.md](.github/Rules/verify-and-commit.md) to commit and push

# UPDATES

# TEST [CONFIRMED]

Existing generation and C++ compilation tests confirm this task:

- `CompilerTest_LoadAndCompile` should regenerate RPC C++ files using the new `Rpc`-prefixed file names.
- `Generated_CppRpc.vcxitems`, `Generated_ReflectionRpc.vcxitems`, and their filters should reference the new generated file names and should not keep obsolete duplicate object-file rename workarounds for RPC `Event.cpp` or `Overloading.cpp`.
- `TestCasesRpc.cpp` should include the new generated file names.
- Outdated generated RPC files should be deleted, and generated-file additions and removals should balance as expected for a rename.
- `CompilerTest_LoadAndCompile`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` should pass after regeneration.

# PROPOSALS

- No.1 Prefix generated RPC C++ file names with `Rpc` [CONFIRMED]

## No.1 Prefix generated RPC C++ file names with `Rpc`

### CODE CHANGE
- Changed the RPC compiler test input names in `TestRpcCompile.cpp` so generated include, reflection, and default C++ file names are prefixed with `Rpc`.
- Regenerated the RPC C++ outputs and `TestCasesRpc.cpp`, producing `Rpc*` generated files and removing the stale non-prefixed generated files.
- Updated `Generated_CppRpc.vcxitems` and `Generated_ReflectionRpc.vcxitems` to reference the prefixed RPC files, removed the obsolete `Event.cpp` and `Overloading.cpp` object-file rename workarounds, and added the import include path needed for clean RPC builds.
- Removed the obsolete RPC `Event.cpp` and `Overloading.cpp` exclusions from the Linux `CppTest*` vmake files.

### CONFIRMED
- Ran `CompilerTest_LoadAndCompile`; it regenerated the RPC outputs successfully.
- Built `UnitTest.sln` for Debug x64 and Debug Win32; both builds succeeded with 0 warnings and 0 errors.
- Ran Debug x64 `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`; all passed 2/2 test files and 227/227 test cases.
- Ran Debug Win32 `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`; all passed 2/2 test files and 227/227 test cases.
- Verified `Test\SourceCppGenRpc` has no stale non-prefixed generated files except `TestCasesRpc.cpp`, and its generated-file status is 496 deletes, 496 adds, and 1 modified `TestCasesRpc.cpp`.
- Verified there are no remaining `ObjectFileName`, `Rpc_Overloading`, `Rpc_Event`, or stale `SourceCppGenRpc\Event.cpp` / `SourceCppGenRpc\Overloading.cpp` references in the updated RPC project and Linux vmake files.
- Verified every `SourceCppGenRpc` path referenced by `Generated_CppRpc.vcxitems` and `Generated_ReflectionRpc.vcxitems` exists.
- Ran `git diff --check`; no whitespace errors were reported.
