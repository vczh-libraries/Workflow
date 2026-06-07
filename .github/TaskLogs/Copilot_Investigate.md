# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

## Task 1

Delete `Test/Resources/Runtime`, `IndexRuntime.txt`, `TestRuntimeCompiler.cpp` and all related stuff, including generated files. I would like to remove this test file completely.
Delete `IRpcOperations::GetList(Event)?Ops`, as list ops are called by `RpcCalleeObject(Event)?OpsForList`.
- This would also delete the last two arguments from `IRpcController::Register`.
Delete `IRpcObjectOps::RegisterService`.
- This would also require `RpcLifecycleBase::RegisterService` to call the dispatcher directly just like how `RequestService` is done.

Remove `RpcLifecycleBase::dispatcher`, all access should go through `GetDispatcher`, which should already be the case.
Remove `RpcLifeCycleBase::GetDispatcher` overloading, as it becomes useless when `dispatcher` is removed.
Add `bool broadcast` to `IRpcJsonMessageDispatcher::OnJsonRequest`, set it to true when calling from `RpcJsonEventObjectOps`.

In `Rpc.d.ts`:
- Remove `RequestService` related stuff.
- Remove `targetClientId` from `IObjectOps_RegisterService_Request` as it is a broadcast request.
- Rename `IObjectEventOps_InvokeEvent_Response` to `Broadcast_Response`, all broadcast request should return this.
- `Test/TypeScript` should builds after changing to `Rpc.d.ts` with new generated files from other test projects.

## Task 2

Add `Test/Apps/ChatBot` folder, with a Workflow module `ChatAPI.txt`:
```Workflow
module ChatAPI;

namespace chatapi
{
  @rpc:Interface
  interface IChatServer
  {
    event OnUserAdded(name: string);
    event OnUserRemoved(name: string);
    event OnSpoken(speakerName: string, message: string);
    event OnServerShutdown();

    func AddUser(name: string) : bool;
    func RemoveUser(name : string) : bool;
    func Speak(speakerName: string, message: string) : void;
  }
}
```

Add `TestAppCompile.cpp` to `CompilerTest_LoadAndCompile` and build `Test/Apps/ChatBot` to `Test/Generated/Apps/ChatBot`:
- x86 code generated to `Cpp32` folder, git ignored.
- x64 code generated to `Cpp64` folder, git ignored.
- merged code generated to `Cpp` folder, git tracked.
- Using `ChatBotApp` as the assembly name to `WfCppConfig`.
Add `Test/UnitTest/Generated_Apps_ChatBot/Generated_Apps_ChatBot.vcxitems` to track `Test/Generated/Apps/ChatBot/Cpp`:
- Put it in `UnitTest.sln` under `Source Files` with other vcxitems projects.
- No need to separate reflection or other code in two vcxitems projects.

Under `Test/UnitTest` add `ChatBotServer/ChatBotServer.vcxproj` and `ChatBotClient/ChatBotClient.vcxproj`, both should be CLI application.
With `VCZH_DEBUG_NO_REFLECTION` for all configuration and `VCZH_CHECK_MEMORY_LEAKS` for `Debug`.
Put them in `UnitTest.sln` under `Apps_ChatBot`, just like how `*.vcxitems` in `Source Files`.
Both project uses `VlppImport.vcxitems`, `VlppWorkflow_Library.vcxitems`, `Generated_Apps_ChatBot.vcxitems`.
You are going to add `Test/Generated/Apps/ChatBot/Cpp` to the import folder so all files can be directly #include.

Add two `Main.cpp` to each `Source Files` solution folder, each should include the main header in `Test/Generated/Apps/ChatBot`.
`main` function returns 0, nothing need to be done.

No testing is needed for this task, but you should ensure the whole solution builds.

# TEST

Task 1: remove the Runtime compiler-test category and RPC redirection APIs, then verify the solution rebuilds, `CompilerTest_LoadAndCompile` can regenerate outputs, and `Test/TypeScript` builds against the updated `Rpc.d.ts`.

Task 2: add the ChatBot app sample and project wiring, then verify `Test/UnitTest/UnitTest.sln` builds.

# PROPOSALS

- No.1 Remove obsolete Runtime tests and RPC redirections, then regenerate affected outputs [CONFIRMED]
- No.2 Add ChatBot app generation and CLI projects [CONFIRMED]

## No.1 Remove obsolete Runtime tests and RPC redirections

### CODE CHANGE

Removed the `Runtime` compiler-test input set and all tracked generated/baseline files that depended on it. The Windows compiler-test project, filters, Linux build metadata, and runtime unit tests no longer reference the deleted `TestRuntimeCompile.cpp`, `IndexRuntime.txt`, or `Test/Resources/Runtime` files.

Simplified RPC registration wiring:
- Removed `IRpcOperations::GetListOps` / `GetListEventOps` and the extra list-op arguments from `IRpcController::Register`.
- Removed `IRpcObjectOps::RegisterService`; service registration now goes through `IRpcDispatcher` directly from lifecycle/JSON dispatching.
- Removed the stored dispatcher from `RpcLifecycleBase`; concrete lifecycles provide `GetDispatcher`.
- Added `broadcast` to `IRpcJsonMessageDispatcher::OnJsonRequest`, using `true` for event/broadcast requests.

Updated `Rpc.d.ts` and regenerated affected C++/metadata/TypeScript outputs so the removed APIs disappear from generated surfaces and broadcast requests share `Broadcast_Response`.

### CONFIRMED

- `CompilerTest_GenerateMetadata` x64 passed.
- `CompilerTest_GenerateMetadata` Win32 passed.
- `CompilerTest_LoadAndCompile` x64 passed and regenerated outputs.
- `Test/UnitTest/UnitTest.sln` Debug x64 passed.
- `Test/UnitTest/UnitTest.sln` Debug Win32 passed.
- `Test/TypeScript/prepare.ps1; npm run build` passed.
- `LibraryTest` Debug x64 and Win32 passed.
- `RuntimeTest` Debug x64 and Win32 passed.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` Debug x64 and Win32 passed.
- Stale-reference scan for deleted Runtime resources and removed RPC accessors only found the task log text and the intentionally remaining `IRpcListOps`/`IRpcListEventOps` types.

## No.2 Add ChatBot app generation and CLI projects

### CODE CHANGE

Added `Test/Apps/ChatBot/ChatAPI.txt` with the `chatapi::IChatServer` RPC interface and generated the merged C++ app output under `Test/Generated/Apps/ChatBot/Cpp`. The architecture-specific `Cpp32` and `Cpp64` generated folders are produced by `CompilerTest_LoadAndCompile` and ignored by git.

Updated `CompilerTest_LoadAndCompile` to generate the ChatBot app with the `ChatBotApp` assembly name and merge the x86/x64 app output into the tracked `Cpp` folder. Added app output path helpers and wired the new test file into the Windows and Linux compiler-test project metadata.

Added `Generated_Apps_ChatBot.vcxitems`, `ChatBotServer`, and `ChatBotClient`, and placed them in `UnitTest.sln` under `Source Files` and `Apps_ChatBot` respectively. Both CLI app projects import `VlppImport.vcxitems`, `VlppWorkflow_Library.vcxitems`, and `Generated_Apps_ChatBot.vcxitems`; their `Main.cpp` files include `ChatBotApp.h` and return 0.

Workflow event declarations are type-only in the parser grammar, so `ChatAPI.txt` uses type-only event payloads while preserving the requested function argument names and generated C++ event signatures.

### CONFIRMED

- Built `CompilerTest_LoadAndCompile` Debug x64 to regenerate ChatBot app output.
- `CompilerTest_LoadAndCompile` produced `Test/Generated/Apps/ChatBot/Cpp32`, `Cpp64`, and merged tracked `Cpp` output.
- `Test/UnitTest/UnitTest.sln` Debug x64 passed.
- `Test/UnitTest/UnitTest.sln` Debug Win32 passed.
