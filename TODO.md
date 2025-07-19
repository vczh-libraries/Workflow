# TODO

## Known Issues

- `let X = Y in (F(X.a.b.c))` reports `X does not exist in the current scope` in binding property value
  - while `F((let X = Y.a.b).c)` works
- Windows and Linux test output inconsistecy on
  - Token index different (but row and column are consistent) due to `\r\n` and `\n`.
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

- Attributes.
  - Add attributes to `VlppReflection` metadata.
  - Some "RPC" atttributes.
- Dealing with predefined interfaces:
  - `IAsync`, implemented into the protocol.
  - Collections interfaces, RPC could choose a return value of an argument to be
    - Lazy, methods implemented into the protocol.
    - Serializable, the whole collection is sent as data.
- Server and clients
  - The whole RPC is hosted by a server.
  - Multiple clients could connect to the server.
  - A client only talks to the server, a server talks to all clients.
  - When one client accesses an object that implements by another client, the server redirect requests between these clients.
- Implementation of interfaces:
  - Service interface:
    - There can be only one instance that implements such interface and it is implemented in the known client.
    - The instance of such interface could be passed around.
      - When an instance of such interface is processed in the RPC server, it will check if this is the only instance.
  - Object interface:
    - There is no limitation of implementation of such interface.
    - Object id and client id defines an instance.
- RPC details:
  - Requires all pointers are shared.
  - When sending a shared pointer from one side to another:
    - Before a method is answered, all passing objects are alive.
    - Necessary lifetime notification must be sent before answering a method including:
      - A client need to keep an object alive.
      - A client doesn't need an object to be alive.
- Boilerplate codes to be generated:
  - Implementation of interfaces, when calling it, it talks to the server or client.
  - Implementation of requests, when being called, it talks to local objects.
  - Connection processing is not included.
- Tool chain:
  - Metadata could be also serialized to JSON, but cannot be deserialized from JSON.
  - A stand alone tool that compiles interfaces with RPC attributes, read from metadata.
    - Source code will be integrated into GacGen.
    - It generates files about RPC:
      - Extra metadata for the RPC.
      - Server and client boilerplate code for Workflow.
      - Server and client boilerplate code for C++.
        - Reflectable.

## Optional

- Support `<category>name` identifier, make generated code re-parsable.
- Recognize collection interfaces instead of collection entities.
  - Reconsider how to implement collection creating code in Workflow.
  - Only if `Vlpp` decides to add collection interfaces.
- Virtual functions.
- Class implements interfaces.
