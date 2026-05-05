# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- If you can't fix a runtime issue within a few rounds of guess-and-edit, you need to debug the process.
- I am a fan of crash early. When something should happen, it should just happen, do not play a game like "what if it is not the case" and silently covers the issue. One example is that, if an object should not be null, then we should just use it, if a nullable object should not be null, we should just cast it. No test is performed in this case, using it will crash if it is null, and we know there is a problem. Fix the actual problem instead of doing "error tolerance".
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Workflow Script Best Practice

- Avoid explicit type specification whenever possible:
  - Prefer `var v = e;` whenever `T` can be omitted.
  - Prefer `var v : T = e;` over `var v = e over T;` if `T` cannot avoid.
  - When implicit type conversion works at the place, avoid `cast`, `as` and `infer` expression.
  - Prefer `cast *` over `cast T` when the context accepts `T`.

## Task 1

In `CppTypes.(h|cpp)` there is already a reflectable `Point` struct. I would like you to update `Rpc/PrimitiveTypes.txt` to use it.
This is a test to ensure, even when RPC references types introduced by C++ it is still working.

# UPDATES

# TEST [CONFIRMED]

Update the existing `Test/Resources/Rpc/PrimitiveTypes.txt` RPC sample so `IService.ProcessStruct` uses the reflectable C++ struct `test::Point`, not a Workflow-defined `RpcPrimitiveTest::Point`.

Success criteria:
- `CompilerTest_LoadAndCompile` accepts the sample and regenerates RPC metadata/wrappers.
- The regenerated `Metadata_PrimitiveTypes.txt` records `ProcessStruct(value : ::test::Point) : (::test::Point)`.
- The `PrimitiveTypes` runtime RPC test still returns `[6][12][1.75][2.875][Hi!][false][Autumn][13,27]`.
- Required unit tests from `Project.md` and `.github\Rules\verify-and-commit.md` pass.

# PROPOSALS

- No.1 Replace the local Workflow Point with reflectable C++ test::Point [CONFIRMED]

## No.1 Replace the local Workflow Point with reflectable C++ test::Point

### CODE CHANGE

Updated `Test/Resources/Rpc/PrimitiveTypes.txt`:
- Removed the Workflow-defined `RpcPrimitiveTest::Point` struct.
- Changed `IService.ProcessStruct` and its service implementation to use `test::Point`.
- Returned the updated point literal directly so the sample follows the Workflow script best-practice preference for avoiding unnecessary explicit type specifications.

Regenerated compiler/RPC outputs with `CompilerTest_LoadAndCompile`, including:
- `Test/Generated/RpcMetadata32/Metadata_PrimitiveTypes.txt`
- `Test/Generated/RpcMetadata64/Metadata_PrimitiveTypes.txt`
- `Test/Generated/RpcMetadata32/Wrapper_PrimitiveTypes*.txt`
- `Test/Generated/RpcMetadata64/Wrapper_PrimitiveTypes*.txt`
- `Test/Generated/CppRpc32/PrimitiveTypes*`
- `Test/Generated/CppRpc64/PrimitiveTypes*`
- `Test/SourceCppGenRpc/PrimitiveTypes*`
- `Test/Generated/Workflow32/Assembly.Rpc.PrimitiveTypes.txt`
- `Test/Generated/Workflow32/Parsing.Rpc.PrimitiveTypes.txt`
- `Test/Generated/Workflow64/Assembly.Rpc.PrimitiveTypes.txt`
- `Test/Generated/Workflow64/Parsing.Rpc.PrimitiveTypes.txt`

### CONFIRMED

The regenerated RPC metadata now records `func ProcessStruct(value : ::test::Point) : (::test::Point);`.
Both 32-bit and 64-bit metadata include `namespace test { struct Point { ... } }`, proving that an RPC method can reference a reflectable type introduced by C++.

Verification:
- Debug x64 rebuild: passed.
- Debug Win32 build: passed.
- `LibraryTest` x64: passed 2/2 files, 14/14 cases.
- `LibraryTest` Win32: passed 2/2 files, 14/14 cases.
- `CompilerTest_GenerateMetadata` x64: passed 1/1 files, 2/2 cases.
- `CompilerTest_GenerateMetadata` Win32: passed 1/1 files, 2/2 cases.
- `CompilerTest_LoadAndCompile` x64: passed 6/6 files, 699/699 cases.
- Debug x64 rebuild after regeneration: passed.
- Debug Win32 rebuild after regeneration: passed.
- `RuntimeTest` x64: passed 4/4 files, 257/257 cases.
- `RuntimeTest` Win32: passed 4/4 files, 257/257 cases.
- `CppTest` x64: passed 2/2 files, 223/223 cases.
- `CppTest` Win32: passed 2/2 files, 223/223 cases.
- `CppTest_Metaonly` x64: passed 2/2 files, 223/223 cases.
- `CppTest_Metaonly` Win32: passed 2/2 files, 223/223 cases.
- `CppTest_Reflection` x64: passed 2/2 files, 223/223 cases.
- `CppTest_Reflection` Win32: passed 2/2 files, 223/223 cases.

The `PrimitiveTypes` RPC runtime output remains `[6][12][1.75][2.875][Hi!][false][Autumn][13,27]`.
