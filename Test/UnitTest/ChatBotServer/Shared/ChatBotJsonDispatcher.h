#ifndef VCZH_WORKFLOW_TEST_CHATBOTJSONDISPATCHER
#define VCZH_WORKFLOW_TEST_CHATBOTJSONDISPATCHER

#include "ChatBotApp.h"
#include <VlppOS.h>

namespace chatbot
{
	using JsonPackage				= vl::Ptr<vl::glr::json::JsonNode>;
	using JsonChannel				= vl::inter_process::IChannel<JsonPackage>;
	using JsonChannelClient			= vl::inter_process::IChannelClient<JsonPackage>;
	using JsonChannelServer			= vl::inter_process::IChannelServer<JsonPackage>;
	using JsonNetworkChannelClient	= vl::inter_process::NetworkProtocolChannelClient<JsonPackage, vl::glr::json::JsonNodeListSerializer>;
	using JsonLocalChannelClient	= vl::inter_process::NetworkProtocolLocalChannelClient<JsonPackage, vl::glr::json::JsonNodeListSerializer>;
	using JsonNetworkChannelServer	= vl::inter_process::NetworkProtocolChannelServer<JsonPackage, vl::glr::json::JsonNodeListSerializer>;

	constexpr const wchar_t* RpcChannel			= L"RpcChannel";
	constexpr const wchar_t* ChatBotHttpBaseUrl	= L"/WorkflowChatBot";
	constexpr vl::vint ChatBotHttpPort			= 28763;

	vl::Ptr<vl::glr::json::Parser> CreateChatBotJsonParser();

	class TaskQueue : public vl::Object
	{
	private:
		vl::SpinLock							lockTasks;
		vl::Semaphore							semaphoreTasks;
		vl::collections::List<vl::Func<void()>>	tasks;
		bool									exitTaskQueued = false;

	public:
		TaskQueue();
		~TaskQueue();

		void									QueueTask(vl::Func<void()> task);
		void									QueueExitTask();
		void									RunTaskQueue();
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
		JsonChannel* rpcChannel = nullptr;
		vl::atomic_vint nextRequestId = 0;
		vl::atomic_vint activeJsonRequests = 0;

		vl::SpinLock lockMessages;
		vl::Semaphore semaphoreMessages;
		vl::collections::List<ReceivedJsonMessage> messages;
		vl::collections::List<ReceivedJsonMessage> bufferedResponses;

	protected:
		vl::Ptr<vl::rpc_controller::RpcJsonDispatcher> rpcDispatcher;
		vl::Ptr<vl::rpc_controller::RpcJsonLifecycle> lifecycle;
		vl::Ptr<::rpcops_IOps_ChatBotApp> ops;

		static vl::vint GetChatServerTypeId();

		JsonChannel* GetRpcChannel();
		vl::vint GetLocalClientId();

		void InitializeRpc(vl::vint clientId);
		void PushReceivedMessage(vl::vint senderClientId, JsonPackage message);
		ReceivedJsonMessage PopReceivedMessage();
		bool TryPopBufferedResponse(vl::vint requestId, ReceivedJsonMessage& message);
		void PushBufferedResponse(ReceivedJsonMessage message);
		void SendJsonResponse(vl::vint receiverClientId, JsonPackage response);
		void FlushChannel();

		virtual void SchedulaTask(vl::Func<void()> task) = 0;
		virtual void SendJsonRequest(JsonPackage message, bool broadcast) = 0;
		virtual bool HandleIncomingRequest(vl::vint senderClientId, JsonPackage request);
		virtual bool HandleUnmatchedResponse(vl::vint senderClientId, JsonPackage response);
		virtual void AfterRequestResponse(vl::vint senderClientId, JsonPackage request);

		void ProcessRequestAndSendResponse(vl::vint senderClientId, JsonPackage request);
		JsonPackage TranslateRequest(JsonPackage request);

	public:
		ChatBotJsonDispatcherBase(JsonChannel* channel);

		vl::vint AllocateRequestId() override;
		JsonPackage OnJsonRequest(JsonPackage message, bool broadcast) override;
		void OnRead(vl::vint senderClientId, const JsonPackage& package) override;

		void FinalizeRpc();
	};

	class ChatBotJsonDispatcherClient : public ChatBotJsonDispatcherBase
	{
	private:
		vl::vint serverLocalClientId = -1;
		vl::Ptr<chatapi::IChatServer> chatServer;

	protected:
		void SendJsonRequest(JsonPackage message, bool broadcast) override;

	public:
		ChatBotJsonDispatcherClient(JsonChannel* channel);

		void LoginClient(vl::vint clientId);
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
		vl::Ptr<chatapi::IChatServer> service;
		vl::vint localClientId = -1;

		// covers localClientId and pendingLoginClientIds
		vl::SpinLock lockClients;
		vl::collections::List<vl::vint> pendingLoginClientIds;

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
		ChatBotJsonDispatcherServer(JsonChannel* channel, vl::Ptr<chatapi::IChatServer> chatServerService);

		void RegisterLocalClient(vl::vint clientId);
		void RegisterClient(vl::vint clientId);
		void DisconnectClient(vl::vint clientId);
		vl::vint GetServerClientId();
	};
}

#endif
