# TODO

## Known Issues

- `let X = Y in (F(X.a.b.c))` reports `X does not exist in the current scope` in binding property value
  - while `F((let X = Y.a.b).c)` works
- Compiler complexity could be O(n2) which makes big single GacXML compilation slow.
  - or not complexity problem but just AST pooling?

## Workflow Interface Based RPC

- [TODO_RPC.md](./TODO_RPC.md)

## Improvements

### RPC

- Verify `WfErrors::Cpp*`, probably not needed as errors should have been catched in eariler phases.
- `WfCpp_WriteReflection.cpp` is changed because `INVOKE(GET?)_INTERFACE_PROXY` does not handle overloading property. This should be a VlppParser bug. Fix the bug and revert this file.
  - https://github.com/vczh-libraries/Workflow/commit/7b0e54a1964774cbb011ea1e675aa8c72a3fbec4
- Can `WfInterfaceInstance` not inherit from `IDescriptable`?
- Check error handling. Ensure crashing instead of covering.
- A dedicated guidelines for codegen tools, including input output format and .d.ts

### GacGen Awareness

- Dump binary type metadata including only new types created in Workflow. Need `VlppReflection` supporting.

### Generic

- Compile-time generic type annotation, no runtime type information.
  - C++ Reflection macros for generic type annotation.
    - Apply to collections, tasks, etc.
    - Remove the current generic interface syntax invented for `IAsync`, and change how `await` determines return type.
  - Only applies to interfaces and its members.
    - Add base class `IInvokableMemberInfo` for `IEventInfo`, `IPropertyInfo` and `IMethodInfo`.
    - Move `GetCpp` to `IInvokableMemberInfo`.
    - Add `GetGenericHint` to `IInvokableMemberInfo`.
    - Fix `IValue*` collection reflections, how to apply `KeyType<T>`?
  - When calling a function:
    - Perform overloading checking with non-generic version.
    - Check argument types with generic version if exists.
      - Report error if failed.
      - Use the generic version of return type if succeeded.
      - If a generic method is called using `F<>(...)` annotation, then ignore the checking, use the non-generic version of return type.
      - Generic parameter is not allowed on members, all parameter must be deduced from arguments.
  - When using an interface type:
    - Use the non-generic version if generic parameter is not specified.
    - Use the generic version of generic parameter is specified.

### Async ViewModel Binding

- In `bind(...)` expression, add an annotation to convert `IAsync<T>` to `T`, could be `await` expression.
- If multiple `IAsync` don't depend on each other, they could be awaited parallelly.
- When a bind expression need to be re-evaluate, if there are still unfinished `IAsync<T>`, cancel and ignore all of them.

## C++ Code Generation

- Generated C++ code try not to create IValueList if apply `{1 2 3}` on `List<int>` argument.
- In generated C++ code, when a collection instance is required from an interface:
  - Try to cast to the collection instance directly using the pointer inside the implementation of the interface.
  - Create the collection instance and copy all the data if the above step failed.

## Optional

- Object constructor with property assignments:
  - `new Object { IntProp: 3; ReadonlyList: {1 2 3}; }`
  - Optimize Workflow RPC JSON Serialization code.
- Support `<category>name` identifier, make generated code re-parsable.
- Recognize collection interfaces instead of collection entities.
  - Reconsider how to implement collection creating code in Workflow.
  - Only if `Vlpp` decides to add collection interfaces.
- Virtual functions.
- Class implements interfaces.
