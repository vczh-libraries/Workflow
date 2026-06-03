# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

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

Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.
The last work of refactoring for letting `IRpcList(Event)?Ops` being redirection of `IRpcObject(Event)?Ops` didn't update the document.
You are going to read documents and figure out if anything mis-aligned.

# UPDATES

# TEST [CONFIRMED]

Use the existing RPC tests and downstream TypeScript validation as behavioral coverage:
- Build and run `LibraryTest`, because `Test/UnitTest/LibraryTest/TestLibraryRpcByval.cpp` exercises byval/byref collection RPC flows, observable-list item changes, and the dispatcher/lifecycle helpers.
- Build and run generated RPC coverage through `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`, because generated wrappers call the same predefined RPC operations and JSON paths.
- Run `Test/TypeScript/prepare.ps1` followed by `npm run build`, because `Test/TypeScript/Rpc.d.ts` documents the generic JSON RPC envelope and is used by the JSON validation package.

Additional success criteria:
- `IRpcDispatcher` no longer declares or implements `BroadcastFromClient_ListEventOps` or `SendToClient_ListOps`.
- Code under `Source/Library/Rpc` no longer calls these removed dispatcher methods.
- `BoxRpcObjectReference`, `BoxRpcException`, `BoxRpcEventExceptionMap`, `UnboxRpcEventExceptionMap`, `BoxPrimitiveArgument`, `UnboxPrimitiveArgument`, and `CreateRpcEventExceptionMap` are removed from declarations and implementations, with call sites using `BoxValue` and `UnboxValue`.
- Helper functions declared in `WfLibraryRpc.h` are implemented in `WfLibraryRpc.cpp` rather than `WfLibraryRpcWrapperes.cpp`.
- `RpcCallerListOps` methods do not call `ReadMethodResult` after `InvokeListMethod`, because `InvokeListMethod` already reads method results.
- RPC documentation no longer describes list operations as separate `IRpcDispatcher` request envelopes after list ops became redirections through object ops.

# PROPOSALS

- No.1 Remove list-specific dispatcher transport and collapse RPC value helpers [CONFIRMED]

## No.1 Remove list-specific dispatcher transport and collapse RPC value helpers

Delete the list-specific dispatcher methods because list operation interfaces are now caller/callee adapters over object operations. Runtime collection wrappers should create `RpcCallerListOps` from `IRpcDispatcher::SendToClient_ObjectOps`, and observable-list native event forwarding should create `RpcCallerListEventOps` from `IRpcDispatcher::BroadcastFromClient_ObjectEventOps`. Remove specialized boxing helper functions in favor of `BoxValue` and `UnboxValue`, and keep the public helper implementations in `WfLibraryRpc.cpp`.

### CODE CHANGE

- Removed `IRpcDispatcher::BroadcastFromClient_ListEventOps` and `IRpcDispatcher::SendToClient_ListOps` from the interface, reflection registration, and test dispatcher mocks.
- Updated runtime byref collection wrappers to synthesize `RpcCallerListOps` from remote object ops.
- Updated observable-list event forwarding to route through object-event ops with `RpcCallerListEventOps`.
- Moved public RPC helper implementations declared in `WfLibraryRpc.h` from `WfLibraryRpcWrappers.cpp` to `WfLibraryRpc.cpp`.
- Removed `BoxRpcObjectReference`, `BoxRpcException`, `BoxRpcEventExceptionMap`, `UnboxRpcEventExceptionMap`, `BoxPrimitiveArgument`, `UnboxPrimitiveArgument`, and `CreateRpcEventExceptionMap`, replacing their call sites with `BoxValue`, `UnboxValue`, and `IValueDictionary::Create`.
- Changed `InvokeListMethod` to return the checked/deserialized method result and removed duplicate `ReadMethodResult` calls from `RpcCallerListOps`.
- Updated RPC documentation to describe list ops and list events as local adapters transported through object ops and object-event ops.

### CONFIRMED

- Debug x64 build passed.
- Debug Win32 build passed.
- `LibraryTest` passed on x64 and Win32.
- `CompilerTest_GenerateMetadata` passed on x64 and Win32.
- `CompilerTest_LoadAndCompile` passed on x64.
- `RuntimeTest` passed on x64 and Win32.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` passed on x64 and Win32.
- `Test/TypeScript/prepare.ps1` passed.
- `npm run build` in `Test/TypeScript` passed.
- `..\Tools\Tools\Build.ps1 Workflow` completed without a script error after running Release unit tests, TypeScript build, and release generation.
