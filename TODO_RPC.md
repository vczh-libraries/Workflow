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
  - Inside a wrapper, byval strong typed collection will be serialized/deserialized with loops, each level of loop expands one level of container.
    - Byref containers have no choice but to use weak type serialization, these will be implemented in containers wrappers (how)?
    - Dispatcher or Lifecycle may need to expose a weak type serialization API, so that when JsonNode is seen, they are called.
    - Or always call these API and then Dispatcher will do JSON or other or no transformation automatically.
     - RpcDualJsonDispatcher
  - Run JSON serialization for predefined types, generated types and operations in `RpcDualDispatcherMock`.
- Refactor wrapper generation to use C++ type -> ITypeInfo -> WfType helper, eliminate innecessary helpers.
- Document RPC.
- Document compiler process.
- Document AST building helper functions.
