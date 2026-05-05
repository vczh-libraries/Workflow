# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

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

# UPDATES

- Task 1: Added `Test\Source\TestCasesRpc.h` with the stable `RunRpcTestCase` template helper that used to be emitted into generated RPC test cases.
- Task 1: Updated `Test\UnitTest\CompilerTest_LoadAndCompile\TestRpcCompile.cpp` to emit `#include "../Source/TestCasesRpc.h"` and stop writing the helper template body.
- Task 1: Added `TestCasesRpc.h` to `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` project/filter files under the same `Source Files` Solution Explorer folder as `TestCasesRpc.cpp`.
- Task 1: Regenerated RPC test harness outputs so `Test\SourceCppGenRpc\TestCasesRpc.cpp`, `Test\Generated\CppRpc32\TestCasesRpc.cpp`, and `Test\Generated\CppRpc64\TestCasesRpc.cpp` include the shared header and no longer contain the `RunRpcTestCase` template body.
- Task 2: Added `IRpcObjectOps::RegisterService(vint typeId, Ptr<IDescriptable> service)` to the RPC controller interface, reflection proxy, and metadata.
- Task 2: Updated RPC generation so generated object ops own a `_services` dictionary, `RegisterService` stores or removes services by type id, and `RequestService` converts the stored service pointer through lifecycle `PtrToRef`.
- Task 2: Updated lifecycle mocks and tests so service registration flows from lifecycle into local object ops; `RpcDualLifecycleMock` now unregisters service entries from local object ops when local service objects are released.
- Task 2: Regenerated RPC C++ and metadata outputs for Win32 and x64 after the generator and reflection changes.

# TEST

- For Task 1, build Debug Win32 and Debug x64 for `Test\UnitTest\UnitTest.sln`, run `CompilerTest_LoadAndCompile` so `Test\SourceCppGenRpc\TestCasesRpc.cpp` is regenerated from `TestRpcCompile.cpp`, then build and run `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` for both platforms.
- Success criteria for Task 1: generated `TestCasesRpc.cpp` includes `../Source/TestCasesRpc.h`, no longer contains the `RunRpcTestCase` template body, all generated RPC test cases still compile, and all `CppTest*` RPC cases pass.
- For Task 2, build Debug Win32 and Debug x64 for `Test\UnitTest\UnitTest.sln`, run `CompilerTest_LoadAndCompile` to refresh generated RPC output, then run the full matrix: `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` across the required platforms.
- Success criteria for Task 2: lifecycle service registration goes through local generated object ops, generated `RequestService` reads from object ops storage, service unregister clears that storage, and the RPC destructor samples release service objects correctly.

# PROPOSALS

- No.1 Move `RunRpcTestCase` to `Test\Source\TestCasesRpc.h`

## No.1 Move `RunRpcTestCase` to `Test\Source\TestCasesRpc.h`

Move the stable `RunRpcTestCase` template helper out of generated `Test\SourceCppGenRpc\TestCasesRpc.cpp` into a real source header. Update `CompilerTest_LoadAndCompile\TestRpcCompile.cpp` so it emits `#include "../Source/TestCasesRpc.h"` instead of writing the template function, and add the new header to `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` in the same Solution Explorer folder as `TestCasesRpc.cpp`.

### CODE CHANGE

- `RunRpcTestCase` now lives in `Test\Source\TestCasesRpc.h` and keeps the same two-lifecycle setup, event bridge fallback, wrapper factory registration, service/client invocation, assertion, and tracked-wrapper cleanup behavior.
- Generated `TestCasesRpc.cpp` keeps only the sample-specific includes, type loading function, and `RunRpcTestCase<...>` calls with generated lambdas.

### VALIDATION

- `get_errors` reported no diagnostics for `Test\Source\TestCasesRpc.h`, `Test\SourceCppGenRpc\TestCasesRpc.cpp`, or `Test\UnitTest\CompilerTest_LoadAndCompile\TestRpcCompile.cpp`.
- Debug Win32 and Debug x64 builds passed after the direct edit and again after `CompilerTest_LoadAndCompile` regenerated C++ output.
- `LibraryTest` Win32/x64 passed: 2/2 test files, 14/14 test cases.
- `CompilerTest_GenerateMetadata` Win32/x64 passed: 1/1 test files, 2/2 test cases.
- `CompilerTest_LoadAndCompile` x64 passed: 6/6 test files, 689/689 test cases.
- `RuntimeTest` Win32/x64 passed: 4/4 test files, 243/243 test cases.
- `CppTest` Win32/x64 passed: 2/2 test files, 209/209 test cases.
- `CppTest_Metaonly` Win32/x64 passed: 2/2 test files, 209/209 test cases.
- `CppTest_Reflection` Win32/x64 passed: 2/2 test files, 209/209 test cases.

# PROPOSALS

- No.2 Route RPC service registration through object ops

## No.2 Route RPC service registration through object ops

Add `IRpcObjectOps::RegisterService` and move service object storage into generated object ops. Keep lifecycle APIs string-based, but make lifecycle implementations resolve the full name to a type id and call local object ops for the actual service registration.

### CODE CHANGE

- `IRpcObjectOps` now declares `RegisterService(vint typeId, Ptr<IDescriptable> service)`, and reflection metadata/proxy support the new method.
- Generated RPC object ops allocate `_services : system::Interface^[int]`, implement `RegisterService` by type id, remove entries when `service` is null, and implement `RequestService` by returning `_lc.PtrToRef(_services[typeId])`.
- `RpcLifecycleMock` and `RpcDualLifecycleMock` forward service registration to the registered object ops callback.
- `RpcDualLifecycleMock::UnregisterLocalObject` now detects service objects and clears the generated object ops service entry with `RegisterService(typeId, nullptr)`, fixing the destructor ownership regression in the RPC `Dtor` samples.
- Runtime and generated RPC test helpers register local generated object ops with each lifecycle before running the cross-lifecycle RPC scenarios and clear those callbacks during teardown.

### VALIDATION

- Debug x64 build passed with 0 warnings and 0 errors.
- Debug Win32 build passed with 0 warnings and 0 errors.
- `LibraryTest` Win32/x64 passed: 2/2 test files, 14/14 test cases.
- `CompilerTest_GenerateMetadata` Win32/x64 passed: 1/1 test files, 2/2 test cases.
- `CompilerTest_LoadAndCompile` x64 passed both internal passes: 6/6 test files, 689/689 test cases.
- `RuntimeTest` Win32/x64 passed: 4/4 test files, 243/243 test cases.
- `CppTest` Win32/x64 passed: 2/2 test files, 209/209 test cases.
- `CppTest_Metaonly` Win32/x64 passed: 2/2 test files, 209/209 test cases.
- `CppTest_Reflection` Win32/x64 passed: 2/2 test files, 209/209 test cases.