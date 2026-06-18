# !!!LEARNING!!!

# Orders

- Run TypeScript RPC schema verification after JSON or dispatcher-schema changes [12]
- Wire generated RPC C++ files into shared vcxitems after `CompilerTest_LoadAndCompile` [7]
- Run `CompilerTest_LoadAndCompile` at least once before downstream RPC generated-C++ tests [7]
- Compare `RuntimeTest` and `CppTest*` failures before choosing an RPC root cause [6]
- Preserve RPC/Workflow sample intent; only adjust syntax or diagnostics [6]
- Commit direct edits and large generated outputs separately when requested [5]
- Add every RPC sample resource to the `CompilerTest_LoadAndCompile` project folder [5]
- RPC byref/byval container samples must verify wrapper/copy semantics at every level [5]
- Validate ChatBot RPC apps with a real multi-process scenario [5]
- Pure refactors should not touch generated RPC outputs [5]
- Split RPC sample definitions and tests consistently [4]
- Type-check shared `Rpc.d.ts` standalone [3]
- Workflow analyzer error tests may change values when preserving the error code [1]
- Workflow samples use `raise`, not `throw` [1]
- Workflow range syntax for inclusive generated loops is `range [1, xs.Count]` [1]
- RPC destructor samples should track non-service objects when services never unregister [1]
- Enumerate generated app vcxitems explicitly [1]
- Use generated `Parsing.*.txt` logs for `CompilerTest_LoadAndCompile` Workflow compile errors [1]
- Add analyzer-error samples for reserved RPC transport types [1]
- Split broad RPC samples by focused behavior [1]
- Use static scans for Workflow library declaration and inline-refactor checks [1]
- Verify generated RPC file renames with stale-reference scans [1]
- Audit RPC JSON request transcripts for request id pairing [1]
- Track generated TypeScript RPC fixtures in git [1]
- Use 2-space indentation in Workflow sample files [1]
- Verify Workflow library file moves with stale-reference scans [1]

# Refinements

## Compare `RuntimeTest` and `CppTest*` failures before choosing an RPC root cause

For RPC behavior failures, keep running enough of `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` to see whether the failure exists in VM execution, generated C++ execution, or both. A mismatch often identifies whether the issue is in `RpcDualLifecycleMock` / runtime support, generated wrapper Workflow, generated C++, or project wiring.

## Wire generated RPC C++ files into shared vcxitems after `CompilerTest_LoadAndCompile`

When new or renamed RPC samples generate C++ files under `Test/SourceCppGenRpc`, add the corresponding `.cpp` / `.h` entries to `Test/UnitTest/Generated_CppRpc/Generated_CppRpc.vcxitems` and reflection files to `Test/UnitTest/Generated_ReflectionRpc/Generated_ReflectionRpc.vcxitems`. Link failures in `CppTest*` after successful `CompilerTest_LoadAndCompile` are often just missing shared-item entries or stale references to generated files that were renamed.

## Enumerate generated app vcxitems explicitly

Generated app shared-item projects such as `Test/UnitTest/Generated_Apps_ChatBot/Generated_Apps_ChatBot.vcxitems` should list generated `.cpp` and `.h` files explicitly. Do not use wildcard includes such as `*.cpp` or `*.h`; keep the file list reviewable and stable after `CompilerTest_LoadAndCompile` regenerates app output.

## Preserve RPC/Workflow sample intent; only adjust syntax or diagnostics

When implementing requested Workflow samples, keep the sample meaning intact and avoid changing parser/compiler/codegen surfaces unless the task explicitly allows it. It is acceptable to fix typos, legal Workflow syntax, namespace names, or add richer output that preserves the scenario being tested.

For exception samples such as `ListOps_OblistEventException`, keep service-side throwing handlers at the original point. Do not move the throwing behavior to the client side just to stabilize generated transcripts, because that removes the exception-handling path the sample exists to test.

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

For refactors that only reorganize code or move helper functions, generated RPC files should remain unchanged. If `CompilerTest_LoadAndCompile` updates generated files after such a refactor, investigate the behavior change instead of accepting the diff blindly. Release amalgamation files may still change after the required `Build.ps1 Workflow` step when source files changed.

This includes moving RPC analyzer files or extracting shared RPC AST helper functions: after `CompilerTest_LoadAndCompile` and the full required workflow, `Test/Generated` and generated Workflow/C++ test output folders should still have no diff.

If `Tools/Tools/Build.ps1 Workflow` rewrites dependency files under `Import` during a pure Workflow refactor, do not keep those generated dependency changes unless the task is explicitly about dependency import syncing. `Project.md` treats `Import` as prepared foreign dependency output, so restore those script-produced `Import` changes after confirming they are unrelated to the requested Workflow change.

## Add every RPC sample resource to the `CompilerTest_LoadAndCompile` project folder

Adding a sample to `IndexRpc.txt` is not enough for solution hygiene. Ensure every `Test/Resources/Rpc/*.txt` sample is also included under the `Resource Files/Rpc` folder in the `CompilerTest_LoadAndCompile` project and filters.

## Run `CompilerTest_LoadAndCompile` at least once before downstream RPC generated-C++ tests

Some compiler generated binaries are not covered by git. Before relying on `RuntimeTest` or `CppTest*` results for RPC work, run `CompilerTest_LoadAndCompile` at least once so metadata, wrapper scripts, and generated C++ inputs are current.

This is also required after changing RPC test harness signatures or generated wrapper helper contracts: stale generated C++ can keep calling the old harness until `CompilerTest_LoadAndCompile` refreshes `TestCasesRpc.cpp` and related generated files.

Generated app work has the same requirement. Run `CompilerTest_LoadAndCompile` after ChatBot RPC/app generation changes so `Test/Generated/Apps/ChatBot/Cpp` and the generated app shared items match the current compiler output before building or running the app projects.

## Run TypeScript RPC schema verification after JSON or dispatcher-schema changes

After changing RPC JSON serialization, generated `.d.ts` output, JSON value dumps, or the shared dispatcher schema, run `Test/TypeScript/prepare.ps1` and then `npm run build` in `Test/TypeScript`. When package files or dependencies change, run the package install step first (`npm install` or `npm ci` as appropriate). TypeScript failures often point to real mismatches between generated values and the documented RPC JSON schema.

This also applies when adding JSON request/response transcript folders such as `JsonRequest32` and `JsonRequest64`. Add them to `prepare.ps1` and `tsconfig.json`, then verify the TypeScript package against `Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>` values.

When schema declarations are centralized in `DataSchema32` and `DataSchema64`, `prepare.ps1` should copy `Serialization_*.d.ts` only into those folders. `JsonValues*` and `JsonRequest*` fixtures should import the schemas from `../DataSchema*`, then the TypeScript package build verifies that the generated imports and values still match.

When `Release/Rpc.d.ts` is the canonical shared dispatcher schema, `Test/TypeScript/prepare.ps1` should copy it into the package-local `Test/TypeScript/Rpc.d.ts` working file before verification. Keep the package-local copy generated/ignored instead of treating it as the source of truth.

## Split RPC sample definitions and tests consistently

Split RPC samples into `Rpc/SAMPLE.txt` for RPC definitions only and `Rpc/SAMPLE_Test.txt` for executable test logic such as globals, helpers, `serviceMain`, and `clientMain`. Add only `SAMPLE=expected` to `IndexRpc.txt`, but include both files under `Resource Files/Rpc` in `CompilerTest_LoadAndCompile`.

## Use 2-space indentation in Workflow sample files

When adding or updating Workflow samples, indent the sample body with 2 spaces even if provided draft snippets use a wider indent. Keep requested semantics intact while normalizing sample formatting to the repo preference.

## Type-check shared `Rpc.d.ts` standalone

When `Release/Rpc.d.ts` changes, type-check it directly with strict TypeScript settings before relying on the full package build. Run `Test/TypeScript/prepare.ps1` to refresh the package-local copy before the full package build. This catches envelope-schema mistakes even when no generated `Serialization_*.d.ts` file happens to instantiate the affected shape.

This is especially important when dispatcher request shapes are renamed or removed, such as replacing a local-service declaration request with a remote-service declaration request.

## Add analyzer-error samples for reserved RPC transport types

When RPC validation rejects internal transport types such as `system::RpcObjectReference` or `system::RpcException`, add AnalyzerError samples for each forbidden signature position: return/property value, method argument, and event argument. Verify the expected H-series error prefixes rather than only relying on runtime samples.

## Split broad RPC samples by focused behavior

When a single RPC sample grows to cover unrelated behavior, split it into focused samples that preserve the same core shape but isolate one expectation at a time, such as inherited member access, method exceptions, and event exceptions. Focused samples make expected strings shorter and failures easier to diagnose.

## Use static scans for Workflow library declaration and inline-refactor checks

For Workflow library header/source cleanup, use static scans alongside builds to verify mechanical invariants such as no non-`constexpr` inline function definitions remaining in `Source/Library/WfLibrary*.h`, `Source/Library/Rpc/WfLibrary*.h`, or `Source/Library/RpcJson/WfLibrary*.h`, and no namespace-level free-function forward declarations missing explicit `extern` in the matching library files.

## Verify Workflow library file moves with stale-reference scans

After moving Workflow library headers or source files, scan source includes, project files, filters, docs, and non-generated build metadata for old paths and class names. Also parse touched XML project files and check that every referenced moved file exists, because a successful source build can miss stale project/filter metadata.

## Verify generated RPC file renames with stale-reference scans

After generated RPC C++ file renames, verify that generated-file adds and deletes balance as expected for the rename, `TestCasesRpc.cpp` includes the new names, every path referenced by `Generated_CppRpc.vcxitems` and `Generated_ReflectionRpc.vcxitems` exists, and stale names or old `ObjectFileName` workarounds no longer appear in project, filter, or Linux vmake files.

## Audit RPC JSON request transcripts for request id pairing

When generating `Test/TypeScript/JsonRequest32` and `Test/TypeScript/JsonRequest64`, audit the transcript files in addition to type-checking them. Each test-case file should start request ids at 1, increment per request, and have exactly one response that reuses the matching request id. Sample representative files manually and prefer a small script for the complete per-file audit when many transcripts are generated.

## Track generated TypeScript RPC fixtures in git

Generated TypeScript RPC fixture files under `Test/TypeScript` should be checked in unless a task explicitly says otherwise. Do not leave `JsonValues*`, `JsonRequest*`, or similar generated `*.ts` fixture folders hidden behind `.gitignore` when the purpose is to review and verify stable generated JSON output.

## Validate ChatBot RPC apps with a real multi-process scenario

For ChatBot RPC app changes, run one `ChatBotServer` and three `ChatBotClient` processes against the real transport. Join clients one at a time, verify prior clients see join messages, send multiple chat rounds from every client, verify client exit notifications, then exit the server and confirm remaining clients shut down cleanly. Include server stdout expectations when the server mirrors client-visible service events.

For ChatBot dispatcher refactors, also use static scans under `Test/UnitTest/ChatBot*` to confirm no `Thread::Sleep`, dispatcher-owned task queue methods, or unnecessary explicit `Ptr<T>` conversion constructors remain in the touched source.

Also scan for stale class and file names after dispatcher splits or renames, including old dispatcher subclasses, old channel server adapters, and temporary registration guards that should have disappeared.
