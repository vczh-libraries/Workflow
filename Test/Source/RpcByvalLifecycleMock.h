#ifndef VCZH_WORKFLOW_TEST_RPC_BYVAL_LIFECYCLE_MOCK
#define VCZH_WORKFLOW_TEST_RPC_BYVAL_LIFECYCLE_MOCK

#include "RpcLifecycleMock.h"

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

		class RpcByvalLifecycleMock : public RpcLifecycleMock
		{
			friend class RpcObjectTracker;
		private:
			vint																																		clientId = 1;
			vint																																		nextObjectId = 1;
			rpc_controller::RpcObjectReference																											lastRegisteredObject;
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
			bool																	IsTracked(vint objectId)const;
		public:
			RpcByvalLifecycleMock();

			rpc_controller::RpcObjectReference										GetLastRegisteredObject()const;
			Ptr<reflection::IDescriptable>											CreateCallerProxy(rpc_controller::RpcObjectReference ref);

			// IRpcController

			rpc_controller::RpcObjectReference										RegisterLocalObject(vint typeId)override;
			void																	UnregisterLocalObject(rpc_controller::RpcObjectReference ref)override;
			void																	AcquireRemoteObject(rpc_controller::RpcObjectReference ref)override;
			void																	ReleaseRemoteObject(rpc_controller::RpcObjectReference ref)override;

			// IRpcLifeCycle

			Ptr<reflection::IDescriptable>											RefToPtr(rpc_controller::RpcObjectReference ref)override;
			rpc_controller::RpcObjectReference										PtrToRef(Ptr<reflection::IDescriptable> obj)override;

			// IRpcListEventOps

			void																	OnItemChanged(rpc_controller::RpcObjectReference ref, vint index, vint oldCount, vint newCount)override;
		};
	}
}

#endif
