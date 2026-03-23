# TODO

## Known Issues

- `let X = Y in (F(X.a.b.c))` reports `X does not exist in the current scope` in binding property value
  - while `F((let X = Y.a.b).c)` works
- Compiler complexity could be O(n2) which makes big single GacXML compilation slow.
  - or not complexity problem but just AST pooling?

## Workflow Interface Based RPC

- [TODO_RPC.md](./TODO_RPC.md)

## Improvements

### GacGen Awareness

- Dump binary type metadata including only new types created in Workflow. Need `VlppReflection` supporting.

### Attributes

- Attributes.
  - Add attributes to `VlppReflection` metadata.
    - Parameters must be enum of serializable primitive types.
    - Attribute name will be referenced by its `ITypeDescriptor`.
  - An attribute `X` becomes a `vl::reflection_metadata::XAttribute` struct instead of hardcoding in the compiler constructor.
    - Define and reflect all `@cpp:*` and `@rpc:*` attributes.
    - `@cpp:Attribute` must be applied to the struct to make it recognizable (needs VlppReflection metadata support).
    - In metadata only the short type name is kept as a string.
    - It becomes a struct, arguments initialize all fields in order, no default value.
    - Only allow limited primitive types as field types.
      - C++ registered attributes will also be checked when applied.
    - (optional) User defined attributes in Workflow.

### Generic

- Compile-time generic type annotation.
  - C++ Reflection macros for generic type annotation.
    - Apply to collections, tasks, etc.
    - Remove the current generic interface syntax invented for `IAsync`, and change how `await` determines return type.
  - Only applies to interfaces and its members.
    - Add base class `IInvokableMemberInfo` for `IEventInfo`, `IPropertyInfo` and `IMethodInfo`.
    - Move `GetCpp` to `IInvokableMemberInfo`.
    - Add `GetGenericHint` to `IInvokableMemberInfo`.
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

- Support `<category>name` identifier, make generated code re-parsable.
- Recognize collection interfaces instead of collection entities.
  - Reconsider how to implement collection creating code in Workflow.
  - Only if `Vlpp` decides to add collection interfaces.
- Virtual functions.
- Class implements interfaces.
