# !!!EXECUTION!!!

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
Edit `REPO-ROOT\Source\Library\WfLibraryPredefined.h`.

In `namespace vl::__vwsn` (next to the existing `att_cpp_*` structs), append the following 6 no-argument attribute structs (placement is flexible as long as it is still inside `namespace vl::__vwsn`).

Insert (proposed to append after `att_cpp_Friend`):

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

## STEP 2: Register @rpc attributes in library reflection
Modify:
- `REPO-ROOT\Source\Library\WfLibraryReflection.h`
- `REPO-ROOT\Source\Library\WfLibraryReflection.cpp`

### STEP 2.1: WfLibraryReflection.h
Extend `WORKFLOW_LIBRARY_ATTRIBUTE_TYPES(F)` to include the new `att_rpc_*` types.

The resulting macro should look like (add the new `att_rpc_*` lines; do not remove existing ones):

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

### STEP 2.2: WfLibraryReflection.cpp
1) Add `IMPL_TYPE_INFO_RENAME(...)` lines near the existing `att_cpp_*` renames:

    IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_rpc_Interface, system::workflow_attributes::att_rpc_Interface)
    IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_rpc_Ctor, system::workflow_attributes::att_rpc_Ctor)
    IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_rpc_Byval, system::workflow_attributes::att_rpc_Byval)
    IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_rpc_Byref, system::workflow_attributes::att_rpc_Byref)
    IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_rpc_Cached, system::workflow_attributes::att_rpc_Cached)
    IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_rpc_Dynamic, system::workflow_attributes::att_rpc_Dynamic)

2) Add reflection member blocks in the metadata section (empty structs, so no `STRUCT_MEMBER` lines):

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

## STEP 3: Add a new analyzer pass source file and include it in the compiler shared-items project
Create:
- `REPO-ROOT\Source\Analyzer\WfAnalyzer_ValidateRPC.cpp`

Add it to:
- `REPO-ROOT\Test\UnitTest\VlppWorkflow_Compiler\VlppWorkflow_Compiler.vcxitems`
  - Add a `ClCompile Include="..\..\..\Source\Analyzer\WfAnalyzer_ValidateRPC.cpp"` entry.
- `REPO-ROOT\Test\UnitTest\VlppWorkflow_Compiler\VlppWorkflow_Compiler.vcxitems.filters`
  - Add a new filter folder `Analyzer\Pass 06`.
  - Put `WfAnalyzer_ValidateRPC.cpp` under that filter.

## STEP 4: Declare the public entry point and the new pass section in WfAnalyzer.h
Modify `REPO-ROOT\Source\Analyzer\WfAnalyzer.h`.

Immediately before the existing `Expanding Virtual Nodes` title comment, insert:

    /***********************************************************************
    RPC Analyzing
    ***********************************************************************/

    extern void ValidateModuleRPC(WfLexicalScopeManager* manager, Ptr<WfModule> module);

## STEP 5: Add RPC-related error declarations (H*) to WfErrors
Modify:
- `REPO-ROOT\Source\Analyzer\WfAnalyzer.h`
- `REPO-ROOT\Source\Analyzer\WfAnalyzer_Errors.cpp`

### STEP 5.1: Error code assignment (helper-template based)
Assign one `Hn` per helper-template:
- H0: `@rpc:Interface` can only apply to an interface definition, but not `FULL-NAME`.
- H1: `@rpc:Interface` cannot be applied to interface `FULL-NAME` because its base type `FULL-NAME` is not serializable.
- H2: `@rpc:Interface` cannot be applied to interface `FULL-NAME` because its member `MEMBER-NAME` is not serializable.
- H3: `@rpc:Ctor` can only apply to an interface definition with `@rpc:Interface`.
- H4: `@rpc:xxx` can only apply to a property, a method or a parameter. (`xxx` = `Byval` | `Byref`)
- H5: `@rpc:xxx` can only be used inside an interface type with `@rpc:Interface`. (`xxx` = `Byval` | `Byref` | `Cached` | `Dynamic`)
- H6: `@rpc:xxx` cannot be used on member `MEMBER-NAME` because it does not have a strong typed collection types. (`xxx` = `Byval` | `Byref`)
- H7: `@rpc:xxx` can only apply to a property. (`xxx` = `Cached` | `Dynamic`)
- H8: `@rpc:xxx` cannot be used on member `MEMBER-NAME` because it already has `@rpc:yyy`. (`xxx`,`yyy` = (`Byref`,`Byval`) or (`Dynamic`,`Cached`))

### STEP 5.2: WfErrors declarations to add
In `struct WfErrors` (append near the end; follow existing alignment conventions), add:

    // H: RPC attribute checking
    static glr::ParsingError			RpcInterfaceCanOnlyApplyToInterface(WfAttribute* node, const WString& fullName);																			// H0
    static glr::ParsingError			RpcInterfaceBaseNotSerializable(WfType* node, const WString& interfaceFullName, const WString& baseFullName);											// H1
    static glr::ParsingError			RpcInterfaceMemberNotSerializable(WfDeclaration* node, const WString& interfaceFullName, const WString& memberName);													// H2
    static glr::ParsingError			RpcInterfaceMemberNotSerializable(WfFunctionArgument* node, const WString& interfaceFullName, const WString& memberName);											// H2

    static glr::ParsingError			RpcCtorCanOnlyApplyToRpcInterface(WfAttribute* node);																															// H3

    static glr::ParsingError			RpcAttributeCanOnlyApplyToPropertyMethodOrParameter(WfAttribute* node, const WString& attributeName);																	// H4
    static glr::ParsingError			RpcAttributeCanOnlyBeUsedInsideRpcInterface(WfAttribute* node, const WString& attributeName);																// H5
    static glr::ParsingError			RpcAttributeRequiresStrongTypedCollection(WfAttribute* node, const WString& attributeName, const WString& memberName);												// H6
    static glr::ParsingError			RpcAttributeCanOnlyApplyToProperty(WfAttribute* node, const WString& attributeName);																// H7
    static glr::ParsingError			RpcAttributeCannotCoexistWithOther(WfAttribute* node, const WString& attributeName, const WString& otherAttributeName, const WString& memberName);				// H8

Implement these functions in `WfAnalyzer_Errors.cpp` using the existing `WfErrors` pattern, ensuring the `Hn:` prefix in messages is stable.

Anchoring:
- H0/H3/H4/H5/H6/H7/H8: anchor to `WfAttribute*`.
- H1: anchor to the base-type AST node (`WfType*`).
- H2: anchor to the failing member declaration (`WfDeclaration*`) or failing parameter node (`WfFunctionArgument*`).

## STEP 6: Implement ValidateModuleRPC in a two-phase design (AST phase + reflection phase)
Implement in `REPO-ROOT\Source\Analyzer\WfAnalyzer_ValidateRPC.cpp`.

Start file with (skeleton):

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

### STEP 6.1: Phase 1 (AST) helper: detect generic base types
Add helper:

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

### STEP 6.2: Phase 2 helpers (reflection-based predicates)
Implement helpers matching the following shapes:

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

    static bool IsStrongTypedCollection(ITypeInfo* type)
    {
    	if (!type || type->GetDecorator() != ITypeInfo::SharedPtr) return false;
    	switch (type->GetHint())
    	{
    	case TypeInfoHint::Array:
    	case TypeInfoHint::List:
    	case TypeInfoHint::Dictionary:
    		break;
    	default:
    		return false;
    	}

    	auto e = type->GetElementType();
    	if (!e || e->GetDecorator() != ITypeInfo::Generic) return false;

    	switch (type->GetHint())
    	{
    	case TypeInfoHint::Array:
    	case TypeInfoHint::List:
    		return e->GetGenericArgumentCount() == 1;
    	case TypeInfoHint::Dictionary:
    		return e->GetGenericArgumentCount() == 2;
    	default:
    		return false;
    	}
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

Also implement a serializable-return predicate that additionally accepts `void` and `Async` return types per `TODO_RPC_Definition.md` (return-type rule only; parameters still use `IsSerializableType`):

    static bool IsAsyncReturnType(ITypeInfo* type)
    {
    	if (!type || type->GetDecorator() != ITypeInfo::SharedPtr) return false;
    	auto e = type->GetElementType();
    	if (!e || e->GetDecorator() != ITypeInfo::TypeDescriptor) return false;

    	auto td = e->GetTypeDescriptor();
    	auto asyncTd = GetTypeDescriptor<IAsync>();		// renamed to system::Async in Workflow
    	return td && td->CanConvertTo(asyncTd);
    }

    static bool IsSerializableReturnType(ITypeInfo* type, ITypeDescriptor* rpcInterfaceAttrTd, const collections::SortedList<ITypeDescriptor*>& rpcInterfaceTds)
    {
    	if (!type) return false;

    	// allow "void" return type
    	if (type->GetDecorator() == ITypeInfo::TypeDescriptor && type->GetTypeDescriptor() == GetTypeDescriptor<VoidValue>()) return true;

    	// allow Async^ / Async-derived return types
    	if (IsAsyncReturnType(type)) return true;

    	return IsSerializableType(type, rpcInterfaceAttrTd, rpcInterfaceTds);
    }

### STEP 6.3: Phase 1 and Phase 2 responsibilities
Implement as described in `Copilot_Task.md` and `TODO_RPC_Definition.md`, but make the Phase 1 → Phase 2 handoff explicit so Phase 2 can report correct anchors and cover all H6 targets.

Phase 1 (AST, errors marked `(AST)` only):
- Traverse the module AST and validate attribute placement / coexistence rules:
  - H0 / H3 / H4 / H5(Byval/Byref) / H7 / H8.
- Collect workflow-defined RPC interfaces (for Phase 2):
  - For each interface declaration that has `@rpc:Interface` and passes H0, resolve and record its `ITypeDescriptor*` (workflow-defined type descriptors must be discoverable from the manager/type system at this point).
  - Record an H1 anchor map for each such interface: for every `WfType*` in its `baseTypes` list, resolve the corresponding base `ITypeDescriptor*` and store `baseTd -> baseTypeNode`.
    - Phase 2 must use this map so H1 is anchored to the correct `WfType*` in source.
- Collect attribute occurrences that require Phase 2 reflection checks (do not emit reflection-phase errors here):
  - `@rpc:Cached/@rpc:Dynamic` on properties: record the `WfAttribute*` + owning type so Phase 2 can enforce the non-AST inside-interface rule.
  - `@rpc:Byval/@rpc:Byref` that passed AST checks: record the `WfAttribute*` + the target kind (property / method return / parameter) so Phase 2 can enforce H6 using resolved `ITypeInfo*`.

Recommended Phase 1 output structures:
- `collections::SortedList<ITypeDescriptor*> workflowRpcInterfaceTds;` (workflow-defined interfaces with `@rpc:Interface`)
- `collections::Dictionary<ITypeDescriptor*, collections::Dictionary<ITypeDescriptor*, WfType*>> baseTypeNodeMap;` (per-interface: baseTd -> baseType WfType*)
- A list of deferred checks for Phase 2 that retains the `WfAttribute*` anchor.

Phase 2 (reflection: `ITypeDescriptor` / `ITypeInfo`):
- Precompute attribute type descriptors (e.g., `att_rpc_Interface`, `att_rpc_Byval`, `att_rpc_Byref`, `att_rpc_Cached`, `att_rpc_Dynamic`) once.
- H1 (base interfaces):
  - For each workflow-defined RPC interface `td` in `workflowRpcInterfaceTds`, iterate `td->GetBaseTypeDescriptor(index)`.
  - For each base type descriptor `baseTd`, if `!IsRpcInterfaceTd(baseTd, rpcInterfaceAttrTd, workflowRpcInterfaceTds)` then report H1.
  - Use `baseTypeNodeMap[td][baseTd]` to anchor the error to the exact `WfType*` node from the interface’s `baseTypes` list.
- H2 (member serializability) and H6 (strong typed collection requirement for Byval/Byref):
  - Enumerate members in a stable way using reflection:
    - Properties: `td->GetProperty(i)`; ignore inherited properties by checking `property->GetOwnerTypeDescriptor() == td`.
      - H2: check `IsSerializableType(property->GetReturn(), ...)`.
      - H6: if the property has `@rpc:Byval` or `@rpc:Byref`, enforce `IsStrongTypedCollection(property->GetReturn())`.
    - Methods: for each `td->GetMethodGroup(i)` (ignore inherited groups by owner check), then for each `group->GetMethod(j)`:
      - Parameters must satisfy `IsSerializableType(paramType, ...)`.
      - Return type must satisfy `IsSerializableReturnType(returnType, ...)` (explicitly allow `void` and `Async` per spec).
      - H2 triggers when any parameter type or the return type fails the above.
      - H6: if a parameter has `@rpc:Byval/@rpc:Byref`, enforce `IsStrongTypedCollection(paramType)`; if a method has `@rpc:Byval/@rpc:Byref`, enforce `IsStrongTypedCollection(returnType)`.
- H5 for `@rpc:Cached/@rpc:Dynamic` (non-AST inside-interface rule):
  - For each deferred cached/dynamic occurrence from Phase 1, determine whether the owning type is an RPC interface via `IsRpcInterfaceTd(ownerTd, ...)`.
  - If not, report H5 anchored to the recorded `WfAttribute*`.

Pre-existing interfaces are assumed correct; do not validate their members.

## STEP 7: Wire ValidateModuleRPC into the analyzer pipeline
Modify `REPO-ROOT\Source\Analyzer\WfAnalyzer.cpp`, in `WfLexicalScopeManager::Rebuild`.

After the existing semantic-validation loop, insert (before `#undef EXIT_IF_ERRORS_EXIST`):

    EXIT_IF_ERRORS_EXIST;
    for (auto module : modules)
    {
    	ValidateModuleRPC(this, module);
    }

## STEP 8: Add analyzer-error tests for RPC checker (compile errors)
Update:
- `REPO-ROOT\Test\Resources\IndexAnalyzerError.txt` (append new basenames, no extension)
- Add new files under `REPO-ROOT\Test\Resources\AnalyzerError\`.

Add the following new test files (each verifies the first generated error only; ensure each file triggers its intended `Hn` error first):

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

H2_RpcInterface_MemberNotSerializable_Property.txt

    module test;

    @rpc:Interface
    interface IMy
    {
    	// raw pointer to interface is not serializable (must be ^)
    	prop P : IMy* {}
    }

H2_RpcInterface_MemberNotSerializable_Return.txt

    module test;

    @rpc:Interface
    interface IMy
    {
    	// raw pointer to interface is not serializable (must be ^)
    	func M() : IMy*;
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

H6_RpcByval_NotStrongCollection_Property.txt

    module test;

    @rpc:Interface
    interface IMy
    {
    	@rpc:Byval
    	prop P : int {}
    }

H6_RpcByref_NotStrongCollection_Property.txt

    module test;

    @rpc:Interface
    interface IMy
    {
    	@rpc:Byref
    	prop P : int {}
    }

H6_RpcByval_NotStrongCollection_Parameter.txt

    module test;

    @rpc:Interface
    interface IMy
    {
    	func M(@rpc:Byval x : int) : int[];
    }

H6_RpcByref_NotStrongCollection_Parameter.txt

    module test;

    @rpc:Interface
    interface IMy
    {
    	func M(@rpc:Byref x : int) : int[];
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

## STEP 9: Positive / non-error sanity checks (recommended)
Add 4 new Codegen samples under `REPO-ROOT\Test\Resources\Codegen\` and append to `REPO-ROOT\Test\Resources\IndexCodegen.txt` with expected output `OK`:

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

  RpcVoidReturn_Positive=OK

      module test;
      using system::*;

      @rpc:Interface
      interface IRpc
      {
      	func Ping() : void;
      }

      func main() : string
      {
      	return "OK";
      }

  RpcAsyncReturn_Positive=OK

      module test;
      using system::*;

      @rpc:Interface
      interface IRpc
      {
      	func GetAsync() : Async^;
      }

      func main() : string
      {
      	return "OK";
      }

## STEP 10: Verification
Run (initial verification per task note):
- `CompilerTest_GenerateMetadata` (`Debug|Win32` and `Debug|x64`).
  - If reflection metadata baselines change, update `REPO-ROOT\Test\Resources\Baseline\Reflection{32,64}.txt` from `REPO-ROOT\Test\UnitTest\Generated\Reflection{32,64}.txt`.
- `CompilerTest_LoadAndCompile` (`Debug|x64`).

Sanity check while running tests:
- Ensure `H0_RpcInterface_OnGenericInterface.txt` reliably reports H0 first (i.e., no earlier parse/type-resolution error from `$interface` or `Async<string>`).

After the above pass and the RPC checker is stable, run the remaining unit test projects per `REPO-ROOT\Project.md`.

# FIXING ATTEMPTS

# !!!FINISHED!!!

