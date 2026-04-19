#include "RpcDualLifecycleMock.h"

namespace vl
{
	namespace rpc_controller_test
	{
		using namespace reflection;
		using namespace reflection::description;
		using namespace rpc_controller;
		using namespace collections;

		RpcDualObjectTracker::RpcDualObjectTracker(RpcDualLifecycleMock* m, RpcObjectReference r)
			: mock(m), ref(r)
		{
		}

		RpcDualObjectTracker::~RpcDualObjectTracker()
		{
			if (mock->IsTracked(ref.objectId))
			{
				mock->UntrackLocalObject(ref);
			}
		}

/***********************************************************************
* RpcDualLifeCycleAdapter
***********************************************************************/

		RpcDualLifeCycleAdapter::RpcDualLifeCycleAdapter(RpcDualLifecycleMock* _mock)
			: mock(_mock)
		{
		}

		IRpcController* RpcDualLifeCycleAdapter::GetController()
		{
			return mock->GetController();
		}

		Ptr<IDescriptable> RpcDualLifeCycleAdapter::RefToPtr(RpcObjectReference ref)
		{
			return mock->RefToPtr(ref);
		}

		RpcObjectReference RpcDualLifeCycleAdapter::PtrToRef(Ptr<IDescriptable> obj)
		{
			return mock->PtrToRef(obj);
		}

		void RpcDualLifeCycleAdapter::RegisterService(const WString& fullName, Ptr<IDescriptable> service)
		{
			mock->RegisterService(fullName, service);
		}

		Ptr<IDescriptable> RpcDualLifeCycleAdapter::RequestService(const WString& fullName)
		{
			return mock->RequestService(fullName);
		}

/***********************************************************************
* RpcDualLifecycleMock (Constructor and Setup)
***********************************************************************/

		RpcDualLifecycleMock::RpcDualLifecycleMock(vint _clientId)
			: clientId(_clientId)
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

		RpcDualLifecycleMock::~RpcDualLifecycleMock()
		{
			for (auto service : services.Values())
			{
				if (auto obj = service->SafeAggregationCast<IRpcWrapperBase>())
				{
					obj->DisconnectFromLifecycle();
				}
			}
			while (localObjects.Count() > 0)
			{
				auto ref = refsById.Values()[refsById.Count() - 1];
				UntrackLocalObject(ref);
			}
			ownedObjects.Clear();
			services.Clear();
		}

		void RpcDualLifecycleMock::SetPeer(RpcDualLifecycleMock* _peer)
		{
			peer = _peer;
		}

		void RpcDualLifecycleMock::SetIdMap(const Dictionary<WString, vint>& _idMap)
		{
			for (auto&& [key, value] : _idMap)
			{
				idMap.Set(key, value);
			}
		}

		void RpcDualLifecycleMock::SetAdapter(RpcDualLifeCycleAdapter* _adapter)
		{
			adapter = _adapter;
		}

		void RpcDualLifecycleMock::RegisterWrapperFactory(Func<Ptr<IDescriptable>(vint, IRpcLifeCycle*)> factory)
		{
			universalWrapperFactory = factory;
		}

		vint RpcDualLifecycleMock::DecideTypeId(IDescriptable* obj)const
		{
			if (dynamic_cast<IValueObservableList*>(obj)) return RpcTypeId_IValueObservableList;
			if (dynamic_cast<IValueDictionary*>(obj)) return RpcTypeId_IValueDictionary;
			if (dynamic_cast<IValueArray*>(obj)) return RpcTypeId_IValueArray;
			if (dynamic_cast<IValueList*>(obj)) return RpcTypeId_IValueList;
			if (dynamic_cast<IValueReadonlyList*>(obj)) return RpcTypeId_IValueReadonlyList;
			if (dynamic_cast<IValueEnumerator*>(obj)) return RpcTypeId_IValueEnumerator;
			if (dynamic_cast<IValueEnumerable*>(obj)) return RpcTypeId_IValueEnumerable;

			// Check registered services
			for (vint i = 0; i < services.Count(); i++)
			{
				if (services.Values()[i].Obj() == obj)
				{
					auto& name = services.Keys()[i];
					if (auto index = idMap.Keys().IndexOf(name); index != -1)
					{
						return idMap.Values()[index];
					}
				}
			}

			return RpcTypeId_NotFound;
		}

		void RpcDualLifecycleMock::TrackLocalObject(RpcObjectReference ref, IDescriptable* obj)
		{
			localObjects.Set(ref.objectId, obj);
			refsByPtr.Set(obj, ref);
			refsById.Set(ref.objectId, ref);

			if (auto observable = dynamic_cast<IValueObservableList*>(obj))
			{
				auto handler = observable->ItemChanged.Add([this, ref](vint index, vint oldCount, vint newCount)
				{
					this->OnItemChanged(ref, index, oldCount, newCount);
				});
				eventHandlers.Set(ref.objectId, handler);
			}

			if (auto descriptable = dynamic_cast<DescriptableObject*>(obj))
			{
				auto tracker = Ptr(new RpcDualObjectTracker(this, ref));
				descriptable->SetInternalProperty(L"RpcLifecycleTracker", tracker);
			}
		}

		void RpcDualLifecycleMock::UntrackLocalObject(RpcObjectReference ref)
		{
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
			ownedObjects.Remove(ref.objectId);
			observableProxies.Remove(ref.objectId);
		}

		bool RpcDualLifecycleMock::IsTracked(vint objectId)const
		{
			return localObjects.Keys().Contains(objectId);
		}

		Ptr<IDescriptable> RpcDualLifecycleMock::CreateCallerProxy(RpcObjectReference ref)
		{
			// For built-in collection types, use the standard proxy factories
			if (proxyFactories.Keys().Contains(ref.typeId))
			{
				return proxyFactories.Get(ref.typeId)(adapter ? (IRpcLifeCycle*)adapter : (IRpcLifeCycle*)this, ref);
			}

			// Fallback to universal wrapper factory
			if (universalWrapperFactory)
			{
				CHECK_ERROR(adapter != nullptr, L"RpcDualLifecycleMock::CreateCallerProxy requires an adapter.");
				return universalWrapperFactory(ref.typeId, adapter);
			}

			CHECK_FAIL(L"RpcDualLifecycleMock::CreateCallerProxy cannot find a proxy factory.");
			return nullptr;
		}

/***********************************************************************
* RpcDualLifecycleMock (IRpcController)
***********************************************************************/

		void RpcDualLifecycleMock::Register(Ptr<IRpcObjectOps> _objectCallback, Ptr<IRpcObjectEventOps> _eventCallback, Ptr<IRpcListOps> _listCallback, Ptr<IRpcListEventOps> _listEventCallback)
		{
			objectCallback = _objectCallback.Obj();
			eventCallback = _eventCallback.Obj();
			listCallback = _listCallback.Obj();
			listEventCallback = _listEventCallback.Obj();

			// Do not call SyncIds on Workflow proxy objects.
			// The Workflow proxy's SyncIds cannot convert Dictionary<WString,vint> through reflection.
			// IDs are already synced via global variables set during <initialize>.
		}

		RpcObjectReference RpcDualLifecycleMock::RegisterLocalObject(vint typeId)
		{
			auto ref = RpcObjectReference{ clientId, nextObjectId++, typeId };
			typeIds.Set(ref.objectId, typeId);
			refCounts.Set(ref.objectId, 1);
			return ref;
		}

		void RpcDualLifecycleMock::UnregisterLocalObject(RpcObjectReference ref)
		{
			UntrackLocalObject(ref);
			refCounts.Remove(ref.objectId);
			typeIds.Remove(ref.objectId);
		}

		void RpcDualLifecycleMock::AcquireRemoteObject(RpcObjectReference ref)
		{
			if (ref.clientId == clientId)
			{
				if (refCounts.Keys().Contains(ref.objectId))
				{
					refCounts.Set(ref.objectId, refCounts.Get(ref.objectId) + 1);
				}
			}
			else
			{
				// Route to peer
				peer->AcquireRemoteObject(ref);
			}
		}

		void RpcDualLifecycleMock::ReleaseRemoteObject(RpcObjectReference ref)
		{
			if (ref.clientId == clientId)
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
			else
			{
				// Route to peer
				peer->ReleaseRemoteObject(ref);
			}
		}

/***********************************************************************
* RpcDualLifecycleMock (IRpcLifeCycle)
***********************************************************************/

		Ptr<IDescriptable> RpcDualLifecycleMock::RequestService(const WString& fullName)
		{
			// Check locally first
			if (auto index = services.Keys().IndexOf(fullName); index != -1)
			{
				return services.Values()[index];
			}

			// Use registered wrapper factories
			if (idMap.Keys().Contains(fullName))
			{
				auto typeId = idMap.Get(fullName);
				CHECK_ERROR(adapter != nullptr, L"RpcDualLifecycleMock::RequestService requires an adapter.");
				if (universalWrapperFactory)
				{
					return universalWrapperFactory(typeId, adapter);
				}
			}

			return nullptr;
		}

		Ptr<IDescriptable> RpcDualLifecycleMock::RefToPtr(RpcObjectReference ref)
		{
			if (ref.clientId == clientId)
			{
				// Local object
				if (localObjects.Keys().Contains(ref.objectId))
				{
					return Ptr(localObjects.Get(ref.objectId));
				}
				// Local but not tracked yet - create proxy for built-in collection types
				if (proxyFactories.Keys().Contains(ref.typeId))
				{
					return proxyFactories.Get(ref.typeId)(this, ref);
				}
				CHECK_FAIL(L"RpcDualLifecycleMock::RefToPtr cannot find local object.");
				return nullptr;
			}
			else
			{
				// Remote object - create a wrapper proxy
				return CreateCallerProxy(ref);
			}
		}

		RpcObjectReference RpcDualLifecycleMock::PtrToRef(Ptr<IDescriptable> obj)
		{
			CHECK_ERROR(obj, L"RpcDualLifecycleMock::PtrToRef requires a value.");
			if (refsByPtr.Keys().Contains(obj.Obj()))
			{
				auto ref = refsByPtr.Get(obj.Obj());
				refCounts.Set(ref.objectId, refCounts.Get(ref.objectId) + 1);
				return ref;
			}

			auto typeId = DecideTypeId(obj.Obj());
			CHECK_ERROR(typeId != RpcTypeId_NotFound, L"RpcDualLifecycleMock::PtrToRef: DecideTypeId returned RpcTypeId_NotFound (unknown type).");
			auto ref = RegisterLocalObject(typeId);
			ownedObjects.Set(ref.objectId, obj);
			TrackLocalObject(ref, obj.Obj());
			return ref;
		}

/***********************************************************************
* RpcDualLifecycleMock (IRpcListEventOps)
***********************************************************************/

		void RpcDualLifecycleMock::OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)
		{
			if (observableProxies.Keys().Contains(ref.objectId))
			{
				observableProxies.Get(ref.objectId)->ItemChanged(index, oldCount, newCount);
			}
		}
	}
}
