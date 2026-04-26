# Repro

## Sample Rpc\DtorList.txt

Follow job.new-sample.md to add a new sample:

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

namespace YourFavoriteNamespace // use RpcDtorList
{
	@rpc:Interface
	interface IValue
	{
	}

	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
    func Set(@rpc:Byref xs : IValue^[]) : void;
    func Clear() : void;
    func Hold() : void;
    func Unhold() : void;
	}
}

var s = "";

func serviceMain(lc : IRpcLifeCycle*) : void
{
	var serviceObj = new (YourFavoriteNamespace::IService^)
	{
    var _List : IValue^[] = null;
    var _Value : IValue^ = null;

    override func Set(xs : IValue^[]) : void
    {
			if ((xs as (system::IRpcWrapperBase^) is null))
			{
				raise "Parameter xs should be a wrapper object in serviceMain";
			}
      _List = xs;
    }

    override func Clear() : void
    {
      _List.Clear();
    }

    override func Hold() : void
    {
      _Value = _List[0];
    }

    override func Unholde() : void
    {
      _Value = null;
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
	};
}

func clientMain(lc : IRpcLifeCycle*) : string
{
	var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");

  {
    var xs : IValue^[] = {};
		s = $"$(s)[a1]";
    service.Set(xs);
    xs.Add(MakeValue());
		s = $"$(s)[a2]";
    service.Clear();
		s = $"$(s)[a3]";
    xs = null;
  }
  
  {
    var xs : IValue^[] = {};
		s = $"$(s)[b1]";
    service.Set(xs);
    xs.Add(MakeValue());
		s = $"$(s)[b2]";
    xs = null;
		s = $"$(s)[b3]";
    service.Clear();
  }
  
  {
		s = $"$(s)[c1]";
    service.Set({MakeValue()});
    service.Hold();
		s = $"$(s)[c2]";
    service.Clear();
		s = $"$(s)[c3]";
    service.Unhold();
  }
  
  {
		s = $"$(s)[d1]";
    service.Set({MakeValue()});
    service.Hold();
		s = $"$(s)[d2]";
    service.Unhold();
		s = $"$(s)[d3]";
    service.Clear();
  }
  
  {
    var x = MakeValue();
		s = $"$(s)[e1]";
    service.Set({x});
    service.Hold();
    service.Clear();
		s = $"$(s)[e2]";
    service.Unhold();
		s = $"$(s)[e3]";
    x = null;
  }
  
  {
    var xs : IValue^[] = {MakeValue()};
		s = $"$(s)[f1]";
    service.Set(xs);
    service.Hold();
    service.Clear();
		s = $"$(s)[f2]";
    service.Unhold();
		s = $"$(s)[f3]";
    xs = null;
  }

  // [a1][a2][a3][IValue][b1][b2][b3][IValue][c1][c2][c3][IValue][d1][d2][d3][IValue][e1][e2][e3][IValue][f1][f2][f3][IValue]
  return s;
}
```

## Sample Rpc\DtorList2.txt

Just like Rpc\DtorList.txt but with the following changes:

In `IService` from `func Set(@rpc:Byref xs : IValue^[]) : void;` to `@rpc:Byref func Make() : IValue^[];`
therefore the implementation becomes
```Workflow
override func Make() : IValue^[]
{
  _List = {};
  return _List;
}
```

In `clientMain`, copy the first 5 sections and change:

Section 1 and 2 from
```Workflow
var xs : IValue^[] = {};
s = $"$(s)[...]";
service.Set(xs);
```
To
```Workflow
var xs : IValue^[] = null;
s = $"$(s)[...]";
xs = service.Make();
```

Section 3 and 4 from
```Workflow
s = $"$(s)[...]";
service.Set({MakeValue()});
```
To
```Workflow
s = $"$(s)[...]";
service.Make().Add(MakeValue());
```

Section 5 from
```Workflow
var x = MakeValue();
s = $"$(s)[...]";
service.Set({x});
```
To
```Workflow
var x = MakeValue();
s = $"$(s)[...]";
service.Make().Add(x);
```

## Goal

In this task you are going to build and run test cases to verify if these cases are working, according to `TODO_RPC_Definition.md`
This test is to ensure that:
- Elements in containers are correctly deleted at the right timing. (DtorList.txt)
- When elements and containers are created at different side, it still function. (DtorList2.txt)

## Restriction

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
