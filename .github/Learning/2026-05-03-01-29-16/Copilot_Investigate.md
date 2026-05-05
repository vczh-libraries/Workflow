# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

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

# UPDATES

# TEST

Task 1 can be verified by existing RPC generation and runtime tests. The change must update reflection metadata, generated Workflow wrapper script, and generated C++ RPC samples so that only interface shared pointers call `RpcBoxByref`, `RpcUnboxByref`, `RpcBoxByval`, or `RpcUnboxByval`; primitive, nullable, and other object values should cast directly between their actual type and `object`.

Success criteria for Task 1:
- Debug Win32 and x64 builds pass.
- Existing RPC samples compile through `CompilerTest_LoadAndCompile`.
- Generated reflection baselines expose `RpcBoxByval` as taking `system::Interface^` and `RpcUnboxByval` as returning `system::Interface^`.
- Generated RPC C++ no longer calls RPC lifecycle boxing helpers for primitive and nullable arguments, while interface shared pointer values still round-trip through `RpcObjectReference`.
- Required unit test projects pass in the order from `Project.md`.

Task 2 can be verified by metadata and generated wrapper output. The change must remove `IRpcObjectOps::InvokeMethodAsync` from the C++ interface, reflection registration/proxy, generated Workflow `IRpcObjectOps` implementations, and generated C++ RPC samples.

Success criteria for Task 2:
- Debug Win32 and x64 builds pass after Task 2.
- Existing RPC samples compile through `CompilerTest_LoadAndCompile`.
- Generated reflection baselines and generated RPC C++ no longer contain `InvokeMethodAsync`.
- Required unit test projects pass in the order from `Project.md`.

# PROPOSALS

- No.1 Refine RPC boxing helpers to interface-shaped conversions

## No.1 Refine RPC boxing helpers to interface-shaped conversions

Change the reflected helper signatures so byref boxing exposes `RpcObjectReference`, byref unboxing returns `Interface^`, byval boxing accepts `Interface^`, and byval unboxing returns `Interface^`. Keep private dynamic `Value` helpers for list and dictionary wrappers. Update generated wrapper Workflow AST so only shared interface values call RPC lifecycle helpers; primitive, nullable, and other non-interface values cast directly to or from `object`.

### CODE CHANGE

- Changed public RPC lifecycle helper signatures in `WfLibraryRpc.h` / reflection registration:
  - `RpcBoxByref(Interface^, IRpcLifecycle*) -> RpcObjectReference`
  - `RpcUnboxByref(RpcObjectReference, IRpcLifecycle*) -> Interface^`
  - `RpcBoxByval(Interface^, IRpcLifecycle*) -> object`
  - `RpcUnboxByval(object, IRpcLifecycle*) -> Interface^`
- Removed the public `BoxRpcObject` path and made public byref helpers delegate directly to `IRpcLifecycle::PtrToRef` / `RefToPtr`, while preserving null handling.
- Kept internal `Value` based helpers for recursive collection boxing/unboxing, so list/dictionary wrappers can still transform nested interface values.
- Updated RPC wrapper generation to record parameter/return `ITypeInfo`, call RPC helper functions only for shared interface values, and cast non-interface values directly to/from `object`.
- Used null-preserving weak interface casts after `RpcUnboxByref` / `RpcUnboxByval`, because `cdb` showed `CppTest Debug Win32` failed in `Rpc:Nullable` when generated code emitted `Ensure(SharedPtrCast<IValue>(RpcUnboxByval(...)))` for a null interface argument.
- Updated C++ helper and reflection value accessors to try `dynamic_cast` before aggregation casts for interface pointers, matching the new direct boxing shape.
- Regenerated RPC metadata, generated C++ samples, generated Workflow assembly text, and reflection baselines.

### CONFIRMED

- Debugged the failing `CppTest Debug Win32 /F:TestCasesRpc.cpp` process directly with x86 `cdb`; the first-chance C++ exception stack identified `WfLibraryCppHelper.h:58` through generated `Nullable.cpp:838`.
- Focused `CppTest Debug Win32` passed after the generator fix, including `Rpc:Nullable`.
- Full Task 1 verification passed from `Test\UnitTest`:
  - Build Debug Win32 and x64
  - LibraryTest Debug Win32 and x64
  - CompilerTest_GenerateMetadata Debug Win32 and x64
  - CompilerTest_LoadAndCompile Debug x64
  - Rebuild Debug Win32 and x64 after generated C++ refresh
  - RuntimeTest Debug Win32 and x64
  - CppTest Debug Win32 and x64
  - CppTest_Metaonly Debug Win32 and x64
  - CppTest_Reflection Debug Win32 and x64
- Verification logs: `C:\Users\vczh\AppData\Local\Temp\Workflow_Task1_verify_20260503_010537`.
