# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

## Task 1

`Test/UnitTest/Generated_Apps_ChatBot/Generated_Apps_ChatBot.vcxitems` has wild cards, which is not allowed, enumerate files explicitly.

Refactor of `IRpcLifecycle`:

- Rename `IRpcLifecycle::RegisterService` to `RegisterLocalService`, registering a local object as a service.
- Add `IRpcLifecycle::DeclareRemoteService(vint typeId, vint clientId)`, declare a registered remote service.
- Add `IRpcLifecycle::GetRegisteredLocalServices`, returns a const& dictionary of local registered services.
- Update `IRpcLifecycle::RequestService(vint typeId)`, returns the local registered service or call `IRpcDispatcher::RequestService`.
- Add `IRpcLifecycle::Initialize` calling `IRpcDispatcher::Initialize`. `RegisterLocalService` will throw after that.
- New functions will be implemented in `RpcLifecycleBase`.
  - `RegisterLocalService` throw if the type id already exists.
  - `DeclareRemoteService` overrides old entries if type id already exists.
  - `RequestService` prioritize local service over remote service.
- Test samples need to change so that it calls `IRpcLifecycle::RegisterLocalService`.

Refactor of `IRpcDispatcher`:
- Delete `IRpcDispatcher::IsRegisteredService`.
- Update `IRpcDispatcher::RegisterService` to `IRpcDispatcher::DeclareLocalService(typeId, clientId)`.
- Add `IRpcDispatcher::Initialize`.
- In test mocks:
  - `Initialize` keeps empty.
  - `RegisterService` calls `IRpcLifecycle::DeclareRemoteService` of the other side.
  - Services registration will always be in `RpcLifecycleBase`, test mocks should no longer maintain service registration.
  - Currently both `RpcLifecycleBase` and all `IRpcDispatcher` mocks maintain services, which is incorrect.

The goal is that, when user implements its own protocol of RPC, `IRpcDispatcher` and `IRpcJsonMessageDispatcher` will be pure for data transmission, they should not maintain any data or state.

# UPDATES

## UPDATE

I think it is better to keep IRpcLifecycle::RequestService using string (type name) instead of vint (type id) as the argument

Just an amendment, please continue to follow [investigate.prompt.md](.github/prompts/investigate.prompt.md) to finish the work.

# TEST [CONFIRMED]

The change will be verified by static scans and the existing RPC/unit-test flow:
- `Generated_Apps_ChatBot.vcxitems` must contain explicit generated ChatBot file entries instead of wildcard includes.
- No source/test/generated sample should keep calling `IRpcLifecycle::RegisterService`; samples should call `RegisterLocalService`.
- `IRpcDispatcher` should no longer expose `IsRegisteredService` or service-storage behavior, and dispatcher mocks should only relay local-service declarations to other lifecycles.
- `RpcLifecycleBase` should own local and remote service registration, reject local registrations after `Initialize`, reject duplicate local service type ids, override duplicate remote declarations, and prefer local services in `RequestService(typeName)`.
- Build and unit-test coverage should include at least the affected solution build plus RPC-related unit tests, and TypeScript verification should run if shared RPC schema output changes.

Static scans confirmed the old surface is present: `Generated_Apps_ChatBot.vcxitems` uses wildcard `*.cpp` / `*.h` includes, `IRpcDispatcher` exposes `IsRegisteredService` and `RegisterService`, `RpcDualDispatcherMockBase` stores a service dictionary, and RPC samples call `lc.RegisterService(...)`.

# PROPOSALS

- No.1 Move service ownership to `RpcLifecycleBase` and make dispatchers relay-only [CONFIRMED]

## No.1 Move service ownership to `RpcLifecycleBase` and make dispatchers relay-only

### CODE CHANGE

Implement the requested lifecycle/dispatcher refactor in one pass:
- Change the reflected lifecycle API to `Initialize`, `RegisterLocalService(typeId, service)`, `DeclareRemoteService(typeId, clientId)`, `RequestService(typeName)`, plus a C++ `GetRegisteredLocalServices()` accessor.
- Store local service objects and remote service declarations in `RpcLifecycleBase`; local registration rejects duplicate type ids and rejects calls after initialization, while remote declarations overwrite earlier client ids.
- Replace dispatcher service storage with relay-only methods: `Initialize()`, `DeclareLocalService(typeId, clientId)`, and `RequestService(typeId)`.
- Update dual-lifecycle test dispatchers so local declarations call `DeclareRemoteService` on the other lifecycle and service requests read lifecycle-owned local service dictionaries.
- Update RPC samples to call `RegisterLocalService(rpc_GetIds()[...], ...)` and request services by full type name strings, then regenerate affected outputs.
- Enumerate the ChatBot generated `.cpp` and `.h` files explicitly in `Generated_Apps_ChatBot.vcxitems`.

### CONFIRMED

Implemented the lifecycle/dispatcher refactor with lifecycle-owned service state:
- `IRpcLifecycle::RegisterService` is now `RegisterLocalService(typeId, service)`.
- `IRpcLifecycle::RequestService(typeName)` resolves the type name through the lifecycle id map, returns a matching local service first, and falls back to `IRpcDispatcher::RequestService(typeId)` only for declared remote services.
- `IRpcDispatcher` no longer exposes `IsRegisteredService`; dispatcher declaration is now `DeclareLocalService(typeId, clientId)`, and `Initialize()` was added.
- `RpcLifecycleBase` owns local and remote service maps, rejects duplicate local registrations, rejects local registration after initialization, overwrites remote declarations, and exposes `GetRegisteredLocalServices()`.
- Dual-lifecycle mocks relay local declarations to the other lifecycle and request service objects from lifecycle-owned local service dictionaries.
- JSON object ops no longer carry service-registration messages.
- RPC samples and generated outputs use `RegisterLocalService(rpc_GetIds()[...], service)` for registration and `RequestService("...")` for lookup.
- `Generated_Apps_ChatBot.vcxitems` explicitly enumerates the generated ChatBot source/header files.

Verification completed:
- Static scan: no scoped source/test/TypeScript references to `RegisterService`, `IsRegisteredService`, or `IObjectOps_RegisterService`.
- Static scan: no RPC sample calls to `RequestService(rpc_GetIds` or `RegisterService(`.
- Reflection baseline check: dispatcher `RequestService(typeId)` remains numeric, lifecycle `RequestService(typeName)` is string.
- Debug solution builds: `MSBUILD "UnitTest.sln" /m:8 /nr:false /p:Configuration=Debug;Platform=x64` and `Platform=Win32`.
- Debug unit executables via `copilotExecute.ps1`: `LibraryTest` x64/Win32, `CompilerTest_GenerateMetadata` x64/Win32, `CompilerTest_LoadAndCompile` x64, `RuntimeTest` x64/Win32, `CppTest` x64/Win32, `CppTest_Metaonly` x64/Win32, `CppTest_Reflection` x64/Win32.
- TypeScript: `Test/TypeScript/prepare.ps1` and `npm run build`.
- Full required verification: `C:\Code\VczhLibraries\Tools\Tools\Build.ps1 Workflow`.
