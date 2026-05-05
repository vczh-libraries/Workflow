# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.

## Investigation

- Figure out what `decideTypeId` in `RunRpcTestCase` does and see if there is a better way.
  - It is used to call `RegisterLocalObject`, unfortunately the current implementation can't make a local object tells the lifecycle what it type id is.
- Verify `WfErrors::Cpp*`, probably not needed as errors should have been catched in eariler phases.
- `WfCpp_WriteReflection.cpp` is changed because `INVOKE(GET?)_INTERFACE_PROXY` does not handle overloading property. This should be a VlppParser bug. Fix the bug and revert this file.
  - https://github.com/vczh-libraries/Workflow/commit/7b0e54a1964774cbb011ea1e675aa8c72a3fbec4

## ToDo

- Continue to add more test cases until all features are covered.
  - Sending cached property values proactively from remote to wrapper.
  - Interface inheritance.
- Generate JSON from "RPC metadata in Workflow syntax".
  - Review JSON related constructions.
  - In `rpcjson_Serialize` and `rpcjson_Deserialize` for serializable custom primitive types, it should serialize like enum but the second element is string.
    - Need to inject a serializable struct to `CppTypes.(h|cpp)`.
    - Need to update the .d.ts file as well.
  - Make sure arguments are serialized to expected JSON representation.
    - Make a `RpcDualDispatcherJson` to record all JSON objects, code gen ts files and create a npm project to build against the schema, just like VlppParser2.
- Refactor wrapper generation to use C++ type -> ITypeInfo -> WfType helper, eliminate innecessary helpers.
- Document RPC.
- Document compiler process.
- Document AST building helper functions.
