# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.

## Investigation

- Verify `WfErrors::Cpp*`, probably not needed as errors should have been catched in eariler phases.
- `WfCpp_WriteReflection.cpp` is changed because `INVOKE(GET?)_INTERFACE_PROXY` does not handle overloading property. This should be a VlppParser bug. Fix the bug and revert this file.
  - https://github.com/vczh-libraries/Workflow/commit/7b0e54a1964774cbb011ea1e675aa8c72a3fbec4
- Can `WfInterfaceInstance` not inherit from `IDescriptable`?
- Check error handling. Ensure crashing instead of covering.

## ToDo

- `ChatBotServer` and `ChatBotClient` project.
  - Declaration of service registration from every client connection.
  - How does `InvokeEvent` collect results from all client? Might need to update `IChannel::BroadcastFromClient` and enable a return value.
    - Or just ask the server local channel to dispatch and consolidate.
  - Update `UnitTest/README.md` and `Project.md`.

## Knowledge Base

- RPC
  - use `TODO_RPC_(Definition|Json|GeneratedWrappers).md`
  - how to start up
- Compiler processing and passes.
- AST building helper functions.
  - Refactor is needed to scan if some functions in RPC code generation is duplicating those in `WfAnalyzer.h`.
