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

## STEP 4: Expose boxing helpers to Workflow as `system::IRpcLifeCycle` static methods (reflection-safe) [DONE]
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

## STEP 5: Replace text-printing + parsing with direct `Ptr<WfModule>` construction [DONE]
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

## STEP 6: Generate deterministic IDs and lookup tables from existing `WfRpcMetadata` [DONE]
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

## STEP 7: Build a generation model from `metadataModule` (property-level Byref is authoritative for accessors) [DONE]
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

## Fixing attempt No.1
- Previous change failed because `BuildInterfaceModels` appended `RpcPropertyModel`, `RpcMethodModel`, `RpcEventModel`, and `RpcInterfaceModel` instances into `vl::collections::List` by copy. These models contain nested `vl::collections::List` members, so their implicit copy constructors are deleted when `List` becomes move-only, producing `error C2280` while compiling `Source\Analyzer\WfAnalyzer_GenerateRpc.cpp`.
- I changed every affected append site in `Source\Analyzer\WfAnalyzer_GenerateRpc.cpp` to use `Move(...)` when transferring those temporary models into their destination lists.
- This should fix the break because `vl::collections::List::Add` can then consume the move-only model values without requiring deleted copy constructors, matching the container semantics used elsewhere in this codebase.

## Fixing attempt No.2
- The previous fix failed because `Move(...)` is not a helper available in this translation unit or the repo-wide utility headers included by `Source\Analyzer\WfAnalyzer_GenerateRpc.cpp`, so the compiler reported `error C3861: 'Move': identifier not found`.
- I replaced those calls with `std::move(...)`, which is the move utility actually available through the existing C++ standard library includes already pulled in by the project headers.
- This should solve the remaining break because it preserves the intended move semantics from attempt 1 while using the valid, standard move function that `vl::collections::List::Add` accepts.

## Fixing attempt No.3
- After the builds succeeded, `CompilerTest_LoadAndCompile` still failed during `GenerateModuleRpc` because the emitted wrapper AST used `new interface` for `system::IRpcObjectOps` / `system::IRpcObjectEventOps`, and it emitted static helper calls as `system::IRpcLifeCycle.Rpc...(...)`. Workflow rejected those constructs with errors including `A23` (no constructor), `A6` (arguments not allowed in new interface expression), and `A7` (type symbol used as an expression).
- I reworked the generator so the endpoint ops are emitted as named Workflow classes with constructors (`RpcEndpointObjectOps`, `RpcEndpointObjectEventOps`) plus separate factory functions that instantiate them with `new ...^(lc)`. I also changed lifecycle helper calls to use Workflow static-call syntax via `system::IRpcLifeCycle::Rpc...(...)`.
- This should fix the code-generation failure because it matches Workflow’s proven syntax patterns: constructors are invoked on named classes, and static methods are referenced through `::` child expressions instead of member-expression syntax.

## Fixing attempt No.4
- The class-based rework in attempt 3 still failed: Workflow reported `G8` override errors because named `class` declarations do not satisfy interface overrides for `system::IRpcObjectOps` / `system::IRpcObjectEventOps` in this context.
- I changed the endpoint ops surface again so `RpcEndpointObjectOps` and `RpcEndpointObjectEventOps` are emitted as named Workflow interfaces inheriting the reflected RPC interfaces, and the factories now return `new RpcEndpointObjectOps^ { ... }` / `new RpcEndpointObjectEventOps^ { ... }` with the actual state and overrides.
- This should fix the remaining semantic break because the anonymous implementations now target Workflow-defined interfaces with inherited RPC members, which is the same pattern used by valid `new Interface^ { override ... }` code elsewhere in the repository.

## Fixing attempt No.5
- Attempt 4 still failed because the reflected RPC interfaces themselves were registered with `BEGIN_INTERFACE_MEMBER_NOPROXY(...)`. Workflow then rejected `RpcEndpointObjectOps : system::IRpcObjectOps` and `RpcEndpointObjectEventOps : system::IRpcObjectEventOps` with `G6`, explicitly requiring proxy-capable interface metadata (`system::InterfaceProxy^` constructor support) for those base interfaces.
- I updated `Source\Library\WfLibraryReflection.cpp` so the implementable RPC interfaces (`vl::rpc_controller::IRpcIdSync`, `IRpcListOps`, `IRpcListEventOps`, `IRpcObjectOps`, `IRpcObjectEventOps`, and `IRpcController`) use `BEGIN_INTERFACE_MEMBER(...)` instead of the `NOPROXY` form. I kept `vl::rpc_controller::IRpcLifeCycle` on the `NOPROXY` path because this task only needs its static helpers to be callable from Workflow, not implemented in Workflow.
- This should resolve the proxy-related semantic errors by making the RPC callback interfaces inheritably visible to Workflow, while preserving the intended special handling for `system::IRpcLifeCycle`.

## Fixing attempt No.6
- Attempt 5 broke the C++ build because switching additional RPC interfaces to proxy-enabled reflection requires concrete `ValueInterfaceProxy<...>` specializations. The repository already provides proxy classes for interfaces like `ICoroutine` and `IAsync`, but not for the RPC callback interfaces, so MSVC failed in `Source\Library\WfLibraryReflection.cpp` when it tried to use missing `Create` members on `ValueInterfaceProxy<vl::rpc_controller::IRpcIdSync>`.
- I narrowed the proxy-enabled reflection set to only the interfaces that must be implemented from Workflow for this task (`vl::rpc_controller::IRpcIdSync`, `IRpcObjectOps`, `IRpcObjectEventOps`) and added matching proxy implementations in `Source\Library\WfLibraryReflection.h`. I reverted the unrelated RPC interfaces back to `NOPROXY`.
- This should restore buildability while still enabling the generated Workflow endpoint objects to implement the RPC callback interfaces that the wrapper factories need.

## Fixing attempt No.7
- After proxy support was added, the generated wrapper module still failed semantic analysis with `G6` / `A6`. The emitted intermediate interfaces (`RpcEndpointObjectOps`, `RpcEndpointObjectEventOps`) were unnecessary and actually introduced extra inheritance constraints that Workflow still rejected.
- I simplified the generator again so the factories now implement `system::IRpcObjectOps` and `system::IRpcObjectEventOps` directly using `new (::system::IRpcObjectOps^) { ... }` / `new (::system::IRpcObjectEventOps^) { ... }`, leveraging the newly added proxy-enabled reflection for those interfaces.
- This should eliminate the remaining wrapper-semantic failure by removing the problematic intermediate interface layer while preserving proxy-backed direct implementation of the reflected RPC interfaces.

## Fixing attempt No.8
- Attempt 7 proved too aggressive: direct `new (::system::IRpcObjectOps^) { ... }` / `new (::system::IRpcObjectEventOps^) { ... }` still triggered `A23` constructor errors during wrapper-module validation, which means Workflow continued to treat those reflected interfaces as unsuitable direct `new interface` targets even after proxy metadata was added.
- I restored the intermediate Workflow interface layer (`RpcEndpointObjectOps`, `RpcEndpointObjectEventOps`) so the generated anonymous implementations again target Workflow-defined interfaces that inherit the RPC callback contracts, while keeping the proxy-enabled reflection support introduced in attempts 5 and 6.
- This should give the validator the shape it expects: Workflow-defined interfaces as `new interface` targets, backed by reflected base interfaces that now expose proxy constructors.

## Fixing attempt No.9
- The remaining `RequestService` failure came from semantic errors in the generated wrapper module, not from C++ compilation. The parser log showed `G6`/`A6` on the intermediate Workflow interfaces (`RpcEndpointObjectOps`, `RpcEndpointObjectEventOps`) plus missing `IRpcLifeCycle::Rpc...` helpers in reflection output, which indicated the wrapper shape was still wrong and the library reflection changes needed to be rebuilt.
- I switched the generator back to direct anonymous implementations of `system::IRpcObjectOps` / `system::IRpcObjectEventOps`, removed the now-harmful intermediate interface declarations from the emitted module, and refreshed the directly related RPC header so the library reflection / helper changes are rebuilt into the test binaries.
- This should solve the blocker because the reflected RPC callback interfaces already have proxy implementations in C++, so the generated Workflow wrappers can target them directly once the updated reflection metadata (including `IRpcLifeCycle::RpcBox*` / `RpcUnbox*`) is actually rebuilt and loaded.

## Fixing attempt No.10
- After regenerating reflection metadata, the remaining semantic errors narrowed to plain `A6` on the two direct `new (system::IRpcObjectOps^) { ... }` / `new (system::IRpcObjectEventOps^) { ... }` expressions. That proved the direct reflected-interface approach is still invalid even with proxy constructors and visible `IRpcLifeCycle::Rpc...` helpers.
- I restored the generated intermediate Workflow interfaces (`RpcEndpointObjectOps`, `RpcEndpointObjectEventOps`) and their factory targets, while keeping the rebuilt reflection metadata changes from the previous attempt.
- This should work now because the original `G6` blocker was the stale reflection metadata; with proxy-capable `IRpcIdSync` / `IRpcObjectOps` / `IRpcObjectEventOps` and static lifecycle helper methods now generated into metadata, the Workflow-defined intermediate interfaces should inherit those contracts cleanly and remain valid `new interface` targets.

## Fixing attempt No.11
- The regenerated x86 wrapper text finally exposed the remaining parser issue precisely: the emitted `new interface` targets were printed as `new (::RpcEndpointObjectOps^)` / `new (::RpcEndpointObjectEventOps^)`. Existing Workflow samples use ordinary reference types there (`new IAdder^`, `new RpcTest::IService^`), and the parser was reporting `A6` before it ever got to semantic validation.
- I changed the generator’s type builder to emit normal `WfReferenceType` roots instead of `WfTopQualifiedType`, removing the leading global `::` prefix from generated type syntax while keeping qualified child fragments like `system::IRpcObjectOps` intact.
- This should clear the last `RequestService` blocker because the wrapper module will now print and reparse with the same type spelling style that the Workflow parser already accepts for `new interface`, casts, and declarations.

## Fixing attempt No.12
- The latest regenerated wrapper made the remaining `A6` source clear: the errors were no longer on the RPC wrapper interfaces themselves, but on `new (system::Dictionary^)()` / `new (system::Array^)()` inside the generated body. Those are interface types, so Workflow rejects constructor-call syntax on them as “arguments are not allowed in new interface expression”.
- I replaced those generated allocations with constructor expressions (`{}` in Workflow AST form) while keeping the declared target types as `system::Dictionary^` / `system::Array^`. The existing code still resizes and mutates the resulting collections the same way afterward.
- This should let the wrapper module pass semantic analysis because the generated collections now use Workflow’s collection-constructor syntax instead of invalid `new interface` constructor-call syntax.

# !!!FINISHED!!!

# !!!VERIFIED!!!
