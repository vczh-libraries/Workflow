investigate repro

- you have to follow `REPO-ROOT/.github/Guidelines/Coding.md` when coding.
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.
  - It is important to do task one by one strictly, by me designing tasks in this way, we can achieve:
  - Easy-to-understand commits for file changing that is easy to review.
  - Limit side effects so that you don't have to deal with massive of issues at the same time.

## Task 1

`RpcJsonListEventOpsMock::OnItemChanged` and `RpcJsonObjectEventOpsMock::InvokeEvent` should call `dispatcher->RecordJsonValue` on the returned value.
- In order to do this, `IRpcListEventOps::OnItemChanged` and `IRpcObjectEventOps::InvokeEvent` should return `Value` instead of `RpcEventExceptionMap`.
- Such return values should be serialized and deserialized with JSON.
- But since JSON mock version of them do not do JSON serialization and deserialization, it is JSON `IRpcSerializer` doing them.
- So they can't call `ReadMethodException` and `ReadEventException`, generated ops implementations and those in `WfLibraryRpcWrappers.(h|cpp)` should call them after deserializing return values.
  - Currenty they returns `RpcEventExceptionMap` so return values from `OnItemChanged` and `InvokeEvent` are not involved in `IRpcSerializer` serializing and deserializing, you should add it.
  - `rpcops_IOps_CreateJson` currently call `ReadEventException` after `rpcjson_Deserialize`, which is good.
  - `rpcops_IOps_CreateJson` currently call `ReadMethodException` before `rpcjson_Deserialize`, which is bad.
    - `rpcjson_Deserialize` should be called before `ReadMethodException` so that `ReadMethodException` does not need to handle JSON values.
    - Even when a method returns primitive types, `rpcjson_Deserialize` should be called and then do casting, instead of inlining for example reading `JsonString` for `string`, etc.
  - `rpcops_IOps_Create` looks good, no need to change
- Since `ReadMethodException` and `ReadEventException` is called after JSON representation of exceptions are deserialized to `RpcException` and `RpcEventExceptionMap`, so these two read functions don't handle JSON representations.

`JsonSerializePredefinedTypes` and `JsonDeserializePredefinedTypes` should handle serialization for `RpcException` and `RpcObjectReference`.
- Remove serialization of these two types in generated code.

Generated wrappers need to use generated strong typed version of ops interface for event handling.
- Take `Rpc/Inheritance_EventException` as an example.
- `rpcwrapper_RpcInheritanceEventException__IDerived` calls `rpclistener_RpcInheritanceEventException__IDerived` at the end.
- `rpclistener_RpcInheritanceEventException__IDerived` calls `InvokeEvent` by itself, which is wrong, `ops` should be passed to it, and it calls the strong typed version.
- This will affect test cases in `TestCasesRpc.cpp`.
  - Remove `attachLocalEvents` argument from `RunRpcTestCase`.
    - Since the only possible implementation is only calling `rpclistener_Attach`, `RunRpcTestCase` should just call it by themselves.
    - But when there is no event involved, `rpclistener_Attach` is not generated, so `bool HasEvent` should be added to `RunRpcTestCase` as a new template argument.
    - Therefore you can protect the code in `if constexpr (HasEvent)` so that when `rpclistener_Attach` is not generated there is no C++ compile error.
  - When `VCZH_DEBUG_NO_REFLECTION` is defined, use the JSON version.
  - You need to verify `RunRpcTestCase`, all JSON version functions should only be called when `VCZH_DEBUG_NO_REFLECTION` is defined, there are some other cases, e.g., `oo1`, `oeo1`, `oo2`, `oeo2`, `ops1`, `ops2`.
  - `lc1->attachLocalEventsCallback` will need `ops1` and `lc2->attachLocalEventsCallback` will need `ops2`, so these 6 objects are recommended to be created right after `lc1` and `lc2` are created.