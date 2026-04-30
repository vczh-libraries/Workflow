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

			Value BoxRpcObject(Ptr<IDescriptable> obj, vint typeId)
			{
				auto raw = obj.Obj();
				switch (typeId)
				{
				case RpcTypeId_IValueObservableList:
					return BoxValue(Ptr(dynamic_cast<IValueObservableList*>(raw)));
				case RpcTypeId_IValueDictionary:
					return BoxValue(Ptr(dynamic_cast<IValueDictionary*>(raw)));
				case RpcTypeId_IValueArray:
					return BoxValue(Ptr(dynamic_cast<IValueArray*>(raw)));
				case RpcTypeId_IValueList:
					return BoxValue(Ptr(dynamic_cast<IValueList*>(raw)));
				case RpcTypeId_IValueReadonlyList:
					return BoxValue(Ptr(dynamic_cast<IValueReadonlyList*>(raw)));
				case RpcTypeId_IValueEnumerator:
					return BoxValue(Ptr(dynamic_cast<IValueEnumerator*>(raw)));
				case RpcTypeId_IValueEnumerable:
					return BoxValue(Ptr(dynamic_cast<IValueEnumerable*>(raw)));
				default:
					return BoxValue(obj);
				}
			}

			template<typename TWrapper, typename TInterface>
			Ptr<TInterface> CreateTrackedProxy(IRpcLifecycle* lc, RpcObjectReference ref)
			{
				auto obj = lc->RefToPtr(ref);
				CHECK_ERROR(obj, L"RPC proxy creation returned null.");
				if (auto wrapper = Ptr(obj.Obj()->SafeAggregationCast<IRpcWrapperBase>()))
				{
					auto proxy = Ptr(obj.Obj()->SafeAggregationCast<TInterface>());
					CHECK_ERROR(proxy, L"RPC proxy creation returned an unexpected type.");
					return proxy;
				}
				return Ptr(new TWrapper(lc, ref));
			}

			void SetRemoteObjectHold(IRpcLifecycle* lc, RpcObjectReference ref, bool hold)
			{
				lc->GetDispatcher()->SendToClient_ObjectOps(ref.clientId)->ObjectHold(ref, lc->GetClientId(), hold);
			}

			const WString RpcByvalKeepAliveProperty = L"RpcByvalKeepAlive";

			class RpcByvalKeepAlive : public Object
			{
			public:
				List<Ptr<IDescriptable>> objects;
			};

			IRpcListOps* GetRemoteListOps(IRpcLifecycle* lc, RpcObjectReference ref)
			{
				return lc->GetDispatcher()->SendToClient_ListOps(ref.clientId);
			}
		}
		
/***********************************************************************
* RpcByrefEnumerator
***********************************************************************/

		RpcByrefEnumerator::RpcByrefEnumerator(IRpcLifecycle* lc, RpcObjectReference enumeratorRef)
			: lifecycle(lc)
			, ref(enumeratorRef)
		{
			if (!lifecycle) CHECK_FAIL(L"Invalid IRpcLifecycle.");
			SetRemoteObjectHold(lifecycle, ref, true);
		}

		RpcByrefEnumerator::~RpcByrefEnumerator()
		{
			if (lifecycle) SetRemoteObjectHold(lifecycle, ref, false);
		}

		void RpcByrefEnumerator::DisconnectFromLifecycle()
		{
			lifecycle = nullptr;
		}

		Value RpcByrefEnumerator::GetCurrent()
		{
			return RpcUnboxByref(GetRemoteListOps(lifecycle, ref)->EnumGetCurrent(ref), lifecycle);
		}

		vint RpcByrefEnumerator::GetIndex()
		{
			return index;
		}

		bool RpcByrefEnumerator::Next()
		{
			if (GetRemoteListOps(lifecycle, ref)->EnumNext(ref))
			{
				index++;
				return true;
			}
			return false;
		}

		RpcByrefEnumerable::RpcByrefEnumerable(IRpcLifecycle* lc, RpcObjectReference enumerableRef)
			: lifecycle(lc)
			, ref(enumerableRef)
		{
			if (!lifecycle) CHECK_FAIL(L"Invalid IRpcLifecycle.");
			SetRemoteObjectHold(lifecycle, ref, true);
		}

		RpcByrefEnumerable::~RpcByrefEnumerable()
		{
			if (lifecycle) SetRemoteObjectHold(lifecycle, ref, false);
		}

		void RpcByrefEnumerable::DisconnectFromLifecycle()
		{
			lifecycle = nullptr;
		}

		Ptr<IValueEnumerator> RpcByrefEnumerable::CreateEnumerator()
		{
			return CreateTrackedProxy<RpcByrefEnumerator, IValueEnumerator>(lifecycle, GetRemoteListOps(lifecycle, ref)->EnumCreate(ref));
		}
		
/***********************************************************************
* RpcByrefReadonlyList
***********************************************************************/

		RpcByrefReadonlyList::RpcByrefReadonlyList(IRpcLifecycle* lc, RpcObjectReference listRef)
			: lifecycle(lc)
			, ref(listRef)
		{
			if (!lifecycle) CHECK_FAIL(L"Invalid IRpcLifecycle.");
			SetRemoteObjectHold(lifecycle, ref, true);
		}

		RpcByrefReadonlyList::~RpcByrefReadonlyList()
		{
			if (lifecycle) SetRemoteObjectHold(lifecycle, ref, false);
		}

		void RpcByrefReadonlyList::DisconnectFromLifecycle()
		{
			lifecycle = nullptr;
		}

		Ptr<IValueEnumerator> RpcByrefReadonlyList::CreateEnumerator()
		{
			return CreateTrackedProxy<RpcByrefEnumerator, IValueEnumerator>(lifecycle, GetRemoteListOps(lifecycle, ref)->EnumCreate(ref));
		}

		vint RpcByrefReadonlyList::GetCount()
		{
			return GetRemoteListOps(lifecycle, ref)->ListGetCount(ref);
		}

		Value RpcByrefReadonlyList::Get(vint index)
		{
			return RpcUnboxByref(GetRemoteListOps(lifecycle, ref)->ListGet(ref, index), lifecycle);
		}

		bool RpcByrefReadonlyList::Contains(const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListContains(ref, RpcBoxByref(value, lifecycle));
		}

		vint RpcByrefReadonlyList::IndexOf(const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListIndexOf(ref, RpcBoxByref(value, lifecycle));
		}
		
/***********************************************************************
* RpcByrefList
***********************************************************************/

		RpcByrefList::RpcByrefList(IRpcLifecycle* lc, RpcObjectReference listRef)
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
			GetRemoteListOps(lifecycle, ref)->ListSet(ref, index, RpcBoxByref(value, lifecycle));
		}

		vint RpcByrefList::Add(const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListAdd(ref, RpcBoxByref(value, lifecycle));
		}

		vint RpcByrefList::Insert(vint index, const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListInsert(ref, index, RpcBoxByref(value, lifecycle));
		}

		bool RpcByrefList::Remove(const Value& value)
		{
			auto index = IndexOf(value);
			return index == -1 ? false : GetRemoteListOps(lifecycle, ref)->ListRemoveAt(ref, index);
		}

		bool RpcByrefList::RemoveAt(vint index)
		{
			return GetRemoteListOps(lifecycle, ref)->ListRemoveAt(ref, index);
		}

		void RpcByrefList::Clear()
		{
			GetRemoteListOps(lifecycle, ref)->ListClear(ref);
		}
		
/***********************************************************************
* RpcByrefArray
***********************************************************************/

		RpcByrefArray::RpcByrefArray(IRpcLifecycle* lc, RpcObjectReference arrayRef)
			: lifecycle(lc)
			, ref(arrayRef)
		{
			if (!lifecycle) CHECK_FAIL(L"Invalid IRpcLifecycle.");
			SetRemoteObjectHold(lifecycle, ref, true);
		}

		RpcByrefArray::~RpcByrefArray()
		{
			if (lifecycle) SetRemoteObjectHold(lifecycle, ref, false);
		}

		void RpcByrefArray::DisconnectFromLifecycle()
		{
			lifecycle = nullptr;
		}

		Ptr<IValueEnumerator> RpcByrefArray::CreateEnumerator()
		{
			return CreateTrackedProxy<RpcByrefEnumerator, IValueEnumerator>(lifecycle, GetRemoteListOps(lifecycle, ref)->EnumCreate(ref));
		}

		vint RpcByrefArray::GetCount()
		{
			return GetRemoteListOps(lifecycle, ref)->ListGetCount(ref);
		}

		Value RpcByrefArray::Get(vint index)
		{
			return RpcUnboxByref(GetRemoteListOps(lifecycle, ref)->ListGet(ref, index), lifecycle);
		}

		bool RpcByrefArray::Contains(const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListContains(ref, RpcBoxByref(value, lifecycle));
		}

		vint RpcByrefArray::IndexOf(const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListIndexOf(ref, RpcBoxByref(value, lifecycle));
		}

		void RpcByrefArray::Set(vint index, const Value& value)
		{
			GetRemoteListOps(lifecycle, ref)->ListSet(ref, index, RpcBoxByref(value, lifecycle));
		}

		void RpcByrefArray::Resize(vint size)
		{
			auto count = GetCount();
			if (size > count) CHECK_FAIL(L"RpcByrefArray::Resize cannot grow.");
			for (vint i = count - 1; i >= size; i--)
			{
				GetRemoteListOps(lifecycle, ref)->ListRemoveAt(ref, i);
			}
		}
		
/***********************************************************************
* RpcByrefObservableList
***********************************************************************/

		RpcByrefObservableList::RpcByrefObservableList(IRpcLifecycle* lc, RpcObjectReference listRef)
			: lifecycle(lc)
			, ref(listRef)
		{
			if (!lifecycle) CHECK_FAIL(L"Invalid IRpcLifecycle.");
			SetRemoteObjectHold(lifecycle, ref, true);
		}

		RpcByrefObservableList::~RpcByrefObservableList()
		{
			if (lifecycle) SetRemoteObjectHold(lifecycle, ref, false);
		}

		void RpcByrefObservableList::DisconnectFromLifecycle()
		{
			lifecycle = nullptr;
		}

		Ptr<IValueEnumerator> RpcByrefObservableList::CreateEnumerator()
		{
			return CreateTrackedProxy<RpcByrefEnumerator, IValueEnumerator>(lifecycle, GetRemoteListOps(lifecycle, ref)->EnumCreate(ref));
		}

		vint RpcByrefObservableList::GetCount()
		{
			return GetRemoteListOps(lifecycle, ref)->ListGetCount(ref);
		}

		Value RpcByrefObservableList::Get(vint index)
		{
			return RpcUnboxByref(GetRemoteListOps(lifecycle, ref)->ListGet(ref, index), lifecycle);
		}

		bool RpcByrefObservableList::Contains(const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListContains(ref, RpcBoxByref(value, lifecycle));
		}

		vint RpcByrefObservableList::IndexOf(const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListIndexOf(ref, RpcBoxByref(value, lifecycle));
		}

		void RpcByrefObservableList::Set(vint index, const Value& value)
		{
			GetRemoteListOps(lifecycle, ref)->ListSet(ref, index, RpcBoxByref(value, lifecycle));
		}

		vint RpcByrefObservableList::Add(const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListAdd(ref, RpcBoxByref(value, lifecycle));
		}

		vint RpcByrefObservableList::Insert(vint index, const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListInsert(ref, index, RpcBoxByref(value, lifecycle));
		}

		bool RpcByrefObservableList::Remove(const Value& value)
		{
			auto index = IndexOf(value);
			return index == -1 ? false : GetRemoteListOps(lifecycle, ref)->ListRemoveAt(ref, index);
		}

		bool RpcByrefObservableList::RemoveAt(vint index)
		{
			return GetRemoteListOps(lifecycle, ref)->ListRemoveAt(ref, index);
		}

		void RpcByrefObservableList::Clear()
		{
			GetRemoteListOps(lifecycle, ref)->ListClear(ref);
		}
		
/***********************************************************************
* RpcByrefDictionary
***********************************************************************/

		RpcByrefDictionary::RpcByrefDictionary(IRpcLifecycle* lc, RpcObjectReference dictRef)
			: lifecycle(lc)
			, ref(dictRef)
		{
			if (!lifecycle) CHECK_FAIL(L"Invalid IRpcLifecycle.");
			SetRemoteObjectHold(lifecycle, ref, true);
		}

		RpcByrefDictionary::~RpcByrefDictionary()
		{
			if (lifecycle) SetRemoteObjectHold(lifecycle, ref, false);
		}

		void RpcByrefDictionary::DisconnectFromLifecycle()
		{
			lifecycle = nullptr;
		}

		Ptr<IValueReadonlyList> RpcByrefDictionary::GetKeys()
		{
			return CreateTrackedProxy<RpcByrefReadonlyList, IValueReadonlyList>(lifecycle, GetRemoteListOps(lifecycle, ref)->DictGetKeys(ref));
		}

		Ptr<IValueReadonlyList> RpcByrefDictionary::GetValues()
		{
			return CreateTrackedProxy<RpcByrefReadonlyList, IValueReadonlyList>(lifecycle, GetRemoteListOps(lifecycle, ref)->DictGetValues(ref));
		}

		vint RpcByrefDictionary::GetCount()
		{
			return GetRemoteListOps(lifecycle, ref)->DictGetCount(ref);
		}

		Value RpcByrefDictionary::Get(const Value& key)
		{
			return RpcUnboxByref(GetRemoteListOps(lifecycle, ref)->DictGet(ref, RpcBoxByref(key, lifecycle)), lifecycle);
		}

		void RpcByrefDictionary::Set(const Value& key, const Value& value)
		{
			GetRemoteListOps(lifecycle, ref)->DictSet(ref, RpcBoxByref(key, lifecycle), RpcBoxByref(value, lifecycle));
		}

		bool RpcByrefDictionary::Remove(const Value& key)
		{
			return GetRemoteListOps(lifecycle, ref)->DictRemove(ref, RpcBoxByref(key, lifecycle));
		}

		void RpcByrefDictionary::Clear()
		{
			GetRemoteListOps(lifecycle, ref)->DictClear(ref);
		}
		
/***********************************************************************
* RpcCalleeListOps
***********************************************************************/

		RpcCalleeListOps::RpcCalleeListOps(IRpcLifecycle* lc)
			: lifecycle(lc)
		{
			if (!lifecycle) CHECK_FAIL(L"Invalid IRpcLifecycle.");
		}

		RpcObjectReference RpcCalleeListOps::EnumCreate(RpcObjectReference ref)
		{
			auto obj = lifecycle->RefToPtr(ref);
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

			return lifecycle->PtrToRef(enumerator);
		}

		bool RpcCalleeListOps::EnumNext(RpcObjectReference enumerator)
		{
			auto obj = lifecycle->RefToPtr(enumerator);
			auto e = Ptr(obj.Obj()->SafeAggregationCast<IValueEnumerator>());
			CHECK_ERROR(e, L"RpcCalleeListOps::EnumNext cannot find the target enumerator.");
			return e->Next();
		}

		Value RpcCalleeListOps::EnumGetCurrent(RpcObjectReference enumerator)
		{
			auto obj = lifecycle->RefToPtr(enumerator);
			auto e = Ptr(obj.Obj()->SafeAggregationCast<IValueEnumerator>());
			CHECK_ERROR(e, L"RpcCalleeListOps::EnumGetCurrent cannot find the target enumerator.");
			return RpcBoxByref(e->GetCurrent(), lifecycle);
		}

		vint RpcCalleeListOps::ListGetCount(RpcObjectReference ref)
		{
			auto obj = lifecycle->RefToPtr(ref);
			if (auto roList = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyList>()))
				return roList->GetCount();
			CHECK_FAIL(L"RpcCalleeListOps::ListGetCount cannot find the target list.");
			return 0;
		}

		Value RpcCalleeListOps::ListGet(RpcObjectReference ref, vint index)
		{
			auto obj = lifecycle->RefToPtr(ref);
			if (auto roList = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyList>()))
				return RpcBoxByref(roList->Get(index), lifecycle);
			CHECK_FAIL(L"RpcCalleeListOps::ListGet cannot find the target list.");
			return {};
		}

		void RpcCalleeListOps::ListSet(RpcObjectReference ref, vint index, const Value& value)
		{
			auto trivial = RpcUnboxByref(value, lifecycle);
			auto obj = lifecycle->RefToPtr(ref);
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
			auto trivial = RpcUnboxByref(value, lifecycle);
			auto obj = lifecycle->RefToPtr(ref);
			if (auto list = Ptr(obj.Obj()->SafeAggregationCast<IValueList>()))
				return list->Add(trivial);
			CHECK_FAIL(L"RpcCalleeListOps::ListAdd cannot find a writable list.");
			return -1;
		}

		vint RpcCalleeListOps::ListInsert(RpcObjectReference ref, vint index, const Value& value)
		{
			auto trivial = RpcUnboxByref(value, lifecycle);
			auto obj = lifecycle->RefToPtr(ref);
			if (auto list = Ptr(obj.Obj()->SafeAggregationCast<IValueList>()))
				return list->Insert(index, trivial);
			CHECK_FAIL(L"RpcCalleeListOps::ListInsert cannot find a writable list.");
			return -1;
		}

		bool RpcCalleeListOps::ListRemoveAt(RpcObjectReference ref, vint index)
		{
			auto obj = lifecycle->RefToPtr(ref);
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
			auto obj = lifecycle->RefToPtr(ref);
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
			auto trivial = RpcUnboxByref(value, lifecycle);
			auto obj = lifecycle->RefToPtr(ref);
			if (auto roList = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyList>()))
				return roList->Contains(trivial);
			CHECK_FAIL(L"RpcCalleeListOps::ListContains cannot find the target list.");
			return false;
		}

		vint RpcCalleeListOps::ListIndexOf(RpcObjectReference ref, const Value& value)
		{
			auto trivial = RpcUnboxByref(value, lifecycle);
			auto obj = lifecycle->RefToPtr(ref);
			if (auto roList = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyList>()))
				return roList->IndexOf(trivial);
			CHECK_FAIL(L"RpcCalleeListOps::ListIndexOf cannot find the target list.");
			return -1;
		}

		vint RpcCalleeListOps::DictGetCount(RpcObjectReference ref)
		{
			auto obj = lifecycle->RefToPtr(ref);
			auto dict = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyDictionary>());
			CHECK_ERROR(dict, L"RpcCalleeListOps::DictGetCount cannot find the target dictionary.");
			return dict->GetCount();
		}

		Value RpcCalleeListOps::DictGet(RpcObjectReference ref, const Value& key)
		{
			auto obj = lifecycle->RefToPtr(ref);
			auto dict = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyDictionary>());
			CHECK_ERROR(dict, L"RpcCalleeListOps::DictGet cannot find the target dictionary.");
			return RpcBoxByref(dict->Get(RpcUnboxByref(key, lifecycle)), lifecycle);
		}

		void RpcCalleeListOps::DictSet(RpcObjectReference ref, const Value& key, const Value& value)
		{
			auto obj = lifecycle->RefToPtr(ref);
			auto dict = Ptr(obj.Obj()->SafeAggregationCast<IValueDictionary>());
			CHECK_ERROR(dict, L"RpcCalleeListOps::DictSet cannot find the target dictionary.");
			dict->Set(RpcUnboxByref(key, lifecycle), RpcUnboxByref(value, lifecycle));
		}

		bool RpcCalleeListOps::DictRemove(RpcObjectReference ref, const Value& key)
		{
			auto obj = lifecycle->RefToPtr(ref);
			auto dict = Ptr(obj.Obj()->SafeAggregationCast<IValueDictionary>());
			CHECK_ERROR(dict, L"RpcCalleeListOps::DictRemove cannot find the target dictionary.");
			return dict->Remove(RpcUnboxByref(key, lifecycle));
		}

		void RpcCalleeListOps::DictClear(RpcObjectReference ref)
		{
			auto obj = lifecycle->RefToPtr(ref);
			auto dict = Ptr(obj.Obj()->SafeAggregationCast<IValueDictionary>());
			CHECK_ERROR(dict, L"RpcCalleeListOps::DictClear cannot find the target dictionary.");
			dict->Clear();
		}

		bool RpcCalleeListOps::DictContainsKey(RpcObjectReference ref, const Value& key)
		{
			auto obj = lifecycle->RefToPtr(ref);
			auto dict = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyDictionary>());
			CHECK_ERROR(dict, L"RpcCalleeListOps::DictContainsKey cannot find the target dictionary.");
			return dict->GetKeys()->Contains(RpcUnboxByref(key, lifecycle));
		}

		RpcObjectReference RpcCalleeListOps::DictGetKeys(RpcObjectReference ref)
		{
			auto obj = lifecycle->RefToPtr(ref);
			auto dict = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyDictionary>());
			CHECK_ERROR(dict, L"RpcCalleeListOps::DictGetKeys cannot find the target dictionary.");
			return lifecycle->PtrToRef(dict->GetKeys());
		}

		RpcObjectReference RpcCalleeListOps::DictGetValues(RpcObjectReference ref)
		{
			auto obj = lifecycle->RefToPtr(ref);
			auto dict = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyDictionary>());
			CHECK_ERROR(dict, L"RpcCalleeListOps::DictGetValues cannot find the target dictionary.");
			return lifecycle->PtrToRef(dict->GetValues());
		}
		
/***********************************************************************
* RpcCalleeListEventBridge
***********************************************************************/

		RpcCalleeListEventBridge::RpcCalleeListEventBridge(IRpcLifecycle* lc)
			: lifecycle(lc)
		{
			if (!lifecycle) CHECK_FAIL(L"Invalid IRpcLifecycle.");
		}

		void RpcCalleeListEventBridge::OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)
		{
			auto controller = lifecycle->GetController();
			controller->SetItemChangedSuppressedFlag(ref, true);
			try
			{
				auto obj = lifecycle->RefToPtr(ref);
				auto observable = Ptr(obj.Obj()->SafeAggregationCast<IValueObservableList>());
				CHECK_ERROR(observable, L"RpcCalleeListEventBridge::OnItemChanged cannot find the target observable list.");
				observable->ItemChanged(index, oldCount, newCount);
			}
			catch (...)
			{
				controller->SetItemChangedSuppressedFlag(ref, false);
				throw;
			}
			controller->SetItemChangedSuppressedFlag(ref, false);
		}
		
/***********************************************************************
* Helpers
***********************************************************************/

		Value RpcBoxByref(const Value& trivial, IRpcLifecycle* lc)
		{
			if (!lc) CHECK_FAIL(L"IRpcLifecycle cannot be null.");
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

		Value RpcUnboxByref(const Value& serializable, IRpcLifecycle* lc)
		{
			if (!lc) CHECK_FAIL(L"IRpcLifecycle cannot be null.");
			if (serializable.IsNull()) return serializable;

			if (IsRpcObjectReferenceValue(serializable))
			{
				auto ref = GetRpcObjectReference(serializable);
				return BoxRpcObject(lc->RefToPtr(ref), ref.typeId);
			}

			return serializable;
		}

		Value RpcBoxByvalInternal(const Value& trivial, IRpcLifecycle* lc, Dictionary<const DescriptableObject*, bool>& visited, List<Ptr<IDescriptable>>& keepAlive)
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
							RpcBoxByvalInternal(dictKey, lc, visited, keepAlive),
							RpcBoxByvalInternal(roDict->Get(dictKey), lc, visited, keepAlive)
						);
					}
					visited.Remove(key);
					return BoxValue(dict);
				}

				if (auto obsList = trivial.GetRawPtr() ? Ptr(trivial.GetRawPtr()->SafeAggregationCast<IValueObservableList>()) : nullptr)
				{
					auto key = static_cast<const DescriptableObject*>(trivial.GetRawPtr());
					if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval boxing does not support cycles.");
					visited.Add(key, true);

					auto list = IValueObservableList::Create();
					for (vint i = 0; i < obsList->GetCount(); i++)
					{
						list->Add(RpcBoxByvalInternal(obsList->Get(i), lc, visited, keepAlive));
					}
					visited.Remove(key);
					return BoxValue(list);
				}

				if (auto array = trivial.GetRawPtr() ? Ptr(trivial.GetRawPtr()->SafeAggregationCast<IValueArray>()) : nullptr)
				{
					auto key = static_cast<const DescriptableObject*>(trivial.GetRawPtr());
					if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval boxing does not support cycles.");
					visited.Add(key, true);

					auto list = IValueArray::Create();
					list->Resize(array->GetCount());
					for (vint i = 0; i < array->GetCount(); i++)
					{
						list->Set(i, RpcBoxByvalInternal(array->Get(i), lc, visited, keepAlive));
					}
					visited.Remove(key);
					return BoxValue(list);
				}

				if (auto roList = trivial.GetRawPtr() ? Ptr(trivial.GetRawPtr()->SafeAggregationCast<IValueReadonlyList>()) : nullptr)
				{
					auto key = static_cast<const DescriptableObject*>(trivial.GetRawPtr());
					if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval boxing does not support cycles.");
					visited.Add(key, true);

					auto list = IValueList::Create();
					for (vint i = 0; i < roList->GetCount(); i++)
					{
						list->Add(RpcBoxByvalInternal(roList->Get(i), lc, visited, keepAlive));
					}
					visited.Remove(key);
					return BoxValue(list);
				}

				if (auto raw = trivial.GetRawPtr())
				{
					if (auto obj = dynamic_cast<IDescriptable*>(raw))
					{
						auto ref = lc->PtrToRef(Ptr<IDescriptable>(obj));
						if (obj->SafeAggregationCast<IRpcWrapperBase>())
						{
							keepAlive.Add(Ptr<IDescriptable>(obj));
						}
						return BoxValue(ref);
					}
				}
			}
			return trivial;
		}

		Value RpcBoxByval(const Value& trivial, IRpcLifecycle* lc)
		{
			if (!lc) CHECK_FAIL(L"IRpcLifecycle cannot be null.");
			Dictionary<const DescriptableObject*, bool> visited;
			List<Ptr<IDescriptable>> keepAlive;
			auto serializable = RpcBoxByvalInternal(trivial, lc, visited, keepAlive);
			if (keepAlive.Count() > 0)
			{
				if (auto obj = serializable.GetRawPtr())
				{
					if (auto root = dynamic_cast<DescriptableObject*>(obj))
					{
						auto holder = Ptr(new RpcByvalKeepAlive);
						CopyFrom(holder->objects, keepAlive);
						root->SetInternalProperty(RpcByvalKeepAliveProperty, holder);
					}
				}
			}
			return serializable;
		}

		Value RpcUnboxByvalInternal(const Value& serializable, IRpcLifecycle* lc, Dictionary<const DescriptableObject*, bool>& visited)
		{
			if (serializable.IsNull()) return serializable;

			if (IsRpcObjectReferenceValue(serializable))
			{
				auto ref = GetRpcObjectReference(serializable);
				auto obj = lc->RefToPtr(ref);
				return BoxRpcObject(obj, ref.typeId);
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

				if (auto obsList = serializable.GetRawPtr() ? Ptr(serializable.GetRawPtr()->SafeAggregationCast<IValueObservableList>()) : nullptr)
				{
					auto key = static_cast<const DescriptableObject*>(serializable.GetRawPtr());
					if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval unboxing does not support cycles.");
					visited.Add(key, true);

					auto list = IValueObservableList::Create();
					for (vint i = 0; i < obsList->GetCount(); i++)
					{
						list->Add(RpcUnboxByvalInternal(obsList->Get(i), lc, visited));
					}
					visited.Remove(key);
					return BoxValue(list);
				}

				if (auto array = serializable.GetRawPtr() ? Ptr(serializable.GetRawPtr()->SafeAggregationCast<IValueArray>()) : nullptr)
				{
					auto key = static_cast<const DescriptableObject*>(serializable.GetRawPtr());
					if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval unboxing does not support cycles.");
					visited.Add(key, true);

					auto list = IValueArray::Create();
					list->Resize(array->GetCount());
					for (vint i = 0; i < array->GetCount(); i++)
					{
						list->Set(i, RpcUnboxByvalInternal(array->Get(i), lc, visited));
					}
					visited.Remove(key);
					return BoxValue(list);
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

		Value RpcUnboxByval(const Value& serializable, IRpcLifecycle* lc)
		{
			if (!lc) CHECK_FAIL(L"IRpcLifecycle cannot be null.");
			Dictionary<const DescriptableObject*, bool> visited;
			return RpcUnboxByvalInternal(serializable, lc, visited);
		}
	}
}
