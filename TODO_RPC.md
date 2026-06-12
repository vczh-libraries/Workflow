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
  - Declaration of service registration from every client connection.
    - `ChatBotJsonDispatcherClient`, give a channel and it registers itself as a reader, initiating the registration process:
      - When receiving `DeclareLocalService` it is written down, unless the registered service list is sent, it throws.
      - A new method `DeclareLocalService` is created for this, no more passing `DeclareRemoteService` directly.
      - Send required service list and registered service list.
      - Block until receiving enough `DeclareRemoteService`.
    - `ChatBotJsonDispatcherServer`, only offer central controlling and broadcast message redirecting, must be the first registered client:
      - Exposed a method so that the server could notice a connected client.
      - When a client is connected with registered services, `DeclareRemoteService` will be broadcasted. Synchronization required.
  - Extract from the `ChatBotJsonDispatcher*` as many as possible into the library.

## Knowledge Base

- A dedicated guidelines for VlppOS oriented synchronization best practice.
- A dedicated guidelines for codegen tools, including input output format and .d.ts
- RPC
  - use `TODO_RPC_(Definition|Json|GeneratedWrappers).md`
  - how to start up
  - Update `document-and-commit.md` to maintain those KB files instead.
- Compiler processing and passes.
- AST building helper functions.
  - Refactor is needed to scan if some functions in RPC code generation is duplicating those in `WfAnalyzer.h`.
