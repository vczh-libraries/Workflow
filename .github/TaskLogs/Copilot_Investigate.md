# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

## Task 1

You are going to add these classes in `WfLibraryRpcJson.(h|cpp)`.
- `RpcJsonLifecycle`, implementing `RpcLifecycleBase`.
- `RpcJsonDispatcher`, implementing `IRpcDispatcher`.
It builds classes that `RunRpcTestCase_JsonRequest` need but keeps general so that it can be reused, with more enhancement.

You will need to update the `[Configuration]` section of `IRpcLifecycle` comment because this task makes it easier.
Just write how `RunRpcTestCase_JsonRequest` configurate `RpcJsonLifecycle` with simiar wording.

You will also need to update `Rpc.d.ts` as new protocols are added, otherwise `Test/Typescript` won't build:
- `Broadcast_Response::rpcMethod` will be `Broadcast_Response` instead of the current value, it becomes a response of all broadcast requests.
- Update all `rpcMethod` to add `Request:` or `Response:` at the beginning to make it clear.
- I have prepared a `BroadcastRequest` and `DirectRequest`, all requests and responses should inherit from these interfaces and remove duplicated members.

### RpcJsonDispatcher

Just like `RpcJsonObjectOps` and `RpcJsonObjectEventOps`, `RpcJsonDispatcher` translates everything to `IRpcJsonMessageDispatcher` along with a `Translate(message, dispatcher, lifecycle)` function:
- `Initialize` and `Finalize` keeps empty.
- `DeclareLocalService` becomes a broadcast request, when `Translate` receives it, it calls `IRpcLifecycle::DeclareRemoteService`.
- `BroadcastFromClient_ObjectEventOps` returns `RpcJsonObjectEventOps`.
- `SendToClient_ObjectOps` returns `RpcJsonObjectOps`.
- Delete `IRpcDispatcher::RequestService`:
  - Currently only `RpcLifecycleBase::RequestService` is calling this function.
  - Now when requesting for a remote service, it is going to just constructor a `RpcObjectReference` and let `RefToPtr` does the job.
  - You will find that those information are just enough, it should be no need to call `IRpcDispatcher::RequestService` from the beginning.

### RpcJsonLifecycle

- `Register` function:
  - parameters:
    - `Ptr<IRpcSerializer> _serializer`
    - `Ptr<IRpcObjectOps> _objectOps`
    - `Ptr<IRpcObjectEventOps> _objectEventOps`
    - `Func<void(RpcObjectReference, reflection::IDescriptable*)> _eventAttacher`
  - calls `GetController()->Register` with arguments:
    - `new RpcCalleeObjectOpsForList(new RpcCalleeListOps(this, _serializer), _objectOps, _serializer)`
    - `new RpcCalleeObjectEventOpsForList(new RpcCalleeListEventOps(this, _serializer), _objectEventOps, _serializer)`.
    - All objects that `new` here should be captured with `Ptr` in the class so that they will be automatically deleted.
- `GetSerializer` returns `_serializer` from `Register`.
- `GetDispatcher` returns the constructor parameter `RpcJsonDispatcher*`, but do not change the return type of `GetDispatcher`.
- `AttachLocalObjectEvents` calls `_eventAttacher` when it is not null, otherwise it does nothing.
- Othere 3 arguments from `Register` are assumed non null, no testing or assertion is required.

### Refactoring RunRpcTestCase_JsonRequest

- `RpcDualJsonLifecycleMock` inherits from `RpcJsonLifecycle`, or do not create this class if there is nothing more to add. My idea is that this class should not longer exist, but make your own judgement.
- `RpcDualJsonRequestDispatcherMock` will inherit from `RpcJsonDispatcher`, or delete this class if there is nothing more to add. My idea is that this class should not longer exist, but make your own judgement.
- `RpcDualJsonRequestBridge`inherits from `IRpcJsonMessageDispatcher`, which is pretty much a slice of the current `RpcDualJsonRequestDispatcherMock`:
  - `RpcDualJsonRequestDispatcherMock` used to connect two `RpcDualJsonLifecycleMock`.
  - But in the new design, each client will have its own lifecycle and dispatcher.
  - Now `RpcDualJsonRequestBridge` is going to connect two lifecycles.
  - After `RpcDualJsonRequestBridge` has the two pointer to lifecycles, it could easily read the `sourceClientId` from the message, and just call translate on objects from the other side. The reason is that, a dual version of mock only serve two clients.
    - This is pretty much what has been done in `RpcDualJsonRequestDispatcherMock::OnJsonRequest` but needs some twists.
    - When `targetClientId` exists, you are going to `CHECK_FAIL` that it is different from `sourceClientId`.

### General Refactoring

No need to reset any pointer or shared pointer member to null in the destructor.
You written some destructors like this but it is totally unnecessary.
Find out all code that doing this in `Source/Library/Rpc` and `Test/Source`, clean them.

# TEST [CONFIRMED]

The change is validated by the existing RPC test matrix because the requested reusable classes are exercised by `RunRpcTestCase_JsonRequest`, which is selected by the meta-only reflection configuration. Success criteria:

- `LibraryTest` passes for Debug Win32 and Debug x64.
- `CompilerTest_GenerateMetadata` passes for Debug Win32 and Debug x64 because RPC reflection surface changed.
- `CompilerTest_LoadAndCompile` passes for Debug x64 and regenerates JSON request transcripts with the new protocol envelopes.
- `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` pass for Debug Win32 and Debug x64 after generated files are current.
- `Test/TypeScript/prepare.ps1` followed by `npm run build` passes because `Rpc.d.ts` must match generated JSON request transcripts.
- Static scans find no remaining `IRpcDispatcher::RequestService` implementation or reflection registration, and no obsolete `RpcDualJsonRequestDispatcherMock` usage.

# PROPOSALS [CONFIRMED]

- No.1 Reusable JSON lifecycle and dispatcher [CONFIRMED]

## No.1 Reusable JSON lifecycle and dispatcher [CONFIRMED]

Implement `RpcJsonDispatcher` and `RpcJsonLifecycle` in `WfLibraryRpcJson.(h|cpp)`, move JSON-request test setup to those production classes, and leave only a dual-client `IRpcJsonMessageDispatcher` bridge in tests. Service declarations become broadcast JSON requests, and `RpcLifecycleBase::RequestService` constructs the remote service reference directly from lifecycle-owned remote-service state. The JSON request schema gains `Request:`/`Response:` method names plus a shared `Broadcast_Response`.

### CODE CHANGE

- Added `RpcJsonDispatcher` to translate dispatcher operations through `IRpcJsonMessageDispatcher`, including the new `Request:IRpcDispatcher_DeclareLocalService` broadcast request and `Response:Broadcast_Response` response.
- Added `RpcJsonLifecycle::Register` to own serializer, generated object/event ops, list callee adapters, and optional local-event attachment.
- Removed `IRpcDispatcher::RequestService` from the interface, reflection registration, and dual flat dispatcher mock.
- Changed registered services to use deterministic `RpcObjectReference { clientId, typeId, typeId }`, allowing request-service to call `RefToPtr` without dispatcher lookup.
- Refactored `RunRpcTestCase_JsonRequest` to use per-client `RpcJsonDispatcher`/`RpcJsonLifecycle` plus `RpcDualJsonRequestBridge`.
- Updated `Rpc.d.ts` for request/response-prefixed `rpcMethod` values, shared request bases, and shared broadcast response.

### CONFIRMED

- `copilotBuild.ps1 -Configuration Debug -Platform Win32`: passed.
- `copilotBuild.ps1 -Configuration Debug -Platform x64`: passed.
- `LibraryTest` Debug Win32 and Debug x64: passed.
- `CompilerTest_GenerateMetadata` Debug Win32 and Debug x64: passed.
- `CompilerTest_LoadAndCompile` Debug x64: passed and refreshed reflection metadata.
- `RuntimeTest` Debug Win32 and Debug x64: passed.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` Debug Win32 and Debug x64: passed.
- `Test/TypeScript/prepare.ps1` and `npm run build`: passed.
- After removing an unused bridge helper, reran `copilotBuild.ps1 -Configuration Debug -Platform Win32`, `copilotBuild.ps1 -Configuration Debug -Platform x64`, `CppTest_Metaonly` Debug Win32/x64, and TypeScript prepare/build: all passed.
- Static scans found no obsolete unprefixed JSON RPC method names, no remaining dispatcher-side `RequestService(vint)` contract or reflection entry, and no destructor pointer-reset cleanup left in `Source/Library/Rpc` or `Test/Source`.
