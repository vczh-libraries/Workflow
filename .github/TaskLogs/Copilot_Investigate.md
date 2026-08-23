# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

I would like you to write

../Workflow/Test/StartRpcStdio.ps1, assuming the solution has been build with debug x64, start RpcStdioTest\_(Driver|Service).

../GacUI/Test/StartCore.ps1, according to ../GacUI/DebugRemoteProtocolWithNativeRenderer.md, to run core side test app with following options:

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
