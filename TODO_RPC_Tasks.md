investigate repro

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.
- You should perform coding, testing, git push per task, one after another, no mixing is allowed.

## Task 1

Delete `Test/Resources/Runtime`, `IndexRuntime.txt`, `TestRuntimeCompiler.cpp` and all related stuff, including generated files. I would like to remove this test file completely.

## Task 2

Add `Test/Apps/ChatBot` folder, with a Workflow module `ChatAPI.txt`:
```Workflow
module ChatAPI;

namespace chatapi
{
  @rpc:Interface
  interface IChatServer
  {
    func AddUser(name: string, client: IChatClient^) : bool;
    func RemoveUser(name : string, client: IChatClient^) : bool;
    func Speak(speakerName: string, message: string) : void;
  }

  @rpc:Interface
  interface IChatClient
  {
    event OnUserAdded(name: string);
    event OnUserRemoved(name: string);
    event OnSpoken(speakerName: string, message: string);
    event OnServerShutdown();
  }
}
```

Add `TestAppCompile.cpp` to `CompilerTest_LoadAndCompile` and build `Test/Apps/ChatBot` to `Test/Generated/Apps/ChatBot`:
- x86 code generated to `Cpp32` folder, git ignored.
- x64 code generated to `Cpp64` folder, git ignored.
- merged code generated to `Cpp` folder, git tracked.
Add `Test/UnitTest/Generated_Apps_ChatBot/Generated_Apps_ChatBot.vcxitems` to track `Test/Generated/Apps/ChatBot/Cpp`, put it in `UnitTest.sln` under `Source Files` with other vcxitems projects.

## Task 3

Under `Test/UnitTest` add `ChatBotServer/ChatBotServer.vcxproj` and `ChatBotClient/ChatBotClient.vcxproj`, both should be CLI application.
Put them in `UnitTest.sln` under `Apps_ChatBot`, just like `Source Files`.
Both project uses `VlppImport.vcxitems`, `VlppWorkflow_Library.vcxitems`, `Generated_Apps_ChatBot.vcxitems`.
You are going to add `Test/Generated/Apps/ChatBot/Cpp` to the import folder so all files can be directly #include.

### ChatBotServer

It hosts an `localhost:8888/WorkflowChatBot` using `vl::inter_process::HttpServer`.
A `RpcChat` channel is used.
When the server receives a client, if it does not implement a `RpcChat` channel, reject it immediately.
Implement `IChatServer`, register it has a server so that it is exposed to all client.
- `AddServer` maintains a name with a `IChatClient^` in a `Dictionary`. If the name already exists, returns false.
  - Only when succeeded triggers `OnUserAdded` on all clients.
- `RemoveUser` removes `name` from the dictionary, verify that the name is binding with that `IChatClient^` object, otherwise returns false. `OnServerShutdown` will not be triggered.
  - Only when succeeded triggers `OnUserRemoved` on all clients.
- `Speak` broadcast messages to all other client. `speakerName` is the speaker, all clients except the `speakerName` will have its `OnSpoken` event triggered.
When user press ENTER, the server triggers all `OnServerShutdown`, shot the `HttpServer` and exit. `Console::Read` could be used here.

### ChatBotClient

Implement `IChatClient` and hook it with the `RpcChat` channel.
When the application starts, `Console::Read` a user name and call `IChatServer::AddUser`.
When `exit` is entered, call `IChatServer::RemoveUser` and exit.
When `OnUserAdded` is triggered, print a line `speakerName joined`.
When `OnUserRemoved` is triggered, print a line `speakerName left`.
When `OnSpoken` is triggered, print a line `speakerName> message`.
When `OnServerShutdown` is triggered, stop the `HttpClient` and exit, no `IChatServer` call is needed, including `RemoveUser`.

### Connecting RPC with Channel

Just like `RunRpcTestCase_JsonRequest`, you need to start the dispatcher with a `IRpcJsonMessageDispatcher` implementation:
- You are only allowed to use source files in `Source` and `Test/Generated/Apps/ChatBot/Cpp`.
- `IRpcJsonMessageDispatcher::AllocateRequestId` should be incremental from 1.
- `ReadRequestId` is useful to know the id entering and leaving `IRpcJsonMessageDispatcher::OnJsonRequest`.
- `Console::WriteLine` is not thread safe, you can only call this function in the same thread.

`IRpcJsonMessageDispatcher::OnJsonRequest` is a blocked function, but requests could be nested, for example:
- A request sends to remote.
- Remote sends a new request.
- Local responses that request.
- Remote responses the original request.
So you should not expect that the subsequent response should be responding to your request. Therefore the application should be organized like this.

`TaskLoopThread`
- Inherit from `Thread` and expose:
  - `PushTask`, adds it to the list (`List<Func<void()>>`) and increase a semaphore.
  - `Exit`, set a flag, increase a semaphore, all subsequent `PushTask` will fail.
  - `HasExited`, return a bool that is only set to true in the last line of the thread function.
  - The thread function itself will wait for a semaphore in a loop:
    - Remove the first item from the list.
    - Execute it after `SpinLock` protection, or when the list is empty, it means `Exit` is called, quit the loop. Therefore the thread function exits.
  - All three functions will need to use the same `SpinLock` to access the list and the exit flag.

For Client:
- `main` function starts all initialization.
  - Start `TaskLoopThread`
    - `Initialize` with `Ptr<IChatServer>` and `Ptr<IChatClient>`.
      - All events on `IChatClient` is hooked, when anything is triggered, `PushTask` to call `Console::Write`.
      - Except `OnServerShotdown`, prints "Server shotdown, press ENTER to exit.".
  - `main` will listen `Console::Read` until `TashLoopThread::HasExited` returns true.
    - The first input will be the user name, call `PushTask` for `IChatServer::AddUser`. Even when the first input is `exit`, `exit` becomes the user name, instead of a existing action.
    - When user input `exit`, `PushTask` for `IChatServer::RemoveUser` and print "Press ENTER to exit." and `Exit`. DO NOT break the loop here. User will need to make another input to exit.
    - When user input others, `PushTask` for `IChatServer::Speak`.
    - From the second input, read `HasExited` first, if it returns true, ignore the input text, skip any action and break the loop to exit the main function.

For Server:
- `main` function starts all initialization.
  - Start `TaskLoopThread`
    - `Initialize` with `Ptr<IChatServer>`.
      - All events on `IChatServer` is hooked, when anything is triggered, `PushTask` to call other `IChatClient`.
  - `main` will listen to two `Console::Read`.
    - The first one call `PushTask` for `IChatClient::OnServerShutdown` and print "Press ENTER to exit.".
    - The second one does nothing, and exit the main function.

Sharable code between client and server should be put in `Test/UnitTest/ChatBotServer/Shared`.
- The task loop thread class could be shared, but the constructor and `Initialize` function will be implemented separately by inheriting in server and client.
- Only put code that shares, and make them sharable as your best effort.
- In the shared code there should not be any flag to distinguish client and server, otherwise you are putting non-sharable code.

### Testing

To make sure the code works, you are going to run the below complete process and ensure a complete success:
- You are required to start a server with three client:
- Only after when a client joined with a name you can start the next client.
- When the second client joined, the first client should be notified.
- When the third client joined, the first two clients should be notified.
- Chat on each client as a round, do 3 round.
- `exit` on one client, the others should be notified.
- Exit the server, the rest of the clients should also exit.
