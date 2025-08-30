# TODO

## Known Issues

- `let X = Y in (F(X.a.b.c))` reports `X does not exist in the current scope` in binding property value
  - while `F((let X = Y.a.b).c)` works
- Windows and Linux test output inconsistecy on
  - Token index different (but row and column are consistent) due to `\r\n` and `\n`.
  - We can force `\r\n` in unit test, normalizing all inputs.
- Compiler complexity could be O(n2) which makes big single GacXML compilation slow.
- Figure it out: https://github.com/vczh-libraries/Workflow/commit/01663cd6b75f018d7405151a66820743c2115898
  - Only crash in Release x64

## Progressing

## 2.0

- Generated C++ code try not to create IValueList if apply `{1 2 3}` on `List<int>` argument.
- Dump binary type metadata including only new types created in Workflow.
- In generated C++ code, when a collection instance is required from an interface:
  - Try to cast to the collection instance directly using the pointer inside the implementation of the interface.
  - Create the collection instance and copy all the data if the above step failed.

## Generic

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

## Async ViewModel Binding

- In `bind(...)` expression, add an annotation to convert `IAsync<T>` to `T`, could be `await` expression.
- If multiple `IAsync` don't depend on each other, they could be awaited parallelly.
- When a bind expression need to be re-evaluate, if there are still unfinished `IAsync<T>`, cancel and ignore all of them.

## ViewModel Remoting C++ Codegen

### Dedicated repo for RPC

- RPC compiler for remote protocols and object models
  - GacUI will call it to generate the remote protocol for GacUI Core so a file renaming might be necessary.
- RPC routers for communicating multiple protocol clients with one server on multiple protocols at the same time.
  - Channels will be moved here from GacUI.
  - There are some helpers defined in GacUI remote protocol but they requires generated C++ code to work. Consider offering a general solution, e.g., generate them together.
- RPC library defines a remote protocol for object models.
  - There will be a centralized object model servers.
  - Interface implementations could be offered in the server itself, or in different clients.
  - Clients only talk to the server directly, server will redirect requests if clients need to talk to each other.
  - When an interface is defined as a service, only one live instances will be allowed, clients could request its instance by its full name.
- RPC compiler for object models, generating C++ code for serialization, interfaces and implementation of interfaces that would interact with the remote protocol for object models.
  - remote protocols and object models will share the same "value type" definition.
  - all interface instances must be shared pointer.
- when full reflection is on, the serialization built on top of reflection will be offered, it can starts without any extra C++ code.
  - value type serialization.
  - interface implementation for invoker, the real implementaiton is in remote.
  - local interface implementation can be handled by a class that implements it.
  - it requires metadata of them are ready, cannot just boost from a object model definition.
- Runtime life cycle management.
  - When sending a shared pointer from one side to another:
    - Before a method is answered (responded with a return value), all passing objects are alive.
    - Necessary lifetime notification must be sent before answering a method including:
      - A client keeps an object alive.
      - A client doesn't keep an object alive.
- Support `IAsync` as function return values.
- Support collections in both lazy and serializable ways.
  - It is decided in function signatures.

### Improvements to Workflow

- Attributes.
  - Add attributes to `VlppReflection` metadata.
  - Make attributes `X` be `reflection_metadata::XAttribute` instead of hardcoding in the compiler constructor.
    - Define and reflect all `Cpp*` attributes.
    - `@Attribute` must be applied to the struct to make it recognizable (needs VlppReflection metadata support).
    - In metadata only the short type name is kept as a string.
    - It becomes a struct, arguments initialize all fields in order, no default value.
    - Only allow limited primitive types as field types.
      - C++ registered attributes will also be checked when applied.
    - (optional) User defined attributes in Workflow.
  - Some "RPC" atttributes.

### Details

- Prepare attributes for object model.
  - Wnen a struct is RPC serializable:
    - Workflow will generates it normally in assembly.
    - Workflow will not generate it in C++.
  - When an interface is RPC serializable:
    - Workflow will generates it normally in assembly.
    - Workflow will not generate it in C++.
  - Consider how to offer a class as a RPC service implementation in Workflow.
  - Workflow will generate an object model definition, the RPC compiler will take it and generate C++ code.

## Optional

- Support `<category>name` identifier, make generated code re-parsable.
- Recognize collection interfaces instead of collection entities.
  - Reconsider how to implement collection creating code in Workflow.
  - Only if `Vlpp` decides to add collection interfaces.
- Virtual functions.
- Class implements interfaces.
