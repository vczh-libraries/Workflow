# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Follow job.new-sample.md to add a new Rpc\Dtor.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

namespace YourFavoriteNamespace // use RpcDtorTest
{
    @rpc:Interface
    @rpc:Ctor
    interface IService
    {
    }
}

var s = "Not Deleted";

func serviceMain(lc : IRpcLifeCycle*) : void
{
    serviceObj = new (YourFavoriteNamespace::IService^)
    {
    delete { s = "Deleted"; }
    };
    lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func clientMain(lc : IRpcLifeCycle*) : string
{
    var obj = lc.RequestService("YourFavoriteNamespace::IService");
    var clientObj = cast (YourFavoriteNamespace::IService^) obj;
  // remote object becomes a wrapper, crashing means bug in the RPC framework
  var wrapperObj = cast (IRpcWrapperBase^) clientObj;
  wrapperObj = null;
  var m = $"[$(s)]"; // s should be "Not Deleted"
  clientObj = null;  // Releasing the wrapper causing ReleaseRemoteObject and ObjectHold(false) to be called
  var m = $"[$(s)]"; // s should be "Deleted"
  return s;
}
```

Understand what the test case trying to say, you are not allowed to change:
- The content of the sample, unless it doesn't build.
- Workflow parser.
- Workflow compiling.
- Workflow to C++ code generation.

You are highly possibly need to fix implementation of `IRpcLifecycle` and its connected interfaces if sample fails in either `RuntimeTest` or `CppTest*`.
- If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
- After finishing everything, git commit and git push to the current branch.
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.

# UPDATES

# TEST

Add a new `Rpc\Dtor.txt` sample and register it in `IndexRpc.txt` and the `CompilerTest_LoadAndCompile` resource list.

The sample should confirm this lifecycle behavior:
- Casting a remote service object to `IRpcWrapperBase^` must succeed and must not crash.
- Releasing that temporary wrapper reference must not destroy the remote service object.
- Releasing the final client-side wrapper reference must invoke remote release and trigger the Workflow destructor, changing `s` from `"Not Deleted"` to `"Deleted"`.

Success criteria:
- `CompilerTest_LoadAndCompile` compiles the new sample and generates merged C++ output for both x86 and x64.
- `RuntimeTest`, `CppTest`, `CppTest_Metaonly` and `CppTest_Reflection` all pass for Win32 and x64 with the new `Rpc:Dtor` case.
- If any failure only appears in runtime or only in generated C++, use that split to narrow the bug to lifecycle/runtime behavior rather than parser/compiler/codegen.

# PROPOSALS