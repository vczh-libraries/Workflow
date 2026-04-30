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

- Rework `Dtor.txt` and `Dtor2.txt` as the service now no longer being deleted before unit test ends.
- Review `RpcControllerMock` and promote it to become a standard implementation `RpcController`.
- Review `RpcDualLifecycleMock`, split as many thing as possible to the base class `RpcLifecycleMock`, which does not have the knowledge of "dual", and promote to become a standard implementation `RpcLifecycleBase`.
- Review `LocalLifecycleMock` in `TestCasesRpc.h`.
- Remove `RpcByvalLifecycleMock`, replace it with `RpcDualLifecycleMock`.
- Continue to add more test cases until all features are covered.
  - Sending cached property values proactively from remote to wrapper.
  - Interface inheritance.
- Generate JSON from "RPC metadata in Workflow syntax".
  - JSON for predefined types.
  - Run JSON serialization for predefined types, generated types and operations in `RpcDualDispatcherMock`.
- Refactor wrapper generation to use C++ type -> ITypeInfo -> WfType helper, eliminate innecessary helpers.
- Document RPC.
- Document compiler process.
- Document AST building helper functions.
