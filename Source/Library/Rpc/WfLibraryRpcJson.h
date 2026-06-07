/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::RPC

JSON Helpers:
***********************************************************************/

#ifndef VCZH_WORKFLOW_LIBRARY_RPC_JSON
#define VCZH_WORKFLOW_LIBRARY_RPC_JSON

#include "WfLibraryRpc.h"
#include <VlppGlrParser.h>

namespace vl
{
	namespace rpc_controller
	{
		using RpcJsonSerializeCallback = Func<Ptr<glr::json::JsonNode>(const reflection::description::Value&)>;
		using RpcJsonDeserializeCallback = Func<reflection::description::Value(Ptr<glr::json::JsonNode>)>;

		extern Ptr<glr::json::JsonNode>				JsonSerializePredefinedTypes(const reflection::description::Value& value, const RpcJsonSerializeCallback& rpcjson_Serialize);
		extern reflection::description::Value		JsonDeserializePredefinedTypes(const reflection::description::Value& value, const RpcJsonDeserializeCallback& rpcjson_Deserialize);

		class IRpcJsonMessageDispatcher
			: public virtual reflection::IDescriptable
			, public reflection::Description<IRpcJsonMessageDispatcher>
		{
		public:
			virtual vint							AllocateRequestId() = 0;
			virtual Ptr<glr::json::JsonNode>		OnJsonRequest(Ptr<glr::json::JsonNode> message, bool broadcast) = 0;
		};

		class RpcJsonObjectOps : public Object, public IRpcObjectOps
		{
		private:
			vint									sourceClientId = RpcClientId_Invalid;
			vint									targetClientId = RpcClientId_Invalid;
			IRpcJsonMessageDispatcher*				dispatcher = nullptr;

		public:
			RpcJsonObjectOps(IRpcJsonMessageDispatcher* _dispatcher);
			RpcJsonObjectOps(vint _sourceClientId, vint _targetClientId, IRpcJsonMessageDispatcher* _dispatcher);
			~RpcJsonObjectOps();

			reflection::description::Value			InvokeMethod(RpcObjectReference ref, vint methodId, Ptr<reflection::description::IValueArray> arguments)override;
			void									EndInvokeMethod(vint slot)override;
			void									ObjectHold(RpcObjectReference ref, vint remoteClientId, bool hold)override;

			static Ptr<glr::json::JsonNode>			Translate(Ptr<glr::json::JsonNode> message, IRpcObjectOps* ops, IRpcLifecycle* lifecycle = nullptr);
		};

		class RpcJsonObjectEventOps : public Object, public IRpcObjectEventOps
		{
		private:
			vint									sourceClientId = RpcClientId_Invalid;
			IRpcJsonMessageDispatcher*				dispatcher = nullptr;

		public:
			RpcJsonObjectEventOps(IRpcJsonMessageDispatcher* _dispatcher);
			RpcJsonObjectEventOps(vint _sourceClientId, IRpcJsonMessageDispatcher* _dispatcher);
			~RpcJsonObjectEventOps();

			reflection::description::Value			InvokeEvent(RpcObjectReference ref, vint eventId, Ptr<reflection::description::IValueArray> arguments)override;

			static Ptr<glr::json::JsonNode>			Translate(Ptr<glr::json::JsonNode> message, IRpcObjectEventOps* ops);
		};

		extern vint									ReadRequestId(Ptr<glr::json::JsonNode> message);
		extern void									WriteRequestId(Ptr<glr::json::JsonNode> message, vint requestId);
	}
}

#endif
