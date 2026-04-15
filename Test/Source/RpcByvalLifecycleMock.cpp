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
			proxyFactories.Set(RpcTypeId_IValueReadonlyList, [this](IRpcLifeCycle* lc, RpcObjectReference ref) -> Ptr<IDescriptable>
			{
				return Ptr(new RpcByrefReadonlyList(lc, ref));
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
			if (dynamic_cast<IValueReadonlyList*>(obj)) return RpcTypeId_IValueReadonlyList;
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
* RpcByvalLifecycleMock (IRpcController)
***********************************************************************/

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
