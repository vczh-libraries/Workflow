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
  - Review.
  - `IRpcJsonMessageDispatcher::DefaultDispatch` so that the parsing part of each `JsonRequest` implementation could be shared.
  - `ChatBotJsonDispatcherBase` implementa `IChannelReader` and only accept an `IChannel` in the constructor.
  - `Http(Server|Client)` should be created in each `Main.cpp`.
  - `ChatBotJsonDispatcherServer` exposes functions for all kinds of registration, no longer handling connection verification.
  - Extract task queue to a class, `ChatBotJsonDispatcherBase` no longer use the task queue directly.
    - In each `Main.cpp` a task queue is created, `ChatBotJsonDispatcherBase` will call back `Main.cpp` handles the actual thread dispatching work.
    - Move the task queue to `VlppOS`.
  - Declaration of service registration from every client connection.
  - Extract from the shared code as many as possible into the library.

## Knowledge Base

- New task queue in VlppOS
  - A dedicated guidelines for VlppOS oriented synchronization best practice.
  - A dedicated guidelines for codegen tools, including input output format and .d.ts
- RPC
  - use `TODO_RPC_(Definition|Json|GeneratedWrappers).md`
  - how to start up
  - Update `document-and-commit.md` to maintain those KB files instead.
- Compiler processing and passes.
- AST building helper functions.
  - Refactor is needed to scan if some functions in RPC code generation is duplicating those in `WfAnalyzer.h`.
