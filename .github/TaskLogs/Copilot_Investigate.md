# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Follow job.new-sample.md to add a new `Rpc\FailDoubleRegistration.txt`

- Keep the sample meaning intact except if needed to compile.
- Do not modify Workflow parser.
- Do not modify Workflow compiling.
- Do not modify Workflow to C++ code generation.
- If runtime or generated-C++ tests fail, fix `RpcDualLifecycleMock` and connected interfaces first.
- Pass all unit test, fix any test failure including pre-existings.
- After finishing everything, git commit and git push to the current branch.
- DO NOT ASK ME ANY QUESTION.

# UPDATES

## UPDATE

I approve your idea of the original work but I would like you to do one little refactoring, don't add RpcDualObjectOwner, you should just add information to RpcDualObjectTracker. And I believe RpcDualObjectTracker::ref should have written the client id there. By the way rename `RpcDualObjectTracker` to `RpcDualLocalObjectTracker`. No more embedded property types need to add

# TEST [CONFIRMED]

- Existing coverage is sufficient:
	- `CompilerTest_LoadAndCompile` compiles the new `Test/Resources/Rpc/FailDoubleRegistration.txt`, regenerates RPC generated sources, regenerates RPC metadata, and updates the generated `TestCasesRpc.cpp` expectations.
	- `RuntimeTest` compares the workflow runtime result of `Rpc/FailDoubleRegistration` against `Test/Resources/IndexRpc.txt`.
	- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` compare the generated C++ result of `Rpc:FailDoubleRegistration` against the regenerated `TestCasesRpc.cpp` expectation.
	- Because the user explicitly asked to pass all unit tests including pre-existing failures, the full unit-test executable matrix for this solution is also part of the acceptance criteria.
- Success criteria:
	- The new sample compiles and regenerates all expected generated artifacts.
	- `RuntimeTest` logs `FailDoubleRegistration=[call][service:Received 1st][client:Received 2nd][call][service:Received 1st][exception][call][exception][call][exception]`.
	- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` produce the same result as `RuntimeTest`.
	- The full unit-test executable set passes in both `Debug|x64` and `Debug|Win32`: `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`.
- Confirmation:
	- `Debug|x64`: solution build succeeded, and `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` all passed.
	- `Debug|Win32`: solution build succeeded, and the same executable matrix all passed. `CompilerTest_LoadAndCompile` ended with `Passed test files: 6/6` and `Passed test cases: 588/588`.
	- `RuntimeTest` and every `CppTest*` variant now agree on `FailDoubleRegistration=[call][service:Received 1st][client:Received 2nd][call][service:Received 1st][exception][call][exception][call][exception]` in both architectures.

# PROPOSALS

- No.1 Enforce Persistent RPC Ownership With Catchable Lifecycle Exceptions [DENIED]
- No.2 Persist Ownership In RpcDualLocalObjectTracker [CONFIRMED]

## No.1 Enforce Persistent RPC Ownership With Catchable Lifecycle Exceptions

### CODE CHANGE

- Added `Test/Resources/Rpc/FailDoubleRegistration.txt` as the new RPC sample, preserving its intended ownership semantics and only normalizing syntax where required for compilation.
- Updated `Test/Resources/IndexRpc.txt` with the expected `FailDoubleRegistration` log string.
- Added `FailDoubleRegistration.txt` to `Test/UnitTest/CompilerTest_LoadAndCompile/CompilerTest_LoadAndCompile.vcxproj` and `.filters` so the compile/regeneration test includes the new sample.
- Regenerated the new RPC outputs through `CompilerTest_LoadAndCompile`, which updated `Test/SourceCppGenRpc/TestCasesRpc.cpp`, produced the generated `FailDoubleRegistration` source and reflection files in `Test/SourceCppGenRpc`, and produced the generated dumps and metadata in `Test/Generated/CppRpc32`, `Test/Generated/CppRpc64`, `Test/Generated/RpcMetadata32`, `Test/Generated/RpcMetadata64`, `Test/Generated/Workflow32`, and `Test/Generated/Workflow64`.
- Added the generated `FailDoubleRegistration` implementation and reflection files to `Test/UnitTest/Generated_CppRpc/Generated_CppRpc.vcxitems` and `Test/UnitTest/Generated_ReflectionRpc/Generated_ReflectionRpc.vcxitems` so the generated C++ test projects compile and link the new case.
- Updated `Test/Source/RpcDualLifecycleMock.h` and `Test/Source/RpcDualLifecycleMock.cpp` to preserve first-touch ownership across transient tracking:
	- introduced `RpcDualObjectOwner` and `InternalProperty_LocalObjectOwner` on `DescriptableObject` instances;
	- checked the owner and live tracker in `TrackLocalObject` and `PtrToRef` so an object cannot be silently re-registered by the opposite lifecycle after the temporary tracker disappears;
	- changed the expected cross-lifecycle ownership conflicts from `CHECK_ERROR`-based `vl::Error` throws to explicit `vl::Exception` throws so the Workflow sample `try/catch` logs `[exception]` instead of aborting the process.

### DENIED BY USER

- The original runtime bug was not in Workflow parsing, Workflow compilation, or Workflow-to-C++ generation. The new sample compiled and regenerated successfully, but ownership could still silently flip between the client and service lifecycles after a transient tracker was released. That produced the wrong no-exception output in `RuntimeTest` and the same mismatch in generated-C++ paths.
- Persisting the first owning client on the object itself fixed the real problem. The live tracker is only valid while the current lifecycle is actively tracking the object, but the object still needs a stable ownership identity after that tracker is removed. `InternalProperty_LocalObjectOwner` provides that stable identity, so later attempts from the other lifecycle are rejected instead of creating a second registration.
- The first ownership fix changed the failure mode in the right direction but still failed `RuntimeTest` because `CHECK_ERROR` throws `vl::Error`, while the generated Workflow sample catches `const ::vl::Exception&`. Converting only the expected ownership-conflict branches to `vl::Exception` preserved the invariant checks for true internal corruption while making the semantic conflict catchable by the sample, which is exactly what the new test is asserting.
- Validation confirmed the proposal in both execution architectures and both target architectures. `RuntimeTest` passed with the expected logged exceptions, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` matched it, and the broader `Debug|x64` and `Debug|Win32` unit-test executable matrices also passed. The final fix therefore satisfies the new sample without changing the forbidden parser/compiler/code-generation surfaces and without leaving regressions in the surrounding test suite.
- The user approved the ownership-preservation direction but explicitly rejected the additional `RpcDualObjectOwner` embedded property type, asking for the persistent owner information to stay inside the local-object tracker and for `RpcDualObjectTracker` to be renamed to `RpcDualLocalObjectTracker`. This proposal is therefore denied by user preference even though the behavior was correct.

## No.2 Persist Ownership In RpcDualLocalObjectTracker

### CODE CHANGE

- Removed the separate `RpcDualObjectOwner` type and `InternalProperty_LocalObjectOwner` from `Test/Source/RpcDualLifecycleMock.h` and `Test/Source/RpcDualLifecycleMock.cpp`.
- Renamed `RpcDualObjectTracker` to `RpcDualLocalObjectTracker`.
- Extended `RpcDualLocalObjectTracker` so it keeps the original `RpcObjectReference` including the first owner `clientId`, and added `Attach`, `Detach`, `GetClientId`, and `IsTracked` helpers.
- Changed `TrackLocalObject` to reuse an existing detached `RpcDualLocalObjectTracker` when the same owning lifecycle touches the object again, and to reject registration immediately when the stored `clientId` belongs to another lifecycle.
- Changed `UntrackLocalObject` to detach the tracker instead of clearing the internal property, so ownership information remains on the object without introducing another embedded property type.
- Changed `PtrToRef` to use the tracker alone for both live-tracked and detached ownership decisions: return the existing ref when the tracker is active in the current lifecycle, or throw `vl::Exception` when the stored client id belongs to a different lifecycle.

### CONFIRMED

- This refactor preserves the same root fix while matching the user-requested design. The object still remembers which lifecycle first owned it, but that state now lives in `RpcDualLocalObjectTracker::ref.clientId` instead of a second internal property.
- Keeping the tracker object attached in a detached state is enough to prevent ownership flip-flops. When the live tracking ends, `Detach()` clears only the runtime backpointer to the lifecycle instance; it does not erase the stored `RpcObjectReference`, so later `PtrToRef` and `TrackLocalObject` calls can still detect whether the object belongs to the same lifecycle or a different one.
- The renamed tracker keeps the implementation surface cleaner than the previous version because there is only one local-object internal property to reason about. That satisfies the user's constraint that no more embedded property types should be added.
- Validation remained clean after the refactor. `Debug|x64` `RuntimeTest` still matched the `FailDoubleRegistration` exception sequence, and the full executable matrices for `Debug|x64` and `Debug|Win32` both passed for `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `LibraryTest`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`.
