# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Currently `Codegen` category test cases generates C++ source code to `Test\SourceCppGen`, the `TestCases.cpp` is added to `CppTest*` projects directly, test cases are added to `Generated_(Cpp|Reflection)`.

I would like to do the same thing for `Rpc` category test cases. Generate everything to `Test\SourceCppGenRpc`, with test cases and `TestCasesRpc.cpp`. Add them to test projects using the same pattern.

Running rpc test cases is more complicated. Need to checkout how `RuntimeTest` project's `TestRuntime.cpp` runs `Codegen` and how `TestRpc.cpp` runs `Rpc`, then copy the same logic in `TestCasesRpc.cpp` in the same pattern like `TestCases.cpp`.

To test the project, after `CompilerTest_LoadAndCompile` rebuild the solution because generated C++ will change, before running `CppTest*`. These 3 test projects compile same source files using 3 different reflection compiler options (VCZH_DEBUG_NO_REFLECTION, VCZH_DEBUG_METAONLY_REFLECTION and nothing), to verify if generated C++ code satisfies any reflection options.

The main work is to:
1. Modify `TestRpcCompile.cpp` to generate C++ code and `TestCasesRpc.cpp` (like `TestCodegen.cpp` does for Codegen)
2. Add `GetCppMergePathRpc()` helper function for `SourceCppGenRpc` path
3. Create `Generated_CppRpc` and `Generated_ReflectionRpc` vcxitems projects
4. Add those vcxitems + `TestCasesRpc.cpp` to all 3 CppTest* projects
5. The `TestCasesRpc.cpp` needs the RPC dual lifecycle mock pattern from `TestRpc.cpp`

# UPDATES

## UPDATE

After `CompilerTest_LoadAndCompile` succeeds and generates files to `SourceCppGenRpc/`, building the full solution reveals three C++ code generator bugs in the generated `RequestService.cpp` and `RequestService.h`:

1. **SyncIds signature mismatch**: `IRpcIdSync::SyncIds` in C++ takes `const Dictionary<WString, vint>&`, but the code generator produces `Ptr<IValueReadonlyDictionary>` (the Workflow VM signature). This causes `cannot instantiate abstract class` and `did not override any base class methods` errors.

2. **Constructor parameter naming**: In constructor bodies, the generated code references `lc` instead of the mangled parameter name `__vwsnctor_lc`, causing `'lc': undeclared identifier` errors.

3. **TestCasesRpc.cpp missing namespace**: The generated `TestCasesRpc.cpp` was missing `using namespace vl::collections;`, causing `Dictionary` to be unresolved. This was fixed in `TestRpcCompile.cpp`.

## UPDATE

But the `this->` in C++ codegen can keep. Revert all other changes to C++ codegen part (`Source/Cpp/`), use other way to finish the work.

# TEST

N/A

# PROPOSALS

- No.1 Add C++ code generation to TestRpcCompile.cpp and create supporting infrastructure [CONFIRMED]
- No.2 Fix directory creation for both x86 and x64 output paths [CONFIRMED]
- No.3 Fix C++ code generator bugs for RPC interfaces [DENIED]
- No.4 Change IRpcIdSync::SyncIds to use boxed type Ptr<IValueReadonlyDictionary>

## No.1 Add C++ code generation to TestRpcCompile.cpp and create supporting infrastructure

### CODE CHANGE

**Test/Source/Helper.h and Helper.cpp**: Added `GetCppOutputPathRpc()`, `GetCppOutputPath32Rpc()`, `GetCppOutputPath64Rpc()`, `GetCppMergePathRpc()` path functions.

**Test/Source/RpcDualLifecycleMock.h and .cpp**: Added `wrapperFactories` dictionary and `RegisterWrapperFactory` method. Updated `CreateCallerProxy` and `RequestService` to check `wrapperFactories` before falling back to `LoadFunction`.

**Test/UnitTest/CompilerTest_LoadAndCompile/TestRpcCompile.cpp**: Heavy modifications to:
- Capture `rpcTypeFullNamesPerItem` during compilation
- Generate C++ code via `GenerateCppFiles()` to `GetCppOutputPathRpc()`
- Generate `TestCasesRpc.cpp` with RPC dual lifecycle mock pattern and wrapper factory registration

**Test/UnitTest/CompilerTest_LoadAndCompile/Main.cpp**: Added RPC output directory creation/cleanup and RPC merge step.

**Test/UnitTest/Generated_CppRpc/Generated_CppRpc.vcxitems**: New shared items project referencing `RequestService.cpp` and `RequestService.h`.

**Test/UnitTest/Generated_ReflectionRpc/Generated_ReflectionRpc.vcxitems**: New shared items project referencing `RequestServiceReflection.cpp` and `RequestServiceReflection.h`.

**Test/UnitTest/UnitTest.sln**: Added both vcxitems projects with NestedProjects and SharedMSBuildProjectFiles entries.

**Test/UnitTest/CppTest/CppTest.vcxproj, CppTest_Metaonly/CppTest_Metaonly.vcxproj, CppTest_Reflection/CppTest_Reflection.vcxproj**: Added imports for both Rpc vcxitems and `TestCasesRpc.cpp`.

**Test/UnitTest/CppTest_Reflection/Main.cpp**: Added `LoadTestCaseRpcTypes()` extern and call in `LoadTypes()`.

### CONFIRMED

CompilerTest_LoadAndCompile passes all 580 test cases (6/6 files). C++ code is generated to `CppRpc32/` and `CppRpc64/`, merged to `SourceCppGenRpc/`. TestCasesRpc.cpp is generated with correct RPC dual lifecycle mock pattern.

## No.2 Fix directory creation for both x86 and x64 output paths

### CODE CHANGE

**Test/UnitTest/CompilerTest_LoadAndCompile/Main.cpp**: Changed directory creation to explicitly create both `CppRpc32` and `CppRpc64` (and `Cpp32`/`Cpp64`) instead of relying on `GetCppOutputPathRpc()` which depends on runtime `testCpuArchitecture`. Used a lambda `createOrCleanFolder` with separate string literal messages for each CHECK_ERROR call.

### CONFIRMED

The x86 pass was failing because `GetCppOutputPathRpc()` resolved to `CppRpc32\` during x86 test run, but only `CppRpc64\` was created at startup (since `testCpuArchitecture` defaults to `AsExecutable` = x64 on 64-bit build). Creating both directories at startup fixes the issue. All 580 test cases pass on both x86 and x64 passes.

## No.3 Fix C++ code generator bugs for RPC interfaces

The Workflow C++ code generator in `Source/Cpp/` has bugs when generating code for RPC-related interfaces:

1. **SyncIds signature mismatch** (in `RequestService.h` lines 89, 102): The generated class `__vwsnc2_RequestService_rpc_IRpcObjectOps__vl_rpc_controller_IRpcObjectOps` overrides `SyncIds(Ptr<IValueReadonlyDictionary> ids)` but the base class `IRpcIdSync` declares `SyncIds(const Dictionary<WString, vint>& ids)`. The code generator emits the Workflow VM-level type signature instead of the native C++ type signature.

2. **Constructor parameter naming** (in `RequestService.cpp` lines 115, 171, 190): Constructor bodies reference `lc` but the parameter is named `__vwsnctor_lc`. The code generator fails to use the mangled parameter name in the constructor body.

3. **Abstract class instantiation** (in `RequestService.cpp` lines 75, 80): Consequence of bug #1. Since `SyncIds` override doesn't match, the class remains abstract and `Ptr(new ...)` fails.

These bugs cause all 3 CppTest* projects to fail compilation (119 errors total). The generated `RequestServiceReflection.cpp/.h` files are correct (properly guarded with `#ifndef VCZH_DEBUG_NO_REFLECTION`).

### CODE CHANGE

(Not yet attempted - requires changes to `Source/Cpp/` code generator files)

### DENIED BY USER

The user explicitly instructed to revert all changes to the C++ codegen part (`Source/Cpp/`). The `this->` fix in `WfCpp_Expression.cpp` (constructor parameter naming fix for `WfFunctionArgument`) can stay, but all other codegen changes must be reverted. The SyncIds signature mismatch must be solved through other means, not by modifying the C++ code generator.

## No.4 Change IRpcIdSync::SyncIds to use boxed type Ptr<IValueReadonlyDictionary>

The C++ code generator always produces reflection-level types for collection parameters (e.g., `Ptr<IValueReadonlyDictionary>` instead of `const Dictionary<WString, vint>&`). Instead of fixing the codegen, change the `IRpcIdSync::SyncIds` interface in `Source/Library/WfLibraryRpc.h` to use `Ptr<IValueReadonlyDictionary>` as the parameter type. This way the generated C++ code naturally matches the base class signature.

The `SyncIds` method is deliberately not called through Workflow proxies (the proxy implementation does `CHECK_FAIL`), and `RpcDualLifecycleMock::Register` already skips calling `SyncIds` on Workflow proxy objects. The only caller that passes a real dictionary is `RpcLifecycleMock::Register`, which needs to be updated to box the dictionary first.

Files to change:
1. `Source/Library/WfLibraryRpc.h` - Change `SyncIds` parameter from `const Dictionary<WString, vint>&` to `Ptr<IValueReadonlyDictionary>`
2. `Source/Library/WfLibraryReflection.h` - Update proxy implementations
3. `Test/Source/RpcLifecycleMock.h` and `.cpp` - Update override signature and boxing in Register
4. `Test/UnitTest/CompilerTest_LoadAndCompile/TestRpcCompile.cpp` - Fix IDescriptable namespace qualification and add `#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA` guard for TestCasesRpc.cpp content
5. CppTest_Reflection project - Ensure RpcDualLifecycleMock.cpp is linked (fix LNK2019)

### CODE CHANGE

(To be implemented)
