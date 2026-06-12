#ifndef VCZH_WORKFLOW_TEST_CHATBOTJSONDISPATCHERSERVER
#define VCZH_WORKFLOW_TEST_CHATBOTJSONDISPATCHERSERVER

#include "ChatBotHttp.h"

namespace chatbot
{
	class ChatBotJsonDispatcherServer
		: public vl::Object
		, public virtual vl::inter_process::IChannelReader<JsonPackage>
	{
	public:
		struct PendingBroadcast : public vl::Object
		{
			vl::vint											originalClientId = -1;
			vl::vint											originalRequestId = -1;
			vl::vint											redirectedRequestId = -1;
			bool												hasNonNullResponse = false;
			vl::collections::List<vl::vint>						expectedClientIds;
			vl::collections::Dictionary<vl::vint, JsonPackage>	responses;
		};

		struct CompletedBroadcast
		{
			vl::vint											originalClientId = -1;
			JsonPackage											response;
		};

	private:
		JsonChannel*														rpcChannel = nullptr;
		vl::atomic_vint														nextRequestId = 0;
		vl::atomic_vint														localClientId = -1;

		// covers pendingLoginClientIds
		vl::SpinLock														lockClients;
		vl::collections::List<vl::vint>										pendingLoginClientIds;

		vl::SpinLock														lockBroadcasts;
		vl::collections::SortedList<vl::vint>								connectedClientIds;
		vl::collections::Dictionary<vl::WString, vl::Ptr<PendingBroadcast>>	pendingBroadcasts;
		vl::collections::Dictionary<vl::vint, vl::WString>					redirectedBroadcasts;
		vl::collections::List<JsonPackage>									cachedServiceDeclarations;

		JsonChannel*									GetRpcChannel();
		vl::vint										AllocateRequestId();
		vl::WString										MakeBroadcastKey(vl::vint clientId, vl::vint requestId);
		JsonPackage										CreateBroadcastResponse(vl::vint sourceClientId, vl::vint targetClientId, vl::vint requestId, vl::Ptr<PendingBroadcast> pending);
		CompletedBroadcast								CompleteBroadcastLocked(const vl::WString& key);
		void											DeliverCompletedBroadcast(const CompletedBroadcast& completed);
		JsonPackage										StartBroadcast(vl::vint originalClientId, vl::vint originalRequestId, JsonPackage message);
		void											StartBroadcastAndDrop(vl::vint originalClientId, JsonPackage message);
		bool											TryHandleBroadcastResponse(vl::vint senderClientId, JsonPackage response);
		void											HandleServiceDeclaration(vl::vint senderClientId, JsonPackage request);
		void											SendLoginMessages(vl::vint clientId);
		void											SendJsonResponse(vl::vint receiverClientId, JsonPackage response);
		void											FlushChannel();

	protected:
		virtual void									SchedulaTask(vl::Func<void()> task) = 0;

	public:
		ChatBotJsonDispatcherServer(JsonChannel* channel);

		void											RegisterLocalClient(vl::vint clientId);
		void											RegisterClient(vl::vint clientId);
		void											DisconnectClient(vl::vint clientId);
		vl::vint										GetServerClientId();
		void											OnRead(vl::vint senderClientId, const JsonPackage& package) override;
	};
}

#endif
