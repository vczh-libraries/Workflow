# Repro

There are multiple tasks in this instruction.
You should finish them one by one.
At the end of these tasks, I will instruct you to git commit and push.
You should always git commit and push before execution the next ask.
The above instructions are extremely important. You must obey.

All mentioned sample files are in Test\Resources\Rpc

## Task 1

Use powershell to copy `Collection_*.txt` to `CollectionOblist_*.txt`.
No building or testing is required for this task.
git commit and push. DO NOT ASK ME ANY QUESTION, just finish the task at your own judgement.

## Task 2

All `CollectionOblist_*.txt` has the top level namespace for rpc interfaces, called `RpcCollection`.
Change them to `RpcCollectionOblist`.
Including all other code in these files that reference to `RpcCollection`.
Add these cases to `IndexRpc.txt`. You could just copy everything from `Collection_*` samples.
No building or testing is required for this task.
git commit and push. DO NOT ASK ME ANY QUESTION, just finish the task at your own judgement.

## Task 3

Override all `CollectionOblist_*_Default.txt` with `CollectionOblist_*_InByref_OutByref.txt`.
Override all `CollectionOblist_*_PropDefault.txt` with `CollectionOblist_*_PropByref.txt`.
Now for all overrided files, you will see both namespace changes and verification changes, only keep the verification changes, revert the namespace changes.
Notice the above overriding works I just ask you to do, override expected test case results in `IndexRpc.txt` for these overrided `CollectionOblist_*` cases.
No building or testing is required for this task.
git commit and push. DO NOT ASK ME ANY QUESTION, just finish the task at your own judgement.

## Task 4

For all `CollectionOblist_*` files, change every `T[]` to `observe T[]`. Containers could be nested, therefore:
- `T[]` -> `observe T[]`.
- `T[][]` -> `observe (observe T[])[]`.
- `T[U][]` -> `observe (T[U])[]`.
- `T[][U]` -> `(observe T[])[U]`.
Notice that the container syntax is left associated.
No building or testing is required for this task.
git commit and push. DO NOT ASK ME ANY QUESTION, just finish the task at your own judgement.

## Task 5

In this task you are going to build and run test cases to verify if these cases are working.
According to `TODO_RPC_Definition.md`, following the above tasks should just pass all unit test projects.
Processing `observe T[]` is just like `T[]`, but the only difference is that:
- For `T[]`, the default option will be `@rpc:Byval` when T is non-interface or containers of non-interfaces, recursively.
- For `observe T[]`, the default option will always be `@rpc:Byref`. That why in `Task 3` those overriding works exist.
This is the purpose of these tests.

This test is to ensure that:
- `@rpc:Byref` or `@rpc:Byval` attributes on properties, including absense, will be transferred to getter's return value and setter's parameter, if the property type is a collection. Property in interfaces are implemented by a pair of override functions.
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
- Pass all unit test, fix any test failure including pre-existings.
- After finishing everything, git commit and git push to the current branch.
  - Two commits are required. First commit only has all modified files and files you created directly, second commit has all new files that not created by you (aka auto generated)
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.

## Task 6.

For all existing `Collection_*_Prop*.txt`, `CollectioDist_*_Prop*.txt`, `CollectionOblist_*_Prop*.txt` tests,
attach `@rpc:Dynamic` to all properties in the rpc interface,
run `..\Tools\Tools\Build.ps1 Workflow`.
Maybe some log files will change, but the test should just pass.
- Pass all unit test, fix any test failure including pre-existings.
- git commit and push. DO NOT ASK ME ANY QUESTION, just finish the task at your own judgement.
