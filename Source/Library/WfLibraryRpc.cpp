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
		}
		
/***********************************************************************
* RpcByrefEnumerator
***********************************************************************/

		RpcByrefEnumerator::RpcByrefEnumerator(IRpcLifeCycle* lc, RpcObjectReference enumeratorRef)
			: lifeCycle(lc)
			, controller(lc ? lc->GetController() : nullptr)
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
			, controller(lc ? lc->GetController() : nullptr)
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
		
/***********************************************************************
* RpcByrefReadonlyList
***********************************************************************/

		RpcByrefReadonlyList::RpcByrefReadonlyList(IRpcLifeCycle* lc, RpcObjectReference listRef)
			: lifeCycle(lc)
			, controller(lc ? lc->GetController() : nullptr)
			, ref(listRef)
		{
			if (!lifeCycle || !controller) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
		}

		RpcByrefReadonlyList::~RpcByrefReadonlyList()
		{
			controller->ReleaseRemoteObject(ref);
		}

		Ptr<IValueEnumerator> RpcByrefReadonlyList::CreateEnumerator()
		{
			return Ptr(new RpcByrefEnumerator(lifeCycle, controller->EnumCreate(ref)));
		}

		vint RpcByrefReadonlyList::GetCount()
		{
			return controller->ListGetCount(ref);
		}

		Value RpcByrefReadonlyList::Get(vint index)
		{
			return RpcUnboxByref(controller->ListGet(ref, index), lifeCycle);
		}

		bool RpcByrefReadonlyList::Contains(const Value& value)
		{
			return controller->ListContains(ref, RpcBoxByref(value, lifeCycle));
		}

		vint RpcByrefReadonlyList::IndexOf(const Value& value)
		{
			return controller->ListIndexOf(ref, RpcBoxByref(value, lifeCycle));
		}
		
/***********************************************************************
* RpcByrefList
***********************************************************************/

		RpcByrefList::RpcByrefList(IRpcLifeCycle* lc, RpcObjectReference listRef)
			: RpcByrefReadonlyList(lc, listRef)
		{
		}

		RpcByrefList::~RpcByrefList()
		{
		}

		Ptr<IValueEnumerator> RpcByrefList::CreateEnumerator()
		{
			return RpcByrefReadonlyList::CreateEnumerator();
		}

		vint RpcByrefList::GetCount()
		{
			return RpcByrefReadonlyList::GetCount();
		}

		Value RpcByrefList::Get(vint index)
		{
			return RpcByrefReadonlyList::Get(index);
		}

		bool RpcByrefList::Contains(const Value& value)
		{
			return RpcByrefReadonlyList::Contains(value);
		}

		vint RpcByrefList::IndexOf(const Value& value)
		{
			return RpcByrefReadonlyList::IndexOf(value);
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
		
/***********************************************************************
* RpcByrefArray
***********************************************************************/

		RpcByrefArray::RpcByrefArray(IRpcLifeCycle* lc, RpcObjectReference arrayRef)
			: lifeCycle(lc)
			, controller(lc ? lc->GetController() : nullptr)
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
		
/***********************************************************************
* RpcByrefObservableList
***********************************************************************/

		RpcByrefObservableList::RpcByrefObservableList(IRpcLifeCycle* lc, RpcObjectReference listRef)
			: lifeCycle(lc)
			, controller(lc ? lc->GetController() : nullptr)
			, ref(listRef)
		{
			if (!lifeCycle || !controller) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
		}

		RpcByrefObservableList::~RpcByrefObservableList()
		{
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
		
/***********************************************************************
* RpcByrefDictionary
***********************************************************************/

		RpcByrefDictionary::RpcByrefDictionary(IRpcLifeCycle* lc, RpcObjectReference dictRef)
			: lifeCycle(lc)
			, controller(lc ? lc->GetController() : nullptr)
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
			return Ptr(new RpcByrefReadonlyList(lifeCycle, controller->DictGetKeys(ref)));
		}

		Ptr<IValueReadonlyList> RpcByrefDictionary::GetValues()
		{
			return Ptr(new RpcByrefReadonlyList(lifeCycle, controller->DictGetValues(ref)));
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
		
/***********************************************************************
* RpcCalleeListOps
***********************************************************************/

		RpcCalleeListOps::RpcCalleeListOps(IRpcLifeCycle* lc)
			: lifeCycle(lc)
		{
			if (!lifeCycle) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
		}

		RpcObjectReference RpcCalleeListOps::EnumCreate(RpcObjectReference ref)
		{
			auto obj = lifeCycle->RefToPtr(ref);
			Ptr<IValueEnumerator> enumerator;
			if (auto xs = Ptr(obj.Obj()->SafeAggregationCast<IValueEnumerable>()))
			{
				enumerator = xs->CreateEnumerator();
			}
			else
			{
				CHECK_FAIL(L"RpcCalleeListOps::EnumCreate cannot find the target collection.");
				return {};
			}

			return lifeCycle->PtrToRef(enumerator);
		}

		bool RpcCalleeListOps::EnumNext(RpcObjectReference enumerator)
		{
			auto obj = lifeCycle->RefToPtr(enumerator);
			auto e = Ptr(obj.Obj()->SafeAggregationCast<IValueEnumerator>());
			CHECK_ERROR(e, L"RpcCalleeListOps::EnumNext cannot find the target enumerator.");
			return e->Next();
		}

		Value RpcCalleeListOps::EnumGetCurrent(RpcObjectReference enumerator)
		{
			auto obj = lifeCycle->RefToPtr(enumerator);
			auto e = Ptr(obj.Obj()->SafeAggregationCast<IValueEnumerator>());
			CHECK_ERROR(e, L"RpcCalleeListOps::EnumGetCurrent cannot find the target enumerator.");
			return RpcBoxByref(e->GetCurrent(), lifeCycle);
		}

		vint RpcCalleeListOps::ListGetCount(RpcObjectReference ref)
		{
			auto obj = lifeCycle->RefToPtr(ref);
			if (auto roList = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyList>()))
				return roList->GetCount();
			CHECK_FAIL(L"RpcCalleeListOps::ListGetCount cannot find the target list.");
			return 0;
		}

		Value RpcCalleeListOps::ListGet(RpcObjectReference ref, vint index)
		{
			auto obj = lifeCycle->RefToPtr(ref);
			if (auto roList = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyList>()))
				return RpcBoxByref(roList->Get(index), lifeCycle);
			CHECK_FAIL(L"RpcCalleeListOps::ListGet cannot find the target list.");
			return {};
		}

		void RpcCalleeListOps::ListSet(RpcObjectReference ref, vint index, const Value& value)
		{
			auto trivial = RpcUnboxByref(value, lifeCycle);
			auto obj = lifeCycle->RefToPtr(ref);
			if (auto array = Ptr(obj.Obj()->SafeAggregationCast<IValueArray>()))
			{
				array->Set(index, trivial);
				return;
			}
			if (auto list = Ptr(obj.Obj()->SafeAggregationCast<IValueList>()))
			{
				list->Set(index, trivial);
				return;
			}
			CHECK_FAIL(L"RpcCalleeListOps::ListSet cannot find the target list.");
		}

		vint RpcCalleeListOps::ListAdd(RpcObjectReference ref, const Value& value)
		{
			auto trivial = RpcUnboxByref(value, lifeCycle);
			auto obj = lifeCycle->RefToPtr(ref);
			if (auto list = Ptr(obj.Obj()->SafeAggregationCast<IValueList>()))
				return list->Add(trivial);
			CHECK_FAIL(L"RpcCalleeListOps::ListAdd cannot find a writable list.");
			return -1;
		}

		vint RpcCalleeListOps::ListInsert(RpcObjectReference ref, vint index, const Value& value)
		{
			auto trivial = RpcUnboxByref(value, lifeCycle);
			auto obj = lifeCycle->RefToPtr(ref);
			if (auto list = Ptr(obj.Obj()->SafeAggregationCast<IValueList>()))
				return list->Insert(index, trivial);
			CHECK_FAIL(L"RpcCalleeListOps::ListInsert cannot find a writable list.");
			return -1;
		}

		bool RpcCalleeListOps::ListRemoveAt(RpcObjectReference ref, vint index)
		{
			auto obj = lifeCycle->RefToPtr(ref);
			if (auto array = Ptr(obj.Obj()->SafeAggregationCast<IValueArray>()))
			{
				CHECK_ERROR(index == array->GetCount() - 1, L"RpcCalleeListOps::ListRemoveAt only supports tail removal for arrays.");
				array->Resize(index);
				return true;
			}
			if (auto list = Ptr(obj.Obj()->SafeAggregationCast<IValueList>()))
				return list->RemoveAt(index);
			CHECK_FAIL(L"RpcCalleeListOps::ListRemoveAt cannot find the target list.");
			return false;
		}

		void RpcCalleeListOps::ListClear(RpcObjectReference ref)
		{
			auto obj = lifeCycle->RefToPtr(ref);
			if (auto array = Ptr(obj.Obj()->SafeAggregationCast<IValueArray>()))
			{
				array->Resize(0);
				return;
			}
			if (auto list = Ptr(obj.Obj()->SafeAggregationCast<IValueList>()))
			{
				list->Clear();
				return;
			}
			CHECK_FAIL(L"RpcCalleeListOps::ListClear cannot find the target list.");
		}

		bool RpcCalleeListOps::ListContains(RpcObjectReference ref, const Value& value)
		{
			auto trivial = RpcUnboxByref(value, lifeCycle);
			auto obj = lifeCycle->RefToPtr(ref);
			if (auto roList = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyList>()))
				return roList->Contains(trivial);
			CHECK_FAIL(L"RpcCalleeListOps::ListContains cannot find the target list.");
			return false;
		}

		vint RpcCalleeListOps::ListIndexOf(RpcObjectReference ref, const Value& value)
		{
			auto trivial = RpcUnboxByref(value, lifeCycle);
			auto obj = lifeCycle->RefToPtr(ref);
			if (auto roList = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyList>()))
				return roList->IndexOf(trivial);
			CHECK_FAIL(L"RpcCalleeListOps::ListIndexOf cannot find the target list.");
			return -1;
		}

		vint RpcCalleeListOps::DictGetCount(RpcObjectReference ref)
		{
			auto obj = lifeCycle->RefToPtr(ref);
			auto dict = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyDictionary>());
			CHECK_ERROR(dict, L"RpcCalleeListOps::DictGetCount cannot find the target dictionary.");
			return dict->GetCount();
		}

		Value RpcCalleeListOps::DictGet(RpcObjectReference ref, const Value& key)
		{
			auto obj = lifeCycle->RefToPtr(ref);
			auto dict = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyDictionary>());
			CHECK_ERROR(dict, L"RpcCalleeListOps::DictGet cannot find the target dictionary.");
			return RpcBoxByref(dict->Get(RpcUnboxByref(key, lifeCycle)), lifeCycle);
		}

		void RpcCalleeListOps::DictSet(RpcObjectReference ref, const Value& key, const Value& value)
		{
			auto obj = lifeCycle->RefToPtr(ref);
			auto dict = Ptr(obj.Obj()->SafeAggregationCast<IValueDictionary>());
			CHECK_ERROR(dict, L"RpcCalleeListOps::DictSet cannot find the target dictionary.");
			dict->Set(RpcUnboxByref(key, lifeCycle), RpcUnboxByref(value, lifeCycle));
		}

		bool RpcCalleeListOps::DictRemove(RpcObjectReference ref, const Value& key)
		{
			auto obj = lifeCycle->RefToPtr(ref);
			auto dict = Ptr(obj.Obj()->SafeAggregationCast<IValueDictionary>());
			CHECK_ERROR(dict, L"RpcCalleeListOps::DictRemove cannot find the target dictionary.");
			return dict->Remove(RpcUnboxByref(key, lifeCycle));
		}

		void RpcCalleeListOps::DictClear(RpcObjectReference ref)
		{
			auto obj = lifeCycle->RefToPtr(ref);
			auto dict = Ptr(obj.Obj()->SafeAggregationCast<IValueDictionary>());
			CHECK_ERROR(dict, L"RpcCalleeListOps::DictClear cannot find the target dictionary.");
			dict->Clear();
		}

		bool RpcCalleeListOps::DictContainsKey(RpcObjectReference ref, const Value& key)
		{
			auto obj = lifeCycle->RefToPtr(ref);
			auto dict = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyDictionary>());
			CHECK_ERROR(dict, L"RpcCalleeListOps::DictContainsKey cannot find the target dictionary.");
			return dict->GetKeys()->Contains(RpcUnboxByref(key, lifeCycle));
		}

		RpcObjectReference RpcCalleeListOps::DictGetKeys(RpcObjectReference ref)
		{
			auto obj = lifeCycle->RefToPtr(ref);
			auto dict = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyDictionary>());
			CHECK_ERROR(dict, L"RpcCalleeListOps::DictGetKeys cannot find the target dictionary.");
			return lifeCycle->PtrToRef(dict->GetKeys());
		}

		RpcObjectReference RpcCalleeListOps::DictGetValues(RpcObjectReference ref)
		{
			auto obj = lifeCycle->RefToPtr(ref);
			auto dict = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyDictionary>());
			CHECK_ERROR(dict, L"RpcCalleeListOps::DictGetValues cannot find the target dictionary.");
			return lifeCycle->PtrToRef(dict->GetValues());
		}
		
/***********************************************************************
* RpcCalleeListEventBridge
***********************************************************************/

		RpcCalleeListEventBridge::RpcCalleeListEventBridge(IRpcLifeCycle* lc)
			: lifeCycle(lc)
		{
			if (!lifeCycle) CHECK_FAIL(L"Invalid IRpcLifeCycle.");
		}

		void RpcCalleeListEventBridge::OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)
		{
			lifeCycle->GetController()->OnItemChanged(ref, index, oldCount, newCount);
		}
		
/***********************************************************************
* Helpers
***********************************************************************/

		Value RpcBoxByref(const Value& trivial, IRpcLifeCycle* lc)
		{
			if (!lc) CHECK_FAIL(L"IRpcLifeCycle cannot be null.");
			if (trivial.IsNull()) return trivial;

			if (trivial.GetValueType() == Value::SharedPtr)
			{
				if (auto raw = trivial.GetRawPtr())
				{
					if (auto obj = dynamic_cast<IDescriptable*>(raw))
					{
						return BoxValue(lc->PtrToRef(Ptr<IDescriptable>(obj)));
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

				if (auto raw = trivial.GetRawPtr())
				{
					if (auto obj = dynamic_cast<IDescriptable*>(raw))
					{
						return BoxValue(lc->PtrToRef(Ptr<IDescriptable>(obj)));
					}
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
			if (serializable.IsNull()) return serializable;

			if (IsRpcObjectReferenceValue(serializable))
			{
				return BoxValue(lc->RefToPtr(GetRpcObjectReference(serializable)));
			}

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
