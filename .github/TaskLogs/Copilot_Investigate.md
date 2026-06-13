# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

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

# UPDATES

# TEST [CONFIRMED]

The refactor will be confirmed by:
- Static inspection that `ChatBotServer` and `ChatBotClient` no longer define `ChatBotTaskThread` or `GetRpcChannel`, and directly index `GetChannels()[WString::Unmanaged(RpcChannel)]`.
- Static inspection that `ChatBotServer` no longer uses `(Begin|End)AcceptingLocalClient` or `RegisterLocalClient`, and that the server dispatcher reads the first local client id from `IChannelClient::GetClientId`.
- Static inspection that ChatBot event output is shared from `ChatBotHttp.cpp`, lock fields in `ChatBotJsonDispatcher(Client|Server).h` have coverage comments, and no dispatcher implementation subclasses remain in `main.cpp`.
- Static inspection that the HTTP transport object and `JsonNetworkChannelServer` are started/stopped separately instead of one `ChatBotHttpChannelServer` inheriting from both.
- Static inspection that redundant empty `IChannelClient` / `IChannelServer` overrides are removed from ChatBot and `VlppOS` test code where the base classes already provide defaults.
- Build `VlppOS` `Debug|Win32` and `Debug|x64`, then run the focused or full `UnitTest` including `TestInterProcess.cpp`.
- Regenerate or release `VlppOS` into its `Release` output, copy the non-`IncludeOnly` release files into `Workflow/Import`, and build Workflow.
- Build Workflow unit-test solution in `Debug|Win32` and `Debug|x64`.
- Run `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` in the required configurations.
- Run `Test/TypeScript/prepare.ps1` and `npm run build`.
- Run the ChatBot multi-process procedure with one server and three clients named `Tom`, `Jerry`, and `Spike`: join one at a time, verify join notifications, send three chat rounds, exit one client, verify leave notifications, exit the server, and confirm the remaining clients exit.

# PROPOSALS

- No.1 Collapse redundant ChatBot transport wrappers and run RPC work on the main task queue [CONFIRMED]

## No.1 Collapse redundant ChatBot transport wrappers and run RPC work on the main task queue

Move the remaining `ChatBotJsonDispatcher(Server|Client)` subclasses into concrete `Json(Network|Local)ChannelClient` subclasses so the clients own dispatcher initialization, channel binding, service registration, and server/local connection setup. Let `main` call `TaskQueue::RunTaskQueue()` after setup instead of starting a dedicated task thread, so RPC service methods and event handlers run on the main thread.

Split reusable ChatBot helpers into `ChatBotHttp.h` / `ChatBotHttp.cpp`: JSON parser creation and common `IChatServer` event subscriptions live out of line, and both applications use the same event output helper. The server owns `HttpServer` and `JsonNetworkChannelServer` separately, connects its broadcast-redirect local client first, connects the service local client second, and starts HTTP after local RPC setup.

Simplify channel access and connection validation by removing `GetRpcChannel`, `(Begin|End)AcceptingLocalClient`, and `RegisterLocalClient`. For dependency cleanup, rely on the already-defaulted channel callbacks in `NetworkProtocolChannelClientBase` / `NetworkProtocolChannelServer`, delete redundant empty overrides from `VlppOS` tests, release `VlppOS`, and import the regenerated files into Workflow.

### CODE CHANGE

Implemented the ChatBot refactor in the server and client applications:
- Removed `GetRpcChannel` helpers and now uses `GetChannels()[WString::Unmanaged(RpcChannel)]` at the call sites that need the RPC channel.
- Removed `ChatBotTaskThread`; both applications set up RPC and console workers first, then call `TaskQueue::RunTaskQueue()` directly on the `main` thread.
- Added `mainThreadId` in `ChatBotHttp.cpp` and checks in all `ChatServerService` methods, shared `IChatServer` event handlers, and the client `OnServerShutdown` event handler.
- Moved common parser creation and `IChatServer` event handler wiring to the new out-of-line `ChatBotHttp.cpp`, shared by both ChatBot projects.
- Replaced the old `ChatBotHttpChannelServer` multiple-inheritance shape with a `ChatBotHttpServer` adapter over a separate `JsonNetworkChannelServer`, and starts/stops the HTTP server and JSON channel server independently.
- Reworked ChatBot dispatchers so the concrete channel clients own nested `Dispatcher` classes, expose dispatcher getters, and encapsulate local/network connection setup.
- Replaced `RegisterLocalClient`, `(Begin|End)AcceptingLocalClient`, pending login buffering, and the old local-client flag with a first server local client whose id is read directly from `IChannelClient::GetClientId`.
- Added explicit server-client-id propagation for the second local service client and an internal logout system message so a client that intentionally exits is removed from server broadcast bookkeeping immediately.
- Added required `// covers ...` comments for all dispatcher locks.

Implemented the VlppOS cleanup and release:
- Added default empty implementations to `INetworkProtocolCallback` methods so test callback classes do not need boilerplate empty overrides.
- Removed unnecessary empty callback overrides from `VlppOS/Test/Source/TestInterProcess.cpp`.
- Regenerated `VlppOS/Release/VlppOS.h` and imported the updated release header into `Workflow/Import/VlppOS.h`.
- Re-ran Workflow release generation, updating the generated `Release` files.

### CONFIRMED

Confirmed by static inspection:
- No remaining `GetRpcChannel`, `ChatBotTaskThread`, `(Begin|End)AcceptingLocalClient`, `RegisterLocalClient`, old `ChatBotHttpChannelServer`, stale `SchedulaTask`, or temporary shutdown markers in the ChatBot sources.
- No remaining redundant empty `OnReadError`, `OnLocalError`, `OnConnected`, or `OnDisconnected` overrides in the cleaned ChatBot and VlppOS test sources.
- Dispatcher lock fields in `ChatBotJsonDispatcherClient.h` and `ChatBotJsonDispatcherServer.h` now have `// covers ...` comments.
- `git diff --check` exits successfully in both Workflow and VlppOS.

Confirmed by build and test:
- `VlppOS` `Debug|Win32` and `Debug|x64` build passed.
- `VlppOS` `UnitTest` `Debug|Win32` and `Debug|x64` passed: 12 files, 117 cases.
- `Workflow` `Debug|Win32` and `Debug|x64` build passed after the final ChatBot changes.
- `Workflow` unit tests passed in the earlier focused Debug sweep for `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`.
- `Test/TypeScript/prepare.ps1` and `npm run build` passed.
- `C:\Code\VczhLibraries\Tools\Tools\Build.ps1 Workflow` passed after the final source changes, including Release Win32/x64 unit tests, TypeScript install/build, Workflow release, and CppMerge.
- The standard ChatBot procedure passed in `Debug|x64` and again in `Release|x64`: one server plus clients `Tom`, `Jerry`, and `Spike`; clients joined one at a time; join notifications were delivered; three chat rounds from all clients were broadcast; `Jerry` exited and the others saw the leave notification; server exit shut down the remaining clients; all four processes exited with code 0.
