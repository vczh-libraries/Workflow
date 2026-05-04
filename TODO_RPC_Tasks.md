# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.
- If you can't fix a runtime issue within a few rounds of guess-and-edit, you need to debug the process.
- I am a fan of crash early. When something should happen, it should just happen, do not play a game like "what if it is not the case" and silently covers the issue. One example is that, if an object should not be null, then we should just use it, if a nullable object should not be null, we should just cast it. No test is performed in this case, using it will crash if it is null, and we know there is a problem. Fix the actual problem instead of doing "error tolerance".

## Task 1

Read `TODO_RPC_InByref_OutByval_Failure.md`, it describes an issue when a RPC function returns collections of interfaces with byval policy. `CppTest*` crashes because JSON serialization is turned on, `RuntimeTest` is good because it is not turned on. `RpcByvalKeepAliveProperty` handles the case when JSON serialization is turned on, but with JSON turned on, this object unfortunately released before the return value is received by the caller side. But for byval parameter it is fine, because `InvokeMethod` only returns when the remote function is fully executed.

So I would like to address this issue properly. The goal will be:
- Change the interface to resolve this issue at the root cause.
- Remove `RpcByvalKeepAliveProperty` and `RpcByvalKeepAlive` as the new construction should work universally.

We defined a new class in `WfLibraryRpc.h` and its equivalent Workflow code looks like this
```Workflow
class RpcByvalReturnValue
{
    var value : object;
    var slot : int;
}
```

These struct rules should be followed:
- Only when the return value is marked with `@rpc:Byval`:
  - The actual returned object will be `RpcByvalReturnValue`, its `value` stores the copied collection, `slot` will be an incremental index maintained inside the `InvokeMethod` implementation.
    - `rpcops_IRpcObjectOps` and `rpcops_IRpcObjectOpsJson` implements `InvokeMethod`.
    - The initial slot should be 0, each time a `RpcByvalReturnValue` is required it increments.
    - Both `_slot : int` and `_byvalReturnValues : object[int]` will be the interface implementation's field, just like `_lc`.
    - When a `RpcByvalReturnValue` is returned, the `value` will be added to the map with its slot as the key. Note that the value added to `_byvalReturnValues` is `RpcByvalReturnValue::value`.
  - At the generated wrapper side which calls `InvokeMethod` all the time, cast the return value right away to `RpcByvalReturnValue`, retrieve the value and the slot.
    - No testing is allowed. If it should be `RpcByvalReturnValue`, cast it to `RpcByvalReturnValue^` directly. Otherwise, just assume it is the actual value you need.
    - Firstly, deserialize the object properly (just as how it is done now), put it in a variable.
    - Secondly, `IRpcObjectOps::EndInvokeMethod` at the same object you call `InvokeMethod`, its signature will be `func EndInvokeMethod(slot : value) : void`.
    - When `rpcops_IRpcObjectOps` and `rpcops_IRpcObjectOpsJson` receives the `InvokeMethod` call, it removes the cached object from `_byvalReturnValues`.
- When the return value is not marked with `@rpc:Byval`, everything should not change.
- Generated code must precisely know if the object should be `RpcByvalReturnValue` or not:
  - No testing is allowed. If it should be `RpcByvalReturnValue`, cast it to `RpcByvalReturnValue^` directly. Otherwise, just assume it is the actual value you need.

When this is implemented, now the callee side will save the return value until it is alive at the caller side, and then caller side will tell callee side to release it.
By doing this, it solves the issue that, when a collection of interfaces is passed with the byval policy, the return value is released too early causing all interface elements to release.
When the caller side get the object and then call `EndInvokeMethod`, the return value (byval copied collection of interface) will be hold by the caller side because the caller side now grabs the wrapper.
In this way, `RpcByvalKeepAliveProperty` and `RpcByvalKeepAlive` will be no longer needed when JSON is not turned on, and the issue happens today when JSON is turned on should also gone.
