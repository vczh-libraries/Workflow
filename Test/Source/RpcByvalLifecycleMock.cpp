#include "RpcByvalLifecycleMock.h"

namespace vl
{
	namespace rpc_controller_test
	{
		using namespace reflection;
		using namespace reflection::description;
		using namespace rpc_controller;

		RpcObjectTracker::RpcObjectTracker(RpcByvalLifecycleMock* m, RpcObjectReference r)
			: mock(m), ref(r)
		{
		}

		RpcObjectTracker::~RpcObjectTracker()
		{
			if (mock->IsTracked(ref.objectId))
			{
				mock->UntrackLocalObject(ref);
			}
		}

		RpcByvalLifecycleMock::RpcByvalLifecycleMock()
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
				auto proxy = Ptr(new RpcByrefObservableList(lc, ref));
				observableProxies.Set(ref.objectId, dynamic_cast<IValueObservableList*>(proxy.Obj()));
				return proxy;
			});
			proxyFactories.Set(RpcTypeId_IValueDictionary, [this](IRpcLifeCycle* lc, RpcObjectReference ref) -> Ptr<IDescriptable>
			{
				return Ptr(new RpcByrefDictionary(lc, ref));
			});
		}

		vint RpcByvalLifecycleMock::DecideTypeId(IDescriptable* obj)const
		{
			if (dynamic_cast<IValueObservableList*>(obj)) return RpcTypeId_IValueObservableList;
			if (dynamic_cast<IValueDictionary*>(obj)) return RpcTypeId_IValueDictionary;
			if (dynamic_cast<IValueArray*>(obj)) return RpcTypeId_IValueArray;
			if (dynamic_cast<IValueList*>(obj)) return RpcTypeId_IValueList;
			if (dynamic_cast<IValueEnumerator*>(obj)) return RpcTypeId_IValueEnumerator;
			if (dynamic_cast<IValueEnumerable*>(obj)) return RpcTypeId_IValueEnumerable;
			CHECK_FAIL(L"RpcByvalLifecycleMock::DecideTypeId cannot determine the proxy type.");
			return 0;
		}

		void RpcByvalLifecycleMock::TrackLocalObject(RpcObjectReference ref, IDescriptable* obj)
		{
			localObjects.Set(ref.objectId, obj);
			refsByPtr.Set(obj, ref);
			refsById.Set(ref.objectId, ref);

			// Subscribe to ItemChanged for observable lists
			if (auto observable = dynamic_cast<IValueObservableList*>(obj))
			{
				auto handler = observable->ItemChanged.Add([this, ref](vint index, vint oldCount, vint newCount)
				{
					this->OnItemChanged(ref, index, oldCount, newCount);
				});
				eventHandlers.Set(ref.objectId, handler);
			}

			// Use DescriptableObject internal property for release tracking
			if (auto descriptable = dynamic_cast<DescriptableObject*>(obj))
			{
				auto tracker = Ptr(new RpcObjectTracker(this, ref));
				descriptable->SetInternalProperty(L"RpcLifecycleTracker", tracker);
			}
		}

		void RpcByvalLifecycleMock::UntrackLocalObject(RpcObjectReference ref)
		{
			// Unsubscribe from observable list events
			if (eventHandlers.Keys().Contains(ref.objectId))
			{
				if (localObjects.Keys().Contains(ref.objectId))
				{
					if (auto observable = dynamic_cast<IValueObservableList*>(localObjects.Get(ref.objectId)))
					{
						observable->ItemChanged.Remove(eventHandlers.Get(ref.objectId));
					}
				}
				eventHandlers.Remove(ref.objectId);
			}

			if (localObjects.Keys().Contains(ref.objectId))
			{
				auto obj = localObjects.Get(ref.objectId);
				refsByPtr.Remove(obj);
				localObjects.Remove(ref.objectId);
			}
			refsById.Remove(ref.objectId);

			// Clean up ownership
			ownedObjects.Remove(ref.objectId);

			// Clean up observable proxy tracking
			observableProxies.Remove(ref.objectId);
		}

		IRpcListOps* RpcByvalLifecycleMock::RequireListCallback()const
		{
			CHECK_ERROR(listCallback != nullptr, L"RpcByvalLifecycleMock requires a list callback.");
			return listCallback;
		}

		bool RpcByvalLifecycleMock::IsTracked(vint objectId)const
		{
			return localObjects.Keys().Contains(objectId);
		}

		RpcObjectReference RpcByvalLifecycleMock::GetLastRegisteredObject()const
		{
			return lastRegisteredObject;
		}

		Ptr<IDescriptable> RpcByvalLifecycleMock::CreateCallerProxy(RpcObjectReference ref)
		{
			CHECK_ERROR(proxyFactories.Keys().Contains(ref.typeId), L"RpcByvalLifecycleMock::CreateCallerProxy cannot find a proxy factory.");
			return proxyFactories.Get(ref.typeId)(const_cast<RpcByvalLifecycleMock*>(this), ref);
		}

/***********************************************************************
* RpcByvalLifecycleMock (IRpcIdSync)
***********************************************************************/

		void RpcByvalLifecycleMock::SyncIds(Ptr<IValueDictionary> ids)
		{
			(void)ids;
		}

/***********************************************************************
* RpcByvalLifecycleMock (IRpcController)
***********************************************************************/

		Ptr<IValueDictionary> RpcByvalLifecycleMock::Register(Ptr<IRpcObjectOps> _objectCallback, Ptr<IRpcObjectEventOps> _eventCallback, Ptr<IRpcListOps> _listCallback, Ptr<IRpcListEventOps> _listEventCallback)
		{
			objectCallback = _objectCallback.Obj();
			eventCallback = _eventCallback.Obj();
			listCallback = _listCallback.Obj();
			listEventCallback = _listEventCallback.Obj();

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

/***********************************************************************
* RpcByvalLifecycleMock (IRpcLifeCycle)
***********************************************************************/

		Ptr<IRpcController> RpcByvalLifecycleMock::GetController()const
		{
			return Ptr(const_cast<RpcByvalLifecycleMock*>(this));
		}

		Ptr<IDescriptable> RpcByvalLifecycleMock::RefToPtr(RpcObjectReference ref)
		{
			// Check local objects first (callee-side use)
			if (localObjects.Keys().Contains(ref.objectId))
			{
				return Ptr(localObjects.Get(ref.objectId));
			}
			// Otherwise create a proxy (caller-side use)
			if (proxyFactories.Keys().Contains(ref.typeId))
			{
				return proxyFactories.Get(ref.typeId)(const_cast<RpcByvalLifecycleMock*>(this), ref);
			}
			CHECK_FAIL(L"RpcByvalLifecycleMock::RefToPtr cannot find a proxy factory.");
			return nullptr;
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

			auto ref = RegisterLocalObject(DecideTypeId(obj.Obj()));
			// Store ownership for objects not externally owned (e.g. enumerators created by callee)
			ownedObjects.Set(ref.objectId, obj);
			TrackLocalObject(ref, obj.Obj());
			return ref;
		}

/***********************************************************************
* RpcByvalLifecycleMock (IRpcObjectOps)
***********************************************************************/

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

/***********************************************************************
* RpcByvalLifecycleMock (IRpcObjectEventOps)
***********************************************************************/

		void RpcByvalLifecycleMock::InvokeEvent(RpcObjectReference ref, vint eventId, Ptr<IValueArray> arguments)
		{
			(void)ref;
			(void)eventId;
			(void)arguments;
			CHECK_FAIL(L"Not Supported!");
		}

/***********************************************************************
* RpcByvalLifecycleMock (IRpcListOps)
***********************************************************************/

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

/***********************************************************************
* RpcByvalLifecycleMock (IRpcListEventOps)
***********************************************************************/

		void RpcByvalLifecycleMock::OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)
		{
			if (observableProxies.Keys().Contains(ref.objectId))
			{
				observableProxies.Get(ref.objectId)->ItemChanged(index, oldCount, newCount);
			}
		}
	}
}
