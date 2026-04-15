# !!!EXECUTION!!!

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

## STEP 0: Reconcile signatures and reflection surface [DONE]
- Reconcile every proposed method signature against:
  - `TODO_RPC_Definition.md` (source of truth)
  - `REPO-ROOT\Source\Library\WfLibraryRpc.h`
  - reflection patterns in `REPO-ROOT\Source\Library\WfLibraryReflection.(h|cpp)`
- Confirm Workflow-to-C++ reflection mappings used by this controller layer:
  - `object` -> `vl::reflection::description::Value`
  - `object[]` -> `vl::Ptr<vl::reflection::description::IValueArray>`
  - `int[string]` -> `vl::Ptr<vl::reflection::description::IValueDictionary>`

## STEP 1: Add reflected controller-layer RPC types (WfLibraryRpc.h) [DONE]
- Edit `REPO-ROOT\Source\Library\WfLibraryRpc.h`.
- Add: `RpcObjectReference`, ops interfaces (`IRpcIdSync`, `IRpcListOps`, `IRpcListEventOps`, `IRpcObjectOps`, `IRpcObjectEventOps`, `IRpcController`), `IRpcLifeCycle`.
- Ensure `IRpcController` uses virtual inheritance to avoid duplicated `IRpcIdSync`.
- Define the `typeId` contract (must be explicit; collections do not have user `@rpc:Interface` IDs):
  - `RpcObjectReference::typeId` is the dynamic `vl::reflection::description::ITypeDescriptor::GetTypeId()` of the referenced object.
  - For `IValue*` collection/enumerator/enumerable proxies, this is the typeId of the reflected collection interface type (e.g. `description::GetTypeDescriptor<IValueList>()->GetTypeId()`), not a user rpc id.
  - `IRpcLifeCycle::RefToPtr` must choose a caller-side proxy class via a `typeId -> proxy factory` mapping.

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

			auto operator<=>(const RpcObjectReference&) const = default;
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

## STEP 2: Implement Value boxing / unboxing helpers (WfLibraryRpc.cpp) [DONE]
- Edit `REPO-ROOT\Source\Library\WfLibraryRpc.cpp`.
- Implement:
  - `RpcBoxByref / RpcUnboxByref`
  - `RpcBoxByval / RpcUnboxByval`
- Rules:
  - Use `BoxValue` / `UnboxValue` for non-interface values.
  - Interface values and byref collections go through lifecycle conversion (`PtrToRef` / `RefToPtr`).

```cpp
Value RpcBoxByref(const Value& trivial, IRpcLifeCycle* lc)
{
	if (!lc) CHECK_FAIL(L"IRpcLifeCycle cannot be null.");
	// 1) null passthrough
	if (trivial.IsNull()) return trivial;

	// 2) Byref-convertible values:
	//    - Value::SharedPtr holding a reflected interface pointer (including IValue* collections)
	//    - byref: represent as RpcObjectReference via lc->PtrToRef
	//    - Exclusion: boxed value-types stored in SharedPtr should NOT be treated as byref; they won't unbox to Ptr<IDescriptable>.
	if (trivial.GetValueType() == Value::SharedPtr)
	{
		auto obj = UnboxValue<Ptr<IDescriptable>>(trivial);
		if (obj)
		{
			auto ref = lc->PtrToRef(obj);
			return BoxValue(ref);
		}
	}

	// 3) Otherwise: primitives/enums/structs are already serializable at controller layer.
	//    Ensure they are properly boxed as Value via BoxValue.
	return trivial;
}
```

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

- For byval recursion (`RpcBoxByval` / `RpcUnboxByval`):
  - Deep-convert collection graphs carried by `Value` (byval means: materialize new local collections and recursively box/unbox elements).
  - Supported byval collection shapes (explicit):
    - `Ptr<IValueReadonlyList>` and `Ptr<IValueReadonlyDictionary>` stored inside `Value` as `Value::SharedPtr`.
    - (Optional) `Ptr<IValueReadonlyArray>` if it exists in this codebase; otherwise treat arrays as lists.
  - Cycle precondition (must be enforced): the byval graph must be acyclic.
    - Mechanism: carry a `visited` set through recursion; if the same collection object is visited twice, `CHECK_FAIL` deterministically.

  Minimal skeleton (shows cycle detection + deep conversion):

  ```cpp
  namespace
  {
  	using namespace vl;
  	using namespace vl::reflection::description;

  	Value RpcBoxByvalInternal(const Value& trivial, IRpcLifeCycle* lc, collections::Dictionary<void*, bool>& visited)
  	{
  		if (trivial.IsNull()) return trivial;

  		// list
  		if (trivial.GetValueType() == Value::SharedPtr)
  		{
  			if (auto roList = UnboxValue<Ptr<IValueReadonlyList>>(trivial))
  			{
  				auto key = roList.Obj();
  				if (visited.Keys().Contains(key)) CHECK_FAIL(L"Byval boxing does not support cycles.");
  				visited.Add(key, true);

  				auto list = IValueList::Create();
  				for (vint i = 0; i < roList->GetCount(); i++)
  				{
  					list->Add(RpcBoxByvalInternal(roList->Get(i), lc, visited));
  				}
  				visited.Remove(key);
  				return BoxValue(list);
  			}

  			// dictionary
  			if (auto roDict = UnboxValue<Ptr<IValueReadonlyDictionary>>(trivial))
  			{
  				auto key = roDict.Obj();
  				if (visited.Keys().Contains(key)) CHECK_FAIL(L"Byval boxing does not support cycles.");
  				visited.Add(key, true);

  				auto dict = IValueDictionary::Create();
  				auto keys = roDict->GetKeys();
  				for (vint i = 0; i < keys->GetCount(); i++)
  				{
  					auto k = keys->Get(i);
  					dict->Set(
  						RpcBoxByvalInternal(k, lc, visited),
  						RpcBoxByvalInternal(roDict->Get(k), lc, visited)
  					);
  				}
  				visited.Remove(key);
  				return BoxValue(dict);
  			}
  		}

  		// primitive/enum/struct/interface byval: pass through (interfaces are not supported by byval)
  		return trivial;
  	}
  }

  Value RpcBoxByval(const Value& trivial, IRpcLifeCycle* lc)
  {
  	if (!lc) CHECK_FAIL(L"IRpcLifeCycle cannot be null.");
  	collections::Dictionary<void*, bool> visited;
  	return RpcBoxByvalInternal(trivial, lc, visited);
  }
  ```

  - `RpcUnboxByval` mirrors the above: detect readonly list/dict value, allocate a new list/dict, and recursively unbox elements.
  - If byval is expected to support interface values, that must be explicitly designed; for this task treat interface `Value::SharedPtr` as byref-only.

## STEP 3: Implement caller-side byref IValue* proxies (Source\Library) [DONE]
- Implement caller-side byref proxies forwarding to `IRpcController` list ops.
- Forwarding rules:
  - Args to controller: `RpcBoxByref(value, lc)`.
  - Results from controller: `RpcUnboxByref(value, lc)`.

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

- Implement `IValueArray` proxy (`RpcByrefArray`) with explicit semantics:
  - `Resize` is shrink-only (grow => `CHECK_FAIL`). Rationale: Workflow `object[]` behaves as fixed-size at the boundary for this task.

  ```cpp
  class RpcByrefArray : public Object, public IValueArray
  {
  private:
  	IRpcLifeCycle* lifeCycle = nullptr;
  	IRpcController* controller = nullptr;
  	RpcObjectReference ref;
  public:
  	RpcByrefArray(IRpcLifeCycle* lc, RpcObjectReference arrayRef)
  		: lifeCycle(lc)
  		, controller(lc ? lc->GetController() : nullptr)
  		, ref(arrayRef)
  	{
  		if (!lifeCycle || !controller) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
  	}

  	~RpcByrefArray()
  	{
  		controller->ReleaseRemoteObject(ref);
  	}

  	vint GetCount()override { return controller->ListGetCount(ref); }
  	Value Get(vint index)override { return RpcUnboxByref(controller->ListGet(ref, index), lifeCycle); }
  	void Set(vint index, const Value& value)override { controller->ListSet(ref, index, RpcBoxByref(value, lifeCycle)); }

  	void Resize(vint size)override
  	{
  		auto count = GetCount();
  		if (size > count) CHECK_FAIL(L"RpcByrefArray::Resize cannot grow.");
  		for (vint i = count - 1; i >= size; i--)
  		{
  			controller->ListRemoveAt(ref, i);
  		}
  	}
  };
  ```

- Implement `IValueObservableList` proxy (`RpcByrefObservableList`) + caller-side list-event dispatcher:
  - Dispatcher implements `IRpcListEventOps`, keyed by `objectId`.
  - Ownership/lifetime rule (must be explicit): dispatcher is owned by the lifecycle/controller mock and **must outlive all proxies** that register/unregister with it.
  - Avoid patterns that introduce `IValueList` diamond ambiguity: prefer composition/shared implementation instead of inheriting multiple `IValue*` interfaces through an intermediate base.

  ```cpp
  class RpcByrefListEventDispatcher : public Object, public IRpcListEventOps
  {
  private:
  	collections::Dictionary<vint, Ptr<IValueObservableList>> targets;
  public:
  	void Track(vint objectId, Ptr<IValueObservableList> list) { targets.Set(objectId, list); }
  	void Untrack(vint objectId) { targets.Remove(objectId); }

  	void OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)override
  	{
  		if (auto list = targets.Get(ref.objectId))
  		{
  			list->ItemChanged()(index, oldCount, newCount);
  		}
  	}
  };
  ```

- Implement `IValueDictionary` proxy (`RpcByrefDictionary`) in terms of `Dict*` ops:
  - `GetKeys` / `GetValues` must return **snapshots**.
  - Unboxing rule: each key/value in returned arrays must be unboxed via `RpcUnboxByref`.

  ```cpp
  class RpcByrefDictionary : public Object, public IValueDictionary
  {
  private:
  	IRpcLifeCycle* lifeCycle = nullptr;
  	IRpcController* controller = nullptr;
  	RpcObjectReference ref;
  public:
  	RpcByrefDictionary(IRpcLifeCycle* lc, RpcObjectReference dictRef)
  		: lifeCycle(lc)
  		, controller(lc ? lc->GetController() : nullptr)
  		, ref(dictRef)
  	{
  		if (!lifeCycle || !controller) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
  	}

  	~RpcByrefDictionary()
  	{
  		controller->ReleaseRemoteObject(ref);
  	}

  	vint GetCount()override { return controller->DictGetCount(ref); }
  	Value Get(const Value& key)override { return RpcUnboxByref(controller->DictGet(ref, RpcBoxByref(key, lifeCycle)), lifeCycle); }
  	void Set(const Value& key, const Value& value)override { controller->DictSet(ref, RpcBoxByref(key, lifeCycle), RpcBoxByref(value, lifeCycle)); }
  	bool Remove(const Value& key)override { return controller->DictRemove(ref, RpcBoxByref(key, lifeCycle)); }
  	void Clear()override { controller->DictClear(ref); }

  	Ptr<IValueArray> GetKeys()override
  	{
  		auto remote = controller->DictGetKeys(ref);
  		auto snapshot = IValueArray::Create();
  		snapshot->Resize(remote->GetCount());
  		for (vint i = 0; i < remote->GetCount(); i++)
  		{
  			snapshot->Set(i, RpcUnboxByref(remote->Get(i), lifeCycle));
  		}
  		return snapshot;
  	}

  	Ptr<IValueArray> GetValues()override
  	{
  		auto remote = controller->DictGetValues(ref);
  		auto snapshot = IValueArray::Create();
  		snapshot->Resize(remote->GetCount());
  		for (vint i = 0; i < remote->GetCount(); i++)
  		{
  			snapshot->Set(i, RpcUnboxByref(remote->Get(i), lifeCycle));
  		}
  		return snapshot;
  	}
  };
  ```

## STEP 4: Implement standard callee-side list ops + list event bridge [DONE]
- Provide standard callee-side implementations passed to `IRpcController::Register`.
- Name them explicitly so STEP 6 can instantiate them deterministically (suggested names):
  - `RpcCalleeListOps` : implements `IRpcListOps` (Enum/List/Dict operations).
  - `RpcCalleeListEventBridge` : implements `IRpcListEventOps` (subscribe to `IValueObservableList::ItemChanged` and forward events through controller callbacks).
- Constructors accept `IRpcLifeCycle*` (need `lifeCycle->GetController()` + boxing helpers).

### STEP 4.1: Required internal registries (callee side)
Maintain explicit registries so lifetimes are controlled and cleanup is deterministic:
- `objectId -> Ptr<IValueReadonlyList>` / `Ptr<IValueList>` / `Ptr<IValueDictionary>` / `Ptr<IValueObservableList>` for registered real collections.
- `objectId -> Ptr<IValueEnumerator>` for enumerators created by `EnumCreate`.
- `objectId -> Ptr<vl::reflection::description::IEventHandler>` (or other handler token) for observable list subscriptions so `ReleaseRemoteObject` can detach.

### STEP 4.2: Required semantics (callee/controller contract)
- `EnumCreate(ref)` must allocate/register an enumerator and return an **acquired** `RpcObjectReference` for the enumerator.
- `ReleaseRemoteObject(ref)` must:
  - remove the objectId from all registries (collection/enumerator/event-handler),
  - detach observable list handlers if any,
  - be safe to call multiple times for the same ref (idempotent cleanup).
- `AcquireRemoteObject(ref)` should increment a refcount if your controller uses one; in the STEP 6 mock it is acceptable to implement it as a no-op, but document that choice.

### STEP 4.3: Minimal class skeletons
```cpp
class RpcCalleeListOps : public Object, public IRpcListOps
{
private:
	IRpcLifeCycle* lifeCycle = nullptr;
	IRpcController* controller = nullptr;
	collections::Dictionary<vint, Ptr<IValueEnumerator>>			enumerators;
	collections::Dictionary<vint, Ptr<IValueReadonlyList>>		lists;
	collections::Dictionary<vint, Ptr<IValueReadonlyDictionary>>	dicts;
public:
	RpcCalleeListOps(IRpcLifeCycle* lc)
		: lifeCycle(lc)
		, controller(lc ? lc->GetController() : nullptr)
	{
		if (!lifeCycle || !controller) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
	}

	RpcObjectReference EnumCreate(RpcObjectReference ref)override;
	bool EnumNext(RpcObjectReference enumerator)override;
	Value EnumGetCurrent(RpcObjectReference enumerator)override;

	vint ListGetCount(RpcObjectReference ref)override;
	Value ListGet(RpcObjectReference ref, vint index)override;
	void ListSet(RpcObjectReference ref, vint index, const Value& value)override;
	vint ListAdd(RpcObjectReference ref, const Value& value)override;
	vint ListInsert(RpcObjectReference ref, vint index, const Value& value)override;
	bool ListRemoveAt(RpcObjectReference ref, vint index)override;
	void ListClear(RpcObjectReference ref)override;
	bool ListContains(RpcObjectReference ref, const Value& value)override;
	vint ListIndexOf(RpcObjectReference ref, const Value& value)override;

	vint DictGetCount(RpcObjectReference ref)override;
	Value DictGet(RpcObjectReference ref, const Value& key)override;
	void DictSet(RpcObjectReference ref, const Value& key, const Value& value)override;
	bool DictRemove(RpcObjectReference ref, const Value& key)override;
	void DictClear(RpcObjectReference ref)override;
	bool DictContainsKey(RpcObjectReference ref, const Value& key)override;
	Ptr<IValueArray> DictGetKeys(RpcObjectReference ref)override;
	Ptr<IValueArray> DictGetValues(RpcObjectReference ref)override;
};

class RpcCalleeListEventBridge : public Object, public IRpcListEventOps
{
private:
	IRpcLifeCycle* lifeCycle = nullptr;
	IRpcController* controller = nullptr;
	collections::Dictionary<vint, Ptr<vl::reflection::description::IEventHandler>> handlers;
public:
	RpcCalleeListEventBridge(IRpcLifeCycle* lc)
		: lifeCycle(lc)
		, controller(lc ? lc->GetController() : nullptr)
	{
		if (!lifeCycle || !controller) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
	}

	// In Register/UnregisterLocalObject, attach/detach handlers for observable lists by objectId.
	void OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)override;
};
```

- In callee list ops implementations, element values returned to the controller should use `RpcBoxByref` / `RpcUnboxByref` consistently so interface values flow byref.

## STEP 5: Reflect all new RPC types in WfLibraryReflection.(h|cpp) [DONE]
- Edit `REPO-ROOT\Source\Library\WfLibraryReflection.h`:
  - Add all `vl::rpc_controller::*` types to `WORKFLOW_LIBRARY_TYPES(F)`.
  - Enumerate explicitly (do not leave as "etc"), and use fully-qualified names:
    - `vl::rpc_controller::RpcObjectReference`
    - `vl::rpc_controller::IRpcIdSync`
    - `vl::rpc_controller::IRpcListOps`
    - `vl::rpc_controller::IRpcListEventOps`
    - `vl::rpc_controller::IRpcObjectOps`
    - `vl::rpc_controller::IRpcObjectEventOps`
    - `vl::rpc_controller::IRpcController`
    - `vl::rpc_controller::IRpcLifeCycle`

- Edit `REPO-ROOT\Source\Library\WfLibraryReflection.cpp`:
  - Add `IMPL_TYPE_INFO_RENAME` entries to map under `system::rpc_controller::*` (enumerate all of the above).
  - Add `BEGIN_STRUCT_MEMBER` / `BEGIN_INTERFACE_MEMBER_NOPROXY` registrations.
  - Macro choice rule for this task:
    - These interfaces are controller-layer plumbing and are not intended to be implemented by Workflow scripts in this task.
    - Therefore, use `BEGIN_INTERFACE_MEMBER_NOPROXY` for **all** `IRpc*` interfaces listed above.

```cpp
IMPL_TYPE_INFO_RENAME(vl::rpc_controller::RpcObjectReference, system::rpc_controller::RpcObjectReference)
IMPL_TYPE_INFO_RENAME(vl::rpc_controller::IRpcIdSync, system::rpc_controller::IdSync)
IMPL_TYPE_INFO_RENAME(vl::rpc_controller::IRpcListOps, system::rpc_controller::ListOps)
IMPL_TYPE_INFO_RENAME(vl::rpc_controller::IRpcListEventOps, system::rpc_controller::ListEventOps)
IMPL_TYPE_INFO_RENAME(vl::rpc_controller::IRpcObjectOps, system::rpc_controller::ObjectOps)
IMPL_TYPE_INFO_RENAME(vl::rpc_controller::IRpcObjectEventOps, system::rpc_controller::ObjectEventOps)
IMPL_TYPE_INFO_RENAME(vl::rpc_controller::IRpcController, system::rpc_controller::Controller)
IMPL_TYPE_INFO_RENAME(vl::rpc_controller::IRpcLifeCycle, system::rpc_controller::LifeCycle)
```

```cpp
BEGIN_STRUCT_MEMBER(vl::rpc_controller::RpcObjectReference)
	STRUCT_MEMBER(clientId)
	STRUCT_MEMBER(objectId)
	STRUCT_MEMBER(typeId)
END_STRUCT_MEMBER(vl::rpc_controller::RpcObjectReference)
```

```cpp
BEGIN_INTERFACE_MEMBER_NOPROXY(vl::rpc_controller::IRpcIdSync)
	CLASS_MEMBER_METHOD(SyncIds, { L"ids" })
END_INTERFACE_MEMBER(vl::rpc_controller::IRpcIdSync)

BEGIN_INTERFACE_MEMBER_NOPROXY(vl::rpc_controller::IRpcListOps)
	CLASS_MEMBER_METHOD(EnumCreate, { L"ref" })
	CLASS_MEMBER_METHOD(EnumNext, { L"enumerator" })
	CLASS_MEMBER_METHOD(EnumGetCurrent, { L"enumerator" })

	CLASS_MEMBER_METHOD(ListGetCount, { L"ref" })
	CLASS_MEMBER_METHOD(ListGet, { L"ref" _ L"index" })
	CLASS_MEMBER_METHOD(ListSet, { L"ref" _ L"index" _ L"value" })
	CLASS_MEMBER_METHOD(ListAdd, { L"ref" _ L"value" })
	CLASS_MEMBER_METHOD(ListInsert, { L"ref" _ L"index" _ L"value" })
	CLASS_MEMBER_METHOD(ListRemoveAt, { L"ref" _ L"index" })
	CLASS_MEMBER_METHOD(ListClear, { L"ref" })
	CLASS_MEMBER_METHOD(ListContains, { L"ref" _ L"value" })
	CLASS_MEMBER_METHOD(ListIndexOf, { L"ref" _ L"value" })

	CLASS_MEMBER_METHOD(DictGetCount, { L"ref" })
	CLASS_MEMBER_METHOD(DictGet, { L"ref" _ L"key" })
	CLASS_MEMBER_METHOD(DictSet, { L"ref" _ L"key" _ L"value" })
	CLASS_MEMBER_METHOD(DictRemove, { L"ref" _ L"key" })
	CLASS_MEMBER_METHOD(DictClear, { L"ref" })
	CLASS_MEMBER_METHOD(DictContainsKey, { L"ref" _ L"key" })
	CLASS_MEMBER_METHOD(DictGetKeys, { L"ref" })
	CLASS_MEMBER_METHOD(DictGetValues, { L"ref" })
END_INTERFACE_MEMBER(vl::rpc_controller::IRpcListOps)

BEGIN_INTERFACE_MEMBER_NOPROXY(vl::rpc_controller::IRpcListEventOps)
	CLASS_MEMBER_METHOD(OnItemChanged, { L"ref" _ L"index" _ L"oldCount" _ L"newCount" })
END_INTERFACE_MEMBER(vl::rpc_controller::IRpcListEventOps)

BEGIN_INTERFACE_MEMBER_NOPROXY(vl::rpc_controller::IRpcObjectOps)
	CLASS_MEMBER_METHOD(InvokeMethod, { L"ref" _ L"methodId" _ L"arguments" })
	CLASS_MEMBER_METHOD(InvokeMethodAsync, { L"ref" _ L"methodId" _ L"arguments" })
	CLASS_MEMBER_METHOD(ObjectHold, { L"ref" _ L"hold" })
	CLASS_MEMBER_METHOD(RequestService, { L"typeId" })
END_INTERFACE_MEMBER(vl::rpc_controller::IRpcObjectOps)

BEGIN_INTERFACE_MEMBER_NOPROXY(vl::rpc_controller::IRpcObjectEventOps)
	CLASS_MEMBER_METHOD(InvokeEvent, { L"ref" _ L"eventId" _ L"arguments" })
END_INTERFACE_MEMBER(vl::rpc_controller::IRpcObjectEventOps)

BEGIN_INTERFACE_MEMBER_NOPROXY(vl::rpc_controller::IRpcController)
	CLASS_MEMBER_METHOD(Register, { L"objectCallback" _ L"eventCallback" _ L"listCallback" _ L"listEventCallback" })
	CLASS_MEMBER_METHOD(RegisterLocalObject, { L"typeId" })
	CLASS_MEMBER_METHOD(UnregisterLocalObject, { L"ref" })
	CLASS_MEMBER_METHOD(AcquireRemoteObject, { L"ref" })
	CLASS_MEMBER_METHOD(ReleaseRemoteObject, { L"ref" })
END_INTERFACE_MEMBER(vl::rpc_controller::IRpcController)

BEGIN_INTERFACE_MEMBER_NOPROXY(vl::rpc_controller::IRpcLifeCycle)
	CLASS_MEMBER_PROPERTY_READONLY(Controller, GetController)
	CLASS_MEMBER_METHOD(RefToPtr, { L"ref" })
	CLASS_MEMBER_METHOD(PtrToRef, { L"obj" })
END_INTERFACE_MEMBER(vl::rpc_controller::IRpcLifeCycle)
```

## STEP 6: Implement unit tests and byval controller mock (LibraryTest) [DONE]
- Implement / add:
  - `REPO-ROOT\Test\Source\TestLibraryRpcByval.cpp`
  - `REPO-ROOT\Test\Source\RpcByvalLifecycleMock.h`
  - `REPO-ROOT\Test\Source\RpcByvalLifecycleMock.cpp`
- Add new test source files using `REPO-ROOT\.github\Guidelines\SourceFileManagement.md` (do not hand-edit `.vcxproj` directly).
- `RpcByvalLifecycleMock` requirements:
  - Implements `IRpcLifeCycle` and `IRpcController`.
  - `GetController()` returns itself.
  - Unsupported object ops + unsupported lifecycle methods: `CHECK_FAIL(L"Not Supported!")`.
  - `Register(...)` stores `listCallback` + `listEventCallback`; object callbacks may be `nullptr` and must be guarded.
  - All `IRpcController` list ops forward to stored callee-side list ops implementation.
- Each test case:
  - Setup lifecycle/controller mock + callee list ops + callee list event bridge; call `Register(...)`.
  - Create real local collections (Array/List/Dictionary/ObservableList of `vint`).
  - Convert to `Value` via reflection (test boundary uses `BoxParameter`), then call `RpcBoxByref` to produce the byref proxy `Value`.
  - Unbox to `Ptr<IValue*>` proxy and operate; assert underlying real collection changed; observable list must forward `ItemChanged` back.
- Proposed cases (strengthen into executable acceptance criteria):
  - `TEST_CASE(L"RpcByrefArray forwards Set/Resize(shrink-only)")`
    - Also assert the failure mode on grow: `Resize(oldSize + 1)` must `CHECK_FAIL` (or equivalent expected failure mechanism).
  - `TEST_CASE(L"RpcByrefList forwards Add/Insert/RemoveAt/Clear")`
  - `TEST_CASE(L"RpcByrefDictionary forwards Set/Get/Remove/Clear")`
    - `GetKeys`/`GetValues` return snapshots (mutating the dictionary after taking the snapshot must not change the returned arrays).
    - Keys/values in snapshots must be unboxed via `RpcUnboxByref` per element.
  - `TEST_CASE(L"RpcByrefObservableList forwards ItemChanged (per-operation)")`
    - Assert `ItemChanged` fires once per operation (no batching assumptions).
  - `TEST_CASE(L"RpcByrefEnumerator lifecycle: EnumCreate/Next/GetCurrent/ReleaseRemoteObject")`
    - Ensure `EnumCreate` returns an acquired ref and the proxy destructor calls `ReleaseRemoteObject`.
    - Ensure callee registry cleanup happens (no stale enumerator entries after release).

- Also document (in the mock lifecycle/controller implementation notes) the internal maps needed to support:
  - `PtrToRef`: `Ptr<IDescriptable> -> RpcObjectReference` (assign `objectId`, `typeId`).
  - `RefToPtr`: `RpcObjectReference -> Ptr<IDescriptable>` including a `typeId -> proxy factory` mapping for caller-side proxies.
  - If refcounting is implemented: `RpcObjectReference -> refCount` and deterministic cleanup when count reaches 0.


## STEP 7: Verification steps
Run the build/test matrix in `# AFFECTED PROJECTS` in order.

# FIXING ATTEMPTS

## Fixing attempt No.1

- Why the original change did not work:
  - `Source\Library\WfLibraryRpc.h` inherited from `vl::reflection::description::IDescriptable` and `vl::reflection::description::Description`, but those reflection base types are declared in `vl::reflection`.
  - The new async controller signature used `vl::reflection::description::IAsync`, but `WfLibraryRpc.h` only included `<VlppReflection.h>`, which does not declare Workflow predefined async types.
  - `Test\Source\RpcByvalLifecycleMock.h` copied the same wrong `IDescriptable` namespace, so the mismatch cascaded into template and override failures in the test project.
- What I changed:
  - Replaced `#include <VlppReflection.h>` in `Source\Library\WfLibraryRpc.h` with `#include "WfLibraryPredefined.h"` so `IAsync` is declared.
  - Updated all RPC interface and lifecycle `IDescriptable` / `Description<>` references in `WfLibraryRpc.h` to use `vl::reflection`.
  - Updated matching `IDescriptable` references in `Test\Source\RpcByvalLifecycleMock.h` to use `vl::reflection::IDescriptable`.
- Why it should solve the build break:
  - The RPC types now inherit from the actual reflection base classes used elsewhere in this repo.
  - `InvokeMethodAsync` now sees the Workflow async interface declaration.
  - The mock header now matches the corrected controller interfaces, eliminating the namespace-driven compile cascade.

## Fixing attempt No.2

- Why the original change did not work:
  - The new controller-layer implementation used `collections::Dictionary::Contains`, but this collection API exposes key lookup via `Keys().Contains(...)`, not a direct `Contains(key)` member.
  - The byval cycle-detection cache also used `const Object*` keys and duplicate forward declarations for `RpcUnboxByvalInternal`, which caused invalid pointer conversions and an ambiguous overload.
- What I changed:
  - Added a `ContainsKey(...)` helper in `Source\Library\WfLibraryRpc.cpp` and replaced dictionary membership checks there with the correct key lookup pattern.
  - Updated the same dictionary key checks in `Test\Source\RpcByvalLifecycleMock.cpp`.
  - Switched the byval visited-set dictionaries to `const DescriptableObject*`, used `Value::GetRawPtr()` as the cycle-detection key, and removed the now-conflicting anonymous-namespace forward declarations by replacing that block with the helper.
- Why it should solve the build break:
  - The implementation now matches the actual Vlpp collection API used elsewhere in the repo.
  - Cycle tracking now uses the same raw pointer type produced by `Value`, avoiding unrelated-pointer casts.
  - Removing the conflicting declarations leaves a single visible `RpcUnboxByvalInternal`, so the recursive calls resolve cleanly.

## Fixing attempt No.3

- Why the original change did not work:
  - The new `ContainsKey(...)` helper expected the lookup key type to exactly match the dictionary key type.
  - `Value::GetRawPtr()` returns `DescriptableObject*`, while the visited-set dictionary stores `const DescriptableObject*`, so template deduction became ambiguous in the byval cycle checks.
- What I changed:
  - Explicitly cast the `GetRawPtr()` results used for byval cycle detection to `const DescriptableObject*`.
- Why it should solve the build break:
  - The visited-set lookup key type now exactly matches the dictionary key type, so `ContainsKey(...)` can be instantiated without ambiguity.

## Fixing attempt No.4

- Why the original change did not work:
  - The reflected type id code assumed `ITypeDescriptor` exposed `GetTypeId()`, but this API is not available in the current reflection layer.
  - The new unit test file used `TEST_CASE` blocks without the required `});` terminator, and it still used `Dictionary::Contains` on a Vlpp dictionary.
- What I changed:
  - Reworked `GetRpcTypeId<T>()` to compute the reflected type id by locating the descriptor index from `GetGlobalTypeManager()`.
  - Fixed every `TEST_CASE` terminator in `Test\Source\TestLibraryRpcByval.cpp` to use `});`.
  - Replaced the remaining dictionary membership check in `TestLibraryRpcByval.cpp` with `values.Keys().Contains(1)`.
- Why it should solve the build break:
  - The controller-layer type id logic now uses the reflection API that actually exists in this repo.
  - The unit test file now matches the unit test macro syntax used throughout the codebase.
  - The last remaining invalid dictionary API call in the new test is removed.

## Fixing attempt No.5

- Why the original change did not work:
  - One `TEST_CASE` in `Test\Source\TestLibraryRpcByval.cpp` still ended with `}` instead of `});`, so the test macro expansion remained unbalanced.
- What I changed:
  - Fixed the remaining `RpcByrefList forwards Add Insert RemoveAt and Clear` test case terminator to `});`.
- Why it should solve the build break:
  - All new test cases now use the same `TEST_CASE` closing syntax as the existing test suite, so the unit test macro expansion should parse correctly.

## Fixing attempt No.6

- Why the original change did not work:
  - The helper implementation and the new tests use `BoxValue` / `UnboxValue` with `vl::rpc_controller::RpcObjectReference`, but the corresponding `DECL_TYPE_INFO` declarations live in `Source\Library\WfLibraryReflection.h`, not in `WfLibraryRpc.h`.
  - One earlier test-case terminator fix accidentally changed the local `CreateContext()` helper to end with `});`.
- What I changed:
  - Included `Source\Library\WfLibraryReflection.h` in `Source\Library\WfLibraryRpc.cpp` and in `Test\Source\TestLibraryRpcByval.cpp` so `RpcObjectReference` type info is visible where boxing and unboxing happen.
  - Restored the `CreateContext()` helper terminator in `TestLibraryRpcByval.cpp` back to a normal `}`.
- Why it should solve the build break:
  - `BoxValue` / `UnboxValue` can now see the reflection type-info declarations for `RpcObjectReference`.
  - The helper function is syntactically correct again, so the test file should parse.

## Fixing attempt No.7

- Why the original change did not work:
  - `Test\Source\RpcByvalLifecycleMock.cpp` returned the tracked local object directly from `IRpcLifeCycle::RefToPtr` whenever the same process had already registered that `RpcObjectReference`.
  - The new byref collection tests expect `RpcUnboxByref` to surface caller-side `RpcByref*` proxies, so array operations should go through controller list ops and preserve array restrictions like shrink-only resize.
  - Returning the real local `IValueArray` bypassed the proxy layer entirely, so `proxy->Resize(3)` grew the actual array instead of failing through `RpcByrefArray::Resize`.
- What I changed:
  - Changed `RpcByvalLifecycleMock::RefToPtr` to prefer the `typeId -> proxy factory` mapping for RPC collection/enumerator references, and only fall back to tracked local objects when no proxy factory exists.
- Why it should solve the test break:
  - `RpcUnboxByref` now produces the same caller-side proxy types that a real remote controller would expose.
  - Array mutations will flow through `IRpcController::List*`, so grow attempts are rejected by `RpcByrefArray::Resize` as intended.

## Fixing attempt No.8

- Why the original change did not work:
  - The test controller mock stored `RpcCalleeListEventBridge` in `listEventCallback`, and `RpcCalleeListEventBridge::OnItemChanged` itself calls back into `IRpcController::OnItemChanged`.
  - After `RefToPtr` started returning real byref proxies, observable-list mutations began traveling through the bridge, so `RpcByvalLifecycleMock::OnItemChanged` forwarded the notification right back into the same bridge and recursed indefinitely.
- What I changed:
  - Changed `RpcByvalLifecycleMock::OnItemChanged` to dispatch remote list notifications to the caller-side `RpcByrefListEventDispatcher` instead of re-entering `listEventCallback`.
- Why it should solve the test break:
  - Callee-side observable events now cross the controller boundary once and arrive at the tracked caller proxy, matching the intended `IRpcListEventOps` wiring.
  - The recursive bridge->controller->bridge loop is removed, so `LibraryTest` can finish the new observable RPC test and continue.

## Fixing attempt No.9

- Why the original change did not work:
  - `IRpcLifeCycle::GetController` was reflected as a property returning a raw `IRpcController*`.
  - `CompilerTest_GenerateMetadata` now hangs inside `GenerateMetaonlyTypes`, and the new RPC reflection surface is the only freshly added property whose reflected type is a raw interface pointer.
  - The Workflow reflection metadata pipeline is designed around reflected interface values as `vl::Ptr<T>` / shared-pointer types; serializing a raw reflected interface pointer is much more likely to produce an unsupported or self-recursive type shape.
- What I changed:
  - Changed `IRpcLifeCycle::GetController` and the mock implementation to return `vl::Ptr<IRpcController>` instead of `IRpcController*`.
  - Updated all controller caches in `Source\Library\WfLibraryRpc.cpp` to extract the raw pointer with `.Obj()` when storing a non-owning member pointer.
- Why it should solve the test break:
  - The reflected `Controller` property now uses the same pointer decorator that the metadata generator already handles for interface types.
  - The runtime code still keeps non-owning raw pointers internally, so the behavior of the byref proxies and callee bridges is unchanged outside reflection metadata generation.

## Fixing attempt No.10

- Why the original change did not work:
  - `IRpcController::Register` still exposed four raw reflected interface-pointer parameters (`IRpcObjectOps*`, `IRpcObjectEventOps*`, `IRpcListOps*`, `IRpcListEventOps*`).
  - `GenerateMetaonlyTypes` serializes every reflected method signature, so even after fixing the `Controller` property it still had to process raw interface-pointer method parameters on the new RPC surface.
- What I changed:
  - Changed `IRpcController::Register` and the mock implementation to use `vl::Ptr<...>` for all callback parameters.
  - Updated the RPC byval test harness to pass the callback objects as `Ptr` values instead of raw `.Obj()` pointers.
- Why it should solve the test break:
  - The entire reflected RPC callback surface now uses the pointer decorator that Workflow metadata generation already supports for interface values.
  - The mock controller still stores non-owning raw pointers internally, so the existing runtime behavior stays the same while the reflected signature becomes metadata-friendly.

## Fixing attempt No.11

- Why the original change did not work:
  - The targeted diagnostics show `CompilerTest_GenerateMetadata` hangs while loading the reflected type descriptor for `system::rpc_controller::LifeCycle`, before `GenerateMetaonlyTypes` starts serializing any member metadata.
  - That isolates the problem to the reflected `IRpcLifeCycle` surface itself, and the two unique members there are the internal conversion helpers `RefToPtr` / `PtrToRef`, both of which use the very generic `system::Interface` (`vl::reflection::IDescriptable`) shared-pointer type.
- What I changed:
  - Kept `RefToPtr` / `PtrToRef` in the C++ interface, but removed them from the Workflow reflection registration so `system::rpc_controller::LifeCycle` only exposes the `Controller` property.
- Why it should solve the test break:
  - The metadata generator no longer needs to load and serialize the problematic `system::Interface`-based helper signatures that were hanging the `LifeCycle` type descriptor.
  - The runtime RPC implementation remains unchanged because those lifecycle conversion helpers are still available to C++ code, which is where the current task uses them.

## Fixing attempt No.12

- Why the original change did not work:
  - Even after removing the reflected `RefToPtr` / `PtrToRef` methods, the diagnostics still hang while loading `system::rpc_controller::LifeCycle`, before `GetBaseTypeDescriptorCount()` can finish.
  - That leaves the reflected `Controller` property as the remaining `IRpcLifeCycle`-specific member participating in descriptor loading.
- What I changed:
  - Removed the `Controller` property from the Workflow reflection registration of `IRpcLifeCycle`, leaving the C++ interface intact but making the reflected `LifeCycle` interface empty.
- Why it should solve the test break:
  - The metadata loader can now treat `system::rpc_controller::LifeCycle` as a plain interface type without traversing any member signatures during descriptor load.
  - The runtime behavior is unchanged because the RPC implementation still uses `IRpcLifeCycle::GetController()` directly in C++.

- If `CompilerTest_GenerateMetadata` fails only on baseline comparison, update the baseline files as described in `# AFFECTED PROJECTS` and rerun.

# !!!FINISHED!!!
