investigate repro

- you have to follow `REPO-ROOT/.github/Guidelines/Coding.md` when coding.
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.
  - It is important to do task one by one strictly, by me designing tasks in this way, we can achieve:
  - Easy-to-understand commits for file changing that is easy to review.
  - Limit side effects so that you don't have to deal with massive of issues at the same time.

## Task 1

General Workflow script AST building improvements:
- `try-catch` and `try-finally` cound be merged into one single `WfTryStatement`. You are going to also find if such pattern appear anywhere else and fix them as well.

Just like `ReadEventException`, you are going to add a `ReadMethodException`.
In generated `rpcops_IOps_CreateJson`, between calling `objectOps.InvokeMethod` and processing actual return value, exception handling are identical among every methods.
Rewrite this piece of code in C++ and the script could just call `IRpcLifecycle::ReadMethodException`.
`ReadMethodException` will read return value directly from `InvokeMethod`, if there is an exception it raise an exception, otherwise nothing happens.
So A call to `ReadMethodException` without worrying about the return value is enough.

In generated `rpcops_IRpcObjectEventOpsJson`:
- `raise "Unknown RPC event id.";` is not an exception raised in any remote event handlers, it is a local error. So it cannot be turned into a record in the returned map.
- The key of the returned map is actually the remote client id which raise the exception, not the local client.
  - The same to `IRpcListEventOps` implementation in `WfLibraryRpcWrappers.(h|cpp)`.
  - A few test results in `IndexRpc.txt` may be impacted.
  - In `rpcops_IRpcObjectEventOpsJson` you are not able to obtainer such information, they should be done in C++ authored `IRpcDispatcher` implementation.
- When the RPC definition has no event, the whole function could be collapsed to one single `raise "Unknown RPC event id.";` instead of a big piece of boilerplate code.
  - This also matches the semantic after fixing, because only exceptions raised in remote event handlers are put into the map.

In generated `rpcops_IRpcObjectOpsJson`:
- `raise "Unknown RPC method id.";` is not an exception raised in any remote method call, it is a local error. So it cannot be turned into `RpcException`.

To keep the generated code clean, when handling unknown id exceptions, you could first `var unknownId = false`, and set it to true in the `default` case.
In this way you are able to check this variable in the no-exception branch and raise the exception accordingly.
Only do this when raising the unknown id exception directly isn't work because it is guarded in a try-catch which turn the exception into something else, otherwise you should still raise the exception directly.

Don't handle exceptions in any ops interface implementation, instead we should do it in `IRpcDispatcher` in this way:
- This section only talks about wrappers which read the remote return value and turn it back into an exception, not including ops interfaces which catch the exception and turn into serializable exception value to pass it back.
- Rename `RpcDualDispatcherMock` to `RpcDualDispatcherMockBase`.
- Create a `RpcDualDispatcherMock` inheriting from `RpcDualDispatcherMockBase`, doing things like `RpcDualJsonDispatcherMock` that create one more indirection of 8 ops interface instances.
- Now you got JSON version and non-JSON version of ops interface wrappers for dispatcher mocks, you are going to read the return value of `InvokeMethod`, `InvokeEvent` and `OnItemChanged`, to see if the return value is representing an exception, and throw it directly.
  - `InvokeEvent` and `OnItemChanged` returns a map, but in the dual mock there is only one remote client, so when exception happens you will know the "remote client id", the code would be simpler.
- In this way all code that calls the dispatcher don't need to worry about the exception anymore.
  - Unless in `rpcops_IRpcObjectEventOpsJson` you still need to call `SetEventSuppressedFlag` when event happens.

## Task 2

`Rpc/Oblist_EventException` should attach the handler which raise an exception in `serviceMain`. The current test case `clientMain` attach an event handler to raise exception, and it raise the event, that would be a local exception not a remote exception. It doesn't do the expected test. Fix it like this:

```Workflow
module Rpc;
using system::*;
using RpcOblistEventException::*;

func CrashItemChanged(index : int, oldCount : int, newCount : int) : void
{
  raise $"$(index),$(oldCount),$(newCount)";
}

func serviceMain(lc : IRpcLifecycle*) : void
{
  var serverObj = new (IServer^)
  {
    var _List : observe int[] = null;

    override func GetOblist() : observe int[]
    {
      _List = {};
      attach(_List.ItemChanged, CrashItemChanged);
      return _List;
    }
  };
  lc.RegisterService("RpcOblistEventException::IServer", serverObj);
}

func clientMain(lc : IRpcLifecycle*) : string
{
  var server = cast (IServer^) lc.RequestService("RpcOblistEventException::IServer");
  var xs = server.GetOblist();

  try
  {
    xs.Add(0);
  }
  catch (ex)
  {
    return ex.Message;
  }

  return "No Exception";
}
```
