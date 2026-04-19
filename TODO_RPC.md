# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.
- References:
  - [Definition](./TODO_RPC_Definition.md)
  - [Scenarios](./TODO_RPC_Scenarios.md)

## ToDo

- Figure out what `decideTypeId` in `RunRpcTestCase` does and see if there is a better way.
- Continue to add more test cases until all features are covered.
  - Destructor calling.
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

# Prompt

Follow job.new-sample.md to add a new Rpc\ServiceWrapper.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

namespace RpcWrapperTest
{
	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
    func Self(obj : IService^): bool;
	}
}

var serviceObj : IService^ = null;

func serviceMain(lc : IRpcLifeCycle*) : void
{
	serviceObj = new (YourFavoriteNamespace::IService^)
	{
    override func Self(obj : IService^): bool
    {
      return obj == serviceObj;
    }
	};
	lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func clientMain(lc : IRpcLifeCycle*) : string
{
	var obj = lc.RequestService("YourFavoriteNamespace::IService");
	var clientObj = cast (YourFavoriteNamespace::IService^) obj;

  // serviceObj is stored by serviceMain so it is the real object.
  // clientObj should be its wrapper because they live in different lifecycle space.
  // when clientObj sents back from clientMain to serviceMain it should get the real object back.
  // because an object should be a wrapper if and only if it is a remote object.
	return $"[$(clientObj == serviceObj)][$(obj.Self(clientObj))]"; // false and true
}
```

Understand what the test case trying to say, you are not allowed to change:
- The content of the sample, unless it doesn't build.
- Workflow parser.
- Workflow compiling.
- Workflow to C++ code generation.

You are also required to update the generation for TestCasesRpc.cpp. All test cases are almost the same except one line:
`auto& instance = ::vl_workflow_global::RequestService::Instance();`
- Verify if my statement is correct, or if not, what else is different?
- Make a template function so that you can just call `RunRpcTestCase<::vl_workflow_global::RequestService>` instead of just duplicating code in test cases.

You are highly possibly need to fix implementation of `IRpcLifecycle` and its connected interfaces if sample fails in either `RuntimeTest` or `CppTest*`.
- If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
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
