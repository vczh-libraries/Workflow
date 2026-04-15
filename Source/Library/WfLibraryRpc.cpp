#include "WfLibraryRpc.h"
#include "WfLibraryReflection.h"

namespace vl
{
	using namespace collections;

	namespace rpc_controller
	{
		using namespace reflection;
		using namespace reflection::description;

		namespace
		{
			template<typename T>
			vint GetRpcTypeId()
			{
#ifndef VCZH_DEBUG_NO_REFLECTION
				if (auto typeDescriptor = GetTypeDescriptor<T>())
				{
					if (auto manager = GetGlobalTypeManager())
					{
						for (vint i = 0; i < manager->GetTypeDescriptorCount(); i++)
						{
							if (manager->GetTypeDescriptor(i) == typeDescriptor)
							{
								return i;
							}
						}
					}
				}
#endif
				if constexpr (std::same_as<T, IValueEnumerable>)
				{
					return RpcTypeId_IValueEnumerable;
				}
				else if constexpr (std::same_as<T, IValueEnumerator>)
				{
					return RpcTypeId_IValueEnumerator;
				}
				else if constexpr (std::same_as<T, IValueArray>)
				{
					return RpcTypeId_IValueArray;
				}
				else if constexpr (std::same_as<T, IValueList>)
				{
					return RpcTypeId_IValueList;
				}
				else if constexpr (std::same_as<T, IValueObservableList>)
				{
					return RpcTypeId_IValueObservableList;
				}
				else if constexpr (std::same_as<T, IValueDictionary>)
				{
					return RpcTypeId_IValueDictionary;
				}
				else
				{
					return 0;
				}
			}

			bool IsRpcObjectReferenceValue(const Value& value)
			{
				return value.GetValueType() == Value::BoxedValue && value.GetBoxedValue().Cast<IValueType::TypedBox<RpcObjectReference>>();
			}

			RpcObjectReference GetRpcObjectReference(const Value& value)
			{
				auto boxed = value.GetBoxedValue().Cast<IValueType::TypedBox<RpcObjectReference>>();
				CHECK_ERROR(boxed, L"RpcObjectReference is expected.");
				return boxed->value;
			}

			template<typename K, typename V>
			bool ContainsKey(const Dictionary<K, V>& xs, const K& key)
			{
				return xs.Keys().Contains(key);
			}

			template<typename T>
			Ptr<T> GetValue(const Dictionary<vint, Ptr<T>>& xs, vint objectId, const wchar_t* error)
			{
				CHECK_ERROR(ContainsKey(xs, objectId), error);
				return xs.Get(objectId);
			}
		}

		void RpcByrefListEventDispatcher::Track(vint objectId, IValueObservableList* list)
		{
			targets.Set(objectId, list);
		}

		void RpcByrefListEventDispatcher::Untrack(vint objectId)
		{
			targets.Remove(objectId);
		}

		void RpcByrefListEventDispatcher::OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)
		{
			if (ContainsKey(targets, ref.objectId))
			{
				targets.Get(ref.objectId)->ItemChanged(index, oldCount, newCount);
			}
		}

		RpcByrefEnumerator::RpcByrefEnumerator(IRpcLifeCycle* lc, RpcObjectReference enumeratorRef)
			: lifeCycle(lc)
			, controller(lc ? lc->GetController().Obj() : nullptr)
			, ref(enumeratorRef)
		{
			if (!lifeCycle || !controller) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
		}

		RpcByrefEnumerator::~RpcByrefEnumerator()
		{
			controller->ReleaseRemoteObject(ref);
		}

		Value RpcByrefEnumerator::GetCurrent()
		{
			return RpcUnboxByref(controller->EnumGetCurrent(ref), lifeCycle);
		}

		vint RpcByrefEnumerator::GetIndex()
		{
			return index;
		}

		bool RpcByrefEnumerator::Next()
		{
			if (controller->EnumNext(ref))
			{
				index++;
				return true;
			}
			return false;
		}

		RpcByrefEnumerable::RpcByrefEnumerable(IRpcLifeCycle* lc, RpcObjectReference enumerableRef)
			: lifeCycle(lc)
			, controller(lc ? lc->GetController().Obj() : nullptr)
			, ref(enumerableRef)
		{
			if (!lifeCycle || !controller) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
		}

		RpcByrefEnumerable::~RpcByrefEnumerable()
		{
			controller->ReleaseRemoteObject(ref);
		}

		Ptr<IValueEnumerator> RpcByrefEnumerable::CreateEnumerator()
		{
			return Ptr(new RpcByrefEnumerator(lifeCycle, controller->EnumCreate(ref)));
		}

		RpcByrefList::RpcByrefList(IRpcLifeCycle* lc, RpcObjectReference listRef)
			: lifeCycle(lc)
			, controller(lc ? lc->GetController().Obj() : nullptr)
			, ref(listRef)
		{
			if (!lifeCycle || !controller) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
		}

		RpcByrefList::~RpcByrefList()
		{
			controller->ReleaseRemoteObject(ref);
		}

		Ptr<IValueEnumerator> RpcByrefList::CreateEnumerator()
		{
			return Ptr(new RpcByrefEnumerator(lifeCycle, controller->EnumCreate(ref)));
		}

		vint RpcByrefList::GetCount()
		{
			return controller->ListGetCount(ref);
		}

		Value RpcByrefList::Get(vint index)
		{
			return RpcUnboxByref(controller->ListGet(ref, index), lifeCycle);
		}

		bool RpcByrefList::Contains(const Value& value)
		{
			return controller->ListContains(ref, RpcBoxByref(value, lifeCycle));
		}

		vint RpcByrefList::IndexOf(const Value& value)
		{
			return controller->ListIndexOf(ref, RpcBoxByref(value, lifeCycle));
		}

		void RpcByrefList::Set(vint index, const Value& value)
		{
			controller->ListSet(ref, index, RpcBoxByref(value, lifeCycle));
		}

		vint RpcByrefList::Add(const Value& value)
		{
			return controller->ListAdd(ref, RpcBoxByref(value, lifeCycle));
		}

		vint RpcByrefList::Insert(vint index, const Value& value)
		{
			return controller->ListInsert(ref, index, RpcBoxByref(value, lifeCycle));
		}

		bool RpcByrefList::Remove(const Value& value)
		{
			auto index = IndexOf(value);
			return index == -1 ? false : controller->ListRemoveAt(ref, index);
		}

		bool RpcByrefList::RemoveAt(vint index)
		{
			return controller->ListRemoveAt(ref, index);
		}

		void RpcByrefList::Clear()
		{
			controller->ListClear(ref);
		}

		RpcByrefArray::RpcByrefArray(IRpcLifeCycle* lc, RpcObjectReference arrayRef)
			: lifeCycle(lc)
			, controller(lc ? lc->GetController().Obj() : nullptr)
			, ref(arrayRef)
		{
			if (!lifeCycle || !controller) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
		}

		RpcByrefArray::~RpcByrefArray()
		{
			controller->ReleaseRemoteObject(ref);
		}

		Ptr<IValueEnumerator> RpcByrefArray::CreateEnumerator()
		{
			return Ptr(new RpcByrefEnumerator(lifeCycle, controller->EnumCreate(ref)));
		}

		vint RpcByrefArray::GetCount()
		{
			return controller->ListGetCount(ref);
		}

		Value RpcByrefArray::Get(vint index)
		{
			return RpcUnboxByref(controller->ListGet(ref, index), lifeCycle);
		}

		bool RpcByrefArray::Contains(const Value& value)
		{
			return controller->ListContains(ref, RpcBoxByref(value, lifeCycle));
		}

		vint RpcByrefArray::IndexOf(const Value& value)
		{
			return controller->ListIndexOf(ref, RpcBoxByref(value, lifeCycle));
		}

		void RpcByrefArray::Set(vint index, const Value& value)
		{
			controller->ListSet(ref, index, RpcBoxByref(value, lifeCycle));
		}

		void RpcByrefArray::Resize(vint size)
		{
			auto count = GetCount();
			if (size > count) CHECK_FAIL(L"RpcByrefArray::Resize cannot grow.");
			for (vint i = count - 1; i >= size; i--)
			{
				controller->ListRemoveAt(ref, i);
			}
		}

		RpcByrefObservableList::RpcByrefObservableList(IRpcLifeCycle* lc, RpcObjectReference listRef, Ptr<RpcByrefListEventDispatcher> listDispatcher)
			: lifeCycle(lc)
			, controller(lc ? lc->GetController().Obj() : nullptr)
			, ref(listRef)
			, dispatcher(listDispatcher)
		{
			if (!lifeCycle || !controller) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
		}

		RpcByrefObservableList::~RpcByrefObservableList()
		{
			if (dispatcher)
			{
				dispatcher->Untrack(ref.objectId);
			}
			controller->ReleaseRemoteObject(ref);
		}

		Ptr<IValueEnumerator> RpcByrefObservableList::CreateEnumerator()
		{
			return Ptr(new RpcByrefEnumerator(lifeCycle, controller->EnumCreate(ref)));
		}

		vint RpcByrefObservableList::GetCount()
		{
			return controller->ListGetCount(ref);
		}

		Value RpcByrefObservableList::Get(vint index)
		{
			return RpcUnboxByref(controller->ListGet(ref, index), lifeCycle);
		}

		bool RpcByrefObservableList::Contains(const Value& value)
		{
			return controller->ListContains(ref, RpcBoxByref(value, lifeCycle));
		}

		vint RpcByrefObservableList::IndexOf(const Value& value)
		{
			return controller->ListIndexOf(ref, RpcBoxByref(value, lifeCycle));
		}

		void RpcByrefObservableList::Set(vint index, const Value& value)
		{
			controller->ListSet(ref, index, RpcBoxByref(value, lifeCycle));
		}

		vint RpcByrefObservableList::Add(const Value& value)
		{
			return controller->ListAdd(ref, RpcBoxByref(value, lifeCycle));
		}

		vint RpcByrefObservableList::Insert(vint index, const Value& value)
		{
			return controller->ListInsert(ref, index, RpcBoxByref(value, lifeCycle));
		}

		bool RpcByrefObservableList::Remove(const Value& value)
		{
			auto index = IndexOf(value);
			return index == -1 ? false : controller->ListRemoveAt(ref, index);
		}

		bool RpcByrefObservableList::RemoveAt(vint index)
		{
			return controller->ListRemoveAt(ref, index);
		}

		void RpcByrefObservableList::Clear()
		{
			controller->ListClear(ref);
		}

		RpcByrefDictionary::RpcByrefDictionary(IRpcLifeCycle* lc, RpcObjectReference dictRef)
			: lifeCycle(lc)
			, controller(lc ? lc->GetController().Obj() : nullptr)
			, ref(dictRef)
		{
			if (!lifeCycle || !controller) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
		}

		RpcByrefDictionary::~RpcByrefDictionary()
		{
			controller->ReleaseRemoteObject(ref);
		}

		Ptr<IValueReadonlyList> RpcByrefDictionary::GetKeys()
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

		Ptr<IValueReadonlyList> RpcByrefDictionary::GetValues()
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

		vint RpcByrefDictionary::GetCount()
		{
			return controller->DictGetCount(ref);
		}

		Value RpcByrefDictionary::Get(const Value& key)
		{
			return RpcUnboxByref(controller->DictGet(ref, RpcBoxByref(key, lifeCycle)), lifeCycle);
		}

		void RpcByrefDictionary::Set(const Value& key, const Value& value)
		{
			controller->DictSet(ref, RpcBoxByref(key, lifeCycle), RpcBoxByref(value, lifeCycle));
		}

		bool RpcByrefDictionary::Remove(const Value& key)
		{
			return controller->DictRemove(ref, RpcBoxByref(key, lifeCycle));
		}

		void RpcByrefDictionary::Clear()
		{
			controller->DictClear(ref);
		}

		RpcCalleeListOps::RpcCalleeListOps(IRpcLifeCycle* lc)
			: lifeCycle(lc)
			, controller(lc ? lc->GetController().Obj() : nullptr)
		{
			if (!lifeCycle || !controller) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
		}

		void RpcCalleeListOps::RegisterLocalObject(RpcObjectReference ref, Ptr<IDescriptable> obj)
		{
			if (auto array = obj.Cast<IValueArray>())
			{
				arrays.Set(ref.objectId, array);
			}
			if (auto list = obj.Cast<IValueList>())
			{
				lists.Set(ref.objectId, list);
			}
			if (auto dict = obj.Cast<IValueDictionary>())
			{
				dicts.Set(ref.objectId, dict);
			}
			if (auto observable = obj.Cast<IValueObservableList>())
			{
				observableLists.Set(ref.objectId, observable);
			}
		}

		void RpcCalleeListOps::UnregisterLocalObject(RpcObjectReference ref)
		{
			enumerators.Remove(ref.objectId);
			arrays.Remove(ref.objectId);
			lists.Remove(ref.objectId);
			dicts.Remove(ref.objectId);
			observableLists.Remove(ref.objectId);
		}

		bool RpcCalleeListOps::HasTrackedObject(vint objectId)const
		{
			return ContainsKey(arrays, objectId) || ContainsKey(lists, objectId) || ContainsKey(dicts, objectId) || ContainsKey(observableLists, objectId);
		}

		bool RpcCalleeListOps::HasTrackedEnumerator(vint objectId)const
		{
			return ContainsKey(enumerators, objectId);
		}

		RpcObjectReference RpcCalleeListOps::EnumCreate(RpcObjectReference ref)
		{
			Ptr<IValueEnumerator> enumerator;
			if (ContainsKey(arrays, ref.objectId))
			{
				enumerator = arrays.Get(ref.objectId)->CreateEnumerator();
			}
			else if (ContainsKey(observableLists, ref.objectId))
			{
				enumerator = observableLists.Get(ref.objectId)->CreateEnumerator();
			}
			else if (ContainsKey(lists, ref.objectId))
			{
				enumerator = lists.Get(ref.objectId)->CreateEnumerator();
			}
			else
			{
				CHECK_FAIL(L"RpcCalleeListOps::EnumCreate cannot find the target collection.");
				return {};
			}

			auto enumeratorRef = controller->RegisterLocalObject(GetRpcTypeId<IValueEnumerator>());
			enumerators.Set(enumeratorRef.objectId, enumerator);
			return enumeratorRef;
		}

		bool RpcCalleeListOps::EnumNext(RpcObjectReference enumerator)
		{
			return GetValue(enumerators, enumerator.objectId, L"RpcCalleeListOps::EnumNext cannot find the target enumerator.")->Next();
		}

		Value RpcCalleeListOps::EnumGetCurrent(RpcObjectReference enumerator)
		{
			return RpcBoxByref(GetValue(enumerators, enumerator.objectId, L"RpcCalleeListOps::EnumGetCurrent cannot find the target enumerator.")->GetCurrent(), lifeCycle);
		}

		vint RpcCalleeListOps::ListGetCount(RpcObjectReference ref)
		{
			if (ContainsKey(arrays, ref.objectId)) return arrays.Get(ref.objectId)->GetCount();
			if (ContainsKey(observableLists, ref.objectId)) return observableLists.Get(ref.objectId)->GetCount();
			if (ContainsKey(lists, ref.objectId)) return lists.Get(ref.objectId)->GetCount();
			CHECK_FAIL(L"RpcCalleeListOps::ListGetCount cannot find the target list.");
			return 0;
		}

		Value RpcCalleeListOps::ListGet(RpcObjectReference ref, vint index)
		{
			if (ContainsKey(arrays, ref.objectId)) return RpcBoxByref(arrays.Get(ref.objectId)->Get(index), lifeCycle);
			if (ContainsKey(observableLists, ref.objectId)) return RpcBoxByref(observableLists.Get(ref.objectId)->Get(index), lifeCycle);
			if (ContainsKey(lists, ref.objectId)) return RpcBoxByref(lists.Get(ref.objectId)->Get(index), lifeCycle);
			CHECK_FAIL(L"RpcCalleeListOps::ListGet cannot find the target list.");
			return {};
		}

		void RpcCalleeListOps::ListSet(RpcObjectReference ref, vint index, const Value& value)
		{
			auto trivial = RpcUnboxByref(value, lifeCycle);
			if (ContainsKey(arrays, ref.objectId))
			{
				arrays.Get(ref.objectId)->Set(index, trivial);
				return;
			}
			if (ContainsKey(observableLists, ref.objectId))
			{
				observableLists.Get(ref.objectId)->Set(index, trivial);
				return;
			}
			if (ContainsKey(lists, ref.objectId))
			{
				lists.Get(ref.objectId)->Set(index, trivial);
				return;
			}
			CHECK_FAIL(L"RpcCalleeListOps::ListSet cannot find the target list.");
		}

		vint RpcCalleeListOps::ListAdd(RpcObjectReference ref, const Value& value)
		{
			auto trivial = RpcUnboxByref(value, lifeCycle);
			if (ContainsKey(observableLists, ref.objectId))
			{
				return observableLists.Get(ref.objectId)->Add(trivial);
			}
			if (ContainsKey(lists, ref.objectId))
			{
				return lists.Get(ref.objectId)->Add(trivial);
			}
			CHECK_FAIL(L"RpcCalleeListOps::ListAdd cannot find a writable list.");
			return -1;
		}

		vint RpcCalleeListOps::ListInsert(RpcObjectReference ref, vint index, const Value& value)
		{
			auto trivial = RpcUnboxByref(value, lifeCycle);
			if (ContainsKey(observableLists, ref.objectId))
			{
				return observableLists.Get(ref.objectId)->Insert(index, trivial);
			}
			if (ContainsKey(lists, ref.objectId))
			{
				return lists.Get(ref.objectId)->Insert(index, trivial);
			}
			CHECK_FAIL(L"RpcCalleeListOps::ListInsert cannot find a writable list.");
			return -1;
		}

		bool RpcCalleeListOps::ListRemoveAt(RpcObjectReference ref, vint index)
		{
			if (ContainsKey(arrays, ref.objectId))
			{
				auto array = arrays.Get(ref.objectId);
				CHECK_ERROR(index == array->GetCount() - 1, L"RpcCalleeListOps::ListRemoveAt only supports tail removal for arrays.");
				array->Resize(index);
				return true;
			}
			if (ContainsKey(observableLists, ref.objectId))
			{
				return observableLists.Get(ref.objectId)->RemoveAt(index);
			}
			if (ContainsKey(lists, ref.objectId))
			{
				return lists.Get(ref.objectId)->RemoveAt(index);
			}
			CHECK_FAIL(L"RpcCalleeListOps::ListRemoveAt cannot find the target list.");
			return false;
		}

		void RpcCalleeListOps::ListClear(RpcObjectReference ref)
		{
			if (ContainsKey(arrays, ref.objectId))
			{
				arrays.Get(ref.objectId)->Resize(0);
				return;
			}
			if (ContainsKey(observableLists, ref.objectId))
			{
				observableLists.Get(ref.objectId)->Clear();
				return;
			}
			if (ContainsKey(lists, ref.objectId))
			{
				lists.Get(ref.objectId)->Clear();
				return;
			}
			CHECK_FAIL(L"RpcCalleeListOps::ListClear cannot find the target list.");
		}

		bool RpcCalleeListOps::ListContains(RpcObjectReference ref, const Value& value)
		{
			auto trivial = RpcUnboxByref(value, lifeCycle);
			if (ContainsKey(arrays, ref.objectId)) return arrays.Get(ref.objectId)->Contains(trivial);
			if (ContainsKey(observableLists, ref.objectId)) return observableLists.Get(ref.objectId)->Contains(trivial);
			if (ContainsKey(lists, ref.objectId)) return lists.Get(ref.objectId)->Contains(trivial);
			CHECK_FAIL(L"RpcCalleeListOps::ListContains cannot find the target list.");
			return false;
		}

		vint RpcCalleeListOps::ListIndexOf(RpcObjectReference ref, const Value& value)
		{
			auto trivial = RpcUnboxByref(value, lifeCycle);
			if (ContainsKey(arrays, ref.objectId)) return arrays.Get(ref.objectId)->IndexOf(trivial);
			if (ContainsKey(observableLists, ref.objectId)) return observableLists.Get(ref.objectId)->IndexOf(trivial);
			if (ContainsKey(lists, ref.objectId)) return lists.Get(ref.objectId)->IndexOf(trivial);
			CHECK_FAIL(L"RpcCalleeListOps::ListIndexOf cannot find the target list.");
			return -1;
		}

		vint RpcCalleeListOps::DictGetCount(RpcObjectReference ref)
		{
			return GetValue(dicts, ref.objectId, L"RpcCalleeListOps::DictGetCount cannot find the target dictionary.")->GetCount();
		}

		Value RpcCalleeListOps::DictGet(RpcObjectReference ref, const Value& key)
		{
			auto dict = GetValue(dicts, ref.objectId, L"RpcCalleeListOps::DictGet cannot find the target dictionary.");
			auto trivialKey = RpcUnboxByref(key, lifeCycle);
			return RpcBoxByref(dict->Get(trivialKey), lifeCycle);
		}

		void RpcCalleeListOps::DictSet(RpcObjectReference ref, const Value& key, const Value& value)
		{
			auto dict = GetValue(dicts, ref.objectId, L"RpcCalleeListOps::DictSet cannot find the target dictionary.");
			dict->Set(RpcUnboxByref(key, lifeCycle), RpcUnboxByref(value, lifeCycle));
		}

		bool RpcCalleeListOps::DictRemove(RpcObjectReference ref, const Value& key)
		{
			auto dict = GetValue(dicts, ref.objectId, L"RpcCalleeListOps::DictRemove cannot find the target dictionary.");
			return dict->Remove(RpcUnboxByref(key, lifeCycle));
		}

		void RpcCalleeListOps::DictClear(RpcObjectReference ref)
		{
			GetValue(dicts, ref.objectId, L"RpcCalleeListOps::DictClear cannot find the target dictionary.")->Clear();
		}

		bool RpcCalleeListOps::DictContainsKey(RpcObjectReference ref, const Value& key)
		{
			auto keys = GetValue(dicts, ref.objectId, L"RpcCalleeListOps::DictContainsKey cannot find the target dictionary.")->GetKeys();
			return keys->Contains(RpcUnboxByref(key, lifeCycle));
		}

		Ptr<IValueArray> RpcCalleeListOps::DictGetKeys(RpcObjectReference ref)
		{
			auto keys = GetValue(dicts, ref.objectId, L"RpcCalleeListOps::DictGetKeys cannot find the target dictionary.")->GetKeys();
			auto snapshot = IValueArray::Create();
			snapshot->Resize(keys->GetCount());
			for (vint i = 0; i < keys->GetCount(); i++)
			{
				snapshot->Set(i, RpcBoxByref(keys->Get(i), lifeCycle));
			}
			return snapshot;
		}

		Ptr<IValueArray> RpcCalleeListOps::DictGetValues(RpcObjectReference ref)
		{
			auto values = GetValue(dicts, ref.objectId, L"RpcCalleeListOps::DictGetValues cannot find the target dictionary.")->GetValues();
			auto snapshot = IValueArray::Create();
			snapshot->Resize(values->GetCount());
			for (vint i = 0; i < values->GetCount(); i++)
			{
				snapshot->Set(i, RpcBoxByref(values->Get(i), lifeCycle));
			}
			return snapshot;
		}

		RpcCalleeListEventBridge::RpcCalleeListEventBridge(IRpcLifeCycle* lc)
			: lifeCycle(lc)
			, controller(lc ? lc->GetController().Obj() : nullptr)
		{
			if (!lifeCycle || !controller) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
		}

		void RpcCalleeListEventBridge::RegisterLocalObject(RpcObjectReference ref, Ptr<IDescriptable> obj)
		{
			if (auto observable = obj.Cast<IValueObservableList>())
			{
				refs.Set(ref.objectId, ref);
				sources.Set(ref.objectId, observable);
				handlers.Set(ref.objectId, observable->ItemChanged.Add([this, ref](vint index, vint oldCount, vint newCount)
				{
					controller->OnItemChanged(ref, index, oldCount, newCount);
				}));
			}
		}

		void RpcCalleeListEventBridge::UnregisterLocalObject(RpcObjectReference ref)
		{
			if (ContainsKey(handlers, ref.objectId))
			{
				if (ContainsKey(sources, ref.objectId))
				{
					sources.Get(ref.objectId)->ItemChanged.Remove(handlers.Get(ref.objectId));
				}
				handlers.Remove(ref.objectId);
			}
			refs.Remove(ref.objectId);
			sources.Remove(ref.objectId);
		}

		bool RpcCalleeListEventBridge::HasTrackedHandler(vint objectId)const
		{
			return ContainsKey(handlers, objectId);
		}

		void RpcCalleeListEventBridge::OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)
		{
			controller->OnItemChanged(ref, index, oldCount, newCount);
		}

		Value RpcBoxByref(const Value& trivial, IRpcLifeCycle* lc)
		{
			if (!lc) CHECK_FAIL(L"IRpcLifeCycle cannot be null.");
			if (trivial.IsNull()) return trivial;

			if (trivial.GetValueType() == Value::SharedPtr)
			{
				if (auto raw = trivial.GetRawPtr())
				{
					if (auto obj = Ptr(raw->SafeAggregationCast<IDescriptable>()))
					{
						return BoxValue(lc->PtrToRef(obj));
					}
				}
			}

			return trivial;
		}

		Value RpcUnboxByref(const Value& serializable, IRpcLifeCycle* lc)
		{
			if (!lc) CHECK_FAIL(L"IRpcLifeCycle cannot be null.");
			if (serializable.IsNull()) return serializable;

			if (IsRpcObjectReferenceValue(serializable))
			{
				return BoxValue(lc->RefToPtr(GetRpcObjectReference(serializable)));
			}

			return serializable;
		}

		Value RpcBoxByvalInternal(const Value& trivial, IRpcLifeCycle* lc, Dictionary<const DescriptableObject*, bool>& visited)
		{
			(void)lc;
			if (trivial.IsNull()) return trivial;
			if (trivial.GetValueType() == Value::SharedPtr)
			{
				if (auto roDict = trivial.GetRawPtr() ? Ptr(trivial.GetRawPtr()->SafeAggregationCast<IValueReadonlyDictionary>()) : nullptr)
				{
					auto key = static_cast<const DescriptableObject*>(trivial.GetRawPtr());
					if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval boxing does not support cycles.");
					visited.Add(key, true);

					auto dict = IValueDictionary::Create();
					auto keys = roDict->GetKeys();
					for (vint i = 0; i < keys->GetCount(); i++)
					{
						auto dictKey = keys->Get(i);
						dict->Set(
							RpcBoxByvalInternal(dictKey, lc, visited),
							RpcBoxByvalInternal(roDict->Get(dictKey), lc, visited)
						);
					}
					visited.Remove(key);
					return BoxValue(dict);
				}

				if (auto roList = trivial.GetRawPtr() ? Ptr(trivial.GetRawPtr()->SafeAggregationCast<IValueReadonlyList>()) : nullptr)
				{
					auto key = static_cast<const DescriptableObject*>(trivial.GetRawPtr());
					if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval boxing does not support cycles.");
					visited.Add(key, true);

					auto list = IValueList::Create();
					for (vint i = 0; i < roList->GetCount(); i++)
					{
						list->Add(RpcBoxByvalInternal(roList->Get(i), lc, visited));
					}
					visited.Remove(key);
					return BoxValue(list);
				}
			}
			return trivial;
		}

		Value RpcBoxByval(const Value& trivial, IRpcLifeCycle* lc)
		{
			if (!lc) CHECK_FAIL(L"IRpcLifeCycle cannot be null.");
			Dictionary<const DescriptableObject*, bool> visited;
			return RpcBoxByvalInternal(trivial, lc, visited);
		}

		Value RpcUnboxByvalInternal(const Value& serializable, IRpcLifeCycle* lc, Dictionary<const DescriptableObject*, bool>& visited)
		{
			(void)lc;
			if (serializable.IsNull()) return serializable;
			if (serializable.GetValueType() == Value::SharedPtr)
			{
				if (auto roDict = serializable.GetRawPtr() ? Ptr(serializable.GetRawPtr()->SafeAggregationCast<IValueReadonlyDictionary>()) : nullptr)
				{
					auto key = static_cast<const DescriptableObject*>(serializable.GetRawPtr());
					if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval unboxing does not support cycles.");
					visited.Add(key, true);

					auto dict = IValueDictionary::Create();
					auto keys = roDict->GetKeys();
					for (vint i = 0; i < keys->GetCount(); i++)
					{
						auto dictKey = keys->Get(i);
						dict->Set(
							RpcUnboxByvalInternal(dictKey, lc, visited),
							RpcUnboxByvalInternal(roDict->Get(dictKey), lc, visited)
						);
					}
					visited.Remove(key);
					return BoxValue(dict);
				}

				if (auto roList = serializable.GetRawPtr() ? Ptr(serializable.GetRawPtr()->SafeAggregationCast<IValueReadonlyList>()) : nullptr)
				{
					auto key = static_cast<const DescriptableObject*>(serializable.GetRawPtr());
					if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval unboxing does not support cycles.");
					visited.Add(key, true);

					auto list = IValueList::Create();
					for (vint i = 0; i < roList->GetCount(); i++)
					{
						list->Add(RpcUnboxByvalInternal(roList->Get(i), lc, visited));
					}
					visited.Remove(key);
					return BoxValue(list);
				}
			}
			return serializable;
		}

		Value RpcUnboxByval(const Value& serializable, IRpcLifeCycle* lc)
		{
			if (!lc) CHECK_FAIL(L"IRpcLifeCycle cannot be null.");
			Dictionary<const DescriptableObject*, bool> visited;
			return RpcUnboxByvalInternal(serializable, lc, visited);
		}
	}
}
