# !!!LEARNING!!!

# Orders

- Use `CLASS_MEMBER_STATIC_EXTERNALMETHOD` for registering free functions as reflection static methods [1]
- Enable Workflow proxy support for all related implementable interfaces consistently [1]
- Workflow `new interface` on reflected C++ interfaces requires proxy registration (`BEGIN_INTERFACE_MEMBER`) [1]
- Workflow static method call syntax uses `::` not `.` [1]
- `BEGIN_INTERFACE_MEMBER` (proxy-enabled) requires `ValueInterfaceProxy<T>` specialization in C++ header [1]
- `WfTopQualifiedType` vs `WfReferenceType`: use `WfTopQualifiedType` only when global `::` qualification is needed [1]
- Workflow collection types (`system::Dictionary^`, `system::Array^`) use constructor-expression `{}` not constructor-call `()` [1]

# Refinements

## Use `CLASS_MEMBER_STATIC_EXTERNALMETHOD` for registering free functions as reflection static methods

When exposing existing namespace-scope free functions (e.g. `vl::rpc_controller::RpcBoxByref`) as static methods on a Workflow type descriptor (e.g. `system::IRpcLifeCycle`), use `CLASS_MEMBER_STATIC_EXTERNALMETHOD` in `WfLibraryReflection.cpp` instead of adding C++ static member wrapper functions to the class declaration. Example:
```cpp
CLASS_MEMBER_STATIC_EXTERNALMETHOD(RpcBoxByref, { L"trivial" _ L"lc" }, Value(*)(const Value&, IRpcLifeCycle*), vl::rpc_controller::RpcBoxByref)
```
This keeps the class declaration clean and avoids code duplication.

## Enable Workflow proxy support for all related implementable interfaces consistently

When adding Workflow proxy support (`BEGIN_INTERFACE_MEMBER` + `BEGIN_INTERFACE_PROXY_SHAREDPTR`) for an interface hierarchy (e.g. RPC callback interfaces), enable it for all leaf/intermediate implementable interfaces consistently, not just the ones immediately needed by the current task. For example, `IRpcListOps` and `IRpcListEventOps` should get proxy support alongside `IRpcObjectOps` and `IRpcObjectEventOps`, even if list-related wrappers are not yet generated. `IRpcController` and `IRpcLifeCycle` remain `NOPROXY` because they are not implemented from Workflow.

## Workflow `new interface` on reflected C++ interfaces requires proxy registration (`BEGIN_INTERFACE_MEMBER`)

Workflow's `new InterfaceType^ { ... }` expression only works on interfaces that have proxy support registered in C++ reflection. If the interface uses `BEGIN_INTERFACE_MEMBER_NOPROXY`, the `new interface` expression will fail with semantic errors like `A6` or `A23`. The fix is to switch the interface to `BEGIN_INTERFACE_MEMBER` and provide a `ValueInterfaceProxy<T>` specialization — not to introduce intermediate Workflow-defined interfaces as a workaround.

## Workflow static method call syntax uses `::` not `.`

In Workflow script, static method calls on type descriptors use `::` syntax, not `.` member-expression syntax. For example, call `system::IRpcLifeCycle::RpcBoxByref(value, lc)` not `system::IRpcLifeCycle.RpcBoxByref(value, lc)`. Using `.` triggers semantic error `A7` (type symbol used as an expression).

## `BEGIN_INTERFACE_MEMBER` (proxy-enabled) requires `ValueInterfaceProxy<T>` specialization in C++ header

Switching a reflected interface from `BEGIN_INTERFACE_MEMBER_NOPROXY` to `BEGIN_INTERFACE_MEMBER` in `WfLibraryReflection.cpp` requires a corresponding `ValueInterfaceProxy<T>` specialization (using `BEGIN_INTERFACE_PROXY_SHAREDPTR` / `INTERFACE_PROXY_FORWARD` macros) in the reflection header (`WfLibraryReflection.h`). Without this, the C++ build fails because the reflection machinery tries to use a missing `Create` member on the proxy template.

## `WfTopQualifiedType` vs `WfReferenceType`: use `WfTopQualifiedType` only when global `::` qualification is needed

`WfTopQualifiedType` emits a leading `::` in printed output, functioning like C++ global scope qualification. Use it when you explicitly need to reference a name from the global scope (e.g. to avoid ambiguity with a local name). In most generated code, `WfReferenceType` is the correct root node for type names, with `WfChildType` chaining for qualified fragments like `system::IRpcObjectOps`. Defaulting to `WfTopQualifiedType` when global qualification is unnecessary will produce names like `::MyType` that may not match what the parser expects in certain contexts.

## Workflow collection types (`system::Dictionary^`, `system::Array^`) use constructor-expression `{}` not constructor-call `()`

In Workflow, `system::Dictionary^` and `system::Array^` are interface types. You cannot use constructor-call syntax like `new (system::Dictionary^)()` because the parser reports `A6` (arguments not allowed in `new interface` expression). Instead, use the collection constructor expression syntax (empty `{}` in AST form). This applies to any generated code that allocates dictionaries or arrays in Workflow wrapper modules.
