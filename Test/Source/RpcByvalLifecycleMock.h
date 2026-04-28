#ifndef VCZH_WORKFLOW_TEST_RPC_BYVAL_LIFECYCLE_MOCK
#define VCZH_WORKFLOW_TEST_RPC_BYVAL_LIFECYCLE_MOCK

#include "RpcControllerMock.h"

namespace vl
{
	namespace rpc_controller_test
	{
		class RpcByvalLifecycleMock;
		class RpcByvalControllerMock;

		class RpcObjectTracker : public Object
		{
		private:
			RpcByvalControllerMock*											controller;
			rpc_controller::RpcObjectReference									ref;
		public:
			RpcObjectTracker(RpcByvalControllerMock* c, rpc_controller::RpcObjectReference r);
			~RpcObjectTracker();
		};

		class RpcByvalControllerMock : public RpcControllerMock
		{
			friend class RpcByvalLifecycleMock;
			friend class RpcObjectTracker;
		private:
			RpcByvalLifecycleMock*																												lifecycle = nullptr;
			vint																																		clientId = 1;
			vint																																		nextObjectId = 1;
			rpc_controller::RpcObjectReference																											lastRegisteredObject;
			collections::Dictionary<vint, vint>																											refCounts;
			collections::Dictionary<vint, vint>																											typeIds;
			collections::Dictionary<vint, reflection::IDescriptable*>																					localObjects;
			collections::Dictionary<vint, Ptr<reflection::IDescriptable>>																				ownedObjects;
			collections::Dictionary<const reflection::IDescriptable*, rpc_controller::RpcObjectReference>												refsByPtr;
			collections::Dictionary<vint, rpc_controller::RpcObjectReference>																			refsById;
			collections::Dictionary<vint, reflection::description::IValueObservableList*>																observableProxies;
			collections::Dictionary<vint, Ptr<EventHandler>>																							eventHandlers;

			void																	TrackLocalObject(rpc_controller::RpcObjectReference ref, reflection::IDescriptable* obj);
			void																	UntrackLocalObject(rpc_controller::RpcObjectReference ref);
			bool																	IsTracked(vint objectId)const;
		public:
			RpcByvalControllerMock(RpcByvalLifecycleMock* lc);

			rpc_controller::RpcObjectReference										GetLastRegisteredObject()const;

			// IRpcController

			rpc_controller::RpcObjectReference										RegisterLocalObject(vint typeId)override;
			void																	UnregisterLocalObject(rpc_controller::RpcObjectReference ref)override;
			void																	AcquireRemoteObject(rpc_controller::RpcObjectReference ref)override;
			void																	ReleaseRemoteObject(rpc_controller::RpcObjectReference ref)override;

			// IRpcListEventOps

			void																	OnItemChanged(rpc_controller::RpcObjectReference ref, vint index, vint oldCount, vint newCount)override;
		};

		class RpcByvalLifecycleMock : public Object, public rpc_controller::IRpcLifeCycle
		{
			friend class RpcByvalControllerMock;
		private:
			RpcByvalControllerMock																												controller;
			collections::Dictionary<WString, Ptr<reflection::IDescriptable>>																		services;
			collections::Dictionary<vint, Func<Ptr<reflection::IDescriptable>(rpc_controller::IRpcLifeCycle*, rpc_controller::RpcObjectReference)>>		proxyFactories;

			vint																	DecideTypeId(reflection::IDescriptable* obj)const;
		public:
			RpcByvalLifecycleMock();
			~RpcByvalLifecycleMock();

			rpc_controller::RpcObjectReference										GetLastRegisteredObject()const;
			Ptr<reflection::IDescriptable>											CreateCallerProxy(rpc_controller::RpcObjectReference ref);

			// IRpcLifeCycle

			RpcByvalControllerMock*													GetController()override;
			void																	RegisterService(const WString& fullName, Ptr<reflection::IDescriptable> service)override;
			Ptr<reflection::IDescriptable>											RequestService(const WString& fullName)override;
			Ptr<reflection::IDescriptable>											RefToPtr(rpc_controller::RpcObjectReference ref)override;
			rpc_controller::RpcObjectReference										PtrToRef(Ptr<reflection::IDescriptable> obj)override;
		};
	}
}

#endif
