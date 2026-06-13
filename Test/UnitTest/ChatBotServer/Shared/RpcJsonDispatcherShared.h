#ifndef VCZH_WORKFLOW_TEST_RPCJSONDISPATCHERSHARED
#define VCZH_WORKFLOW_TEST_RPCJSONDISPATCHERSHARED

#include "../../../../Source/Library/Rpc/WfLibraryRpcJson.h"
#include <VlppOS.h>

namespace vl::rpc_controller::channeling
{
	using JsonPackage				= vl::Ptr<vl::glr::json::JsonNode>;
	using JsonChannel				= vl::inter_process::IChannel<JsonPackage>;
	using JsonChannelClient			= vl::inter_process::IChannelClient<JsonPackage>;
	using JsonChannelServer			= vl::inter_process::IChannelServer<JsonPackage>;
	using JsonNetworkChannelClient	= vl::inter_process::NetworkProtocolChannelClient<JsonPackage, vl::glr::json::JsonNodeListSerializer>;
	using JsonLocalChannelClient	= vl::inter_process::NetworkProtocolLocalChannelClient<JsonPackage, vl::glr::json::JsonNodeListSerializer>;
	using JsonNetworkChannelServer	= vl::inter_process::NetworkProtocolChannelServer<JsonPackage, vl::glr::json::JsonNodeListSerializer>;
	using TaskQueue					= vl::TaskQueue;
}

#endif
