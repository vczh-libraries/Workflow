# !!!EXECUTION!!!

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

### Edit: `REPO-ROOT\Source\Library\WfLibraryPredefined.h`
Insert the following block right before the final `#endif` (i.e. after the existing closing braces of `namespace vl { namespace reflection { namespace description { ... }}}`).

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

## STEP 2: Register and rename attribute types in Workflow library reflection (Task 1)

### Edit: `REPO-ROOT\Source\Library\WfLibraryReflection.h`
1) Add `WORKFLOW_LIBRARY_ATTRIBUTE_TYPES(F)` right before `#define WORKFLOW_LIBRARY_TYPES(F)\`.
2) Append `WORKFLOW_LIBRARY_ATTRIBUTE_TYPES(F)\` right after `F(Versioning)\` inside `WORKFLOW_LIBRARY_TYPES(F)`.

Replace the existing `#define WORKFLOW_LIBRARY_TYPES(F)\` block with:

```
#define WORKFLOW_LIBRARY_ATTRIBUTE_TYPES(F)\
			F(vl::__vwsn::att_cpp_File)\
			F(vl::__vwsn::att_cpp_UserImpl)\
			F(vl::__vwsn::att_cpp_Private)\
			F(vl::__vwsn::att_cpp_Protected)\
			F(vl::__vwsn::att_cpp_Friend)\

#define WORKFLOW_LIBRARY_TYPES(F)\
			F(Sys)						\
			F(Math)						\
			F(Localization)					\
			F(CoroutineStatus)				\
			F(CoroutineResult)				\
			F(ICoroutine)					\
			F(EnumerableCoroutine::IImpl)	\
			F(EnumerableCoroutine)			\
			F(AsyncStatus)					\
			F(AsyncContext)					\
			F(IAsync)						\
			F(IPromise)					\
			F(IFuture)					\
			F(IAsyncScheduler)				\
			F(AsyncCoroutine::IImpl)		\
			F(AsyncCoroutine)				\
			F(StateMachine)					\
			F(Versioning)					\
			WORKFLOW_LIBRARY_ATTRIBUTE_TYPES(F)\
```

### Edit: `REPO-ROOT\Source\Library\WfLibraryReflection.cpp`

#### 2.1 Add type renames
In the `#ifndef VCZH_DEBUG_NO_REFLECTION` TypeName block, append these lines right after the existing `IMPL_TYPE_INFO_RENAME(...Versioning...)` line:

```
IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_cpp_File, system::workflow_attributes::att_cpp_File)
IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_cpp_UserImpl, system::workflow_attributes::att_cpp_UserImpl)
IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_cpp_Private, system::workflow_attributes::att_cpp_Private)
IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_cpp_Protected, system::workflow_attributes::att_cpp_Protected)
IMPL_TYPE_INFO_RENAME(vl::__vwsn::att_cpp_Friend, system::workflow_attributes::att_cpp_Friend)
```

#### 2.2 Register attribute structs
In the `#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA` reflection registration block, insert the following right after `#define _ ,` and before `BEGIN_CLASS_MEMBER(Sys)`:

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

### Edit: `REPO-ROOT\Source\Analyzer\WfAnalyzer.h`

#### 3.1 Remove the hardcoded map field
Remove:

```
typedef collections::Dictionary<AttributeKey, Ptr<ITypeInfo>>										AttributeTypeMap;
...
AttributeTypeMap															attributes;
```

#### 3.2 Add resolver types, cache, and APIs
Add these declarations inside `class WfLexicalScopeManager`:
- Keep `typedef collections::Pair<WString, WString> AttributeKey;`
- Add the following right after `AttributeKey` typedef, and place cache + methods adjacent to existing public fields/methods (same access pattern as the current `attributes` field):

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
	WString							GetWorkflowAttributeTypeName(const WString& category, const WString& name);
```

### Edit: `REPO-ROOT\Source\Analyzer\WfAnalyzer.cpp`

#### 3.3 Remove hardcoded attribute list
In `WfLexicalScopeManager::WfLexicalScopeManager(...)`, delete these lines:

```
attributes.Add({ L"cpp", L"File" }, TypeInfoRetriver<WString>::CreateTypeInfo());
attributes.Add({ L"cpp", L"UserImpl" }, TypeInfoRetriver<void>::CreateTypeInfo());
attributes.Add({ L"cpp", L"Private" }, TypeInfoRetriver<void>::CreateTypeInfo());
attributes.Add({ L"cpp", L"Protected" }, TypeInfoRetriver<void>::CreateTypeInfo());
attributes.Add({ L"cpp", L"Friend" }, TypeInfoRetriver<ITypeDescriptor*>::CreateTypeInfo());
```

#### 3.4 Implement resolver functions
Insert the following implementations after `WfLexicalScopeManager::~WfLexicalScopeManager()` and before the next method (`AddModule(...)`):

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

## STEP 4: Update semantic validation to use the resolver (Task 2)

### Edit: `REPO-ROOT\Source\Analyzer\WfAnalyzer_ValidateSemantic_Declaration.cpp`
In `ValidateSemanticDeclarationVisitor::Visit(List<Ptr<WfAttribute>>& attributes)`, replace the body of the `for (auto attribute : attributes)` loop with:

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

### Edit: `REPO-ROOT\Source\Emitter\WfEmitter.cpp`
In `WfAttributeEvaluator::GetAttributeValue(Ptr<WfAttribute> att)`, replace:

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
```

Then, right after:

```
auto value = ins.valueParameter;
```

Insert:

```
// @cpp:Friend: serialize typeof(T) as reflection name so it round-trips via GetTypeDescriptor(string)
if (att->category.value == L"cpp" && att->name.value == L"Friend")
{
	if (value.type == runtime::WfInsType::Unknown && value.typeDescriptor != nullptr)
	{
		value = runtime::WfRuntimeValue(value.typeDescriptor->GetTypeName());
	}
	CHECK_ERROR(value.type == runtime::WfInsType::String, L"WfAttributeEvaluator::GetAttributeValue(...)#Unexpected value in attribute: @cpp:Friend.");
}
```

Then ensure the cache write and return path both use the possibly-converted `value`:

```
attributeValues.Add(att, value);
return value;
```

## STEP 6: Update C++ codegen for @cpp:Friend to match the new string representation

### Edit: `REPO-ROOT\Source\Cpp\WfCpp_WriteClass.cpp`
In `WfCppConfig::WriteHeader_Class(...)`, inside the loop:

```
for (auto attribute : attributeEvaluator->GetAttributes(decl->attributes, L"cpp", L"Friend"))
{
	...
}
```

Replace:

```
auto attValue = attributeEvaluator->GetAttributeValue(attribute);
CHECK_ERROR(attValue.type == runtime::WfInsType::Unknown && attValue.typeDescriptor != nullptr, L"Unexpected value in attribute: @cpp.Friend.");
auto td = attValue.typeDescriptor;
```

With:

```
auto attValue = attributeEvaluator->GetAttributeValue(attribute);
CHECK_ERROR(attValue.type == runtime::WfInsType::String, L"Unexpected value in attribute: @cpp.Friend.");
auto td = description::GetTypeDescriptor(attValue.stringValue);
CHECK_ERROR(td != nullptr, L"Unexpected value in attribute: @cpp.Friend.");
```

## STEP 7: Emit ATTRIBUTE_* macros in generated C++ reflection code (Task 3)

### Edit: `REPO-ROOT\Source\Cpp\WfCpp.h`
Add a shared helper declaration in `namespace vl::workflow::cppcodegen` (near other `extern` declarations, e.g. next to `GenerateExpression(...)`):

```
extern void						WriteWStringLiteralUnmanaged(stream::StreamWriter& writer, const WString& value);
```

### Edit: `REPO-ROOT\Source\Cpp\WfCpp_Expression.cpp`
1) Add the helper implementation (place near the top under `Expression Helpers`, before the expression visitor class):

```
void WriteWStringLiteralUnmanaged(stream::StreamWriter& writer, const WString& value)
{
	writer.WriteString(L"::vl::WString::Unmanaged(L\"");
	for (vint i = 0; i < value.Length(); i++)
	{
		auto c = value[i];
		switch (c)
		{
		case L'\\': writer.WriteString(L"\\\\"); break;
		case L'\'': writer.WriteString(L"\\\'"); break;
		case L'\"': writer.WriteString(L"\\\""); break;
		case L'\r': writer.WriteString(L"\\r"); break;
		case L'\n': writer.WriteString(L"\\n"); break;
		case L'\t': writer.WriteString(L"\\t"); break;
		default: writer.WriteChar(c);
		}
	}
	writer.WriteString(L"\")");
}
```

2) Replace the body of `Visit(WfStringExpression* node)` with:

```
WriteWStringLiteralUnmanaged(writer, node->value.value);
```

### Edit: `REPO-ROOT\Source\Cpp\WfCpp_WriteReflection.cpp`

#### 7.1 Build reverse maps from reflection objects to AST declarations
At the beginning of `WfCppConfig::WriteCpp_Reflection(stream::StreamWriter& writer)` (after `LoadTypes(this, tds);`), add:

```
Dictionary<ITypeDescriptor*, Ptr<WfDeclaration>> typeDecls;
for (auto [decl, index] : indexed(manager->declarationTypes.Keys()))
{
	typeDecls.Add(manager->declarationTypes.Values()[index].Obj(), decl);
}

Dictionary<IMemberInfo*, Ptr<WfDeclaration>> memberDecls;
for (auto [decl, index] : indexed(manager->declarationMemberInfos.Keys()))
{
	memberDecls.Add(manager->declarationMemberInfos.Values()[index].Obj(), decl);
}
```

#### 7.2 Add attribute emission helpers
Add local helpers inside `WfCppConfig::WriteCpp_Reflection(...)` (before the `for (auto td : tds)` loop that writes `BEGIN_*_MEMBER` blocks):

```
auto ResolveAttributeTd = [&](Ptr<WfAttribute> att)
{
	return manager->ResolveWorkflowAttribute(att->category.value, att->name.value);
};

auto WriteAttributeMacro = [&](const wchar_t* indent, const wchar_t* macroName, Ptr<WfAttribute> att, const wchar_t* paramName)
{
	auto info = ResolveAttributeTd(att);
	if (!info.exists || info.attributeType == nullptr) return;

	writer.WriteString(indent);
	writer.WriteString(macroName);
	writer.WriteString(L"(");

	// ATTRIBUTE_PARAMETER(PARAMETER_NAME, TYPE, ...)
	if (paramName)
	{
		writer.WriteString(L"L\"");
		writer.WriteString(ConvertName(paramName));
		writer.WriteString(L"\", ");
	}

	writer.WriteString(ConvertType(info.attributeType));

	// NOTE: currently all serialized attribute arguments are required to be WString.
	if (info.hasArgument)
	{
		auto v = attributeEvaluator->GetAttributeValue(att);
		CHECK_ERROR(v.type == runtime::WfInsType::String, L"Attribute argument must be serialized as WString here.");
		writer.WriteString(L", ");
		WriteWStringLiteralUnmanaged(writer, v.stringValue);
	}

	writer.WriteLine(L")");
};
```

#### 7.3 Insert macro emissions at mandatory positions
Emit attributes only when the reverse lookup succeeds (reflection traversal may include members/descriptors not originating from Workflow source declarations).

- Type-level:
	- Immediately after emitting `BEGIN_STRUCT_MEMBER(...)` / `BEGIN_CLASS_MEMBER(...)` / `BEGIN_INTERFACE_MEMBER(...)`, locate the corresponding `WfDeclaration` using `typeDecls` and emit:
		- `ATTRIBUTE_TYPE(...)` for each attribute in that declaration’s `attributes`.

- Member-level:
	- Immediately after each emitted `STRUCT_MEMBER(...)` / `CLASS_MEMBER_*...(...)` line, locate the member declaration using `memberDecls` (the `IMemberInfo*` used by that registration line) and emit:
		- `ATTRIBUTE_MEMBER(...)` for each attribute in that declaration’s `attributes`.

- Parameter-level:
	- Immediately after the method/ctor registration line (and after method-level `ATTRIBUTE_MEMBER(...)`), emit `ATTRIBUTE_PARAMETER(...)` for each parameter attribute.
	- The parameter name passed to `ATTRIBUTE_PARAMETER` must match the name used by the method/ctor registration line. Avoid mismatches by sourcing the name from the same place (e.g. the `WfFunctionArgument` or reflection `IParameterInfo`) and applying the same name transform.

Use the parameter-name mapping helper when you have a `WfFunctionDeclaration*`:

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

## STEP 8: Run tests and update baselines
Follow `# AFFECTED PROJECTS` in order.
- Expect `CompilerTest_GenerateMetadata` to require baseline updates due to new reflected types.
	- Update `REPO-ROOT\Test\Resources\Baseline\Reflection64.txt` from `REPO-ROOT\Test\UnitTest\Generated\Reflection64.txt`.
	- Update `REPO-ROOT\Test\Resources\Baseline\Reflection32.txt` from `REPO-ROOT\Test\UnitTest\Generated\Reflection32.txt`.
- Ensure both `Debug|x64` and `Debug|Win32` configurations pass independently.

# FIXING ATTEMPTS

# !!!FINISHED!!!
