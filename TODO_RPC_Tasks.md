investigate repro

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.

Since all `IRpcList(Event)?Ops` implementations are redirection of `IRpcObject(Event)?Ops`,
so there should not be any necessity of invoking `IRpcDispatcher`'s `BroadcastFromClient_ListEventOps` and `SendToClient_ListOps`.
Delete these two methods from this interface and do all clean ups.

Many helper functions defined in `WfLibraryRpc.h` are implemented in `WfLibraryRpcWrapperes.cpp`.
Move them to `WfLibraryRpc.cpp`.
Delete these functions and replace them by `BoxValue` and `UnboxValue`:
- `BoxRpcObjectReference`
- `BoxRpcException`
- `BoxRpcEventExceptionMap`
- `UnboxRpcEventExceptionMap`
- `BoxPrimitiveArgument`
- `UnboxPrimitiveArgument`
`BoxValue` and `UnboxValue` already hanelds nullptr so you don't need to repeat it again. Scan through all files under `Source/Library/Rpc` and find out if there is any necessary nullptr test, remove them as well.
Delete `CreateRpcEventExceptionMap` as it is just a renaming.

`InvokeListMethod` already calls `ReadMethodResult`, so all RpcCallerListOps methods calling `ReadMethodResult` again is unnecessary, remove them.

Delete the following three private helper classes in `Test/Source/RpcDualLifecycleMock.cpp`:
- `RpcBroadcastObjectEventOpsMock`
- `RpcObjectEventOpsMock`
- `RpcObjectOpsMock`

Keep `RpcDualDispatcherMockBase`, `RpcDualDispatcherMock`, and `RpcDualJsonDispatcherMock`.
Only simplify their internal routing:
- `RpcDualDispatcherMockBase::BroadcastFromClient_ObjectEventOps` can directly return the other lifecycle's `GetController()->GetObjectEventOps()`.
- `RpcDualDispatcherMockBase::SendToClient_ObjectOps` already directly redirects to the target lifecycle and should stay that simple.
- `RpcDualDispatcherMock` no longer needs private cached wrapper fields or overrides if it becomes identical to the base dispatcher.
- `RpcDualJsonDispatcherMock` still needs its JSON-recording wrappers because `DumpJsonValues` feeds `Test/TypeScript/JsonValues32` and `JsonValues64`, and `tsconfig.json` type-checks those files.

I am confident this cleanup is correct because all current dual dispatcher usages construct exactly two lifecycles, so broadcast has only one possible target: the other client.
The existing broadcast helper already adds only `GetOtherLifecycle(selfClientId)->GetController()->GetObjectEventOps()`, so its multi-target merge path is unused in this framework.
The flat method/event wrapper helpers only forward calls and call `ReadMethodException` or `ReadEventException`, while generated RPC caller code and list wrappers already perform exception decoding after dispatcher calls.
Do not remove the dispatcher service registry, finalization, or object hold/unhold routing behavior.

Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.
The last work of refactoring for letting `IRpcList(Event)?Ops` being redirection of `IRpcObject(Event)?Ops` didn't update the document.
You are going to read documents and figure out if anything mis-aligned.
