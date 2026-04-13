# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.

## Steps

- Remove async return value.
- Generate rpc metadata in strong typed and JSON.
  - Regenerate Workflow AST as metadata.
    - Collect all used structs and enums, collect all `@rpc:Interface` interfaces from the current assembly, and regenerate their AST.
      - `GetTypeFromTypeInfo` function will be useful when reconstructing them.
      - Attributes on interfaces and their content should remain.
      - Shortcut declarations like auto properties will not generate, use their original forms.
    - Put them in one module in enums/structs/interfaces order.
      - Enums are ordered by name.
      - Structs are ordered by visiting order, keep struct dependency correct.
      - Interface are ordered by AST order.
      - Namespaces will be merged as much as possible.
  - Testing
    - Update TestRuntimeCompiler.cpp to add attribute metadata assertion.
    - Use `testCpuArchitecture` to understand what architecture you are in and serialize the generated metadata to:
      - Test\Generated\RpcMetadata(32|64)\{itemName}.txt
        - There is only one `Attributes.txt` under `Runtime` so itemName becomes Attributes
    - Compare it to baseline files
      - Test\Resources\Baseline\RpcMetadata(32|64)\{itemName.txt}
      - There is no such baseline files at the beginning, you are going to make them, so that they can exactly match the generated rpc metadata. And it is also a chance for you to verify if the output is right.
    - You must break baseline and metadata files into lines to compare them just like what CompilerTest_GenerateMetadata is doing
    - Rpc metadata is a workflow module AST, call the `WfPrint` with `stream::GenerateToStream` to generate workflow source files from it. You can find many samples about how to use them directly.
    - In order to verify generated Rpc metadata is legit, you need to stores all all rpc metadata to a `Dictioary<WString, Ptr<WfModule>> rpcMetadatas;` right inside `TEST_FILE`, key is itemName, and make another TEST_CATEGORY, run each key-value pair in each TEST_CASE. Follow the first TEST_CATEGORY to make loop right. But this time you don't need to print anything in this second TEST_CATEGORY, just assert that no build errors are generated.
- Add communication layer with unit test implementation.
  - Note: no metadata involved here, this is the architecture of commands.
  - Unit test in `LibraryTest`.
- Create dynamic serialization implementation based on reflection for all touched rpc types.
  - Server and client will contains copy of same interface types, but loading two assemblies into a process conflicts in reflection.
  - Should allow the same schema mapping to equivalent interfaces in different namespaces.
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
