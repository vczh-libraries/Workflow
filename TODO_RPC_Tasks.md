# Repro

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
