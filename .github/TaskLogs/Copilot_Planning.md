# !!!PLANNING!!!

# UPDATES

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

# EXECUTION PLAN

## STEP 1: Introduce `vl::workflow::analyzer::WfRpcMetadata` and move storage

### What to change

Update `REPO-ROOT\Source\Analyzer\WfAnalyzer.h`:

1) Add a new struct **before** `class WfLexicalScopeManager`:

    namespace vl::workflow::analyzer
    {
    	struct WfRpcMetadata
    	{
    		vl::Ptr<vl::workflow::WfModule>                                                  rpcMetadata;
    		vl::collections::Dictionary<vl::WString, vl::workflow::WfDeclaration*>           typeNames;
    		vl::collections::Dictionary<vl::WString, vl::workflow::WfFunctionDeclaration*>   methodNames;
    		vl::collections::Dictionary<vl::WString, vl::workflow::WfEventDeclaration*>      eventNames;
    
    		vl::collections::List<vl::WString>                                              typeFullNames;
    		vl::collections::List<vl::WString>                                              methodFullNames;
    		vl::collections::List<vl::WString>                                              eventFullNames;
    
    		void Clear()
    		{
    			rpcMetadata = nullptr;
    			typeNames.Clear();
    			methodNames.Clear();
    			eventNames.Clear();
    			typeFullNames.Clear();
    			methodFullNames.Clear();
    			eventFullNames.Clear();
    		}
    	};
    }

2) Replace `Ptr<WfModule> rpcMetadata;` on `WfLexicalScopeManager` with:

    WfRpcMetadata                            rpc;

3) Update `WfLexicalScopeManager::Clear(...)` to clear RPC state:

    void WfLexicalScopeManager::Clear(bool keepTypeDescriptorNames, bool deleteModules)
    {
    	...
    	rpc.Clear();
    	...
    }

### Why

- The task requires a single cohesive RPC metadata concept that holds (1) the metadata module and (2) precomputed lookup tables for type/method/event full names.
- The lookup tables will be consumed by wrapper generation; keeping them together avoids duplicated traversal and keeps determinism rules in one place.


## STEP 2: Populate `WfRpcMetadata` during RPC validation (name + overload rules)

### What to change

Update `REPO-ROOT\Source\Analyzer\WfAnalyzer_ValidateRPC.cpp`:

1) Change the assignment:

    // old
    manager->rpcMetadata = metadataModule;

    // new
    manager->rpc.rpcMetadata = metadataModule;

2) While generating the metadata module, also fill RPC lookup tables and deterministic ordered-name lists:

- `manager->rpc.typeNames` + `manager->rpc.typeFullNames`
- `manager->rpc.methodNames` + `manager->rpc.methodFullNames`
- `manager->rpc.eventNames` + `manager->rpc.eventFullNames`

Concrete rules to implement:

- Type full name key format: identifiers joined with `"::"` (same as C++), derived from the reflected type name for the interface in `rpcMetadata`.
  - Implementation note: current code already uses `ITypeDescriptor::GetTypeName()` (which is `"a::b::C"`). Use that as the base for interface full names.

- Method full name base: `<FULL-TYPE-NAME>.<MethodName>`.

- Overload disambiguation:
  - Only apply disambiguation when more than one method shares the same base name.
  - First try: append `_ArgName1_ArgName2...` using argument *names* in declaration order. If there is no argument, append nothing.
  - If still colliding: append `_0`, `_1`, ... **including the first one**.
    - Tie-breaker: use a deterministic order (the interface member declaration order from the AST is acceptable; do not rely on hash/dictionary order).

Suggested helper (local to `WfAnalyzer_ValidateRPC.cpp`):

    static vl::WString MakeRpcMethodFullName(
    	const vl::WString& typeFullName,
    	vl::workflow::WfFunctionDeclaration* methodDecl,
    	bool appendArgNames)
    {
    	auto name = typeFullName + L"." + methodDecl->name.value;
    	if (appendArgNames)
    	{
    		for (auto arg : methodDecl->arguments)
    		{
    			name += L"_" + arg->name.value;
    		}
    	}
    	return name;
    }

Population algorithm sketch:

- For each generated RPC interface declaration (`WfClassDeclaration*`) in the metadata module (in module declaration order):
  - Insert `typeNames.Add(typeFullName, interfaceDecl)` (store the metadata-declaration node).
  - Append `typeFullNames.Add(typeFullName)`.
  - Collect member declarations in AST order:
    - Methods: `WfFunctionDeclaration*` (including expanded declarations if needed).
    - Events: `WfEventDeclaration*`.
  - For methods:
    - Group by base name `<type>.<name>`.
    - If group size == 1: final name == base.
    - Else (only within this base-name group):
      1) compute candidate with arg-name suffix and check uniqueness inside the group.
      2) if duplicates remain, assign numeric suffixes `_0/_1/...` by stable AST iteration order (do not rely on any dictionary iteration order).
    - Insert into `manager->rpc.methodNames.Add(finalName, methodDecl)`.
    - Append `manager->rpc.methodFullNames.Add(finalName)`.
  - For events:
    - Final name `<type>.<EventName>`.
    - Insert into `manager->rpc.eventNames.Add(finalName, eventDecl)`.
    - Append `manager->rpc.eventFullNames.Add(finalName)`.

### Why

- Wrapper generation needs a stable mapping from names to declarations.
- Overload naming rules are part of the public contract for generated wrappers (`Wrapper_<CASE>.txt`). Implementing them during validation ensures all downstream passes use the same canonical names.


## STEP 3: Add `GenerateModuleRpc` to generate wrapper modules from `WfRpcMetadata`

### What to change

1) Create a new file: `REPO-ROOT\Source\Analyzer\WfAnalyzer_GenerateRpc.cpp`.

2) Add a declaration to `REPO-ROOT\Source\Analyzer\WfAnalyzer.h` **below** `ValidateModuleRPC`:

    extern vl::Ptr<vl::workflow::WfModule> GenerateModuleRpc(WfLexicalScopeManager* manager);

3) Implement `GenerateModuleRpc(manager)` to return a new `WfModule`:

- `moduleType = WfModuleType::Module`.
- `name = "RpcMetadata"`.

Wrapper module content requirements (first implementation can be minimal but must compile with the Rpc test case):

A) Global `int` IDs (deterministic)

- Variables:
  - `rpctype_xxx`, `rpcmethod_xxx`, `rpcevent_xxx`
- `xxx` is a **collision-safe mangled** full name:
  - Replace `::` with `__`.
  - Replace `.` with `_`.
  - Add collision detection (if two distinct names mangle to the same `xxx`, fail metadata validation early).
- ID values start from 0 and increase across all groups:
  - types first, then methods, then events.

Deterministic ordering proposal:
- Do **not** reconstruct order by sorting/iterating map keys during wrapper generation.
- Types: iterate `manager->rpc.typeFullNames` (populated in STEP 2) sequentially, then assign IDs.
- Methods: iterate `manager->rpc.methodFullNames` sequentially, then assign IDs.
- Events: iterate `manager->rpc.eventFullNames` sequentially, then assign IDs.

B) Global name maps

Generate both directions (self-contained wrappers):

- `rpcNameToId : int[string]` (key: full name, value: id)
- `rpcIdToName : string[int]` (key: id, value: full name)

C) Ops factories + interface wrapper factories

Generate the required functions with these signatures:

- Callee-side ops factory functions:

    func rpc_IRpcObjectOps(lc: system::rpc_controller::LifeCycle*) : system::rpc_controller::ObjectOps^;
    func rpc_IRpcObjectEventOps(lc: system::rpc_controller::LifeCycle*) : system::rpc_controller::ObjectEventOps^;

  (Plus any other `IRpcXXXOps` interfaces needed by current metadata; start with object/event ops to support the first test case.)

- Caller-side wrapper factory for each `@rpc:Interface` interface type:

    func <ns>::rpcwrapper_<InterfaceName>(lc: system::rpc_controller::LifeCycle*) : <ns>::<InterfaceName>^;

For the first test case, it is sufficient that:
- The wrapper object implements the interface.
- **Compile-only milestone**: wrapper method bodies may be stubs (e.g., `return "";` for `string`, `return null;` for reference types) as long as pass-2 compilation and assembly loading succeed.
  - (Optional later milestone) Implement real delegation to `IRpcObjectOps.InvokeMethod(...)` using the computed `rpcmethod_xxx` ID.
- Byref/byval selection (when delegation is implemented):
  - Specify how `@rpc:Byref` is detected: read from AST attribute lists on the metadata declarations / arguments (preferred for determinism) or via descriptors populated by `PopulateAttributesOnTypeDescriptors(...)` (must be stated explicitly).
  - If `@rpc:Byref` is present on the method / property / argument, use `RpcBoxByref` / `RpcUnboxByref`.
  - Otherwise use `RpcBoxByval` / `RpcUnboxByval`.
- Ensure wrapper dependencies are present on the reflection surface / metadata module as needed, otherwise pass-2 compile will fail. At minimum, verify/reflect:
  - Boxing helpers: `RpcBoxByref`, `RpcUnboxByref`, `RpcBoxByval`, `RpcUnboxByval`.
  - Ops factory entry points used by generated code: `rpc_IRpcObjectOps`, `rpc_IRpcObjectEventOps`.

### Why

- The task requires turning analyzer-produced RPC metadata into a wrapper module that can be compiled together with Workflow scripts.
- Splitting wrapper generation into a dedicated entry point keeps `ValidateModuleRPC` focused on validation and metadata-only production.


## STEP 4: Add name-based service APIs to `IRpcLifeCycle` and reflect them

### What to change

1) Update `REPO-ROOT\Source\Library\WfLibraryRpc.h` (`vl::rpc_controller::IRpcLifeCycle`):

    class IRpcLifeCycle
    	: public virtual vl::reflection::IDescriptable
    	, public vl::reflection::Description<IRpcLifeCycle>
    {
    	public:
    		virtual vl::Ptr<IRpcController>                    GetController()const = 0;
    		virtual vl::Ptr<vl::reflection::IDescriptable>     RefToPtr(RpcObjectReference ref) = 0;
    		virtual RpcObjectReference                         PtrToRef(vl::Ptr<vl::reflection::IDescriptable> obj) = 0;
    
    		virtual void                                       RegisterService(const vl::WString& fullName, vl::Ptr<vl::reflection::IDescriptable> service) = 0;
    		virtual vl::Ptr<vl::reflection::IDescriptable>     RequestService(const vl::WString& fullName) = 0;
    	};

2) Update `REPO-ROOT\Source\Library\WfLibraryReflection.cpp`:

Inside:

    BEGIN_INTERFACE_MEMBER_NOPROXY(vl::rpc_controller::IRpcLifeCycle)

Add members:

    CLASS_MEMBER_METHOD(GetController, NO_PARAMETER)
    CLASS_MEMBER_METHOD(RefToPtr, { L"ref" })
    CLASS_MEMBER_METHOD(PtrToRef, { L"obj" })
    CLASS_MEMBER_METHOD(RegisterService, { L"fullName" _ L"service" })
    CLASS_MEMBER_METHOD(RequestService, { L"fullName" })

3) Update **all** `vl::rpc_controller::IRpcLifeCycle` implementations to satisfy new pure virtuals (fail-fast stub is acceptable where the feature isn’t used yet).

At minimum, update these test mocks (and also search for any other concrete subclasses/mocks in the repo):

- `REPO-ROOT\Test\Source\RpcLifecycleMock.h/.cpp`
- `REPO-ROOT\Test\Source\RpcByvalLifecycleMock.h/.cpp`

Example stub:

    void RpcByvalLifecycleMock::RegisterService(const WString& fullName, Ptr<IDescriptable> service)
    {
    	(void)fullName;
    	(void)service;
    	CHECK_FAIL(L"Not Supported!");
    }

    Ptr<IDescriptable> RpcByvalLifecycleMock::RequestService(const WString& fullName)
    {
    	(void)fullName;
    	CHECK_FAIL(L"Not Supported!");
    	return nullptr;
    }

### Why

- The new `Rpc` Workflow test case must type-check calls to `LifeCycle.RegisterService(name, interface^)` and `LifeCycle.RequestService(name)`.
- Reflection must expose these methods; otherwise compiling Workflow scripts will fail before wrapper generation is exercised.


## STEP 5: Add the new `Rpc` resource category and the first simple case

### What to change

1) Create folder: `REPO-ROOT\Test\Resources\Rpc\`.
2) Create index file: `REPO-ROOT\Test\Resources\IndexRpc.txt`.
3) Add first case file: `REPO-ROOT\Test\Resources\Rpc\RequestService.txt` (case name used in IndexRpc.txt).

Workflow script content (minimal, compile-only):

    module Rpc;

    using system::*;
    using system::rpc_controller::*;

    @rpc:Interface
    @rpc:Ctor
    interface RequestService
    {
    	func GetText() : string;
    }

    func serviceMain(lc: LifeCycle*) : void
    {
    	var service = new interface RequestService
    	{
    		func GetText() : string
    		{
    			return "Hello";
    		}
    	};
    	// full name must match the interface full name rule
    	lc.RegisterService("Rpc::RequestService", service);
    }

    func clientMain(lc: LifeCycle*) : string
    {
    	var obj = lc.RequestService("Rpc::RequestService");
    	var service = cast RequestService^ obj;
    	return service.GetText();
    }

### Why

- This is the smallest end-to-end input that forces:
  - RPC metadata discovery (`@rpc:Interface`),
  - wrapper generation for one interface method,
  - compilation of a script against the generated wrapper APIs.


## STEP 6: Add `TestRpcCompiler.cpp` to `CompilerTest_LoadAndCompile` (2-pass compilation)

### What to change

Add `REPO-ROOT\Test\UnitTest\CompilerTest_LoadAndCompile\TestRpcCompiler.cpp` modeled after `TestRuntimeCompile.cpp` but split into two passes per case:

Pass 1 (metadata + wrapper generation only):
- Load one `Rpc` case.
- `manager.Rebuild(true, true)` to run validation, including `ValidateModuleRPC`.
- Expect `manager.rpc.rpcMetadata` non-null.
- Call `GenerateModuleRpc(&manager)`.
- Print wrapper module with `WfPrint(...)` and save to:

  `Test\Generated\RpcMetadata(32|64)\Wrapper_<CASE>.txt`

Pass 2 (compile script + wrapper, but skip RPC validation/metadata):
- Clear and load the case again.
- Add the wrapper module (parsed back to AST, or keep the `Ptr<WfModule>` in memory).
- Introduce a per-invocation compile option for pass 2 to skip `ValidateModuleRPC` (and therefore skip RPC metadata regeneration) to avoid the module-name collision (`RpcMetadata` vs `RpcMetadata`).
  - Avoid test-only mutable state on production classes.
  - Concrete proposal: add an optional parameter to `WfLexicalScopeManager::Rebuild(...)` (defaulting to current behavior), e.g.:

        void Rebuild(bool keepTypeDescriptorNames, bool validateRpc = true);

    and in `WfLexicalScopeManager::Rebuild(...)`:

        PopulateAttributesOnTypeDescriptors(this);
        if (validateRpc)
        {
        	for (auto module : modules)
        	{
        		ValidateModuleRPC(this, module);
        	}
        }

  - In `TestRpcCompiler.cpp`:
    - Pass 1: `manager.Rebuild(true, true);`
    - Pass 2: `manager.Rebuild(true, false);`

- Generate parsing log for pass 2:

  `Test\Generated\Workflow(32|64)\Parsing.Rpc.<CASE>.txt`

### Why

- Pass 1 establishes the wrapper artifact deterministically and avoids generating parsing logs (per requirement).
- Pass 2 verifies that the wrapper is sufficient for normal compilation/codegen.


## STEP 7: Add `RuntimeTest\TestRpc.cpp` to load assemblies for Rpc cases

### What to change

In `REPO-ROOT\Test\UnitTest\RuntimeTest`, add `TestRpc.cpp` following the pattern of the existing runtime tests:

- Load `Rpc` cases from `IndexRpc.txt`.
- Compile/generate assemblies (or load binaries if the test infrastructure already does so).
- Only validate that assemblies load/deserialize successfully.
- Do **not** execute `serviceMain` / `clientMain` (lifecycle not implemented yet).

### Why

- Ensures the generated wrapper + script combination is not only compilable, but also produces loadable VM assemblies.


## STEP 8: Verification checkpoint for `Parsing.Runtime.<CASE>.txt` generation

### What to do

- Treat this as a verification checkpoint, not a planned implementation step.
- Run the existing runtime compile tests and confirm that `Test\Generated\Workflow(32|64)\Parsing.Runtime.<CASE>.txt` is produced.
  - Current code already calls:

        LogSampleParseResult(L"Runtime", itemName, sample, module, &manager);

- Only if the artifact is missing for some Runtime path:
  - Identify the missing call site and add the corresponding `LogSampleParseResult(L"Runtime", ...)`.

### Pass criteria

- For at least one representative Runtime case, `Parsing.Runtime.<CASE>.txt` exists after the test run.

### Why

- Makes the requirement concrete and prevents contradictory “seems missing” implementation work when logging already exists.


# TEST PLAN

## Unit tests / verification sequence

Follow `REPO-ROOT\Project.md` verification requirements:

1) Build `REPO-ROOT\Test\UnitTest\UnitTest.sln` in `Debug|x64`.
2) Build `REPO-ROOT\Test\UnitTest\UnitTest.sln` in `Debug|Win32`.
3) Run unit test projects in the required order:
   - `LibraryTest` (Debug|x64, Debug|Win32)
   - `CompilerTest_GenerateMetadata` (Debug|x64, Debug|Win32) when lifecycle/reflection changes affect baselines
   - `CompilerTest_LoadAndCompile` (Debug|x64)
   - `CppTest`, `CppTest_Metaonly`, `CppTest_Reflection` (Debug|x64, Debug|Win32)
   - `RuntimeTest` (Debug|x64, Debug|Win32)

## Expected generated artifacts

- `Test\Generated\RpcMetadata(32|64)\Wrapper_RequestService.txt` exists and is deterministic.
- `Test\Generated\Workflow(32|64)\Parsing.Rpc.RequestService.txt` is generated by pass 2.
- If reflection surface changes (new `LifeCycle` methods), expect baseline differences in:
  - `Test\Resources\Baseline\Reflection32.txt`
  - `Test\Resources\Baseline\Reflection64.txt`
  Update baselines only by copying from generated reflection outputs per existing project rules.

# !!!FINISHED!!!

