# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

I would like you to implement a working Console::Read:
- Delete `TryReadLine`.
- Add `Nullable<WString> Console::TryRead()` which handles stdio, redirection and EOF just right. Returns null when it can't read just like what `TryReadLine` does.
- `Console::Read` calls `TryRead` and return `WString::Empty` for null.
You can first update the imported Vlpp(.Windows|Linux).(h|cpp), and when it works:
- git commit Workflow
- copy the implemention to `Vlpp`, and release Vlpp to Workflow, if the updating is correct, the imported Vlpp* files should not be touched.

You will need to run the standard test process for ChatBot(Server|Client) to make sure your code works.

Move `TaskQueue` to `VlppOS`. Put it below the `RepeatingTaskExecutor`, release VlppOS to Workflow therefore you can remove `TaskQueue` from `Test/UnitTest/ChatBotServer/Shared`, under the same namespace of `RepeatingTaskExecutor`

Read Vlpp and VlppOS knowledge base **IN THE Workflow REPO**:
- In where Console and redirection is mentioned, mention `TryRead`.
- In where ThreadPoolLite is mentioned, mention TaskQueue.
- Mention the above two thing in ../GacUI/ToDo/1.4.0.0.md 's `## New Feature` section.

Eventually, release Vlpp and VlppOS to GacUI, make sure it builds, but no testing is needed.

do not update Copilot_Investigation.md in other repo, everything is going to Workflow repo.
You will need to commit Vlpp/VlppOS/Workflow/GacUI

# UPDATES

# TEST

The change is confirmed by:
- Building Vlpp `Debug|x64` and `Debug|Win32`, then running Vlpp `UnitTest` for both platforms.
- Building VlppOS `Debug|x64` and `Debug|Win32`, then running VlppOS `UnitTest` for both platforms.
- Building Workflow `Debug|x64` and `Debug|Win32` after imported dependency changes.
- Running Workflow `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` in the required configurations.
- Running Workflow `Test/TypeScript/prepare.ps1` and `npm run build`.
- Running the full ChatBot multi-process process with one `ChatBotServer` and three `ChatBotClient` processes, feeding input through redirected stdin so `Console::TryRead` must read from pipes and detect EOF correctly.
- Verifying clients join one at a time, previous clients receive join messages, every client and the server receive three rounds of chat messages, one client exits and the others observe it, then server exit shuts down the remaining clients.
- Building GacUI `Debug|x64` and `Debug|Win32` after releasing Vlpp and VlppOS to GacUI.

# PROPOSALS

- No.1 Promote nullable console input and task queues into dependency APIs [CONFIRMED]

## No.1 Promote nullable console input and task queues into dependency APIs

Implement `vl::console::Console::TryRead` in Vlpp, using console-aware reading on Windows and stream-state-aware reading on Linux. Keep `Console::Read` as a compatibility wrapper returning `WString::Empty` when `TryRead` returns null. Update ChatBot to call `Console::TryRead` directly and delete its local `TryReadLine`.

Move the ChatBot dispatcher `TaskQueue` helper into VlppOS under `vl`, directly after `RepeatingTaskExecutor`, then consume it from Workflow through the released VlppOS import. Update Workflow knowledge base text for `Console::TryRead` and `TaskQueue`, and update GacUI 1.4.0.0 new-feature notes.

### CODE CHANGE

- Added `vl::console::Console::TryRead()` to the imported Workflow `Vlpp` header and platform implementations.
- Changed `vl::console::Console::Read()` to call `TryRead()` and return `WString::Empty` when no line can be read.
- Removed the duplicated `TryReadLine` helpers from `ChatBotServer` and `ChatBotClient`.
- Updated `ChatBotServer` and `ChatBotClient` to use `Console::TryRead()` for interactive and redirected stdin.
- Added `vl::console::Console::TryRead()` to Vlpp source and release files.
- Added `vl::TaskQueue` to VlppOS source and release files, with a unit test.
- Released VlppOS into Workflow and replaced the local ChatBot `TaskQueue` with the shared `vl::TaskQueue`.
- Fixed a pre-existing Vlpp Win32 Linq test failure by returning an `OrderByKey` key by value instead of returning a reference into a temporary tuple.
- Updated Workflow knowledge base pages for `Console::TryRead` and `TaskQueue`.
- Released Vlpp and VlppOS into GacUI and documented both APIs in `ToDo/1.4.0.0.md`.
