#include "WfLibraryRpcWrappers.h"
#include "WfLibraryRpcLifecycle.h"
#include "WfLibraryRpcJson.h"
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

			template<typename ...TArgs>
			Ptr<IValueArray> CreateRpcArguments(TArgs&& ...args)
			{
				auto arguments = IValueArray::Create();
				arguments->Resize(sizeof...(TArgs));
				vint index = 0;
				((arguments->Set(index++, std::forward<TArgs>(args))), ...);
				return arguments;
			}

			Value BoxPrimitiveArgument(IRpcSerializer* serializer, vint value)
			{
				return SerializeValue(serializer, BoxValue(value));
			}

			vint UnboxPrimitiveArgument(IRpcSerializer* serializer, const Value& value)
			{
				return UnboxValue<vint>(DeserializeValue(serializer, value));
			}

			Value ReadMethodResult(IRpcSerializer* serializer, const Value& value)
			{
				auto deserialized = DeserializeValue(serializer, value);
				ReadMethodException(deserialized);
				return deserialized;
			}

			Value InvokeListMethod(IRpcObjectOps* objectOps, IRpcSerializer* serializer, RpcObjectReference ref, vint methodId, Ptr<IValueArray> arguments)
			{
				auto result = objectOps->InvokeMethod(ref, methodId, arguments);
				ReadMethodResult(serializer, result);
				return result;
			}

			bool IsRpcListMethodId(vint methodId)
			{
				switch (methodId)
				{
				case RpcMethodId_IValueEnumerable_CreateEnumerator:
				case RpcMethodId_IValueEnumerator_Next:
				case RpcMethodId_IValueEnumerator_GetCurrent:
				case RpcMethodId_IValueReadonlyList_GetCount:
				case RpcMethodId_IValueReadonlyList_Get:
				case RpcMethodId_IValueList_Set:
				case RpcMethodId_IValueList_Add:
				case RpcMethodId_IValueList_Insert:
				case RpcMethodId_IValueList_RemoveAt:
				case RpcMethodId_IValueList_Clear:
				case RpcMethodId_IValueReadonlyList_Contains:
				case RpcMethodId_IValueReadonlyList_IndexOf:
				case RpcMethodId_IValueReadonlyDictionary_GetCount:
				case RpcMethodId_IValueReadonlyDictionary_Get:
				case RpcMethodId_IValueDictionary_Set:
				case RpcMethodId_IValueDictionary_Remove:
				case RpcMethodId_IValueDictionary_Clear:
				case RpcMethodId_IValueReadonlyDictionary_ContainsKey:
				case RpcMethodId_IValueReadonlyDictionary_GetKeys:
				case RpcMethodId_IValueReadonlyDictionary_GetValues:
					return true;
				default:
					return false;
				}
			}

			extern Value RpcBoxValueByref(const Value& trivial, IRpcLifecycle* lc);
			extern Value RpcUnboxValueByref(const Value& serializable, IRpcLifecycle* lc);
			extern Value RpcCopyValueByvalInternal(const Value& trivial, Dictionary<const DescriptableObject*, bool>& visited);
			extern Value RpcBoxValueByvalInternal(const Value& trivial, IRpcLifecycle* lc, Dictionary<const DescriptableObject*, bool>& visited);
			extern Value RpcUnboxValueByvalInternal(const Value& serializable, IRpcLifecycle* lc, Dictionary<const DescriptableObject*, bool>& visited);

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
* RpcCalleeListEventOps
***********************************************************************/

		RpcCalleeListEventOps::RpcCalleeListEventOps(IRpcLifecycle* lc, IRpcSerializer* _serializer)
			: lifecycle(lc)
			, serializer(_serializer)
		{
			if (!lifecycle) CHECK_FAIL(L"Invalid IRpcLifecycle.");
		}

		Value RpcCalleeListEventOps::OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)
		{
			auto controller = lifecycle->GetController();
			auto obj = lifecycle->RefToPtr(ref);
			auto observable = Ptr(obj.Obj()->SafeAggregationCast<IValueObservableList>());
			CHECK_ERROR(observable, L"RpcCalleeListEventOps::OnItemChanged cannot find the target observable list.");
			controller->SetItemChangedSuppressedFlag(ref, true);
			RpcEventExceptionMap exceptions;
			try
			{
				observable->ItemChanged(index, oldCount, newCount);
			}
			catch (const Exception& ex)
			{
				exceptions = CreateRpcEventExceptionMap();
				exceptions->Set(BoxValue(lifecycle->GetClientId()), BoxRpcException(RpcException{ ex.Message() }));
			}
			catch (...)
			{
				controller->SetItemChangedSuppressedFlag(ref, false);
				throw;
			}
			controller->SetItemChangedSuppressedFlag(ref, false);
			return SerializeValue(serializer, BoxRpcEventExceptionMap(exceptions));
		}

/***********************************************************************
* RpcCalleeObjectOpsForList
***********************************************************************/

		RpcCalleeObjectOpsForList::RpcCalleeObjectOpsForList(Ptr<RpcCalleeListOps> _listOps, Ptr<IRpcObjectOps> _objectOps, IRpcSerializer* _serializer)
			: listOps(_listOps)
			, objectOps(_objectOps)
			, serializer(_serializer)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcCalleeObjectOpsForList::RpcCalleeObjectOpsForList(...)#"
			CHECK_ERROR(listOps && objectOps, ERROR_MESSAGE_PREFIX L"List ops and object ops are required.");
#undef ERROR_MESSAGE_PREFIX
		}

		Value RpcCalleeObjectOpsForList::InvokeMethod(RpcObjectReference ref, vint methodId, Ptr<IValueArray> arguments)
		{
			if (!IsRpcListMethodId(methodId))
			{
				return objectOps->InvokeMethod(ref, methodId, arguments);
			}

			try
			{
				switch (methodId)
				{
				case RpcMethodId_IValueEnumerable_CreateEnumerator:
					return SerializeValue(serializer, BoxValue(listOps->EnumCreate(ref)));
				case RpcMethodId_IValueEnumerator_Next:
					return SerializeValue(serializer, BoxValue(listOps->EnumNext(ref)));
				case RpcMethodId_IValueEnumerator_GetCurrent:
					return listOps->EnumGetCurrent(ref);
				case RpcMethodId_IValueReadonlyList_GetCount:
					return SerializeValue(serializer, BoxValue(listOps->ListGetCount(ref)));
				case RpcMethodId_IValueReadonlyList_Get:
					return listOps->ListGet(ref, UnboxPrimitiveArgument(serializer, arguments->Get(0)));
				case RpcMethodId_IValueList_Set:
					listOps->ListSet(ref, UnboxPrimitiveArgument(serializer, arguments->Get(0)), arguments->Get(1));
					return SerializeValue(serializer, Value());
				case RpcMethodId_IValueList_Add:
					return SerializeValue(serializer, BoxValue(listOps->ListAdd(ref, arguments->Get(0))));
				case RpcMethodId_IValueList_Insert:
					return SerializeValue(serializer, BoxValue(listOps->ListInsert(ref, UnboxPrimitiveArgument(serializer, arguments->Get(0)), arguments->Get(1))));
				case RpcMethodId_IValueList_RemoveAt:
					return SerializeValue(serializer, BoxValue(listOps->ListRemoveAt(ref, UnboxPrimitiveArgument(serializer, arguments->Get(0)))));
				case RpcMethodId_IValueList_Clear:
					listOps->ListClear(ref);
					return SerializeValue(serializer, Value());
				case RpcMethodId_IValueReadonlyList_Contains:
					return SerializeValue(serializer, BoxValue(listOps->ListContains(ref, arguments->Get(0))));
				case RpcMethodId_IValueReadonlyList_IndexOf:
					return SerializeValue(serializer, BoxValue(listOps->ListIndexOf(ref, arguments->Get(0))));
				case RpcMethodId_IValueReadonlyDictionary_GetCount:
					return SerializeValue(serializer, BoxValue(listOps->DictGetCount(ref)));
				case RpcMethodId_IValueReadonlyDictionary_Get:
					return listOps->DictGet(ref, arguments->Get(0));
				case RpcMethodId_IValueDictionary_Set:
					listOps->DictSet(ref, arguments->Get(0), arguments->Get(1));
					return SerializeValue(serializer, Value());
				case RpcMethodId_IValueDictionary_Remove:
					return SerializeValue(serializer, BoxValue(listOps->DictRemove(ref, arguments->Get(0))));
				case RpcMethodId_IValueDictionary_Clear:
					listOps->DictClear(ref);
					return SerializeValue(serializer, Value());
				case RpcMethodId_IValueReadonlyDictionary_ContainsKey:
					return SerializeValue(serializer, BoxValue(listOps->DictContainsKey(ref, arguments->Get(0))));
				case RpcMethodId_IValueReadonlyDictionary_GetKeys:
					return SerializeValue(serializer, BoxValue(listOps->DictGetKeys(ref)));
				case RpcMethodId_IValueReadonlyDictionary_GetValues:
					return SerializeValue(serializer, BoxValue(listOps->DictGetValues(ref)));
				}
			}
			catch (const Exception& ex)
			{
				return SerializeValue(serializer, BoxRpcException(RpcException{ ex.Message() }));
			}
			CHECK_FAIL(L"Unknown RPC list method id.");
			return {};
		}

		void RpcCalleeObjectOpsForList::EndInvokeMethod(vint slot)
		{
			objectOps->EndInvokeMethod(slot);
		}

		void RpcCalleeObjectOpsForList::ObjectHold(RpcObjectReference ref, vint remoteClientId, bool hold)
		{
			objectOps->ObjectHold(ref, remoteClientId, hold);
		}

		void RpcCalleeObjectOpsForList::RegisterService(vint typeId, Ptr<IDescriptable> service)
		{
			objectOps->RegisterService(typeId, service);
		}

/***********************************************************************
* RpcCalleeObjectEventOpsForList
***********************************************************************/

		RpcCalleeObjectEventOpsForList::RpcCalleeObjectEventOpsForList(Ptr<RpcCalleeListEventOps> _listEventOps, Ptr<IRpcObjectEventOps> _objectEventOps, IRpcSerializer* _serializer)
			: listEventOps(_listEventOps)
			, objectEventOps(_objectEventOps)
			, serializer(_serializer)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcCalleeObjectEventOpsForList::RpcCalleeObjectEventOpsForList(...)#"
			CHECK_ERROR(listEventOps && objectEventOps, ERROR_MESSAGE_PREFIX L"List event ops and object event ops are required.");
#undef ERROR_MESSAGE_PREFIX
		}

		Value RpcCalleeObjectEventOpsForList::InvokeEvent(RpcObjectReference ref, vint eventId, Ptr<IValueArray> arguments)
		{
			if (eventId == RpcEventId_IValueObservableList_ItemChanged)
			{
				return listEventOps->OnItemChanged(
					ref,
					UnboxPrimitiveArgument(serializer, arguments->Get(0)),
					UnboxPrimitiveArgument(serializer, arguments->Get(1)),
					UnboxPrimitiveArgument(serializer, arguments->Get(2))
					);
			}
			return objectEventOps->InvokeEvent(ref, eventId, arguments);
		}

/***********************************************************************
* RpcCallerListOps
***********************************************************************/

		RpcCallerListOps::RpcCallerListOps(IRpcObjectOps* _objectOps, IRpcSerializer* _serializer)
			: objectOps(_objectOps)
			, serializer(_serializer)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcCallerListOps::RpcCallerListOps(...)#"
			CHECK_ERROR(objectOps, ERROR_MESSAGE_PREFIX L"Object ops are required.");
#undef ERROR_MESSAGE_PREFIX
		}

		RpcObjectReference RpcCallerListOps::EnumCreate(RpcObjectReference ref)
		{
			auto result = InvokeListMethod(objectOps, serializer, ref, RpcMethodId_IValueEnumerable_CreateEnumerator, CreateRpcArguments());
			return UnboxValue<RpcObjectReference>(ReadMethodResult(serializer, result));
		}

		bool RpcCallerListOps::EnumNext(RpcObjectReference enumerator)
		{
			auto result = InvokeListMethod(objectOps, serializer, enumerator, RpcMethodId_IValueEnumerator_Next, CreateRpcArguments());
			return UnboxValue<bool>(ReadMethodResult(serializer, result));
		}

		Value RpcCallerListOps::EnumGetCurrent(RpcObjectReference enumerator)
		{
			return InvokeListMethod(objectOps, serializer, enumerator, RpcMethodId_IValueEnumerator_GetCurrent, CreateRpcArguments());
		}

		vint RpcCallerListOps::ListGetCount(RpcObjectReference ref)
		{
			auto result = InvokeListMethod(objectOps, serializer, ref, RpcMethodId_IValueReadonlyList_GetCount, CreateRpcArguments());
			return UnboxValue<vint>(ReadMethodResult(serializer, result));
		}

		Value RpcCallerListOps::ListGet(RpcObjectReference ref, vint index)
		{
			return InvokeListMethod(objectOps, serializer, ref, RpcMethodId_IValueReadonlyList_Get, CreateRpcArguments(BoxPrimitiveArgument(serializer, index)));
		}

		void RpcCallerListOps::ListSet(RpcObjectReference ref, vint index, const Value& value)
		{
			InvokeListMethod(objectOps, serializer, ref, RpcMethodId_IValueList_Set, CreateRpcArguments(BoxPrimitiveArgument(serializer, index), value));
		}

		vint RpcCallerListOps::ListAdd(RpcObjectReference ref, const Value& value)
		{
			auto result = InvokeListMethod(objectOps, serializer, ref, RpcMethodId_IValueList_Add, CreateRpcArguments(value));
			return UnboxValue<vint>(ReadMethodResult(serializer, result));
		}

		vint RpcCallerListOps::ListInsert(RpcObjectReference ref, vint index, const Value& value)
		{
			auto result = InvokeListMethod(objectOps, serializer, ref, RpcMethodId_IValueList_Insert, CreateRpcArguments(BoxPrimitiveArgument(serializer, index), value));
			return UnboxValue<vint>(ReadMethodResult(serializer, result));
		}

		bool RpcCallerListOps::ListRemoveAt(RpcObjectReference ref, vint index)
		{
			auto result = InvokeListMethod(objectOps, serializer, ref, RpcMethodId_IValueList_RemoveAt, CreateRpcArguments(BoxPrimitiveArgument(serializer, index)));
			return UnboxValue<bool>(ReadMethodResult(serializer, result));
		}

		void RpcCallerListOps::ListClear(RpcObjectReference ref)
		{
			InvokeListMethod(objectOps, serializer, ref, RpcMethodId_IValueList_Clear, CreateRpcArguments());
		}

		bool RpcCallerListOps::ListContains(RpcObjectReference ref, const Value& value)
		{
			auto result = InvokeListMethod(objectOps, serializer, ref, RpcMethodId_IValueReadonlyList_Contains, CreateRpcArguments(value));
			return UnboxValue<bool>(ReadMethodResult(serializer, result));
		}

		vint RpcCallerListOps::ListIndexOf(RpcObjectReference ref, const Value& value)
		{
			auto result = InvokeListMethod(objectOps, serializer, ref, RpcMethodId_IValueReadonlyList_IndexOf, CreateRpcArguments(value));
			return UnboxValue<vint>(ReadMethodResult(serializer, result));
		}

		vint RpcCallerListOps::DictGetCount(RpcObjectReference ref)
		{
			auto result = InvokeListMethod(objectOps, serializer, ref, RpcMethodId_IValueReadonlyDictionary_GetCount, CreateRpcArguments());
			return UnboxValue<vint>(ReadMethodResult(serializer, result));
		}

		Value RpcCallerListOps::DictGet(RpcObjectReference ref, const Value& key)
		{
			return InvokeListMethod(objectOps, serializer, ref, RpcMethodId_IValueReadonlyDictionary_Get, CreateRpcArguments(key));
		}

		void RpcCallerListOps::DictSet(RpcObjectReference ref, const Value& key, const Value& value)
		{
			InvokeListMethod(objectOps, serializer, ref, RpcMethodId_IValueDictionary_Set, CreateRpcArguments(key, value));
		}

		bool RpcCallerListOps::DictRemove(RpcObjectReference ref, const Value& key)
		{
			auto result = InvokeListMethod(objectOps, serializer, ref, RpcMethodId_IValueDictionary_Remove, CreateRpcArguments(key));
			return UnboxValue<bool>(ReadMethodResult(serializer, result));
		}

		void RpcCallerListOps::DictClear(RpcObjectReference ref)
		{
			InvokeListMethod(objectOps, serializer, ref, RpcMethodId_IValueDictionary_Clear, CreateRpcArguments());
		}

		bool RpcCallerListOps::DictContainsKey(RpcObjectReference ref, const Value& key)
		{
			auto result = InvokeListMethod(objectOps, serializer, ref, RpcMethodId_IValueReadonlyDictionary_ContainsKey, CreateRpcArguments(key));
			return UnboxValue<bool>(ReadMethodResult(serializer, result));
		}

		RpcObjectReference RpcCallerListOps::DictGetKeys(RpcObjectReference ref)
		{
			auto result = InvokeListMethod(objectOps, serializer, ref, RpcMethodId_IValueReadonlyDictionary_GetKeys, CreateRpcArguments());
			return UnboxValue<RpcObjectReference>(ReadMethodResult(serializer, result));
		}

		RpcObjectReference RpcCallerListOps::DictGetValues(RpcObjectReference ref)
		{
			auto result = InvokeListMethod(objectOps, serializer, ref, RpcMethodId_IValueReadonlyDictionary_GetValues, CreateRpcArguments());
			return UnboxValue<RpcObjectReference>(ReadMethodResult(serializer, result));
		}

/***********************************************************************
* RpcCallerListEventOps
***********************************************************************/

		RpcCallerListEventOps::RpcCallerListEventOps(IRpcObjectEventOps* _objectEventOps, IRpcSerializer* _serializer)
			: objectEventOps(_objectEventOps)
			, serializer(_serializer)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcCallerListEventOps::RpcCallerListEventOps(...)#"
			CHECK_ERROR(objectEventOps, ERROR_MESSAGE_PREFIX L"Object event ops are required.");
#undef ERROR_MESSAGE_PREFIX
		}

		Value RpcCallerListEventOps::OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)
		{
			auto arguments = CreateRpcArguments(
				BoxPrimitiveArgument(serializer, index),
				BoxPrimitiveArgument(serializer, oldCount),
				BoxPrimitiveArgument(serializer, newCount)
				);
			auto result = objectEventOps->InvokeEvent(ref, RpcEventId_IValueObservableList_ItemChanged, arguments);
			ReadEventException(UnboxRpcEventExceptionMap(DeserializeValue(serializer, result)));
			return result;
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

		void ReadMethodException(const Value& value)
		{
			if (value.GetValueType() == Value::BoxedValue)
			{
				if (auto boxed = value.GetBoxedValue().Cast<IValueType::TypedBox<RpcException>>())
				{
					throw Exception(boxed->value.message);
				}
			}
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
