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

Refactor `TestRpcCompile.cpp`.
- `MangleRpcFullName` is already in `WfAnalyzer_GenerateRpc.cpp`, `TestRpcCompile.cpp` should not have a copy, it should just use that function directly.
  - The same to `FindRpcTypeDescriptor`.
- Reorganize test case in `TEST_CATEGORY(L"RPC compilation")` into multiple functions.
  - Currently the test case do the following thing:
    1. manager.Clear + compile sample + `GenerateModuleRpc` + `GenerateModuleRpcJson`
    2. save logs.
    3. compile sample with wrappers.
    4. save logs.
  - I would like you to:
    - Extract step 1 to `CompileRpcSample`, `wrapperModule` and `wrapperJsonModule` could be lrvalue parameters.
    - Extra step 3 to `LinkRpcSample`.
    - Assertion against `manager` members should be include in these functions
- Some RPC samples are not added to `CompilerTest_LoadAndCompile`'s `Resource Files/Rpc` solution explorer folder. All should be there, but it won't affect compiling.

If you implement it correctly, you will see `CompileTest_LoadAndCompile` do not touch any generated files.
If anything is affected, you should investigate why and fix it.
If anything is not affected, your verification could just stop at `CompileTest_LoadAndCompile` cause all following test projects should be fine.

## Task 2

I would like you to add `@rpc:IdString(string)`, and `@rpc:IdNumber(int)`.
- They are not supposed to be used by users, but they should appear in `WfLexicalScopeManager::rpcMetadata->metadataModule`.
  - If these two attributes appear at the input, just ignore it. Ensure those will not be brought into `WfLexicalScopeManager::rpcMetadata->metadataModule`.
    - You can focus on clearning interfaces, methods, events. If they are attached to other members, don't care.
  - You are going to add `@rpc:IdString(string)`, and `@rpc:IdNumber(int)` to `WfLexicalScopeManager::rpcMetadata->metadataModule`:
    - They are used on each interface, method, event.
    - They specify generated text id and number id for them.
    - Add `SortedList<WString> orderedIds;` to `WfRpcMetadata`:
      - Running `ValidateModuleRPC` should eventually fill `orderedIds`.
      - In generated `Wrapper_*.txt`, there are a lot of ID variables and `rpc_GetIds` generated in this file. Now generation of them should use `orderedIds`. It is OK that in generated files those numbers are changed, or generated `IRpcObject(Event)Ops` handles ids in different order.
    - The generated `metadataModule` should leverage `orderedIds` to attach `@rpc:IdString(string)`, and `@rpc:IdNumber(int)` to interfaces, methods, events.
- You should update `TODO_RPC_Definition.md` to mention these new attributes.

This might affect generated files so full test should run.

## Task 3

In `WfAnalyzer_ValidateRpc.cpp` add a new function `CopyAndClearRpcMetadata`, accepting `Ptr<WfModule>`, returning `Ptr<WfModule>`.
- In `TestRpcCompile.cpp` add `VerifyRpcMetadata`, call it right after `CompileRpcSample`.
- It will do
  - Save `WfLexicalScopeManager::rpcMetadata->metadataModule` to a variable.
  - manager.Clear
  - Call `CopyAndClearRpcMetadata` on that variable.
  - Compile.
  - Compare the old ``WfLexicalScopeManager::rpcMetadata->metadataModule` with the new `WfLexicalScopeManager::rpcMetadata->metadataModule`.
    - You can just assert there `WfPrint` result should be identical.
    - The test is to ensure that, compile against the same RPC definition multiple times won't affect `orderedIds`.
- `CopyAndClearRpcMetadata` should obviously returned a copy of the input:
  - Copying is already implemented, reuse that function.
  - You should also remove existing types from the output module.
    - For example, in `Rpc/PrimitiveTypes.txt`, there is `test::Point` This type is referenced so it is there. But it is actually defined in `CppTypes.h`, so if you just compile the metadata it will fail by duplicating a struct.
    - So you should recursively walk through the module using visitors, making full names during walking, and try to call `GetTypeDescriptor` on that name. If it returns nullptr, it means the type does not exist in the environment, and you can keep that declaration.

This might affect generated files so full test should run.

## Task 4

You are going to split each `Rpc/SAMPLE.txt` into two files:
- `Rpc/SAMPLE.txt`: containing only RPC definitions.
- `Rpc/SAMPLE_Test.txt`: containing the rest.
All of the should be in `CompilerTest_LoadAndCompile`'s `Resource Files/Rpc` solution explorer folder.
Now in `CompileRpcSample` you only load the definiton. But in `LinkRpcSample` you will also need to load the `*_Test.txt`.

Scan
- `TODO_RPC_Sample.md` which is a template of task instructions.
- `Project.md` which explains how unit test works.
- `new-sample.md` and `new-sample-rpc.md` are instructions guiding you how to update samples.
And write the new knowledge about `*_Test.txt` in it.

This might affect generated files so full test should run.

## Task 5

This is a verification task, make sure all mentiond documents:
- `TODO_RPC_Definition.md`
- `TODO_RPC_Sample.md`
- `Project.md`
- `new-sample.md` and `new-sample-rpc.md`
Are property changed. If they are not changed, you should review the original instruction `TODO_RPC_Tasks.md` with the implementation (you might want to just scan the file or read git commits), and fix them.

# UPDATES

# TEST [CONFIRMED]

The requested tasks are implementation and verification tasks against existing compiler/RPC tests.

Success criteria:
- Each task is completed in order.
- For Task 1, `CompilerTest_LoadAndCompile` passes and does not modify generated files.
- For Tasks 2 through 4, generated files are updated where expected and the full test flow required by `Project.md` and `.github\Rules\verify-and-commit.md` passes.
- Task 5 verifies the listed documentation files against the implemented behavior.

# PROPOSALS

- No.1 Implement the requested RPC compiler, metadata, sample, and documentation changes task by task

## No.1 Implement the requested RPC compiler, metadata, sample, and documentation changes task by task

### CODE CHANGE

Task 1:
- Removed the duplicate local `MangleRpcFullName` and `FindRpcTypeDescriptor` implementations from `Test/UnitTest/CompilerTest_LoadAndCompile/TestRpcCompile.cpp`.
- Declared the existing `rpc_generating::MangleRpcFullName` and `rpc_generating::FindRpcTypeDescriptor` helpers in `Source/Analyzer/WfAnalyzer.h` and used them directly from the RPC compile test.
- Extracted RPC definition compilation to `CompileRpcSample`, including manager clearing, parsing, rebuild, RPC metadata assertions, and wrapper generation assertions.
- Extracted wrapper linking to `LinkRpcSample`, including manager clearing, parsing, wrapper module linking, rebuild, parse logging, and manager error assertions.
- Updated `CompilerTest_LoadAndCompile.vcxproj` and `.filters` so every current `Test/Resources/Rpc/*.txt` sample appears under `Resource Files\Rpc`.

### TASK 1 CONFIRMED

Task 1 verification:
- Debug x64 solution build passed with 0 warnings and 0 errors.
- `CompilerTest_LoadAndCompile` passed both architecture passes, with 6/6 test files and 699/699 test cases each pass.
- Running `CompilerTest_LoadAndCompile` did not modify any generated files; `git status` only showed the source, project metadata, and investigation log edits.
- Debug Win32 solution build passed with 0 warnings and 0 errors.
