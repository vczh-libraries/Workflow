#ifndef VCZH_WORKFLOW_TEST_CHATBOTHTTP
#define VCZH_WORKFLOW_TEST_CHATBOTHTTP

#include "ChatBotApp.h"
#include <VlppOS.h>

namespace chatbot
{
	using JsonPackage				= vl::Ptr<vl::glr::json::JsonNode>;
	using JsonChannel				= vl::inter_process::IChannel<JsonPackage>;
	using JsonChannelClient			= vl::inter_process::IChannelClient<JsonPackage>;
	using JsonChannelServer			= vl::inter_process::IChannelServer<JsonPackage>;
	using JsonNetworkChannelClient	= vl::inter_process::NetworkProtocolChannelClient<JsonPackage, vl::glr::json::JsonNodeListSerializer>;
	using JsonLocalChannelClient		= vl::inter_process::NetworkProtocolLocalChannelClient<JsonPackage, vl::glr::json::JsonNodeListSerializer>;
	using JsonNetworkChannelServer	= vl::inter_process::NetworkProtocolChannelServer<JsonPackage, vl::glr::json::JsonNodeListSerializer>;
	using TaskQueue					= vl::TaskQueue;

	constexpr const wchar_t* RpcChannel			= L"RpcChannel";
	constexpr const wchar_t* ChatBotHttpBaseUrl	= L"/WorkflowChatBot";
	constexpr vl::vint ChatBotHttpPort			= 28763;

	inline vl::Ptr<vl::glr::json::Parser> CreateChatBotJsonParser()
	{
		return vl::Ptr(new vl::glr::json::Parser);
	}
}

#endif
