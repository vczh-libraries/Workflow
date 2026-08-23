# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

I would like you to write

../Workflow/Test/StartRpcStdio.ps1, assuming the solution has been build with debug x64, start RpcStdioTest\_(Driver|Service).

../GacUI/Test/StartCore.ps1, according to ../GacUI/.github/Jobs/DebugRemoteProtocolWithNativeRenderer.md, to run core side test app with following options:

- test apps:
  - CppTest\_Rvm: -App cpptest\_rvm
  - RemotingTest\_Core: -App fct|rpt|rvmt
- Network protocol: -protocol fct|rpt|rvmt
- Optional argument to connect RemotingTest\_RvmHost with /cli: -cli

../GacUI/Test/StartRendrerer.ps1:

- Network protocol: -protocol fct|rpt|rvmt
- Optional argument for automation service port: -port XXXX

Update ../(Workflow|GacUI)/Project.md to mention these test scripts.

run each powershell script and make sure it actually works, no need to verify the content of test app, just verify it actually starts the correct test app and could interact with the user as expected

../(iGac|wGac)/test\_core.sh: same argument design with StartCore.ps1, but using the same naming convention with test.sh. Need to run non-incremental build on used project in GacUI first, and then start it. Update any markdown document which mentions test.sh to mention test\_core.sh. Unfortunately you are not able to test it directly.

When RemotingTest\_RvmHost is needed, the building (only in wGac and iGac) and run need to start after 1 second of starting core app.

commit and push all local changes once finished

Forgot to say, ../Workflow/Test/StartRpcStdio.sh is also needed

# UPDATES

# TEST [CONFIRMED]

The requested launchers are absent in the baseline: neither Workflow stdio launcher, neither GacUI PowerShell launcher, nor either portable `test_core.sh` exists. `Workflow/Project.md`, `GacUI/Project.md`, and the Markdown documentation that currently describes `test.sh` do not mention the requested launchers.

The implementation will be accepted when all of the following conditions hold:

- `Workflow/Test/StartRpcStdio.ps1` and `Workflow/Test/StartRpcStdio.sh` resolve their repository-relative Debug x64 driver and service binaries, accept an optional exact-name skip-list path, and start the driver with the service executable as its child command.
- `GacUI/Test/StartCore.ps1` maps `cpptest_rvm`, `fct`, `rpt`, and `rvmt` to the correct Debug x64 executable and application selector, maps `http`, `pipe`, and `minihttp` to the documented network selectors, supports `-Cli` only for applications that need `RemotingTest_RvmHost`, and starts a manual host one second after the requester/Core when CLI mode is not selected.
- `GacUI/Test/StartRendrerer.ps1` maps the same three Windows transports to `RemotingTest_Rendering_Win32` and validates and forwards an optional automation port.
- Both `iGac/test_core.sh` and `wGac/test_core.sh` use the existing `test.sh` option naming convention, support the four Core-side app selections through portable `/MiniHttp`, perform full builds for the required GacUI Linux projects, and defer manual `RemotingTest_RvmHost` build/start until one second after starting the requester/Core.
- All four PowerShell launcher paths are exercised on Windows far enough to confirm the selected executable and arguments, interactive availability, and process cleanup. The two portable launchers receive static syntax and behavior review only, as requested.
- `Workflow/Project.md`, `GacUI/Project.md`, and every Markdown document in GacUI/iGac/wGac that mentions `test.sh` also explains or points to `test_core.sh` where appropriate.

# PROPOSALS

- No.1 Add repository-relative orchestration launchers [CONFIRMED]

## No.1 Add repository-relative orchestration launchers

Add thin PowerShell and Bash launchers that validate their small public option sets, resolve every executable and build helper relative to the launcher location, and forward the exact selectors documented by the existing test applications. Keep the application and transport dimensions distinct: `cpptest_rvm|fct|rpt|rvmt` selects the requester/Core workload, while `http|pipe|minihttp` selects a Windows network transport and portable launchers accept only `minihttp`.

The Workflow launchers will call `RpcStdioTest_Driver` in the current terminal so its output and failures stay interactive, supplying the matching Debug x64 service executable as the driver's child command and forwarding an optional skip file.

The Windows Core launcher will start the selected requester/Core as a child process and wait for it. For manual RVM mode it will wait one second and then start `RemotingTest_RvmHost` with the same transport; CLI mode will instead pass the quoted host executable path through `/Cli:` and let the test app auto-launch it. The renderer launcher will remain attached to `RemotingTest_Rendering_Win32` and forward its selected transport and optional validated automation port.

The iGac/wGac `test_core.sh` launchers will follow `test.sh`'s `--app:...`, `--protocol:...`, `--cli`, and `--unblock` spelling. They will use the sibling GacUI full-build wrapper for every GacUI project they consume. Manual RVM runs will start the local requester or GacUI Core first, sleep one second, and only then full-build and start the host. CLI runs require the host executable to exist when the requester/Core starts, so they will full-build the host before launch and pass its absolute path without starting a separate host. Blocking runs will wait on the primary requester/Core; unblocked runs will print its PID.

Update project and platform documentation with launcher syntax, sequencing, protocol limits, and the distinction between the existing native `test.sh` and new GacUI Core-side `test_core.sh`. Verify the PowerShell launchers through real Debug x64 processes and their interactive endpoints, then terminate every test process; validate the portable scripts statically without claiming runtime execution.

### CODE CHANGE

Added `Workflow/Test/StartRpcStdio.ps1` and `Workflow/Test/StartRpcStdio.sh`. Both launchers resolve the matching repository-relative driver and service binaries, validate an optional skip-list file, preserve paths when constructing the service child command, and keep the driver attached to the calling terminal. `Workflow/Project.md` now documents both launchers.

Added `GacUI/Test/StartCore.ps1` with separate validated app and transport option sets. It maps `cpptest_rvm` to `CppTest_Rvm`, maps `fct`, `rpt`, and `rvmt` to the corresponding `RemotingTest_Core` selector, passes `/Cli:<host>` where supported, or starts a manual `RemotingTest_RvmHost` after a one-second delay. Added the intentionally named `GacUI/Test/StartRendrerer.ps1` to launch `RemotingTest_Rendering_Win32` with the selected transport and optional validated automation port. Both launchers return process objects for interactive orchestration and cleanup. `GacUI/Project.md` and the remote-protocol guides now document the commands.

Added matching `iGac/test_core.sh` and `wGac/test_core.sh` launchers using `test.sh`-style `--app:`, `--protocol:`, `--cli`, and `--unblock` arguments. They accept the portable `/MiniHttp` transport, invoke the GacUI Linux project build wrapper with `-f`, start manual requesters/Core processes before the required one-second delay and host build/start, prebuild the host for Core `/Cli` mode, and clean up an already-started primary process if a later host build fails or the launcher is interrupted. Updated every Markdown file in GacUI, iGac, and wGac that mentions `test.sh` so it also describes or points to `test_core.sh`.

### CONFIRMED

The Workflow PowerShell launcher was run against the Debug x64 binaries with a temporary skip list that retained `Rpc:PrimitiveTypes`. It launched `RpcStdioTest_Driver.exe` with the exact quoted `RpcStdioTest_Service.exe` command, completed the RPC case successfully, and exited with code 0.

The complete GacUI Debug x64 solution built successfully with 0 warnings and 0 errors. Real launcher runs covered every requested app selector and every Windows transport: `/Http /FCT`, `/MiniHttp /RPT`, `/Pipe /RVMT /Cli:<host>`, and standalone `CppTest_Rvm /Http`; standalone `CppTest_Rvm /Cli:<host>` was also exercised. Process inspection confirmed the selected executable and exact command line in each run. Manual requester mode started `RemotingTest_RvmHost /Http` 1.029 seconds later, while both CLI requesters auto-launched the host with exact `/Cli`. Core, standalone requester, and renderer automation endpoints returned HTTP 200, and renderer/requester `/IO` endpoints accepted exact `!Exit`. Renderer checks covered both `/port:8890` and the omitted-port default at 8889. All launched test processes were terminated after verification.

PowerShell AST parsing passed for all three new `.ps1` files. Bash syntax parsing passed for the Workflow, iGac, and wGac scripts; all new shell files are LF-only. Static inspection in both portable repositories confirmed `sleep 1` precedes the manual host full build and launch, and that every consumed GacUI Linux project is built with `-f`. Per the request, the iGac and wGac launchers were not runtime-executed. `git diff --check` passed in all repositories, and a repository-wide scan confirmed that every GacUI/iGac/wGac Markdown file mentioning `test.sh` also mentions `test_core.sh`.
