#include "ChatBotApp.h"
#include "..\ChatBotServer\Shared\ChatBotHttp.h"
#include "..\ChatBotServer\Shared\ChatBotJsonDispatcherClient.h"
#include <VlppOS.Windows.h>

using namespace vl;
using namespace vl::collections;
using namespace vl::console;
using namespace vl::glr::json;
using namespace vl::inter_process;
using namespace chatbot;

class ChatBotChannelClient : public JsonNetworkChannelClient
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
			CHECK_ERROR(taskQueue, L"ChatBotChannelClient::Dispatcher needs a task queue.");
		}
	};

	JsonChannelClient::ChannelMap channelNames;
	Ptr<Dispatcher> dispatcher;

public:
	ChatBotChannelClient(Ptr<INetworkProtocolClient> client, Ptr<Parser> parser)
		: JsonNetworkChannelClient(client, parser)
	{
		channelNames.Add(WString::Unmanaged(RpcChannel), nullptr);
	}

	const JsonChannelClient::ChannelNameList& OnGetChannelNames() override
	{
		return channelNames.Keys();
	}

	void Connect(Ptr<TaskQueue> taskQueue, const List<WString>& waitingForServices)
	{
		dispatcher = Ptr(new Dispatcher(taskQueue));
		dispatcher->WaitForServer(this, GetChannels()[WString::Unmanaged(RpcChannel)], waitingForServices);
	}

	ChatBotJsonDispatcherClient* GetDispatcher()
	{
		CHECK_ERROR(dispatcher, L"ChatBotChannelClient has not been connected.");
		return dispatcher.Obj();
	}
};

class ChatBotInputThread : public Thread
{
private:
	Ptr<TaskQueue> taskQueue;
	Ptr<chatapi::IChatServer> chatServer;
	ChatBotJsonDispatcherClient* dispatcher = nullptr;
	WString userName;

protected:
	void Run() override
	{
		auto line = Console::TryRead();
		while (line)
		{
			if (line.Value() == WString::Unmanaged(L"exit"))
			{
				taskQueue->QueueTask(Func<void()>([server = chatServer, name = userName]()
				{
					server->RemoveUser(name);
				}));
				taskQueue->QueueTask(Func<void()>([dispatcher = dispatcher]()
				{
					dispatcher->NotifyServerClientDisconnected();
				}));
				taskQueue->QueueExitTask();
				break;
			}
			else
			{
				taskQueue->QueueTask(Func<void()>([server = chatServer, name = userName, message = line.Value()]()
				{
					server->Speak(name, message);
				}));
			}
			line = Console::TryRead();
		}
	}

public:
	ChatBotInputThread(Ptr<TaskQueue> _taskQueue, Ptr<chatapi::IChatServer> _chatServer, ChatBotJsonDispatcherClient* _dispatcher, const WString& _userName)
		: taskQueue(_taskQueue)
		, chatServer(_chatServer)
		, dispatcher(_dispatcher)
		, userName(_userName)
	{
		CHECK_ERROR(taskQueue, L"ChatBotInputThread needs a task queue.");
		CHECK_ERROR(chatServer, L"ChatBotInputThread needs a chat server.");
		CHECK_ERROR(dispatcher, L"ChatBotInputThread needs a dispatcher.");
	}
};

int main()
{
	mainThreadId = Thread::GetCurrentThreadId();

	// ---- Core Objects ----
	auto parser = CreateChatBotJsonParser();
	auto taskQueue = Ptr(new TaskQueue);
	auto httpClient = Ptr(new HttpClient(WString::Unmanaged(ChatBotHttpBaseUrl), ChatBotHttpPort));
	auto channelClient = Ptr(new ChatBotChannelClient(httpClient, parser));

	// ---- RPC Connection ----
	List<WString> waitingForServices;
	waitingForServices.Add(WString::Unmanaged(L"chatapi::IChatServer"));
	channelClient->Connect(taskQueue, waitingForServices);
	auto dispatcher = channelClient->GetDispatcher();
	auto chatServer = dispatcher->GetChatServer();
	AttachChatServerEventHandlers(chatServer.Obj());
	chatServer->OnServerShutdown.Add(Func<void()>([taskQueue]()
	{
#define ERROR_MESSAGE_PREFIX L"ChatBotClient::main()#"
		CHECK_ERROR(mainThreadId == Thread::GetCurrentThreadId(), ERROR_MESSAGE_PREFIX L"OnServerShutdown should be invoked on the main thread.");
#undef ERROR_MESSAGE_PREFIX
		taskQueue->QueueExitTask();
	}));

	// ---- Console ----
	Console::Write(L"Enter your name: ");
	auto inputUserName = Console::TryRead();
	if (!inputUserName)
	{
		return 0;
	}
	auto userName = inputUserName.Value();
	Console::SetTitle(L"ChatBotClient: " + userName);
	taskQueue->QueueTask(Func<void()>([server = chatServer, name = userName]()
	{
		server->AddUser(name);
	}));
	auto inputThread = Ptr(new ChatBotInputThread(taskQueue, chatServer, dispatcher, userName));
	inputThread->Start();

	// ---- Main Task Queue ----
	taskQueue->RunTaskQueue();
	dispatcher->FinalizeRpc();
	httpClient->Stop();
	return 0;
}
