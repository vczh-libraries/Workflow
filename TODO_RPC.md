# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.

## Steps

- Workflow types with attribute should also exist in runtime (VM constructed ITypeDescriptor instances)
- Generate rpc metadata in strong typed and JSON, for types defined in current assembly only.
  - All signed integers will share the same type in metadata, also for unsigned integers.
  - C++ codegen still respect the original integer types.
- Add communication layer with unit test implementation.
- Create dynamic serialization implementation based on reflection for all touched rpc types.
- Add C++ codegen.

## Thoughts

- Serialization data types and interfaces for view model will generate serialization protocol data types
- Ensure generated C++ code works
- Ensure metadata of view model (need a workflow term not GacUI term) and protocol data types
  - Messages for interfaces should be easily inferred or hard coded
- Reflection enabled implementation when serialization view models are launched purely by interpretation
  - both caller side and callee side
  - ensured by unit test, it will be executed directly and generate C++ unit test just like normal workflow script testing

## Cross Resource Compiling

- Some rpc types might not have source code.
- VlppReflection supports to add attributes in metadata is required.
- Workflow compiler will create rpc metadata along with script, as long as C++ codegen, but initiating is a separate and optional process.

## Allowed Types

- Interfaces (`@rpc:*`)
- Enums
- Serializable Primitive Types with string (by `VlppReflection`)
- Structs, all members must be serializable primitive types or qualified structs
- Predefined Container Types
- `IAsync` or it derived interfaces are only allowed in method return types.
- Non `IAsync` including `void` return types means this method blocks.

## Attributes

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

## Implementation

- If an interface is implemented remotely, a typed proxy implementing such interface will be available to delegate everything to an general proxy interface, which does not have knowledge about the schema of the interface being implemented remotely.
- The general proxy interface is already generic and all data are serialized, object life-cycle is handled here, it talks to a thread scheduler and a centralized dispatcher.
- Thread scheduler and dispatcher could be implemented differently in different situation:
  - Unit Test: everything is straight forward.
  - GacUI with Remote Protocol: the remote protocol will handle the actual data transferring.
    - A JSON data type might need to add to the remote protocol.
    - Currently GacUI could require remote view model to use JSON.
- A set of messages are defined, but they do not depends on actual RPC interfaces. Types (int for id), methods (int for id) and arguments are all dynamic.
- Life cycle management
  - All arguments will be alive until the method being invoked is responded.
  - Any client could require an object to be alive and release later.

## Dispatching

- Registration and a central server implementation, will signal all clients when all interface constructors are ready.
- The client could run in the same thread with the server.

## C++/Workflow Code Generation

- C++ code generation:
  - Interfaces are forced to use `Ptr<T>`.
  - Type serialization with JSON.
  - Invoker interface implementation to bridge the actual remote implementation, any remote objects will create such implementation so that local code can call.
- All C++ code generated thing will have a runtime implementation therefore RPC can run immediately with a Workflow compiler.
  - Type implementation.
  - Invoker interface implementation.
- Only remote interface proxy implementation is included in this section.

## Casting to Child Interface

- A remote object is known to implement exactly one leaf interface.
- If an object is expected to implement multiple interface, an interface inheriting from all these interfaces are required to exist.
- When running with interpretor, casing with reflection should work.
- When running with generated C++ code, `dynamic_cast` should work.
- Although interface implementing is dynamic, a process is expected to only know a fixed set of interfaces that could be implemented remotely.
