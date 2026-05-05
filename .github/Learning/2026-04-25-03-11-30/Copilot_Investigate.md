# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

## Task 1

- `Collection_Nested_*` tests `int[][]`.
- `CollectionDist_Nested_*` tests `string[int][]`.
- `CollectionDist_Interface_Nested_*` tests `IValue^[int][int]`.
But:
- `Collection_Interface_Nested_*` tests `IValue^[][]`. You are going to change it to `IValue^[][int]`. The new types means a dictionary of key `int` and value `IValue^[]`. So we are able to test if the outer container is dictionary and the inner container is list, which is not currently covered.

## Task 2

Follow job.new-sample.md and add 8 new samples:
- Read `Rpc\Collection(Dist)?(_Interface)?(_Nested)?_*.txt`
- Copy them to `Rpc\Collection(Dist)?(_Interface)?(_Nested)?_Default.txt`
  - When `_Interface`, copy from `_InByref_OutByref`.
  - When not `_Interface`, copy from `_InByval_OutByval`.
  - Remove all `@rpc:Byval` and `@rpc:Byref` from samples, according to `TODO_RPC_Definition.md`, following the above two rules should just pass all unit test projects. This is the purpose of these tests.
  - Remember to change the namespaces for interfaces, because types could not conflict across samples.

## General Instructions

This test is to ensure that:
- when a container is transferred with byref, a wrapper is created.
- when a container is transferred with byval, a copy (non wrapper) is created.
- when byval or byref is not explicitly written, the correct default option will be selected and applied to these containers. It depends on the actual type.
- when byval or byref is applied to a nested container, all levels of container applies.
- when a container contain interfaces, these interfaces are passed between lifecycles as expected, just like normal interface value.

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
- **PER EACH TASK**
  - Pass all unit test, fix any test failure including pre-existings.
  - After finishing everything, git commit and git push to the current branch.
    - Two commits are required. First commit only has all modified files and files you created directly, second commit has all new files that not created by you (aka auto generated)
  - DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.

# UPDATES

# TEST [CONFIRMED]

- Modify `Collection_Interface_Nested_*` samples from `IValue^[][]` to `IValue^[][int]` so they exercise dictionary outside and list inside while preserving the existing byval/byref behavior matrix.
- Add eight `_Default` RPC samples for collection, nested collection, interface collection, and nested interface collection in list and dictionary forms.
- For non-interface containers, the default samples copy the `_InByval_OutByval` behavior and assert copied local containers.
- For interface containers, the default samples copy the `_InByref_OutByref` behavior and assert wrapper containers and cross-lifecycle interface value ownership.
- Success criteria:
  - `CompilerTest_LoadAndCompile` accepts the modified and new RPC samples and generates the C++ RPC outputs.
  - `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` pass the default samples and the modified nested interface samples for Win32 and x64.
  - The full required debug Win32/x64 unit-test matrix passes, followed by the final umbrella `Build.ps1 Workflow` validation.

# PROPOSALS

- No.1 Apply default RPC container tests and fix default transfer behavior as needed

## No.1 Apply default RPC container tests and fix default transfer behavior as needed

### CODE CHANGE

- Updated `Collection_Interface_Nested_InByval_OutByval`, `Collection_Interface_Nested_InByval_OutByref`, `Collection_Interface_Nested_InByref_OutByval`, and `Collection_Interface_Nested_InByref_OutByref` from `IValue^[][]` to `IValue^[][int]`, including dictionary construction and indexing in the sample body.
- Added the eight `_Default` RPC samples and registered their expected outputs in `Test/Resources/IndexRpc.txt`.
- Added the generated C++ and reflection RPC files for the eight default samples to the shared item manifests consumed by `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`.
- Fixed default RPC transfer selection in `Source/Analyzer/WfAnalyzer_ValidateRPC.cpp` so strong typed collections default to `@rpc:Byref` when their element or dictionary value is, or recursively contains, an RPC interface.
- Updated `TODO_RPC_Definition.md` to describe the interface-containing collection default rule.

### CONFIRMED

- Debug Win32/x64 builds passed after source and generated RPC updates.
- `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` all passed for Debug Win32 and Debug x64.
- The previous `Collection_Interface_Nested_Default` runtime failure was fixed; `RuntimeTest` passed for both platforms.
- Final umbrella validation passed: `& C:\Code\VczhLibraries\Tools\Tools\Build.ps1 Workflow` returned exit code 0.