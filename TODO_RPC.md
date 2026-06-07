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

- `ChatBotServer` and `ChatBotClient` project.
  - Declaration of service registration from every client connection.
    - Delete `IRpcDispatcher::IsRegisteredService`.
    - Add `IRpcLifecycle::RegisterLocalService`, registering a local object as a service.
    - Add `IRpcLifecycle::GetRegisteredLocalServices`, a dictionary.
    - Add `IRpcLifecycle::RequestService`, returns the local registered service or call `IRpcDispatcher::RequestService`.
    - Add `IRpcLifecycle::Initialize` calling `IRpcDispatcher::Initialize`. `RegisterLocalService` will throw after that.
    - Delete `IRpcDispatcher::RegisterService`.
    - Add `IRpcDispatcher::Initialize`. All mocks keeps an empty implementation.
      - In the real implementation, all local registered service will be sent to the server, and `IRpcLifecycle::NewRemoteServices(clientId, typeIds)` will be called whenever a remote service appears.
  - How does `InvokeEvent` collect results from all client? Might need to update `IChannel::BroadcastFromClient` and enable a return value.
  - Update `UnitTest/README.md` and `Project.md`.

## Knowledge Base

- RPC
  - use `TODO_RPC_(Definition|Json|GeneratedWrappers).md`
  - how to start up
- Compiler processing and passes.
- AST building helper functions.
  - Refactor is needed to scan if some functions in RPC code generation is duplicating those in `WfAnalyzer.h`.
