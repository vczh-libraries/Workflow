# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

- `Tools` repo update:
  - Add `StartRpcStdio.sh` to `vgo vbuild Workflow`.
  - Add `StartRpcStdio.ps1` to `Build.ps1 -Project Workflow`.
- You can find a recent commit making `StartRpcStdio.(ps1|sh)` to work:
  - The root cause is the original test cases assume service and client are running in the same memory space, so in that commit they are separated. But the change is not making the script obvious enough for that intent.
  - Further improvements are required for each rpc test case Workflow script (txt files)
    - In the script there are multiple global variables, remove which is no longer used.
    - If a global variable, or a global function is only used (including recursively/indirectly) in service side, move it inside the service class (inside `new (IService^)` inside `serviceMain`).
    - Otherwise, such global variable or function should only be used in client side, or when a function is stateless it could be shared between service/client side:
      - If a global variable or a global function is not used (including recursively/indirectly) in service side, move them below `serviceMain`.
    - Add the above policy to `Project.md` and `.github/Rules/new-sample-rpc.md`.
  - Since no semantic in test cases is actually changed, I would expect `IndexRpc.txt` should not be touched as the result should just be the same.
- To verify, both `StartRpcStdio.ps1` and `UnitTest` should be executed and ensure every single case passes.

# UPDATES

# TEST [CONFIRMED]

Use the existing `Test/Resources/IndexRpc.txt` contract and its generated C++/stdio harnesses to verify that reorganizing RPC fixture globals does not change behavior. The investigation must first inventory every module-level variable and function in `Test/Resources/Rpc/*_Test.txt`, classify direct and transitive service-side usage from `serviceMain`, and confirm the current files violate the requested ownership/layout policy.

The completed change must satisfy all of these conditions:

- No unused module-level variable or function remains in any indexed RPC test fixture.
- Every stateful variable or function used only by the service side is a member of the anonymous service implementation created inside `serviceMain`; service-side helpers continue to access the same state and preserve call order.
- Every client-only module-level variable or function is declared below `serviceMain`; stateless helpers shared by both sides may remain above `serviceMain`.
- `Project.md` and `.github/Rules/new-sample-rpc.md` document the same ownership/layout policy.
- `Test/Resources/IndexRpc.txt` is byte-for-byte unchanged.
- The Workflow repository UnitTest verification and `Test/StartRpcStdio.ps1` without a skip list both pass every case.
- The Tools repository build orchestration invokes `Workflow/Test/StartRpcStdio.sh` from `vgo vbuild Workflow` and `Workflow/Test/StartRpcStdio.ps1` from `Build.ps1 -Project Workflow`.

The current repository contains 126 indexed RPC test fixtures. A structural inventory found module-level declarations before `serviceMain` in 124 fixtures: 123 files have module-level variables and 61 files have module-level helper functions. Representative violations include the service-owned `xsService` collection state in all collection fixtures, service-owned result state and callbacks in event/destructor fixtures, and client-only result variables and handlers declared above `serviceMain`. Only `PrimitiveTypes_Test.txt` and `RequestService_Test.txt` already have no such declarations.

The inventory also confirms why the organization is misleading: commit `6e41bc852` fixed the stdio process boundary by adding service methods that return service-owned observations, but retained those observations as module globals. The service and driver now have distinct copies of the same declarations even though each stateful copy belongs to only one process.

The baseline Debug x64 solution builds successfully with 0 warnings and 0 errors. `Test/Resources/IndexRpc.txt` currently has SHA-256 `FDE03E98102BEF91FA1C6E729E04518DED74B3EB167416F95FD5AF1755AE4397`, which will be used to confirm the behavioral contract was not edited.

# PROPOSALS

- No.1 Make RPC fixture process ownership explicit and make stdio verification canonical [CONFIRMED]

## No.1 Make RPC fixture process ownership explicit and make stdio verification canonical

Reorganize every indexed `*_Test.txt` fixture according to the process that owns each declaration, without changing any RPC contract or expected result:

- Move service-only state into the anonymous `IService`/`IServer` implementation created by `serviceMain`, adding explicit member types where module-level inference was previously used.
- Move service-only helper functions into that same anonymous implementation, including helpers reached transitively from service methods.
- Keep client-only state and helper functions at module scope immediately below `serviceMain` and before `clientMain`.
- Keep a stateless helper at module scope when it is genuinely shared by service and client code. Remove declarations that become unused.
- Split deliberately duplicated cross-process identity objects in `LocalAndWrapper_Test.txt` and `ServiceWrapper_Test.txt` into clearly named service-owned and client-owned declarations so the existing equality checks retain the same meaning without implying shared memory.

Apply the same policy text to `Project.md` and `.github/Rules/new-sample-rpc.md`, emphasizing that the separation is required even though the in-memory harness can make a module global appear shared. Preserve `IndexRpc.txt` exactly and allow `CompilerTest_LoadAndCompile` to regenerate derived outputs from the manually edited Workflow fixtures.

Make stdio verification part of both canonical build entry points:

- In the Tools repository, make `vgo vbuild Workflow` run `Workflow/Test/StartRpcStdio.sh` after all Workflow vmake projects have built, capture a nonzero exit in the existing `.vbuild/Workflow` failure log, and pass no skip list.
- Allow `Workflow/Test/StartRpcStdio.ps1` to select an explicitly named configuration/platform while retaining Debug x64 as its no-argument default. Invoke it in a child PowerShell process from the Tools Workflow build path using the already-built Release x64 binaries, so the script's deliberate `exit` cannot terminate the parent `Build.ps1` before release generation completes.

### CODE CHANGE

Reorganized the 124 noncompliant fixtures in `Test/Resources/Rpc/*_Test.txt` without changing their indexed inputs or expected outputs. Service-owned collections, result state, callbacks, and transitively service-only helpers are now members of the anonymous service implementation in `serviceMain`. Client-owned state and handlers now follow `serviceMain`; the only declarations left above it are stateless helpers shared by both processes. Unused declarations were removed. Fixtures that intentionally compare wrapped or local identities now use separately named service and client objects, and destructor fixtures use service-owned collection members so returned closures retain writable state under Workflow's capture rules.

Added the ownership/layout policy to `Project.md` and `.github/Rules/new-sample-rpc.md`. Regenerated the C++ and expected compiler artifacts exclusively through `CompilerTest_LoadAndCompile`; `Test/Resources/IndexRpc.txt` was not edited.

Extended `Test/StartRpcStdio.ps1` with validated configuration and platform parameters while retaining Debug x64 as the default and preserving optional skip-list behavior. Updated the Tools repository so `Build.ps1 -Project Workflow` launches the Release x64 stdio suite in a child process using the current PowerShell host executable, which works under both Windows PowerShell and PowerShell 7. `vgo vbuild Workflow` runs `StartRpcStdio.sh` and records failures in the existing Workflow vbuild log.

### CONFIRMED

The final structural audit covers all 126 indexed fixtures: no module-level variables remain above `serviceMain`, no unused module-level declarations remain, and the 127 functions still above `serviceMain` are the intended stateless shared helpers (`CheckList`, `CheckValue`, `FormatItemChanged`, `MakeValue`, and `Modify`). `IndexRpc.txt` remains byte-for-byte unchanged at SHA-256 `FDE03E98102BEF91FA1C6E729E04518DED74B3EB167416F95FD5AF1755AE4397`.

`CompilerTest_LoadAndCompile` regenerated and compiled every fixture for both x86 and x64, passing 711/711 cases on each target. The Debug Win32 and x64 solutions build with 0 warnings and 0 errors. The complete UnitTest matrix passes on both targets: LibraryTest 21/21, CompilerTest_GenerateMetadata 2/2, RuntimeTest 261/261, CppTest 229/229, CppTest_Metaonly 229/229, and CppTest_Reflection 229/229; the compiler load-and-compile run covers both generated targets as noted above. `Test/StartRpcStdio.ps1` passes all 126 cases with no skip list in both Debug Win32 and Debug x64. The TypeScript preparation and `tsc --noEmit` build also pass.

The PowerShell parser accepts both modified `.ps1` files, `bash -n` accepts the modified `vgo`, and all manually edited files pass `git diff --check`. These results confirm that the fixture organization now exposes the real service/client process boundary while preserving behavior, and that both canonical build entry points include the stdio verification requested by the proposal.

The canonical `Tools/Build.ps1 Workflow` run passes the complete Release Win32/x64 build and test matrix, the TypeScript build, all 126 Release x64 stdio cases through the new Tools hook, and release generation. This provides end-to-end confirmation of the Windows orchestration rather than relying only on a direct invocation of the test script.
