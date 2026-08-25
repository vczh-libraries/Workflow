# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

`TODO_Task.md` already perform refactoring to RPC test samples, here are more. For any test sample <SAMPLE> there is:
- Test/Resources/Rpc/<SAMPLE>.txt
- Test/Resources/Rpc/<SAMPLE>_Test.txt

Currently <SAMPLE>.txt has RPC declaration, and <SAMPLE>_Test.txt has interface implementation and test code.
Now we need to refactor them using a new pattern:
- <SAMPLE>.txt has RPC interface and shared code.
- <SAMPLE>_Client.txt has client side only code.
- <SAMPLE>_Service.txt has service side only code.

According to the previous `TODO_Task.md` refactoring task, we should be able to do:
- <SAMPLE>_Service.txt only has a `serviceMain` function, with no other global declaration..
- <SAMPLE>_Client.txt has `clientMain` and things that are only used directly or indirectly from `clientMain`.
- <SAMPLE>.txt will also include shared functions, that used directly or indirectly from both `serviceMain` and `clientMain`. There should be no shared variables, all `serviceMain` used variables are already 

In `TestRpcCompiler.cpp` in `CompileRpcSample`, after all input are parsed, call a function `VerifyRpcSample` to make sure that:
- <SAMPLE>_Service.txt only has one top level function declaration which is `serviceMain`, no need to check the signature.
- <SAMPLE>_Client.txt has a top level function declaration which is `clientMain`, no need to check the signature. Note that unlike <SAMPLE>_Service.txt, we don't require `clientMain` is "the only top level declaration".
- <SAMPLE>.txt should not have top level variable declarations.
  
Additionally:
- Update `Project.md` and `.github/Rules/new-sample-rpc.md` to say the new pattern. We still want that `clientMain` and `serviceMain` could run in different process, that's why they don't share any variable.
  - In `new-sample-rpc.md` move the pattern about rpc sample to a new section `## Rpc Sample Convention`.
  - In `Project.md`, we don't repeat the information, instead we should refer to `## Rpc Sample Convention` in `new-sample-rpc.md`. Remove duplicated things about rpc sample convention in `Project.md`.
- Update the demo in the task template `TaskTemplate_NewSample.md` to follow the new pattern.
- Since no semantic in test cases is actually changed, I would expect `IndexRpc.txt` should not be touched as the result should just be the same.
- To verify, both `StartRpcStdio.ps1` and `UnitTest` should be executed and ensure every single case passes.

# UPDATES

# TEST [CONFIRMED]

Refactor the existing RPC fixture files without changing test semantics, then use `CompilerTest_LoadAndCompile` as the structural and compilation test for all 126 indexed RPC samples:

- Every indexed sample must load `SAMPLE.txt`, `SAMPLE_Client.txt`, and `SAMPLE_Service.txt` successfully.
- `VerifyRpcSample` must reject a service module unless its only top-level declaration is a function named `serviceMain`, reject a client module without a top-level function named `clientMain`, and reject a definition/shared module containing a top-level variable declaration.
- `Test/Resources/IndexRpc.txt` must remain byte-for-byte unchanged.
- Generated executable behavior must remain unchanged: the full UnitTest suite and the no-skip `Test/StartRpcStdio.ps1` suite must pass every case.

The current compiler harness only loads `SAMPLE.txt` and `SAMPLE_Test.txt`, so it cannot load or verify the requested three-file convention. Renaming a fixture to the new `_Client.txt` / `_Service.txt` shape is therefore the direct reproduction: `CompilerTest_LoadAndCompile` cannot assemble that sample until the harness is updated.

The structural reproduction found 126 legacy `*_Test.txt` files and no `*_Client.txt` or `*_Service.txt` files. Its explicit three-file-convention assertion failed. `TestRpcCompile.cpp` also loads only `itemName + L"_Test"` and has no `VerifyRpcSample`, confirming that the requested layout and invariants are not implemented.

# PROPOSALS

- No.1 Split fixtures by process and validate parsed modules [CONFIRMED]

## No.1 Split fixtures by process and validate parsed modules

Replace every legacy `SAMPLE_Test.txt` with `SAMPLE_Client.txt` and `SAMPLE_Service.txt`. Preserve the existing module preamble in both process-specific files, move the complete `serviceMain` declaration into the service file, move all declarations after `serviceMain` into the client file, and move stateless shared functions that currently precede `serviceMain` into `SAMPLE.txt`. The previous RPC-fixture refactor already nested service-only state and helpers inside `serviceMain`, placed client-only declarations after it, and left only stateless cross-process helpers before it, so this split preserves behavior while making ownership explicit.

Update `TestRpcCompile.cpp` to parse all three inputs before compilation, call a new `VerifyRpcSample` on the parsed modules, and compile RPC metadata from the definition/shared module only. Reparse fresh syntax trees for linking and reconstruct the historical definition/test module layout before adding the generated wrapper modules, so the source ownership split does not alter declaration order or runtime assembly layout. The verifier will inspect direct module declarations: the service module must contain exactly one `WfFunctionDeclaration` named `serviceMain`, the client module must contain a `WfFunctionDeclaration` named `clientMain`, and the definition/shared module must not contain a `WfVariableDeclaration`.

Replace every legacy resource entry in the `CompilerTest_LoadAndCompile` project and filters with explicit client and service entries. Update generated-source comments to list the three inputs. Document the convention once in `new-sample-rpc.md`, make `Project.md` refer to it, and update the reusable sample guidance/template to avoid contradictory two-file instructions.

### CODE CHANGE

- Replaced all 126 `SAMPLE_Test.txt` fixtures with `SAMPLE_Client.txt` and `SAMPLE_Service.txt`. Moved 51 groups of shared stateless functions into their existing `SAMPLE.txt` definition modules. A normalized-text audit reconstructed every new file from the previous committed inputs and confirmed that no fixture logic changed.
- Added `VerifyRpcSample` to `TestRpcCompile.cpp`, parsed all three modules in `CompileRpcSample`, and invoked verification immediately after parsing. `LinkRpcSample` reparses fresh syntax trees and reconstructs the historical definition/test module layout before linking the flat and JSON wrappers, preserving declaration order and generated behavior while keeping the source files process-specific. Updated generated-source comments to name all three source files.
- Replaced all 126 legacy RPC resource entries in both `CompilerTest_LoadAndCompile.vcxproj` and its filters with explicit client and service entries. XML parsing and a static file-layout audit confirmed 126 definition, 126 client, and 126 service files with no legacy files.
- Centralized the three-file/process-ownership rules in `new-sample-rpc.md`, replaced the duplicated `Project.md` text with a link, and aligned `new-sample.md`, the learning documents, and `TaskTemplate_NewSample.md` with the new convention.
- Made the two observable-list event fixtures retain and detach their event-handler tokens at their terminal paths. This removes the reference cycles exposed by the complete leak-checked runtime matrix without changing their indexed output.
- Preserved `Test/Resources/IndexRpc.txt` byte-for-byte.

### CONFIRMED

The proposal implements and enforces the requested ownership boundary for every indexed RPC sample. The resource and project audits found 126 definition files, 126 client files, 126 service files, and no legacy `SAMPLE_Test.txt` files; both project XML files parsed successfully and contain exactly 126 client and 126 service entries. `CompilerTest_LoadAndCompile` exercised `VerifyRpcSample` for every fixture and passed 711/711 cases on both x64 and Win32. `IndexRpc.txt` retained SHA-256 `FDE03E98102BEF91FA1C6E729E04518DED74B3EB167416F95FD5AF1755AE4397`.

Both Debug builds completed with zero warnings and zero errors. The complete leak-checked unit-test matrix passed on x64 and Win32: `LibraryTest` 21/21, `CompilerTest_GenerateMetadata` 2/2, `CompilerTest_LoadAndCompile` 711/711, `RuntimeTest` 261/261, and each of `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` 229/229. No memory leak was reported. The no-skip stdio RPC suite matched the expected output for all 126 cases on both platforms. TypeScript preparation and `npm run build` also passed.

Generated-output review found the expected parsing-log updates for all 126 samples on both platforms. The generated C++ changes are provenance-only except for the two explicit event-handler lifecycle fixes, and assembly changes are limited to those same two fixtures on x64 and Win32. No stale `_Test` resource references remain in the active guidance or compiler project, and the fixed-width generated assembly listings are the only changed files reported by the whitespace audit.
