# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.
- If you can't fix a runtime issue within a few rounds of guess-and-edit, you need to debug the process.

## Task 1

I would like you to perform a refactoring, to remove these 3 functions from `Sys` reflection registration:
- `RpcGetSerializedArgument`
- `RpcSerializeEventArgument`
- `RpcTransferByvalKeepAlive`

### RpcGetSerializedArgument

There is no need to have that function. In the caller side, you just always assume `arguments` is not null and retrieve its element right away.

### RpcSerializeEventArgument

There is no need to have that function. The funcion only checks if the serializer exists. You should just check that in the generated call side,:
- Firstly box all values to `arguments` variable.
- Secondly, when `serialize` exists, serialize them inplace in a loop.

### RpcTransferByvalKeepAlive

I suspect calling this function is unnecessary, for example in `Wrapper_Collection_Default_Json.txt` the function looks like:
```Workflow
override func InvokeMethod_RpcCollection__Default__IService_DoList(ref : system::RpcObjectReference, arg_xs : ::system::Int32[]) : (::system::Int32[])
{
    var arguments : system::Array^ = {};
    arguments.Resize(1);
    var jsonValue0 : object = system::IRpcLifecycle::RpcBoxByval(arg_xs, _lc);
    var jsonNode1 : system::JsonNode^ = rpcjson_Serialize(jsonValue0);
    system::Sys::RpcTransferByvalKeepAlive(jsonValue0, jsonNode1);
    arguments.Set(0, jsonNode1);
    var jsonResult : system::JsonNode^ = (cast (system::JsonNode^) _lc.Dispatcher.SendToClient_ObjectOps(ref.clientId).InvokeMethod(ref, rpcmethod_RpcCollection__Default__IService_DoList, arguments));
    var jsonValue2 : object = rpcjson_Deserialize(jsonResult);
    return (cast (::system::Int32[]) system::IRpcLifecycle::RpcUnboxByval(jsonValue2, _lc));
}
```

`RpcBoxByval` already "kept alive" elements in `jsonValue0`.
`_lc.Dispatcher.SendToClient_ObjectOps` is a blocking function, it won't return before the remote side finishing the work.
So those elements still alive after calling `_lc.Dispatcher.SendToClient_ObjectOps` because `jsonValue0` still alive.
Then I believe even when you don't call `system::Sys::RpcTransferByvalKeepAlive` everything should still be fine.
You need to verify this.

## Task 2

You are going to create `TODO_RPC_GeneratedWrappers.md` to the following sections:
- Describe when to call generated functions and what do they do. Another section for generated functions about JSON serialization.
- Describe how do generated functions finish their work. Another section for generated functions about JSON serialization.

Assume the reader is one who want to use generated C++ version from Workflow, willing to adopt RPC. By the way, no need to cover the usage of attributes because the are already in `TODO_RPC_Definition.md`. You can reference the following materials:
- `TODO_RPC_Definitions.md`
- `TODO_RPC_Json.md`
- `TestCasesRpc.h`.
But do not mention anything specific to test projects.

## Task 3

In `Wrapper_*.txt` and `Wrapper_*_Json.txt`, there are a lot of "in function variable" which have types.
Variable types could be omitted if they are inside a function.
You are going to remove all of them to keep them clean, unless any specific change causes problem.
