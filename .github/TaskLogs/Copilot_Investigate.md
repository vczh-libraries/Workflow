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
- `RpcStdioTest_Driver` accepts a service launch command and an optional skipped-case list file, runs every unskipped `IndexRpc.txt` case in its own service process, and reports the value returned by `clientMain`; exceptions and transport failures remain fail-fast.
- `RpcStdioTest_Service` accepts exactly one case name, runs only that case's `serviceMain`, and reserves stdout for the stdio-redirection protocol.
- The service and driver endpoints retain lifecycle client ids 1 and 2 respectively so existing expected event-exception text remains valid; the JSON RPC broker uses a separate local client id.
- Both projects build in Debug and Release for Win32 and x64, the driver completes all transport-compatible RPC fixtures against `RpcStdioTest_Service` on Windows using the requested skip-file mechanism for fixtures whose client code assumes shared process globals, and the existing required Workflow unit-test and TypeScript verification remain green.
- Both projects are present in `UnitTest.sln`, `Project.md` documents their roles and invocation, and `Test/Linux/RpcStdioTest_Driver/vmake` plus `Test/Linux/RpcStdioTest_Service/vmake` describe portable builds without claiming unperformed Linux runtime verification.

# PROPOSALS

- No.1 Generate role-specific stdio harnesses over the existing JSON RPC channel stack [DENIED]
- No.2 Generate a fail-fast transport harness with fixture-controlled skipping

## No.1 Generate role-specific stdio harnesses over the existing JSON RPC channel stack

Extend the existing `TestRpcCompile.cpp` per-sample metadata collection and final harness generation so the same `IndexRpc.txt` iteration that writes `TestCasesRpc.cpp` also writes two role-specific dispatchers. The driver dispatcher will enumerate every case, honor an exact-name skipped-case set, provide the existing expected string and generated service type names, and instantiate the matching generated C++ assembly. The service dispatcher will select exactly one matching generated assembly by case name. Event-bearing assemblies will continue to be identified from the compiler's RPC event metadata so listener attachment is generated only when the assembly actually has events.

Add manually maintained `TestCasesRpcStdio_Driver.h` and `TestCasesRpcStdio_Service.h` templates that configure each generated assembly with the existing JSON serializer, JSON object/event operations, strong typed operations, id map, wrapper factory, and optional event attacher. No Workflow compiler, RPC runtime, or transport API change is required.

The driver side will compose the existing channel and JSON RPC classes over `StdioRedirectionServer`. For each case it will:

1. Start a fresh channel server and launch the supplied service command with the case name appended.
2. Wait for the remote service channel to be admitted as client 1.
3. Connect the driver RPC endpoint as local client 2.
4. Connect the JSON RPC broadcasting broker as local client 3, start its task queue, and register clients 1 and 2.
5. Initialize the driver lifecycle while waiting for the compiler-collected `@rpc:Ctor` service names, execute the existing `clientMain`, compare its result with the existing `IndexRpc.txt` expectation, finalize RPC, and stop the per-case server and task queue.

The service side will compose `StdioRedirectionClient` with the same channel/JSON RPC client layer, register the selected case's services through the existing `serviceMain`, initialize the dispatcher, and run its task queue until the parent closes the redirected connection. Stdout remains exclusively owned by the framed protocol. Both executables will retain fail-fast behavior for invalid arguments, unknown cases, transport failures, and result mismatches.

Add two no-reflection C++ application projects based on the existing C++/ChatBot test-project configuration. Each project will import the Workflow library, shared test types, and generated RPC assemblies; compile its generated dispatcher and manually maintained header; and be added to all four `UnitTest.sln` configurations. Add matching Linux `vmake` files and update `Project.md` with the two project roles, invocation contract, skip-file format, and generation relationship.

### CODE CHANGE

The attempted implementation extended `CompilerTest_LoadAndCompile` to generate driver and service dispatchers for all 126 `IndexRpc.txt` entries, added shared driver/service harness headers over `StdioRedirectionServer` and `StdioRedirectionClient`, added both application projects to all solution configurations, added Linux `vmake` files, and documented the projects in `Project.md`. The driver launched one service process per case, preserved endpoint client ids 1 and 2, configured the JSON RPC broker as client 3, ran `clientMain`, and compared its return value with the existing `IndexRpc.txt` result.

### DENIED

The projects and all 126 generated dispatch entries built successfully in Debug x64 with zero warnings and zero errors, and the first case completed its RPC exchange. However, the proposal's requirement to compare the split-process `clientMain` result with the old in-process expectation is invalid for the existing fixtures.

`Collection_Default` expects `[123][1234][12345]`, but the stdio run correctly produced `[123][][12345]`: `serviceMain` assigns the service process's `xsService`, while `clientMain` reads the distinct driver process's `xsService`. `Collection_InByref_OutByref` then terminated while formatting that empty driver-side collection. A CDB stack trace placed the exception in `Rpc_Collection_InByref_OutByref::Print5` and `clientMain`, after the RPC method had returned, rather than in the stdio channel or JSON dispatcher.

Static inspection found this shared-global pattern throughout all 96 collection fixtures and in several destructor, event, registration, and wrapper fixtures. An instrumented run that retained every RPC operation and exception check but only reported result differences completed all 70 cases whose result formatter did not index an empty service-side global. Core method, property, event, inheritance, exception, nullable, primitive, list-operation, and wrapper scenarios reached the expected transport behavior. Therefore the transport composition is viable, but exact comparison against the existing single-process oracle cannot be part of this proposal without changing the fixtures or introducing a nonstandard service-state synchronization protocol, either of which contradicts the intended reusable external-service boundary.

## No.2 Generate a fail-fast transport harness with fixture-controlled skipping

Retain the role-specific generator, projects, and JSON-over-stdio composition from No.1, but follow the executable contract literally at the process boundary. The driver dispatcher will enumerate every `IndexRpc.txt` case by name without importing the old single-process expected string. Each unskipped case will launch a fresh provider, initialize the requested services, execute the complete existing `clientMain`, and print its returned value for diagnostic and interoperability use. Any exception raised by `clientMain`, provider disconnect, malformed message, missing service, invalid argument, or other transport/lifecycle failure will remain uncaught and fail fast as requested.

The optional exact-name skip file is the compatibility boundary for existing fixtures whose client code directly reads service-owned module globals. This avoids changing the established Workflow fixtures and avoids adding a C++-specific hidden state protocol that external TypeScript or other service providers could not implement. It also preserves generation for all 126 indexed cases: providers can select any case, while a driver invocation can select the subset meaningful for that separate-process provider.

Verification will build both projects in all four Windows configurations, confirm generated driver/service dispatch coverage matches `IndexRpc.txt`, run the transport-compatible cases against the matching C++ service on Win32 and x64, exercise exact-name skipping, and run the repository's required existing native and TypeScript test sequence.

### CODE CHANGE
