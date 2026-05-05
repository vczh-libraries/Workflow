# Repro

Follow
- `.github\Rules\new-sample.md`
- `.github\Rules\new-sample-rpc.md`
- `.github\Rules\verify-and-commit.md`
to add new samples, indentation should be double spaces no matter how the code below is written:

RPC samples are split into two files:
- `Rpc\SAMPLE.txt` contains only RPC definitions.
- `Rpc\SAMPLE_Test.txt` contains executable test logic, including globals, helpers, `serviceMain`, and `clientMain`.

Only `SAMPLE` is added to `IndexRpc.txt`. Both files must appear in `CompilerTest_LoadAndCompile` under `Resource Files\Rpc`.

## Sample Rpc\EventArgs.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

namespace YourFavoriteNamespace // use RpcEventArgs
{
  @rpc:Interface
  interface IValue
  {
    @rpc:Cached
    prop Value : string {const, not observe}
  }

	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
    func Print(s : string?, v : IValue^) : string;
	}
}
```

## Sample Rpc\EventArgs_Test.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

var s = "";

func serviceMain(lc : IRpcLifecycle*) : void
{
	var serviceObj = new (YourFavoriteNamespace::IService^)
	{
    override func Print(s : string?, v : IValue^) : string
    {
      return $"[$(cast string s ?? 'null')][$(v.Value ?? 'null')]";
    }
	};
	lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func clientMain(lc : IRpcLifecycle*) : string
{
	var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");

  s = $"$(s)$(service.Print("abc", null))";
  s = $"$(s)$(service.Print(null, new IValue^ { override func GetValue() { return "def"; } }))";

  return s; // [abc][null][null][def]
}
```

## Goal

In this task you are going to build and run test cases to verify if these cases are working, according to `TODO_RPC_Definition.md`
This test is to ensure that:
- Since all primitive types are serializable, nullable type could only be applied to primitive types, so it should also be serializable.
  - If nullable types is rejected with PRC specific errors, it should be fixed.
  - There are also limited types that could be used with the nullable operator "?". Verify if such types are all serializable. If yes, it is easy to say all nullable types are also serializable. If no, then you need to look into `T` when you see `T?`. Answer this question when you finish the work.
- Passing null is handled properly.
  - If passing null crashes, `Rpc(B|Unb)oxBy(val|ref)` should be the first thing to look at.

If the current implemention is correct, the added samples should just pass the test.
