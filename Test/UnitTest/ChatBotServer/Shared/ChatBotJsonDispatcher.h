#ifndef VCZH_WORKFLOW_TEST_CHATBOTJSONDISPATCHER
#define VCZH_WORKFLOW_TEST_CHATBOTJSONDISPATCHER

#include "ChatBotApp.h"
#include <VlppOS.h>

namespace chatbot
{
	using JsonPackage = vl::Ptr<vl::glr::json::JsonNode>;
	using JsonChannel = vl::inter_process::IChannel<JsonPackage>;
	using JsonChannelClient = vl::inter_process::IChannelClient<JsonPackage>;
	using JsonChannelServer = vl::inter_process::IChannelServer<JsonPackage>;
	using JsonNetworkChannelClient = vl::inter_process::NetworkProtocolChannelClient<JsonPackage, vl::glr::json::JsonNodeListSerializer>;
	using JsonLocalChannelClient = vl::inter_process::NetworkProtocolLocalChannelClient<JsonPackage, vl::glr::json::JsonNodeListSerializer>;
	using JsonNetworkChannelServer = vl::inter_process::NetworkProtocolChannelServer<JsonPackage, vl::glr::json::JsonNodeListSerializer>;

	constexpr const wchar_t* RpcChannel = L"RpcChannel";
	constexpr const wchar_t* ChatBotHttpBaseUrl = L"/WorkflowChatBot";
	constexpr vl::vint ChatBotHttpPort = 28763;

	vl::Ptr<vl::glr::json::Parser> CreateChatBotJsonParser();

	class ChatBotJsonDispatcherServer;

	class ChatBotChannelClient : public JsonNetworkChannelClient
	{
	private:
		JsonChannelClient::ChannelMap channelNames;

	public:
		ChatBotChannelClient(vl::Ptr<vl::inter_process::INetworkProtocolClient> client, vl::Ptr<vl::glr::json::Parser> parser);

		const JsonChannelClient::ChannelNameList& OnGetChannelNames() override;
		void OnConnected(vl::vint clientId) override;
		void OnDisconnected() override;
		void OnReadError(const vl::WString& errorMessage) override;
		void OnLocalError(const vl::WString& errorMessage, bool fatal) override;
	};

	class ChatBotLocalChannelClient : public JsonLocalChannelClient
	{
	private:
		JsonChannelClient::ChannelMap channelNames;

	public:
		ChatBotLocalChannelClient(vl::Ptr<vl::glr::json::Parser> parser);

		const JsonChannelClient::ChannelNameList& OnGetChannelNames() override;
		void OnConnected(vl::vint clientId) override;
		void OnDisconnected() override;
		void OnReadError(const vl::WString& errorMessage) override;
		void OnLocalError(const vl::WString& errorMessage, bool fatal) override;
	};

	class ChatBotChannelServer : public JsonNetworkChannelServer
	{
	private:
		ChatBotJsonDispatcherServer* dispatcher = nullptr;

	public:
		ChatBotChannelServer(vl::Ptr<vl::glr::json::Parser> parser);

		void SetDispatcher(ChatBotJsonDispatcherServer* value);
		vl::inter_process::WaitForClientResult OnClientConnected(vl::vint clientId, const JsonChannelClient::ChannelNameList& availableChannels) override;
		void OnClientDisconnected(vl::vint clientId) override;
	};

	class ChatBotJsonDispatcherBase
		: public vl::Object
		, public virtual vl::rpc_controller::IRpcJsonMessageDispatcher
		, public virtual vl::inter_process::IChannelReader<JsonPackage>
	{
	protected:
		struct ReceivedJsonMessage
		{
			vl::vint senderClientId = -1;
			JsonPackage message;
		};

	private:
		vl::Ptr<vl::glr::json::Parser> parser;
		JsonChannel* rpcChannel = nullptr;
		vl::atomic_vint nextRequestId = 0;
		vl::atomic_vint activeJsonRequests = 0;

		vl::SpinLock lockMessages;
		vl::Semaphore semaphoreMessages;
		vl::collections::List<ReceivedJsonMessage> messages;

		vl::SpinLock lockTasks;
		vl::Semaphore semaphoreTasks;
		vl::collections::List<vl::Func<void()>> tasks;
		bool exitTaskQueued = false;

	protected:
		vl::Ptr<vl::rpc_controller::RpcJsonDispatcher> rpcDispatcher;
		vl::Ptr<vl::rpc_controller::RpcJsonLifecycle> lifecycle;
		vl::Ptr<::rpcops_IOps_ChatBotApp> ops;

		static vl::vint GetChatServerTypeId();
		static JsonChannel* GetRpcChannel(vl::Ptr<JsonChannelClient> client);

		vl::Ptr<vl::glr::json::Parser> GetParser();
		JsonChannel* GetRpcChannel();
		vl::vint GetLocalClientId();

		void InitializeChannel(JsonChannel* channel);
		void InitializeRpc(vl::vint clientId);
		void PushReceivedMessage(vl::vint senderClientId, JsonPackage message);
		ReceivedJsonMessage PopReceivedMessage();
		void SendJsonResponse(vl::vint receiverClientId, JsonPackage response);
		void FlushChannel();

		virtual void SendJsonRequest(JsonPackage message, bool broadcast) = 0;
		virtual bool HandleIncomingRequest(vl::vint senderClientId, JsonPackage request);
		virtual bool HandleUnmatchedResponse(vl::vint senderClientId, JsonPackage response);
		virtual void AfterRequestResponse(vl::vint senderClientId, JsonPackage request);

		void ProcessRequestAndSendResponse(vl::vint senderClientId, JsonPackage request);
		JsonPackage TranslateRequest(JsonPackage request);

	public:
		ChatBotJsonDispatcherBase();

		vl::vint AllocateRequestId() override;
		JsonPackage OnJsonRequest(JsonPackage message, bool broadcast) override;
		void OnRead(vl::vint senderClientId, const JsonPackage& package) override;

		void QueueTask(vl::Func<void()> task);
		void QueueExitTask();
		void RunTaskQueue();
		void FinalizeRpc();
	};

	class ChatBotJsonDispatcherClient : public ChatBotJsonDispatcherBase
	{
	private:
		vl::Ptr<JsonChannelClient> channelClient;
		vl::vint serverLocalClientId = -1;
		vl::Ptr<chatapi::IChatServer> chatServer;

	protected:
		void SendJsonRequest(JsonPackage message, bool broadcast) override;

	public:
		void LoginClient(vl::Ptr<JsonChannelClient> client);
		vl::Ptr<chatapi::IChatServer> GetChatServer();
	};

	class ChatBotJsonDispatcherServer : public ChatBotJsonDispatcherBase
	{
	public:
		struct PendingBroadcast : public vl::Object
		{
			vl::vint originalClientId = -1;
			vl::vint originalRequestId = -1;
			vl::vint redirectedRequestId = -1;
			bool hasNonNullResponse = false;
			vl::collections::List<vl::vint> expectedClientIds;
			vl::collections::Dictionary<vl::vint, JsonPackage> responses;
		};

		struct CompletedBroadcast
		{
			vl::vint originalClientId = -1;
			JsonPackage response;
		};

	private:
		vl::Ptr<JsonChannelServer> channelServer;
		vl::Ptr<ChatBotLocalChannelClient> localClient;
		vl::Ptr<chatapi::IChatServer> service;
		vl::vint localClientId = -1;
		bool acceptingLocalClient = false;

		vl::SpinLock lockBroadcasts;
		vl::collections::SortedList<vl::vint> connectedClientIds;
		vl::collections::Dictionary<vl::WString, vl::Ptr<PendingBroadcast>> pendingBroadcasts;
		vl::collections::Dictionary<vl::vint, vl::WString> redirectedBroadcasts;

		vl::WString MakeBroadcastKey(vl::vint clientId, vl::vint requestId);
		JsonPackage CreateBroadcastResponse(vl::vint sourceClientId, vl::vint targetClientId, vl::vint requestId, vl::Ptr<PendingBroadcast> pending);
		CompletedBroadcast CompleteBroadcastLocked(const vl::WString& key);
		void DeliverCompletedBroadcast(const CompletedBroadcast& completed);
		JsonPackage StartBroadcast(vl::vint originalClientId, vl::vint originalRequestId, JsonPackage message);
		bool TryHandleBroadcastResponse(vl::vint senderClientId, JsonPackage response);
		void SendLoginMessage(vl::vint clientId);

	protected:
		void SendJsonRequest(JsonPackage message, bool broadcast) override;
		bool HandleIncomingRequest(vl::vint senderClientId, JsonPackage request) override;
		bool HandleUnmatchedResponse(vl::vint senderClientId, JsonPackage response) override;
		void AfterRequestResponse(vl::vint senderClientId, JsonPackage request) override;

	public:
		ChatBotJsonDispatcherServer(vl::Ptr<JsonChannelServer> server, vl::Ptr<chatapi::IChatServer> chatServerService);

		void Start();
		vl::inter_process::WaitForClientResult AcceptClient(vl::vint clientId, const JsonChannelClient::ChannelNameList& availableChannels);
		void DisconnectClient(vl::vint clientId);
		vl::vint GetServerClientId();
	};
}

#endif
