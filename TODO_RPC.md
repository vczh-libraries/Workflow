# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.

## ToDo

- Add `IRpcController` to the rpc library (new pair of file).
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
