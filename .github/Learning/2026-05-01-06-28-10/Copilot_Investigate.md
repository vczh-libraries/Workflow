# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Previously due to `Event<T>::operator` bug which moved arguments into multiple handlers, causing argument value (e.g. string or shared pointers) to be moved and become empty when passing to the second handler. Now the bug is fixed, we should remove the workaround.

In the generated wrapper Workflow script, when calling events, we have to prepare variables for each event argument before passing it (because in this way the operator() infer arguments to lvalue reference, std::forward does not move). We can remove the workaround, by not creating variables and pass values directly into events.

Follow [verify-and-commit.md](.github/Rules/verify-and-commit.md) to finish the work.

# UPDATES

# TEST [CONFIRMED]

Use the existing RPC event generated-code suite as the test signal. `Test/Generated/CppRpc64/EventArgs.cpp` currently confirms the workaround by emitting `auto arg0` and `auto arg1` local variables in `IRpcObjectEventOps::InvokeEvent`, then passing those variables into `EventInvoke`.

Success criteria:

- `Source/Analyzer/WfAnalyzer_GenerateRpc.cpp` should generate event invocation arguments directly from `RpcUnboxByval`/`RpcUnboxByref` expressions instead of creating one Workflow variable per event argument.
- Regenerated RPC event C++ should call `EventInvoke(...)(...)` with direct unbox/cast expressions and no per-event-argument `argN` locals in `InvokeEvent`.
- Existing RPC/runtime/generated C++ unit tests must pass through the verification flow required by `Project.md` and `.github/Rules/verify-and-commit.md`.

# PROPOSALS

- No.1 Pass generated RPC event arguments directly [CONFIRMED]

## No.1 Pass generated RPC event arguments directly

### CODE CHANGE

Implemented the generator cleanup and imported the required event-dispatch fix:

- Updated `Source/Analyzer/WfAnalyzer_GenerateRpc.cpp` so `BuildInvokeEventBranch` adds cast/unbox expressions directly to the generated event call instead of emitting one temporary variable per event argument.
- Regenerated RPC outputs. Event wrappers now emit calls such as `target.SomethingHappened((cast ...) RpcUnboxByval(...), (cast ...) RpcUnboxByref(...))` without `argN` locals in `InvokeEvent`.
- Imported the fixed `Event<void(TArgs...)>::operator()` into `Import/Vlpp.h`: single-handler events still forward rvalues, while multi-handler events call each handler with lvalue arguments to avoid moving from the same argument more than once.
- Regenerated release outputs via `..\Tools\Tools\Build.ps1 Workflow`.

### CONFIRMED

The proposal works because the generated event-dispatch script no longer constructs event-argument temporary variables, and the fixed `Event` implementation prevents direct rvalue arguments from being moved into the first handler when multiple handlers are attached.

Verification completed:

- Debug Win32/x64 build via `.github\Scripts\copilotBuild.ps1`.
- `LibraryTest` Debug Win32/x64.
- `CompilerTest_GenerateMetadata` Debug Win32/x64.
- `CompilerTest_LoadAndCompile` Debug x64.
- Post-generation Debug Win32/x64 rebuild.
- `RuntimeTest` Debug Win32/x64.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` Debug Win32/x64.
- Full `..\Tools\Tools\Build.ps1 Workflow` release verification.
- Generated-output scan found no `auto argN`/`var argN` event-argument unbox locals in the changed RPC event outputs.
