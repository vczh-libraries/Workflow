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

- `IChannel::(SendTo|BroadcastFrom)Client` should remove the `senderClientId` argument because they should know.
  - `BroadcastFromClient` add a blocked client id list argument.
- `ChatBotServer` and `ChatBotClient` project.
  - Declaration of service registration from every client connection.
    - A dedicated server is used.
    - When a client is connected to the server, a message containing the local client id of the server is sent.
    - A new method will be added to `RpcJsonDispatcherChannel` to wait for this message, declare all required services, and wait for another message to unblock.
  - Broadcasting requests:
    - All broadcast requests are sent to this local client, which will be redirected to all client.
      - Pay attention to potential race condition when broadcasting and accepting new clients at the same time.
    - After all clients response, the local client will response the consolidated result.
  - Update `UnitTest/README.md` and `Project.md`.

## Knowledge Base

- RPC
  - use `TODO_RPC_(Definition|Json|GeneratedWrappers).md`
  - how to start up
- Compiler processing and passes.
- AST building helper functions.
  - Refactor is needed to scan if some functions in RPC code generation is duplicating those in `WfAnalyzer.h`.
