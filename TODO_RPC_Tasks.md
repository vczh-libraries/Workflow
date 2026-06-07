investigate repro

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

## Task 1

`Test/UnitTest/Generated_Apps_ChatBot/Generated_Apps_ChatBot.vcxitems` has wild cards, which is not allowed, enumerate files explicitly.

Refactor of `IRpcLifecycle`:

- Rename `IRpcLifecycle::RegisterService` to `RegisterLocalService`, registering a local object as a service.
- Add `IRpcLifecycle::DeclareRemoteService(vint typeId, vint clientId)`, declare a registered remote service.
- Add `IRpcLifecycle::GetRegisteredLocalServices`, returns a const& dictionary of local registered services.
- Update `IRpcLifecycle::RequestService(vint typeId)`, returns the local registered service or call `IRpcDispatcher::RequestService`.
- Add `IRpcLifecycle::Initialize` calling `IRpcDispatcher::Initialize`. `RegisterLocalService` will throw after that.
- New functions will be implemented in `RpcLifecycleBase`.
  - `RegisterLocalService` throw if the type id already exists.
  - `DeclareRemoteService` overrides old entries if type id already exists.
  - `RequestService` prioritize local service over remote service.
- Test samples need to change so that it calls `IRpcLifecycle::RegisterLocalService`.

Refactor of `IRpcDispatcher`:
- Delete `IRpcDispatcher::IsRegisteredService`.
- Update `IRpcDispatcher::RegisterService` to `IRpcDispatcher::DeclareLocalService(typeId, clientId)`.
- Add `IRpcDispatcher::Initialize`.
- In test mocks:
  - `Initialize` keeps empty.
  - `RegisterService` calls `IRpcLifecycle::DeclareRemoteService` of the other side.
  - Services registration will always be in `RpcLifecycleBase`, test mocks should no longer maintain service registration.
  - Currently both `RpcLifecycleBase` and all `IRpcDispatcher` mocks maintain services, which is incorrect.

The goal is that, when user implements its own protocol of RPC, `IRpcDispatcher` and `IRpcJsonMessageDispatcher` will be pure for data transmission, they should not maintain any data or state.

## Task 1

### ChatBotServer.vcxproj

It hosts an `localhost:8888/WorkflowChatBot` using `vl::inter_process::HttpServer`.
A `RpcChat` channel is used.
Register a local client for RPC, remember its client id.
When the server receives a client:
- if it does not implement a `RpcChat` channel, reject it immediately.
- otherwise, send the local client id back.
Implement `IChatServer`, register it has a server so that it is exposed to all client.
- `AddUser` maintains a name with the client id in a `Dictionary`. If the name already exists, returns false.
  - Only when succeeded triggers `OnUserAdded` on all clients.
- `RemoveUser` removes `name` from the dictionary, verify that the name is binding with that client, otherwise returns false. `OnServerShutdown` will not be triggered.
  - Only when succeeded triggers `OnUserRemoved` on all clients.
- `Speak` broadcast messages to all other client. `speakerName` is the speaker, all clients except the `speakerName` will have its `OnSpoken` event triggered.
When user press ENTER, the server triggers all `OnServerShutdown`, shot the `HttpServer` and exit. `Console::Read` could be used here.

`main` function:
- Starts all initialization.
- When user input `exit`, call `IChatServer::OnServerShutdown`, stop the server and exit.

### ChatBotClient.vcxproj

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

### Connecting RPC with Channel

Sharable code between client and server should be put in `Test/UnitTest/ChatBotServer/Shared`:
- `ChatBotDispatcher`
- `ChatBotJsonDispatcher`.

Just like `RunRpcTestCase_JsonRequest`, you need to start the dispatcher with a `IRpcJsonMessageDispatcher` implementation:
- You are only allowed to use source files in `Source` and `Test/Generated/Apps/ChatBot/Cpp`.
- `IRpcJsonMessageDispatcher::AllocateRequestId` should be incremental from 1.
- `ReadRequestId` is useful to know the id entering and leaving `IRpcJsonMessageDispatcher::OnJsonRequest`.

`IRpcJsonMessageDispatcher::OnJsonRequest` is a blocked function, but requests could be nested, for example:
- A request sends to remote.
- Remote sends a new request.
- Local responses that request.
- Remote responses the original request.
So you should not expect that the subsequent response should be responding to your request. Therefore the application should be organized like this.

`ChatBotJsonDispatcher` implements both `Thread` and `IRpcJsonMessageDispatcher`, accepts a clientId and `SpinLock& lockUI` comming from the `### Thread Safety` section.
- The thread maintains a task list `List<Ptr<JsonNode>>`, with a `SpinLock lockDispatcher` and `Semaphore lockTasks`.
  - `NotifyExit`: lockDispatcher{ exited = true; } increase lockTasks;
  - `PushJson`: lockDispatcher{ if (exited) return false; push JsonNode; } increase lockTasks; return true;
  - `TryPopJson`: decrease lockTasks; lockDispatcher { if (exited && empty list) return nullptr; pop task; } return task;
- In `OnJsonRequest`:
  - **HOW TO WAIT FOR ALL EVENT RESPONSES**?

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
