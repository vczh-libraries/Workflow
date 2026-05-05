# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

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

# UPDATES

# TEST

- Add C++ helpers registered as static members on `system::IRpcLifecycle` and update generated `rpcjson_Serialize` / `rpcjson_Deserialize` to delegate predefined primitives and unknown collection schemas to them after generated enum and struct cases.
- Regenerate RPC C++ samples by running `CompilerTest_LoadAndCompile`; success criteria:
  - Generated unknown JSON serialization functions keep custom enum/struct cases but no longer repeat primitive and collection fallback blocks.
  - Existing RPC JSON samples, especially byref collection paths, still pass in `RuntimeTest` and `CppTest*`.
  - Reflection metadata updates include the two new static members on `IRpcLifecycle`.

# PROPOSALS

- No.1 Move predefined unknown JSON schemas into reflected C++ helpers

## No.1 Move predefined unknown JSON schemas into reflected C++ helpers

### CODE CHANGE

Add `WfLibraryRpcJson.h` and `WfLibraryRpcJson.cpp` under `Source/Library/Rpc` with static helper functions for predefined primitive and unknown collection JSON schemas. Register them as static external methods on `IRpcLifecycle`, then change the RPC JSON generator so only custom enum and struct cases remain generated before delegating to the helpers.

### RESULT

Implemented `IRpcLifecycle::JsonSerializePredefinedTypes` and `IRpcLifecycle::JsonDeserializePredefinedTypes` in C++ and registered them as static methods. Updated RPC JSON code generation to keep generated custom enum/struct handling and delegate predefined primitives plus unknown list/map schemas to the C++ helpers using recursive callbacks.

Verification completed:
- Debug x64 build passed.
- Debug Win32 build passed.
- `LibraryTest` passed on x64 and Win32.
- `CompilerTest_GenerateMetadata` passed on x64 and Win32.
- `CompilerTest_LoadAndCompile` passed on x64.
- Regenerated source build passed on x64 and Win32.
- `RuntimeTest` passed on x64 and Win32.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` passed on x64 and Win32.
