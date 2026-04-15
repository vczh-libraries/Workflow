# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.
- References:
  - [Definition](./TODO_RPC_Definition.md)
  - [Scenarios](./TODO_RPC_Scenarios.md)

## ToDo

- Update Workflow compiler to generate list of unique identifier (string) for all rpc types, methods, events.
- Implementation in test library that connects a caller and a callee controller, all in one client.
  - Enable serialization pipeline injection.
  - `DescriptableObject::SetInternalProperty` will be used to receive destructor call of local objects, to know if an object becomes unavailable.
- Workflow compiler generates wrappers to call the controller.
  - Only supports `@rpc:Byval` and `@rpc:Cached`, also no `@rpc:Ctor`, no containers of interfaces, CHECK_FAIL if these happen.
  - Caller side interface implementation (in Workflow script), to call object ops and listen to object event ops.
    - Destructor could call `ReleaseRemoteObject`.
  - Callee side object ops implementation (in Workflow script), attach to all events to call object event ops.
  - JSON serialization based on reflection.
  - Lifecycle can be managed by wrapper destructor, acquire/release will be only called once per wrapper.
  - Now test cases can run.
    - Create a Rpc category, do not do C++ code generation. TestRuntimeCompile.cpp will handle this.
    - Need to expose another function besides of `main` and `RuntimeTest` will pick them up and connect them with controller constructions.
- C++ codegen for wrappers (should be normal codegen) with JSON serizliation in C++.
  - Generate C++ code from Rpc category with a new pattern.
- Implement event handling.
- Implement `@rpc:Byref` and `ObservableList`.
- Implement `@rpc:Dynamic`.
- Implement `@rpc:Ctor`.
- Solve interface inheritance and casting.
- Solve containers of interfaces.
- Solve duplicating serialization when two different assemblies share the same set of structs/enums.
