#include "RpcByvalLifecycleMock.h"

namespace vl
{
	namespace rpc_controller_test
	{
		using namespace reflection;
		using namespace reflection::description;
		using namespace rpc_controller;

		RpcByvalLifecycleMock::RpcByvalLifecycleMock()
			: dispatcher(Ptr(new RpcByrefListEventDispatcher))
		{
			proxyFactories.Set(RpcTypeId_IValueEnumerable, [this](IRpcLifeCycle* lc, RpcObjectReference ref) -> Ptr<IDescriptable>
			{
				return Ptr(new RpcByrefEnumerable(lc, ref));
			});
			proxyFactories.Set(RpcTypeId_IValueEnumerator, [this](IRpcLifeCycle* lc, RpcObjectReference ref) -> Ptr<IDescriptable>
			{
				return Ptr(new RpcByrefEnumerator(lc, ref));
			});
			proxyFactories.Set(RpcTypeId_IValueArray, [this](IRpcLifeCycle* lc, RpcObjectReference ref) -> Ptr<IDescriptable>
			{
				return Ptr(new RpcByrefArray(lc, ref));
			});
			proxyFactories.Set(RpcTypeId_IValueList, [this](IRpcLifeCycle* lc, RpcObjectReference ref) -> Ptr<IDescriptable>
			{
				return Ptr(new RpcByrefList(lc, ref));
			});
			proxyFactories.Set(RpcTypeId_IValueObservableList, [this](IRpcLifeCycle* lc, RpcObjectReference ref) -> Ptr<IDescriptable>
			{
				auto proxy = Ptr(new RpcByrefObservableList(lc, ref, dispatcher));
				dispatcher->Track(ref.objectId, proxy.Cast<IValueObservableList>());
				return proxy;
			});
			proxyFactories.Set(RpcTypeId_IValueDictionary, [this](IRpcLifeCycle* lc, RpcObjectReference ref) -> Ptr<IDescriptable>
			{
				return Ptr(new RpcByrefDictionary(lc, ref));
			});
		}

		vint RpcByvalLifecycleMock::DecideTypeId(Ptr<IDescriptable> obj)const
		{
			if (obj.Cast<IValueObservableList>()) return RpcTypeId_IValueObservableList;
			if (obj.Cast<IValueDictionary>()) return RpcTypeId_IValueDictionary;
			if (obj.Cast<IValueArray>()) return RpcTypeId_IValueArray;
			if (obj.Cast<IValueList>()) return RpcTypeId_IValueList;
			if (obj.Cast<IValueEnumerator>()) return RpcTypeId_IValueEnumerator;
			if (obj.Cast<IValueEnumerable>()) return RpcTypeId_IValueEnumerable;
			CHECK_FAIL(L"RpcByvalLifecycleMock::PtrToRef cannot determine the proxy type.");
			return 0;
		}

		void RpcByvalLifecycleMock::TrackLocalObject(RpcObjectReference ref, Ptr<IDescriptable> obj)
		{
			localObjects.Set(ref.objectId, obj);
			refsByPtr.Set(obj.Obj(), ref);
			refsById.Set(ref.objectId, ref);

			if (auto callee = dynamic_cast<RpcCalleeListOps*>(listCallback))
			{
				callee->RegisterLocalObject(ref, obj);
			}
			if (auto bridge = dynamic_cast<RpcCalleeListEventBridge*>(listEventCallback))
			{
				bridge->RegisterLocalObject(ref, obj);
			}
		}

		void RpcByvalLifecycleMock::UntrackLocalObject(RpcObjectReference ref)
		{
			if (localObjects.Keys().Contains(ref.objectId))
			{
				auto obj = localObjects.Get(ref.objectId);
				refsByPtr.Remove(obj.Obj());
				localObjects.Remove(ref.objectId);
			}
			refsById.Remove(ref.objectId);

			if (auto callee = dynamic_cast<RpcCalleeListOps*>(listCallback))
			{
				callee->UnregisterLocalObject(ref);
			}
			if (auto bridge = dynamic_cast<RpcCalleeListEventBridge*>(listEventCallback))
			{
				bridge->UnregisterLocalObject(ref);
			}
		}

		IRpcListOps* RpcByvalLifecycleMock::RequireListCallback()const
		{
			CHECK_ERROR(listCallback != nullptr, L"RpcByvalLifecycleMock requires a list callback.");
			return listCallback;
		}

		RpcObjectReference RpcByvalLifecycleMock::GetLastRegisteredObject()const
		{
			return lastRegisteredObject;
		}

		Ptr<RpcByrefListEventDispatcher> RpcByvalLifecycleMock::GetDispatcher()const
		{
			return dispatcher;
		}

		void RpcByvalLifecycleMock::SyncIds(Ptr<IValueDictionary> ids)
		{
			(void)ids;
		}

		Ptr<IValueDictionary> RpcByvalLifecycleMock::Register(IRpcObjectOps* _objectCallback, IRpcObjectEventOps* _eventCallback, IRpcListOps* _listCallback, IRpcListEventOps* _listEventCallback)
		{
			objectCallback = _objectCallback;
			eventCallback = _eventCallback;
			listCallback = _listCallback;
			listEventCallback = _listEventCallback;

			auto ids = IValueDictionary::Create();
			if (objectCallback) objectCallback->SyncIds(ids);
			if (eventCallback) eventCallback->SyncIds(ids);
			return ids;
		}

		RpcObjectReference RpcByvalLifecycleMock::RegisterLocalObject(vint typeId)
		{
			lastRegisteredObject = { clientId, nextObjectId++, typeId };
			typeIds.Set(lastRegisteredObject.objectId, typeId);
			refCounts.Set(lastRegisteredObject.objectId, 1);
			return lastRegisteredObject;
		}

		void RpcByvalLifecycleMock::UnregisterLocalObject(RpcObjectReference ref)
		{
			UntrackLocalObject(ref);
			refCounts.Remove(ref.objectId);
			typeIds.Remove(ref.objectId);
		}

		void RpcByvalLifecycleMock::AcquireRemoteObject(RpcObjectReference ref)
		{
			(void)ref;
			// The test controller creates acquired references directly from RegisterLocalObject / PtrToRef.
		}

		void RpcByvalLifecycleMock::ReleaseRemoteObject(RpcObjectReference ref)
		{
			if (!refCounts.Keys().Contains(ref.objectId))
			{
				return;
			}

			auto count = refCounts.Get(ref.objectId) - 1;
			if (count <= 0)
			{
				UnregisterLocalObject(ref);
			}
			else
			{
				refCounts.Set(ref.objectId, count);
			}
		}

		Value RpcByvalLifecycleMock::InvokeMethod(RpcObjectReference ref, vint methodId, Ptr<IValueArray> arguments)
		{
			(void)ref;
			(void)methodId;
			(void)arguments;
			CHECK_FAIL(L"Not Supported!");
			return {};
		}

		Ptr<IAsync> RpcByvalLifecycleMock::InvokeMethodAsync(RpcObjectReference ref, vint methodId, Ptr<IValueArray> arguments)
		{
			(void)ref;
			(void)methodId;
			(void)arguments;
			CHECK_FAIL(L"Not Supported!");
			return nullptr;
		}

		void RpcByvalLifecycleMock::ObjectHold(RpcObjectReference ref, bool hold)
		{
			(void)ref;
			(void)hold;
			CHECK_FAIL(L"Not Supported!");
		}

		RpcObjectReference RpcByvalLifecycleMock::RequestService(vint typeId)
		{
			(void)typeId;
			CHECK_FAIL(L"Not Supported!");
			return {};
		}

		void RpcByvalLifecycleMock::OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)
		{
			CHECK_ERROR(listEventCallback != nullptr, L"RpcByvalLifecycleMock requires a list event callback.");
			listEventCallback->OnItemChanged(ref, index, oldCount, newCount);
		}

		void RpcByvalLifecycleMock::InvokeEvent(RpcObjectReference ref, vint eventId, Ptr<IValueArray> arguments)
		{
			(void)ref;
			(void)eventId;
			(void)arguments;
			CHECK_FAIL(L"Not Supported!");
		}

		IRpcController* RpcByvalLifecycleMock::GetController()const
		{
			return const_cast<RpcByvalLifecycleMock*>(this);
		}

		Ptr<IDescriptable> RpcByvalLifecycleMock::RefToPtr(RpcObjectReference ref)
		{
			if (localObjects.Keys().Contains(ref.objectId))
			{
				return localObjects.Get(ref.objectId);
			}
			CHECK_ERROR(proxyFactories.Keys().Contains(ref.typeId), L"RpcByvalLifecycleMock::RefToPtr cannot find a proxy factory.");
			return proxyFactories.Get(ref.typeId)(const_cast<RpcByvalLifecycleMock*>(this), ref);
		}

		RpcObjectReference RpcByvalLifecycleMock::PtrToRef(Ptr<IDescriptable> obj)
		{
			CHECK_ERROR(obj, L"RpcByvalLifecycleMock::PtrToRef requires a value.");
			if (refsByPtr.Keys().Contains(obj.Obj()))
			{
				auto ref = refsByPtr.Get(obj.Obj());
				refCounts.Set(ref.objectId, refCounts.Get(ref.objectId) + 1);
				return ref;
			}

			auto ref = RegisterLocalObject(DecideTypeId(obj));
			TrackLocalObject(ref, obj);
			return ref;
		}

		RpcObjectReference RpcByvalLifecycleMock::EnumCreate(RpcObjectReference ref)
		{
			return RequireListCallback()->EnumCreate(ref);
		}

		bool RpcByvalLifecycleMock::EnumNext(RpcObjectReference enumerator)
		{
			return RequireListCallback()->EnumNext(enumerator);
		}

		Value RpcByvalLifecycleMock::EnumGetCurrent(RpcObjectReference enumerator)
		{
			return RequireListCallback()->EnumGetCurrent(enumerator);
		}

		vint RpcByvalLifecycleMock::ListGetCount(RpcObjectReference ref)
		{
			return RequireListCallback()->ListGetCount(ref);
		}

		Value RpcByvalLifecycleMock::ListGet(RpcObjectReference ref, vint index)
		{
			return RequireListCallback()->ListGet(ref, index);
		}

		void RpcByvalLifecycleMock::ListSet(RpcObjectReference ref, vint index, const Value& value)
		{
			RequireListCallback()->ListSet(ref, index, value);
		}

		vint RpcByvalLifecycleMock::ListAdd(RpcObjectReference ref, const Value& value)
		{
			return RequireListCallback()->ListAdd(ref, value);
		}

		vint RpcByvalLifecycleMock::ListInsert(RpcObjectReference ref, vint index, const Value& value)
		{
			return RequireListCallback()->ListInsert(ref, index, value);
		}

		bool RpcByvalLifecycleMock::ListRemoveAt(RpcObjectReference ref, vint index)
		{
			return RequireListCallback()->ListRemoveAt(ref, index);
		}

		void RpcByvalLifecycleMock::ListClear(RpcObjectReference ref)
		{
			RequireListCallback()->ListClear(ref);
		}

		bool RpcByvalLifecycleMock::ListContains(RpcObjectReference ref, const Value& value)
		{
			return RequireListCallback()->ListContains(ref, value);
		}

		vint RpcByvalLifecycleMock::ListIndexOf(RpcObjectReference ref, const Value& value)
		{
			return RequireListCallback()->ListIndexOf(ref, value);
		}

		vint RpcByvalLifecycleMock::DictGetCount(RpcObjectReference ref)
		{
			return RequireListCallback()->DictGetCount(ref);
		}

		Value RpcByvalLifecycleMock::DictGet(RpcObjectReference ref, const Value& key)
		{
			return RequireListCallback()->DictGet(ref, key);
		}

		void RpcByvalLifecycleMock::DictSet(RpcObjectReference ref, const Value& key, const Value& value)
		{
			RequireListCallback()->DictSet(ref, key, value);
		}

		bool RpcByvalLifecycleMock::DictRemove(RpcObjectReference ref, const Value& key)
		{
			return RequireListCallback()->DictRemove(ref, key);
		}

		void RpcByvalLifecycleMock::DictClear(RpcObjectReference ref)
		{
			RequireListCallback()->DictClear(ref);
		}

		bool RpcByvalLifecycleMock::DictContainsKey(RpcObjectReference ref, const Value& key)
		{
			return RequireListCallback()->DictContainsKey(ref, key);
		}

		Ptr<IValueArray> RpcByvalLifecycleMock::DictGetKeys(RpcObjectReference ref)
		{
			return RequireListCallback()->DictGetKeys(ref);
		}

		Ptr<IValueArray> RpcByvalLifecycleMock::DictGetValues(RpcObjectReference ref)
		{
			return RequireListCallback()->DictGetValues(ref);
		}
	}
}
