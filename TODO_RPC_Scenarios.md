# Workflow RPC Controller - Scenario Analysis

This document proves that the controller interface design in [TODO_RPC_Definition.md](./TODO_RPC_Definition.md) can support all RPC scenarios.

## Architecture Overview

The communication chain is symmetric:
```
Caller -> CallerWrapper -> IRpcController -> protocol -> IRpcController -> objectCallback -> Callee
```

- **Caller**: application code that calls typed interface methods.
- **CallerWrapper**: generated per-type proxy implementing the `@rpc:Interface`. Translates typed calls to untyped `IRpcObjectOps` calls on the controller.
- **IRpcController**: inherits all four Ops interfaces. Routes operations based on `RpcObjectReference.clientId` — local operations are dispatched to registered callbacks; remote operations go through the protocol.
- **protocol**: serialization layer (JSON, binary, etc.). Not defined by the controller interfaces.
- **objectCallback**: the callee's registered `IRpcObjectOps` implementation. Dispatches untyped calls to the real objects.
- **Callee**: the real interface implementation.

Each client (process) has one `IRpcController` instance.

**Caller side — `IRpcController`:** Through its base interfaces the controller provides:
- `IRpcObjectOps`: invoke methods on a remote object and request services.
- `IRpcListOps`: invoke operations on a remote byref collection.
- `IRpcObjectEventOps`: broadcast events on a local object to all remote holders.
- `IRpcListEventOps`: broadcast change notifications on a local observable collection to remote holders.

**Callee side — `Register` callbacks:** The `Register` function sets up all callee-side callbacks (raw pointers `*`, since the controller does not own callback lifetime) that handle the reverse direction:
- `objectCallback : IRpcObjectOps*`: handles incoming requests to invoke methods on a local object, and to serve `RequestService` queries.
- `listCallback : IRpcListOps*`: handles incoming requests for list/dict operations on a local byref collection.
- `eventCallback : IRpcObjectEventOps*`: handles incoming event notifications from a remote object.
- `listEventCallback : IRpcListEventOps*`: handles incoming change notifications from a remote observable collection.

Standard implementations exist for `listCallback` and `listEventCallback`. `Register` returns the `int[string]` ID mapping and calls `SyncIds(ids)` on the registered `objectCallback` and `eventCallback`.

### Values at the Controller Layer

All `object` values flowing through the controller interfaces are serializable values (primitives, structs, enums, or containers of serializable values recursively), or `RpcObjectReference` structs. An `object` value at this layer is **never** an actual interface instance — those are always represented as `RpcObjectReference`. The controller does not hold or manage actual objects; it only assigns IDs and routes signals.

### ID Synchronization

A central server assigns integer IDs for all known method names, property names, event names, and type names. The mapping is `int[string]`. When a client calls `Register`, the controller returns this mapping. The controller also calls `SyncIds(ids)` on the registered `objectCallback` and `eventCallback` (both inherit `IRpcIdSync`), so they can translate between integer IDs and semantic names.

### RpcObjectReference Everywhere

There are no local tokens. All operations use `RpcObjectReference` directly:
- `RpcObjectReference.clientId` identifies the owning client.
- `RpcObjectReference.objectId` identifies the object within that client.
- `RpcObjectReference.typeId` identifies the type of the object (the `@rpc:Interface` type or collection type), using the same integer ID scheme.
- The controller routes based on `clientId`: if local, handle directly; if remote, forward over the protocol.

`RegisterLocalObject(typeId)` is semantically a constructor for `RpcObjectReference` — the controller assigns `clientId` and `objectId` and stores the given `typeId`. It does not hold or manage the actual object.

### RpcObjectReference in Arguments

The controller does **not** inspect or translate argument values. Instead:
- CallerWrapper converts interface arguments to `RpcObjectReference` structs before passing them in `object[]`.
- The callee's objectCallback detects `RpcObjectReference` structs in arguments and creates proxies.

This keeps the controller type-agnostic. The wrappers know the types and handle conversions.

### Reference Counting Convention

When an `RpcObjectReference` is returned from a method call or event notification, it represents an **acquired reference** that the receiver owns. The receiver must call `ReleaseRemoteObject` when done (typically in the proxy destructor). If the reference is passed further, the sender calls `AcquireRemoteObject` before passing.

### ObjectHold

The callee's objectCallback can call `controller.ObjectHold(ref, true)` to inform the controller that the object must not be released even if ref count reaches 0. Calling `ObjectHold(ref, false)` restores ordinary ref-count rules. This is informational — it does not increment or decrement any count. Use case: the callee knows the object is still in use locally even after all remote clients have released their references.

### Event Model

There is no explicit subscribe/unsubscribe at the controller level. The C++ object model does not support subscription callbacks. Instead:
- When the callee registers an object via `RegisterLocalObject`, the objectCallback automatically subscribes to all events on the real object.
- When an event fires, the objectCallback calls `controller.InvokeEvent(ref, eventId, args)`.
- The controller routes the notification to all remote clients that hold acquired references to that object.
- Releasing a reference via `ReleaseRemoteObject` stops event delivery.

## Scenario 1: Simple Method Call

**Interface:**
```
@rpc:Interface
interface ISimple
{
    func Add(a : int, b : int) : int;
}
```

**Goal:** Caller on Client B calls `Add(3, 5)` on an `ISimple` instance owned by Client A.

**CallerWrapper on Client B:**
```
class SimpleCallerProxy : ISimple
{
    var controller : IRpcController^;
    var ref : RpcObjectReference;

    override func Add(a : int, b : int) : int
    {
        var args = {cast object a, cast object b};
        var result = controller.InvokeMethod(ref, METHOD_ADD, args);
        return cast int result;
    }

    // destructor calls controller.ReleaseRemoteObject(ref)
}
```

**Flow:**
1. CallerWrapper calls `controller.InvokeMethod(ref, METHOD_ADD, [3, 5])`.
2. Controller sees `ref.clientId` != local, routes request to Client A over the protocol.
3. Client A's controller receives the request, calls `objectCallback.InvokeMethod(ref, METHOD_ADD, [3, 5])`.
4. objectCallback looks up the real object by `ref.objectId`, calls `realObject.Add(3, 5)` → returns `8`.
5. objectCallback returns `cast object 8`.
6. Client A's controller sends response: `8`.
7. Client B's controller receives response, returns `cast object 8` from `InvokeMethod`.
8. CallerWrapper casts result to `int` → returns `8`.

**Symmetry demonstrated:** Both sides use `IRpcObjectOps.InvokeMethod` — the caller calls it on the controller, the callee implements it in the objectCallback.

## Scenario 2: Async Method Call

**Interface:**
```
@rpc:Interface
interface ICompute
{
    func LongRunning(input : string) : system::Async^;
}
```

**CallerWrapper:**
```
override func LongRunning(input : string) : system::Async^
{
    var args = {cast object input};
    return controller.InvokeMethodAsync(ref, METHOD_LONGRUNNING, args);
}
```

**Flow:**
1. CallerWrapper calls `controller.InvokeMethodAsync(ref, METHOD_LONGRUNNING, ["hello"])`.
2. Caller's controller creates an `IFuture`, sends async request to Client A, returns the `IFuture` as `system::Async^` immediately.
3. Client A's controller calls `objectCallback.InvokeMethodAsync(ref, METHOD_LONGRUNNING, ["hello"])`.
4. objectCallback calls `realObject.LongRunning("hello")` which returns `IAsync^`.
5. objectCallback returns the `IAsync^`.
6. Client A's controller awaits the `IAsync^` completion, then sends the result back.
7. Client B's controller receives the response, calls `promise.SendResult(value)` on the `IFuture`.
8. Caller's coroutine resumes with the result.

**Symmetry demonstrated:** Both sides use `IRpcObjectOps.InvokeMethodAsync`. The caller's controller returns a future immediately; the callee's objectCallback returns `IAsync^` which the callee's controller awaits.

## Scenario 3: Property Access

**Interface:**
```
@rpc:Interface
interface ISimple
{
    prop Name : string {GetName, SetName : NameChanged}
    func GetName() : string;
    func SetName(value : string) : void;
    event NameChanged();
}
```

**CallerWrapper:**
```
override func GetName() : string
{
    return cast string controller.InvokeMethod(ref, METHOD_GETNAME, {});
}
override func SetName(value : string) : void
{
    controller.InvokeMethod(ref, METHOD_SETNAME, {cast object value});
}
```

**Flow (Get):**
1. CallerWrapper calls `controller.InvokeMethod(ref, METHOD_GETNAME, [])` — property getters are regular method calls.
2. Controller routes to Client A.
3. Client A's controller calls `objectCallback.InvokeMethod(ref, METHOD_GETNAME, [])`.
4. objectCallback calls `realObject.GetName()` → `"Alice"`.
5. Returns through the chain.

**Regarding @rpc:Cached and @rpc:Dynamic:**
- `@rpc:Cached`: CallerWrapper maintains a local cache. On cache miss, calls `controller.InvokeMethod` for the getter. On associated event, clears cache.
- `@rpc:Dynamic`: CallerWrapper always calls `controller.InvokeMethod` for the getter.
- The controller interface does not distinguish these — property access is just method invocation. Caching is a CallerWrapper responsibility.

## Scenario 4: Event Notification

**Interface:**
```
@rpc:Interface
interface ISimple
{
    prop Name : string {GetName, SetName : NameChanged}
    event NameChanged();
}
```

**Goal:** Caller on Client B receives `NameChanged` notifications from a remote `ISimple` on Client A.

**Setup:** Client B holds an acquired reference to the `ISimple` object. The callee's objectCallback on Client A has subscribed to all events on the real object during `RegisterLocalObject`.

**Flow (Event Fires):**
1. `realObject.SetName("Charlie")` triggers `NameChanged()` on Client A.
2. objectCallback's event handler detects the event.
3. objectCallback calls `controller.InvokeEvent(ref, EVENT_NAMECHANGED, [])` — the controller as `IRpcObjectEventOps`.
4. Client A's controller knows Client B holds an acquired reference to `ref`.
5. Sends event notification to Client B over the protocol.
6. Client B's controller receives the notification, calls `eventCallback.InvokeEvent(ref, EVENT_NAMECHANGED, [])`.
7. eventCallback matches `(ref, EVENT_NAMECHANGED)` to the CallerWrapper proxy, raises the local `NameChanged` event.
8. Caller's event listeners are notified.

**No explicit subscribe/unsubscribe.** Holding an acquired reference to the object means the client receives all its events. Releasing the reference via `ReleaseRemoteObject` stops delivery. This avoids the C++ object model limitation of not having subscription callbacks.

**Symmetry demonstrated:** Event notification uses `IRpcObjectEventOps` symmetrically — callee calls `controller.InvokeEvent(...)` to push, caller's `eventCallback.InvokeEvent(...)` to receive.

## Scenario 5: Interface Instance as Return Value

**Interface:**
```
@rpc:Interface
interface INested
{
    func GetSimple() : ISimple^;
}
```

**Goal:** Caller on Client B calls `GetSimple()` which returns a remote `ISimple^` from Client A.

**objectCallback on Client A:**
```
override func InvokeMethod(ref : RpcObjectReference, methodId : int, arguments : object[]) : object
{
    if (methodId == METHOD_GETSIMPLE)
    {
        var result : ISimple^ = realObject.GetSimple();
        var resultRef = controller.RegisterLocalObject(TYPE_ISIMPLE);
        // objectCallback records: resultRef.objectId -> result
        controller.AcquireRemoteObject(resultRef);  // +1 for the caller
        return cast object resultRef;
    }
}
```

**CallerWrapper on Client B:**
```
override func GetSimple() : ISimple^
{
    var result = controller.InvokeMethod(ref, METHOD_GETSIMPLE, {});
    var simpleRef = cast RpcObjectReference result;
    // simpleRef is already acquired (convention), caller owns it
    return new SimpleCallerProxy(controller, simpleRef);
    // proxy destructor calls controller.ReleaseRemoteObject(simpleRef)
}
```

**Flow:**
1. CallerWrapper calls `controller.InvokeMethod(ref, METHOD_GETSIMPLE, [])`.
2. Routes to Client A's objectCallback.
3. objectCallback gets the real `ISimple^`, allocates a ref: `RegisterLocalObject(TYPE_ISIMPLE)` → `RpcObjectReference{clientA, newId, TYPE_ISIMPLE}`.
4. objectCallback records the mapping from `newId` to the real object in its own dispatch table.
5. objectCallback acquires: `AcquireRemoteObject(resultRef)` — ref count = 1 for the caller.
6. Returns `RpcObjectReference` as `object`.
7. Result flows back to Client B.
8. CallerWrapper creates `SimpleCallerProxy(controller, simpleRef)`.
9. Proxy destructor will call `ReleaseRemoteObject(simpleRef)` when the proxy is destroyed.

## Scenario 6: Interface Instance as Parameter

**Interface:**
```
@rpc:Interface
interface INested
{
    func SetSimple(value : ISimple^) : void;
}
```

**Goal:** Caller on Client B passes a local `ISimple^` to a method on Client A.

**CallerWrapper on Client B:**
```
override func SetSimple(value : ISimple^) : void
{
    var argRef : RpcObjectReference;
    if (value is SimpleCallerProxy)  // already a remote proxy
    {
        argRef = (cast SimpleCallerProxy^ value).ref;
        controller.AcquireRemoteObject(argRef);  // +1 for the callee
    }
    else  // local object, register it
    {
        argRef = controller.RegisterLocalObject(TYPE_ISIMPLE);
        // objectCallback records: argRef.objectId -> value
        controller.AcquireRemoteObject(argRef);  // +1 for the callee
    }
    var args = {cast object argRef};
    controller.InvokeMethod(ref, METHOD_SETSIMPLE, args);
}
```

**objectCallback on Client A:**
```
override func InvokeMethod(ref : RpcObjectReference, methodId : int, arguments : object[]) : object
{
    if (methodId == METHOD_SETSIMPLE)
    {
        var argRef = cast RpcObjectReference arguments[0];
        // argRef is already acquired (convention)
        var proxy = new SimpleCallerProxy(controller, argRef);
        realObject.SetSimple(proxy);
        // proxy destructor calls ReleaseRemoteObject when callee is done
        return null;
    }
}
```

**Flow:**
1. CallerWrapper converts the `ISimple^` to `RpcObjectReference`, acquires it for the callee.
2. If local: allocates via `RegisterLocalObject(TYPE_ISIMPLE)` and records the mapping. If already a proxy: uses the existing ref.
3. Puts the `RpcObjectReference` in arguments, calls `InvokeMethod`.
4. Callee's objectCallback receives `RpcObjectReference{clientB, id, TYPE_ISIMPLE}`.
5. Creates a proxy with that ref. When the callee accesses the `ISimple^`, the proxy calls **back to Client B** (bidirectional P2P).

## Scenario 7: @rpc:Byval Collection

**Interface:**
```
@rpc:Interface
interface ICollections
{
    @rpc:Byval
    func GetNames() : string[];
}
```

**Goal:** Caller receives a complete copy of the collection.

**objectCallback on callee:**
```
override func InvokeMethod(ref : RpcObjectReference, methodId : int, arguments : object[]) : object
{
    if (methodId == METHOD_GETNAMES)
    {
        return cast object realObject.GetNames();  // serialize entire list
    }
}
```

**CallerWrapper:**
```
override func GetNames() : string[]
{
    var result = controller.InvokeMethod(ref, METHOD_GETNAMES, {});
    return cast string[] result;
}
```

**Flow:**
1. CallerWrapper calls `controller.InvokeMethod(ref, METHOD_GETNAMES, [])`.
2. Callee returns the entire list as a serialized `object`.
3. Protocol serializes: `["Alice", "Bob", "Charlie"]`.
4. Caller receives deserialized list.

**Conclusion:** Byval collections are simple passthroughs. No remote reference is created. No controller-level distinction.

## Scenario 8: @rpc:Byref Collection as Return Value

**Interface:**
```
@rpc:Interface
interface ICollections
{
    @rpc:Byref
    func GetScores() : int[];
}
```

**Goal:** Caller receives a proxy to a remote collection. All operations go through `IRpcListOps`.

**objectCallback on Client A:**
```
override func InvokeMethod(ref : RpcObjectReference, methodId : int, arguments : object[]) : object
{
    if (methodId == METHOD_GETSCORES)
    {
        var list = realObject.GetScores();
        var listRef = controller.RegisterLocalObject(TYPE_INT_LIST);
        // listCallback records: listRef.objectId -> list
        controller.AcquireRemoteObject(listRef);  // +1 for the caller
        return cast object listRef;
    }
}
```

**CallerWrapper on Client B:**
```
override func GetScores() : int[]
{
    var result = controller.InvokeMethod(ref, METHOD_GETSCORES, {});
    var listRef = cast RpcObjectReference result;
    return new ByrefIntListProxy(controller, listRef);
}
```

**ByrefIntListProxy (generated, implements `int[]` interface):**
```
class ByrefIntListProxy
{
    var controller : IRpcController^;
    var ref : RpcObjectReference;

    func GetCount() : int
    {
        return controller.ListGetCount(ref);
    }
    func Get(index : int) : int
    {
        return cast int controller.ListGet(ref, index);
    }
    func Set(index : int, value : int) : void
    {
        controller.ListSet(ref, index, cast object value);
    }
    // ... etc.
    // destructor calls controller.ReleaseRemoteObject(ref)
}
```

**Flow:**
1. objectCallback allocates a ref for the local list: `RegisterLocalObject(TYPE_INT_LIST)` → `RpcObjectReference{clientA, listId, TYPE_INT_LIST}`.
2. listCallback records the mapping from `listId` to the real list.
3. Acquires and returns the reference.
4. CallerWrapper creates `ByrefIntListProxy` wrapping the reference.
5. Every list operation on the proxy calls `controller.ListGetCount(ref)`, `controller.ListGet(ref, ...)`, etc.
6. Controller routes to Client A where the registered `listCallback` (standard implementation) accesses the real list.

**Key point:** The `listCallback` registered via `Register` handles incoming list operations. The standard implementation looks up the real list by `objectId` and calls its methods.

## Scenario 9: @rpc:Byref Collection as Parameter

**Interface:**
```
@rpc:Interface
interface IMethodParams
{
    func Process(@rpc:Byval items : int[], @rpc:Byref refs : string[]) : void;
}
```

**Goal:** Caller passes a byref collection as parameter. Callee operates on the caller's collection remotely.

**CallerWrapper:**
```
override func Process(items : int[], refs : string[]) : void
{
    // items: @rpc:Byval -> serialize entire list
    var arg0 = cast object items;

    // refs: @rpc:Byref -> register locally, send reference
    var refsRef = controller.RegisterLocalObject(TYPE_STRING_LIST);
    // listCallback records: refsRef.objectId -> refs
    controller.AcquireRemoteObject(refsRef);  // +1 for the callee
    var arg1 = cast object refsRef;

    controller.InvokeMethod(ref, METHOD_PROCESS, {arg0, arg1});
}
```

**objectCallback on callee:**
```
override func InvokeMethod(ref : RpcObjectReference, methodId : int, arguments : object[]) : object
{
    if (methodId == METHOD_PROCESS)
    {
        var items = cast int[] arguments[0];                   // byval: direct value
        var refsRef = cast RpcObjectReference arguments[1];    // byref: reference
        var refsProxy = new ByrefStringListProxy(controller, refsRef);
        realObject.Process(items, refsProxy);
        // proxy destructor calls ReleaseRemoteObject
        return null;
    }
}
```

**Flow:**
1. Caller allocates a ref for its local `string[]`: `RegisterLocalObject(TYPE_STRING_LIST)` → `RpcObjectReference{clientB, refsId, TYPE_STRING_LIST}`.
2. listCallback records the mapping, then acquires and sends the reference to the callee.
3. Callee creates a proxy wrapping `RpcObjectReference{clientB, refsId, TYPE_STRING_LIST}`.
4. When `realObject.Process` accesses `refs` (e.g., `refs.Get(0)`), the proxy calls `controller.ListGet(refsRef, 0)`.
5. The callee's controller sees `refsRef.clientId == clientB`, routes the list-get request **back to the caller** (bidirectional P2P).
6. Client B's controller calls its registered `listCallback.ListGet(refsRef, 0)`, which accesses the real list.

**Note:** During execution of `Process`, the callee makes calls back to the caller. The controller must handle concurrent incoming and outgoing requests. This is an implementation requirement, not an interface design concern.

## Scenario 10: Singleton Service (@rpc:Ctor)

**Interface:**
```
@rpc:Interface
@rpc:Ctor
interface IWithCtor
{
    func GetValue() : int;
}
```

**Goal:** Client B requests the `IWithCtor` service. Client A, which provides the service, handles the request through its objectCallback.

**Setup on Client A (callee):**
```
var realObject = CreateWithCtorImpl();
var serviceRef = controller.RegisterLocalObject(TYPE_IWITHCTOR);
// objectCallback records: serviceRef.objectId -> realObject
// objectCallback also records: TYPE_IWITHCTOR -> serviceRef (for RequestService)
```

**CallerWrapper on Client B:**
```
static func RequestIWithCtor(controller : IRpcController^) : IWithCtor^
{
    var ref = controller.RequestService(TYPE_IWITHCTOR);
    return new WithCtorCallerProxy(controller, ref);
}
```

**Flow:**
1. Client A creates the real object, allocates a ref via `RegisterLocalObject(TYPE_IWITHCTOR)`, records the mapping in its objectCallback.
2. Client B calls `controller.RequestService(TYPE_IWITHCTOR)` — `RequestService` is part of `IRpcObjectOps`.
3. Client B's controller routes the request to Client A's objectCallback.
4. Client A's `objectCallback.RequestService(TYPE_IWITHCTOR)` returns the `RpcObjectReference` for the service.
5. Client B creates a proxy.

**No factory interface needed.** The callee creates the real object and registers it locally. The objectCallback knows which services it provides and responds to `RequestService` queries.

## Scenario 11: P2P Object Passing Between Multiple Clients

**Setup:**
- Client A creates `IFoo` object F and registers it.
- Client B gets a reference to F.
- Client B passes F to Client C in a method call.

**Flow:**
1. Client A allocates: `RegisterLocalObject(TYPE_IFOO)` → `RpcObjectReference{clientA, idA, TYPE_IFOO}`.
2. Client B holds `RpcObjectReference{clientA, idA, TYPE_IFOO}` (acquired).
3. Client B passes F to Client C:
   a. CallerWrapper calls `controller.AcquireRemoteObject({clientA, idA, TYPE_IFOO})` — ref count on Client A: 2.
   b. Puts `RpcObjectReference{clientA, idA, TYPE_IFOO}` in arguments.
   c. Sends to Client C.
4. Client C's objectCallback receives `RpcObjectReference{clientA, idA, TYPE_IFOO}`.
5. Client C creates a proxy with that reference.
6. Client C calls methods on F **directly to Client A** (not through Client B).
7. When Client B releases: `ReleaseRemoteObject({clientA, idA, TYPE_IFOO})` → Client A ref count: 1.
8. When Client C releases: `ReleaseRemoteObject({clientA, idA, TYPE_IFOO})` → Client A ref count: 0.

**Key Points:**
- The `RpcObjectReference` always points to the OWNER (Client A).
- Client C communicates directly with Client A, not through Client B.
- Reference counting is per-owner: Client A tracks how many remote clients hold references.
- No token translation needed — `RpcObjectReference` is globally meaningful.

## Scenario 12: Object Lifecycle and Reference Counting

**Lifecycle of a remote interface object:**

1. **Allocation:** Callee calls `RegisterLocalObject(typeId)` → `RpcObjectReference{calleeClient, id, typeId}`. The controller allocates IDs only — the objectCallback records the mapping from `id` to the real object in its own dispatch table.
2. **Export:** objectCallback returns the reference (after calling `AcquireRemoteObject`). Remote ref count = 1.
3. **Usage:** Caller uses the reference for method calls (including property getters/setters), etc.
4. **Release:** Caller's proxy destructor calls `ReleaseRemoteObject(ref)`. Remote ref count = 0.
5. **Cleanup:** When remote ref count is 0 (and `ObjectHold` is not active), the callee can call `UnregisterLocalObject(ref)` to remove the ID from the controller's table and clean up the real object from the objectCallback's dispatch table.

**Convention for returning references:**
- When objectCallback returns an `RpcObjectReference` in a method result, it calls `AcquireRemoteObject` first. The caller inherits the reference and is responsible for `ReleaseRemoteObject`.
- When passing a reference in arguments, the sender calls `AcquireRemoteObject`. The receiver calls `ReleaseRemoteObject` when done.

**Local lifecycle:**
- `RegisterLocalObject(typeId)` allocates an ID in the controller. The objectCallback manages the actual object.
- `UnregisterLocalObject` removes the ID. Any remote clients still holding references will get errors on subsequent operations.
- The callee uses `UnregisterLocalObject` when the object is being destroyed or is no longer available.

**ObjectHold:**
- The callee's objectCallback can call `controller.ObjectHold(ref, true)` to prevent cleanup even when ref count reaches 0.
- This is useful when the callee knows the object is still in use locally (e.g., stored in a local collection) and should remain accessible if new remote callers appear.
- Calling `ObjectHold(ref, false)` restores ordinary ref-count rules.

**Multiple holders:**
- If three clients hold the same reference: ref count = 3.
- Each independently calls `ReleaseRemoteObject` when done.
- Only when ALL have released does ref count reach 0.

**Smart pointer integration (C++/Workflow):**
- Proxies are stored in `Ptr<T>` / `T^`.
- When the last `Ptr<Proxy>` is destroyed, the proxy destructor calls `controller.ReleaseRemoteObject(ref)`.
- This is automatic and transparent.

## Scenario 13: Non-RefCounted Language Support (TypeScript, Python)

**Problem:** TypeScript and Python use garbage collection, not reference counting. Proxy objects may not be destroyed promptly.

**Solutions available within the controller interface:**

1. **Explicit dispose pattern:** Generated proxy classes include a `dispose()` / `close()` method that calls `ReleaseRemoteObject`. Codegen generates `with` (Python) or `using` (TS) support.
2. **Weak reference + GC hook:** TS/Python runtimes support weak reference callbacks. When GC collects a proxy, the callback calls `ReleaseRemoteObject`. Delayed but automatic.
3. **Session-based cleanup:** When a client disconnects, all references it holds are released. The controller tracks which clients hold which references.
4. **Batch release:** Language bindings can batch `ReleaseRemoteObject` calls periodically.

**The controller interface does NOT need modification.** `AcquireRemoteObject` and `ReleaseRemoteObject` are explicit primitives. How and when they are called depends on the language binding.

## Scenario 14: Event with Interface Arguments

**Interface:**
```
@rpc:Interface
interface INotifier
{
    event ObjectUpdated(ISimple^);
}
```

**Per spec:** Event arguments are always `@rpc:Byval`. However, interface arguments in events are `RpcObjectReference` values (since interfaces are always serialized as references).

**Flow when event fires on Client A:**
1. Real object fires `ObjectUpdated(simpleObj)`.
2. objectCallback's event handler:
   - Allocates ref: `RegisterLocalObject(TYPE_ISIMPLE)` → `RpcObjectReference{clientA, newId, TYPE_ISIMPLE}`.
   - Records the mapping from `newId` to `simpleObj`.
   - Acquires: `AcquireRemoteObject(newRef)`.
   - Calls `controller.InvokeEvent(ref, EVENT_OBJECTUPDATED, [cast object newRef])`.
3. Controller routes notification to all holders (e.g., Client B).
4. Client B's `eventCallback.InvokeEvent(ref, EVENT_OBJECTUPDATED, [cast object newRef])`.
5. eventCallback detects `RpcObjectReference` in args.
6. Creates proxy: `new SimpleCallerProxy(controller, newRef)`.
7. Raises local event: `raise ObjectUpdated(proxy)`.

**Conclusion:** Interface arguments in events work through the same `RpcObjectReference` mechanism. The `InvokeEvent(ref, eventId, object[])` signature is generic enough to carry any argument types.

## Scenario 15: Interface Inheritance and Casting

**Interface:**
```
@rpc:Interface
interface IBase { func GetId() : int; }

@rpc:Interface
interface IDerived : IBase { func GetLabel() : string; }
```

**Per spec:** A remote object is known to implement exactly one leaf interface.

**Method ID allocation:**
- `IBase`: method 0 = `GetId`
- `IDerived`: method 0 = `GetId` (inherited), method 1 = `GetLabel`

**The objectCallback for IDerived handles all methods, including inherited ones.**

**Casting from `IBase^` to `IDerived^`:**
1. The caller has an `IBase^` proxy with `RpcObjectReference ref`.
2. The caller knows from `ref.typeId` that the real type is `IDerived`.
3. The caller creates a new `IDerivedCallerProxy(controller, ref)` using the same ref.
4. No controller method is needed — casting is a local operation on the proxy.

**Casting from `IDerived^` to `IBase^`:**
Same ref, different wrapper. Local operation, no remote call.

**Conclusion:** Casting is handled entirely by the CallerWrapper. The controller is type-agnostic. The same `RpcObjectReference` works regardless of which interface view is used. The `typeId` field helps the caller determine the real type for up-casting.

## Scenario 16: Byref Dictionary Operations

**Interface:**
```
@rpc:Interface
interface IConfig
{
    @rpc:Byref
    func GetSettings() : string[string];
}
```

**ByrefDictionaryProxy (generated):**
```
class ByrefStringStringDictProxy
{
    var controller : IRpcController^;
    var ref : RpcObjectReference;

    func GetCount() : int { return controller.DictGetCount(ref); }
    func Get(key : string) : string { return cast string controller.DictGet(ref, cast object key); }
    func Set(key : string, value : string) : void { controller.DictSet(ref, cast object key, cast object value); }
    func Remove(key : string) : bool { return controller.DictRemove(ref, cast object key); }
    func ContainsKey(key : string) : bool { return controller.DictContainsKey(ref, cast object key); }
    func GetKeys() : string[] { return cast string[] controller.DictGetKeys(ref); }
    func GetValues() : string[] { return cast string[] controller.DictGetValues(ref); }
    // destructor calls controller.ReleaseRemoteObject(ref)
}
```

**Conclusion:** Dictionary operations use `IRpcListOps.Dict*` methods. Same pattern as list proxies. Handled by the registered `listCallback`.

## Scenario 17: Same-Process Optimization

**Goal:** When caller and callee are in the same process, avoid protocol overhead.

**The controller interface does not change.** The optimization is in the implementation:

1. Caller calls `controller.InvokeMethod(ref, methodId, args)`.
2. Controller sees `ref.clientId` == local client.
3. For interface operations: calls `objectCallback.InvokeMethod(ref, methodId, args)` directly.
4. For list operations: calls `listCallback.ListGet(ref, ...)` directly.
5. No serialization, no protocol, no network. Returns result directly.

This applies to ALL operations: method calls (including property getters/setters), events, list/dict operations.

**Conclusion:** The `RpcObjectReference.clientId` check enables transparent same-process optimization. The caller does not know (or care) whether the target is local or remote.

## Scenario 18: Auto Properties

**Interface:**
```
@rpc:Interface
interface IAutoProps
{
    prop ReadWrite : string {}
    prop ReadOnly : int {const}
    prop NoObserve : string {not observe}
    prop ConstNoObserve : int {const, not observe}
}
```

**Auto properties are syntactic sugar.** At the RPC level:
- `ReadWrite`: Get/Set property, generates change event.
- `ReadOnly`: Get property only.
- `NoObserve`: Get/Set property, no change event.
- `ConstNoObserve`: Get property only, no change event.

All handled by `IRpcObjectOps.InvokeMethod` — property getters and setters are just regular methods at the controller level. The CallerWrapper decides which properties have setters and which have events. Caching (`@rpc:Cached` / `@rpc:Dynamic`) is handled entirely by the CallerWrapper. The objectCallback generated for auto properties reads/writes backing fields.

## Scenario 19: Observable Byref Collection Change Notifications

**Interface:**
```
@rpc:Interface
interface IObservable
{
    @rpc:Byref
    prop Items : int[] {GetItems : ItemsChanged}
    func GetItems() : int[];
    event ItemsChanged();
}
```

**Goal:** When items in the byref `ObservableList` change on the callee side, the caller receives change notifications via `IRpcListEventOps`.

**Flow:**
1. Caller gets a byref list proxy via `controller.InvokeMethod(ref, METHOD_GETITEMS, [])` — the property getter is a regular method call.
2. The callee's `listEventCallback` (standard implementation, registered via `Register`) monitors the registered observable list for changes.
3. When items change on Client A (insert, remove, replace, or clear), the standard implementation calls `controller.OnItemChanged(listRef, index, oldCount, newCount)` — mirroring the `IValueObservableList.ItemChanged` event.
4. Controller routes the notification to Client B.
5. Client B's `listEventCallback` receives `OnItemChanged(listRef, index, oldCount, newCount)`.
6. Updates the local byref list proxy state or fires local observable events.
7. Additionally, the interface-level `ItemsChanged` event fires through `IRpcObjectEventOps.InvokeEvent`.

**Note:** `IRpcListEventOps` is specifically for `IValueObservableList` change notifications. Regular byref list operations (read/write) go through `IRpcListOps`.

## Scenario 20: ID Synchronization

**Goal:** Ensure all clients agree on integer IDs for methods, properties, events, and types.

**Flow:**
1. Central server assigns IDs: `{"ISimple.Add": 1, "ISimple.Name": 2, "ISimple.NameChanged": 3, "IWithCtor": 100, ...}`.
2. Client A connects and calls `Register(objectCallback, eventCallback, listCallback, listEventCallback)`.
3. Controller returns `int[string]` mapping.
4. Controller calls `objectCallback.SyncIds(ids)` and `eventCallback.SyncIds(ids)` so the callbacks know the mapping.
5. Client B connects similarly, gets the same mapping.
6. Both clients now use integer IDs: `controller.InvokeMethod(ref, 1, [3, 5])` means `ISimple.Add(3, 5)`.

**Why IDs instead of strings?** Integer comparisons are faster and protocol messages are smaller. The mapping is established once during registration.

**Conclusion:** `IRpcIdSync` ensures all parties agree on the ID scheme. The mapping is distributed during `Register`.
