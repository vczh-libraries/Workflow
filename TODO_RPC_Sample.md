# Prompt

Follow job.new-sample.md to add a new Rpc\Overloading.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

namespace YourFavoriteNamespace // use RpcOverloadingTest
{
  @rpc:Interface
  interface IStringRepresentable
  {
    func StringValue : string;
  }

	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
    func ToStringInt(value : int) : string;
    func ToString(value : bool) : string;
    func ToString(value : string) : string;
    func ToString(value : IStringRepresentable^) : string;
    func ToString(value1 : int, value2 : bool, value3 : string, value 4 : IStringRepresentable^) : string;
	}
}

var s = "";

func serviceMain(lc : IRpcLifeCycle*) : void
{
	var serviceObj = new (YourFavoriteNamespace::IService^)
	{
    /*
    Implement those functions by returning either $"$(value)" or value.StringValue.
    For the last function concat them using ",".
    */
	};
	lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func clientMain(lc : IRpcLifeCycle*) : string
{
	var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");
  var value1 = 123;
  var value2 = true;
  var value3 = "abc";
  var value4 = new YourFavoriteNamespace::IStringRepresentable^ { override func GetStringValue() { return "xyz"; } };

  s = $"[$(service.ToStringInt(value1))]";
  s = $"[$(service.ToString(value2))]";
  s = $"[$(service.ToString(value3))]";
  s = $"[$(service.ToString(value4))]";
  s = $"[$(service.ToString(value1, value2, value3, value4))]";
  return s;
}
```

This test varify if the RPC compiler is able to handle overloading.
The RPC method name is by default YourFavoriteNamespace::IService.GetValue.
But in this case we need identical name to differentiate overloading functions, and they become:
- XXX.ToStringInt
- XXX.ToString(value).1
- XXX.ToString(value).2
- XXX.ToString(value).3
- XXX.ToString(value1,value2,value3,value4)
Method id constants will "(),." replaced with "_", e.g., `XXX_ToString_value__1` and `XXX_ToString_value1_value2_value3_value4_`.
You need to handle the case that mangled constant names conflict by reporting an error.
Review `RpcGeneratedNameConflict` and `RpcMangledNameConflict` error for details.
You are able to verify if generated names and mangled names are correct by reading Test\Generated\RpcMetadata(32|64)\Wrapper_Overloading.txt.
Generated names are for string-form ids.
Mangled names are for constant variable names.

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
