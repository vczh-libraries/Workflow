#include "ChatBotApp.h"
#include "..\ChatBotServer\Shared\ChatBotHttp.h"
#include "..\ChatBotServer\Shared\ChatBotJsonDispatcherClient.h"
#include <VlppOS.Windows.h>

using namespace vl;
using namespace vl::collections;
using namespace vl::console;
using namespace vl::glr::json;
using namespace vl::inter_process;
using namespace vl::rpc_controller::channeling;
using namespace chatbot;

/***********************************************************************
* ChatBotRpcHostingClient
*   communicate with the server through JSON-RPC
*   to support RPC based Workflow interfaces
***********************************************************************/

class ChatBotRpcHostingClient : public JsonNetworkChannelClient
{
private:
	JsonChannelClient::ChannelMap		channelNames;
	Ptr<ChatBotJsonDispatcherClient>	dispatcher;

public:
	ChatBotRpcHostingClient(Ptr<INetworkProtocolClient> client, Ptr<Parser> parser)
		: JsonNetworkChannelClient(client, parser)
	{
		channelNames.Add(WString::Unmanaged(RpcChannel), nullptr);
	}

	const JsonChannelClient::ChannelNameList& OnGetChannelNames() override
	{
		return channelNames.Keys();
	}

	void OnConnected(vint clientId) override
	{
		CHECK_ERROR(dispatcher, L"ChatBotRpcHostingClient needs a dispatcher.");
		dispatcher->InitializeRpc(clientId);
	}

	void Connect(Ptr<TaskQueue> taskQueue, const List<WString>& waitingForServices)
	{
		dispatcher = Ptr(new ChatBotJsonDispatcherClient(taskQueue));
		dispatcher->WaitForServer(this, GetChannels()[WString::Unmanaged(RpcChannel)], waitingForServices);
		dispatcher->Initialize();
	}

	ChatBotJsonDispatcherClient* GetDispatcher()
	{
		CHECK_ERROR(dispatcher, L"ChatBotRpcHostingClient has not been connected.");
		return dispatcher.Obj();
	}
};

/***********************************************************************
* ChatBotInputThread
*   accepting user console input
*   when the application exits, the thread is abandoned
***********************************************************************/

class ChatBotInputThread : public Thread
{
private:
	Ptr<TaskQueue>						taskQueue;
	Ptr<chatapi::IChatServer>			chatServer;
	ChatBotJsonDispatcherClient*		dispatcher = nullptr;
	WString								userName;

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

/***********************************************************************
* main
***********************************************************************/

int main()
{
	mainThreadId = Thread::GetCurrentThreadId();

	// ---- Core Objects ----
	auto parser = Ptr(new Parser);
	auto taskQueue = Ptr(new TaskQueue);
	auto httpClient = Ptr(new HttpClient(WString::Unmanaged(ChatBotHttpBaseUrl), ChatBotHttpPort));
	auto channelClient = Ptr(new ChatBotRpcHostingClient(httpClient, parser));

	// ---- RPC Connection ----
	List<WString> waitingForServices;
	waitingForServices.Add(WString::Unmanaged(L"chatapi::IChatServer"));
	channelClient->Connect(taskQueue, waitingForServices);
	auto dispatcher = channelClient->GetDispatcher();
	auto chatServer = dispatcher->GetRpcLifecycle()->RequestService(WString::Unmanaged(L"chatapi::IChatServer")).Cast<chatapi::IChatServer>();
	CHECK_ERROR(chatServer, L"ChatBotClient failed to request IChatServer.");
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
