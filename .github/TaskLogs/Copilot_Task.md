# !!!TASK!!!

# PROBLEM DESCRIPTION
Two Wrapper_RequestService.txt both have 3 function returns null, your work is to implement it. The function generating these workflow script functions have some implementation but it is incorrect. You will need to follow these.

Here are what you have done:
- Since the original code will compile with the generated wrappers, so it doesn't need to include anything from `rpcMetadata`.
- The generated wrapper will be in a module called `RpcMetadata`, which is the same as the one in `rpcMetadata`.
- Global variables of `int`
  - `rpctype_xxx`, `rpcmethod_xxx` and `rpcevent_xxx`, all comes from `WfRpcMetadata`.
  - Mangling rule: `::` -> `__` and `.` -> `_`.
  - List type followed by method followed by event, values from 0 ascending even across group.

Here are what you have not done (mentioned functions are created but the implementation is not right)
- Global lookup tables (commit to the real types):
  - `rpcNameToId : int[string]` mapping the original full names (`::` and `.`) to IDs.
  - `rpcIdToName : string[int]` mapping IDs back to names.
- Implementations of endpoint-side ops interfaces, global function `rpc_IRpcXXXOps` returning `IRpcXXXOps^`:
  - `IRpcObjectOps`: Call `Rpc(UnbB)oxBy(ref|val)` according to if `@rpc:Byref` is attached or not (doesn't matter if `@rpc:Byval` is attached or not because it is the default value):
    - For property getter and setter, see the property. You need to read property definition to know which methods are getters and setters, the method names mean nothing.
    - For method return type, see the method.
    - For method argument, see the argument.
  - `IRpcObjectEventOps`: Call `Rpc(Unb|B)oxByval` for argument serialization.
  - These functions are not reflectable yet, you need to register them as `IRpcLifeCycle`'s static functions.
    - Boxing/unboxing helpers: `RpcBoxByref`, `RpcUnboxByref`, `RpcBoxByval`, `RpcUnboxByval`.
    - Ops factories: `rpc_IRpcObjectOps`, `rpc_IRpcObjectEventOps`.
  - Ops factories take an input of `IRpcLifeCycle*`.
  - Those global variables help to know which ID is which.
- Implementations of caller-side `@rpc:Interface` interface proxies:
  - If the function name is `my::favorite::namespace_name::interface_name`, the function will be `my::favorite::namespace_name::rpcwrapper_interface_name`.
  - The function returns `interface_name^`.
  - The function takes a `IRpcLifeCycle*` as a parameter and redirects all methods/events to the controller, following the same byval/byref rules above.
- Caller-side outbound calls:
  - `IRpcObjectOps` takes method arguments from the proxy and sends them to the remote.
  - `IRpcObjectEventOps` takes event arguments from the proxy; these events are triggered locally on the proxy, so each generated proxy must `attach(...)` its own events to forward byval-serialized args to `controller.InvokeEvent(...)`.
- Endpoint-side inbound calls:
  - `IRpcObjectOps` deserializes arguments and calls methods on local objects (either real objects or proxies created by `IRpcLifeCycle::RefToPtr`).
  - `IRpcObjectEventOps` deserializes event args (always byval) and invokes events on the local target object.
  - Callee side does not hook events from the actual service object; lifecycle-managed event hookup is deferred to future `IRpcLifeCycle` work.
- No more global variable to create.
- Ops interface implementations are for both service and client.
- You cannot assume the ops implementation will be registered to the same `IRpcLifeCycle` instance with the one passed to the function to create it.

In these implementation, you will need to check out type name, method name, event name, read the original declaration, generate different code according to declaration and attributes of each type/method/event. All information can be retrieved from the WfRpcContext::metadataModule AST. Every single method return type or method argument or event argument have different types and different attributes, that is why you need `Rpc(UnbB)oxBy(ref|val)` functions. Follow the instruction to make them visible to the Workflow compiler.

WfAnalyzer_GenerateRpc.cpp currently has a stub implementation that creates Wrapper_RequestService.txt, but it is implemented in a completely wrong way. A correct way to make a workflow script is to build a Ptr<WfModule> and then use WfPrint function to convert the AST to text. So you need to completely remove the text printing part in GenerateModuleRpc, but you can use the existing idea and the output Wrapper_RequestService.txt for reference.

Test case is already created in Rpc\RequestService.txt. I don't think you need to change it, and the unit test should already picking it up. To test the code, follow `Project.md` verification requirements (at minimum: LibraryTest, CompilerTest_GenerateMetadata, CompilerTest_LoadAndCompile (x64 only), CppTest, CppTest_Metaonly, CppTest_Reflection; build and run both Debug|Win32 and Debug|x64 except where noted).

# UPDATES

# INSIGHTS AND REASONING
## What is broken today (evidence)
- `Source\\Analyzer\\WfAnalyzer_GenerateRpc.cpp` currently generates wrapper code by manually writing text into a string (`writer.WriteLine(...)`) and then calling `ParseModule(source, ...)`.
  - This is fragile and already incorrect for the required logic (boxing/unboxing decisions, property getter/setter mapping, event dispatch, etc.).
  - It also contradicts the requirement to “build a `Ptr<WfModule>` and then use `WfPrint`”.
- The generated wrapper outputs (`Test\\Generated\\RpcMetadata{32,64}\\Wrapper_RequestService.txt`) show three stubbed functions returning `null`:
  - `rpc_IRpcObjectOps(...)`
  - `rpc_IRpcObjectEventOps(...)`
  - `RpcTest::rpcwrapper_IService(...)`
  These stubs compile but do not provide any runtime behavior.
- `Source\\Analyzer\\WfAnalyzer_GenerateRpc.cpp` has a guard (`CanGenerateWrapperObject`) that blocks wrapper generation for interfaces containing events. This must be removed or reworked, otherwise required event-enabled wrappers will never be generated.
- The RPC metadata module (`manager.rpcMetadata->metadataModule`) already contains the information needed to generate correct wrappers:
  - Properties are represented as `WfPropertyDeclaration` containing explicit getter/setter method names (see `WfAnalyzer_ValidateRPC.cpp`, `GenerateInterfaceDecl`), so property semantics must be derived from the property node (not from method names).
  - Byref/Byval attributes are attached to properties, methods, and parameters in AST.

## Goal
Generate a Workflow module `module RpcMetadata;` (wrapper module) that:
1) Defines stable integer IDs (`rpctype_*`, `rpcmethod_*`, `rpcevent_*`) and name/id lookup tables.
2) Implements both caller-side and callee-side RPC ops wiring using those IDs and the metadata AST.
3) Uses `RpcBoxByref/RpcUnboxByref/RpcBoxByval/RpcUnboxByval` consistently according to `@rpc:Byref` (default is Byval).
4) Is generated by directly constructing a `Ptr<WfModule>` AST (no text printing + parsing).

## Proposed architecture
### A. Split “metadata” vs “wrapper” responsibilities
- Keep `manager->rpcMetadata->metadataModule` as the schema module (already built in `WfAnalyzer_ValidateRPC.cpp`).
- Rework `GenerateModuleRpc(WfLexicalScopeManager* manager)` to build a *second* module: the wrapper module (also named `RpcMetadata`, matching current behavior/expectation).
  - This wrapper module contains:
    - Global constants and lookup dictionaries.
    - Endpoint-side ops factories (global functions):
      - `rpc_IRpcObjectOps(lc : system::IRpcLifeCycle*) : system::IRpcObjectOps^`
      - `rpc_IRpcObjectEventOps(lc : system::IRpcLifeCycle*) : system::IRpcObjectEventOps^`
    - One `rpcwrapper_*` factory function per RPC interface under the interface’s namespace.

### B. Build wrapper module using AST, not text
- Replace the current “`GenerateToStream` + manual text + `ParseModule`” implementation with a direct AST builder:
  - `auto module = Ptr(new WfModule); module->moduleType = WfModuleType::Module; module->name.value = L"RpcMetadata";`
  - Populate `module->declarations` with `WfVariableDeclaration`, `WfFunctionDeclaration`, `WfNamespaceDeclaration`, and `WfClassDeclaration` nodes.
- Rationale:
  - Ensures output is always syntactically valid according to the parser’s AST rules.
  - Makes it feasible to generate correct expressions/statements (especially `if / else if` dispatch, casts, object creation, array construction) without worrying about escaping or indentation.
  - Unit tests already serialize modules using `WfPrint` (`TestRpcCompiler.cpp`), so AST generation naturally matches the test harness.

### C. Create an intermediate “generation model” from `metadataModule`
To avoid scattering AST queries, build an internal, non-AST description first (in C++), then emit AST.

Concrete C++ shapes (sketch):
- `struct RpcParamModel { vl::WString name; vl::WString typeFullName; bool byref; };`
- `struct RpcMethodModel { vl::WString fullName; vl::vint methodId; collections::List<RpcParamModel> params; vl::WString returnTypeFullName; bool returnByref; enum class Kind { Normal, PropertyGetter, PropertySetter } kind; };`
- `struct RpcEventModel { vl::WString fullName; vl::vint eventId; collections::List<RpcParamModel> params; };`
- `struct RpcPropertyModel { vl::WString name; bool byref; vl::WString getterMethodFullName; vl::WString setterMethodFullName; };`
- `struct RpcInterfaceModel { vl::WString fullName; vl::vint typeId; collections::List<RpcPropertyModel> properties; collections::List<RpcMethodModel> methods; collections::List<RpcEventModel> events; };`

Model extraction rules (from the metadata AST):
- Collect RPC interface list in deterministic order using `manager->rpcMetadata->typeFullNames`.
- For each interface declaration (from `manager->rpcMetadata->typeNames[fullName]`), collect:
  - Properties from `WfPropertyDeclaration` (getter/setter method names + property attributes).
  - Methods from `WfFunctionDeclaration`, and classify as getter/setter by matching method names against property getter/setter names (method names alone are not authoritative).
  - Events from `WfEventDeclaration`.
- Byref propagation:
  - For non-property methods: byref is determined from method return / parameter attributes.
  - For property accessors: byref is determined from the property declaration’s attribute and applied to the getter return and setter value parameter.

### D. Generated global ID constants and lookup tables
- Keep the existing ID policy:
  - IDs assigned in order: all types, then all methods, then all events.
  - Global variables:
    - `rpctype_<mangled>`
    - `rpcmethod_<mangled>`
    - `rpcevent_<mangled>`
  - Mangling: replace `::` with `__`, replace `.` with `_`.
- Add the required lookup maps (no speculation):
  - `rpcNameToId : int[string]` mapping the original full names (`::` and `.`) to IDs.
  - `rpcIdToName : string[int]` mapping IDs back to names.
- Deterministic ordering (ID stability contract):
  - Types: follow `WfAnalyzer_ValidateRPC.cpp` ordering (interfaces in declaration order; enums sorted by name; structs in dependency visit order).
  - Methods/events within a type: follow source declaration order from the metadata AST; overloads use the existing full-name disambiguation rule (see `MakeRpcMethodFullName(...)` in `WfAnalyzer_ValidateRPC.cpp`).

### E. Endpoint-side ops implementations (dispatching inbound calls/events)
Generate two Workflow classes (used for both service and client endpoints):
- `class RpcEndpointObjectOps : system::IRpcObjectOps`
- `class RpcEndpointObjectEventOps : system::IRpcObjectEventOps`

Factory functions return instances of those classes:
- `rpc_IRpcObjectOps(lc)` returns `new RpcEndpointObjectOps(lc)`.
- `rpc_IRpcObjectEventOps(lc)` returns `new RpcEndpointObjectEventOps(lc)`.

These classes must implement *all* pure virtual methods (including inherited `IRpcIdSync::SyncIds`) per `Source\Library\WfLibraryRpc.h`:
- `SyncIds(ids)`
- `InvokeMethod(ref, methodId, arguments)`
- `InvokeMethodAsync(ref, methodId, arguments)`
- `ObjectHold(ref, hold)`
- `RequestService(typeId)`
- `InvokeEvent(ref, eventId, arguments)`

Key behaviors:
1) `SyncIds(ids)`:
   - The controller mock calls `objectCallback.SyncIds(ids)` / `eventCallback.SyncIds(ids)` with an empty dictionary (see `Test\Source\RpcLifecycleMock.cpp`).
   - Implement by inserting all `(fullName -> id)` pairs from `rpcNameToId` into `ids`.

2) `RequestService(typeId)`:
   - Convert `typeId` to full name using `rpcIdToName`.
   - Call `lc.RequestService(fullName)` to get the service object.
   - Convert to reference using `lc.PtrToRef(service)`.

3) `InvokeMethod(ref, methodId, arguments)`:
   - Determine which interface/method corresponds to `methodId` using generated `if / else if` chains comparing against `rpcmethod_*` constants.
   - Convert `ref` to an object pointer via `lc.RefToPtr(ref)` and cast to the correct interface type.
   - Deserialize each argument from `arguments[i]` using `system::IRpcLifeCycle.RpcUnboxBy(ref|val)` depending on parameter/property attributes.
   - Invoke the interface method.
   - Serialize the return value using `system::IRpcLifeCycle.RpcBoxBy(ref|val)` depending on return/property attributes.

4) `InvokeMethodAsync(ref, methodId, arguments)`:
   - Must exist for compilation.
   - Current tests do not exercise async RPC; a simple initial behavior is to raise a “Not Supported” error (or provide an immediately-completed `system::Async^` if a standard constructor exists).

5) `ObjectHold(ref, hold)`:
   - Must exist for compilation.
   - Maintain an internal hold table keyed by `RpcObjectReference` to keep a strong reference to `lc.RefToPtr(ref)` while `hold == true`, and release it when `hold == false`.

6) `InvokeEvent(ref, eventId, arguments)`:
   - Determine which interface/event corresponds to `eventId` using generated `if / else if` chains comparing against `rpcevent_*` constants.
   - Deserialize event args using `RpcUnboxByval` (events are always byval).
   - Invoke the event on the local target object returned by `lc.RefToPtr(ref)`.
   - Do not hook events from the actual service object here; lifecycle-managed hookup is deferred to future `IRpcLifeCycle` work.

Notes / constraints:
- Ops should only rely on the `IRpcLifeCycle*` passed to the factory and the IDs; they should not assume they are registered with the same controller instance that belongs to that lifecycle.

### F. Caller-side wrappers for each `@rpc:Interface`
For each interface `Ns1::Ns2::IFoo`, generate:
- Namespace nesting matching the interface.
- A wrapper factory function:
  - `func rpcwrapper_IFoo(lc : system::IRpcLifeCycle*) : (IFoo^)`

Implementation strategy:
- Generate a Workflow class `RpcCaller_IFoo : IFoo` capturing:
  - `lc : system::IRpcLifeCycle*`
  - `controller : system::IRpcController^` (from `lc.Controller`)
  - `ref : system::RpcObjectReference` (obtained by `controller.RequestService(rpctype_...)` or by requesting by name + mapping)
- For each method:
  - Build an `IValueArray` for arguments.
  - Serialize each argument using `RpcBoxBy(ref|val)` based on parameter attributes.
  - Call `controller.InvokeMethod(ref, rpcmethod_..., args)`.
  - Deserialize return value using `RpcUnboxBy(ref|val)` based on return/property attributes.
- For each event:
  - In Workflow, use `attach(...)` explicitly: attach a handler to the proxy’s own event so raising the event locally serializes args using `RpcBoxByval` and forwards to `controller.InvokeEvent(ref, rpcevent_..., boxedArgs)`.
  - For inbound events, rely on `IRpcObjectEventOps` callback (`RpcEndpointObjectEventOps.InvokeEvent`) to locate the local target object via `lc.RefToPtr(ref)` and invoke the event locally.

### G. Make required helpers/factories visible to Workflow
- The wrapper module needs to call:
  - Boxing/unboxing helpers: `RpcBoxByref`, `RpcUnboxByref`, `RpcBoxByval`, `RpcUnboxByval` (implemented in `Source\\Library\\WfLibraryRpc.cpp`).
  - Ops factories: `rpc_IRpcObjectOps`, `rpc_IRpcObjectEventOps`.
- If these are not currently callable from Workflow scripts, expose them via reflection as static members on `system::IRpcLifeCycle` in `Source\\Library\\WfLibraryReflection.cpp` (per requirement).
  - Design choice: add `CLASS_MEMBER_STATIC_METHOD(RpcBoxByref, ...)` etc, and register the factories too, inside `BEGIN_INTERFACE_MEMBER_NOPROXY(vl::rpc_controller::IRpcLifeCycle)`.
  - This keeps the API surface anchored to the lifecycle abstraction and avoids polluting unrelated global namespaces.

## Expected impact
- `Wrapper_RequestService.txt` will change to contain real implementations instead of returning `null`.
- `CompilerTest_LoadAndCompile` already compares generated `Test\\Generated\\RpcMetadata{32,64}\\*.txt` against baselines in `Test\\Resources\\Baseline\\RpcMetadata{32,64}\\*.txt`; update baselines when expected outputs change.
- If reflection registration changes, `CompilerTest_GenerateMetadata` baseline comparisons for `Reflection{32,64}.txt` may also need updates.

# AFFECTED PROJECTS
- Build the solution in folder REPO-ROOT\\Test\\UnitTest (Debug|Win32).
- Build the solution in folder REPO-ROOT\\Test\\UnitTest (Debug|x64).
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

# !!!FINISHED!!!

