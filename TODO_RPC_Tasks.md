# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.
- If you can't fix a runtime issue within a few rounds of guess-and-edit, you need to debug the process.

## Task 1

- `BoxRpcObject` looks useless, if a pointer could be `dynamic_cast`-ed before calling `BoxValue`, then it should just call `BoxValue`.
- `Rpc(B|Unb)oxByref` looks uesless too. When the input is a pointer, it should just call `IRpcLifecycle::(PtrToRef|RefToPtr)`.
- Change `RpcBoxByval`'s first argument to `Ptr<IDescriptable>`.
- Change `RpcUnboxByval`'s return value to `Ptr<IDescriptable>`.

`Rpc(B|Unb)oxBy(ref|val)` only perform changing for shared pointers of interfaces, in other cases (null and primitive type) they just return the argument.
So it is better to declare the function in this way. But they should still handle `nullptr`.
This will affect how wrapper Workflow script is generated. Today every arguments call `Rpc(B|Unb)oxBy(ref|val)` before passing values to ops interfaces.
Now they only accept interfaces, so other types (including primitive types and nullable types) will skip the call, and cast themselves between the actual type and `object`.

It is good that, after doing this refactoring, we will be able to show in the type system, should an interface be converted to `RpcObjectReference` or not. Although the type information is not used so far, but they still exist before being added to the `arguments: IValueArray^` array.

## Task 2

Remove `IRpcObjectOps::InvokeMethodAsync` as it is totally not used or implemented.

## Task 3

Add a second argument to `GenerateModuleRpc` called `WString assemblyName`.
In `TestRpcCompile.cpp` you can pass the result of `MakeRpcCppAssemblyName` to it.
And in `GenerateModuleRpc` function, you are going to generate an interface called `rpcops_IOps_${assemblyName}`, with two functions `rpcops_IOps_Create` and `rpcops_IOps_CreateJson`.

The `rpcops_IOps_*` has all stronge typed `InvokeMethod` and `InvokeEvent`. They convert the `arguments : system:L:Array^` argument into multiple strong typed arguments. Those types come from wrappers calling `InvokeMethod` and `InvokeEvent`, those wrapper constructor functions will take an `rpcops_IOps_*^`, instead of adding all arguments to an array, it calls all `InvokeMethod`.

Pay attention when generating these `InvokeMethod`'s in order not to be trapped into overloading trouble, those name should be consistant with generated `rpcmethod_*` and `rpcevent_*` ids, that is, method names become `InvokeMethod_*` and `InvokeEvent_*`. `InvokeMethod`'s return value will be strong typed too, or `void`.

`rpcops_IOps_Create` returns an `rpcops_IOps_*^`, with all `InvokeMethod` and `InvokeEvent` implementation, doing the adding all arguments to `arguments` array thing, and then call `_lc.Dispatcher.SentToClient_Object(Event?)Ops(...)....`. Return value of `InvokeMethod_*` will also be strong typed, therefore you can just do a cast from `IRpcObjectOps::InvokeMethod`, as that type will be exactly what `IRpcObjectOps::InvokeMethod` returns.

`rpcops_IOps_CreateJson` does one extra thing, before adding arguments to `arguments`, it do JSON serialization by calling generated helpers. Generated helpers are just for some type, when the argument is a predefined primitive type, the `system::JsonNode^` creating code are inlined. Return value of `InvokeMethod_*` will need to be JSON deserialized. You can assume that the return value from `IRpcObjectOps::InvokeMethod` will be a `system::JsonNode^`, so a cast is enough.

Since all wrappers now expect an `rpcops_IOps_*^` object for redirection. `TestCasesRpc.cpp` should create a shared implementation using `rpcops_IOps_Create` in each test case, passing to all wrapper constructors.

`rpcops_IOps_CreateJson` is not used, it is expected, but the code will still be validated by Workflow compiler.

Here is an example, when you have
```Workflow
namespace RpcPrimitiveTest
{
    interface IService
    {
        func ProcessInt(value : int) : void;
    }
}
```

It generates
```Workflow
interface rpcops_IOps_<whatever>
{
    func InvokeMethod_RpcPrimitiveTest__IService_ProcessInt(..., arg_value : int) : void
}
```

## Task 4

Just like how JSON serialization is processed in `## Task 3`:
- Rename `rpc_IRpcObjectOps` to `rpcops_IRpcObjectOps`.
- Rename `rpc_IRpcObjectEventOps` to `rpcops_IRpcObjectEventOps`.
- Generate `rpcops_IRpcObjectOpsJson` and `rpcops_IRpcObjectEventOpsJson`, do JSON serialization and deserialization, as all types are known.
  - Since JSON serialization boxes all arguments before passing to ops interfaces, the revert way would be receiving values from ops interfaces, do JSON deserilization, and then unboxes.
