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

- Continue to add more test cases until all features are covered.
  - Cached properties with dtor affected by cached value
    - Interface type
    - Collection type
  - Sending cached property values proactively from remote to wrapper.
  - Event argument default byref/byval options.
  - Passing null to interface.
  - Passing null to nullable.
- Generate JSON from "RPC metadata in Workflow syntax".
  - JSON for predefined types.
  - `RpcLifecycleBase` and `RpcLifecycleJson`.
    - So `IRpcController` implementation should be assigned not inherited.
    - This could remove reflection registration for `RpcLfiecycle`.
  - Extract some implementation from `RpcDualLifecycleMock`.
  - `RpcLifecycleJson` should notnhave knowledge of where JSON goes and async/sync.
  - `TestCasesRpc.cpp` will call both, and JSON version runs in both sync and async.
    - May need 3 files to call a shared cases list from the header.
    - Need to re-initiate Workflow global variables, like reload/unload global storage in each test file.
- Solve interface inheritance and casting.
- C++ codegen for rpc data type JSON serialization.
- Delete Runtime test category.
- Update `TestLibraryRpcByval.cpp` in `LibraryTest` to use `RpcDualLifecycleMock`, remove `RpcByvalLifecycleMock`.
  - May need to refactor how to retrieve ops interfaces.
- Sort `WfErrors`.
