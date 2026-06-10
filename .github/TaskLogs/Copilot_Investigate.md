# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

## Task 1

`CompilerTest_LoadAndCompiler` builds `Test/Apps/Chatbot/ChatAPI.txt`, updating protocols between `ChatBotServer` and `ChatBotClient`.

### Connecting RPC with Channel

Sharable code between client and server should be put in `Test/UnitTest/ChatBotServer/Shared`:
- `ChatBotJsonDispatcherBase` implementing `IRpcJsonMessageDispatcher`, doing extra network communicating, using `vl::inter_process`.
- `ChatBotJsonDispatcherClient` implementing `ChatBotJsonDispatcherBase`, with additional login in process.
- `ChatBotJsonDispatcherServer` implementing `ChatBotJsonDispatcherBase`, with additional server utilities.
- The underlying implementation should be `vl::inter_process::Http(Server|Client)`, but `ChatBotJsonDispatcher` should not knows it.
- Configuration is similar to `RunRpcTestCase_JsonRequest` but now different clients are running in different processes.
- `RpcChannel` is the only channel name in use.

Extra method `ChatBotJsonDispatcherServer::Start`, starting a `IChannelServer`. A `Ptr<IChannelServer>` is passed via constructor argument.
- A local client is registered, remember its client id.
- It hosts `IChatServer` as a registered service.
- When accepting a client, verify if it has the `RpcChannel` channel, if not drop the connection immediately.

Extra method `ChatBotJsonDispatcherClient::LoginClient`, with a `Ptr<IChannelClient>` passed via constructor argument:
- After its `IChannelClient` connecting to `IChannelServer`, it waits for the first message from the server telling the client the client id of its local client, which is used to communicate with system messages.
- Direct messages will be sent to the actual client.

When `ChatBotJsonDispatcherClient` needs to broadcast a message:
- It sends a direct message to the local client id from the server (aka `ChatBotJsonDispatcherServer`).
- `ChatBotJsonDispatcherServer` actually broadcasts messages to all client except the sender, by putting the original client id to block list, with a new `rpcRequestId`.
- All `ChatBotJsonDispatcherClient` responses to this message, a `Broadcast_Response` will be created, sent back to `ChatBotJsonDispatcherServer`.
- `ChatBotJsonDispatcherServer` consolidate all responses into one, by putting all `Broadcast_Response::response` map into one, and when all responses are null, keeps it null.
- The consolidated response is sent back to the original `ChatBotJsonDispatcherClient`, using the original `rpcRequestId`.

`rpcRequestId` needs to be generated when any `ChatBotJsonDispatcher(Client|Server)` initiate a message, which means `rpcRequestId` tights to the real sender it. When `ChatBotJsonDispatcherServer` redirects broadcasted messages, all `ChatBotJsonDispatcherClient` will read `sourceClientId` the same to `ChatBotJsonDispatcherServer`.

Each `IRpcLifecycle` has a client id, it could just use the one returning from `IChannelClient`.

The critical thing is that, `ChatBotJsonDispatcherServer` can only responses to the original client when all responses comes, and multiple broadcast messages will happen simutaneously. So it needs to maintain a map:
- Key is `clientId x requestId`, identify a broadcast message from a client.
- Value is a list of `expectedClientIdList x clientId x response`, storing which client has reponsed.
- Since `expectedClientIdList` is all clients except the original one, it is easy to retrieve, but clients may keep logging, I suggest `SPIN_LOCK` to the same lock is applied to these places:
  - When accepting a client, SPIN_LOCK { add the client to the list }.
  - When redirecting a broadcast message, SPIN_LOCK { update the map, call `IChannel::BroadcastFromClient` }.
  - When a client is disconnecting, SPIN_LOCK { remove the client from the list, delete itself from all `expectedClientIdList` from the map mentioned above }
  - When a response is received from a broadcast message, SPIN_LOCK { update the map }, call `IChannel::SendToClient` if all responses are ready.
  - Other places that is required but not mentioned here.

### ChatBotJsonDispatcherBase working with async `IChannel`

Sending and reading messages are async, but `OnJsonRequest` needs to be blocked. We can use a `SpinLock` and a `Semaphore` to make a message loop:
- When receiving messages: SpinLock { push the message }, increase Semaphore.
- When popuping up a message: decrease Semaphore, SpinLock { pop a message }.
In `OnJsonRequest`, after sending a message, it will looping on popuping message:
- If it is a request, process it (by running a proper `Translate` static message).
  - This might trigger nested `OnJsonRequest`, implementing `OnJsonRequest` in this way ensures correctness.
- If it is a response to the request it just sent, return.
- If it is a response to other request, properly push the response back to the list again just like it is receiving one.
Everytime when `OnJsonRequest` enters or leave, a `atomic_vint` needs to be maintained:
- increasing happens at the very beginning of `OnJsonRequest`.
- decrasing happen in the `SPIN_LOCK` and before actually popup up messages.
WHen pushing the message to the list because on `OnReadString` is called:
- In the `SPIN_LOCK`, before pushing the message, verify the `atomic_vint` and find out if it is zero:
  - If it is not zero, continue to push the message.
  - If it is zero, process the request directly, raise an error if it is a response (because a response should only happen when the `atomic_vint` is positive).

Then we get one problem, processing messages and `OnJsonRequest` should only happen in one thread:
- `ChatBotJsonDispatcherBase::RunTaskQueue` will use the same `SpinLock` + `Semaphore` mechanism to run a list of `Func<void()>`.
  - `RunTaskQueue` popups tasks in a dead loop.
  - When user types anything, queue a task to this list using `ChatBotJsonDispatcherBase::QueueTask`.
  - When `OnReadString` decides to process the request directly, `QueueTask`.
  - In this way, `OnJsonRequest` will always happen in this queue, nothings need to do as it is by natural single threaded.
- `ChatBotJsonDispatcherBase::QueueExitTask` will raise the semaphore, so when `RunTaskQueue` is unblocked by the samphore but find out the task list is empty, it quit the loop directly.

### ChatBotServer.vcxproj

Host a `vl::inter_process::HttpServer` with `ChatBotJsonDispatcherServer`.
`main` function:
- Starts all initialization and host the `IChatServer` instance.
- Run `RunTaskQueue` in a new sub class of `Thread`.
- When user input `exit`, call `QueueExitTask`, wait for the thread to exit, call `IChatServer::OnServerShutdown`, stop the server and exit.

`AddUser` triggers `OnUserAdded`.
`RemoveUser` triggers `OnUserRemoved`.
`Speak` triggers `OnSpoken`.
Typing exit triggers `OnServerShutdown`.
If `ChatBotJsonDispatcherBase` is implemented correctly, calling of these functions resulting in the task queue, so no `QueueTask` is required.

### ChatBotClient.vcxproj

Host a `vl::inter_process::HttpClient` with `ChatBotJsonDispatcherClient`.
- Starts all initialization and request the `IChatServer` service.
- User type the name, whatever it reads it becomes the user name, `QueueTask` with `IChatServer::AddUser`.
- Run `RunTaskQueue` in a new sub class of `Thread`. `AddUser` will be actually executed here.
- When user input `exit`, `QueueTask` with `IChatServer::RemoveUser`, call `QueueExitTask`, wait for the thread to exit, stop the server and exit.

When `OnUserAdded` is triggered, print a line `speakerName joined`.
When `OnUserRemoved` is triggered, print a line `speakerName left`.
When `OnSpoken` is triggered, print a line `speakerName> message`.
When `OnServerShutdown` is triggered, call `QueueExitTask`, wait for the thread to exit, stop the server and exit.

### Testing

To make sure the code works, you are going to run the below complete process and ensure a complete success:
- You are required to start a server with three client:
- Only after when a client joined with a name you can start the next client.
- When the second client joined, the first client should be notified.
- When the third client joined, the first two clients should be notified.
- Chat on each client as a round, do 3 round.
- `exit` on one client, the others should be notified.
- Exit the server, the rest of the clients should also exit.

I am going to run this testing process after you claiming to finish the work, you'd better run this testing process carefully to make sure your claim is true.

# TEST

The issue is a missing end-to-end real transport implementation for the generated ChatBot RPC application. The confirmation test is:

- Build `Test/UnitTest/UnitTest.sln` for Debug x64 after `CompilerTest_LoadAndCompile` has generated `Test/Generated/Apps/ChatBot/Cpp`.
- Run `CompilerTest_LoadAndCompile` so the generated ChatBot API code and app shared items are current.
- Start one `ChatBotServer` process and three `ChatBotClient` processes.
- Join clients one at a time with distinct names, verifying that prior clients receive the expected `speakerName joined` lines.
- Send three chat rounds from all three clients, verifying each recipient receives `speakerName> message` for messages sent by the other clients.
- Exit one client and verify the remaining clients receive `speakerName left`.
- Exit the server and verify the remaining clients receive server shutdown and exit.

Success requires the build and generated app compiler step to pass, the interactive multi-process chat scenario to complete without hangs or crashes, and no protocol mismatch in the RPC JSON channel routing.

# PROPOSALS

- No.1 Add a shared channel-backed ChatBot JSON dispatcher and wire both apps to it [CONFIRMED]

## No.1 Add a shared channel-backed ChatBot JSON dispatcher

Implement `ChatBotJsonDispatcherBase`, `ChatBotJsonDispatcherClient`, and `ChatBotJsonDispatcherServer` under `Test/UnitTest/ChatBotServer/Shared`, using `vl::inter_process` channel abstractions instead of exposing the HTTP transport to the dispatcher layer. The server owns service registration and broadcast fan-in/fan-out, while clients log in, request the generated `IChatServer` service, and process callbacks through the dispatcher's single-threaded task queue.

### CODE CHANGE

- Update `CompilerTest_LoadAndCompile` ChatBot generation to add `GenerateModuleRpc` and `GenerateModuleRpcJson` output modules before generating C++ files, so `ChatBotApp` exposes generated RPC wrappers, object ops, event ops, serializers, and id metadata.
- Add a shared ChatBot dispatcher layer under `Test/UnitTest/ChatBotServer/Shared`:
  - `ChatBotJsonDispatcherBase` implements `IRpcJsonMessageDispatcher` and `IChannelReader<JsonPackage>`, keeps an async channel message queue and a single-threaded task queue, routes generated JSON RPC requests through generated translators, and synchronously waits for matching responses.
  - `ChatBotJsonDispatcherClient` logs in through a channel client, learns the server's local client id, declares the remote `IChatServer` service, and sends client-originated broadcasts through the server.
  - `ChatBotJsonDispatcherServer` registers a local server channel client, hosts `IChatServer`, validates incoming clients for `RpcChannel`, redirects client broadcasts through the server, and consolidates multi-client broadcast responses.
- Wire `ChatBotServer` to host an HTTP channel server, run dispatcher tasks on a dedicated thread, implement `IChatServer`, and broadcast shutdown before stopping.
- Wire `ChatBotClient` to connect through an HTTP channel client, join under the typed name, print server event callbacks, queue chat input through the dispatcher task queue, and exit on either local `exit` or server shutdown.
- Keep redirected broadcast requests distinct from the original request by cloning the JSON request before changing request/source ids, so the original sender still matches the consolidated response id.
- Treat a remote `IChatServer::RemoveUser` call as the end of that app client's logical channel session after its response is sent. This removes the client from the server's expected broadcast response set before later shutdown broadcasts.

### CONFIRMED

- `Debug|x64` build passed after regenerating ChatBot outputs.
- `CompilerTest_LoadAndCompile` passed and regenerated `Test/Generated/Apps/ChatBot/Cpp/ChatBotApp*`.
- `Debug|Win32` build passed.
- Multi-process x64 scenario passed with one `ChatBotServer` and three `ChatBotClient` processes:
  - Alice joined first, then Bob joined and Alice saw `Bob joined`.
  - Carol joined third and Alice/Bob saw `Carol joined`.
  - Three chat rounds completed, with all clients receiving `Alice> alice-N`, `Bob> bob-N`, and `Carol> carol-N`.
  - Bob exited and Alice/Carol saw `Bob left`.
  - Server exited and the remaining Alice/Carol client processes exited cleanly.
  - Final exit codes: server 0, Alice 0, Bob 0, Carol 0.
