# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

- Delete `IRpcSyncIds` and all `SyncIds` implementations. They end up empty so I decided to remove it.
- Delete `IRpcController::Register` from the interface, keep `Register` in each implementation.
- All collection wrappers should also implement `IRpcWrapperBase`.
- Below items only apply to RpcDualLifecycleMock and its sub classes (does not apply to RpcByvalLifecycleMock)
  - There is a registration of collection proxy dictionary `proxyFactories`, but it is only used once, so remove the proxy but do a switch-case instead.
  - In `RpcDualLifecycleMock::RefToPtr` the `proxyFactories` is used again, but it is in the local object branches. Any local object should just be the object itself instead of using wrappers. So I believe `proxyFactories` should not be used here.
  - Currently collection wrappers are managed differently with wrappers generated from Workflow, I would like you to merge them.
  - Wrappers in mock should be stored using `Ptr<IRpcWrapperBase>` or `IRpcWrapperBase*` accordingly, instead of `IDescriptable`.
  - All wrappers should implement `IRpcWrapperBase`, if any casting fails throw exceptions.
- In generated wrappers:
  - Change `rpcwrapper_XXXX_IService` return type to the generated interface that inherits from both IService and IRpcWrapperBase.
  - Change `rpcwrapper_Create` to also return `IRpcWrapperBase^`.
  - Change the wrapper factory to return `Ptr<IRpcWrapperBase>`.

All test projects must pass, even when there is any pre-existing failures fix them as well.

# UPDATES

# TEST

The test is the existing Rpc test cases in TestCasesRpc.cpp plus RuntimeTest, CppTest, CppTest_Metaonly, CppTest_Reflection. Success criteria:
- All test projects pass for both Win32 and x64
- No regressions

# PROPOSALS
