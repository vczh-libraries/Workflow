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
