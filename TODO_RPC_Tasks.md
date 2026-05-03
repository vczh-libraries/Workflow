# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

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

Remove `IRpcObjectOps::InvokeMethodAsync` as it is totally not used or impleme
