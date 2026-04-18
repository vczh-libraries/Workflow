# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.
- References:
  - [Definition](./TODO_RPC_Definition.md)
  - [Scenarios](./TODO_RPC_Scenarios.md)

## ToDo

```prompt
You are going to continue finishing `RuntimeTest` project to make `Rpc` category unit test work.

Task 1. Complete `IndexRpc.txt` to add expected execution result, just like how `IndexCodegen.txt` does.

Task 2. Add `Test\Source\RpcDualLifecycleMock.(h|cpp)`, inheriting from `RpcLifecycleMock.h`, add them to `RuntimeTest`, it does the following thing:
- The implementation is similar to `RpcByvalLifecycleMock.h`, but the mock is about to be doubly created.
- In any `Test\Resource\Rpc\*.txt` there will be `serviceMain`, `clientMain` functions.
- It generates `Test\Generated\RpcMetadata(32|64)\Wrapper_*.txt`, there will be `rpc_GetIds`, `rpc_IRpcObjectOps`, `rpc_IRpcObjectEventOps`, `rpcwrapper_<interface-name>` functions.
- In `TestRpc.cpp` the test case will initialize them:
  - Create `RpcDualLifecycleMock`, lc1 and lc2.
  - Run workflow assembly initialization function from the loaded assembly.
  - Create list ops default implementation, lo1(lc1), leo1(lc1), lo2(lc2), leo2(lc2)
  - Create object ops implementation, from `rpc_IRpcObjectOps` and `rpc_IRpcObjectEventOps`, oo1(lc1), oeo1(lc1), oo2(lc2), oeo2(lc2)
  - Register lo1, leo1, oo1, oeo1 to lc2.
  - Register lo2, leo2, oo2, oeo2, to lc1.
  - Notice the registration will use another one's ops implementations. It simulates two separates execution domain lc1 and lc2, and lc1 and lc2 talk to each other using those ops implementation. This the reason why ops needs to initialize with their own lc but be registered into another lc.
  - run `serviceMain` with lc1 to initialize lc1.
  - run `clientMain` with lc2 to get the result. The return value should match `IndexRpc.txt` just like how `Codegen` category does.
- `RuntimeTest` will need to run `rpc_GetIds` and give the map to `RpcDualLifecycleMock`, in `Register` function the map will be handed to each ops implementation's `IRpcSyncIds` base interface. Although many `IRpcSyncIds` implementations only have empty implementation but this step is needed. And the id information, containing ids for types/methods/events, will be useful for `RpcDualLifecycleMock` implementation too.
- The lc1 and lc2 will need different client id, you can say 1 and 2, and this number could be passed to their constructors.
- When each lc knows its own client id, when `RpcObjectReference` has a different type id, you will know the actual implementation will be in another lc, so when this lc is asking for an interface that from that lc, the type id will help you find the type, therefore call `rpcwrapper_<interface-name>`.
  - The current naming convention of its name, is the name after the last "::" of the full type name, or when there is no namespace before the type name, is the complete full interface name.
  - The wrapper will create an interface implementation, redirecting everything to its lc, and lc will pass information to another lc's ops implementation, accessing the actual interface implementation. It sounds like COM, yeah.

Firstly implement the code, git commit and push, for backup, DO NOT ASK ANY QUESTION.
Secondly run the unit test, and after everything passed, git commit and push, DO NOT ASK ANY QUESTION.
Even when any failure is not caused by your change you will have to fix it.

After everything is done, run `..\Tools\Tools\Build.ps1 Workflow` for a complete CI, fix any issue, git commit and push, DO NOT ASK ANY QUESTION
```

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
