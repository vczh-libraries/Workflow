#include "WfLibraryRpc.h"
#include "../WfLibraryReflection.h"

namespace vl
{
	namespace rpc_controller
	{
		using namespace collections;
		using namespace reflection;
		using namespace reflection::description;

		namespace
		{
#ifndef VCZH_WORKFLOW_RPC_OBJECT_REFERENCE_VALUE_HELPERS
#define VCZH_WORKFLOW_RPC_OBJECT_REFERENCE_VALUE_HELPERS
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
#endif

			bool IsNullRpcObjectReference(RpcObjectReference ref)
			{
				return ref.clientId == RpcClientId_Invalid
					&& ref.objectId == RpcObjectId_Invalid
					&& ref.typeId == RpcTypeId_NotFound;
			}

			template<typename K, typename V>
			bool ContainsKey(const Dictionary<K, V>& xs, const K& key)
			{
				return xs.Keys().Contains(key);
			}

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

			Value RpcUnboxValueByvalInternal(const Value& serializable, IRpcLifecycle* lc, Dictionary<const DescriptableObject*, bool>& visited)
			{
				if (serializable.IsNull()) return serializable;

				if (IsRpcObjectReferenceValue(serializable))
				{
					auto ref = GetRpcObjectReference(serializable);
					auto obj = RpcUnboxByref(ref, lc);
					return BoxValue(obj);
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

		void MergeRpcEventExceptionMap(RpcEventExceptionMap target, RpcEventExceptionMap source)
		{
			if (!source) return;
			auto keys = source->GetKeys();
			for (vint i = 0; i < keys->GetCount(); i++)
			{
				auto key = keys->Get(i);
				target->Set(key, source->Get(key));
			}
		}

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
			return RpcBoxByval(BoxValue(trivial), lc);
		}

		Value RpcBoxByval(const Value& trivial, IRpcLifecycle* lc)
		{
			if (!lc) CHECK_FAIL(L"IRpcLifecycle cannot be null.");
			if (trivial.IsNull()) return {};
			Dictionary<const DescriptableObject*, bool> visited;
			return RpcBoxValueByvalInternal(trivial, lc, visited);
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
