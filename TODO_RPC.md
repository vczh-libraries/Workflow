# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.

## Investigation

- Figure out what `decideTypeId` in `RunRpcTestCase` does and see if there is a better way.
  - It is used to call `RegisterLocalObject`, unfortunately the current implementation can't make a local object tells the lifecycle what it type id is.
- `RpcLfiecycle` registered in reflection, because `IRpcLifecycle::GetController` returning itself causes problem in `RuntimeTest`
  - Without reflection registration, the metadata doesn't say it implements two interfaces at the same time.
- Verify `WfErrors::Cpp*`, probably not needed as errors should have been catched in eariler phases.
- `WfCpp_WriteReflection.cpp` is changed because `INVOKE(GET?)_INTERFACE_PROXY` does not handle overloading property. This should be a VlppParser bug. Fix the bug and revert this file.
  - https://github.com/vczh-libraries/Workflow/commit/7b0e54a1964774cbb011ea1e675aa8c72a3fbec4

## ToDo

- Prompt: only when building fails due to binary occupation, kill cdb and kill the test process.
- Continue to add more test cases until all features are covered.
  - Simpler collection cases using property, testing if property attributes applied to both getter return values and setter arguments.
    - Use dynamic properties, ignore the fact that caching is not implemented.
  - Collections of interfaces.
    - Collections of interfaces should by default `@rpc:Byref`
    - Observable list should by default `@rpc:Byref`
    - Regardless of the attributes, interfaces should always be passed using wrappers.
  - Collections of collections of interfaces.
  - Observable list.
  - Cached properties (by default)
  - Dynamic properties
  - Sending cached property values proactively from remote to wrapper.
  - Events
- Generate JSON from "RPC metadata in Workflow syntax".
  - Generate with wrappers.
- Solve interface inheritance and casting.
- C++ codegen for rpc data type JSON serialization.
- Delete Runtime test category.
- Update `TestLibraryRpcByval.cpp` in `LibraryTest` to use `RpcDualLifecycleMock`, remove `RpcByvalLifecycleMock`.
  - Refactor `RpcDualLifecycleMock` to make it a common implementation for `GacUI`.
  - May need to refactor how to retrieve ops interfaces.

## Prompt

I would like you to update a the Workflow compiler.

During compilation, if any RPC attributes appear in the input, `ValidateModuleRPC_GenerateMetadata` in `WfAnalyzer_ValidateRPC.cpp` will be called to collect all RPC related information and prepare a RPC interfaces definition also in Workflow, representing as a WfModule, in `WfLexicalScopeManager->rpcMetadata->metadataModule`.

And there is an optional process that will be involved separately, not in the default compiling process, which is `GenerateModuleRpc` in `WfAnalyzer_GenerateRpc.cpp`.

We allow `@rpc:Byval` and `@rpc:Byref` to no appear on collection types properties/return values/parameters, and the default option will be decided. It is implemented in `GenerateModuleRpc`. I would like you to do such change:
- Move the decision to `ValidateModuleRPC_GenerateMetadata`, that's said, when generating the RPC Workflow Metadata, all missing default option between `@rpc:Byval` and `@rpc:Byref` on collection types return values/parameters, should be filled. Also for picking `@rpc:Dynamic` or `@rpc:Cached` for properties default options.
  - The RPC Workflow metadata contains expanded property syntax, that's said, properties like `prop Name:Type{...}` will generates getter and setter functions in an interface.
  - If no `@rpc:Byval` or `@rpc:Byref` is on the property of collection type, the getter and setter should have the default option attached.
  - If yes, the getter and setter will have them attached. But there is no need to remove them from the property.
  - You need to update Test\Resource\Runtime\AttributesRpc.txt to make it contains enough cases (different situation) to verify if you have done it correctly.
    - Test\Generated\RpcMetadata(32|64)\AttributesRpc.txt will reflect the change.
- `GenerateModuleRpc` would just use all explicitly attached attributes, instead of judging by itself, to decide the bahavior of wrappers.
  - Wrapper behavior around `@rpc:Dynamic` and `@rpc:Cached` is not implemented yet, it is fine, we will do this part in the future.
  - New errors should be created when any attribute is missing. That's said, no byref/byval on collections type return value/parameters, no dynamic/cached on properties.
  - All `GenerateModuleRpc` specific errors should begin with `I` instead of `H`. Apply this to existing errors if anything need to fix.

RPC definitions and rules is in `TODO_RPC_Definition.md`, check it out.
Pay attention to the `Default options between @rpc:Byval and @rpc:Byref` section, because this is an updated rule and it does not match the current implementation.

You should run all unit test projects with debug and both Win32/x64 to ensure your change works.
In `Rpc` test samples, all collection objects currently have explicit `@rpc:Byref` or `@rpc:Byval` attached, so your change should not affect them.
- Pass all unit test, fix any test failure including pre-existings.
- If `CompilerTest_LoadAndCompile` passed but follow up test fails, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
- After finishing everything, git commit and git push to the current branch.
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
