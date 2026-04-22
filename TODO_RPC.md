# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.
- References:
  - [Definition](./TODO_RPC_Definition.md)
  - [Scenarios](./TODO_RPC_Scenarios.md)

## ToDo

- Figure out what `decideTypeId` in `RunRpcTestCase` does and see if there is a better way.
  - It is used to call `RegisterLocalObject`, unfortunately the current implementation can't make a local object tells the lifecycle what it type id is.
- `RpcLfiecycle` registered in reflection, because `IRpcLifecycle::GetController` returning itself causes problem in `RuntimeTest`
  - Without reflection registration, the metadata doesn't say it implements two interfaces at the same time.
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

namespace YourFavoriteNamespace // use RpcDtor3Test
{
  @rpc:Interface
  interface IContainer { prop Value : IValue^ {const} }

  @rpc:Interface
  interface IValue{}

	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
    func ContainValue(value : IValue^) : IContainer^;
	}
}

var s = "";

func serviceMain(lc : IRpcLifeCycle*) : void
{
	var serviceObj = new (YourFavoriteNamespace::IService^)
	{
    override func ContainValue(value : IValue^) : IContainer^
    {
      if ((value as (IRpcWrapperBase^) is null)) { throw "IValue(value) should be a local object in serviceMain"; }
      return new (YourFavoriteNamespace::IContainer^)
      {
        delete
        {
          s = $"$(s)[IContainer]";
        }

        override func GetValue() : IValue^
        {
          return value;
        }
      };
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
  }
}

func clientMain(lc : IRpcLifeCycle*) : string
{
	var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");
  if ((oservicebj as (IRpcWrapperBase^) is null)) { throw "IService(service) should be a wrapper object in clientMain"; }
  {
    s = $"$(s)[1]";
    var container = service.ContaineValue(MakeValue());
    if ((container as (IRpcWrapperBase^) is null)) { throw "IContainer(container) should be a wrapper object in clientMain"; }
    if ((container.Value as (IRpcWrapperBase^) is not null)) { throw "IValue(container.Value) should be a local object in clientMain"; }
    container = null;
    /*
      At the moment, container wrapper is deleted.
      ObjectHold will be called from client to service to free the real container.
      The value wrapper in the real container will be deleted.
      Object hold will be called from service to client to free the real value.
      Destructor of IValue will be called after destructor of IContainer.
    */
    s = $"$(s)[2]";
  }
  {
    var container = service.ContaineValue(MakeValue());
    var value = container.Value;
    if ((container as (IRpcWrapperBase^) is null)) { throw "IContainer(container) should be a wrapper object in clientMain"; }
    if ((value as (IRpcWrapperBase^) is not null)) { throw "IValue(value) should be a local object in clientMain"; }
    container = null;
    /*
      At the moment, container wrapper is deleted.
      ObjectHold will be called from client to service to free the real container.
      The value wrapper in the real container will be deleted.
      Object hold will be called from service to client to free the real value.
      but the real value object is still hold at client side.
    */
    s = $"$(s)[3]";
    var = null;
    /*
    The real value object is supposed to be deleted now.
    Because the client side lifecycle is no longer holding it.
    */
    s = $"$(s)[4]";
  }
  return s; // [1][IValue][IContainer][2][IContainer][3][IValue][4]
}
```

Understand what the test case trying to say, you are not allowed to change:
- The content of the sample, unless it doesn't build.
- Workflow parser.
- Workflow compiling.
- Workflow to C++ code generation.

You are highly possibly need to fix implementation of `RpcDualLifecycleMock` and its connected interfaces if sample fails in either `RuntimeTest` or `CppTest*`.
- The comment in the sample describes how `RpcDualLifecycleMock` and the generated C++ code is supposed to work.
  - The generated C++ code is very straight forward, if it fails, check `RpcDualLifecycleMock` first.
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
