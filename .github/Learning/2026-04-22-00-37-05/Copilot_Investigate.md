# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

If `RpcDualLifeCycleAdapter` looks redundant then it is redundant. In `TestCasesRpc.cpp` an adaptor redirects everything to `LocalRpcMock`, and the private `mock` variable does not change.There should be a way to eliminate `RpcDualLifeCycleAdapter` completely.

I would like you to find a way to remove the adaptor completely. To do experiment, you can just change `TestCasesRpc.cpp` and run `CppTest` with Debug|x64 directly. Once you find a valid way, update the function generating `TestCasesRpc.cpp` and run all tests. And then change `TestRpc.cpp` to not use the adaptor in the same way. And then remove RpcDualLifeCycleAdapter completely.

Eventually you will need to run all test projects to make sure your change works. git commit and push after finishing.

# UPDATES

## UPDATE

I believe there is no reason to add `RpcLifecycleMock` to reflection, because IRpcLifecycle::GetController returns the controller interface. You can cast the object to IRpcLifecycle first and then give it to Workflow.

Contionue to follow #file:investigate.prompt.md  to finish the work

# TEST [CONFIRMED]

- Generated-side probe:
	- remove `RpcDualLifeCycleAdapter` usage from `Test/SourceCppGenRpc/TestCasesRpc.cpp`
	- run `CppTest` with `Debug|x64`
- Generator propagation:
	- update `Test/UnitTest/CompilerTest_LoadAndCompile/TestRpcCompile.cpp`
	- regenerate checked-in RPC harness files
	- rerun generated C++ test projects
- Reflected runtime validation:
	- update `Test/UnitTest/RuntimeTest/TestRpc.cpp` to use direct lifecycle objects
	- run `RuntimeTest` with `Debug|x64`
- Success criteria:
	- `RpcDualLifeCycleAdapter` is removed completely from `Test/Source/RpcDualLifecycleMock.h/.cpp`
	- generated RPC tests still pass after replacing adapter-based teardown with explicit tracked-wrapper disconnect before lifecycle disposal
	- reflected `RuntimeTest` no longer fails in `RPC binary assemblies` with `Argument "thisObject" cannot convert from "system::IRpcController*" to "system::IRpcLifeCycle*".`
	- required debug project matrix passes for `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` on `Win32` and `x64`
- Validation results:
	- `CppTest` `Debug|x64` passed with `Passed test files: 2/2` and `Passed test cases: 109/109`
	- `RuntimeTest` `Debug|x64` passed with `Passed test files: 4/4` and `Passed test cases: 143/143`
	- the full required debug test matrix completed successfully through repo wrappers
	- `Tools/Tools/Build.ps1 Workflow` was also attempted as a stricter repo-wide harness, but release executable launch was blocked by local Application Control policy after `CompilerTest_GenerateMetadata`, so final validation used the repo debug wrapper sequence instead

# PROPOSALS

- No.1 Remove `RpcDualLifeCycleAdapter` completely by using `RpcDualLifecycleMock` directly and disconnecting wrappers explicitly [CONFIRMED]
- No.2 Remove the extra `RpcLifecycleMock` reflection registration and rely on `IRpcLifeCycle*` boxing only [DENIED]

## No.1 Remove `RpcDualLifeCycleAdapter` completely by using `RpcDualLifecycleMock` directly and disconnecting wrappers explicitly

### CODE CHANGE

- `Test/SourceCppGenRpc/TestCasesRpc.cpp`, `Test/UnitTest/CompilerTest_LoadAndCompile/TestRpcCompile.cpp`, and checked-in generated RPC harnesses in `Test/Generated/CppRpc32/TestCasesRpc.cpp` and `Test/Generated/CppRpc64/TestCasesRpc.cpp` now pass `lc1.Obj()` / `lc2.Obj()` directly to list ops, object ops, `serviceMain`, `clientMain`, and `rpcwrapper_Create(...)`.
- The generated/local mock adds `DisconnectTrackedWrappersBeforeDispose()` and calls it explicitly before disposal to preserve the teardown timing that used to be hidden by the adapter destructor.
- `Test/UnitTest/RuntimeTest/TestRpc.cpp` now uses direct lifecycle objects in the reflected RPC path and keeps the wrapper factory argument boxed as `BoxValue<IRpcLifeCycle*>(wrapperFactoryLifecycle)`.
- `Test/Source/RpcDualLifecycleMock.h/.cpp` removes `RpcDualLifeCycleAdapter` completely.
- `Test/Source/RpcLifecycleMock.h`, `Test/Source/CppTypes.h`, and `Test/Source/CppTypes.cpp` register `RpcLifecycleMock` as a reflected concrete class with `IRpcLifeCycle` and `IRpcController` bases.
- Reflection metadata and baselines were regenerated for this new reflected type in `Test/Generated/Reflection32.bin`, `Test/Generated/Reflection64.bin`, `Test/Generated/Reflection32.txt`, `Test/Generated/Reflection64.txt`, `Test/Generated/Reflection32[2].txt`, `Test/Generated/Reflection64[2].txt`, `Test/Resources/Baseline/Reflection32.txt`, and `Test/Resources/Baseline/Reflection64.txt`.

### [CONFIRMED]

- The adapter was redundant as a call-forwarder, but its destructor timing had been hiding a real cleanup-order dependency on the generated side.
- Generated C++ RPC tests work without the adapter once tracked wrappers are disconnected explicitly before lifecycle/global-context teardown.
- The reflected Workflow path still needs the lifecycle object to cross the reflection boundary as something convertible to `system::IRpcLifeCycle*`. Making `RpcLifecycleMock` a reflected concrete class satisfies that requirement without reintroducing the adapter.
- With both pieces in place, `RpcDualLifeCycleAdapter` is removed completely and both generated and reflected RPC tests pass.

## No.2 Remove the extra `RpcLifecycleMock` reflection registration and rely on `IRpcLifeCycle*` boxing only

### CODE CHANGE

- Temporarily removed `reflection::Description<RpcLifecycleMock>` and the `RpcLifecycleMock` registration from `Test/Source/CppTypes.h/.cpp`.
- Kept the direct lifecycle path in `Test/UnitTest/RuntimeTest/TestRpc.cpp` and continued boxing the wrapper factory argument as `BoxValue<IRpcLifeCycle*>(wrapperFactoryLifecycle)`.
- Reordered `RpcLifecycleMock` base classes to test whether the interface cast alone would make Workflow see `system::IRpcLifeCycle*`.

### [DENIED]

- This experiment reproduced the original reflected failure: `Argument "thisObject" cannot convert from "system::IRpcController*" to "system::IRpcLifeCycle*".`
- `BoxValue<T*>` for raw pointers ignores the requested interface descriptor and calls `Value::From(object)`, so casting to `IRpcLifeCycle*` before boxing does not change the runtime descriptor Workflow sees.
- `IRpcLifeCycle::GetController()` cannot help here because the failure happens before Workflow can invoke any member on the receiver; the boxed receiver itself must already be convertible to `system::IRpcLifeCycle*`.
- The concrete reflection registration was restored immediately after this denial, and `RuntimeTest` `Debug|x64` returned to passing.
