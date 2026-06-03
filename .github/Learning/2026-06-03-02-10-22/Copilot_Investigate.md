# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.

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

# UPDATES

# TEST [CONFIRMED]

Confirm by static inspection that the three helper classes are removed, `RpcDualDispatcherMock` remains available, `RpcDualJsonDispatcherMock` still wraps operations for JSON recording, and service registry/finalization/object hold routing remain in place.

Build and run the relevant Workflow unit-test path after changing `Test/Source/RpcDualLifecycleMock.cpp`, including `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` for both Win32 and x64 where applicable. Then run `Test/TypeScript/prepare.ps1` and `npm run build` to verify the generated JSON value files remain type-checkable.

Success means all required C++ unit tests and the TypeScript build pass without errors, and no generated JSON-recording behavior is removed from `RpcDualJsonDispatcherMock`.

# PROPOSALS

- No.1 SIMPLIFY DUAL DISPATCHER ROUTING [CONFIRMED]

## No.1 SIMPLIFY DUAL DISPATCHER ROUTING

Remove the private flat RPC operation helper wrappers from `Test/Source/RpcDualLifecycleMock.cpp`. Make `RpcDualDispatcherMockBase::BroadcastFromClient_ObjectEventOps` directly return the other lifecycle controller's event ops. Remove the base broadcast cache because two-lifecycle dual dispatcher usage has only one broadcast target. Reduce `RpcDualDispatcherMock` to a derived class with only its constructor, because it will behave identically to `RpcDualDispatcherMockBase`. Keep the service registry, finalization ordering, and object hold/unhold routing unchanged. Leave `RpcDualJsonDispatcherMock` wrappers intact so `DumpJsonValues` continues to record JSON values for the TypeScript checks.

### CODE CHANGE

Updated `Test/Source/RpcDualLifecycleMock.cpp` and `Test/Source/RpcDualLifecycleMock.h`:
- Removed `RpcBroadcastObjectEventOpsMock`, `RpcObjectEventOpsMock`, and `RpcObjectOpsMock`.
- Removed the non-JSON broadcast/object ops caches from `RpcDualDispatcherMockBase` and `RpcDualDispatcherMock`.
- Changed `RpcDualDispatcherMockBase::BroadcastFromClient_ObjectEventOps` to directly return `GetOtherLifecycle(selfClientId)->GetController()->GetObjectEventOps()`.
- Left `RpcDualDispatcherMockBase::SendToClient_ObjectOps`, service registry, finalization order, and object hold/unhold routing unchanged.
- Left `RpcDualJsonDispatcherMock` wrappers unchanged, preserving JSON value recording for `JsonValues32` and `JsonValues64`.

### CONFIRMED

Static inspection confirmed the three helper classes and stale cached non-JSON dispatcher fields are gone, while `RpcDualDispatcherMockBase`, `RpcDualDispatcherMock`, and `RpcDualJsonDispatcherMock` remain. The JSON dispatcher still wraps `BroadcastFromClient_ObjectEventOps` and `SendToClient_ObjectOps` with `RpcJsonObjectEventOpsMock` and `RpcJsonObjectOpsMock`, so `DumpJsonValues` still receives recorded JSON arguments and return values.

Verification passed:
- `copilotBuild.ps1 -Configuration Debug -Platform Win32`: build succeeded with 0 warnings and 0 errors.
- `copilotBuild.ps1 -Configuration Debug -Platform x64`: build succeeded with 0 warnings and 0 errors.
- `LibraryTest` Win32 and x64: 2/2 files, 14/14 cases.
- `CompilerTest_GenerateMetadata` Win32 and x64: 1/1 files, 2/2 cases.
- `CompilerTest_LoadAndCompile` x64: both internal phases reported 6/6 files and 709/709 cases.
- `RuntimeTest` Win32 and x64: 4/4 files, 261/261 cases.
- `CppTest` Win32 and x64: 2/2 files, 227/227 cases.
- `CppTest_Metaonly` Win32 and x64: 2/2 files, 227/227 cases.
- `CppTest_Reflection` Win32 and x64: 2/2 files, 227/227 cases.
- `Test/TypeScript/prepare.ps1`: completed successfully.
- `npm run build` in `Test/TypeScript`: `tsc --noEmit` completed successfully.
- `Build.ps1 Workflow`: completed successfully, including release builds, unit tests, TypeScript build, and release packaging. The script rewrote dependency files under `Import`, and those generated dependency changes were restored because `Project.md` does not allow modifying `Import`.
