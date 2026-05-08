#include "WfLibraryRpcWrappers.h"
#include "WfLibraryRpcLifecycle.h"
#include "../WfLibraryReflection.h"

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

			bool IsNullRpcObjectReference(RpcObjectReference ref)
			{
				return ref.clientId == RpcClientId_Invalid
					&& ref.objectId == RpcObjectId_Invalid
					&& ref.typeId == RpcTypeId_NotFound;
			}

			IRpcWrapperBase* CastRpcWrapperBase(IDescriptable* obj)
			{
				if (!obj) return nullptr;
				if (auto wrapper = dynamic_cast<IRpcWrapperBase*>(obj)) return wrapper;
				return obj->SafeAggregationCast<IRpcWrapperBase>();
			}

			template<typename TInterface>
			TInterface* CastRpcInterface(IDescriptable* obj)
			{
				if (!obj) return nullptr;
				if (auto proxy = dynamic_cast<TInterface*>(obj)) return proxy;
				return obj->SafeAggregationCast<TInterface>();
			}

			template<typename K, typename V>
			bool ContainsKey(const Dictionary<K, V>& xs, const K& key)
			{
				return xs.Keys().Contains(key);
			}

			template<typename TWrapper, typename TInterface>
			Ptr<TInterface> CreateTrackedProxy(IRpcLifecycle* lc, RpcObjectReference ref, IRpcSerializer* serializer)
			{
				auto obj = [&]()
				{
					if (auto lifecycle = dynamic_cast<RpcLifecycleBase*>(lc))
					{
						return lifecycle->RefToPtr(ref, serializer);
					}
					return lc->RefToPtr(ref);
				}();
				CHECK_ERROR(obj, L"RPC proxy creation returned null.");
				if (auto wrapper = Ptr(CastRpcWrapperBase(obj.Obj())))
				{
					auto proxy = Ptr(CastRpcInterface<TInterface>(obj.Obj()));
					CHECK_ERROR(proxy, L"RPC proxy creation returned an unexpected type.");
					return proxy;
				}
				return Ptr(new TWrapper(lc, ref, serializer));
			}

			void SetRemoteObjectHold(IRpcLifecycle* lc, RpcObjectReference ref, bool hold)
			{
				lc->GetDispatcher()->SendToClient_ObjectOps(ref.clientId)->ObjectHold(ref, lc->GetClientId(), hold);
			}

			IRpcListOps* GetRemoteListOps(IRpcLifecycle* lc, RpcObjectReference ref)
			{
				return lc->GetDispatcher()->SendToClient_ListOps(ref.clientId);
			}

			Value SerializeValue(IRpcSerializer* serializer, const Value& value)
			{
				return serializer ? serializer->Serialize(value) : value;
			}

			Value DeserializeValue(IRpcSerializer* serializer, const Value& value)
			{
				return serializer ? serializer->Deserialize(value) : value;
			}

			Value RpcBoxValueByref(const Value& trivial, IRpcLifecycle* lc);
			Value RpcUnboxValueByref(const Value& serializable, IRpcLifecycle* lc);
			Value RpcCopyValueByvalInternal(const Value& trivial, Dictionary<const DescriptableObject*, bool>& visited);
			Value RpcBoxValueByvalInternal(const Value& trivial, IRpcLifecycle* lc, Dictionary<const DescriptableObject*, bool>& visited);
			Value RpcUnboxValueByvalInternal(const Value& serializable, IRpcLifecycle* lc, Dictionary<const DescriptableObject*, bool>& visited);
		}
		
/***********************************************************************
* RpcByrefEnumerator
***********************************************************************/

		RpcByrefEnumerator::RpcByrefEnumerator(IRpcLifecycle* lc, RpcObjectReference enumeratorRef, IRpcSerializer* _serializer)
			: lifecycle(lc)
			, serializer(_serializer)
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
			return RpcUnboxValueByref(DeserializeValue(serializer, GetRemoteListOps(lifecycle, ref)->EnumGetCurrent(ref)), lifecycle);
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

		RpcByrefEnumerable::RpcByrefEnumerable(IRpcLifecycle* lc, RpcObjectReference enumerableRef, IRpcSerializer* _serializer)
			: lifecycle(lc)
			, serializer(_serializer)
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
			return CreateTrackedProxy<RpcByrefEnumerator, IValueEnumerator>(lifecycle, GetRemoteListOps(lifecycle, ref)->EnumCreate(ref), serializer);
		}
		
/***********************************************************************
* RpcByrefReadonlyList
***********************************************************************/

		RpcByrefReadonlyList::RpcByrefReadonlyList(IRpcLifecycle* lc, RpcObjectReference listRef, IRpcSerializer* _serializer)
			: lifecycle(lc)
			, serializer(_serializer)
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
			return CreateTrackedProxy<RpcByrefEnumerator, IValueEnumerator>(lifecycle, GetRemoteListOps(lifecycle, ref)->EnumCreate(ref), serializer);
		}

		vint RpcByrefReadonlyList::GetCount()
		{
			return GetRemoteListOps(lifecycle, ref)->ListGetCount(ref);
		}

		Value RpcByrefReadonlyList::Get(vint index)
		{
			return RpcUnboxValueByref(DeserializeValue(serializer, GetRemoteListOps(lifecycle, ref)->ListGet(ref, index)), lifecycle);
		}

		bool RpcByrefReadonlyList::Contains(const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListContains(ref, SerializeValue(serializer, RpcBoxValueByref(value, lifecycle)));
		}

		vint RpcByrefReadonlyList::IndexOf(const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListIndexOf(ref, SerializeValue(serializer, RpcBoxValueByref(value, lifecycle)));
		}
		
/***********************************************************************
* RpcByrefList
***********************************************************************/

		RpcByrefList::RpcByrefList(IRpcLifecycle* lc, RpcObjectReference listRef, IRpcSerializer* _serializer)
			: RpcByrefReadonlyList(lc, listRef, _serializer)
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
			GetRemoteListOps(lifecycle, ref)->ListSet(ref, index, SerializeValue(serializer, RpcBoxValueByref(value, lifecycle)));
		}

		vint RpcByrefList::Add(const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListAdd(ref, SerializeValue(serializer, RpcBoxValueByref(value, lifecycle)));
		}

		vint RpcByrefList::Insert(vint index, const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListInsert(ref, index, SerializeValue(serializer, RpcBoxValueByref(value, lifecycle)));
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

		RpcByrefArray::RpcByrefArray(IRpcLifecycle* lc, RpcObjectReference arrayRef, IRpcSerializer* _serializer)
			: lifecycle(lc)
			, serializer(_serializer)
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
			return CreateTrackedProxy<RpcByrefEnumerator, IValueEnumerator>(lifecycle, GetRemoteListOps(lifecycle, ref)->EnumCreate(ref), serializer);
		}

		vint RpcByrefArray::GetCount()
		{
			return GetRemoteListOps(lifecycle, ref)->ListGetCount(ref);
		}

		Value RpcByrefArray::Get(vint index)
		{
			return RpcUnboxValueByref(DeserializeValue(serializer, GetRemoteListOps(lifecycle, ref)->ListGet(ref, index)), lifecycle);
		}

		bool RpcByrefArray::Contains(const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListContains(ref, SerializeValue(serializer, RpcBoxValueByref(value, lifecycle)));
		}

		vint RpcByrefArray::IndexOf(const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListIndexOf(ref, SerializeValue(serializer, RpcBoxValueByref(value, lifecycle)));
		}

		void RpcByrefArray::Set(vint index, const Value& value)
		{
			GetRemoteListOps(lifecycle, ref)->ListSet(ref, index, SerializeValue(serializer, RpcBoxValueByref(value, lifecycle)));
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

		RpcByrefObservableList::RpcByrefObservableList(IRpcLifecycle* lc, RpcObjectReference listRef, IRpcSerializer* _serializer)
			: lifecycle(lc)
			, serializer(_serializer)
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
			return CreateTrackedProxy<RpcByrefEnumerator, IValueEnumerator>(lifecycle, GetRemoteListOps(lifecycle, ref)->EnumCreate(ref), serializer);
		}

		vint RpcByrefObservableList::GetCount()
		{
			return GetRemoteListOps(lifecycle, ref)->ListGetCount(ref);
		}

		Value RpcByrefObservableList::Get(vint index)
		{
			return RpcUnboxValueByref(DeserializeValue(serializer, GetRemoteListOps(lifecycle, ref)->ListGet(ref, index)), lifecycle);
		}

		bool RpcByrefObservableList::Contains(const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListContains(ref, SerializeValue(serializer, RpcBoxValueByref(value, lifecycle)));
		}

		vint RpcByrefObservableList::IndexOf(const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListIndexOf(ref, SerializeValue(serializer, RpcBoxValueByref(value, lifecycle)));
		}

		void RpcByrefObservableList::Set(vint index, const Value& value)
		{
			GetRemoteListOps(lifecycle, ref)->ListSet(ref, index, SerializeValue(serializer, RpcBoxValueByref(value, lifecycle)));
		}

		vint RpcByrefObservableList::Add(const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListAdd(ref, SerializeValue(serializer, RpcBoxValueByref(value, lifecycle)));
		}

		vint RpcByrefObservableList::Insert(vint index, const Value& value)
		{
			return GetRemoteListOps(lifecycle, ref)->ListInsert(ref, index, SerializeValue(serializer, RpcBoxValueByref(value, lifecycle)));
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

		RpcByrefDictionary::RpcByrefDictionary(IRpcLifecycle* lc, RpcObjectReference dictRef, IRpcSerializer* _serializer)
			: lifecycle(lc)
			, serializer(_serializer)
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
			return CreateTrackedProxy<RpcByrefReadonlyList, IValueReadonlyList>(lifecycle, GetRemoteListOps(lifecycle, ref)->DictGetKeys(ref), serializer);
		}

		Ptr<IValueReadonlyList> RpcByrefDictionary::GetValues()
		{
			return CreateTrackedProxy<RpcByrefReadonlyList, IValueReadonlyList>(lifecycle, GetRemoteListOps(lifecycle, ref)->DictGetValues(ref), serializer);
		}

		vint RpcByrefDictionary::GetCount()
		{
			return GetRemoteListOps(lifecycle, ref)->DictGetCount(ref);
		}

		Value RpcByrefDictionary::Get(const Value& key)
		{
			auto serializedKey = SerializeValue(serializer, RpcBoxValueByref(key, lifecycle));
			auto serializedValue = GetRemoteListOps(lifecycle, ref)->DictGet(ref, serializedKey);
			return RpcUnboxValueByref(DeserializeValue(serializer, serializedValue), lifecycle);
		}

		void RpcByrefDictionary::Set(const Value& key, const Value& value)
		{
			auto serializedKey = SerializeValue(serializer, RpcBoxValueByref(key, lifecycle));
			auto serializedValue = SerializeValue(serializer, RpcBoxValueByref(value, lifecycle));
			GetRemoteListOps(lifecycle, ref)->DictSet(ref, serializedKey, serializedValue);
		}

		bool RpcByrefDictionary::Remove(const Value& key)
		{
			return GetRemoteListOps(lifecycle, ref)->DictRemove(ref, SerializeValue(serializer, RpcBoxValueByref(key, lifecycle)));
		}

		void RpcByrefDictionary::Clear()
		{
			GetRemoteListOps(lifecycle, ref)->DictClear(ref);
		}
		
/***********************************************************************
* RpcCalleeListOps
***********************************************************************/

		RpcCalleeListOps::RpcCalleeListOps(IRpcLifecycle* lc, IRpcSerializer* _serializer)
			: lifecycle(lc)
			, serializer(_serializer)
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
			return SerializeValue(serializer, RpcBoxValueByref(e->GetCurrent(), lifecycle));
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
				return SerializeValue(serializer, RpcBoxValueByref(roList->Get(index), lifecycle));
			CHECK_FAIL(L"RpcCalleeListOps::ListGet cannot find the target list.");
			return {};
		}

		void RpcCalleeListOps::ListSet(RpcObjectReference ref, vint index, const Value& value)
		{
			auto trivial = RpcUnboxValueByref(DeserializeValue(serializer, value), lifecycle);
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
			auto trivial = RpcUnboxValueByref(DeserializeValue(serializer, value), lifecycle);
			auto obj = lifecycle->RefToPtr(ref);
			if (auto list = Ptr(obj.Obj()->SafeAggregationCast<IValueList>()))
				return list->Add(trivial);
			CHECK_FAIL(L"RpcCalleeListOps::ListAdd cannot find a writable list.");
			return -1;
		}

		vint RpcCalleeListOps::ListInsert(RpcObjectReference ref, vint index, const Value& value)
		{
			auto trivial = RpcUnboxValueByref(DeserializeValue(serializer, value), lifecycle);
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
			auto trivial = RpcUnboxValueByref(DeserializeValue(serializer, value), lifecycle);
			auto obj = lifecycle->RefToPtr(ref);
			if (auto roList = Ptr(obj.Obj()->SafeAggregationCast<IValueReadonlyList>()))
				return roList->Contains(trivial);
			CHECK_FAIL(L"RpcCalleeListOps::ListContains cannot find the target list.");
			return false;
		}

		vint RpcCalleeListOps::ListIndexOf(RpcObjectReference ref, const Value& value)
		{
			auto trivial = RpcUnboxValueByref(DeserializeValue(serializer, value), lifecycle);
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
			auto trivialKey = RpcUnboxValueByref(DeserializeValue(serializer, key), lifecycle);
			return SerializeValue(serializer, RpcBoxValueByref(dict->Get(trivialKey), lifecycle));
		}

		void RpcCalleeListOps::DictSet(RpcObjectReference ref, const Value& key, const Value& value)
		{
			auto obj = lifecycle->RefToPtr(ref);
			auto dict = Ptr(obj.Obj()->SafeAggregationCast<IValueDictionary>());
			CHECK_ERROR(dict, L"RpcCalleeListOps::DictSet cannot find the target dictionary.");
			auto trivialKey = RpcUnboxValueByref(DeserializeValue(serializer, key), lifecycle);
			auto trivialValue = RpcUnboxValueByref(DeserializeValue(serializer, value), lifecycle);
			dict->Set(trivialKey, trivialValue);
		}

		bool RpcCalleeListOps::DictRemove(RpcObjectReference ref, const Value& key)
		{
			auto obj = lifecycle->RefToPtr(ref);
			auto dict = Ptr(obj.Obj()->SafeAggregationCast<IValueDictionary>());
			CHECK_ERROR(dict, L"RpcCalleeListOps::DictRemove cannot find the target dictionary.");
			return dict->Remove(RpcUnboxValueByref(DeserializeValue(serializer, key), lifecycle));
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
			return dict->GetKeys()->Contains(RpcUnboxValueByref(DeserializeValue(serializer, key), lifecycle));
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

		RpcCalleeListEventBridge::RpcCalleeListEventBridge(IRpcLifecycle* lc, IRpcSerializer* _serializer)
			: lifecycle(lc)
			, serializer(_serializer)
		{
			if (!lifecycle) CHECK_FAIL(L"Invalid IRpcLifecycle.");
		}

		RpcEventExceptionMap RpcCalleeListEventBridge::OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)
		{
			auto controller = lifecycle->GetController();
			auto obj = lifecycle->RefToPtr(ref);
			auto observable = Ptr(obj.Obj()->SafeAggregationCast<IValueObservableList>());
			CHECK_ERROR(observable, L"RpcCalleeListEventBridge::OnItemChanged cannot find the target observable list.");
			controller->SetItemChangedSuppressedFlag(ref, true);
			RpcEventExceptionMap exceptions;
			try
			{
				observable->ItemChanged(index, oldCount, newCount);
			}
			catch (const Exception& ex)
			{
				exceptions = CreateRpcEventExceptionMap();
				exceptions->Set(BoxValue(lifecycle->GetClientId()), BoxValue(RpcException{ ex.Message() }));
			}
			catch (...)
			{
				controller->SetItemChangedSuppressedFlag(ref, false);
				throw;
			}
			controller->SetItemChangedSuppressedFlag(ref, false);
			return exceptions;
		}
		
/***********************************************************************
* Helpers
***********************************************************************/

		RpcObjectReference RpcBoxByref(Ptr<IDescriptable> trivial, IRpcLifecycle* lc)
		{
			if (!lc) CHECK_FAIL(L"IRpcLifecycle cannot be null.");
			if (!trivial) return {};
			return lc->PtrToRef(trivial);
		}

		Ptr<IDescriptable> RpcUnboxByref(RpcObjectReference serializable, IRpcLifecycle* lc)
		{
			if (!lc) CHECK_FAIL(L"IRpcLifecycle cannot be null.");
			if (IsNullRpcObjectReference(serializable)) return nullptr;
			return lc->RefToPtr(serializable);
		}

		namespace
		{
			Value RpcBoxValueByref(const Value& trivial, IRpcLifecycle* lc)
			{
				if (!lc) CHECK_FAIL(L"IRpcLifecycle cannot be null.");
				if (trivial.IsNull()) return trivial;

				if (trivial.GetValueType() == Value::SharedPtr)
				{
					if (auto raw = trivial.GetRawPtr())
					{
						if (auto obj = dynamic_cast<IDescriptable*>(raw))
						{
							return BoxValue(RpcBoxByref(Ptr<IDescriptable>(obj), lc));
						}
					}
				}

				return trivial;
			}

			Value RpcUnboxValueByref(const Value& serializable, IRpcLifecycle* lc)
			{
				if (!lc) CHECK_FAIL(L"IRpcLifecycle cannot be null.");
				if (serializable.IsNull()) return serializable;

				if (IsRpcObjectReferenceValue(serializable))
				{
					auto ref = GetRpcObjectReference(serializable);
					auto obj = RpcUnboxByref(ref, lc);
					return obj ? BoxValue(obj) : Value{};
				}

				return serializable;
			}
		}

		namespace
		{
			template<typename TInterface>
			Ptr<TInterface> TryGetValueInterface(const Value& value)
			{
				if (auto raw = value.GetRawPtr())
				{
					return Ptr(dynamic_cast<TInterface*>(raw));
				}
				return nullptr;
			}

			Value RpcCopyValueByvalInternal(const Value& trivial, Dictionary<const DescriptableObject*, bool>& visited)
			{
				if (trivial.IsNull()) return trivial;
				if (trivial.GetValueType() == Value::SharedPtr)
				{
					if (auto roDict = TryGetValueInterface<IValueReadonlyDictionary>(trivial))
					{
						auto key = static_cast<const DescriptableObject*>(trivial.GetRawPtr());
						if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval copying does not support cycles.");
						visited.Add(key, true);

						auto dict = IValueDictionary::Create();
						auto keys = roDict->GetKeys();
						for (vint i = 0; i < keys->GetCount(); i++)
						{
							auto dictKey = keys->Get(i);
							dict->Set(
								RpcCopyValueByvalInternal(dictKey, visited),
								RpcCopyValueByvalInternal(roDict->Get(dictKey), visited)
							);
						}
						visited.Remove(key);
						return BoxValue(dict);
					}

					if (auto obsList = TryGetValueInterface<IValueObservableList>(trivial))
					{
						auto key = static_cast<const DescriptableObject*>(trivial.GetRawPtr());
						if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval copying does not support cycles.");
						visited.Add(key, true);

						auto list = IValueObservableList::Create();
						for (vint i = 0; i < obsList->GetCount(); i++)
						{
							list->Add(RpcCopyValueByvalInternal(obsList->Get(i), visited));
						}
						visited.Remove(key);
						return BoxValue(list);
					}

					if (auto array = TryGetValueInterface<IValueArray>(trivial))
					{
						auto key = static_cast<const DescriptableObject*>(trivial.GetRawPtr());
						if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval copying does not support cycles.");
						visited.Add(key, true);

						auto list = IValueArray::Create();
						list->Resize(array->GetCount());
						for (vint i = 0; i < array->GetCount(); i++)
						{
							list->Set(i, RpcCopyValueByvalInternal(array->Get(i), visited));
						}
						visited.Remove(key);
						return BoxValue(list);
					}

					if (auto roList = TryGetValueInterface<IValueReadonlyList>(trivial))
					{
						auto key = static_cast<const DescriptableObject*>(trivial.GetRawPtr());
						if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval copying does not support cycles.");
						visited.Add(key, true);

						auto list = IValueList::Create();
						for (vint i = 0; i < roList->GetCount(); i++)
						{
							list->Add(RpcCopyValueByvalInternal(roList->Get(i), visited));
						}
						visited.Remove(key);
						return BoxValue(list);
					}
				}
				return trivial;
			}

			Value RpcBoxValueByvalInternal(const Value& trivial, IRpcLifecycle* lc, Dictionary<const DescriptableObject*, bool>& visited)
			{
				if (trivial.IsNull()) return trivial;
				if (trivial.GetValueType() == Value::SharedPtr)
				{
					if (auto roDict = TryGetValueInterface<IValueReadonlyDictionary>(trivial))
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
								RpcBoxValueByvalInternal(dictKey, lc, visited),
								RpcBoxValueByvalInternal(roDict->Get(dictKey), lc, visited)
							);
						}
						visited.Remove(key);
						return BoxValue(dict);
					}

					if (auto obsList = TryGetValueInterface<IValueObservableList>(trivial))
					{
						auto key = static_cast<const DescriptableObject*>(trivial.GetRawPtr());
						if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval boxing does not support cycles.");
						visited.Add(key, true);

						auto list = IValueObservableList::Create();
						for (vint i = 0; i < obsList->GetCount(); i++)
						{
							list->Add(RpcBoxValueByvalInternal(obsList->Get(i), lc, visited));
						}
						visited.Remove(key);
						return BoxValue(list);
					}

					if (auto array = TryGetValueInterface<IValueArray>(trivial))
					{
						auto key = static_cast<const DescriptableObject*>(trivial.GetRawPtr());
						if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval boxing does not support cycles.");
						visited.Add(key, true);

						auto list = IValueArray::Create();
						list->Resize(array->GetCount());
						for (vint i = 0; i < array->GetCount(); i++)
						{
							list->Set(i, RpcBoxValueByvalInternal(array->Get(i), lc, visited));
						}
						visited.Remove(key);
						return BoxValue(list);
					}

					if (auto roList = TryGetValueInterface<IValueReadonlyList>(trivial))
					{
						auto key = static_cast<const DescriptableObject*>(trivial.GetRawPtr());
						if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval boxing does not support cycles.");
						visited.Add(key, true);

						auto list = IValueList::Create();
						for (vint i = 0; i < roList->GetCount(); i++)
						{
							list->Add(RpcBoxValueByvalInternal(roList->Get(i), lc, visited));
						}
						visited.Remove(key);
						return BoxValue(list);
					}

					if (auto raw = trivial.GetRawPtr())
					{
						if (auto obj = dynamic_cast<IDescriptable*>(raw))
						{
							auto ref = lc->PtrToRef(Ptr<IDescriptable>(obj));
							return BoxValue(ref);
						}
					}
				}
				return trivial;
			}
		}

		Value RpcCopyByval(const Value& trivial, IRpcLifecycle* lc)
		{
			if (!lc) CHECK_FAIL(L"IRpcLifecycle cannot be null.");
			if (trivial.IsNull()) return {};
			Dictionary<const DescriptableObject*, bool> visited;
			return RpcCopyValueByvalInternal(trivial, visited);
		}

		Value RpcBoxByval(Ptr<IDescriptable> trivial, IRpcLifecycle* lc)
		{
			if (!lc) CHECK_FAIL(L"IRpcLifecycle cannot be null.");
			if (!trivial) return {};
			return RpcBoxByval(BoxValue(trivial), lc);
		}

		Value RpcBoxByval(const Value& trivial, IRpcLifecycle* lc)
		{
			if (!lc) CHECK_FAIL(L"IRpcLifecycle cannot be null.");
			if (trivial.IsNull()) return {};
			Dictionary<const DescriptableObject*, bool> visited;
			return RpcBoxValueByvalInternal(trivial, lc, visited);
		}

		namespace
		{
			Value RpcUnboxValueByvalInternal(const Value& serializable, IRpcLifecycle* lc, Dictionary<const DescriptableObject*, bool>& visited)
			{
				if (serializable.IsNull()) return serializable;

				if (IsRpcObjectReferenceValue(serializable))
				{
					auto ref = GetRpcObjectReference(serializable);
					auto obj = RpcUnboxByref(ref, lc);
					return obj ? BoxValue(obj) : Value{};
				}

				if (serializable.GetValueType() == Value::SharedPtr)
				{
					if (auto roDict = TryGetValueInterface<IValueReadonlyDictionary>(serializable))
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
								RpcUnboxValueByvalInternal(dictKey, lc, visited),
								RpcUnboxValueByvalInternal(roDict->Get(dictKey), lc, visited)
							);
						}
						visited.Remove(key);
						return BoxValue(dict);
					}

					if (auto obsList = TryGetValueInterface<IValueObservableList>(serializable))
					{
						auto key = static_cast<const DescriptableObject*>(serializable.GetRawPtr());
						if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval unboxing does not support cycles.");
						visited.Add(key, true);

						auto list = IValueObservableList::Create();
						for (vint i = 0; i < obsList->GetCount(); i++)
						{
							list->Add(RpcUnboxValueByvalInternal(obsList->Get(i), lc, visited));
						}
						visited.Remove(key);
						return BoxValue(list);
					}

					if (auto array = TryGetValueInterface<IValueArray>(serializable))
					{
						auto key = static_cast<const DescriptableObject*>(serializable.GetRawPtr());
						if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval unboxing does not support cycles.");
						visited.Add(key, true);

						auto list = IValueArray::Create();
						list->Resize(array->GetCount());
						for (vint i = 0; i < array->GetCount(); i++)
						{
							list->Set(i, RpcUnboxValueByvalInternal(array->Get(i), lc, visited));
						}
						visited.Remove(key);
						return BoxValue(list);
					}

					if (auto roList = TryGetValueInterface<IValueReadonlyList>(serializable))
					{
						auto key = static_cast<const DescriptableObject*>(serializable.GetRawPtr());
						if (ContainsKey(visited, key)) CHECK_FAIL(L"Byval unboxing does not support cycles.");
						visited.Add(key, true);

						auto list = IValueList::Create();
						for (vint i = 0; i < roList->GetCount(); i++)
						{
							list->Add(RpcUnboxValueByvalInternal(roList->Get(i), lc, visited));
						}
						visited.Remove(key);
						return BoxValue(list);
					}
				}
				return serializable;
			}
		}

		Ptr<IDescriptable> RpcUnboxByval(const Value& serializable, IRpcLifecycle* lc)
		{
			if (!lc) CHECK_FAIL(L"IRpcLifecycle cannot be null.");
			if (serializable.IsNull()) return nullptr;
			Dictionary<const DescriptableObject*, bool> visited;
			auto trivial = RpcUnboxValueByvalInternal(serializable, lc, visited);
			if (auto raw = trivial.GetRawPtr())
			{
				if (auto obj = dynamic_cast<IDescriptable*>(raw))
				{
					return Ptr<IDescriptable>(obj);
				}
			}

			CHECK_FAIL(L"Interface value or null is expected.");
			return nullptr;
		}

		void ReadEventException(RpcEventExceptionMap exceptions)
		{
			if (!exceptions || exceptions->GetCount() == 0) return;

			WString message;
			auto keys = exceptions->GetKeys();
			for (vint i = 0; i < keys->GetCount(); i++)
			{
				auto key = keys->Get(i);
				auto exception = UnboxValue<RpcException>(exceptions->Get(key));
				message += itow(UnboxValue<vint>(key)) + L":" + exception.message + L";";
			}
			throw Exception(message);
		}
	}
}
