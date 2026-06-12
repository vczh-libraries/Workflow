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

class ChatBotTaskThread : public Thread
{
private:
	Ptr<TaskQueue> taskQueue;

protected:
	void Run() override
	{
		taskQueue->RunTaskQueue();
	}

public:
	ChatBotTaskThread(Ptr<TaskQueue> _taskQueue)
		: taskQueue(_taskQueue)
	{
	}
};

JsonChannel* GetRpcChannel(JsonChannelClient* client)
{
	auto&& channels = client->GetChannels();
	auto index = channels.Keys().IndexOf(WString::Unmanaged(RpcChannel));
	CHECK_ERROR(index != -1, L"RpcChannel is missing.");
	auto channel = channels.Values()[index];
	CHECK_ERROR(channel, L"RpcChannel is null.");
	return channel;
}

class ChatBotChannelClient : public JsonNetworkChannelClient
{
private:
	JsonChannelClient::ChannelMap channelNames;

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

	void OnConnected(vint) override
	{
	}

	void OnDisconnected() override
	{
	}

	void OnReadError(const WString&) override
	{
	}

	void OnLocalError(const WString&, bool) override
	{
	}
};

class ChatBotJsonDispatcherClientImpl : public ChatBotJsonDispatcherClient
{
private:
	Ptr<TaskQueue> taskQueue;

protected:
	void SchedulaTask(Func<void()> task) override
	{
		taskQueue->QueueTask(task);
	}

public:
	ChatBotJsonDispatcherClientImpl(Ptr<TaskQueue> _taskQueue)
		: taskQueue(_taskQueue)
	{
		CHECK_ERROR(taskQueue, L"ChatBotJsonDispatcherClientImpl needs a task queue.");
	}
};

class ChatBotInputThread : public Thread
{
private:
	Ptr<TaskQueue> taskQueue;
	Ptr<chatapi::IChatServer> chatServer;
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
	ChatBotInputThread(Ptr<TaskQueue> _taskQueue, Ptr<chatapi::IChatServer> _chatServer, const WString& _userName)
		: taskQueue(_taskQueue)
		, chatServer(_chatServer)
		, userName(_userName)
	{
	}
};

int main()
{
	auto parser = CreateChatBotJsonParser();
	auto httpClient = Ptr(new HttpClient(WString::Unmanaged(ChatBotHttpBaseUrl), ChatBotHttpPort));
	auto channelClient = Ptr(new ChatBotChannelClient(httpClient, parser));

	auto taskQueue = Ptr(new TaskQueue);
	auto dispatcher = Ptr(new ChatBotJsonDispatcherClientImpl(taskQueue));
	List<WString> waitingForServices;
	waitingForServices.Add(WString::Unmanaged(L"chatapi::IChatServer"));
	dispatcher->WaitForServer(channelClient.Obj(), GetRpcChannel(channelClient.Obj()), waitingForServices);

	auto chatServer = dispatcher->GetChatServer();
	chatServer->OnUserAdded.Add(Func<void(WString)>([](WString speakerName)
	{
		Console::WriteLine(speakerName + WString::Unmanaged(L" joined"));
	}));
	chatServer->OnUserRemoved.Add(Func<void(WString)>([](WString speakerName)
	{
		Console::WriteLine(speakerName + WString::Unmanaged(L" left"));
	}));
	chatServer->OnSpoken.Add(Func<void(WString, WString)>([](WString speakerName, WString message)
	{
		Console::WriteLine(speakerName + WString::Unmanaged(L"> ") + message);
	}));
	chatServer->OnServerShutdown.Add(Func<void()>([taskQueue]()
	{
		taskQueue->QueueExitTask();
	}));

	Console::Write(L"Enter your name: ");
	auto inputUserName = Console::TryRead();
	if (!inputUserName)
	{
		return 0;
	}
	auto userName = inputUserName.Value();
	Console::SetTitle(L"ChatBotClient: " + userName);
	auto taskThread = Ptr(new ChatBotTaskThread(taskQueue));
	taskThread->Start();
	taskQueue->QueueTask(Func<void()>([server = chatServer, name = userName]()
	{
		server->AddUser(name);
	}));

	auto inputThread = Ptr(new ChatBotInputThread(taskQueue, chatServer, userName));
	inputThread->Start();
	taskThread->Wait();
	dispatcher->FinalizeRpc();
	httpClient->Stop();
	return 0;
}
