# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- If you can't fix a runtime issue within a few rounds of guess-and-edit, you need to debug the process.
- I am a fan of crash early. When something should happen, it should just happen, do not play a game like "what if it is not the case" and silently covers the issue. One example is that, if an object should not be null, then we should just use it, if a nullable object should not be null, we should just cast it. No test is performed in this case, using it will crash if it is null, and we know there is a problem. Fix the actual problem instead of doing "error tolerance".
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Task 1

- In `Wrapper_*.txt` and `Wrapper_*_Json.txt`, there are a lot of "in function variable" which have types, local variable types should be omitted when possible.
  - Precisely tell for each variable, is its type can be omitted?
    - If can, omit its type in the variable declaration.
    - Otherwise, just keep it.
  - DO NOT use `infer` or any other way to move the type from the variable to its initializer expression, that's even worse.
- There are also some `of` expression doing type inferring. If implicit type conversion work at that place, inferring is not needed.
  - Precisely tell for each `infer` expression, is type inferring unnecessary?
    - If unnecessary, remove the `infer` part from the expression.
    - Otherwise, just keep it.
  - DO NOT use casting just to replace the `infer` with other thing that still has a type, that's even worse
- Check all strong cast `cast` and weak cast `as`, if implicit type conversion work at that place, casting is not needed.
- Prefer `var v : T = e;` over `var v = e over T;` if `T` cannot avoid.
- The goal is to not write types in Workflow as much as you can. But we know not all types can be avoided. So you need to do it case by case.

# UPDATES

- Confirmed the relevant generated outputs are produced from `Source/Analyzer/WfAnalyzer_GenerateRpc*.cpp`, so the fix belongs in the generators.
- Regenerated RPC wrappers with `CompilerTest_LoadAndCompile`.
- Text sweep after regeneration found no remaining `of`, `null of (...)`, or `cast (object)` in `Test/Generated/RpcMetadata32/Wrapper_*.txt` or `Test/Generated/RpcMetadata64/Wrapper_*.txt`.

# TEST

- Update the RPC wrapper generators instead of editing `Test/Generated` directly, because `Wrapper_*.txt` and `Wrapper_*_Json.txt` are generated outputs.
- Regenerate wrappers with `CompilerTest_LoadAndCompile`; success criteria:
  - Local variables whose initializer has an explicit type, such as `new T`, `cast T`, `as T`, typed function calls, member access, and default `int` literals, print without `: T`.
  - Local variables whose initializer needs an expected type, such as `null` or `{}`, keep `var v : T = e;`.
  - Struct constructor `of T` expressions are removed only where the function return type supplies the expected type.
  - Callback `of func(...)` expressions are removed only if overload resolution accepts the bare function reference.
  - Strong and weak casts remain where they are required to downcast, unbox, perform numeric/string conversions, select event/interface members, or make a nullable conversion explicit.
- Run generated wrapper tests: build Debug x64 and Win32, run `CompilerTest_LoadAndCompile`, then run `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` on x64 and Win32. Run additional required unit tests from `Project.md` according to `verify-and-commit.md`.

# PROPOSALS

- [CONFIRMED] No.1 Omit local variable types at generation sites where the initializer already carries the type

## No.1 Omit local variable types at generation sites where the initializer already carries the type

### CODE CHANGE

- Added inferred local variable generation for RPC wrapper locals where the initializer already determines the type (`new T`, casts, typed calls/member values, and integer literals).
- Kept explicit local variable types for `null` and `{}` initializers, because those require expected types in Workflow.
- Removed redundant JSON wrapper `of` expressions for struct constructor returns and recursive JSON callback function references after confirming the return type/call target supplies the expected type.
- Removed redundant object boxing casts where the expression is already accepted by implicit conversion.
- Kept casts/as expressions that still perform downcasts, interface extraction, unboxing, nullable conversion, or JSON node conversion.
- Changed typed cached-property fields from `null of (T)` to `null`, preserving the field type while avoiding a redundant infer expression.

### RESULT

Implemented in the RPC wrapper generators and regenerated generated wrapper/assembly text outputs.

Verification completed:
- Debug x64 build passed.
- Debug Win32 build passed.
- `CompilerTest_LoadAndCompile` passed on x64: 6/6 files, 699/699 cases.
- `LibraryTest` passed on x64 and Win32: 2/2 files, 14/14 cases.
- `CompilerTest_GenerateMetadata` passed on x64 and Win32: 1/1 files, 2/2 cases.
- `RuntimeTest` passed on x64 and Win32: 4/4 files, 257/257 cases.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` passed on x64 and Win32: each 2/2 files, 223/223 cases.
