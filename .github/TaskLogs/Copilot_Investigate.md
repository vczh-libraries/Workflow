# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Follow job.new-sample.md to add a new Rpc\Dtor3.txt

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

# UPDATES

## UPDATE

Just one thing to say, between [1] and [2], the order of IContainer and IValue releasing is not important. Just make CppTest and RuntimeTest generating the same result. You can pick the easier one.

Continue to follow #file:investigate.prompt.md to finish the work

# TEST [CONFIRMED]

- Existing coverage is sufficient:
	- `CompilerTest_LoadAndCompile` regenerates RPC generated C++ from `Test/Resources/IndexRpc.txt` and must include `Dtor3` successfully.
	- `RuntimeTest` compares the workflow runtime result of `Rpc/Dtor3` against `IndexRpc.txt`.
	- `CppTest` compares the generated C++ result of `Rpc:Dtor3` against the generated `TestCasesRpc.cpp` expectation, which is emitted from `IndexRpc.txt` during `CompilerTest_LoadAndCompile`.
- Success criteria:
	- After regeneration, `RuntimeTest` and `CppTest` must both use the same `Dtor3` expected string.
	- `CompilerTest_LoadAndCompile`, `RuntimeTest`, and `CppTest` must pass for the touched configuration.
	- `CppTest_Metaonly` and `CppTest_Reflection` must also pass because they compile the same generated RPC surface through different reflection configurations.
- Confirmation:
	- `Debug|x64`: `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` all passed.
	- `Debug|Win32`: the same matrix passed, including `CompilerTest_LoadAndCompile` with `Passed test files: 6/6` and `Passed test cases: 587/587`.
	- `RuntimeTest` and every `CppTest*` variant now agree on `Dtor3=[1][IContainer][IValue][2][IContainer][3][IValue][4]`.

# PROPOSALS

- No.1 Align Dtor3 Expected Output [CONFIRMED]

## No.1 Align Dtor3 Expected Output

### CODE CHANGE

- Added `Test/Resources/Rpc/Dtor3.txt` as the new sample and normalized only the parts that were required to make it build while preserving the sample intent: namespace/type usage, the explicit getter-backed property shape, `raise` syntax, and wrapper checks through `object` temporaries.
- Added `Dtor3.txt` to `Test/UnitTest/CompilerTest_LoadAndCompile/CompilerTest_LoadAndCompile.vcxproj` and `.filters` so the compile/regeneration test includes the new RPC sample.
- Updated `Test/Resources/IndexRpc.txt` to the destructor order that is actually produced consistently by the runtime and accepted by the user clarification: `Dtor3=[1][IContainer][IValue][2][IContainer][3][IValue][4]`.
- Regenerated the Dtor3 outputs through `CompilerTest_LoadAndCompile`, which updated `Test/SourceCppGenRpc/TestCasesRpc.cpp` and produced the new `Dtor3` generated source, reflection, metadata, and workflow dump files under `Test/SourceCppGenRpc`, `Test/Generated/CppRpc32`, `Test/Generated/CppRpc64`, `Test/Generated/RpcMetadata32`, `Test/Generated/RpcMetadata64`, `Test/Generated/Workflow32`, and `Test/Generated/Workflow64`.
- Added the generated Dtor3 implementation and reflection files to `Test/UnitTest/Generated_CppRpc/Generated_CppRpc.vcxitems` and `Test/UnitTest/Generated_ReflectionRpc/Generated_ReflectionRpc.vcxitems` so `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` link the new generated case.

### CONFIRMED

- The initial failure was a Dtor3 expectation mismatch, not a parser/compiler/code-generation failure. `RuntimeTest` already produced a stable destructor order for Dtor3. After the user clarified that the relative release order of `IContainer` and `IValue` between `[1]` and `[2]` was acceptable as long as `RuntimeTest` and `CppTest*` matched, aligning `Test/Resources/IndexRpc.txt` with the observed runtime order solved the behavioral mismatch without changing Workflow parser behavior, Workflow compilation, Workflow-to-C++ code generation, or `RpcDualLifecycleMock`.
- Regeneration exposed a separate build-integration issue: `CompilerTest_LoadAndCompile` generated `Dtor3.cpp`, `Dtor3Reflection.cpp`, and the new `Rpc:Dtor3` case in `TestCasesRpc.cpp`, but the shared vcxitems used by `CppTest*` still only compiled the existing Dtor/Dtor2 generated files. Adding Dtor3 to both shared item manifests removed the unresolved external/link failures in the generated C++ test projects.
- Validation confirmed the proposal in both architectures. `Debug|x64` and `Debug|Win32` solution builds succeeded, and `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` all passed. The final accepted Dtor3 output is `Dtor3=[1][IContainer][IValue][2][IContainer][3][IValue][4]` in both the workflow runtime path and the generated C++ path.
