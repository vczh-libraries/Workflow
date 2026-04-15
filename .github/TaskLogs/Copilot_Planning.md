# !!!PLANNING!!!

# UPDATES

# AFFECTED PROJECTS
- Build the solution in folder `REPO-ROOT\Test\UnitTest` (`Debug|x64`).
- Always Run UnitTest project `LibraryTest` (`Debug|x64`).
- Always Run UnitTest project `CompilerTest_GenerateMetadata` (`Debug|x64`) because `Source\Library` changes can affect reflected types.
  - If reflection metadata baselines change, update `REPO-ROOT\Test\Resources\Baseline\Reflection64.txt` from `REPO-ROOT\Test\UnitTest\Generated\Reflection64.txt` and rerun.
- Always Run UnitTest project `CompilerTest_LoadAndCompile` (`Debug|x64`).
- Always Run UnitTest project `CppTest` (`Debug|x64`).
- Always Run UnitTest project `CppTest_Metaonly` (`Debug|x64`).
- Always Run UnitTest project `CppTest_Reflection` (`Debug|x64`).

- Build the solution in folder `REPO-ROOT\Test\UnitTest` (`Debug|Win32`).
- Always Run UnitTest project `LibraryTest` (`Debug|Win32`).
- Always Run UnitTest project `CompilerTest_GenerateMetadata` (`Debug|Win32`).
  - If reflection metadata baselines change, update `REPO-ROOT\Test\Resources\Baseline\Reflection32.txt` from `REPO-ROOT\Test\UnitTest\Generated\Reflection32.txt` and rerun.
- Always Run UnitTest project `CppTest` (`Debug|Win32`).
- Always Run UnitTest project `CppTest_Metaonly` (`Debug|Win32`).
- Always Run UnitTest project `CppTest_Reflection` (`Debug|Win32`).

# EXECUTION PLAN

## STEP 0: Reconcile signatures and reflection surface

### What to do
- Before coding, reconcile every proposed method signature against:
  - `TODO_RPC_Definition.md` (source of truth)
  - the current stub in `REPO-ROOT\Source\Library\WfLibraryRpc.h`
  - existing reflection patterns in `REPO-ROOT\Source\Library\WfLibraryReflection.(h|cpp)`
- Confirm Workflow-to-C++ reflection mappings used by this controller layer:
  - `object` -> `vl::reflection::description::Value`
  - `object[]` -> `vl::Ptr<vl::reflection::description::IValueArray>`
  - `int[string]` -> `vl::Ptr<vl::reflection::description::IValueDictionary>`
- If any mapping is not supported as-is in this repo, update the interface signatures (and then update reflection registration + mocks + tests) before implementing code.

### Why
This prevents signature drift and avoids late-stage compile/reflection failures.

## STEP 1: Add reflected controller-layer RPC types (WfLibraryRpc.h)

### What to change
Update `REPO-ROOT\Source\Library\WfLibraryRpc.h` to define the controller-layer RPC surface under `namespace vl::rpc_controller`.

Add:
- `struct RpcObjectReference { vint clientId; vint objectId; vint typeId; };`
- Ops interfaces matching `TODO_RPC_Definition.md`:
  - `IRpcIdSync`
  - `IRpcListOps`
  - `IRpcListEventOps`
  - `IRpcObjectOps` (declared only; no implementations required in this task)
  - `IRpcObjectEventOps` (declared only; no implementations required in this task)
  - `IRpcController`
- `IRpcLifeCycle`

All interfaces should be reflectable (inherit `vl::reflection::description::IDescriptable` and `vl::reflection::description::Description<T>`), and `IRpcController` must use **virtual inheritance** for all base ops interfaces to avoid duplicated `IRpcIdSync` subobjects.

### Why
- `TODO_RPC.md` requires everything in `WfLibraryRpc.h` to be reflected via `WfLibraryReflection.(h|cpp)`.
- `TODO_RPC_Definition.md` defines the ABI between generated typed proxies and the protocol-agnostic controller.
- Virtual inheritance prevents diamond/duplicate base issues when `IRpcController` inherits both `IRpcObjectOps` and `IRpcObjectEventOps` (each inheriting `IRpcIdSync`).

### Proposed code (headers)
(Exact signatures should stay aligned with `TODO_RPC_Definition.md`.)

```cpp
namespace vl
{
	namespace rpc_controller
	{
		struct RpcObjectReference
		{
			vl::vint clientId = 0;
			vl::vint objectId = 0;
			vl::vint typeId   = 0;
		};

		class IRpcIdSync
			: public virtual vl::reflection::description::IDescriptable
			, public vl::reflection::description::Description<IRpcIdSync>
		{
		public:
			// ids : int[string]
			virtual void SyncIds(vl::Ptr<vl::reflection::description::IValueDictionary> ids) = 0;
		};

		class IRpcListOps
			: public virtual vl::reflection::description::IDescriptable
			, public vl::reflection::description::Description<IRpcListOps>
		{
		public:
			virtual RpcObjectReference EnumCreate(RpcObjectReference ref) = 0;
			virtual bool EnumNext(RpcObjectReference enumerator) = 0;
			virtual vl::reflection::description::Value EnumGetCurrent(RpcObjectReference enumerator) = 0;

			virtual vl::vint ListGetCount(RpcObjectReference ref) = 0;
			virtual vl::reflection::description::Value ListGet(RpcObjectReference ref, vl::vint index) = 0;
			virtual void ListSet(RpcObjectReference ref, vl::vint index, const vl::reflection::description::Value& value) = 0;
			virtual vl::vint ListAdd(RpcObjectReference ref, const vl::reflection::description::Value& value) = 0;
			virtual vl::vint ListInsert(RpcObjectReference ref, vl::vint index, const vl::reflection::description::Value& value) = 0;
			virtual bool ListRemoveAt(RpcObjectReference ref, vl::vint index) = 0;
			virtual void ListClear(RpcObjectReference ref) = 0;
			virtual bool ListContains(RpcObjectReference ref, const vl::reflection::description::Value& value) = 0;
			virtual vl::vint ListIndexOf(RpcObjectReference ref, const vl::reflection::description::Value& value) = 0;

			virtual vl::vint DictGetCount(RpcObjectReference ref) = 0;
			virtual vl::reflection::description::Value DictGet(RpcObjectReference ref, const vl::reflection::description::Value& key) = 0;
			virtual void DictSet(RpcObjectReference ref, const vl::reflection::description::Value& key, const vl::reflection::description::Value& value) = 0;
			virtual bool DictRemove(RpcObjectReference ref, const vl::reflection::description::Value& key) = 0;
			virtual void DictClear(RpcObjectReference ref) = 0;
			virtual bool DictContainsKey(RpcObjectReference ref, const vl::reflection::description::Value& key) = 0;

			// object[]
			virtual vl::Ptr<vl::reflection::description::IValueArray> DictGetKeys(RpcObjectReference ref) = 0;
			virtual vl::Ptr<vl::reflection::description::IValueArray> DictGetValues(RpcObjectReference ref) = 0;
		};

		class IRpcObjectOps
			: public virtual IRpcIdSync
			, public vl::reflection::description::Description<IRpcObjectOps>
		{
		public:
			virtual vl::reflection::description::Value InvokeMethod(
				RpcObjectReference ref,
				vl::vint methodId,
				vl::Ptr<vl::reflection::description::IValueArray> arguments
			) = 0;

			virtual vl::Ptr<vl::reflection::description::IAsync> InvokeMethodAsync(
				RpcObjectReference ref,
				vl::vint methodId,
				vl::Ptr<vl::reflection::description::IValueArray> arguments
			) = 0;

			virtual void ObjectHold(RpcObjectReference ref, bool hold) = 0;
			virtual RpcObjectReference RequestService(vl::vint typeId) = 0;
		};

		class IRpcListEventOps
			: public virtual vl::reflection::description::IDescriptable
			, public vl::reflection::description::Description<IRpcListEventOps>
		{
		public:
			virtual void OnItemChanged(RpcObjectReference ref, vl::vint index, vl::vint oldCount, vl::vint newCount) = 0;
		};

		class IRpcObjectEventOps
			: public virtual IRpcIdSync
			, public vl::reflection::description::Description<IRpcObjectEventOps>
		{
		public:
			virtual void InvokeEvent(
				RpcObjectReference ref,
				vl::vint eventId,
				vl::Ptr<vl::reflection::description::IValueArray> arguments
			) = 0;
		};

		class IRpcController
			: public virtual IRpcListOps
			, public virtual IRpcObjectOps
			, public virtual IRpcListEventOps
			, public virtual IRpcObjectEventOps
			, public vl::reflection::description::Description<IRpcController>
		{
		public:
			// returns ids : int[string]
			virtual vl::Ptr<vl::reflection::description::IValueDictionary> Register(
				IRpcObjectOps* objectCallback,
				IRpcObjectEventOps* eventCallback,
				IRpcListOps* listCallback,
				IRpcListEventOps* listEventCallback
			) = 0;

			virtual RpcObjectReference RegisterLocalObject(vl::vint typeId) = 0;
			virtual void UnregisterLocalObject(RpcObjectReference ref) = 0;

			virtual void AcquireRemoteObject(RpcObjectReference ref) = 0;
			virtual void ReleaseRemoteObject(RpcObjectReference ref) = 0;
		};

		class IRpcLifeCycle
			: public virtual vl::reflection::description::IDescriptable
			, public vl::reflection::description::Description<IRpcLifeCycle>
		{
		public:
			virtual IRpcController* GetController()const = 0;
			virtual vl::Ptr<vl::reflection::description::IDescriptable> RefToPtr(RpcObjectReference ref) = 0;
			virtual RpcObjectReference PtrToRef(vl::Ptr<vl::reflection::description::IDescriptable> obj) = 0;
		};

		vl::reflection::description::Value RpcBoxByref(const vl::reflection::description::Value& trivial, IRpcLifeCycle* lc);
		vl::reflection::description::Value RpcUnboxByref(const vl::reflection::description::Value& serializable, IRpcLifeCycle* lc);
		vl::reflection::description::Value RpcBoxByval(const vl::reflection::description::Value& trivial, IRpcLifeCycle* lc);
		vl::reflection::description::Value RpcUnboxByval(const vl::reflection::description::Value& serializable, IRpcLifeCycle* lc);
	}
}
```

Notes to lock down during implementation:
- Do not use any `using namespace` inside `Source\...\.h` (e.g. `WfLibraryRpc.h`); keep all types fully-qualified.
- Keep Workflow signature intent aligned with `TODO_RPC_Definition.md`:
  - `object` => `vl::reflection::description::Value`
  - `object[]` => `vl::Ptr<vl::reflection::description::IValueArray>`
  - `int[string]` => `vl::Ptr<vl::reflection::description::IValueDictionary>`
- `InvokeMethodAsync` return type should match the repository’s reflected async interface (`vl::reflection::description::IAsync`).

## STEP 2: Implement Value boxing / unboxing helpers (WfLibraryRpc.cpp)

### What to change
Update `REPO-ROOT\Source\Library\WfLibraryRpc.cpp` to implement:
- `RpcBoxByref / RpcUnboxByref`
- `RpcBoxByval / RpcUnboxByval`

Rules (from `TODO_RPC.md` / `Copilot_Task.md`):
- All values passed to `IRpcController` must be **serializable objects** (primitive/enum/struct, `RpcObjectReference`, or byval-recursively boxed collections).
- All values surfaced by byref wrappers / real implementations are **trivial objects**.
- For non-interface values, use `BoxValue` / `UnboxValue` (NOT `BoxParameter` / `UnboxParameter`).
- Interface values (`Value::SharedPtr` to a reflected interface) and byref collections must go through lifecycle conversion (`PtrToRef` / `RefToPtr`).

### Why
These helpers are the single place to enforce the trivial-vs-serializable boundary; all byref wrappers and callee dispatch code should depend on them to avoid drift.

### Proposed implementation outline (key branches)
Pseudo-structure for `RpcBoxByref`:

```cpp
Value RpcBoxByref(const Value& trivial, IRpcLifeCycle* lc)
{
	if (!lc) CHECK_FAIL(L"IRpcLifeCycle cannot be null.");
	// 1) null passthrough
	if (trivial.IsNull()) return trivial;

	// 2) If it is a reflected interface pointer (including IValue* collections):
	//    - byref: represent as RpcObjectReference via lc->PtrToRef
	//    - NOTE: for IValue* collections, this registers the collection as a local object too.
	if (/* trivial is SharedPtr interface */)
	{
		auto obj = UnboxValue<Ptr<IDescriptable>>(trivial);
		auto ref = lc->PtrToRef(obj);
		return BoxValue(ref);
	}

	// 3) Otherwise: primitives/enums/structs are already serializable at controller layer.
	//    Ensure they are properly boxed as Value via BoxValue.
	return trivial;
}
```

Pseudo-structure for `RpcUnboxByref`:

```cpp
Value RpcUnboxByref(const Value& serializable, IRpcLifeCycle* lc)
{
	if (!lc) CHECK_FAIL(L"IRpcLifeCycle cannot be null.");
	if (serializable.IsNull()) return serializable;

	// If it is RpcObjectReference => trivial interface pointer via lc->RefToPtr
	if (serializable.GetTypeDescriptor() == GetTypeDescriptor<RpcObjectReference>())
	{
		auto ref = UnboxValue<RpcObjectReference>(serializable);
		auto obj = lc->RefToPtr(ref);
		return BoxValue(obj);
	}

	// Otherwise pass through (primitives/enums/structs)
	return serializable;
}
```

For byval recursion (`RpcBoxByval` / `RpcUnboxByval`):
- Detect `IValueReadonlyList` / `IValueReadonlyDictionary` inside `Value` (they are trivial objects but need deep conversion).
- Allocate a new list/dictionary (`IValueList::Create()` / `IValueDictionary::Create()` or `IValueArray::Create()` when preserving array-ness is necessary).
- For each element/key/value: recursively call byval boxing/unboxing.
- Precondition: the byval collection graph must be acyclic. If a cycle is detected / observed during boxing, treat it as invalid input and `CHECK_FAIL` (cycle detection is out of scope for this task unless it becomes necessary).

## STEP 3: Implement caller-side byref IValue* proxies (Source\Library)

### What to change
Add concrete classes (likely in new source files under `REPO-ROOT\Source\Library`, or inside `WfLibraryRpc.cpp` if keeping everything local is preferred) implementing:
- `vl::reflection::description::IValueEnumerable`
- `vl::reflection::description::IValueEnumerator`
- `vl::reflection::description::IValueList`
- `vl::reflection::description::IValueArray` (required by tests using `collections::Array<T>`)
- `vl::reflection::description::IValueObservableList`
- `vl::reflection::description::IValueDictionary`

Each proxy constructor takes:
- `IRpcLifeCycle* lifeCycle`
- `RpcObjectReference ref`

Forwarding rules (byref semantics):
- Any `Value` argument going to controller ops must be `RpcBoxByref(value, lc)`.
- Any `Value` result coming back must be `RpcUnboxByref(value, lc)`.

### Why
This is the core required deliverable: byref collection proxies that allow unit tests (and later generated wrappers) to operate on remote collections through `IRpcListOps`.

### Proposed code skeletons
Enumerator proxy (note `IValueEnumerator` has `GetIndex()` but controller enum ops don’t expose it; we track it locally):

```cpp
class RpcByrefEnumerator : public Object, public IValueEnumerator
{
private:
	IRpcLifeCycle* lifeCycle = nullptr;
	IRpcController* controller = nullptr;
	RpcObjectReference ref;
	vint index = -1;
public:
	RpcByrefEnumerator(IRpcLifeCycle* lc, RpcObjectReference enumeratorRef)
		: lifeCycle(lc)
		, controller(lc ? lc->GetController() : nullptr)
		, ref(enumeratorRef)
	{
		if (!lifeCycle || !controller) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
	}

	~RpcByrefEnumerator()
	{
		controller->ReleaseRemoteObject(ref);
	}

	Value GetCurrent()override
	{
		return RpcUnboxByref(controller->EnumGetCurrent(ref), lifeCycle);
	}

	vint GetIndex()override
	{
		return index;
	}

	bool Next()override
	{
		if (controller->EnumNext(ref))
		{
			index++;
			return true;
		}
		return false;
	}
};
```

Enumerable proxy:

```cpp
class RpcByrefEnumerable : public Object, public IValueEnumerable
{
private:
	IRpcLifeCycle* lifeCycle = nullptr;
	IRpcController* controller = nullptr;
	RpcObjectReference ref;
public:
	RpcByrefEnumerable(IRpcLifeCycle* lc, RpcObjectReference enumerableRef)
		: lifeCycle(lc)
		, controller(lc ? lc->GetController() : nullptr)
		, ref(enumerableRef)
	{
		if (!lifeCycle || !controller) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
	}

	~RpcByrefEnumerable()
	{
		controller->ReleaseRemoteObject(ref);
	}

	Ptr<IValueEnumerator> CreateEnumerator()override
	{
		// EnumCreate returns an acquired reference; receiver owns it.
		auto enumRef = controller->EnumCreate(ref);
		return Ptr(new RpcByrefEnumerator(lifeCycle, enumRef));
	}
};
```

List proxy:

```cpp
class RpcByrefList : public Object, public IValueList
{
protected:
	IRpcLifeCycle* lifeCycle = nullptr;
	IRpcController* controller = nullptr;
	RpcObjectReference ref;
public:
	RpcByrefList(IRpcLifeCycle* lc, RpcObjectReference listRef)
		: lifeCycle(lc)
		, controller(lc ? lc->GetController() : nullptr)
		, ref(listRef)
	{
		if (!lifeCycle || !controller) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
	}

	~RpcByrefList()
	{
		controller->ReleaseRemoteObject(ref);
	}

	Ptr<IValueEnumerator> CreateEnumerator()override
	{
		return Ptr(new RpcByrefEnumerator(lifeCycle, controller->EnumCreate(ref)));
	}

	vint GetCount()override { return controller->ListGetCount(ref); }
	Value Get(vint index)override { return RpcUnboxByref(controller->ListGet(ref, index), lifeCycle); }
	bool Contains(const Value& value)override { return controller->ListContains(ref, RpcBoxByref(value, lifeCycle)); }
	vint IndexOf(const Value& value)override { return controller->ListIndexOf(ref, RpcBoxByref(value, lifeCycle)); }

	void Set(vint index, const Value& value)override { controller->ListSet(ref, index, RpcBoxByref(value, lifeCycle)); }
	vint Add(const Value& value)override { return controller->ListAdd(ref, RpcBoxByref(value, lifeCycle)); }
	vint Insert(vint index, const Value& value)override { return controller->ListInsert(ref, index, RpcBoxByref(value, lifeCycle)); }
	bool Remove(const Value& value)override
	{
		// No direct remove-by-value in controller; implement by IndexOf+RemoveAt.
		auto index = IndexOf(value);
		return index == -1 ? false : controller->ListRemoveAt(ref, index);
	}
	bool RemoveAt(vint index)override { return controller->ListRemoveAt(ref, index); }
	void Clear()override { controller->ListClear(ref); }
};
```

Array proxy (`IValueArray`) — required by `TODO_RPC.md` tests:
- Map `Set` to `ListSet`.
- Decide `Resize` semantics now (this task): **shrink-only**.
  - If `size > GetCount()`, `CHECK_FAIL` (grow is out of scope until we have a well-defined default-value creation path).
  - If `size < GetCount()`, call `ListRemoveAt` repeatedly from the end.

Observable list proxy:
- Avoid inheriting from `RpcByrefList` to prevent `IValueList` diamond issues (`IValueObservableList` inherits `public virtual IValueList`).
- Implement `IValueObservableList` directly (with shared helpers/composition for the list operations).
- Event wiring/lifetime:
  - A caller-side dispatcher object implements `IRpcListEventOps` and is owned by the lifecycle/controller layer.
  - The dispatcher is passed to `IRpcController::Register(..., listEventCallback)`.
  - `RpcByrefObservableList` registers itself on construction and unregisters on destruction (objectId-keyed map is sufficient under a single-threaded assumption).
- Threading model (this task): assume list event delivery happens on the same thread that mutates/observes the proxies (no locking). If later required, add synchronization around the dispatcher map.
- Tests must assert per-operation event granularity (do not assume batching).

Dictionary proxy:
- Implement `IValueDictionary` in terms of `Dict*` ops.
- Return `GetKeys` / `GetValues` as snapshot `IValueReadonlyList` (e.g. `IValueArray::Create(...)`) after unboxing each element via `RpcUnboxByref`.

## STEP 4: Implement standard callee-side list ops + list event bridge

### What to change
Provide standard implementations (C++ classes) that can be passed to `IRpcController::Register`:
- `IRpcListOps` implementation: dispatch to real local `IValue*` collections/enumerators.
- `IRpcListEventOps` implementation: attach to real local `IValueObservableList::ItemChanged` and call `controller->OnItemChanged(...)`.

These implementations must accept `IRpcLifeCycle*` in constructors (as required by `TODO_RPC.md`) so they can use:
- `lifeCycle->GetController()` to call controller methods
- `RpcBoxByref` / `RpcUnboxByref` for payload conversion

### Why
Unit tests will construct a controller mock that redirects list ops to these standard implementations, verifying end-to-end behavior (proxy -> controller -> callee ops -> real collection and observable event back).

### Key details to specify
- `DictGetKeys/DictGetValues` return `object[]` at controller layer; represent this as `vl::Ptr<IValueArray>` and treat it as a snapshot array. Callee implementation should snapshot keys/values and box each element via `RpcBoxByref`.
- Enumerator lifecycle must be explicit end-to-end:
  - Callee-side `EnumCreate` must allocate/register an enumerator objectId and return an **acquired** `RpcObjectReference`.
  - Caller-side `RpcByrefEnumerator` releases its `RpcObjectReference` in the destructor.
  - Callee-side enumerator registry cleanup is tied to `IRpcController::ReleaseRemoteObject` for that enumerator reference.
- Observable-list change signature must match `Import\VlppReflection.h`:
  - `ItemChanged(index, oldCount, newCount)`.

## STEP 5: Reflect all new RPC types in WfLibraryReflection.(h|cpp)

### What to change
Update:
- `REPO-ROOT\Source\Library\WfLibraryReflection.h`
  - Add the following to `WORKFLOW_LIBRARY_TYPES(F)` using fully-qualified names:
    - `vl::rpc_controller::RpcObjectReference`
    - `vl::rpc_controller::IRpcIdSync`
    - `vl::rpc_controller::IRpcListOps`
    - `vl::rpc_controller::IRpcListEventOps`
    - `vl::rpc_controller::IRpcObjectOps`
    - `vl::rpc_controller::IRpcObjectEventOps`
    - `vl::rpc_controller::IRpcController`
    - `vl::rpc_controller::IRpcLifeCycle`
- `REPO-ROOT\Source\Library\WfLibraryReflection.cpp`
  - Add `IMPL_TYPE_INFO_RENAME` entries to map them under `system::rpc_controller::*`.
  - Add `BEGIN_STRUCT_MEMBER` / `BEGIN_INTERFACE_MEMBER(_NOPROXY)` blocks.

### Why
`TODO_RPC.md` explicitly requires reflection coverage for everything declared in `WfLibraryRpc.h`.

### Proposed reflection registration snippets
Type rename:

```cpp
IMPL_TYPE_INFO_RENAME(vl::rpc_controller::RpcObjectReference, system::rpc_controller::RpcObjectReference)
IMPL_TYPE_INFO_RENAME(vl::rpc_controller::IRpcController, system::rpc_controller::Controller)
// ... etc
```

Struct:

```cpp
BEGIN_STRUCT_MEMBER(vl::rpc_controller::RpcObjectReference)
	STRUCT_MEMBER(clientId)
	STRUCT_MEMBER(objectId)
	STRUCT_MEMBER(typeId)
END_STRUCT_MEMBER(vl::rpc_controller::RpcObjectReference)
```

Interfaces (argument names are important for Workflow compilation):

```cpp
BEGIN_INTERFACE_MEMBER_NOPROXY(vl::rpc_controller::IRpcListOps)
	CLASS_MEMBER_METHOD(EnumCreate, { L"ref" })
	CLASS_MEMBER_METHOD(EnumNext, { L"enumerator" })
	CLASS_MEMBER_METHOD(EnumGetCurrent, { L"enumerator" })

	CLASS_MEMBER_METHOD(ListGetCount, { L"ref" })
	CLASS_MEMBER_METHOD(ListGet, { L"ref" _ L"index" })
	CLASS_MEMBER_METHOD(ListSet, { L"ref" _ L"index" _ L"value" })
	// ...
END_INTERFACE_MEMBER(vl::rpc_controller::IRpcListOps)

BEGIN_INTERFACE_MEMBER_NOPROXY(vl::rpc_controller::IRpcLifeCycle)
	CLASS_MEMBER_PROPERTY_READONLY(Controller, GetController)
	CLASS_MEMBER_METHOD(RefToPtr, { L"ref" })
	CLASS_MEMBER_METHOD(PtrToRef, { L"obj" })
END_INTERFACE_MEMBER(vl::rpc_controller::IRpcLifeCycle)
```

Use `BEGIN_INTERFACE_MEMBER_NOPROXY` for **all** these controller-layer interfaces in this task:
- They are C++-only plumbing (callbacks are raw pointers in `Register`, and the controller does not own their lifetime).
- `END_INTERFACE_MEMBER_NOPROXY(...)` does not exist; always close with `END_INTERFACE_MEMBER(TYPENAME)`.

## STEP 6: Implement unit tests and byval controller mock (LibraryTest)

### What to change
Implement `REPO-ROOT\Test\Source\TestLibraryRpcByval.cpp` (currently empty) and add:
- `REPO-ROOT\Test\Source\RpcByvalLifecycleMock.h`
- `REPO-ROOT\Test\Source\RpcByvalLifecycleMock.cpp`

Add the new test source files using the repository’s source-file management process (see `REPO-ROOT\.github\Guidelines\SourceFileManagement.md`).
- Do not hand-edit `.vcxproj` directly in the plan; update the appropriate shared items/project definitions the repo uses for test sources so the new `.cpp` is compiled.

Test requirements (from `TODO_RPC.md`):
- `RpcByvalLifecycleMock` implements `IRpcLifeCycle` and `IRpcController` (inherits both).
  - `Controller` property returns itself.
  - Object ops interfaces (`IRpcObjectOps` / `IRpcObjectEventOps`) and unsupported lifecycle methods should `CHECK_FAIL(L"Not Supported!")`.
  - `Register` stores `listCallback` and `listEventCallback` pointers; object/event callbacks may be `nullptr` and must be guarded.
  - All `IRpcController` list ops forward to the stored callee-side list ops implementation.
- Each test case:
  1) Create lifecycle/controller mock + callee list ops + callee list event bridge.
  2) Call `Register(...)`.
  3) Create a real local collection (Array/List/Dictionary/ObservableList of `vint`).
  4) Convert it to `Value` using reflection (via `BoxParameter` at the test boundary), then call `RpcBoxByref` to produce the **byref proxy** value.
  5) Unbox to `Ptr<IValue*>` of the proxy and operate on it.
  6) Assert that the real collection mutated accordingly; for observable list, also verify remote `ItemChanged` is forwarded back.

Proposed test cases in `TestLibraryRpcByval.cpp`:
- `TEST_CASE(L"RpcByrefArray forwards Set/Resize(shrink-only)")`
- `TEST_CASE(L"RpcByrefList forwards Add/Insert/RemoveAt/Clear")`
- `TEST_CASE(L"RpcByrefDictionary forwards Set/Get/Remove/Clear")`
- `TEST_CASE(L"RpcByrefObservableList forwards ItemChanged (per-operation)")`

## STEP 7: Verification steps (to be executed in later phases)

After implementation, run this exact build/test matrix in order:

- Build the solution in folder `REPO-ROOT\Test\UnitTest` (`Debug|x64`).
- Always Run UnitTest project `LibraryTest` (`Debug|x64`).
- Always Run UnitTest project `CompilerTest_GenerateMetadata` (`Debug|x64`).
  - If reflection metadata baselines change, update `REPO-ROOT\Test\Resources\Baseline\Reflection64.txt` from `REPO-ROOT\Test\UnitTest\Generated\Reflection64.txt` and rerun.
- Always Run UnitTest project `CompilerTest_LoadAndCompile` (`Debug|x64`).
- Always Run UnitTest project `CppTest` (`Debug|x64`).
- Always Run UnitTest project `CppTest_Metaonly` (`Debug|x64`).
- Always Run UnitTest project `CppTest_Reflection` (`Debug|x64`).

- Build the solution in folder `REPO-ROOT\Test\UnitTest` (`Debug|Win32`).
- Always Run UnitTest project `LibraryTest` (`Debug|Win32`).
- Always Run UnitTest project `CompilerTest_GenerateMetadata` (`Debug|Win32`).
  - If reflection metadata baselines change, update `REPO-ROOT\Test\Resources\Baseline\Reflection32.txt` from `REPO-ROOT\Test\UnitTest\Generated\Reflection32.txt` and rerun.
- Always Run UnitTest project `CppTest` (`Debug|Win32`).
- Always Run UnitTest project `CppTest_Metaonly` (`Debug|Win32`).
- Always Run UnitTest project `CppTest_Reflection` (`Debug|Win32`).

# !!!FINISHED!!!

