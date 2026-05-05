# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

I am going to allow `delete{}` destructor in `new InterfaceType^(or *){...}` expression. Currently the parser forbids it. The goal is to:

1. Create a new `Codegen/NewInterfaceDtor.txt` test case, doing what `ClassDtor` does but rewriting it to use interface and new interface expression.
2. `delete{}` is optional and at most only once (this is ensured for class but a new test case is needed to test the `TooManyDestructor` error for new interface expression, error code G13). One more helper function for `WfNewInterfaceExpression` needs to be added to `WfErrors`.
3. Run all test cases in debug with both Win32 and X64 to verify.
4. First create the `NewInterfaceDtor.txt` test case and the test will just fail, fix it until every test project works. Fix the compiler to pass `CompilerTest_LoadAndCompile` project first.
5. `delete{}` should work just like one in class to serve the destructor purpose as expected. In C++ code generation, just generate a destructor. This will make `CppTest*` test projects pass.
6. For RuntimeTest: make the virtual machine work. Process the `CreateInterface` instruction correctly. Add a `destructorFunction` to `WfRuntimeInterfaceInstance`, and when `IMethodInfo` parameter is nullptr assign to `destructorFunction`. Call `destructorFunction` in `WfRuntimeInterfaceInstance`'s destructor. Fix `NewInterfaceExpressionVisitor` to process the `WfDestructorDeclaration` branch. Also check all other affected places in visitors starting from `WfNewInterfaceExpression` processing.

# UPDATES

# TEST

1. Create `Test/Resources/Codegen/NewInterfaceDtor.txt` - a test case that uses `delete{}` in a new interface expression, analogous to `ClassDtor.txt`.
2. Create `Test/Resources/AnalyzerError/G13_DuplicatedDestructorInNewExpression.txt` - a test case that verifies the `TooManyDestructor` error for new interface expressions.
3. Add entries to `IndexCodegen.txt` and `IndexAnalyzerError.txt`.
4. Build and run `CompilerTest_LoadAndCompile` with Debug|x64 - expect initial failure, then fix.
5. Build and run `CppTest`, `CppTest_Metaonly`, `CppTest_Reflection` with both Debug|x64 and Debug|Win32.
6. Build and run `RuntimeTest` (if exists) or `LibraryTest` with both Debug|x64 and Debug|Win32.
7. Criteria: All test cases pass in all projects, both Win32 and x64, in Debug configuration.

# PROPOSALS

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
