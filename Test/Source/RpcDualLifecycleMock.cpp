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
				mock->RemoveLocalObject(ref, false);
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
* RpcDualDispatcherMock
***********************************************************************/

		RpcDualLifecycleMock* RpcDualDispatcherMock::GetLifecycle(vint clientId)const
		{
			if (lifecycle1 && lifecycle1->GetClientId() == clientId) return lifecycle1;
			if (lifecycle2 && lifecycle2->GetClientId() == clientId) return lifecycle2;
			CHECK_FAIL(L"RpcDualDispatcherMock::GetLifecycle: Unknown client id.");
			return nullptr;
		}

		RpcDualLifecycleMock* RpcDualDispatcherMock::GetOtherLifecycle(vint clientId)const
		{
			if (lifecycle1 && lifecycle1->GetClientId() == clientId) return lifecycle2;
			if (lifecycle2 && lifecycle2->GetClientId() == clientId) return lifecycle1;
			CHECK_FAIL(L"RpcDualDispatcherMock::GetOtherLifecycle: Unknown client id.");
			return nullptr;
		}

		RpcDualDispatcherMock::RpcDualDispatcherMock(RpcDualLifecycleMock* lc1, RpcDualLifecycleMock* lc2)
			: lifecycle1(lc1)
			, lifecycle2(lc2)
		{
			CHECK_ERROR(lifecycle1 && lifecycle2, L"RpcDualDispatcherMock requires two lifecycles.");
			lifecycle1->dispatcher = this;
			lifecycle2->dispatcher = this;
		}

		RpcDualDispatcherMock::~RpcDualDispatcherMock()
		{
		}

		void RpcDualDispatcherMock::Finalize()
		{
			lifecycle2->Finalize();
			lifecycle1->Finalize();
			services.Clear();
		}

		bool RpcDualDispatcherMock::IsRegisteredService(RpcObjectReference ref)
		{
			return services.Values().Contains(ref);
		}

		void RpcDualDispatcherMock::RegisterService(vint typeId, RpcObjectReference ref)
		{
			CHECK_ERROR(!services.Keys().Contains(typeId), L"RpcDualDispatcherMock::RegisterService: Service is already registered.");
			services.Set(typeId, ref);
			GetLifecycle(ref.clientId)->LocalObjectHold(ref, ref.clientId);
		}

		RpcObjectReference RpcDualDispatcherMock::RequestService(vint typeId)
		{
			auto index = services.Keys().IndexOf(typeId);
			CHECK_ERROR(index != -1, L"RpcDualDispatcherMock::RequestService: Service is not registered.");
			return services.Values()[index];
		}

		IRpcListEventOps* RpcDualDispatcherMock::BroadcastFromClient_ListEventOps(vint selfClientId)
		{
			return GetOtherLifecycle(selfClientId)->GetController()->GetListEventOps();
		}

		IRpcObjectEventOps* RpcDualDispatcherMock::BroadcastFromClient_ObjectEventOps(vint selfClientId)
		{
			return GetOtherLifecycle(selfClientId)->GetController()->GetObjectEventOps();
		}

		IRpcListOps* RpcDualDispatcherMock::SendToClient_ListOps(vint targetClientId)
		{
			return GetLifecycle(targetClientId)->GetController()->GetListOps();
		}

		IRpcObjectOps* RpcDualDispatcherMock::SendToClient_ObjectOps(vint targetClientId)
		{
			return GetLifecycle(targetClientId)->GetController()->GetObjectOps();
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

		void RpcDualLifecycleMock::DisconnectWrappersForFinalize()
		{
			for (auto [ref, properties] : wrapperProperties)
			{
				if (properties.root)
				{
					auto trackerObj = properties.root->GetInternalProperty(InternalProperty_WrapperTracker);
					CHECK_ERROR(trackerObj, L"RpcDualLifecycleMock::DisconnectWrappersForFinalize: Wrapper tracker missing.");
					auto tracker = trackerObj.Cast<RpcDualWrapperTracker>();
					CHECK_ERROR(tracker, L"RpcDualLifecycleMock::DisconnectWrappersForFinalize: Invalid wrapper tracker type.");
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
					if (this->GetController()->GetItemChangedSuppressedFlag(ref))
					{
						return;
					}
					this->GetDispatcher()->BroadcastFromClient_ListEventOps(this->GetClientId())->OnItemChanged(ref, index, oldCount, newCount);
				});
				props->eventHandler = handler;
			}

			AttachLocalObjectEvents(ref, obj);

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

		void RpcDualLifecycleMock::UntrackLocalObject(RpcObjectReference ref, bool clearInternalProperty)
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

			auto rawPtr = props->rawPtr;
			props->rawPtr = nullptr;

			if (clearInternalProperty && rawPtr)
			{
				if (auto descriptable = dynamic_cast<DescriptableObject*>(rawPtr))
				{
					if (auto trackerObj = descriptable->GetInternalProperty(InternalProperty_LocalObjectTracker))
					{
						auto tracker = trackerObj.Cast<RpcDualLocalObjectTracker>();
						CHECK_ERROR(tracker, L"RpcDualLifecycleMock::UntrackLocalObject: Invalid local object tracker type.");
						tracker->Detach();
						descriptable->SetInternalProperty(InternalProperty_LocalObjectTracker, nullptr);
					}
				}
			}

			props->ownedPtr = nullptr;
		}

		void RpcDualLifecycleMock::RemoveLocalObject(RpcObjectReference ref, bool clearInternalProperty)
		{
			if (localObjectProperties.Keys().Contains(ref.objectId))
			{
				UntrackLocalObject(ref, clearInternalProperty);
				localObjectProperties.Remove(ref.objectId);
			}
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
			CHECK_ERROR(!wrapperProperties.Keys().Contains(ref), L"RpcDualLifecycleMock::TrackWrapper: Reference already tracked.");
			wrapperProperties.Set(ref, properties);
		}

		void RpcDualLifecycleMock::UntrackWrapper(RpcObjectReference ref)
		{
			wrapperProperties.Remove(ref);
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

		Ptr<IDescriptable> RpcDualLifecycleMock::CreateCallerProxy(RpcObjectReference ref)
		{
			IRpcLifecycle* lc = this;
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
* RpcDualLifecycleMock (IRpcLifecycle)
***********************************************************************/

		void RpcDualLifecycleMock::Finalize()
		{
			DisconnectWrappersForFinalize();
			while (localObjectProperties.Count() > 0)
			{
				auto props = localObjectProperties.Values().Get(localObjectProperties.Count() - 1);
				RemoveLocalObject(props->ref, true);
			}
			controller.Finalize();
		}

		vint RpcDualLifecycleMock::GetClientId()
		{
			return clientId;
		}

		IRpcDispatcher* RpcDualLifecycleMock::GetDispatcher()
		{
			CHECK_ERROR(dispatcher, L"RpcDualLifecycleMock::GetDispatcher: No dispatcher registered.");
			return dispatcher;
		}

		RpcControllerDefault* RpcDualLifecycleMock::GetController()
		{
			return &controller;
		}

		void RpcDualLifecycleMock::LocalObjectHold(RpcObjectReference ref, vint remoteClientId)
		{
			CHECK_ERROR(ref.clientId == clientId, L"RpcDualLifecycleMock::LocalObjectHold: Ref is not local.");
			auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
			CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::LocalObjectHold: Object not registered.");
			auto props = localObjectProperties.Values().Get(index);
			if (!props->interestedClients.Contains(remoteClientId))
			{
				props->interestedClients.Add(remoteClientId);
			}
		}

		void RpcDualLifecycleMock::LocalObjectUnhold(RpcObjectReference ref, vint remoteClientId)
		{
			CHECK_ERROR(ref.clientId == clientId, L"RpcDualLifecycleMock::LocalObjectUnhold: Ref is not local.");
			auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
			CHECK_ERROR(index != -1, L"RpcDualLifecycleMock::LocalObjectUnhold: Object not registered.");
			auto props = localObjectProperties.Values().Get(index);
			CHECK_ERROR(props->interestedClients.Contains(remoteClientId), L"RpcDualLifecycleMock::LocalObjectUnhold: Client does not hold this object.");
			props->interestedClients.Remove(remoteClientId);
			if (props->interestedClients.Count() == 0)
			{
				RemoveLocalObject(ref, true);
			}
		}

		void RpcDualLifecycleMock::RegisterService(const WString& fullName, Ptr<IDescriptable> service)
		{
			auto index = idMap.Keys().IndexOf(fullName);
			if (index == -1)
			{
				throw Exception(L"Unknown RPC type id.");
			}
			controller.GetObjectOps()->RegisterService(idMap.Values()[index], service);
		}

		Ptr<IDescriptable> RpcDualLifecycleMock::RequestService(const WString& fullName)
		{
			if (idMap.Keys().Contains(fullName))
			{
				auto typeId = idMap.Get(fullName);
				auto ref = GetDispatcher()->RequestService(typeId);
				return RefToPtr(ref);
			}

			return nullptr;
		}

		Ptr<IDescriptable> RpcDualLifecycleMock::RefToPtr(RpcObjectReference ref)
		{
			if (ref.clientId == clientId)
			{
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

			auto typeId = DecideTypeId(obj.Obj());
			CHECK_ERROR(typeId != RpcTypeId_NotFound, L"RpcDualLifecycleMock::PtrToRef: DecideTypeId returned RpcTypeId_NotFound (unknown type).");
			auto ref = RpcObjectReference{ clientId, ++nextObjectId, typeId };
			CHECK_ERROR(!localObjectProperties.Keys().Contains(ref.objectId), L"RpcDualLifecycleMock::PtrToRef: Object ID already registered.");
			auto props = Ptr(new RpcLocalObjectProperties);
			props->ref = ref;
			props->ownedPtr = obj;
			localObjectProperties.Set(ref.objectId, props);

			TrackLocalObject(ref, obj.Obj());
			return ref;
		}
	}
}
