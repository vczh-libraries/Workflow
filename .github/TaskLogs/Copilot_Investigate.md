# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Good, do FailDoubleRegistration_SharedMemsp, test Worlflow and GacJS like the task you just finished, commit and push once finished

# UPDATES

# TEST [CONFIRMED]

The source history before commit `6e41bc8521ee2d61ff9f2dbf4b3be2d6c330a892` confirms that the original `FailDoubleRegistration` fixture exercised one concrete RPC object pointer being offered by two different lifecycles. Its first call assigned ownership of `objs[0]` to the client lifecycle and `objs[1]` to the service lifecycle; later calls deliberately offered each object from the opposite lifecycle and expected `PtrToRef` to reject the operation. This depends on `clientMain` and `serviceMain` sharing the same object instances and cannot execute across stdio processes.

The completed change must satisfy all of these conditions:

- Keep the current process-safe `FailDoubleRegistration` fixture and its `[exception]` expectation unchanged.
- Restore the historical object-ownership fixture as the three-file `FailDoubleRegistration_SharedMemsp` sample, using a distinct namespace and preserving its complete `[call][service:Received 1st][client:Received 2nd][call][service:Received 1st][exception][call][exception][call][exception]` oracle.
- Add the restored case to `IndexRpc.txt`, the compiler resource project and filters, all prescribed generated outputs, `StartRpcStdio_SharedMemspSkipList.txt`, and `StartRpcStdio_DtorSkipList.txt`.
- Workflow in-memory Runtime and generated-C++ tests must execute the restored case, while native stdio must continue to execute every destructor case and skip exactly the three `*_SharedMemsp` cases.
- GacJS must import and generate all 129 indexed cases, include a handwritten service registration for the restored interface, and skip the new same-memory case through the compatibility list.
- The complete Workflow and GacJS build/test procedures and both requested stdio integrations must pass.

# PROPOSALS

- No.1 Restore the original ownership-conflict fixture under the shared-memory convention [CONFIRMED]

## No.1 Restore the original ownership-conflict fixture under the shared-memory convention

Copy the historical `IObject`/`IService.SetObject` contract and four-call ownership-conflict sequence into a new `FailDoubleRegistration_SharedMemsp` sample. Split it according to the current RPC sample layout: the common file owns only RPC declarations, the client file owns the deliberately shared objects/log and client helper logic, and the service file owns only `serviceMain` while intentionally accessing that shared test state. Give the fixture a distinct namespace so it can coexist with the current service-registration fixture.

Keep the current `FailDoubleRegistration` untouched. Add the restored case to the index, resource projects, and both compatibility lists; regenerate all Workflow artifacts. Update GacJS's handwritten service map before import/codegen, update documented dynamic counts, and verify the full Workflow and GacJS matrices requested by the user.

### CODE CHANGE

- Restored the historical `IObject`/`IService.SetObject` ownership-conflict fixture as the distinct `FailDoubleRegistration_SharedMemsp` common, client, and service files. The split fixture preserves the original two shared objects, cross-lifecycle holds, four-call sequence, comments, and complete exception-timing oracle; only the namespace and explicitly typed shared log differ to coexist with the current split fixtures and compile across their source order.
- Kept the current process-safe `FailDoubleRegistration` source triplet and `[exception]` expectation unchanged. Added the restored case to `IndexRpc.txt`, the compiler resource project and filters, generated Workflow/metadata/C++ outputs, and both generated C++ project item lists.
- Added the restored case to `StartRpcStdio_SharedMemspSkipList.txt`, so native C++ stdio skips exactly the three shared-memory-only fixtures while continuing to execute all destructor fixtures. Added it to `StartRpcStdio_DtorSkipList.txt`, so external language providers additionally skip the eight cases that require deterministic destruction.
- Updated GacJS's handwritten service translation before import, preserving the service-side returned-object and hold behavior. Imported and generated the new binding/registry entry, updated the verification guide to the 129-case, 118-pass, 11-skip contract, and confirmed a second RPC code-generation pass was deterministic.

### CONFIRMED

The restored fixture reproduces the pre-`6e41bc852` four-call lifecycle ownership test under the in-memory harness, and the current process-safe fixture remains byte-for-byte unchanged. A structural audit found 129 unique indexed cases, exactly three native shared-memory skips, eleven external-provider compatibility skips, all three restored source files, all x86/x64 generated artifacts, and matching compiler/generated project references.

Workflow generation passed for x86 and x64 with 714/714 files. Debug Win32 and x64 solution builds completed with zero warnings and errors; Runtime passed 264/264 cases, and Cpp, Metaonly, and Reflection each passed 232/232 cases on both architectures with clean leak scans. TypeScript preparation/type checking and native stdio passed; native stdio executed all eight destructor cases and skipped only the three `*_SharedMemsp` cases. The canonical Release Workflow build passed the full UnitTest, TypeScript, native stdio, and packaging sequence.

GacJS import, two deterministic code-generation passes, generated-source linting, and all nine package builds passed. Its complete test run passed all ten packages, including the stdio integration. The standalone GacJS stdio run completed successfully with all 118 cross-process-compatible cases and exactly eleven intentional skips. A final audit confirmed that all 129 Workflow index entries have matching generated GacJS cases and handwritten service registrations, including the restored fixture's exact expected oracle.
