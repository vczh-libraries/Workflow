# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.

## Steps

- Add `IWorkflowRpcController` to the rpc library (new pair of file).
  - Callee side list ops have default implementation.
  - `Byval` and `Byref` helper functions for containers.
- Implementation in test library that connects a caller and a callee controller, all in one client.
  - Enable serialization pipeline injection.
- Workflow compiler generates wrappers to call the controller.
  - Only supports `@rpc:Byval` and `@rpc:Cached`, also no `@rpc:Ctor`, no containers of interfaces, CHECK_FAIL if these happen.
  - JSON serialization based on reflection.
  - Lifecycle can be managed by wrapper destructor, acquire/release will be only called once per wrapper.
  - Now test cases can run.
- C++ codegen for wrappers (should be normal codegen) with JSON serizliation in C++.
- Implement event handling.
- Implement `@rpc:Byref` and `ObservableList`.
- Implement `@rpc:Dynamic`.
- Implement `@rpc:Ctor`.
- Solve interface inheritance and casting.
- Solve containers of interfaces.
- Solve duplicating serialization when two different assemblies share the same set of structs/enums.

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

- See [TODO_RPC_Definition.md](./TODO_RPC_Definition.md)

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
