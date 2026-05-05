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

# TEST

- Stage 1 confirmation:
  - `CompilerTest_LoadAndCompile` `Debug|x64` must compile all four edited `CollectionDict_*` samples.
  - `RuntimeTest` `Debug|x64` must keep the expected outputs for all four non-nested dictionary cases.
- Stage 2 final confirmation:
  - Add all eight nested collection RPC samples and wire them into the compile, runtime, and generated C++ projects.
  - Run the required unit test flow for the changed source and generated outputs.
  - If failures appear, check `RpcBoxByval`, `RpcBoxByref`, `RpcUnboxByval`, `RpcUnboxByref`, and `RpcDualLifecycleMock` first.
- Current results:
  - `CompilerTest_LoadAndCompile` `Debug|x64`: `Passed test files: 6/6`, `Passed test cases: 597/597`.
  - `RuntimeTest` `Debug|x64`: `Passed test files: 4/4`, `Passed test cases: 154/154`.
  - Verified runtime outputs:
    - `CollectionDict_InByval_OutByval` => `[1A2B3C][1A2B3C4D][1A2B3C4D5E]`
    - `CollectionDict_InByval_OutByref` => `[1A2B3C][1A2B3C4D5E][1A2B3C4D5E]`
    - `CollectionDict_InByref_OutByval` => `[1A2B3C4D][1A2B3C4D][1A2B3C4D5E]`
    - `CollectionDict_InByref_OutByref` => `[1A2B3C4D5E][1A2B3C4D5E][1A2B3C4D5E]`

# PROPOSALS

- No.1 Replace fixed-width dictionary print helpers [CONFIRMED]

## No.1 Replace fixed-width dictionary print helpers

### CODE CHANGE

- Updated the four existing `Test\Resources\Rpc\CollectionDict_*` samples to use a single `Print` helper.
- The new helper builds the output string by iterating `key` from `1` to `xs.Count` and appending `$(key)$(xs[key])` for each entry.
- The call sites in `clientMain` now use the same helper for origin, service, and client snapshots.

### CONFIRMED

The stage-1 failure was entirely inside the sample scripts. After replacing the fixed-width dictionary print helpers with a single count-driven `Print` function, `CompilerTest_LoadAndCompile` and `RuntimeTest` both passed on `Debug|x64`, and the expected dictionary RPC outputs stayed unchanged. No runtime lifecycle or RPC boxing changes were required for this slice.