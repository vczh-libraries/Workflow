# Guideline of Adding New Workflow Sample in RPC Category

Checkout `new-samples.md` and follow the instructions.
If the current implemention is correct, the added samples should just pass the test.

## Verifying Samples

Workflow script syntax and semantic should be intuitive.
During reading the sample, you should verify it with the goal of the task.
Ensure all logs or exceptions in the sample accurately reflected the intention of the design.
Ensure the expected result would be what users would expect.
RPC samples are split into `SAMPLE.txt` and `SAMPLE_Test.txt`.
Verify that `SAMPLE.txt` contains only RPC definitions required for metadata/wrapper generation, and that `SAMPLE_Test.txt` contains all executable logic such as globals, helper functions, `serviceMain`, and `clientMain`.
Only `SAMPLE` should be listed in `IndexRpc.txt`; both files should be present in `CompilerTest_LoadAndCompile`'s `Resource Files\Rpc` folder.

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
