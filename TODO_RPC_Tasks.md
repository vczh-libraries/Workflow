investigate repro

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

Refactor related code affecting `ChatBot(Server|Client)` projects.
To ensure the refactoring works, all test cases should pass, and the standard procedure of testing `ChatBot(Server|Client)` is also required.

The `GetRpcChannel` function is not necessary, by just using `client->GetChannels()[WString::Unmanaged(RpcChannel)]` it has the same behavior, aka crash when not found.
Find out similar thing and simplify the source code.

`ChatBotTaskThread` is also not necessary. There is two reason:
- It doesn't need to `Start()` when queued tasks don't really need to run at that moment.
- Calling `QueueExitTask` should just let `RunTaskQueue` stops.
So try to remove this class, and instead setup everything and then call `RunTaskQueue` in the `main` function directly, which will just behave like `Thread::Wait`.

Inside all `ChatServerService` methods, and all event handlers to `IChatServer`'s events:
- At the beginning of `main`, `Thread::GetCurrentThreadId`'s result will be stored to a global variable `mainThreadId`.
- All these places are going to `CHECK_ERROR(mainThreadId == Thread::GetCurrentThreadid(), ERROR_MESSAGE_PREFIX L"...")`.
To make sure that everything is actually called in the same thread.
By letting `main` call `RunTaskQueue`, all service accessing should be in the same thread, which is the thread calling `main` function. These two works are related.

I don't like how `ChatBotHttpChannelServer` inheriting from `HttpServer`:
- `HttpServer`'s sub class should be a concrete class which talking to `JsonNetworkChannelServer`.
- I believe there is no need to override `OnClientConnected(INetworkProtocolConnection*)` to do only redirection to the base implementation, try to delete it.
- `main` is going to start `HttpServer` and `JsonNetworkChannelServer` separately.
- I guess a `JsonNetworkChannelServer` subclass may also be not needed by doing this.

Both `main` functions listen to `IChatService`'s events, but the code is the same, I would like a function in `ChatBotHttp.h` handling this.
And create a `ChatBotHttp.cpp` to pair with it to avoid inline functions.

In `ChatBotServer`'s `main` function `(Begin|End)AcceptingLocalClient` is called twice.
According to the document `TODO_RPC_JsonRequest.md` and how `ChatBotJsonDispatcherServer` handles connection:
- It is easy to find that, there is no matter the client is connecting first or `IChatServer` implemention is connecting first.
- If `IChatServer` is registered before any client connection, the client should just unblock immediately.
- If a client is connecting too fast, the client will wait for `IChatServer` to connect.
- Being able to request for the `IChatServer` service is the timing to start `ChatBotInputThread`, therefore it is easy to handle user inputed names after `IChatServer` is available.
Therefore in `ChatBotServer`'s `main` function you can do the setup in this order:
- Setup dispatcher and everything RPC related.
- Connect a `JsonLocalChannelClient` to handle broadcasted request redirection (using `ChatBotJsonDispatcherServer`), let's call it `1st server client`.
- Connect a `JsonLocalChannelClient` to handle `IChatServer` (using `ChatBotJsonDispatcherClient`), let's call it `2nd server client`.
- These two clients should be two different sub class, so they to make the responsibility clear.
You don't need to set `RegisterLocalClient`, instead let the dispatcher knows pointers to the two clients:
- When a client id of `1st server client` is needed, it reads `IChannelClient::GetClientId` directly, and `CHECK_ERROR` to make sure it is not `-1`.
`(Begin|End)AcceptingLocalClient` and the flag will be not necessary at all:
- `main` is a local function, informations about if registering a local client is good, can be inferred. No need to do guarding like that.
- By doing the setup like this, `dispatcher` will always be non-null and `RpcChannel` is always there anyway.
- you can just keep the `RpcChannel` guard and drop the other two.
- If you need to know if a `clientId` is a local client, you could just call `IChannelServer::IsLocalClient` function.
- By starting the server after registering the first `ChatBotJsonDispatcherClient`, you can ensure the first `OnClientConnected` call will be the `1st server client`.

In `ChatBotJsonDispatcher(Server|Client).h` all locks are missing the `// covers ...` comments required by the coding guidelines.

Currently there are one `JsonNetworkChannelClient` sub class, one `JsonLocalChannelClient` sub class, one `ChatBotJsonDispatcherServer` sub class, two `ChatBotJsonDispatcherClient` sub class:
- In the new design, there will be one `JsonNetworkChannelClient` sub class and two `JsonLocalChannelClient` sub class.
- Sub classes of `ChatBotJsonDispatcher(Server|Client)` becomes a internal class called `Dispatcher` in sub classes of `Json(Network|Local)ChannelClient`.
- Client will handle the initialization of dispatcher as many as it can:
  - Try to expose a function on each 3 client class, that could initialize the dispatcher, connect to the server, and do whatever is needed after that.
  - Client will expose the dispatcher as a field from a getter, so that `main` function could use it.
- `main` functions are too messy, use `// ---- DESCRIPTION ----` to group initializations of different group of source code by its semantic.

To avoid overriding functions just for `IChannel(Server|Client)` with empty implementation:
- `NetworkProtocolChannelClientBase` and `NetworkProtocolChannelServer` should override them with empty implemention, for functions that they do not implement.
- Delete unnecessary empty implementations in `VlppOS`'s `TestInterProcess.cpp`, to make sure the change is working.
- Release `VlppOS` to `Workflow` to clean up two `main.cpp`.
