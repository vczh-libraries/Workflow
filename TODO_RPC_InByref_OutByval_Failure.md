# TODO: RPC InByref OutByval JSON Lifetime Failure

## Failing Shape

Representative sample:

- `Test/Generated/RpcMetadata64/Wrapper_Collection_Interface_InByref_OutByval.txt`
- `Test/Generated/RpcMetadata64/Wrapper_Collection_Interface_InByref_OutByval_Json.txt`

The same shape also appears in the 32-bit generated metadata files. The sample passes an interface collection by reference into the remote service and returns a copied collection by value:

```w
func InvokeMethod_RpcCollection__Interface__InByref__OutByval__IService_DoList(
    ref : system::RpcObjectReference,
    arg_xs : ::RpcCollection::Interface::InByref::OutByval::IValue^[]
) : (::RpcCollection::Interface::InByref::OutByval::IValue^[])
```

The returned byval collection contains interface values. During RPC transfer, those interface values become `system::RpcObjectReference` values and need the original wrappers to stay alive until the caller finishes `RpcUnboxByval`.

## Plain Wrapper Path

In `Wrapper_Collection_Interface_InByref_OutByval.txt`, the callee-side object ops returns the result of `RpcBoxByval` directly:

```w
case rpcmethod_RpcCollection__Interface__InByref__OutByval__IService_DoList:
{
    var target : RpcCollection::Interface::InByref::OutByval::IService^ =
        (cast (RpcCollection::Interface::InByref::OutByval::IService^) _lc.RefToPtr(ref));
    return system::IRpcLifecycle::RpcBoxByval(
        target.DoList(
            (cast (::RpcCollection::Interface::InByref::OutByval::IValue^[])
                system::IRpcLifecycle::RpcUnboxByref((cast (system::RpcObjectReference) arguments[0]), _lc))
        ),
        _lc);
}
```

The caller-side ops immediately unboxes the returned object:

```w
return (cast (::RpcCollection::Interface::InByref::OutByval::IValue^[])
    system::IRpcLifecycle::RpcUnboxByval(
        _lc.Dispatcher.SendToClient_ObjectOps(ref.clientId).InvokeMethod(
            ref,
            rpcmethod_RpcCollection__Interface__InByref__OutByval__IService_DoList,
            arguments),
        _lc));
```

This works because `RpcBoxByval` attaches `RpcByvalKeepAlive` to the boxed return container. Since that boxed container is the actual RPC return value, it survives until the caller calls `RpcUnboxByval`.

## JSON Wrapper Path

In `Wrapper_Collection_Interface_InByref_OutByval_Json.txt`, the JSON callee-side object ops has a different lifetime:

```w
case rpcmethod_RpcCollection__Interface__InByref__OutByval__IService_DoList:
{
    var target : RpcCollection::Interface::InByref::OutByval::IService^ =
        (cast (RpcCollection::Interface::InByref::OutByval::IService^) _lc.RefToPtr(ref));
    var jsonValue0 : system::RpcObjectReference =
        rpcjson_Deserialize_Struct_system__RpcObjectReference((cast (system::JsonNode^) arguments[0]));
    var jsonValue1 : object =
        system::IRpcLifecycle::RpcBoxByval(
            target.DoList(
                (cast (::RpcCollection::Interface::InByref::OutByval::IValue^[])
                    system::IRpcLifecycle::RpcUnboxByref((cast (system::RpcObjectReference) jsonValue0), _lc))
            ),
            _lc);
    var jsonNode2 : system::JsonNode^ = rpcjson_Serialize(jsonValue1);
    return jsonNode2;
}
```

The important difference is:

```w
var jsonValue1 : object = system::IRpcLifecycle::RpcBoxByval(...);
var jsonNode2 : system::JsonNode^ = rpcjson_Serialize(jsonValue1);
return jsonNode2;
```

`jsonValue1` is the boxed byval transfer object with `RpcByvalKeepAlive` attached. It is not returned. Only `jsonNode2` is returned.

The caller-side JSON ops later reconstructs ordinary serializable values from the JSON node and then unboxes:

```w
var jsonResult : system::JsonNode^ =
    (cast (system::JsonNode^) _lc.Dispatcher.SendToClient_ObjectOps(ref.clientId).InvokeMethod(
        ref,
        rpcmethod_RpcCollection__Interface__InByref__OutByval__IService_DoList,
        arguments));
var jsonValue1 : object = rpcjson_Deserialize(jsonResult);
return (cast (::RpcCollection::Interface::InByref::OutByval::IValue^[])
    system::IRpcLifecycle::RpcUnboxByval(jsonValue1, _lc));
```

By this point, the callee-side temporary `jsonValue1` from `rpcops_IRpcObjectOpsJson.InvokeMethod` may already be destroyed, so its internal `RpcByvalKeepAlive` may already be gone.

## Why This Fails Only With JSON Serialization

Plain transport returns the boxed byval `object` directly:

```text
RpcBoxByval(...) -> returned object -> caller RpcUnboxByval(...)
```

The keepalive holder stays attached to the returned object through the full transfer.

JSON transport serializes the boxed byval object first:

```text
RpcBoxByval(...) -> rpcjson_Serialize(...) -> returned JsonNode -> caller rpcjson_Deserialize(...) -> caller RpcUnboxByval(...)
```

The `JsonNode` contains `RpcObjectReference` data, but it does not carry the internal `RpcByvalKeepAlive` property attached to the boxed byval container.

When the callee-side temporary boxed object is released too early, its kept-alive wrappers can be released too early. Wrapper deletion sends:

```w
_lc.Dispatcher.SendToClient_ObjectOps(_ref.clientId).ObjectHold(_ref, _lc.ClientId, false);
```

That can drive the original owner through `LocalObjectUnhold`, which may remove the local object registration. Then caller-side `RpcUnboxByval` sees `RpcObjectReference` values reconstructed from JSON and attempts to resolve them back through `RefToPtr`, but the local object has already been unregistered.

The observed failure is therefore consistent with:

```text
RpcUnboxByval -> RpcUnboxByref -> RefToPtr -> Object not registered
```

## Suspected Generator Location

The root cause is likely in the Workflow code that generates the JSON object ops implementation, specifically the implementation that becomes this generated C++ class:

```text
__vwsnc9_Rpc_Collection_Interface_InByref_OutByval_rpcops_IRpcObjectOpsJson__vl_rpc_controller_IRpcObjectOps
```

In the wrapper text this corresponds to:

```w
func rpcops_IRpcObjectOpsJson(lc : system::IRpcLifecycle*) : (system::IRpcObjectOps^)
```

and especially the generated `InvokeMethod` branch for:

```w
rpcmethod_RpcCollection__Interface__InByref__OutByval__IService_DoList
```

The likely source generator area is `Source/Analyzer/WfAnalyzer_GenerateRpc_JsonSerialization.cpp`, where JSON dispatch return values are generated by first computing a transfer value and then serializing it to `system::JsonNode`.

## Fix Direction

Avoid making local object registrations live longer globally. The narrower fix should keep the byval transfer keepalive alive across the JSON serialize/deserialize boundary.

Possible approaches to investigate:

1. Make JSON RPC return keep an owning reference to the original boxed byval transfer object until caller-side unboxing finishes.
2. Transfer explicit keepalive information beside the JSON node when a byval return contains `RpcObjectReference` values.
3. Adjust generated JSON object ops so `RpcByvalKeepAlive` is not lost between `RpcBoxByval` and caller-side `RpcUnboxByval`.

Any fix should preserve the plain transport behavior and should specifically cover samples ending in `_InByref_OutByval` plus `Rpc:DtorPropCachedListByval`.
