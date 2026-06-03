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
			virtual Ptr<glr::json::JsonNode>		OnJsonRequest(Ptr<glr::json::JsonNode> message) = 0;
		};

		class RpcJsonObjectOps : public Object, public IRpcObjectOps
		{
		public:
			RpcJsonObjectOps(IRpcJsonMessageDispatcher* _dispatcher, IRpcSerializer* _serializer);
			~RpcJsonObjectOps();

			static Ptr<glr::json::JsonNode>			Translate(Ptr<glr::json::JsonNode> message, IRpcObjectOps* ops);
		};

		class RpcJsonObjectEventOps : public Object, public IRpcObjectEventOps
		{
		public:
			RpcJsonObjectEventOps(IRpcJsonMessageDispatcher* _dispatcher, IRpcSerializer* _serializer);
			~RpcJsonObjectEventOps();

			static Ptr<glr::json::JsonNode>			Translate(Ptr<glr::json::JsonNode> message, IRpcObjectEventOps* ops);
		};

		extern vint									ReadRequestId(Ptr<glr::json::JsonNode> message);
		extern void									WriteRequestId(Ptr<glr::json::JsonNode> message, vint requestId);
	}
}

#endif
