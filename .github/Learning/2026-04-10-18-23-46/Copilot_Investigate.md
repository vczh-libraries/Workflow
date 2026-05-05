# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Create a new test category `Runtime` with sample `Runtime/Attributes.txt`, following the pattern of `IndexDebugger.txt` and the `Debugger` folder.

In `Attributes.txt`, create a Workflow script with class/interface/struct types, marking the types and every possible kind of members and parameters with `@test:*` attributes.

Write unit tests in the `RuntimeTest` project to:
1. Load `Runtime/Attributes.txt`, compile it, load types into memory, then walk through `ITypeDescriptor` from workflow types to verify all attributes are accessible via the standard reflection API.
2. Load pre-compiled binary of the same sample and do the same attribute verification.

The user suspects that when workflow virtual machine loads these types, the `WfCustomType`/`WfTypeImpl` mock type descriptors do NOT load attributes into the mock objects, so attributes cannot be read from the standard reflection API.

Also add `TestRuntime.cpp` to `CompilerTest_LoadAndCompile` project to compile `Runtime` category samples (without C++ codegen), and a corresponding test in `RuntimeTest` to load the compiled binary and verify attributes.

# UPDATES

## Confirmed: Attributes NOT Populated on WfCustomType Runtime Descriptors

### Root Cause Analysis

The Workflow compiler (`WfEmitter_Assembly.cpp:GenerateAssembly()`) creates `WfTypeImpl` with `WfClass`, `WfInterface`, `WfStruct` objects but **never calls `RegisterAttribute()`** on any of these type descriptors or their members.

The `RegisterAttribute()` method exists on `AttributeBagSource` (base of `TypeDescriptorImpl`), and `WfCustomType` inherits from `TypeDescriptorImpl`. But the emitter pipeline simply never transfers attribute information from the AST (`WfAttribute` nodes) to the runtime type descriptors.

### Evidence

1. `Source/Emitter/WfEmitter_Assembly.cpp` (line ~180-210): `GenerateAssembly()` transfers classes/interfaces/structs/enums to `assembly->typeImpl` but never processes attributes.
2. `Source/Runtime/WfRuntimeTypeDescriptor.cpp`: `WfCustomType` and its subclasses (`WfClass`, `WfInterface`, etc.) never call `RegisterAttribute()`.
3. Grep for `RegisterAttribute` in `Source/` returns zero hits.
4. Test `TestRuntime.cpp` "Runtime attributes from source": Compiles a Workflow script with @test:* attributes on all member types, verifies types ARE registered in the global type manager, but confirms `GetAttributeCount() == 0` on all type descriptors and members.

### Test Results

- `RuntimeTest` x64: 131/131 passed
- `RuntimeTest` Win32: 131/131 passed

### Key Finding: Module vs Namespace

- `module test;` does NOT create a type name prefix. Types are registered as `MyClass`, not `test::MyClass`.
- Only `namespace foo { ... }` blocks create type name prefixes.

## Pending Work

1. Add `TestRuntime.cpp` to `CompilerTest_LoadAndCompile` project to compile Runtime samples.
2. Enable the "from binary" test case in RuntimeTest.
3. When the bug is fixed, update `AssertRuntimeAttributes` to verify attributes ARE present instead of asserting they are missing.

# TEST

Create a Workflow script `Runtime/Attributes.txt` that defines classes, interfaces, and structs with `@test:*` attributes on types and members.

Write C++ test cases in `Test/Source/TestRuntime.cpp` (a new file, distinct from the existing `TestRuntime.cpp` which is about Codegen):
- Actually wait - the existing `TestRuntime.cpp` is already about Codegen. We need to either add to it or create a separate file. Per the user's instructions, we add test cases to `TestRuntime.cpp` in the RuntimeTest project, and create a new `TestRuntime.cpp` in the CompilerTest_LoadAndCompile project.

Test 1 (in RuntimeTest): Compile `Runtime/Attributes.txt` directly, load types into memory, walk `ITypeDescriptor` and verify attributes on types and members.
Test 2 (in RuntimeTest): Load pre-compiled binary of `Runtime/Attributes.txt` (compiled by CompilerTest_LoadAndCompile), load types, walk `ITypeDescriptor` and verify the same attributes.

Success criteria:
- Both test cases pass in `Debug|Win32` and `Debug|x64`.
- Attributes on types, methods, properties, events, constructors, and parameters are all accessible from the reflection API.

# PROPOSALS

## Proposal: Populate Attributes During Assembly Generation

The fix should be in `Source/Emitter/WfEmitter_Assembly.cpp:GenerateAssembly()`. After building each `WfClass`/`WfInterface`/`WfStruct`, iterate over the AST `WfAttribute` nodes for each declaration and its members, evaluate attribute constructor arguments, create `IAttributeInfo` instances, and call `RegisterAttribute()` on the corresponding type descriptor or member info.

Key steps:
1. For each class/interface declaration and its members, find the corresponding `WfAttribute` AST nodes.
2. Resolve each attribute type via `GetTypeDescriptor()` for the attribute struct.
3. Evaluate constant constructor arguments.
4. Call `RegisterAttribute()` on the target `AttributeBagSource`.

Also need to ensure binary serialization/deserialization of `WfAssembly` includes attribute data, so the "from binary" path also works.
