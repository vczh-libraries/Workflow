#ifndef VCZH_WORKFLOW_TEST_RPCJSONDISPATCHERCLIENT
#define VCZH_WORKFLOW_TEST_RPCJSONDISPATCHERCLIENT

#include "RpcJsonDispatcherShared.h"

namespace vl::rpc_controller::channeling
{
	/// <summary>
	/// A IChannelReader for hosting and connecting RPC services
	/// </summary>
	class RpcJsonDispatcherClient
		: public vl::Object
		, public virtual vl::rpc_controller::IRpcJsonMessageDispatcher
		, public virtual vl::inter_process::IChannelReader<JsonPackage>
	{
	protected:
		using RequestType = vl::rpc_controller::IRpcJsonMessageDispatcher::RequestType;

	private:
		struct ReceivedJsonMessage
		{
			vl::vint									senderClientId = -1;
			JsonPackage									message;
		};

		JsonChannel*									rpcChannel = nullptr;
		vl::atomic_vint									nextRequestId = 0;
		vl::atomic_vint									activeJsonRequests = 0;
		vl::atomic_vint									initialized = 0;
		vl::atomic_vint									serverLocalClientId = -1;
		vl::Ptr<vl::rpc_controller::RpcJsonDispatcher>	rpcDispatcher;
		vl::Ptr<vl::rpc_controller::RpcJsonLifecycle>	lifecycle;

		// covers messages and bufferedResponses
		vl::SpinLock									lockMessages;
		vl::Semaphore									semaphoreMessages;
		vl::collections::List<ReceivedJsonMessage>		messages;
		vl::collections::List<ReceivedJsonMessage>		bufferedResponses;

		// covers cachedIncomingServiceDeclarations and cachedOutgoingServiceDeclarations
		vl::SpinLock									lockServiceDeclarations;
		vl::collections::List<JsonPackage>				cachedIncomingServiceDeclarations;
		vl::collections::List<JsonPackage>				cachedOutgoingServiceDeclarations;

		// covers waitingForServices
		vl::SpinLock									lockWaitingForServices;
		vl::collections::List<vl::WString>				waitingForServices;
		vl::EventObject									eventWaitingForServices;
		bool											eventWaitingForServicesCreated = false;
		vl::EventObject									eventServerLocalClientId;

		void											PrepareConnection(JsonChannel* channel, const vl::collections::List<vl::WString>& _waitingForServices);
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

	protected:
		virtual void									ScheduleTask(vl::Func<void()> task) = 0;
		void											SetRpcObjects(vl::Ptr<vl::rpc_controller::RpcJsonDispatcher> _rpcDispatcher, vl::Ptr<vl::rpc_controller::RpcJsonLifecycle> _lifecycle);
		vl::rpc_controller::RpcJsonLifecycle*			GetRpcJsonLifecycle();

	public:
		RpcJsonDispatcherClient();

		void											WaitForServer(JsonChannelClient* channelClient, JsonChannel* channel, const vl::collections::List<vl::WString>& _waitingForServices);
		vl::vint										ConnectLocalClient(JsonChannelServer* channelServer, vl::Ptr<JsonChannelClient> localClient, JsonChannel* channel, const vl::collections::List<vl::WString>& _waitingForServices);
		void											Initialize();
		vl::rpc_controller::IRpcLifecycle*				GetRpcLifecycle();
		vl::rpc_controller::IRpcDispatcher*				GetRpcDispatcher();
		void											SetServerLocalClientId(vl::vint clientId);
		void											NotifyServerClientDisconnected();

		vl::vint										AllocateRequestId() override;
		JsonPackage										OnJsonRequest(JsonPackage message, RequestType requestType) override;
		void											OnRead(vl::vint senderClientId, const JsonPackage& package) override;

		virtual void									FinalizeRpc();
	};

	class RpcJsonDispatcherClientForTaskQueue : public RpcJsonDispatcherClient
	{
	private:
		vl::Ptr<TaskQueue>								taskQueue;

	protected:
		void											ScheduleTask(vl::Func<void()> task) override;

	public:
		RpcJsonDispatcherClientForTaskQueue(vl::Ptr<TaskQueue> _taskQueue);
	};
}

#endif
