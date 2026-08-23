# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

The goal of this task is to reuse all existing RPC test cases, but create another pair of test app:
- RpcStdioTest_Driver
- RpcStdioTest_Service
added to `UnitTest.sln`.

To start them, say `RpcStdioTest_Driver <cli-command-to-start=RpcStdioTest_Service> [apath-to-SkippedTestCaseListFile]`,
and `RpcStdioTest_Driver` will run all test cases unless one is in the skipped list,
on each test case it starts `cli-command-to-start=RpcStdioTest_Service`, with an extra argument of the test case name.
Note that it doesn't have to be `RpcStdioTest_Service`, but this is what we verify against right now, in the future other people could offer their own `RpcStdioTest_Service`.

`RpcStdioTest_Driver` connects `RpcStdioTest_Service` via stdio redirection, just like how `../GacUI/DebugRemoteProtocolWithNativeRenderer.md` is done with `/cli` option.
The stdio redirection implementation of `INetworkProtocolServer` is already done in VlppOS.

Test case names are in `IndexRpc.txt`.
`RpcStdioTest_Driver` will execute the client part, which is the service consumer. It execute all test cases at once, between each test case `RpcStdioTest_Service` is launched separatedly.
`RpcStdioTest_Service` will execute the server part, which is the service provider. It execute exactly the specified test case.
Since this is a test project, when anything unexpected happened, if there is already an exception for that let it crash, otherwise throw one.

When generating `TestCasesRpc.cpp`, generates `TestCasesRpcStdio_(Driver|Service).cpp`, as well as a manually written `TestCasesRpcStdio_(Driver|Service).h` for shared code across cases.
This will be the main part of `RpcStdioTest_(Driver|Service)`.

The background is that, GacJS is also doing RPC codegen in TypeScript as a service provider, after `RpcStdioTest_Driver` is done, we could pass the `nodejs to launch a package` command and it will run service providers written in TypeScript, so that we could offer this test project as part of the tool, for people who want to implement RPC in their own languages. But you don't need to worry about anything about GacJS right now, just focus on Workflow repo itself. Since stdio redirection facility has been well tested on Windows/Linux/macOS as well as a simple demo in GacJS, so I believe you can just limit every change in the Workflow repo, and more strictly, I think most of the code do not need to be touch, adding the mentioned codegen with two new projects should be enough.

Update `Project.md` accordingly using similar languages in that document. Create vmake for these new test projects in `Test/Linux` but don't worry about how to test them.

# UPDATES

# TEST [CONFIRMED]

The requested stdio RPC harness is absent in the baseline. A static repository scan confirmed that neither project is present in `Test/UnitTest/UnitTest.sln`, the project and Linux `vmake` directories do not exist, `TestRpcCompile.cpp` generates only `TestCasesRpc.cpp`, and neither manually maintained stdio harness header nor generated stdio case dispatcher exists.

The implementation will be accepted when all of the following conditions hold:

- `CompilerTest_LoadAndCompile` generates `TestCasesRpcStdio_Driver.cpp` and `TestCasesRpcStdio_Service.cpp` from every valid entry in `IndexRpc.txt`, alongside the existing `TestCasesRpc.cpp`, without changing the RPC sample definitions or their expected values.
- `RpcStdioTest_Driver` accepts a service launch command and an optional skipped-case list file, runs every unskipped `IndexRpc.txt` case in its own service process, and compares `clientMain` with the existing expected result.
- `RpcStdioTest_Service` accepts exactly one case name, runs only that case's `serviceMain`, and reserves stdout for the stdio-redirection protocol.
- The service and driver endpoints retain lifecycle client ids 1 and 2 respectively so existing expected event-exception text remains valid; the JSON RPC broker uses a separate local client id.
- Both projects build in Debug and Release for Win32 and x64, the driver completes the full RPC case list against `RpcStdioTest_Service` on Windows, and the existing required Workflow unit-test and TypeScript verification remain green.
- Both projects are present in `UnitTest.sln`, `Project.md` documents their roles and invocation, and `Test/Linux/RpcStdioTest_Driver/vmake` plus `Test/Linux/RpcStdioTest_Service/vmake` describe portable builds without claiming unperformed Linux runtime verification.

# PROPOSALS
