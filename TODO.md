# TODO

## Known Issues

- `let X = Y in (F(X.a.b.c))` reports `X does not exist in the current scope` in binding property value
  - while `F((let X = Y.a.b).c)` works
- Compiler complexity could be O(n2) which makes big single GacXML compilation slow.
- Figure it out: https://github.com/vczh-libraries/Workflow/commit/01663cd6b75f018d7405151a66820743c2115898
  - Only crash in Release x64
  - `ResetDebuggerForCurrentThread` is created as a workaround

## Workflow Interface Based RPC

- Allowed Types:
  - Interfaces (`@rpc:*`)
  - Enums
  - Serializable Primitive Types with string (by `VlppReflection`)
  - Structs, all members must be serializable primitive types or qualified structs
  - Predefined Container Types
- Attributes:
  - `@rpc:Interface` on interface, all members should be serializable.
    - `@rpc:ctor` on interface, all members should be serializable, and an constructor will be offered by a client.
    - They are required and exclusive on any serializable interface.
  - `@rpc:byval` or `@rpc:byref` on property, on method (for return value), on parameter, representing that such container is:
    - **by value**: the whole collection is sent to a client, and it doesn't keep track on changes from the other side.
      - It only works when the element types are not interfaces.
      - If its element types are collection type, they are passed **by value** as well.
    - **by reference**: the collection is treated as remote object.
      - If its element types are collection type, they are passed **by reference** as well.
    - They are exclusive. The default option is `@rpc:byref`.
  - `@rpc:cached` or `@rpc:dynamic` on property, for any cached property:
    - The property value will not change.
    - The cache will refresh if the associated changed event occurs.
    - If there is no associated changed event, the cache will never refresh.
    - Values of cached properties will be sent along with the remote object to any client.
    - They are required and exclusive on any qualified property. When it is on an inteface, it becomes the default option on any qualified property.
- Dedicated interfaces for delegate actual sending/receiving implementation.
  - Interface for executing tasks in a dedicated thread. When an interface is running as GacUI view model, the interface will be implemented by GacUI so that everything runs in main thread.
  - Registration and a central server implementation, will signal all clients when all interface constructors are ready.
  - The client could run in the same thread with the server.
  - Generic is introduced to decouple the actual serialization result.
    - Default JSON implementation.
    - Implementation for invoker interface (acts like reflection) and protocol sending/receiving interfaces will be offered by client.
    - A set of messages are defined, but they do not depends on actual RPC interfaces. Types (int for id), methods (int for id) and arguments are all dynamic.
- Life cycle management
  - All arguments will be alive until the method being invoked is responded.
  - Any client could require an object to be alive and release later.
- C++ code generation:
  - Interfaces are forced to use `Ptr<T>`.
  - Type serialization with JSON.
  - Invoker interface implementation to bridge the actual remote implementation, any remote objects will create such implementation so that local code can call.
- All C++ code generated thing will have a runtime implementation therefore RPC can run immediately with a Workflow compiler.
  - Type implementation.
  - Invoker interface implementation.

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
