# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.
- References:
  - [Definition](./TODO_RPC_Definition.md)
  - [Scenarios](./TODO_RPC_Scenarios.md)

## ToDo

- Continue to add more test cases until all features are covered.
  - Local objects and remote objects.
    - Local objects pass to remote and pass back, remote side gets a wrapper, expect the same local object instance.
    - Remote objects pass to remote and pass back, remote side gets a local object, expect the same wrapper instance.
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

- Execute the task.
- Pass all unit test, fix any test failure including pre-existings.
- git commit and git push to the current branch.
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.

Follow job.new-sample.md to add a new Rpc\LocalAndWrapper.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

namespace RpcWrapperTest
{
  @rpc:Interface
  interface IObj1{}

  @rpc:Interface
  interface IObj2{}

	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
    func Exchange1(o : IObj1^) : IObj2^;
    func Exchange1(o : IObj2^) : IObj1^;
	}
}

var clientObj1 : IObj1^ = new IObj1^{}; // clientObj1 is a client side object
var serviceObj2 : IObj2^ = new IObj2^{}; // serviceObj2 is a service side object

var serviceReceived_Obj1 : IObj1^ = null;
var serviceReceived_Obj2 : IObj2^ = null;
var clientReceived_Obj1 : IObj1^ = null;
var clientReceived_Obj2 : IObj2^ = null;

func serviceMain(lc : IRpcLifeCycle*) : void
{
	var service = new (YourFavoriteNamespace::IService^)
	{
    override func Exchange1(o : IObj1^) : IObj2^
    {
      // serviceReceived_Obj1 should be a wrapper.
      // client sents clientObj1 and service receives it.
      // this is a remote object for service, IRpcLifeCycle will wrap it.
      serviceReceived_Obj1 = o;
      return serviceObj2;
    }

    override func Exchange2(o : IObj2^) : IObj1^
    {
      // serviceReceived_Obj2 should be real.
      // client sents clientReceived_Obj2 to and service receives it.
      // this is a local object for service, IRpcLifeCycle will recognize it and grab the stored local object.
      serviceReceived_Obj2 = o;
      return serviceReceived_Obj1;
    }
	};
	lc.RegisterService("YourFavoriteNamespace::IService", service);
}

func clientMain(lc : IRpcLifeCycle*) : string
{
	var obj = lc.RequestService("YourFavoriteNamespace::IService");
	var service = cast (YourFavoriteNamespace::IService^) obj;

  // clientReceived_Obj2 should be a wrapper and clientReceived_Obj1 should be real.
  // this is a reversed operation.
  clientReceived_Obj2 = service.Exchange1(clientObj1);
  clientReceived_Obj1 = service.Exchange2(clientReceived_Obj1);

	return
    $"[$(serviceReceived_Obj1 == clientObj1)]" &  // false
    $"[$(serviceReceived_Obj2 == serviceObj2)]" & // true
    $"[$(clientReceived_Obj1 == clientObj1)]" &   // true
    $"[$(clientReceived_Obj2 == serviceObj2)]";   // false
}
```

Understand what the test case trying to say, you are not allowed to change:
- The content of the sample, unless it doesn't build.
- Workflow parser.
- Workflow compiling.
- Workflow to C++ code generation.

You are highly possibly need to fix implementation of `IRpcLifecycle` and its connected interfaces if sample fails in either `RuntimeTest` or `CppTest*`.

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

