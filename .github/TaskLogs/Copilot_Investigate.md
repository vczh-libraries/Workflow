# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Follow
- `.github\Rules\new-sample.md`
- `.github\Rules\new-sample-rpc.md`
- `.github\Rules\verify-and-commit.md`
to add new samples, indentation should be double spaces no matter how the code below is written:

RPC samples are split into two files:
- `Rpc\SAMPLE.txt` contains only RPC definitions.
- `Rpc\SAMPLE_Test.txt` contains executable test logic, including globals, helpers, `serviceMain`, and `clientMain`.

Only `SAMPLE` is added to `IndexRpc.txt`. Both files must appear in `CompilerTest_LoadAndCompile` under `Resource Files\Rpc`.

## Sample Rpc\Inheritance.txt

```Workflow
module Rpc;
using system::*;

namespace YourFavoriteNamespace // use RpcInheritance
{
  @rpc:Interface
  interface IDerived : IOne, ITwo
  {
    func SetDerivedValue() : void;
  }

  @rpc:Interface
  interface IValue
  {
    @rpc:Cached
    prop Value : string {const, observe}
  }

  @rpc:Interface
  interface IOne : IValue
  {
    func SetOneValue() : void;
  }

  @rpc:Interface
  interface ITwo : IValue
  {
    func SetITwoValue() : void;
  }

	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
    func CreateOne() : IOne^;
    func CreateTwo() : ITwo^;
    func CreateDerivec() : IDerived^;
	}
}
```

## Sample Rpc\Inheritance_Test.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;
using YourFavoriteNamespace::*;

var s = "";

func serviceMain(lc : IRpcLifecycle*) : void
{
	var serviceObj = new (YourFavoriteNamespace::IService^)
	{
    override func CreateOne() : IOne^
    {
      return new IOne^
      {
        var value : string = "";
        override func GetValue() : string { return value; }
        override func SetOneValue() : void
        {
          value = "One";
          ValueChanged();
        }
      }
    }
    
    override func CreateTwo() : ITwo^
    {
      // Copy CreateOne but set the value to "Two"
    }
    
    override func CreateDerived() : IDerived^
    {
      // Copy CreateOne but set the value to "Derived"
      // Meanwhile Set(One|Two)Value should raise exception
    }
	};
	lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func clientMain(lc : IRpcLifecycle*) : string
{
	var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");
  var one = cast (IOne^) service.CreateOne();
  // repeat for two and derived

  s = $"$(s)[$(one.Value)]";
  one.SetOneValue();
  s = $"$(s)[$(one.Value)]";
  // repeat for two and derived

  return s; // [][One][][Two][][Derived]
}
```

## Goal

In this task you are going to build and run test cases to verify if these cases are working, according to `TODO_RPC_Definition.md`
This test is to ensure that:
- Wrapper Workflow Script code generation take members of base types of interfaces into consideration recursively.

If the current implemention is correct, the added samples should just pass the test.

# UPDATES

# TEST [CONFIRMED]

Add an RPC sample pair `Rpc\Inheritance.txt` and `Rpc\Inheritance_Test.txt` to verify that generated RPC wrapper Workflow Script code recursively includes inherited interface members. The definition file should contain only RPC interfaces in namespace `RpcInheritance`; the test file should implement `IService`, create `IOne`, `ITwo`, and `IDerived` values, read the inherited cached `Value` property through wrappers, and mutate through valid interface methods.

Success criteria:
- `IndexRpc.txt` contains only `Inheritance=[][One][][Two][][Derived]`.
- Both sample files are included as `Resource Files\Rpc` in `CompilerTest_LoadAndCompile`.
- `CompilerTest_LoadAndCompile` passes after adding the sample.
- Runtime and generated C++ RPC execution pass, proving the generated wrappers handle recursive base interface members.

# PROPOSALS

- No.1 Add the RPC inheritance sample pair and wire it into the RPC sample index and compiler test project [CONFIRMED]

## No.1 Add the RPC inheritance sample pair and wire it into the RPC sample index and compiler test project

### CODE CHANGE

- Added split RPC sample files `Test\Resources\Rpc\Inheritance.txt` and `Test\Resources\Rpc\Inheritance_Test.txt` in namespace `RpcInheritance`.
- Added `Inheritance=[][One][][Two][][Derived]` to `Test\Resources\IndexRpc.txt`.
- Added both sample files to `CompilerTest_LoadAndCompile` under `Resource Files\Rpc`.
- Added generated C++ RPC and reflection source entries for the new sample to `Generated_CppRpc.vcxitems` and `Generated_ReflectionRpc.vcxitems`.
- Updated RPC wrapper generation in `WfAnalyzer_GenerateRpc.cpp` to collect inherited RPC properties and methods recursively, matching the existing recursive event collection. Generated wrapper interfaces and wrapper factories now implement inherited cached property invalidators, getters, and base interface methods.

### CONFIRMED

- The initial sample reproduced the generator issue: `CompilerTest_LoadAndCompile` failed with `Interface method not implemented` errors in `Parsing.Rpc.Inheritance.txt` for inherited `GetValue`, `SetOneValue`, and `SetITwoValue`.
- After updating wrapper generation, `CompilerTest_LoadAndCompile` passed on Debug x64.
- The first generated-code build failed with unresolved `Rpc_Inheritance` symbols until the new generated C++ RPC and reflection files were added to the shared vcxitems files; after that, Debug x64 and Win32 builds passed.
- `Wrapper_Inheritance.txt` now shows `IRpcWrapper_IOne`, `IRpcWrapper_ITwo`, and `IRpcWrapper_IDerived` declaring inherited `_rpcInvalidate_Value`, and the generated wrapper objects implement inherited `GetValue`, `SetOneValue`, and `SetITwoValue` where applicable.
- Debug x64 and Win32 `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` all passed. The new `Rpc:Inheritance` case returned `[][One][][Two][][Derived]` in runtime and all generated C++ variants.
- The full release `Build.ps1 Workflow` pass completed successfully.
