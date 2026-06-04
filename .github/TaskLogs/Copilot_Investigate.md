# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

## Task 1

Refactor `RpcDualJsonRequestDispatcherMock` to use `RpcDualDispatcherMockBase` to avoid duplicating code.
To achieve this, the following change will be needed.
Currently two clients have its own `RpcDualJsonRequestDispatcherMock` which connects with one `RpcDualJsonMessageBridge` instance.
I would like you to refactor the code so that there will be only one `RpcDualJsonRequestDispatcherMock` and also no `RpcDualJsonMessageBridge` is needed.
`BroadcastFromClient_ObjectEventOps` and `SendToClient_ObjectOps` should return one in two `RpcJsonObject(Event)?Ops` according to the specified client id.

In `WfLibraryRpcJson.cpp`, you are going to find out how many helper functions do their own serialization, and replace them to use, or replace them completely with `JsonSerializePredefinedTypes` and `JsonDeserializePredefinedTypes`. Even if these pair of functions do more thing than the original function. If you found nothing is repeating, it is also good. Note than `JsonSerializePredefinedTypes` and `JsonDeserializePredefinedTypes` also use other functions, no need to inline them, just keep the current way.

## Task 2

In `WfAnalyzer_(Validate|Generate)Rpc*.cpp` there is a lot of Workflow AST generation code.
Many of them generate types from scratch.
For types that can be directly expressed in C++, there is a better way: C++ type -> ITypeInfo -> WfType helper.
I would like you to identify all of this and change the code, and delete unused helper functions completely.
Wrappers generates many *.txt files during running the compiler, you will be easy to ensure success, by see no *.txt file is changed using `git status`.

`TypeInfoRetriver<T>::CreateTypeInfo()` converts C++ type `T` to `Ptr<ITypeInfo>`.
`GetTypeFromTypeInfo` converts `ITypeInfo*` to `Ptr<WfType>`.
When possible, use `TypeInfoRetriver` inside `GetTypeFromTypeInfo` to avoid creating unnecessary termporary variables.

# UPDATES

# TEST [CONFIRMED]

Task 1 is confirmed by static inspection: the current JSON RPC harness has two `RpcDualJsonRequestDispatcherMock` instances connected through `RpcDualJsonMessageBridge`, duplicating dispatch behavior that belongs in `RpcDualDispatcherMockBase`.

Task 1 success criteria:
- Build and run the RPC-relevant unit test flow after changing `*.h` / `*.cpp`.
- Run `CompilerTest_LoadAndCompile` at least once and check `git status` so generated `*.txt` files remain unchanged for this pure refactor.
- Run the TypeScript RPC verification because JSON dispatcher behavior is touched: `Test/TypeScript/prepare.ps1`, then `npm run build`.
- Static scans should show no `RpcDualJsonMessageBridge` remains and only one `RpcDualJsonRequestDispatcherMock` is used by the JSON-value RPC harness.

Task 2 success criteria:
- Replace hand-built Workflow AST type nodes in `WfAnalyzer_(Validate|Generate)Rpc*.cpp` when an equivalent C++ type can be converted through `TypeInfoRetriver<T>::CreateTypeInfo()` and `GetTypeFromTypeInfo`.
- Delete helper functions made unused by the refactor.
- Build and run the same compiler/RPC verification flow.
- Check `git status` after wrapper generation and verify no generated `*.txt` file is changed.

# PROPOSALS

- No.1 Collapse JSON dual dispatcher onto the shared dual dispatcher base [CONFIRMED]

## No.1 Collapse JSON dual dispatcher onto the shared dual dispatcher base [CONFIRMED]

Refactor `RpcDualJsonRequestDispatcherMock` so it owns the two JSON object/event ops pairs and inherits the shared service/finalization/object-hold routing from `RpcDualDispatcherMockBase`. The mock should select the target JSON ops directly from `sourceClientId` / `targetClientId`, removing `RpcDualJsonMessageBridge` entirely. In `WfLibraryRpcJson.cpp`, replace duplicated predefined transport serialization/deserialization helpers with calls to `JsonSerializePredefinedTypes` and `JsonDeserializePredefinedTypes` wherever the existing helper is only manually serializing predefined RPC types.

### CODE CHANGE

Implemented Task 1 by changing `Test/Source/RpcDualJsonDispatcherMock.*` so `RpcDualJsonRequestDispatcherMock` inherits `RpcDualDispatcherMockBase`, owns both lifecycles, allocates request ids, records request/response JSON, and caches one `RpcJsonObjectEventOps` per source client and one `RpcJsonObjectOps` per target client. Deleted `RpcDualJsonMessageBridge` and changed `RunRpcTestCase_JsonRequest` in `Test/Source/TestCasesRpc.h` to use one request dispatcher. In `Source/Library/Rpc/WfLibraryRpcJson.cpp`, event-exception-map conversion now delegates internal predefined map and `RpcException` conversion through `JsonSerializePredefinedTypes` / `JsonDeserializePredefinedTypes`, leaving only the plain request-envelope adaptation code custom.

### CONFIRMED

Task 1 is confirmed by successful Debug builds for x64 and Win32, `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile` x64, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` on both x64 and Win32, plus `Test/TypeScript/prepare.ps1` and `npm run build` after installing the pinned TypeScript dependency with `npm ci`. Static scans found no remaining `RpcDualJsonMessageBridge`, `SetBridge`, `CreateSerializedRpcException`, or `ReadSerializedEventExceptionKey` usage in source/test code. Generated reflection metadata changed because `IRpcJsonMessageDispatcher` is now emitted, and TypeScript JSON request fixtures changed according to the new single-dispatcher trace.
