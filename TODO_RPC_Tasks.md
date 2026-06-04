investigate repro

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

## Task 1

`IRpcListOps` missing `IValueArray::Resize` equivalent constructions:
- `RpcByrefArray::Resize` has to do wired implementation, which is incorrect and should be fixed to be straight forward.
- `RpcCalleeListOps::ListClear` calls `IValueArray::Resize(0)`, which is incorrect, it should crash on array as array has no `Clear`.
Fix it by adding `ArrayResize` to `IRpcListOps`.

Rename `Oblist_EventException` rpc sample to `ListOps_OblistEventException`:
- Namespace in the sample change from `RpcOblistEventException` to `RpcListOpsOblistEventException`
- Update all affected files.

## Task 2

Follow
- `.github\Rules\new-sample.md`
- `.github\Rules\new-sample-rpc.md`
- `.github\Rules\document-and-commit.md`
to add new samples, indentation should be double spaces no matter how the code below is written:

RPC samples are split into two files:
- `Rpc\SAMPLE.txt` contains only RPC definitions.
- `Rpc\SAMPLE_Test.txt` contains executable test logic, including globals, helpers, `serviceMain`, and `clientMain`.

Only `SAMPLE` is added to `IndexRpc.txt`. Both files must appear in `CompilerTest_LoadAndCompile` under `Resource Files\Rpc`.

### Sample Rpc\ListOps_ListException.txt

```Workflow
module Rpc;
using system::*;

namespace YourFavoriteNamespace // use ListOpsArray
{
  @rpc:Interface
  @rpc:Ctor
  interface IServer
  {
    func DoSomethingWrong(xs : int[]) : int;
  }
}
```

### Sample Rpc\ListOps_ListException_Test.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;
using YourFavoriteNamespace::*;

var s = "";

func serviceMain(lc : IRpcLifecycle*) : void
{
  var serviceObj = new (YourFavoriteNamespace::IService^)
  {
    override func DoSomethingWrong(xs : int[]) : int
    {
        return xs[10];
    }
  };
  lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func clientMain(lc : IRpcLifecycle*) : string
{
  var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");
  try
  {
    service.DoSomethingWrong({});
  }
  catch (ex)
  {
    s = ex;
  }
  return s;
}
```

### Sample Rpc\ListOps_DictionaryException.txt

```Workflow
module Rpc;
using system::*;

namespace YourFavoriteNamespace // use ListOpsArray
{
  @rpc:Interface
  @rpc:Ctor
  interface IServer
  {
    func DoSomethingWrong(xs : int[string]) : int;
  }
}
```

### Sample Rpc\ListOps_DictionaryException_Test.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;
using YourFavoriteNamespace::*;

var s = "";

func serviceMain(lc : IRpcLifecycle*) : void
{
  var serviceObj = new (YourFavoriteNamespace::IService^)
  {
    override func DoSomethingWrong(xs : int[string]) : int
    {
        return xs["Anyway"];
    }
  };
  lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func clientMain(lc : IRpcLifecycle*) : string
{
  var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");
  try
  {
    service.DoSomethingWrong({});
  }
  catch (ex)
  {
    s = ex;
  }
  return s;
}
```

## Goal

In this task you are going to build and run test cases to verify if these cases are working, according to `TODO_RPC_Definition.md`
This test is to ensure that:
- Both `Exception` and `Error` will be catched.

If the current implemention is correct, the added samples should just pass the test.
Scan through all `InvokeMethod` and `InvokeEvent` code around try-catch and make sure both `Exception` and `Error` are treated equally.
- Even when `Error` is for fatal error but since this is the Workflow environment `Exception` and `Error` are treated as the same thing.
