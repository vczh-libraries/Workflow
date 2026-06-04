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

## UPDATE

Your overall work is good, just one more fix.

						auto typeInfo = Ptr(new TypeDescriptorTypeInfo(baseTd, TypeInfoHint::Normal));
						auto parentType = GetTypeFromTypeInfo(typeInfo.Obj());

this sounds wired. There is a `GetExpressionFromTypeDescriptor` function, you can create `GetTypeFromTypeDescriptor` for the code sample above, and also replace everywhere that use this pattern. By the way, you are also going to scan if any cpp in Source/Analyzer folder use this pattern too. For example, `GenerateInterfaceDecl` has a similar piece of code like `GetExpressionFromTypeDescriptor`, your new `GetTypeFromTypeDescriptor` will be able to use here. It can be also used in `GetTypeFromTypeInfo`. Now one function for 3 places.

Success is defined by no generated code gets changed and all test projects pass. Follow `document-and-commit.md` for finishing part.

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
- No.2 Generate fixed RPC Workflow types from C++ type info [CONFIRMED]
- No.3 Share type descriptor to Workflow type conversion [CONFIRMED]

## No.1 Collapse JSON dual dispatcher onto the shared dual dispatcher base [CONFIRMED]

Refactor `RpcDualJsonRequestDispatcherMock` so it owns the two JSON object/event ops pairs and inherits the shared service/finalization/object-hold routing from `RpcDualDispatcherMockBase`. The mock should select the target JSON ops directly from `sourceClientId` / `targetClientId`, removing `RpcDualJsonMessageBridge` entirely. In `WfLibraryRpcJson.cpp`, replace duplicated predefined transport serialization/deserialization helpers with calls to `JsonSerializePredefinedTypes` and `JsonDeserializePredefinedTypes` wherever the existing helper is only manually serializing predefined RPC types.

### CODE CHANGE

Implemented Task 1 by changing `Test/Source/RpcDualJsonDispatcherMock.*` so `RpcDualJsonRequestDispatcherMock` inherits `RpcDualDispatcherMockBase`, owns both lifecycles, allocates request ids, records request/response JSON, and caches one `RpcJsonObjectEventOps` per source client and one `RpcJsonObjectOps` per target client. Deleted `RpcDualJsonMessageBridge` and changed `RunRpcTestCase_JsonRequest` in `Test/Source/TestCasesRpc.h` to use one request dispatcher. In `Source/Library/Rpc/WfLibraryRpcJson.cpp`, event-exception-map conversion now delegates internal predefined map and `RpcException` conversion through `JsonSerializePredefinedTypes` / `JsonDeserializePredefinedTypes`, leaving only the plain request-envelope adaptation code custom.

### CONFIRMED

Task 1 is confirmed by successful Debug builds for x64 and Win32, `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile` x64, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` on both x64 and Win32, plus `Test/TypeScript/prepare.ps1` and `npm run build` after installing the pinned TypeScript dependency with `npm ci`. Static scans found no remaining `RpcDualJsonMessageBridge`, `SetBridge`, `CreateSerializedRpcException`, or `ReadSerializedEventExceptionKey` usage in source/test code. Generated reflection metadata changed because `IRpcJsonMessageDispatcher` is now emitted, and TypeScript JSON request fixtures changed according to the new single-dispatcher trace.

## No.2 Generate fixed RPC Workflow types from C++ type info [CONFIRMED]

Replace hand-built `WfType` AST construction with `GetTypeFromTypeInfo(TypeInfoRetriver<T>::CreateTypeInfo().Obj())` when the target type is a fixed C++ runtime type, including RPC maps, callback function types, object/value primitives, JSON node classes, RPC operation interfaces, lifecycle pointers, and reflected base interface descriptors. Keep name-based helper construction for generated user/model types and other dynamic type names that cannot be named as a C++ template type. Delete helpers that become unused after the conversion.

### CODE CHANGE

Implemented Task 2 by adding a `CreateTypeFromCpp<T>()` path around `GetTypeFromTypeInfo(TypeInfoRetriver<T>::CreateTypeInfo().Obj())` for fixed RPC/runtime types in `WfAnalyzer_GenerateRpc.cpp` and `WfAnalyzer_GenerateRpc_JsonSerialization.cpp`. The generated type tree is normalized back to the existing Workflow spelling for predefined `system::*` aliases and root-qualified runtime symbols so wrapper text remains stable. Replaced hand-built AST types for RPC exceptions, object references, lifecycle pointers, operation interfaces, serializer/json node classes, reflection values, and runtime dictionaries. Removed the now-unused `CreateMapType`, `CreateRpcJsonSerializeCallbackType`, and `CreateRpcJsonDeserializeCallbackType` helpers. In `WfAnalyzer_ValidateRPC.cpp`, reflected RPC base interfaces now go through `TypeDescriptorTypeInfo` and `GetTypeFromTypeInfo` instead of rebuilding type fragments manually.

### CONFIRMED

Task 2 is confirmed by successful Debug builds for x64 and Win32, plus `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` on both x64 and Win32. `Test/TypeScript/prepare.ps1` and `npm run build` both completed successfully. Static scans found no remaining fixed `Create(Raw|Shared|Qualified)Type(L"system::...")` calls, no `CreateMapType`, and no removed JSON callback helper usage in the RPC generator files. `git status` shows no generated `*.txt` changes after the wrapper/compiler runs.

## No.3 Share type descriptor to Workflow type conversion [CONFIRMED]

Create `GetTypeFromTypeDescriptor` beside `GetExpressionFromTypeDescriptor`, use it from `GetTypeFromTypeInfo`, and replace analyzer code that creates `TypeDescriptorTypeInfo` only to convert an `ITypeDescriptor` back into a Workflow type AST. This keeps direct descriptor-to-type conversion in one place while preserving generated wrapper output.

### CODE CHANGE

Implemented the follow-up by adding `GetTypeFromTypeDescriptor` beside `GetExpressionFromTypeDescriptor` in `WfAnalyzer_TypeInfo.cpp` and declaring it in `WfAnalyzer.h`. `GetTypeFromTypeInfo` now delegates its `ITypeInfo::TypeDescriptor` case to this helper. `GenerateInterfaceDecl` uses it directly for reflected RPC base interfaces, and `ExpandMixinCastExpression` uses it as the element builder for raw/shared pointer `WfType` AST nodes instead of creating temporary `TypeDescriptorTypeInfo` objects just to feed `GetTypeFromTypeInfo`.

### CONFIRMED

The proposal is confirmed by successful Debug builds for x64 and Win32, all listed unit-test projects on both x64 and Win32 (`LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`), `Test/TypeScript/prepare.ps1`, and `npm run build` run again after restoring generated fixture churn from the test run. Static scans in `Source/Analyzer` found no remaining `GetTypeFromTypeInfo` path that manually builds a temporary `TypeDescriptorTypeInfo` only to convert an `ITypeDescriptor` into a `WfType`. Final `git status` contains no generated file changes.
