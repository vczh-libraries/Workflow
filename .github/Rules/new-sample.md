# Guideline of Adding New Workflow Sample

This instruction focuses on adding `Test\Resources\Codegen\*.txt` and `Test\Resources\Rpc\*.txt`.
Each sample should have an index in `Test\Resources\Index(Codegen|Rpc).txt`, keeping this file sorted.

## Common Instructions

- Samples here are testing the Workflow compiler against legal script. Illegal scripts will be covered in other categories.
- The Workflow compiler may need to generate more Workflow script from the input, such generation should be done by constructing Workflow AST instead of building a string.
- DO NOT introduce breaking changes UNLESS explicitly instructed. The Workflow compiler have been tested by many cases in `Codegen` category, try your best not to intruduce changing to existing samples.
- C++ code generation is sensitive, you should always try to make minimum or even no changes to C++ code generation, until you have no other choices. Besides type reflection part, generated C++ code should not use any reflection features, and all `CppTest`, `CppTest_Metaonly` and `CppTest_Reflection` should actually run the code and pass.
  - By saying "not use any reflection features", it means you are not allowed to use reflection features and protect it with preprocessor intrinsics, you should always do it in non-reflection related ways.
- Any sample should not be disabled in `CppTest`, `CppTest_Metaonly`, `CppTest_Reflection` and `RuntimeTest`.

## Codegen Sample

A `Codegen` sample is a Workflow script following the pattern:

```Workflow
module test;
using system::*;

func main():string
{
  return "Test Result";
}
```

Calculations are performed and the result returns from `main`. In `Test\Resources\IndexCodegen.txt`, the expected result of the sample should be written down. And the script goes through the following process:
- It is compiled in `CompilerTest_LoadAndCompile`'s `TestCodegen.cpp`, producing log files, Workflow binary and C++ source files.
- The Workflow binary will be loaded in `RuntimeTest`'s `TestRuntime.cpp`, which runs the `main` function and compare the result against `IndexCodegen.txt`.
- Generate C++ source files should be added to `Generated_Cpp` and `Generated_Reflection` projects, and they will be picked up by `CppTest`, `CppTest_Metaonly` and `CppTest_Reflection` projects. They do the same thing to run the `main` function against `IndexCodegen.txt`, but everything is generated in `TestCases.cpp`.

## Rpc Sample

A `Rpc` sample is a pair of Workflow scripts. `Test\Resources\Rpc\SAMPLE.txt` contains only RPC definitions and is used for RPC metadata/wrapper generation:

```Workflow
module Rpc;
using system::*;

namespace YourFavoriteNamespace
{
	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
	}
}
```

`Test\Resources\Rpc\SAMPLE_Test.txt` contains the executable test logic:

```Workflow
module Rpc;
using system::*;
using YourFavoriteNamespace::*;

func serviceMain(lc : IRpcLifecycle*) : void
{
	var service = new (YourFavoriteNamespace::IService^)
	{
	};
	lc.RegisterService("YourFavoriteNamespace::IService", service);
}

func clientMain(lc : IRpcLifecycle*) : string
{
	var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");
	return "Test Result";
}
```

Calculations are performed and the result returns from `clientMain`. In `Test\Resources\IndexRpc.txt`, the expected result of the sample should be written down under `SAMPLE` only; do not add `SAMPLE_Test` to the index. Both files should be added to `CompilerTest_LoadAndCompile` under `Resource Files\Rpc`. The scripts go through the following process:
- `SAMPLE.txt` is compiled in `CompilerTest_LoadAndCompile`'s `TestRpcCompile.cpp` to produce RPC metadata and wrappers.
- `SAMPLE.txt`, `SAMPLE_Test.txt`, and the generated wrappers are linked in `TestRpcCompile.cpp`, producing log files, Workflow binary and C++ source files.
- The Workflow binary will be loaded in `RuntimeTest`'s `TestRpc.cpp`, which initializes the Rpc environment, calls `serviceMain` followed by `clientMain` and compare the result of `clientMain` against `IndexRpc.txt`.
- Generate C++ source files should be added to `Generated_CppRpc` and `Generated_ReflectionRpc` projects, and they will be picked up by `CppTest`, `CppTest_Metaonly` and `CppTest_Reflection` projects. They do the same thing to run the `clientMain` function against `IndexCodegen.txt`, but everything is generated in `TestCasesRpc.cpp`.
