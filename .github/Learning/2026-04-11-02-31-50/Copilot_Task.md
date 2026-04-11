# !!!TASK!!!

# PROBLEM DESCRIPTION
Recently VlppReflection project updated a new feature, to register attributes to reflections:
- ATTRIBUTE_TYPE
- ATTRIBUTE_MEMBER
- ATTRIBUTE_PARAMETER
You should read the knowledge base to understand them.

Your goal is to change the fact that Workflow hardcoded supported attributes in WfLexicalScopeManager::WfLexicalScopeManager. You will notice in this constructor there is a list of attributes, each either have one argument or zero argument. `void` means zero argument, other type means one argument. When declaring these attributes into a struct, the only argument could be represented as a field called "argument" if exists.

Task 1, you need to declare all these attributes into the WfLibraryPredefined.h, with there reflections in WfLibraryReflection.h|cpp. All attribute structs should be put into the namespaces vl::__vwsn.

If an attribute called @cpp:Private, it means:
in C++ its full type name is vl::__vwsn::att_cpp_Private.
in Workflow(reflected type) its full type name is system::workflow_attributes::att_cpp_Private.

To test this, you should run the CompilerTest_GenerateMetadata unit test projects. Even when you implemented it correctly, it is going to fail because new types are added causing the dumped metadata in text format changed comparing to baseline. You can leverage this chance to git diff the file and see if all you added are there, and replace the baseline file with the generated file to make it pass.

Task 2, you need to change the workflow compiler to remove the hardcoded attribute list. Now all needed attributes are added to the predefined library therefore no hardcoded list need to exist. It means you need to completely remove the list itself. This will cause many code to break and it is expected.

Here are proposals to fix these breaks:

When an attribute is parsed, before the change the compiler read the hardcoded attribute list, after the change you should follow the coding convention to read the reflected type to know the argument type and see if it exists. You need to create a function to convert the attribute name to the system::workflow_attributes::att_X_Y string so that everywhere could use this function to find the attribute type.

The workflow script allow zero or one argument on an attribute, that is fine, when an attribute has multiple arguments (aka multiple fields in the attribute type struct), use the first one, which should be its only field in its ITypeDescriptor.

Predefined @cpp:X attributes are used in C++ code generation from workflow script, the current code read attributes from AST, this is OK, just keep it.

Task 3, fix generated C++ reflection code from Workflow. Now you will need too add ATTRIBUTE_* macros in the generated C++ code. All types (except non-member functions/variables) will generated C++ code, all attributes attached to them needs to be in their generated reflection registration.

Follow Project.md to run **ALL** test projects in their required configuration.

# UPDATES

# INSIGHTS AND REASONING

## Background: VlppReflection attribute registration
The new feature is implemented as reflection registration-time macros:
- `ATTRIBUTE_TYPE(TYPE, ...)` attaches an attribute to the enclosing `ITypeDescriptor`.
- `ATTRIBUTE_MEMBER(TYPE, ...)` attaches an attribute to the most recently registered member.
- `ATTRIBUTE_PARAMETER(PARAMETER_NAME, TYPE, ...)` attaches an attribute to a named parameter of the most recently registered method/constructor.

Evidence:
- Knowledge base: `.github\KnowledgeBase\KB_VlppReflection_AttributeRegistration.md`.
- Macro definitions: `Import\VlppReflection.h` (`ATTRIBUTE_TYPE/ATTRIBUTE_MEMBER/ATTRIBUTE_PARAMETER`).

Key constraint from VlppReflection:
- Each attribute argument is boxed and must have a non-null `GetSerializableType()`.
- This is enforced by `CHECK_ERROR(valueType && valueType->GetSerializableType(), L"ATTRIBUTE_*#Attribute argument must be a serializable reflected value.")` in `Import\VlppReflection.h`.

This impacts any attribute whose logical argument is an `ITypeDescriptor*` (not serializable in this repository, and files under `Import\` are not allowed to be modified).

## Current Workflow state (before changes)
Workflow currently hardcodes supported attributes in `WfLexicalScopeManager::WfLexicalScopeManager`:
- `Source\Analyzer\WfAnalyzer.cpp` contains:
  - `@cpp:File` expects `WString`
  - `@cpp:UserImpl` expects `void`
  - `@cpp:Private` expects `void`
  - `@cpp:Protected` expects `void`
  - `@cpp:Friend` expects `ITypeDescriptor*`

The hardcoded map is used by:
- Semantic validation: `Source\Analyzer\WfAnalyzer_ValidateSemantic_Declaration.cpp` looks up `(category,name)` in `manager->attributes` and validates the optional argument against the expected `ITypeInfo`.
- Constant evaluation for attribute values: `Source\Emitter\WfEmitter.cpp` (`WfAttributeEvaluator::GetAttributeValue`) uses `manager->attributes[{category,name}]` as the expected type when generating expression instructions.

C++ code generation behavior of these attributes is documented in:
- `.github\KnowledgeBase\manual\workflow\codegen.md`.

## Design goals
1. Define attribute structs in the predefined library so attribute types are discoverable via reflection (`system::workflow_attributes::att_*`).
2. Remove the hardcoded `(category,name)->ITypeInfo` list and resolve attribute existence/argument type from reflection at compile time.
3. Ensure Workflow-generated C++ reflection registration emits `ATTRIBUTE_*` macros so attributes appear in reflection metadata for generated types.

## Task 1 design: declare and reflect attribute structs
### What to add
- Add one struct per supported attribute in `Source\Library\WfLibraryPredefined.h`.
  - All structs live in `namespace vl::__vwsn`.
  - Naming convention: `att_<category>_<Name>`.
  - If the attribute accepts an argument, expose a single field named `argument`.

Concrete examples for the currently hardcoded list:
- `vl::__vwsn::att_cpp_File` with `WString argument;`
- `vl::__vwsn::att_cpp_UserImpl` with no fields
- `vl::__vwsn::att_cpp_Private` with no fields
- `vl::__vwsn::att_cpp_Protected` with no fields
- `vl::__vwsn::att_cpp_Friend` with `WString argument;` (see serializability discussion below)

### Attribute struct requirements (hard requirements)
These attribute structs exist to be consumed by `ATTRIBUTE_*` and by the Workflow compiler’s reflection-based resolver, so they must follow the VlppReflection rules:
- Every attribute type is a reflected **struct** (`BEGIN_STRUCT_MEMBER` / `END_STRUCT_MEMBER`).
- Must remain **aggregate-initializable** (no user-defined constructors), because `ATTRIBUTE_*` uses `TYPE{...}`.
- Must define `auto operator<=>(const T&) const = default;` (including empty structs), consistent with `KB_VlppReflection_AttributeRegistration.md` examples.
- Workflow attributes are “0 or 1 argument”; therefore, attribute structs used here must have **0 or 1 field**.
  - If 1 field exists, it should be named `argument`.
  - If a struct has >1 field, the compiler should diagnose this deterministically (do not silently pick an arbitrary field).

### Reflection registration and Workflow type names
- Register these structs in `Source\Library\WfLibraryReflection.h/.cpp`.
- Rename each reflected type name so Workflow sees:
  - `system::workflow_attributes::att_cpp_Private`

Reflection wiring checklist (avoid “declared but not registered” gaps):
- Add `DECL_TYPE_INFO(vl::__vwsn::att_...)` declarations for each new attribute type.
- Add the new types into `WORKFLOW_LIBRARY_TYPES(F)` in `Source\Library\WfLibraryReflection.h` so both declaration and loader registration stay consistent.
- In `Source\Library\WfLibraryReflection.cpp`:
  - Use `IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_..., system::workflow_attributes::att_...)`.
  - Add a `BEGIN_STRUCT_MEMBER(att_...)` block (and `STRUCT_MEMBER(argument)` when applicable).

### Serializability and @cpp:Friend (must be string-based)
There is a conflict between:
- Workflow manual: `@cpp:Friend(typeof(YourType))` (`.github\KnowledgeBase\manual\workflow\codegen.md`).
- Attribute macro constraint: attribute arguments must be serializable (`KB_VlppReflection_AttributeRegistration.md` + `Import\VlppReflection.h`).

`ITypeDescriptor*` is not serializable in this repository (and we must not modify `Import\`). Because `ATTRIBUTE_*` enforces `GetSerializableType()!=nullptr` for every boxed argument, the attribute struct field type cannot be `ITypeDescriptor*`.

Decision (no fallback):
- Define `vl::__vwsn::att_cpp_Friend` as `WString argument;` and treat the argument as a stable C++ type name string.

Required conversion pipeline for `@cpp:Friend(typeof(T))`:
- Semantic validation must accept `typeof(T)` as a valid attribute argument even though the reflected attribute field type is `WString`.
- Constant evaluation must convert the constant `ITypeDescriptor*` from `typeof(T)` into a `WString` (e.g., via `vl::reflection::description::CppGetFullName(ITypeDescriptor*)`).
- Both C++ friend emission and generated reflection code must consume the string form so `ATTRIBUTE_* (att_cpp_Friend, L"...")` is valid and serializable.

## Task 2 design: remove hardcoded attribute list and resolve from reflection
### Remove the constructor list
- Delete the `attributes.Add(...)` calls in `WfLexicalScopeManager::WfLexicalScopeManager` (`Source\Analyzer\WfAnalyzer.cpp`).
- Replace the `AttributeTypeMap attributes;` field (in `Source\Analyzer\WfAnalyzer.h`) with a reflection-driven resolver + cache.

### Central resolver API
Add a central function on the manager (or a small helper in analyzer/emitter shared code) to:
1. Convert `(category,name)` to the reflected attribute type name:
   - `system::workflow_attributes::att_<category>_<name>`
   - To avoid lossy mapping and silent collisions, initially require `<category>` and `<name>` to be identifier-compatible (`[A-Za-z0-9_]` only). If not, treat it as “not exists” and emit the same diagnostic as an unknown attribute.
2. Resolve the attribute `ITypeDescriptor*` via `vl::reflection::description::GetTypeDescriptor(typeName)`.
3. Determine if an argument exists and its expected type from the attribute struct’s properties:
   - 0 properties → “no argument”.
   - 1 property → “one argument”, and its type is the expected argument type.
   - >1 properties → **invalid attribute type for Workflow**; diagnose deterministically (do not silently pick the first property).

Cache:
- Use a concrete cache (e.g., `collections::Dictionary<WString, ResolvedAttributeInfo>` keyed by the full reflected type name).
- Cache both “found” and “not found”, and (when found) cache the resolved expected-argument `ITypeInfo` / `ITypeDescriptor` so semantic validation and constant evaluation do not repeatedly scan reflection metadata.

### Update semantic validation
- Update `Source\Analyzer\WfAnalyzer_ValidateSemantic_Declaration.cpp` to call the resolver instead of indexing `manager->attributes`.
- Preserve current error behavior:
  - If the attribute type cannot be resolved → emit `AttributeNotExists`.
  - If an argument is missing/present incorrectly → keep existing argument-count validation logic.

### Update attribute constant evaluation
- Update `Source\Emitter\WfEmitter.cpp` (`WfAttributeEvaluator::GetAttributeValue`) to get the expected argument type from the resolver (instead of `manager->attributes[...]`).

### (Follow-up) Attach attributes to Workflow runtime reflection (types/members/parameters)
This is **not required** to accomplish Tasks 1–3 as currently scoped (declare attribute types, remove hardcoded expected-arg map, and emit `ATTRIBUTE_*` in generated C++ reflection). Keeping it in-scope adds risk and introduces a second “source of truth”.

Decision:
- De-scope this from the current task set. Existing `@cpp:*` code generation continues to read attributes from the Workflow AST as it does today.

If/when implemented later, it must:
- Use `vl::reflection::description::AttributeInfoImpl` + `AttributeBagSource::RegisterAttribute`.
- Only register serializable argument values (same `GetSerializableType()!=nullptr` constraint as `ATTRIBUTE_*`).

## Task 3 design: emit ATTRIBUTE_* macros in Workflow-generated C++ reflection code
### Where to change
- Update `Source\Cpp\WfCpp_WriteReflection.cpp` to output attribute registration macros in generated C++ code.

### What to emit
For generated reflected items:
- Type-level attributes: emit `ATTRIBUTE_TYPE(::vl::__vwsn::att_<cat>_<name>, ...)` inside the generated `BEGIN_*_MEMBER` / `END_*_MEMBER` block for that type.
- Member-level attributes: immediately after the member registration macro it annotates, emit `ATTRIBUTE_MEMBER(...)` for attributes on that member.
- Parameter-level attributes: immediately after the method/constructor registration macro, emit `ATTRIBUTE_PARAMETER(L"param", ...)` for each attribute on that parameter.

Hard requirements from `KB_VlppReflection_AttributeRegistration.md` (ordering + matching):
- `ATTRIBUTE_MEMBER` must immediately follow the member registration it annotates.
- `ATTRIBUTE_PARAMETER` must follow a method/constructor registration, and the parameter name must match exactly.

### Emitting attribute arguments
- For attributes with no argument in the Workflow AST: emit `ATTRIBUTE_*` with no extra arguments.
- For attributes with an argument:
  - Evaluate/serialize the constant expression into a C++ literal expression matching the attribute field type.
  - Only emit arguments that are serializable reflected primitives (per the VlppReflection constraint).

Special handling:
- If `@cpp:Friend` is implemented as `WString` (Task 1 preferred), then `@cpp:Friend(typeof(T))` should be converted to a `WString` constant so `ATTRIBUTE_TYPE(att_cpp_Friend, L"...")` is valid and serializable.

## Risks / edge cases
- Attribute name to type name mapping must be deterministic and stable, otherwise metadata baselines will churn.
- Any attribute argument type not serializable will cause a hard failure at reflection-registration time (for generated C++ code) due to the `CHECK_ERROR` in `Import\VlppReflection.h`.
- Introducing new reflected types (Task 1) will change `CompilerTest_GenerateMetadata` outputs; baseline updates are expected per `Project.md`.
- Generated reflection `.cpp` that emits `ATTRIBUTE_* (::vl::__vwsn::att_...)` must include the header that declares these structs (directly or transitively).

## Verification (beyond pass/fail)
In addition to running the listed unit tests and updating baselines when expected, explicitly verify:
- Metadata text output contains the expected `@Attribute:` / `@ParamAttribute:` lines for newly registered attributes (format per `KB_VlppReflection_AttributeRegistration.md`).
- Workflow-generated C++ reflection output contains the expected `ATTRIBUTE_TYPE/ATTRIBUTE_MEMBER/ATTRIBUTE_PARAMETER` calls in the correct positions.

# AFFECTED PROJECTS
- Build the solution in folder `REPO-ROOT\Test\UnitTest` (`Debug|x64`).
- Always Run UnitTest project `LibraryTest` (`Debug|x64`).
- Always Run UnitTest project `CompilerTest_GenerateMetadata` (`Debug|x64`) because `REPO-ROOT\Source\Library` and compiler code will change.
  - If baseline `.txt` changes are expected, update `REPO-ROOT\Test\Resources\Baseline\Reflection64.txt` from `REPO-ROOT\Test\UnitTest\Generated\Reflection64.txt` and rerun.
- Always Run UnitTest project `CompilerTest_LoadAndCompile` (`Debug|x64`).
- Always Run UnitTest project `CppTest` (`Debug|x64`).
- Always Run UnitTest project `CppTest_Metaonly` (`Debug|x64`).
- Always Run UnitTest project `CppTest_Reflection` (`Debug|x64`).

- Build the solution in folder `REPO-ROOT\Test\UnitTest` (`Debug|Win32`).
- Always Run UnitTest project `LibraryTest` (`Debug|Win32`).
- Always Run UnitTest project `CompilerTest_GenerateMetadata` (`Debug|Win32`).
  - If baseline `.txt` changes are expected, update `REPO-ROOT\Test\Resources\Baseline\Reflection32.txt` from `REPO-ROOT\Test\UnitTest\Generated\Reflection32.txt` and rerun.
- Always Run UnitTest project `CppTest` (`Debug|Win32`).
- Always Run UnitTest project `CppTest_Metaonly` (`Debug|Win32`).
- Always Run UnitTest project `CppTest_Reflection` (`Debug|Win32`).

# !!!FINISHED!!!
