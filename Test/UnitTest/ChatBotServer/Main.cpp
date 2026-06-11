#include "ChatBotApp.h"
#include "Shared/ChatBotJsonDispatcher.h"
#include <VlppOS.Windows.h>
#include <stdio.h>

using namespace vl;
using namespace vl::collections;
using namespace vl::console;
using namespace vl::glr::json;
using namespace vl::inter_process;
using namespace chatbot;

bool TryReadLine(WString& line)
{
	DWORD mode = 0;
	if (GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &mode))
	{
		line = Console::Read();
		return true;
	}

	wchar_t buffer[4096] = { 0 };
	if (!fgetws(buffer, sizeof(buffer) / sizeof(*buffer), stdin))
	{
		return false;
	}

	line = buffer;
	while (line.Length() > 0)
	{
		auto c = line[line.Length() - 1];
		if (c != L'\r' && c != L'\n')
		{
			break;
		}
		line = line.Left(line.Length() - 1);
	}
	return true;
}

class ChatServerService : public Object, public virtual chatapi::IChatServer
{
private:
	SortedList<WString> users;

public:
	bool AddUser(const WString& name) override
	{
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
		if (users.Contains(speakerName))
		{
			OnSpoken(speakerName, message);
		}
	}
};

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

class ChatBotLocalChannelClient : public JsonLocalChannelClient
{
private:
	JsonChannelClient::ChannelMap channelNames;

public:
	ChatBotLocalChannelClient(Ptr<Parser> parser)
		: JsonLocalChannelClient(parser)
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

class ChatBotHttpChannelServer : public JsonNetworkChannelServer, public HttpServer
{
private:
	ChatBotJsonDispatcherServer* dispatcher = nullptr;
	bool acceptingLocalClient = false;

public:
	ChatBotHttpChannelServer(Ptr<Parser> parser)
		: JsonNetworkChannelServer(parser)
		, HttpServer(WString::Unmanaged(ChatBotHttpBaseUrl), ChatBotHttpPort)
	{
	}

	WaitForClientResult OnClientConnected(INetworkProtocolConnection* connection) override
	{
		return JsonNetworkChannelServer::OnClientConnected(connection);
	}

	WaitForClientResult OnClientConnected(vint clientId, const JsonChannelClient::ChannelNameList& availableChannels) override
	{
		if (!availableChannels.Contains(WString::Unmanaged(RpcChannel)))
		{
			return WaitForClientResult::Reject;
		}

		if (acceptingLocalClient)
		{
			return WaitForClientResult::Accept;
		}

		if (!dispatcher)
		{
			return WaitForClientResult::Reject;
		}

		dispatcher->RegisterClient(clientId);
		return WaitForClientResult::Accept;
	}

	void OnClientDisconnected(vint clientId) override
	{
		if (dispatcher)
		{
			dispatcher->DisconnectClient(clientId);
		}
	}

	void SetDispatcher(ChatBotJsonDispatcherServer* value)
	{
		dispatcher = value;
	}

	void BeginAcceptingLocalClient()
	{
		acceptingLocalClient = true;
	}

	void EndAcceptingLocalClient()
	{
		acceptingLocalClient = false;
	}

	void StartChannelServer()
	{
		JsonNetworkChannelServer::Start();
	}

	void StartHttpServer()
	{
		HttpServer::Start();
	}

	void Start() override
	{
		StartChannelServer();
		StartHttpServer();
	}

	void Stop() override
	{
		HttpServer::Stop();
		JsonNetworkChannelServer::Stop();
	}

	bool IsStopped() override
	{
		return HttpServer::IsStopped() || JsonNetworkChannelServer::IsStopped();
	}
};

class ChatBotJsonDispatcherServerImpl : public ChatBotJsonDispatcherServer
{
private:
	Ptr<TaskQueue> taskQueue;

protected:
	void SchedulaTask(Func<void()> task) override
	{
		taskQueue->QueueTask(task);
	}

public:
	ChatBotJsonDispatcherServerImpl(JsonChannel* channel, Ptr<chatapi::IChatServer> service, Ptr<TaskQueue> _taskQueue)
		: ChatBotJsonDispatcherServer(channel, service)
		, taskQueue(_taskQueue)
	{
		CHECK_ERROR(taskQueue, L"ChatBotJsonDispatcherServerImpl needs a task queue.");
	}
};

int main()
{
	auto parser = CreateChatBotJsonParser();
	auto channelServer = Ptr(new ChatBotHttpChannelServer(parser));
	auto service = Ptr(new ChatServerService);
	service->OnUserAdded.Add(Func<void(WString)>([](WString speakerName)
	{
		Console::WriteLine(speakerName + WString::Unmanaged(L" joined"));
	}));
	service->OnUserRemoved.Add(Func<void(WString)>([](WString speakerName)
	{
		Console::WriteLine(speakerName + WString::Unmanaged(L" left"));
	}));
	service->OnSpoken.Add(Func<void(WString, WString)>([](WString speakerName, WString message)
	{
		Console::WriteLine(speakerName + WString::Unmanaged(L"> ") + message);
	}));

	channelServer->StartChannelServer();
	auto localClient = Ptr(new ChatBotLocalChannelClient(parser));
	channelServer->BeginAcceptingLocalClient();
	auto localClientId = channelServer->ConnectLocalClient(localClient);
	channelServer->EndAcceptingLocalClient();
	CHECK_ERROR(localClientId != -1, L"ChatBotServer failed to connect its local client.");

	auto taskQueue = Ptr(new TaskQueue);
	auto dispatcher = Ptr(new ChatBotJsonDispatcherServerImpl(GetRpcChannel(localClient.Obj()), service, taskQueue));
	dispatcher->RegisterLocalClient(localClientId);
	channelServer->SetDispatcher(dispatcher.Obj());

	auto taskThread = Ptr(new ChatBotTaskThread(taskQueue));
	taskThread->Start();
	channelServer->StartHttpServer();

	Console::SetTitle(L"ChatBotServer (localhost:" + itow(ChatBotHttpPort) + ChatBotHttpBaseUrl + L")");
	Console::WriteLine(L"Ready to start ChatBotClient.");
	WString line;
	while (TryReadLine(line) && line != WString::Unmanaged(L"exit"))
	{
	}

	taskQueue->QueueExitTask();
	taskThread->Wait();
	service->OnServerShutdown();
	dispatcher->FinalizeRpc();
	channelServer->Stop();
	return 0;
}
