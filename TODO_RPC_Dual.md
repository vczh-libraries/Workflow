# Verifying RpcDualLifecycleMock

This document focuses on `vl::rpc_controller_test::RpcDualLifecycleMock`, defined in `Test/Source/RpcDualLifecycleMock.h` and implemented in `Test/Source/RpcDualLifecycleMock.cpp`.

The question is: how many parts of `vl::rpc_controller_test::RpcDualLifecycleMock` actually depend on the knowledge of "dual", meaning there are exactly two lifecycle instances running together?

## Short Answer

I count 0 exact-dual behavior parts inside `vl::rpc_controller_test::RpcDualLifecycleMock`.

It knows only generic lifecycle concepts:

- its own `clientId`,
- local object ids allocated inside this lifecycle,
- whether a `vl::rpc_controller::RpcObjectReference` is local by comparing `ref.clientId == clientId`,
- local object lifetime and interested-client tracking,
- wrapper tracking for remote references,
- event forwarding through `vl::rpc_controller::IRpcDispatcher`,
- service lookup through `vl::rpc_controller::IRpcDispatcher`.

It does not choose "the other lifecycle". It does not assume only client ids `1` and `2`. It does not decide how many clients receive a broadcast. All cross-client choices are behind `vl::rpc_controller::IRpcDispatcher`.

Therefore, splitting is not mainly about moving exact-dual logic out of this class. The useful split is to promote the reusable local lifecycle implementation to shared RPC code as `RpcLifecycleBase`, rename/promote `RpcControllerMock` to `RpcController`, and remove concrete type coupling to the current dual test class name.

## Lifecycle Inventory

| Part | Exact-dual dependency? | Analysis |
| --- | --- | --- |
| `clientId`, constructor, `GetClientId` | No | A lifecycle needs its own client id in any topology. |
| `dispatcher`, `GetDispatcher` | No | The stored type is `vl::rpc_controller::IRpcDispatcher*`. The current private-field setup is concrete-coupled, but the behavior is generic. |
| `RpcControllerMock controller`, `GetController` | No | This is local controller ownership, not dual routing. In the refactoring this should become shared `RpcController`, not a test mock. |
| `idMap`, `SetIdMap`, `RegisterService`, `RequestService` | No | Type-name resolution is local; service authority is delegated to dispatcher. |
| `nextObjectId`, `localObjectProperties` | No | Object id allocation is per lifecycle. |
| `RpcLocalObjectProperties::interestedClients` | No | It stores a list of client ids, so it is already more general than two clients. |
| `LocalObjectHold`, `LocalObjectUnhold` | No | They validate that the ref is local and update interested clients. They do not know who else exists. |
| `PtrToRef` | No | It creates or reuses a local reference. It does not increment remote interest and does not route. |
| `RefToPtr` | No | It distinguishes local vs remote by `clientId`; all non-local refs are handled uniformly. |
| Local object trackers | No behaviorally | They store one owner lifecycle and one ref. The concrete `RpcDual*` names and `RpcDualLifecycleMock*` pointer types should become shared data structures using `RpcLifecycleBase*`. |
| Wrapper tracking | No behaviorally | It is keyed by full `vl::rpc_controller::RpcObjectReference`, so it supports any remote client id. The concrete tracker/property types should move to shared code with `RpcLifecycleBase`. |
| `CreateCallerProxy`, `RegisterWrapperFactory` | No | Wrapper construction is based on `ref.typeId` and the registered factory, not on dual topology. |
| `DecideTypeId` | No | Predefined collection type detection is generic; generated interface type detection belongs in a hook. |
| `AttachLocalObjectEvents` | No | It is a generated-object hook. Generated listeners accept `vl::rpc_controller::IRpcLifecycle*`; the current test callback type is the concrete coupling. |
| Observable-list `ItemChanged` forwarding | No | It calls dispatcher broadcast APIs. The dispatcher decides the target set. |
| `Finalize`, `DisconnectWrappersForFinalize` | No | Cleanup is local. Cross-lifecycle finalization order belongs outside the lifecycle. |

## Why Split If There Is No Dual Knowledge?

The split is still useful, but for a different reason: `vl::rpc_controller_test::RpcDualLifecycleMock` is already a mostly generic lifecycle implementation hidden behind a dual-specific name and several concrete type references. The intended end state is that `RpcDualLifecycleMock` derives from shared `RpcLifecycleBase`, while `RpcControllerMock` becomes shared `RpcController`.

Benefits of extracting a base:

- Reuse the tricky local mechanics: `PtrToRef`, `RefToPtr`, interested-client tracking, wrapper tracking, tracker cleanup, service object lifetime, and finalization are easy to get subtly wrong. A new implementation should use shared `RpcLifecycleBase`, not fork this code.
- Make the architecture explicit: lifecycle is local state plus object/reference conversion; dispatcher is routing. This matches `TODO_RPC_Definition.md`.
- Reduce future accidental coupling: once the reusable class is `RpcLifecycleBase` and the controller is shared `RpcController`, adding a non-dual dispatcher does not require pretending the lifecycle or controller is a test-only dual mock.
- Isolate the test topology: exact-two-client assumptions remain in routing code, not in local lifecycle state.

There is no strong semantic reason to keep a non-empty `RpcDualLifecycleMock` subclass if every behavior moves to the base.

What may force a thin `RpcDualLifecycleMock` derived class to remain is mostly C++ and source-compatibility shape:

- existing generated/test code includes `RpcDualLifecycleMock.h`,
- `RunRpcTestCase` currently mentions `RpcDualLifecycleMock*` in the event-attachment callback,
- tracker classes currently store `RpcDualLifecycleMock*`,
- the dispatcher currently writes the lifecycle's private `dispatcher` field through friendship,
- a concrete derived class name may be useful while generated code and tests are migrated to shared `RpcLifecycleBase`.

If those references are migrated to `RpcLifecycleBase` or to `vl::rpc_controller::IRpcLifecycle*`, `RpcDualLifecycleMock` should not need its own lifecycle logic. It can be a thin derived class with inherited constructors and no behavior beyond generated-test hook glue.

## Recommended Split

Promote the reusable pieces to shared RPC code:

- rename/promote `RpcControllerMock` to `RpcController`,
- create `RpcLifecycleBase` as the base class of `vl::rpc_controller_test::RpcDualLifecycleMock`,
- move reusable data structures with them, including local-object properties, local-object trackers, wrapper trackers, and wrapper-property records.

`RpcLifecycleBase` should contain local lifecycle mechanics:

- client id and dispatcher storage,
- shared `RpcController` ownership,
- local object id allocation,
- local object tracking and cleanup,
- interested-client tracking,
- wrapper tracking and wrapper factory,
- `RefToPtr` and `PtrToRef`,
- service name/id mapping,
- predefined collection type id detection,
- local observable-list event attachment,
- finalization of wrappers, local objects, and controller,
- virtual hooks for generated type ids and generated object event attachment.

Do not list pure `vl::rpc_controller_test::RpcDualDispatcherMock` responsibilities as split work for `vl::rpc_controller_test::RpcDualLifecycleMock`. The only topology-related item that actually crosses into `RpcDualLifecycleMock` today is dispatcher wiring:

- `vl::rpc_controller_test::RpcDualLifecycleMock` has a private `vl::rpc_controller::IRpcDispatcher* dispatcher` field and `GetDispatcher()` returns it.
- `vl::rpc_controller_test::RpcDualLifecycleMock` declares `friend class RpcDualDispatcherMock` only so the dispatcher can assign that private field.
- `vl::rpc_controller_test::RpcDualDispatcherMock::RpcDualDispatcherMock` currently writes `lifecycle1->dispatcher = this` and `lifecycle2->dispatcher = this`.

After extracting `RpcLifecycleBase`, this should become constructor injection or a protected/public setup API on the base lifecycle, for example `SetDispatcher(vl::rpc_controller::IRpcDispatcher*)`. Then `vl::rpc_controller_test::RpcDualDispatcherMock` can keep all exact-dual routing internally without needing friendship to `RpcDualLifecycleMock`.

After this split, `vl::rpc_controller_test::RpcDualLifecycleMock` should not contain dual behavior. It should derive from `RpcLifecycleBase` and exist only as a compatibility type or a small subclass that supplies no more than construction/hook glue.

## Conclusion

`vl::rpc_controller_test::RpcDualLifecycleMock` has no exact-dual behavior to preserve. The real reason to split is to move its generic local lifecycle implementation, the current `RpcControllerMock` behavior, and related data structures into shared `RpcLifecycleBase` / `RpcController` code. If the split does not remove concrete `RpcDual*` type coupling or enable another implementation to reuse the shared base, then an empty subclass by itself does not buy much.
