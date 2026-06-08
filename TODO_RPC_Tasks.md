investigate repro

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
