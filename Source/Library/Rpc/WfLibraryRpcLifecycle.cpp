#include "WfLibraryRpcLifecycle.h"
#include "WfLibraryRpcWrappers.h"

namespace vl
{
	namespace rpc_controller
	{
		using namespace reflection;
		using namespace reflection::description;
		using namespace collections;

		namespace
		{
			bool IsRpcWrapperObject(DescriptableObject* obj)
			{
				if (!obj) return false;
				if (dynamic_cast<IRpcWrapperBase*>(obj)) return true;
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				if (auto typeDescriptor = obj->GetTypeDescriptor())
				{
					if (auto wrapperType = GetTypeDescriptor(WString::Unmanaged(L"system::IRpcWrapperBase")))
					{
						return typeDescriptor->CanConvertTo(wrapperType);
					}
				}
#endif
				return false;
			}
		}

		WString RpcLifecycleBase::InternalProperty_LocalObjectTracker = WString::Unmanaged(L"RpcLocalObjectTracker");
		WString RpcLifecycleBase::InternalProperty_WrapperTracker = WString::Unmanaged(L"RpcWrapperTracker");

		RpcLocalObjectTracker::RpcLocalObjectTracker(RpcLifecycleBase* lc, RpcObjectReference r)
			: lifecycle(lc), ref(r)
		{
		}

		RpcLocalObjectTracker::~RpcLocalObjectTracker()
		{
			if (lifecycle && lifecycle->IsTracked(ref.objectId))
			{
				lifecycle->RemoveLocalObject(ref, false);
			}
		}

		void RpcLocalObjectTracker::Attach(RpcLifecycleBase* lc, RpcObjectReference r)
		{
			lifecycle = lc;
			ref = r;
		}

		void RpcLocalObjectTracker::Detach()
		{
			lifecycle = nullptr;
		}

		RpcWrapperTracker::RpcWrapperTracker(RpcLifecycleBase* lc, RpcObjectReference r)
			: lifecycle(lc), ref(r)
		{
		}

		RpcWrapperTracker::~RpcWrapperTracker()
		{
			if (lifecycle)
			{
				lifecycle->UntrackWrapper(ref);
			}
		}

		void RpcWrapperTracker::Detach()
		{
			lifecycle = nullptr;
		}

/***********************************************************************
* RpcLifecycleBase (Constructor and Setup)
***********************************************************************/

		RpcLifecycleBase::RpcLifecycleBase(vint _clientId)
			: clientId(_clientId)
		{
		}

		RpcLifecycleBase::~RpcLifecycleBase()
		{
		}

		void RpcLifecycleBase::SetIdMap(const Dictionary<WString, vint>& _idMap)
		{
			CopyFrom(idMap, _idMap);
		}

		void RpcLifecycleBase::SetSerializer(Ptr<IRpcSerializer> _serializer)
		{
			serializer = _serializer;
		}

		IRpcSerializer* RpcLifecycleBase::GetSerializer()
		{
			return serializer.Obj();
		}

		void RpcLifecycleBase::RegisterWrapperFactory(UniversalWrapperFactory factory)
		{
			universalWrapperFactory = factory;
		}

		void RpcLifecycleBase::DisconnectWrappersForFinalize()
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcLifecycleBase::DisconnectWrappersForFinalize()#"
			for (auto [ref, properties] : wrapperProperties)
			{
				if (properties.root)
				{
					auto trackerObj = properties.root->GetInternalProperty(InternalProperty_WrapperTracker);
					CHECK_ERROR(trackerObj, ERROR_MESSAGE_PREFIX L"Wrapper tracker missing.");
					auto tracker = trackerObj.Cast<RpcWrapperTracker>();
					CHECK_ERROR(tracker, ERROR_MESSAGE_PREFIX L"Invalid wrapper tracker type.");
					tracker->Detach();
					properties.root->SetInternalProperty(InternalProperty_WrapperTracker, nullptr);
				}

				if (properties.proxy)
				{
					properties.proxy->DisconnectFromLifecycle();
				}
			}
			wrapperProperties.Clear();
#undef ERROR_MESSAGE_PREFIX
		}

		vint RpcLifecycleBase::DecideTypeId(IDescriptable* obj)const
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

		void RpcLifecycleBase::TrackLocalObject(RpcObjectReference ref, IDescriptable* obj)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcLifecycleBase::TrackLocalObject(RpcObjectReference, reflection::IDescriptable*)#"
			auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
			CHECK_ERROR(index != -1, ERROR_MESSAGE_PREFIX L"Object not registered.");
			auto props = localObjectProperties.Values().Get(index);
			CHECK_ERROR(props->rawPtr == nullptr, ERROR_MESSAGE_PREFIX L"Object already tracked.");

			props->rawPtr = obj;

			if (auto observable = dynamic_cast<IValueObservableList*>(obj))
			{
				auto handler = observable->ItemChanged.Add([this, ref](vint index, vint oldCount, vint newCount)
				{
					if (this->GetController()->GetItemChangedSuppressedFlag(ref))
					{
						return;
					}
					auto listEventOps = Ptr(new RpcCallerListEventOps(
						this->GetDispatcher()->BroadcastFromClient_ObjectEventOps(this->GetClientId()),
						serializer.Obj()
						));
					listEventOps->OnItemChanged(ref, index, oldCount, newCount);
				});
				props->eventHandler = handler;
			}

			AttachLocalObjectEvents(ref, obj);

			if (auto descriptable = dynamic_cast<DescriptableObject*>(obj))
			{
				Ptr<RpcLocalObjectTracker> tracker;
				if (auto trackerObj = descriptable->GetInternalProperty(InternalProperty_LocalObjectTracker))
				{
					tracker = trackerObj.Cast<RpcLocalObjectTracker>();
					CHECK_ERROR(tracker, ERROR_MESSAGE_PREFIX L"Invalid local object tracker type.");
					if (tracker->GetClientId() != ref.clientId)
					{
						throw Exception(ERROR_MESSAGE_PREFIX L"Object already tracked by a different client.");
					}
					CHECK_ERROR(!tracker->IsTracked(), ERROR_MESSAGE_PREFIX L"Object already tracked.");
				}
				else
				{
					tracker = Ptr(new RpcLocalObjectTracker(this, ref));
					descriptable->SetInternalProperty(InternalProperty_LocalObjectTracker, tracker);
				}

				tracker->Attach(this, ref);
			}
#undef ERROR_MESSAGE_PREFIX
		}

		void RpcLifecycleBase::UntrackLocalObject(RpcObjectReference ref, bool clearInternalProperty)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcLifecycleBase::UntrackLocalObject(RpcObjectReference, bool)#"
			auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
			CHECK_ERROR(index != -1, ERROR_MESSAGE_PREFIX L"Object not registered.");
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
						auto tracker = trackerObj.Cast<RpcLocalObjectTracker>();
						CHECK_ERROR(tracker, ERROR_MESSAGE_PREFIX L"Invalid local object tracker type.");
						tracker->Detach();
						descriptable->SetInternalProperty(InternalProperty_LocalObjectTracker, nullptr);
					}
				}
			}

			props->ownedPtr = nullptr;
#undef ERROR_MESSAGE_PREFIX
		}

		void RpcLifecycleBase::RemoveLocalObject(RpcObjectReference ref, bool clearInternalProperty)
		{
			if (localObjectProperties.Keys().Contains(ref.objectId))
			{
				UntrackLocalObject(ref, clearInternalProperty);
				localObjectProperties.Remove(ref.objectId);
			}
		}

		bool RpcLifecycleBase::IsTracked(vint objectId)const
		{
			auto index = localObjectProperties.Keys().IndexOf(objectId);
			return index != -1 && localObjectProperties.Values().Get(index)->rawPtr != nullptr;
		}

		void RpcLifecycleBase::TrackWrapper(DescriptableObject* root, IRpcWrapperBase* proxy, RpcObjectReference ref)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcLifecycleBase::TrackWrapper(reflection::DescriptableObject*, IRpcWrapperBase*, RpcObjectReference)#"
			CHECK_ERROR(root != nullptr, ERROR_MESSAGE_PREFIX L"Wrapper does not implement DescriptableObject.");
			CHECK_ERROR(proxy != nullptr, ERROR_MESSAGE_PREFIX L"Wrapper is null.");
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
			root = root->SafeGetAggregationRoot();
#endif
			CHECK_ERROR(root->GetInternalProperty(InternalProperty_WrapperTracker) == nullptr, ERROR_MESSAGE_PREFIX L"Wrapper already tracked.");

			auto tracker = Ptr(new RpcWrapperTracker(this, ref));
			root->SetInternalProperty(InternalProperty_WrapperTracker, tracker);

			RpcWrapperProperties properties;
			properties.root = root;
			properties.proxy = proxy;
			CHECK_ERROR(!wrapperProperties.Keys().Contains(ref), ERROR_MESSAGE_PREFIX L"Reference already tracked.");
			wrapperProperties.Set(ref, properties);
#undef ERROR_MESSAGE_PREFIX
		}

		void RpcLifecycleBase::UntrackWrapper(RpcObjectReference ref)
		{
			wrapperProperties.Remove(ref);
		}

		bool RpcLifecycleBase::TryGetTrackedWrapperRef(DescriptableObject* obj, RpcObjectReference& ref)const
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcLifecycleBase::TryGetTrackedWrapperRef(reflection::DescriptableObject*, RpcObjectReference&)const#"
			auto wrapperRoot = obj;
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
			if (wrapperRoot)
			{
				wrapperRoot = wrapperRoot->SafeGetAggregationRoot();
			}
#endif

			if (wrapperRoot)
			{
				if (auto trackerObj = wrapperRoot->GetInternalProperty(InternalProperty_WrapperTracker))
				{
					auto tracker = trackerObj.Cast<RpcWrapperTracker>();
					CHECK_ERROR(tracker, ERROR_MESSAGE_PREFIX L"Invalid internal property type.");
					CHECK_ERROR(tracker->GetLifecycle() == this, ERROR_MESSAGE_PREFIX L"Wrapper registered to a different lifecycle.");
					ref = tracker->GetRef();
					return true;
				}
			}
			return false;
#undef ERROR_MESSAGE_PREFIX
		}

		IRpcWrapperBase* RpcLifecycleBase::GetTrackedWrapper(RpcObjectReference ref)const
		{
			if (auto index = wrapperProperties.Keys().IndexOf(ref); index != -1)
			{
				return wrapperProperties.Values()[index].proxy;
			}
			return nullptr;
		}

		Ptr<IDescriptable> RpcLifecycleBase::CreateCallerProxy(RpcObjectReference ref, IRpcSerializer* serializer)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcLifecycleBase::CreateCallerProxy(RpcObjectReference)#"
			IRpcLifecycle* lc = this;
			Ptr<IRpcWrapperBase> wrapper;

			switch (ref.typeId)
			{
			case RpcTypeId_IValueEnumerable:
				wrapper = Ptr(new RpcByrefEnumerable(lc, ref, serializer));
				break;
			case RpcTypeId_IValueEnumerator:
				wrapper = Ptr(new RpcByrefEnumerator(lc, ref, serializer));
				break;
			case RpcTypeId_IValueArray:
				wrapper = Ptr(new RpcByrefArray(lc, ref, serializer));
				break;
			case RpcTypeId_IValueReadonlyList:
				wrapper = Ptr(new RpcByrefReadonlyList(lc, ref, serializer));
				break;
			case RpcTypeId_IValueList:
				wrapper = Ptr(new RpcByrefList(lc, ref, serializer));
				break;
			case RpcTypeId_IValueObservableList:
				wrapper = Ptr(new RpcByrefObservableList(lc, ref, serializer));
				break;
			case RpcTypeId_IValueDictionary:
				wrapper = Ptr(new RpcByrefDictionary(lc, ref, serializer));
				break;
			default:
				CHECK_ERROR(universalWrapperFactory, ERROR_MESSAGE_PREFIX L"No wrapper factory registered.");
				wrapper = universalWrapperFactory(ref, this);
				CHECK_ERROR(wrapper, ERROR_MESSAGE_PREFIX L"Wrapper factory returned null.");
				break;
			}

			auto descriptable = dynamic_cast<IDescriptable*>(wrapper.Obj());
			CHECK_ERROR(descriptable, ERROR_MESSAGE_PREFIX L"Wrapper does not implement IDescriptable.");
			auto root = dynamic_cast<DescriptableObject*>(descriptable);
			TrackWrapper(root, wrapper.Obj(), ref);
			return Ptr(descriptable);
#undef ERROR_MESSAGE_PREFIX
		}

/***********************************************************************
* RpcLifecycleBase (IRpcLifecycle)
***********************************************************************/

		void RpcLifecycleBase::Finalize()
		{
			DisconnectWrappersForFinalize();
			while (localObjectProperties.Count() > 0)
			{
				auto props = localObjectProperties.Values().Get(localObjectProperties.Count() - 1);
				RemoveLocalObject(props->ref, true);
			}
			registeredLocalServices.Clear();
			registeredRemoteServices.Clear();
			controller.Finalize();
		}

		void RpcLifecycleBase::Initialize()
		{
			if (!initialized)
			{
				GetDispatcher()->Initialize();
				initialized = true;
			}
		}

		vint RpcLifecycleBase::GetClientId()
		{
			return clientId;
		}

		RpcControllerDefault* RpcLifecycleBase::GetController()
		{
			return &controller;
		}

		const RpcLocalServiceMap& RpcLifecycleBase::GetRegisteredLocalServices()
		{
			return registeredLocalServices;
		}

		void RpcLifecycleBase::LocalObjectHold(RpcObjectReference ref, vint remoteClientId)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcLifecycleBase::LocalObjectHold(RpcObjectReference, vint)#"
			CHECK_ERROR(ref.clientId == clientId, ERROR_MESSAGE_PREFIX L"Ref is not local.");
			auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
			CHECK_ERROR(index != -1, ERROR_MESSAGE_PREFIX L"Object not registered.");
			auto props = localObjectProperties.Values().Get(index);
			if (!props->interestedClients.Contains(remoteClientId))
			{
				props->interestedClients.Add(remoteClientId);
			}
#undef ERROR_MESSAGE_PREFIX
		}

		void RpcLifecycleBase::LocalObjectUnhold(RpcObjectReference ref, vint remoteClientId)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcLifecycleBase::LocalObjectUnhold(RpcObjectReference, vint)#"
			CHECK_ERROR(ref.clientId == clientId, ERROR_MESSAGE_PREFIX L"Ref is not local.");
			auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
			CHECK_ERROR(index != -1, ERROR_MESSAGE_PREFIX L"Object not registered.");
			auto props = localObjectProperties.Values().Get(index);
			CHECK_ERROR(props->interestedClients.Contains(remoteClientId), ERROR_MESSAGE_PREFIX L"Client does not hold this object.");
			props->interestedClients.Remove(remoteClientId);
			if (props->interestedClients.Count() == 0)
			{
				RemoveLocalObject(ref, true);
			}
#undef ERROR_MESSAGE_PREFIX
		}

		void RpcLifecycleBase::RegisterLocalService(vint typeId, Ptr<IDescriptable> service)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcLifecycleBase::RegisterLocalService(vint, Ptr<IDescriptable>)#"
			CHECK_ERROR(service, ERROR_MESSAGE_PREFIX L"Service is required.");
			if (initialized)
			{
				throw Exception(ERROR_MESSAGE_PREFIX L"RegisterLocalService cannot be called after Initialize.");
			}
			if (registeredLocalServices.Keys().Contains(typeId))
			{
				throw Exception(ERROR_MESSAGE_PREFIX L"Service is already registered.");
			}

			auto ref = PtrToRef(service);
			LocalObjectHold(ref, clientId);
			registeredLocalServices.Set(typeId, service);
			GetDispatcher()->DeclareLocalService(typeId, clientId);
#undef ERROR_MESSAGE_PREFIX
		}

		void RpcLifecycleBase::DeclareRemoteService(vint typeId, vint remoteClientId)
		{
			registeredRemoteServices.Set(typeId, remoteClientId);
		}

		Ptr<IDescriptable> RpcLifecycleBase::RequestService(WString typeName)
		{
			auto typeIndex = idMap.Keys().IndexOf(typeName);
			if (typeIndex == -1)
			{
				return nullptr;
			}
			auto typeId = idMap.Values()[typeIndex];

			if (auto index = registeredLocalServices.Keys().IndexOf(typeId); index != -1)
			{
				return registeredLocalServices.Values()[index];
			}

			if (registeredRemoteServices.Keys().Contains(typeId))
			{
				auto ref = GetDispatcher()->RequestService(typeId);
				return RefToPtr(ref);
			}

			return nullptr;
		}

		Ptr<IDescriptable> RpcLifecycleBase::RefToPtr(RpcObjectReference ref)
		{
			return RefToPtr(ref, serializer.Obj());
		}

		Ptr<IDescriptable> RpcLifecycleBase::RefToPtr(RpcObjectReference ref, IRpcSerializer* serializer)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcLifecycleBase::RefToPtr(RpcObjectReference)#"
			if (ref.clientId == clientId)
			{
				auto index = localObjectProperties.Keys().IndexOf(ref.objectId);
				CHECK_ERROR(index != -1, ERROR_MESSAGE_PREFIX L"Object not registered.");
				auto props = localObjectProperties.Values().Get(index);
				CHECK_ERROR(props->rawPtr != nullptr, ERROR_MESSAGE_PREFIX L"Object not tracked.");
				return Ptr(props->rawPtr);
			}
			else
			{
				if (auto index = wrapperProperties.Keys().IndexOf(ref); index != -1)
				{
					auto proxy = wrapperProperties.Values()[index].proxy;
					auto descriptable = dynamic_cast<IDescriptable*>(proxy);
					CHECK_ERROR(descriptable, ERROR_MESSAGE_PREFIX L"Wrapper does not implement IDescriptable.");
					return Ptr(descriptable);
				}

				return CreateCallerProxy(ref, serializer);
			}
#undef ERROR_MESSAGE_PREFIX
		}

		RpcObjectReference RpcLifecycleBase::PtrToRef(Ptr<IDescriptable> obj)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcLifecycleBase::PtrToRef(Ptr<reflection::IDescriptable>)#"
			CHECK_ERROR(obj, ERROR_MESSAGE_PREFIX L"A value is required.");

			if (auto descObj = dynamic_cast<DescriptableObject*>(obj.Obj()))
			{
				RpcObjectReference wrapperRef;
				if (TryGetTrackedWrapperRef(descObj, wrapperRef))
				{
					return wrapperRef;
				}

				if (IsRpcWrapperObject(descObj))
				{
					CHECK_FAIL(ERROR_MESSAGE_PREFIX L"Wrapper tracker lookup unexpectedly failed.");
				}
			}

			if (auto descObj = dynamic_cast<DescriptableObject*>(obj.Obj()))
			{
				if (auto trackerObj = descObj->GetInternalProperty(InternalProperty_LocalObjectTracker))
				{
					auto tracker = trackerObj.Cast<RpcLocalObjectTracker>();
					CHECK_ERROR(tracker, ERROR_MESSAGE_PREFIX L"Invalid internal property type.");
					if (tracker->GetClientId() != clientId)
					{
						throw Exception(ERROR_MESSAGE_PREFIX L"Object registered to a different lifecycle.");
					}
					if (tracker->IsTracked())
					{
						CHECK_ERROR(tracker->GetLifecycle() == this, ERROR_MESSAGE_PREFIX L"Object tracker registered to an invalid lifecycle instance.");
						return tracker->GetRef();
					}
				}
			}

			auto typeId = DecideTypeId(obj.Obj());
			CHECK_ERROR(typeId != RpcTypeId_NotFound, ERROR_MESSAGE_PREFIX L"DecideTypeId returned RpcTypeId_NotFound (unknown type).");
			auto ref = RpcObjectReference{ clientId, ++nextObjectId, typeId };
			CHECK_ERROR(!localObjectProperties.Keys().Contains(ref.objectId), ERROR_MESSAGE_PREFIX L"Object ID already registered.");
			auto props = Ptr(new RpcLocalObjectProperties);
			props->ref = ref;
			props->ownedPtr = obj;
			localObjectProperties.Set(ref.objectId, props);

			TrackLocalObject(ref, obj.Obj());
			return ref;
#undef ERROR_MESSAGE_PREFIX
		}
	}
}
