# !!!TASK!!!

# PROBLEM DESCRIPTION
Create a `WfRpcMetadata` struct before `WfLexicalScopeManager` to contain:
- `rpcMetadata`: Move from `WfLexicalScopeManager` to here.
- `typeNames`: A `Dictionary<WString, WfDeclaration*>` storing all full name of types in `rpcMetadata`, identifiers joined with "::" just like C++/Workfoow.
- `methodNames`: A `SortedList<WString, WfFunctionDeclaration*>` storing all full name of methods in `rpcMetadata`.
  - It includes all methods in all interfaces (`rpcMetadata` interfaces are all marked with `@rpc:Interface` already)
  - The method name would be `<FULL-TYPE-NAME>.<MethodName>`.
  - **ONLY WHEN** overloading exists, the method name will be appended by `_ArgName1_ArgName2...`. Nothing will be appended if there is no argument.
  - For overloading methods that still result in the same name, `_0`, `_1`... need to be appended, including the first one.
- `eventNames`: Just like method names but it stores `WfEventDeclaration` and there will be no overloading.

Create a very simple test case in a new `Rpc` category, you needs to create a `Rpc` folder as well as `IndexRpc.txt`, with the following content:
- `serviceMain` function accepts a `IRpcLifecycle*` and run.
- `clientMain` function accepts a `IRpcLifecycle*` and register a service.
- A service will be an interface with `@rpc:Ctor`. For the first new test case it will be named after `RequestService`.
- The interface has a `GetText` method without argument, the `serviceMain` will use create an implementation:
  - `GetText` returns a text.
  - Call lifecycle interface's `RegisterService(string, interface^)` to register the service with its full name. This function does not exist, you need to create it, there is an implementation in `LibraryTest` for testing purpose, say `CHECK_FAIL(L"Not Supported!");` there.
  - `clientMain` will all `RequestService(string): interface^` and do a cast (also not exists as above), call `GetText` and return the result as the test case result.

To build this case, you will need to do the similar thing as `TestRuntimeCompile.cpp` in `CompilerTest_LoadAndCompile`, but in a new file called `TestRpcCompiler.cpp`, using a shared manager like other test files:
- Clear and load a case, compile. And you will get the `WfRpcMetadata`, no log file will be created.
- Generate wrapper implementations from `WfRpcMetadata`, write this file to `Test\Generated\RpcMetadata(32|64)\Wrapper_<CASE-NAME>.txt`.
  - It will not be another pass, create `WfAnalyzer_GenerateRpc.cpp`, taking the manager and returning a new `WfModule`.
  - The exported function will be `GenerateModuleRpc` and extern in `WfAnalyzer.h` below ValidateModuleRPC.
- Clear and load the case again as well as the generated wrapper, do a normal compiling like `TestRuntime.cpp` processing `Codegen` but skip the C++ code generation.
  - `TestRuntimeCompile.cpp` seems to not generate `Parsing.Runtime.CASE-NAME.txt`, fix it.
  - `TestRpcCompiler.cpp` will do `Parsing.Rpc.CASE-NAME.txt` too, but the wrapper file does not need to create this file.
- `RuntimeTest` needs to load these cases in a new file `TestRpc.cpp`, doing what `TestRuntime.cpp` do, but only load the assembly, no running is needed. Because the lifecycle interface implementation is not created yet.

In the generated wrapper, you need to do these things:
- Since the original code will compile with the generated wrappers, so it doesn't need to include anything from `rpcMetadata`.
- The generated wrapper will be in a module called `RpcMetadata`, which is the same as the one in `rpcMetadata`.
- Global variables of `int`
  - `rpctype_xxx`, `rpcmethod_xxx` and `rpcevent_xxx`, all comes from `WfRpcMetadata`, `::` and `.` will be replaced by `_`.
  - List type followed by method followed by event, values from 0 ascending even across group.
- Global variables of `int[string]`, maps from these number to their original name (`::` and `.` version).
- Implementations callee side ops interface, global function `rpc_IRpcXXXOps` function returning `IRpcXXXOps^`:
  - `IRpcObjectOps`: Call `Rpc(UnbB)oxBy(ref|val)` according to if `@rpc:Byref` is attached or not (doesn't matter if `@rpc:Byval` is attached or not because it is the default value):
    - For property getter and setter, see the property. You need to read property definition to know which methods are getters and setters, the method names mean nothing.
    - For method return type, see the method.
    - For method argument, see the argument.
  - `IRpcObjectEventOps`: Call `Rpc(Unb|B)oxByval` for argument serialization.
  - These functions are not reflectable yet, you need to register them as `IRpcLifecycle`'s static function.
  - Callee side ops interfaces take an input of `IRpcLifecycle*`.
  - Those global variables help to know which ID is which.
- Implementations caller side `@rpc:Interface` interface implementation:
  - If the function name is `my::favorite::namespace_name::interface_name`, the function will be `my::favorite::namespace_name::rpcwrapper_interface_name`.
  - The function returns `interface_name^`.
  - The function takes a `IRpcLifecycle*` as a parameter and implement redirect all methods to those ops interfaces, following the same rule above about byval or byref.
- Caller side ops:
  - `IRpcObjectOps` will take method arguments from the wrapper and send them to the remote.
  - `IRpcObjectEventOps` will take event arguments from wrapper, these events will be triggered in the caller side directly, so in each rpc interface wrapper all events will be listened to call `IRpcObjectEventOps`.
- Callee side ops:
  - `IRpcObjectOps` do the reverse, read arguments and call methods.
  - `IRpcObjectEventOps` do the reverse, read arguments and call events.
  - You will notice that, the implementation is symmentric but why callee side doesn't hook events from the actual interface (unlike the caller side hooking wrappers), because it will be part of the future implementation of `IRpcLifecycle`.
- No more global variable to create.
- Ops interface implementations are for both service and client.
- You cannot assume the ops implementation will be registered to the same `IRpcLifecycle` instance with the one passed to the function to create it.

Since the test case can't be actually launched yet, just compile in `CompileTest_LoadAndCompile` and load assembly in `RuntimeTest` is enough.
Actually execution will be in the future when `IRpcLifecycle` is implemented.

# UPDATES
- (none)

# INSIGHTS AND REASONING

## Current codebase observations (evidence)

- RPC metadata already exists as a generated `vl::workflow::WfModule` named `RpcMetadata`.
  - `Source\Analyzer\WfAnalyzer_ValidateRPC.cpp` builds the metadata module inside `ValidateModuleRPC_GenerateMetadata(...)` and currently stores it on `WfLexicalScopeManager` (via `manager->rpcMetadata = metadataModule;`).
- RPC validation is part of the normal analyzer rebuild pipeline.
  - `Source\Analyzer\WfAnalyzer.cpp` calls `ValidateModuleRPC(this, module);` from `WfLexicalScopeManager::Rebuild(...)`.
- The runtime reflection surface for RPC lifecycle exists but is currently empty.
  - `Source\Library\WfLibraryReflection.cpp` has `BEGIN_INTERFACE_MEMBER_NOPROXY(vl::rpc_controller::IRpcLifeCycle)` with no members, while other RPC ops (`IRpcObjectOps`, etc.) are reflected.
  - Type rename shows Workflow-side name is `system::rpc_controller::LifeCycle` (`IMPL_TYPE_INFO_RENAME(vl::rpc_controller::IRpcLifeCycle, system::rpc_controller::LifeCycle)`). This is a naming mismatch with the problem statement’s `IRpcLifecycle*`.
- Compiler tests already baseline-compare RPC metadata for `Runtime` cases.
  - `Test\UnitTest\CompilerTest_LoadAndCompile\TestRuntimeCompile.cpp` writes `Test\Generated\RpcMetadata{32|64}\<case>.txt` and compares against `Test\Resources\Baseline\RpcMetadata{32|64}`.
- Parse logs are produced via a shared helper.
  - `Test\Source\Helper.cpp` provides `LogSampleParseResult(category, itemName, module);` which writes `Test\Generated\Workflow{32|64}\Parsing.<Category>.<Case>.txt`.

These existing structures strongly suggest we should:
1) keep `ValidateModuleRPC` as the source of truth for “what is RPC and what needs wrappers”,
2) move “where metadata is stored” and “precomputed name lookup tables” into a new struct, and
3) add a dedicated wrapper-generation step that consumes this struct.

## Proposed architecture

### 1) Introduce `WfRpcMetadata` (data ownership and lookup tables)

Add a new `struct WfRpcMetadata` in `Source\Analyzer\WfAnalyzer.h` before `WfLexicalScopeManager`.

Fields (concrete types, no placeholders):
- `Ptr<vl::workflow::WfModule> rpcMetadata;`
  - Moved from `WfLexicalScopeManager`.
- Final unique-name maps (lookup by full name):
  - `vl::collections::Dictionary<vl::WString, vl::workflow::WfDeclaration*> typeNames;`
  - `vl::collections::Dictionary<vl::WString, vl::workflow::WfFunctionDeclaration*> methodNames;`
  - `vl::collections::Dictionary<vl::WString, vl::workflow::WfEventDeclaration*> eventNames;`
- Deterministic ID ordering (do not rely on `Dictionary` key order):
  - `vl::collections::List<vl::WString> typeFullNames;`
  - `vl::collections::List<vl::WString> methodFullNames;`
  - `vl::collections::List<vl::WString> eventFullNames;`

Construction algorithm (single-pass, no redundant traversal):
- `ValidateModuleRPC_GenerateMetadata(...)` remains responsible for producing the `RpcMetadata` module.
- While constructing metadata declarations, also compute and record:
  - the final full name for each type/method/event (post-disambiguation),
  - populate the corresponding `*Names` dictionary,
  - append the full name into the corresponding `*FullNames` list in the exact order that will define IDs.
- If temporary grouping is needed during name construction (e.g. before disambiguation), use a temporary `vl::collections::Group<vl::WString, vl::workflow::WfFunctionDeclaration*>` (or equivalent) and only store final unique names in the `Dictionary` fields.

Naming rules (core requirement, now fully specified):
- Type full names: join nested namespaces/types with `::`.
- Method full names: `<FULL-TYPE-NAME>.<MethodName>`.
  - Overload disambiguation is applied only when multiple methods share the same base `<FULL-TYPE-NAME>.<MethodName>`.
  - Step 1: append `_ArgName1_ArgName2...` using parameter *names* in declaration order. If there is no argument, append nothing.
  - Step 2: if there are still collisions (same final name after Step 1), append numeric suffixes `_0`, `_1`, ... **including the first one**.
    - Tie-breaker for `_0/_1/...`: use AST declaration order within the interface (stable across runs) so `Wrapper_<CASE>.txt` is deterministic.
- Event full names: `<FULL-TYPE-NAME>.<EventName>` (no overloading).

Reset semantics (required by `collections::Dictionary` behavior):
- `collections::Dictionary` copy assignment is deleted. Any “reset” must call `.Clear()` on each `Dictionary`/`List` field (or reconstruct the whole `WfRpcMetadata` by move/swap).


### 2) Store RPC metadata on the manager as a single concept

Modify `WfLexicalScopeManager` (in `Source\Analyzer\WfAnalyzer.h`) to replace the existing `rpcMetadata` field with a single `WfRpcMetadata rpc;` (or similarly named field).

Rationale:
- Keeps RPC-related information cohesive.
- Avoids additional recomputation across phases (validation, wrapper generation, tests).

### 3) Add wrapper-generation entry point: `GenerateModuleRpc`

Create a new analyzer source file `Source\Analyzer\WfAnalyzer_GenerateRpc.cpp` implementing:
- `Ptr<vl::workflow::WfModule> GenerateModuleRpc(WfLexicalScopeManager* manager);`

Export/extern:
- Add an `extern` declaration in `Source\Analyzer\WfAnalyzer.h` below `ValidateModuleRPC` declarations, per requirement.

Input/Output contract:
- Input: the manager after `ValidateModuleRPC` has run (so `manager->rpc` is populated).
- Output: a new `WfModule` representing the generated wrapper module.
  - Module name must be `RpcMetadata` (exactly matching the metadata module name).

Wrapper module contents (high-level):

1. Global ID constants:
- Generate global `int` variables:
  - `rpctype_xxx`, `rpcmethod_xxx`, `rpcevent_xxx`.
  - Name mangling: replace `::` and `.` with `_`.
- ID assignment must be deterministic:
  - Types first, then methods, then events.
  - Values are assigned from 0 ascending across all groups (i.e., method IDs continue after the last type ID, event IDs continue after the last method ID).

2. Global name mapping dictionaries:
- Workflow generic direction is treated as: `T[K]` means **value type `T`**, **key type `K`**.
- To avoid any ambiguity and make wrappers self-sufficient, generate **both directions** with explicit names:
  - `rpcNameToId : int[string]` (key: full name, value: ID)
  - `rpcIdToName : string[int]` (key: ID, value: full name)
- These maps cover type/method/event IDs in the unified 0..N-1 range (types first, then methods, then events).


3. Callee-side ops implementations:
- Generate concrete implementations for ops interfaces (e.g. `system::rpc_controller::ObjectOps^`, `ObjectEventOps^`, etc.; exact reflected names depend on `WfLibraryReflection.cpp` renames).
- Provide global factory functions `rpc_IRpcXXXOps(lifecycle: LifeCycle*): IRpcXXXOps^`.
  - These functions must exist for both service and client usage.
  - They must not assume the created ops object is registered to the same lifecycle instance that is passed in.
- Boxing/unboxing and byref/byval:
  - Wrapper-generated code must call `Rpc(Unb|B)oxBy(ref|val)` based on `@rpc:Byref` presence.
  - For property accessors, “getter vs setter” must be determined from the property declaration, not method naming.

4. Caller-side wrappers for `@rpc:Interface` interfaces:
- For each `@rpc:Interface` interface type `my::ns::IService`, generate a factory function:
  - `my::ns::rpcwrapper_IService(lifecycle: LifeCycle*): IService^`.
- The returned object implements `IService` and redirects:
  - Methods to `IRpcObjectOps`.
  - Events are hooked on the caller side and dispatched through `IRpcObjectEventOps`.

### 4) Required library API surface for compilation

The new Workflow test case requires two lifecycle methods that do not currently exist:
- `RegisterService(string, interface^)`.
- `RequestService(string): interface^`.

Design proposal:
- Add these as members on `vl::rpc_controller::IRpcLifeCycle` in `Source\Library\WfLibraryRpc.h`.
- Reflect them in `Source\Library\WfLibraryReflection.cpp` under `BEGIN_INTERFACE_MEMBER_NOPROXY(vl::rpc_controller::IRpcLifeCycle)`.
  - This is also where we will add the “static functions for boxing/unboxing” required by the wrapper generator.

Rationale:
- The immediate goal is compile-time availability to allow wrapper code and test cases to type-check.
- Runtime behavior is not required yet (tests do not execute RPC), so test-only implementations can fail fast.

Test-only implementation location:
- Implement stub behavior in existing test lifecycle mocks (e.g. `Test\Source\RpcLifecycleMock.*`) or in the `LibraryTest` side per requirement.
  - The stub can use `CHECK_FAIL(L"Not Supported!");` from Vlpp (see KB guidance in `.github\KnowledgeBase\Index.md`, exception-handling section).

Lifecycle naming contract (no alias / no rename):
- Treat `IRpcLifecycle` as a spelling mistake in the problem statement.
- Keep the existing C++ interface `vl::rpc_controller::IRpcLifeCycle` and the existing Workflow name `system::rpc_controller::LifeCycle` as the contract.
- All new Workflow test cases in this task will use `LifeCycle*` (not `IRpcLifecycle*`).


## Test strategy and artifacts

### 1) New `Rpc` resource category

Add a new resource category:
- `Test\Resources\Rpc\` folder.
- `Test\Resources\IndexRpc.txt` listing cases.

Add the first case (explicit Workflow sketch):
- `serviceMain(lifecycle: LifeCycle*) : void`
  - Create an implementation object of `RequestService`.
  - Call `lifecycle.RegisterService("<FULL-INTERFACE-NAME>", service)`.
- `clientMain(lifecycle: LifeCycle*) : string`
  - Call `lifecycle.RequestService("<FULL-INTERFACE-NAME>")`.
  - Cast to `RequestService^`, call `GetText()`, return the string.
- Service interface:
  - Must satisfy existing analyzer rule: `@rpc:Ctor` can only apply to an interface that also has `@rpc:Interface`.
  - Therefore the case uses:
    - `@rpc:Interface`
    - `@rpc:Ctor`
    - `interface RequestService { func GetText() : string; }`


### 2) Compiler test: `TestRpcCompiler.cpp`

In `Test\UnitTest\CompilerTest_LoadAndCompile`, add `TestRpcCompiler.cpp` modeled after `TestRuntimeCompile.cpp`, but with a 2-pass flow:

Pass 1 (metadata only):
- Clear/load one `Rpc` case and compile far enough to produce `WfRpcMetadata` (i.e., to run `ValidateModuleRPC`).
- Do not generate parsing logs for this pass (per requirement).

Wrapper generation:
- Call `GenerateModuleRpc(manager)` to produce the wrapper `WfModule`.
- Print it to `Test\Generated\RpcMetadata{32|64}\Wrapper_<CASE-NAME>.txt`.
  - This is a generated artifact, not a baseline input.

Pass 2 (compile case + wrapper, codegen to assembly, no C++ generation):
- Clear/load the same case again.
- Load the generated wrapper module as an additional module.
- **Prevent module name collision**: since both the RPC metadata module and the wrapper module are named `RpcMetadata`, pass 2 must **skip RPC validation/metadata generation**.
  - Design commitment: introduce a test-only compile option/flag that disables `ValidateModuleRPC` for this pass, because the wrapper module is already provided and we only need type-checking + codegen.
- Perform the normal compilation pipeline similar to what `TestRuntime.cpp` does for executable cases, but stop after VM assembly generation (skip C++ code generation).
- Generate parse log:
  - `Test\Generated\Workflow{32|64}\Parsing.Rpc.<CASE-NAME>.txt`.


Additionally:
- Re-check `TestRuntimeCompile.cpp` behavior regarding `Parsing.Runtime.<CASE-NAME>.txt`.
  - The shared helper supports this; if missing, the fix is to ensure the runtime compile test calls `LogSampleParseResult(L"Runtime", ...)` consistently (as other categories do).

### 3) Runtime test: `RuntimeTest\TestRpc.cpp`

In `Test\UnitTest\RuntimeTest`, add `TestRpc.cpp` that:
- Loads `Rpc` cases similarly to `TestRuntime.cpp`.
- Only loads/deserializes the generated assembly; does not execute `serviceMain/clientMain`.

Rationale:
- Lifecycle is not implemented yet, but assembly load ensures:
  - the wrapper module type-checks with the case,
  - the codegen pipeline is stable,
  - serialization/deserialization of assemblies is correct.

## Determinism and baseline impact

- Determinism is required for stable `Wrapper_<CASE>.txt` generation and stable ID assignment.
  - Determinism is enforced by `WfRpcMetadata::{typeFullNames, methodFullNames, eventFullNames}` (explicit ordered lists) plus the overload tie-breaker rule (AST declaration order for `_0/_1/...`).
- Baseline comparisons that may change:
  - Existing `RpcMetadata{32|64}\<case>.txt` may change if the metadata module content changes.
- Wrapper baseline policy (explicit):
  - `Wrapper_<CASE-NAME>.txt` is a generated artifact written under `Test\Generated\RpcMetadata{32|64}`.
  - In this task it is **not** baseline-compared; acceptance is: file exists, non-empty, and is deterministic across repeated runs on the same architecture.


## Resolved design decisions (review-driven)

To make implementation/test behavior deterministic and reviewable, the following are committed (not open questions):

1) **`WfRpcMetadata` collections are concrete `vl::collections` types**
- Final unique-name maps are `Dictionary<WString, ...>` (`typeNames`, `methodNames`, `eventNames`).
- Deterministic ID order is defined by explicit `List<WString>` sequences (`typeFullNames`, `methodFullNames`, `eventFullNames`).

2) **`int[string]` direction is fixed and both maps are generated**
- Treat `T[K]` as value `T`, key `K`.
- Generate `rpcNameToId : int[string]` and `rpcIdToName : string[int]`.

3) **Lifecycle naming is locked (no alias)**
- C++: `vl::rpc_controller::IRpcLifeCycle`.
- Workflow: `system::rpc_controller::LifeCycle`.

4) **Pass-2 module collision is handled explicitly**
- Wrapper module name remains `RpcMetadata` (per requirement).
- Pass 2 disables `ValidateModuleRPC` so the metadata module is not regenerated and cannot collide with the wrapper module.

5) **`@rpc:Ctor` semantics are explicit**
- `@rpc:Ctor` is only meaningful on an `@rpc:Interface` interface (existing analyzer rule).
- For metadata/wrapper generation it behaves like a normal RPC interface for method/event enumeration; additionally it marks the interface as eligible for lifecycle-level `RegisterService(name, interface^)` / `RequestService(name): interface^` usage.

6) **Single-pass metadata population**
- `ValidateModuleRPC_GenerateMetadata` populates name maps and ordered lists while constructing the metadata module (no second traversal).

7) **Reset semantics are explicit**
- No copy assignment is used for `Dictionary` fields; reset uses `.Clear()` on all `Dictionary`/`List` fields (or reconstruct by move/swap).

8) **Runtime parse log scope is minimal**
- Do not change `TestRuntimeCompile.cpp` unless `Parsing.Runtime.<CASE>.txt` is actually missing; if missing, add the missing `LogSampleParseResult(L"Runtime", ...)` call.

9) **Overload disambiguation is deterministic**
- Disambiguation uses arg-name suffixing first, then `_0/_1/...` with AST declaration order as the tie-breaker.

10) **`RequestService` name overlap is acknowledged**
- `IRpcObjectOps::RequestService(vint typeId)` already exists.
- New lifecycle methods are separate and name-based; document this distinction in code/comments and wrapper generation naming to avoid confusion.

11) **Ops factories and boxing helpers are reflected in a stable way**
- Reflection commits to exposing required ops factory functions and boxing/unboxing helpers as `LifeCycle` static functions in `WfLibraryReflection.cpp` (validate macro compatibility with `BEGIN_INTERFACE_MEMBER_NOPROXY(vl::rpc_controller::IRpcLifeCycle)`).

12) **Reflection scope is complete for wrapper compilation**
- `LifeCycle` reflection will include existing members (`GetController`, `RefToPtr`, `PtrToRef`) plus any wrapper-required helpers (`RpcBoxByref`, `RpcUnboxByref`, `RpcBoxByval`, `RpcUnboxByval`) and the new service registry APIs.

13) **Concrete Workflow test sketch is part of the design**
- The `Rpc` case uses `LifeCycle*`, `@rpc:Interface`, and `@rpc:Ctor` explicitly and calls `RegisterService/RequestService` by full interface name.

14) **Wrapper artifact acceptance criteria are explicit**
- Wrapper files are not baseline-compared in this task; determinism and existence are the acceptance criteria.

15) **Expected artifacts per phase are explicit**
- Pass 1: metadata produced, no parsing log; wrapper file written.
- Pass 2: assembly codegen succeeds; `Parsing.Rpc.<CASE>.txt` generated.

# AFFECTED PROJECTS

- Build the solution in folder `REPO-ROOT\Test\UnitTest` (Debug|x64).
- Build the solution in folder `REPO-ROOT\Test\UnitTest` (Debug|Win32).
- Always Run UnitTest project `LibraryTest` (Debug|x64, Debug|Win32).
- Run UnitTest project `CompilerTest_GenerateMetadata` (Debug|x64, Debug|Win32) only when files in `REPO-ROOT\Source\Library`, `REPO-ROOT\Source\Parser`, or `REPO-ROOT\Test\Source` are changed (reflection/lifecycle API changes will trigger this).
- Always Run UnitTest project `CompilerTest_LoadAndCompile` (Debug|x64).
- Always Run UnitTest project `RuntimeTest` (Debug|x64, Debug|Win32).
- Always Run UnitTest project `CppTest` (Debug|x64, Debug|Win32).
- Always Run UnitTest project `CppTest_Metaonly` (Debug|x64, Debug|Win32).
- Always Run UnitTest project `CppTest_Reflection` (Debug|x64, Debug|Win32).

# !!!FINISHED!!!
