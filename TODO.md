# TODO

## Known Issues

- `let X = Y in (F(X.a.b.c))` reports `X does not exist in the current scope` in binding property value
  - while `F((let X = Y.a.b).c)` works
- Windows and Linux test output inconsistecy on
  - Token index different (but row and column are consistent) due to `\r\n` and `\n`.
- Compiler complexity could be O(n2) which makes big single GacXML compilation slow.

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

- In `bind(...)` expression, add an annotation to convert `IAsync<T>` to `T`.
- If multiple `IAsync` don't depend on each other, they could be awaited parallelly.
- When a bind expression need to be re-evaluate, if there are still unfinished `IAsync<T>`, cancel and ignore all of them.

## ViewModel Remoting C++ Codegen

- Attributes.
- Deal with methods returning `IAsync`.
- Deal with auto properties returning `IAsync`.
  - Think about how auto properties generate getters and setters with proper attributes.
  - If a getter returns `IAsync<T>`, the setter could still accept `T` and returns `IAsync`, for async auto properties.
    - Need a new syntax for async auto properties, could apply to methods as well.
  - When a new value of an auto property or an async auto property get pushed from the remote side:
    - Using different attributes to control caching logic (or avoid caching).
    - Such attributes could also apply to methods without parameter.
- Runtime:
  - Requires all pointers are shared.
  - Attribute for interfaces that have default remote constructors.
  - The native side need to provide functions for controlling lifetime:
    - AllocateObjectId():int, this is called when the object is sent to the native side for the first time. The default reference counter is 0.
    - IncreaseReference(int):void
    - DecreaseReference(int):void
    - When reference counter is not 0, the object must be kept alive and querable by id.
  - Default C++ code generated implementation on a set of interfaces.

## Optional

- Support `<category>name` identifier, make generated code re-parsable.
- Recognize collection interfaces instead of collection entities.
  - Reconsider how to implement collection creating code in Workflow.
  - Only if `Vlpp` decides to add collection interfaces.
- Virtual functions.
- Class implements interfaces.
