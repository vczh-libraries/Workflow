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

## UPDATE

Looks like the script is stuck, I have run CI for you, LoadAndCompile works, but CppTest fail. The rest of the 3 I don't know. And I have committed your work so far. The compiler looks good, so the problem might still be in RpcDualLifecycleMock.

## UPDATE

actually you can also change the clientMain to do
m = $"$(m)[$(s)]";
return m;
so it could return [Not Deleted][Deleted], giving you richer information

Continue to follow #file:investigate.prompt.md to finish the work.

# TEST [CONFIRMED]

Add a new `Rpc\Dtor.txt` sample and register it in `IndexRpc.txt` and the `CompilerTest_LoadAndCompile` resource list.

The sample should confirm this lifecycle behavior:
- Casting a remote service object to `IRpcWrapperBase^` must succeed and must not crash.
- Releasing that temporary wrapper reference must not destroy the remote service object.
- Releasing the final client-side wrapper reference must invoke remote release and trigger the Workflow destructor, changing `s` from `"Not Deleted"` to `"Deleted"`.

Success criteria:
- `CompilerTest_LoadAndCompile` compiles the new sample and generates merged C++ output for both x86 and x64.
- `RuntimeTest`, `CppTest`, `CppTest_Metaonly` and `CppTest_Reflection` all pass for Win32 and x64 with the new `Rpc:Dtor` case.
- If any failure only appears in runtime or only in generated C++, use that split to narrow the bug to lifecycle/runtime behavior rather than parser/compiler/codegen.

Observed confirmation:
- `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` all fail on `Rpc:Dtor` in x64 with `actual = Not Deleted`, so the failure is in shared lifecycle behavior rather than parser/compiler/code generation.
- Rebuilding the sample with a local `serviceObj` removes one artificial root reference, but the failure still reproduces, so the remaining problem is in RPC lifetime management.

# PROPOSALS

- No.1 Stop RpcDualLifecycleMock From Pinning Wrappers And Services

## No.1 Stop RpcDualLifecycleMock From Pinning Wrappers And Services

`Rpc:Dtor` only passes if the client-side proxy can actually die when the last user-visible reference is released, and if the server-side service object is not kept alive forever by the mock's service registry. The current mock keeps strong `Ptr<>` ownership in both places, which delays destruction until lifecycle teardown.

### CODE CHANGE

- Updated `Test/Source/RpcDualLifecycleMock.h/.cpp` so wrapper tracking is non-owning:
    - `wrapperEntries` now keep raw pointers instead of strong `Ptr<IRpcWrapperBase>` references.
    - Added `RpcDualWrapperTracker` via internal properties so tracked wrappers can unregister themselves when the wrapper object dies.
    - `PtrToRef` can still resolve wrappers and local objects without extending their lifetime.
- Updated `UnregisterLocalObject` to remove the matching entry from `services`, so releasing the last remote reference can actually release the registered service object.
- Added `DisconnectTrackedWrappers()` and used it from both `RpcWorkflowLifecycleMock::~RpcWorkflowLifecycleMock()` and `RpcDualLifeCycleAdapter::~RpcDualLifeCycleAdapter()`.
    - This prevents surviving workflow wrappers from running their destructors after the adapter `_lc` capture has already been destroyed.
    - Without this, `LocalAndWrapper` still crashed during teardown even after `Rpc:Dtor` started passing.
- Updated `Test/Resources/Rpc/Dtor.txt` to return the richer signal `[Not Deleted][Deleted]`, and updated the expected result in `Test/Resources/IndexRpc.txt` and generated RPC test cases.

### CONFIRMED

- Isolated `RuntimeTest` x64 runs showed:
    - `Rpc:Dtor` now returns `[Not Deleted][Deleted]`.
    - `Rpc:ServiceWrapper` passes.
    - The remaining failure moved to teardown after `Rpc:LocalAndWrapper`.
- Debugging that teardown crash showed a workflow wrapper destructor invoking through a freed adapter capture, which confirmed the second issue was cleanup ordering after wrappers stopped being pinned.
- After disconnecting tracked wrappers in `RpcDualLifeCycleAdapter::~RpcDualLifeCycleAdapter()`, the full matrix passes:
    - `RuntimeTest` Win32 and x64
    - `CppTest` Win32 and x64
    - `CppTest_Metaonly` Win32 and x64
    - `CppTest_Reflection` Win32 and x64
- Conclusion: the root cause was in shared RPC lifecycle mock ownership and teardown behavior, not in Workflow parsing, compilation, or Workflow-to-C++ generation.