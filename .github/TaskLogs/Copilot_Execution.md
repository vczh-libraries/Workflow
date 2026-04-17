# !!!EXECUTION!!!

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
- Before changing generation logic, confirm each assumed Workflow construct is valid in this repo’s Workflow runtime/grammar by locating an existing usage (preferred) or validating via compilation in existing tests:
  - Map iteration syntax (used by `SyncIds`).
  - `system::Dictionary^` mutation API (e.g. `Set`) and the expected key/value direction for `SyncIds(ids : system::Dictionary^)`.
  - `system::Array^` construction + mutation (`Resize`, `Set`).
  - Cast syntax used in emitted expressions.
  - Workflow class member methods can read module-level declarations (e.g. `rpcmethod_*`, `rpctype_*`, `rpcNameToId`, `rpcIdToName`).
  - Static methods registered with `CLASS_MEMBER_STATIC_METHOD(...)` under `BEGIN_INTERFACE_MEMBER_NOPROXY(vl::rpc_controller::IRpcLifeCycle)` are callable from Workflow as `system::IRpcLifeCycle.RpcBoxByref(...)`, `system::IRpcLifeCycle.RpcUnboxByref(...)`, etc.
  - `system::RpcObjectReference` is usable as a `system::Dictionary^` key in Workflow (needed for `ObjectHold` hold-table).
  - `attach(event, func...)` return value / token lifetime rules (whether the token must be stored to keep the attachment alive).

- Failure protocol (apply immediately, and record the finding/decision in `# FIXING ATTEMPTS` before proceeding to later steps):
  - If module-level access is not allowed, pass IDs/tables into generated class constructors and store them as fields.
  - If `NOPROXY` static methods are not callable from Workflow, rework STEP 4 so boxing helpers become Workflow-callable (while still anchored on `IRpcLifeCycle` if possible), and update wrapper generation call sites accordingly.
  - If `system::RpcObjectReference` is not a valid Workflow dictionary key, key the hold table by a deterministic surrogate (e.g. a numeric ID or a string derived from the reference, whichever is available in Workflow) and document the choice.
  - If `attach(...)` tokens do not need retention, do not generate retention fields; otherwise generate fields and assign tokens.
  - If any container API / cast syntax differs, update emitted codegen to match the proven syntax and re-run `CompilerTest_LoadAndCompile` as the primary gate.

## STEP 2: Lock down wrapper surface, Workflow-visible types, and wrapper shape (stateful vs anonymous)
- In `Source\Analyzer\WfAnalyzer_GenerateRpc.cpp`, rework `GenerateModuleRpc(WfLexicalScopeManager* manager)` so the wrapper module contains:
  - ID constants (`rpctype_*`, `rpcmethod_*`, `rpcevent_*`) and lookup maps (`rpcNameToId`, `rpcIdToName`).
  - Non-stub implementations of:
    - `func rpc_IRpcObjectOps(lc : system::IRpcLifeCycle*) : (system::IRpcObjectOps^)`
    - `func rpc_IRpcObjectEventOps(lc : system::IRpcLifeCycle*) : (system::IRpcObjectEventOps^)`
  - One wrapper factory per `@rpc:Interface`, e.g. `namespace RpcTest { func rpcwrapper_IService(lc : system::IRpcLifeCycle*) : (IService^) { ... } }`.
- Commit to named Workflow classes for ops and proxies, because we need reliable mutable state:
  - Hold table for `ObjectHold`.
  - (Potentially) retained handler tokens for `attach(...)` so outbound event forwarding stays alive.
- Use Workflow-visible signatures (must match reflection):
  - `system::IRpcObjectOps.InvokeMethod(..., arguments : system::Array^) : system::Object`
  - `system::IRpcObjectEventOps.InvokeEvent(..., arguments : system::Array^) : system::Void`
  - `system::IRpcIdSync.SyncIds(ids : system::Dictionary^) : system::Void`

## STEP 3: Ensure wrapper generation does not incorrectly block event interfaces
- In `Source\Analyzer\WfAnalyzer_GenerateRpc.cpp`, remove or rework the guard `CanGenerateWrapperObject` (it rejects interfaces containing `WfEventDeclaration`), so event-enabled wrappers are generated.

## STEP 4: Expose boxing helpers to Workflow as `system::IRpcLifeCycle` static methods (reflection-safe)
- Add Workflow-visible entry points for boxing/unboxing helpers implemented in `Source\Library\WfLibraryRpc.cpp`:
  - `RpcBoxByref`, `RpcUnboxByref`, `RpcBoxByval`, `RpcUnboxByval`.
- Register them in `Source\Library\WfLibraryReflection.cpp` under `BEGIN_INTERFACE_MEMBER_NOPROXY(vl::rpc_controller::IRpcLifeCycle)` using `CLASS_MEMBER_STATIC_METHOD(...)`.
- Keep these helpers declared in `WfLibraryRpc.h` and implemented out-of-line in `WfLibraryRpc.cpp`.

Concrete code sketch (header declaration + out-of-line definitions + reflection registration)
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
- In `Source\Analyzer\WfAnalyzer_GenerateRpc.cpp::GenerateModuleRpc`:
  - Delete the `GenerateToStream([&](TextWriter& writer){ ... writer.WriteLine(...) ... })` pathway.
  - Delete the `ParseModule(source, manager->workflowParser)` call.
  - Construct the wrapper module as a `vl::Ptr<vl::workflow::WfModule>` directly and return it.

- AST construction notes (fragile, but required for correctness):
  - Keep AST helper builders in an anonymous namespace in `WfAnalyzer_GenerateRpc.cpp`.
  - String literal token values must include surrounding quotes (token text is printed verbatim by `WfPrint`).
  - Attribute traversal must use `WfDeclaration::attributes` and `WfAttribute::category` / `WfAttribute::name`.
  - Reuse existing mangling / escaping helpers to avoid duplication and drift.
  - When dereferencing `vl::Ptr<T>`, use `.Obj()` (not `*ptr`).

## STEP 6: Generate deterministic IDs and lookup tables from existing `WfRpcMetadata`
- Use `manager->rpcMetadata->typeFullNames`, `methodFullNames`, `eventFullNames` to:
  - Assign IDs: types first, then methods, then events, from 0 upward.
  - Emit variables:
    - `var rpctype_<mangled> : int = <id>;`
    - `var rpcmethod_<mangled> : int = <id>;`
    - `var rpcevent_<mangled> : int = <id>;`
  - Emit lookup maps:
    - `rpcNameToId : int[string]` mapping original full names to IDs.
    - `rpcIdToName : string[int]` mapping IDs back to names.
- Determinism contract (must preserve):
  - Confirm these lists are populated deterministically by `Source\Analyzer\WfAnalyzer_ValidateRPC.cpp` before emission.
  - ID ordering (types then methods then events, 0-based) is part of the RPC contract.

## STEP 7: Build a generation model from `metadataModule` (property-level Byref is authoritative for accessors)
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

## STEP 8: Implement endpoint-side ops factories with full model-driven dispatch (no placeholders)
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
  - Emit an `if / else if / else` chain driven directly from the generation model:
    - Compare `methodId` / `eventId` against the corresponding `rpcmethod_*` / `rpcevent_*` constants.
    - Deserialize arguments from `arguments[i]` using `system::IRpcLifeCycle.RpcUnboxByref/Byval` according to the model.
    - Convert `ref` to an object pointer via `lc.RefToPtr(ref)` and cast to the correct interface.
    - Call the method / invoke the event.
    - For methods, serialize the return value via `system::IRpcLifeCycle.RpcBoxByref/Byval` according to the model.
  - In the final `else`, raise a clear error indicating unknown ID.
- Implement `ObjectHold(ref, hold)`:
  - Maintain a hold table inside `RpcEndpointObjectOps` keyed by `system::RpcObjectReference` if supported (per STEP 1); otherwise key by the surrogate decided in STEP 1.
  - On `hold == true`, insert (or overwrite) the entry; on `hold == false`, remove it.
- `InvokeMethodAsync`:
  - If intentionally out of scope for this task, raise a consistent “Not Supported” error and document that tests do not cover async.

## STEP 9: Implement caller-side `rpcwrapper_*` proxies (stateful, event token retention)
- For each `@rpc:Interface`, generate a named Workflow class `RpcCaller_<InterfaceName> : <InterfaceName>` capturing:
  - `lc : system::IRpcLifeCycle*`
  - `controller : system::IRpcController^` (from `lc.Controller`)
  - `ref : system::RpcObjectReference` (from `controller.RequestService(...)`):
    - If `IRpcController.RequestService` accepts `typeId : int`, call `controller.RequestService(rpctype_...)`.
    - If it only accepts `fullName : string`, call `controller.RequestService(rpcIdToName[rpctype_...])`.
  - A field to retain outbound `attach(...)` handler tokens if required by Workflow semantics (decided by STEP 1).
- Generate `rpcwrapper_<InterfaceName>(lc : system::IRpcLifeCycle*) : (Interface^)` that constructs and returns the proxy.
- For each method:
  - Allocate `system::Array^` arguments and fill it with boxed args using `system::IRpcLifeCycle.RpcBoxByref/Byval` per the model.
  - Call `controller.InvokeMethod(ref, rpcmethod_..., args)`.
  - Unbox return via `RpcUnboxByref/Byval` per the model and cast to the declared return type.
- For each event:
  - Attach a handler to the proxy’s own event to forward outbound events:
    - Box args byval only.
    - Call `controller.InvokeEvent(ref, rpcevent_..., boxedArgs)`.
  - Retain the handler token iff STEP 1 confirms it is required; document the chosen branch in `# FIXING ATTEMPTS`.

# TEST PLAN

## Baselines and what may change / be created
- Wrapper metadata baselines:
  - `Test\Resources\Baseline\RpcMetadata32\Wrapper_RequestService.txt`
  - `Test\Resources\Baseline\RpcMetadata64\Wrapper_RequestService.txt`
  - If these wrapper baseline files do not exist yet, create them by copying from the generated outputs after STEP 1–9 changes.
- Reflection baselines (only if STEP 4 changes reflection output):
  - `Test\Resources\Baseline\Reflection32.txt`
  - `Test\Resources\Baseline\Reflection64.txt`

## Baseline update / creation procedure
- Run the generators/tests that produce:
  - `Test\Generated\RpcMetadata32\Wrapper_RequestService.txt`
  - `Test\Generated\RpcMetadata64\Wrapper_RequestService.txt`
- Copy the updated generated files over the corresponding `Test\Resources\Baseline\...` files.
- Re-run the same tests to confirm the baseline is clean.

## Verification sequence (per # AFFECTED PROJECTS)
1) Build both Debug|Win32 and Debug|x64 solutions (use repo scripts per build/run rules, e.g. `.github\Scripts\copilotBuild.ps1`).
2) Run unit tests in the listed order (use `.github\Scripts\copilotExecute.ps1`).

## Key gates / what they prove
1) **CompilerTest_LoadAndCompile (Debug|x64)**
- Primary gate for wrapper module validity: it regenerates wrappers and compiles them together with RPC samples.
- Confirms wrapper code uses Workflow-visible APIs and that `system::IRpcLifeCycle.Rpc(Box|Unbox)By*` are callable.

2) **CompilerTest_GenerateMetadata (Debug|Win32 and Debug|x64)**
- Confirms reflection output is stable.
- If STEP 4 adds new static methods, ensure they appear under `system::IRpcLifeCycle`.

## Manual sanity checks (fast, before the full suite)
- Open regenerated `Test\Generated\RpcMetadata{32,64}\Wrapper_RequestService.txt` and confirm:
  - `rpc_IRpcObjectOps` and `rpc_IRpcObjectEventOps` return real objects (not `null`).
  - `rpcwrapper_*` returns a real proxy implementation.
  - No remaining placeholder stubs for dispatch (`return null;`, `raise "Not Implemented!"`, etc.).

## Coverage note
- Current RPC tests may not fully exercise event forwarding paths; treat event handling as “compile-gated” unless additional event-containing RPC samples are covered by existing tests.

# FIXING ATTEMPTS
- None yet.

# !!!FINISHED!!!

