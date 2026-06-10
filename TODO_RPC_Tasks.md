investigate repro

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
- Starts all initialization.
- User type the name, this will be the first message of all clients, the server will then remember the name of a client id.
- When user input `exit`, call `QueueExitTask` followed by `IChatServer::OnServerShutdown`, stop the server and exit.

### ChatBotClient.vcxproj

Host a `vl::inter_process::HttpClient` with `ChatBotJsonDispatcherClient`.

Start a manual signal event.
Start a `HttpClient` with `RpcChat` channel.
After connecting to the server, wait for the first message from the channel, it should be the client id representing the server, we calls it `server id`. Signal the manual signal event.
Request `IChatServer` and hook all events.
When the application starts, `Console::Read` a user name and call `IChatServer::AddUser`.
When `exit` is entered, call `IChatServer::RemoveUser` and exit.
When `OnUserAdded` is triggered, print a line `speakerName joined`.
When `OnUserRemoved` is triggered, print a line `speakerName left`.
When `OnSpoken` is triggered, print a line `speakerName> message`.
When `OnServerShutdown` is triggered, stop the `HttpClient` and exit, no `IChatServer` call is needed, including `RemoveUser`.

`main` function:
- Starts all initialization, wait for the manual signal event to signal.
- When `OnServerShutdown`, print "Server shutdown, press ENTER to exit.".
- Wait for the first input and call `IChatServer::AddUser`. Even when the first input is `exit`, `exit` becomes the user name.
- When user input `exit`, call `IChatServer::RemoveUser`, stop the client and exit.
- When user input others, call `IChatServer::Speak`.
- From the second input, if `OnServerShutdown` has triggered, stop the client and exit.

### Thread Safety

- A global spin lock should be held to run:
  - `Console::Write`.
  - `IChatServer` in two `main` function.
  - Consecutive calls could be called in one single `SPIN_LOCK`.
- Such spin lock will be a variable in two `main` function.

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
