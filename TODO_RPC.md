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
- C++ codegen for wrappers (should be normal codegen) with JSON serizliation in C++.
  - Generate C++ code from Rpc category with the new pattern.
  - This time new lifecycle implementations will be created tonise JSON serialization for values (protocol not included).
- Solve interface inheritance and casting.
- Delete Runtime test category.
