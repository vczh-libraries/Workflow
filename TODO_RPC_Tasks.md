# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- If you can't fix a runtime issue within a few rounds of guess-and-edit, you need to debug the process.
- I am a fan of crash early. When something should happen, it should just happen, do not play a game like "what if it is not the case" and silently covers the issue. One example is that, if an object should not be null, then we should just use it, if a nullable object should not be null, we should just cast it. No test is performed in this case, using it will crash if it is null, and we know there is a problem. Fix the actual problem instead of doing "error tolerance".
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Workflow Script Best Practice

- Avoid explicit type specification whenever possible:
  - Prefer `var v = e;` whenever `T` can be omitted.
  - Prefer `var v : T = e;` over `var v = e over T;` if `T` cannot avoid.
  - When implicit type conversion works at the place, avoid `cast`, `as` and `infer` expression.
  - Prefer `cast *` over `cast T` when the context accepts `T`.

## Task 1

- In `rpcjson_Serialize` and `rpcjson_Deserialize` for serializable custom primitive types, it should serializelike enum but the second element is string.
  - Need to inject a serializable struct to `CppTypes.(h|cpp)`.
  - Need to update the .d.ts file as well.
- In order to test it, you are going to add serialization to `Point` in `CppTypes.(h|cpp)` so that it could be `cast string` to like `123,456`.
- In sample `Rpc/PrimitiveTypes` to add a function for `Point`, and print it.
- In this way `Point` is used, so you can verify two things:
  - `Point` is defined outside and workflow code generation for RPC still handle it.
  - In JSON serialization it is serialized to `[type,string]` instead of `{"$":...}`.
  - You will be able to know if you have done it right by reading at the generated .d.ts file.
