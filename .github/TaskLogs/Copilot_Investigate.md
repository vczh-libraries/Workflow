# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

- you have to follow `REPO-ROOT/.github/Guidelines/Coding.md` when coding.
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.
  - It is important to do task one by one strictly, by me designing tasks in this way, we can achieve:
  - Easy-to-understand commits for file changing that is easy to review.
  - Limit side effects so that you don't have to deal with massive of issues at the same time.

## Task 1

When `IObjectEventOps::InvokeEvent` does not raise exception, it should return null.
When it raises exception, the exception message should be formatted like `clientId:message;...`. The last semicolon should exist, it is not just a delimeter.
But the wrapper should still verify if it is not null and not empty to raise the exception.
It will affect the last two block of `Rpc/Inheritance` sample's expected test result in `IndexRpc.txt`.

## Task 2

I would like to split `Rpc/Inheritance` sample into 3, keep the `s` thing, All 4 interfaces and their inheritance relationship remain in all 3 samples:
- `Rpc/Inheritance`: No event is declared. No member raise exception and no try-catch (except `IDerived::Set(One|Two)Value`, they still raise exceptions, to make sure they will not be called in the test case). No `GuardCrashAtClient` is declared.
- `Rpc/Inheritance_MethodException`: No event is declared, `CreateOne`, `CreateTwo`, `GuardCrashAtClient` is not declared. `Value` property and `SetDerivedValue` is not declared. It only calls `Set(One|Two)Value` and test their exceptions.
- `Rpc/Inheritance_EventException`: Deleting all members except events and `GuardCrashAndClient`. It only trigger two events and test their exceptions.

All 3 samples should remain calling involved members just like the original sample, so the first one leaving 4 `[]`, and the other leaving 2 `[]`, in the test result.

## Task 3

Now `IRpcListEventOps::OnItemChanged` will accept exceptions just like `IObjectEventOps::InvokeEvent`.
You need to create a `Rpc/Oblist_EventException`, the `IServer::GetOblist` creates an `observe int[]`, handle its `ItemChanged` event to raise an exception (need to format all arguments to the exception message), save it in a field and return it. Now `clientMain` inside a try-catch, it adds 0 to the list, triggering `ItemChanged` and catch the exception and return.

Unlike object and wrappers, container related stuff are all implemented in C++, mainly in `WfLibraryRpcWrappers.(h|cpp)`.
You can register a static function `IRpcLifecycle::ReadEventException`, in both object wrappers and container wrappers, call it with the return value from `InvokeEvent` and `OnItemChanged`:
- When the argument is null or is empty, nothing happens.
- When the argument has anything, format the exception message and raise it, using `vl::Exception`.
- In this way we DRY.

Remember to update documents and `Rpc.d.ts`.

# UPDATES

# TEST [CONFIRMED]

Task 1 uses the existing `Rpc/Inheritance` runtime sample. The expected result in `Test/Resources/IndexRpc.txt` should change the two event-exception blocks to include the reporting client id and a trailing semicolon:
- `derived.CrashAtServer()` should report `1:CrashedAtServer;`.
- `derived.GuardCrashAtClient()` should report `2:CrashedAtClient;`.

Success criteria:
- `IRpcObjectEventOps::InvokeEvent` returns null when no remote handler raises.
- A non-null, non-empty event exception map raises a `vl::Exception` whose message is `clientId:message;...`.
- The generated `Rpc/Inheritance` runtime test passes with the updated expected output.

Confirmed by Win32 and x64 builds plus unit tests. Runtime and generated C++ RPC tests now produce `[1:CrashedAtServer;][2:CrashedAtClient;]` for `Rpc/Inheritance`.

# PROPOSALS

- No.1 Add shared object event exception reader [CONFIRMED]

## No.1 Add shared object event exception reader

### CODE CHANGE

- Added `vl::rpc_controller::ReadEventException` and registered it on `system::IRpcLifecycle`.
- Updated generated object wrappers to call `IRpcLifecycle::ReadEventException` when reading event invocation results.
- Updated object event ops generation, including JSON serialization generation, to return null for no event exceptions and a non-empty exception map otherwise.
- Updated the RPC lifecycle mock and `Rpc/Inheritance` expected output.

### CONFIRMED

- `copilotBuild.ps1 -Configuration Debug -Platform x64`
- `copilotBuild.ps1 -Configuration Debug -Platform Win32`
- `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`
- `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, `CppTest_Reflection` on Win32 and x64
- `Test/TypeScript/prepare.ps1`, `npm run build`

- No.2 Split inheritance RPC sample [CONFIRMED]

## No.2 Split inheritance RPC sample

### CODE CHANGE

- Split `Rpc/Inheritance` into no-event, method-exception, and event-exception samples while preserving the four-interface inheritance shape and global `s`.
- Made the no-event `Value` property dynamic so member calls can update observable results without declaring events.
- Added resource/project entries and regenerated RPC metadata, C++ RPC wrappers, and TypeScript artifacts.

### CONFIRMED

- `CompilerTest_LoadAndCompile` x64
- `copilotBuild.ps1 -Configuration Debug -Platform x64`
- `copilotBuild.ps1 -Configuration Debug -Platform Win32`
- `RuntimeTest` Win32 and x64
- `CppTest`, `CppTest_Metaonly`, `CppTest_Reflection` Win32 and x64
- `Test/TypeScript/prepare.ps1`, `npm run build`
