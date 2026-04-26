# Repro

Follow job.new-sample.md to add a new sample: Rpc\Event.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

namespace YourFavoriteNamespace // use RpcEvent
{
	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
    event SomethingHappened(string);

    func MakeItHappen() : void;
    func Watch() : void;
	}
}
var s = "";

func serviceMain(lc : IRpcLifeCycle*) : void
{
	var serviceObj = new (YourFavoriteNamespace::IService^)
	{
    override func MakeItHappen() : void
    {
      SomethingHappened("A");
    }

    override func Watch() : void
    {
      attach(SomethingHappened, [s = $"$(s)[serviceMain:$($1)]"]);
      // or use func (something : string) : void { s = $"$(s)[serviceMain:$(something)]"; }
    }
	};
	lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func clientMain(lc : IRpcLifeCycle*) : string
{
	var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");

  // serviceMain raises an event and clientMain handles it
  var handler = attach(service.SomethingHappened, [s = $"$(s)[serviceMain:$($1)]"]);
  service.MakeItHappen();
  detach(service.SomethingHappened, handler);

  // clientMain raises and event and serviceMain handles it
  service.Watch();
  service.SomethingHappened("B");

  // [clientMain:A][serviceMain:B]
  return s;
}
```

In this task you are going to build and run test cases to verify if these cases are working, according to `TODO_RPC_Definition.md`
This test is to ensure that:
- The sample tests if raising an event works bidirectionaly

Understand what the test case trying to say, you are not allowed to change:
- The content of the sample, unless it doesn't build.
- Workflow parser.
- Workflow compiling.
- Workflow to C++ code generation.

You are highly possibly need to fix:
- `Rpc(B|Unb)oxBy(val|ref)`, as these 4 C++ functions are directly called in generated wrapper classes written Workflow script.
- The wrapper classes generation.
- implementation of `RpcDualLifecycleMock` and its connected interfaces if sample fails in either `RuntimeTest` or `CppTest*`.
- The generated C++ code is very straight forward, if it fails, check `RpcDualLifecycleMock` first.
  - The comment in the sample describes how `RpcDualLifecycleMock` and the generated C++ code is supposed to work.

If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
  - Pass all unit test, fix any test failure including pre-existings.
  - After finishing everything, git commit and git push to the current branch.
  - Two commits are required. First commit only has all modified files and files you created directly, second commit has all new files that not created by you (aka auto generated)
  - DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
