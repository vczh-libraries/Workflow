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
		extern reflection::description::Value			JsonDeserializePredefinedTypes(const reflection::description::Value& value, const RpcJsonDeserializeCallback& rpcjson_Deserialize);
	}
}

#endif
