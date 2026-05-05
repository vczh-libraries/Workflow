# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- If you can't fix a runtime issue within a few rounds of guess-and-edit, you need to debug the process.
- I am a fan of crash early. When something should happen, it should just happen, do not play a game like "what if it is not the case" and silently covers the issue. One example is that, if an object should not be null, then we should just use it, if a nullable object should not be null, we should just cast it. No test is performed in this case, using it will crash if it is null, and we know there is a problem. Fix the actual problem instead of doing "error tolerance".
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Task 1

- In `rpcjson_Serialize` and `rpcjson_Deserialize`, after all custom types are processed, the rest should be given to C++ written functions.
  - Such functions accept function pointer of `rpcjson_Serialize` and `rpcjson_Deserialize` so that they could handle byref collection properly.
  - `func IRpcLifecycle::JsonSerializePredefinedTypes(value : object, rpcjson_Serialize : func (value) : (system::JsonNode^))`
  - `func IRpcLifecycle::JsonDeserializePredefinedTypes(value : object, rpcjson_Deserialize : func (system::JsonNode^) : (value))`
  - The above functions will be put in `WfLibraryRpcJson.(h|cpp)` in `Test/Source/Library/Rpc`, but registered as static members to `IRpcLifecycle`.
  - The above functions handle `UnknownType_PrimitiveSchema` in `TODO_RPC_JSON.md`, as well as byref collection serialization.
  - Code generation of `rpcjson_Serialize` and `rpcjson_Deserialize` can know skip these types, avoid repeating code in all these sample outputs.
