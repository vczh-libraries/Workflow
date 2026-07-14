# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

commit and push all local changes

## Task 1
This task is for VlppOS repo
Http(Server|Client)(Api)?.Windows.(h|cpp)  using `vl::inter_process::windows_http` namespace.
NamedPipe.Windows.(h|cpp) using `vl::inter_process::named_pipe` namespace.
update knowledge base in VlppOS to reflect this change

## Task 2
Release VlppOS to Workflow and fix build breaks due to namespace changing
To verify, you need to follow the SOP to run chatbot test projects.

## Task 3
Release VlppOS to GacUI and fix build breaks due to namespace changing.
To verify, you need to:
- follow the SOP to run RemotingTest_(Core|Rendering_Win32) and make sure `/RCP /HTTP` works
- Run GacJS against RemotingTest_Core and make sure `/RCP /HTTP` works

## Task 4
Update document website to reflect the namespace changing, if anything actually updates, publish it

## Task 5
Run [job.Windows.copilotInitAll.prompt.md](Tools/Jobs/job.Windows.copilotInitAll.prompt.md) but skip learning.

# UPDATES

# TEST [CONFIRMED]

Run `Tools\Tools\Build.ps1 Workflow` before changing Workflow source. The prescribed release/import step must copy the new VlppOS release into `Workflow\Import`. The build is expected to expose every release-consumer break before Workflow source is changed: first any merged-header/include-order incompatibility in the newly packaged upstream source, then the ChatBot HTTP composition points where `HttpServer` and `HttpClient` no longer live directly in `vl::inter_process`. This confirms the downstream break and proves that the imported public headers are being consumed.

After the fix:

1. Run the complete single-repository Workflow build/release command successfully so all C++ test projects, generated code, TypeScript verification and Workflow release generation pass with the imported VlppOS release.
2. Build the Debug x64 solution through the Workflow wrapper and run the complete ChatBot SOP with one server and clients named Tom, Jerry and Spike.
3. Start clients sequentially, verify join notifications, exchange three rounds in which every client sends one message, exit one client and verify the others see its leave notification, then exit the server and verify the remaining clients terminate automatically.

## Confirmation

`Tools\Tools\Build.ps1 Workflow` imported the new VlppOS release and stopped at its first solution-build failure. A follow-up Debug x64 build through Workflow's logging wrapper reproduced nine compiler errors in every project consuming `Import\VlppOS.Windows.cpp`: `RPC_WSTR` could not convert to `WString` in `HttpServerConnection::GenerateNewGuid`. The newly packaged `AsyncSocket.Windows.h` precedes `NetworkProtocol.Windows.h` in the merged release and includes `Windows.h` before the latter header can define its established lean/RPC include policy. This confirms a real generated-release integration break in the upstream source, before compilation can reach the expected ChatBot namespace call sites.

# PROPOSALS

- No.1 Preserve the Windows lean-header boundary upstream and import the concrete HTTP namespace [CONFIRMED]

## No.1 Preserve the Windows lean-header boundary upstream and import the concrete HTTP namespace

Fix the generated-release integration error at its source in VlppOS, not in `Workflow\Import`. `AsyncSocket.Windows.h` must continue to include Winsock 2 before `Windows.h`, but it should also define `WIN32_LEAN_AND_MEAN` before that first `Windows.h` inclusion. This prevents the newly earlier AsyncSocket section in the merged release header from pulling RPC declarations before `NetworkProtocol.Windows.h` defines `RPC_USE_NATIVE_WCHAR`; the latter header can then include `rpc.h` with the same native-wide-string contract used by the standalone sources. Regenerate the VlppOS release and copy all non-`IncludeOnly` root release artifacts back into Workflow.

Once the upstream merged release compiles, add `using namespace vl::inter_process::windows_http;` at the Windows HTTP composition boundary in both ChatBot server and client source files. Keep the existing `using namespace vl::inter_process;` for generic channel/protocol APIs and status enums. No named-pipe namespace import is needed because Workflow has no named-pipe concrete usage.

Run the complete Workflow build/release pipeline so the imported release, all generated/runtime variants and TypeScript checks are covered, then execute the interactive three-client ChatBot SOP to validate real HTTP channel behavior and shutdown.

### CODE CHANGE

VlppOS commit `27558ec` defines `WIN32_LEAN_AND_MEAN` before the first `Windows.h` inclusion in `AsyncSocket.Windows.h`, then regenerates `Release/VlppOS.Windows.h`. Workflow imports that generated header unchanged. `ChatBotServer/Main.cpp` and `ChatBotClient/Main.cpp` add `using namespace vl::inter_process::windows_http;` beside the retained generic inter-process namespace.

The Debug x64 Workflow wrapper build succeeds with 0 warnings and 0 errors. The interactive ChatBot SOP also passes through the prescribed CLI wrapper: Tom, Jerry and Spike join sequentially; every client observes all nine messages across three rounds; Jerry and Spike observe `Tom left`; the server exits on ENTER; and the server plus all three clients exit with code 0.

Finally, `Tools\Tools\Build.ps1 Workflow` completes with exit code 0. It rebuilds Release Win32 and x64, executes `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `CppTest`, `CppTest_Metaonly`, `CppTest_Reflection` and `RuntimeTest` on both platforms, passes the TypeScript `tsc --noEmit` build, regenerates the Workflow release and rebuilds CppMerge.
