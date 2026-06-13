#include "ChatBotApp.h"
#include "Shared/ChatBotHttp.h"
#include "Shared/ChatBotJsonDispatcherClient.h"
#include "Shared/ChatBotJsonDispatcherServer.h"
#include <VlppOS.Windows.h>

using namespace vl;
using namespace vl::collections;
using namespace vl::console;
using namespace vl::glr::json;
using namespace vl::inter_process;
using namespace chatbot;

class ChatServerService : public Object, public virtual chatapi::IChatServer
{
private:
	SortedList<WString> users;

public:
	bool AddUser(const WString& name) override
	{
#define ERROR_MESSAGE_PREFIX L"ChatServerService::AddUser(const WString&)#"
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
#define ERROR_MESSAGE_PREFIX L"ChatServerService::RemoveUser(const WString&)#"
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
#define ERROR_MESSAGE_PREFIX L"ChatServerService::Speak(const WString&, const WString&)#"
		CHECK_ERROR(mainThreadId == Thread::GetCurrentThreadId(), ERROR_MESSAGE_PREFIX L"Speak should be invoked on the main thread.");
#undef ERROR_MESSAGE_PREFIX

		if (users.Contains(speakerName))
		{
			OnSpoken(speakerName, message);
		}
	}
};

class ChatBotChannelServer : public JsonNetworkChannelServer
{
private:
	ChatBotJsonDispatcherServer* dispatcher = nullptr;

public:
	ChatBotChannelServer(Ptr<Parser> parser)
		: JsonNetworkChannelServer(parser)
	{
	}

	WaitForClientResult OnClientConnected(vint clientId, const JsonChannelClient::ChannelNameList& availableChannels) override
	{
		if (!availableChannels.Contains(WString::Unmanaged(RpcChannel)))
		{
			return WaitForClientResult::Reject;
		}

		if (dispatcher && dispatcher->HasServerClientId())
		{
			dispatcher->RegisterClient(clientId);
		}

		return WaitForClientResult::Accept;
	}

	void OnClientDisconnected(vint clientId) override
	{
		if (dispatcher && dispatcher->HasServerClientId())
		{
			dispatcher->DisconnectClient(clientId);
		}
	}

	void SetDispatcher(ChatBotJsonDispatcherServer* value)
	{
		dispatcher = value;
	}
};

class ChatBotHttpServer : public HttpServer
{
private:
	JsonNetworkChannelServer* channelServer = nullptr;

public:
	ChatBotHttpServer(JsonNetworkChannelServer* _channelServer)
		: HttpServer(WString::Unmanaged(ChatBotHttpBaseUrl), ChatBotHttpPort)
		, channelServer(_channelServer)
	{
		CHECK_ERROR(channelServer, L"ChatBotHttpServer needs a channel server.");
	}

	WaitForClientResult OnClientConnected(INetworkProtocolConnection* connection) override
	{
		return channelServer->OnClientConnected(connection);
	}
};

class ChatBotServerChannelClient : public JsonLocalChannelClient
{
private:
	class Dispatcher : public ChatBotJsonDispatcherServer
	{
	private:
		Ptr<TaskQueue> taskQueue;

	protected:
		void ScheduleTask(Func<void()> task) override
		{
			taskQueue->QueueTask(task);
		}

	public:
		Dispatcher(JsonChannelClient* serverClient, JsonChannel* channel, Ptr<TaskQueue> _taskQueue)
			: ChatBotJsonDispatcherServer(serverClient, channel)
			, taskQueue(_taskQueue)
		{
			CHECK_ERROR(taskQueue, L"ChatBotServerChannelClient::Dispatcher needs a task queue.");
		}
	};

	JsonChannelClient::ChannelMap channelNames;
	Ptr<TaskQueue> taskQueue;
	Ptr<Dispatcher> dispatcher;

public:
	ChatBotServerChannelClient(Ptr<Parser> parser)
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
		CHECK_ERROR(channelServer, L"ChatBotServerChannelClient needs a channel server.");
		CHECK_ERROR(self, L"ChatBotServerChannelClient needs itself as a shared pointer.");
		taskQueue = _taskQueue;

		auto clientId = channelServer->ConnectLocalClient(self);
		CHECK_ERROR(clientId != -1, L"ChatBotServerChannelClient failed to connect.");
		dispatcher = Ptr(new Dispatcher(this, GetChannels()[WString::Unmanaged(RpcChannel)], taskQueue));
	}

	ChatBotJsonDispatcherServer* GetDispatcher()
	{
		CHECK_ERROR(dispatcher, L"ChatBotServerChannelClient has not been connected.");
		return dispatcher.Obj();
	}
};

class ChatBotServiceChannelClient : public JsonLocalChannelClient
{
private:
	class Dispatcher : public ChatBotJsonDispatcherClient
	{
	private:
		Ptr<TaskQueue> taskQueue;

	protected:
		void ScheduleTask(Func<void()> task) override
		{
			taskQueue->QueueTask(task);
		}

	public:
		Dispatcher(Ptr<TaskQueue> _taskQueue)
			: taskQueue(_taskQueue)
		{
			CHECK_ERROR(taskQueue, L"ChatBotServiceChannelClient::Dispatcher needs a task queue.");
		}
	};

	JsonChannelClient::ChannelMap channelNames;
	Ptr<TaskQueue> taskQueue;
	Ptr<Dispatcher> dispatcher;

public:
	ChatBotServiceChannelClient(Ptr<Parser> parser)
		: JsonLocalChannelClient(parser)
	{
		channelNames.Add(WString::Unmanaged(RpcChannel), nullptr);
	}

	const JsonChannelClient::ChannelNameList& OnGetChannelNames() override
	{
		return channelNames.Keys();
	}

	vint Connect(JsonChannelServer* channelServer, Ptr<JsonChannelClient> self, Ptr<TaskQueue> _taskQueue, vint serverClientId, const List<WString>& waitingForServices)
	{
		CHECK_ERROR(channelServer, L"ChatBotServiceChannelClient needs a channel server.");
		CHECK_ERROR(self, L"ChatBotServiceChannelClient needs itself as a shared pointer.");
		taskQueue = _taskQueue;
		dispatcher = Ptr(new Dispatcher(taskQueue));
		auto clientId = dispatcher->ConnectLocalClient(channelServer, self, GetChannels()[WString::Unmanaged(RpcChannel)], waitingForServices);
		dispatcher->SetServerLocalClientId(serverClientId);
		return clientId;
	}

	ChatBotJsonDispatcherClient* GetDispatcher()
	{
		CHECK_ERROR(dispatcher, L"ChatBotServiceChannelClient has not been connected.");
		return dispatcher.Obj();
	}
};

class ChatBotExitThread : public Thread
{
private:
	Ptr<TaskQueue> taskQueue;

protected:
	void Run() override
	{
		Console::TryRead();
		taskQueue->QueueExitTask();
	}

public:
	ChatBotExitThread(Ptr<TaskQueue> _taskQueue)
		: taskQueue(_taskQueue)
	{
		CHECK_ERROR(taskQueue, L"ChatBotExitThread needs a task queue.");
	}
};

int main()
{
	mainThreadId = Thread::GetCurrentThreadId();

	// ---- Core Objects ----
	auto parser = CreateChatBotJsonParser();
	auto taskQueue = Ptr(new TaskQueue);
	auto channelServer = Ptr(new ChatBotChannelServer(parser));
	auto httpServer = Ptr(new ChatBotHttpServer(channelServer.Obj()));
	auto service = Ptr(new ChatServerService);
	AttachChatServerEventHandlers(service.Obj());

	// ---- Local Channel Clients ----
	channelServer->Start();
	auto serverLocalClient = Ptr(new ChatBotServerChannelClient(parser));
	serverLocalClient->Connect(channelServer.Obj(), serverLocalClient, taskQueue);
	channelServer->SetDispatcher(serverLocalClient->GetDispatcher());

	auto serviceLocalClient = Ptr(new ChatBotServiceChannelClient(parser));
	List<WString> waitingForServices;
	serviceLocalClient->Connect(channelServer.Obj(), serviceLocalClient, taskQueue, serverLocalClient->GetDispatcher()->GetServerClientId(), waitingForServices);
	auto serviceDispatcher = serviceLocalClient->GetDispatcher();
	serviceDispatcher->RegisterLocalService(service);
	serviceDispatcher->Initialize();

	// ---- HTTP Transport ----
	httpServer->Start();

	// ---- Console ----
	Console::SetTitle(L"ChatBotServer (localhost:" + itow(ChatBotHttpPort) + ChatBotHttpBaseUrl + L")");
	Console::WriteLine(L"Ready to start ChatBotClient.");
	Console::WriteLine(L"Press ENTER to exit.");
	auto exitThread = Ptr(new ChatBotExitThread(taskQueue));
	exitThread->Start();

	// ---- Main Task Queue ----
	taskQueue->RunTaskQueue();
	exitThread->Wait();
	service->OnServerShutdown();
	serviceDispatcher->FinalizeRpc();
	httpServer->Stop();
	channelServer->Stop();
	return 0;
}
