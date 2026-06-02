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
- Check error handling. Ensure crashing instead of covering.

## ToDo

- Redirect list ops to object ops.
  - Review
  - Test case to handle `IRpcListOps` exceptions.
- Review JSON related constructions until the work is done.
- JSON schema for rpc interfaces.
  - JSON async dispatcher with JSON channel
  - logging from JSON channel
  - Triple threading demo only for metaonly. Meanwhile no reflection logs JSON values.
  - TeatCasesRpc.h test macros and include different header files.
- Refactor wrapper generation to use C++ type -> ITypeInfo -> WfType helper, eliminate innecessary helpers.
- Consider how to run a CLI based dual processes demo to test RPC.

## Document

- RPC
  - use `TODO_RPC_(Definition|Json|GeneratedWrappers).md`
  - how to start up
- Compiler processing and passes.
- AST building helper functions.
  - Refactor is needed to scan if some functions in RPC code generation is duplicating those in `WfAnalyzer.h`.
- Workflow best practices, language features and syntax sugars are invented to keep the code clean, prefer them.
