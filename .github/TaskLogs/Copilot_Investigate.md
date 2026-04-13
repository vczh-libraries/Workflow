# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Delete `CopyAttributes` and `CopyAttribute` from `WfAnalyzer_ValidateRPC.cpp`, because they should be recreated from reflection metadata. Functions like `FindOriginalClassDecl` and `FindOriginalMemberDecl` should also be removed since they are no longer needed.

Requirements:
- The whole rpc metadata is recreated from ITypeDescriptor instances
- Input module ASTs is just for limiting how many `@rpc:Interface` marked interfaces are picked up (already calculated and put in context.workflowRpcInterfaceTds)
- The original module ASTs should not be used for any reason
- Add auto properties to the test case AttributesRpc.txt to verify expanded properties appear correctly in metadata
- Run CompilerTest_LoadAndCompile, expect it to fail because baselines need to be updated
- Verify the change by git diff

# UPDATES

# TEST

1. Build `CompilerTest_LoadAndCompile` and run it - expect test failure due to baseline mismatch.
2. Run git diff to verify the metadata output changes show:
   - Base types now use fully-qualified names (e.g., `::IBase` instead of `IBase`)
   - Attributes are regenerated from reflection (should be identical for RPC attributes)
   - Auto properties from `IAutoProps` are expanded into regular properties, methods, and events
3. Update baselines to match the new output.
4. Re-run tests to confirm they pass.

# PROPOSALS

In `Attributes.txt`, create a Workflow script with class/interface/struct types, marking the types and every possible kind of members and parameters with `@test:*` attributes.

Write unit tests in the `RuntimeTest` project to:
1. Load `Runtime/Attributes.txt`, compile it, load types into memory, then walk through `ITypeDescriptor` from workflow types to verify all attributes are accessible via the standard reflection API.
2. Load pre-compiled binary of the same sample and do the same attribute verification.

The user suspects that when workflow virtual machine loads these types, the `WfCustomType`/`WfTypeImpl` mock type descriptors do NOT load attributes into the mock objects, so attributes cannot be read from the standard reflection API.

Also add `TestRuntime.cpp` to `CompilerTest_LoadAndCompile` project to compile `Runtime` category samples (without C++ codegen), and a corresponding test in `RuntimeTest` to load the compiled binary and verify attributes.

# UPDATES

## UPDATE: Test Infrastructure Complete

All test infrastructure has been created:
- `Test/Resources/IndexRuntime.txt` with `Attributes` entry
- `Test/Resources/Runtime/Attributes.txt` with Workflow script containing classes/interfaces/structs decorated with `@test:*` attributes
- `Test/Source/TestRuntimeCompile.cpp` for `CompilerTest_LoadAndCompile` project (compiles Runtime samples and saves binaries)
- `Test/Source/TestRuntime.cpp` for `RuntimeTest` project (tests attribute access from source and from binary)
- `AssertRuntimeAttributes` verifies expected attribute values on all type descriptors, constructors, methods, properties, events, and struct fields

## UPDATE: Bug Confirmed - Attributes NOT Populated

### Root Cause Analysis

The Workflow compiler (`WfEmitter_Assembly.cpp:GenerateAssembly()`) creates `WfTypeImpl` with `WfClass`, `WfInterface`, `WfStruct` objects but **never calls `RegisterAttribute()`** on any of these type descriptors or their members.

The `RegisterAttribute()` method exists on `AttributeBagSource` (base of `TypeDescriptorImpl`), and `WfCustomType` inherits from `TypeDescriptorImpl`. But the emitter pipeline simply never transfers attribute information from the AST (`WfAttribute` nodes) to the runtime type descriptors.

### Evidence

1. `Source/Emitter/WfEmitter_Assembly.cpp` (line ~180-210): `GenerateAssembly()` transfers classes/interfaces/structs/enums to `assembly->typeImpl` but never processes attributes.
2. `Source/Runtime/WfRuntimeTypeDescriptor.cpp`: `WfCustomType` and its subclasses (`WfClass`, `WfInterface`, etc.) never call `RegisterAttribute()`.
3. Grep for `RegisterAttribute` in `Source/` returns zero hits.

### Test Results (Attribute Assertions Verify Expected Values)

- `CompilerTest_LoadAndCompile` x64: 546/546 passed (generates `Assembly.Runtime.Attributes.bin`)
- `RuntimeTest` x64: FAILS on "Runtime attributes from source" with `Assertion failure: att != nullptr`
- `RuntimeTest` Win32: FAILS on "Runtime attributes from source" with `Assertion failure: att != nullptr`

The failure confirms the user's suspicion: when the Workflow VM loads types, `WfCustomType` mock type descriptors do NOT have attributes populated, so `FindAttribute()` returns `nullptr`.

### Key Finding: Module vs Namespace

- `module test;` does NOT create a type name prefix. Types are registered as `MyClass`, not `test::MyClass`.
- Only `namespace foo { ... }` blocks create type name prefixes.

# TEST [CONFIRMED]

Create a Workflow script `Runtime/Attributes.txt` that defines classes, interfaces, and structs with `@test:*` attributes on types and members.

Write C++ test cases:
- `Test/Source/TestRuntimeCompile.cpp` (in `CompilerTest_LoadAndCompile`): Compiles Runtime samples and saves binary assemblies.
- `Test/Source/TestRuntime.cpp` (in `RuntimeTest`): Tests attribute access from source compilation and from binary loading.

Test 1 (in RuntimeTest): "Runtime attributes from source" - Compile `Runtime/Attributes.txt` directly, load types into memory, walk `ITypeDescriptor` and verify attributes on types and members.
Test 2 (in RuntimeTest): "Runtime attributes from binary" - Load pre-compiled binary of `Runtime/Attributes.txt` (compiled by CompilerTest_LoadAndCompile), load types, walk `ITypeDescriptor` and verify the same attributes.

`AssertRuntimeAttributes` verifies:
- `MyClass`: `@test:Int(1)` on type, `@test:Int(10)`/`@test:Int(20)` on constructors, `@test:Range("hello")` on `GetX`, `@test:List("setter")` on `SetX`, `@test:Map(2.5)` on property `X`, `@test:Int(100)` on event `XChanged`
- `MyInterface`: `@test:List("my-interface")` on type, `@test:Int(42)` on `DoSomething`, `@test:Range("prop-value")` on property `Value`, `@test:Map(3.14)` on event `ValueChanged`
- `MyStruct`: `@test:Range("my-struct")` on type, `@test:Int(7)` on field `a`, `@test:List("field-b")` on field `b`

Success criteria:
- Both "from source" and "from binary" test cases pass in `Debug|Win32` and `Debug|x64`.
- All assertions in `AssertRuntimeAttributes` pass (attributes accessible via reflection API).

Current state: Both tests FAIL with `Assertion failure: att != nullptr` on both x64 and Win32, confirming the bug.

# PROPOSALS

- No.1 Populate Attributes During Assembly Generation

## No.1 Populate Attributes During Assembly Generation

The fix should be in `Source/Emitter/WfEmitter_Assembly.cpp:GenerateAssembly()`. After building each `WfClass`/`WfInterface`/`WfStruct`, iterate over the AST `WfAttribute` nodes for each declaration and its members, evaluate attribute constructor arguments, create `IAttributeInfo` instances, and call `RegisterAttribute()` on the corresponding type descriptor or member info.

Key steps:
1. For each class/interface declaration and its members, find the corresponding `WfAttribute` AST nodes.
2. Resolve each attribute type via `GetTypeDescriptor()` for the attribute struct.
3. Evaluate constant constructor arguments.
4. Call `RegisterAttribute()` on the target `AttributeBagSource`.

Also need to ensure binary serialization/deserialization of `WfAssembly` includes attribute data, so the "from binary" path also works.

### CODE CHANGE
