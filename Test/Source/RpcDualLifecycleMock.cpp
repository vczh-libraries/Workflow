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
			: mock(m), ref(r)
		{
			(void)p;
		}

		RpcDualWrapperTracker::~RpcDualWrapperTracker()
		{
			if (mock)
			{
				mock->UntrackWrapper(ref);
			}
		}

		void RpcDualWrapperTracker::Detach()
		{
			mock = nullptr;
		}

/***********************************************************************
* RpcDualControllerMock
***********************************************************************/

		RpcDualControllerMock::RpcDualControllerMock(RpcDualLifecycleMock* lc, vint _clientId)
			: lifecycle(lc), clientId(_clientId)
		{
		}

		RpcDualControllerMock::~RpcDualControllerMock()
		{
			UnregisterAllLocalObjects();
		}

		vint RpcDualControllerMock::GetClientId()const
		{
			return clientId;
		}

		const Dictionary<vint, Ptr<RpcLocalObjectProperties>>& RpcDualControllerMock::GetLocalObjectProperties()const
		{
			return localObjectProperties;
		}

		void RpcDualControllerMock::UnregisterAllLocalObjects()
		{
			while (localObjectProperties.Count() > 0)
			{
				auto props = localObjectProperties.Values().Get(localObjectProperties.Count() - 1);
				UnregisterLocalObject(props->ref);
			}
		}

/***********************************************************************
* RpcDualControllerMock (IRpcController)
***********************************************************************/

		void RpcDualControllerMock::Register(Ptr<IRpcObjectOps> _objectCallback, Ptr<IRpcObjectEventOps> _eventCallback, Ptr<IRpcListOps> _listCallback, Ptr<IRpcListEventOps> _listEventCallback)
		{
			objectCallback = _objectCallback;
			eventCallback = _eventCallback;
			listCallback = _listCallback;
			listEventCallback = _listEventCallback;

			// Do not call SyncIds on Workflow proxy objects.
			// The Workflow proxy's SyncIds cannot convert Dictionary<WString,vint> through reflection.
			// IDs are already synced via global variables set during <initialize>.
		}

		RpcObjectReference RpcDualControllerMock::RegisterLocalObject(vint typeId)
		{
			auto ref = RpcObjectReference{ clientId, ++nextObjectId, typeId };
			CHECK_ERROR(!localObjectProperties.Keys().Contains(ref.objectId), L"RpcDualControllerMock::RegisterLocalObject: Object ID already registered.");
			auto props = Ptr(new RpcLocalObjectProperties());
			props->ref = ref;
			localObjectProperties.Set(ref.objectId, props);
			return ref;
		}

		void RpcDualControllerMock::UnregisterLocalObject(RpcObjectReference ref)
		{
			auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
			CHECK_ERROR(index != -1, L"RpcDualControllerMock::UnregisterLocalObject: Object not registered.");

			if (lifecycle->localObjectCallback)
			{
				lifecycle->localObjectCallback->RegisterService(ref.typeId, nullptr);
			}
			lifecycle->UntrackLocalObject(ref);
			localObjectProperties.Remove(ref.objectId);
		}

		void RpcDualControllerMock::AcquireRemoteObject(RpcObjectReference ref)
		{
			if (ref.clientId == clientId)
			{
				auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
				CHECK_ERROR(index != -1, L"RpcDualControllerMock::AcquireRemoteObject: Object not registered.");
				localObjectProperties.Values().Get(index)->refCount++;
			}
			else
			{
				RpcControllerMock::ObjectHold(ref, clientId, true);
			}
		}

		void RpcDualControllerMock::ReleaseRemoteObject(RpcObjectReference ref)
		{
			if (ref.clientId == clientId)
			{
				auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
				CHECK_ERROR(index != -1, L"RpcDualControllerMock::ReleaseRemoteObject: Object not registered.");
				auto props = localObjectProperties.Values().Get(index);
				props->refCount--;
				if (props->refCount <= 0)
				{
					UnregisterLocalObject(ref);
				}
			}
			else
			{
				RpcControllerMock::ObjectHold(ref, clientId, false);
			}
		}

/***********************************************************************
* RpcDualControllerMock (IRpcObjectEventOps)
***********************************************************************/

		void RpcDualControllerMock::InvokeEvent(RpcObjectReference ref, vint eventId, Ptr<IValueArray> arguments)
		{
			if (!lifecycle->BeginForwardingEvent(ref, eventId))
			{
				return;
			}

			RpcControllerMock::InvokeEvent(ref, eventId, arguments);
			lifecycle->EndForwardingEvent(ref, eventId);
		}

/***********************************************************************
* RpcDualControllerMock (IRpcListEventOps)
***********************************************************************/

		void RpcDualControllerMock::OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)
		{
			if (auto proxy = lifecycle->GetTrackedWrapper(ref))
			{
				if (auto observable = dynamic_cast<IValueObservableList*>(proxy))
				{
					observable->ItemChanged(index, oldCount, newCount);
					return;
				}
			}

			if (listEventCallback)
			{
				RpcControllerMock::OnItemChanged(ref, index, oldCount, newCount);
			}
		}

/***********************************************************************
* RpcDualLifecycleMock (Constructor and Setup)
***********************************************************************/

		RpcDualLifecycleMock::RpcDualLifecycleMock(vint _clientId)
			: controller(this, _clientId)
		{
		}

		RpcDualLifecycleMock::~RpcDualLifecycleMock()
		{
			DisconnectTrackedWrappers();
			localObjectCallback = nullptr;
		}

		void RpcDualLifecycleMock::SetIdMap(const Dictionary<WString, vint>& _idMap)
		{
			for (auto&& [key, value] : _idMap)
			{
				idMap.Set(key, value);
			}
		}

		void RpcDualLifecycleMock::RegisterWrapperFactory(UniversalWrapperFactory factory)
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

			return RpcTypeId_NotFound;
		}

		bool RpcDualLifecycleMock::AttachLocalObjectEvents(RpcObjectReference ref, IDescriptable* obj, List<Func<void()>>& detachments)
		{
			return false;
		}

		bool RpcDualLifecycleMock::BeginForwardingEvent(RpcObjectReference ref, vint eventId)
		{
			if (!HasEventTarget(ref))
			{
				return false;
			}

			auto event = RpcDualEventDispatch{ ref,eventId };
			if (forwardingEvents.Contains(event))
			{
				return false;
			}
			forwardingEvents.Add(event);
			return true;
		}

		void RpcDualLifecycleMock::EndForwardingEvent(RpcObjectReference ref, vint eventId)
		{
			auto event = RpcDualEventDispatch{ ref,eventId };
			CHECK_ERROR(forwardingEvents.Remove(event), L"RpcDualLifecycleMock::EndForwardingEvent: Event was not forwarding.");
		}

		void RpcDualLifecycleMock::TrackLocalObject(RpcObjectReference ref, IDescriptable* obj)
		{
			auto&& localObjectProperties = controller.GetLocalObjectProperties();
			auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
			CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::TrackLocalObject: Object not registered.");
			auto props = localObjectProperties.Values().Get(index);
			CHECK_ERROR(props->rawPtr == nullptr, L"RpcDualLifecycleMock::TrackLocalObject: Object already tracked.");

			props->rawPtr = obj;

			if (auto observable = dynamic_cast<IValueObservableList*>(obj))
			{
				auto handler = observable->ItemChanged.Add([this, ref](vint index, vint oldCount, vint newCount)
				{
					this->GetController()->OnItemChanged(ref, index, oldCount, newCount);
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
			auto&& localObjectProperties = controller.GetLocalObjectProperties();
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

			auto rawPtr = props->rawPtr;
			props->rawPtr = nullptr;

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

			props->ownedPtr = nullptr;
		}

		bool RpcDualLifecycleMock::IsTracked(vint objectId)const
		{
			auto&& localObjectProperties = controller.GetLocalObjectProperties();
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
			CHECK_ERROR(!wrapperProperties.Keys().Contains(ref), L"RpcDualLifecycleMock::TrackWrapper: Reference already tracked.");
			wrapperProperties.Set(ref, properties);
			controller.AcquireRemoteObject(ref);
		}

		void RpcDualLifecycleMock::UntrackWrapper(RpcObjectReference ref)
		{
			wrapperProperties.Remove(ref);
		}

		void RpcDualLifecycleMock::DisconnectTrackedWrappers()
		{
			for (auto [ref, properties] : wrapperProperties)
			{
				controller.ReleaseRemoteObject(ref);

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
			wrapperProperties.Clear();
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

		IRpcWrapperBase* RpcDualLifecycleMock::GetTrackedWrapper(RpcObjectReference ref)const
		{
			if (auto index = wrapperProperties.Keys().IndexOf(ref); index != -1)
			{
				return wrapperProperties.Values()[index].proxy;
			}
			return nullptr;
		}

		bool RpcDualLifecycleMock::HasEventTarget(RpcObjectReference ref)const
		{
			if (ref.clientId == controller.GetClientId())
			{
				return IsTracked(ref.objectId);
			}
			return wrapperProperties.Keys().Contains(ref);
		}

		Ptr<IDescriptable> RpcDualLifecycleMock::CreateCallerProxy(RpcObjectReference ref)
		{
			IRpcLifeCycle* lc = this;
			Ptr<IRpcWrapperBase> wrapper;

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
				CHECK_ERROR(universalWrapperFactory, L"RpcDualLifecycleMock::CreateCallerProxy: No wrapper factory registered.");
				wrapper = universalWrapperFactory(ref, this);
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
* RpcDualLifecycleMock (IRpcLifeCycle)
***********************************************************************/

		RpcDualControllerMock* RpcDualLifecycleMock::GetController()
		{
			return &controller;
		}

		void RpcDualLifecycleMock::RegisterService(const WString& fullName, Ptr<IDescriptable> service)
		{
			auto index = idMap.Keys().IndexOf(fullName);
			if (index == -1)
			{
				throw Exception(L"Unknown RPC type id.");
			}
			CHECK_ERROR(localObjectCallback, L"RpcDualLifecycleMock::RegisterService: No local object callback registered.");
			localObjectCallback->RegisterService(idMap.Values()[index], service);
		}

		Ptr<IDescriptable> RpcDualLifecycleMock::RequestService(const WString& fullName)
		{
			if (idMap.Keys().Contains(fullName))
			{
				auto typeId = idMap.Get(fullName);
				CHECK_ERROR(universalWrapperFactory, L"RpcDualLifecycleMock::RequestService: No wrapper factory registered.");
				if (localObjectCallback)
				{
					auto localRef = localObjectCallback->RequestService(typeId);
					if (localRef)
					{
						return RefToPtr(localRef.Value());
					}
				}
				auto ref = controller.RequestService(typeId);
				CHECK_ERROR(ref, L"RpcDualLifecycleMock::RequestService: RPC service is not registered.");
				return CreateCallerProxy(ref.Value());
			}

			return nullptr;
		}

		Ptr<IDescriptable> RpcDualLifecycleMock::RefToPtr(RpcObjectReference ref)
		{
			if (ref.clientId == controller.GetClientId())
			{
				auto&& localObjectProperties = controller.GetLocalObjectProperties();
				auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
				CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::RefToPtr: Object not registered.");
				auto props = localObjectProperties.Values().Get(index);
				CHECK_ERROR(props->rawPtr != nullptr, L"RpcDualLifecycleMock::RefToPtr: Object not tracked.");
				return Ptr(props->rawPtr);
			}
			else
			{
				if (auto index = wrapperProperties.Keys().IndexOf(ref); index != -1)
				{
					auto proxy = wrapperProperties.Values()[index].proxy;
					auto descriptable = dynamic_cast<IDescriptable*>(proxy);
					CHECK_ERROR(descriptable, L"RpcDualLifecycleMock::RefToPtr: Wrapper does not implement IDescriptable.");
					return Ptr(descriptable);
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

			if (auto descObj = dynamic_cast<DescriptableObject*>(obj.Obj()))
			{
				if (auto trackerObj = descObj->GetInternalProperty(InternalProperty_LocalObjectTracker))
				{
					auto tracker = trackerObj.Cast<RpcDualLocalObjectTracker>();
					CHECK_ERROR(tracker, L"RpcDualLifecycleMock::PtrToRef: Invalid internal property type.");
					if (tracker->GetClientId() != controller.GetClientId())
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

			auto typeId = DecideTypeId(obj.Obj());
			CHECK_ERROR(typeId != RpcTypeId_NotFound, L"RpcDualLifecycleMock::PtrToRef: DecideTypeId returned RpcTypeId_NotFound (unknown type).");
			auto ref = controller.RegisterLocalObject(typeId);

			auto&& localObjectProperties = controller.GetLocalObjectProperties();
			auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
			CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::PtrToRef: Failed to register object.");
			localObjectProperties.Values().Get(index)->ownedPtr = obj;

			TrackLocalObject(ref, obj.Obj());
			return ref;
		}
	}
}
