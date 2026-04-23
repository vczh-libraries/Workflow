# Prompt

Follow job.new-sample.md to add:
- Rpc\Collection_InByval_OutByval.txt
- Rpc\Collection_InByval_OutByref.txt
- Rpc\Collection_InByref_OutByval.txt
- Rpc\Collection_InByref_OutByref.txt

Some comments in the sample will tell you how to write different test cases for them.
You need to delete these guiding comments in generated output.

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;

namespace YourFavoriteNamespace // use RpcCollection::InBy(val|ref)::OutBy(val|ref)
{
	@rpc:Interface
	@rpc:Ctor
	interface IService
	{
    @rpc:Byval or @rpc:Byref // this is controlled by OutBy(val|ref)
    func DoList(
      @rpc:Byval or @rpc:Byref // this is controlled by InBy(val|ref)
      xs : int[]
      ) : int[];
	}
}

var xsOrigin : int[] = {1; 2; 3};
var xsService : int[] = null;
var xsClient : int[] = null;
var s = "";

func serviceMain(lc : IRpcLifeCycle*) : void
{
	var serviceObj = new (YourFavoriteNamespace::IService^)
	{
    func DoList(xs : int[]) : int[]
    {
      // when InByval
      if ((xs as (system::IRpcWrapperBase^) is not null)) { raise "Parameter xs should be a copied local object in serviceMain"; }
      // when InByref
      if ((xs as (system::IRpcWrapperBase^) is null)) { raise "Parameter xs should be a wrapper object in serviceMain"; }

      xsService = xs;
      xs.Add(4);
      return xs;
    }
	};
	lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func Print(xs : int[]) : string
{
  var s = "";
  for (var x in xs)
  {
    s = $"$(s)$(x)";
  }
  return s;
}

func clientMain(lc : IRpcLifeCycle*) : string
{
	var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");
  var xs = service.DoList(xsOrigin);
  
  // when OutByval
  if ((xs as (system::IRpcWrapperBase^) is not null)) { raise "Return value xs should be a copied local object in clientMain"; }
  // when InByval and OutByref
  if ((xs as (system::IRpcWrapperBase^) is null)) { raise "Return value xs should be a wrapper object in clientMain"; }
  // when InByref and OutByref
  if (xs != xsOrigin) { raise "Return value xs should be the original parameter object in clientMain"; }

  xsClient = xs;
  xs.Add(5);

  s = $"$(s)[$(Print(xsOrigin))]";
  s = $"$(s)[$(Print(xsService))]";
  s = $"$(s)[$(Print(xsClient))]";

  // If InByref, then xsService will be connecting to xsOrigin
  // If OutByref, then xsClient will be connecting to xsService
  // Therefore we have these expected results:
  // InByval_OutByval: [123][1234][12345]
  // InByval_OutByref: [123][12345][12345]
  // InByref_OutByval: [1234][1234][12345]
  // InByref_OutByref: [12345][12345][12345]
  return s;
}
```

Processing containers are a little bit more complex comparing to interfaces.
When byref is specified, an wrapper will be created to connect to the original container.
When byval is specified, a copy will be created.
Therefore according to Byref or Byval, the object retrieved from lifecycle may be an IRpcWrapperBase^ or may not, unlike interfaces all remote objects are wrappers.

Understand what the test case trying to say, you are not allowed to change:
- The content of the sample, unless it doesn't build.
- Workflow parser.
- Workflow compiling.
- Workflow to C++ code generation.

Create 4 samples first, add one single case to `IndexRpc.txt` once a time, therefore to execute the following instructions one after another.
So you should commit and push 4 times, and in the first commit, 4 samples will be added but only one is in used.
Do them in the order as listed at the beginning.
`Rpc(B|Unb)oxBy(val|ref)` should be in the highest priority attemp to fix, as these 4 C++ functions are directly called in generated wrapper classes written Workflow script.

You are highly possibly need to fix implementation of `RpcDualLifecycleMock` and its connected interfaces if sample fails in either `RuntimeTest` or `CppTest*`.
- The comment in the sample describes how `RpcDualLifecycleMock` and the generated C++ code is supposed to work.
  - The generated C++ code is very straight forward, if it fails, check `RpcDualLifecycleMock` first.
- If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
- Pass all unit test, fix any test failure including pre-existings.
- After finishing everything, git commit and git push to the current branch.
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
