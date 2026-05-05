# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

ollow job.new-sample.md to:
- fix `Rpc\Collection(Dist)?(_Nested)?_*.txt`

Consider the same prefix, each group should contain 4 samples:
- InByref_OutByref:
  - serviceMain should ensure xs is a wrapper
  - clientMain should ensure xs is xsOrigin
- InByref_OutByval
  - serviceMain should ensure xs is a wrapper
  - clientMain should ensure xs is not xsOrigin and xs is not a wrapper
- InByval_OutByref
  - serviceMain should ensure xs is not a wrapper
  - clientMain should ensure xs is a wrapper
- InByval_OutByval
  - serviceMain should ensure xs is not a wrapper
  - clientMain should ensure xs is not xsOrigin and xs is not a wrapper

A few checks were already done but most were not.
Testing if something is or is not a wrapper is very common across samples, check out how others perform the check.

This test is to ensure that:
- when a container is transferred with byref, a wrapper is created.
- when a container is transferred with byval, a copy (non wrapper) is created.
- when byval or byref is applied to a nested container, all levels of container applies.

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

# UPDATES

# TEST

# PROPOSALS