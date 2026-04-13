# Workflow RPC Controller - Scenario Analysis

This document proves that the controller interface design in [TODO_RPC_Definition.md](./TODO_RPC_Definition.md) can support all RPC scenarios.

## Architecture Overview

The communication chain is symmetric:
```
Caller -> CallerWrapper -> IWorkflowRpcController -> protocol -> IWorkflowRpcController -> objectCallback -> Callee
```

- **Caller**: application code that calls typed interface methods.
- **CallerWrapper**: generated per-type proxy implementing the `@rpc:Interface`. Translates typed calls to untyped `IWorkflowRpcObjectOps` calls on the controller.
- **IWorkflowRpcController**: inherits all four Ops interfaces. Routes operations based on `RpcObjectReference.clientId` — local operations are dispatched to registered callbacks; remote operations go through the protocol.
- **protocol**: serialization layer (JSON, binary, etc.). Not defined by the controller interfaces.
- **objectCallback**: the callee's registered `IWorkflowRpcObjectOps` implementation. Dispatches untyped calls to the real objects.
- **Callee**: the real interface implementation.

Each client (process) has one `IWorkflowRpcController` instance. The `Register` function sets up callee-side callbacks: `objectCallback` for handling incoming interface operations, `eventCallback` for receiving event notifications. List/dictionary operations use a standard implementation provided by the controller itself.

### RpcObjectReference Everywhere

There are no local tokens. All operations use `RpcObjectReference` directly:
- `RpcObjectReference.clientId` identifies the owning client.
- `RpcObjectReference.objectId` identifies the object within that client.
- The controller routes based on `clientId`: if local, handle directly; if remote, forward over the protocol.

### RpcObjectReference in Arguments

The controller does **not** inspect or translate argument values. Instead:
- CallerWrapper converts interface arguments to `RpcObjectReference` structs before passing them in `object[]`.
- The callee's objectCallback detects `RpcObjectReference` structs in arguments and creates proxies.

This keeps the controller type-agnostic. The wrappers know the types and handle conversions.

### Reference Counting Convention

When an `RpcObjectReference` is returned from a method call or event notification, it represents an **acquired reference** that the receiver owns. The receiver must call `ReleaseObject` when done (typically in the proxy destructor). If the reference is passed further, the sender calls `AcquireObject` before passing.

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
    var controller : IWorkflowRpcController^;
    var ref : RpcObjectReference;

    override func Add(a : int, b : int) : int
    {
        var args = {cast object a, cast object b};
        var result = controller.InvokeMethod(ref, METHOD_ADD, args);
        return cast int result;
    }

    // destructor calls controller.ReleaseObject(ref)
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

**Symmetry demonstrated:** Both sides use `IWorkflowRpcObjectOps.InvokeMethod` — the caller calls it on the controller, the callee implements it in the objectCallback.

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

**Symmetry demonstrated:** Both sides use `IWorkflowRpcObjectOps.InvokeMethodAsync`. The caller's controller returns a future immediately; the callee's objectCallback returns `IAsync^` which the callee's controller awaits.

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
    return cast string controller.GetProperty(ref, PROP_NAME);
}
override func SetName(value : string) : void
{
    controller.SetProperty(ref, PROP_NAME, cast object value);
}
```

**Flow (Get):**
1. CallerWrapper calls `controller.GetProperty(ref, PROP_NAME)`.
2. Controller routes to Client A.
3. Client A's controller calls `objectCallback.GetProperty(ref, PROP_NAME)`.
4. objectCallback calls `realObject.GetName()` → `"Alice"`.
5. Returns through the chain.

**Regarding @rpc:Cached and @rpc:Dynamic:**
- `@rpc:Cached`: CallerWrapper maintains a local cache. On cache miss, calls `controller.GetProperty`. On associated event, clears cache.
- `@rpc:Dynamic`: CallerWrapper always calls `controller.GetProperty`.
- The controller interface does not distinguish these. Caching is a CallerWrapper responsibility.

## Scenario 4: Event Subscription and Notification

**Interface:**
```
@rpc:Interface
interface ISimple
{
    prop Name : string {GetName, SetName : NameChanged}
    event NameChanged();
}
```

**Goal:** Caller on Client B subscribes to `NameChanged` on a remote `ISimple` on Client A.

**CallerWrapper subscribes (during proxy initialization):**
```
var subId = controller.SubscribeEvent(ref, EVENT_NAMECHANGED);
```

**Flow (Subscribe):**
1. CallerWrapper calls `controller.SubscribeEvent(ref, EVENT_NAMECHANGED)`.
2. Client B's controller records the subscription and routes to Client A.
3. Client A's controller calls `objectCallback.SubscribeEvent(ref, EVENT_NAMECHANGED)`.
4. objectCallback subscribes to `realObject.NameChanged`, generates subscription ID, returns it.
5. Client A's controller records: "Client B subscribed to (ref, EVENT_NAMECHANGED)".
6. Subscription ID flows back to Client B's CallerWrapper.

**Flow (Event Fires):**
1. `realObject.SetName("Charlie")` triggers `NameChanged()` on Client A.
2. objectCallback's event handler detects the event.
3. objectCallback calls `controller.InvokeEvent(ref, EVENT_NAMECHANGED, [])` — the controller as `IWorkflowRpcObjectEventOps`.
4. Client A's controller looks up subscribers for `(ref, EVENT_NAMECHANGED)` → finds Client B.
5. Sends event notification to Client B over the protocol.
6. Client B's controller receives the notification, calls `eventCallback.InvokeEvent(ref, EVENT_NAMECHANGED, [])`.
7. eventCallback matches `(ref, EVENT_NAMECHANGED)` to the CallerWrapper proxy, raises the local `NameChanged` event.
8. Caller's event listeners are notified.

**Flow (Unsubscribe):**
1. CallerWrapper calls `controller.UnsubscribeEvent(ref, subId)`.
2. Routes to Client A: `objectCallback.UnsubscribeEvent(ref, subId)`.
3. objectCallback detaches from the real event.

**Symmetry demonstrated:** Event subscription uses `IWorkflowRpcObjectOps` (request direction). Event notification uses `IWorkflowRpcObjectEventOps` (notification direction). Both interfaces are used symmetrically — callee calls `controller.InvokeEvent(...)` to push, caller's `eventCallback.InvokeEvent(...)` to receive.

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
        var resultRef = controller.RegisterObject(cast object result);
        controller.AcquireObject(resultRef);  // +1 for the caller
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
    // proxy destructor calls controller.ReleaseObject(simpleRef)
}
```

**Flow:**
1. CallerWrapper calls `controller.InvokeMethod(ref, METHOD_GETSIMPLE, [])`.
2. Routes to Client A's objectCallback.
3. objectCallback gets the real `ISimple^`, registers it: `RegisterObject(obj)` → `RpcObjectReference{clientA, newId}`.
4. objectCallback acquires: `AcquireObject(resultRef)` — ref count = 1 for the caller.
5. Returns `RpcObjectReference` as `object`.
6. Result flows back to Client B.
7. CallerWrapper creates `SimpleCallerProxy(controller, simpleRef)`.
8. Proxy destructor will call `ReleaseObject(simpleRef)` when the proxy is destroyed.

**No `ResolveToken`/`ImportReference` needed.** The `RpcObjectReference` is used directly. The convention ensures the reference is acquired before returning.

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
        controller.AcquireObject(argRef);  // +1 for the callee
    }
    else  // local object, register it
    {
        argRef = controller.RegisterObject(cast object value);
        controller.AcquireObject(argRef);  // +1 for the callee
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
        // proxy destructor calls ReleaseObject when callee is done
        return null;
    }
}
```

**Flow:**
1. CallerWrapper converts the `ISimple^` to `RpcObjectReference`, acquires it for the callee.
2. If local: registers via `RegisterObject` first. If already a proxy: uses the existing ref.
3. Puts the `RpcObjectReference` in arguments, calls `InvokeMethod`.
4. Callee's objectCallback receives `RpcObjectReference{clientB, id}`.
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

**Goal:** Caller receives a proxy to a remote collection. All operations go through `IWorkflowRpcListOps`.

**objectCallback on Client A:**
```
override func InvokeMethod(ref : RpcObjectReference, methodId : int, arguments : object[]) : object
{
    if (methodId == METHOD_GETSCORES)
    {
        var list = realObject.GetScores();
        var listRef = controller.RegisterObject(cast object list);
        controller.AcquireObject(listRef);  // +1 for the caller
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
    var controller : IWorkflowRpcController^;
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
    // destructor calls controller.ReleaseObject(ref)
}
```

**Flow:**
1. objectCallback registers the local list: `RegisterObject(list)` → `RpcObjectReference{clientA, listId}`.
2. Acquires and returns the reference.
3. CallerWrapper creates `ByrefIntListProxy` wrapping the reference.
4. Every list operation on the proxy calls `controller.ListGetCount(ref)`, `controller.ListGet(ref, ...)`, etc.
5. Controller routes to Client A where the **standard `IWorkflowRpcListOps` implementation** accesses the real list.

**Key point:** No custom callback needed for list operations. The controller's standard implementation handles them by looking up the registered object and calling the real list's methods.

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
    var refsRef = controller.RegisterObject(cast object refs);
    controller.AcquireObject(refsRef);  // +1 for the callee
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
        // proxy destructor calls ReleaseObject
        return null;
    }
}
```

**Flow:**
1. Caller registers its local `string[]`: `RegisterObject(refs)` → `RpcObjectReference{clientB, refsId}`.
2. Sends the byval list data and the byref reference to the callee.
3. Callee creates a proxy wrapping `RpcObjectReference{clientB, refsId}`.
4. When `realObject.Process` accesses `refs` (e.g., `refs.Get(0)`), the proxy calls `controller.ListGet(refsRef, 0)`.
5. The callee's controller sees `refsRef.clientId == clientB`, routes the list-get request **back to the caller** (bidirectional P2P).
6. Client B's controller handles it locally using the standard `IWorkflowRpcListOps` implementation.

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

**Goal:** Client B requests the `IWithCtor` service. Client A, which registered the service, creates it.

**Setup on Client A (callee):**
```
var factory = new WithCtorFactory();
controller.RegisterService(TYPE_IWITHCTOR, factory);
```

**IWorkflowRpcObjectFactory implementation:**
```
class WithCtorFactory : IWorkflowRpcObjectFactory
{
    override func Create(controller : IWorkflowRpcController^) : RpcObjectReference
    {
        var realObject = CreateWithCtorImpl();
        var ref = controller.RegisterObject(cast object realObject);
        // objectCallback dispatch table is updated to handle this object
        return ref;
    }
}
```

**CallerWrapper on Client B:**
```
static func RequestIWithCtor(controller : IWorkflowRpcController^) : IWithCtor^
{
    var ref = controller.RequestService(TYPE_IWITHCTOR);
    return new WithCtorCallerProxy(controller, ref);
}
```

**Flow:**
1. Client B calls `controller.RequestService(TYPE_IWITHCTOR)`.
2. Client B's controller knows (from registration) that Client A provides `TYPE_IWITHCTOR`.
3. Routes request to Client A.
4. Client A's controller calls `factory.Create(controller)` → gets `RpcObjectReference`.
5. Returns the reference to Client B.
6. Subsequent calls to `RequestService(TYPE_IWITHCTOR)` return the same reference (singleton).

**Constraint:** Only one client may call `RegisterService` for a given `typeId`. A second registration throws.

## Scenario 11: P2P Object Passing Between Multiple Clients

**Setup:**
- Client A creates `IFoo` object F and registers it.
- Client B gets a reference to F.
- Client B passes F to Client C in a method call.

**Flow:**
1. Client A registers F: `RegisterObject(F)` → `RpcObjectReference{clientA, idA}`.
2. Client B holds `RpcObjectReference{clientA, idA}` (acquired).
3. Client B passes F to Client C:
   a. CallerWrapper calls `controller.AcquireObject({clientA, idA})` — ref count on Client A: 2.
   b. Puts `RpcObjectReference{clientA, idA}` in arguments.
   c. Sends to Client C.
4. Client C's objectCallback receives `RpcObjectReference{clientA, idA}`.
5. Client C creates a proxy with that reference.
6. Client C calls methods on F **directly to Client A** (not through Client B).
7. When Client B releases: `ReleaseObject({clientA, idA})` → Client A ref count: 1.
8. When Client C releases: `ReleaseObject({clientA, idA})` → Client A ref count: 0.

**Key Points:**
- The `RpcObjectReference` always points to the OWNER (Client A).
- Client C communicates directly with Client A, not through Client B.
- Reference counting is per-owner: Client A tracks how many remote clients hold references.
- No token translation needed — `RpcObjectReference` is globally meaningful.

## Scenario 12: Object Lifecycle and Reference Counting

**Lifecycle of a remote interface object:**

1. **Registration:** Callee calls `RegisterObject(obj)` → `RpcObjectReference{calleeClient, id}`. The controller holds a strong reference to the object.
2. **Export:** objectCallback returns the reference (after calling `AcquireObject`). Remote ref count = 1.
3. **Usage:** Caller uses the reference for method calls, property access, etc.
4. **Release:** Caller's proxy destructor calls `ReleaseObject(ref)`. Remote ref count = 0.
5. **Cleanup:** When remote ref count is 0 and the controller determines no local references remain, the object may be removed from the table.

**Convention for returning references:**
- When objectCallback returns an `RpcObjectReference` in a method result, it calls `AcquireObject` first. The caller inherits the reference and is responsible for `ReleaseObject`.
- When passing a reference in arguments, the sender calls `AcquireObject`. The receiver calls `ReleaseObject` when done.

**Multiple holders:**
- If three clients hold the same reference: ref count = 3.
- Each independently calls `ReleaseObject` when done.
- Only when ALL have released does ref count reach 0.

**Smart pointer integration (C++/Workflow):**
- Proxies are stored in `Ptr<T>` / `T^`.
- When the last `Ptr<Proxy>` is destroyed, the proxy destructor calls `controller.ReleaseObject(ref)`.
- This is automatic and transparent.

## Scenario 13: Non-RefCounted Language Support (TypeScript, Python)

**Problem:** TypeScript and Python use garbage collection, not reference counting. Proxy objects may not be destroyed promptly.

**Solutions available within the controller interface:**

1. **Explicit dispose pattern:** Generated proxy classes include a `dispose()` / `close()` method that calls `ReleaseObject`. Codegen generates `with` (Python) or `using` (TS) support.
2. **Weak reference + GC hook:** TS/Python runtimes support weak reference callbacks. When GC collects a proxy, the callback calls `ReleaseObject`. Delayed but automatic.
3. **Session-based cleanup:** When a client disconnects, all references it holds are released. The controller tracks which clients hold which references.
4. **Batch release:** Language bindings can batch `ReleaseObject` calls periodically.

**The controller interface does NOT need modification.** `AcquireObject` and `ReleaseObject` are explicit primitives. How and when they are called depends on the language binding.

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
   - Registers `simpleObj`: `RegisterObject(obj)` → `RpcObjectReference{clientA, newId}`.
   - Acquires: `AcquireObject(newRef)`.
   - Calls `controller.InvokeEvent(ref, EVENT_OBJECTUPDATED, [cast object newRef])`.
3. Controller routes notification to Client B.
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
2. The caller knows from metadata that the real type is `IDerived`.
3. The caller creates a new `IDerivedCallerProxy(controller, ref)` using the same ref.
4. No controller method is needed — casting is a local operation on the proxy.

**Casting from `IDerived^` to `IBase^`:**
Same ref, different wrapper. Local operation, no remote call.

**Conclusion:** Casting is handled entirely by the CallerWrapper. The controller is type-agnostic. The same `RpcObjectReference` works regardless of which interface view is used.

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
    var controller : IWorkflowRpcController^;
    var ref : RpcObjectReference;

    func GetCount() : int { return controller.DictGetCount(ref); }
    func Get(key : string) : string { return cast string controller.DictGet(ref, cast object key); }
    func Set(key : string, value : string) : void { controller.DictSet(ref, cast object key, cast object value); }
    func Remove(key : string) : bool { return controller.DictRemove(ref, cast object key); }
    func ContainsKey(key : string) : bool { return controller.DictContainsKey(ref, cast object key); }
    func GetKeys() : string[] { return cast string[] controller.DictGetKeys(ref); }
    func GetValues() : string[] { return cast string[] controller.DictGetValues(ref); }
    // destructor calls controller.ReleaseObject(ref)
}
```

**Conclusion:** Dictionary operations use `IWorkflowRpcListOps.Dict*` methods. Same pattern as list proxies. Handled by the standard implementation.

## Scenario 17: Same-Process Optimization

**Goal:** When caller and callee are in the same process, avoid protocol overhead.

**The controller interface does not change.** The optimization is in the implementation:

1. Caller calls `controller.InvokeMethod(ref, methodId, args)`.
2. Controller sees `ref.clientId` == local client.
3. For interface operations: calls `objectCallback.InvokeMethod(ref, methodId, args)` directly.
4. For list operations: accesses the registered list object directly.
5. No serialization, no protocol, no network. Returns result directly.

This applies to ALL operations: method calls, property access, events, list/dict operations.

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

All handled by `IWorkflowRpcObjectOps.GetProperty` and `SetProperty`. The CallerWrapper decides which properties have setters and which have events. The objectCallback generated for auto properties reads/writes backing fields.

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

**Goal:** When items in the byref list change on the callee side, the caller receives change notifications via `IWorkflowRpcListEventOps`.

**Flow:**
1. Caller gets a byref list proxy via `controller.GetProperty(ref, PROP_ITEMS)`.
2. The callee's controller monitors the registered observable list for changes (standard implementation).
3. When an item is inserted on Client A, the standard implementation calls `controller.OnListItemInserted(listRef, index, value)`.
4. Controller routes the notification to Client B.
5. Client B's standard `IWorkflowRpcListEventOps` implementation receives `OnListItemInserted(listRef, index, value)`.
6. Updates the local byref list proxy state or fires local observable events.
7. Additionally, the interface-level `ItemsChanged` event fires through `IWorkflowRpcObjectEventOps.InvokeEvent`.

**Conclusion:** `IWorkflowRpcListEventOps` provides fine-grained collection change notifications. Both sides use the standard implementation. Interface-level events (`ItemsChanged`) are handled separately by `IWorkflowRpcObjectEventOps`.
