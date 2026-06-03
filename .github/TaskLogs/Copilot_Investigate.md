# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

## Task 1

This is a pure coding task, you can finish the task just after the solution compiles.

You are going to enable reflection on `IRpcJsonMessageDispatcher` and implement:
- `RpcJsonObjectOps`
- `RpcJsonObjectEventOps`
They translate every methods of the ops interfaces under the schema described in `Rpc.d.ts`.
`AllocateRequestId` will be called to make a new request id.
Each `Translate` static method does the reverse thing, taking a JSON request and call the method of a ops interface, returning a response.
`ReadRequestId` and `WriteRequestId` operates request id on a json message (request or response). `WriteRequestId` will create a request id if `rpcRequestId` does not exist.
One thing need to modify in `Rpd.d.ts`, each request and response should have a `rpcRequestId:number` field.

## Task 2

This is a coding and testing task.

Now we are going to use constructions from `Task 1` to update `CppTest_Metaonly`.
A new function `RunRpcTestCase_JsonRequest` will be created for `CppTest_Metaonly` in `TestCasesRpc.h`.
New mock constructions will be added to `RpcDualJsonDispatcherMock.(h|cpp)`.
Basically the `RunRpcTestCase_JsonRequest` does things like this:
- In `RunRpcTestCase_(Flat|JsonValue)` functions one dispatcher connects two lifecycles. But here we use different dispatchers.
We can call it `RpcDualJsonRequestDispatcherMock`, in order to make the code simpler, it implements both `IRpcDispatcher` and `IRpcJsonMessageDispatcher`.

Unlike `_JsonValue`, now we doesn't need 3 mocks for ops, we needs one mock just for `IRpcJsonMessageDispatcher` to record complete requests and responses.
Two `IRpcDispatcher` mocks will their own `IRpcJsonMessageDispatcher` mocks, done by `RpcDualJsonRequestDispatcherMock` naturally.
Since there are only two side, so both `SendToClient` and `BroadcastFromClient` does the same thing, aka sending requests to another client.
So these functions of the dispatcher could just return `RpcJsonObjec(Event)?Ops`, which then call `IRpcJsonMessageDispatcher`.

We will need a `RpcDualJsonMessageBridge` to connect two different `RpcDualJsonRequestDispatcherMock`.
Its constructor receives two dispatchers and store the bridge in them just like the pattern used by `RpcDualJsonRequestDispatcherMock`.
When `OnJsonRequest` is called, it examine the `rpcMethod` and check if it begins with `IObjectOps_` or `IObjectEventOps_`.
And then call the correct `Translate` function on ops interfaces from the other side.
We don't care about recording json requests and responses yet, in this task we should make `CppTest_Metaonly` works.

The following header needs to be added to `RpcDualJsonDispatcherMock.(h|cpp)` to specify constructions for `_JsonValue` and `_JsonRequest`.
```C++
/***********************************************************************
* blahblahblah
***********************************************************************/
```

FYI request id is designed to allow all requests and responses running nested and async while ops interface methods can still block.
Serious engineering will run them in a message loop.
But here we run everything in one single thread, so we just create incremental request id for each occurance in each test case.
We will not check request id values and do stuff accordingly.
The goal is to test if JSON request and response serialize correctly, not try to mess with async stuff.
So I would like to see each response use the request id in the request, but each request uses different request ids, starting from 1 in each test case.

## Task 3

This is a coding and testing task.

Now we should record json requests and responses in `RpcDualJsonMessageBridge` and save them to `REPO-ROOT/Test/TypeScript/JsonRequest(32|64)` just like the pattern used by `RpcDualJsonDispatcherMock`.
JSON are stores in an array in each .ts files just like how `REPO-ROOT/Test/TypeScript/JsonValue(32|64)` files are doing.
But its type becomes `Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>`.
`Request<T>` and `Response<T>` are from `Rpc.d.ts`.
New .ts files should be recognized in `tsconfig.json`.
Until now we can say we are doing JSON Rpc in our expected schema:
- This is our primary goal in these tasks.
- We define success by both `CppTest_Metadata` and `REPO-ROOT/Test/TypeScript` pass.
- Sample some files and make sure this actually happens from `Task 2`:
  - > So I would like to see each response use the request id in the request, but each request uses different request ids, starting from 1 in each test case.

# UPDATES

- Implemented JSON request-envelope ops and reflected `IRpcJsonMessageDispatcher`.
- Added the Metaonly JsonRequest dispatcher route and verified it with `CppTest_Metaonly`.
- Added JsonRequest32/64 transcript generation and TypeScript coverage.
- Audited all generated JsonRequest files for request id pairing and per-file monotonic request ids.

# TEST

Task 1 succeeds when `IRpcJsonMessageDispatcher` reflection and the JSON request/response ops helpers compile, and `Rpc.d.ts` request/response schema includes `rpcRequestId:number`.

Task 2 succeeds when `CppTest_Metaonly` compiles and runs with the new JSON request dispatcher path.

Task 3 succeeds when JSON request/response transcript files are generated under `Test/TypeScript/JsonRequest32` and `Test/TypeScript/JsonRequest64`, included by `tsconfig.json`, type-check against `Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>`, and sampled output shows request ids starting from 1 per test case with matching response ids.

Final verification succeeds when the related C++ unit tests pass and the TypeScript package passes `prepare.ps1` and `npm run build`.

# PROPOSALS

- No.1 ADD JSON REQUEST ENVELOPE OPS

## No.1 ADD JSON REQUEST ENVELOPE OPS

Implement `RpcJsonObjectOps` and `RpcJsonObjectEventOps` as JSON-envelope adapters around `IRpcJsonMessageDispatcher`: client-side methods build `Rpc.d.ts` request objects, allocate/write `rpcRequestId`, send them through `OnJsonRequest`, and return the payload in the same `JsonNode`-boxed shape expected by generated JSON RPC wrappers. Static `Translate` methods parse the request envelopes, call the generated JSON ops on the receiving side, and build response envelopes reusing the request id. Add reflection metadata/proxy support for `IRpcJsonMessageDispatcher` and update `Rpc.d.ts` request/response interfaces with `rpcRequestId:number`.

### CODE CHANGE

Updated `Source/Library/Rpc/WfLibraryRpcJson.h` and `Source/Library/Rpc/WfLibraryRpcJson.cpp`:
- Added state and method overrides to `RpcJsonObjectOps` and `RpcJsonObjectEventOps`.
- Implemented JSON request construction for object method calls, end-invoke, object hold, and object events.
- Implemented lifecycle-backed `RegisterService` request construction and translation using serialized `RpcObjectReference` values.
- Implemented response decoding back into the `Value` shapes expected by generated JSON RPC wrappers, including byval return wrappers.
- Implemented static `Translate` functions for object and event requests, including response envelope creation and event exception map normalization.
- Implemented `ReadRequestId` and `WriteRequestId`.

Updated `Source/Library/WfLibraryReflection.h` and `Source/Library/WfLibraryReflection.cpp`:
- Added `IRpcJsonMessageDispatcher` to the reflected type list.
- Added a metadata-only interface proxy for `AllocateRequestId` and `OnJsonRequest`.
- Registered interface metadata and type name `system::IRpcJsonMessageDispatcher`.

Updated `Test/TypeScript/Rpc.d.ts`:
- Added `rpcRequestId: number` to every request and response interface.

Updated `Test/Source/RpcDualJsonDispatcherMock.h` and `Test/Source/RpcDualJsonDispatcherMock.cpp`:
- Split the file sections into `Json Value` and `Json Request`.
- Added `RpcDualJsonRequestDispatcherMock`, implementing both `IRpcDispatcher` and `IRpcJsonMessageDispatcher`.
- Added `RpcDualJsonMessageBridge` to connect two request dispatchers, route `IObjectOps_` and `IObjectEventOps_` requests to the opposite lifecycle, and share service registration and request id allocation across the test case.
- Added lifecycle-aware JSON object-op translation and bridge service reference ownership for serialized `RegisterService` requests.
- Added bridge-level request/response recording and `DumpJsonRequests`.

Updated `Test/Source/TestCasesRpc.h`:
- Added `RunRpcTestCase_JsonRequest`.
- Routed `CppTest_Metaonly` to `RunRpcTestCase_JsonRequest`.
- Dumped JsonRequest transcripts after each RPC test case.

Updated `Test/Source/Helper.h` and `Test/Source/Helper.cpp`:
- Added `GetJsonRequestOutputPath`.
- Added a helper-only path macro so `CppTest_Metaonly` can link the JSON output path helpers without pulling in the full helper implementation used by other tests.

Updated `Test/UnitTest/CppTest_Metaonly/CppTest_Metaonly.vcxproj` and `.filters`:
- Added `RpcDualJsonDispatcherMock.cpp/h` so Metaonly links the request dispatcher and bridge implementations.
- Added `Helper.cpp` with the helper-only path macro for JsonRequest output paths.

Updated `Test/TypeScript/prepare.ps1` and `Test/TypeScript/tsconfig.json`:
- Added `JsonRequest32` and `JsonRequest64` to TypeScript preparation and type-check coverage.

Generated `Test/TypeScript/JsonRequest32` and `Test/TypeScript/JsonRequest64`:
- Added 124 JsonRequest transcript files per architecture.
- Each transcript exports `(Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>)[]`.

### CONFIRMED

- `copilotBuild.ps1 -Configuration Debug -Platform x64` passed.
- `copilotBuild.ps1 -Configuration Debug -Platform Win32` passed.
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Metaonly` passed for x64.
- `copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Metaonly -Configuration Debug -Platform Win32` passed.
- `Test/TypeScript/prepare.ps1` passed.
- `npm run build` in `Test/TypeScript` passed.
- Sampled `JsonRequest64/JsonRequest_PrimitiveTypes.ts` and `JsonRequest64/JsonRequest_Event.ts`; requests start at id 1 and responses reuse request ids, including nested event requests.
- A Node audit checked all 248 JsonRequest files and confirmed every request id has exactly one request and one response, and request ids increment from 1 per file.
