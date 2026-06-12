#ifndef VCZH_WORKFLOW_TEST_CHATBOTJSONDISPATCHERCLIENT
#define VCZH_WORKFLOW_TEST_CHATBOTJSONDISPATCHERCLIENT

#include "ChatBotHttp.h"

namespace chatbot
{
	class ChatBotJsonDispatcherClient
		: public vl::Object
		, public virtual vl::rpc_controller::IRpcJsonMessageDispatcher
		, public virtual vl::inter_process::IChannelReader<JsonPackage>
	{
	protected:
		using RequestType = vl::rpc_controller::IRpcJsonMessageDispatcher::RequestType;

		struct ReceivedJsonMessage
		{
			vl::vint									senderClientId = -1;
			JsonPackage									message;
		};

	private:
		JsonChannel*									rpcChannel = nullptr;
		vl::atomic_vint									nextRequestId = 0;
		vl::atomic_vint									activeJsonRequests = 0;
		vl::atomic_vint									initialized = 0;
		vl::atomic_vint									serverLocalClientId = -1;

		vl::SpinLock									lockMessages;
		vl::Semaphore									semaphoreMessages;
		vl::collections::List<ReceivedJsonMessage>		messages;
		vl::collections::List<ReceivedJsonMessage>		bufferedResponses;

		vl::SpinLock									lockServiceDeclarations;
		vl::collections::List<JsonPackage>				cachedIncomingServiceDeclarations;
		vl::collections::List<JsonPackage>				cachedOutgoingServiceDeclarations;

		vl::SpinLock									lockWaitingForServices;
		vl::collections::List<vl::WString>				waitingForServices;
		vl::EventObject									eventWaitingForServices;
		bool											eventWaitingForServicesCreated = false;
		vl::EventObject									eventServerLocalClientId;

	protected:
		vl::Ptr<vl::rpc_controller::RpcJsonDispatcher>	rpcDispatcher;
		vl::Ptr<vl::rpc_controller::RpcJsonLifecycle>	lifecycle;
		vl::Ptr<::rpcops_IOps_ChatBotApp>				ops;
		vl::Ptr<chatapi::IChatServer>					chatServer;

		virtual void									SchedulaTask(vl::Func<void()> task) = 0;

		JsonChannel*									GetRpcChannel();
		void											PrepareConnection(JsonChannel* channel, const vl::collections::List<vl::WString>& _waitingForServices);
		void											InitializeRpc(vl::vint clientId);
		void											ProcessCachedIncomingServiceDeclarations();
		void											SendCachedOutgoingServiceDeclarations();
		void											ProcessIncomingServiceDeclaration(JsonPackage request);
		void											UpdateWaitingForServices(JsonPackage request);
		void											WaitForServerClientId();
		void											WaitForExpectedServices();
		void											SendJsonRequest(JsonPackage message, RequestType requestType);
		void											PushReceivedMessage(vl::vint senderClientId, JsonPackage message);
		ReceivedJsonMessage								PopReceivedMessage();
		bool											TryPopBufferedResponse(vl::vint requestId, ReceivedJsonMessage& message);
		void											PushBufferedResponse(ReceivedJsonMessage message);
		void											SendJsonResponse(vl::vint receiverClientId, JsonPackage response);
		void											FlushChannel();
		void											ProcessRequestAndSendResponse(vl::vint senderClientId, JsonPackage request);
		JsonPackage										TranslateRequest(JsonPackage request);

	public:
		ChatBotJsonDispatcherClient();

		void											WaitForServer(JsonChannelClient* channelClient, JsonChannel* channel, const vl::collections::List<vl::WString>& _waitingForServices);
		vl::vint										ConnectLocalClient(JsonChannelServer* channelServer, vl::Ptr<JsonChannelClient> localClient, JsonChannel* channel, const vl::collections::List<vl::WString>& _waitingForServices);
		void											RegisterLocalService(vl::Ptr<chatapi::IChatServer> service);
		void											Initialize();
		vl::Ptr<chatapi::IChatServer>					GetChatServer();

		vl::vint										AllocateRequestId() override;
		JsonPackage										OnJsonRequest(JsonPackage message, RequestType requestType) override;
		void											OnRead(vl::vint senderClientId, const JsonPackage& package) override;

		void											FinalizeRpc();
	};
}

#endif
