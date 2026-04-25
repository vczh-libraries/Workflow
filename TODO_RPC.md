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
  - Simpler collection cases using property, testing if property attributes applied to both getter return values and setter arguments.
    - Use dynamic properties, ignore the fact that caching is not implemented.
    - `_Default.txt` -> `_PropDefault.txt`, `_PropByref.txt`, `_PropByval.txt`.
  - Observable list.
  - Observable list should by default `@rpc:Byref`.
  - Destructors in collection elements.
  - Cached properties (by default)
  - Dynamic properties
  - Sending cached property values proactively from remote to wrapper.
  - Events
- Generate JSON from "RPC metadata in Workflow syntax".
  - Generate with wrappers.
- Solve interface inheritance and casting.
- C++ codegen for rpc data type JSON serialization.
- Delete Runtime test category.
- Update `TestLibraryRpcByval.cpp` in `LibraryTest` to use `RpcDualLifecycleMock`, remove `RpcByvalLifecycleMock`.
  - Refactor `RpcDualLifecycleMock` to make it a common implementation for `GacUI`.
  - May need to refactor how to retrieve ops interfaces.
