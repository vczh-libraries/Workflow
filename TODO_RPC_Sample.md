# Repro

Follow job.new-sample.md to add new samples, indentation should be double spaces no matter how the code below is written:

## Sample Rpc\PropCached.txt

Also copy `PropDefault.txt` but removing `@rpc:Cached`, so that we can verify omitting means `@rpc:Cached` for properties.

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

namespace YourFavoriteNamespace // use RpcPropCached or RpcPropDefault
{
	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
    @rpc:Cached // no such line in PropDefault.txt
    prop Value : string {const}

    func SetValue(value : string) : void;
    func Signal() : void;
	}
}

var s = "";

func serviceMain(lc : IRpcLifeCycle*) : void
{
	var serviceObj = new (YourFavoriteNamespace::IService^)
	{
    var _Value : string = "A";

    override func GetValue() : string
    {
      return _Value;
    }

    override func SetValue(value : string) : void
    {
      _Value = value;
    }

    override func Signal() : void
    {
      ValueChanged();
    }
	};
	lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func clientMain(lc : IRpcLifeCycle*) : string
{
	var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");
  attach(service.ValueChanged, [s = $"$(s)[ValueChanged]]);

  // call GetValue the first time triggers the read
  s = $"$(s)[$(service.Value)];

  // call SetValue, but the event is not signaled, so @rpc:Cached properties updating on wrapper side is not triggered
  service.SetValue("B");
  s = $"$(s)[$(service.Value)];

  // trigger the event
  service.Siangl();
  s = $"$(s)[$(service.Value)];

  return s; // [A][A][ValueChanged][B]
}
```

## Sample Rpc\PropDynamic.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

namespace YourFavoriteNamespace // use RpcPropDynamic
{
	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
    @rpc:Dynamic
    prop Value : string {const}

    func SetValue(value : string) : void;
    func Signal() : void;
	}
}

var s = "";

func serviceMain(lc : IRpcLifeCycle*) : void
{
	var serviceObj = new (YourFavoriteNamespace::IService^)
	{
    var _Value : string = "A";

    override func GetValue() : string
    {
      return _Value;
    }

    override func SetValue(value : string) : void
    {
      _Value = value;
    }

    override func Signal() : void
    {
      ValueChanged();
    }
	};
	lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func clientMain(lc : IRpcLifeCycle*) : string
{
	var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");
  attach(service.ValueChanged, [s = $"$(s)[ValueChanged]]);

  // call GetValue the first time triggers the read
  s = $"$(s)[$(service.Value)];

  // call SetValue, no matter ValueChanged triggered or not, @rpc:Dynamic always perform reading on GetValue
  service.SetValue("B");
  s = $"$(s)[$(service.Value)];

  // trigger the event
  service.Siangl();
  s = $"$(s)[$(service.Value)];

  return s; // [A][B][ValueChanged][B]
}
```

## Goal

In this task you are going to build and run test cases to verify if these cases are working, according to `TODO_RPC_Definition.md`
This test is to ensure that:
- @rpc:Cached properties cache their value on the first reading, and the cache will be invalidate when the associated changed event triggered.

## Implementation

For auto property, without `{... not observe}` the event will always be `PropertyNameChanged`. For trival property, the event name is specified in `{... : EventName}`.
Follow the WfAssembly generation and C++ reflection code generation to figure out how such information should be correctly read.
It is acceptable when @rpc:Cached decorates an property without associated event. In this case there will be no refreshing you can do, it is fine. Extra mechanism will be offered in the future. The expected behavior of such property is to, only call the getter when the property is first read, and the value in the wrapper remain unchanged forever.
In the generated wrapper, you can generated `PropertyName<Cached> : T` and `PropertyName<Available> : bool` pair of variables for caching.
When the getter is called, only when it is unavailable, the remote caller will be called and the cached value will be refreshed, and then it becomes available.
The wrapper should also attach to the property changed event (if it exists), and set it to unavailable when the event is triggered.

## Verifying Samples

Workflow script syntax and semantic should be intuitive.
During reading the sample, you should verify it with the goal of the task.
Ensure all logs or exceptions in the sample accurately reflected the intention of the design.
Ensure the expected result would be what users would expect.

## Restriction

Understand what the test case trying to say, you are not allowed to change:
- The content of the verified sample, unless it doesn't build.
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
    - Typical files that are generated: `Test\Generated`, `Test\SourceCppGen`, `Test\SourceCppGenRpc`, `Test\UnitTest\Generated_*`.
  - DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
