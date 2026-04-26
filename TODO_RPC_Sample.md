# Repro

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
- The wrapper classes generation.
- implementation of `RpcDualLifecycleMock` and its connected interfaces if sample fails in either `RuntimeTest` or `CppTest*`.
- The generated C++ code is very straight forward, if it fails, check `RpcDualLifecycleMock` first.
  - The comment in the sample describes how `RpcDualLifecycleMock` and the generated C++ code is supposed to work.

If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
  - Pass all unit test, fix any test failure including pre-existings.
  - After finishing everything, git commit and git push to the current branch.
  - Two commits are required. First commit only has all modified files and files you created directly, second commit has all new files that not created by you (aka auto generated)
  - DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
