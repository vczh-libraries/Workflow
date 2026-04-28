# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

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

# UPDATES

# TEST

Added `Rpc\EventArgs.txt` and registered it in `Test\Resources\IndexRpc.txt`.

The sample raises an RPC event with two collection arguments:

- `int[]` should use the default by-value behavior, matching function arguments. After the event is delivered, the service mutates its original list with `xs.Add(5)`, and the client-side captured `xs` should remain `[1][2]`.
- `observe int[]` should use the default by-reference behavior, matching function arguments. After the event is delivered, the service mutates its original observable list with `ys.Add(6)`, and the client-side captured `ys` should reflect `[3][4][6]`.

The success criteria is that `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` run the sample and return:

```text
[1][2][3][4][6]
```

This confirms event arguments use default RPC collection transfer options because Workflow has no syntax for applying `@rpc:Byval` or `@rpc:Byref` to event arguments directly.

Status: CONFIRMED.

Initial result before the generator fix:

- `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` all returned `[1][2][3][4]` for `EventArgs`.
- Generated wrappers boxed and unboxed both event arguments with `RpcBoxByval` and `RpcUnboxByval`.

This showed the sample was valid and exposed a wrapper-generation issue for event argument transfer defaults.

# PROPOSALS

- No.1 Add RPC event argument default-option sample
- No.2 Fix generated wrapper transfer for RPC event arguments

## No.1 Add RPC event argument default-option sample

### CODE CHANGE

Added `Test/Resources/Rpc/EventArgs.txt` with a service event:

- First argument: `int[]`, expected default by-value behavior.
- Second argument: `observe int[]`, expected default by-reference behavior.

Updated `Test/Resources/IndexRpc.txt`:

```text
EventArgs=[1][2][3][4][6]
```

Status: CONFIRMED.

## No.2 Fix generated wrapper transfer for RPC event arguments

### FINDING

`WfAnalyzer_GenerateRpc.cpp` previously forced all generated event wrapper arguments through by-value boxing and unboxing. Event arguments cannot carry `@rpc:Byval` or `@rpc:Byref` attributes, so the generator must compute the default transfer mode from the reflected event argument type.

While implementing this, the reflected event handler generic type needed one extra offset: generic argument 0 is the event sender, and `WfEventDeclaration::arguments` starts at the first user-declared event payload argument.

### CODE CHANGE

Updated `Source/Analyzer/WfAnalyzer_GenerateRpc.cpp` to:

- Detect strong typed RPC collection types from `ITypeInfo`.
- Apply the default rule used by RPC definitions: observable collections and collections containing RPC interfaces are by reference; other strong collections are by value.
- Store the computed event argument transfer mode in `RpcParamModel::byref`.
- Generate `RpcBoxByref`/`RpcUnboxByref` for by-reference event arguments and keep `RpcBoxByval`/`RpcUnboxByval` for by-value event arguments.

Generated `EventArgs.cpp` now contains:

```text
arg0: RpcBoxByval / RpcUnboxByval
arg1: RpcBoxByref / RpcUnboxByref
```

Status: CONFIRMED.

# VERIFICATION

Confirmed all of the following passed:

- `copilotBuild.ps1 -Configuration Debug -Platform x64`
- `copilotBuild.ps1 -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable LibraryTest -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_GenerateMetadata -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_LoadAndCompile -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable RuntimeTest -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Metaonly -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Reflection -Configuration Debug -Platform x64`
- `copilotExecute.ps1 -Mode UnitTest -Executable LibraryTest -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_GenerateMetadata -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_LoadAndCompile -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable RuntimeTest -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Metaonly -Configuration Debug -Platform Win32`
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Reflection -Configuration Debug -Platform Win32`
- `C:\Code\VczhLibraries\Tools\Tools\Build.ps1 Workflow`

Status: CONFIRMED.
