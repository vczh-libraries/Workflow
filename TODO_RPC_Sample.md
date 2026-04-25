# Prompt

Follow job.new-sample.md to:
- convert `Rpc\Collection(Dist)?_*.txt` to `Rpc\Collection(Dist)?_Interface_*.txt`
- not including `_Nested` samples, so in total there willbe 8 samples.

New samples perform exactly the same check, but there are two places to modify.

1) `int[]` and `string[int]` will be changed to `IValue^[]` and `IValue^[int]`.
```
@rpc:Interface
interface IValue
{
  int or string Value {const}
}
```

In each sample there will be a `func MakeValue(value : int or string) : IValue^`. All list items or dictionary values will be created using `MakeValue`.

2) Extra tests before `return` statement in `clientMain`.
Because:
- item 4 is added to serviceMain owned xsService.
- item 5 is added to clientMain owned xsClient.
So you need to check every item in xsService and xsClient that:
- Only item 4 in xsService is not a wrapper but all others are.
- Only item 4 in xsClient is a wrapper but all others are not.

This test is to ensure that:
- when a container is transferred with byref, a wrapper is created.
- when a container is transferred with byval, a copy (non wrapper) is created.
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
