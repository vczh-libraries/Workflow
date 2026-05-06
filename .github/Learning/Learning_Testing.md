# !!!LEARNING!!!

# Orders

- Compare `RuntimeTest` and `CppTest*` failures before choosing an RPC root cause [6]
- Wire generated RPC C++ files into shared vcxitems after `CompilerTest_LoadAndCompile` [5]
- Preserve RPC/Workflow sample intent; only adjust syntax or diagnostics [5]
- RPC byref/byval container samples must verify wrapper/copy semantics at every level [5]
- Commit direct edits and large generated outputs separately when requested [4]
- Workflow analyzer error tests may change values when preserving the error code [1]
- Workflow samples use `raise`, not `throw` [1]
- Workflow range syntax for inclusive generated loops is `range [1, xs.Count]` [1]
- RPC destructor samples should track non-service objects when services never unregister [1]
- Use generated `Parsing.*.txt` logs for `CompilerTest_LoadAndCompile` Workflow compile errors [1]
- Pure refactors should not touch generated RPC outputs [1]
- Add every RPC sample resource to the `CompilerTest_LoadAndCompile` project folder [1]
- Run `CompilerTest_LoadAndCompile` at least once before downstream RPC generated-C++ tests [1]

# Refinements

## Compare `RuntimeTest` and `CppTest*` failures before choosing an RPC root cause

For RPC behavior failures, keep running enough of `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` to see whether the failure exists in VM execution, generated C++ execution, or both. A mismatch often identifies whether the issue is in `RpcDualLifecycleMock` / runtime support, generated wrapper Workflow, generated C++, or project wiring.

## Wire generated RPC C++ files into shared vcxitems after `CompilerTest_LoadAndCompile`

When new RPC samples generate C++ files under `Test/SourceCppGenRpc`, add the corresponding `.cpp` / `.h` entries to `Test/UnitTest/Generated_CppRpc/Generated_CppRpc.vcxitems` and reflection files to `Test/UnitTest/Generated_ReflectionRpc/Generated_ReflectionRpc.vcxitems`. Link failures in `CppTest*` after successful `CompilerTest_LoadAndCompile` are often just missing shared-item entries.

## Preserve RPC/Workflow sample intent; only adjust syntax or diagnostics

When implementing requested Workflow samples, keep the sample meaning intact and avoid changing parser/compiler/codegen surfaces unless the task explicitly allows it. It is acceptable to fix typos, legal Workflow syntax, namespace names, or add richer output that preserves the scenario being tested.

## RPC byref/byval container samples must verify wrapper/copy semantics at every level

For RPC collection samples, byref means a wrapper should be observed on the receiving side; byval means a copied non-wrapper should be observed. Nested containers and containers containing interfaces need checks at both outer and inner levels, and interface elements should follow normal lifecycle wrapper/local-object rules.

## Commit direct edits and large generated outputs separately when requested

For tasks that add or modify samples or generator code and cause many generated outputs to change, the user often wants one commit for direct/source/sample edits and a separate commit for auto-generated files (`Test/Generated`, `Test/SourceCppGen`, `Test/SourceCppGenRpc`, `Test/UnitTest/Generated_*`). Follow the task-specific commit instruction when present.

## Workflow analyzer error tests may change values when preserving the error code

Analyzer error samples exist to trigger specific error codes. If a refactor makes old attribute values invalid (for example because test attributes must use primitive reflected fields), the sample values can change as long as the intended error type/code remains the same; error text changes are acceptable.

## Workflow samples use `raise`, not `throw`

Workflow script samples should use `raise` for exceptions. `throw` is not valid Workflow syntax and will fail during `CompilerTest_LoadAndCompile` before runtime behavior is involved.

## Workflow range syntax for inclusive generated loops is `range [1, xs.Count]`

The syntax `range [1..xs.Count]` does not parse in this codebase. For inclusive loops over dictionary keys or indexes from one to count, use `for (key in range [1, xs.Count])`.

## RPC destructor samples should track non-service objects when services never unregister

Registered services are intentionally not unregistered during normal RPC lifetime. Destructor tests that need to observe deletion should move the destructor to a separate value/interface object and manipulate that object's lifetime, not the registered service object itself.

## Use generated `Parsing.*.txt` logs for `CompilerTest_LoadAndCompile` Workflow compile errors

When `CompilerTest_LoadAndCompile` runs stably and reports Workflow compile errors, inspect the generated `Test/Generated/Workflow(32|64)/Parsing.CATEGORY.SAMPLE.txt` logs first. A debugger is more useful for crashes or runtime failures than for ordinary Workflow semantic diagnostics.

## Pure refactors should not touch generated RPC outputs

For refactors that only reorganize code or move helper functions, generated RPC files should remain unchanged. If `CompilerTest_LoadAndCompile` updates generated files after such a refactor, investigate the behavior change instead of accepting the diff blindly.

## Add every RPC sample resource to the `CompilerTest_LoadAndCompile` project folder

Adding a sample to `IndexRpc.txt` is not enough for solution hygiene. Ensure every `Test/Resources/Rpc/*.txt` sample is also included under the `Resource Files/Rpc` folder in the `CompilerTest_LoadAndCompile` project and filters.

## Run `CompilerTest_LoadAndCompile` at least once before downstream RPC generated-C++ tests

Some compiler generated binaries are not covered by git. Before relying on `RuntimeTest` or `CppTest*` results for RPC work, run `CompilerTest_LoadAndCompile` at least once so metadata, wrapper scripts, and generated C++ inputs are current.
