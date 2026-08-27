# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Good, now do this, while keeping the current `LocalAndWrapper` and `ServiceWrapper`, you restore original ones and rename original ones to `LocalAndWrapper_`SharedMemsp and `ServiceWrapper_`SharedMemsp :

- In `Project.md` it refers to the pattern writting rpc samples, it might refer another document I can't remember, add in the actual file saying that `*_SharedMemsp` is supposed to be tested when `clientMain` and `serviceMain` could run in the same memory space
- Fix [IndexRpc.txt](Test/Resources/IndexRpc.txt) to have the restored test samples
- Fix the current `LocalAndWrapper` and `ServiceWrapper` to remove assertions that test nothing, also fix [IndexRpc.txt](Test/Resources/IndexRpc.txt)&#x20;
- In [StartRpcStdio\_DtorSkipList.txt](Test/StartRpcStdio_DtorSkipList.txt) , add the two restored test case, because these can't be tested when `serviceMain` and `clientMain` are in different processes.
- Run `UnitTest`, [StartRpcStdio.ps1](Test/StartRpcStdio.ps1) , the `StartRpcStdio.ps1` in the sibling GacJS repo. Since test samples are changed, you need to re-import and change manually translated `serviceMain` for `LocalAndWrapper` and `ServiceWrapper` in TypeScript. Also check GacJS repo's `AGENTS.md` , I believe there are rules about re-import when rpc test samples are changed, review those words and see if it mentioned, and if not please add, that `Workflow` commits should be reviewed and see if any rpc test samples are changed, and if yes, update manually translated code before re-import.
- commit and push all local changes once finishing.

# UPDATES

## UPDATE

One thing to correct, since Workflow's StartRpcStdio.(ps1|sh) with both side C++, doesn't need to skip the dtor stuff, so you need to create a skip list which just contain the two \*\_SharedMemsp test samples, and feed this file to StartRpcStdio.(ps1|sh)

# TEST [CONFIRMED]

Use the existing RPC fixture source and indexes to confirm the coverage regression, then use the restored fixtures and all required native/TypeScript verification to confirm the change:

- Before the change, `IndexRpc.txt` has only `LocalAndWrapper` and `ServiceWrapper`; no `*_SharedMemsp` fixture exists.
- `LocalAndWrapper` returns two `false` identity comparisons against unrelated, never-transmitted objects, and `ServiceWrapper` returns a redundant `clientObj is null` result.
- The restored `LocalAndWrapper_SharedMemsp` and `ServiceWrapper_SharedMemsp` fixtures must use the original shared-memory identity assertions and run in the in-memory Runtime/Cpp test harnesses.
- The process-safe `LocalAndWrapper` and `ServiceWrapper` fixtures must retain wrapper/local and owner-side round-trip assertions while removing the meaningless return components.
- `IndexRpc.txt` must contain the correct expected results for all four cases. `StartRpcStdio_DtorSkipList.txt` must add the two same-memory fixtures to its eight destructor-sensitive cases for language-provider verification, while the native C++ `StartRpcStdio.ps1` and `StartRpcStdio.sh` runs must use a new skip list containing only the two same-memory fixtures.
- Workflow sample guidance must document the `*_SharedMemsp` convention in the actual RPC convention document.
- GacJS guidance must require reviewing Workflow commits for RPC sample changes, updating handwritten service implementations first, and then re-importing.
- Workflow UnitTest verification, Workflow `StartRpcStdio.ps1`, GacJS import/codegen/build/test, and GacJS `StartRpcStdio.ps1` must all pass.

The source audit confirms the initial state described above, including the two weakened assertions and the missing fixtures/skip-list entries. GacJS `AGENTS.md` currently documents re-import after sibling GacUI changes but does not document the Workflow RPC-sample review/manual-translation requirement.

# PROPOSALS

- No.1 Restore same-memory fixtures and simplify process-safe coverage [CONFIRMED]

## No.1 Restore same-memory fixtures and simplify process-safe coverage

Restore the pre-refactor identity tests as `LocalAndWrapper_SharedMemsp` and `ServiceWrapper_SharedMemsp`, keeping their service and client code in separate convention files but deliberately using shared module variables because these cases are restricted to the in-memory harness. Document this suffix exception in `.github/Rules/new-sample-rpc.md`, add both cases to `IndexRpc.txt`, compiler project resources, the language-provider compatibility skip list, and a new native C++ stdio skip list containing only these two cases. Update both Windows and Linux canonical Workflow build entry points to pass the new native skip list to `StartRpcStdio.ps1` or `StartRpcStdio.sh`.

Simplify the process-safe cases so they return only meaningful owner-side round-trip identity results while retaining all explicit `IRpcWrapperBase` wrapper/local assertions. Regenerate compiler, metadata, C++, and TypeScript outputs through the prescribed Workflow tools.

Update the GacJS handwritten service implementations for the changed process-safe cases and add implementations for the restored same-memory cases as required by its import/code-generation flow. Extend GacJS `AGENTS.md` with the missing Workflow-change review rule, then run the complete Workflow and GacJS validation requested by the user.

### CODE CHANGE

- Restored the original identity tests as the distinct `LocalAndWrapper_SharedMemsp` and `ServiceWrapper_SharedMemsp` source triplets. The restored Local case keeps the original service-side `GetServiceResult` oracle and both client-side identity comparisons; the restored Service case keeps the original cross-entrypoint service identity comparison.
- Reduced the process-safe `LocalAndWrapper` expectation to `[true][true]` and `ServiceWrapper` to `[true]`, removing only comparisons against unrelated or already-proven objects. Their wrapper/local assertions and owner-side round-trip identity checks remain intact.
- Added both restored cases and expectations to `IndexRpc.txt`, the compiler resource project, generated Workflow/metadata/C++ outputs, and generated project item lists.
- Added both cases to `StartRpcStdio_DtorSkipList.txt` for external language providers. Added `StartRpcStdio_SharedMemspSkipList.txt` with only those two cases, and updated the canonical Windows and Linux Workflow build entry points in the sibling Tools repository to pass it to the native stdio scripts. Native C++ therefore continues to execute all destructor cases.
- Documented the `*_SharedMemsp` convention in `.github/Rules/new-sample-rpc.md`, with matching references in `Project.md` and the testing guidance.
- Updated GacJS's current and restored handwritten service translations, imported and regenerated all 128 RPC cases, documented the 118-pass/10-skip compatibility contract, and added the missing Workflow RPC-change review order to GacJS `AGENTS.md`.

### CONFIRMED

The restored fixtures reproduce the pre-refactor identity oracles under the in-memory harness, while the process-safe fixtures now report only meaningful results. A structural audit found 128 unique indexed cases, exactly two native shared-memory skips, ten external-provider compatibility skips, all six restored source files, all x86/x64 generated artifacts, and matching project references.

Workflow generation passed for x86 and x64 with 713/713 files. Debug Win32 and x64 solution builds completed with zero errors and warnings; Runtime passed 263/263 cases, and Cpp, Metaonly, and Reflection each passed 231/231 cases on both architectures with clean leak scans. The canonical Release Workflow build also passed the complete UnitTest, TypeScript, native stdio, and packaging sequence. The native stdio run executed all eight destructor cases and skipped only the two `*_SharedMemsp` cases.

GacJS import, code generation, generated-source linting, and the build of all nine packages passed. Its complete test run passed all ten packages, and its stdio integration executed all 118 cross-process-compatible cases successfully while skipping exactly the eight destructor and two shared-memory cases. A final audit confirmed that all 128 Workflow index entries have matching generated GacJS cases and handwritten service registrations, including the exact restored Local service oracle. The Windows and Linux Tools entry-point changes pass their PowerShell and Bash parser checks.
