# Extra Guidelines of Adding New Workflow Sample in RPC Category

Checkout `new-samples.md` and follow the instructions.
If the current implemention is correct, the added samples should just pass the test.

## Rpc Sample Convention

RPC samples are split into three files:

- `SAMPLE.txt` contains RPC interfaces and stateless functions shared by the service and client. It must not contain top-level variables.
- `SAMPLE_Service.txt` contains only the top-level `serviceMain` function. Variables and functions used only by the service, including indirect use through other service helpers, belong inside the anonymous service implementation created in `serviceMain`.
- `SAMPLE_Client.txt` contains `clientMain` and all variables and functions used only by the client, including indirect use through other client helpers.

The service and client can run in different processes. They must not share variables, even though the in-memory test harness loads all three files into one process. Stateless functions may remain in `SAMPLE.txt` only when both `serviceMain` and `clientMain` use them directly or indirectly. Remove unused declarations.

Only `SAMPLE` should be listed in `IndexRpc.txt`. All three files should be present in `CompilerTest_LoadAndCompile` under `Resource Files\Rpc`.

## Verifying Samples

Workflow script syntax and semantic should be intuitive.
During reading the sample, you should verify it with the goal of the task.
Ensure all logs or exceptions in the sample accurately reflected the intention of the design.
Ensure the expected result would be what users would expect.
Verify that every RPC sample follows the `Rpc Sample Convention`.

## Restriction

Understand what the test case trying to say, you are not allowed to change:
- The content of the verified sample, unless it doesn't build.
- Workflow parser.
- Workflow compiling.
- Workflow to C++ code generation.

You are highly possibly need to fix:
- `Rpc(B|Unb)oxBy(val|ref)`, as these 4 C++ functions are directly called in generated wrapper classes written Workflow script.
- The wrapper classes generation.
- implementation of `RpcDualLifecycleMock` and its connected interfaces if sample fails in either `RuntimeTest` or `CppTest*`.
- The generated C++ code is very straight forward, if it fails, check `RpcDualLifecycleMock` first.
  - The comment in the sample describes how `RpcDualLifecycleMock` and the generated C++ code is supposed to work.
