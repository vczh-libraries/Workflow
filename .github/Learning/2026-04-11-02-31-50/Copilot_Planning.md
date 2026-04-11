# !!!PLANNING!!!

# UPDATES

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

# EXECUTION PLAN

## STEP 1: Add reflected attribute structs (Task 1)

### What to change
Add one struct per supported Workflow attribute to `REPO-ROOT\Source\Library\WfLibraryPredefined.h` under `namespace vl::__vwsn`.

Current hardcoded attribute list (to be removed in Task 2) contains:
- `@cpp:File` (one argument: `WString`)
- `@cpp:UserImpl` (no argument)
- `@cpp:Private` (no argument)
- `@cpp:Protected` (no argument)
- `@cpp:Friend` (currently treated as `ITypeDescriptor*`, but must become serializable for `ATTRIBUTE_*`)

### Why this is necessary
Task 2 requires the compiler to discover supported attributes from reflection instead of a hardcoded list.
Task 3 requires generated C++ reflection registration code to reference concrete attribute types (structs) in `ATTRIBUTE_TYPE/ATTRIBUTE_MEMBER/ATTRIBUTE_PARAMETER` macros.

### Code to add (example)
Append near the end of `WfLibraryPredefined.h` (exact placement can follow existing grouping conventions):

```
namespace vl
{
	namespace __vwsn
	{
		struct att_cpp_File
		{
			WString						argument;
			auto operator<=>(const att_cpp_File&) const = default;
		};

		struct att_cpp_UserImpl
		{
			auto operator<=>(const att_cpp_UserImpl&) const = default;
		};

		struct att_cpp_Private
		{
			auto operator<=>(const att_cpp_Private&) const = default;
		};

		struct att_cpp_Protected
		{
			auto operator<=>(const att_cpp_Protected&) const = default;
		};

		// NOTE:
		// - Must be serializable for ATTRIBUTE_* (cannot be ITypeDescriptor*).
		// - Store the reflected type name (ITypeDescriptor::GetTypeName()) for round-tripping.
		struct att_cpp_Friend
		{
			WString						argument;
			auto operator<=>(const att_cpp_Friend&) const = default;
		};
	}
}
```

Hard requirements to keep in mind:
- Must remain aggregate-initializable (no user-defined constructors).
- Must be `struct` (for `BEGIN_STRUCT_MEMBER`).
- If one argument exists, name it `argument`.

## STEP 2: Register and rename attribute types in Workflow library reflection (Task 1)

### What to change
Update `REPO-ROOT\Source\Library\WfLibraryReflection.h` and `REPO-ROOT\Source\Library\WfLibraryReflection.cpp`:
1. Add `DECL_TYPE_INFO` for the new structs.
2. Ensure `WfLoadLibraryTypes()` registers them using `BEGIN_STRUCT_MEMBER`.
3. Rename reflected type names to Workflow-visible names:
   - C++: `vl::__vwsn::att_cpp_Private`
   - Workflow/reflection name: `system::workflow_attributes::att_cpp_Private`

### Why this is necessary
Task 2 resolves attribute existence and argument type by calling `GetTypeDescriptor(L"system::workflow_attributes::att_...")`.
Without registration + renaming, attributes cannot be discovered from reflection.

### Code to add
In `WfLibraryReflection.h`, extend the type list to include attributes. To keep the file tidy, create a sub-list macro:

```
#ifndef VCZH_DEBUG_NO_REFLECTION

#define WORKFLOW_LIBRARY_ATTRIBUTE_TYPES(F)\
		F(vl::__vwsn::att_cpp_File)\
		F(vl::__vwsn::att_cpp_UserImpl)\
		F(vl::__vwsn::att_cpp_Private)\
		F(vl::__vwsn::att_cpp_Protected)\
		F(vl::__vwsn::att_cpp_Friend)\

#define WORKFLOW_LIBRARY_TYPES(F)\
		F(Sys)\
		...\
		F(Versioning)\
		WORKFLOW_LIBRARY_ATTRIBUTE_TYPES(F)\

		WORKFLOW_LIBRARY_TYPES(DECL_TYPE_INFO)

#endif
```

In `WfLibraryReflection.cpp`, add rename macros near the existing `IMPL_TYPE_INFO_RENAME(...)` block:

```
#ifndef VCZH_DEBUG_NO_REFLECTION

IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_cpp_File, system::workflow_attributes::att_cpp_File)
IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_cpp_UserImpl, system::workflow_attributes::att_cpp_UserImpl)
IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_cpp_Private, system::workflow_attributes::att_cpp_Private)
IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_cpp_Protected, system::workflow_attributes::att_cpp_Protected)
IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_cpp_Friend, system::workflow_attributes::att_cpp_Friend)

#endif
```

And register structs in `WfLoadLibraryTypes()` inside `#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA`:

```
BEGIN_STRUCT_MEMBER(vl::__vwsn::att_cpp_File)
	STRUCT_MEMBER(argument)
END_STRUCT_MEMBER(vl::__vwsn::att_cpp_File)

BEGIN_STRUCT_MEMBER(vl::__vwsn::att_cpp_UserImpl)
END_STRUCT_MEMBER(vl::__vwsn::att_cpp_UserImpl)

BEGIN_STRUCT_MEMBER(vl::__vwsn::att_cpp_Private)
END_STRUCT_MEMBER(vl::__vwsn::att_cpp_Private)

BEGIN_STRUCT_MEMBER(vl::__vwsn::att_cpp_Protected)
END_STRUCT_MEMBER(vl::__vwsn::att_cpp_Protected)

BEGIN_STRUCT_MEMBER(vl::__vwsn::att_cpp_Friend)
	STRUCT_MEMBER(argument)
END_STRUCT_MEMBER(vl::__vwsn::att_cpp_Friend)
```

## STEP 3: Introduce a reflection-driven attribute resolver (Task 2)

### What to change
Remove the hardcoded list from `WfLexicalScopeManager::WfLexicalScopeManager` in `REPO-ROOT\Source\Analyzer\WfAnalyzer.cpp` and replace the `attributes` field in `REPO-ROOT\Source\Analyzer\WfAnalyzer.h` with a resolver API + cache (keep `AttributeKey` as the cache key; only remove `AttributeTypeMap` and the `attributes` field).

### Why this is necessary
Hardcoding supported attributes prevents user-extended attributes and blocks Task 3 from being reflection-driven.
The compiler must be able to:
- Determine whether an attribute exists.
- Determine whether it accepts an argument.
- Determine the expected argument type (from the reflected struct property type).

### Resolver API and mapping rules
Workflow attribute `@<category>:<Name>` maps to reflected type name:
- `system::workflow_attributes::att_<category>_<Name>`

Implementation details:
- Require `category` and `Name` to be identifier-compatible (`[A-Za-z0-9_]` only) to avoid ambiguous mapping.
- Use `vl::reflection::description::GetTypeDescriptor(typeName)` to locate the type.
- Require the descriptor to be a reflected struct.
- Count properties:
  - 0 properties => no argument
  - 1 property => one argument, expected type is that property type
  - >1 properties => treat as **not a Workflow attribute type**. Cache `exists = false` so semantic validation consistently reports `AttributeNotExists` (do not leave a partially-initialized `exists = true`).

### Proposed code
In `WfAnalyzer.h`, keep `AttributeKey` (it remains the resolver cache key), remove `AttributeTypeMap` and the `attributes` field, and add a resolver cache struct + dictionary:

- Keep:
```
typedef collections::Pair<WString, WString> AttributeKey;
```

- Remove:
```
typedef collections::Dictionary<AttributeKey, Ptr<ITypeInfo>> AttributeTypeMap;
...
AttributeTypeMap attributes;
```

- Add:
```
struct ResolvedWorkflowAttribute
{
	bool							exists = false;
	bool							hasArgument = false;
	Ptr<ITypeInfo>				argumentType;
	ITypeDescriptor*			attributeType = nullptr;
};

protected:
	collections::Dictionary<AttributeKey, ResolvedWorkflowAttribute>	resolvedAttributes;

public:
	ResolvedWorkflowAttribute		ResolveWorkflowAttribute(const WString& category, const WString& name);
	WString								GetWorkflowAttributeTypeName(const WString& category, const WString& name);
```

Mandatory behavior notes (do not weaken diagnostics):
- `ResolveWorkflowAttribute` must be generic; do not embed attribute-specific coercions here.
- For `propCount == 0`, keep `hasArgument == false` and keep `argumentType == nullptr`.
- For `propCount > 1`, cache `exists == false` so callers reliably take the `AttributeNotExists` path.

In `WfAnalyzer.cpp`, implement:

```
WString WfLexicalScopeManager::GetWorkflowAttributeTypeName(const WString& category, const WString& name)
{
	auto IsId = [](const WString& s)
	{
		for (vint i = 0; i < s.Length(); i++)
		{
			auto c = s[i];
			if (!((c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z') || c == L'_'))
			{
				return false;
			}
		}
		return s.Length() > 0;
	};

	if (!IsId(category) || !IsId(name)) return L"";
	return L"system::workflow_attributes::att_" + category + L"_" + name;
}

WfLexicalScopeManager::ResolvedWorkflowAttribute WfLexicalScopeManager::ResolveWorkflowAttribute(const WString& category, const WString& name)
{
	ResolvedWorkflowAttribute info;
	auto key = AttributeKey(category, name);
	if (auto index = resolvedAttributes.Keys().IndexOf(key); index != -1)
	{
		return resolvedAttributes.Values()[index];
	}

	auto typeName = GetWorkflowAttributeTypeName(category, name);
	if (typeName == L"")
	{
		resolvedAttributes.Add(key, info);
		return info;
	}

	auto td = description::GetTypeDescriptor(typeName);
	if (!td || td->GetTypeDescriptorFlags() != TypeDescriptorFlags::Struct)
	{
		resolvedAttributes.Add(key, info);
		return info;
	}

	auto propCount = td->GetPropertyCount();
	if (propCount == 0)
	{
		info.exists = true;
		info.attributeType = td;
		info.hasArgument = false;
		info.argumentType = nullptr;
	}
	else if (propCount == 1)
	{
		info.exists = true;
		info.attributeType = td;
		info.hasArgument = true;
		info.argumentType = CopyTypeInfo(td->GetProperty(0)->GetReturn());
	}
	else
	{
		// Not a valid Workflow attribute type (must be 0 or 1 property).
		// Treat as NOT EXISTS so semantic validation deterministically reports AttributeNotExists.
		info.exists = false;
		info.attributeType = nullptr;
		info.hasArgument = false;
		info.argumentType = nullptr;
	}

	resolvedAttributes.Add(key, info);
	return info;
}
```

Also remove the hardcoded `attributes.Add(...)` lines in `WfLexicalScopeManager` constructor.

## STEP 4: Update semantic validation to use the resolver (Task 2)

### What to change
In `REPO-ROOT\Source\Analyzer\WfAnalyzer_ValidateSemantic_Declaration.cpp`, replace `manager->attributes` lookups with `ResolveWorkflowAttribute`.

### Why this is necessary
Without it, the compiler will continue to accept only the removed hardcoded list and will crash or misdiagnose attributes.

### Proposed code change
Replace:

```
auto key = Pair<WString, WString>(attribute->category.value, attribute->name.value);
vint index = manager->attributes.Keys().IndexOf(key);
if (index == -1)
{
	manager->errors.Add(WfErrors::AttributeNotExists(attribute.Obj()));
}
else
{
	auto expectedType = manager->attributes.Values()[index];
	...
}
```

With:

```
auto info = manager->ResolveWorkflowAttribute(attribute->category.value, attribute->name.value);
if (!info.exists)
{
	manager->errors.Add(WfErrors::AttributeNotExists(attribute.Obj()));
	continue;
}

if (attribute->value)
{
	// Keep validation type-directed for most attributes.
	if (attribute->category.value == L"cpp" && attribute->name.value == L"Friend"
		&& info.hasArgument
		&& info.argumentType
		&& info.argumentType->GetTypeDescriptor() == description::GetTypeDescriptor<WString>())
	{
		// @cpp:Friend(typeof(T)) historically allowed; keep behavior + diagnostics.
		// Accept either a string literal (WString) or typeof(T) (ITypeDescriptor*) constant.
		ValidateConstantExpression(manager, attribute->value, nullptr);
		auto actualType = GetExpressionType(manager, attribute->value, nullptr);
		if (actualType)
		{
			auto actualTd = actualType->GetTypeDescriptor();
			if (actualTd != description::GetTypeDescriptor<WString>()
				&& actualTd != description::GetTypeDescriptor<ITypeDescriptor*>())
			{
				manager->errors.Add(WfErrors::ExpressionCannotImplicitlyConvertToType(attribute->value.Obj(), actualType.Obj(), info.argumentType.Obj()));
			}
		}
	}
	else
	{
		ValidateConstantExpression(manager, attribute->value, info.argumentType);
	}
}
else
{
	if (info.hasArgument)
	{
		manager->errors.Add(WfErrors::AttributeMissValue(attribute.Obj()));
	}
}
```

## STEP 5: Update attribute constant evaluation (Emitter) (Task 2)

### What to change
Update `REPO-ROOT\Source\Emitter\WfEmitter.cpp` (`WfAttributeEvaluator::GetAttributeValue`) to obtain the expected argument type from reflection (resolver), and to support the `@cpp:Friend(typeof(T)) -> WString` conversion.

### Why this is necessary
After removing the hardcoded map, `manager->attributes[{...}]` will no longer exist.
Also, Task 3 needs attribute argument values available as serializable constants when emitting `ATTRIBUTE_*`.

### Proposed code change
Replace:

```
auto typeInfo = manager->attributes[{att->category.value, att->name.value}];
GenerateExpressionInstructions(context, att->value, typeInfo);
```

With:

```
auto info = manager->ResolveWorkflowAttribute(att->category.value, att->name.value);
CHECK_ERROR(info.exists, L"WfAttributeEvaluator::GetAttributeValue(...)#Attribute not resolved.");

// Preserve existing type-directed behavior for most attributes.
// Only relax the target type for @cpp:Friend(typeof(T)) compatibility.
if (att->category.value == L"cpp" && att->name.value == L"Friend"
	&& info.hasArgument
	&& info.argumentType
	&& info.argumentType->GetTypeDescriptor() == description::GetTypeDescriptor<WString>())
{
	GenerateExpressionInstructions(context, att->value, nullptr);
}
else
{
	GenerateExpressionInstructions(context, att->value, info.argumentType);
}
...
auto value = ins.valueParameter;

// @cpp:Friend: serialize typeof(T) as reflection name so it round-trips via GetTypeDescriptor(string)
if (att->category.value == L"cpp" && att->name.value == L"Friend")
{
	if (value.type == runtime::WfInsType::Unknown && value.typeDescriptor != nullptr)
	{
		value = runtime::WfRuntimeValue(value.typeDescriptor->GetTypeName());
	}
	CHECK_ERROR(value.type == runtime::WfInsType::String, L"WfAttributeEvaluator::GetAttributeValue(...)#Unexpected value in attribute: @cpp:Friend.");
}

attributeValues.Add(att, value);
return value;
```

This keeps the compiler behavior stable while ensuring the produced value is serializable for `ATTRIBUTE_*`.

## STEP 6: Update C++ codegen for @cpp:Friend to match the new string representation

### What to change
Update `REPO-ROOT\Source\Cpp\WfCpp_WriteClass.cpp` where it currently expects `@cpp:Friend` to evaluate to `(Unknown, ITypeDescriptor*)`.

### Why this is necessary
After Step 5, `@cpp:Friend(typeof(T))` is converted to a string (the reflected type name), so existing `CHECK_ERROR(attValue.type == Unknown ...)` will fail.

### Proposed code change
Replace the current logic:

```
auto attValue = attributeEvaluator->GetAttributeValue(attribute);
CHECK_ERROR(attValue.type == runtime::WfInsType::Unknown && attValue.typeDescriptor != nullptr, ...);
auto td = attValue.typeDescriptor;
```

With:

```
auto attValue = attributeEvaluator->GetAttributeValue(attribute);
CHECK_ERROR(attValue.type == runtime::WfInsType::String, L"Unexpected value in attribute: @cpp.Friend.");
auto td = description::GetTypeDescriptor(attValue.stringValue);
CHECK_ERROR(td != nullptr, L"Unexpected value in attribute: @cpp.Friend.");
```

The rest of the friend generation logic (checking whether the type is in the current compilation by `manager->typeNames[td]`) stays unchanged.

## STEP 7: Emit ATTRIBUTE_* macros in generated C++ reflection code (Task 3)

### What to change
Update `REPO-ROOT\Source\Cpp\WfCpp_WriteReflection.cpp` to emit:
- `ATTRIBUTE_TYPE(...)` for attributes on reflected types.
- `ATTRIBUTE_MEMBER(...)` for attributes on reflected members.
- `ATTRIBUTE_PARAMETER(...)` for attributes on method/ctor parameters.

### Why this is necessary
Without emitting these macros, attributes are lost from the generated reflection metadata, and the new VlppReflection feature cannot observe them.
Ordering constraints (from KB) require:
- `ATTRIBUTE_MEMBER` immediately follows the member registration macro.
- `ATTRIBUTE_PARAMETER` follows the method/constructor registration macro, with exact parameter name match.

### Data source for attributes
Use AST declarations that produced the generated types:
- `WfCppConfig::tdDecls` maps `ITypeDescriptor*` -> `Ptr<WfDeclaration>`.
- `manager->declarationMemberInfos` maps `Ptr<WfDeclaration>` -> `Ptr<IMemberInfo>`.

Build a reverse map at the beginning of `WriteCpp_Reflection`:

```
Dictionary<IMemberInfo*, Ptr<WfDeclaration>> memberDecls;
for (auto [decl, index] : indexed(manager->declarationMemberInfos.Keys()))
{
	memberDecls.Add(manager->declarationMemberInfos.Values()[index].Obj(), decl);
}
```

### Emission helpers (proposed)
Add local helper functions in `WfCpp_WriteReflection.cpp`:

1) Resolve attribute type descriptor

```
auto ResolveAttributeTd = [&](Ptr<WfAttribute> att)
{
	auto info = config->manager->ResolveWorkflowAttribute(att->category.value, att->name.value);
	return info;
};
```

2) Write a single macro call for a resolved attribute

```
auto WriteAttributeMacro = [&](const wchar_t* indent, const wchar_t* macroName, Ptr<WfAttribute> att, const wchar_t* paramName)
{
	auto info = ResolveAttributeTd(att);
	if (!info.exists) return; // should not happen after semantic validation

	writer.WriteString(indent);
	writer.WriteString(macroName);
	writer.WriteString(L"(");
	writer.WriteString(ConvertType(info.attributeType));

	if (paramName)
	{
		writer.WriteString(L", L\"");
		writer.WriteString(ConvertName(paramName));
		writer.WriteString(L"\"");
	}

	if (info.hasArgument)
	{
		auto v = config->attributeEvaluator->GetAttributeValue(att);
		// Attribute arguments must be serialized constants here.
		CHECK_ERROR(v.type == runtime::WfInsType::String, L"Attribute argument must be serialized as WString here.");
		writer.WriteString(L", ");
		WriteWStringLiteralUnmanaged(writer, v.stringValue);
	}

	writer.WriteLine(L")");
};
```

Implementation note (mandatory): do not duplicate the escaping switch from `WfCpp_Expression.cpp::Visit(WfStringExpression*)`.
- Extract a shared helper (e.g. `WriteWStringLiteralUnmanaged(stream::StreamWriter& writer, const WString& value)`) declared in `WfCpp.h` and implemented in an existing `Source\\Cpp\\*.cpp`, and use it from both expression writer and reflection writer.

Additional hard requirement:
- Ensure the generated reflection `.cpp` that emits `ATTRIBUTE_*(::vl::__vwsn::att_cpp_...)` includes (directly or transitively) the header that declares these structs. Since `WfLibraryReflection.h` includes `WfLibraryPredefined.h`, including `WfLibraryReflection.h` is sufficient.

### Where to insert macros
These ordering constraints are **mandatory** (VlppReflection requirement):
- **Type-level**: immediately after `BEGIN_*_MEMBER(...)` and before any `CLASS_MEMBER_BASE(...)` / member registrations.
- **Struct member-level**: immediately after the `STRUCT_MEMBER(name)` line for that member.
- **Class/interface member-level**: immediately after the exact `CLASS_MEMBER_*` registration line for that member.
- **Method/ctor**:
  - Immediately after the `CLASS_MEMBER_CONSTRUCTOR(...)` / `CLASS_MEMBER_*METHOD*(...)` line, emit `ATTRIBUTE_MEMBER(...)` for the method itself.
  - Then emit `ATTRIBUTE_PARAMETER(L"paramName", ...)` for each parameter attribute, using the **exact parameter name string** matching what is passed in the `{ L"..." }` parameter-name list.


### Parameter attribute mapping
For a matched method decl, map parameter attributes by name:

```
auto FindArg = [&](WfFunctionDeclaration* f, const WString& name) -> Ptr<WfFunctionArgument>
{
	return From(f->arguments)
		.Where([&](Ptr<WfFunctionArgument> a)
		{
			return a->name.value == name;
		})
		.First(Ptr<WfFunctionArgument>());
};
```

Then for each `arg->attributes`, emit:

```
ATTRIBUTE_PARAMETER(L"paramName", <attrType>, ...)
```

## STEP 8: Run tests and update baselines

### What to do
Follow `# AFFECTED PROJECTS` in order.

### Why this is necessary
- Task 1 introduces new reflected types, which will change metadata dumps.
- Task 2 affects semantic analysis and constant evaluation.
- Task 3 affects generated C++ reflection output, exercised by CppTest suites.

### Expected baseline updates
`CompilerTest_GenerateMetadata` will likely fail initially due to new types.
Before accepting any baseline update, require a strict diff review:
- Diffs must be limited to the intended new `system::workflow_attributes::att_cpp_*` additions.
- Any unrelated reflection drift must be investigated/fixed before updating baselines.

After confirming the diff is limited to the intended additions, update baseline files:
- `REPO-ROOT\Test\Resources\Baseline\Reflection64.txt`
- `REPO-ROOT\Test\Resources\Baseline\Reflection32.txt`

Then rerun the same tests to confirm stability.

# !!!FINISHED!!!
