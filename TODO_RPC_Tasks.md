investigate repro

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

## Task 1

Refactoring of `ChatBot(Server|Client)` projects.
This is not the immediate goal but the long term goal is to make `ChatBotJsonDispatcher*` being general enough to be accepted into the library.
You are required to run unit test projects as well as the test process for `ChatBot(Server|Client)` to make sure you change actually works.

Scan through all source files under `Test/UnitTest/ChatBot*` and make sure:
- No need to use explicitly `Ptr<T>` constructor for shared pointer type conversion.
- Delete all `Thread::Sleep`.
  - In `ChatBotJsonDispatcherBase::OnJsonRequest` the loop already calls `PopReceivedMessage` which will be blocked by semaphore.
  - In `ChatBotJsonDispatcherServer::SendLoginMessage` I think the logic needs to be rewritten.
    - In `ChatBotJsonDispatcherServer::AcceptClient` you could stores all clientId into a pending client id list if local client id is not set.
    - When local client id is set, send the id to all pending client ids.
    - Protect local client id and pending client id list in the same spin lock.
- Avoid any polling, I strongly prefer async scheduling.
  - `ChatBotJsonDispatcherServer::SendLoginMessage` is an example, you need to check other pieces of the code and see if there is any polling to be improved.

Make a static function `IRpcJsonMessageDispatcher::DefaultDispatch`, prefer raw pointer for arguments, and only when you need shared pointer use `Ptr<T>`.
There are code to share between:
- `ChatBotJsonDispatcherBase::OnJsonRequest`
- `RpcDualJsonRequestBridge::OnJsonRequest`
Put the parsing-and-call-translate part to `IRpcJsonMessageDispatcher::DefaultDispatch`, but remember not to touch any synchronization stuff in it.

`ChatBotJsonDispatcherBase` implementa `IChannelReader` and only accept an `IChannel` in the constructor.
- `Http(Server|Client)` should be created in each `Main.cpp`.
- `ChatBotJsonDispatcherServer` exposes functions for all kinds of registration, no longer handling connection verification:
  - It should be in `Main.cpp` to make sure both side uses `RpcChannel` channel, `ChatBotJsonDispatcher*` no need to know that.
  - Therefore the rejection of connection should be in server's `Main.cpp` too.
  - Only when a connection is accepted, the client id will be passed to `ChatBotJsonDispatcherServer` for following works, e.g. send back the local client id.
  - Keep synchronization stuff in `ChatBotJsonDispatcher(Server|Base)`, `Main.cpp` should not be worry about too much.

Extract task queue to a class `TaskQueue`, `ChatBotJsonDispatcherBase` no longer use the task queue directly.
- This specifically means `RunTaskQueue`, `QueueTask`,  `QueueExitTask` and the underlying task list with related locks.
- `ChatBotJsonDispatcherBase` opens a abstract function `SchedulaTask(Func<void()>)`:
  - `Main.cpp` will implement `ChatBotJsonDispatcher(Server|Client)` to override this function and call `QueueTask`.
  - Therefore `TaskQueue` is also needed to be created in `Main.cpp` and `RunTaskQueue` still calls in `main`.

The idea of the refactoring is that, user might have their own mechanism of task scheduling
so `TaskQueue` here is just a default option
`ChatBotJsonDispatcher*` should not assume the underlying structure
