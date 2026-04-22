# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

In Test\Resources\Rpc\Dtor2.txt, there is some assertion to check if an object is expected or not expected to implement `IRpcWrapperBase`. Implementing `IRpcWrapperBase` means a remote object, not implementing means a local object. A local object could pass through lifecycle to another lifecycle and becomes a remote object.

You are going to add similar tests to Dtor/LocalAndWrapper/ServiceWrapper, whenever serviceMain or clientMain gets an object value through IRpcLifecycle or IService, do the test. Read IndexRpc.txt to get their expected output and you will easily figure out if an object implements `IRpcWrapperBase` or not.

Change these test cases, run all test projects to make sure your change work. git commit and push after finishing, DO NOT ASK ME ANY QUESTION

# UPDATES

# TEST [CONFIRMED]

- Add Dtor2-style `IRpcWrapperBase` assertions to the RPC samples named in the repro:
	- `Dtor`: check that the object returned by `IRpcLifeCycle::RequestService` in `clientMain` is a wrapper.
	- `LocalAndWrapper`: check that `Exchange1` receives a wrapper in `serviceMain`, `Exchange2` receives a local object in `serviceMain`, the service returned from `RequestService` is a wrapper in `clientMain`, `Exchange1` returns a wrapper in `clientMain`, and `Exchange2` returns a local object in `clientMain`.
	- `ServiceWrapper`: check that `Self` receives a local object in `serviceMain`, and the object returned by `RequestService` is a wrapper in `clientMain`.
- Use `CompilerTest_LoadAndCompile` `Debug|x64` as the focused check because it is the cheapest way to catch Workflow-script type or RPC-generation errors after editing the samples.
- Regenerate and validate the generated RPC harnesses touched by the samples in `Test/Generated/CppRpc32`, `Test/Generated/CppRpc64`, and `Test/SourceCppGenRpc`.
- Run the required debug project matrix after the focused check:
	- `LibraryTest` Win32 and x64
	- `CompilerTest_GenerateMetadata` Win32 and x64
	- `CompilerTest_LoadAndCompile` x64
	- rebuild Debug Win32 and x64
	- `RuntimeTest` Win32 and x64
	- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` Win32 and x64
- Run `Tools/Tools/Build.ps1 Workflow` as the final repo-wide validation.
- Success criteria:
	- the expected outputs in `IndexRpc.txt` remain unchanged for `Dtor`, `LocalAndWrapper`, and `ServiceWrapper`
	- the new assertions correctly distinguish wrapper versus local objects at each boundary described in the repro
	- the focused compiler test, full debug matrix, and repo-wide wrapper all complete successfully
- Validation results:
	- `CompilerTest_LoadAndCompile` `Debug|x64` passed after the final edit, including `LocalAndWrapper`, `ServiceWrapper`, `Dtor`, and `Dtor2`, with `Passed test files: 6/6` and `Passed test cases: 586/586`
	- the full required debug Win32/x64 matrix completed with exit code `0`
	- generated RPC execution still matched the `IndexRpc.txt` outputs, including `Rpc:LocalAndWrapper=[false][true][true][false]`, `Rpc:ServiceWrapper=[false][true]`, `Rpc:Dtor=[Not Deleted][Deleted]`, and `Rpc:Dtor2=[Not Deleted][Deleted]`
	- `Tools/Tools/Build.ps1 Workflow` completed successfully and refreshed release artifacts as part of the repo-wide validation

# PROPOSALS

- No.1 Add wrapper/local assertions to `Dtor`, `LocalAndWrapper`, and `ServiceWrapper` using Dtor2-style wrapper probes [CONFIRMED]

## No.1 Add wrapper/local assertions to `Dtor`, `LocalAndWrapper`, and `ServiceWrapper` using Dtor2-style wrapper probes

### CODE CHANGE

- Added a wrapper assertion in `Test/Resources/Rpc/Dtor.txt` so `clientMain` verifies that the service returned by `IRpcLifeCycle::RequestService` implements `system::IRpcWrapperBase`.
- Added wrapper/local assertions in `Test/Resources/Rpc/LocalAndWrapper.txt` for every object that crosses an RPC boundary through `IRpcLifeCycle` or `IService`.
- Added wrapper/local assertions in `Test/Resources/Rpc/ServiceWrapper.txt` for the `RequestService` result in `clientMain` and the `Self` argument received in `serviceMain`.
- For interface-typed values in `LocalAndWrapper` and `ServiceWrapper`, the probe first assigns the value to `object` and then performs `as (system::IRpcWrapperBase^)`, matching the working pattern from `Dtor2.txt`.
- Regenerated the corresponding checked-in generated files in `Test/Generated/Workflow32`, `Test/Generated/Workflow64`, `Test/Generated/CppRpc32`, `Test/Generated/CppRpc64`, and `Test/SourceCppGenRpc`.

### [CONFIRMED]

- The first attempt to probe `IRpcWrapperBase` directly from interface-typed values caused `CompilerTest_LoadAndCompile` to stop at `LocalAndWrapper` with `Assertion failure: manager.errors.Count() == 0`, which narrowed the issue to the type of the probe rather than the wrapper expectation itself.
- Switching those checks to the same object-based probe style used in `Dtor2.txt` fixed the compiler failure and preserved the expected runtime outputs from `IndexRpc.txt`.
- After the final sample edits, the focused compile check passed, the required debug Win32/x64 unit-test matrix passed, and the repo-wide `Tools/Tools/Build.ps1 Workflow` wrapper also passed.
- The confirmed fix is therefore to add the requested assertions while probing interface-typed values through `object` before testing `IRpcWrapperBase`.
