# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.
- References:
  - [Definition](./TODO_RPC_Definition.md)
  - [Scenarios](./TODO_RPC_Scenarios.md)

## ToDo

- Verify `RpcDualLifecycleMock` and `TestCasesRpc.cpp`.
  - Ensure rpcwrapper_typename encodes namespace.
  - Generate wrapper picker function from type id, remove TestCasesRpc.cpp and TestRpc.cpp constructions.
- Continue to add more test cases until all features are covered.
  - Complete `.github\jobs\NewSample.prompt.md` guiding AI to add new Workflow test case and do necessary bug fixings.
- C++ codegen for rpc data type JSON serialization.
- Solve interface inheritance and casting.
- Delete Runtime test category.
