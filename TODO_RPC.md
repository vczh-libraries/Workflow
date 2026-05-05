# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.

## Investigation

- Figure out what `decideTypeId` in `RunRpcTestCase` does and see if there is a better way.
  - It is used to call `RegisterLocalObject`, unfortunately the current implementation can't make a local object tells the lifecycle what it type id is.
- Verify `WfErrors::Cpp*`, probably not needed as errors should have been catched in eariler phases.
- `WfCpp_WriteReflection.cpp` is changed because `INVOKE(GET?)_INTERFACE_PROXY` does not handle overloading property. This should be a VlppParser bug. Fix the bug and revert this file.
  - https://github.com/vczh-libraries/Workflow/commit/7b0e54a1964774cbb011ea1e675aa8c72a3fbec4
- Can `WfInterfaceInstance` not inherit from `IDescriptable`?

## ToDo

- Continue to add more test cases until all features are covered.
  - Sending cached property values proactively from remote to wrapper.
  - Interface inheritance.
- Generate JSON from "RPC metadata in Workflow syntax".
  - Review JSON related constructions.
  - Make sure arguments are serialized to expected JSON representation.
    - Make a `RpcDualDispatcherJson` to record all JSON objects, code gen ts files and create a npm project to build against the schema, just like VlppParser2.
- Refactor wrapper generation to use C++ type -> ITypeInfo -> WfType helper, eliminate innecessary helpers.
- Document RPC
  - use `TODO_RPC_(Definition|Json|GeneratedWrappers).md`
  - how to start up
- Document compiler process.
- Document AST building helper functions.
  - Refactor is needed to scan if some functions in RPC code generation is duplicating those in `WfAnalyzer.h`.
- Document Workflow best practices, language features and syntax sugars are invented to keep the code clean, prefer them.
