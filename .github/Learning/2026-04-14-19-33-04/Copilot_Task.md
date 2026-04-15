# !!!TASK!!!

# PROBLEM DESCRIPTION
You are going to implement a RPC attribute checker in the Workflow compiler, the definition is in TODO_RPC_Definition.md, ignore the `## Semantic` part because only code generation uses it, attribute checker does not use it.

Task 1.

Define all attributes in WfLibrary(Predefined|Reflection).(h|cpp) just like those @cpp:* attributes.
These types will only be picked up by the compiler after CompilerTest_GenerateMetadata is executed.

Task 2.

Implement the checker.
You need to create a `WfAnalyzer_ValidateRPC.cpp` and put it in the `VlppWorkflow_Compiler` project, and put the file in the `Pass 06` solution explorer folder.

The file defines the `ValidateModuleRPC` function, it will be exposed in in WfAnalyzer.h. This header file is big, you must extern the function right before the `Expanding Virtual Nodes` title comment, and you also needs to add a title comment for this function called `RPC Analyzing`.

The function will be called in WfAnalyzer.cpp, in function `WfLexicalScopeManager::Rebuild`, right after the loop that calls `ValidateModuleSemantic`:
```
EXIT_IF_ERRORS_EXIST;
ValidateModuleRPC(this, module);
```

The function verify errors in two phase.

The first phase is verifying against AST, with all errors that marked (AST). You need to see if an attribute is put on a wrong thing. During verifying you should write down all interfaces that marked with `@rpc:Interface`, to make the next phase easier.

The second phase is verifying against `ITypeDescriptor`, with the rest of the errors. All `ITypeDescriptor` for workflow types should already been created, you should use the list from the first phase to find out all `ITypeDescriptor` for all interfaces marked with `@rpc:Interface`. Remember some types will also be marked with `@rpc:Interface` but they are not created from the current workflow script (pre-existing), this is why you need to use the list. But remember, an interface marked with `@rpc:Interface` from the list may inherits a pre-existing interface. Pre-existing interfaces are assumed correct, you should not check into them. Therefor go through the list and check their members are good enough.

Each phase should become a function in `WfAnalyzer_ValidateRPC.cpp`, but they don't need to extern in `WfAnalyer.h`.

Errors should always map to static functions in `WfErrors` struct, no exception. Follow the naming convention.

Task 3.

Prepare test cases. You are going to add new test cases to `IndexAnalyzerError.txt`. Test cases in the list are for checking compile errors. All mentioned errors should be included here. Each case only verify against the first generated error (by the error code in the file name: `ERRORCODE_TITLE.txt`), so if an error could be triggered by multiple ways, multiple cases need to be created.

Other note:

You don't need to produce anything else yet in `ValidateModuleRPC`, just generate compile errors.
Since this does not affect code generation, so only CompilerTest_GenerateMetadata (debug Win32 and x64) and CompilerTest_LoadAndCompile (debug x64) are needed to run to validate your change. Only when everything is ready and these two projects pass, run the rest.

# UPDATES

# INSIGHTS AND REASONING

## Existing structure / evidence
- Analyzer pass pipeline: `Source\Analyzer\WfAnalyzer.cpp` runs `ValidateModuleSemantic(this, module);` in `WfLexicalScopeManager::Rebuild` after scope completion.
- `Source\Analyzer\WfAnalyzer.h` lists extern analyzer entry points, and the next section starts at the title comment `Expanding Virtual Nodes`.
- Compile errors are centralized as `static glr::ParsingError ...` helpers on `WfErrors` in `WfAnalyzer.h`, implemented in `Source\Analyzer\WfAnalyzer_Errors.cpp` with a stable `Xnn:` prefix in the message text.
- The shared compiler item project is `Test\UnitTest\VlppWorkflow_Compiler\VlppWorkflow_Compiler.vcxitems`, and the Solution Explorer folder layout is controlled by `VlppWorkflow_Compiler.vcxitems.filters` (currently defines Analyzer\Pass 01..05).

## Task 1 design: predefined RPC attribute types
### What to add
Define 6 new no-argument attribute structs in `Source\Library\WfLibraryPredefined.h` in the same style as existing `vl::__vwsn::att_cpp_*`:
- `vl::__vwsn::att_rpc_Interface`
- `vl::__vwsn::att_rpc_Ctor`
- `vl::__vwsn::att_rpc_Byval`
- `vl::__vwsn::att_rpc_Byref`
- `vl::__vwsn::att_rpc_Cached`
- `vl::__vwsn::att_rpc_Dynamic`

Conventions to match existing attribute structs:
- Must be `struct` types in `namespace vl::__vwsn`.
- No fields (per TODO_RPC_Definition.md: “All attributes here do not have argument”).
- Provide `auto operator<=>(const T&) const = default;` so equality/ordering is consistent with existing attribute types.

### Reflection registration
Update `Source\Library\WfLibraryReflection.h/.cpp` similarly to the `att_cpp_*` types:
- Extend `WORKFLOW_LIBRARY_ATTRIBUTE_TYPES(F)` to include the new `att_rpc_*` types.
- Add `IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_rpc_*, system::workflow_attributes::att_rpc_*)` entries.
- Add `BEGIN_STRUCT_MEMBER(vl::__vwsn::att_rpc_*) ... END_STRUCT_MEMBER(...)` blocks (empty struct body).

Expected effect:
- These attribute types appear in generated reflection metadata and become resolvable by the Workflow compiler only after `CompilerTest_GenerateMetadata` regenerates the metadata snapshot.

## Task 2 design: RPC checker (Pass 06)
### File / project wiring
Add `Source\Analyzer\WfAnalyzer_ValidateRPC.cpp` and include it in the shared items project:
- Add a `ClCompile Include=...\Source\Analyzer\WfAnalyzer_ValidateRPC.cpp` entry to `Test\UnitTest\VlppWorkflow_Compiler\VlppWorkflow_Compiler.vcxitems`.
- Update `VlppWorkflow_Compiler.vcxitems.filters`:
  - Create a new filter folder `Analyzer\Pass 06`.
  - Attach `WfAnalyzer_ValidateRPC.cpp` to this filter.

### Public entry point
Expose one new analyzer entry point from `Source\Analyzer\WfAnalyzer.h`:
- Insert a new title comment section:
  - `RPC Analyzing`
- Add `extern void ValidateModuleRPC(WfLexicalScopeManager* manager, Ptr<WfModule> module);`
- Place this section immediately before the existing `Expanding Virtual Nodes` title comment, as required.

### Call site
Integrate the call in `WfLexicalScopeManager::Rebuild` (`Source\Analyzer\WfAnalyzer.cpp`). The provided snippet references `module`, so the implementation must preserve that variable scope. The intended behavior is:
- Run semantic validation for all modules.
- If no errors were produced, run RPC validation for all modules.

Concretely, this can be expressed as:
- keep the existing semantic-validation loop,
- then:
  - `EXIT_IF_ERRORS_EXIST;`
  - a new loop over `modules` calling `ValidateModuleRPC(this, module);`

This matches the "exit early" style already used between passes in `Rebuild`.

Important placement note:
- `WfLexicalScopeManager::Rebuild` uses a local `EXIT_IF_ERRORS_EXIST` macro which is `#undef`’d near the end of the function. The new RPC validation loop (and its `EXIT_IF_ERRORS_EXIST;` guard) must be inserted before the `#undef EXIT_IF_ERRORS_EXIST`.

### ValidateModuleRPC design: two-phase checking
Implement `ValidateModuleRPC` in `WfAnalyzer_ValidateRPC.cpp` as an orchestrator:
- `ValidateModuleRPC_Ast(...)` (Phase 1)
- `ValidateModuleRPC_Reflection(...)` (Phase 2)

Only `ValidateModuleRPC` is externed in `WfAnalyzer.h`.

#### Phase 1: AST validation (errors marked “(AST)”)
Goal: validate attribute placement / mutual exclusion and any spec items explicitly marked `(AST)`.

Traversal approach:
- Mirror the existing analyzer style (visitor-based traversal used by `ValidateModuleSemantic`), but restrict to `@rpc:*` attributes.
- Track the "current owning type" and (when applicable) the "current owning interface" so member/parameter validations can reason about "inside an interface type with `@rpc:Interface`".

Generic interface detection (for the `@rpc:Interface` rule):
- Workflow interfaces do not declare type parameters directly on `WfClassDeclaration`; instead, type arguments appear in the AST as `vl::workflow::WfFunctionType` (`result` = the base type, `arguments` = type arguments).
- Treat an interface declaration as "generic" (and therefore invalid for `@rpc:Interface`) when any of its `baseTypes` contains a `WfFunctionType` (directly or nested), i.e. it inherits from something like `IBased<T>`.

Per-attribute short-circuit / suppression ordering:
- Implement checks for each attribute occurrence in the exact order defined in `TODO_RPC_Definition.md`.
- Emit at most ONE error per attribute occurrence in Phase 1.
- If an earlier Phase-1 error triggers for an attribute occurrence, do not run later Phase-1 checks for that same attribute occurrence, and do not enqueue any Phase-2 work item for it.

Checks to implement (from `TODO_RPC_Definition.md`, AST-marked only):
- (AST) `@rpc:Interface` can only apply to an interface definition (non-generic per the rule above).
- (AST) `@rpc:Ctor` can only apply to an interface definition with `@rpc:Interface`.
- (AST) `@rpc:Byval` / `@rpc:Byref` can only apply to a property, a method, or a parameter.
- (AST) `@rpc:Cached` / `@rpc:Dynamic` can only apply to a property.
- (AST) Mutual exclusions:
  - `@rpc:Byref` cannot be used when `@rpc:Byval` already exists on the same target.
  - `@rpc:Dynamic` cannot be used when `@rpc:Cached` already exists on the same property.
- (AST) "Inside RPC interface" constraints (AST-marked only for Byval/Byref):
  - `@rpc:Byval` / `@rpc:Byref` can only be used inside an interface type with `@rpc:Interface`.

Output of Phase 1 (inputs for Phase 2):
- Collect workflow-defined interface declarations that carry `@rpc:Interface`.
  - Record both the AST node (e.g. `WfClassDeclaration*`) and its corresponding `ITypeDescriptor*` via `manager->declarationTypes[interfaceDecl]`.
- Collect (only) attribute occurrences that require reflection checks and have passed all Phase-1 checks for that attribute occurrence:
  - `@rpc:Byval` / `@rpc:Byref` occurrences on properties / methods / parameters (for the strong-typed-collection rule).
  - `@rpc:Cached` / `@rpc:Dynamic` occurrences on properties (for the inside-interface rule, which is NOT AST-marked for these attributes).

#### Phase 2: reflection validation (all remaining errors)
Goal: validate the remaining spec items against reflection (`ITypeDescriptor`, `ITypeInfo`) while honoring Phase-1 suppression.

Key constraints from the problem statement:
- Use the Phase 1 list to locate workflow-defined RPC interfaces.
- Pre-existing interfaces may also carry `@rpc:Interface`, but they are not in the Phase 1 list.
- Pre-existing interfaces are assumed correct: do not recursively validate their members.

Per-attribute short-circuit / suppression ordering:
- Only run Phase-2 checks for attribute occurrences explicitly enqueued from Phase 1 (i.e. those that did not trigger any Phase-1 error on the same attribute occurrence).

What Phase 2 validates:
- For each workflow-defined RPC interface from the Phase 1 list (`@rpc:Interface` is present):
  - Base interfaces serializability:
    - For each base interface, require the base interface type itself is a serializable RPC interface (i.e. it carries `@rpc:Interface` in reflected metadata). If not, emit the base-type error.
    - Do NOT recursively validate members of pre-existing base interfaces.
  - Member serializability:
    - Validate every property type and every method signature declared on the workflow-defined interface:
      - all parameter types must be serializable,
      - return type must be serializable, where `void` and `Async` are special-cased as serializable returns per spec.
    - Emit the member error for each failing member.
- For each enqueued `@rpc:Cached` / `@rpc:Dynamic` occurrence:
  - Validate it is inside an interface type that carries `@rpc:Interface` in reflection metadata (this rule is NOT AST-marked for these attributes).
- For each enqueued `@rpc:Byval` / `@rpc:Byref` occurrence:
  - Validate the strong-typed-collection constraint against the associated type:
    - property: property type
    - parameter: parameter type
    - method: method return type

Notes on the "serializable type" predicate (make it concrete and reuse existing helpers when possible):
- Prefer implementing one reusable predicate over `Ptr<ITypeInfo>` (with access to the owning `ITypeDescriptor` when needed).
- Use reflection classification (e.g. `ITypeDescriptor::GetTypeDescriptorFlags()`) for predefined primitives / structs / enums.
- Use `ITypeInfo` hints to identify strong typed collections (Workflow syntax `T[]`, `T{}`, `T{V}`), e.g. `TypeInfoHint::Array`, `TypeInfoHint::List`, `TypeInfoHint::Dictionary`, and validate their element/key/value types recursively.
- For interface types:
  - only allow shared pointer (`T^` / `TypeInfoDecorator::SharedPtr`) to an interface carrying `@rpc:Interface`.
  - reject raw pointer (`T*` / `TypeInfoDecorator::RawPtr`).
- Weak typed collection abstractions (e.g. `system::Enumerable`) are just interfaces without `@rpc:Interface` and are therefore rejected by the same rule above.
- Method return special cases: accept `void` and `Async` as serializable return types per `TODO_RPC_Definition.md` (prefer reusing any existing "is async type" helper already used by the compiler/type system rather than hard-coding names).
## Error mapping (WfErrors)
All RPC-related diagnostics must be implemented as `static` helpers on `WfErrors`:
- Add a new "H" family of errors (error codes `H\d+`) following the existing convention.
- Each error text should closely follow `TODO_RPC_Definition.md` (excluding the “(AST)” annotation).
- Implement declarations in `WfAnalyzer.h` and definitions in `WfAnalyzer_Errors.cpp`.

Error helper grouping (per spec):
- Plan helper sharing up-front: errors with identical formats should share one helper function, with only the `xxx` fragments becoming parameters.
  - Example group candidates:
    - "`@rpc:XXX` can only apply to ..." (across multiple attributes)
    - "cannot be used on member `MEMBER-NAME` because it already has ..." (`Byref` vs `Byval`, `Dynamic` vs `Cached`)
    - "can only be used inside an interface type with `@rpc:Interface`" (multiple attributes)

Suppression / ordering:
- Implement per-attribute short-circuiting exactly as in the spec: for each attribute occurrence, evaluate checks in order and report only the first applicable error for that attribute occurrence.
- Phase 2 must only run checks for attribute occurrences that passed all Phase-1 checks for that attribute occurrence.

FULL-NAME / MEMBER-NAME formatting and anchoring:
- Ensure error texts follow the spec naming:
  - `FULL-NAME` is the type full name.
  - For members: `type.member`.
  - For parameters: `type.method(parameter)`.
- Prefer using reflection names for Phase-2 errors (e.g. `ITypeDescriptor::GetTypeName()`, `IMemberInfo::GetName()`), and keep Phase-1 formatting consistent with Phase-2 formatting.
- Attach errors to the most relevant AST node for code range:
  - placement/mutual-exclusion errors: the `WfAttribute*` node.
  - base-type errors: the base-type node (or interface declaration node if a base-type node is not available).
  - member/parameter-related errors: the failing member declaration node / argument node when possible (fall back to the owning interface declaration node if necessary for consistency).
## Task 3 design: analyzer-error test coverage
### File format / indexing
- Each analyzer error test is a `.txt` Workflow module under `Test\Resources\AnalyzerError\`.
- `Test\Resources\IndexAnalyzerError.txt` lists basenames (no extension). Each entry corresponds to one file.
- Each test only checks the first reported error, determined by the error code embedded in the file name.

### Planned test cases
Add new `H*` cases covering every error mentioned in TODO_RPC_Definition.md. When the same error can be triggered in multiple distinct ways, create multiple files so each one reliably produces that error first.

Examples of minimal scenario coverage (non-exhaustive):
- `@rpc:Interface` applied to a non-interface declaration.
- `@rpc:Ctor` applied to:
  - a non-interface declaration,
  - an interface without `@rpc:Interface`.
- `@rpc:Byval` / `@rpc:Byref` applied to an unsupported target (e.g. variable declaration).
- `@rpc:Byval` / `@rpc:Byref` applied to a member or parameter outside any `@rpc:Interface` interface (Phase 1 inside-interface failure).
- `@rpc:Byref` on a member that already has `@rpc:Byval`.
- `@rpc:Cached` / `@rpc:Dynamic` applied to non-property targets.
- `@rpc:Dynamic` on a property that already has `@rpc:Cached`.
- `@rpc:Cached` used on a property inside a non-`@rpc:Interface` interface (Phase 2 inside-interface failure).
- `@rpc:Dynamic` used on a property inside a non-`@rpc:Interface` interface (Phase 2 inside-interface failure).
- Interface base type without `@rpc:Interface` (including an interface inheriting from a pre-existing base interface lacking `@rpc:Interface`).
- Interface member with unserializable types (e.g. raw pointers `T*` to RPC interfaces, weak-typed interfaces such as `system::Enumerable`, etc.).
- `@rpc:Byval` / `@rpc:Byref` strong-typed-collection failures:
  - property type is not a strong typed collection,
  - method return type is not a strong typed collection,
  - parameter type is not a strong typed collection.
- Valid (negative) cases to prevent false positives:
  - a method returning `void` in an RPC interface,
  - a method returning `Async` in an RPC interface.
## Verification plan
Per the problem statement, since this only adds attributes + analyzer errors (no codegen impact yet), initial verification focuses on:
- `CompilerTest_GenerateMetadata` (Debug|Win32 and Debug|x64): ensures new `att_rpc_*` reflected types are generated and picked up.
- `CompilerTest_LoadAndCompile` (Debug|x64): ensures the compiler reports the new analyzer errors and new analyzer-error test cases pass.

Only after these pass should the rest of unit tests be executed.

# AFFECTED PROJECTS
- Build the solution in folder `REPO-ROOT\Test\UnitTest` (`Debug|x64`).
- Always Run UnitTest project `CompilerTest_GenerateMetadata` (`Debug|x64`).
  - If reflection metadata baselines change, update `REPO-ROOT\Test\Resources\Baseline\Reflection64.txt` from `REPO-ROOT\Test\UnitTest\Generated\Reflection64.txt` and rerun.
- Always Run UnitTest project `CompilerTest_LoadAndCompile` (`Debug|x64`).

- Build the solution in folder `REPO-ROOT\Test\UnitTest` (`Debug|Win32`).
- Always Run UnitTest project `CompilerTest_GenerateMetadata` (`Debug|Win32`).
  - If reflection metadata baselines change, update `REPO-ROOT\Test\Resources\Baseline\Reflection32.txt` from `REPO-ROOT\Test\UnitTest\Generated\Reflection32.txt` and rerun.

- After the above projects pass and the RPC checker is stable, run the remaining unit test projects per `REPO-ROOT\Project.md`.

# !!!FINISHED!!!
