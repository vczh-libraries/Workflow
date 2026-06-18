#ifndef VCZH_WORKFLOW_TEST_CHATBOTJSONDISPATCHERCLIENT
#define VCZH_WORKFLOW_TEST_CHATBOTJSONDISPATCHERCLIENT

#include "../../../../Source/Library/RpcJson/WfLibraryRpcJsonDispatcherClient.h"
#include "ChatBotApp.h"

namespace chatbot
{
	class ChatBotJsonDispatcherClient
		: public vl::rpc_controller::channeling::RpcJsonDispatcherClientForTaskQueue
	{
	public:
		ChatBotJsonDispatcherClient(vl::Ptr<vl::rpc_controller::channeling::TaskQueue> _taskQueue);

		void								InitializeRpc(vl::vint clientId);
	};
}

#endif
