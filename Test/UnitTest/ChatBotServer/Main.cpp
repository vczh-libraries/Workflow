#include "ChatBotApp.h"
#include "Shared/ChatBotHttp.h"
#include "Shared/ChatBotJsonDispatcherClient.h"
#include "../../../Source/Library/RpcJson/WfLibraryRpcJsonDispatcherServer.h"
#include <VlppOS.Windows.h>

using namespace vl;
using namespace vl::collections;
using namespace vl::console;
using namespace vl::glr::json;
using namespace vl::inter_process;
using namespace vl::inter_process::windows_http;
using namespace vl::rpc_controller;
using namespace vl::rpc_controller::channeling;
using namespace chatbot;

/***********************************************************************
* ChatServerImpl
*   implementation of IChatServer
***********************************************************************/

class ChatServerImpl : public Object, public virtual chatapi::IChatServer
{
private:
	SortedList<WString> users;

public:
	bool AddUser(const WString& name) override
	{
#define ERROR_MESSAGE_PREFIX L"ChatServerImpl::AddUser(const WString&)#"
		CHECK_ERROR(mainThreadId == Thread::GetCurrentThreadId(), ERROR_MESSAGE_PREFIX L"AddUser should be invoked on the main thread.");
#undef ERROR_MESSAGE_PREFIX

		if (users.Contains(name))
		{
			return false;
		}

		users.Add(name);
		OnUserAdded(name);
		return true;
	}

	bool RemoveUser(const WString& name) override
	{
#define ERROR_MESSAGE_PREFIX L"ChatServerImpl::RemoveUser(const WString&)#"
		CHECK_ERROR(mainThreadId == Thread::GetCurrentThreadId(), ERROR_MESSAGE_PREFIX L"RemoveUser should be invoked on the main thread.");
#undef ERROR_MESSAGE_PREFIX

		if (!users.Contains(name))
		{
			return false;
		}

		users.Remove(name);
		OnUserRemoved(name);
		return true;
	}

	void Speak(const WString& speakerName, const WString& message) override
	{
#define ERROR_MESSAGE_PREFIX L"ChatServerImpl::Speak(const WString&, const WString&)#"
		CHECK_ERROR(mainThreadId == Thread::GetCurrentThreadId(), ERROR_MESSAGE_PREFIX L"Speak should be invoked on the main thread.");
#undef ERROR_MESSAGE_PREFIX

		if (users.Contains(speakerName))
		{
			OnSpoken(speakerName, message);
		}
	}
};

/***********************************************************************
* ChatBotRpcBroadcastingLocalClient
*   implement request broadcasting mechanism
***********************************************************************/

class ChatBotRpcBroadcastingLocalClient : public JsonLocalChannelClient
{
private:
	JsonChannelClient::ChannelMap channelNames;
	Ptr<TaskQueue> taskQueue;
	Ptr<RpcJsonDispatcherServerForTaskQueue> dispatcher;

public:
	ChatBotRpcBroadcastingLocalClient(Ptr<Parser> parser)
		: JsonLocalChannelClient(parser)
	{
		channelNames.Add(WString::Unmanaged(RpcChannel), nullptr);
	}

	const JsonChannelClient::ChannelNameList& OnGetChannelNames() override
	{
		return channelNames.Keys();
	}

	void Connect(JsonChannelServer* channelServer, Ptr<JsonChannelClient> self, Ptr<TaskQueue> _taskQueue)
	{
		CHECK_ERROR(channelServer, L"ChatBotRpcBroadcastingLocalClient needs a channel server.");
		CHECK_ERROR(self, L"ChatBotRpcBroadcastingLocalClient needs itself as a shared pointer.");
		taskQueue = _taskQueue;

		auto clientId = channelServer->ConnectLocalClient(self);
		CHECK_ERROR(clientId != -1, L"ChatBotRpcBroadcastingLocalClient failed to connect.");
		dispatcher = Ptr(new RpcJsonDispatcherServerForTaskQueue(this, GetChannels()[WString::Unmanaged(RpcChannel)], taskQueue));
	}

	RpcJsonDispatcherServer* GetDispatcher()
	{
		CHECK_ERROR(dispatcher, L"ChatBotRpcBroadcastingLocalClient has not been connected.");
		return dispatcher.Obj();
	}
};

/***********************************************************************
* ChatBotRpcHostingLocalClient
*   communicate with the server through JSON-RPC
*   to support RPC based Workflow interfaces
***********************************************************************/

class ChatBotRpcHostingLocalClient : public JsonLocalChannelClient
{
private:
	JsonChannelClient::ChannelMap channelNames;
	Ptr<TaskQueue> taskQueue;
	Ptr<ChatBotJsonDispatcherClient> dispatcher;

public:
	ChatBotRpcHostingLocalClient(Ptr<Parser> parser)
		: JsonLocalChannelClient(parser)
	{
		channelNames.Add(WString::Unmanaged(RpcChannel), nullptr);
	}

	const JsonChannelClient::ChannelNameList& OnGetChannelNames() override
	{
		return channelNames.Keys();
	}

	void OnConnected(vint clientId) override
	{
		CHECK_ERROR(dispatcher, L"ChatBotRpcHostingLocalClient needs a dispatcher.");
		dispatcher->InitializeRpc(clientId);
	}

	vint Connect(JsonChannelServer* channelServer, Ptr<JsonChannelClient> self, Ptr<TaskQueue> _taskQueue, vint serverClientId, const List<WString>& waitingForServices)
	{
		CHECK_ERROR(channelServer, L"ChatBotRpcHostingLocalClient needs a channel server.");
		CHECK_ERROR(self, L"ChatBotRpcHostingLocalClient needs itself as a shared pointer.");
		taskQueue = _taskQueue;
		dispatcher = Ptr(new ChatBotJsonDispatcherClient(taskQueue));
		auto clientId = dispatcher->ConnectLocalServer(channelServer, self, GetChannels()[WString::Unmanaged(RpcChannel)], waitingForServices);
		dispatcher->SetServerLocalClientId(serverClientId);
		return clientId;
	}

	ChatBotJsonDispatcherClient* GetDispatcher()
	{
		CHECK_ERROR(dispatcher, L"ChatBotRpcHostingLocalClient has not been connected.");
		return dispatcher.Obj();
	}
};

/***********************************************************************
* ChatBotHttpServer
*   a HTTP server to connect all ChatBotClient projects
***********************************************************************/

class ChatBotHttpServer : public JsonNetworkChannelServer<HttpServer>
{
private:
	RpcJsonDispatcherServer* dispatcher = nullptr;

public:
	ChatBotHttpServer(Ptr<Parser> parser)
		: JsonNetworkChannelServer<HttpServer>(parser, WString::Unmanaged(ChatBotHttpBaseUrl), ChatBotHttpPort)
	{
	}

	WaitForClientResult OnClientConnected(vint clientId, const JsonChannelClient::ChannelNameList& availableChannels, JsonChannelClient* localClient) override
	{
		if (!availableChannels.Contains(WString::Unmanaged(RpcChannel)))
		{
			return WaitForClientResult::Reject;
		}

		if (localClient && dynamic_cast<ChatBotRpcBroadcastingLocalClient*>(localClient))
		{
			return WaitForClientResult::Accept;
		}

		if (!dispatcher || !dispatcher->HasServerClientId())
		{
			return WaitForClientResult::Reject;
		}

		dispatcher->RegisterClient(clientId);
		return WaitForClientResult::Accept;
	}

	void OnClientDisconnected(vint clientId) override
	{
		if (dispatcher && dispatcher->HasServerClientId())
		{
			dispatcher->DisconnectClient(clientId);
		}
	}

	void SetDispatcher(RpcJsonDispatcherServer* value)
	{
		dispatcher = value;
	}
};

/***********************************************************************
* main
***********************************************************************/

int main()
{
	mainThreadId = Thread::GetCurrentThreadId();

	// ---- Core Objects ----
	auto parser = Ptr(new Parser);
	auto taskQueue = Ptr(new TaskQueue);
	auto channelServer = Ptr(new ChatBotHttpServer(parser));
	auto service = Ptr(new ChatServerImpl);
	AttachChatServerEventHandlers(service.Obj());

	// ---- Local Channel Clients ----
	channelServer->Start();
	auto serverLocalClient = Ptr(new ChatBotRpcBroadcastingLocalClient(parser));
	serverLocalClient->Connect(channelServer.Obj(), serverLocalClient, taskQueue);
	channelServer->SetDispatcher(serverLocalClient->GetDispatcher());

	auto serviceLocalClient = Ptr(new ChatBotRpcHostingLocalClient(parser));
	List<WString> waitingForServices;
	serviceLocalClient->Connect(channelServer.Obj(), serviceLocalClient, taskQueue, serverLocalClient->GetDispatcher()->GetServerClientId(), waitingForServices);
	auto serviceDispatcher = serviceLocalClient->GetDispatcher();
	auto serviceLifecycle = serviceDispatcher->GetRpcLifecycle();
	auto typeId = serviceLifecycle->GetTypeIdFromName(WString::Unmanaged(L"chatapi::IChatServer"));
	CHECK_ERROR(typeId != RpcTypeId_NotFound, L"ChatBotServer failed to find IChatServer type id.");
	serviceLifecycle->RegisterLocalService(typeId, service);
	serviceDispatcher->Initialize();

	// ---- Console ----
	Console::SetTitle(L"ChatBotServer (localhost:" + itow(ChatBotHttpPort) + ChatBotHttpBaseUrl + L")");
	Console::WriteLine(L"Ready to start ChatBotClient.");
	Console::WriteLine(L"Press ENTER to exit.");
	ThreadPoolLite::QueueLambda([taskQueue]()
	{
		Console::TryRead();
		taskQueue->QueueExitTask();
	});

	// ---- Main Task Queue ----
	taskQueue->RunTaskQueue();
	service->OnServerShutdown();
	serviceDispatcher->FinalizeRpc();
	channelServer->Stop();
	return 0;
}
