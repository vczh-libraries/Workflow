# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.
- References:
  - [Definition](./TODO_RPC_Definition.md)
  - [Scenarios](./TODO_RPC_Scenarios.md)

## ToDo

```prompt
Create a `WfRpcMetadata` struct before `WfLexicalScopeManager` to contain:
- `rpcMetadata`: Move from `WfLexicalScopeManager` to here.
- `typeNames`: A `Dictionary<WString, WfDeclaration*>` storing all full name of types in `rpcMetadata`, identifiers joined with "::" just like C++/Workfoow.
- `methodNames`: A `SortedList<WString, WfFunctionDeclaration*>` storing all full name of methods in `rpcMetadata`.
  - It includes all methods in all interfaces (`rpcMetadata` interfaces are all marked with `@rpc:Interface` already)
  - The method name would be `<FULL-TYPE-NAME>.<MethodName>`.
  - **ONLY WHEN** overloading exists, the method name will be appended by `_ArgName1_ArgName2...`. Nothing will be appended if there is no argument.
  - For overloading methods that still result in the same name, `_0`, `_1`... need to be appended, including the first one.
- `eventNames`: Just like method names but it stores `WfEventDeclaration` and there will be no overloading.

Create a very simple test case in a new `Rpc` category, you needs to create a `Rpc` folder as well as `IndexRpc.txt`, with the following content:
- `serviceMain` function accepts a `IRpcLifecycle*` and run.
- `clientMain` function accepts a `IRpcLifecycle*` and register a service.
- A service will be an interface with `@rpc:Ctor`. For the first new test case it will be named after `RequestService`.
- The interface has a `GetText` method without argument, the `serviceMain` will use create an implementation:
  - `GetText` returns a text.
  - Call lifecycle interface's `RegisterService(string, interface^)` to register the service with its full name. This function does not exist, you need to create it, there is an implementation in `LibraryTest` for testing purpose, say `CHECK_FAIL(L"Not Supported!");` there.
  - `clientMain` will all `RequestService(string): interface^` and do a cast (also not exists as above), call `GetText` and return the result as the test case result.

To build this case, you will need to do the similar thing as `TestRuntimeCompile.cpp` in `CompilerTest_LoadAndCompile`, but in a new file called `TestRpcCompiler.cpp`, using a shared manager like other test files:
- Clear and load a case, compile. And you will get the `WfRpcMetadata`, no log file will be created.
- Generate wrapper implementations from `WfRpcMetadata`, write this file to `Test\Generated\RpcMetadata(32|64)\Wrapper_<CASE-NAME>.txt`.
  - It will not be another pass, create `WfAnalyzer_GenerateRpc.cpp`, taking the manager and returning a new `WfModule`.
  - The exported function will be `GenerateModuleRpc` and extern in `WfAnalyzer.h` below ValidateModuleRPC.
- Clear and load the case again as well as the generated wrapper, do a normal compiling like `TestRuntime.cpp` processing `Codegen` but skip the C++ code generation.
  - `TestRuntimeCompile.cpp` seems to not generate `Parsing.Runtime.CASE-NAME.txt`, fix it.
  - `TestRpcCompiler.cpp` will do `Parsing.Rpc.CASE-NAME.txt` too, but the wrapper file does not need to create this file.
- `RuntimeTest` needs to load these cases in a new file `TestRpc.cpp`, doing what `TestRuntime.cpp` do, but only load the assembly, no running is needed. Because the lifecycle interface implementation is not created yet.

In the generated wrapper, you need to do these things:
- Since the original code will compile with the generated wrappers, so it doesn't need to include anything from `rpcMetadata`.
- The generated wrapper will be in a module called `RpcMetadata`, which is the same as the one in `rpcMetadata`.
- Global variables of `int`
  - `rpctype_xxx`, `rpcmethod_xxx` and `rpcevent_xxx`, all comes from `WfRpcMetadata`, `::` and `.` will be replaced by `_`.
  - List type followed by method followed by event, values from 0 ascending even across group.
- Global variables of `int[string]`, maps from these number to their original name (`::` and `.` version).
- Implementations callee side ops interface, global function `rpc_IRpcXXXOps` function returning `IRpcXXXOps^`:
  - `IRpcObjectOps`: Call `Rpc(UnbB)oxBy(ref|val)` according to if `@rpc:Byref` is attached or not (doesn't matter if `@rpc:Byval` is attached or not because it is the default value):
    - For property getter and setter, see the property. You need to read property definition to know which methods are getters and setters, the method names mean nothing.
    - For method return type, see the method.
    - For method argument, see the argument.
  - `IRpcObjectEventOps`: Call `Rpc(Unb|B)oxByval` for argument serialization.
  - These functions are not reflectable yet, you need to register them as `IRpcLifecycle`'s static function.
  - Callee side ops interfaces take an input of `IRpcLifecycle*`.
  - Those global variables help to know which ID is which.
- Implementations caller side `@rpc:Interface` interface implementation:
  - If the function name is `my::favorite::namespace_name::interface_name`, the function will be `my::favorite::namespace_name::rpcwrapper_interface_name`.
  - The function returns `interface_name^`.
  - The function takes a `IRpcLifecycle*` as a parameter and implement redirect all methods to those ops interfaces, following the same rule above about byval or byref.
- Caller side ops:
  - `IRpcObjectOps` will take method arguments from the wrapper and send them to the remote.
  - `IRpcObjectEventOps` will take event arguments from wrapper, these events will be triggered in the caller side directly, so in each rpc interface wrapper all events will be listened to call `IRpcObjectEventOps`.
- Callee side ops:
  - `IRpcObjectOps` do the reverse, read arguments and call methods.
  - `IRpcObjectEventOps` do the reverse, read arguments and call events.
  - You will notice that, the implementation is symmentric but why callee side doesn't hook events from the actual interface (unlike the caller side hooking wrappers), because it will be part of the future implementation of `IRpcLifecycle`.
- No more global variable to create.
- Ops interface implementations are for both service and client.
- You cannot assume the ops implementation will be registered to the same `IRpcLifecycle` instance with the one passed to the function to create it.

Since the test case can't be actually launched yet, just compile in `CompileTest_LoadAndCompile` and load assembly in `RuntimeTest` is enough.
Actually execution will be in the future when `IRpcLifecycle` is implemented.
```

- Solve duplicating serialization when two different assemblies share the same set of structs/enums.
  - After generating assemblies, collect all generated rpc metadata module, remove existing and duplicating declarations.
  - And then do codegen works for rpc.
  - generate list of unique identifier (string) for all rpc types, methods, events.
- Workflow compiler generates wrappers to call the controller.
  - Add RPC test categories.
  - Load test case and generate metadata.
  - Load metadata and generate wrappers.
  - Load test case with wrappers and generate C++/binary.
  - One very simple test case to call a `@rpc:Ctor` service.
    - `serviceMain` accepts a lifecycle interface and run.
    - `clientMain` accepts a lifecycle interface and register a service.
    - They have client id 1 and 2.
    - lifecycle1 redirect ops to lifecycle2 callee ops, vice versa.
    - it means lifecycle2 callee ops are created with lifecycle2 reference and are registered into lifecycle1.
- `RumtimeTest` need to create two set of objects, call `serviceMain` followed by `clientMain`.
  - `DescriptableObject::SetInternalProperty` will be used to receive destructor call of local objects, to know if an object becomes unavailable.
- Continue to add more test cases until all features are covered.
- C++ codegen for wrappers (should be normal codegen) with JSON serizliation in C++.
  - Generate C++ code from Rpc category with the new pattern.
  - This time new lifecycle implementations will be created tonise JSON serialization for values (protocol not included).
- Solve interface inheritance and casting.
- Delete Runtime test category.
