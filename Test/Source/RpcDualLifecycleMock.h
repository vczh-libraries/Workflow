#ifndef VCZH_WORKFLOW_TEST_RPC_DUAL_LIFECYCLE_MOCK
#define VCZH_WORKFLOW_TEST_RPC_DUAL_LIFECYCLE_MOCK

#include "../../Source/Library/Rpc/WfLibraryRpcLifecycle.h"
#include "../../Source/Library/Rpc/WfLibraryRpcWrappers.h"

namespace vl
{
	namespace rpc_controller_test
	{
		class RpcDualLifecycleMock;
		class RpcDualDispatcherMockBase;
		class RpcDualDispatcherMock;

		class RpcDualDispatcherMockBase : public Object, public rpc_controller::IRpcDispatcher
		{
		private:
			RpcDualLifecycleMock*													lifecycle1 = nullptr;
			RpcDualLifecycleMock*													lifecycle2 = nullptr;
			Ptr<rpc_controller::IRpcObjectEventOps>									objectEventBroadcastOps1;
			Ptr<rpc_controller::IRpcObjectEventOps>									objectEventBroadcastOps2;
			collections::Dictionary<vint, rpc_controller::RpcObjectReference>		services;

		protected:
			RpcDualLifecycleMock*                                                    GetLifecycle(vint clientId)const;
			RpcDualLifecycleMock*                                                    GetOtherLifecycle(vint clientId)const;

		public:
			RpcDualDispatcherMockBase(RpcDualLifecycleMock* lc1, RpcDualLifecycleMock* lc2);
			~RpcDualDispatcherMockBase();

			void																	Finalize()override;
			bool																	IsRegisteredService(rpc_controller::RpcObjectReference ref)override;
			void																	RegisterService(vint typeId, rpc_controller::RpcObjectReference ref)override;
			rpc_controller::RpcObjectReference										RequestService(vint typeId)override;
			rpc_controller::IRpcListEventOps*										BroadcastFromClient_ListEventOps(vint selfClientId)override;
			rpc_controller::IRpcObjectEventOps*										BroadcastFromClient_ObjectEventOps(vint selfClientId)override;
			rpc_controller::IRpcListOps*											SendToClient_ListOps(vint targetClientId)override;
			rpc_controller::IRpcObjectOps*											SendToClient_ObjectOps(vint targetClientId)override;
		};

		class RpcDualDispatcherMock : public RpcDualDispatcherMockBase
		{
		private:
			RpcDualLifecycleMock*													lifecycle1 = nullptr;
			RpcDualLifecycleMock*													lifecycle2 = nullptr;
			Ptr<rpc_controller::IRpcListEventOps>									listEventOps1;
			Ptr<rpc_controller::IRpcListEventOps>									listEventOps2;
			Ptr<rpc_controller::IRpcObjectEventOps>									objectEventOps1;
			Ptr<rpc_controller::IRpcObjectEventOps>									objectEventOps2;
			Ptr<rpc_controller::IRpcListOps>										listOps1;
			Ptr<rpc_controller::IRpcListOps>										listOps2;
			Ptr<rpc_controller::IRpcObjectOps>										objectOps1;
			Ptr<rpc_controller::IRpcObjectOps>										objectOps2;

		public:
			RpcDualDispatcherMock(RpcDualLifecycleMock* lc1, RpcDualLifecycleMock* lc2);

			rpc_controller::IRpcListEventOps*										BroadcastFromClient_ListEventOps(vint selfClientId)override;
			rpc_controller::IRpcObjectEventOps*										BroadcastFromClient_ObjectEventOps(vint selfClientId)override;
			rpc_controller::IRpcListOps*											SendToClient_ListOps(vint targetClientId)override;
			rpc_controller::IRpcObjectOps*											SendToClient_ObjectOps(vint targetClientId)override;
		};

		class RpcDualLifecycleMock : public rpc_controller::RpcLifecycleBase
		{
		protected:
			void																	AttachLocalObjectEvents(rpc_controller::RpcObjectReference ref, reflection::IDescriptable* obj)override;
		public:
			using rpc_controller::RpcLifecycleBase::RpcLifecycleBase;

			void																	SetDispatcher(rpc_controller::IRpcDispatcher* _dispatcher);
		};
	}
}

#endif
