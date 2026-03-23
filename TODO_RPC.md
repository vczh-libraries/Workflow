# Workflow Interface Based RPC

## Preparation

- Move the type serialization part from remote protocol to here and become part of the workflow supporting library
  - Parser
  - JSON serialization
  - Binary serialization
  - Metadata saving
- Refactor GacUI and ensure GacJS bot affected
  - Might have namespace changing but we should allow to specify namespace for generated C++ data type

## Thoughts

- Serialization data types and interfaces for view model will generate serialization protocol data types
- Ensure generated C++ code works
- Ensure metadata of view model (need a workflow term not GacUI term) and protocol data types
  - Messages for interfaces should be easily inferred or hard coded
- Reflection enabled implementation when serialization view models are launched purely by interpretation
  - both caller side and callee side
  - ensured by unit test, it will be executed directly and generate C++ unit test just like normal workflow script testing

## Definitions

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
