# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.
- If you can't fix a runtime issue within a few rounds of guess-and-edit, you need to debug the process.

## Task 1

You are going to perform a tough change. So I strongly recommend you that, when a unit test project fails, immediatelly rerun it with a debugger.
The goal is to test if JSON serialization works, core of the change is in generated wrapper Workflow script (`Wrapper_*_Json.txt`).
Basically, when a wrapper pass message to the remote object, or when any object triggers an event and notify all lifecycle, it does the follow thing to arguments and return values
- Turn an value to its serializable form, e.g., values in primitive types are untouched, interfaces or wrappers become `RpcObjectReference`, byval collections are copied and all elements are either byval copied or serialized.
- Values in serializable forms are serialized to JSON.
- JSON values get pass through `IRpc(Object|List)(Event)?Ops`.
- In the other side, JSON values are deserialized to its serializable form.
- Serializable forms are translated back to interfaces, wrappers, or values in primitive types.

I would like the test to be limited in `CppRest*` projects, DO NOT modify `RuntimeTest` project. So basically you only want to touch `TestCasesRpc.h`, and any other places if bugged.

In order to turn on JSON serialization, several things need to be done:
- In each test case, call `rpcops_IRpcSerializer`, when implementations of `IRpcListOps` and `IRpcListEventOps` are created, pass `IRpcSerializer*` in.
- When any wrappers for collection types are created, pass pass `IRpcSerializer*` in. You may need to register the object into `RpcLifecycleBase`, but use its shared pointer form so that `RpcLifecycleBase` owns `IRpcSerializer^`, but no need to change the `IRpcLifecycle` interface.
- Call `rpcops_IOps_CreateJson` instead of `rpcops_IOps_Create`.
- Call `rpcops_IRpcObjectOpsJson` instead of `rpcops_IRpcObjectOps`.
- Call `rpcops_IRpcObjectEventOpsJson` instead of `rpcops_IRpcObjectEventOps`.

You should run `CompilerTest_LoadAndCompile` at least ones, because some compiler generated binaries are not covered by git.
After that, if you only change `TestCasesRpc.h`, only `CppTest*` need to run.

Test results, aka `IndexRpc.txt`, should remain the same. These change only add one extra processing of JSON serialization on each side, they should not affect the semantic of the test in any mean.

The implementation might be buggy, be prepared to fail for a lot of times. So I strongly recommend you that, when a unit test project fails, immediatelly rerun it with a debugger, therefore you don't lost in your way.

## Task 2

You are going to create `TODO_RPC_GeneratedWrappers.md` to the following sections:
- Describe when to call generated functions and what do they do. Another section for generated functions about JSON serialization.
- Describe how do generated functions finish their work. Another section for generated functions about JSON serialization.

Assume the reader is one who want to use generated C++ version from Workflow, willing to adopt RPC. By the way, no need to cover the usage of attributes because the are already in `TODO_RPC_Definition.md`. You can reference the following materials:
- `TODO_RPC_Definitions.md`
- `TODO_RPC_Json.md`
- `TestCasesRpc.h`.
But do not mention anything specific to test projects.

# UPDATES

# TEST

Task 1 succeeds when RPC generated C++ tests run through the JSON RPC operation path without changing the expected `IndexRpc.txt` semantics. The minimum verification is to run `CompilerTest_LoadAndCompile` once, then run `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` after the `TestCasesRpc.h` change.

Task 2 succeeds when `TODO_RPC_GeneratedWrappers.md` explains generated wrapper APIs and JSON wrapper APIs for generated C++ users without relying on test-project-specific wording.

# PROPOSALS

- No.1 Enable JSON RPC test path and preserve byval keep-alive across JSON serialization

## No.1 Enable JSON RPC test path and preserve byval keep-alive across JSON serialization

### CODE CHANGE

The proposed change switches the generated C++ RPC test harness to the JSON RPC operations and serializer, passes the serializer into list-operation bridges and byref collection wrappers, and updates generated JSON wrapper code so a boxed byval value can transfer its internal RPC keep-alive holder to the JSON node produced from it. The runtime helper for transferring this internal property is exposed through the reflected `IRpcLifecycle` static helper surface so generated Workflow wrapper code can call it.
