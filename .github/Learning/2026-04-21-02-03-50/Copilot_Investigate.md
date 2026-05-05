# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Follow job.new-sample.md to add a new Rpc\Dtor2.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

namespace YourFavoriteNamespace // use RpcDtor2Test
{
    @rpc:Interface
    @rpc:Ctor
    interface IService
    {
    func GetServiceAgain() : IService^;
    }
}

var s = "Not Deleted";

func serviceMain(lc : IRpcLifeCycle*) : void
{
    var serviceObj = new (RpcDtorTest::IService^)
    {
    delete { s = "Deleted"; }
    override func GetServiceAgain() : IService^
    {
      var obj = lc.RequestService("YourFavoriteNamespace::IService");
      if ((obj as (IRpcWrapperBase^) is not null)) { throw "IService(obj) should be a local object in serviceMain"; }
      return obj;
    }
    };
    lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func clientMain(lc : IRpcLifeCycle*) : string
{
    var obj = cast (RpcPrimitiveTest::IService^) lc.RequestService("YourFavoriteNamespace::IService");
  if ((obj as (IRpcWrapperBase^) is null)) { throw "IService(obj) should be a wrapper object in clientMain"; }
  var wrapperObj = obj.GetServiceAgain();
  if ((wrapperObj as (IRpcWrapperBase^) is null)) { throw "IService(wrapperObj) should be a wrapper object in clientMain"; }
  wrapperObj = null;
  var m = $"[$(s)]"; // s should be "Not Deleted"
  clientObj = null;  // Releasing the wrapper causing ReleaseRemoteObject and ObjectHold(false) to be called
  var m = $"[$(s)]"; // s should be "Deleted"
  return s;
}
```

Understand what the test case trying to say, you are not allowed to change:
- This test is an upgrade to Rpc\Dtor.txt. If both fail, fix Dtor.txt first and then this one.
- The content of the sample, unless it doesn't build.
- Workflow parser.
- Workflow compiling.
- Workflow to C++ code generation.

You are highly possibly need to fix implementation of `RpcDualLifecycleMock` and its connected interfaces if sample fails in either `RuntimeTest` or `CppTest*`.
- If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
- Pass all unit test, fix any test failure including pre-existings.
- After finishing everything, git commit and git push to the current branch.
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.

# UPDATES

## UPDATE

Added `Test/Resources/Rpc/Dtor2.txt` and registered it in `Test/Resources/IndexRpc.txt` and `Test/UnitTest/CompilerTest_LoadAndCompile/CompilerTest_LoadAndCompile.vcxproj`.

The first `CompilerTest_LoadAndCompile` failure happened before runtime behavior was involved:
- Workflow syntax does not support `throw` in the sample. It must use `raise`.
- After fixing that, the analyzer reported `A11: Expression of type "RpcDtor2Test::IService^" cannot explicitly convert to "system::IRpcWrapperBase^".`

The Dtor2 sample needed to preserve the intended behavior while matching Workflow's type system:
- keep the request result as raw `object`
- cast once to `RpcDtor2Test::IService^` for method calls
- keep wrapper checks on raw `object` values instead of explicitly converting `IService^` to `IRpcWrapperBase^`

After the sample compiled, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` still failed to link in Debug x64.
The unresolved externals were all for `vl_workflow_global::Dtor2::*` and `LoadDtor2Types`.

Root cause: the shared RPC item lists were not updated for the newly generated Dtor2 files.
- `Test/UnitTest/Generated_CppRpc/Generated_CppRpc.vcxitems` was missing `Dtor2.cpp` and `Dtor2.h`.
- `Test/UnitTest/Generated_ReflectionRpc/Generated_ReflectionRpc.vcxitems` was missing `Dtor2Reflection.cpp` and `Dtor2Reflection.h`.

No change to `Test/Source/RpcDualLifecycleMock.*` was required for Dtor2. Once the sample and RPC shared item lists were correct, runtime and generated-C++ tests both passed on Win32 and x64.

# TEST

- Build Debug x64: passed.
- Build Debug Win32: passed.
- `LibraryTest` Debug x64: passed, `14/14`.
- `LibraryTest` Debug Win32: passed, `14/14`.
- `CompilerTest_GenerateMetadata` Debug x64: passed, `2/2`.
- `CompilerTest_GenerateMetadata` Debug Win32: passed, `2/2`.
- `CompilerTest_LoadAndCompile` Debug x64: passed, `585/585`.
  - `Dtor2` compiled and serialized successfully: `52902 bytes`.
- `RuntimeTest` Debug x64: passed, `143/143`.
  - `Rpc:Dtor2` expected `[Not Deleted][Deleted]`, actual `[Not Deleted][Deleted]`.
- `RuntimeTest` Debug Win32: passed, `143/143`.
  - `Rpc:Dtor2` expected `[Not Deleted][Deleted]`, actual `[Not Deleted][Deleted]`.
- `CppTest` Debug x64: passed, `109/109`.
- `CppTest` Debug Win32: passed, `109/109`.
- `CppTest_Metaonly` Debug x64: passed, `109/109`.
- `CppTest_Metaonly` Debug Win32: passed, `109/109`.
- `CppTest_Reflection` Debug x64: passed, `109/109`.
- `CppTest_Reflection` Debug Win32: passed, `109/109`.

# PROPOSALS

- No.1 Fix Dtor2 sample semantics and RPC project wiring

## No.1 Fix Dtor2 sample semantics and RPC project wiring

`Rpc:Dtor2` was not blocked by RPC lifetime behavior in `RuntimeTest` or `CppTest*`.
The actual blockers were one invalid Workflow-language construct in the sample, one Workflow type-system restriction in the wrapper check, and missing generated-file registrations in the shared RPC project items.

### CODE CHANGE

- Added `Test/Resources/Rpc/Dtor2.txt`.
- Added `Dtor2=[Not Deleted][Deleted]` to `Test/Resources/IndexRpc.txt`.
- Added Dtor2 to `Test/UnitTest/CompilerTest_LoadAndCompile/CompilerTest_LoadAndCompile.vcxproj` and `.filters`.
- Updated the Dtor2 sample so it uses `raise` instead of `throw`.
- Updated the Dtor2 sample so client-side wrapper checks are performed on raw `object` values:
  - `var obj = lc.RequestService("RpcDtor2Test::IService");`
  - `var clientObj = cast (RpcDtor2Test::IService^) obj;`
  - `var wrapperObj : object = clientObj.GetServiceAgain();`
- Added generated Dtor2 source entries to `Test/UnitTest/Generated_CppRpc/Generated_CppRpc.vcxitems`.
- Added generated Dtor2 reflection entries to `Test/UnitTest/Generated_ReflectionRpc/Generated_ReflectionRpc.vcxitems`.

### CONFIRMED

- The compiler, runtime VM path, and generated-C++ path all accept Dtor2 after those changes.
- `Rpc:Dtor2` passes in `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` on both Win32 and x64.
- `RpcDualLifecycleMock` did not need modification for this task.