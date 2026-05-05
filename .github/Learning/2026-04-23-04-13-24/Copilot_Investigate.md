# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Follow job.new-sample.md to add:
- Rpc\CollectionDict_InByval_OutByval.txt
- Rpc\CollectionDict_InByval_OutByref.txt
- Rpc\CollectionDict_InByref_OutByval.txt
- Rpc\CollectionDict_InByref_OutByref.txt

Just like Rpc\Collection_*.txt but all `int[]` will be replaced with `string[int]`, and the main namespace `RpcCollection` becomes `RpcCollectionDict`.
The original Copilot_Investigate.md has the log of 4 Rpc\Collection_*.txt cases you just done.
They tested against list, now we are doing dictionaries, good news is that most challenges should have already been solved during testing against lists.
So keep new test cases identical to old test cases, only changing from list to dictionary, as well as some related data.

Treat list elements as keys, and treat values as key-th letters.
Codes like `xs.Add(4)` becomes `xs.Add(4, "D")`.
An original list printing "123" becomes "1A2B3C".
All list items are literals, not dynamically constructed, so it should be easy to do.
No fancy trick is needed in these samples written in Workflow.

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
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.

# UPDATES

- Added four new RPC dictionary samples in `Test/Resources/Rpc`:
  - `CollectionDict_InByval_OutByval.txt`
  - `CollectionDict_InByval_OutByref.txt`
  - `CollectionDict_InByref_OutByval.txt`
  - `CollectionDict_InByref_OutByref.txt`
- Activated the four dictionary cases in `Test/Resources/IndexRpc.txt` with expected values:
  - `CollectionDict_InByval_OutByval=[1A2B3C][1A2B3C4D][1A2B3C4D5E]`
  - `CollectionDict_InByval_OutByref=[1A2B3C][1A2B3C4D5E][1A2B3C4D5E]`
  - `CollectionDict_InByref_OutByval=[1A2B3C4D][1A2B3C4D][1A2B3C4D5E]`
  - `CollectionDict_InByref_OutByref=[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]`
- Added the new sample files to `Test/UnitTest/CompilerTest_LoadAndCompile/CompilerTest_LoadAndCompile.vcxproj` and `.filters`.
- Added the generated dictionary RPC source entries to:
  - `Test/UnitTest/Generated_CppRpc/Generated_CppRpc.vcxitems`
  - `Test/UnitTest/Generated_ReflectionRpc/Generated_ReflectionRpc.vcxitems`
- Regenerated the corresponding outputs in:
  - `Test/Generated/CppRpc32`
  - `Test/Generated/CppRpc64`
  - `Test/Generated/RpcMetadata32`
  - `Test/Generated/RpcMetadata64`
  - `Test/Generated/Workflow32`
  - `Test/Generated/Workflow64`
  - `Test/SourceCppGenRpc`

# TEST [CONFIRMED]

- Criteria:
  - All four new dictionary RPC samples must compile in `CompilerTest_LoadAndCompile`.
  - `RuntimeTest` must produce the expected values for all four dictionary cases on Win32 and x64.
  - `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` must also produce the expected values for all four dictionary cases on Win32 and x64.
  - The required repo-wide `Tools\Tools\Build.ps1 Workflow` wrapper must complete without surfacing a failure.
- Rebuilt `Test/UnitTest/UnitTest.sln`:
  - `Debug|Win32`: `Build succeeded. 0 Warning(s). 0 Error(s).`
  - `Debug|x64`: `Build succeeded. 0 Warning(s). 0 Error(s).`
- Ran `LibraryTest`:
  - `Win32`: `Passed test files: 2/2`, `Passed test cases: 14/14`
  - `x64`: `Passed test files: 2/2`, `Passed test cases: 14/14`
- Ran `CompilerTest_GenerateMetadata`:
  - `Win32`: `Passed test files: 1/1`, `Passed test cases: 2/2`
  - `x64`: `Passed test files: 1/1`, `Passed test cases: 2/2`
- Ran `CompilerTest_LoadAndCompile` on `x64`:
  - `Passed test files: 6/6`, `Passed test cases: 597/597`
  - Confirmed the generated merge includes all four new dictionary RPC cases.
- Ran `RuntimeTest`:
  - `Win32`: `Passed test files: 4/4`, `Passed test cases: 154/154`
  - `x64`: `Passed test files: 4/4`, `Passed test cases: 154/154`
- Verified new runtime RPC expectations in both architectures:
  - `CollectionDict_InByval_OutByval` => `[1A2B3C][1A2B3C4D][1A2B3C4D5E]`
  - `CollectionDict_InByval_OutByref` => `[1A2B3C][1A2B3C4D5E][1A2B3C4D5E]`
  - `CollectionDict_InByref_OutByval` => `[1A2B3C4D][1A2B3C4D][1A2B3C4D5E]`
  - `CollectionDict_InByref_OutByref` => `[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]`
- Ran `CppTest`:
  - `Win32`: `Passed test files: 2/2`, `Passed test cases: 120/120`
  - `x64`: `Passed test files: 2/2`, `Passed test cases: 120/120`
- Ran `CppTest_Metaonly`:
  - `Win32`: `Passed test files: 2/2`, `Passed test cases: 120/120`
  - `x64`: `Passed test files: 2/2`, `Passed test cases: 120/120`
- Ran `CppTest_Reflection`:
  - `Win32`: `Passed test files: 2/2`, `Passed test cases: 120/120`
  - `x64`: `Passed test files: 2/2`, `Passed test cases: 120/120`
- Ran `C:\Code\VczhLibraries\Tools\Tools\Build.ps1 Workflow`:
  - The wrapper rebuilt and executed the required release Workflow pipeline, including the release unit test projects and release packaging steps, and it returned without surfacing a failure.

# PROPOSALS

- No.1 Add dictionary samples and generated RPC wiring [CONFIRMED]

## No.1 Add dictionary samples and generated RPC wiring

### CODE CHANGE

- Added the four new dictionary RPC samples in `Test/Resources/Rpc`.
- Added expected outputs to `Test/Resources/IndexRpc.txt`.
- Added the new sample files to `CompilerTest_LoadAndCompile.vcxproj` and `.filters`.
- Regenerated and included the new C++ RPC outputs in `Generated_CppRpc.vcxitems` and `Generated_ReflectionRpc.vcxitems`.
- Regenerated the related files under `Test/Generated/*` and `Test/SourceCppGenRpc`.

### CONFIRMED

The new dictionary cases compiled and passed in `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` on both Win32 and x64 without any additional changes to `RpcBoxByval`, `RpcBoxByref`, `RpcUnboxByval`, `RpcUnboxByref`, or `RpcDualLifecycleMock`. This confirms that the earlier list-stage fixes already generalized to strong dictionary containers, and the remaining work for this task was limited to adding the new samples, wiring them into the test projects, and committing the generated outputs.
