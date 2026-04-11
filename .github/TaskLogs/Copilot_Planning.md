# !!!PLANNING!!!

# UPDATES

# AFFECTED PROJECTS
- Build the solution in folder `REPO-ROOT\Test\UnitTest` (`Debug|x64`).
- Always Run UnitTest project `CompilerTest_GenerateMetadata` (`Debug|x64`).
  - If reflection metadata baselines change, update `REPO-ROOT\Test\Resources\Baseline\Reflection64.txt` from `REPO-ROOT\Test\UnitTest\Generated\Reflection64.txt` and rerun.
- Always Run UnitTest project `CompilerTest_LoadAndCompile` (`Debug|x64`).

- Build the solution in folder `REPO-ROOT\Test\UnitTest` (`Debug|Win32`).
- Always Run UnitTest project `CompilerTest_GenerateMetadata` (`Debug|Win32`).
  - If reflection metadata baselines change, update `REPO-ROOT\Test\Resources\Baseline\Reflection32.txt` from `REPO-ROOT\Test\UnitTest\Generated\Reflection32.txt` and rerun.

- After the above projects pass and the RPC checker is stable, run the remaining unit test projects per `REPO-ROOT\Project.md`.

# EXECUTION PLAN

## STEP 1: Add @rpc attribute types to the Workflow library
### What to change
Modify `REPO-ROOT\Source\Library\WfLibraryPredefined.h` in `namespace vl::__vwsn` (next to existing `att_cpp_*` structs) to add 6 no-argument attribute structs:
- `att_rpc_Interface`
- `att_rpc_Ctor`
- `att_rpc_Byval`
- `att_rpc_Byref`
- `att_rpc_Cached`
- `att_rpc_Dynamic`

Proposed code to append after `att_cpp_Friend` (exact placement flexible as long as still inside `namespace vl::__vwsn`):

    struct att_rpc_Interface
    {
    	auto operator<=>(const att_rpc_Interface&) const = default;
    };

    struct att_rpc_Ctor
    {
    	auto operator<=>(const att_rpc_Ctor&) const = default;
    };

    struct att_rpc_Byval
    {
    	auto operator<=>(const att_rpc_Byval&) const = default;
    };

    struct att_rpc_Byref
    {
    	auto operator<=>(const att_rpc_Byref&) const = default;
    };

    struct att_rpc_Cached
    {
    	auto operator<=>(const att_rpc_Cached&) const = default;
    };

    struct att_rpc_Dynamic
    {
    	auto operator<=>(const att_rpc_Dynamic&) const = default;
    };

### Why
- The compiler resolves Workflow attributes by their reflected types. These structs (and the reflection wiring in the next step) define the attribute types so `@rpc:*` becomes a known attribute category for the compiler.
- All attributes in `TODO_RPC_Definition.md` are explicitly “no argument”, so these structs must be empty.

## STEP 2: Register @rpc attributes in library reflection
### What to change
Modify:
- `REPO-ROOT\Source\Library\WfLibraryReflection.h`
- `REPO-ROOT\Source\Library\WfLibraryReflection.cpp`

1) In `WfLibraryReflection.h`, extend `WORKFLOW_LIBRARY_ATTRIBUTE_TYPES(F)`:

    #define WORKFLOW_LIBRARY_ATTRIBUTE_TYPES(F)\
    		F(vl::__vwsn::att_cpp_File)\
    		F(vl::__vwsn::att_cpp_UserImpl)\
    		F(vl::__vwsn::att_cpp_Private)\
    		F(vl::__vwsn::att_cpp_Protected)\
    		F(vl::__vwsn::att_cpp_Friend)\
    		F(vl::__vwsn::att_rpc_Interface)\
    		F(vl::__vwsn::att_rpc_Ctor)\
    		F(vl::__vwsn::att_rpc_Byval)\
    		F(vl::__vwsn::att_rpc_Byref)\
    		F(vl::__vwsn::att_rpc_Cached)\
    		F(vl::__vwsn::att_rpc_Dynamic)\

2) In `WfLibraryReflection.cpp`:
- Add `IMPL_TYPE_INFO_RENAME(...)` lines near the existing `att_cpp_*` renames:

    IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_rpc_Interface, system::workflow_attributes::att_rpc_Interface)
    IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_rpc_Ctor, system::workflow_attributes::att_rpc_Ctor)
    IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_rpc_Byval, system::workflow_attributes::att_rpc_Byval)
    IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_rpc_Byref, system::workflow_attributes::att_rpc_Byref)
    IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_rpc_Cached, system::workflow_attributes::att_rpc_Cached)
    IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_rpc_Dynamic, system::workflow_attributes::att_rpc_Dynamic)

- Add reflection member blocks in the metadata section (empty structs, so no `STRUCT_MEMBER` lines):

    BEGIN_STRUCT_MEMBER(vl::__vwsn::att_rpc_Interface)
    END_STRUCT_MEMBER(vl::__vwsn::att_rpc_Interface)

    BEGIN_STRUCT_MEMBER(vl::__vwsn::att_rpc_Ctor)
    END_STRUCT_MEMBER(vl::__vwsn::att_rpc_Ctor)

    BEGIN_STRUCT_MEMBER(vl::__vwsn::att_rpc_Byval)
    END_STRUCT_MEMBER(vl::__vwsn::att_rpc_Byval)

    BEGIN_STRUCT_MEMBER(vl::__vwsn::att_rpc_Byref)
    END_STRUCT_MEMBER(vl::__vwsn::att_rpc_Byref)

    BEGIN_STRUCT_MEMBER(vl::__vwsn::att_rpc_Cached)
    END_STRUCT_MEMBER(vl::__vwsn::att_rpc_Cached)

    BEGIN_STRUCT_MEMBER(vl::__vwsn::att_rpc_Dynamic)
    END_STRUCT_MEMBER(vl::__vwsn::att_rpc_Dynamic)

### Why
- `ValidateModuleRPC` phase 2 needs to query `IAttributeBag` on `ITypeDescriptor` / `IMemberInfo` and compare the attribute type descriptor. These types must exist in reflection metadata.
- The compiler only “picks up” new attribute types after `CompilerTest_GenerateMetadata` regenerates the reflection snapshot; this is why Task 1 is required.

## STEP 3: Add a new analyzer pass source file and include it in the compiler shared-items project
### What to change
Create `REPO-ROOT\Source\Analyzer\WfAnalyzer_ValidateRPC.cpp`.

Add it to:
- `REPO-ROOT\Test\UnitTest\VlppWorkflow_Compiler\VlppWorkflow_Compiler.vcxitems`
  - Add a `ClCompile Include="..\..\..\Source\Analyzer\WfAnalyzer_ValidateRPC.cpp"` entry.
- `REPO-ROOT\Test\UnitTest\VlppWorkflow_Compiler\VlppWorkflow_Compiler.vcxitems.filters`
  - Add a new filter folder `Analyzer\Pass 06`.
  - Put `WfAnalyzer_ValidateRPC.cpp` under that filter.

### Why
- The repo uses the `VlppWorkflow_Compiler` shared-items project as the compiler build unit. New analyzer files must be added there to compile.
- “Pass 06” keeps consistent Solution Explorer organization with existing analyzer passes.

## STEP 4: Declare the public entry point and the new pass section in WfAnalyzer.h
### What to change
Modify `REPO-ROOT\Source\Analyzer\WfAnalyzer.h`:
- Insert a new title section immediately before the existing `Expanding Virtual Nodes` title comment:

    /***********************************************************************
    RPC Analyzing
    ***********************************************************************/

    extern void ValidateModuleRPC(WfLexicalScopeManager* manager, Ptr<WfModule> module);

### Why
- Task requirement: the `extern` must appear right before `Expanding Virtual Nodes` and must be under its own title comment.
- The pass is invoked from `WfLexicalScopeManager::Rebuild`, so it must be visible from `WfAnalyzer.cpp`.

## STEP 5: Add RPC-related error declarations (H*) to WfErrors
### What to change
Modify `REPO-ROOT\Source\Analyzer\WfAnalyzer.h` and `REPO-ROOT\Source\Analyzer\WfAnalyzer_Errors.cpp`.

### Error code assignment (consolidated per helper-format sharing rule)
Per `TODO_RPC_Definition.md` (“multiple error messages with the same format can share one helper function … each helper function will be assigned with a unique error code”), assign codes per helper template (not per bullet):

- H0: `@rpc:Interface` can only apply to an interface definition, but not `FULL-NAME`.
- H1: `@rpc:Interface` cannot be applied to interface `FULL-NAME` because its base type `FULL-NAME` is not serializable.
- H2: `@rpc:Interface` cannot be applied to interface `FULL-NAME` because its member `MEMBER-NAME` is not serializable.
- H3: `@rpc:Ctor` can only apply to an interface definition with `@rpc:Interface`.
- H4: `@rpc:xxx` can only apply to a property, a method or a parameter. (`xxx` = `Byval` | `Byref`)
- H5: `@rpc:xxx` can only be used inside an interface type with `@rpc:Interface`. (`xxx` = `Byval` | `Byref` | `Cached` | `Dynamic`)
- H6: `@rpc:xxx` cannot be used on member `MEMBER-NAME` because it does not have a strong typed collection types. (`xxx` = `Byval` | `Byref`)
- H7: `@rpc:xxx` can only apply to a property. (`xxx` = `Cached` | `Dynamic`)
- H8: `@rpc:xxx` cannot be used on member `MEMBER-NAME` because it already has `@rpc:yyy`. (`xxx`,`yyy` = (`Byref`,`Byval`) or (`Dynamic`,`Cached`))

Notes:
- Analyzer-error tests can still (and should) include multiple files per template to cover distinct trigger paths; they will share the same `Hn_` prefix.

### Proposed WfErrors API (declarations)
Add at the end of `struct WfErrors`:

    // H: RPC attribute checking
    static glr::ParsingError			RpcInterfaceCanOnlyApplyToInterface(WfAttribute* node, const WString& fullName);											// H0
    static glr::ParsingError			RpcInterfaceBaseNotSerializable(WfType* node, const WString& interfaceFullName, const WString& baseFullName);						// H1
    static glr::ParsingError			RpcInterfaceMemberNotSerializable(WfDeclaration* node, const WString& interfaceFullName, const WString& memberName);						// H2
    static glr::ParsingError			RpcInterfaceMemberNotSerializable(WfFunctionArgument* node, const WString& interfaceFullName, const WString& memberName);					// H2

    static glr::ParsingError			RpcCtorCanOnlyApplyToRpcInterface(WfAttribute* node);																// H3

    static glr::ParsingError			RpcAttributeCanOnlyApplyToPropertyMethodOrParameter(WfAttribute* node, const WString& attributeName);								// H4
    static glr::ParsingError			RpcAttributeCanOnlyBeUsedInsideRpcInterface(WfAttribute* node, const WString& attributeName);									// H5
    static glr::ParsingError			RpcAttributeRequiresStrongTypedCollection(WfAttribute* node, const WString& attributeName, const WString& memberName);						// H6
    static glr::ParsingError			RpcAttributeCanOnlyApplyToProperty(WfAttribute* node, const WString& attributeName);									// H7
    static glr::ParsingError			RpcAttributeCannotCoexistWithOther(WfAttribute* node, const WString& attributeName, const WString& otherAttributeName, const WString& memberName);		// H8

Anchoring rule reminder (for better diagnostics):
- H0/H3/H4/H5/H6/H7/H8 anchor to the attribute node (`WfAttribute*`).
- H1 anchors to the specific base-type AST node (`WfType*`).
- H2 anchors to the failing member declaration (`WfDeclaration*`) or failing parameter node (`WfFunctionArgument*`) when available.

### Why
- All RPC diagnostics must be expressed as `WfErrors` helpers.
- Using a stable H-code mapping makes analyzer-error tests deterministic (tests only assert the `Hn:` prefix).

## STEP 6: Implement ValidateModuleRPC in a two-phase design (AST phase + reflection phase)
### What to change
Implement in `REPO-ROOT\Source\Analyzer\WfAnalyzer_ValidateRPC.cpp`:

1) Public entry point:

    #include "WfAnalyzer.h"

    namespace vl
    {
    	namespace workflow
    	{
    		namespace analyzer
    		{
    			using namespace collections;
    			using namespace reflection;
    			using namespace reflection::description;

    			static void ValidateModuleRPC_Ast(...);
    			static void ValidateModuleRPC_Reflection(...);

    			void ValidateModuleRPC(WfLexicalScopeManager* manager, Ptr<WfModule> module)
    			{
    				// Phase 1 collects workflow-defined RPC interfaces and enqueues attribute occurrences for phase 2.
    				// Phase 2 uses reflection (ITypeDescriptor/ITypeInfo) to validate the remaining rules.
    			}
    		}
    	}
    }

2) Phase 1: AST validation (deterministic, two sub-passes)

Key responsibilities (ONLY errors marked “(AST)” in `TODO_RPC_Definition.md`), and ensure results are deterministic (not declaration-order dependent):

- Sub-pass 1a (type-level scan):
  - Validate `@rpc:Interface` placement: only on an interface definition, and the interface must be non-generic per spec (i.e. its `baseTypes` contains no `WfFunctionType` occurrence such as `Async<string>`).
    - If valid, record the interface as a workflow-defined RPC interface for Phase 2.
  - Validate `@rpc:Ctor` placement: only on an interface definition with `@rpc:Interface`.

- Sub-pass 1b (member-level scan, after the RPC-interface set is complete):
  - Validate placement:
    - `@rpc:Byval/@rpc:Byref` only on property/method/parameter.
    - `@rpc:Cached/@rpc:Dynamic` only on property.
  - Validate AST mutual exclusions:
    - `@rpc:Byref` cannot be used when `@rpc:Byval` exists on the same target.
    - `@rpc:Dynamic` cannot be used when `@rpc:Cached` exists on the same property.
  - Validate AST inside-RPC-interface constraint (AST-marked only for `Byval`/`Byref`):
    - `@rpc:Byval/@rpc:Byref` can only be used inside an interface type with `@rpc:Interface`.

Per-attribute suppression convention (matches the spec wording):
- For each attribute occurrence, evaluate checks in the exact order in `TODO_RPC_Definition.md` and emit at most ONE error for that occurrence.
- Only enqueue Phase-2 work items when the attribute occurrence passed all Phase-1 checks for that occurrence.

Phase-1 outputs to feed Phase 2:
- `rpcInterfaceDecls`: workflow-defined interfaces in this module that have a valid `@rpc:Interface`.
  - Capture: `WfClassDeclaration* interfaceDecl`, its `ITypeDescriptor* interfaceTd` (resolved from `manager`), and the `WfAttribute*` node (for anchoring H0 if needed).
- `rpcInterfaceTds`: a fast membership set built from `rpcInterfaceDecls` (e.g. `SortedList<ITypeDescriptor*>`), used to recognize workflow-defined RPC interfaces during Phase 2.
- Enqueued “phase 2 checks” work items (each carries sufficient context; do not enqueue a raw `WfAttribute*` alone):
  - For `@rpc:Byval/@rpc:Byref`: { attribute node, owning type declaration (interface/class), member kind (property/method/parameter), and member identity (memberName; for parameters also include methodName + parameterIndex/parameterName) }.
  - For `@rpc:Cached/@rpc:Dynamic`: { attribute node, owning type declaration (interface/class), propertyName }.

IMPORTANT (workflow-defined attribute visibility):
- Workflow-defined `@rpc:*` attributes are populated into reflection in the emitter phase (after analysis). Phase 2 must not rely on `HasAttribute(td, rpcInterfaceAttrTd)` to detect workflow-defined `@rpc:Interface`.
  - For workflow-defined types: use `rpcInterfaceTds`.
  - For pre-existing/imported types: use reflection attributes.

Implementation notes / concrete AST patterns:
- Interface syntax:

    interface IFoo
    {
    	func M(a : int) : void;
    	prop P : int {GetP}
    }

- Parameter attributes live on `WfFunctionArgument::attributes`.
- To detect “generic interface” per spec, scan `interfaceDecl->baseTypes` for any `WfFunctionType` occurrence (direct or nested inside pointer/child types).

Recommended helper for generic-base detection:

    static bool ContainsFunctionType(WfType* type)
    {
    	class V : public Object, public WfType::IVisitor
    	{
    	public:
    		bool found = false;
    		void Visit(WfFunctionType* node)override { found = true; }
    		void Visit(WfRawPointerType* node)override { node->element->Accept(this); }
    		void Visit(WfSharedPointerType* node)override { node->element->Accept(this); }
    		void Visit(WfNullableType* node)override { node->element->Accept(this); }
    		void Visit(WfEnumerableType* node)override { node->element->Accept(this); }
    		void Visit(WfMapType* node)override { if(node->key) node->key->Accept(this); node->value->Accept(this); }
    		void Visit(WfObservableListType* node)override { node->element->Accept(this); }
    		void Visit(WfChildType* node)override { node->parent->Accept(this); }
    		void Visit(WfPredefinedType* node)override {}
    		void Visit(WfTopQualifiedType* node)override {}
    		void Visit(WfReferenceType* node)override {}
    	};
    	V v; type->Accept(&v); return v.found;
    }

3) Phase 2: reflection validation

Key responsibilities (all remaining errors), while respecting workflow-defined attribute visibility:

- Validate the `@rpc:Interface` semantics for workflow-defined RPC interfaces only (those collected in Phase 1):
  - Base interfaces:
    - For each explicitly declared base type in `interfaceDecl->baseTypes`, require the resolved base type is serializable.
    - Concretely, the base must be an interface marked with `@rpc:Interface`.
      - If the base type is workflow-defined in the current compilation: treat it as an RPC interface only when its `ITypeDescriptor*` is in `rpcInterfaceTds`.
      - If the base type is pre-existing/imported: treat it as an RPC interface only when reflection reports it has `@rpc:Interface` (via `HasAttribute(baseTd, rpcInterfaceAttrTd)`).
    - Emit H1 once per ineligible base type, anchored on the specific base-type AST node (not the `@rpc:Interface` attribute).
  - Member serializability:
    - Validate every property type and every method signature declared on the workflow-defined RPC interface:
      - all parameter types must be serializable,
      - return type must be serializable, where `void` and `Async` are special-cased as serializable return types per spec.
    - Emit H2 once per ineligible member/parameter, anchored on the failing member declaration / parameter node when available.
  - Pre-existing base interfaces are assumed correct: do not recursively validate members of imported base interfaces.

- Validate inside-interface rules that are NOT AST-marked:
  - `@rpc:Cached/@rpc:Dynamic` can only be used inside an interface type with `@rpc:Interface`.
  - For workflow-defined owning types, use `rpcInterfaceTds` membership instead of reflection attributes (reflection attributes are not yet populated).

- Validate strong-typed-collection rule:
  - `@rpc:Byval/@rpc:Byref` requires the associated type to be a strong typed collection (Workflow collection syntax).
  - The work item must already identify which type to check:
    - property: `IPropertyInfo::GetReturn()`
    - method: `IMethodInfo::GetReturn()`
    - parameter: `IParameterInfo::GetType()`

Concrete reflection APIs to use (verified in `Import\VlppReflection.h`):
- Attributes:
  - `IAttributeBag::GetAttributeCount()` / `GetAttribute(i)` / `IAttributeInfo::GetAttributeType()`.
- Member types:
  - `IPropertyInfo::GetReturn()`
  - `IMethodInfo::GetReturn()`, `GetParameterCount()`, `GetParameter(i)->GetType()`

Concrete "strong typed collection" shape (verified in `WfAnalyzer_TypeInfo.cpp`):
- Workflow collection syntax is compiled to:
  - `SharedPtr` → `Generic` → `TypeDescriptor` is one of:
    - `IValueEnumerable` (1 arg)
    - `IValueReadonlyList` / `IValueList` (1 arg)
    - `IValueReadonlyDictionary` / `IValueDictionary` (2 args)
    - `IValueObservableList` (1 arg)

Proposed helper:

    static bool IsStrongTypedCollection(ITypeInfo* type)
    {
    	if (!type || type->GetDecorator() != ITypeInfo::SharedPtr) return false;
    	auto e = type->GetElementType();
    	if (!e || e->GetDecorator() != ITypeInfo::Generic) return false;

    	auto td = e->GetTypeDescriptor();
    	return td == GetTypeDescriptor<IValueEnumerable>()
    		|| td == GetTypeDescriptor<IValueReadonlyList>()
    		|| td == GetTypeDescriptor<IValueList>()
    		|| td == GetTypeDescriptor<IValueReadonlyDictionary>()
    		|| td == GetTypeDescriptor<IValueDictionary>()
    		|| td == GetTypeDescriptor<IValueObservableList>();
    }

Serializable-type predicate (matches `TODO_RPC_Definition.md` strictly):
- Primitive / struct / enum: accept based on `ITypeDescriptor::GetTypeDescriptorFlags()`.
  - Exclude void explicitly (Workflow void is `description::GetTypeDescriptor<VoidValue>()`).
- Strong typed collections: accept if it is a strong typed collection AND all generic arguments are serializable.
- Interface type: accept ONLY when the type is `SharedPtr` to an interface that has `@rpc:Interface`.
  - For workflow-defined interfaces in the current compilation: use `rpcInterfaceTds` membership.
  - For pre-existing/imported interfaces: use `HasAttribute(td, rpcInterfaceAttrTd)`.
- NOTE: `Nullable<T>` is not listed as serializable in `TODO_RPC_Definition.md`; do not accept it unless the spec is explicitly extended.

Proposed helpers (pseudocode-level):

    static bool HasAttribute(IAttributeBag* bag, ITypeDescriptor* attrTd)
    {
    	for (vint i = 0; i < bag->GetAttributeCount(); i++)
    	{
    		if (bag->GetAttribute(i)->GetAttributeType() == attrTd) return true;
    	}
    	return false;
    }

    static bool IsRpcInterfaceTd(ITypeDescriptor* td, ITypeDescriptor* rpcInterfaceAttrTd, const collections::SortedList<ITypeDescriptor*>& rpcInterfaceTds)
    {
    	if (!td) return false;
    	if (rpcInterfaceTds.Contains(td)) return true;
    	return HasAttribute(td, rpcInterfaceAttrTd);
    }

    static bool IsSerializableType(ITypeInfo* type, ITypeDescriptor* rpcInterfaceAttrTd, const collections::SortedList<ITypeDescriptor*>& rpcInterfaceTds)
    {
    	if (!type) return false;
    	switch (type->GetDecorator())
    	{
    	case ITypeInfo::SharedPtr:
    		{
    			auto e = type->GetElementType();
    			if (!e) return false;

    			// strong typed collections
    			if (e->GetDecorator() == ITypeInfo::Generic)
    			{
    				if (!IsStrongTypedCollection(type)) return false;
    				for (vint i = 0; i < e->GetGenericArgumentCount(); i++)
    				{
    					if (!IsSerializableType(e->GetGenericArgument(i), rpcInterfaceAttrTd, rpcInterfaceTds)) return false;
    				}
    				return true;
    			}

    			// interface pointers
    			if (e->GetDecorator() == ITypeInfo::TypeDescriptor)
    			{
    				auto td = e->GetTypeDescriptor();
    				return (td->GetTypeDescriptorFlags() & TypeDescriptorFlags::Interface) != TypeDescriptorFlags::Undefined
    					&& IsRpcInterfaceTd(td, rpcInterfaceAttrTd, rpcInterfaceTds);
    			}

    			return false;
    		}

    	case ITypeInfo::TypeDescriptor:
    		{
    			auto td = type->GetTypeDescriptor();
    			if (td == GetTypeDescriptor<VoidValue>()) return false;
    			switch (td->GetTypeDescriptorFlags())
    			{
    			case TypeDescriptorFlags::Primitive:
    			case TypeDescriptorFlags::Struct:
    			case TypeDescriptorFlags::FlagEnum:
    			case TypeDescriptorFlags::NormalEnum:
    				return true;
    			default:
    				return false;
    			}
    		}

    	default:
    		return false;
    	}
    }

Serializable return type predicate:
- Allow `void` (i.e. `VoidValue`) and allow `Async` (reflection name `system::Async`, type descriptor `GetTypeDescriptor<reflection::description::IAsync>()` renamed in the library) in addition to `IsSerializableType`.


### Why
- The spec explicitly separates AST-only checks and reflection-based checks.
- Using workflow-defined RPC interface list avoids validating members of pre-existing interfaces (assumed correct) while still enforcing that bases are RPC interfaces.
- Using the concrete type-info shapes from `WfAnalyzer_TypeInfo.cpp` prevents mis-detecting collection types (type hints are always `Normal` for these).

## STEP 7: Wire ValidateModuleRPC into the analyzer pipeline
### What to change
Modify `REPO-ROOT\Source\Analyzer\WfAnalyzer.cpp`, in `WfLexicalScopeManager::Rebuild`.

Insert after the existing semantic-validation loop, matching the “run semantic for all modules, then run RPC for all modules” intent:

    EXIT_IF_ERRORS_EXIST;
    for (auto module : modules)
    {
    	ValidateModuleRPC(this, module);
    }

IMPORTANT: place this before `#undef EXIT_IF_ERRORS_EXIST`.

### Why
- The RPC checker must run after semantic validation (so type descriptors and member infos are fully built) but before later passes.
- The `EXIT_IF_ERRORS_EXIST` guard ensures RPC errors don’t cascade on top of semantic errors.

# TEST PLAN

## Coverage requirements
Add analyzer-error tests that cover every compile error message described in `TODO_RPC_Definition.md`.

Notes:
- The harness checks only the error-code prefix derived from filename (`Hn_...`), and only asserts the FIRST reported error.
- Because H-codes are consolidated per helper-template (STEP 5), multiple spec bullets will intentionally share a single H-code.
  - Still add multiple test files when the same template is triggered by different attributes or different trigger paths.

## Planned new analyzer-error test files
Update:
- `REPO-ROOT\Test\Resources\IndexAnalyzerError.txt` (append all new basenames)
- Add new `.txt` files under `REPO-ROOT\Test\Resources\AnalyzerError\`.

Proposed minimal cases (grouped by consolidated H-code; multiple files per code where needed):

H0_RpcInterface_NotOnInterface_Class.txt

    module test;

    @rpc:Interface
    class A
    {
    }

H0_RpcInterface_OnGenericInterface.txt

    module test;
    using system::*;

    @rpc:Interface
    $interface IMyAsync : Async<string>;

H1_RpcInterface_BaseNotSerializable.txt

    module test;

    interface IBase
    {
    }

    @rpc:Interface
    interface IDerived : IBase
    {
    }

H2_RpcInterface_MemberNotSerializable.txt

    module test;

    @rpc:Interface
    interface IMy
    {
    	// raw pointer to interface is not serializable (must be ^)
    	func M(x : IMy*) : void;
    }

H3_RpcCtor_NotOnRpcInterface_Interface.txt

    module test;

    @rpc:Ctor
    interface IMy
    {
    }

H3_RpcCtor_NotOnRpcInterface_Class.txt

    module test;

    @rpc:Ctor
    class A
    {
    }

H4_RpcByval_WrongTarget.txt

    module test;

    interface IMy
    {
    }

    @rpc:Byval
    var x = 0;

H4_RpcByref_WrongTarget.txt

    module test;

    interface IMy
    {
    }

    @rpc:Byref
    var x = 0;

H5_RpcByval_NotInsideRpcInterface.txt

    module test;

    interface IMy
    {
    	@rpc:Byval
    	prop P : int {GetP}

    	func GetP() : int;
    }

H5_RpcByref_NotInsideRpcInterface.txt

    module test;

    interface IMy
    {
    	@rpc:Byref
    	func M() : int[];
    }

H5_RpcCached_NotInsideRpcInterface.txt

    module test;

    interface IMy
    {
    	@rpc:Cached
    	prop P : int {GetP}

    	func GetP() : int;
    }

H5_RpcDynamic_NotInsideRpcInterface.txt

    module test;

    interface IMy
    {
    	@rpc:Dynamic
    	prop P : int {GetP}

    	func GetP() : int;
    }

H6_RpcByval_NotStrongCollection.txt

    module test;

    @rpc:Interface
    interface IMy
    {
    	@rpc:Byval
    	func M() : int;
    }

H6_RpcByref_NotStrongCollection.txt

    module test;

    @rpc:Interface
    interface IMy
    {
    	@rpc:Byref
    	func M() : int;
    }

H7_RpcCached_WrongTarget.txt

    module test;

    @rpc:Cached
    func main() : void
    {
    }

H7_RpcDynamic_WrongTarget.txt

    module test;

    @rpc:Dynamic
    func main() : void
    {
    }

H8_RpcByref_WithByval.txt

    module test;

    @rpc:Interface
    interface IMy
    {
    	@rpc:Byval
    	@rpc:Byref
    	func M() : int[];
    }

H8_RpcDynamic_WithCached.txt

    module test;

    @rpc:Interface
    interface IMy
    {
    	@rpc:Cached
    	@rpc:Dynamic
    	prop P : int {GetP}

    	func GetP() : int;
    }

## Positive / non-error sanity checks (recommended)
Analyzer-error tests only cover failures. To prevent overly-restrictive implementations (especially for `@rpc:Byval/@rpc:Byref` on valid strong typed collections), add a couple of legal scripts that actually run through analyzer + codegen:

- Add 2 new Codegen samples under `REPO-ROOT\Test\Resources\Codegen\` and append to `REPO-ROOT\Test\Resources\IndexCodegen.txt` with expected output `OK`:

  RpcByval_Positive=OK

      module test;
      using system::*;

      @rpc:Interface
      interface IRpc
      {
      	@rpc:Byval
      	func GetAll() : int[];
      }

      func main() : string
      {
      	return "OK";
      }

  RpcByref_Positive=OK

      module test;
      using system::*;

      @rpc:Interface
      interface IRpc
      {
      	@rpc:Byref
      	func GetAll() : int[];
      }

      func main() : string
      {
      	return "OK";
      }

These two scripts will be compiled (and executed) by existing Codegen/Runtime unit tests, ensuring valid `@rpc:Byval/@rpc:Byref` usages are accepted.

# !!!FINISHED!!!
