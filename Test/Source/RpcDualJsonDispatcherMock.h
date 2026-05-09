#ifndef VCZH_WORKFLOW_TEST_RPC_DUAL_JSON_DISPATCHER_MOCK
#define VCZH_WORKFLOW_TEST_RPC_DUAL_JSON_DISPATCHER_MOCK

#include "RpcDualLifecycleMock.h"
#include "../../Source/Library/Rpc/WfLibraryRpcJson.h"

namespace vl
{
	namespace rpc_controller_test
	{
		class RpcDualJsonDispatcherMock : public RpcDualDispatcherMockBase
		{
		private:
			RpcDualLifecycleMock*									lifecycle1 = nullptr;
			RpcDualLifecycleMock*									lifecycle2 = nullptr;
			collections::List<Ptr<glr::json::JsonNode>>				jsonValues;
			Ptr<rpc_controller::IRpcListEventOps>					listEventOps1;
			Ptr<rpc_controller::IRpcListEventOps>					listEventOps2;
			Ptr<rpc_controller::IRpcObjectEventOps>					objectEventOps1;
			Ptr<rpc_controller::IRpcObjectEventOps>					objectEventOps2;
			Ptr<rpc_controller::IRpcListOps>						listOps1;
			Ptr<rpc_controller::IRpcListOps>						listOps2;
			Ptr<rpc_controller::IRpcObjectOps>						objectOps1;
			Ptr<rpc_controller::IRpcObjectOps>						objectOps2;

		public:
			RpcDualJsonDispatcherMock(RpcDualLifecycleMock* lc1, RpcDualLifecycleMock* lc2);

			void													RecordJsonValue(const reflection::description::Value& value);
			void													RecordJsonArguments(Ptr<reflection::description::IValueArray> arguments);
			collections::List<Ptr<glr::json::JsonNode>>&			JsonValues();
			void													DumpJsonValues(const WString& itemName);

			rpc_controller::IRpcListEventOps*						BroadcastFromClient_ListEventOps(vint selfClientId)override;
			rpc_controller::IRpcObjectEventOps*						BroadcastFromClient_ObjectEventOps(vint selfClientId)override;
			rpc_controller::IRpcListOps*							SendToClient_ListOps(vint targetClientId)override;
			rpc_controller::IRpcObjectOps*							SendToClient_ObjectOps(vint targetClientId)override;
		};
	}
}

#endif
