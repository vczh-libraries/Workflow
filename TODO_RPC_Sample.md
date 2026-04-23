# Prompt

Follow job.new-sample.md to add a new Rpc\Dtor2.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

namespace YourFavoriteNamespace // use RpcDtor3Test
{
  @rpc:Interface
  interface IContainer { prop Value : IValue^ {const} }

  @rpc:Interface
  interface IValue{}

	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
    func ContainValue(value : IValue^) : IContainer^;
	}
}

var s = "";

func serviceMain(lc : IRpcLifeCycle*) : void
{
	var serviceObj = new (YourFavoriteNamespace::IService^)
	{
    override func ContainValue(value : IValue^) : IContainer^
    {
      if ((value as (IRpcWrapperBase^) is null)) { throw "IValue(value) should be a local object in serviceMain"; }
      return new (YourFavoriteNamespace::IContainer^)
      {
        delete
        {
          s = $"$(s)[IContainer]";
        }

        override func GetValue() : IValue^
        {
          return value;
        }
      };
    }
	};
	lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func MakeValue() : IValue^
{
  return new (YourFavoriteNamespace::IValue^)
  {
    delete
    {
      s = $"$(s)[IValue]";
    }
  }
}

func clientMain(lc : IRpcLifeCycle*) : string
{
	var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");
  if ((oservicebj as (IRpcWrapperBase^) is null)) { throw "IService(service) should be a wrapper object in clientMain"; }
  {
    s = $"$(s)[1]";
    var container = service.ContaineValue(MakeValue());
    if ((container as (IRpcWrapperBase^) is null)) { throw "IContainer(container) should be a wrapper object in clientMain"; }
    if ((container.Value as (IRpcWrapperBase^) is not null)) { throw "IValue(container.Value) should be a local object in clientMain"; }
    container = null;
    /*
      At the moment, container wrapper is deleted.
      ObjectHold will be called from client to service to free the real container.
      The value wrapper in the real container will be deleted.
      Object hold will be called from service to client to free the real value.
      Destructor of IValue will be called after destructor of IContainer.
    */
    s = $"$(s)[2]";
  }
  {
    var container = service.ContaineValue(MakeValue());
    var value = container.Value;
    if ((container as (IRpcWrapperBase^) is null)) { throw "IContainer(container) should be a wrapper object in clientMain"; }
    if ((value as (IRpcWrapperBase^) is not null)) { throw "IValue(value) should be a local object in clientMain"; }
    container = null;
    /*
      At the moment, container wrapper is deleted.
      ObjectHold will be called from client to service to free the real container.
      The value wrapper in the real container will be deleted.
      Object hold will be called from service to client to free the real value.
      but the real value object is still hold at client side.
    */
    s = $"$(s)[3]";
    var = null;
    /*
    The real value object is supposed to be deleted now.
    Because the client side lifecycle is no longer holding it.
    */
    s = $"$(s)[4]";
  }
  return s; // [1][IValue][IContainer][2][IContainer][3][IValue][4]
}
```

Understand what the test case trying to say, you are not allowed to change:
- The content of the sample, unless it doesn't build.
- Workflow parser.
- Workflow compiling.
- Workflow to C++ code generation.

You are highly possibly need to fix implementation of `RpcDualLifecycleMock` and its connected interfaces if sample fails in either `RuntimeTest` or `CppTest*`.
- The comment in the sample describes how `RpcDualLifecycleMock` and the generated C++ code is supposed to work.
  - The generated C++ code is very straight forward, if it fails, check `RpcDualLifecycleMock` first.
- If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
- Pass all unit test, fix any test failure including pre-existings.
- After finishing everything, git commit and git push to the current branch.
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
