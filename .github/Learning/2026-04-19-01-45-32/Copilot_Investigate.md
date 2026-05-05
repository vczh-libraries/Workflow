# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

You must complete all following tasks. In order to make the work easier, when performing each task you should:
- Execute the task.
- Pass all unit test, fix any test failure including pre-existings.
- git commit and git push to the current branch.

Perform this per task, one after another, and only after all task is done:
- Run `..\Tools\Tools\Build.ps1 Workflow` for a complete CI.
- Pass all unit test, fix any test failure including pre-existings.
- git commit and git push to the current branch.

Perform each task like a new `# Repro`, which means when a task is done, you can wipe the `Copilot_Investigate.md` for the next task.

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.

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

## Task 3

Follow job.new-sample.md to create a new `Rpc\PrimitiveTypes.txt`. This test case is for verifying if all kinds of primitive types are correctly handled.

All types includes predefine primitive types, a custom struct, a custom enum.

Each type has a function in `IService` and it will look like this:

```Workflow
// declaration
func ProcessInt(value : int) : int;

// implementation
override func ProcessInt(value : int) : int
{
  return value + 1;
}
```

You need to perform a random change to each primitive type, and join them in `clientMain` like `[x][y][z]...`, and see if all functions return expected values.

# UPDATES

# TEST

N/A

# PROPOSALS
