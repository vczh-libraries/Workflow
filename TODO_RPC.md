# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.
- References:
  - [Definition](./TODO_RPC_Definition.md)
  - [Scenarios](./TODO_RPC_Scenarios.md)

## ToDo

- Verify `RpcDualLifecycleMock` and `TestCasesRpc.cpp`.
  - Ensure rpcwrapper_typename encodes namespace.
  - Generate wrapper picker function from type id, remove TestCasesRpc.cpp and TestRpc.cpp constructions.
- Continue to add more test cases until all features are covered.
- C++ codegen for rpc data type JSON serialization.
- Solve interface inheritance and casting.
- Delete Runtime test category.

# Prompt

## Task 1

Add `IRpcWrapperBase` interface to `WfLibraryRpc.h` with reflection. This interface must have a proxy with a function DisconnectFromLifecycle().

In `RpcLifecycleMock::~RpcLifecycleMock` before doing `services.Clear()`, each service must be cast to `IRpcWrapperBase`, and when successful call its `DisconnectFromLifecycle` function. Same thing must be done to `RpcDualLifecycleMock`'s destructor, so that the current workaround could be avoided.

And in the generated wrapper, a little twist must be performed. Checkout the current `Wrapper_RequestService.txt` which comes from `Test\Resource\Rpc\RequestService.txt`, currently the wrapper constructor function looks like this:

```Workflow
namespace RpcTest
{
    func rpcwrapper_IService(lc : system::IRpcLifeCycle*) : (RpcTest::IService^)
    {
        ...
        var proxy : RpcTest::IService^ = new (RpcTest::IService^)
        {
            var _lc : system::IRpcLifeCycle* = lc;

            var _controller : system::IRpcController* = controller;

            var _ref : system::RpcObjectReference = proxyRef;

            override func GetText() : (::system::String)
            {
                ...
            }
        };
        return proxy;
    }
}
```

Look at the very beginning of the file, it has a variable
```Workflow
var rpctype_RpcTest__IService : int = 0;
```
defining the type id of this interface. I would like you to use the same naming convention, by converting
```Workflow
namespace RpcTest
{
  func rpcwrapper_IService(lc : system::IRpcLifeCycle*) : (RpcTest::IService^) ...
}
```
to
```Workflow

namespace RpcTest
{
  interface IRpcWrapper_IService : ::RpcTest::IService, system::IRpcWrapperBase
  {
  }
}

func rpcwrapper_RpcTest__IService(lc : system::IRpcLifeCycle*) : (RpcTest::IService^) ...
```

While keeping the function returning `IService^`, but inside the function an instance of `RpcTest::IRpcWrapper_IService` must be created. The namespace `RpcTest` is here because `IService` is defined in `RpcTest`. And then you can do this in the instance of this interface:
- Remove `controller` in the function and `_controller` in the interface, you can always use `_lc.Controller` for that. No need to make a cache.
- In `IRpcWrapperBase::DisconnectFromLifecycle` set `_lc` to `null`. Any function should check the `_lc` and throw when it is null.
- In its destructor `delete{}`, call `_lc.Controller.ReleaseRemoteObject`. But here you don't need to throw when `_lc` is `null`, instead skip the `ReleaseRemoteObject` when `_lc` is `null`.

## Task 2

Functions like `rpcwrapper_RpcTest__IService` are created for each `@rpc:Ctor` interface. But I would like you to add a dispatcher function:

```Workflow
func rpcwrapper_Create(typeId : int, lc : system::IRpcLifeCycle*) : interface^;
```

To call correspinding `rpcwrapper_RpcTest__IService` according to its `typeId` (which will be `rpctype_RpcTest__IService`). The pattern is easy to build, and when a type id does not exist, or it is not an interface marked with `@rpc:Ctor`, throw.

So you can perform changes to `RpcWorkflowLifecycleMock` in `TestRpc.cpp` and generated `LocalRpcMock` in `TestCasesRpc.cpp` to make it simpler.