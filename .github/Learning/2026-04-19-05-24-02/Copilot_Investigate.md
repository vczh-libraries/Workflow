# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Follow job.new-sample.md to add a new Rpc\LocalAndWrapper.txt sample.

The sample tests that when interface objects cross RPC boundaries:
- Remote objects are wrapped (serviceReceived_Obj1 != clientObj1, clientReceived_Obj2 != serviceObj2)
- Local objects are recognized and unwrapped (serviceReceived_Obj2 == serviceObj2, clientReceived_Obj1 == clientObj1)

The sample defines three @rpc:Interface interfaces (IObj1, IObj2, IService with @rpc:Ctor) in the RpcWrapperTest namespace. IService has Exchange1 (takes IObj1^, returns IObj2^) and Exchange2 (takes IObj2^, returns IObj1^).

Expected result: `[false][true][true][false]`

Fixes are allowed to:
- IRpcLifeCycle implementation and connected interfaces
- Runtime library (RpcBoxByval, RpcUnboxByval, etc.)
- Test mock code (RpcDualLifecycleMock, etc.)

NOT allowed to change:
- Workflow parser
- Workflow compiler
- Workflow to C++ code generation

# UPDATES

# TEST

The test is the sample itself. Success criteria:
- Sample compiles in CompilerTest_LoadAndCompile
- RuntimeTest passes with expected result `[false][true][true][false]`
- CppTest, CppTest_Metaonly, CppTest_Reflection all pass

# PROPOSALS
