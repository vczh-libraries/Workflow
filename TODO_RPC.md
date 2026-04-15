# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.
- References:
  - [Definition](./TODO_RPC_Definition.md)
  - [Scenarios](./TODO_RPC_Scenarios.md)

## ToDo

- Update Workflow compiler to generate list of unique identifier (string) for all rpc types, methods, events.
- Add `IRpcController` and `IRpcLifeCycle` to the `WfLibraryRpc.(h|cpp).
  - Caller side `IValue*` implementation to call list ops and listen to list event ops.
    - Each `IValue*` collection (enumeratle, list, dictionary, observable) should have a wrapper implementation, accepting `IRpcLifeCycle` and a `RpcObjectReference` arguments in their constructors. Only writable versions are needed because they also implement readonly version interfaces.
    - `RpcByref*` inherits from `IValue*`, meaning they implement `@rpc:Byref` containers. When convert between objects and serializable objects, they should use the `byref` helper function.
  - Callee side list ops implementation to call `IValue*`, attach to all events to call list event ops.
    - Callee side implementations accepting `IRpcLifeCycle` argument in their constructors.
  - No object ops implementations are introduced in this task.
  - Boxing and unboxing helper functions:
    - `RpcBoxByref`, `RpcUnboxByref`, `RpcBoxByval`, `RpcUnboxByval`.
    - `Box` converts from trivial objects to serializable objects. `Unbox` do the reverse.
    - Trivial objects means objects in their original form, like an `@rpc:Interface` interface implementation, `IValue*` collections, enums, structs.
    - Serializable objects means objects in their controller form. `@rpc:Interface` and `byref` `IValue*` collections become `RpcObjectReference`. `byval` `IValue*` collections become their `IValue*` version of serializable objects (this happens recursively, so boxing/unboxing byval collections could just be recursive functions).
    - All these functions convert between `Value` to `Value`, taking `IRpcLifeCycle` as their second argument.
    - All values passing to `IRpcController` should be serializable objects. All values passing to or returning from the interface of wrappers or `@rpc:Interface` implementations should be trivial objects. The `BoxValue` and `UnboxValue` function would help to process non-interface values.
  - Everything in `WfLibraryRpc.h` should have reflection handled in `WfLibraryReflectin.(h|cpp)`.
  - Testing (complete in `TestLibraryRpcByval.cpp` in `LibraryTest`)
    - You are going to implement a `IRpcLifecycle` and `IRpcController` implementations that only work on `byval` controllers, called `RpcByvalLifecycleMock` (it should be in `RpcByvalLifecycleMock.(h|cpp)` but in the same folder of `TestLibraryRpcByval.cpp`). It means methods in `IRpcLifecycle` and two object ops interfaces do `CHECK_FAIL(L"Not Supported!");`. The implementation inherits from both interface so the `Controller` property just returns itself. In `IRpcController`, `Register` only writes down two list ops interfaces, instances for two object ops interfaces will be nullptr so they are ignored. List ops in `IRpcController` will just redirect all calls to life ops passed to the `Register` function. They will use the `callee side implementations` described above.
    - In every test case, a life cycle object and two ops objects are created, call `Register` to finish the initialization, and being testing `caller side implementations`. Usually a collection of `vint` is used, use the reflection API to convert it to a `IValue*` shared pointer, box it to `Value`, call `RpcBoxByref` to box it again to get the `caller side implementation` but still in `Value`, then unbox it to `IValue*` which is actually the `caller side implementation`. Note these two `IValue*` implementations are different, the first one is the reflection implementation to redirect everything to the actually collection type, the second one will call `IRpcController` inside.
    - You should use Array to `IValueArray`, List to `IValueList`, Dictionary to `IValueDictionary`, ObservableList to `IValueObservableList`, operate the caller side implementation and see if the actual collection is changed, and for observable list, operate the actual collection and see if the event in the caller side `IValueObservableList` is raised. Each container type will be in its separate `TEST_CASE`.
    - Only `BoxParameter` and `UnboxParameter` could handle collection boxing, but in `Rpc(Unb|B)oxBy(Val|Ref)` should use `BoxValue` and `UnboxValue` because all reference type (interface implementation) should be handled separatedly.
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
