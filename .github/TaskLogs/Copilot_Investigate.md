# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Follow job.new-sample.md to add a new sample: Rpc\EventOblist.txt

Checkout Rpc\Event.txt, follow the pattern, but this time you are going to run the `ItemChanged` event for an `observe int[]` collection. Checkout how how observable list is used in `Rpx\CollectionOblist_*` samples, and check out the `ObservableList<T>` source code to understand how `ItemChanged` is triggered.

Follow Rpc\Event.txt to complete Rpc\EventOblist.txt to, basically this will be done in its `clientMain` function:
- Send an `observe int[]` to `IService` and the service will hold the object, such object will be obviously a wrapper. Because `observe int[]` is by default passed by reference, so no attributes are needed.
- Make a function to modify the `observable int[]` object. The function can assume that the list is empty, and after some manipulation, the list will still be empty. But multiple `ItemChanged` event will be triggered.
- Just like Rpc\Event.txt, clientMain will first register an event handler, ask `IService` to run the function internally, so that events will be triggered on the wrapper and passed from `serviceMain` to `clientMain`. Unregister the event handler.
- Ask `IService` to register the event on the wrapper, and in `clientMain` call the function again, so that events will be triggered on the local object and passed from `clientMain` to `serviceMain`.
- Each `ItemChanged` occurance will write down who handles the event, and the event arguments, in a compact way.
- I will leave you to decide the details of what should be in `Rpc\EventOblist.txt`.

In this task you are going to build and run test cases to verify if these cases are working, according to `TODO_RPC_Definition.md`
This test is to ensure that:
- In `WfLibraryRpc.(h|cpp)` and `RpcDualLifecycleMock.(h|cpp)`, the predefined implementation handles observable list correctly.
- This is for container, not for interfaces, so I believe no codegen fixing will be involved, until you find that it is impossible to avoid.

Understand what the test case trying to say, you are not allowed to change:
- The content of the sample, unless it doesn't build.
- Workflow parser.
- Workflow compiling.
- Workflow to C++ code generation.

You are highly possibly need to fix:
- `Rpc(B|Unb)oxBy(val|ref)`, as these 4 C++ functions are directly called in generated wrapper classes written Workflow script.
- implementation of `RpcDualLifecycleMock` and its connected interfaces if sample fails in either `RuntimeTest` or `CppTest*`.
- The generated C++ code is very straight forward, if it fails, check `RpcDualLifecycleMock` first.
  - The comment in the sample describes how `RpcDualLifecycleMock` and the generated C++ code is supposed to work.

If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
  - Pass all unit test, fix any test failure including pre-existings.
  - After finishing everything, git commit and git push to the current branch.
  - Two commits are required. First commit only has all modified files and files you created directly, second commit has all new files that not created by you (aka auto generated)
  - DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.

# UPDATES

# TEST [CONFIRMED]

- Add `Test/Resources/Rpc/EventOblist.txt` following `Rpc/Event.txt`, but use `observe int[]` and a shared `Modify(xs)` helper that performs `Add`, `Insert`, `Set`, `RemoveAt`, and `Clear`, so `ItemChanged(index, oldCount, newCount)` fires multiple times while the list starts and ends empty.
- Update `Test/Resources/IndexRpc.txt` so the expected result contains both directions of event delivery:
  - `clientMain` attaches locally, asks the service to mutate the held wrapper, and receives `[clientMain:...]`.
  - `serviceMain` attaches on the held wrapper, the client mutates the local observable list, and receives `[serviceMain:...]`.
- The initial repro was confirmed in `RuntimeTest`: before fixing `RpcDualLifecycleMock`, `EventOblist` only produced the `clientMain` half, proving that observable-list `ItemChanged` was not forwarded from the local object back to the wrapper listener on the opposite lifecycle.
- Success criteria:
  - `RuntimeTest` must show `EventOblist` expected == actual on `Win32` and `x64`.
  - `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` must all include `Rpc:EventOblist` and match expected output on `Win32` and `x64`.
  - `LibraryTest` must still pass its existing RPC observable-list coverage on `Win32` and `x64`.
  - `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, and `..\Tools\Tools\Build.ps1 Workflow` must all pass so generated RPC artifacts and release builds stay consistent.

# PROPOSALS

- No.1 Forward observable-list `ItemChanged` by full `RpcObjectReference` in `RpcDualLifecycleMock` [CONFIRMED]

## No.1 Forward observable-list `ItemChanged` by full `RpcObjectReference` in `RpcDualLifecycleMock`

### CODE CHANGE

- Added the new sample `Test/Resources/Rpc/EventOblist.txt` and its expected output in `Test/Resources/IndexRpc.txt`.
- Updated `Test/Source/RpcDualLifecycleMock.cpp` so `OnItemChanged` first looks for a local wrapper using full `RpcObjectReference` equality, dispatches directly to that wrapper when found, and only falls back to `RpcLifecycleMock::OnItemChanged` when no local wrapper owns the reference.
- Added `EventOblist` generated translation units to `Test/UnitTest/Generated_CppRpc/Generated_CppRpc.vcxitems` and `Test/UnitTest/Generated_ReflectionRpc/Generated_ReflectionRpc.vcxitems`, because the new generated files were created correctly by `CompilerTest_LoadAndCompile` but were not linked into `CppTest_Metaonly` and `CppTest_Reflection`.
- Regenerated the RPC outputs under `Test/Generated/*` and `Test/SourceCppGenRpc/*`.

### CONFIRMED

- The root cause was in `RpcDualLifecycleMock::OnItemChanged`. It tried to match wrappers only by `ref.objectId`, but by-reference containers are identified by the full `RpcObjectReference`, including the owning lifecycle. That made the dual-lifecycle mock unreliable when the same object id existed on both sides.
- Because of that mismatch, when `serviceMain` attached `ItemChanged` to the held wrapper and `clientMain` mutated the local observable list, the wrapper-side listener did not reliably receive forwarded notifications. The original repro showed exactly that: `RuntimeTest` produced only the `[clientMain:...]` half and missed every `[serviceMain:...]` record.
- After matching the full `RpcObjectReference` and forwarding to the paired lifecycle only when there is no local wrapper to consume the event, both directions now work:
  - service-side mutations on the held wrapper notify `clientMain`
  - client-side mutations on the local observable list notify `serviceMain`
- Verification completed successfully with:
  - `LibraryTest` on `Debug|Win32` and `Debug|x64`
  - `CompilerTest_GenerateMetadata` on `Debug|Win32` and `Debug|x64`
  - `CompilerTest_LoadAndCompile` on `Debug|Win32` and `Debug|x64`
  - `RuntimeTest` on `Debug|Win32` and `Debug|x64`
  - `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` on `Debug|Win32` and `Debug|x64`
  - `..\Tools\Tools\Build.ps1 Workflow`
- All tests passed, and `EventOblist` matched expected output in runtime execution, direct generated C++ execution, metaonly reflection, and full reflection paths.
