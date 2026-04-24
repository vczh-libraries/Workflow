# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Follow job.new-sample.md to:
- fix Rpc\CollectionDist_*.txt
- add Rpc\Collection(Dict)?_Nested_InBy(val|ref)_OutBy(val|ref).txt

Firstly fix the non nested version of all 4 CollectionDict_*.
You will find there are Print3/Print4/Print5, you can make it into one single Print, by:
- First check the Count property.
- Secondly use key from 1 to Count to get the value.
- Thirdly rewrite it as how Collection_* implementa the Print function, using a for each loop, but you can say
  - `for (var key in range [1..xs.Count])`

After finishing the fix, git commit and git push to the current branch.
DO NOT ASK ME ANY QUESTION

Secondly, write nested version of all existing 8 collection samples:
- int[] becomes int[][] or (int[])[], use the second one if the first one doesn't compile.
- string[int] becomes string[int] or (string[int])[int].
- Do everything that was done as non nested samples, but:
  - In nested version, xs[0] will be the container to check.
  - in clientMain, the container must be initialized using:
    - {{1 2 3}} or {{1 2 3} as int[]}
    - {{...}} or {{...} as string[int]}
    - pick the shorter one if possible.
  - when test again object equality or if it implements the wrapper interface, check the outer container
  - when adding extra element or printing, check the jnner container.

This test is to ensure that, when byval or byref is applied to a nested container, all levels of container applies.

Processing containers are a little bit more complex comparing to interfaces.
When byref is specified, an wrapper will be created to connect to the original container.
When byval is specified, a copy will be created.
Therefore according to Byref or Byval, the object retrieved from lifecycle may be an IRpcWrapperBase^ or may not, unlike interfaces all remote objects are wrappers.

Understand what the test case trying to say, you are not allowed to change:
- The content of the sample, unless it doesn't build.
- Workflow parser.
- Workflow compiling.
- Workflow to C++ code generation.

`Rpc(B|Unb)oxBy(val|ref)` should be in the highest priority attemp to fix, as these 4 C++ functions are directly called in generated wrapper classes written Workflow script.

You are highly possibly need to fix implementation of `RpcDualLifecycleMock` and its connected interfaces if sample fails in either `RuntimeTest` or `CppTest*`.
- The comment in the sample describes how `RpcDualLifecycleMock` and the generated C++ code is supposed to work.
  - The generated C++ code is very straight forward, if it fails, check `RpcDualLifecycleMock` first.
- If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
- Pass all unit test, fix any test failure including pre-existings.
- After finishing everything, git commit and git push to the current branch.
  - Two commits are required. First commit only has all modified files and files you created directly, second commit has all new files that not created by you (aka auto generated)
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.

So in the whole work you will make 3 commits and push twice. Do the second task after pushing the first one.

# UPDATES

## UPDATE

- Stage 1 scope is limited to the four existing non-nested dictionary RPC samples.
- Replaced the fixed-width `Print3` / `Print4` / `Print5` helpers in all four `Rpc\CollectionDict_*` samples with a single `Print` helper that iterates keys from `1` to `xs.Count`.
- The literal workflow syntax from the request, `for (var key in range [1..xs.Count])`, does not parse in this codebase. The working equivalent is `for (key in range [1, xs.Count])`, which preserves the intended inclusive `1..Count` behavior.
- Confirmed the updated non-nested samples compile and still produce the same runtime outputs.
- Next step after the first push: add the eight nested collection RPC samples, then run the full required test matrix and fix any lifecycle or boxing regressions.

## UPDATE

- Added the eight nested RPC samples under `Test\Resources\Rpc` for list and dictionary inputs across all four `InBy(val|ref)_OutBy(val|ref)` combinations.
- Updated `Test\Resources\IndexRpc.txt` with the expected outputs for all eight nested samples.
- Wired the nested samples into `CompilerTest_LoadAndCompile.vcxproj` and `.filters` so they are compiled and generated with the existing RPC sample set.
- `CompilerTest_LoadAndCompile` `Debug|x64` then passed with the nested samples included, confirming the sample syntax and expected outputs were valid.
- The first runtime regression appeared in `RuntimeTest` `Debug|x64` at `Collection_Nested_InByval_OutByref`, where nested byref collection access failed with: `Argument "thisObject" cannot convert from "Ptr<system::IRpcWrapperBase>" to "system::Enumerator*".`
- A first attempt changed `BoxRpcObject` to attach explicit interface descriptors when boxing collection wrappers. That did not fix the failure after a real rebuild because `Value::GetTypeDescriptor()` for `RawPtr` and `SharedPtr` still reports the underlying object's own reflected descriptor.
- The actual fix was to make the concrete RPC byref collection wrappers themselves reflected classes:
  - add `Description<...>` inheritance to the `RpcByref*` wrapper classes in `Source\Library\WfLibraryRpc.h`
  - add the wrapper classes to `WORKFLOW_LIBRARY_TYPES` in `Source\Library\WfLibraryReflection.h`
  - register type info and collection-interface base classes for those wrappers in `Source\Library\WfLibraryReflection.cpp`
- That change makes nested byref wrapper objects surface as collection wrapper types instead of only `system::IRpcWrapperBase`, which restores nested element access, iteration, and conversion.
- Updated `Generated_CppRpc.vcxitems` and `Generated_ReflectionRpc.vcxitems` to include the newly generated nested RPC files so `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` link correctly.
- Removed the earlier experimental `BoxRpcObject` boxing tweak after confirming it was not causal, keeping the final patch set limited to the reflected-wrapper fix and sample/project wiring.

# TEST

- Stage 1 confirmation:
  - `CompilerTest_LoadAndCompile` `Debug|x64` must compile all four edited `CollectionDict_*` samples.
  - `RuntimeTest` `Debug|x64` must keep the expected outputs for all four non-nested dictionary cases.
- Stage 2 final confirmation:
  - Add all eight nested collection RPC samples and wire them into the compile, runtime, and generated C++ projects.
  - Run the required unit test flow for the changed source and generated outputs.
  - If failures appear, check `RpcBoxByval`, `RpcBoxByref`, `RpcUnboxByval`, `RpcUnboxByref`, and `RpcDualLifecycleMock` first.
- Current results:
  - Stage 1 completion before the first push:
    - `CompilerTest_LoadAndCompile` `Debug|x64`: `Passed test files: 6/6`, `Passed test cases: 597/597`.
    - `RuntimeTest` `Debug|x64`: `Passed test files: 4/4`, `Passed test cases: 154/154`.
  - Stage 2 final validation:
    - `CompilerTest_LoadAndCompile` `Debug|x64`: `Passed test files: 6/6`, `Passed test cases: 605/605`.
    - `UnitTest.sln` build `Debug|x64`: `Build succeeded`, `0 Error(s)`.
    - `UnitTest.sln` build `Debug|Win32`: `Build succeeded`, `0 Error(s)`.
    - `RuntimeTest` `Debug|x64`: `Passed test files: 4/4`, `Passed test cases: 162/162`.
    - `RuntimeTest` `Debug|Win32`: `Passed test files: 4/4`, `Passed test cases: 162/162`.
    - `CppTest` `Debug|x64`: `Passed test files: 2/2`, `Passed test cases: 128/128`.
    - `CppTest` `Debug|Win32`: `Passed test files: 2/2`, `Passed test cases: 128/128`.
    - `CppTest_Metaonly` `Debug|x64`: `Passed test files: 2/2`, `Passed test cases: 128/128`.
    - `CppTest_Metaonly` `Debug|Win32`: `Passed test files: 2/2`, `Passed test cases: 128/128`.
    - `CppTest_Reflection` `Debug|x64`: `Passed test files: 2/2`, `Passed test cases: 128/128`.
    - `CppTest_Reflection` `Debug|Win32`: `Passed test files: 2/2`, `Passed test cases: 128/128`.
    - All eight nested collection and dictionary RPC cases matched the expected outputs in both `RuntimeTest` and `CppTest*`.

# PROPOSALS

- No.1 Replace fixed-width dictionary print helpers [CONFIRMED]

## No.1 Replace fixed-width dictionary print helpers

### CODE CHANGE

- Updated the four existing `Test\Resources\Rpc\CollectionDict_*` samples to use a single `Print` helper.
- The new helper builds the output string by iterating `key` from `1` to `xs.Count` and appending `$(key)$(xs[key])` for each entry.
- The call sites in `clientMain` now use the same helper for origin, service, and client snapshots.

### CONFIRMED

The stage-1 failure was entirely inside the sample scripts. After replacing the fixed-width dictionary print helpers with a single count-driven `Print` function, `CompilerTest_LoadAndCompile` and `RuntimeTest` both passed on `Debug|x64`, and the expected dictionary RPC outputs stayed unchanged. No runtime lifecycle or RPC boxing changes were required for this slice.

- No.2 Force interface descriptors during RPC boxing [REJECTED]

## No.2 Force interface descriptors during RPC boxing

### CODE CHANGE

- A temporary experiment changed `BoxRpcObject` in `Source\Library\WfLibraryRpc.cpp` to box collection wrappers through explicit interface descriptors.
- The intent was to make nested byref wrappers appear as `IValueList` / `IValueDictionary` instead of `IRpcWrapperBase` when they re-entered Workflow runtime code.

### REJECTED

This did not survive validation. For reflected pointer values in this codebase, `Value::GetTypeDescriptor()` ignores the explicit descriptor passed to `BoxValue` and reports the underlying object's own reflected type. After a real rebuild, `RuntimeTest` still failed in the same place. The experiment was removed from the final patch set.

- No.3 Reflect concrete byref collection wrappers [CONFIRMED]

## No.3 Reflect concrete byref collection wrappers

### CODE CHANGE

- Added `Description<...>` inheritance to the `RpcByrefEnumerator`, `RpcByrefEnumerable`, `RpcByrefReadonlyList`, `RpcByrefList`, `RpcByrefArray`, `RpcByrefObservableList`, and `RpcByrefDictionary` classes in `Source\Library\WfLibraryRpc.h`.
- Registered those wrapper classes in `Source\Library\WfLibraryReflection.h` and `Source\Library\WfLibraryReflection.cpp`, including their collection-interface base classes and `IRpcWrapperBase` where applicable.
- Updated the shared RPC generated-project item lists so the newly generated nested RPC sources are compiled and linked in all `CppTest*` variants.

### CONFIRMED

The nested runtime failure was caused by concrete byref collection wrapper instances only surfacing as `system::IRpcWrapperBase` at runtime. Once the wrapper classes themselves became reflected collection types, nested values such as `xs[0]` converted and iterated correctly. The final validation matrix passed on both `Debug|x64` and `Debug|Win32` for `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`.