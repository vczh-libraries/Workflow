#include "RpcDualLifecycleMock.h"

namespace vl
{
	namespace rpc_controller_test
	{
		using namespace reflection;
		using namespace reflection::description;
		using namespace rpc_controller;
		using namespace collections;

		WString RpcDualLifecycleMock::InternalProperty_LocalObjectTracker = WString::Unmanaged(L"RpcLocalObjectTracker");
		WString RpcDualLifecycleMock::InternalProperty_WrapperTracker = WString::Unmanaged(L"RpcWrapperTracker");

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

		RpcDualWrapperTracker::RpcDualWrapperTracker(RpcDualLifecycleMock* m, IRpcWrapperBase* p, RpcObjectReference r)
			: mock(m), proxy(p), ref(r)
		{
		}

		RpcDualWrapperTracker::~RpcDualWrapperTracker()
		{
			if (mock && proxy)
			{
				mock->UntrackWrapper(proxy);
			}
		}

		void RpcDualWrapperTracker::Detach()
		{
			mock = nullptr;
			proxy = nullptr;
		}

/***********************************************************************
* RpcDualLifeCycleAdapter
***********************************************************************/

		RpcDualLifeCycleAdapter::RpcDualLifeCycleAdapter(RpcDualLifecycleMock* _mock)
			: mock(_mock)
		{
		}

		RpcDualLifeCycleAdapter::~RpcDualLifeCycleAdapter()
		{
			if (mock)
			{
				mock->DisconnectTrackedWrappers();
			}
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
			DisconnectTrackedWrappers();
			while (localObjectProps.Count() > 0)
			{
				auto props = localObjectProps.Values().Get(localObjectProps.Count() - 1);
				UnregisterLocalObject(props->ref);
			}
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

		void RpcDualLifecycleMock::RegisterWrapperFactory(Func<Ptr<IRpcWrapperBase>(vint, IRpcLifeCycle*)> factory)
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
			auto index = localObjectProps.Keys().IndexOf(ref.objectId);
			CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::TrackLocalObject: Object not registered.");
			auto props = localObjectProps.Values().Get(index);
			CHECK_ERROR(props->rawPtr == nullptr, L"RpcDualLifecycleMock::TrackLocalObject: Object already tracked.");

			props->rawPtr = obj;

			if (auto observable = dynamic_cast<IValueObservableList*>(obj))
			{
				auto handler = observable->ItemChanged.Add([this, ref](vint index, vint oldCount, vint newCount)
				{
					this->OnItemChanged(ref, index, oldCount, newCount);
				});
				props->eventHandler = handler;
			}

			if (auto descriptable = dynamic_cast<DescriptableObject*>(obj))
			{
				auto tracker = Ptr(new RpcDualObjectTracker(this, ref));
				descriptable->SetInternalProperty(InternalProperty_LocalObjectTracker, tracker);
			}
		}

		void RpcDualLifecycleMock::UntrackLocalObject(RpcObjectReference ref)
		{
			auto index = localObjectProps.Keys().IndexOf(ref.objectId);
			CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::UntrackLocalObject: Object not registered.");
			auto props = localObjectProps.Values().Get(index);

			if (props->eventHandler)
			{
				if (auto observable = dynamic_cast<IValueObservableList*>(props->rawPtr))
				{
					observable->ItemChanged.Remove(props->eventHandler);
				}
				props->eventHandler = nullptr;
			}

			// Clear rawPtr first so IsTracked returns false, preventing re-entrant cleanup
			auto rawPtr = props->rawPtr;
			props->rawPtr = nullptr;

			// Clear internal property (tracker destructor checks IsTracked → false → no re-entry)
			if (rawPtr)
			{
				if (auto descriptable = dynamic_cast<DescriptableObject*>(rawPtr))
				{
					descriptable->SetInternalProperty(InternalProperty_LocalObjectTracker, nullptr);
				}
			}

			// Clear owned ptr last (may destroy the object, but internal property is already cleared)
			props->ownedPtr = nullptr;
		}

		bool RpcDualLifecycleMock::IsTracked(vint objectId)const
		{
			auto index = localObjectProps.Keys().IndexOf(objectId);
			return index != -1 && localObjectProps.Values().Get(index)->rawPtr != nullptr;
		}

		void RpcDualLifecycleMock::TrackWrapper(DescriptableObject* root, IRpcWrapperBase* proxy, RpcObjectReference ref)
		{
			CHECK_ERROR(root != nullptr, L"RpcDualLifecycleMock::TrackWrapper: Wrapper does not implement DescriptableObject.");
			CHECK_ERROR(proxy != nullptr, L"RpcDualLifecycleMock::TrackWrapper: Wrapper is null.");

			auto tracker = Ptr(new RpcDualWrapperTracker(this, proxy, ref));
			root->SetInternalProperty(InternalProperty_WrapperTracker, tracker);

			RpcWrapperEntry entry;
			entry.root = root;
			entry.proxy = proxy;
			entry.ref = ref;
			wrapperEntries.Add(entry);
		}

		void RpcDualLifecycleMock::UntrackWrapper(IRpcWrapperBase* proxy)
		{
			for (vint i = 0; i < wrapperEntries.Count(); i++)
			{
				if (wrapperEntries[i].proxy == proxy)
				{
					wrapperEntries.RemoveAt(i);
					return;
				}
			}
		}

		void RpcDualLifecycleMock::DisconnectTrackedWrappers()
		{
			while (wrapperEntries.Count() > 0)
			{
				auto entry = wrapperEntries[wrapperEntries.Count() - 1];
				wrapperEntries.RemoveAt(wrapperEntries.Count() - 1);

				if (entry.root)
				{
					if (auto trackerObj = entry.root->GetInternalProperty(InternalProperty_WrapperTracker))
					{
						auto tracker = trackerObj.Cast<RpcDualWrapperTracker>();
						if (tracker)
						{
							tracker->Detach();
						}
						entry.root->SetInternalProperty(InternalProperty_WrapperTracker, nullptr);
					}
				}

				if (entry.proxy)
				{
					entry.proxy->DisconnectFromLifecycle();
				}
			}
		}

		bool RpcDualLifecycleMock::TryGetTrackedWrapperRef(DescriptableObject* obj, RpcObjectReference& ref)const
		{
			if (auto trackerObj = obj->GetInternalProperty(InternalProperty_WrapperTracker))
			{
				auto tracker = trackerObj.Cast<RpcDualWrapperTracker>();
				CHECK_ERROR(tracker, L"RpcDualLifecycleMock::TryGetTrackedWrapperRef: Invalid internal property type.");
				CHECK_ERROR(tracker->GetMock() == this, L"RpcDualLifecycleMock::TryGetTrackedWrapperRef: Wrapper registered to a different lifecycle.");
				ref = tracker->GetRef();
				return true;
			}
			return false;
		}

		Ptr<IDescriptable> RpcDualLifecycleMock::CreateCallerProxy(RpcObjectReference ref)
		{
			auto lc = adapter ? (IRpcLifeCycle*)adapter : (IRpcLifeCycle*)this;
			Ptr<IRpcWrapperBase> wrapper;

			// For built-in collection types, use switch-case
			switch (ref.typeId)
			{
			case RpcTypeId_IValueEnumerable:
				wrapper = Ptr(new RpcByrefEnumerable(lc, ref));
				break;
			case RpcTypeId_IValueEnumerator:
				wrapper = Ptr(new RpcByrefEnumerator(lc, ref));
				break;
			case RpcTypeId_IValueArray:
				wrapper = Ptr(new RpcByrefArray(lc, ref));
				break;
			case RpcTypeId_IValueReadonlyList:
				wrapper = Ptr(new RpcByrefReadonlyList(lc, ref));
				break;
			case RpcTypeId_IValueList:
				wrapper = Ptr(new RpcByrefList(lc, ref));
				break;
			case RpcTypeId_IValueObservableList:
				wrapper = Ptr(new RpcByrefObservableList(lc, ref));
				break;
			case RpcTypeId_IValueDictionary:
				wrapper = Ptr(new RpcByrefDictionary(lc, ref));
				break;
			default:
				// Fallback to universal wrapper factory
				CHECK_ERROR(universalWrapperFactory, L"RpcDualLifecycleMock::CreateCallerProxy: No wrapper factory registered.");
				CHECK_ERROR(adapter != nullptr, L"RpcDualLifecycleMock::CreateCallerProxy requires an adapter.");
				pendingProxyRef = ref;
				wrapper = universalWrapperFactory(ref.typeId, adapter);
				pendingProxyRef = {};
				CHECK_ERROR(wrapper, L"RpcDualLifecycleMock::CreateCallerProxy: wrapper factory returned null.");
				break;
			}

			auto descriptable = dynamic_cast<IDescriptable*>(wrapper.Obj());
			CHECK_ERROR(descriptable, L"RpcDualLifecycleMock::CreateCallerProxy: wrapper does not implement IDescriptable.");
			auto root = dynamic_cast<DescriptableObject*>(descriptable);
			TrackWrapper(root, wrapper.Obj(), ref);
			return Ptr(descriptable);
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
			CHECK_ERROR(!localObjectProps.Keys().Contains(ref.objectId), L"RpcDualLifecycleMock::RegisterLocalObject: Object ID already registered.");
			auto props = Ptr(new RpcLocalObjectProperties());
			props->ref = ref;
			props->refCount = 1;
			localObjectProps.Set(ref.objectId, props);
			return ref;
		}

		void RpcDualLifecycleMock::UnregisterLocalObject(RpcObjectReference ref)
		{
			auto index = localObjectProps.Keys().IndexOf(ref.objectId);
			CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::UnregisterLocalObject: Object not registered.");
			auto props = localObjectProps.Values().Get(index);

			for (vint i = services.Count() - 1; i >= 0; i--)
			{
				if (services.Values()[i].Obj() == props->rawPtr)
				{
					services.Remove(services.Keys()[i]);
				}
			}

			UntrackLocalObject(ref);
			localObjectProps.Remove(ref.objectId);
		}

		void RpcDualLifecycleMock::AcquireRemoteObject(RpcObjectReference ref)
		{
			if (ref.clientId == clientId)
			{
				auto index = localObjectProps.Keys().IndexOf(ref.objectId);
				CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::AcquireRemoteObject: Object not registered.");
				localObjectProps.Values().Get(index)->refCount++;
			}
			else
			{
				CHECK_ERROR(peer != nullptr, L"RpcDualLifecycleMock::AcquireRemoteObject: No peer configured.");
				peer->AcquireRemoteObject(ref);
			}
		}

		void RpcDualLifecycleMock::ReleaseRemoteObject(RpcObjectReference ref)
		{
			if (ref.clientId == clientId)
			{
				auto index = localObjectProps.Keys().IndexOf(ref.objectId);
				CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::ReleaseRemoteObject: Object not registered.");
				auto props = localObjectProps.Values().Get(index);
				props->refCount--;
				if (props->refCount <= 0)
				{
					UnregisterLocalObject(ref);
				}
			}
			else
			{
				CHECK_ERROR(peer != nullptr, L"RpcDualLifecycleMock::ReleaseRemoteObject: No peer configured.");
				peer->ReleaseRemoteObject(ref);
			}
		}

/***********************************************************************
* RpcDualLifecycleMock (IRpcObjectOps)
***********************************************************************/

		RpcObjectReference RpcDualLifecycleMock::RequestService(vint typeId)
		{
			if (pendingProxyRef.clientId != 0)
			{
				return pendingProxyRef;
			}
			return RpcLifecycleMock::RequestService(typeId);
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
				CHECK_ERROR(universalWrapperFactory, L"RpcDualLifecycleMock::RequestService: No wrapper factory registered.");
				auto ref = RpcLifecycleMock::RequestService(typeId);
				return CreateCallerProxy(ref);
			}

			return nullptr;
		}

		Ptr<IDescriptable> RpcDualLifecycleMock::RefToPtr(RpcObjectReference ref)
		{
			if (ref.clientId == clientId)
			{
				// Local object
				auto index = localObjectProps.Keys().IndexOf(ref.objectId);
				CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::RefToPtr: Object not registered.");
				auto props = localObjectProps.Values().Get(index);
				CHECK_ERROR(props->rawPtr != nullptr, L"RpcDualLifecycleMock::RefToPtr: Object not tracked.");
				return Ptr(props->rawPtr);
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

			if (auto descObj = dynamic_cast<DescriptableObject*>(obj.Obj()))
			{
				RpcObjectReference wrapperRef;
				if (TryGetTrackedWrapperRef(descObj, wrapperRef))
				{
					return wrapperRef;
				}

				if (auto wrapperBase = descObj->SafeAggregationCast<IRpcWrapperBase>())
				{
					for (vint i = 0; i < wrapperEntries.Count(); i++)
					{
						auto&& entry = wrapperEntries[i];
						if (entry.proxy == wrapperBase)
						{
							return entry.ref;
						}
					}
				}
			}

			// Check if this is an already-tracked local object (via internal property)
			if (auto descObj = dynamic_cast<DescriptableObject*>(obj.Obj()))
			{
				if (auto trackerObj = descObj->GetInternalProperty(InternalProperty_LocalObjectTracker))
				{
					auto tracker = trackerObj.Cast<RpcDualObjectTracker>();
					CHECK_ERROR(tracker, L"RpcDualLifecycleMock::PtrToRef: Invalid internal property type.");
					CHECK_ERROR(tracker->GetMock() == this, L"RpcDualLifecycleMock::PtrToRef: Object registered to a different lifecycle.");
					auto trackerRef = tracker->GetRef();
					auto index = localObjectProps.Keys().IndexOf(trackerRef.objectId);
					CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::PtrToRef: Tracked object not registered.");
					localObjectProps.Values().Get(index)->refCount++;
					return trackerRef;
				}
			}

			// Register new local object
			auto typeId = DecideTypeId(obj.Obj());
			CHECK_ERROR(typeId != RpcTypeId_NotFound, L"RpcDualLifecycleMock::PtrToRef: DecideTypeId returned RpcTypeId_NotFound (unknown type).");
			auto ref = RegisterLocalObject(typeId);

			auto index = localObjectProps.Keys().IndexOf(ref.objectId);
			CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::PtrToRef: Failed to register object.");
			localObjectProps.Values().Get(index)->ownedPtr = obj;

			TrackLocalObject(ref, obj.Obj());
			return ref;
		}

/***********************************************************************
* RpcDualLifecycleMock (IRpcListEventOps)
***********************************************************************/

		void RpcDualLifecycleMock::OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)
		{
			for (vint i = 0; i < wrapperEntries.Count(); i++)
			{
				auto&& entry = wrapperEntries[i];
				if (entry.ref.objectId == ref.objectId)
				{
					if (auto observable = dynamic_cast<IValueObservableList*>(entry.proxy))
					{
						observable->ItemChanged(index, oldCount, newCount);
						return;
					}
				}
			}
		}
	}
}
