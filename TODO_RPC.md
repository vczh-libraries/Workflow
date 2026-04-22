# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.
- References:
  - [Definition](./TODO_RPC_Definition.md)
  - [Scenarios](./TODO_RPC_Scenarios.md)

## ToDo

- Figure out what `decideTypeId` in `RunRpcTestCase` does and see if there is a better way.
  - It is used to call `RegisterLocalObject`, unfortunately the current implementation can't make a local object tells the lifecycle what it type id is.
- Other clean up in RpcDualLifecycleMock.
  - Task: Review all public APIs in `RpcDualLifecycleMock`.
  - Task: Renames wrapperEntries and RpcWrapperEntry to use Properties instead of Entries.
  - Task: `InternalProperty_LocalObjectTracker` and `InternalProperty_WrapperTracker` When it is needed to initialize, verify:
    - For local object, if it is already assigned, always `CHECK_ERROR` to make sure the client id matches, no fallback allowed.
    - For wrapper object, since it is assigned after wrapper creation, always `CHECK_ERROR` to make sure the value exists before using it, no fallback allowed.
- Verify in LocalAndWrapper/ServiceWrapper, case a remote object to IRpcWrapperBase and ensure it is successful (like Rpc\Dtor2.txt)
- Continue to add more test cases until all features are covered.
  - Destructor calling.
  - Register local object in two lifecycles.
  - Function overloading.
  - Byval and byref collections.
    - A function accepts a byval or byref arguments, add one element, return it back as byval or byref arguments.
      - Byval -> byval.
      - Byval -> byref.
      - Byref -> byval.
      - Byref -> byref.
    - remote side will remember the object.
    - local side add another element.
    - check performed in boch local and remote side to see if side effects are visible or invisible as expected.
    - Including array, list, dictionary, observable list.
  - Simpler collection cases using property, testing if property attributes applied to both getter return values and setter arguments.
  - Collections of interfaces.
  - Collections of collections of primitive types.
  - Collections of collections of interfaces.
- C++ codegen for rpc data type JSON serialization.
- Solve interface inheritance and casting.
- Delete Runtime test category.
- Update `TestLibraryRpcByval.cpp` in `LibraryTest` to use `RpcDualLifecycleMock`, remove `RpcByvalLifecycleMock`.
  - Refactor `RpcDualLifecycleMock` to make it a common implementation for `GacUI`.
  - May need to refactor how to retrieve ops interfaces.

# Prompt

Follow job.new-sample.md to add a new Rpc\Dtor2.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

namespace YourFavoriteNamespace // use RpcDtor2Test
{
	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
    func GetServiceAgain() : IService^;
	}
}

var s = "Not Deleted";

func serviceMain(lc : IRpcLifeCycle*) : void
{
	var serviceObj = new (RpcDtorTest::IService^)
	{
    delete { s = "Deleted"; }
    override func GetServiceAgain() : IService^
    {
      var obj = lc.RequestService("YourFavoriteNamespace::IService");
      if ((obj as (IRpcWrapperBase^) is not null)) { throw "IService(obj) should be a local object in serviceMain"; }
      return obj;
    }
	};
	lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func clientMain(lc : IRpcLifeCycle*) : string
{
	var obj = cast (RpcPrimitiveTest::IService^) lc.RequestService("YourFavoriteNamespace::IService");
  if ((obj as (IRpcWrapperBase^) is null)) { throw "IService(obj) should be a wrapper object in clientMain"; }
  var wrapperObj = obj.GetServiceAgain();
  if ((wrapperObj as (IRpcWrapperBase^) is null)) { throw "IService(wrapperObj) should be a wrapper object in clientMain"; }
  wrapperObj = null;
  var m = $"[$(s)]"; // s should be "Not Deleted"
  clientObj = null;  // Releasing the wrapper causing ReleaseRemoteObject and ObjectHold(false) to be called
  var m = $"[$(s)]"; // s should be "Deleted"
  return s;
}
```

Understand what the test case trying to say, you are not allowed to change:
- The content of the sample, unless it doesn't build.
- Workflow parser.
- Workflow compiling.
- Workflow to C++ code generation.

You are highly possibly need to fix implementation of `RpcDualLifecycleMock` and its connected interfaces if sample fails in either `RuntimeTest` or `CppTest*`.
- If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
- Pass all unit test, fix any test failure including pre-existings.
- After finishing everything, git commit and git push to the current branch.
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.

# Prompt

You must complete all following tasks. In order to make the work easier, when performing each task you should:
- Execute the task.
- Pass all unit test, fix any test failure including pre-existings.
- git commit and git push to the current branch.

Perform this per task, one after another, and only after all task is done:
- Run `..\Tools\Tools\Build.ps1 Workflow` for a complete CI.
- Pass all unit test, fix any test failure including pre-existings.
- git commit and git push to the current branch.

Perform each task like a new `# Repro`, which means when a task is done, you can wipe the `Copilot_Investigate.md` for the next task.

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.

## Task 1
