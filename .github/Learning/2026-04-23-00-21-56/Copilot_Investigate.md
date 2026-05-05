# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Follow `job.new-sample.md` to add a new `Rpc\Overloading.txt`.

- Preserve the sample meaning except if needed to compile.
- Verify that RPC metadata and generated wrapper ids handle overloaded interface methods.
- Generated names should become:
  - `RpcOverloadingTest::IService.ToStringInt`
  - `RpcOverloadingTest::IService.ToString(value).1`
  - `RpcOverloadingTest::IService.ToString(value).2`
  - `RpcOverloadingTest::IService.ToString(value).3`
  - `RpcOverloadingTest::IService.ToString(value1,value2,value3,value4)`
- Mangled constants should become:
  - `rpcmethod_RpcOverloadingTest__IService_ToStringInt`
  - `rpcmethod_RpcOverloadingTest__IService_ToString_value__1`
  - `rpcmethod_RpcOverloadingTest__IService_ToString_value__2`
  - `rpcmethod_RpcOverloadingTest__IService_ToString_value__3`
  - `rpcmethod_RpcOverloadingTest__IService_ToString_value1_value2_value3_value4_`
- `RpcGeneratedNameConflict` and `RpcMangledNameConflict` should still report real collisions.
- Do not change the Workflow parser or unrelated compiler/code-generation behavior.
- Pass all unit tests, then commit and push.

# UPDATES

## UPDATE

I kept the change localized to RPC metadata naming, RPC id mangling, reflection proxy emission for overloaded methods, and the test wiring needed to avoid assembly and object-file collisions with the existing non-RPC `Overloading` sample.

# TEST [CONFIRMED]

- Existing coverage is sufficient:
  - `CompilerTest_LoadAndCompile` compiles `Test/Resources/Rpc/Overloading.txt`, regenerates `Test/SourceCppGenRpc/Overloading.*`, regenerates `Test/Generated/RpcMetadata32/Wrapper_Overloading.txt` and `Test/Generated/RpcMetadata64/Wrapper_Overloading.txt`, and refreshes `TestCasesRpc.cpp`.
  - `RuntimeTest` compares the workflow runtime result of `Rpc/Overloading` against `Test/Resources/IndexRpc.txt`.
  - `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` compare the generated C++ result of `Rpc:Overloading` against the regenerated `TestCasesRpc.cpp` expectation.
  - Existing RPC metadata-conflict coverage continues to exercise `RpcGeneratedNameConflict` and `RpcMangledNameConflict`.
  - Because the user explicitly asked to pass all unit tests including pre-existing failures, the full unit-test executable matrix for this solution is also part of the acceptance criteria.
- Success criteria:
  - The new sample compiles and regenerates all expected generated artifacts.
  - `Test/Generated/RpcMetadata32/Wrapper_Overloading.txt` and `Test/Generated/RpcMetadata64/Wrapper_Overloading.txt` show `ToString(value).1`, `.2`, `.3`, and the corresponding mangled constant names.
  - `RuntimeTest` logs `Overloading=[123][true][abc][xyz][123,true,abc,xyz]`.
  - `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` produce the same result as `RuntimeTest`.
  - The full unit-test executable set passes in both `Debug|x64` and `Debug|Win32`: `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`.
- Confirmation:
  - `Debug|x64`: solution build succeeded, `CompilerTest_LoadAndCompile` ended with `Passed test files: 6/6` and `Passed test cases: 589/589`, `RuntimeTest` ended with `Passed test files: 4/4` and `Passed test cases: 146/146`, and `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` each ended with `Passed test files: 2/2` and `Passed test cases: 112/112`.
  - `Debug|Win32`: solution build succeeded, and the same executable matrix all passed with the same per-project file and case counts.
  - `Test/Generated/RpcMetadata32/Wrapper_Overloading.txt` and `Test/Generated/RpcMetadata64/Wrapper_Overloading.txt` both contain:
    - `RpcOverloadingTest::IService.ToString(value).1`
    - `RpcOverloadingTest::IService.ToString(value).2`
    - `RpcOverloadingTest::IService.ToString(value).3`
    - `rpcmethod_RpcOverloadingTest__IService_ToString_value__1`
    - `rpcmethod_RpcOverloadingTest__IService_ToString_value__2`
    - `rpcmethod_RpcOverloadingTest__IService_ToString_value__3`

# PROPOSALS

- No.1 Disambiguate RPC metadata names, mangled constants, reflection proxy lookup, and build outputs for overloaded methods [CONFIRMED]

## No.1 Disambiguate RPC metadata names, mangled constants, reflection proxy lookup, and build outputs for overloaded methods

### CODE CHANGE

- Added `Test/Resources/Rpc/Overloading.txt` as the new RPC sample, preserving the requested overload set and expected result string.
- Updated `Test/Resources/IndexRpc.txt` with `Overloading=[123][true][abc][xyz][123,true,abc,xyz]`.
- Added `Overloading.txt` to `Test/UnitTest/CompilerTest_LoadAndCompile/CompilerTest_LoadAndCompile.vcxproj` and `.filters` so the compile and regeneration test includes the new sample.
- Updated `Source/Analyzer/WfAnalyzer_ValidateRPC.cpp` so overload candidates are formatted as `Method(arg1,arg2,...)`, and only true duplicates receive numeric suffixes `.1`, `.2`, `.3`, preserving readable generated ids while still letting collision validation run.
- Updated `Source/Analyzer/WfAnalyzer_GenerateRpc.cpp` so RPC constant-name mangling now normalizes `.`, `(`, `)`, and `,` to `_`, matching the new overload-aware ids emitted into wrapper metadata.
- Updated `Source/Cpp/WfCpp_WriteReflection.cpp` so interface proxy code resolves overloaded interface members by exact method-pointer binary data when a method group contains more than one overload, while keeping the existing macro path for non-overloaded methods.
- Updated `Test/UnitTest/CompilerTest_LoadAndCompile/TestRpcCompile.cpp` so generated RPC C++ assemblies use an `Rpc_` prefix and the generated harness tracks the real per-item assembly name, preventing collisions with the existing non-RPC `Overloading` sample.
- Updated `Test/UnitTest/Generated_Cpp/Generated_Cpp.vcxitems`, `Test/UnitTest/Generated_CppRpc/Generated_CppRpc.vcxitems`, and `Test/UnitTest/Generated_ReflectionRpc/Generated_ReflectionRpc.vcxitems` with explicit object file names so `Overloading.cpp` and `Rpc_Overloading.cpp` do not overwrite each other in shared intermediate directories.
- Regenerated the new RPC outputs through `CompilerTest_LoadAndCompile`, which updated `Test/SourceCppGenRpc/TestCasesRpc.cpp`, produced the generated `Overloading` source and reflection files in `Test/SourceCppGenRpc`, and produced the generated dumps and metadata in `Test/Generated/CppRpc32`, `Test/Generated/CppRpc64`, `Test/Generated/RpcMetadata32`, `Test/Generated/RpcMetadata64`, `Test/Generated/Workflow32`, and `Test/Generated/Workflow64`.

### CONFIRMED

- The real failure was not in the Workflow parser. RPC metadata originally had no stable way to distinguish overloads, and the reflection proxy generator relied on name-only lookup macros that are ambiguous once overloads exist.
- Converting metadata ids to `Method(arg1,arg2,...)` keeps the generated string ids readable, and appending numeric suffixes only when the full candidate name still duplicates preserves the existing `RpcGeneratedNameConflict` and `RpcMangledNameConflict` validation instead of hiding real collisions.
- Extending mangling to treat overload punctuation the same way as `.` keeps the constant variables aligned with the new generated ids, which is exactly what `Wrapper_Overloading.txt` now shows.
- Overloaded interface proxies also need exact method selection when reflection is enabled. Resolving overloaded members by exact method pointer binary data fixes only that overloaded case and leaves the simpler macro-based path unchanged for single-method groups.
- The separate `Rpc_` assembly prefix plus explicit object names solved the build-system collision with the existing non-RPC `Overloading` sample without renaming either sample or changing user-facing output.
- Validation is clean across workflow runtime, generated C++, metaonly generated C++, and reflection-generated C++ in both architectures.
