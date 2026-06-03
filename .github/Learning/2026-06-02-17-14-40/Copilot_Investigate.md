# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

The main goal of the work is to implement `IRpcListOps` and `IRpcListEventOps` based on `IRpcObjectOps` and `IRpcObjectEventOps`.
- `IRpcObjectOps` and `IRpcObjectEventOps` are basically generated along with Workflow interfaces marking with `@rpc:Interface`.
- `IRpcListOps` and `IRpcListEventOps` are for reflectable container interfaces `IValue*`.
- It ends up that, `IRpcListEventOps::OnItemChanged` is copying `IRpcListEventOps::InvokeEvent`, `IRpcListOps::*` is copying `IRpcObjectOps::InvokeEvent` and not all features are finished. Keeping special implementations for list ops are a waste of resource.

Here comes up with the solution: keeping two list ops interface but redirect everything to object ops:
- Just like `RpcTypeId_IValue*`, you need to predefine method ids for `IRpcListObs::*` which are mapping to all `IValue*::*` calls, and predefine the event id for `IValueObservableList::ItemChanged` event.
- There are `RpcCalleeListOps` and `RpcCalleeListEventBridge`:
  - Keep `RpcCalleeListOps`.
  - Rename `RpcCalleeListEventBridge` to `RpcCalleeListEventOps`.
  - Add `RpcCalleeObjectOpsForList` and `RpcCalleeObjectEventOpsForList`:
    - They check the method id or event id, when it is for list ops, call the list ops, otherwise forward the call to the underlying object ops.
    - So `RpcCalleeObjectOpsForList` needs a pointer of `RpcCalleeListOps` and a pointer of a real underlying `IRpcObjectOps`.
    - The same to `RpcCalleeObjectEventOpsForList`.
    - Anywhere when `RpcCalleeListOps` and `RpcCalleeListEventBridge` are installed, you should create `RpcCalleeObjectOpsForList` and `RpcCalleeObjectEventOpsForList` so that they could work.
- Prepare `RpcCallerListOps` and `RpcCallerListEventOps` in the same file:
  - They redirect all calls to `IRpcObjectOps` and `IRpcEventOps` at the caller side.
  - Caller side `IRpcListOps` and `IRpcListEventOps` implementations are currently created by `RpcDualDispatcherMock` and `RpcDualJsonDispatcherMock` for test cases. All these will no longer needed, they should be replaced by `RpcCallerListOps` and `RpcCallerListEventOps`. I guess `WrapOps` function will also no longer needed. Try to remove it and see what happens.
- Since now list ops only do redirection to and from object ops, there should be no more other list ops implementations other than the callee and the caller ones.
- `REPO-ROOT:/Test/TypeScript/Rpc.d.ts` has constructions for list ops, they are also no longer needed:
  - By remember to copy all predefined ids for list type/method/event there.
- Keep the code clean:
  - Besides of following the instructions for better C++ coding guidelines, when you are doing changes, you should always think about, if any construction no long needed or no longer making sense due to the change, and remove the proactively. I don't like to see redirections only to exist after refactoring. You should take risk to refactor unnecessary code even it could break the test, just fix them afterwards.

# UPDATES

# TEST [CONFIRMED]

Use the existing RPC collection tests in `Test/UnitTest/LibraryTest/TestLibraryRpcByval.cpp` and the RPC execution helpers in `Test/Source/TestCasesRpc.h` as the behavioral coverage. These tests already exercise byref arrays, lists, dictionaries, observable-list `ItemChanged`, and enumerator lifetime through two lifecycles and a dispatcher. They should continue to pass when list operations are redirected through object operations.

Additional success criteria:
- All `RpcByref*` wrappers still work for flat and JSON dispatchers.
- List method calls are transported as `IRpcObjectOps::InvokeMethod` calls using predefined list method ids.
- Observable-list item changes are transported as `IRpcObjectEventOps::InvokeEvent` calls using a predefined list event id.
- The test dispatcher mocks no longer contain separate list ops wrappers; they reuse object ops instrumentation.
- `Test/TypeScript/Rpc.d.ts` no longer declares separate list operation request and response envelopes, and instead exposes predefined type, method, and event ids.

# PROPOSALS

- No.1 Redirect list ops through object ops

## No.1 Redirect list ops through object ops

Introduce predefined negative ids for the reflectable `IValue*` container methods and `IValueObservableList::ItemChanged`. Keep `RpcCalleeListOps` as the only callee implementation of actual container behavior, rename `RpcCalleeListEventBridge` to `RpcCalleeListEventOps`, and add object-op adapters that intercept predefined list ids before delegating all other ids to the generated object ops. Add caller-side list adapters that implement `IRpcListOps` and `IRpcListEventOps` by calling `IRpcObjectOps::InvokeMethod` and `IRpcObjectEventOps::InvokeEvent`. Update lifecycle wiring, test mocks, and TypeScript declarations so list traffic uses object/event ops as the single transport path.

### CODE CHANGE

- Added predefined method ids for all list/enumerable/dictionary operations and a predefined event id for `IValueObservableList::ItemChanged` in `Source/Library/Rpc/WfLibraryRpc.h`.
- Kept `RpcCalleeListOps` as the callee implementation of real container behavior and renamed `RpcCalleeListEventBridge` to `RpcCalleeListEventOps`.
- Added `RpcCalleeObjectOpsForList` and `RpcCalleeObjectEventOpsForList` to intercept predefined list method/event ids and forward all other ids to generated object/event ops.
- Added `RpcCallerListOps` and `RpcCallerListEventOps` so caller-side list APIs are transported through `IRpcObjectOps::InvokeMethod` and `IRpcObjectEventOps::InvokeEvent`.
- Removed the dedicated list mock wrappers and shared `WrapOps` helpers from `RpcDualDispatcherMock` and `RpcDualJsonDispatcherMock`; list ops now reuse object ops instrumentation.
- Updated RPC test lifecycle registration to install the object/event adapters wherever callee list ops are installed.
- Removed list operation request/response envelopes from `Test/TypeScript/Rpc.d.ts` and exposed the predefined list type/method/event ids there.

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
- `..\Tools\Tools\Build.ps1 Workflow` passed.
