# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Task 1

Refactor `RpcDualLifecycleMock.(h|cpp).

- `controller.localObjectProperties` is readonly in `RpcDualLifecycleMock`, Make a `const ...& RpcDualControllerMock::GetLocalObjectProperties();`.
- I don't think `RpcDualControllerModk::pendingProxyRef` is even useful, we should delete it. It is changed in `RpcDualLifecycleMock::CreateCallerProxy`, but `universalWrapperFactory` does not call `RequestService`, which means `RequestService` is supposed to always see an empty value. Delete this variable and the meaningless code using it.
- Now the only two private `RpcDualControllerMock` members usage in `RpcDualLifecycleMock` is eliminated, we should remove the `friend` declaration.
- In `RpcDualLifecycleMock`:
  - `wrapperProperties` should be a dictionary whose key is the `ref`. Since this is the only way how it search data.
  - `suppressedEvents` should be a SortedList, and by making `RpcDualEventDispatch` comparible, `IsSameEvent` could be deleted.
    - The idea of `suppressedEvents` actually looks wired. Because when we received an event, but the object is not used in this lifecycle (by finding no record of this local object or wrapper), the event could just be discarded. So maybe we could just delete `suppressedEvents` and code using it directly.
  - `forwardingEvents` should be a SortedList, pushing and poping is not necessary, just add and remove.
    - `forwardingEvents` should not be static. And if the intention is to enable communication between lifecycles, this would be totally wrong to do it in this way. Lifecycles do not talk to each other through anything other than 4 ops interfaces.
  - `try-catch` in `RpcDualControllerMock::InvokeEvent` is unnecessary, there should be no exception, catching it just hide the problem. If any exception is raised, it is a bug (so far).
  - `services` should not be necessary, because the actual service map is stored in generated `IRpcObjectOps` in each wrapper Workflow script. `RpcDualControllerMock` is not supposed to maintain anything.
    - `DisconnectServices` looks like need to be moved to the destructor in generated `IRpcObjectOps` implementations.
  - `UnregisterAllLocalObjects` could be moved to `RpcDualControllerMock`, better inside its destructor.

# UPDATES

- Refactored `RpcDualControllerMock` so lifecycle code reads local object properties through `GetLocalObjectProperties()` and no longer needs friendship.
- Removed `pendingProxyRef`; generated wrapper creation now receives the concrete `RpcObjectReference` directly.
- Changed wrapper tracking in `RpcDualLifecycleMock` to use `Dictionary<RpcObjectReference, RpcWrapperProperties>`.
- Deleted `suppressedEvents` and `IsSameEvent`. Unknown lifecycle events are discarded, and echo prevention is handled by an instance `SortedList<RpcDualEventDispatch>` for currently forwarding events.
- Removed lifecycle-owned service state. Generated `IRpcObjectOps` remains the owner of service mappings, unregisters services when local objects go away, and disconnects service wrappers in its destructor.
- Moved local object unregister-all cleanup onto `RpcDualControllerMock` and its destructor.
- Removed the `try-catch` from `RpcDualControllerMock::InvokeEvent`.
- Added explicit wrapper-disconnect cleanup in the RPC test lifecycles so both sides release remote references while peer lifecycles are still alive.

# TEST

- [CONFIRMED] `copilotBuild.ps1 -Configuration Debug -Platform Win32`
- [CONFIRMED] `copilotBuild.ps1 -Configuration Debug -Platform x64`
- [CONFIRMED] `LibraryTest` Debug Win32
- [CONFIRMED] `LibraryTest` Debug x64
- [CONFIRMED] `CompilerTest_GenerateMetadata` Debug Win32
- [CONFIRMED] `CompilerTest_GenerateMetadata` Debug x64
- [CONFIRMED] `CompilerTest_LoadAndCompile` Debug x64
- [CONFIRMED] post-generation `copilotBuild.ps1 -Configuration Debug -Platform Win32`
- [CONFIRMED] post-generation `copilotBuild.ps1 -Configuration Debug -Platform x64`
- [CONFIRMED] `RuntimeTest` Debug Win32
- [CONFIRMED] `RuntimeTest` Debug x64
- [CONFIRMED] `CppTest` Debug Win32
- [CONFIRMED] `CppTest` Debug x64
- [CONFIRMED] `CppTest_Metaonly` Debug Win32
- [CONFIRMED] `CppTest_Metaonly` Debug x64
- [CONFIRMED] `CppTest_Reflection` Debug Win32
- [CONFIRMED] `CppTest_Reflection` Debug x64

# PROPOSALS

## No.1 Refactor dual RPC lifecycle mock [CONFIRMED]

The requested refactor is implemented and verified. The only extra scope required was updating the generated RPC wrapper contract and expected generated outputs, because removing `pendingProxyRef` means generated wrapper construction must accept the actual object reference instead of attempting to request it indirectly.
