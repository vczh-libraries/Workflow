# !!!PLANNING!!!

# UPDATES

# AFFECTED PROJECTS
- Build the solution in folder REPO-ROOT\Test\UnitTest (Debug|Win32).
- Build the solution in folder REPO-ROOT\Test\UnitTest (Debug|x64).
- Always Run UnitTest project LibraryTest (Debug|Win32).
- Always Run UnitTest project LibraryTest (Debug|x64).
- Always Run UnitTest project CompilerTest_GenerateMetadata (Debug|Win32).
- Always Run UnitTest project CompilerTest_GenerateMetadata (Debug|x64).
- Always Run UnitTest project CompilerTest_LoadAndCompile (Debug|x64).
- Always Run UnitTest project CppTest (Debug|Win32).
- Always Run UnitTest project CppTest (Debug|x64).
- Always Run UnitTest project CppTest_Metaonly (Debug|Win32).
- Always Run UnitTest project CppTest_Metaonly (Debug|x64).
- Always Run UnitTest project CppTest_Reflection (Debug|Win32).
- Always Run UnitTest project CppTest_Reflection (Debug|x64).

# EXECUTION PLAN

## STEP 1: Validate Workflow/runtime semantics that the generator must rely on
**What to change**
- Before changing generation logic, confirm each assumed Workflow construct is valid in this repo’s Workflow runtime/grammar by locating an existing usage (preferred) or validating via compilation in existing tests:
  - Map iteration syntax (used by `SyncIds`).
  - `system::Dictionary^` mutation API (e.g. `Set`) and the expected key/value direction for `SyncIds(ids : system::Dictionary^)`.
  - `system::Array^` construction + mutation (`Resize`, `Set`).
  - Cast syntax used in emitted expressions.
  - Closure/capture rules if we capture module-level variables inside generated implementations.
  - `attach(event, func...)` return value / token lifetime rules (whether the token must be stored to keep the attachment alive).

**Why**
- The plan relies on several Workflow surface semantics that, if wrong, can make generated wrappers compile but behave incorrectly (or fail compilation late).


## STEP 2: Lock down wrapper surface, Workflow-visible types, and wrapper shape (stateful vs anonymous)
**What to change**
- In `Source\Analyzer\WfAnalyzer_GenerateRpc.cpp`, rework `GenerateModuleRpc(WfLexicalScopeManager* manager)` so the wrapper module contains:
  - ID constants (`rpctype_*`, `rpcmethod_*`, `rpcevent_*`) and lookup maps (`rpcNameToId`, `rpcIdToName`).
  - Non-stub implementations of:
    - `func rpc_IRpcObjectOps(lc : system::IRpcLifeCycle*) : (system::IRpcObjectOps^)`
    - `func rpc_IRpcObjectEventOps(lc : system::IRpcLifeCycle*) : (system::IRpcObjectEventOps^)`
  - One wrapper factory per `@rpc:Interface`, e.g. `namespace RpcTest { func rpcwrapper_IService(lc : system::IRpcLifeCycle*) : (IService^) { ... } }`.
- Commit to *named Workflow classes* for ops and proxies, because we need reliable mutable state:
  - Hold table for `ObjectHold`.
  - (Potentially) retained handler tokens for `attach(...)` so outbound event forwarding stays alive.

**Why**
- `Test\Generated\RpcMetadata{32,64}\Wrapper_RequestService.txt` currently contains three `return null;` stubs, so there is no runtime behavior.
- Anonymous interface implementations make it ambiguous where mutable state lives; named classes make the lifetime and ownership explicit and reduce reliance on closure/capture semantics.

**Important detail: use Workflow-visible signatures**
From `Test\Resources\Baseline\Reflection{32,64}.txt`, the RPC interfaces are exposed as:
- `system::IRpcObjectOps.InvokeMethod(..., arguments : system::Array^) : system::Object`
- `system::IRpcObjectEventOps.InvokeEvent(..., arguments : system::Array^) : system::Void`
- `system::IRpcIdSync.SyncIds(ids : system::Dictionary^) : system::Void`
So generated wrapper code must use `system::Array^`, `system::Dictionary^`, and `system::Object` (never `vl::reflection::description::Value` in Workflow signatures).


## STEP 3: Ensure wrapper generation does not incorrectly block event interfaces
**What to change**
- Remove or rework any guards (e.g. `CanGenerateWrapperObject`) that prevent generating wrappers for interfaces containing events.

**Why**
- This must happen *before* wrapper generation (ops + proxies). Otherwise event-containing interfaces can be silently skipped and later steps can appear “done” while output is incomplete.


## STEP 4: Expose boxing helpers to Workflow as `system::IRpcLifeCycle` static methods (reflection-safe)
**What to change**
- Add Workflow-visible entry points for boxing/unboxing helpers implemented in `Source\Library\WfLibraryRpc.cpp`:
  - `RpcBoxByref`, `RpcUnboxByref`, `RpcBoxByval`, `RpcUnboxByval`.
- Register them in `Source\Library\WfLibraryReflection.cpp` under `BEGIN_INTERFACE_MEMBER_NOPROXY(vl::rpc_controller::IRpcLifeCycle)` using `CLASS_MEMBER_STATIC_METHOD(...)`.
- Keep these helpers declared in `WfLibraryRpc.h` and implemented *out-of-line* in `WfLibraryRpc.cpp` to avoid header-ordering pitfalls and to match the codebase’s header style.
- Clarify the boundary explicitly:
  - Boxing/unboxing helpers are C++ and must be reflection-registered to be callable from Workflow.
  - Ops factories (`rpc_IRpcObjectOps` / `rpc_IRpcObjectEventOps`) are generated Workflow functions in the wrapper module; they are not C++ reflection registrations.

**Why**
- These helper functions do not appear in `Reflection{32,64}.txt` today.
- Once wrapper bodies stop being stubs, they must call these helpers to correctly serialize/deserialize arguments and return values based on `@rpc:Byref` (default is byval). Without reflection registration, `CompilerTest_LoadAndCompile` will fail.

**Concrete code sketch (header declaration + out-of-line definitions + reflection registration)**
1) Declare static forwarding functions in the interface header:

```cpp
// File: Source\Library\WfLibraryRpc.h

namespace vl
{
	namespace rpc_controller
	{
		class IRpcLifeCycle
			: public virtual vl::reflection::IDescriptable
			, public vl::reflection::Description<IRpcLifeCycle>
		{
		public:
			// existing pure virtual members...

			static vl::reflection::description::Value RpcBoxByref(const vl::reflection::description::Value& trivial, IRpcLifeCycle* lc);
			static vl::reflection::description::Value RpcUnboxByref(const vl::reflection::description::Value& serializable, IRpcLifeCycle* lc);
			static vl::reflection::description::Value RpcBoxByval(const vl::reflection::description::Value& trivial, IRpcLifeCycle* lc);
			static vl::reflection::description::Value RpcUnboxByval(const vl::reflection::description::Value& serializable, IRpcLifeCycle* lc);
		};
	}
}
```

2) Define them out-of-line:

```cpp
// File: Source\Library\WfLibraryRpc.cpp

vl::reflection::description::Value vl::rpc_controller::IRpcLifeCycle::RpcBoxByref(const vl::reflection::description::Value& trivial, IRpcLifeCycle* lc)
{
	return vl::rpc_controller::RpcBoxByref(trivial, lc);
}

// (and the other three)
```

3) Register in reflection:

```cpp
// File: Source\Library\WfLibraryReflection.cpp

BEGIN_INTERFACE_MEMBER_NOPROXY(vl::rpc_controller::IRpcLifeCycle)
	CLASS_MEMBER_PROPERTY_READONLY_FAST(Controller)
	CLASS_MEMBER_METHOD(RefToPtr, { L"ref" })
	CLASS_MEMBER_METHOD(PtrToRef, { L"obj" })
	CLASS_MEMBER_METHOD(RegisterService, { L"fullName" _ L"service" })
	CLASS_MEMBER_METHOD(RequestService, { L"fullName" })

	CLASS_MEMBER_STATIC_METHOD(RpcBoxByref, { L"trivial" _ L"lc" })
	CLASS_MEMBER_STATIC_METHOD(RpcUnboxByref, { L"serializable" _ L"lc" })
	CLASS_MEMBER_STATIC_METHOD(RpcBoxByval, { L"trivial" _ L"lc" })
	CLASS_MEMBER_STATIC_METHOD(RpcUnboxByval, { L"serializable" _ L"lc" })
END_INTERFACE_MEMBER(vl::rpc_controller::IRpcLifeCycle)
```


## STEP 5: Replace text-printing + parsing with direct `Ptr<WfModule>` construction
**What to change**
- In `Source\Analyzer\WfAnalyzer_GenerateRpc.cpp::GenerateModuleRpc`:
  - Delete the `GenerateToStream([&](TextWriter& writer){ ... writer.WriteLine(...) ... })` pathway.
  - Delete the `ParseModule(source, manager->workflowParser)` call.
  - Construct the wrapper module as a `vl::Ptr<vl::workflow::WfModule>` directly and return it.

**Why**
- The current approach “prints text then parses it back” is fragile.
- `TestRpcCompiler.cpp` prints the returned module using `WfPrint(wrapperModule, ...)`. AST-first generation matches the test harness and reduces escaping/quoting risks.

**AST construction notes**
- Keep any AST helper functions local (anonymous namespace in the `.cpp`) and consistent with existing direct-construction patterns.
- Preserve fragile-but-required details:
  - String literal token values must include quotes (printing uses the token verbatim).
  - Attribute extraction should traverse `WfDeclaration::attributes` and read `WfAttribute::category` / `name`.
- Prefer reusing existing helper functions (name mangling, string escaping) from the current implementation to avoid duplication/drift.


## STEP 6: Generate deterministic IDs and lookup tables from existing `WfRpcMetadata`
**What to change**
- Use `manager->rpcMetadata->typeFullNames`, `methodFullNames`, `eventFullNames` (already populated deterministically in `WfAnalyzer_ValidateRPC.cpp`) to:
  - Assign IDs: types first, then methods, then events, from 0 upward.
  - Emit variables:
    - `var rpctype_<mangled> : int = <id>;`
    - `var rpcmethod_<mangled> : int = <id>;`
    - `var rpcevent_<mangled> : int = <id>;`
  - Emit lookup maps:
    - `rpcNameToId : int[string]` mapping original full names to IDs.
    - `rpcIdToName : string[int]` mapping IDs back to names.

**Why**
- ID stability is part of the RPC contract and must match the existing ordering logic in `WfAnalyzer_ValidateRPC.cpp`.


## STEP 7: Build a generation model from `metadataModule` (property-level Byref is authoritative for accessors)
**What to change**
- In `WfAnalyzer_GenerateRpc.cpp`, build an intermediate C++ model by traversing `manager->rpcMetadata->metadataModule` declarations:
  - Interfaces: `WfClassDeclaration` with `kind == WfClassKind::Interface` and `@rpc:Interface`.
  - Properties: `WfPropertyDeclaration` (getter/setter method names + property-level `@rpc:Byref`).
  - Methods: `WfFunctionDeclaration` (return/parameter-level `@rpc:Byref` for non-property methods).
  - Events: `WfEventDeclaration` (events are always boxed/unboxed byval).
- During model extraction, build the maps:
  - `getterMethodName -> propertyByrefFlag`
  - `setterMethodName -> propertyByrefFlag`
- Classify each method as:
  - Normal method (use method-level return/param attributes)
  - Property getter/setter (ignore method-level Byref; use property-level Byref for getter return and setter value)

**Why**
- Requirement: “For property getter and setter, see the property. You need to read property definition to know which methods are getters and setters, the method names mean nothing.”


## STEP 8: Implement endpoint-side ops factories with full model-driven dispatch (no placeholders)
**What to change**
- Generate named Workflow classes in the wrapper module:
  - `class RpcEndpointObjectOps : system::IRpcObjectOps`
  - `class RpcEndpointObjectEventOps : system::IRpcObjectEventOps`
- Generate factories:
  - `func rpc_IRpcObjectOps(lc : system::IRpcLifeCycle*) : (system::IRpcObjectOps^) { return new RpcEndpointObjectOps(lc); }`
  - `func rpc_IRpcObjectEventOps(lc : system::IRpcLifeCycle*) : (system::IRpcObjectEventOps^) { return new RpcEndpointObjectEventOps(lc); }`
- In both ops classes:
  - Implement `SyncIds(ids)` by publishing all `(fullName -> id)` pairs into `ids`.
  - Implement `RequestService(typeId)` by `rpcIdToName[typeId]` then `lc.RequestService(fullName)` then `lc.PtrToRef(service)`.
- For `InvokeMethod` and `InvokeEvent`:
  - Do **not** emit `raise "Not Implemented!"` stubs.
  - Emit an `if / else if / else` chain driven directly from the generation model:
    - Compare `methodId` / `eventId` against the corresponding `rpcmethod_*` / `rpcevent_*` constants.
    - Deserialize arguments from `arguments[i]` using `system::IRpcLifeCycle.RpcUnboxByref/Byval` according to the model.
    - Convert `ref` to an object pointer via `lc.RefToPtr(ref)` and cast to the correct interface.
    - Call the method / invoke the event.
    - For methods, serialize the return value via `system::IRpcLifeCycle.RpcBoxByref/Byval` according to the model.
  - In the final `else`, raise a clear error indicating unknown ID.
- Implement `ObjectHold(ref, hold)` as protocol-critical:
  - Maintain a hold table inside `RpcEndpointObjectOps` keyed by `system::RpcObjectReference` to a strong reference of the target object returned by `lc.RefToPtr(ref)`.
  - On `hold == true`, insert (or overwrite) the entry; on `hold == false`, remove it.
- Scope-gate `InvokeMethodAsync` explicitly:
  - If intentionally out of scope for this task, raise a consistent “Not Supported” error and document that tests do not cover async.

**Why**
- This removes the core correctness risk where the plan “looks complete” but runtime behavior is still stubbed.
- `ObjectHold` prevents premature GC / lifetime bugs; treating it as optional risks silent correctness failures.


## STEP 9: Implement caller-side `rpcwrapper_*` proxies (stateful, event token retention)
**What to change**
- For each `@rpc:Interface`, generate a named Workflow class `RpcCaller_<InterfaceName> : <InterfaceName>` capturing:
  - `lc : system::IRpcLifeCycle*`
  - `controller : system::IRpcController^` (from `lc.Controller`)
  - `ref : system::RpcObjectReference` (from `controller.RequestService(rpctype_...)`)
  - A field to retain outbound `attach(...)` handler tokens if required by Workflow semantics.
- Generate `rpcwrapper_<InterfaceName>(lc : system::IRpcLifeCycle*) : (Interface^)` that constructs and returns the proxy.
- For each method:
  - Allocate `system::Array^` arguments and fill it with boxed args using `system::IRpcLifeCycle.RpcBoxByref/Byval` per the model.
  - Call `controller.InvokeMethod(ref, rpcmethod_..., args)`.
  - Unbox return via `RpcUnboxByref/Byval` per the model and cast to the declared return type.
- For each event:
  - Attach a handler to the proxy’s own event to forward outbound events:
    - Box args byval only.
    - Call `controller.InvokeEvent(ref, rpcevent_..., boxedArgs)`.
  - Retain the handler token if required.

**Why**
- Aligns with the task doc’s expectation (named classes) and ensures mutable state (event token retention) has an explicit home.


# TEST PLAN

## Baselines and what may change
- Wrapper metadata baselines:
  - `Test\Resources\Baseline\RpcMetadata32\...`
  - `Test\Resources\Baseline\RpcMetadata64\...`
  - Expect wrapper output files such as `Wrapper_RequestService.txt` to change once stubs are removed.
- Reflection baselines (only if Step 4 changes reflection output):
  - `Test\Resources\Baseline\Reflection32.txt`
  - `Test\Resources\Baseline\Reflection64.txt`

## Baseline update procedure (when expected)
- Run the generators/tests that produce:
  - `Test\Generated\RpcMetadata32\...`
  - `Test\Generated\RpcMetadata64\...`
- Copy the updated generated files over the corresponding `Test\Resources\Baseline\...` files.
- Re-run the same tests to confirm clean baseline.

## Verification sequence (per #AFFECTED PROJECTS)
1) Build both Debug|Win32 and Debug|x64 solutions.
2) Run unit tests in the listed order (Always Run items first).

## Key gates / what they prove
1) **CompilerTest_LoadAndCompile (Debug|x64)**
- Primary gate for wrapper module validity: it regenerates wrappers and compiles them together with RPC samples.
- Confirms wrapper code only uses Workflow-visible APIs and that `system::IRpcLifeCycle.Rpc(Box|Unbox)By*` are callable.

2) **CompilerTest_GenerateMetadata (Debug|Win32 and Debug|x64)**
- Confirms reflection output is stable.
- If Step 4 adds new static methods, ensure they appear under `system::IRpcLifeCycle`.

## Manual sanity checks (fast, before the full suite)
- Open regenerated `Test\Generated\RpcMetadata{32,64}\Wrapper_RequestService.txt` and confirm:
  - `rpc_IRpcObjectOps` and `rpc_IRpcObjectEventOps` return real objects (not `null`).
  - `rpcwrapper_*` returns a real proxy implementation.
  - No remaining placeholder stubs for dispatch (`Not Implemented!`).
  - All boxing/unboxing calls use `system::IRpcLifeCycle.Rpc(Box|Unbox)By(ref|val)` and Workflow-visible types.

# !!!FINISHED!!!
