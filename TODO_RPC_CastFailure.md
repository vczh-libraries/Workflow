# Workflow Interface Based RPC (Cast Failure)

## Summary

The `Dtor2.txt` failure is caused by `vl::reflection::DescriptableObject::SafeAggregationCast<T>` finding more than one valid result when `T` is `vl::reflection::IDescriptable`.

This is not a normal C++ multiple-inheritance ambiguity inside one object. It is an aggregation ambiguity:

- `vl::workflow::runtime::WfInterfaceInstance` directly inherits `vl::reflection::IDescriptable`.
- The same `vl::workflow::runtime::WfInterfaceInstance` also aggregates generated `vl::reflection::description::ValueInterfaceProxy<TInterface>` objects for the interfaces it implements.
- Those generated proxy objects are also `vl::reflection::IDescriptable`.

Therefore a broad cast to `vl::reflection::IDescriptable` can find both the runtime root object and the generated interface proxy object.

## Dtor2 Example

`Test/Resources/Rpc/Dtor2.txt` defines an RPC interface `RpcDtor2Test::IValue` and an RPC constructor interface `RpcDtor2Test::IService`.

The service implementation creates an anonymous Workflow object:

```workflow
override func MakeValue() : IValue^
{
  return new (RpcDtor2Test::IValue^)
  {
    delete
    {
      s = "Deleted";
    }
  };
}
```

This anonymous `RpcDtor2Test::IValue` implementation is the original object involved in the failing cast. At runtime it is represented by a `vl::workflow::runtime::WfInterfaceInstance`.

The client calls:

```workflow
var value = clientObj.MakeValue();
```

The generated wrapper sends an RPC method call to the service side. On the service side, generated object operations dispatch the method id for `RpcDtor2Test::IService.MakeValue`, recover the local service object, and call:

```workflow
target.MakeValue()
```

The return value is the anonymous `RpcDtor2Test::IValue` object above. The generated RPC code then boxes it for transfer, for example the generated assembly contains the shape:

```workflow
system::IRpcLifecycle::RpcBoxByval(target.MakeValue(), _lc)
```

In the failing configuration, when this helper is represented as accepting `system::Interface^`, reflection must convert the Workflow value to the C++ parameter type `vl::Ptr<vl::reflection::IDescriptable>`. The generic shared pointer value accessor performs:

```cpp
value.GetRawPtr()->SafeAggregationCast<T>()
```

with `T = vl::reflection::IDescriptable`. That is where the failure happens.

The object being cast is the `vl::workflow::runtime::WfInterfaceInstance` for the anonymous `RpcDtor2Test::IValue` object returned from `MakeValue`. It is not the service object, the generated wrapper, an RPC object reference, or a JSON node.

## Why SafeAggregationCast Reaches ValueInterfaceProxy

`vl::workflow::runtime::WfInterfaceInstance` is constructed with an `vl::Ptr<vl::reflection::description::IValueInterfaceProxy>`. This proxy is the Workflow VM dispatcher for the implemented methods.

During construction, `vl::workflow::runtime::WfInterfaceInstance::WfInterfaceInstance` creates aggregation parents:

```cpp
InitializeAggregation(baseCtors.Count());
for (auto [ctor, index] : indexed(baseCtors))
{
  auto value = ctor->Invoke(Value(), arguments);
  ...
  SetAggregationParent(index, ptr);
}
```

Each `ctor` is the reflected constructor for a generated interface proxy type. For `RpcDtor2Test::IValue`, it creates a `vl::reflection::description::ValueInterfaceProxy<RpcDtor2Test::IValue>` object and stores the same `IValueInterfaceProxy` in it.

`SafeAggregationCast<T>` starts from the aggregation root, checks `dynamic_cast<T*>(this)`, and then recursively checks every aggregation parent:

```cpp
auto expected = dynamic_cast<T*>(this);
if (expected)
{
  CHECK_ERROR(result == nullptr, L"...Found multiple ways...");
  result = expected;
}
if (IsAggregated())
{
  for (...)
  {
    parent->SafeAggregationCast<T>(result);
  }
}
```

For a concrete interface cast like `RpcDtor2Test::IValue`, this is the intended path:

```text
WfInterfaceInstance                                      no RpcDtor2Test::IValue match
  -> ValueInterfaceProxy<RpcDtor2Test::IValue>           RpcDtor2Test::IValue match
```

For `vl::reflection::IDescriptable`, the same traversal finds two matches:

```text
WfInterfaceInstance                                      IDescriptable match
  -> ValueInterfaceProxy<RpcDtor2Test::IValue>           IDescriptable match
```

The second match triggers the `Found multiple ways to do aggregation cast` error.

## Why WfInterfaceInstance Does Not Need IDescriptable

`vl::workflow::runtime::WfInterfaceInstance` needs to be a reflected runtime object, but it does not need to be a reflected interface.

`vl::reflection::Description<WfInterfaceInstance>` already supplies the `vl::reflection::DescriptableObject` base used by:

- `vl::reflection::description::Value` raw pointer and shared pointer storage.
- Runtime type descriptor access.
- Aggregation root and parent mechanics.
- Reference counting and disposal integration.

The knowledge base describes this distinction: a reflectable class needs `Description<T>`, while `IDescriptable` is required for reflectable interfaces without another interface base. `WfInterfaceInstance` is a runtime class, not one of the user-facing interfaces being implemented.

Workflow interface method dispatch also does not depend on `WfInterfaceInstance` being `IDescriptable`. `vl::workflow::runtime::WfInterfaceMethod::InvokeInternal` casts the receiver to `WfInterfaceInstance` and then invokes the stored VM proxy:

```cpp
auto instance = thisObject.GetRawPtr()->SafeAggregationCast<WfInterfaceInstance>();
return instance->GetProxy()->Invoke(this, IValueList::Create(From(arguments)));
```

The public C++ interface identity is supplied by the generated aggregation parent:

```text
ValueInterfaceProxy<RpcDtor2Test::IValue>
```

That object implements `RpcDtor2Test::IValue` and forwards calls to the stored `IValueInterfaceProxy`. It is the object that should satisfy casts to concrete user interfaces.

This supports changing the runtime shape from:

```cpp
class WfInterfaceInstance
  : public Object
  , public reflection::IDescriptable
  , public reflection::Description<WfInterfaceInstance>
```

to:

```cpp
class WfInterfaceInstance
  : public Object
  , public reflection::Description<WfInterfaceInstance>
```

Under this shape, `WfInterfaceInstance` remains a reflectable runtime object and an aggregation root, but it no longer contributes an extra `IDescriptable` candidate. Concrete interface casts continue to resolve through the generated `ValueInterfaceProxy<TInterface>` aggregation parent.

One remaining rule is still important: `vl::reflection::IDescriptable` is too broad to describe a specific user interface when multiple generated interface proxy parents are present. RPC code should prefer concrete interface types for interface values whenever the static type is known.
