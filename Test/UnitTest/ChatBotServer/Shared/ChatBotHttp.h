#ifndef VCZH_WORKFLOW_TEST_CHATBOTHTTP
#define VCZH_WORKFLOW_TEST_CHATBOTHTTP

#include "ChatBotApp.h"

namespace chatbot
{
	constexpr const wchar_t* RpcChannel			= L"RpcChannel";
	constexpr const wchar_t* ChatBotHttpBaseUrl	= L"/WorkflowChatBot";
	constexpr vl::vint ChatBotHttpPort			= 28763;

	extern vl::vint mainThreadId;

	extern void									AttachChatServerEventHandlers(chatapi::IChatServer* chatServer);
}

#endif
