# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Follow job.new-sample.md to add new samples, indentation should be double spaces no matter how the code below is written:

## Sample Rpc\EventArgs.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

namespace YourFavoriteNamespace // use RpcEventArgs
{
  @rpc:Interface
  interface IValue
  {
    @rpc:Cached
    prop Value : string {const, not observe}
  }

	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
    func Print(s : string?, v : IValue^) : string;
	}
}

var s = "";

func serviceMain(lc : IRpcLifeCycle*) : void
{
	var serviceObj = new (YourFavoriteNamespace::IService^)
	{
    override func Print(s : string?, v : IValue^) : string
    {
      return $"[$(cast string s ?? 'null')][$(v.Value ?? 'null')]";
    }
	};
	lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func clientMain(lc : IRpcLifeCycle*) : string
{
	var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");

  s = $"$(s)$(service.Print("abc", null))";
  s = $"$(s)$(service.Print(null, new IValue^ { override func GetValue() { return "def"; } }))";

  return s; // [abc][null][null][def]
}
```

## Goal

In this task you are going to build and run test cases to verify if these cases are working, according to `TODO_RPC_Definition.md`
This test is to ensure that:
- Since all primitive types are serializable, nullable type could only be applied to primitive types, so it should also be serializable.
  - If nullable types is rejected with PRC specific errors, it should be fixed.
  - There are also limited types that could be used with the nullable operator "?". Verify if such types are all serializable. If yes, it is easy to say all nullable types are also serializable. If no, then you need to look into `T` when you see `T?`. Answer this question when you finish the work.
- Passing null is handled properly.
  - If passing null crashes, `Rpc(B|Unb)oxBy(val|ref)` should be the first thing to look at.

If the current implemention is correct, the added samples should just pass the test.

## Verifying Samples

Workflow script syntax and semantic should be intuitive.
During reading the sample, you should verify it with the goal of the task.
Ensure all logs or exceptions in the sample accurately reflected the intention of the design.
Ensure the expected result would be what users would expect.

## Restriction

Understand what the test case trying to say, you are not allowed to change:
- The content of the verified sample, unless it doesn't build.
- Workflow parser.
- Workflow compiling.
- Workflow to C++ code generation.

You are highly possibly need to fix:
- `Rpc(B|Unb)oxBy(val|ref)`, as these 4 C++ functions are directly called in generated wrapper classes written Workflow script.
- The wrapper classes generation.
- implementation of `RpcDualLifecycleMock` and its connected interfaces if sample fails in either `RuntimeTest` or `CppTest*`.
- The generated C++ code is very straight forward, if it fails, check `RpcDualLifecycleMock` first.
  - The comment in the sample describes how `RpcDualLifecycleMock` and the generated C++ code is supposed to work.

If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
  - Pass all unit test, fix any test failure including pre-existings.
  - After finishing everything, git commit and git push to the current branch.
  - Two commits are required. First commit only has all modified files and files you created directly, second commit has all new files that not created by you (aka auto generated)
    - Typical files that are generated: `Test\Generated`, `Test\SourceCppGen`, `Test\SourceCppGenRpc`, `Test\UnitTest\Generated_*`.
  - DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.

# UPDATES

## UPDATE

You accidentally stopped, please continue to follow #file:investigate.prompt.md to finish the work.

To my observation, you should have already finished most of editing works, I have committed all changes so far, but please take a look at the latest commit, it has generated C++ files. These files have not been added to vcxitems and filters yet. It is highly possibly that CompilerTest_LoadAndCompile have finished.

so please fix project files and then run `..\Tools\Tools\Build.ps1 Workflow` to make sure it works, or fix the code to address any test failures, including pre-existings.

# TEST [CONFIRMED]

Added an RPC sample for nullable arguments and null interface arguments. The requested `Rpc\EventArgs.txt` file already exists for a separate event-argument test, so this investigation adds the new case as `Rpc\Nullable.txt` to preserve existing coverage.

The sample should compile as a legal `@rpc:Interface` declaration with a `string?` method parameter and should run through both VM runtime and generated C++ RPC wrappers. The expected client result is:

```text
[abc][null][null][def]
```

Success criteria:

- `CompilerTest_LoadAndCompile` accepts the nullable RPC parameter without RPC-specific serializability errors.
- `RuntimeTest` runs `Rpc\Nullable.txt` and returns the expected result.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` run generated C++ for `Rpc\Nullable.txt` and return the expected result.
- Null values passed through nullable parameters and RPC interface shared pointer parameters do not crash in `RpcBoxByval`, `RpcUnboxByval`, `RpcBoxByref`, or `RpcUnboxByref`.

# PROPOSALS

- No.1 Register generated Nullable RPC files in shared item projects [CONFIRMED]

## No.1 Register generated Nullable RPC files in shared item projects

### CODE CHANGE

The latest commit already contained the generated RPC C++ files for `Rpc\Nullable.txt` in `Test\SourceCppGenRpc`, which indicates that `CompilerTest_LoadAndCompile` had already finished successfully.

Updated the two shared item project files consumed by `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`:

- Added `Nullable.cpp` and `Nullable.h` to `Test\UnitTest\Generated_CppRpc\Generated_CppRpc.vcxitems`.
- Added `NullableReflection.cpp` and `NullableReflection.h` to `Test\UnitTest\Generated_ReflectionRpc\Generated_ReflectionRpc.vcxitems`.

I also checked for corresponding RPC `.filters` files. Unlike `Generated_Cpp` and `Generated_Reflection`, the RPC shared-item folders currently do not contain `Generated_CppRpc.vcxitems.filters` or `Generated_ReflectionRpc.vcxitems.filters`, so there was no existing RPC filter file to update.

### CONFIRMED

This proposal is confirmed.

Running `..\Tools\Tools\Build.ps1 Workflow` succeeded after the shared item updates. The wrapper build rebuilt and executed all required Workflow test projects, including `CompilerTest_LoadAndCompile`, `CppTest`, `CppTest_Metaonly`, `CppTest_Reflection`, and `RuntimeTest`, and then continued into Workflow release packaging.

This means the remaining issue was not nullable RPC semantics or null handling in `RpcBoxByval`, `RpcUnboxByval`, `RpcBoxByref`, or `RpcUnboxByref`. The generated sample and runtime behavior were already correct; the C++ test projects simply were not compiling the newly generated `Nullable` translation units.

For the nullable-type question from the task: all legal nullable types are serializable in the current design. `TODO_RPC_Definition.md` states that nullable can only apply to primitive, struct, and enum types, and all nullable types should be serializable. The implementation matches that design: `ValidateTypeStructure` only permits nullable over those categories, and `WfAnalyzer_ValidateRPC.cpp` handles `ITypeInfo::Nullable` by recursively validating the element type for RPC serializability.
