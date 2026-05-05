# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Follow job.new-sample.md to add new samples, indentation should be double spaces no matter how the code below is written:

## Sample Rpc\DtorPropCached.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

namespace YourFavoriteNamespace // use RpcDtorPropCached
{
  @rpc:Interface
  interface IValue
  {
  }

	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
    @rpc:Cached
    prop Value : IValue^ {const}

    func SetValue(value : IValue^) : void;
    func Signal() : void;
	}
}

var s = "";

func MakeValue(value : string) : IValue^
{
  return new IValue^
  {
    delete
    {
      s = $"$(s)[Deleted:$(value)]";
    }
  }
}

func serviceMain(lc : IRpcLifeCycle*) : void
{
	var serviceObj = new (YourFavoriteNamespace::IService^)
	{
    var _Value : IValue^ = MakeValue("A");

    override func GetValue() : IValue^
    {
      return _Value;
    }

    override func SetValue(value : IValue^) : void
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

  // No wrapper in the service wrapper, calling SetValue would cause the A value to be destroyed immediately
  s = $"$(s)[1];
  service.SetValue(MakeValue("B"));

  // call GetValue the first time triggers the read, storing B value in the service wrapper
  s = $"$(s)[2];
  var v = service.Value;

  // call SetValue, but the event is not signaled, so @rpc:Cached properties updating is not visible in the service wrapper
  s = $"$(s)[3];
  service.SetValue(MakeValue("C"));
  if (v != service.Value)
  {
    raise "IService::Value in clientMain should not change before calling IService::Signal";
  }

  // trigger the event, the cached property value in the service wrapper is marked inavailable, but the B value is still there
  s = $"$(s)[4];
  service.Signal();

  // Read the property again, the B value in the service wrapper will be replaced
  // No outside variable to catch the C value, but it is still in the service wrapper
  // A value is in variable v
  s = $"$(s)[5];
  service.Value;

  // Reset v causing B value to destroy
  s = $"$(s)[6];
  v = null;

  // call SetValue again
  s = $"$(s)[7];
  service.SetValue(MakeValue("D"));

  // trigger the event
  s = $"$(s)[8];
  service.Signal();

  // Read the property again, the C value in the service wrapper will be replaced
  s = $"$(s)[9];
  service.Value;

  return s; // [1][Deleted:A][2][3][4][5][6][Delete:B][7][8][9][Deleted:C]
}
```

## Sample Rpc\DtorPropCachedListByval.txt

The same to `Rpc\DtorPropCached.txt` but change the:
- property type to `IValue^[]`, initialized with A in the list, and later replaced each time, with a new list containing a new value.
- use `@rpc:Byval` on property.
- variable v in clientMain becomes IValue^[] too.
- the output should remain. even when the property container is copied, but the IValue still a reference.

## Sample Rpc\DtorPropCachedListVByref.txt

The same to `Rpc\DtorPropCached.txt` but change the:
- property type to `IValue^[]`, initialized with A in the list, and later replaced each time, with a new list containing a new value.
- use `@rpc:Byref` on property.
- variable v in clientMain becomes IValue^[] too.
- the output should remain. the property container is wrapperd, it should affect the lifecycle of its element which is a IValue reference.

## Goal

In this task you are going to build and run test cases to verify if these cases are working, according to `TODO_RPC_Definition.md`
This test is to ensure that:
- @rpc:Cached properties cache their value on the first reading, and the cache will be invalidate when the associated changed event triggered.

If the current implemention is correct, the added samples should just pass the test.

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

# UPDATES

No additional updates.

# TEST [CONFIRMED]

Add three RPC samples:

- `Rpc\DtorPropCached.txt`: scalar `@rpc:Cached` interface reference property.
- `Rpc\DtorPropCachedListByval.txt`: `@rpc:Cached @rpc:Byval` list property whose element is an interface reference.
- `Rpc\DtorPropCachedListVByref.txt`: `@rpc:Cached @rpc:Byref` list property whose element is an interface reference.

The success criteria is that all three samples produce:

```text
[1][Deleted:A][2][3][4][ValueChanged][5][6][Deleted:B][7][8][ValueChanged][9][Deleted:C]
```

This confirms the first property read caches the value, changing the server-side value without signaling keeps the cached client value visible, and signaling `ValueChanged` invalidates the cache so the next read replaces the cached wrapper and releases the previous referenced value at the expected time. The list variants must keep the same destructor order even when the list container is copied or wrapped, because the contained `IValue^` is still a reference.

The test is confirmed by Debug Win32/x64 `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`, plus the final Release `Build.ps1 Workflow` run.

# PROPOSALS

- No.1 Add cached RPC destructor samples [CONFIRMED]

## No.1 Add cached RPC destructor samples

### CODE CHANGE

Added the three requested RPC sample files under `Test\Resources\Rpc` and registered them in `Test\Resources\IndexRpc.txt` with the confirmed output including the two `ValueChanged` event deliveries. The sample syntax keeps two-space indentation and uses block comments plus explicit event handler functions so the existing Workflow parser accepts the files.

Generated RPC workflow metadata and C++ outputs were refreshed for Win32 and x64. `Test\UnitTest\Generated_CppRpc\Generated_CppRpc.vcxitems` and `Test\UnitTest\Generated_ReflectionRpc\Generated_ReflectionRpc.vcxitems` were updated to include the generated sample sources so the generated C++ test projects link successfully.

No production runtime, parser, compiler, or code generator change was required.

### CONFIRMED

All three new samples pass in the runtime interpreter path and in generated C++ paths. `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` all report the expected destructor log for the scalar cached property and both list cached-property variants, proving the existing cached-property invalidation and reference lifetime behavior is already correct for these cases.
