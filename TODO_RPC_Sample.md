# Repro

Follow job.new-sample.md and add 24 new samples:
- Copy `Rpc\Collection*_Default.txt` to `Rpc\Collection*_PropDefault.txt`
- Copy `Rpc\Collection*_InByval_OutByval.txt` to `Rpc\Collection*_PropByval.txt`
- Copy `Rpc\Collection*_InByref_OutByref.txt` to `Rpc\Collection*_PropByref.txt`
- Remember to change the namespaces for interfaces, because types could not conflict across samples.

## Extra Modification

These new samples are for testing collection properties in interfaces. So extra modification should apply during copying.

In `IService` change `func DoList(xs : T) : T` to `prop T List {not observe}`.

The original `IService::DoList` implementation looks like:
```
override func DoList(xs : T) : T
{
  ANYCODE(xs);
	return xs;
}
```

Now it becomes
```
var _List : T = null;

override func GetList() : T
{
  return _List;
}

override func SetList(xs : T) : void
{
  ANYCODE(xs); // should be just simple copy
  _List = xs;
}
```

In `clientMain` change `var xs = service.DoList(xsOrigin);` to
```
service.List = xsOrigin;
var xs = service.List;
```

Keep all others exactly identical. According to `TODO_RPC_Definition.md`, following the above rules should just pass all unit test projects. This is the purpose of these tests.

## General Instruction

This test is to ensure that:
- `@rpc:Byref` or `@rpc:Byval` attributes on properties, including absense, will be transferred to getter's return value and setter's parameter, if the property type is a collection. Property in interfaces are implemented by a pair of override functions.
- when a container is transferred with byref, a wrapper is created.
- when a container is transferred with byval, a copy (non wrapper) is created.
- when byval or byref is not explicitly written, the correct default option will be selected and applied to these containers. It depends on the actual type.
- when byval or byref is applied to a nested container, all levels of container applies.
- when a container contain interfaces, these interfaces are passed between lifecycles as expected, just like normal interface value.

Processing containers are a little bit more complex comparing to interfaces.
When byref is specified, an wrapper will be created to connect to the original container.
When byval is specified, a copy will be created.
Therefore according to Byref or Byval, the object retrieved from lifecycle may be an IRpcWrapperBase^ or may not, unlike interfaces all remote objects are wrappers.

Understand what the test case trying to say, you are not allowed to change:
- The content of the sample, unless it doesn't build.
- Workflow parser.
- Workflow compiling.
- Workflow to C++ code generation.

`Rpc(B|Unb)oxBy(val|ref)` should be in the highest priority attemp to fix, as these 4 C++ functions are directly called in generated wrapper classes written Workflow script.

You are highly possibly need to fix implementation of `RpcDualLifecycleMock` and its connected interfaces if sample fails in either `RuntimeTest` or `CppTest*`.
- The comment in the sample describes how `RpcDualLifecycleMock` and the generated C++ code is supposed to work.
  - The generated C++ code is very straight forward, if it fails, check `RpcDualLifecycleMock` first.
- If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
- Pass all unit test, fix any test failure including pre-existings.
- After finishing everything, git commit and git push to the current branch.
  - Two commits are required. First commit only has all modified files and files you created directly, second commit has all new files that not created by you (aka auto generated)
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
