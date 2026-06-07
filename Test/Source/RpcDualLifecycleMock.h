#ifndef VCZH_WORKFLOW_TEST_RPC_DUAL_LIFECYCLE_MOCK
#define VCZH_WORKFLOW_TEST_RPC_DUAL_LIFECYCLE_MOCK

#include "../../Source/Library/Rpc/WfLibraryRpcLifecycle.h"

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

			RpcDualLifecycleMock*													GetLifecycle(vint clientId)const;
			RpcDualLifecycleMock*													GetOtherLifecycle(vint clientId)const;
		public:
			RpcDualDispatcherMockBase(RpcDualLifecycleMock* lc1, RpcDualLifecycleMock* lc2);
			~RpcDualDispatcherMockBase();

			void																	Finalize()override;
			void																	Initialize()override;
			void																	DeclareLocalService(vint typeId, vint clientId)override;
			rpc_controller::RpcObjectReference										RequestService(vint typeId)override;
			rpc_controller::IRpcObjectEventOps*										BroadcastFromClient_ObjectEventOps(vint selfClientId)override;
			rpc_controller::IRpcObjectOps*											SendToClient_ObjectOps(vint targetClientId)override;
		};

		class RpcDualDispatcherMock : public RpcDualDispatcherMockBase
		{
		public:
			RpcDualDispatcherMock(RpcDualLifecycleMock* lc1, RpcDualLifecycleMock* lc2);
		};

		class RpcDualLifecycleMock : public rpc_controller::RpcLifecycleBase
		{
		private:
			rpc_controller::IRpcDispatcher*											dispatcher = nullptr;
		protected:
			void																	AttachLocalObjectEvents(rpc_controller::RpcObjectReference ref, reflection::IDescriptable* obj)override;
		public:
			using rpc_controller::RpcLifecycleBase::RpcLifecycleBase;

			rpc_controller::IRpcDispatcher*											GetDispatcher()override;
			void																	SetDispatcher(rpc_controller::IRpcDispatcher* _dispatcher);
		};
	}
}

#endif
