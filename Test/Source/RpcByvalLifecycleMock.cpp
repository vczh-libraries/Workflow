#include "RpcByvalLifecycleMock.h"

namespace vl
{
	namespace rpc_controller_test
	{
		using namespace reflection;
		using namespace reflection::description;
		using namespace rpc_controller;
		using namespace collections;

		RpcObjectTracker::RpcObjectTracker(RpcByvalControllerMock* c, RpcObjectReference r)
			: controller(c), ref(r)
		{
		}

		RpcObjectTracker::~RpcObjectTracker()
		{
			if (controller && controller->IsTracked(ref.objectId))
			{
				controller->RemoveLocalObject(ref);
			}
		}

		void RpcObjectTracker::Detach()
		{
			controller = nullptr;
		}

/***********************************************************************
* RpcByvalControllerMock
***********************************************************************/

		RpcByvalControllerMock::RpcByvalControllerMock(RpcByvalLifecycleMock* lc)
			: lifecycle(lc)
		{
		}

		void RpcByvalControllerMock::TrackLocalObject(RpcObjectReference ref, IDescriptable* obj)
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
				auto tracker = Ptr(new RpcObjectTracker(this, ref));
				descriptable->SetInternalProperty(L"RpcLifecycleTracker", tracker);
			}
		}

		void RpcByvalControllerMock::UntrackLocalObject(RpcObjectReference ref)
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
				if (auto descriptable = dynamic_cast<DescriptableObject*>(obj))
				{
					if (auto trackerObj = descriptable->GetInternalProperty(L"RpcLifecycleTracker"))
					{
						auto tracker = trackerObj.Cast<RpcObjectTracker>();
						CHECK_ERROR(tracker, L"RpcByvalControllerMock::UntrackLocalObject: Invalid tracker type.");
						tracker->Detach();
						descriptable->SetInternalProperty(L"RpcLifecycleTracker", nullptr);
					}
				}
				localObjects.Remove(ref.objectId);
			}
			refsById.Remove(ref.objectId);
			ownedObjects.Remove(ref.objectId);
			observableProxies.Remove(ref.objectId);
		}

		void RpcByvalControllerMock::RemoveLocalObject(RpcObjectReference ref)
		{
			if (localObjects.Keys().Contains(ref.objectId))
			{
				UntrackLocalObject(ref);
			}
			interestedClients.Remove(ref.objectId);
		}

		bool RpcByvalControllerMock::IsTracked(vint objectId)const
		{
			return localObjects.Keys().Contains(objectId);
		}

		RpcObjectReference RpcByvalControllerMock::GetLastRegisteredObject()const
		{
			return lastRegisteredObject;
		}

		RpcObjectReference RpcByvalControllerMock::AllocateLocalObject(vint typeId)
		{
			lastRegisteredObject = { clientId, ++nextObjectId, typeId };
			interestedClients.Set(lastRegisteredObject.objectId, Ptr(new SortedList<vint>));
			return lastRegisteredObject;
		}

		void RpcByvalControllerMock::LocalObjectHold(RpcObjectReference ref, vint remoteClientId)
		{
			CHECK_ERROR(ref.clientId == clientId, L"RpcByvalControllerMock::LocalObjectHold: Ref is not local.");
			CHECK_ERROR(interestedClients.Keys().Contains(ref.objectId), L"RpcByvalControllerMock::LocalObjectHold: Object not registered.");
			auto clients = interestedClients.Get(ref.objectId);
			if (!clients->Contains(remoteClientId))
			{
				clients->Add(remoteClientId);
			}
		}

		void RpcByvalControllerMock::LocalObjectUnhold(RpcObjectReference ref, vint remoteClientId)
		{
			CHECK_ERROR(ref.clientId == clientId, L"RpcByvalControllerMock::LocalObjectUnhold: Ref is not local.");
			CHECK_ERROR(interestedClients.Keys().Contains(ref.objectId), L"RpcByvalControllerMock::LocalObjectUnhold: Object not registered.");
			auto clients = interestedClients.Get(ref.objectId);
			CHECK_ERROR(clients->Contains(remoteClientId), L"RpcByvalControllerMock::LocalObjectUnhold: Client does not hold this object.");
			clients->Remove(remoteClientId);
			if (clients->Count() == 0)
			{
				RemoveLocalObject(ref);
			}
		}

		void RpcByvalControllerMock::Finalize()
		{
			while (refsById.Count() > 0)
			{
				RemoveLocalObject(refsById.Values().Get(refsById.Count() - 1));
			}
			RpcControllerMock::Finalize();
		}

/***********************************************************************
* RpcByvalControllerMock (IRpcListEventOps)
***********************************************************************/

		void RpcByvalControllerMock::OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)
		{
			if (observableProxies.Keys().Contains(ref.objectId))
			{
				observableProxies.Get(ref.objectId)->ItemChanged(index, oldCount, newCount);
			}
		}

/***********************************************************************
* RpcByvalLifecycleMock
***********************************************************************/

		RpcByvalLifecycleMock::RpcByvalLifecycleMock()
			: controller(this)
		{
			proxyFactories.Set(RpcTypeId_IValueEnumerable, [](IRpcLifecycle* lc, RpcObjectReference ref) -> Ptr<IDescriptable>
			{
				return Ptr(new RpcByrefEnumerable(lc, ref));
			});
			proxyFactories.Set(RpcTypeId_IValueEnumerator, [](IRpcLifecycle* lc, RpcObjectReference ref) -> Ptr<IDescriptable>
			{
				return Ptr(new RpcByrefEnumerator(lc, ref));
			});
			proxyFactories.Set(RpcTypeId_IValueArray, [](IRpcLifecycle* lc, RpcObjectReference ref) -> Ptr<IDescriptable>
			{
				return Ptr(new RpcByrefArray(lc, ref));
			});
			proxyFactories.Set(RpcTypeId_IValueReadonlyList, [](IRpcLifecycle* lc, RpcObjectReference ref) -> Ptr<IDescriptable>
			{
				return Ptr(new RpcByrefReadonlyList(lc, ref));
			});
			proxyFactories.Set(RpcTypeId_IValueList, [](IRpcLifecycle* lc, RpcObjectReference ref) -> Ptr<IDescriptable>
			{
				return Ptr(new RpcByrefList(lc, ref));
			});
			proxyFactories.Set(RpcTypeId_IValueObservableList, [this](IRpcLifecycle* lc, RpcObjectReference ref) -> Ptr<IDescriptable>
			{
				auto proxy = Ptr(new RpcByrefObservableList(lc, ref));
				controller.observableProxies.Set(ref.objectId, dynamic_cast<IValueObservableList*>(proxy.Obj()));
				return proxy;
			});
			proxyFactories.Set(RpcTypeId_IValueDictionary, [](IRpcLifecycle* lc, RpcObjectReference ref) -> Ptr<IDescriptable>
			{
				return Ptr(new RpcByrefDictionary(lc, ref));
			});
		}

		RpcByvalLifecycleMock::~RpcByvalLifecycleMock()
		{
			Finalize();
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
			return RpcTypeId_NotFound;
		}

		RpcObjectReference RpcByvalLifecycleMock::GetLastRegisteredObject()const
		{
			return controller.GetLastRegisteredObject();
		}

		Ptr<IDescriptable> RpcByvalLifecycleMock::CreateCallerProxy(RpcObjectReference ref)
		{
			CHECK_ERROR(proxyFactories.Keys().Contains(ref.typeId), L"RpcByvalLifecycleMock::CreateCallerProxy cannot find a proxy factory.");
			return proxyFactories.Get(ref.typeId)(this, ref);
		}

/***********************************************************************
* RpcByvalLifecycleMock (IRpcLifecycle)
***********************************************************************/

		void RpcByvalLifecycleMock::Finalize()
		{
			for (auto service : services.Values())
			{
				if (auto obj = service->SafeAggregationCast<IRpcWrapperBase>())
				{
					obj->DisconnectFromLifecycle();
				}
			}
			services.Clear();
			controller.Finalize();
		}

		vint RpcByvalLifecycleMock::GetClientId()
		{
			return controller.clientId;
		}

		IRpcDispatcher* RpcByvalLifecycleMock::GetDispatcher()
		{
			return this;
		}

		RpcByvalControllerMock* RpcByvalLifecycleMock::GetController()
		{
			return &controller;
		}

		void RpcByvalLifecycleMock::LocalObjectHold(RpcObjectReference ref, vint remoteClientId)
		{
			controller.LocalObjectHold(ref, remoteClientId);
		}

		void RpcByvalLifecycleMock::LocalObjectUnhold(RpcObjectReference ref, vint remoteClientId)
		{
			controller.LocalObjectUnhold(ref, remoteClientId);
		}

		void RpcByvalLifecycleMock::RegisterService(const WString& fullName, Ptr<IDescriptable> service)
		{
			services.Set(fullName, service);
		}

		Ptr<IDescriptable> RpcByvalLifecycleMock::RequestService(const WString& fullName)
		{
			if (auto index = services.Keys().IndexOf(fullName); index != -1)
			{
				return services.Values()[index];
			}
			return nullptr;
		}

		Ptr<IDescriptable> RpcByvalLifecycleMock::RefToPtr(RpcObjectReference ref)
		{
			if (controller.localObjects.Keys().Contains(ref.objectId))
			{
				return Ptr(controller.localObjects.Get(ref.objectId));
			}
			if (proxyFactories.Keys().Contains(ref.typeId))
			{
				return proxyFactories.Get(ref.typeId)(this, ref);
			}
			CHECK_FAIL(L"RpcByvalLifecycleMock::RefToPtr cannot find a proxy factory.");
			return nullptr;
		}

		RpcObjectReference RpcByvalLifecycleMock::PtrToRef(Ptr<IDescriptable> obj)
		{
			CHECK_ERROR(obj, L"RpcByvalLifecycleMock::PtrToRef requires a value.");
			if (controller.refsByPtr.Keys().Contains(obj.Obj()))
			{
				return controller.refsByPtr.Get(obj.Obj());
			}

			auto ref = controller.AllocateLocalObject(DecideTypeId(obj.Obj()));
			controller.ownedObjects.Set(ref.objectId, obj);
			controller.TrackLocalObject(ref, obj.Obj());
			return ref;
		}

/***********************************************************************
* RpcByvalLifecycleMock (IRpcDispatcher)
***********************************************************************/

		bool RpcByvalLifecycleMock::IsRegisteredService(RpcObjectReference ref)
		{
			(void)ref;
			return false;
		}

		void RpcByvalLifecycleMock::RegisterService(vint typeId, RpcObjectReference ref)
		{
			(void)typeId;
			(void)ref;
			CHECK_FAIL(L"RpcByvalLifecycleMock::RegisterService(typeId, ref) is not supported.");
		}

		RpcObjectReference RpcByvalLifecycleMock::RequestService(vint typeId)
		{
			(void)typeId;
			CHECK_FAIL(L"RpcByvalLifecycleMock::RequestService(typeId) is not supported.");
			return {};
		}

		IRpcListEventOps* RpcByvalLifecycleMock::BroadcastFromClient_ListEventOps(vint selfClientId)
		{
			(void)selfClientId;
			return controller.GetListEventOps();
		}

		IRpcObjectEventOps* RpcByvalLifecycleMock::BroadcastFromClient_ObjectEventOps(vint selfClientId)
		{
			(void)selfClientId;
			return controller.GetObjectEventOps();
		}

		IRpcListOps* RpcByvalLifecycleMock::SendToClient_ListOps(vint targetClientId)
		{
			(void)targetClientId;
			return controller.GetListOps();
		}

		IRpcObjectOps* RpcByvalLifecycleMock::SendToClient_ObjectOps(vint targetClientId)
		{
			(void)targetClientId;
			return controller.GetObjectOps();
		}
	}
}
