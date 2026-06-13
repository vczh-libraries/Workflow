# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

investigate repro

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.
This is a multi-task request, follow the investigation instructions precisely.

## Task 1

This task is on `Workflow` repo.

Refactor related code affecting `ChatBot(Server|Client)` projects.
To ensure the refactoring works, all test cases should pass, and the standard procedure of testing `ChatBot(Server|Client)` is also required.

We got 3 `Dispatcher` classes for `ChatBotChannelClient`, `ChatBotServiceChannelClient` and `ChatBotServerChannelClient`.
- These 3 classes are pretty similar.
- We should add `RpcJsonDispatcher(Server|Client)ForTaskQueue` inheriting `ChatBotJsonDispatcher(Server|Client)` putting under them.
  - Be awared that `ChatBotJsonDispatcher(Server|Client)` will be renamed and splitted, so these `*ForTaskQueue` would be in `RpcJsonDispatcher(Server|Client).(h|cpp)`.
- We can just delete these 3 `Dispatcher` classes.

`ChatBotJsonDispatcherClient` should not contain `ops` and `chatServer`:
- Instead it should expose `IRpcLifecycle` and `IRpcDispatcher` via public `GetRpcLifecycle` and `GetRpcDispatcher`.
- `chatServer` is not used in `ChatBotJsonDispatcherClient` at all.
  - In `ChatBotServer` project, it should limit `chatServer` in `Main.cpp`, store it only in who needing it, therefore `ChatBotJsonDispatcherClient::RegisterLocalService` could be deleted.
  - In `ChatBotClient` project, `lifecycle->RequestService` could be called in `Main.cpp`, and `ChatBotJsonDispatcherClient::GetChatServer` could be deleted.
  - `ChatBotJsonDispatcherClient::Initialize` no need to handle this.

Rename `ChatBotJsonDispatcherServer` class and files to `RpcJsonDispatcherServer`, using namespace `vl::rpc_controller::channeling` instead of `chatbot`:
- It looks like to have nothing to do with chatbot.

Split `ChatBotJsonDispatcherClient` class and files to `RpcJsonDispatcherClient` and `ChatBotJsonDispatcherClient`:
- `RpcJsonDispatcherClient` uses namespace `vl::rpc_controller::channeling`.
- `ChatBotJsonDispatcherClient` still uses namespace `chatbot`.
- The goal is to keep `RpcJsonDispatcherClient` not knowing anything about `chatbot`.
- Therefore `ChatBotJsonDispatcherClient::InitializeRpc` and any functions that depending on the knowledge of `chatbot` should be in `ChatBotJsonDispatcherClient`.
  - I suspect `InitializeRpc` is the only function that needs to move.
  - `InitializeRpc` could be called in `IChannelClient::OnConnected`.
  - `ChatBotJsonDispatcherClient::(WaitForServer|ConnectLocalClient)` will no need to call `InitializeRpc` anymore.

`RpcJsonDispatcher(Client|Server)` should make all protected fields and methods private, as your best effort. If anything needs to be used in sub classes, add a protected or public getter.

The goal is to create a more general implementation, so that it could be reused with other RPC definitions. To make sure that the refactoring works:
- `CreateChatBotJsonParser` could be removed, just call `Ptr(new glr::json::Parser)` in all places using this function.
- A `RpcJsonDispatcherShared.h` for all `using` statements.
- Leaving 3 constexpr, `mainThreadId` and `AttachChatServerEventHandlers` still in `ChatBotHttp.(h|cpp)`.
- `RpcJsonDiaptcher(Server|Client).(h|cpp)` should use `RpcJsonDispatcherShared.h`, making sure that these 5 files do not include `ChatBotApp.h`, that's how we know they don't depend on `ChatBotApp` generated code.
- `ChatBotJsonDispatcherClient.(h|cpp)` only uses `RpcJsonDiaptcherClient.h` and `ChatBotApp.h`.
- `Main.cpp` could use all files.

## Task 2

This task is on `VlppOS` repo.
`NetworkProtocolChannelServer` has a serious design problem. It should use a `INetworkProtocolServer` insteads of implementing one.
But there are many other `INetworkProtocolServer` implementations, like `HttpServer` or `NamedPipeServer`, these should be `NetworkProtocolChannelServer`'s base class.
So you could declare the class like this:

```C++
template<typename TPackage, typename TSerialization, typename TServerBase>
class NetworkProtocolChannelServer : public TServerBase, public virtual IChannelServer<TPackage>
{
...
public:
  typename<typename ...TArgs>
  NetworkProtocolChannelServer(TArgs&& ...args)
    : TServerBase(std::forward<TArgs&&>(args)...)
    , ...
  {
    ...
  }

  ...
}
```

Add a `IChannelClient* localClient` to `IChannelServer::OnClientConnected` as the last argument.
It will not non-null when the client is a local client.
It will be null otherwise.

## Task 3

This task is on `Workflow` repo.
Release `VlppOS` to `Workflow`.
`Workflow` will be affected by `Task 2`, you need to refactor the code.

Rename `ChatServerService` to `ChatServerImpl`:
- `OnClientConnected` now will be able to know if it is `ChatBotServerChannelClient` by doing `dynamic_cast`, no need to do complicated tests.
- Since `HttpServer` being able to be a base class of `NetworkProtocolChannelServer`, `ChatBotChannelServer` and `ChatBotHttpServer` now merges into one.

## Task 4

This task is on `GacUI` and `GacJS` repo.
Release `VlppOS` and `Workflow` to `GacUI`.

Follow `Tools/DebugGacUIWithRemoteProtocol.md` to make sure `RemotingTest*` test projects are working.
Follow `Tools/DebugGacUIWithBrowser.md` to make sure `GacJS` is working with `RemotingTest_Core`.

# UPDATES

# TEST

The refactor will be confirmed task-by-task:

- Task 1: Workflow builds and tests after the ChatBot dispatcher split; static scans confirm reusable `RpcJsonDispatcher(Client|Server)` files do not include `ChatBotApp.h`, `ChatBotJsonDispatcherClient` only depends on `RpcJsonDispatcherClient.h` and `ChatBotApp.h`, the three nested ChatBot `Dispatcher` classes are removed, `CreateChatBotJsonParser` is removed, and the ChatBot multi-process procedure passes with one server and three clients.
- Task 2: VlppOS builds and `UnitTest` passes after `NetworkProtocolChannelServer` is templated over its server base, `IChannelServer::OnClientConnected` exposes `localClient`, and local/network clients pass correct nullability.
- Task 3: Workflow imports the regenerated VlppOS release, adapts to the new callback signature, merges the ChatBot channel server with the HTTP-backed server, renames `ChatServerService` to `ChatServerImpl`, builds, tests, and passes the ChatBot procedure again.
- Task 4: GacUI imports regenerated VlppOS and Workflow release files, builds and verifies `RemotingTest_Core` / `RemotingTest_Rendering_Win32` per `Tools/DebugGacUIWithRemoteProtocol.md`, then GacJS builds/tests and verifies browser interop with `RemotingTest_Core` per `Tools/DebugGacUIWithBrowser.md`.

# PROPOSALS

- No.1 Split generic RPC JSON dispatching from ChatBot-specific setup
- No.2 Release VlppOS channel-server changes into Workflow

## No.1 Split generic RPC JSON dispatching from ChatBot-specific setup

Introduce reusable `vl::rpc_controller::channeling` dispatcher classes for channel-backed JSON RPC. Keep generated ChatBot service registration and event attachment in the ChatBot wrapper, then update app mains and channel clients to ask the lifecycle/dispatcher directly for RPC services. This preserves the current task-queue transport behavior while removing generated ChatBot dependencies from the reusable dispatcher layer.

### CODE CHANGE

[CONFIRMED]

- Renamed the reusable server dispatcher to `RpcJsonDispatcherServer` in namespace `vl::rpc_controller::channeling`, with `RpcJsonDispatcherServerForTaskQueue` carrying the task-queue scheduling policy.
- Split the client dispatcher into reusable `RpcJsonDispatcherClient` plus the ChatBot-only `ChatBotJsonDispatcherClient`, keeping generated `ChatBotApp` setup in `InitializeRpc`.
- Added `RpcJsonDispatcherShared.h` for common JSON/channel aliases and removed `CreateChatBotJsonParser`; callers now construct `Ptr(new glr::json::Parser)`.
- Removed the three nested ChatBot `Dispatcher` classes and replaced them with the new `*ForTaskQueue` dispatchers.
- Removed `ops`, `chatServer`, `RegisterLocalService`, and `GetChatServer` from the reusable client dispatcher; `Main.cpp` now requests/registers services through `GetRpcLifecycle`.
- Kept `ChatBotHttp.(h|cpp)` to constants, `mainThreadId`, and event handlers.

### TEST

[CONFIRMED]

- Built `Workflow/Test/UnitTest/UnitTest.sln` Debug Win32 and Debug x64 with `copilotBuild.ps1`: both passed with 0 errors.
- Ran `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` with `copilotExecute.ps1`: all required Win32/x64 runs passed.
- Ran `Test/TypeScript/prepare.ps1` and `npm run build`: passed.
- Ran the ChatBot interactive procedure through `copilotExecute.ps1`: server plus clients `Tom`, `Jerry`, and `Spike`; verified join notifications, three chat rounds from each client, one client exit notification, and server shutdown ending the remaining clients.
- Static scan confirmed `RpcJsonDispatcherShared.h`, `RpcJsonDispatcherClient.(h|cpp)`, and `RpcJsonDispatcherServer.(h|cpp)` do not include or reference `ChatBotApp`; `ChatBotJsonDispatcherClient.h` includes only `RpcJsonDispatcherClient.h` and `ChatBotApp.h`.
- Attempted `..\Tools\Tools\Build.ps1 Workflow`; it reached the release rebuild phase but timed out after 30 minutes without diagnostic output. The orphaned Release x64 build process was stopped. This aggregate pass is recorded as inconclusive; the targeted test matrix above passed.

## No.2 Release VlppOS channel-server changes into Workflow

Import the regenerated VlppOS channel-server API and adapt the ChatBot server sample to the new transport-base template. Collapse the custom HTTP forwarding server into the channel server itself, and use the new `localClient` argument to recognize the embedded `ChatBotServerChannelClient` directly.

### CODE CHANGE

[CONFIRMED]

- Imported regenerated `VlppOS.h` from VlppOS release commit `c93cf95`.
- Changed the shared JSON channel-server alias to `JsonNetworkChannelServer<TServerBase>`.
- Renamed `ChatServerService` to `ChatServerImpl`.
- Changed `ChatBotChannelServer` to derive from `JsonNetworkChannelServer<HttpServer>` and removed the separate `ChatBotHttpServer` adapter.
- Updated `ChatBotChannelServer::OnClientConnected` to accept the new `localClient` argument, `dynamic_cast` the server-side local client, and register only ordinary RPC clients once the dispatcher is ready.

### TEST

[CONFIRMED]

- Built `Workflow/Test/UnitTest/UnitTest.sln` Debug Win32 and Debug x64 with `copilotBuild.ps1`: both passed with 0 warnings and 0 errors.
- Ran `LibraryTest` Debug Win32/x64: both passed 2/2 files and 15/15 cases.
- Ran `CompilerTest_GenerateMetadata` Debug Win32/x64: both passed 1/1 files and 2/2 cases.
- Ran `CompilerTest_LoadAndCompile` Debug x64: passed and produced no tracked generated-file changes.
- Ran `RuntimeTest` Debug Win32/x64: both passed 4/4 files and 261/261 cases.
- Ran `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` Debug Win32/x64: all six runs passed 2/2 files and 229/229 cases.
- Ran `Test/TypeScript/prepare.ps1` followed by `npm run build`: passed.
- Ran the ChatBot standard procedure through `copilotExecute.ps1 -Mode CLI`: server plus `Tom`, `Jerry`, and `Spike`; verified ordered joins, join notifications to existing clients, three chat rounds from each client propagated to server and all clients, one client exit notification, and server shutdown ending remaining clients.
