# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.

## Investigation

- Figure out what `decideTypeId` in `RunRpcTestCase` does and see if there is a better way.
  - It is used to call `RegisterLocalObject`, unfortunately the current implementation can't make a local object tells the lifecycle what it type id is.
- `RpcLfiecycle` registered in reflection, because `IRpcLifecycle::GetController` returning itself causes problem in `RuntimeTest`
  - Without reflection registration, the metadata doesn't say it implements two interfaces at the same time.
- Verify `WfErrors::Cpp*`, probably not needed as errors should have been catched in eariler phases.
- `WfCpp_WriteReflection.cpp` is changed because `INVOKE(GET?)_INTERFACE_PROXY` does not handle overloading property. This should be a VlppParser bug. Fix the bug and revert this file.
  - https://github.com/vczh-libraries/Workflow/commit/7b0e54a1964774cbb011ea1e675aa8c72a3fbec4

## ToDo

- Prompt: only when building fails due to binary occupation, kill cdb and kill the test process.
- Continue to add more test cases until all features are covered.
  - Events
  - Destructors in collection elements.
  - Cached properties (by default)
  - Dynamic properties
  - Double read byval collections with property attributes.
  - Sending cached property values proactively from remote to wrapper.
- Generate JSON from "RPC metadata in Workflow syntax".
  - Generate with wrappers.
- Solve interface inheritance and casting.
- C++ codegen for rpc data type JSON serialization.
- Delete Runtime test category.
- Update `TestLibraryRpcByval.cpp` in `LibraryTest` to use `RpcDualLifecycleMock`, remove `RpcByvalLifecycleMock`.
  - Refactor `RpcDualLifecycleMock` to make it a common implementation for `GacUI`.
  - May need to refactor how to retrieve ops interfaces.

# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow the instruction to commit and push for each task, before doing the next task.

## Task 1

In `TestCasesRpc.cpp` there is a `RunRpcTestCase` function. This file is generated, but this function never changes.
So I believe it could be just moved to `Test\Source\TestCasesRpc.h`.
You can then make `TestRpcCompile.cpp` in `CompilerTest_LoadAndCompile` much more clean.
This file should be also added to all 3 `CppTest*` project in the same solution explorer folder with `TestCasesRpc.cpp`.

## Task 2

Add `IRpcObjectOps::RegisterService` so that lifecycle calls `IRpcObjectOps` instead of opposite.
Currently in the generated wrapper, the `RequestService` function looks like this:
```
override func RequestService(typeId : int) : (system::RpcObjectReference)
{
    switch (typeId)
    {
        case rpctype_RpcEvent__IService:
        {
            return _lc.PtrToRef(_lc.RequestService("RpcEvent::IService"));
        }
        default:
        raise "Unknown RPC type id.";
    }
}
```
I believe by adding `IRpcObjectOps::RegisterService`, you can maintain the type id to service map in this generated `IRpcObjectOps` implementation.
And then the lifecycle implementation, in `RpcDualLifecycleMock.cpp`, could instead redirect the call to here, instead of the opposite way which is currently implemented.
In `RpcDualLifecycleMock.cpp`, there is already a string to id mapping. And the `RegisterService` and `RequestService` in `RpcDualLifecycleMock.cpp` takes a string, so it could be directly used, and inject the same error.

## Task 3

`RpcDualObjectOps` looks redundant. In `TestCasesRpc.cpp`:
- lifecycleA and objectOpsA manages local objects and wrappers for remote objects.
- objectOpsA is generated in Workflow.
- lifecycleA talks to objectOpsB to simulate how clientA talks to clientB.
- `RpcDualObjectOps` looks like just a redirection doing nothing to a objectOpsA/B.
- I believe `RpcDualObjectOps` could just be removed, and replaced by the actual objectOpsA/B.
- By saying removing, you should not introduce any other new classes doing pure redirection. You should find a solution to avoid any unnecessary redirection.

## Task 4

`RpcDualObjectEventOps` and `RpcDualEventForwarder` look like never being used outside of `RpcDualObjectOps`. Remove them.
I believe a better implementation would be:
- In the generated wrapper for an interface with events, all events are listened so that remote side could trigger the event of a wrapper and local side would know.
  - This is already implemented, it is good.
- In the local side, when an object is being used as a local object to the local lifecycle implementation, all events are listened so that the local side could trigger the event of a wrapper and remote side would know.
  - This is implemented but not in the way I expect.
  - There is a big piece of code in `RpcDualLifecycleMock::TrackLocalObject` protected by `VCZH_DESCRIPTABLEOBJECT_WITH_METADATA`, this should be not necessary, remove it.
  - Instead, new functions should be introduced in the generated wrapper Workflow script.
  - You can follow the pattern of `rpcwrapper_*` and `rpcwrapper_Create`, to create multiple `rpclistener_*` and `rpclistener_Attach` for all `@rpc:Interface` with events.
    - **IMPORTANT**: "with events" means not only the interface itself but also in base interfaces.
    - Interface "without events" should not generate the `rpclistener_*` function. If a type id exists but such interface do not have any event, `rpclistener_Attach` should not throw any error.
    - If no `rpclistener_*` function is generated, `rpclistener_Create` should also be skipped.
    - Just like how `rpcwrapper_*` is doing, `rpclistener_*` is a function to attach all events and notify the local lifecycle. So it returns `void`. And `rpcwrapper_*` should call `rpclistener_*`, so the same piece of code will not be generated twice.
    - I believe all generated code from test samples without event involved will still stay unchanged. Because if any event is not declared in a test sample, `rpclistener_*` do not exist.
- I believe in this way, `RpcDualObjectEventOps` and `RpcDualEventForwarder` and the `VCZH_DESCRIPTABLEOBJECT_WITH_METADATA` protected code from `RpcDualLifecycleMock` should be able to remove.
- I believe in this way, `TEST_CASE(L"Rpc:Event")` in `TestCasesRpc.cpp` should avoid generated complex code of of the last two lambdas:
  - The first lambda should be replaced by `rpclistener_*` functions.
  - The second lambda is handling event ids, there is already a generated `rpc_IRpcObjectEventOps` functions in each wrapper Workflow script, I believe you should just use that instead.
- Since `TestRpc.cpp` in `RuntimeTest` basically doing the same thing, new generated functions should just be able to use, instead of generating unnecessary reflection code, maybe this is what the `VCZH_DESCRIPTABLEOBJECT_WITH_METADATA` protected code is doing, anyway this should not be necessary. And such code should be in `TestRpc.cpp` instead of in `RpcDualLifecycleMock`. `RpcDualLifecycleMock` is working for both Workflow VM and generated C++ code, so such option doesn't taste good. If any code is only used in `RuntimeTest`, it should be in `TestRpc.cpp`.

## General Instruction

If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
- Pass all unit test, fix any test failure including pre-existings.
- After finishing everything, git commit and git push to the current branch.
- If in any task you are adding new test sample, or modifying any C++ or Workflow generation code, causing a huge amount of files generated from test samples to change:
  - Two commits are required. First commit only has all modified files and files you created directly, second commit has all new files that not created by you (aka auto generated)
  - Otherwise, One commit is good.
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
