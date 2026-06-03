investigate repro

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
