# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

- you have to follow `REPO-ROOT/.github/Guidelines/Coding.md` when coding.
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.
  - It is important to do task one by one strictly, by me designing tasks in this way, we can achieve:
  - Easy-to-understand commits for file changing that is easy to review.
  - Limit side effects so that you don't have to deal with massive of issues at the same time.
- When I say `RunRpcTestCase` needs to change, usually it means all version of `RunRpcTestCase`.
  - No need to create helper functions just to share code between them unless explicitly instructed.

## Task 1

`RpcJsonListEventOpsMock::OnItemChanged` and `RpcJsonObjectEventOpsMock::InvokeEvent` should call `dispatcher->RecordJsonValue` on the returned value.
- In order to do this, `IRpcListEventOps::OnItemChanged` and `IRpcObjectEventOps::InvokeEvent` should return `Value` instead of `RpcEventExceptionMap`.
- Such return values should be serialized and deserialized with JSON.
- But since JSON mock version of them do not do JSON serialization and deserialization, it is JSON `IRpcSerializer` doing them.
- So they can't call `ReadMethodException` and `ReadEventException`, generated ops implementations and those in `WfLibraryRpcWrappers.(h|cpp)` should call them after deserializing return values.
  - Currenty they returns `RpcEventExceptionMap` so return values from `OnItemChanged` and `InvokeEvent` are not involved in `IRpcSerializer` serializing and deserializing, you should add it.
  - `rpcops_IOps_CreateJson` currently call `ReadEventException` after `rpcjson_Deserialize`, which is good.
  - `rpcops_IOps_CreateJson` currently call `ReadMethodException` before `rpcjson_Deserialize`, which is bad.
    - `rpcjson_Deserialize` should be called before `ReadMethodException` so that `ReadMethodException` does not need to handle JSON values.
    - Even when a method returns primitive types, `rpcjson_Deserialize` should be called and then do casting, instead of inlining for example reading `JsonString` for `string`, etc.
  - `rpcops_IOps_Create` looks good, no need to change
- Since `ReadMethodException` and `ReadEventException` is called after JSON representation of exceptions are deserialized to `RpcException` and `RpcEventExceptionMap`, so these two read functions don't handle JSON representations.

`JsonSerializePredefinedTypes` and `JsonDeserializePredefinedTypes` should handle serialization for `RpcException` and `RpcObjectReference`.
- Remove serialization of these two types in generated code.

Generated wrappers need to use generated strong typed version of ops interface for event handling.
- Take `Rpc/Inheritance_EventException` as an example.
- `rpcwrapper_RpcInheritanceEventException__IDerived` calls `rpclistener_RpcInheritanceEventException__IDerived` at the end.
- `rpclistener_RpcInheritanceEventException__IDerived` calls `InvokeEvent` by itself, which is wrong, `ops` should be passed to it, and it calls the strong typed version.
- This will affect test cases in `TestCasesRpc.cpp`.
  - Remove `attachLocalEvents` argument from `RunRpcTestCase`.
    - Since the only possible implementation is only calling `rpclistener_Attach`, `RunRpcTestCase` should just call it by themselves.
    - But when there is no event involved, `rpclistener_Attach` is not generated, so `bool HasEvent` should be added to `RunRpcTestCase` as a new template argument.
    - Therefore you can protect the code in `if constexpr (HasEvent)` so that when `rpclistener_Attach` is not generated there is no C++ compile error.
  - When `VCZH_DEBUG_NO_REFLECTION` is defined, use the JSON version.
  - You need to verify `RunRpcTestCase`, all JSON version functions should only be called when `VCZH_DEBUG_NO_REFLECTION` is defined, there are some other cases.
    - e.g., `oo1`, `oeo1`, `oo2`, `oeo2`, `ops1`, `ops2`.
    - Only when `VCZH_DEBUG_NO_REFLECTION` is defined, `rpcops_IRpcSerializer` can be used, otherwise just don't call `SetSerializer`.
      - If not setting a serializer crash anything this should be fixed.
      - When a serializer is null, just don't call it.
    - `lc1->attachLocalEventsCallback` will need `ops1` and `lc2->attachLocalEventsCallback` will need `ops2`, so these 6 objects are recommended to be created right after `lc1` and `lc2` are created.

## Task 2

`WfLibraryRpcLifecycle.cpp`:
- All `throw Exception` could just be replaced by `CHECK_FAIL`.

`TestCasesRpc.h`:
- Currently the code is different when `VCZH_DEBUG_NO_REFLECTION` is defined.
- You want to split `RunRpcTestCase` into two functions:
  - `RunRpcTestCase_JsonValue`, it performs what should be done when `VCZH_DEBUG_NO_REFLECTION` is defined.
  - `RunRpcTestCase_Flat`, it performs what should be done when `VCZH_DEBUG_NO_REFLECTION` is not defined.
- Therefore you can `#define RunRpcTestCase` to any of the above functions accordingly, so that only one `VCZH_DEBUG_NO_REFLECTION` test is performed.
- Duplicating code in `RunRpcTestCase_JsonValue` and `RunRpcTestCase_Flat` is fine.
- You can put the above functions in `rpc_controller_test`, therefore all `using namespaces` in `RunRpcTestCase` can be placed in `rpc_controller_test` namespace. This is a header file for test only, it is allowed.

# UPDATES

## Task 2

- Split the RPC C++ test harness into `RunRpcTestCase_JsonValue` and `RunRpcTestCase_Flat`, selected by the `RunRpcTestCase` macro under `VCZH_DEBUG_NO_REFLECTION`.
- Moved the harness namespace imports into `vl::rpc_controller_test` and kept JSON-only serializer/ops setup confined to the JSON-value harness.
- Changed the local unknown RPC type id failure in `RegisterService` to `CHECK_FAIL`.
- Kept the duplicate-registration lifecycle failures as `Exception` after verification showed `Rpc/FailDoubleRegistration` intentionally catches them as user-visible RPC/script exceptions; replacing those with `CHECK_FAIL` throws `vl::Error`, bypasses the existing catch path, and crashes the runtime test instead of producing the expected `[exception]` output.

# TEST

Task 1 will be covered by the existing RPC samples and generated RPC wrapper output:

- `Rpc/Inheritance_EventException` verifies generated wrapper listeners route object events through the generated strong typed ops interface, and that event exceptions still propagate after `IRpcObjectEventOps::InvokeEvent` returns `object`.
- Observable-list RPC samples with event exceptions verify `IRpcListEventOps::OnItemChanged` returns a JSON-serializable value and the caller deserializes it before `ReadEventException`.
- Method-return RPC samples with primitive, byref, byval, and exception returns verify JSON caller ops deserialize the returned JSON value before calling `ReadMethodException`.
- Generated JSON values in `Test/TypeScript/JsonValues*` and the TypeScript build verify `RpcException` and `RpcObjectReference` are serialized by the predefined JSON serializer path, not by per-RPC generated struct serializers.
- `TestCasesRpc.cpp` generation verifies `RunRpcTestCase` uses JSON ops only under `VCZH_DEBUG_NO_REFLECTION`, uses flat ops otherwise, and attaches local event listeners only when the generated RPC assembly has events.

Success criteria:

- The solution builds for Debug x64 and Debug Win32.
- Required unit test projects pass in the order defined by `Project.md`.
- `CompilerTest_LoadAndCompile` regenerates RPC test sources without baseline mismatch after expected generated-output updates are accepted.
- TypeScript preparation and `npm run build` pass after JSON value regeneration.
- Generated code contains no generated JSON serializer/deserializer functions for `system::RpcException` or `system::RpcObjectReference`, and JSON caller ops call `rpcjson_Deserialize` before `ReadMethodException` or `ReadEventException`.

# PROPOSALS

- No.1 Route event return values through `Value` and JSON serialization [CONFIRMED]
- No.2 Split RPC test harness and tighten lifecycle failure handling [CONFIRMED]

## No.1 Route event return values through `Value` and JSON serialization

### CODE CHANGE

- Changed `IRpcListEventOps::OnItemChanged` and `IRpcObjectEventOps::InvokeEvent` to return `Value`, with lifecycle callers deserializing the returned value before calling `ReadEventException`.
- Added predefined JSON handling for `RpcObjectReference`, `RpcException`, and generic collection/map values, and removed generated JSON serializers for the RPC predefined types.
- Updated JSON generated ops so all method/event returns go through `rpcjson_Deserialize` before exception checks and primitive casts.
- Updated generated listener/wrapper creation so local event listeners receive the generated strong typed ops object and invoke the generated event method instead of directly calling dispatcher event APIs.
- Updated RPC mocks and test harnesses so JSON-only helpers are only used under `VCZH_DEBUG_NO_REFLECTION`; flat mode does not require a serializer.
- Regenerated RPC C++/Workflow/metadata outputs and TypeScript JSON values.

### VERIFICATION

- `copilotBuild.ps1 -Configuration Debug -Platform x64`: passed.
- `copilotBuild.ps1 -Configuration Debug -Platform Win32`: passed.
- `LibraryTest` x64 and Win32: passed.
- `CompilerTest_GenerateMetadata` x64 and Win32: passed.
- `CompilerTest_LoadAndCompile` x64: passed and regenerated outputs.
- Rebuilt Debug x64 and Win32 after regeneration: passed.
- `RuntimeTest` x64 and Win32: passed.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` x64 and Win32: passed.
- `Test/TypeScript/prepare.ps1`: passed.
- `npm run build` in `Test/TypeScript`: passed.
- `git diff --check -- . ':(exclude)Test/Generated/Workflow32/Assembly.Rpc.*.txt' ':(exclude)Test/Generated/Workflow64/Assembly.Rpc.*.txt'`: passed. The unscoped check reports fixed-column trailing spaces in regenerated Workflow assembly disassembly files.

## No.2 Split RPC test harness and tighten lifecycle failure handling

### CODE CHANGE

- Split `RunRpcTestCase` into `RunRpcTestCase_JsonValue` and `RunRpcTestCase_Flat` in `rpc_controller_test`, and macro-selected the correct implementation based on `VCZH_DEBUG_NO_REFLECTION`.
- Duplicated the setup intentionally so JSON mode creates serializers, JSON object/event ops, JSON strong typed ops, and JSON dispatcher only in that implementation; flat mode creates flat ops and never calls `SetSerializer`.
- Kept local event attachment in each harness behind `if constexpr (HasEvent)` and passed the generated strong typed ops object to `rpclistener_Attach`.
- Replaced the non-catchable unknown-service-type lifecycle failure with `CHECK_FAIL`, while preserving duplicate-registration conflicts as `Exception` because existing RPC behavior depends on catching them.

### VERIFICATION

- `copilotBuild.ps1 -Configuration Debug -Platform x64`: passed.
- `copilotBuild.ps1 -Configuration Debug -Platform Win32`: passed.
- `LibraryTest` x64 and Win32: passed.
- `CompilerTest_GenerateMetadata` x64 and Win32: passed.
- `CompilerTest_LoadAndCompile` x64: passed.
- `RuntimeTest` x64 and Win32: passed.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` x64 and Win32: passed.
- `Test/TypeScript/prepare.ps1`: passed.
- `npm run build` in `Test/TypeScript`: passed.
- `git diff --check`: passed.
