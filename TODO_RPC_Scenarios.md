# Workflow RPC Controller - Scenario Analysis

This document proves that the controller interface design in [TODO_RPC_Definition.md](./TODO_RPC_Definition.md) can support all RPC scenarios.

## Architecture Overview

The communication chain is:
```
Caller -> CallerWrapper -> IWorkflowRpcController -> protocol -> IWorkflowRpcController -> IWorkflowRpcInterfaceDispatcher -> Callee
```

- **Caller**: application code that calls typed interface methods.
- **CallerWrapper**: generated per-type proxy implementing the `@rpc:Interface`. Translates typed calls to untyped controller calls.
- **IWorkflowRpcController**: the controller interface. All values are `object`. Manages tokens, routing, and lifecycle.
- **protocol**: serialization layer (JSON, binary, etc.). Not defined by the controller interfaces.
- **IWorkflowRpcInterfaceDispatcher**: generated per-type dispatcher on the callee side. Translates untyped controller calls back to typed calls on the real object.
- **Callee**: the real interface implementation.

Each client (process) has one `IWorkflowRpcController` instance. The controller handles both outgoing calls (as a caller) and incoming calls (as a callee). A client can be both caller and callee simultaneously.

### Object Token System

Every remote object (interface or byref collection) gets a local **object token** (an `int`). The token is only meaningful within one client's controller. Different clients have independent token spaces.

When an object reference crosses process boundaries, it is converted to an `RpcObjectReference` struct containing `(clientId, objectId)`. This struct is serializable and globally meaningful.

- `ResolveToken(objectToken) : RpcObjectReference` converts a local token to a global reference.
- `ImportReference(ref) : int` converts a global reference to a local token (also acquires a remote reference).

### RpcObjectReference in Arguments

The controller's `InvokeMethod` takes `object[]` arguments. The controller does **not** inspect or translate argument values. Instead:

- CallerWrapper converts interface arguments to `RpcObjectReference` structs before passing them.
- CalleeDispatcher detects `RpcObjectReference` structs in arguments and converts them back to local tokens/proxies.

This keeps the controller type-agnostic. The wrappers know the types and handle conversions.

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
    var objectToken : int;

    override func Add(a : int, b : int) : int
    {
        var args = {cast object a, cast object b};
        var result = controller.InvokeMethod(objectToken, METHOD_ADD, args);
        return cast int result;
    }
}
```

**Flow:**
1. CallerWrapper calls `controller.InvokeMethod(objectToken, METHOD_ADD, [3, 5])`.
2. Controller resolves `objectToken` -> `RpcObjectReference{clientA, objId}`.
3. Controller sends request to Client A: invoke method METHOD_ADD on objId with args [3, 5].
4. Client A's controller looks up objId -> finds `IWorkflowRpcInterfaceDispatcher`.
5. Client A's controller calls `dispatcher.InvokeMethod(METHOD_ADD, [3, 5])`.
6. Dispatcher calls `realObject.Add(3, 5)` -> returns `8`.
7. Dispatcher returns `cast object 8`.
8. Client A's controller sends response: `8`.
9. Client B's controller receives response, returns `cast object 8` from `InvokeMethod`.
10. CallerWrapper casts result to `int` -> returns `8`.

**Conclusion:** `InvokeMethod` with primitive arguments and return types works through the controller.

## Scenario 2: Async Method Call

**Interface:**
```
@rpc:Interface
interface ICompute
{
    func LongRunning(input : string) : system::Async^;
}
```

**Goal:** Caller awaits an async method on a remote object.

**CallerWrapper:**
```
override func LongRunning(input : string) : system::Async^
{
    var args = {cast object input};
    return controller.InvokeMethodAsync(objectToken, METHOD_LONGRUNNING, args);
}
```

**Flow:**
1. CallerWrapper calls `controller.InvokeMethodAsync(objectToken, METHOD_LONGRUNNING, ["hello"])`.
2. Controller creates an `IFuture`, sends async request to callee.
3. Returns the `IFuture` as `system::Async^` immediately.
4. Callee's controller calls `dispatcher.InvokeMethod(METHOD_LONGRUNNING, ["hello"])`.
5. Dispatcher calls `realObject.LongRunning("hello")` which returns `IAsync^`.
6. Callee's controller detects the `IAsync^` result (the request was marked async), waits for its completion.
7. When the `IAsync` completes with a result value, callee's controller sends the result back.
8. Caller's controller receives the response, calls `promise.SendResult(value)` on the `IFuture`.
9. Caller's coroutine resumes with the result.

**Conclusion:** `InvokeMethodAsync` returns an `IFuture` immediately. The controller handles async completion on the callee side and promise resolution on the caller side.

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
    return cast string controller.GetProperty(objectToken, PROP_NAME);
}
override func SetName(value : string) : void
{
    controller.SetProperty(objectToken, PROP_NAME, cast object value);
}
```

**Flow (Get):**
1. CallerWrapper calls `controller.GetProperty(objectToken, PROP_NAME)`.
2. Controller sends property-get request to callee.
3. Callee's controller calls `dispatcher.GetProperty(PROP_NAME)`.
4. Dispatcher calls `realObject.GetName()` -> `"Alice"`.
5. Returns through the chain.

**Flow (Set):**
1. CallerWrapper calls `controller.SetProperty(objectToken, PROP_NAME, "Bob")`.
2. Controller sends property-set request to callee.
3. Callee's controller calls `dispatcher.SetProperty(PROP_NAME, "Bob")`.
4. Dispatcher calls `realObject.SetName("Bob")`.

**Regarding @rpc:Cached and @rpc:Dynamic:**
- `@rpc:Cached`: CallerWrapper maintains a local cache. On cache miss, calls `controller.GetProperty`. On associated event, clears cache.
- `@rpc:Dynamic`: CallerWrapper always calls `controller.GetProperty`.
- The controller interface does not distinguish these. Caching is a CallerWrapper responsibility.

**Conclusion:** `GetProperty` and `SetProperty` handle property access. Caching semantics are handled by the CallerWrapper.

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

**CallerWrapper subscribes:**
```
// During proxy initialization
var handler = new EventHandlerImpl(func(args : object[]) : void
{
    raise NameChanged();  // raise local event on the proxy
});
var subId = controller.SubscribeEvent(objectToken, EVENT_NAMECHANGED, handler);
```

**Flow (Subscribe):**
1. CallerWrapper calls `controller.SubscribeEvent(objectToken, EVENT_NAMECHANGED, handler)`.
2. Caller's controller sends subscribe request to Client A.
3. Client A's controller calls `dispatcher.SubscribeEvent(EVENT_NAMECHANGED, bridgeHandler)`.
   - `bridgeHandler` is created by Client A's controller. When invoked, it sends an event notification back to Client B.
4. Dispatcher subscribes to `realObject.NameChanged` and forwards to `bridgeHandler`.
5. Returns subscription ID.

**Flow (Event Fires):**
1. `realObject.SetName("Charlie")` triggers `NameChanged()` on Client A.
2. Dispatcher's local handler calls `bridgeHandler.Invoke([])`.
3. `bridgeHandler` sends event notification to Client B through the protocol.
4. Client B's controller receives notification, invokes `handler.Invoke([])`.
5. CallerWrapper's handler raises the local `NameChanged` event.
6. Caller's event listeners are notified.

**Flow (Unsubscribe):**
1. CallerWrapper calls `controller.UnsubscribeEvent(subId)`.
2. Caller's controller sends unsubscribe request to Client A.
3. Client A's controller calls `dispatcher.UnsubscribeEvent(subId)`.
4. Dispatcher detaches from the real event.

**Conclusion:** `SubscribeEvent`, `UnsubscribeEvent`, and `IWorkflowRpcEventHandler` enable remote event subscription. The bridge handlers are internal to the controller implementation.

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

**CalleeDispatcher on Client A:**
```
override func InvokeMethod(methodId : int, arguments : object[]) : object
{
    if (methodId == METHOD_GETSIMPLE)
    {
        var result : ISimple^ = realObject.GetSimple();
        var dispatcher = new SimpleDispatcher(controller, result);
        var token = controller.RegisterInterfaceObject(dispatcher);
        var ref = controller.ResolveToken(token);
        return cast object ref;  // Return RpcObjectReference
    }
}
```

**CallerWrapper on Client B:**
```
override func GetSimple() : ISimple^
{
    var result = controller.InvokeMethod(objectToken, METHOD_GETSIMPLE, {});
    var ref = cast RpcObjectReference result;
    var token = controller.ImportReference(ref);
    return new SimpleCallerProxy(controller, token);
}
```

**Flow:**
1. CallerWrapper calls `controller.InvokeMethod(objectToken, METHOD_GETSIMPLE, [])`.
2. Callee's dispatcher gets the real `ISimple^` object.
3. Dispatcher registers it: `RegisterInterfaceObject(dispatcher)` -> token T.
4. Resolves: `ResolveToken(T)` -> `RpcObjectReference{clientA, objectId}`.
5. Returns `RpcObjectReference` through the protocol.
6. CallerWrapper receives `RpcObjectReference`.
7. Imports: `ImportReference(ref)` -> local token T2 (also acquires reference on Client A).
8. Creates `SimpleCallerProxy(controller, T2)`.
9. Returns the typed proxy to the caller.

**Conclusion:** `RegisterInterfaceObject`, `ResolveToken`, and `ImportReference` enable interface instances to cross process boundaries. The `RpcObjectReference` struct is serializable as a regular struct.

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
    // value could be a local object OR a remote proxy
    var ref : RpcObjectReference;
    if (value is SimpleCallerProxy)  // already a remote proxy
    {
        ref = controller.ResolveToken((cast SimpleCallerProxy^ value).objectToken);
    }
    else  // local object, register it
    {
        var dispatcher = new SimpleDispatcher(controller, value);
        var token = controller.RegisterInterfaceObject(dispatcher);
        ref = controller.ResolveToken(token);
    }
    var args = {cast object ref};
    controller.InvokeMethod(objectToken, METHOD_SETSIMPLE, args);
}
```

**CalleeDispatcher on Client A:**
```
override func InvokeMethod(methodId : int, arguments : object[]) : object
{
    if (methodId == METHOD_SETSIMPLE)
    {
        var ref = cast RpcObjectReference arguments[0];
        var token = controller.ImportReference(ref);
        var proxy = new SimpleCallerProxy(controller, token);
        realObject.SetSimple(proxy);
        return null;
    }
}
```

**Flow:**
1. CallerWrapper converts the `ISimple^` to an `RpcObjectReference`.
   - If it's a local object, registers it first.
   - If it's already a remote proxy, resolves its existing token.
2. The reference `{clientB, objectId}` is serialized and sent to Client A.
3. Callee's dispatcher receives the reference, calls `ImportReference` -> local token.
4. Creates a proxy to call back to Client B when the callee accesses the `ISimple^`.

**Conclusion:** Interface instances can be passed as parameters. The CallerWrapper handles the conversion to `RpcObjectReference`, and the CalleeDispatcher handles the import. Bidirectional communication is natural in P2P.

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

**CalleeDispatcher:**
```
override func InvokeMethod(methodId : int, arguments : object[]) : object
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
    var result = controller.InvokeMethod(objectToken, METHOD_GETNAMES, {});
    return cast string[] result;
}
```

**Flow:**
1. CallerWrapper calls `InvokeMethod`.
2. Callee returns the entire list as a serialized `object`.
3. Protocol serializes: `["Alice", "Bob", "Charlie"]`.
4. Caller receives deserialized list.
5. CallerWrapper casts to `string[]` and returns.

**Conclusion:** Byval collections are simple passthroughs. The entire collection is serialized and no remote reference is created. The controller does not need any special handling.

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

**Goal:** Caller receives a proxy to a remote collection. All operations on the proxy go through the controller.

**CalleeDispatcher:**
```
override func InvokeMethod(methodId : int, arguments : object[]) : object
{
    if (methodId == METHOD_GETSCORES)
    {
        var list = realObject.GetScores();
        var token = controller.RegisterList(cast object list);
        var ref = controller.ResolveToken(token);
        return cast object ref;  // return reference, not the list
    }
}
```

**CallerWrapper:**
```
override func GetScores() : int[]
{
    var result = controller.InvokeMethod(objectToken, METHOD_GETSCORES, {});
    var ref = cast RpcObjectReference result;
    var token = controller.ImportReference(ref);
    return new ByrefIntListProxy(controller, token);
}
```

**ByrefIntListProxy (generated, implements `int[]` interface):**
```
class ByrefIntListProxy
{
    var controller : IWorkflowRpcController^;
    var objectToken : int;

    func GetCount() : int
    {
        return controller.ListGetCount(objectToken);
    }
    func Get(index : int) : int
    {
        return cast int controller.ListGet(objectToken, index);
    }
    func Set(index : int, value : int) : void
    {
        controller.ListSet(objectToken, index, cast object value);
    }
    // ... etc
}
```

**Flow:**
1. Callee registers the local list with `RegisterList`, gets token.
2. Resolves to `RpcObjectReference`, returns through protocol.
3. Caller imports reference, gets local token.
4. Caller creates `ByrefIntListProxy` wrapping the token.
5. Every list operation on the proxy calls the controller's `ListGet`/`ListSet`/etc.
6. Controller routes to the callee, where the real list is accessed.

**Conclusion:** Byref collections use the same token/reference system as interface objects. The controller's `List*` and `Dict*` methods provide the operations. No difference in lifecycle management.

## Scenario 9: @rpc:Byref Collection as Parameter

**Interface:**
```
@rpc:Interface
interface IMethodParams
{
    func Process(@rpc:Byval items : int[], @rpc:Byref refs : string[]) : void;
}
```

**Goal:** Caller passes a byref collection parameter. During execution, the callee operates on the caller's collection remotely.

**CallerWrapper:**
```
override func Process(items : int[], refs : string[]) : void
{
    // items: @rpc:Byval -> serialize entire list
    var arg0 = cast object items;

    // refs: @rpc:Byref -> register locally, send reference
    var refsToken = controller.RegisterList(cast object refs);
    var refsRef = controller.ResolveToken(refsToken);
    var arg1 = cast object refsRef;

    controller.InvokeMethod(objectToken, METHOD_PROCESS, {arg0, arg1});
}
```

**CalleeDispatcher:**
```
override func InvokeMethod(methodId : int, arguments : object[]) : object
{
    if (methodId == METHOD_PROCESS)
    {
        var items = cast int[] arguments[0];                // byval: direct value
        var refsRef = cast RpcObjectReference arguments[1]; // byref: reference
        var refsToken = controller.ImportReference(refsRef);
        var refsProxy = new ByrefStringListProxy(controller, refsToken);
        realObject.Process(items, refsProxy);
        controller.ReleaseObject(refsToken);  // done with the byref param
        return null;
    }
}
```

**Flow:**
1. Caller registers its local `string[]` as a byref list, gets a reference.
2. Sends the byval list data and the byref reference to the callee.
3. Callee imports the byref reference -> local token.
4. Callee creates a proxy for the remote list.
5. When `realObject.Process` accesses `refs` (e.g., `refs.Get(0)`), the proxy calls `controller.ListGet(token, 0)`.
6. The callee's controller sends a list-get request **back to the caller** (bidirectional P2P).
7. The caller's controller finds the local list, returns the value.
8. After `Process` completes, the callee releases the reference.

**Note:** During execution of `Process`, the callee makes calls back to the caller. The controller must handle concurrent incoming and outgoing requests (not block on waiting for a response while ignoring other requests). This is an implementation requirement, not an interface design concern.

**Conclusion:** Byref parameters create bidirectional communication. The same token/reference mechanism works for both directions.

## Scenario 10: Constructor Creation (@rpc:Ctor)

**Interface:**
```
@rpc:Interface
@rpc:Ctor
interface IWithCtor
{
    func GetValue() : int;
}
```

**Goal:** Client B requests creation of an `IWithCtor` instance. Client A, which registered the constructor, creates it.

**Setup on Client A (callee):**
```
var factory = new WithCtorFactory();
controller.RegisterConstructor(TYPE_IWITHCTOR, factory);
```

**IWorkflowRpcObjectFactory implementation:**
```
class WithCtorFactory : IWorkflowRpcObjectFactory
{
    override func Create(controller : IWorkflowRpcController^) : IWorkflowRpcInterfaceDispatcher^
    {
        var realObject = CreateWithCtorImpl();  // application-specific creation
        return new WithCtorDispatcher(controller, realObject);
    }
}
```

**CallerWrapper on Client B:**
```
// Static factory method
static func CreateIWithCtor(controller : IWorkflowRpcController^) : IWithCtor^
{
    var token = controller.CreateObject(TYPE_IWITHCTOR);
    return new WithCtorCallerProxy(controller, token);
}
```

**Flow:**
1. Client B calls `controller.CreateObject(TYPE_IWITHCTOR)`.
2. Client B's controller knows (from server registration) that Client A provides `IWithCtor`.
3. Sends create request to Client A.
4. Client A's controller calls `factory.Create(controller)` -> gets dispatcher.
5. Client A's controller registers the dispatcher -> token T.
6. Resolves to `RpcObjectReference{clientA, T}`, sends back.
7. Client B's controller receives reference, creates local token.
8. Returns the local token to the caller.
9. CallerWrapper creates a proxy.

**Conclusion:** `RegisterConstructor` and `CreateObject` together with `IWorkflowRpcObjectFactory` handle the `@rpc:Ctor` pattern. The factory receives the controller so it can register sub-objects if needed.

## Scenario 11: P2P Object Passing Between Multiple Clients

**Setup:**
- Client A creates `IFoo` object F.
- Client B gets a proxy to F.
- Client B passes F to Client C in a method call.

**Flow:**
1. Client A registers F: dispatcher D, token TA.
2. Client B has imported F: local token TB, which maps to `RpcObjectReference{clientA, idA}`.
3. Client B passes F to Client C:
   a. CallerWrapper calls `controller.ResolveToken(TB)` -> `RpcObjectReference{clientA, idA}`.
   b. Puts the `RpcObjectReference` in arguments.
   c. Sends to Client C.
4. Client C's dispatcher receives `RpcObjectReference{clientA, idA}`.
5. Client C calls `controller.ImportReference({clientA, idA})` -> local token TC.
   - This triggers `AcquireObject` on Client A for idA (ref count: 2).
6. Client C creates a proxy with token TC.
7. Client C now calls methods on F **directly to Client A** (not through Client B).
8. When Client B releases TB: `ReleaseObject(TB)` -> Client A ref count: 1.
9. When Client C releases TC: `ReleaseObject(TC)` -> Client A ref count: 0.

**Key Points:**
- The `RpcObjectReference` always points to the OWNER (Client A).
- Client C communicates directly with Client A, not through Client B.
- Reference counting is per-owner: Client A tracks how many remote clients hold references.
- The token `TB` on Client B and `TC` on Client C are independent local tokens, but both map to the same object on Client A.

**Conclusion:** The token/reference system naturally supports P2P multi-hop object passing. `RpcObjectReference` always identifies the owner, enabling direct communication.

## Scenario 12: Object Lifecycle and Reference Counting

**Goal:** Ensure objects are properly cleaned up when no longer referenced.

**Lifecycle of a remote interface object:**

1. **Registration:** Callee calls `RegisterInterfaceObject(dispatcher)` -> token T. Remote ref count = 0. The dispatcher holds the real object.
2. **Export:** Object reference `RpcObjectReference{calleeClient, T}` is sent to a caller.
3. **Import:** Caller calls `ImportReference(ref)` -> local token. This sends `AcquireObject` to the callee. Remote ref count = 1.
4. **Usage:** Caller uses the local token for method calls, property access, etc.
5. **Release:** Caller drops all proxies. Proxy destructors call `ReleaseObject(localToken)`. Controller sends release to callee. Remote ref count = 0.
6. **Cleanup:** When remote ref count reaches 0, the callee's controller may remove the token (if the callee no longer references the dispatcher either).

**Multiple holders:**
- If three clients import the same object: ref count = 3.
- Each independently calls `ReleaseObject` when done.
- Only when ALL have released does ref count reach 0.

**Smart pointer integration (C++/Workflow):**
- Proxies are stored in `Ptr<T>` / `T^`.
- When the last `Ptr<Proxy>` is destroyed, the proxy destructor calls `controller.ReleaseObject(token)`.
- This is automatic and transparent.

**Conclusion:** `AcquireObject` / `ReleaseObject` provide distributed reference counting. Smart pointer destructors trigger release automatically.

## Scenario 13: Non-RefCounted Language Support (TypeScript, Python)

**Problem:** TypeScript and Python use garbage collection, not reference counting. Proxy objects may not be destroyed promptly.

**Solutions available within the controller interface:**

1. **Explicit dispose pattern:** Generated proxy classes in TS/Python include a `dispose()` or `close()` method that calls `ReleaseObject`. The codegen can generate `with` statement support (Python) or `using` pattern (TS).

2. ** weak reference + GC hook:** TS/Python runtimes support weak reference callbacks. When the GC collects a proxy, the weak ref callback calls `ReleaseObject`. This may be delayed but is automatic.

3. **Session-based cleanup:** When a client disconnects, the server/other clients release ALL references held by that client. The controller implementation tracks which tokens were imported by which client.

4. **Batch release:** The controller's `ReleaseObject` is called once per token. TS/Python bindings can batch releases periodically for efficiency.

**The controller interface does NOT need modification for this.** `AcquireObject` and `ReleaseObject` are explicit primitives. How and when they are called depends on the language binding, not the controller interface.

**Conclusion:** The explicit acquire/release model accommodates any language. Language bindings choose the strategy (explicit, GC-hooked, or session-based) without affecting the controller interface.

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

**Flow when event fires on the callee:**
1. Real object fires `ObjectUpdated(simpleObj)`.
2. Dispatcher's event handler:
   - Registers `simpleObj`: `RegisterInterfaceObject(dispatcher)` -> token.
   - Resolves: `ResolveToken(token)` -> `RpcObjectReference`.
   - Calls `bridgeHandler.Invoke([cast object ref])`.
3. Controller sends event notification with `[RpcObjectReference{calleeClient, id}]`.
4. Caller's controller invokes `handler.Invoke([cast object ref])`.
5. CallerWrapper's handler:
   - Detects `RpcObjectReference` in args.
   - `ImportReference(ref)` -> local token.
   - Creates proxy.
   - Raises local event: `raise ObjectUpdated(proxy)`.

**Conclusion:** Interface arguments in events work through the same `RpcObjectReference` mechanism. The event handler pattern (`IWorkflowRpcEventHandler.Invoke(object[])`) is generic enough to carry any argument types.

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

**The dispatcher for IDerived handles all methods, including inherited ones.**

**Casting from `IBase^` to `IDerived^`:**
1. The caller has an `IBase^` proxy with token T.
2. The caller knows from metadata that the real type is `IDerived`.
3. The caller creates a new `IDerivedCallerProxy(controller, T)` using the same token.
4. No controller method is needed — casting is a local operation on the proxy.

**Casting from `IDerived^` to `IBase^`:**
1. Same token, different wrapper. Create `IBaseCallerProxy(controller, T)`.
2. Local operation, no remote call.

**Conclusion:** Casting is handled entirely by the CallerWrapper. The controller's token system is type-agnostic. The same token works regardless of which interface view is used.

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

**Goal:** Caller receives a proxy to a remote dictionary.

**CalleeDispatcher:** Same pattern as byref list (Scenario 8), using `RegisterDictionary` instead of `RegisterList`.

**ByrefDictionaryProxy (generated):**
```
class ByrefStringStringDictProxy
{
    var controller : IWorkflowRpcController^;
    var objectToken : int;

    func GetCount() : int { return controller.DictGetCount(objectToken); }
    func Get(key : string) : string { return cast string controller.DictGet(objectToken, cast object key); }
    func Set(key : string, value : string) : void { controller.DictSet(objectToken, cast object key, cast object value); }
    func Remove(key : string) : bool { return controller.DictRemove(objectToken, cast object key); }
    func ContainsKey(key : string) : bool { return controller.DictContainsKey(objectToken, cast object key); }
    func GetKeys() : string[] { return cast string[] controller.DictGetKeys(objectToken); }
    func GetValues() : string[] { return cast string[] controller.DictGetValues(objectToken); }
}
```

**Conclusion:** `Dict*` methods on the controller mirror dictionary operations. The proxy pattern is identical to lists.

## Scenario 17: Same-Process Optimization

**Goal:** When caller and callee are in the same process (e.g., unit testing, or same-thread operation), avoid protocol overhead.

**The controller interface does not change.** The optimization is in the implementation:

1. Caller calls `controller.InvokeMethod(token, methodId, args)`.
2. Controller detects that `token` is a LOCAL object (registered via `RegisterInterfaceObject`).
3. Controller directly calls `dispatcher.InvokeMethod(methodId, args)`.
4. No serialization, no protocol, no network.
5. Returns result directly.

This applies to ALL controller operations: method calls, property access, events, list/dict operations.

**Conclusion:** The token system enables transparent same-process optimization. The caller does not know (or care) whether the target is local or remote.

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
- `ReadOnly`: Get property only (no set through RPC, value is set internally).
- `NoObserve`: Get/Set property, no change event.
- `ConstNoObserve`: Get property only, no change event.

**All are handled by `GetProperty` and `SetProperty`.** The CallerWrapper decides:
- Which properties have setters (calls `SetProperty`).
- Which properties have events (calls `SubscribeEvent`).

**The dispatcher side:** Generated getter/setter implementations that read/write auto-property backing fields.

**Conclusion:** Auto properties decompose to property get/set and events, all already supported by the controller interface.
