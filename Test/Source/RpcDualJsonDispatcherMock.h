#ifndef VCZH_WORKFLOW_TEST_RPC_DUAL_JSON_DISPATCHER_MOCK
#define VCZH_WORKFLOW_TEST_RPC_DUAL_JSON_DISPATCHER_MOCK

#include "RpcDualLifecycleMock.h"
#include "../../Source/Library/Rpc/WfLibraryRpcJson.h"

namespace vl
{
	namespace rpc_controller_test
	{
		class RpcDualJsonRequestDispatcherMock;

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

		class RpcDualJsonMessageBridge : public Object
		{
		private:
			RpcDualJsonRequestDispatcherMock*							dispatcher1 = nullptr;
			RpcDualJsonRequestDispatcherMock*							dispatcher2 = nullptr;
			collections::Dictionary<vint, rpc_controller::RpcObjectReference>	services;
			collections::List<Ptr<glr::json::JsonNode>>					jsonRequests;
			vint														nextRequestId = 0;

			RpcDualJsonRequestDispatcherMock*							GetDispatcher(vint clientId)const;
			RpcDualJsonRequestDispatcherMock*							GetOtherDispatcher(RpcDualJsonRequestDispatcherMock* dispatcher)const;

		public:
			RpcDualJsonMessageBridge(RpcDualJsonRequestDispatcherMock* _dispatcher1, RpcDualJsonRequestDispatcherMock* _dispatcher2);
			~RpcDualJsonMessageBridge();

			void														Finalize();
			vint														AllocateRequestId();
			bool														IsRegisteredService(rpc_controller::RpcObjectReference ref);
			void														RegisterService(RpcDualJsonRequestDispatcherMock* dispatcher, vint typeId, rpc_controller::RpcObjectReference ref);
			rpc_controller::RpcObjectReference							RequestService(vint typeId);
			Ptr<glr::json::JsonNode>									OnJsonRequest(RpcDualJsonRequestDispatcherMock* dispatcher, Ptr<glr::json::JsonNode> message);
			void														DumpJsonRequests(const WString& itemName);
		};

		class RpcDualJsonRequestDispatcherMock
			: public Object
			, public rpc_controller::IRpcDispatcher
			, public rpc_controller::IRpcJsonMessageDispatcher
		{
		private:
			RpcDualLifecycleMock*										lifecycle = nullptr;
			RpcDualJsonMessageBridge*									bridge = nullptr;
			vint														nextRequestId = 0;
			Ptr<rpc_controller::IRpcObjectEventOps>						objectEventOps;
			Ptr<rpc_controller::IRpcObjectOps>							objectOps;

		public:
			RpcDualJsonRequestDispatcherMock(RpcDualLifecycleMock* _lifecycle);
			~RpcDualJsonRequestDispatcherMock();

			RpcDualLifecycleMock*										GetLifecycle()const;
			void														SetBridge(RpcDualJsonMessageBridge* _bridge);

			void														Finalize()override;
			bool														IsRegisteredService(rpc_controller::RpcObjectReference ref)override;
			void														RegisterService(vint typeId, rpc_controller::RpcObjectReference ref)override;
			rpc_controller::RpcObjectReference							RequestService(vint typeId)override;
			rpc_controller::IRpcObjectEventOps*							BroadcastFromClient_ObjectEventOps(vint selfClientId)override;
			rpc_controller::IRpcObjectOps*								SendToClient_ObjectOps(vint targetClientId)override;

			vint														AllocateRequestId()override;
			Ptr<glr::json::JsonNode>									OnJsonRequest(Ptr<glr::json::JsonNode> message)override;
		};
	}
}

#endif
