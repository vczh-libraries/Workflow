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
		List<RpcDualEventDispatch> RpcDualLifecycleMock::forwardingEvents;

		bool RpcDualLifecycleMock::IsSameEvent(const RpcDualEventDispatch& event, RpcObjectReference ref, vint eventId)
		{
			return event.ref == ref && event.eventId == eventId;
		}

		bool RpcDualLifecycleMock::TryGetForwardingEventId(RpcObjectReference ref, vint& eventId)
		{
			for (vint i = forwardingEvents.Count() - 1; i >= 0; i--)
			{
				auto&& event = forwardingEvents[i];
				if (event.ref == ref)
				{
					eventId = event.eventId;
					return true;
				}
			}
			return false;
		}

		void RpcDualLifecycleMock::PushForwardingEvent(RpcObjectReference ref, vint eventId)
		{
			forwardingEvents.Add({ ref,eventId });
		}

		void RpcDualLifecycleMock::PopForwardingEvent(RpcObjectReference ref, vint eventId)
		{
			CHECK_ERROR(forwardingEvents.Count() > 0, L"RpcDualLifecycleMock::PopForwardingEvent: Event stack is empty.");
			auto event = forwardingEvents[forwardingEvents.Count() - 1];
			CHECK_ERROR(IsSameEvent(event, ref, eventId), L"RpcDualLifecycleMock::PopForwardingEvent: Event stack mismatch.");
			forwardingEvents.RemoveAt(forwardingEvents.Count() - 1);
		}

		void RpcDualLifecycleMock::SuppressForwardedEvent(RpcObjectReference ref, vint eventId)
		{
			suppressedEvents.Add({ ref,eventId });
		}

		bool RpcDualLifecycleMock::TryConsumeSuppressedEvent(RpcObjectReference ref, vint eventId)
		{
			for (vint i = suppressedEvents.Count() - 1; i >= 0; i--)
			{
				if (IsSameEvent(suppressedEvents[i], ref, eventId))
				{
					suppressedEvents.RemoveAt(i);
					return true;
				}
			}
			return false;
		}

			RpcDualLocalObjectTracker::RpcDualLocalObjectTracker(RpcDualLifecycleMock* m, RpcObjectReference r)
			: mock(m), ref(r)
		{
		}

			RpcDualLocalObjectTracker::~RpcDualLocalObjectTracker()
		{
				if (mock && mock->IsTracked(ref.objectId))
			{
				mock->UntrackLocalObject(ref);
			}
		}

			void RpcDualLocalObjectTracker::Attach(RpcDualLifecycleMock* m, RpcObjectReference r)
		{
				mock = m;
				ref = r;
			}

			void RpcDualLocalObjectTracker::Detach()
			{
				mock = nullptr;
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
			while (localObjectProperties.Count() > 0)
			{
				auto props = localObjectProperties.Values().Get(localObjectProperties.Count() - 1);
				UnregisterLocalObject(props->ref);
			}
			services.Clear();
			localObjectCallback = nullptr;
		}

		void RpcDualLifecycleMock::SetIdMap(const Dictionary<WString, vint>& _idMap)
		{
			for (auto&& [key, value] : _idMap)
			{
				idMap.Set(key, value);
			}
		}

		void RpcDualLifecycleMock::RegisterWrapperFactory(Func<Ptr<IRpcWrapperBase>(vint, IRpcLifeCycle*)> factory)
		{
			universalWrapperFactory = factory;
		}

		void RpcDualLifecycleMock::RegisterLocalObjectOps(Ptr<IRpcObjectOps> objectCallback)
		{
			localObjectCallback = objectCallback;
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

		bool RpcDualLifecycleMock::AttachLocalObjectEvents(RpcObjectReference ref, IDescriptable* obj, List<Func<void()>>& detachments)
		{
			return false;
		}

		void RpcDualLifecycleMock::TrackLocalObject(RpcObjectReference ref, IDescriptable* obj)
		{
			auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
			CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::TrackLocalObject: Object not registered.");
			auto props = localObjectProperties.Values().Get(index);
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

			AttachLocalObjectEvents(ref, obj, props->nativeEventDetachments);

			if (auto descriptable = dynamic_cast<DescriptableObject*>(obj))
			{
					Ptr<RpcDualLocalObjectTracker> tracker;
				if (auto trackerObj = descriptable->GetInternalProperty(InternalProperty_LocalObjectTracker))
				{
						tracker = trackerObj.Cast<RpcDualLocalObjectTracker>();
					CHECK_ERROR(tracker, L"RpcDualLifecycleMock::TrackLocalObject: Invalid local object tracker type.");
						if (tracker->GetClientId() != ref.clientId)
					{
						throw Exception(L"RpcDualLifecycleMock::TrackLocalObject: Object already tracked by a different client.");
					}
						CHECK_ERROR(!tracker->IsTracked(), L"RpcDualLifecycleMock::TrackLocalObject: Object already tracked.");
					}
					else
					{
						tracker = Ptr(new RpcDualLocalObjectTracker(this, ref));
						descriptable->SetInternalProperty(InternalProperty_LocalObjectTracker, tracker);
				}

					tracker->Attach(this, ref);
			}
		}

		void RpcDualLifecycleMock::UntrackLocalObject(RpcObjectReference ref)
		{
			auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
			CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::UntrackLocalObject: Object not registered.");
			auto props = localObjectProperties.Values().Get(index);

			if (props->eventHandler)
			{
				if (auto observable = dynamic_cast<IValueObservableList*>(props->rawPtr))
				{
					observable->ItemChanged.Remove(props->eventHandler);
				}
				props->eventHandler = nullptr;
			}

			for (auto&& detach : props->nativeEventDetachments)
			{
				detach();
			}
			props->nativeEventDetachments.Clear();

			// Clear rawPtr first so IsTracked returns false, preventing re-entrant cleanup
			auto rawPtr = props->rawPtr;
			props->rawPtr = nullptr;

				// Detach the tracker so it keeps ownership information without re-entering cleanup.
			if (rawPtr)
			{
				if (auto descriptable = dynamic_cast<DescriptableObject*>(rawPtr))
				{
						if (auto trackerObj = descriptable->GetInternalProperty(InternalProperty_LocalObjectTracker))
						{
							auto tracker = trackerObj.Cast<RpcDualLocalObjectTracker>();
							CHECK_ERROR(tracker, L"RpcDualLifecycleMock::UntrackLocalObject: Invalid local object tracker type.");
							tracker->Detach();
						}
				}
			}

			// Clear owned ptr last (may destroy the object, but internal property is already cleared)
			props->ownedPtr = nullptr;
		}

		bool RpcDualLifecycleMock::IsTracked(vint objectId)const
		{
			auto index = localObjectProperties.Keys().IndexOf(objectId);
			return index != -1 && localObjectProperties.Values().Get(index)->rawPtr != nullptr;
		}

		void RpcDualLifecycleMock::TrackWrapper(DescriptableObject* root, IRpcWrapperBase* proxy, RpcObjectReference ref)
		{
			CHECK_ERROR(root != nullptr, L"RpcDualLifecycleMock::TrackWrapper: Wrapper does not implement DescriptableObject.");
			CHECK_ERROR(proxy != nullptr, L"RpcDualLifecycleMock::TrackWrapper: Wrapper is null.");
			CHECK_ERROR(root->GetInternalProperty(InternalProperty_WrapperTracker) == nullptr, L"RpcDualLifecycleMock::TrackWrapper: Wrapper already tracked.");

			auto tracker = Ptr(new RpcDualWrapperTracker(this, proxy, ref));
			root->SetInternalProperty(InternalProperty_WrapperTracker, tracker);

			RpcWrapperProperties properties;
			properties.root = root;
			properties.proxy = proxy;
			properties.ref = ref;
			wrapperProperties.Add(properties);
			AcquireRemoteObject(ref);
		}

		void RpcDualLifecycleMock::UntrackWrapper(IRpcWrapperBase* proxy)
		{
			for (vint i = 0; i < wrapperProperties.Count(); i++)
			{
				if (wrapperProperties[i].proxy == proxy)
				{
					wrapperProperties.RemoveAt(i);
					return;
				}
			}
		}

		void RpcDualLifecycleMock::DisconnectTrackedWrappers()
		{
			while (wrapperProperties.Count() > 0)
			{
				auto properties = wrapperProperties[wrapperProperties.Count() - 1];
				wrapperProperties.RemoveAt(wrapperProperties.Count() - 1);
				ReleaseRemoteObject(properties.ref);

				if (properties.root)
				{
					auto trackerObj = properties.root->GetInternalProperty(InternalProperty_WrapperTracker);
					CHECK_ERROR(trackerObj, L"RpcDualLifecycleMock::DisconnectTrackedWrappers: Wrapper tracker missing.");
					auto tracker = trackerObj.Cast<RpcDualWrapperTracker>();
					CHECK_ERROR(tracker, L"RpcDualLifecycleMock::DisconnectTrackedWrappers: Invalid wrapper tracker type.");
					tracker->Detach();
					properties.root->SetInternalProperty(InternalProperty_WrapperTracker, nullptr);
				}

				if (properties.proxy)
				{
					properties.proxy->DisconnectFromLifecycle();
				}
			}
		}

		bool RpcDualLifecycleMock::TryGetTrackedWrapperRef(DescriptableObject* obj, RpcObjectReference& ref)const
		{
			auto wrapperRoot = obj;
			if (auto wrapperBase = obj->SafeAggregationCast<IRpcWrapperBase>())
			{
				auto aggregatedRoot = dynamic_cast<DescriptableObject*>(wrapperBase);
				CHECK_ERROR(aggregatedRoot, L"RpcDualLifecycleMock::TryGetTrackedWrapperRef: Wrapper root does not implement DescriptableObject.");
				wrapperRoot = aggregatedRoot;
			}

			if (auto trackerObj = wrapperRoot->GetInternalProperty(InternalProperty_WrapperTracker))
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
			IRpcLifeCycle* lc = this;
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
				pendingProxyRef = ref;
				wrapper = universalWrapperFactory(ref.typeId, this);
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
			objectCallback = _objectCallback;
			eventCallback = _eventCallback;
			listCallback = _listCallback;
			listEventCallback = _listEventCallback;

			// Do not call SyncIds on Workflow proxy objects.
			// The Workflow proxy's SyncIds cannot convert Dictionary<WString,vint> through reflection.
			// IDs are already synced via global variables set during <initialize>.
		}

		RpcObjectReference RpcDualLifecycleMock::RegisterLocalObject(vint typeId)
		{
			auto ref = RpcObjectReference{ clientId, nextObjectId++, typeId };
			CHECK_ERROR(!localObjectProperties.Keys().Contains(ref.objectId), L"RpcDualLifecycleMock::RegisterLocalObject: Object ID already registered.");
			auto props = Ptr(new RpcLocalObjectProperties());
			props->ref = ref;
			localObjectProperties.Set(ref.objectId, props);
			return ref;
		}

		void RpcDualLifecycleMock::UnregisterLocalObject(RpcObjectReference ref)
		{
			auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
			CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::UnregisterLocalObject: Object not registered.");
			auto props = localObjectProperties.Values().Get(index);

			bool unregisterService = false;
			for (vint i = services.Count() - 1; i >= 0; i--)
			{
				if (services.Values()[i].Obj() == props->rawPtr)
				{
					unregisterService = true;
					services.Remove(services.Keys()[i]);
				}
			}
			if (unregisterService && localObjectCallback)
			{
				localObjectCallback->RegisterService(props->ref.typeId, nullptr);
			}

			UntrackLocalObject(ref);
			localObjectProperties.Remove(ref.objectId);
		}

		void RpcDualLifecycleMock::AcquireRemoteObject(RpcObjectReference ref)
		{
			if (ref.clientId == clientId)
			{
				auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
				CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::AcquireRemoteObject: Object not registered.");
				localObjectProperties.Values().Get(index)->refCount++;
			}
			else
			{
				RpcLifecycleMock::ObjectHold(ref, clientId, true);
			}
		}

		void RpcDualLifecycleMock::ReleaseRemoteObject(RpcObjectReference ref)
		{
			if (ref.clientId == clientId)
			{
				auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
				CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::ReleaseRemoteObject: Object not registered.");
				auto props = localObjectProperties.Values().Get(index);
				props->refCount--;
				if (props->refCount <= 0)
				{
					UnregisterLocalObject(ref);
				}
			}
			else
			{
				RpcLifecycleMock::ObjectHold(ref, clientId, false);
			}
		}

		void RpcDualLifecycleMock::InvokeEvent(RpcObjectReference ref, vint eventId, Ptr<IValueArray> arguments)
		{
			if (TryConsumeSuppressedEvent(ref, eventId))
			{
				return;
			}

			PushForwardingEvent(ref, eventId);
			try
			{
				RpcLifecycleMock::InvokeEvent(ref, eventId, arguments);
			}
			catch (...)
			{
				PopForwardingEvent(ref, eventId);
				throw;
			}
			PopForwardingEvent(ref, eventId);
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

		void RpcDualLifecycleMock::RegisterService(const WString& fullName, Ptr<IDescriptable> service)
		{
			auto index = idMap.Keys().IndexOf(fullName);
			if (index == -1)
			{
				throw Exception(L"Unknown RPC type id.");
			}
			CHECK_ERROR(localObjectCallback, L"RpcDualLifecycleMock::RegisterService: No local object callback registered.");
			localObjectCallback->RegisterService(idMap.Values()[index], service);
			services.Set(fullName, service);
		}

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
				CHECK_ERROR(universalWrapperFactory, L"RpcDualLifecycleMock::RequestService: No wrapper factory registered.");
				auto ref = RpcLifecycleMock::RequestService(typeId);
				return CreateCallerProxy(ref);
			}

			return nullptr;
		}

		Ptr<IDescriptable> RpcDualLifecycleMock::RefToPtr(RpcObjectReference ref)
		{
			vint forwardingEventId = 0;
			auto forwardingEvent = TryGetForwardingEventId(ref, forwardingEventId);

			if (ref.clientId == clientId)
			{
				// Local object
				auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
				CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::RefToPtr: Object not registered.");
				auto props = localObjectProperties.Values().Get(index);
				CHECK_ERROR(props->rawPtr != nullptr, L"RpcDualLifecycleMock::RefToPtr: Object not tracked.");
				if (forwardingEvent)
				{
					SuppressForwardedEvent(ref, forwardingEventId);
				}
				return Ptr(props->rawPtr);
			}
			else
			{
				for (auto&& properties : wrapperProperties)
				{
					if (properties.ref == ref)
					{
						if (forwardingEvent)
						{
							SuppressForwardedEvent(ref, forwardingEventId);
						}
						auto descriptable = dynamic_cast<IDescriptable*>(properties.proxy);
						CHECK_ERROR(descriptable, L"RpcDualLifecycleMock::RefToPtr: Wrapper does not implement IDescriptable.");
						return Ptr(descriptable);
					}
				}

				if (forwardingEvent)
				{
					SuppressForwardedEvent(ref, forwardingEventId);
				}
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
					auto wrapperRoot = dynamic_cast<DescriptableObject*>(wrapperBase);
					CHECK_ERROR(wrapperRoot, L"RpcDualLifecycleMock::PtrToRef: Wrapper root does not implement DescriptableObject.");
					CHECK_ERROR(wrapperRoot->GetInternalProperty(InternalProperty_WrapperTracker), L"RpcDualLifecycleMock::PtrToRef: Wrapper tracker missing.");
					CHECK_FAIL(L"RpcDualLifecycleMock::PtrToRef: Wrapper tracker lookup unexpectedly failed.");
				}
			}

			// Check if this is an already-tracked local object (via internal property)
			if (auto descObj = dynamic_cast<DescriptableObject*>(obj.Obj()))
			{
				if (auto trackerObj = descObj->GetInternalProperty(InternalProperty_LocalObjectTracker))
				{
						auto tracker = trackerObj.Cast<RpcDualLocalObjectTracker>();
					CHECK_ERROR(tracker, L"RpcDualLifecycleMock::PtrToRef: Invalid internal property type.");
						if (tracker->GetClientId() != clientId)
					{
						throw Exception(L"RpcDualLifecycleMock::PtrToRef: Object registered to a different lifecycle.");
					}
						if (tracker->IsTracked())
					{
							CHECK_ERROR(tracker->GetMock() == this, L"RpcDualLifecycleMock::PtrToRef: Object tracker registered to an invalid lifecycle instance.");
							return tracker->GetRef();
					}
				}
			}

			// Register new local object
			auto typeId = DecideTypeId(obj.Obj());
			CHECK_ERROR(typeId != RpcTypeId_NotFound, L"RpcDualLifecycleMock::PtrToRef: DecideTypeId returned RpcTypeId_NotFound (unknown type).");
			auto ref = RegisterLocalObject(typeId);

			auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
			CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::PtrToRef: Failed to register object.");
			localObjectProperties.Values().Get(index)->ownedPtr = obj;

			TrackLocalObject(ref, obj.Obj());
			return ref;
		}

/***********************************************************************
* RpcDualLifecycleMock (IRpcListEventOps)
***********************************************************************/

		void RpcDualLifecycleMock::OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)
		{
			for (vint i = 0; i < wrapperProperties.Count(); i++)
			{
				auto&& properties = wrapperProperties[i];
				if (properties.ref == ref)
				{
					if (auto observable = dynamic_cast<IValueObservableList*>(properties.proxy))
					{
						observable->ItemChanged(index, oldCount, newCount);
						return;
					}
				}
			}

			if (listEventCallback)
			{
				RpcLifecycleMock::OnItemChanged(ref, index, oldCount, newCount);
			}
		}
	}
}
