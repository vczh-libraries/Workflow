investigate repro

Follow
- `.github\Rules\new-sample.md`
- `.github\Rules\new-sample-rpc.md`
- `.github\Rules\verify-and-commit.md`
to add new samples, indentation should be double spaces no matter how the code below is written:

RPC samples are split into two files:
- `Rpc\SAMPLE.txt` contains only RPC definitions.
- `Rpc\SAMPLE_Test.txt` contains executable test logic, including globals, helpers, `serviceMain`, and `clientMain`.

Only `SAMPLE` is added to `IndexRpc.txt`. Both files must appear in `CompilerTest_LoadAndCompile` under `Resource Files\Rpc`.

## Sample Rpc\Inheritance.txt

```Workflow
module Rpc;
using system::*;

namespace YourFavoriteNamespace // use RpcInheritance
{
  @rpc:Interface
  interface IDerived : IOne, ITwo
  {
    func SetDerivedValue() : void;
  }

  @rpc:Interface
  interface IValue
  {
    @rpc:Cached
    prop Value : string {const, observe}
  }

  @rpc:Interface
  interface IOne : IValue
  {
    func SetOneValue() : void;
  }

  @rpc:Interface
  interface ITwo : IValue
  {
    func SetITwoValue() : void;
  }

	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
    func CreateOne() : IOne^;
    func CreateTwo() : ITwo^;
    func CreateDerivec() : IDerived^;
	}
}
```

## Sample Rpc\Inheritance_Test.txt

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
    override func CreateOne() : IOne^
    {
      return new IOne^
      {
        var value : string = "";
        override func GetValue() : string { return value; }
        override func SetOneValue() : void
        {
          value = "One";
          ValueChanged();
        }
      }
    }
    
    override func CreateTwo() : ITwo^
    {
      // Copy CreateOne but set the value to "Two"
    }
    
    override func CreateDerived() : IDerived^
    {
      // Copy CreateOne but set the value to "Derived"
      // Meanwhile Set(One|Two)Value should raise exception
    }
	};
	lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func clientMain(lc : IRpcLifecycle*) : string
{
	var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");
  var one = cast (IOne^) service.CreateOne();
  // repeat for two and derived

  s = $"$(s)[$(one.Value)]";
  one.SetOneValue();
  s = $"$(s)[$(one.Value)]";
  // repeat for two and derived

  return s; // [][One][][Two][][Derived]
}
```

## Goal

In this task you are going to build and run test cases to verify if these cases are working, according to `TODO_RPC_Definition.md`
This test is to ensure that:
- Wrapper Workflow Script code generation take members of base types of interfaces into consideration recursively.

If the current implemention is correct, the added samples should just pass the test.
