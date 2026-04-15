#ifndef VCZH_WORKFLOW_TEST_RPC_BYVAL_LIFECYCLE_MOCK
#define VCZH_WORKFLOW_TEST_RPC_BYVAL_LIFECYCLE_MOCK

#include "../../Source/Library/WfLibraryRpc.h"

namespace vl
{
	namespace rpc_controller_test
	{
		class RpcObjectTracker : public Object
		{
		private:
			class RpcByvalLifecycleMock*										mock;
			rpc_controller::RpcObjectReference									ref;
		public:
			RpcObjectTracker(class RpcByvalLifecycleMock* m, rpc_controller::RpcObjectReference r);
			~RpcObjectTracker();
		};

		class RpcByvalLifecycleMock : public Object, public rpc_controller::IRpcLifeCycle, public rpc_controller::IRpcController
		{
			friend class RpcObjectTracker;
		private:
			vint																																		clientId = 1;
			vint																																		nextObjectId = 1;
			rpc_controller::RpcObjectReference																											lastRegisteredObject;
			IRpcObjectOps*																																objectCallback = nullptr;
			IRpcObjectEventOps*																															eventCallback = nullptr;
			IRpcListOps*																																listCallback = nullptr;
			IRpcListEventOps*																															listEventCallback = nullptr;
			collections::Dictionary<vint, vint>																											refCounts;
			collections::Dictionary<vint, vint>																											typeIds;
			collections::Dictionary<vint, reflection::IDescriptable*>																					localObjects;
			collections::Dictionary<vint, Ptr<reflection::IDescriptable>>																				ownedObjects;
			collections::Dictionary<const reflection::IDescriptable*, rpc_controller::RpcObjectReference>												refsByPtr;
			collections::Dictionary<vint, rpc_controller::RpcObjectReference>																			refsById;
			collections::Dictionary<vint, Func<Ptr<reflection::IDescriptable>(rpc_controller::IRpcLifeCycle*, rpc_controller::RpcObjectReference)>>		proxyFactories;
			collections::Dictionary<vint, reflection::description::IValueObservableList*>																observableProxies;
			collections::Dictionary<vint, Ptr<EventHandler>>																							eventHandlers;

			vint																	DecideTypeId(reflection::IDescriptable* obj)const;
			void																	TrackLocalObject(rpc_controller::RpcObjectReference ref, reflection::IDescriptable* obj);
			void																	UntrackLocalObject(rpc_controller::RpcObjectReference ref);
			IRpcListOps*															RequireListCallback()const;
			bool																	IsTracked(vint objectId)const;
		public:
			RpcByvalLifecycleMock();

			rpc_controller::RpcObjectReference										GetLastRegisteredObject()const;
			Ptr<reflection::IDescriptable>											CreateCallerProxy(rpc_controller::RpcObjectReference ref);

			// IRpcIdSync

			void																	SyncIds(Ptr<reflection::description::IValueDictionary> ids)override;

			// IRpcController

			Ptr<reflection::description::IValueDictionary>							Register(Ptr<IRpcObjectOps> objectCallback, Ptr<IRpcObjectEventOps> eventCallback, Ptr<IRpcListOps> listCallback, Ptr<IRpcListEventOps> listEventCallback)override;
			rpc_controller::RpcObjectReference										RegisterLocalObject(vint typeId)override;
			void																	UnregisterLocalObject(rpc_controller::RpcObjectReference ref)override;
			void																	AcquireRemoteObject(rpc_controller::RpcObjectReference ref)override;
			void																	ReleaseRemoteObject(rpc_controller::RpcObjectReference ref)override;

			// IRpcLifeCycle

			Ptr<rpc_controller::IRpcController>										GetController()const override;
			Ptr<reflection::IDescriptable>											RefToPtr(rpc_controller::RpcObjectReference ref)override;
			rpc_controller::RpcObjectReference										PtrToRef(Ptr<reflection::IDescriptable> obj)override;

			// IRpcObjectOps

			reflection::description::Value											InvokeMethod(rpc_controller::RpcObjectReference ref, vint methodId, Ptr<reflection::description::IValueArray> arguments)override;
			Ptr<reflection::description::IAsync>									InvokeMethodAsync(rpc_controller::RpcObjectReference ref, vint methodId, Ptr<reflection::description::IValueArray> arguments)override;
			void																	ObjectHold(rpc_controller::RpcObjectReference ref, bool hold)override;
			rpc_controller::RpcObjectReference										RequestService(vint typeId)override;

			// IRpcObjectEventOps

			void																	InvokeEvent(rpc_controller::RpcObjectReference ref, vint eventId, Ptr<reflection::description::IValueArray> arguments)override;

			// IRpcListOps

			rpc_controller::RpcObjectReference										EnumCreate(rpc_controller::RpcObjectReference ref)override;
			bool																	EnumNext(rpc_controller::RpcObjectReference enumerator)override;
			reflection::description::Value											EnumGetCurrent(rpc_controller::RpcObjectReference enumerator)override;
			vint																	ListGetCount(rpc_controller::RpcObjectReference ref)override;
			reflection::description::Value											ListGet(rpc_controller::RpcObjectReference ref, vint index)override;
			void																	ListSet(rpc_controller::RpcObjectReference ref, vint index, const reflection::description::Value& value)override;
			vint																	ListAdd(rpc_controller::RpcObjectReference ref, const reflection::description::Value& value)override;
			vint																	ListInsert(rpc_controller::RpcObjectReference ref, vint index, const reflection::description::Value& value)override;
			bool																	ListRemoveAt(rpc_controller::RpcObjectReference ref, vint index)override;
			void																	ListClear(rpc_controller::RpcObjectReference ref)override;
			bool																	ListContains(rpc_controller::RpcObjectReference ref, const reflection::description::Value& value)override;
			vint																	ListIndexOf(rpc_controller::RpcObjectReference ref, const reflection::description::Value& value)override;
			vint																	DictGetCount(rpc_controller::RpcObjectReference ref)override;
			reflection::description::Value											DictGet(rpc_controller::RpcObjectReference ref, const reflection::description::Value& key)override;
			void																	DictSet(rpc_controller::RpcObjectReference ref, const reflection::description::Value& key, const reflection::description::Value& value)override;
			bool																	DictRemove(rpc_controller::RpcObjectReference ref, const reflection::description::Value& key)override;
			void																	DictClear(rpc_controller::RpcObjectReference ref)override;
			bool																	DictContainsKey(rpc_controller::RpcObjectReference ref, const reflection::description::Value& key)override;
			Ptr<reflection::description::IValueArray>								DictGetKeys(rpc_controller::RpcObjectReference ref)override;
			Ptr<reflection::description::IValueArray>								DictGetValues(rpc_controller::RpcObjectReference ref)override;

			// IRpcListEventOps

			void																	OnItemChanged(rpc_controller::RpcObjectReference ref, vint index, vint oldCount, vint newCount)override;
		};
	}
}

#endif
