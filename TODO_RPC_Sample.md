# Repro

Follow job.new-sample.md to add new samples, indentation should be double spaces no matter how the code below is written:

## Sample Rpc\EventArgs.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

namespace YourFavoriteNamespace // use RpcEventArgs
{
	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
    event SomethingHappened(int[], observe int[]);

    func MakeItHappen() : void;
    func AddElement() : void;
	}
}

var s = "";

func Print(xs : int[]) : void
{
  for (x in xs)
  {
    s = $"$(s)[$(x)]";
  }
}

func serviceMain(lc : IRpcLifeCycle*) : void
{
	var serviceObj = new (YourFavoriteNamespace::IService^)
	{
    var xs : int[] = {1 2};
    var ys : observe int[] = {3 4};

    func MakeItHappen() : void
    {
      SomethingHappened(xs, ys);
    }

    func AddElement() : void
    {
      xs.Add(5);
      ys.Add(6);
    }
	};
	lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func clientMain(lc : IRpcLifeCycle*) : string
{
	var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");
  var xs : int[] = null;
  var ys : observe int[] = null;
  attach(service.SomethingHappened, func(_xs : int[], _ys : observe int[]) : void
  {
    xs = _xs;
    ys = _ys;
  });
  
  service.MakeItHappen();
  service.AddElement();
  Print(xs);
  Print(ys);

  return s; // [1][2][3][4][6]
}
```

## Goal

In this task you are going to build and run test cases to verify if these cases are working, according to `TODO_RPC_Definition.md`
This test is to ensure that:
- `@rpc:Byval` and `@rpc:Byref` cannot apply to event arguments, but it uses the default options, according to the same rule for function arguments.

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
