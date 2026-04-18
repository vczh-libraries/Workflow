# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.
- References:
  - [Definition](./TODO_RPC_Definition.md)
  - [Scenarios](./TODO_RPC_Scenarios.md)

## ToDo

- `RumtimeTest` need to create two set of objects, call `serviceMain` followed by `clientMain`.
  - `DescriptableObject::SetInternalProperty` will be used to receive destructor call of local objects, to know if an object becomes unavailable.
  - Run `Rpc/RequestService.txt`
    - `serviceMain` accepts a lifecycle interface and run.
    - `clientMain` accepts a lifecycle interface and register a service.
    - They have client id 1 and 2.
    - lifecycle1 redirect ops to lifecycle2 callee ops, vice versa.
    - it means lifecycle2 callee ops are created with lifecycle2 reference and are registered into lifecycle1.
- Continue to add more test cases until all features are covered.
- C++ codegen for wrappers (should be normal codegen) with JSON serizliation in C++.
  - Generate C++ code from Rpc category with the new pattern.
  - This time new lifecycle implementations will be created tonise JSON serialization for values (protocol not included).
- Solve interface inheritance and casting.
- Delete Runtime test category.
