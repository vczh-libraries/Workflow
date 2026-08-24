# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

  - `RpcStdioTest_Driver` and `RpcStdioTest_Service` should use `VCZH_DEBUG_NO_REFLECTION` and remove dependencies to reflection files.
  - `RpcStdioTest_Driver` should check test results against `IndexRpc.txt` just like `CppTest`. Crash when the result does not match.
  - Remove `RpcStdioTest_CppSkipped.txt`, and `StartRpcStdio.(ps1|sh)` will not by default use it when a skip list is not offered.
    - When a skip list is not offered it should just run every test cases.
    - Prepare a `Test/StartRpcStdio_DtorSkipList.txt` to collect all test cases that require the execution order of destructors. This is for some other languages like TypeScript or C#, which doesn't offer stable destructor execution like C++, skipping such cases is unavoidable. But for C++ this file should not be needed.
  - Currently almost all test case does not produce the correct result, you need to fix it. Make sure **every** test case in `IndexRpc.txt` is working correctly with `StartRpcStdio.ps1` when nothing is skipped.
  - Make sure all test cases actually pass, including `UnitTest` and `StartRpcStdio.ps1` running.
  - Update `Project.md`:
    - Running `StartRpcStdio.(ps1|sh)` is always required along with `UnitTest`.

# UPDATES

# TEST [CONFIRMED]

Build `Test/UnitTest/UnitTest.sln` in Debug x64, then run `Test/StartRpcStdio.ps1` with a checked-in list whose entries do not match any `IndexRpc.txt` case so the current default C++ compatibility skip list is bypassed. The problem is confirmed if any indexed case crashes, returns a value different from its `IndexRpc.txt` expectation, or is not checked against that expectation.

The completed change must satisfy all of these conditions:

- `RpcStdioTest_Driver` and `RpcStdioTest_Service` define `VCZH_DEBUG_NO_REFLECTION` in every configuration, do not import `Generated_ReflectionRpc`, and the generated stdio harnesses include ordinary generated headers rather than `*Reflection.h`.
- `RpcStdioTest_Driver` compares every non-skipped `clientMain` result with the exact expectation decoded from `IndexRpc.txt` and fails immediately on a mismatch.
- Calling `Test/StartRpcStdio.ps1` or `Test/StartRpcStdio.sh` without a skip-list argument runs every `IndexRpc.txt` case; no C++ default skip list remains.
- `Test/StartRpcStdio_DtorSkipList.txt` contains the exact indexed cases whose expected output depends on deterministic destructor execution, for use by providers that cannot guarantee it, but the C++ launchers do not select it automatically.
- Every indexed RPC case succeeds through `Test/StartRpcStdio.ps1` without a skip list, all UnitTest projects pass in their required configurations, generated artifacts and TypeScript consumers remain valid, and the final code review/static scans show no stale reflection or removed-skip-list dependency.

The current Debug x64 solution builds with 0 warnings and 0 errors. Passing `IndexCodegen.txt` as the explicit skip-list argument selects no RPC case names and therefore forces the current driver to begin running the complete index. `Collection_Default` returned `[123][][12345]` instead of the indexed `[123][1234][12345]`; `Collection_InByref_OutByref` then called `abort()` before returning because its client-side formatter indexed the empty driver-process copy of `xsService`.

The current no-argument launcher hides 56 crashing collection fixtures behind `RpcStdioTest_CppSkipped.txt` and exits with code 0 for the remaining 70 cases, but the output demonstrates that it is not a passing test: every completed collection case has an empty service-state bracket; the destructor cases omit service-process destructor messages; `Event` and `EventOblist` omit service-process event messages; `FailDoubleRegistration` no longer exercises the intended duplicate identities; and `LocalAndWrapper` loses a service-process equality result. `RpcStdioTest_Driver` only prints these wrong values and never compares them with `IndexRpc.txt`.

Source inspection confirms the same process-boundary cause. The affected samples store observations in module globals written by `serviceMain` or service implementations, while `clientMain` reads a distinct copy in the driver process. The generated stdio harnesses also include `*Reflection.h`, and both stdio projects import `Generated_ReflectionRpc`, despite already defining `VCZH_DEBUG_NO_REFLECTION` in every configuration.

# PROPOSALS

- No.1 Make RPC fixtures process-independent and enforce the index contract

## No.1 Make RPC fixtures process-independent and enforce the index contract

Keep the production RPC transport and lifecycle behavior unchanged. The failures come from tests observing service-owned state through same-process module globals, so make those observations part of each affected test service contract instead:

- Collection fixtures continue checking byval/byref wrapper and copy semantics at every existing level, but retrieve the formatted service-side collection through a service method rather than reading the driver process's empty `xsService`.
- Event fixtures keep distinct client and service logs and retrieve the service log through RPC, preserving the indexed ordering without relying on one shared `s` variable.
- Destructor fixtures keep client-owned and service-owned destructor logs separate, retrieve service-side checkpoints synchronously, and preserve the existing C++ ordering assertions. The eight `Dtor*` cases are also listed in `Test/StartRpcStdio_DtorSkipList.txt` for providers whose languages do not guarantee deterministic destructor execution.
- `LocalAndWrapper` retrieves comparisons involving service-owned objects from the service itself. Replace `FailDoubleRegistration`'s impossible cross-process shared-pointer-alias setup with a direct duplicate `IRpcLifecycle::RegisterLocalService` rejection test, retaining the fixture's intended duplicate-registration failure semantics on both same-process and stdio lifecycles.

Update `CompilerTest_LoadAndCompile`'s stdio harness generation to include the normal generated assembly entry headers, pass each decoded `IndexRpc.txt` expectation into `RunRpcStdioTestCase`, and fail immediately when `clientMain` returns a different value. Remove `Generated_ReflectionRpc` from both stdio projects while retaining `VCZH_DEBUG_NO_REFLECTION` in every configuration.

Remove the C++ compatibility skip list and make both launchers omit the optional skip argument unless the caller explicitly supplies one. Add the destructor portability list at the requested `Test` path and update `Project.md` so a no-skip `StartRpcStdio` run is mandatory alongside UnitTest verification.

### CODE CHANGE
