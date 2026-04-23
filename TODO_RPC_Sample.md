# Prompt

Follow job.new-sample.md to add a new Rpc\FailDoubleRegistration.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

namespace YourFavoriteNamespace // use RpcFailDoubleRegistrationTest
{
  @rpc:Interface
  interface IObject
  {
    prop Name : string {get}
  }

	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
    func SetObject(obj : IObject^, index : int) : IObject^;
	}
}

func MakeObject(name : string) : IObject^
{
  return new IObject^
  {
    override func GetName() { return name; }
  }
}

var objs : IObject^[] = { MakeObject("1st"); MakeObject("2nd"); };
var s = "";

func serviceMain(lc : IRpcLifeCycle*) : void
{
	var serviceObj = new (YourFavoriteNamespace::IService^)
	{
    override func SetObject(obj : IObject^, index : int) : IObject^
    {
      s = $"$(s)[service:Received $(obj.Name)]";
      return objs[index];
    }
	};
	lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func CallService(service : YourFavoriteNamespace::IService^, indexParameter : int, indexReturn : int) : void
{
  s = $"$(s)[call]";
  try
  {
    var obj = service.SetObject(objs[indexParameter], indexReturn);
    s = $"$(s)[client:Received $(obj.Name)]";
  }
  catch (ex)
  {
    s = $"$(s)[exception]";
  }
}

func clientMain(lc : IRpcLifeCycle*) : string
{
	var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");

  /*
  A call to make client owns 1st and service owns 2nd.
  Because which lifecycle owning an object depends on which lifecycle touching it eariler.
  */
  CallService(service, 0, 1);

  /*
  serviceMain returning objs[0] triggers error.
  */
  CallService(service, 0, 0);

  /*
  clientMain passing objs[1] triggers error.
  */
  CallService(service, 1, 0);
  CallService(service, 1, 1);

  /*
  Return the log which could tell the timing of exception.
  The actual message of the exception does not matter.
  */
  return s; // [call][service:Received 1st][client:Received 2nd][call][service:Received 1st][exception][call][exception][call][exception]
}
```

Understand what the test case trying to say, you are not allowed to change:
- The content of the sample, unless it doesn't build.
- Workflow parser.
- Workflow compiling.
- Workflow to C++ code generation.

`RpcDual(LocalObject|Wrapper)Tracker` and `RpcDual(LocalObject|Wrapper)Properties` are used to track extra informations per object or per object id. Precisely reuse or add information if necessary.

You are highly possibly need to fix implementation of `RpcDualLifecycleMock` and its connected interfaces if sample fails in either `RuntimeTest` or `CppTest*`.
- The comment in the sample describes how `RpcDualLifecycleMock` and the generated C++ code is supposed to work.
  - The generated C++ code is very straight forward, if it fails, check `RpcDualLifecycleMock` first.
- If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
- Pass all unit test, fix any test failure including pre-existings.
- After finishing everything, git commit and git push to the current branch.
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
