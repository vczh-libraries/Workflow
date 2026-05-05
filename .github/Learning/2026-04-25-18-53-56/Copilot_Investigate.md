# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Follow job.new-sample.md to add a new sample: Rpc\Event.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

namespace YourFavoriteNamespace // use RpcEvent
{
    @rpc:Interface
    @rpc:Ctor
    interface IService
    {
    event SomethingHappened(string);

    func MakeItHappen() : void;
    func Watch() : void;
    }
}
var s = "";

func serviceMain(lc : IRpcLifeCycle*) : void
{
    var serviceObj = new (YourFavoriteNamespace::IService^)
    {
    override func MakeItHappen() : void
    {
      SomethingHappened("A");
    }

    override func Watch() : void
    {
      attach(SomethingHappened, [s = $"$(s)[serviceMain:$($1)]"]);
      // or use func (something : string) : void { s = $"$(s)[serviceMain:$(something)]"; }
    }
    };
    lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func clientMain(lc : IRpcLifeCycle*) : string
{
    var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");

  // serviceMain raises an event and clientMain handles it
  var handler = attach(service.SomethingHappened, [s = $"$(s)[serviceMain:$($1)]"]);
  service.MakeItHappen();
  detach(service.SomethingHappened, handler);

  // clientMain raises and event and serviceMain handles it
  service.Watch();
  service.SomethingHappened("B");

  // [clientMain:A][serviceMain:B]
  return s;
}
```

In this task you are going to build and run test cases to verify if these cases are working, according to `TODO_RPC_Definition.md`
This test is to ensure that:
- The sample tests if raising an event works bidirectionaly

Understand what the test case trying to say, you are not allowed to change:
- The content of the sample, unless it doesn't build.
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
  - DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.

# UPDATES

- Added `Test\Resources\Rpc\Event.txt` and registered `Event=[clientMain:A][serviceMain:B]` in `Test\Resources\IndexRpc.txt`.
- Kept the sample behavior intact, with build-compatible handler functions instead of comments or fragile inline handler syntax.
- Added the generated RPC Event C++ and reflection files to `Test\UnitTest\Generated_CppRpc\Generated_CppRpc.vcxitems` and `Test\UnitTest\Generated_ReflectionRpc\Generated_ReflectionRpc.vcxitems`.
- Used explicit `Rpc_Event.obj` and `Rpc_EventReflection.obj` object names to avoid collisions with existing non-RPC Event sources.
- Implemented bidirectional object event forwarding in `RpcDualLifecycleMock`, defined in `Test\Source\RpcDualLifecycleMock.h` and implemented in `Test\Source\RpcDualLifecycleMock.cpp`.
- Added event forwarding state and suppression so generated RPC wrappers can forward local event raises without echoing the same event back into the source wrapper.
- Added native typed event attach support for no-reflection and metaonly generated C++ RPC tests in `Test\UnitTest\CompilerTest_LoadAndCompile\TestRpcCompile.cpp`.
- Added native typed event delivery support so generated C++ tests unbox event arguments into local values before invoking events, preserving by-value strings across multiple handlers.
- Fixed event metadata collection by resolving the original RPC interface descriptor and reading argument types from `IEventInfo::GetHandlerType()->GetElementType()`.
- Fixed `CppTest_Reflection` by loading Workflow library reflection types with `WfLoadLibraryTypes()` before generated RPC reflection metadata in `Test\UnitTest\CppTest_Reflection\Main.cpp`.

# TEST

- Add `Test\Resources\Rpc\Event.txt` using the requested RPC event sample, changing only the sample text if the supplied script does not build.
- Register `Event=[clientMain:A][serviceMain:B]` in `Test\Resources\IndexRpc.txt` so `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` all exercise the same bidirectional event path.
- Build Debug Win32 and Debug x64 for `Test\UnitTest\UnitTest.sln`, run the unit-test sequence from `Project.md`, and continue through `RuntimeTest` plus all `CppTest*` projects even if one RPC event path fails, to identify whether failure is runtime-only, generated-C++-only, or shared.
- Success criteria: the new sample compiles, service-raised event `A` reaches the client handler, client-raised event `B` reaches the service handler, the returned string is exactly `[clientMain:A][serviceMain:B]`, all unit tests pass, and final `..\Tools\Tools\Build.ps1 Workflow` validation passes before pushing.

# VALIDATION

- Debug Win32 build passed with 0 warnings and 0 errors.
- Debug x64 build passed with 0 warnings and 0 errors.
- `RuntimeTest` Win32/x64 passed: 4/4 test files, 243/243 test cases.
- `CppTest` Win32/x64 passed: 2/2 test files, 209/209 test cases.
- `CppTest_Metaonly` Win32/x64 passed: 2/2 test files, 209/209 test cases.
- `CppTest_Reflection` Win32/x64 passed: 2/2 test files, 209/209 test cases.
- `LibraryTest` Win32/x64 passed: 2/2 test files, 14/14 test cases.
- `CompilerTest_GenerateMetadata` Win32/x64 passed: 1/1 test files, 2/2 test cases.
- `CompilerTest_LoadAndCompile` Win32/x64 passed: each run reported two internal summaries of 6/6 test files and 689/689 test cases.
- The `Rpc:Event` test returned `[clientMain:A][serviceMain:B]` in RuntimeTest and all CppTest modes.

# PROPOSALS