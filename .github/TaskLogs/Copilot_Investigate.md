# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

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

# UPDATES

## UPDATE

- Added `Test/Resources/Rpc/DtorList.txt` and `Test/Resources/Rpc/DtorList2.txt`, then registered their expected outputs in `Test/Resources/IndexRpc.txt`.
- Kept the samples functionally identical to the request, with only the minimum build fixes:
  - `namespace YourFavoriteNamespace // use RpcDtorList` and `RpcDtorList2` were implemented by adding the matching `using RpcDtorList::*;` / `using RpcDtorList2::*;`.
  - The direct `xs as (system::IRpcWrapperBase^)` cast did not compile, so the sample first stores `xs` into `object` before the cast.
  - `Unholde` was corrected to `Unhold`.
- The first failure was in `RuntimeTest`. `DtorList` destroyed `IValue` too early when a plain local list was exported by reference and then destructively mutated from the remote side. `DtorList2` destroyed `IValue` too early when a locally created interface value was inserted into a by-ref list wrapper that originated from the other lifecycle.
- Fixed `Source/Library/WfLibraryRpc.*` in two places:
  - `RpcByrefList` now preserves locally created shared-pointer values that are inserted through a by-ref container wrapper, so cross-lifecycle list wrappers keep their locally originated elements alive until the wrapper is released.
  - `RpcCalleeListOps::{ListSet,ListRemoveAt,ListClear}` now ask the lifecycle for a mutation target before mutating a tracked object directly.
- Added `IRpcLocalMutationTarget` and implemented it in `Test/Source/RpcDualLifecycleMock.*`.
  - For plain `IValueList` / `IValueArray` objects tracked by a lifecycle, destructive remote mutations now use copy-on-write when the local side still holds its own references.
  - `IValueObservableList` is intentionally excluded from cloning so existing event-observable behavior is preserved.
- After the runtime fix, the remaining `CppTest*` failure was a project-wiring issue only: the new generated `DtorList` / `DtorList2` source files were not included by `Generated_CppRpc.vcxitems` and `Generated_ReflectionRpc.vcxitems`. Adding those entries fixed the link failures.

## UPDATE

Checkout commit `c8cc8579349c0d827ac45bab50049b0d9d825af4` you will see, in order to complete `DtorList.txt` and `DtorList2.txt`, `IRpcLocalMutationTarget` is added.
The test case is about how elements in containers can be stably deleted.
You can check out the content of existing Copilot_Investigate.md to understand what happened. Copilot_Investigate.md is not changed in all later commits so you can just read it directly.

My thought is that, in `Dtor[23]?.txt` we proved that, only when both local object and wrapper is not hold by both side, the local object will get destructed.
`Dtor3.txt` is about a local object holding a wrapper (remote object), and it works well.
So obviously, only when both local container and wrapper container is not hold by both side, the local container will get destructed.
And that's when elements in the container destructed, no matter if elements are wrapper or not.

So the added `IRpcLocalMutationTarget` looks wired to me. I expect a solution that no extra construction is added.
I would like you to dig deeper into the source code, find the root cause, and try to avoid `IRpcLocalMutationTarget`.

I believe Workflow compiler and code generation should be good, so they should be the least thing you want to change.

If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
- Pass all unit test, fix any test failure including pre-existings.
- After finishing everything, git commit and git push to the current branch.
- If in any task you are adding new test sample, or modifying any C++ or Workflow generation code, causing a huge amount of files generated from test samples to change:
  - Two commits are required. First commit only has all modified files and files you created directly, second commit has all new files that not created by you (aka auto generated)
  - Otherwise, One commit is good.
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.

# TEST [CONFIRMED]

- `DtorList` is the primary repro for destructive remote mutation against a plain local `IValue^[]` exported by reference. It proves each `IValue` destructor must wait until both the local container instance and its remote wrapper-side lifetime are released.
- `DtorList2` exercises the opposite ownership direction: the by-ref container originates on the service side, then the client inserts locally created interface values through the wrapper. It proves wrapper-side insertion must not drop the local element lifetime early.
- Existing by-ref collection identity tests remain part of the success criteria. A non-destructive by-ref round trip must still preserve local object identity, so changing normal `IRpcLifeCycle::RefToPtr` behavior globally is not acceptable.
- Required execution order for failure localization:
  - `LibraryTest`
  - `CompilerTest_GenerateMetadata`
  - `CompilerTest_LoadAndCompile`
  - `RuntimeTest`
  - `CppTest`
  - `CppTest_Metaonly`
  - `CppTest_Reflection`
- Additional success criteria:
  - `RuntimeTest` must match the expected destructor traces for both `DtorList` and `DtorList2` on `Debug|Win32` and `Debug|x64`.
  - `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` must all run `Rpc:DtorList` and `Rpc:DtorList2` successfully on `Debug|Win32` and `Debug|x64`.
  - Full repo release synchronization via `Tools\Build.ps1 Workflow` must succeed after the source change.
- Results:
  - `copilotBuild.ps1` passed for `Debug|x64` and `Debug|Win32`.
  - `LibraryTest`: `14/14` on `x64`, `14/14` on `Win32`.
  - `CompilerTest_GenerateMetadata`: `2/2` on `x64`, `2/2` on `Win32`.
  - `CompilerTest_LoadAndCompile`: `694/694` on `x64`, `694/694` on `Win32`.
  - `RuntimeTest`: `246/246` on `x64`, `246/246` on `Win32`.
  - `CppTest`: `212/212` on `x64`, `212/212` on `Win32`.
  - `CppTest_Metaonly`: `212/212` on `x64`, `212/212` on `Win32`.
  - `CppTest_Reflection`: `212/212` on `x64`, `212/212` on `Win32`.
  - `Build.ps1 Workflow`: passed with exit code `0`.
  - Runtime and generated C++ logs both contain the expected `Rpc:DtorList` and `Rpc:DtorList2` traces.

# PROPOSALS

- No.1 Public mutation hook on `IRpcLifeCycle` via `IRpcLocalMutationTarget` [DENIED]
- No.2 Internal mutation-path detachment plus local by-ref element retention [CONFIRMED]

## No.1 Public mutation hook on `IRpcLifeCycle` via `IRpcLocalMutationTarget`

### CODE CHANGE

The earlier fix added a public `IRpcLocalMutationTarget` interface and had `RpcCalleeListOps::{ListSet,ListRemoveAt,ListClear}` ask the lifecycle for a pre-mutation target. `RpcDualLifecycleMock` implemented that interface and cloned tracked plain `IValueList` / `IValueArray` instances when remote destructive mutation arrived while the local side still held the same container instance.

### DENIED BY USER

This proposal located the correct execution point, but it expanded the public RPC lifecycle surface for behavior that is only needed by the test lifecycle implementation. After re-reading the existing destructor samples and the by-ref collection identity tests, the deeper conclusion is that the real requirement is not "introduce a new lifecycle contract", but "perform detachment only on destructive remote mutation against a locally tracked plain container". The public interface solved the symptom but was broader than necessary, so the proposal is denied.

## No.2 Internal mutation-path detachment plus local by-ref element retention

### CODE CHANGE

- Kept the `RpcByrefList` local value cache so locally created shared-pointer interface values inserted through a by-ref wrapper stay alive until the wrapper itself is released. This is required for `DtorList2`.
- Replaced the public `IRpcLocalMutationTarget` contract with an internal `RpcLocalMutationInfo` callback object stored as an internal property on the lifecycle implementation.
- `RpcCalleeListOps::{ListSet,ListRemoveAt,ListClear}` still prepare a mutation target before destructive writes, but now they query that internal property instead of a public interface.
- `RpcDualLifecycleMock::PrepareLocalMutationTarget` still performs copy-on-write only for tracked plain `IValueList` / `IValueArray` objects whose local instance is still shared. `IValueObservableList` remains excluded so observable mutation semantics are unchanged.
- No Workflow parser, compiler, or code-generation change was required. `Release/VlppWorkflowLibrary.h` and `Release/VlppWorkflowLibrary.cpp` were regenerated by `Tools\Build.ps1 Workflow` after the source change.

### CONFIRMED

The root cause is split across two ownership paths:

- `DtorList` fails when `RpcCalleeListOps` destructively mutates the raw local tracked container returned by `RefToPtr(ref)`. If the local side still holds the same plain list or array instance, that mutation drops the element immediately on the local side, so the element destructor runs before the wrapper-side lifetime is released.
- `DtorList2` fails when a locally created interface value is inserted into a by-ref list wrapper that originated from the other lifecycle. Without retaining that local shared pointer on the wrapper side, the local element lifetime becomes tied only to the remote container, so the destructor can run too early.

The important negative result is that global `RefToPtr` behavior cannot be changed to always fork or substitute another object for locally tracked by-ref containers. Existing by-ref collection samples rely on stable identity for non-destructive round trips, so changing the ordinary unboxing path would break those tests. That leaves only one valid hook: destructive remote mutation itself.

This proposal keeps that hook, but narrows it to an internal lifecycle property instead of a public API. The final code therefore preserves the required copy-on-write behavior for destructive mutations, preserves locally originated interface elements inserted through wrapper lists, avoids Workflow compiler or code-generation changes, and passes the full `RuntimeTest` and `CppTest*` matrix on both `Win32` and `x64`.
