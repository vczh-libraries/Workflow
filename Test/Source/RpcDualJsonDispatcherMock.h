#ifndef VCZH_WORKFLOW_TEST_RPC_DUAL_JSON_DISPATCHER_MOCK
#define VCZH_WORKFLOW_TEST_RPC_DUAL_JSON_DISPATCHER_MOCK

#include "RpcDualLifecycleMock.h"
#include "../../Source/Library/Rpc/WfLibraryRpcJson.h"

namespace vl
{
	namespace rpc_controller_test
	{
/***********************************************************************
* Json Value
***********************************************************************/

		class RpcDualJsonDispatcherMock : public RpcDualDispatcherMockBase
		{
		private:
			RpcDualLifecycleMock*									lifecycle1 = nullptr;
			RpcDualLifecycleMock*									lifecycle2 = nullptr;
			collections::List<Ptr<glr::json::JsonNode>>				jsonValues;
			Ptr<rpc_controller::IRpcObjectEventOps>					objectEventOps1;
			Ptr<rpc_controller::IRpcObjectEventOps>					objectEventOps2;
			Ptr<rpc_controller::IRpcObjectOps>						objectOps1;
			Ptr<rpc_controller::IRpcObjectOps>						objectOps2;

		public:
			RpcDualJsonDispatcherMock(RpcDualLifecycleMock* lc1, RpcDualLifecycleMock* lc2);

			void													RecordJsonValue(const reflection::description::Value& value);
			void													RecordJsonArguments(Ptr<reflection::description::IValueArray> arguments);
			collections::List<Ptr<glr::json::JsonNode>>&			JsonValues();
			void													DumpJsonValues(const WString& itemName);

			rpc_controller::IRpcObjectEventOps*						BroadcastFromClient_ObjectEventOps(vint selfClientId)override;
			rpc_controller::IRpcObjectOps*							SendToClient_ObjectOps(vint targetClientId)override;
		};

/***********************************************************************
* Json Request
***********************************************************************/

		class RpcDualJsonRequestBridge : public Object, public rpc_controller::IRpcJsonMessageDispatcher
		{
		private:
			rpc_controller::IRpcLifecycle*								lifecycle1 = nullptr;
			rpc_controller::IRpcLifecycle*								lifecycle2 = nullptr;
			collections::List<Ptr<glr::json::JsonNode>>					jsonRequests;
			vint														nextRequestId = 0;

			rpc_controller::IRpcLifecycle*								GetOtherLifecycle(vint clientId)const;

		public:
			void														SetLifecycles(rpc_controller::IRpcLifecycle* lc1, rpc_controller::IRpcLifecycle* lc2);

			vint														AllocateRequestId()override;
			Ptr<glr::json::JsonNode>									OnJsonRequest(Ptr<glr::json::JsonNode> message, rpc_controller::IRpcJsonMessageDispatcher::RequestType requestType)override;
			void														DumpJsonRequests(const WString& itemName);
		};
	}
}

#endif
