# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Follow job.new-sample.md to add a new Rpc\ServiceWrapper.txt sample.

The sample tests that when an RPC wrapper object is sent back across the boundary to the service side, it gets unwrapped to the real object:
- `clientObj == serviceObj` → false (clientObj is a wrapper living in a different lifecycle space)
- `obj.Self(clientObj)` → true (when clientObj crosses back to service side, it should be unwrapped to the real serviceObj)

The sample defines one @rpc:Interface @rpc:Ctor interface (IService) in the RpcServiceWrapperTest namespace. IService has a `Self(obj : IService^): bool` method that checks if the passed object equals the stored serviceObj.

Expected result: `[false][true]`

Additionally:
- Update TestCasesRpc.cpp generation to use a template function `RunRpcTestCase<T>` instead of duplicating boilerplate.
- Verify what actually differs between test cases (user claims only the `Instance()` line differs).

Fixes are allowed to:
- IRpcLifeCycle implementation and connected interfaces
- Test mock code (RpcDualLifecycleMock, etc.)
- TestCasesRpc.cpp generation code

NOT allowed to change:
- Sample content (unless it doesn't build — namespace was changed from `RpcWrapperTest` to `RpcServiceWrapperTest` to avoid collision with LocalAndWrapper.txt)
- Workflow parser
- Workflow compiler
- Workflow to C++ code generation (except TestCasesRpc.cpp generation)

# UPDATES

# TEST

The test is the sample `Rpc\ServiceWrapper.txt` itself. Success criteria:
- Sample compiles in CompilerTest_LoadAndCompile
- RuntimeTest passes with expected result `[false][true]`
- CppTest, CppTest_Metaonly, CppTest_Reflection all pass
- TestCasesRpc.cpp generation uses a template function `RunRpcTestCase<T>` instead of duplicated boilerplate

# PROPOSALS
