# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

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

# TEST [CONFIRMED]

The refactor will be verified by:

- Static scans under `Test/UnitTest/ChatBot*` to ensure no `Thread::Sleep`, no dispatcher-owned task queue methods, and no unnecessary explicit `Ptr<T>` conversion constructors remain in the touched ChatBot source.
- The existing unit-test flow required by `Project.md`, at least covering Debug Win32 and x64 builds, `LibraryTest`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, and `CppTest*` where the changed RPC/ChatBot code can surface regressions.
- The TypeScript RPC schema verification (`Test/TypeScript/prepare.ps1` followed by `npm run build`) if generated RPC JSON artifacts or schema-related files change.
- The required ChatBot real-process test: one `ChatBotServer`, three `ChatBotClient` instances named `Tom`, `Jerry`, and `Spike`, join notifications between clients, three chat rounds, one client exit notification, then server shutdown causing remaining clients to exit.

Success means the source scans are clean, all required builds/tests pass without memory leak dumps, and the multi-process ChatBot scenario completes with the expected join/chat/leave/shutdown output.

Confirmed verification:

- Built Debug x64 and Debug Win32 through `.github/Scripts/copilotBuild.ps1`; both completed with 0 errors and 0 warnings.
- Ran `LibraryTest` Debug x64 and Debug Win32: both passed 2/2 files and 15/15 cases.
- Ran `CompilerTest_GenerateMetadata` Debug x64 and Debug Win32: both passed 1/1 file and 2/2 cases.
- Ran `CompilerTest_LoadAndCompile` Debug x64: exited successfully and regenerated no tracked files.
- Ran `RuntimeTest` Debug x64 and Debug Win32: both passed 4/4 files and 261/261 cases.
- Ran `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` Debug x64 and Debug Win32: each passed 2/2 files and 229/229 cases.
- Ran `Test/TypeScript/prepare.ps1` followed by `npm run build`; `tsc --noEmit` completed successfully.
- Ran the required ChatBot process scenario through `.github/Scripts/copilotExecute.ps1 -Mode CLI` with one x64 server and three x64 clients named `Tom`, `Jerry`, and `Spike`. The run confirmed ordered joins, cross-client join notifications, three chat rounds from all three users, `Jerry` leaving with notifications, and server shutdown causing the remaining clients to exit with code 0.
- Static scans under `Test/UnitTest/ChatBot*` confirmed no `Thread::Sleep` remains. The only `Ptr<...>(...)` regex hit is the function type `Func<Ptr<IRpcWrapperBase>(RpcObjectReference, IRpcLifecycle*)>`, not an explicit shared-pointer conversion constructor.
- `git diff --check` completed successfully; it only reported normal line-ending warnings.

Additional note: `C:\Code\VczhLibraries\Tools\Tools\Build.ps1 Workflow` was invoked as the broad verification helper, but it exceeded the 30-minute tool timeout without console output and left orphaned `MSBuild`/`link` processes. Those orphaned processes were stopped. The explicit build, unit-test, TypeScript, and ChatBot verification above all completed successfully.

# PROPOSALS

- No.1 Extract transport-neutral dispatch and scheduling boundaries

## No.1 Extract transport-neutral dispatch and scheduling boundaries [CONFIRMED]

Move the reusable JSON request parse/translate branch into `vl::rpc_controller::IRpcJsonMessageDispatcher::DefaultDispatch`, keeping all wait, queue, and channel synchronization in callers. Refactor `ChatBotJsonDispatcherBase` so it is constructed with a raw `IChannel` pointer, implements only channel reading and RPC message coordination, and calls a virtual `SchedulaTask(Func<void()>)` instead of owning a queue. Add a separate `TaskQueue` default implementation for the CLI applications to use from `Main.cpp`. Move HTTP client/server creation and RpcChannel connection verification into the ChatBot `Main.cpp` files, and make the server dispatcher accept already-verified clients through a registration method that tracks pending clients under the same lock as the server local id. Remove all sleep-based waiting and rely on semaphores/task scheduling/channel callbacks.

### CODE CHANGE

Implemented the proposal with these direct changes:

- Added `vl::rpc_controller::IRpcJsonMessageDispatcher::DefaultDispatch` in `Source/Library/Rpc/WfLibraryRpcJson.*`, delegating request envelopes to `RpcJsonObjectOps::Translate`, `RpcJsonObjectEventOps::Translate`, or `RpcJsonDispatcher::Translate` without owning synchronization.
- Updated `RpcDualJsonRequestBridge::OnJsonRequest` and `ChatBotJsonDispatcherBase::TranslateRequest` to use `DefaultDispatch`.
- Extracted `chatbot::TaskQueue` and removed task-list/semaphore ownership from `ChatBotJsonDispatcherBase`; the base now calls the abstract `SchedulaTask(Func<void()>)` hook.
- Changed `ChatBotJsonDispatcherBase` to be constructed with a selected `IChannel<JsonPackage>*` and to implement only channel reading and RPC coordination.
- Moved ChatBot HTTP/channel client/server wrappers, RpcChannel verification, local-client setup, and task-queue wiring into `ChatBotServer/Main.cpp` and `ChatBotClient/Main.cpp`.
- Reworked `ChatBotJsonDispatcherServer` registration so accepted clients are passed in through `RegisterClient`, the local client id is set through `RegisterLocalClient`, and pending login client ids share the same spin lock as the local client id.
- Removed all ChatBot `Thread::Sleep` usage and replaced unmatched response requeueing with a buffered-response list so `OnJsonRequest` blocks on `PopReceivedMessage` instead of polling.
