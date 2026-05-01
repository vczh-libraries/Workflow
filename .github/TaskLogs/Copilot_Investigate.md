# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Task 1

Due to the previous change which makes registered services no longer unregister, Dtor.txt and Dtor2.txt samples now is not working. You can check out IndexRpc.txt and find that in the long the service is no longer deleted. We need to update these samples. Because service won't release, we should change it to track another object:
- Add a `IValue` interface just like other test cases.
- Move the destructor from IService to IValue implementation.
- clientMain manipulate IValue's lifecycle instead of IValue
- In Dtor2.txt, `func GetServiceAgain() : IService^` will be changed to `func ReviewValue(value : IValue^) : IValue^`. clientMain passes the obtained value to this function, and this function returns itself. Therefore the original RequestService could be replaced by reading the argument, and all tests remain.

You should be able to revert IndexRpc.txt back to use `[Deleted]` for these two samples at the end. Check out comments in Dtor1.txt to understand what were expected to happen before the behacior changing to service lifecycle.

# UPDATES

# TEST [CONFIRMED]

Use the existing RPC destructor samples as the test signal:

- `Test/Resources/Rpc/Dtor.txt` should still return `[Not Deleted][Deleted]`, but the deleted object must be an `IValue` returned through the registered service instead of the registered service itself.
- `Test/Resources/Rpc/Dtor2.txt` should still return `[Not Deleted][Deleted]`, and should preserve the round-trip wrapper/local-object check by passing the obtained `IValue` into `ReviewValue(value : IValue^) : IValue^`.
- `Test/Resources/IndexRpc.txt` should expect `[Deleted]` again for both samples.

Success criteria:

- `Dtor.txt` and `Dtor2.txt` compile through `CompilerTest_LoadAndCompile`.
- RPC runtime output for `Dtor` and `Dtor2` matches `[Not Deleted][Deleted]`.
- The generated RPC C++ tests and full verification flow required by `Project.md` and `.github/Rules/verify-and-commit.md` pass.

# PROPOSALS

- No.1 Track destructors on returned IValue objects [CONFIRMED]

## No.1 Track destructors on returned IValue objects

### CODE CHANGE

Implemented the sample update and regenerated dependent outputs:

- Added `IValue` to `Test/Resources/Rpc/Dtor.txt` and `Test/Resources/Rpc/Dtor2.txt`.
- Moved the destructor state change from the registered `IService` object to newly returned `IValue` objects.
- Changed `Dtor.txt` so `clientMain` requests the service only to obtain an `IValue`, then releases the `IValue` wrapper before checking for `[Deleted]`.
- Changed `Dtor2.txt` by replacing `GetServiceAgain() : IService^` with `ReviewValue(value : IValue^) : IValue^`; the service verifies the passed value is local on the service side and returns it, preserving the wrapper/local-object round trip.
- Restored `Test/Resources/IndexRpc.txt` expectations for `Dtor` and `Dtor2` to `[Not Deleted][Deleted]`.
- Regenerated RPC metadata, Workflow assembly/parsing outputs, and generated C++ RPC source for both x86 and x64 through `CompilerTest_LoadAndCompile` and `Build.ps1 Workflow`.

### CONFIRMED

The proposal works because registered services are no longer the object whose destruction is being tested. `IService` remains registered, while each sample creates an unregistered `IValue` whose wrapper lifetime is controlled by `clientMain`; releasing the value wrapper releases the real value and runs its destructor.

Verification completed:

- Debug Win32/x64 build via `.github\Scripts\copilotBuild.ps1`.
- `LibraryTest` Debug Win32/x64.
- `CompilerTest_GenerateMetadata` Debug Win32/x64.
- `CompilerTest_LoadAndCompile` Debug x64, passing `699/699` test cases and regenerating RPC outputs.
- Post-generation Debug Win32/x64 rebuild.
- `RuntimeTest` Debug Win32/x64, passing `257/257` test cases.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` Debug Win32/x64, each passing `223/223` test cases.
- Full `C:\Code\VczhLibraries\Tools\Tools\Build.ps1 Workflow` release verification.
- Final generated C++ test log confirms `Rpc:Dtor` and `Rpc:Dtor2` both produce `[Not Deleted][Deleted]`.
